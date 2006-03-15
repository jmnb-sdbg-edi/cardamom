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

#include <TraceAndPerf/tracelibrary/InitUtils.hpp>
#include <TraceAndPerf/tracelibrary/Trace.hpp>

#include <Repository/repositoryinterface/RepositoryInterface.hpp>

#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>

#include <Foundation/logging/LogManager.hpp>

namespace 
{
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5;
    
    const std::string NAMING_REPOS_URL_OPTION = "--namingRepos-url";
    const std::string COLLECTOR_NAME_OPTION = "--collector-name";
    
    
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
		   << "[--collector-name=<name>]  Name of trace collector.\n"		   
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
	

    //
    // get Trace Collector Name from arguments
    //
    std::string collectorName = 
         Cdmw::OsSupport::OS::get_option_value (argc, argv, COLLECTOR_NAME_OPTION);
         
    if (collectorName == "no")
    {
        collectorName = "";
        std::cout << "no trace collector defined" << std::endl;
	    std::cout << "  (use the option " << COLLECTOR_NAME_OPTION.c_str()
	              << "=<name> if required"
                  << std::endl;
    }
    
    


    CORBA::ORB_var orb;                        // orb reference
    std::string applicationName = "Tutorial";  // name of application
    std::string processName = "HelloServer";   // name of process in the application
    std::string msg;                           // base message buffer (application and process names)
    Cdmw::Trace::Stream stream;               // stream used by trace service
   
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
        //     "CARDAMOM" has been chosen as the default domain name
        //     Note : RepositoryInterface is necessary for 
        //            trace library InitUtils::init_trace_library
        // ======================================================
        Cdmw::NamingAndRepository::RepositoryInterface::init ("CDMW",
                                                               repository.in());
                                                               
                                                               
        // ===================================================
        // Initialise the trace service
        // ===================================================
        {

	   Cdmw::Logging::LogManager::Init(argc, argv);
            // create the collector name list with the input
            // collector name 
            std::vector<std::string> collectorNameList;
            
            if (collectorName != "")
            {
                collectorNameList.push_back(collectorName);
            }
            
            // Init the trace library
            //     5000    is the time to wait before flushing the current flush area
            //             containing the messages to trace although it is not full.      
            //     2       is the number of flush area to be used to store messages
            //             to trace
            //     50*1024 size of each flush area to be used to store messages to trace
            //             must be a multiple of 1024
            //
            // May raise CORBA::SystemException
            
            Cdmw::Trace::InitUtils::init_trace_library(rootPOA.in(),
                                          applicationName,
                                          processName,
                                          100,5000,2,50*1024,
                                          collectorNameList);
            
            
            // active the trace flushing to trace collector
            // (start to send messages to trace collector)
            CDMW_TRACE_ACTIVE_FLUSHING();
        }
        
        
        std::cout << "Hello Server Started" << std::endl;
        
        std::string baseMsg = "process <" + applicationName + "/" + processName + ">" ;
        
        // ===================================================
        // trace a message (don't use CDMW_TRACE in main)
        // ===================================================
        msg = baseMsg + "started and waiting CORBA messages";  
                                       
        CDMW_TRACE_ST(stream,"TUTOR", 1, msg.c_str());
        
        
        // ===================================================
        // wait for CORBA message
        // ===================================================
        orb->run();

    } 
    catch (const Cdmw::OrbSupport::CORBASystemExceptionWrapper & ex)
    {
        std::cerr << ex.what() << std::endl;
        status = FAILURE;
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
      
    // =====================================================
    // Free all allocated memory and stop the trace service
    // =====================================================
    Cdmw::Trace::FlushAreaMngr::cleanup();
    
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

