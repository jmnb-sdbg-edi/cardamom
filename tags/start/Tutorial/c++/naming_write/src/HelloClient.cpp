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

#include <Foundation/common/System.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/common/Locations.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Repository/naminginterface/NamingInterface.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>

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
    
    std::cout << "Start Hello Client" << std::endl;
    
    
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
        // Initialise the repository interface
        //     "TUTORIAL" has been chosen as the default domain name
        // ======================================================
        Cdmw::NamingAndRepository::RepositoryInterface::init ("TUTORIAL",
                                                               repository.in());
         
         
        // ======================================================
        // Get Naming Interface of HELLO name domain
        // ======================================================
        Cdmw::NamingAndRepository::NamingInterface ni_Hello =
            Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface (
                            "TUTORIAL/HELLO");
	    
    	
        // ===================================================
        // Get the John HelloInterface object from repository
        // ===================================================
	    CORBA::Object_var objref = ni_Hello.resolve ("US/HelloInterface");

	    // get the John HelloInterface proxy
	    HelloModule::HelloInterface_var helloInterfaceJohn = 
			     HelloModule::HelloInterface::_narrow(objref.in());

	    if (CORBA::is_nil(helloInterfaceJohn.in()))
	    {
	        std::cout << "File : " << __FILE__ << " Line : " << __LINE__
	                  << "could not _narrow object to type HelloInterface" << std::endl;
	        return FAILURE;
	    }

        std::cout << "<------- Get John from  TUTORIAL/HELLO/US ------>" 
                  << std::endl;
        
        // ===================================================
        // Call displayHello method of HelloInterfaceJohn object
        // ===================================================
	    helloInterfaceJohn->display_hello();


            
            
            

        // ===================================================
        // try to create the "Mike" HelloInterface servant
        // and register it in the repository using cos naming
        // ===================================================
        
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
		                   
		    helloInterfaceMike = pHelloInterfaceServant->_this();
	    }
	    
	    
        // ======================================================
        // Get Naming Interface of US name domain
        // ======================================================
        Cdmw::NamingAndRepository::NamingInterface ni_Us =
            Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface (
                            "TUTORIAL/HELLO/US");
             
             
        // ==========================================================
        // Try to bind the HelloInterface object in the US naming context
        // ==========================================================        
        try
        {
            ni_Us.bind ("HelloInterfaceMike",helloInterfaceMike.in(),true);
        }
        catch(const CORBA::NO_PERMISSION &e)
        {
            std::cout << ">>>> Creation of HelloInterface for Mike is not permitted \n"
                      << "     in the default root context" << std::endl;
        }
        
        
        // ======================================================  
        // get the admin root context
        // ======================================================
        CosNaming::NamingContext_var admin_root_context =
             repository->resolve_root_context(
                             Cdmw::Common::Locations::ADMIN_ROOT_CONTEXT_ID);
        
        // ======================================================                      
        // create a naming interface on admin root context
        // ====================================================== 
        Cdmw::NamingAndRepository::NamingInterface ni_adminRoot (
                                                         admin_root_context.in());
        
        // ==========================================================
        // Bind the HelloInterface object in the US naming context
        // ==========================================================
        ni_adminRoot.bind ("HelloInterfaceMike",helloInterfaceMike.in(),true);
        
        std::cout << ">>>> Creation of HelloInterface for Mike is successfull \n"
                  << "     in the admin root context" << std::endl;
		 
        
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
    
    // ================================================================
    // Release all static object references held by RepositoryInterface
    // ================================================================
    Cdmw::NamingAndRepository::RepositoryInterface::finish();
    
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
