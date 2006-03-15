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


package cdmw.resources.test;

import cdmw.testutils.Testable;
import cdmw.common.Assert;
import cdmw.common.Locations;
import cdmw.common.Options;

import cdmw.ossupport.OS;
import cdmw.ossupport.ThreadedProcess;
import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.ORBUtils;
import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.ExceptionMinorCodes;

import cdmw.commonsvcs.naming.NamingInterface;
import cdmw.platformmngt.PlatformInterface;
import cdmw.cdmwinit.CdmwInterface;

import com.thalesgroup.CdmwPlatformMngt.ProcessCallback;
import com.thalesgroup.CdmwPlatformMngt.ProcessCallbackHelper;

import org.omg.CORBA.ORB;
import org.omg.CORBA.TRANSIENT;
import org.omg.CORBA.CompletionStatus;

public class MainTest {

    public final static int SUCCESS = 0;
    public final static int FAILURE = 1;

    public final static String REPOSITORY_PORT_OPTION
        = "--RepositoryPort";
    public final static String COLLECTOR_SERV_PORT_OPTION
        = "--CollectorServicePort";
    public final static String COLLECTOR_CDMW_PORT_OPTION
        = "--CollectorCdmwPort";

    // The naming context of the current Process object within the naming
    public final static String PROC_CTXT
        = "CDMW.I/ProcessCallback.simulated/CDMW Test Application/PROC_000";

    // 3 seconds before calling initialise
    public final static int INIT_WAIT_DURATION = 3000;

    // 10 seconds processing time before calling stop
    public final static int PROCESSING_DURATION = 10000;

    public final static int POA_THREAD_POOL_SIZE = 5;


    public static ProcessCallback getProcessCallback(ORB orb)
        throws TRANSIENT {

        Assert.check(orb != null);

        org.omg.CORBA.Object obj = null;

        //
        // Get naming service
        //
        try {
            obj = orb.resolve_initial_references("NameService");
        }
        catch(org.omg.CORBA.ORBPackage.InvalidName in) {
            System.out.println("Can't resolve `NameService'");
            throw new TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                    CompletionStatus.COMPLETED_NO);
        }

