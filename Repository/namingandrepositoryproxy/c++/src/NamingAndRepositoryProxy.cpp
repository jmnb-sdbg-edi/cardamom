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
#include "namingandrepositoryproxy/NamingAndRepositoryProxyBehaviour.hpp"
#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

#include "Foundation/common/Options.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/common/Assert.hpp"
#include <FaultTolerance/ftinit/FTServiceInit.hpp>



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

    const std::string DAEMON_PORT_OPTION = "--DaemonPort";
    const std::string TRACE_OPTION = "--trace";
    const char *TRACE_PRODUCER_DEFAULT_NAME = "RepositoryTraceProducer";

};

#define COUT(mes) std::cout << "NamingAndRepositoryProxy : " << mes << std::endl;
#define CERR(mes) std::cout << "NamingAndRepositoryProxy : FAILURE : " << mes << std::endl;

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
    syntax << DAEMON_PORT_OPTION << "=Daemon_Port";
    syntax << " [" << TRACE_OPTION << "[=TraceProducerName] ]";
    syntax << std::endl;

    return syntax.str();
}

std::string getAdditionalSyntax()
{

    std::ostringstream syntax;

    syntax << Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION << " : The repository will use the specified port for the localisation service (corbaloc)" << std::endl;
    syntax << TRACE_OPTION << " : Traces are enabled. If no name is specified, the producer interface will be named \"" << TRACE_PRODUCER_DEFAULT_NAME << "\"" << std::endl;
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
void notifyPreInitialisationError(const char* message)
{

    PlatformInterface::Notify_fatal_error("CDMW_NAMREP", message);
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
    
    long daemonPort = -1;
    
    // checks the daemon port option (port of the system)
    std::string daemon_port_option =
        OS::get_option_value(argc, argv, DAEMON_PORT_OPTION);

    if (daemon_port_option != "no")
    {
        if (daemon_port_option != "yes")
        {
            daemonPort = ::atoi(daemon_port_option.c_str());

            if (daemonPort <= 0)
            {
                std::string message("Daemon Port option invalid ");
                message += daemon_port_option;
                notifyPreInitialisationError(message.c_str());
                ::exit(EXIT_FAILURE);
            }
        }
    }
    
    if (daemonPort == -1)
    {
        /*****
        std::string message("Daemon Port not specified.");
        message += getSyntax();
        notifyPreInitialisationError(message.c_str());
        ::exit(EXIT_FAILURE);
        *****/
        // use the default system port
        daemonPort = CdmwPlatformMngt::DEFAULT_SYSTEM_PORT;
    	std::cout << "Repository Proxy uses default system port for platform daemon : " 
    	          << daemonPort << std::endl;
    }

    // the return code 
    int ret_code = 0;

    // Initialize the ORB
    StrategyList orbStrategies;
    orbStrategies.add_OrbThreaded();
    orbStrategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);


    CORBA::ORB_var orb;
    ExitType exitType = ORB_SHUTDOWN_AND_DESTROY;

    try
    {
 
        // Initialise FT service
        Cdmw::FT::FTServiceInit::Init( argc, argv, true );

        orb = OrbSupport::ORB_init(argc, argv, orbStrategies);

        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var poa = PortableServer::POA::_narrow(obj.in());

        PortableServer::POAManager_var poaMgr = poa->the_POAManager();
        poaMgr->activate();

        // preinitialisation : command line check up

        if (argc <= 1 )
        {
            std::string message("Missing mandatory arguments.");
            message += getSyntax();
            notifyPreInitialisationError(message.c_str());
        }

        std::string port_option = OS::get_option_value( argc, argv,
                                                        Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION);

        if ( port_option == "no" || port_option == "yes" )
        {
            std::string message("Port not specified.");
            message += getSyntax();
            notifyPreInitialisationError(message.c_str());
        }


        // creates the process behaviour
        std::auto_ptr<NamingAndRepositoryProxyBehaviour> pProcessBehaviour(new NamingAndRepositoryProxyBehaviour(orb.in(),
                                                                                                                 port_option,
                                                                                                                 daemon_port_option,
                                                                                                                 processName,
                                                                                                                 traceProducerName) );
        COUT("Start the initalisation of the Process proxy");
        
        // init Cdmw
        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv, pProcessBehaviour.get() );
        
        
        COUT("End of the initalisation of the Process proxy");
        
        
        orb->run();
        
        COUT("The ORB has exited its main loop");
        
        //orb->shutdown() is done by the ProcessBehaviour
        exitType = ORB_DESTROY;
        
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

    if (!CORBA::is_nil(orb.in()))
    {
        if (exitType == ORB_SHUTDOWN_AND_DESTROY)
            orb->shutdown(false);
	Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
	orb->destroy();
    }

    return ret_code;

}


