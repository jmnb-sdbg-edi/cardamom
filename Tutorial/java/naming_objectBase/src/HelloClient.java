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
            // Get the John HelloInterface object from repository
            // ===================================================
            org.omg.CORBA.Object objref = niDefRoot.resolve ("TUTORIAL/HELLO/US/HelloInterface");
    
            // get the John HelloInterface proxy
            com.thalesgroup.HelloModule.HelloInterface helloInterfaceJohn = 
                com.thalesgroup.HelloModule.HelloInterfaceHelper.narrow(objref);
    
            if (helloInterfaceJohn == null)
            {
                System.out.println ("could not narrow object 'John' to type HelloInterface");
                return;
            }
    
            // ======================================================
            // Call displayHello method of helloInterfaceJohn object
            // ======================================================
            helloInterfaceJohn.display_hello();


            // Note: There is no Java equivalent to the c++ template class 
            //       Cdmw::NamingAndRepository::NamingUtil
            
            
            // ===================================================
            // Get the US naming context from repository
            // ===================================================
            objref = niDefRoot.resolve("TUTORIAL/HELLO/US");
            org.omg.CosNaming.NamingContext namingContextUs = 
                org.omg.CosNaming.NamingContextHelper.narrow(objref);

            // ======================================================  
            // create the naming interface for US context
            // ======================================================                    
            cdmw.namingandrepository.NamingInterface niUs = 
                new cdmw.namingandrepository.NamingInterface(namingContextUs);


            // ===================================================
            // Get the Denver naming context from repository
            // ===================================================
            objref = niUs.resolve("DENVER");
            org.omg.CosNaming.NamingContext namingContextDenver = 
                org.omg.CosNaming.NamingContextHelper.narrow(objref);

            // ======================================================  
            // create the naming interface for DENVER context
            // ======================================================                    
            cdmw.namingandrepository.NamingInterface niDenver = 
                new cdmw.namingandrepository.NamingInterface(namingContextDenver);


            // ===================================================
            // Get the Mike HelloInterface object from repository
            // ===================================================
            objref = niDenver.resolve("HelloInterface");
    
            // get the Mike HelloInterface proxy
            com.thalesgroup.HelloModule.HelloInterface helloInterfaceMike = 
                com.thalesgroup.HelloModule.HelloInterfaceHelper.narrow(objref);
    
            if (helloInterfaceMike == null)
            {
                System.out.println ("could not narrow object 'Mike' to type HelloInterface");
                return;
            }

            // ===================================================
            // Call displayHello method of HelloInterfaceMike object
            // ===================================================
            helloInterfaceMike.display_hello();           
 

            // ===================================================
            // Get the Dallas naming context from repository
            // ===================================================
            objref = niUs.resolve("DALLAS");
            org.omg.CosNaming.NamingContext namingContextDallas = 
                org.omg.CosNaming.NamingContextHelper.narrow(objref);
                                     
            // ======================================================  
            // create the naming interface for DALLAS context
            // ======================================================                    
            cdmw.namingandrepository.NamingInterface niDallas = 
                new cdmw.namingandrepository.NamingInterface(namingContextDallas);
            

            // ===========================================================  
            // Get the HelloInterface objects under Dallas naming context
            // 1st method using list operation
            // ===========================================================
            org.omg.CosNaming.BindingIteratorHolder it =       // Iterator reference
                new org.omg.CosNaming.BindingIteratorHolder();
            org.omg.CosNaming.BindingListHolder     bl =       // Binding list
                new org.omg.CosNaming.BindingListHolder();
            int CHUNK = 5;                            // Chunk size
    
            niDallas.list(CHUNK, bl, it);              // Get first chunk
        
            // for each name of binding list, get the object reference
            for (int i = 0; i < bl.value.length; ++i) 
            {
                if (bl.value[i].binding_type.equals(org.omg.CosNaming.BindingType.nobject))
                {
                    org.omg.CORBA.Object objref2 = 
                        niDallas.resolve(bl.value[i].binding_name[0].id);
                    
                    // get the HelloInterface proxy
                    com.thalesgroup.HelloModule.HelloInterface helloInterface = 
                        com.thalesgroup.HelloModule.HelloInterfaceHelper.narrow(objref2);
    
                    if (helloInterface == null)
                    {
                        System.out.println ("could not narrow object to type HelloInterface");
                        return;
                    }
    
    
                   // ===================================================
                   // Call displayHello method of HelloInterface object
                   // ===================================================
                   helloInterface.display_hello();
                
                }
            }


            if (it.value != null)                 // More bindings?
            {                                     
                boolean more;
                do 
                {
                    more = it.value.next_n(CHUNK, bl);    // Get next chunk
                    
                     
                    for (int i = 0; i < bl.value.length; ++i) 
                    {
                        System.out.print("get ");
                        
                        if (bl.value[i].binding_type.equals(org.omg.CosNaming.BindingType.ncontext))
                        {
                            System.out.print("naming context : ");            
                        }
                        if (bl.value[i].binding_type.equals(org.omg.CosNaming.BindingType.nobject))
                        {
                            System.out.print("object : ");            
                        }
                        
                        System.out.print(bl.value[i].binding_name[0].id);
                        System.out.print("." + bl.value[i].binding_name[0].kind);
                        System.out.println();
                    }
                    
                } while (more);                     // While not done
            
                it.value.destroy();                 // Clean up
            }              
            
            
            // ===========================================================  
            // Get the HelloInterface objects under Dallas naming context
            // 2nd method using listContext operation
            // ===========================================================
            org.omg.CosNaming.BindingListHolder bndList =       // Binding list    
                new org.omg.CosNaming.BindingListHolder(); 
    
            niDallas.listContext(bndList);                      // Get first chunk
            
            
            // for each name of binding list, get the object reference
            for (int i = 0; i < bndList.value.length; ++i)             
            {
                if (bndList.value[i].binding_type.equals(org.omg.CosNaming.BindingType.nobject))
                {
                    org.omg.CORBA.Object objref3 = 
                        niDallas.resolve(bndList.value[i].binding_name[0].id);
                    
                    // get the HelloInterface proxy
                    com.thalesgroup.HelloModule.HelloInterface helloInterface =  
                        com.thalesgroup.HelloModule.HelloInterfaceHelper.narrow(objref3);
    
                    if (helloInterface == null)
                    {
                        System.out.println ("could not narrow object to type HelloInterface");
                        return;
                    }
    
    
                   // ===================================================
                   // Call displayHello method of HelloInterface object
                   // ===================================================
                   helloInterface.display_hello();
                
                }
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
