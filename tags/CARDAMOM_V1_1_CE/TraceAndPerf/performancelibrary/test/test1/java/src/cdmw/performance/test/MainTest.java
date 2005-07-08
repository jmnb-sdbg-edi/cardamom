/* =========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI.
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003.
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General
 * Public License along with CARDAMOM; see the file COPYING. If not, write to
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * =========================================================================== */


package cdmw.performance.test;

import cdmw.common.BadParameterException;
import cdmw.ossupport.OS;
import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;

import cdmw.performance.PerformanceMngr;

import com.thalesgroup.CdmwPerformanceAdmin.PerformanceAdmin;
import com.thalesgroup.CdmwPerformanceAdmin.PerformanceAdminHelper;

public class MainTest {

    private static final String DEFAULT_COMMAND_LIST =
        "verbose\n" + "start\n" + "activate T\n" + "start\n" + "x\n";

    private static final String ACTIVATE_COMMAND_LIST =
        "activate T\n" + "start\n" + "x\n";

    private static final String DEACTIVATE_COMMAND_LIST =
        "activate F\n" + "start\n" + "x\n";

    private static final int DEFAULT_COMMAND = 0;
    private static final int ACTIVATE_COMMAND = 1;
    private static final int DEACTIVATE_COMMAND = 2;

    private static final int SUCCESS = 0;
    private static final int FAILURE = 1;

    private static final int POA_THREAD_POOL_SIZE = 5;

    private static final String DEFAULT_STAT_FILE = "apiStat.txt";

    private static final int MAX_TARGET = 2;

    private static final String HELP_OPTION = "--help";
    private static final String INTERACTIVE_OPTION = "--interactive";
    private static final String ACTIVATE_OPTION = "--activate";
    private static final String DEACTIVATE_OPTION = "--deactivate";
    private static final String LOG_FILE_OPTION = "--logFile";

    private boolean interactiveMode;

    private int activationType;

    private String perfStatFile;

    private org.omg.CORBA.ORB orb;

    public MainTest(
        boolean interactiveMode,
        int activationType,
        String perfStatFile) {

        this.orb = null;
        this.interactiveMode = interactiveMode;
        this.activationType = activationType;
        this.perfStatFile = perfStatFile;

    }

