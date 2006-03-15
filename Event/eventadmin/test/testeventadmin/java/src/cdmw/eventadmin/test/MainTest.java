/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with CARDAMOM; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


package cdmw.eventadmin.test;

import cdmw.testutils.Testable;
import cdmw.common.Options;

import cdmw.ossupport.OS;
import cdmw.ossupport.ThreadedProcess;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.ORBUtils;
import cdmw.orbsupport.StrategyList;

import cdmw.eventadmin.EventServiceAdminTool;
import cdmw.eventadmin.InitException;

public class MainTest extends Testable {

    public static final int POA_THREAD_POOL_SIZE = 5;

    public static org.omg.CORBA.ORB orb = null;

    // The naming context of the current Process object within the naming
    public final static String PROC_CTXT
        = "CDMW.I/ProcessCallback.simulated/CDMW Test Application/PROC_000";

    private String[] args;

    public MainTest(String[] args) {
        this.out = System.out;
        this.name = "EventAdminTest";
        this.args = args;
    }

    public static void main(String[] args) {
        MainTest main = new MainTest(args);
        main.start();
    }

    public void testEventAdminTool(
        String commandFile, int[] awaitedFailedTests) {

        try {
            org.omg.CosNaming.NamingContext nc
                = org.omg.CosNaming.NamingContextHelper.narrow(
                    orb.resolve_initial_references("NameService"));

            EventServiceAdminTool adm
                = new EventServiceAdminTool(orb, nc, "");

            adm.init();

            java.io.BufferedReader in = new java.io.BufferedReader(
                new java.io.FileReader(commandFile));

            println(EventServiceAdminTool.PROMPT_MESSAGE);
            boolean loopFlag = true;
            int count = 0;
            String s;

            while(loopFlag) {
                while((s = in.readLine()) != null) {
                    count++;
                    java.util.StringTokenizer st
                        = new java.util.StringTokenizer(s);
                    String command = "";
                    String arg = "";
                    if (st.hasMoreTokens()) {
                        command = st.nextToken();
                    }
                    while (st.hasMoreTokens()) {
                        arg += st.nextToken();
                        if (st.hasMoreTokens()) arg += " ";
                    }

                    println("@cmd = <" + command + "> @args = <" + arg + ">");
                    int result = adm.runOperation(command, arg, out);
                    if (result == EventServiceAdminTool.EXIT_ORDERED) {
                        loopFlag = false;
                        break;
                    } else if (result == EventServiceAdminTool.OP_FAILURE) {
                        for(int i=0; i<awaitedFailedTests.length; i++) {
                            if (awaitedFailedTests[i] == count) {
                                succeed();
                                break;
                            }
                            if (i == (awaitedFailedTests.length-1)) {
                                fail();
                            }
                        }
                    } else {
                        succeed();
                    }
                    println(EventServiceAdminTool.PROMPT_MESSAGE);
                }
            }

        } catch (InitException ie) {
            println(ie.what());
            fail();
        } catch(org.omg.CORBA.ORBPackage.InvalidName in) {
            println(in.getMessage());
            fail();
        } catch(java.io.IOException ioe) {
            println(ioe.getMessage());
            fail();
        }

    }