        if ( obj == null ) {
            System.out.println("`NameService' is a nil object reference");
            throw new TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                    CompletionStatus.COMPLETED_NO);
        }

        org.omg.CosNaming.NamingContext nc
            = org.omg.CosNaming.NamingContextHelper.narrow(obj);

        if ( nc == null ) {
            System.out.println("`NameService' is not a NamingContext object reference");
            throw new TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                    CompletionStatus.COMPLETED_NO);
        }

        String PROCESS_CALLBACK_BINDING_NAME
            = "CDMW.I/process-callback.simulated";

        // Get reference to the ApplicationAgent interface
        ProcessCallback callback = null;
        try {
            NamingInterface ni = new NamingInterface(nc);
            obj = ni.resolve(PROCESS_CALLBACK_BINDING_NAME);
            callback = ProcessCallbackHelper.narrow(obj);
            if ( callback == null ) {
                System.out.println("Couldn't retrieve reference of the ProcessCallback");
                throw new TRANSIENT(
                    ExceptionMinorCodes.TRANSIENTCannotReachApplicationAgent,
                     CompletionStatus.COMPLETED_NO);
            }
        } catch (Exception e) {
            System.out.println("Couldn't retrieve reference of the ProcessCallback");
            throw new TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachApplicationAgent,
                     CompletionStatus.COMPLETED_NO);
        }

        return callback;
    }

    public static int run(org.omg.CORBA.ORB orb,
        boolean unitTests, String portNumber) {

        CommandThread platform = null;
        if (unitTests) {
            String procURL = "corbaname::localhost:"
                + portNumber + "#" + PROC_CTXT;
            platform = new CommandThread(orb,
                procURL, INIT_WAIT_DURATION, PROCESSING_DURATION);
            platform.start();
        }

        orb.run();
        if (unitTests) {
            try {
                platform.join();
            } catch(InterruptedException ie) {
                return FAILURE;
            }
        }

        return SUCCESS;

    }

    public static void main(String[] args) {

        int timescale = Testable.getTimescale();
        String callbackIOR
            = OS.getOptionValue( args, Options.CALLBACK_IOR_OPTION);

        boolean unitTests = true;
        // --- Is it launched by platform management for integration tests? ---
        if ( callbackIOR.equals("no") && callbackIOR.equals("yes") ) {
            unitTests = false;
        }

        String allTests = OS.getOptionValue(args, "--interactive");
        // ---- Does it use interactive mode? ----
        boolean interactiveMode = false;
        if ( !allTests.equals("no") ) {
            interactiveMode = true;
        }

        // launch Nameservice and SimRepository for unit tests only.
        ThreadedProcess namingp = null;
        ThreadedProcess repositoryp = null;
        ThreadedProcess traceCollector = null;

        String portNumber = "";
        String[] orbInitArgs = null;

        if (unitTests) {

            // We allow the user to specify a different port number for the Name Service
            portNumber = OS.getOptionValue(args, "--CdmwNamePort") ;

            if ( portNumber.equals("no") ) {
                portNumber="5000";
            }

            try {
                // Starting the Name Service
                System.out.println("Starting Name Service on port "
                    + portNumber + "...");
                namingp = ORBUtils.startNamingService(portNumber);
                OS.sleep(timescale * 10000);

                // We allow the user to specify a different port number for the Repository
                String repositoryPort
                    = OS.getOptionValue (args, REPOSITORY_PORT_OPTION);

                if ( repositoryPort.equals("no") ) {
                    repositoryPort = "5020";
                }

               // String namingCorbaloc = " -ORBnaming corbaloc::localhost:"
                 //   + portNumber + "/NameService";
		        String namingCorbaloc = ORBUtils.NAMING_PORT_OPTION_SLAVE + "corbaloc::localhost:" + portNumber + "/NameService";
            
                // Starting the Repository Simulator
                String simRepositoryArgs
                    = ORBUtils.PORT_OPTION_SLAVE + repositoryPort + " " + namingCorbaloc;
                simRepositoryArgs += " -DOM ";
                simRepositoryArgs += TestData.get_RESOURCES_DOMAIN_NAME();
                simRepositoryArgs += " -DOM ";
                simRepositoryArgs += TestData.get_FACTORIES_DOMAIN_NAME();
                simRepositoryArgs += " -DOM ";
                simRepositoryArgs += Locations.CDMW_SERVICES_NAME_DOMAIN;
                simRepositoryArgs += " -DOM ";
                simRepositoryArgs += Locations.CDMW_SERVICES_NAME_DOMAIN;
                simRepositoryArgs += "/";
                simRepositoryArgs += Locations.TRACE_COLLECTORS_NAME_DOMAIN;
                simRepositoryArgs += " -ROOT ";
                simRepositoryArgs += Locations.ADMIN_ROOT_CONTEXT_ID;


                System.out.println("Starting Simulated Repository...");
                repositoryp = OS.createProcess(
                "cdmw_simulated_repository " + simRepositoryArgs);
                OS.sleep(timescale * 10000);

                String repositoryCorbaloc =
                    " --namingRepos-url=corbaloc::localhost:"
                    + repositoryPort + "/SimRepository";

                // We allow the user to specify a different port numbers

                // get TraceCollector Service port
                String collectorServPort
                    = OS.getOptionValue(args, COLLECTOR_SERV_PORT_OPTION);
                if ( collectorServPort.equals("no") ) {
                    collectorServPort = "5031";
                }

                // get TraceCollector Cdmw port
                String collectorCdmwPort
                    = OS.getOptionValue(args, COLLECTOR_CDMW_PORT_OPTION);
                if ( collectorCdmwPort.equals("no") ) {
                    collectorCdmwPort = "5032";
                }

                String traceCollectorArgs =
                    " --collector-name=" + "collector_0"
                    + " --traceFile-name=" + "test_trace"
                    + " --CdmwLocalisationService=" + collectorServPort
                    + " --CdmwProcessPort=" + collectorCdmwPort
                    + repositoryCorbaloc;

                System.out.println("Starting Trace Collector...");
                traceCollector
                    = OS.createProcess("cdmw_trace_collector" + traceCollectorArgs);
                OS.sleep(timescale * 10000);

            } catch(java.io.IOException ioe) {
                System.out.println(ioe.getMessage());
            } catch(InterruptedException ie) {
                System.out.println(ie.getMessage());
            }

            //orbInitArgs = new String[1];
            //orbInitArgs = new String[2];
            //orbInitArgs[0] = "-ORBnaming";
            //orbInitArgs[0] = ORBUtils.NAMING_PORT_OPTION + "corbaloc::localhost:" + portNumber + "/NameService";
            //orbInitArgs[1] = "corbaloc::localhost:"
            //       + portNumber + "/NameService";
            String s = ORBUtils.NAMING_PORT_OPTION + "corbaloc::localhost:" + portNumber + "/NameService";
            orbInitArgs = s.split(" ");
        }

        int status = SUCCESS;
        ORB orb = null;

        String[] args_ = null;
        MainTestProcessControl ctrl = null;

        try {
            // Initialize the ORB
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);

            if (unitTests) {
                orb = ORBSupport.ORBInit(orbInitArgs, orbStrategies);

                 // Add Application Agent IOR and Process Name
                ProcessCallback callback = getProcessCallback(orb);
                String s = orb.object_to_string(callback);
                args_ = new String[3];
                args_[0] = Options.CALLBACK_IOR_OPTION + "=" + s;
                args_[1] = Options.PROCESS_XML_FILE_OPTION + "=Process_Gen.xml";
                args_[2] = Options.VALIDATE_XML_OPTION;

            } else {
                orb = ORBSupport.ORBInit(args, orbStrategies);
                args_ = args;
            }

            java.io.BufferedReader reader;
            if (!interactiveMode) {
                reader = new java.io.BufferedReader(
                    new java.io.StringReader("verbose \n all \n x \n"));
            } else {
                reader = new java.io.BufferedReader(
                    new java.io.InputStreamReader(System.in));
            }
            ctrl = new MainTestProcessControl(orb, reader, System.out);
            CdmwInterface.cdmwInit(orb, args_,
                ctrl, "cdmw.resources.test.CdmwInterface");

            // Get the root POA
            org.omg.CORBA.Object obj =
                orb.resolve_initial_references("RootPOA");
            org.omg.PortableServer.POA rootPOA
                = org.omg.PortableServer.POAHelper.narrow(obj);

            // Run implementation (Activate the POA manager)
            rootPOA.the_POAManager().activate();

            status = run(orb, unitTests, portNumber);

        } catch (Exception ex) {
            ex.printStackTrace();
            status = FAILURE;
        }

        if (orb != null) {
            try {
                CdmwInterface.cdmwCleanup(orb,
                    "cdmw.resources.test.CdmwInterface");
                orb.destroy();
            } catch (Exception ex) {
                ex.printStackTrace();
                status = FAILURE;
            }
        }

        if (unitTests) {
            if ( traceCollector != null ) {
                System.out.println("Killing trace collector...");
                OS.killProcess( traceCollector );
            }
            if ( repositoryp != null ) {
                System.out.println("Killing simulated repository...");
                OS.killProcess( repositoryp );
            }
            if ( namingp != null ) {
                System.out.println("Killing name service...");
                OS.killProcess( namingp );
            }
        }

        if ( status == SUCCESS ) {
            System.out.println("Main test successful.");
        } else {
            System.out.println("Main test failed.");
        }

    }

}

