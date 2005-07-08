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
		                   
		    helloInterfaceJohn = pHelloInterfaceServant->_this();
	    }
	    
	    
	    // =========================================================
        // Get TUTORIAL/HELLO/US name domain from repository
        // =========================================================
        CdmwNamingAndRepository::NameDomain_var helloDomainUs =
            repository->resolve_name_domain ("TUTORIAL/HELLO/US");
             
             
        // ==========================================================
        // Register the HelloInterface object in the US name domain
        // ==========================================================
        // reserve the name "HelloInterface" in the domain
        CdmwNamingAndRepository::NameDomain::RegistrationId_var regIdJohn = 
                                    helloDomainUs->new_name ("HelloInterface");
                                    
        // register the object
        helloDomainUs->register_object (regIdJohn, helloInterfaceJohn.in());
        
        
        

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

