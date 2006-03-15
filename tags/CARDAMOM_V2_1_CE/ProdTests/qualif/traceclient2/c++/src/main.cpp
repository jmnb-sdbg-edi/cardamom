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


#include "Foundation/common/Options.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Locations.hpp"

#include "Foundation/orbsupport/CORBA.hpp"
#include <Foundation/logging/LogManager.hpp> // ECR-0169

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platforminterface/ServiceNames.hpp"

#include "Repository/repositoryinterface/RepositoryInterface.hpp"

#include "TraceAndPerf/tracelibrary/InitUtils.hpp"
#include "TraceAndPerf/tracelibrary/Trace.hpp"

#include "TraceAndPerf/performancelibrary/InitUtils.hpp"
#include "TraceAndPerf/performancelibrary/PerformanceMngr.hpp"

#include "traceclient2/MyProcessBehaviour.hpp"

#include <cstdlib>
#include <string>
#include <exception>
#include <sstream>
#include <iostream>
#include <fstream>




// unnamed namespace used to define data local to the file

namespace
{
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::OrbSupport;
    using namespace Cdmw::PlatformMngt;

    
    const std::string MULTI_DOM_LEVEL_OPTION = "--multi-domain-level";
    const std::string TRACE_MSG_OPTION = "--trace-msg";
    const std::string TRACE_NBR_OPTION = "--trace-nbr";
    const std::string COLLECTOR_NAME_OPTION = "--collector-name";
    const std::string PERF_CYCLE_OPTION = "--perf-cycle-nbr";
    const std::string PERF_ACTIVATION_OPTION = "--perf-activate";
    const std::string PERF_LOG_FILE_OPTION = "--perf-log-file";
    
    const int SUCCESS = 0;
    const int FAILURE = 1;
    
    std::string application_name = "";    
    std::string process_name = "";
    
    const int POA_THREAD_POOL_SIZE = 5;
    
}


void exceptionHandler(void)
{
	std::cout << "Unexpected Exception : Test failed" << std::endl;
}


void exitHandler(void)
{
	std::cout << "Process '" << process_name << "' exited" << std::endl;
}


static void
usage(std::ostream & os, const std::string &program_name)
{
    os << "Usage:\n"
       << program_name << " options\n"
       << "\n"
       << "Options:\n"
       << "--help                             Display this information.\n"
       << "[--multi-domain-level]             Multi domain and level activation.\n"
       << "[--trace-msg=<message>]            Message to trace.\n"
       << "[--trace-nbr=<number>]             Number of messages to trace.\n"
       << "[--collector-name=<name>]          Name of trace collector.\n"
       << "[--perf-cycle-nbr=<number>]        Number of cycles to call the performance probe.\n"
       << "[--perf-activate]                  Performance measure activation.\n"
       << "[--perf-log-file=<file>]           Performance log file. \n"
       << std:: endl;
}
    
    
    
