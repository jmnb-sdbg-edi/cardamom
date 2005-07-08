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
    
    private static final String NAMING_REPOS_URL_OPTION = "--namingRepos-url";
       
    
    private static void usage() {
        System.out.println("Usage:\n");
        System.out.println("helloServer.sh options\n");
        System.out.println("Options:\n");
        System.out.println();
        System.out.println("--help                     Display this information.\n");
        System.out.println();
        System.out.println(NAMING_REPOS_URL_OPTION + "=<URL>  URL of the CDMW Naming and Repository service.\n");
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
            // get the default root context
            // ======================================================
            org.omg.CosNaming.NamingContext defaultRootContext =
                 repository.resolve_root_context(
                    com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT.value);

            // ======================================================  
            // create the naming interface for default root context
            // ======================================================                    
            cdmw.namingandrepository.NamingInterface niDefRoot =
                new cdmw.namingandrepository.NamingInterface(defaultRootContext);


            // ===================================================
            // create the "John" HelloInterface servant
            // ===================================================
            com.thalesgroup.HelloModule.HelloInterface helloInterfaceJohn = null;
        
            {
                HelloInterfaceImpl helloInterfaceServant = new HelloInterfaceImpl("John");
    
                // activate servant on POA
                byte[] oid = rootPOA.activate_object(helloInterfaceServant);
                           
                org.omg.CORBA.Object object = rootPOA.id_to_reference(oid);
                helloInterfaceJohn = com.thalesgroup.HelloModule.HelloInterfaceHelper.narrow(object);
            }

            
            // ======================================================
            // Get NEW_YORK name domain from repository
            // ======================================================
            com.thalesgroup.CdmwNamingAndRepository.NameDomain helloDomainNewYork =
                repository.resolve_name_domain("TUTORIAL/HELLO/USA/NEW_YORK");
                 
                 
            // ================================================================
            // Register the HelloInterface object in the NEW_YORK name domain
            // ================================================================
            // reserve the name "HelloInterface" in the domain
            org.omg.CORBA.Any regIdJohn =  helloDomainNewYork.new_name ("HelloInterface");
            
            // register the object
            helloDomainNewYork.register_object (regIdJohn, helloInterfaceJohn);

            System.out.println("<------- John is registered in TUTORIAL/HELLO/USA/NEW_YORK ------>");

            
            // ===================================================
            // create the "Mike" HelloInterface servant
            // ===================================================
            com.thalesgroup.HelloModule.HelloInterface helloInterfaceMike = null;
        
            {
                HelloInterfaceImpl helloInterfaceServant = new HelloInterfaceImpl("Mike");
    
                // activate servant on POA
                byte[] oid = rootPOA.activate_object(helloInterfaceServant);
                           
                org.omg.CORBA.Object object = rootPOA.id_to_reference(oid);
                helloInterfaceMike = com.thalesgroup.HelloModule.HelloInterfaceHelper.narrow(object);
            }

            
            // ======================================================
            // Get DETROIT name domain from repository
            // ======================================================
            com.thalesgroup.CdmwNamingAndRepository.NameDomain helloDomainDetroit =
                repository.resolve_name_domain("TUTORIAL/HELLO/USA/DETROIT");
                 
                 
            // ================================================================
            // Register the HelloInterface object in the NEW_YORK name domain
            // ================================================================
            // reserve the name "HelloInterface" in the domain
            org.omg.CORBA.Any regIdMike =  helloDomainDetroit.new_name ("HelloInterface");
            
            // register the object
            helloDomainDetroit.register_object (regIdMike, helloInterfaceMike);

            System.out.println("<------- Mike is registered in TUTORIAL/HELLO/USA/DETROIT ------>");

      
            // ========================================================
            // Link the USA name domain to AMERICA name domain
            // ========================================================
            
            // Get the Factory Finder from Default Root Context
            org.omg.CosNaming.NamingContextExt rootCtx = 
                org.omg.CosNaming.NamingContextExtHelper.narrow(defaultRootContext);
                                       
            org.omg.CORBA.Object FFinder = rootCtx.resolve_str (
                com.thalesgroup.CdmwNamingAndRepository.FACTORY_FINDER.value);
            com.thalesgroup.CdmwLifeCycle.FactoryFinder factoryFinder = 
                com.thalesgroup.CdmwLifeCycle.FactoryFinderHelper.narrow(FFinder);
                                       
                                       
            // Get AMERICA name domain from repository
            com.thalesgroup.CdmwNamingAndRepository.NameDomain helloDomainEurope =
                repository.resolve_name_domain ("TUTORIAL/HELLO/AMERICA");
                
            // Get USA name domain from repository
            com.thalesgroup.CdmwNamingAndRepository.NameDomain helloDomainUsa =
                repository.resolve_name_domain ("TUTORIAL/HELLO/USA");
                
            // Get the naming context of USA name domain           
            org.omg.CORBA.Object objUsaCtx = 
                niDefRoot.resolve("TUTORIAL/HELLO/USA");
            org.omg.CosNaming.NamingContext helloContextUsa =
                org.omg.CosNaming.NamingContextHelper.narrow(objUsaCtx);
                       
                
            // Build the name domain description to link
            com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.NameDomainDescription ndDesc =
                new com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.NameDomainDescription();
            ndDesc.the_name_domain = helloDomainUsa;
            ndDesc.the_factory_finder = factoryFinder;
            ndDesc.the_context = helloContextUsa;
            
            
            // reserve the name "USA" in the AMERICA domain
            org.omg.CORBA.Any regIdUsa = 
                helloDomainEurope.new_name ("USA");
                                        
            // link the USA domain under AMERICA domain
            helloDomainEurope.register_name_domain (regIdUsa, ndDesc);


 

            
            System.out.println("Hello Server Started");
             
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