    public void doTests() {
        
        // set number of requested successfull tests
        setNbOfRequestedTestOK(38);

        String callbackIOR
            = OS.getOptionValue( args, Options.CALLBACK_IOR_OPTION);

        boolean unitTests = true;
        // ---- Is it launched by platform management for integration tests? ----
        if ( !callbackIOR.equals("no") && !callbackIOR.equals("yes") ) {
            unitTests = false;
        }

        ThreadedProcess profileManager = null;

        try {
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);

            String[] serverArgs = { "-ORBconfig", "./ob.conf" };
            orb = ORBSupport.ORBInit(serverArgs, orbStrategies);

            if (unitTests) {
                // Starting the simrepository
                cdmw.tools.SimuUtils.startSimu(System.out);
                OS.sleep(getTimescale() * 10000);
                System.out.println("Simrepository started!");

                // Getting ProcessCallback IOR
                callbackIOR = readFromFile(cdmw.tools.SimuUtils.PROCESS_CALLBACK_FILE);
            }

            // Starting the EventChannelManager
            String profileManagerService = "cdmw.eventadmin.EventChannelManager";
            String managerArgs =
                " -ORBconfig ./ob.conf" +
                " " + Options.CALLBACK_IOR_OPTION + "=" + callbackIOR +
                " " + Options.PROCESS_XML_FILE_OPTION + "=./Server.xml" +
                " " + Options.PROCESS_PORT_OPTION + "=15300" +
                " " + Options.LOCALISATION_SERVICE_OPTION + "=15228" +
                " --ProfileXMLFile=./EventChannelConf.xml" +
                " " + Options.VALIDATE_XML_OPTION;
            println(profileManagerService + managerArgs);
            profileManager = OS.createJavaProcess(
                profileManagerService + managerArgs, getProperties());

            OS.sleep(getTimescale() * 10000);
            println("EventChannelManager started");

            String commands;
            cdmw.tools.ProcessAdmin admin = null;
            if (unitTests) {
                String managerURL = "corbaname::localhost:5010#" + PROC_CTXT;
                com.thalesgroup.CdmwPlatformMngt.ProcessDelegate proc = null;
                org.omg.CORBA.Object obj = orb.string_to_object(managerURL);

                if (obj!=null) {
                    proc = com.thalesgroup.CdmwPlatformMngt.ProcessDelegateHelper.narrow(obj);
                    admin = new cdmw.tools.ProcessAdmin(orb, proc);

                    // send initialise order
                    commands= "verbose \n init LAST 2\n exit \n";
                    admin.run(new java.io.BufferedReader(
                        new java.io.StringReader(commands)), System.out);

                    // send run order
                    commands = "verbose \n run\n exit \n";
                    admin.run(new java.io.BufferedReader(
                        new java.io.StringReader(commands)), System.out);
                }
            }

            testEventAdmin();

            testTypedEventAdmin();

            // if we try to stop the EventChannelManager via the ProcessAdmin,
            // we've got a TRANSIENT exception because it has already been 
            // stopped via 'stop_ec_manager' in the commands file
            /*
            if (unitTests) {
                // send stop order
                commands = "verbose \n stop\n exit \n";
                admin.run(new java.io.BufferedReader(
                    new java.io.StringReader(commands)), System.out);
            }
            */

        } catch(Exception e) {
            e.printStackTrace();
            fail(e.toString());
        }

        if (orb != null) {
            try {
                orb.destroy();
            } catch(Exception ex) {
                ex.printStackTrace();
            }
        }

        try {
            if (unitTests) cdmw.tools.SimuUtils.stopSimu(System.out);
            OS.sleep(getTimescale() * 10000);

            System.out.print("Killing EventChannelManager...");
            OS.killProcess(profileManager);
            System.out.println(" Done.");
        } catch (InterruptedException ie) {
            ie.printStackTrace();
        }

    }

    public void testEventAdmin() {

        ThreadedProcess eventServer = null;

        try {
            // Starting Eventservice
            eventServer
                = ORBUtils.startEventService("-ORBconfig ./ob.conf");
            OS.sleep(getTimescale() * 10000);
            println("Eventserv started");

            int[] awaitedFailedTests = {9};
            testEventAdminTool("./ec_manip.cmd", awaitedFailedTests);

            OS.killProcess(eventServer);
        } catch(Exception e) {
            e.printStackTrace();
            fail(e.toString());
        }

    }

    public void testTypedEventAdmin() {

        ThreadedProcess irServ = null;
        ThreadedProcess typedEventServer = null;

        try {
            // Starting irserv (needed by typed event service)
            //irServ = OS.createProcess("irserv -OAport 21412 " +
            //    "-I../../../../../export/idl ../../idl/Echo.idl");
            //OS.sleep(getTimescale() * 5000);
            //println("irserv started");

            OS.sleep(getTimescale() * 10000);
            // Starting TypedEventservice
            typedEventServer
                = ORBUtils.startEventService("-t -ORBconfig ./ob.conf");
            OS.sleep(getTimescale() * 10000);
            println("TypedEventserv started");

            int[] awaitedFailedTests = {5};
            testEventAdminTool(
                "./typed_ec_manip.cmd", awaitedFailedTests);

            OS.killProcess(typedEventServer);
            //OS.killProcess(irServ);
        } catch(Exception e) {
            e.printStackTrace();
            fail(e.toString());
        }

    }

    public static String readFromFile(String file) {

        try {
            java.io.BufferedReader in = new java.io.BufferedReader(
                new java.io.FileReader(file) );
            return in.readLine();
        } catch(java.io.IOException ioe) {
            return "";
        }
    }

}

