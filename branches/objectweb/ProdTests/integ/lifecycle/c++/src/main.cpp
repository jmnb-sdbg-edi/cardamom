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
#include "Foundation/common/System.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/testutils/Testable.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/common/Options.hpp"

#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessCallback.stub.hpp"

#include "lifecycle/MacroDefs.hpp"
#include "lifecycle/TestData.hpp"
#include "lifecycle/CommandThread.hpp"
#include "ConfAndPlug/cdmwinit/CdmwInterface.hpp"
#include "ConfAndPlug/cdmwinit/ProcessControl.hpp"
#include "lifecycle/MyProcessControl.hpp"

#include "SystemMngt/tools/ProcessAdmin.hpp"


#include <string>
#include <sstream>

namespace {
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5;

    // The naming context of the current Process object within the naming
    const char* PROC_CTXT = "CDMW.I/ProcessCallback.simulated/CDMW Test Application/PROC_000";
    const unsigned int INIT_WAIT_DURATION = 3000; // 3 seconds before calling initialise
    const unsigned int PROCESSING_DURATION = 10000; // 10 seconds processing time
                                                    // calling stop
    

CdmwPlatformMngt::ProcessCallback_ptr
get_process_callback(CORBA::ORB_ptr orb)
    throw(CORBA::TRANSIENT)
{
    CDMW_ASSERT(!CORBA::is_nil(orb));
    
    CORBA::Object_var obj = CORBA::Object::_nil();
    //
    // Get naming service
    //
    try
    {
        obj = orb -> resolve_initial_references("NameService");
    }
    catch(const CORBA::ORB::InvalidName&)
    {
        NI_TEST_CERR("Can't resolve `NameService'");
        throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                               CORBA::COMPLETED_NO);
    }

    if(CORBA::is_nil(obj.in()))
    {
        NI_TEST_CERR("`NameService' is a nil object reference");
        throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                               CORBA::COMPLETED_NO);
    }
    
    CosNaming::NamingContextExt_var nc =
        CosNaming::NamingContextExt::_narrow(obj.in());

    if(CORBA::is_nil(nc.in()))
    {
        NI_TEST_CERR("`NameService' is not a NamingContextExt object reference");
        throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                               CORBA::COMPLETED_NO);
    }

    const char* PROCESS_CALLBACK_BINDING_NAME = "CDMW.I/process-callback.simulated";

    // Get reference to the ProcessCallback object
    CdmwPlatformMngt::ProcessCallback_var callback
        = CdmwPlatformMngt::ProcessCallback::_nil();
    {
        try {
            Cdmw::NamingAndRepository::NamingInterface ni(nc.in());
            CORBA::Object_var obj__ = ni.resolve(PROCESS_CALLBACK_BINDING_NAME);
            callback = CdmwPlatformMngt::ProcessCallback::_narrow(obj__.in());
            if (CORBA::is_nil(callback.in())) {
                NI_TEST_CERR("Couldn't retrieve reference of the ProcessCallback"
                             << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
		// TODO: Update minor code
                throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachApplicationAgent,
                                       CORBA::COMPLETED_NO);
            }
        } catch (...) {
            NI_TEST_CERR("Couldn't retrieve reference of the ProcessCallback"
                         << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
	    // TODO: Update minor code
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachApplicationAgent,
                                   CORBA::COMPLETED_NO);
        }
    }
    return callback._retn();
}


}; // End anonymous namespace


int
run(CORBA::ORB_ptr orb, int & argc, char** argv,
    bool interactive_mode,
    bool unit_tests,
    const char* name_port)
{
    Cdmw::LifeCycle::CommandThread * cmd_thr (0);
    if (unit_tests) {
        std::string proc_url("corbaname::localhost:");
        proc_url += name_port;
        proc_url += "#";
        proc_url += PROC_CTXT;
        cmd_thr = new Cdmw::LifeCycle::CommandThread(orb,
                                                      proc_url,
                                                      INIT_WAIT_DURATION,
                                                      PROCESSING_DURATION);
        cmd_thr->start();
    }
    
    orb->run();
    
    if (unit_tests) {
        cmd_thr->join();
        delete cmd_thr;
    }
    return SUCCESS;
}

