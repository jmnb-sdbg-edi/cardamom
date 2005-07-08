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
#include <Repository/naminginterface/NamingUtil.hpp>

#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>

#include "Hello.stub.hpp"




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
        // Get the John HelloInterface object from repository
        // ===================================================
	    CORBA::Object_var objref = ni_defRoot.resolve ("TUTORIAL/HELLO/US/HelloInterface");

	    // get the John HelloInterface proxy
	    HelloModule::HelloInterface_var helloInterfaceJohn = 
			     HelloModule::HelloInterface::_narrow(objref.in());

	    if (CORBA::is_nil(helloInterfaceJohn.in()))
	    {
	        std::cout << "File : " << __FILE__ << " Line : " << __LINE__
	                  << "could not _narrow object to type HelloInterface" << std::endl;
	        return FAILURE;
	    }


        // ===================================================
        // Call displayHello method of HelloInterfaceJohn object
        // ===================================================
	    helloInterfaceJohn->display_hello();



        typedef Cdmw::NamingAndRepository::NamingUtil<CosNaming::NamingContext> Util;
        
        // ===================================================
        // Get the US naming context from repository
        // ===================================================
        
        CosNaming::NamingContext_var namingContextUs = 
                         Util::resolve_name(ni_defRoot,"TUTORIAL/HELLO/US");
                         
        // ======================================================  
        // create the naming interface for US context
        // ======================================================                    
        Cdmw::NamingAndRepository::NamingInterface ni_Us(namingContextUs.in());

        
                         
        // ===================================================
        // Get the Denver naming context from repository
        // ===================================================
        CosNaming::NamingContext_var namingContextDenver = 
                         Util::resolve_name(ni_Us,"DENVER");
                         
        // ======================================================  
        // create the naming interface for DENVER context
        // ======================================================                    
        Cdmw::NamingAndRepository::NamingInterface ni_Denver(namingContextDenver.in());
        
        
        // ===================================================
        // Get the Mike HelloInterface object from repository
        // ===================================================
	    objref = ni_Denver.resolve ("HelloInterface");

	    // get the Mike HelloInterface proxy
	    HelloModule::HelloInterface_var helloInterfaceMike = 
			     HelloModule::HelloInterface::_narrow(objref.in());

	    if (CORBA::is_nil(helloInterfaceMike.in()))
	    {
	        std::cout << "File : " << __FILE__ << " Line : " << __LINE__
	                  << "could not _narrow object to type HelloInterface" << std::endl;
	        return FAILURE;
	    }


        // ===================================================
        // Call displayHello method of HelloInterfaceMike object
        // ===================================================
	    helloInterfaceMike->display_hello();
                         
         
        // ===================================================
        // Get the Dallas naming context from repository
        // ===================================================
        CosNaming::NamingContext_var namingContextDallas = 
                         Util::resolve_name(ni_Us,"DALLAS");
                         
        // ======================================================  
        // create the naming interface for DALLAS context
        // ======================================================                    
        Cdmw::NamingAndRepository::NamingInterface ni_Dallas(namingContextDallas.in()); 
        
        
        // ===========================================================  
        // Get the HelloInterface objects under Dallas naming context
        // 1st method using list operation
        // ===========================================================
        CosNaming::BindingIterator_var it;      // Iterator reference
        CosNaming::BindingList_var     bl;      // Binding list
        const CORBA::ULong CHUNK = 5;           // Chunk size

        ni_Dallas.list(CHUNK,bl,it);            // Get first chunk
        
        
        // for each name of binding list, get the object reference
        for (CORBA::ULong i = 0; i < bl->length(); ++i) 
        {
            if (bl[i].binding_type == CosNaming::nobject)
            {
                CORBA::Object_var objref = 
                    ni_Dallas.resolve(bl[i].binding_name[0].id.in());
                
                // get the HelloInterface proxy
	            HelloModule::HelloInterface_var helloInterface = 
			             HelloModule::HelloInterface::_narrow(objref.in());

	            if (CORBA::is_nil(helloInterface.in()))
	            {
	                std::cout << "File : " << __FILE__ << " Line : " << __LINE__
	                          << "could not _narrow object to type HelloInterface" 
	                          << std::endl;
	                return FAILURE;
	            }


               // ===================================================
               // Call displayHello method of HelloInterface object
               // ===================================================
	           helloInterface->display_hello();
            
            }
        }
        
        

        if (!CORBA::is_nil(it.in()))                 // More bindings?
        {                                     
            CORBA::Boolean more;
            do 
            {
                more = it->next_n(CHUNK,bl);    // Get next chunk
                
                 
                for (CORBA::ULong i = 0; i < bl->length(); ++i) 
                {
                    std::cout << "get ";
                    
                    if (bl[i].binding_type == CosNaming::ncontext)
                    {
                        std::cout << "naming context : ";            
                    }
                    else if (bl[i].binding_type == CosNaming::nobject)
                    {
                        std::cout << "object : ";            
                    }
                    
                    std::cout << bl[i].binding_name[0].id.in();
                    std::cout << "." << bl[i].binding_name[0].kind.in();
                    std::cout << std::endl;
                }
                
            } while (more);                     // While not done
        
            it->destroy();                      // Clean up
        }  
        
        
        
        // ===========================================================  
        // Get the HelloInterface objects under Dallas naming context
        // 2nd method using list_context operation
        // ===========================================================
        Cdmw::NamingAndRepository::NamingInterface::BindingList bndList;   
                                                // Binding list

        ni_Dallas.list_context(bndList);        // Get first chunk
        
        
        // for each name of binding list, get the object reference
        Cdmw::NamingAndRepository::NamingInterface::BindingList::iterator bndListIt;
        
        for (bndListIt=bndList.begin(); bndListIt != bndList.end();             
             ++bndListIt) 
        {
            if ((*bndListIt).binding_type == CosNaming::nobject)
            {
                CORBA::Object_var objref = 
                          ni_Dallas.resolve((*bndListIt).binding_name[0].id.in());
                
                // get the HelloInterface proxy
	            HelloModule::HelloInterface_var helloInterface = 
			             HelloModule::HelloInterface::_narrow(objref.in());

	            if (CORBA::is_nil(helloInterface.in()))
	            {
	                std::cout << "File : " << __FILE__ << " Line : " << __LINE__
	                          << "could not _narrow object to type HelloInterface" 
	                          << std::endl;
	                return FAILURE;
	            }


               // ===================================================
               // Call displayHello method of HelloInterface object
               // ===================================================
	           helloInterface->display_hello();
            
            }
        }
        
                

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

