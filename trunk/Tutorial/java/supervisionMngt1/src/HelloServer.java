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


package cdmw.tutorial;


public class HelloServer {
    
    private static int POA_THREAD_POOL_SIZE = 5;
    
    private static final String COLLECTOR_NAME_OPTION = "--collector-name";
    
    private static void usage() {
        System.out.println("Usage:\n");
        System.out.println("helloServer.sh options\n");
        System.out.println("Options:\n");
        System.out.println();
        System.out.println("--help                     Display this information.\n");
        System.out.println();
        System.out.println("[" + COLLECTOR_NAME_OPTION + "]=<name>  Name of trace collector..\n");
        System.out.println();
    }
    
    
    
    public static void main(String[] args) {
        
        System.out.println("Start Hello Server");
        
        //
        // help argument
        //
        if(args.length > 0 && args[0].equals("--help"))    {    
            usage();
            return;
        }

        //
        // get Trace Collector Name from arguments
        //
        String collectorName = 
            cdmw.ossupport.OS.getOptionValue (args, COLLECTOR_NAME_OPTION);
             
        if (collectorName.equals("no"))
        {
            collectorName = "";
            System.out.println("no trace collector defined");
            System.out.println("  (use the option " + COLLECTOR_NAME_OPTION +
                               "=<name> if required");
        }

        org.omg.CORBA.ORB orb = null;              // orb reference
        String applicationName = "Tutorial";       // name of application
        String processName = "HelloServer";        // name of process in the application
        String msg = null;                         // base message buffer (application and process names)
            
        try  {
            // ===================================================
            // Initialize the ORB
            // ===================================================        
            cdmw.orbsupport.StrategyList orbStrategies = 
                new cdmw.orbsupport.StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
        
            orb = cdmw.orbsupport.ORBSupport.ORBInit(args, orbStrategies);

            // ===================================================
            // Get the root POA 
            // ===================================================
            org.omg.CORBA.Object obj = orb.resolve_initial_references("RootPOA");
            org.omg.PortableServer.POA rootPOA = 
                org.omg.PortableServer.POAHelper.narrow(obj);
            
            // ===================================================
            // Activate the root POA manager
            // ===================================================
            org.omg.PortableServer.POAManager poaManager = rootPOA.the_POAManager();
            poaManager.activate();


            // ==============================================
            // Initialise the platform management interface
            // ==============================================
            
            // Call platform interface setup
            cdmw.platformmngt.PlatformInterface.setup(orb, args);
            
            // creates the hello process behaviour
            HelloProcessBehaviour helloProcess = new HelloProcessBehaviour(orb);
                                                  
            // acknowledge the creation of the process
            cdmw.platformmngt.PlatformInterface.acknowledgeCreation(helloProcess);
                        
            // sample usage of the platform interface                    
            applicationName = cdmw.platformmngt.PlatformInterface.getApplicationName();
    
            processName = cdmw.platformmngt.PlatformInterface.getProcessName();     
            
            //
            // example of using the PlatformInterface for notifying a message
            //
            cdmw.platformmngt.PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                processName, 
                ">>>>>>>>>>>>>> Launching .....");
            
            
            // ===================================================
            // Get the naming and repository
            //     from the platform management services
            // ===================================================
            com.thalesgroup.CdmwNamingAndRepository.Repository repository = null;
                                      
            org.omg.CORBA.Object obj2 = 
                cdmw.platformmngt.PlatformInterface.getService(
                    cdmw.platformmngt.ServiceNames.NAMING_AND_REPOSITORY_SERVICE);
            repository = com.thalesgroup.CdmwNamingAndRepository.RepositoryHelper.narrow(obj2);            
            
            
            // ======================================================   
            // Initialise the repository interface
            //     "CDMW" has been chosen as the default domain name
            //     Note : RepositoryInterface is necessary for 
            //            trace library InitUtils::init_trace_library
            // ======================================================
            cdmw.namingandrepository.RepositoryInterface.init("CDMW", repository);


            // ===================================================
            // Initialise the trace service
            // ===================================================
            {
                // create the collector name list with the input
                // collector name 
                java.util.Vector collectorNameList = new java.util.Vector();
                
                if (collectorName != "")
                {
                    collectorNameList.add(collectorName);
                }
                
                // Init the trace library
                //     5000    is the time to wait before flushing the current flush area
                //             containing the messages to trace although it is not full.      
                //     2       is the number of flush area to be used to store messages
                //             to trace
                //     50*1024 size of each flush area to be used to store messages to trace
                //             must be a multiple of 1024
                //
                // May raise CORBA::SystemException
                
                cdmw.trace.InitUtils.initTraceLibrary(rootPOA,
                                                       applicationName,
                                                       processName,
                                                       100, 5000,
                                                       2,
                                                       50*1024,
                                                       collectorNameList);
                
                // active the trace flushing to trace collector
                // (start to send messages to trace collector)
                cdmw.trace.Tracer.activeFlushing();
            }
            
            
            System.out.println("Hello Server Started");
            
            String baseMsg = "process <" + applicationName + "/" + processName + ">" ;
            
            // ===================================================
            // trace a message 
            // ===================================================
            msg = baseMsg + " started and waiting CORBA messages";  
                                           
            cdmw.trace.Tracer.trace("TUTOR", 
                                     (short) 1, 
                                     msg);


             
            // ===================================================
            // wait for CORBA message
            // ===================================================
            orb.run();

        } catch  (cdmw.common.Exception ex) {
            System.err.println(ex.what());
            ex.printStackTrace();
            
        } catch (org.omg.CORBA.SystemException ex) {
            System.err.println(ex.toString());
            ex.printStackTrace();

        } catch (org.omg.CORBA.UserException ex) {
            System.err.println(ex.toString());
            ex.printStackTrace();
        }

        
        // ========================================================
        // program stopping
        // ========================================================
        
        // ================================================================
        // Release all static object references held by RepositoryInterface
        // ================================================================
        cdmw.namingandrepository.RepositoryInterface.finish();
                
        // =====================================================
        // Free all allocated memory and stop the trace service
        // =====================================================
        cdmw.trace.FlushAreaMngr.cleanup();
        
    
        // =====================================================
        // Destroy orb
        // =====================================================
        if (orb != null) {
            try {
                orb.destroy();
            } catch (org.omg.CORBA.SystemException ex) {
                System.out.println(ex.toString());
                ex.printStackTrace();
            }
        }
        
    }
    
}

