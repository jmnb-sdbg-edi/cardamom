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
            // Get default root context from repository
            // ======================================================
            org.omg.CosNaming.NamingContext rootContext = 
                repository.resolve_root_context(
                    com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT.value);            
            

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
            // Get Naming Interface of Default root context
            // ======================================================
            cdmw.namingandrepository.NamingInterface niDefRoot =
                new cdmw.namingandrepository.NamingInterface(rootContext);
                                                         
             	                
            // =================================================================
            // Try to bind the HelloInterface object in the default root context
            // =================================================================
            try
            {
                niDefRoot.bind ("HelloInterfaceMike", helloInterfaceMike, true);
                System.out.println(">>>> successful binding of HelloInterface for Mike \n" +
                                   "     in the default root context");
            }
            catch(org.omg.CORBA.NO_PERMISSION e)
            {
                System.out.println(">>>> Error in creation of HelloInterface for Mike \n" +
                                   "     in the default root context");
            }           
            
            
                        
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