int main(int argc, char* argv[])
{
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::Common;
    using namespace Cdmw::TestUtils;


    std::string portNumber("");
    
    std::string callbackIOR = OS::get_option_value( argc, argv,
                                                    Options::CALLBACK_IOR_OPTION);

    bool unit_tests = true;
    // ---- Is it launched by platform management for integration tests? ----
    if (callbackIOR != "no" && callbackIOR != "yes")
    {
        unit_tests = false;
    }

    std::string allTests = OS::get_option_value(argc, argv, "--interactive") ;
 
    // ---- Does it use interactive mode? ----
    bool interactive_mode = false;
    if (allTests != "no") {
        interactive_mode = true;
    } 

    // launch Nameservice, SimRepository and TraceCollector or unit tests only.
    OS::ProcessId idNaming, idRepository, idTraceCollector; 

    char** ORB_init_argv = 0;
    int ORB_init_argc(0);
	int timescale = Testable::get_timescale();

    if (unit_tests) 
	{
        // We allow the user to specify a different port number for the Name Service
        std::string option("--CdmwNamePort");
        portNumber = OS::get_option_value(argc, argv, option) ;
        
        if (portNumber == "no") 
		{
           portNumber="5000";
        } 

        // Starting the Name Service
    #if (CDMW_ORB_VDR == orbacus )
        std::string namingservice="nameserv";
        idNaming = OS::create_process( namingservice, "-OAport " + portNumber );
    #elif (CDMW_ORB_VDR == tao )
        std::string namingservice="Naming_Service";
        idNaming = OS::create_process( namingservice, "-ORBEndpoint iiop://localhost:" + portNumber );
    #endif

        OS::sleep(timescale*3000);
    


        // We allow the user to specify a different port number for the Repository
	std::string REPOSITORY_PORT_OPTION = "--RepositoryPort";
	std::string repositoryPort = OS::get_option_value (argc, argv, REPOSITORY_PORT_OPTION);
	
	if (repositoryPort  == "no") {
	    repositoryPort = "5020";
	}
		
        std::string namingCorbaloc =
        #if (CDMW_ORB_VDR == orbacus )
            " -ORBnaming corbaloc::localhost:" + portNumber + "/NameService";
        #elif (CDMW_ORB_VDR == tao )
            " -ORBInitRef NameService=corbaloc:iiop:localhost:" + portNumber + "/NameService";
        #endif
    
        // Starting the Repository Simulator
    #if (CDMW_ORB_VDR == orbacus )
        std::string simRepositoryArgs = "-OAport " + repositoryPort + namingCorbaloc;
    #elif (CDMW_ORB_VDR == tao )
        std::string simRepositoryArgs = "-ORBEndpoint iiop://localhost:" + repositoryPort + namingCorbaloc;
    #endif
        simRepositoryArgs += " -DOM ";
        simRepositoryArgs += Cdmw::LifeCycle::get_RESOURCES_DOMAIN_NAME();
        simRepositoryArgs += " -DOM ";
        simRepositoryArgs += Cdmw::LifeCycle::get_FACTORIES_DOMAIN_NAME();
        simRepositoryArgs += " -DOM ";
        simRepositoryArgs += Cdmw::Common::Locations::CDMW_SERVICES_NAME_DOMAIN;
        simRepositoryArgs += " -DOM ";
        simRepositoryArgs += Cdmw::Common::Locations::CDMW_SERVICES_NAME_DOMAIN;
        simRepositoryArgs += "/";
        simRepositoryArgs += Cdmw::Common::Locations::TRACE_COLLECTORS_NAME_DOMAIN;
        simRepositoryArgs += " -ROOT ";
        simRepositoryArgs += Cdmw::Common::Locations::ADMIN_ROOT_CONTEXT_ID;
       
        idRepository = OS::create_process ("cdmw_simulated_repository", 
                                           simRepositoryArgs);
        OS::sleep(timescale*3000);
        
        std::string strArgv = argv[0] + namingCorbaloc;
        ORB_init_argv = String::to_char_array (String::to_strings(strArgv));
        ORB_init_argc = 3;
	
        // Starting the Trace Collector :
	// "tracecollector --collector-name=<name> --traceFile-name=<name>
	//  --CdmwLocalisationService=<port>
	//  --CdmwProcessPort=<port> --namingRepos_url=<url>"
	
        // We allow the user to specify a different port numbers
	
        // get TraceCollector Service port
	
	std::string COLLECTOR_SERV_PORT_OPTION = "--CollectorServicePort";
        std::string collectorServPort = OS::get_option_value (argc, argv, 
							      COLLECTOR_SERV_PORT_OPTION);
        
        if (collectorServPort  == "no") {
	    collectorServPort = "5031";
        } 

        // get TraceCollector Cdmw port
        std::string COLLECTOR_CDMW_PORT_OPTION = "--CollectorCdmwPort";
        std::string collectorCdmwPort = OS::get_option_value (argc, argv, 
							       COLLECTOR_CDMW_PORT_OPTION);
        
        if (collectorCdmwPort  == "no") {
	    collectorCdmwPort = "5032";
        } 

	std::string repositoryCorbaloc =
	    " --namingRepos-url=corbaloc::localhost:" + repositoryPort + "/SimRepository";
	
	
        std::string traceCollector = "cdmw_trace_collector";
        std::string traceCollectorArgs ("");
        traceCollectorArgs += " --collector-name=";
        traceCollectorArgs += "collector_0";
        traceCollectorArgs += " --traceFile-name=";
        traceCollectorArgs += "test_trace";
        traceCollectorArgs += " --CdmwLocalisationService=";
	traceCollectorArgs += collectorServPort;
        traceCollectorArgs += " --CdmwProcessPort=";
	traceCollectorArgs += collectorCdmwPort;
        traceCollectorArgs += repositoryCorbaloc;
    
        idTraceCollector = OS::create_process (traceCollector, traceCollectorArgs);
	
        OS::sleep(timescale*5000);
    }

    int status = SUCCESS;
    CORBA::ORB_var orb;

    
    char** argv__ = 0;
    int argc__(0);

    try {
	// Command for all the tests
        std::string commands("verbose \n all \n x \n");
	std::istringstream commands_is(commands);

        // Initialize the ORB
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        if (unit_tests) {
            orb = Cdmw::OrbSupport::OrbSupport::ORB_init(ORB_init_argc, ORB_init_argv, 
                                                          orb_strategies);

	    // Add Application Agent IOR and Process Name
            {
		CdmwPlatformMngt::ProcessCallback_var callback = get_process_callback(orb.in());
        CORBA::String_var s = orb->object_to_string(callback.in());
                std::string str(argv[0]);
                str += " ";
                str += Cdmw::Common::Options::CALLBACK_IOR_OPTION;
                str += "=";
                str += s.in();
                str += " ";
                str += Cdmw::Common::Options::PROCESS_XML_FILE_OPTION;
                str += "=Process_Gen.xml";
                str += " ";
                str += Cdmw::Common::Options::VALIDATE_XML_OPTION;
                argv__ = String::to_char_array (String::to_strings(str));
                argc__ = 4;                
            }
        } else {
            orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);

            argv__ = argv;
            argc__ = argc;
        }        

	std::istream * is = 0;

	if (interactive_mode) {
	    // Enter interactive mode
	    is = &std::cin;
	} else {
            is = &commands_is;
	}
	Cdmw::CdmwInit::ProcessControl_var ctrl
            = new Cdmw::LifeCycle::MyProcessControl(orb.in(),
                                                     *is,
                                                     std::cout);

        Cdmw::CdmwInit::CDMW_init(orb.in(), argc__, argv__, ctrl.in());

        //
        // Get the root POA 
        //
        CORBA::Object_var obj = orb -> resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

        //
        // Run implementation (Activate the POA manager)
        //
        PortableServer::POAManager_var poaManager = rootPOA -> the_POAManager();
        poaManager -> activate();

        //Cdmw::CdmwInit::CDMW_init(orb, argc__, argv__);

        status = run(orb.in(), argc__, argv__, interactive_mode, unit_tests, portNumber.c_str());
    } catch (const Cdmw::Exception & ex) {
        NI_TEST_CERR(ex.what());
        status = FAILURE;
    } catch(const CORBA::Exception& ex)  {        
        NI_TEST_CERR(ex);
        status = FAILURE;
    }

	// wait end of trace flushing
    OS::sleep(timescale*5000);

    // We free memory allocated by String::to_char_array
    if (unit_tests) {
        for (int i=0; i < argc__; ++i) {
            delete[] argv__[i];
        }
        delete[] argv__;
    }
    
    if(!CORBA::is_nil(orb.in())) {
        try {
            Cdmw::CdmwInit::CDMW_cleanup(orb.in());
	    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
	    orb->destroy();
        }
        catch(const CORBA::Exception& ex) {     
            NI_TEST_CERR(ex);
            status = FAILURE;
        }
    }        

    if (unit_tests) {
        // We free memory allocated by String::to_char_array
        for (int iArgc=0; iArgc < ORB_init_argc; ++iArgc) {
            delete[] ORB_init_argv[iArgc];
        }
        delete[] ORB_init_argv;
        try {
            // And we kill the TraceCollector, Repository simulator and Name Server
            OS::kill_process (idTraceCollector);
        } catch(const std::exception & ex) {
            NI_TEST_CERR("Cannot kill TraceCollector process :" << ex.what());
            status = FAILURE;
        }
        try {
            OS::kill_process (idRepository);
        } catch(const std::exception & ex) {
            NI_TEST_CERR("Cannot kill SimRepository process :" << ex.what());
            status = FAILURE;
        }
        try {
            OS::kill_process (idNaming);
        } catch(const std::exception & ex) {
            NI_TEST_CERR("Cannot kill NameServe process :" << ex.what());
            status = FAILURE;
        }
    }
    return status;    
}

