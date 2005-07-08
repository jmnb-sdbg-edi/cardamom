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
    
    
    public static void main(String[] args) {
        
        System.out.println("Start Hello Server");
        
        org.omg.CORBA.ORB orb = null;              // orb reference
        
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
            
            
            // ========================================================
            // Get the naming and repository from Repository interface
            // ========================================================
            com.thalesgroup.CdmwNamingAndRepository.Repository repository = 
                cdmw.namingandrepository.RepositoryInterface.getRepository();
                         
            
            // ===================================================
            // create the HelloInterface servant
            // ===================================================
            com.thalesgroup.HelloModule.HelloInterface helloInterface = null;
        
            {
                HelloInterfaceImpl helloInterfaceServant = new HelloInterfaceImpl();
    
		        // activate servant on POA
		        byte[] oid = rootPOA.activate_object(helloInterfaceServant);
		                   
                org.omg.CORBA.Object object = rootPOA.id_to_reference(oid);
		        helloInterface = com.thalesgroup.HelloModule.HelloInterfaceHelper.narrow(object);
	        }


            // ======================================================
            // Get HELLO name domain from repository
            // ======================================================
            com.thalesgroup.CdmwNamingAndRepository.NameDomain helloDomain =
                repository.resolve_name_domain ("TUTORIAL/HELLO");
                 
                 
            // ========================================================
            // Register the HelloInterface object in the HELLO name domain
            // ========================================================
            // reserve the name "Hello1" in the domain
            org.omg.CORBA.Any regId = helloDomain.new_name ("Hello1");
            // register the object
            helloDomain.register_object (regId, helloInterface);
       
 
            System.out.println("Hello Server Started");
            
            String baseMsg = "process <" + applicationName + "/" + processName + ">" ;
            
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
        if (orb != null) {
            try {
                // ===================================================
                // Call generated Cdmw cleanup
                // ===================================================
                cdmw.generated.HelloServerCdmwInterface.cdmwCleanup(orb);
        
                // =====================================================
                // destroy orb
                // =====================================================
                orb.destroy();
                
            } catch (org.omg.CORBA.SystemException ex) {
                System.out.println(ex.toString());
                ex.printStackTrace();
            }
        }

    }
    
}

