/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


package cdmw.eventadmin;

import cdmw.common.Assert;
import cdmw.common.Locations;
import cdmw.ossupport.OS;
import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.ExceptionMinorCodes;

import cdmw.namingandrepository.RepositoryInterface;
import cdmw.namingandrepository.NamingInterface;

import cdmw.platformmngt.PlatformInterface;
import cdmw.platformmngt.ProcessBehaviour;
import cdmw.platformmngt.ServiceNames;

/**
 * This class works as a command line interface to create typed and untyped 
 * event channels, to create/modify/remove profiles, 
 * to list created event channel, to display profiles.
 * This process can be launched with a command file as argument.
 *
 */
public class EventServiceAdmin {

    /**
     * Default return value in case of successful execution
     */
    public static final int SUCCESS = 0;
    
    /**
     * Default return value in case of failed execution
     */
    public static final int FAILURE = 1;  

    /**
     * Default thread pool size of the root POA
     */
    public static final int POA_THREAD_POOL_SIZE = 5;    

    /**
     * Name of the command line option for a command file
     */
    public static final String COMMAND_FILE_NAME_OPTION 
        = "--commandFile-name";

    /**
     * display a help message to use the EventServiceAdmin
     *
     * @param out Stream where the message has to be printed
     */    
    public static void usage(java.io.PrintStream out) {
        out.println("Usage:\n"
        + "EvenServiceAdmin" + " options\n"
        + "\n"
        + "Options:\n"
        + "--help                       Display this information.\n"
        + "[--commandFile-name=<name>]  Name of command file.\n");
    }
    
    /**
     * Thread launched in run operation of ProcessBehaviour
     */
    public static class AdminThread extends Thread {
    
        private EventServiceAdminTool evtAdm;

        public AdminThread(EventServiceAdminTool evtAdm) {
            this.evtAdm = evtAdm;
        }

        public void run() {
            if (evtAdm != null) {
                java.io.BufferedReader in = new java.io.BufferedReader(
                    new java.io.InputStreamReader(System.in));
                evtAdm.run(in, System.out, false);
            } else {
                System.err.println("AdminThread: nothing to run!");
            }
        }

    }
    
    /**
     * Default Process Behaviour
     */
    public static class MyProcessBehaviour extends ProcessBehaviour {
            
        private org.omg.CORBA.ORB orb;

        private org.omg.CosNaming.NamingContext namingContext = null;

        private String commandFilename;
                    
        private AdminThread runThread = null;
            
        public MyProcessBehaviour(org.omg.CORBA.ORB orb, String commandFilename) {
            this.orb = orb;
            this.commandFilename = commandFilename;
        }
          
        public void initialise(
            com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind) {
        
            System.out.println("INFO: cdmw_event_admin init");
                            
            // get the repository service
            try {
                org.omg.CORBA.Object obj = PlatformInterface.getService(
                    ServiceNames.NAMING_AND_REPOSITORY_SERVICE);
                com.thalesgroup.CdmwNamingAndRepository.Repository repository 
                    = com.thalesgroup.CdmwNamingAndRepository.RepositoryHelper.narrow(obj);
                this.namingContext = repository.resolve_root_context(
                    com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT.value);
            } catch (com.thalesgroup.CdmwPlatformMngtService.ServiceNotFound snf) {
                // Cannot Proceed with no CDMW Repository Ior
                throw new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMRepositoryNotFound,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            } catch (com.thalesgroup.CdmwPlatformMngtService.ServiceUnavailable su) {
                // Cannot Proceed with no CDMW Repository Ior
                throw new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMCannotGetRepositoryReference,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            } catch(com.thalesgroup.CdmwNamingAndRepository.RepositoryPackage.NoRootContext nrc) {
                throw new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMCannotGetRepositoryReference,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO); 
            } catch(cdmw.common.BadParameterException bpe) {
                throw new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAM,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            } catch(cdmw.common.BadOrderException boe) {
                throw new org.omg.CORBA.BAD_INV_ORDER(
                    ExceptionMinorCodes.BAD_INV_ORDER,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }

            EventServiceAdminTool adm = new EventServiceAdminTool(
                this.orb, 
                this.namingContext, 
                this.commandFilename);

            try {
                adm.init();
            } catch (InitException ie) {
                System.out.println(ie.what());
                try {
                    PlatformInterface.notifyFatalError(
                        "cdmw_event_admin", ie.what());
                } catch(Exception e) { System.out.println(e.toString());}
                return;
            }

            // if no command file, get input command from standard input
            if (commandFilename.equals("")) {
                this.runThread = new AdminThread(adm);
            // else command got from command file
            } else {
                java.io.BufferedReader commandFile = null;
                try {
                    commandFile = new java.io.BufferedReader(
                        new java.io.FileReader(this.commandFilename));
                } catch(java.io.FileNotFoundException fnfe) {}

                if (commandFile == null) {
                    System.out.println("cannot open file : " + this.commandFilename);
                    try {
                        PlatformInterface.notifyFatalError(
                            "cdmw_event_admin", "cannot open command file");
                    } catch(Exception e) { System.out.println(e.toString());}
                } else {
                    adm.run(commandFile, System.out, true);
                }
            }  
        }
            
