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
            
            
            // ===================================================
            // create the "John" HelloInterface servant
            // ===================================================
            com.thalesgroup.HelloModule.HelloInterface helloInterfaceJohn = null;
        
            {
                HelloInterfaceImpl helloInterfaceServant = new HelloInterfaceImpl("John");
    
                // activate servant on POA
                byte[] oid = rootPOA.activate_object(helloInterfaceServant);
                           
                org.omg.CORBA.Object object = rootPOA.id_to_reference(oid);
                helloInterfaceJohn = helloInterfaceServant._this(orb);
            }

            
            // ======================================================
            // Get TUTORIAL/HELLO/US name domain from repository
            // ======================================================
            com.thalesgroup.CdmwNamingAndRepository.NameDomain helloDomain =
                repository.resolve_name_domain("TUTORIAL/HELLO/US");
                 
                 
            // ========================================================
            // Register the HelloInterface object in the US name domain
            // ========================================================
            // reserve the name "HelloInterface" in the domain
            org.omg.CORBA.Any regIdJohn =  helloDomain.new_name ("HelloInterface");
            
            // register the object
            helloDomain.register_object (regIdJohn, helloInterfaceJohn);


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

            
            // =========================================================
            // Get TUTORIAL/HELLO/US/DENVER name domain from repository
            // =========================================================
            com.thalesgroup.CdmwNamingAndRepository.NameDomain helloDomainDenver =
                repository.resolve_name_domain("TUTORIAL/HELLO/US/DENVER");
                 
                 
            // =============================================================
            // Register the HelloInterface object in the DENVER name domain
            // =============================================================
            // reserve the name "HelloInterface" in the domain
            org.omg.CORBA.Any regIdMike =  helloDomainDenver.new_name ("HelloInterface");
            
            // register the object
            helloDomainDenver.register_object (regIdMike, helloInterfaceMike);


            // ===================================================
            // create the "Bob" HelloInterface servant
            // ===================================================
            com.thalesgroup.HelloModule.HelloInterface helloInterfaceBob = null;
        
            {
                HelloInterfaceImpl helloInterfaceServant = new HelloInterfaceImpl("Bob");
    
                // activate servant on POA
                byte[] oid = rootPOA.activate_object(helloInterfaceServant);
                           
                org.omg.CORBA.Object object = rootPOA.id_to_reference(oid);
                helloInterfaceBob = helloInterfaceServant._this(orb);
            }

            
            // ===================================================
            // create the "Peter" HelloInterface servant
            // ===================================================
            com.thalesgroup.HelloModule.HelloInterface helloInterfacePeter = null;
        
            {
                HelloInterfaceImpl helloInterfaceServant = new HelloInterfaceImpl("Peter");
    
                // activate servant on POA
                byte[] oid = rootPOA.activate_object(helloInterfaceServant);
                           
                org.omg.CORBA.Object object = rootPOA.id_to_reference(oid);
                helloInterfacePeter = helloInterfaceServant._this(orb);
            }

            
            // ===================================================
            // create the "Jack" HelloInterface servant
            // ===================================================
            com.thalesgroup.HelloModule.HelloInterface helloInterfaceJack = null;
        
            {
                HelloInterfaceImpl helloInterfaceServant = new HelloInterfaceImpl("Jack");
    
                // activate servant on POA
                byte[] oid = rootPOA.activate_object(helloInterfaceServant);
                           
                org.omg.CORBA.Object object = rootPOA.id_to_reference(oid);
                helloInterfaceJack = helloInterfaceServant._this(orb);
            }

            
            // =========================================================
            // Get TUTORIAL/HELLO/US/DALLAS name domain from repository
            // =========================================================
            com.thalesgroup.CdmwNamingAndRepository.NameDomain helloDomainDallas =
                repository.resolve_name_domain("TUTORIAL/HELLO/US/DALLAS");
                 
                 
            // =============================================================
            // Register the HelloInterface object in the DALLAS name domain
            // =============================================================
            // reserve the name "HelloInterfaceBob" in the domain
            org.omg.CORBA.Any regIdBob =  helloDomainDallas.new_name ("HelloInterfaceBob");
            
            // register the Bob object
            helloDomainDallas.register_object (regIdBob, helloInterfaceBob);

            // reserve the name "HelloInterfacePeter" in the domain
            org.omg.CORBA.Any regIdPeter =  helloDomainDallas.new_name ("HelloInterfacePeter");
            
            // register the Peter object
            helloDomainDallas.register_object (regIdPeter, helloInterfacePeter);

            // register the Jack object
            helloDomainDallas.register_new_object ("HelloInterfaceJack", helloInterfaceJack);

            
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

