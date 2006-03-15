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


#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "namingandrepository/FTNamingAndRepositoryBehaviour.hpp"
#include "namingandrepository/TestNamingContext_impl.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

#include "Foundation/common/Options.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/common/Assert.hpp"


#include <FaultTolerance/ftinit/FTServiceInit.hpp>
#include <FaultTolerance/ftcommon/FTConfiguration.hpp>
#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp> 


#include <sstream>
#include <fstream>
#include <iostream>
#include <string>

namespace
{

    using namespace Cdmw::NamingAndRepository;
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::OrbSupport;
    using namespace Cdmw::Trace;
    using namespace Cdmw::Common;


    const int POA_THREAD_POOL_SIZE = 10;

    std::string CONFIG_OPTION = "--conf";
    std::string ID_OPTION = "--id";

    std::string TRACE_OPTION = "--trace";
    const char *TRACE_PRODUCER_DEFAULT_NAME = "RepositoryTraceProducer";

    std::string THREADING_TEST_OPTION = "--threading-test";
    const char *THREADING_TEST_NAMING_CONTEXT = "ThreadingTestNamingContext";
    const char *THREADING_TEST_NAMING_CONTEXT_IOR = "/tmp/ThreadingTestNamingContext.IOR";

};

#define COUT(mes) std::cout << "NamingAndRepository : " << mes << std::endl;
#define CERR(mes) std::cout << "NamingAndRepository : FAILURE : " << mes << std::endl;

void exceptionHandler(void)
{

	CERR("UNEXPECTED EXCEPTION HANDLER");

}


void assertionFailedHandler(void)
{

	CERR("ASSERTION FAILED HANDLER");
    ::exit(EXIT_FAILURE);

}


void exitHandler(void)
{
	COUT("Exited");
}


std::string getSyntax()
{

    std::ostringstream syntax;

    syntax << std::endl << "Syntax is :" << std::endl;
    syntax << Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION << "=Port";
    syntax << " " << ID_OPTION << "=PhysicalID";
    syntax << " " << CONFIG_OPTION << "=XMLConfigFile" << std::endl;
    syntax << " [" << Cdmw::Common::Options::VALIDATE_XML_OPTION << "]";
    syntax << " [" << TRACE_OPTION << "[=TraceProducerName] ]";
    syntax << " [" << THREADING_TEST_OPTION << "]";
    syntax << std::endl;

    return syntax.str();
}

std::string getAdditionalSyntax()
{

    std::ostringstream syntax;

    syntax << Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION << " : The repository will use the specified port for the localisation service (corbaloc)" << std::endl;
    syntax << THREADING_TEST_OPTION << " : The repository will create a special purpose naming context for threading tests" << std::endl;
    syntax << TRACE_OPTION << " : Traces are enabled. If no name is specified, the producer interface will be named \"" << TRACE_PRODUCER_DEFAULT_NAME << "\"" << std::endl;
    syntax << Cdmw::Common::Options::VALIDATE_XML_OPTION << " : The XML configuration file will be validated before processing." << std::endl;
    syntax << ID_OPTION << " : Specifies the mandatory physical identifier of the repository" << std::endl;
    syntax << CONFIG_OPTION << " : Specifies the mandatory XML configuration file" << std::endl << std::endl;
    syntax << std::endl;

    return syntax.str();
}

class InitialisationException : public Cdmw::Exception
{
    
    public:
        InitialisationException()
            : Exception( Exception::FATAL, "InitialisationException" )
        {
        }
};


/**
*/ 
void notifyPreInitialisationError(bool platformManaged, const char* message)
{

    if (platformManaged)
    {
        PlatformInterface::Notify_fatal_error("CDMW_NAMREP", message);
    }
    else
    {
        CERR(message);
    }
    
    CDMW_THROW(InitialisationException);

}


enum ExitType
{
    ORB_DESTROY,                // the ORB will be destroyed
    ORB_SHUTDOWN_AND_DESTROY    // the ORB will be shutdown and destroyed
};