int main(int argc, char* argv[])
{    
    std::string trace_message = "";
    
    long trace_count = 0;       // -1 if infinite
    long perf_count = 0;        // -1 if infinite
    
    std::string default_stat_file = "perf_stat.txt";
    
    bool multi_domain_level = false;
    bool perf_activation = false;
 
    // the return code 
    int ret_code = 0;

    std::set_unexpected(&exceptionHandler);

    ::atexit(&exitHandler);

    StrategyList strategy;
    strategy.add_OrbThreaded();
    //strategy.add_PoaThreadPerRequest();
    strategy.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

    CORBA::ORB_var orb;


    //
    // help argument
    //
    if (argc > 1)
    {
        if(strcmp(argv[1], "--help") == 0)
	    {
            usage(std::cout, argv[0]);
            return ret_code;
        }
    }
    
    
    try
    {
        //
        // get multi domain-level option
        //
        
        std::string multi_domain_level_option = 
               OS::get_option_value (argc, argv, MULTI_DOM_LEVEL_OPTION);
        if (multi_domain_level_option == "yes")
        {
            multi_domain_level = true;
        }
        
        //
        // get Trace Message
        //
        
        std::string specifiedTraceMsg = OS::get_option_value (argc, argv, TRACE_MSG_OPTION);
        if (specifiedTraceMsg == "yes")
        {
            std::cout << "bad value for " << TRACE_MSG_OPTION.c_str() << "=<message> option\n";
            std::cout << std::endl;
            return FAILURE;
        }
        else if (specifiedTraceMsg != "no")
        {
	        trace_message = specifiedTraceMsg;
        }
        
        //
        // get number of Trace Messages
        //
        
        std::string trace_count_option = OS::get_option_value (argc, argv, TRACE_NBR_OPTION);
        if (trace_count_option == "yes")
        {
            std::cout << "bad value for " << TRACE_NBR_OPTION.c_str() << "=<number> option\n";
            std::cout << std::endl;
            return FAILURE;
        }
        else if (trace_count_option != "no")
        {
	        std::istringstream trace_count_strm(trace_count_option);
            trace_count_strm >> trace_count;
            if (trace_count_strm.fail()) 
            {
                std::cout << "bad value for " << TRACE_NBR_OPTION.c_str() << "=<number> option\n";
                std::cout << std::endl;
                return FAILURE;
            }
        }
        
        //
        // get Trace Collector Name
        //
        
        std::string collector_name = 
             Cdmw::OsSupport::OS::get_option_value (argc, argv, COLLECTOR_NAME_OPTION);
         
        if (collector_name == "yes")
        {
            std::cout << "bad value for " << COLLECTOR_NAME_OPTION.c_str() << "=<name> option\n";
            std::cout << std::endl;
            return FAILURE;
        }
        else if (collector_name == "no")
        {
            collector_name = "";
            std::cout << "no trace collector defined" << std::endl;
	        std::cout << "  (use the option " << COLLECTOR_NAME_OPTION.c_str()
	                  << "=<name> if required"
                      << std::endl;
        }
  
  
        
        //
        // get number of Performance cycle
        //
                
        std::string perf_count_option = OS::get_option_value (argc, argv, PERF_CYCLE_OPTION);
        if (perf_count_option == "yes")
        {
            std::cout << "bad value for " << PERF_CYCLE_OPTION.c_str() << "=<number> option\n";
            std::cout << std::endl;
            return FAILURE;
        }
        else if (perf_count_option != "no")
        {
	        std::istringstream perf_count_strm(perf_count_option);
            perf_count_strm >> perf_count;
            if (perf_count_strm.fail()) 
            {
                std::cout << "bad value for " << PERF_CYCLE_OPTION.c_str() << "=<number> option\n";
                std::cout << std::endl;
                return FAILURE;
            }
        }
        
        //
        // get performance activation option
        //
        
        std::string perf_activation_option = 
               OS::get_option_value (argc, argv, PERF_ACTIVATION_OPTION);
        if (perf_activation_option == "yes")
        {
            perf_activation = true;
        }
        
        //
	    // get Performance Log File Name
	    //
	    std::string perf_stat_file = OS::get_option_value (argc, argv, PERF_LOG_FILE_OPTION);
	    if (perf_stat_file == "no")
	    {
	        perf_stat_file = default_stat_file;
	    }
        
        
        // initialise the orb    
        orb =  Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategy); 

        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");

        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();

        poaMgr->activate();


        try
        {
            std::cout << "Init PlatformInterface" << std::endl;
                      
            // initialise the platform interface
            PlatformInterface::Setup(orb.in(), argc, argv);
      
            // creates the process behaviour
            std::auto_ptr<MyProcessBehaviour> procBehaviour(
                                new MyProcessBehaviour(orb.in(), multi_domain_level, 
                                                       trace_message, trace_count, perf_count));
                                                                      
            // get processId
            OS::ProcessId processId = OS::get_processId();

            // acknowledge the creation of the process
            PlatformInterface::Acknowledge_creation(procBehaviour.get());
            MyProcessBehaviour * pProcBehaviour = procBehaviour.release();
                    
            // sample usage of the platform interface                    
            application_name = PlatformInterface::Get_application_name();

            process_name = PlatformInterface::Get_process_name();


            std::cout << "Application name : "
                      << application_name.c_str() << std::endl;

            std::cout << "Process name : " 
                      << process_name.c_str() 
                      << " Process id : "
                      << processId << std::endl;
                      
            // ==================================================================  
            // Retreive Repository object reference from the platform management
            // ================================================================== 
            
            std::cout << "Init Repository" << std::endl;
            
            CdmwNamingAndRepository::Repository_var rep =
                                 CdmwNamingAndRepository::Repository::_nil();
    
    
	        CORBA::Object_var obj =
	               PlatformInterface::Get_service(ServiceNames::NAMING_AND_REPOSITORY_SERVICE);
	        rep = CdmwNamingAndRepository::Repository::_narrow(obj.in());
	        
	        // ======================================================   
            // Initialise the repository interface
            //     "CARDAMOM" has been chosen as the default domain name
            //     Note : RepositoryInterface is necessary for 
            //            trace library InitUtils::init_trace_library
            // ======================================================
            Cdmw::NamingAndRepository::RepositoryInterface::init ("CDMW", rep.in());
                                                                  
                                                                  
            // ===================================================
            // Initialise the trace service
            // ===================================================
            
            std::cout << "Init Trace library" << std::endl;
            
            {
                // create the collector name list with the input
                // collector name 
                std::vector<std::string> collector_name_list;
            
                if (collector_name != "")
                {
                    collector_name_list.push_back(collector_name);
                }
            
                // Init the trace library
                //     5000    is the time to wait before flushing the current flush area
                //             containing the messages to trace although it is not full.      
                //     100     message threshold
                //     2       is the number of flush area to be used to store messages
                //             to trace
                //     50*1024 size of each flush area to be used to store messages to trace
                //             must be a multiple of 1024
                //
                // May raise CORBA::SystemException
 		Cdmw::Logging::LogManager::Init(argc, argv);
            
                Cdmw::Trace::InitUtils::init_trace_library(rootPOA.in(),
                                                           application_name,
                                                           process_name,
                                                           5000,
                                                           100,
                                                           2,
                                                           50 * 1024,
                                                           collector_name_list);
            
            
                // active the trace flushing to trace collector
                // (start to send messages to trace collector)
                CDMW_TRACE_ACTIVE_FLUSHING();
            }
            
            
            // ===================================================
            // Initialise the performance service
            // ===================================================
            
            std::cout << "Init Performance library" << std::endl;
            
            {
                // init of PerformanceMngr
                // Register PerformanceMngr object in repository
                // Pattern is :
                //    hostname/appli_name/process_name/PERFORMANCE/PerformanceAdmin
                //
                // May raise CORBA::SystemException
                
                Cdmw::Performance::InitUtils::init_performance_library (
                                                           rootPOA.in(),
                                                           application_name,
                                                           process_name,
                                                           perf_stat_file);
                                                           
                                                           
                // activate or deactivate the performance measure according parameter
	            if (perf_activation)
	            {
                    Cdmw::Performance::PerformanceMngr::activate();
	            }
	            else
	            {
                    Cdmw::Performance::PerformanceMngr::deactivate();
	            }
            }
            
             
            std::string base_msg = "process <" + application_name + "/" + process_name + ">" ;
        
            // ===================================================
            // trace a message (don't use CDMW_TRACE in main)
            // ===================================================
            
            std::cout << "program started and waiting CORBA messages" << std::endl;
            
            
            std::string msg = base_msg + "started and waiting CORBA messages"; 
            
            Cdmw::Trace::Stream stream; 
                                       
            CDMW_TRACE_ST(stream,"TEST", 1, msg.c_str());
                    
        
            // advise behavior that init is done
            pProcBehaviour->set_init_done();
            pProcBehaviour->get_condition().broadcast();
        
            // start orb
            orb->run();
        }
            
        catch(...)
        {
            // cleanup the ORB to exit completely
            orb->shutdown(false);
		    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
		    orb->destroy();
            throw;
        }


        //orb->shutdown(false) is done by the ProcessBehaviour
        
        
        // cleanup the performance library (statistic are written in file)
        Cdmw::Performance::PerformanceMngr::cleanup();
        
        // cleanup the trace library
        Cdmw::Trace::FlushAreaMngr::cleanup();
        
        // cleanup the repository interface
        Cdmw::NamingAndRepository::RepositoryInterface::finish();

        // cleanup the platform interface
        PlatformInterface::Cleanup();
                
        
	    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
	    orb->destroy();
        
    }
    catch (const CORBA::Exception& e)
    {  
    	std::cerr << "FAILURE : CORBA exception raised :" << std::endl
            << e._name() << std::endl;
        ret_code = FAILURE;
    }
    catch(const std::exception &e)
    {
        std::cerr << "FAILURE : Exception raised : " << std::endl
            << e.what() << std::endl;
        ret_code = FAILURE;
    }
    catch(...)
    {
        std::cerr << "FAILURE : Unexpected exception" << std::endl;
        ret_code = FAILURE;
    }

    return ret_code;

}


