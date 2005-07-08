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

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platforminterface/ServiceNames.hpp"

#include "Repository/repositoryinterface/RepositoryInterface.hpp"

#include "TraceAndPerf/tracelibrary/InitUtils.hpp"
#include "TraceAndPerf/tracelibrary/Trace.hpp"

#include "traceclient/MyProcessBehaviour.hpp"

#include <cstdlib>
#include <string>
#include <exception>
#include <sstream>
#include <iostream>
#include <fstream>




const int POA_THREAD_POOL_SIZE = 5;

namespace
{
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::OrbSupport;
    using namespace Cdmw::PlatformMngt;

    std::string applicationName;    
    std::string processName;
    
    std::string traceMessage;
    unsigned long traceCount;

    const std::string TRACE_MSG_OPTION = "--trace-msg";
    const std::string TRACE_NBR_OPTION = "--trace-nbr";
    const std::string COLLECTOR_NAME_OPTION = "--collector-name";
    
    const int SUCCESS = 0;
    const int FAILURE = 1;
    
}


void exceptionHandler(void)
{
	std::cout << "Unexpected Exception : Test failed" << std::endl;
}


void exitHandler(void)
{
	std::cout << "Process '" << processName << "' exited" << std::endl;
}


static void
usage(std::ostream & os, const std::string &program_name)
{
    os << "Usage:\n"
       << program_name << " options\n"
       << "\n"
       << "Options:\n"
       << "--help                             Display this information.\n"
       << "[--trace-msg=<message>]            Message to trace.\n"
       << "[--trace-nbr=<number>]             Number of messages to trace.\n"
       << "[--collector-name=<name>]          Name of trace collector.\n"
       << std:: endl;
}
    
    
    
int main(int argc, char* argv[])
{  
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
        // get Trace Message
        //
        traceMessage = "";
        
        std::string specifiedTraceMsg = OS::get_option_value (argc, argv, TRACE_MSG_OPTION);
        if (specifiedTraceMsg == "yes")
        {
            std::cout << "bad value for " << TRACE_MSG_OPTION.c_str() << "=<message> option\n";
            std::cout << std::endl;
            return FAILURE;
        }
        else if (specifiedTraceMsg != "no")
        {
	        traceMessage = specifiedTraceMsg;
        }
        
        //
        // get number of Trace Messages
        //
        traceCount = 15;
        
        std::string traceCountOption = OS::get_option_value (argc, argv, TRACE_NBR_OPTION);
        if (traceCountOption == "yes")
        {
            std::cout << "bad value for " << TRACE_NBR_OPTION.c_str() << "=<number> option\n";
            std::cout << std::endl;
            return FAILURE;
        }
        else if (traceCountOption != "no")
        {
	        std::istringstream traceCount_strm(traceCountOption);
            traceCount_strm >> traceCount;
            if (traceCount_strm.fail()) 
            {
                std::cout << "bad value for " << TRACE_NBR_OPTION.c_str() << "=<number> option\n";
                std::cout << std::endl;
                return FAILURE;
            }
        }
        
        //
        // get Trace Collector Name
        //
        std::string collectorName = 
             Cdmw::OsSupport::OS::get_option_value (argc, argv, COLLECTOR_NAME_OPTION);
         
        if (collectorName == "yes")
        {
            std::cout << "bad value for " << COLLECTOR_NAME_OPTION.c_str() << "=<name> option\n";
            std::cout << std::endl;
            return FAILURE;
        }
        else if (collectorName == "no")
        {
            collectorName = "";
            std::cout << "no trace collector defined" << std::endl;
	        std::cout << "  (use the option " << COLLECTOR_NAME_OPTION.c_str()
	                  << "=<name> if required"
                      << std::endl;
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
            PlatformInterface::setup(orb.in(), argc, argv);
      
            // creates the process behaviour
            std::auto_ptr<MyProcessBehaviour> procBehaviour(
                                new MyProcessBehaviour(orb.in(), traceMessage, traceCount));
                                                                      
            // get processId
            OS::ProcessId processId = OS::get_processId();

            // acknowledge the creation of the process
            PlatformInterface::acknowledgeCreation(procBehaviour.get());
            MyProcessBehaviour * pProcBehaviour = procBehaviour.release();
                    
            // sample usage of the platform interface                    
            applicationName = PlatformInterface::getApplicationName();

            processName = PlatformInterface::getProcessName();


            std::cout << "Application name : "
                      << applicationName.c_str() << std::endl;

            std::cout << "Process name : " 
                      << processName.c_str() 
                      << " Process id : "
                      << processId << std::endl;
                      
            // ==================================================================  
            // Retreive Repository object reference from the platform management
            // ================================================================== 
            
            std::cout << "Init Repository" << std::endl;
            
            CdmwNamingAndRepository::Repository_var rep =
                                 CdmwNamingAndRepository::Repository::_nil();
    
    
	        CORBA::Object_var obj =
	               PlatformInterface::getService(ServiceNames::NAMING_AND_REPOSITORY_SERVICE);
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
                                                           5000,2,50*1024,
                                                           collectorNameList);
            
            
                // active the trace flushing to trace collector
                // (start to send messages to trace collector)
                CDMW_TRACE_ACTIVE_FLUSHING();
            }
             
            std::string baseMsg = "process <" + applicationName + "/" + processName + ">" ;
        
            // ===================================================
            // trace a message (don't use CDMW_TRACE in main)
            // ===================================================
            
            std::cout << "program started and waiting CORBA messages" << std::endl;
            
            
            std::string msg = baseMsg + "started and waiting CORBA messages"; 
            
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


