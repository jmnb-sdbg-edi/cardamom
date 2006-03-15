/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with CARDAMOM; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include <Foundation/logging/LogManager.hpp> // ECR-0169
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platforminterface/ServiceNames.hpp"
#include "TraceAndPerf/tracelibrary/InitUtils.hpp"
#include "TraceAndPerf/tracelibrary/Trace.hpp"

#include "traceclient2/MyProcessBehaviour.hpp"


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


/**
 *
 */
void
exceptionHandler(void)
{
    std::cout << "Unexpected exception: Test failed" << std::endl;
}


/**
 *
 */
void
exitHandler(void)
{
    std::cout << "Process '" << processName << "' exited" << std::endl;
}


/**
 *
 */
static void
usage(std::ostream & os, const std::string &program_name)
{
    os << "Usage:\n"
       << program_name << " options\n"
       << "\n"
       << "Options:\n"
       << "--help                       Display this information.\n"
       << "[--trace-msg=<message>]      Message to trace.\n"
       << "[--trace-nbr=<number>]       Number of messages to trace.\n"
       << "[--collector-name=<name>]    Name of trace collector.\n"
       << std:: endl;
}

/**
 *
 */
int
main(int argc, char* argv[])
{
    // exit code
    int ret_code = 0;

    // set handlers
    std::set_unexpected(&exceptionHandler);
    ::atexit(&exitHandler);

    StrategyList strategy;
    strategy.add_OrbThreaded();
    strategy.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

    CORBA::ORB_var orb;

    // --help option
    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0) {
            usage(std::cout, argv[0]);
            return ret_code;
        }
    }

    try {
        // get the message to trace
        std::string specifiedTraceMsg =
            OS::get_option_value (argc, argv, TRACE_MSG_OPTION);

        traceMessage = "";
        if (specifiedTraceMsg == "yes") {
            std::cout << "bad value for " << TRACE_MSG_OPTION.c_str()
                      << "=<message> option\n";
            std::cout << std::endl;
            return FAILURE;
        } else if (specifiedTraceMsg != "no") {
            traceMessage = specifiedTraceMsg;
        }

        // get the number of messages to trace
        std::string traceCountOption =
            OS::get_option_value (argc, argv, TRACE_NBR_OPTION);

        traceCount = 15;
        if (traceCountOption == "yes") {
            std::cout << "bad value for " << TRACE_NBR_OPTION.c_str()
                      << "=<number> option\n";
            std::cout << std::endl;
            return FAILURE;
        } else if (traceCountOption != "no") {
            std::istringstream traceCount_strm(traceCountOption);
            traceCount_strm >> traceCount;

            if (traceCount_strm.fail()) {
                std::cout << "bad value for " << TRACE_NBR_OPTION.c_str()
                          << "=<number> option\n";
                std::cout << std::endl;
                return FAILURE;
            }
        }

        // get the name of the TraceCollector
        std::string collectorName =
             OS::get_option_value (argc, argv, COLLECTOR_NAME_OPTION);

        if (collectorName == "yes") {
            std::cout << "bad value for " << COLLECTOR_NAME_OPTION.c_str()
                      << "=<name> option\n";
            std::cout << std::endl;
            return FAILURE;
        } else if (collectorName == "no") {
            collectorName = "";
            std::cout << "no trace collector defined" << std::endl;
            std::cout << "  (use the option " << COLLECTOR_NAME_OPTION.c_str()
                      << "=<name> if necessary)"
                      << std::endl;
        }

        // initialise the ORB
        orb = OrbSupport::ORB_init(argc, argv, strategy);
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA =
            PortableServer::POA::_narrow(obj.in());
        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

        try {
            std::cout << "Init PlatformInterface" << std::endl;

            // initialize the platform interface
            PlatformInterface::Setup(orb.in(), argc, argv);

            // create the process behaviour
            std::auto_ptr<MyProcessBehaviour> procBehaviour(
                new MyProcessBehaviour(orb.in(), traceMessage, traceCount));

            // get processId
            OS::ProcessId processId = OS::get_processId();

            // acknowledge the creation of the process
            PlatformInterface::Acknowledge_creation(procBehaviour.get());
            MyProcessBehaviour* pProcBehaviour = procBehaviour.release();

            // get the name of the application
            applicationName = PlatformInterface::Get_application_name();

            // get the name of the process
            processName = PlatformInterface::Get_process_name();

            // display info messages
            std::cout << "Application name: "
                      << applicationName.c_str() << std::endl;

            std::cout << "Process name: "
                      << processName.c_str()
                      << " Process id: "
                      << processId << std::endl;

            // retreive the reference of the Repository from
            // the platform management
            std::cout << "Init Repository" << std::endl;

            CdmwNamingAndRepository::Repository_var rep =
                CdmwNamingAndRepository::Repository::_nil();
            CORBA::Object_var obj =
                PlatformInterface::Get_service(
                    ServiceNames::NAMING_AND_REPOSITORY_SERVICE);
            rep = CdmwNamingAndRepository::Repository::_narrow(obj.in());

            // initialize the repository interface
            // "CARDAMOM" has been chosen as the default domain name
            // note: the repository interface is required during the
            //       initialization of the trace library
            //       (InitUtils::init_trace_library)
            Cdmw::NamingAndRepository::RepositoryInterface::init("CDMW",
                                                                 rep.in());

            // initialize the trace service
            std::cout << "Init Trace library" << std::endl;
            {
                // build a list of names of collectors
                std::vector<std::string> collectorNameList;

                if (collectorName != "") {
                    collectorNameList.push_back(collectorName);
                }

                // initialize the trace library
                //     5000    time to wait before flushing the current flush
                //             area which holds the messages to trace
                //     100     threshold at which a info message about a
                //             repeating message is output to the collector
                //     2       number of flush area to be used to store messages
                //             to trace
                //     50*1024 size of each flush area
                //             (must be a multiple of 1024)
                // may raise CORBA::SystemException
		Cdmw::Logging::LogManager::Init(argc, argv);
                Cdmw::Trace::InitUtils::init_trace_library(rootPOA.in(),
                                                           applicationName,
                                                           processName,
                                                           5000,
                                                           100,
                                                           2,
                                                           50 * 1024,
                                                           collectorNameList);

                // activate flushing
                CDMW_TRACE_ACTIVE_FLUSHING();
            }

            std::string baseMsg = "process <" + applicationName + "/" +
                                  processName + ">" ;

            std::cout << "program started, waiting for CORBA messages"
                      << std::endl;

            std::string msg = baseMsg + "started, waiting for CORBA messages";
            Cdmw::Trace::Stream stream;
	    /* ECR-0123
            CDMW_TRACE_ST(stream,"TEST", 1, msg.c_str());
	    */
            CDMW_TRACE_ST_CCM(stream, "TEST", "TEST", 1, msg.c_str());

            // advise MyProcessBehaviour that the initialization is complete
            pProcBehaviour->set_init_done();
            pProcBehaviour->get_condition().broadcast();

            // start orb
            orb->run();

            // cleanup the platform interface
            PlatformInterface::Cleanup();

        } catch(...) {
            // cleanup the ORB to exit completely
            orb->shutdown(false);
            Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
            orb->destroy();
            throw;
        }

        //orb->shutdown(false) is called by the ProcessBehaviour
        Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
        orb->destroy();
    } catch (const CORBA::Exception& e) {
        std::cerr << "FAILURE : CORBA exception raised :" << std::endl
            << e._name() << std::endl;
        ret_code = FAILURE;
    } catch(const std::exception &e) {
        std::cerr << "FAILURE : Exception raised : " << std::endl
            << e.what() << std::endl;
        ret_code = FAILURE;
    } catch(...) {
        std::cerr << "FAILURE : Unexpected exception" << std::endl;
        ret_code = FAILURE;
    }

    return ret_code;
}
