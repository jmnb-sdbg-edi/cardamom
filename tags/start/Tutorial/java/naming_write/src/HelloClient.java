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


public class HelloClient {

    private static final int POA_THREAD_POOL_SIZE = 5;
    
    private static final String NAMING_REPOS_URL_OPTION = "--namingRepos-url";
       
    
    private static void usage() {
        System.out.println("Usage:\n");
        System.out.println("helloClient.sh options\n");
        System.out.println("Options:\n");
        System.out.println();
        System.out.println("--help                     Display this information.\n");
        System.out.println();
        System.out.println(NAMING_REPOS_URL_OPTION + "=<URL>  URL of the CDMW Naming and Repository service.\n");
        System.out.println();
    }
    

    
    public static void main(String[] args) {
        
        System.out.println("Start Hello Client");
      
        //
        // help argument
        //
        if(args.length > 0 && args[0].equals("--help"))	{    
            usage();
            return;
        }

        //
        // get Naming & Repository URL from arguments
        //
        String namingReposURL = 
             cdmw.ossupport.OS.getOptionValue (args, NAMING_REPOS_URL_OPTION);
             
        if (namingReposURL.equals("no")) 
        {
            System.out.println("naming and repository url undefined");
            usage();
            return;
        }

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
            // Get the naming and repository
            //     from the input URL
            // ===================================================
            com.thalesgroup.CdmwNamingAndRepository.Repository repository = null;
                                      
            org.omg.CORBA.Object obj2 = orb.string_to_object(namingReposURL);
            repository = com.thalesgroup.CdmwNamingAndRepository.RepositoryHelper.narrow(obj2);            
            

            // ======================================================   
            // Initialise the repository interface
            //     "TUTORIAL" has been chosen as the default domain name
            // ======================================================
            cdmw.namingandrepository.RepositoryInterface.init("TUTORIAL", repository);
             
            
            // ======================================================
            // Get Naming Interface of HELLO name domain
            // ======================================================
            cdmw.namingandrepository.NamingInterface niHello =
                cdmw.namingandrepository.RepositoryInterface.getDomainNamingInterface (
                    "TUTORIAL/HELLO");
	                

            // ===================================================
            // Get the John HelloInterface object from repository
            // ===================================================
            org.omg.CORBA.Object objref = niHello.resolve("US/HelloInterface");
    
            // get the John HelloInterface proxy
            com.thalesgroup.HelloModule.HelloInterface helloInterfaceJohn = 
                com.thalesgroup.HelloModule.HelloInterfaceHelper.narrow(objref);
    
            if (helloInterfaceJohn == null)
            {
                System.out.println ("could not narrow object 'John' to type HelloInterface");
                return;
            }
            
            System.out.println ("<------- Get John from  TUTORIAL/HELLO/US ------>");
            
            // ======================================================
            // Call displayHello method of helloInterfaceJohn object
            // ======================================================
            helloInterfaceJohn.display_hello();


            
            
                
            // ===================================================
            // try to create the "Mike" HelloInterface servant
            // and register it in the repository using cos naming
            // ===================================================
            
            // ===================================================
            // create the "Mike" HelloInterface servant
            // ===================================================
            com.thalesgroup.HelloModule.HelloInterface helloInterfaceMike = null;
            
            {
                HelloInterfaceImpl helloInterfaceServant = new HelloInterfaceImpl("Mike");
    
                // activate servant on POA
                byte[] oid = rootPOA.activate_object(helloInterfaceServant);
                           
                org.omg.CORBA.Object object = rootPOA.id_to_reference(oid);
                helloInterfaceMike = helloInterfaceServant._this(orb);
            }
	    
            // ======================================================
            // Get Naming Interface of US name domain
            // ======================================================
            cdmw.namingandrepository.NamingInterface niUs =
                cdmw.namingandrepository.RepositoryInterface.getDomainNamingInterface (
                    "TUTORIAL/HELLO/US");
             	                
            // ==========================================================
            // Try to bind the HelloInterface object in the US naming context
            // ==========================================================        
            try
            {
                niUs.bind ("HelloInterfaceMike", helloInterfaceMike, true);
            }
            catch(org.omg.CORBA.NO_PERMISSION e)
            {
                System.out.println(">>>> Creation of HelloInterface for Mike is not permitted \n" +
                                   "     in the default root context");
            }           
            
            
            // ======================================================  
            // get the admin root context
            // ======================================================
            org.omg.CosNaming.NamingContext adminRootContext =
                 repository.resolve_root_context(
                    cdmw.common.Locations.ADMIN_ROOT_CONTEXT_ID);
            
            // ======================================================                      
            // create a naming interface on admin root context
            // ====================================================== 
            cdmw.namingandrepository.NamingInterface niAdminRoot =
                new cdmw.namingandrepository.NamingInterface(adminRootContext);            
            
            // ==========================================================
            // Bind the HelloInterface object in the US naming context
            // ==========================================================
            niAdminRoot.bind ("HelloInterfaceMike", helloInterfaceMike, true);
            
           System.out.println(">>>> Creation of HelloInterface for Mike is successfull \n" +
                              "     in the admin root context");            
            
            
                        
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
