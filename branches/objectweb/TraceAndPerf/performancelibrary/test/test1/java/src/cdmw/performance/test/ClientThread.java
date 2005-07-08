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

import java.io.*;


import com.thalesgroup.CdmwPerformanceAdmin.PerformanceAdmin;
import com.thalesgroup.CdmwPerformanceAdmin.PerformanceAdminHelper;

public class ClientThread extends Thread {

    static final String PERF_ADMIN_IOR_FILE = "perfoLibrary.ior";

    private static final String INFO_MESSAGE =
        "Type h or help for a list of available commands.";
    private static final String PROMPT_MESSAGE = "\n%>";
    private static final String UNKNOWN_COMMAND_MESSAGE = "Bad command!>";

    // Definition of a basic operation.
    // Each operation must redefine the method execute.
    abstract class OperationBase {
        public static final int SUCCESS = 0;
        public static final int FAILURE = -1;
        public static final int EXIT_ORDERED = -10;

        public abstract int execute(String[] arg, java.io.PrintStream os);
    }

    // List of all operation.
    // Key is operation name; an operation could be present
    // several time with different operation name
    private java.util.Hashtable operationsList;

    // ORB
    private org.omg.CORBA.ORB orb;

    // verbose mode
    private boolean verbose;

    // Input stream
    java.io.InputStream istream;

    // Output stream
    java.io.PrintStream ostream;

    public ClientThread(
        org.omg.CORBA.ORB orb,
        java.io.InputStream is,
        java.io.OutputStream os) {
        super("ClientThread");
        this.orb = orb;
        this.verbose = false;
        this.istream = is;
        this.ostream = new java.io.PrintStream(os);

        registerOperations();
    }

    // register definition of available operations
    private void registerOperations() {
        operationsList = new java.util.Hashtable();

        // -- no operation
        OperationBase noOperation = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                return SUCCESS;
            }
        };
        operationsList.put("", noOperation);

        // -- help
        OperationBase printHelp = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                os.println(
                    " +======================+============================================+\n"
                        + " | COMMAND              | DESCRIPTION                                |\n"
                        + " +======================+============================================+\n"
                        + " | x | exit             | exit ClientThread mode                     |\n"
                        + " +----------------------+--------------------------------------------+\n"
                        + " | h | help             | print available commands                   |\n"
                        + " +----------------------+--------------------------------------------+\n"
                        + " | verbose              | toggle verbose mode on/off                 |\n"
                        + " +----------------------+--------------------------------------------+\n"
                        + " | activate <T/F>       | activate/deactivate the measure            |\n"
                        + " +----------------------+--------------------------------------------+\n"
                        + " | get_api              | get info of API's                          |\n"
                        + " +----------------------+--------------------------------------------+\n"
                        + " | start                | start API to measure                       |\n"
                        + " +----------------------+--------------------------------------------+\n");

                return SUCCESS;
            }
        };
        operationsList.put("help", printHelp);
        operationsList.put("h", printHelp);

        // ------- exit -------
        OperationBase exit = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                try {
                    if (verbose)
                        os.println("Initiating ORB shutdown.");
                    orb.shutdown(false);
                } catch (org.omg.CORBA.SystemException ex) {
                    printException(ex, os);
                }
                return EXIT_ORDERED;
            }
        };
        operationsList.put("exit", exit);
        operationsList.put("x", exit);

        // ------- verbose -------
        OperationBase toggleVerbose = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                verbose = !verbose;
                os.println(
                    "(verbose mode is " + (verbose ? "on" : "off") + ")");
                return SUCCESS;
            }
        };
        operationsList.put("verbose", toggleVerbose);

        // ------- activate -------
        OperationBase activatePerfo = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                if (arg.length != 1) {
                    os.println("Error: syntax is " + "'activate <T/F>'");
                    return FAILURE;
                }
 String fileName = PERF_ADMIN_IOR_FILE;