        public void run() 
            throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.NotReadyToRun {
            System.out.println("INFO: cdmw_event_admin run");

            if (this.runThread != null) {
                // wait for command
                this.runThread.start();
            } else {
                // a command file was given and execute during init
                // now stop process
                this.orb.shutdown(false);
            }
        }

        public void stop() {
            System.out.println("INFO: cdmw_event_admin stop");
            this.orb.shutdown(false);

            if (this.runThread != null) {
                try {
                    this.runThread.join();
                } catch(InterruptedException ie) {}
            }
        }
            
    }
        

    /**
     * Create and start the EventServiceAdminTool
     *
     * @param orb A reference to the ORB pseudo-object
     * @param nc Default Cdmw Root Naming Context 
     * @param commandFileName Name of the command file
     */
    public static int run(org.omg.CORBA.ORB orb, 
        org.omg.CosNaming.NamingContext nc, 
        String commandFileName) {
        
        EventServiceAdminTool adm 
            = new EventServiceAdminTool(orb, nc, commandFileName);

        try {
            adm.init();
        } catch (InitException ie) {
            System.out.println(ie.what());
            return FAILURE;
        }

        // if no command file, get input command from standard input
        if (commandFileName.equals("")) {
            adm.run(new java.io.BufferedReader(
                new java.io.InputStreamReader(System.in)), System.out, false);
        // else command got from command file
        } else {
            try {
                adm.run(new java.io.BufferedReader(
                    new java.io.FileReader(commandFileName)), System.out, true);
            } catch(java.io.FileNotFoundException fnfe) {
                System.out.println("cannot open file : " + commandFileName);
            }
        }  

        return SUCCESS;
    }

    /**
     * Start the EventServiceAdmin
     *
     * @param args Options of the command line
     */
    public static void main(String[] args) {

        int status = SUCCESS;
        org.omg.CORBA.ORB orb = null;

        //
        // help argument
        //
        if(args.length > 0) {
            if (args[0].equals("--help")) {
                usage(System.out);
                System.exit(SUCCESS);
            }
        }

        try {
            // Initialize the ORB
            cdmw.orbsupport.StrategyList orbStrategies 
                = new cdmw.orbsupport.StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
    
            orb = ORBSupport.ORBInit(args, orbStrategies);
            
            org.omg.CORBA.Object poaObj = 
                orb.resolve_initial_references("RootPOA");
            org.omg.PortableServer.POA poa = 
                org.omg.PortableServer.POAHelper.narrow(poaObj);
            org.omg.PortableServer.POAManager poaManager = poa.the_POAManager();
    
            // get Input Command File name argument
            //
            String commandFile = OS.getOptionValue(args, COMMAND_FILE_NAME_OPTION);
            if (commandFile.equals("no")) {
                commandFile = "";
            }

            boolean platformManaged = 
                PlatformInterface.isLaunchedByPlatformManagement(args);
            
            if (platformManaged) {
                poaManager.activate();
                
                // initialise the platform interface
                PlatformInterface.setup(orb, args);
                
                // Create a Process Behaviour
                // acknowledge the creation of the process
                PlatformInterface.acknowledgeCreation(
                    new MyProcessBehaviour(orb, commandFile));
                
                orb.run();
            } else {
                // Get naming service
                org.omg.CORBA.Object obj = 
                    orb.resolve_initial_references("NameService");
                
                System.out.println("NameService resolved!");
                if(obj == null) {
                    System.err.println("`NameService' is a nil object reference");
                    throw new org.omg.CORBA.TRANSIENT(
                        ExceptionMinorCodes.TRANSIENTCannotReachNameService,
                        org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                }
                
                org.omg.CosNaming.NamingContext nc =
                    org.omg.CosNaming.NamingContextHelper.narrow(obj);
                
                System.out.println("NameService is a naming context!");
                
                if (nc == null) {
                    System.err.println("`NameService' is not a NamingContext object reference");
                    throw new org.omg.CORBA.TRANSIENT(
                        ExceptionMinorCodes.TRANSIENTCannotReachNameService,
                        org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                }
            
                status = run (orb, nc, commandFile);
            }
        } catch(org.omg.CORBA.ORBPackage.InvalidName in) {
            System.out.println("Can't resolve `NameService'");
            status = FAILURE;
        } catch(Exception e) {
            e.printStackTrace();
            status = FAILURE;
        }

        if(orb != null) {
            try {
                orb.destroy();
            } catch(Exception e) {
                e.printStackTrace();
                status = FAILURE;
            }
        }        

        if (status == FAILURE) {
            System.out.println("EventServiceAdmin abnormal exit !");
        }
        
    }
    
}