    private int orbInit(String[] args) {
        try {
            // Initialize the ORB
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);

            orb = ORBSupport.ORBInit(args, orbStrategies);
            return SUCCESS;

        } catch (BadParameterException e) {
            System.err.println(
                "ERROR in strategyList.addPOAThreadPool()\n"
                    + POA_THREAD_POOL_SIZE
                    + " is a bad parameter");
            e.printStackTrace();
            return FAILURE;
        }
    }

    private int orbEnd() {
        if (orb != null) {
            try {
                orb.destroy();
            } catch (org.omg.CORBA.SystemException e) {
                e.printStackTrace(System.err);
                return FAILURE;
            }
        }
        return SUCCESS;
    }

    public int processInit() {
        try {
            // get a reference to the root POA
            org.omg.CORBA.Object rootObj =
                orb.resolve_initial_references("RootPOA");
            org.omg.PortableServer.POA rootPOA =
                org.omg.PortableServer.POAHelper.narrow(rootObj);

            // create and initialize the performance manager
            PerformanceAdmin perfoMngr =
                PerformanceMngr.init(rootPOA, perfStatFile);

            // export the object reference to a file
            String perfoMngrIor = orb.object_to_string(perfoMngr);
            java.io.FileOutputStream file =
                new java.io.FileOutputStream(ClientThread.PERF_ADMIN_IOR_FILE);
            java.io.PrintWriter out = new java.io.PrintWriter(file);
            out.println(perfoMngrIor);
            out.flush();
            file.close();

            // activate the POA Manager
            rootPOA.the_POAManager().activate();

        } catch (BadParameterException e) {
            e.printStackTrace(System.err);
            return FAILURE;
        } catch (java.io.IOException e) {
            e.printStackTrace(System.err);
            return FAILURE;
        } catch (org.omg.CORBA.SystemException e) {
            e.printStackTrace(System.err);
            return FAILURE;
        } catch (org.omg.CORBA.UserException e) {
            e.printStackTrace(System.err);
            return FAILURE;
        }

        return SUCCESS;
    }

    public void processEnd() {
        try {
            PerformanceMngr.cleanup();
        } catch (BadParameterException e) {
            e.printStackTrace();
        }
    }

    private int run() {
        int status = SUCCESS;

        ClientThread client = null;
        if (!interactiveMode) {
            // set command to process
            String command;
            switch (activationType) {
                case ACTIVATE_COMMAND :
                    command = ACTIVATE_COMMAND_LIST;
                    break;

                case DEACTIVATE_COMMAND :
                    command = DEACTIVATE_COMMAND_LIST;
                    break;

                case DEFAULT_COMMAND :
                default :
                    command = DEFAULT_COMMAND_LIST;
                    break;
            }

            // convert string command to stream
            java.io.ByteArrayInputStream is =
                new java.io.ByteArrayInputStream(command.getBytes());

            // create client thread object
            client = new ClientThread(orb, is, System.out);

        } else {
            // create client thread object in interactive mode
            client = new ClientThread(orb, System.in, System.out);
        }

        // start client and run orb
        client.start();
        orb.run();

        // wait end of client thread after corba loop stopping
        try {
            client.join();
        } catch (InterruptedException e) {
            // ignore
        }

        return status;
    }

    public int autoCheck(boolean logCreationSuccess) {
        int status = SUCCESS;
        LogFileTest logFileTest =
            new LogFileTest(
                this.perfStatFile,
                logCreationSuccess,
                MAX_TARGET);
        logFileTest.start();
        return status;
    }

    public static void printUsage(java.io.PrintStream os, String progName) {
        os.println(
            "Usage:\n"
                + progName
                + " options\n"
                + "\n"
                + "Options:\n"
                + "--help                            Display this information.\n"
                + "--interactive                     Start in interactive mode.\n"
                + "\n"
                + "--activate                        Start with performance activated.\n"
                + "--deactivate                      Start with performance deactivated.\n"
                + "--logFile=<name>                  Name of Performance Log File.\n");
    }

    public static void main(String[] args) {

        // help argument
        if (args.length == 1) {
            if (args[0].compareTo(HELP_OPTION) == 0) {
                printUsage(System.out, "java cdmw.performance.test.MainTest");
                return;
            }
        }

        // ---- Does it use interactive mode? ----
        boolean interactiveMode = false;
        int activationType = DEFAULT_COMMAND;
        String allTests = OS.getOptionValue(args, INTERACTIVE_OPTION);
        if (!allTests.equals("no")) {
            interactiveMode = true;
        } else {
            // ---- Get parameter of activation? ----
            // = 0 no parameter : use internal scenario (default)
            // = 1 activation   : use performance activation scenario
            // = 2 deactivation : use performance deactivation scenario
            //
            if (!interactiveMode) {
                String activation = OS.getOptionValue(args, ACTIVATE_OPTION);
                if (activation.compareTo("no") != 0) {
                    activationType = ACTIVATE_COMMAND;
                } else {
                    activation = OS.getOptionValue(args, DEACTIVATE_OPTION);
                    if (activation.compareTo("no") != 0) {
                        activationType = DEACTIVATE_COMMAND;
                    }
                }
            }
        }

        //
        // get Performance Log File Name
        //
        String perfStatFile = OS.getOptionValue(args, LOG_FILE_OPTION);
        if (perfStatFile.compareTo("no") == 0) {
            perfStatFile = DEFAULT_STAT_FILE;
        }

        //
        // New MainTest
        //
        MainTest test =
            new MainTest(interactiveMode, activationType, perfStatFile);

        //
        // start
        //
        int status;

        // Initialize the ORB
        status = test.orbInit(args);

        // Initialize processing
        if (status == SUCCESS) {
            status = test.processInit();
        }

        // Run processing
        if (status == SUCCESS) {
            status = test.run();
        }

        // End Processing
        if (status == SUCCESS) {
            test.processEnd();
        }

        // in auto mode start auto checking
        if (!interactiveMode && activationType == DEFAULT_COMMAND) {
            // check statistics result
            status = test.autoCheck(status == SUCCESS);
        }

        // end of corba processing
        test.orbEnd();

    }

}