org.omg.CORBA.Object obj=null;
try{
               
                // import the object reference from the file
              /*  org.omg.CORBA.Object obj =
                    orb.string_to_object("file:/" + fileName);*/
                File f = new File(fileName);
                BufferedReader br =
                new BufferedReader( new FileReader( f ));

            // get object reference from command-line argument file
            obj = 
                orb.string_to_object( br.readLine() );

            br.close();
            }catch( Exception ex )  {}
                if (obj == null) {
                    os.println("Error opening reference filename :" + fileName);
                }

                // get the PerformanceAdmin proxy
                PerformanceAdmin perfoAdmin =
                    PerformanceAdminHelper.narrow(obj);

                if (perfoAdmin == null) {
                    ostream.println(
                        "Could not narrow object to type PerformanceAdmin");
                    return FAILURE;
                }

                if (arg[0].compareToIgnoreCase("T") == 0) {
                    perfoAdmin.activate();
                } else if (arg[0].compareToIgnoreCase("F") == 0) {
                    perfoAdmin.deactivate();
                } else {
                    os.println("Error: syntax is " + "'activate <T/F>'");
                    return FAILURE;
                }

                return SUCCESS;
            }
        };
        operationsList.put("activate", activatePerfo);

        // ------- get_api -------
        OperationBase getApiInfo = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                TargetThread target = new TargetThread();

                int nbOfApi = target.getApiNbr();
                for (int i = 0; i < nbOfApi; i++) {
                    String apiName = target.getApiName(i);
                    if (apiName != null) {
                        os.println(
                            "api name          : "
                                + apiName
                                + "\n"
                                + "  sleep time msec : "
                                + target.getApiSleepTime(i)
                                + "\n"
                                + "  call counter    : "
                                + target.getApiCount(i)
                                + "\n");
                    }
                }

                return SUCCESS;
            }
        };
        operationsList.put("get_api", getApiInfo);

        // ------- start -------
        OperationBase startTargets = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = FAILURE;

                try {
                    // create 2 target threads
                    TargetThread target1 = new TargetThread();
                    TargetThread target2 = new TargetThread();

                    // start clients
                    target1.start();
                    target2.start();

                    // wait end of targets
                    target1.join();
                    target2.join();

                    result = SUCCESS;
                } catch (InterruptedException e) {
                    e.printStackTrace(new java.io.PrintWriter(os));
                }

                return result;
            }
        };
        operationsList.put("start", startTargets);

    }

    public void run() {
        java.io.BufferedReader inReader =
            new java.io.BufferedReader(new java.io.InputStreamReader(istream));

        printInfo(ostream);

        int operationResult = 0;
        do {
            ostream.print(PROMPT_MESSAGE);
            try {
                // read command
                String command = inReader.readLine();

                // if Ctrl-C readLine() return null
                if (command == null) {
                    break;
                }

                java.util.StringTokenizer st =
                    new java.util.StringTokenizer(command);
                int nbTokens = st.countTokens();

                // if input not empty
                if (nbTokens != 0) {

                    // parse input
                    String commandName = "";
                    String[] commandArgs = new String[nbTokens - 1];
                    try {
                        commandName = st.nextToken();
                        for (int i = 0; i < commandArgs.length; i++) {
                            commandArgs[i] = st.nextToken();
                        }

                    } catch (java.util.NoSuchElementException e) {
                        // should not append
                    }

                    // if verbose print command
                    if (verbose) {
                        ostream.print("@cmd = <" + commandName + "> ");
                        ostream.print("@args = <");
                        for (int i = 0; i < commandArgs.length; i++) {
                            ostream.print(commandArgs[i]);
                            if (i < commandArgs.length - 1)
                                ostream.print(", ");
                        }
                        ostream.println(">");
                    }

                    // execute command
                    OperationBase op =
                        (OperationBase) operationsList.get(commandName);
                    if (op != null) {
                        operationResult = op.execute(commandArgs, ostream);
                    } else {
                        ostream.println(UNKNOWN_COMMAND_MESSAGE);
                        operationResult = OperationBase.FAILURE;
                    }
                }

            } catch (java.io.IOException e) {
                ostream.println("ERROR in readLine:");
                e.printStackTrace(new java.io.PrintWriter(ostream));
                operationResult = OperationBase.FAILURE;
            }

        } while (operationResult != OperationBase.EXIT_ORDERED);
    }

    private void printInfo(java.io.PrintStream os) {
        os.println(
            "\n\n"
                + "\t\t---------------------------------------\n"
                + "\t\t  Cdmw Performance Test               \n"
                + "\t\t---------------------------------------\n"
                + "\n\n"
                + INFO_MESSAGE
                + "\nInfo: (verbose mode is "
                + (verbose ? "on" : "off")
                + ")");
    }

    private void printException(
        cdmw.common.Exception ex,
        java.io.PrintStream os) {
        os.println("CDMW Exception : ");
        ex.printStackTrace(new java.io.PrintWriter(ostream));
    }

    private void printException(
        org.omg.CORBA.SystemException ex,
        java.io.PrintStream os) {
        os.println("CORBA system Exception : ");
        ex.printStackTrace(new java.io.PrintWriter(ostream));
    }

    private void printException(
        org.omg.CORBA.UserException ex,
        java.io.PrintStream os) {
        os.println("CORBA user Exception : ");
        ex.printStackTrace(new java.io.PrintWriter(ostream));
    }

}

