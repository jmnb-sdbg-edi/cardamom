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
    
    private static final int POA_THREAD_POOL_SIZE = 5;
    
    private static final String TYPED_CHANNEL_NAME = "TypedEventChannel1";
    private static final String INTERFACE_REP = com.thalesgroup.TypedEchoHelper.id(); 
    
    public static void main(String[] args) {
        
        System.out.println("Start Hello Server");
        
        org.omg.CORBA.ORB orb = null;                                        // orb reference
        cdmw.event.TypedEventChannelInterface typedeventchannel = null;     // event channel interface        
        
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

            
            // ===================================================
            // create the process control for platform management
            // ===================================================
            cdmw.cdmwinit.ProcessControl processCtrl =
                  new HelloServerProcessControl(orb);
    
    
            // ===================================================
            // Call generated cdmwinit
            // ===================================================
            cdmw.generated.HelloServerCdmwInterface.cdmwInit(orb, args, processCtrl);
            
            
            // ===================================================
            // Get the process name and application name
            // ===================================================
            String processName = cdmw.cdmwinit.InitUtils.getCdmwProcessName();
            
            String applicationName = cdmw.cdmwinit.InitUtils.getCdmwApplicationName();
                 
 
            System.out.println("Hello Server Started");
            
            String baseMsg = "process <" + applicationName + "/" + processName + ">" ;
            

            // ========================================================
            // Get the naming and repository from Repository interface
            // ========================================================
            com.thalesgroup.CdmwNamingAndRepository.Repository repository = null;  
                                      
            repository = cdmw.namingandrepository.RepositoryInterface.getRepository();
              
              
              
            // ========================================================
            // Get the event channel factory from Event Channel Manager
            // ========================================================
    
            // Get NamingInterface object from RepositoryInterface
            cdmw.namingandrepository.NamingInterface ni =
                cdmw.namingandrepository.RepositoryInterface.getDomainNamingInterface(
                    cdmw.common.Locations.CDMW_SERVICES_NAME_DOMAIN);
    
    
            // Get a reference on the EventChannelFactory
            System.out.println("Find a factory to create a typed channel");
            
            org.omg.CORBA.Object obj2 = ni.resolve("EVENT/FACTORIES/DefaultTypedEventChannelFactory");
            
            com.thalesgroup.CdmwEvent.TypedEventChannelFactory factory = 
                com.thalesgroup.CdmwEvent.TypedEventChannelFactoryHelper.narrow(obj2);
            
            if (factory == null) {
                System.out.println("DefaultEventChannelFactory is a nil object reference");
                    
                return;
            }
    
    
            // ========================================================
            // Create a new generic event channel using factory
            // ========================================================
            System.out.println("Creating a new generic channel TypedEventChannel");
    
            org.omg.CosTypedEventChannelAdmin.TypedEventChannel channel =
                factory.create_event_channel(TYPED_CHANNEL_NAME);
    
            // ========================================================
            // Create the associated event channel interface
            // ========================================================
            typedeventchannel = new cdmw.event.TypedEventChannelInterface(channel);   
                   
                   
            // ========================================================
            // Create and connect a push consumer to the channel
            // ========================================================       
            System.out.println("Connect local consumer to TypedEventChannel1");
            
            cdmw.tutorial.EchoImpl pcImpl = 
                new cdmw.tutorial.EchoImpl(orb, rootPOA);
    
            com.thalesgroup.TypedEcho ref = pcImpl._this(orb);
            typedeventchannel.connectTypedPushConsumer(INTERFACE_REP, ref);


            // ===================================================
            // trace a message
            // ===================================================
            String msg = baseMsg + " started and waiting CORBA messages";  
                                           
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

        // ===================================================
        // Event clean up
        // ===================================================
        typedeventchannel.destroy();
        typedeventchannel = null;

        if (orb != null) {
            try {
                // ===================================================
                // Call generated Cdmw cleanup
                // ===================================================
                cdmw.generated.HelloServerCdmwInterface.cdmwCleanup(orb);
        
                // =====================================================
                // Destroy orb
                // =====================================================
                orb.destroy();

            } catch (org.omg.CORBA.SystemException ex) {
                System.out.println(ex.toString());
                ex.printStackTrace();
            }
        }

    }
    
}