int main(int argc, char* argv[])
{

#   ifndef _MSC_VER    

    std::set_unexpected(&exceptionHandler);

    // VC++ contrary to the clause 18.6 of the C++ standard, 
    //  set_unexpected, is not inside the std:: namespace
#   else

    set_unexpected(&exceptionHandler);

#   endif    

    Cdmw::set_assertion_failed(&assertionFailedHandler);

    ::atexit(&exitHandler);

    // get Process Executable Name for trace processing
	std::string processName = argv[0];
	  	
	// suppress process absolute path
	std::string::iterator begin = processName.begin();
	int i = processName.rfind('/') + 1;
	std::string::iterator j = begin + i;
	processName.erase(begin,j);
	
    // trace option
    std::string traceProducerName = OS::get_option_value (argc, argv, TRACE_OPTION);
    
    // get the trace producer name if specified or use the default one
    if (traceProducerName == "no")
    {
        // trace will be deactivated
        traceProducerName = "";
    }
    else if (traceProducerName == "yes")
    {
        // use the default name for the trace producer
        traceProducerName = TRACE_PRODUCER_DEFAULT_NAME;
    }

    // xml option
    bool validate = false;
    std::string validateXML_option =
        OS::get_option_value( argc, argv, Cdmw::Common::Options::VALIDATE_XML_OPTION);
    if ( validateXML_option == "yes" || validateXML_option != "no" )
    {
        validate = true;
    }

    // the return code 
    int ret_code = 0;

    // FIXME what about exception to be caught?
    Cdmw::FT::FTServiceInit::Init( argc, argv );

    // Initialize the ORB
    StrategyList orbStrategies;
    orbStrategies.add_OrbThreaded();
    orbStrategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);
    orbStrategies.add_multicast();


    CORBA::ORB_var orb;
    ExitType exitType = ORB_SHUTDOWN_AND_DESTROY;

    try
    {
 
        orb = OrbSupport::ORB_init(argc, argv, orbStrategies);

        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var poa = PortableServer::POA::_narrow(obj.in());

        PortableServer::POAManager_var poaMgr = poa->the_POAManager();
        poaMgr->activate();

	bool platformManaged = 
	    PlatformInterface::Is_launched_by_PlatformManagement(argc, argv);

        if (platformManaged)
        {
            // initialise the platform interface
            // PlatformInterface::Setup(orb.in(), argc, argv); // FIXME dont do this if started in FT mode, otherwise BadOrderException in generated code
        }

        // preinitialisation : command line check up

        if (argc <= 1 )
        {
            std::string message("Missing mandatory arguments.");
            message += getSyntax();
            notifyPreInitialisationError(platformManaged, message.c_str());
        }

        std::string port_option = OS::get_option_value( argc, argv,
            Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION);

        if ( port_option == "no" || port_option == "yes" )
        {
            std::string message("Port not specified.");
            message += getSyntax();
            notifyPreInitialisationError(platformManaged, message.c_str());
        }

        std::string physicalId = OS::get_option_value( argc, argv, ID_OPTION);
        if ( physicalId == "no" || physicalId == "yes" )
        {
            std::string message("Physical identifier not specified.");
            message += getSyntax();
            notifyPreInitialisationError(platformManaged, message.c_str());
        }

        std::string configurationFile = OS::get_option_value( argc, argv, CONFIG_OPTION);
        if ( configurationFile == "no" || configurationFile == "yes" )
        {
            std::string message("Configuration file not specified.");
            message += getSyntax();
            notifyPreInitialisationError(platformManaged, message.c_str());
        }

        if ( ! OS::file_exists(configurationFile) )
        {
            std::string message("Configuration file '");
            message += configurationFile;
            message += "' not found";
            notifyPreInitialisationError(platformManaged, message.c_str());
        }

        // creates the process behaviour
        std::auto_ptr<FTNamingAndRepositoryBehaviour> pProcessBehaviour(
             new FTNamingAndRepositoryBehaviour (orb.in(), platformManaged,
						 port_option,
						 physicalId,
						 configurationFile, 
						 validate, 
						 processName,
						 traceProducerName));
	
	FTNamingAndRepositoryBehaviour * pProcessBehaviour_ptr =
	    pProcessBehaviour.get();

        Cdmw::CdmwInit::CDMW_init(
            orb.in(), argc, argv, pProcessBehaviour_ptr );
	
	std::string threadingTest = 
	    OS::get_option_value( argc, argv, THREADING_TEST_OPTION);
	if ( threadingTest == "yes" )
            {
		
                // Create a naming context for multithreading access tests
                CosNaming::NamingContextExt_var namingContext = TestNamingContext_impl::create(THREADING_TEST_NAMING_CONTEXT);
		
                // save this naming context's IOR in a file
                CORBA::String_var namingContextIOR = orb->object_to_string(namingContext.in());
		
                std::ofstream namingContextIORFile(THREADING_TEST_NAMING_CONTEXT_IOR);
                namingContextIORFile << namingContextIOR.in();
                namingContextIORFile.close();
		
                COUT("IOR of Naming Context for threading test in " << THREADING_TEST_NAMING_CONTEXT_IOR << " file");		
            }

        // Enter in the main loop
        orb->run();

	// FIXME get rid of this useless variable
	exitType = ORB_DESTROY;

//         if (platformManaged)
//         {
// 	    ASSERT(!"FIXME - gone here");

//             // acknowledge the creation of the process
//             PlatformInterface::Acknowledge_creation(pProcessBehaviour.get());
//             pProcessBehaviour.release();

//             orb->run();
//             COUT("The ORB has exited its main loop");

//             //orb->shutdown() is done by the ProcessBehaviour
//             exitType = ORB_DESTROY;
//         }
//         else
//         {	   
// 	    ASSERT(!"FIXME - gone here");

//             CdmwPlatformMngtBase::StartupKind startupKind;
//             startupKind.state_retrieval = CdmwPlatformMngtBase::LAST_STATE;
//             startupKind.startup_mode = 0;
//             startupKind.specified_state_id = CORBA::string_dup("");

//             (pProcessBehaviour.get())->initialise(startupKind);


//             // create a threading test special purpose naming context if required
//             std::string threadingTest = OS::get_option_value( argc, argv, THREADING_TEST_OPTION);
//             if ( threadingTest == "yes" )
//             {

//                 // Create a naming context for multithreading access tests
//                 CosNaming::NamingContextExt_var namingContext = TestNamingContext_impl::create(THREADING_TEST_NAMING_CONTEXT);

//                 // save this naming context's IOR in a file
//                 CORBA::String_var namingContextIOR = orb->object_to_string(namingContext.in());

//                 std::ofstream namingContextIORFile(THREADING_TEST_NAMING_CONTEXT_IOR);
//                 namingContextIORFile << namingContextIOR.in();
//                 namingContextIORFile.close();

//                 COUT("IOR of Naming Context for threading test in " << THREADING_TEST_NAMING_CONTEXT_IOR << " file");

//             }

//             (pProcessBehaviour.get())->run();

//             orb->run();
//             // the repository can only be killed

//             exitType = ORB_DESTROY;

//         }
    }
    catch(const InitialisationException &e)
    {
        // already logged
        ret_code = EXIT_FAILURE;
    }
    catch(const Cdmw::Exception &e)
    {
        CERR(e.what());
        ret_code = EXIT_FAILURE;
    }
    catch(const CORBA::Exception &e)
    {
        CERR("unexpected " << e._name() << " CORBA exception");
        ret_code = EXIT_FAILURE;
    }
    catch(...)
    {
        CERR("unexpected exception");
        ret_code = EXIT_FAILURE;
    }
    
//     if (!CORBA::is_nil(orb.in()))
//     {
//         if (exitType == ORB_SHUTDOWN_AND_DESTROY)
// 		  orb->shutdown(false);
// 	Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
// 	orb->destroy();
//     }

    if(!CORBA::is_nil(orb.in()))
    {
        try {
	    // FIXME - CORBA_ORB::shutdown segfaults with BAD_INV_ORDER CORBA exception
            Cdmw::CdmwInit::CDMW_cleanup(orb.in());
            Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
            orb->destroy();
        }
        catch(const CORBA::Exception& ex)
        {   
	    return EXIT_FAILURE;
        }
    }       

    return ret_code;
}


