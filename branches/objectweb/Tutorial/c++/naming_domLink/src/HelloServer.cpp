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

#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include <Foundation/common/System.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/common/Locations.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Repository/naminginterface/NamingInterface.hpp>
#include <Repository/naminginterface/NamingUtil.hpp>

#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>

#include "HelloInterface_impl.hpp"


namespace 
{
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5;
    
    const std::string NAMING_REPOS_URL_OPTION = "--namingRepos-url";
    
    static void
    usage(std::ostream & os, const std::string &program_name)
    {
        os << "Usage:\n"
           << program_name << " options\n"
           << "\n"
           << "Options:\n"
           << "--help                     Display this information.\n"
		   << "\n"
		   << "--namingRepos-url=<URL>  URL of the CARDAMOM Naming and Repository service.\n"	   
           << std:: endl;
    }
 

}; // End anonymous namespace



int main(int argc, char* argv[])
{    
    int status = SUCCESS;
    
    std::cout << "Start Hello Server" << std::endl;
    

    //
    // help argument
    //
    if(argc > 1 && strcmp(argv[1], "--help") == 0)
	{
      usage(std::cout, argv[0]);
      return SUCCESS;
    }
 
    
    //
    // get Naming & Repository URL from arguments
    //
    std::string namingReposURL = 
         Cdmw::OsSupport::OS::get_option_value (argc, argv, NAMING_REPOS_URL_OPTION);
         
    if (namingReposURL == "no") 
    {
	    std::cout << "naming and repository url undefined" << std::endl;
	    usage(std::cout, argv[0]);
 	    return FAILURE;
    }
    
    
    
    CORBA::ORB_var orb;                        // orb reference
   
    try 
    {
    	// ===================================================
        // Initialize the ORB
        // ===================================================
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);
        
        // ===================================================
        // Get the root POA 
        // ===================================================
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

        // ===================================================
        // Activate the root POA manager
        // ===================================================
        PortableServer::POAManager_var poaManager = rootPOA->the_POAManager();
        poaManager->activate();
        
        
        // ===================================================
        // Get the naming and repository
        //     from the input URL
        // ===================================================
        CdmwNamingAndRepository::Repository_var repository =
                                  CdmwNamingAndRepository::Repository::_nil();  
                                  
        CORBA::Object_var obj2 = orb->string_to_object(namingReposURL.c_str());
        repository = CdmwNamingAndRepository::Repository::_narrow(obj2.in());
        
        
        // ======================================================  
        // get the default root context
        // ======================================================
        CosNaming::NamingContext_var default_root_context =
             repository->resolve_root_context(
                             CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
                             
        // ======================================================  
        // create the naming interface for default root context
        // ======================================================                    
        Cdmw::NamingAndRepository::NamingInterface ni_defRoot(default_root_context.in());
        
        
        // ===================================================
        // create the "John" HelloInterface servant
        // ===================================================
        HelloModule::HelloInterface_var helloInterfaceJohn;
        
        {
            Cdmw::Tutorial::HelloInterface_impl* pHelloInterfaceServant = 
                            new Cdmw::Tutorial::HelloInterface_impl("John");
    
		    // create an object var to take pointer ownership
		    // (ref count will be decremented when var will be destroyed at the block end)
		    PortableServer::ServantBase_var servant_var = pHelloInterfaceServant;
		
		    // activate servant on POA (ref count is incremented)
		    PortableServer::ObjectId_var oid = 
		                   rootPOA->activate_object(pHelloInterfaceServant);
		                   
		    CORBA::Object_var object = rootPOA->id_to_reference(oid.in());
		    helloInterfaceJohn = HelloModule::HelloInterface::_narrow(object.in());
	    }
	    
	    
	    // ======================================================
        // Get NEW_YORK name domain from repository
        // ======================================================
        CdmwNamingAndRepository::NameDomain_var helloDomainNew_York =
            repository->resolve_name_domain ("TUTORIAL/HELLO/USA/NEW_YORK");
             
             
        // ========================================================
        // Register the HelloInterface object in the NEW_YORK name domain
        // ========================================================
        // reserve the name "HelloInterface" in the domain
        CdmwNamingAndRepository::NameDomain::RegistrationId_var regIdJohn = 
                                    helloDomainNew_York->new_name ("HelloInterface");
        // register the object
        helloDomainNew_York->register_object (regIdJohn, helloInterfaceJohn.in());
        
        std::cout << "<------- John is registered in TUTORIAL/HELLO/USA/NEW_YORK ------>" 
                  << std::endl;
        
        
             
        // ===================================================
        // create the "Mike" HelloInterface servant
        // ===================================================
        HelloModule::HelloInterface_var helloInterfaceMike;
        
        {
            Cdmw::Tutorial::HelloInterface_impl* pHelloInterfaceServant = 
                            new Cdmw::Tutorial::HelloInterface_impl("Mike");
    
		    // create an object var to take pointer ownership
		    // (ref count will be decremented when var will be destroyed at the block end)
		    PortableServer::ServantBase_var servant_var = pHelloInterfaceServant;
		
		    // activate servant on POA (ref count is incremented)
		    PortableServer::ObjectId_var oid = 
		                   rootPOA->activate_object(pHelloInterfaceServant);
		                   
		    CORBA::Object_var object = rootPOA->id_to_reference(oid.in());
		    helloInterfaceMike = HelloModule::HelloInterface::_narrow(object.in());
	    }
	    
	    
	    // ======================================================
        // Get DETROIT name domain from repository
        // ======================================================
        CdmwNamingAndRepository::NameDomain_var helloDomainDetroit =
            repository->resolve_name_domain ("TUTORIAL/HELLO/USA/DETROIT");
             
             
        // ==============================================================
        // Register the HelloInterface object in the DETROIT name domain
        // ==============================================================
        // reserve the name "HelloInterface" in the domain
        CdmwNamingAndRepository::NameDomain::RegistrationId_var regIdMike = 
                                    helloDomainDetroit->new_name ("HelloInterface");
        // register the object
        helloDomainDetroit->register_object (regIdMike, helloInterfaceMike.in());
        
        std::cout << "<------- Mike is registered in TUTORIAL/HELLO/USA/DETROIT ------>" 
                  << std::endl;
        
        
        
        // ========================================================
        // Link the USA name domain to AMERICA name domain
        // ========================================================
        
        // Get the Factory Finder from Default Root Context
        CosNaming::NamingContextExt_var rootCtx = 
                      CosNaming::NamingContextExt::_narrow(default_root_context.in());
                                   
        CORBA::Object_var ffinder_obj = rootCtx->resolve_str (
                                   CdmwNamingAndRepository::FACTORY_FINDER);
        CdmwLifeCycle::FactoryFinder_var factoryFinder = 
                                   CdmwLifeCycle::FactoryFinder::_narrow(ffinder_obj.in());
                                   
                                   
        // Get AMERICA name domain from repository
        CdmwNamingAndRepository::NameDomain_var helloDomainEurope =
            repository->resolve_name_domain ("TUTORIAL/HELLO/AMERICA");
            
        // Get USA name domain from repository
        CdmwNamingAndRepository::NameDomain_var helloDomainUsa =
            repository->resolve_name_domain ("TUTORIAL/HELLO/USA");
            
        // Get the naming context of USA name domain           
        typedef Cdmw::NamingAndRepository::NamingUtil<CosNaming::NamingContext> Util;
        CosNaming::NamingContext_var helloContextUsa =
                   Util::resolve_name(ni_defRoot,"TUTORIAL/HELLO/USA");
                   
            
        // Build the name domain description to link
        CdmwNamingAndRepository::NameDomain::NameDomainDescription ndDesc;
        ndDesc.the_name_domain = helloDomainUsa.in();
        ndDesc.the_factory_finder = factoryFinder.in();
        ndDesc.the_context = helloContextUsa.in();
        
        
        // reserve the name "USA" in the AMERICA domain
        CdmwNamingAndRepository::NameDomain::RegistrationId_var regIdUsa = 
                                    helloDomainEurope->new_name ("USA");
                                    
        // link the USA domain under AMERICA domain
        helloDomainEurope->register_name_domain (regIdUsa, ndDesc);
        
             
        
       

        std::cout << "Hello Server Started" << std::endl;
        
        
        // ===================================================
        // wait for CORBA message
        // ===================================================
        orb->run();

    } 
    catch (const Cdmw::Exception & ex)
    {
        std::cerr << ex.what() << std::endl;
        status = FAILURE;
    } 
    catch(const CORBA::Exception& ex)  
    {        
        std::cerr << ex << std::endl;
        status = FAILURE;
    }
    catch(...)  
    {        
        std::cerr << "unexpected exception" << std::endl;
        status = FAILURE;
    }


    // ========================================================
    // program stopping
    // ========================================================
    
    // ===================================================
    // Call ORB cleanup
    // ===================================================
    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
       
    // =====================================================
    // Destroy orb
    // =====================================================
    if (!CORBA::is_nil(orb.in()))
    {
        try 
        {
            orb -> destroy();
        }
        catch(const CORBA::Exception& ex)
        {           
            std::cerr << ex << std::endl;
            status = FAILURE;
        }
    }        

    return status;    
}

