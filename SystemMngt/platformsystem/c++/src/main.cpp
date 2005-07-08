/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/String.hpp"

#include "platformsystem/SystemLogMngr.hpp"
#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"
#include "SystemMngt/platformlibrary/BoundSyncCall.hpp"
#include "SystemMngt/platformlibrary/Iterator.hpp"
#include "platformsystem/SystemLogMessageId.hpp"

#include "platformsystem/CentralisedSupervisionSystem_impl.hpp"
#include "platformsystem/DistributedSupervisionSystem_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"

#include "SystemMngt/platformvaluetypes/SystemStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/SystemModeChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/HostStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationAgentDeath_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationModeChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ProcessStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/EntityStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/GraphElement_impl.hpp"
#include "SystemMngt/platformvaluetypes/ProcessDef_impl.hpp"
#include "SystemMngt/platformvaluetypes/UserEvent_impl.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"

#include <cstdlib>
#include <string>
#include <sstream>
#include <signal.h>

#ifdef CDMW_USE_FAULTTOLERANCE
#include "SystemMngt/platformlibrary/ClientIDGenerator.hpp"
#include "FaultTolerance/ftinit/FTServiceInit.hpp"
#include <tao/ORB_Core.h>
#endif 

namespace
{
    const std::string CENTRALISED_OPTION = "--centralised";
    const std::string DISTRIBUTED_OPTION = "--distributed";
    const std::string EVENT_TIMEOUT_OPTION = "--event-timeout";
    const std::string CREATION_TIMEOUT_OPTION = "--creation-timeout";
#ifdef CDMW_USE_FAULTTOLERANCE
    const std::string FAULT_MANAGER_CORBALOC = "--FaultManagerRegistration";
#endif    
}; // End anonymous namespace


enum ExitType
{
    ORB_DESTROY,                // the ORB will be destroyed
    ORB_SHUTDOWN_AND_DESTROY    // the ORB will be shutdown and destroyed
};


const int POA_THREAD_POOL_SIZE = 5;   

static CORBA::ORB_var orb; 

void print_syntax(char* program_name)
{
    std::cout << "Syntax :\n";
    std::cout << program_name << " options\n";
    std::cout << "\n";
    std::cout << "Options:\n";
    std::cout << " [" << Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION << "=PORT]";
    std::cout << " [" << CENTRALISED_OPTION << " | " << DISTRIBUTED_OPTION << "]";
    std::cout << " [" << EVENT_TIMEOUT_OPTION << "=time_out]";
    std::cout << " [" << CREATION_TIMEOUT_OPTION << "=time_out]";
#ifdef CDMW_USE_FAULTTOLERANCE
    std::cout << " [" << FAULT_MANAGER_CORBALOC << "=fault_manager_corbaloc]";
    std::cout << " [" << Cdmw::Common::Options::REQUEST_DURATION_TIME << "=time]"; 
#endif
    std::cout << " [" << Cdmw::Common::Options::LOG_FILE_OPTION << "=full_path_to_log_file]";
    std::cout << std::endl;
}


void exceptionHandler(void)
{
    std::cout << "Platform Supervision : UNEXPECTED EXCEPTION HANDLER" << std::endl;
}

#ifdef WIN32
void __cdecl sighandler (int sig)
#else
void sighandler (int sig)
#endif
{
	std::cout << std::endl << "Platform Supervision : stop requested" << std::endl;
	
	try
	{
	    if (!CORBA::is_nil(orb.in()))
	    {
	        // terminate event loop
	        orb->shutdown (false);
	    }
	    
	}
	catch (...)
	{
	    std::cout << "Platform Supervision : unexpected exception in signal handler" << std::endl;
	}
	
}



int main(int argc, char* argv[])
{
    using namespace Cdmw::PlatformMngt;
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::OrbSupport;
    using namespace Cdmw::Common;
    
    // the return code 
    int ret_code = 0;
    ExitType exitType = ORB_SHUTDOWN_AND_DESTROY;
    
    // initialise log manager
    SystemLogMngr::initialise();

    // checks the arguments
    long specificPort = -1;
    std::string specificPortStr;
    bool centralised = true;
    std::string logFile;
    unsigned long eventTimeout;
    unsigned long creationTimeout;
#ifdef CDMW_USE_FAULTTOLERANCE
    size_t duration_time=20000000;
#endif

#   ifndef _MSC_VER    

    std::set_unexpected(&exceptionHandler);

    // VC++ contrary to the clause 18.6 of the C++ standard, 
 	//  set_unexpected, is not inside the std:: namespace
#   else

    set_unexpected(&exceptionHandler);

#   endif 


    /*
    // handle interupt
    signal(SIGINT, sighandler);
    */


    //
    // help argument
    //
    if (argc > 1)
    {
        if (strcmp(argv[1], "--help") == 0)
	    {
            print_syntax (argv[0]);
            return ret_code;
        }
    }

    // checks the number of options
#ifdef CDMW_USE_FAULTTOLERANCE
    if (argc > 8)
#else
    if (argc > 6)
#endif
    {
        LogMngr::logMessage(FTL, MSG_ID_SUPERVISION_INVALID_OPTIONS);
        print_syntax(argv[0]);
        ::exit(EXIT_FAILURE);
    }

    // checks the port option
    std::string port_option =
        OS::get_option_value(argc, argv, Options::LOCALISATION_SERVICE_OPTION);

    if (port_option != "no")
    {
        if (port_option != "yes")
        {
            specificPort = ::atoi(port_option.c_str());

            if (specificPort <= 0)
            {
               LogMngr::logMessage (FTL, MSG_ID_SUPERVISION_INVALID_PORT, port_option);
               ::exit(EXIT_FAILURE);
            }
        }
        else
        {
            LogMngr::logMessage (FTL, MSG_ID_SUPERVISION_PORT_NOT_SPECIFIED);
            print_syntax(argv[0]);
            ::exit(EXIT_FAILURE);
        }

    }
    
    // get default port for supervision system
    else
    {
        specificPort = CdmwPlatformMngt::DEFAULT_SUPERVISION_PORT;
    }

    // checks the centralised/distributed supervision option
    std::string centralised_option =
        OS::get_option_value(argc, argv, CENTRALISED_OPTION);

    std::string distributed_option =
        OS::get_option_value(argc, argv, DISTRIBUTED_OPTION);

    if (centralised_option == "yes" && distributed_option == "yes")
    {
        LogMngr::logMessage (FTL, MSG_ID_SUPERVISION_INVALID_OPTIONS);
        print_syntax(argv[0]);
        ::exit(EXIT_FAILURE);
    }
        
    if (distributed_option == "yes")
    {
        centralised = false;
    }
        
        
    // checks the event notifier timeout option
    std::string event_timeout_option =
        OS::get_option_value(argc, argv, EVENT_TIMEOUT_OPTION);

    if (event_timeout_option != "no")
    {
        if (event_timeout_option != "yes")
        {
            eventTimeout = ::atoi(event_timeout_option.c_str());
            if (eventTimeout <= 0)
            {
               LogMngr::logMessage (FTL, MSG_ID_SUPERVISION_INVALID_EVNT_TIMEOUT, 
                                    event_timeout_option);
               ::exit(EXIT_FAILURE);
            }
        }
        else
        {
            LogMngr::logMessage (FTL, MSG_ID_SUPERVISION_EVNT_TIMEOUT_NOT_SPECIFIED);
            print_syntax(argv[0]);
            ::exit(EXIT_FAILURE);
        }
    }
    
    // set default event notifier timeout in msec
    else
    {
        eventTimeout = 5000;
    }

    // checks the application, agent, process creation timeout option
    std::string creation_timeout_option =
        OS::get_option_value(argc, argv, CREATION_TIMEOUT_OPTION);

    if (creation_timeout_option != "no")
    {
        if (creation_timeout_option != "yes")
        {
            creationTimeout = ::atoi(creation_timeout_option.c_str());

            if (creationTimeout <= 0)
            {
               LogMngr::logMessage (FTL, MSG_ID_SUPERVISION_INVALID_CREAT_TIMEOUT, 
                                    creation_timeout_option);
               ::exit(EXIT_FAILURE);
            }
        }
        else
        {
            LogMngr::logMessage (FTL, MSG_ID_SUPERVISION_CREAT_TIMEOUT_NOT_SPECIFIED);
            print_syntax(argv[0]);
            ::exit(EXIT_FAILURE);
        }
    }
    
    // set default creation timeout in msec
    else
    {
        creationTimeout = 5000;
    }

    // checks the log file option
    std::string logFile_option =
        OS::get_option_value( argc, argv, Options::LOG_FILE_OPTION);

    if (logFile_option != "no")
    {
        if (logFile_option != "yes")
        {
            logFile = logFile_option; 
        }
        else
        {
            LogMngr::logMessage (FTL, MSG_ID_SUPERVISION_LOG_FILE_NOT_SPECIFIED);
            print_syntax(argv[0]);
            ::exit(EXIT_FAILURE);
        }
    }

    
#ifdef CDMW_USE_FAULTTOLERANCE
    // checks the FaultManager corbaloc
    std::string FaultManCorbaloc_option =
        OS::get_option_value( argc, argv, FAULT_MANAGER_CORBALOC);
    
    std::string FaultManCorbaloc;
    
    if ( FaultManCorbaloc_option!= "no")
    {
        if (FaultManCorbaloc_option != "yes")
        {
            FaultManCorbaloc = FaultManCorbaloc_option;           
        }
        else
        {
            LogMngr::logMessage (FTL, "Fault manager corbaloc not specified");
            print_syntax(argv[0]);
            ::exit(EXIT_FAILURE);
        }
    }
    else
    {
        LogMngr::logMessage (FTL, "Fault manager corbaloc not specified");
        print_syntax(argv[0]);
        ::exit(EXIT_FAILURE);
    }   


    //check the request duration time option
    std::string request_duration_time_value =
        OS::get_option_value(argc, argv, Cdmw::Common::Options::REQUEST_DURATION_TIME);
    if ( request_duration_time_value != "no" )
    {
        if ( request_duration_time_value == "yes")
        {
            LogMngr::logMessage(FTL, "the request duration time option is used without any value ");
            ::exit(EXIT_FAILURE); 
        } else {
            duration_time = atoi(request_duration_time_value.c_str());
        }        
    } else {
        LogMngr::logMessage(INF, "the request duration time is the default value");      
    }

    
    // Initialise FT service
    Cdmw::FT::FTServiceInit::init( argc, argv, true );

#endif    

    try
    {
	    // If a log file has been specified, use it
	    if (!logFile.empty())
        {
            LogMngr::logMessage(INF, MSG_ID_SUPERVISION_LOG_FILE, logFile);

	        LogMngr::open(logFile);
            // next messages will be logged in the file
	    }

        // initialises the ORB

        StrategyList strategyList;
        strategyList.add_OrbThreaded();
        //strategyList.add_PoaThreadPerRequest();
        strategyList.add_PoaThreadPool(POA_THREAD_POOL_SIZE);
   
        // set the port to string
        std::ostringstream buffer;
        buffer << specificPort;
        specificPortStr = buffer.str();

        // set orb arguments
        std::string str_opt(argv[0]);
        str_opt += " ";
        str_opt += Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION;
        str_opt += "=";
        str_opt += specificPortStr;

        char** argv__ = Cdmw::Common::String::to_char_array
            (Cdmw::Common::String::to_strings(str_opt));
        int argc__(2);

        // init ORB
        orb = OrbSupport::OrbSupport::ORB_init(argc__, argv__, strategyList);


        // delete temporary arguments
        for (int i=0; i <= argc__; ++i)
        {
            delete[] argv__[i];
        }
        delete[] argv__;
        
        
        // get root poa
        CORBA::Object_var poaObj = orb -> resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(poaObj.in());
	    
        // activate the POA manager
        PortableServer::POAManager_var manager = rootPOA->the_POAManager();
        manager->activate();

#ifdef CDMW_USE_FAULTTOLERANCE
        TAO_ORB_Core::set_endpoint_selector_factory ("FT_Endpoint_Selector_Factory");
        
        orb->_tao_ft_client_id(Cdmw::PlatformMngt::ClientIDGenerator::generate_supervision_client_id(OS::get_hostname()).c_str());
        
        CORBA::Object_var obj;
        obj = orb->resolve_initial_references("ORBPolicyManager");

        CORBA::PolicyManager_var policy_manager 
            = CORBA::PolicyManager::_narrow(obj.in());
        
        // Create and apply an ORB-wide Routed policy
        CORBA::Any any;
        TimeBase::TimeT duration = duration_time;        
        
        any <<= duration;
        CORBA::PolicyList policies(1);
        policies.length(1);
        policies[0] =
            orb->create_policy(::FT::REQUEST_DURATION_POLICY, any);
        
        policy_manager->set_policy_overrides(policies, CORBA::SET_OVERRIDE);
#endif

        // init iterator manager with root poa
        IteratorManager::initialize(rootPOA.in());
 
        // init BoundSyncCall factory
        BoundSyncCallFactory::initialise(orb.in());
        
        // register event factories (value type factory)
        CORBA::ValueFactoryBase_var factory, old_factory;

        factory = new SystemModeChangeFactory;
        old_factory = orb->register_value_factory(
            "IDL:thalesgroup.com/CdmwPlatformMngt/SystemModeChange:1.0",
            factory.in() );

        factory = new SystemStatusChangeFactory;
        old_factory = orb->register_value_factory(
            "IDL:thalesgroup.com/CdmwPlatformMngt/SystemStatusChange:1.0",
            factory.in() );

        factory = new HostStatusChangeFactory;
        old_factory = orb->register_value_factory(
            "IDL:thalesgroup.com/CdmwPlatformMngt/HostStatusChange:1.0",
            factory.in() );

        factory = new ApplicationAgentDeathFactory;
        old_factory = orb->register_value_factory(
            "IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgentDeath:1.0",
            factory.in() );

        factory = new ApplicationModeChangeFactory;
        old_factory = orb->register_value_factory(
            "IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationModeChange:1.0",
            factory.in() );

        factory = new ApplicationStatusChangeFactory;
        old_factory = orb->register_value_factory(
            "IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationStatusChange:1.0",
            factory.in() );

        factory = new ProcessMessageFactory;
        old_factory = orb->register_value_factory(
            "IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessage:1.0",
            factory.in() );

        factory = new ProcessStatusChangeFactory;
        old_factory = orb->register_value_factory(
            "IDL:thalesgroup.com/CdmwPlatformMngt/ProcessStatusChange:1.0",
            factory.in() );

        factory = new SystemEntityStatusChangeFactory;
        old_factory = orb->register_value_factory(
            "IDL:thalesgroup.com/CdmwPlatformMngt/SystemEntityStatusChange:1.0",
            factory.in() );

        factory = new ApplicationEntityStatusChangeFactory;
        old_factory = orb->register_value_factory(
            "IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationEntityStatusChange:1.0",
            factory.in() );

        factory = new ProcessEntityStatusChangeFactory;
        old_factory = orb->register_value_factory(
            "IDL:thalesgroup.com/CdmwPlatformMngt/ProcessEntityStatusChange:1.0",
            factory.in() );

        factory = new GraphElementFactory;
        old_factory = orb->register_value_factory (
            "IDL:thalesgroup.com/CdmwPlatformMngt/GraphElement:1.0",
            factory.in() );
                
        factory = new ProcessDefFactory;
        old_factory = orb->register_value_factory (
            "IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDef:1.0",
            factory.in() );
                
        factory = new ManagedProcessDefFactory;
        old_factory = orb->register_value_factory (
            "IDL:thalesgroup.com/CdmwPlatformMngt/ManagedProcessDef:1.0",
            factory.in() );
                
            factory = new UserEventFactory;
            old_factory = orb->register_value_factory(
                "IDL:thalesgroup.com/CdmwPlatformMngt/UserEvent:1.0",
                factory.in() );

        // create the supervision event notifier
        std::auto_ptr <SupervisionEventNotifier> eventNotifier (
                    new SupervisionEventNotifier (eventTimeout));  
            
        // and start it
        eventNotifier.get()->start();
        
        // create system states
        System_impl::createStates();
            
        // create system servant
        PortableServer::ServantBase_var systemServant;
        CdmwPlatformMngt::System_var systemObject;

        if (centralised)
        {
            // create application states
            Application_impl::createStates();
            // create the centralised system servant
            CentralisedSupervisionSystem_impl* p_centralisedSystem = 
#ifdef CDMW_USE_FAULTTOLERANCE            
                new CentralisedSupervisionSystem_impl (eventNotifier.get(), orb.in(), rootPOA.in(), FaultManCorbaloc);
#else
                new CentralisedSupervisionSystem_impl (eventNotifier.get(), orb.in(), rootPOA.in());            
#endif
            // create an object var to take pointer ownership
            systemServant = p_centralisedSystem;
                  
            // activate object
            systemObject = p_centralisedSystem->_this();
        }
        else
        {
            // create the distributed system servant
            DistributedSupervisionSystem_impl* p_distributedSystem = 
#ifdef CDMW_USE_FAULTTOLERANCE
                new DistributedSupervisionSystem_impl (eventNotifier.get(), orb.in(), rootPOA.in(), FaultManCorbaloc);
#else
            new DistributedSupervisionSystem_impl (eventNotifier.get(), orb.in(), rootPOA.in());            
#endif
            // create an object var to take pointer ownership
            systemServant = p_distributedSystem;
                  
            // activate object
            systemObject = p_distributedSystem->_this();
        }

        // publishes the supervision system corbaloc
        OrbSupport::OrbSupport::bind_object_to_corbaloc (
                     orb.in(), CdmwPlatformMngt::SUPERVISION_NAME, systemObject.in());
    

        // init configuration static members
        Configuration::M_orb = orb.in();
        Configuration::M_timeout = creationTimeout;


        // logs the startup info
        LogMngr::logMessage (INF, MSG_ID_SUPERVISION_STARTUP, specificPortStr);

        LogMngr::logMessage (INF, MSG_ID_SUPERVISION_CORBALOC, OS::get_hostname(),
                             specificPortStr, CdmwPlatformMngt::SUPERVISION_NAME);


        // runs the ORB loop
        orb->run();
        
        std::cout << std::endl << "Platform Supervision : stop in progess" << std::endl;
        
        exitType = ORB_DESTROY;
	
	    // stop the notifier
        eventNotifier.get()->stop();
        
    }

    catch (const CORBA::COMM_FAILURE& ex)
    {
        ret_code = EXIT_FAILURE;

        LogMngr::logMessage (FTL, MSG_ID_SUPERVISION_UNAVAILABLE_PORT,
                             specificPortStr);
	}
    
    catch (const CORBA::Exception& e) {
        std::cerr << "!!!! CORBA::Exception: " << e << std::endl;
	    ret_code = EXIT_FAILURE;
    }
    catch (const std::exception& e) {
        std::cerr << "!!!! std::exception: " << e.what() << std::endl;
	    ret_code = EXIT_FAILURE;
    }
    
	catch (...)
	{
        std::cerr << "!!!! unknown exception. " << std::endl;
	    ret_code = EXIT_FAILURE;
	}
   
    
    try
    {
        if (!CORBA::is_nil(orb.in()))
        {
            // unregister the value factories
            #if CDMW_ORB_VDR != tao
            orb->unregister_value_factory (
                "IDL:thalesgroup.com/CdmwPlatformMngt/HostStatusChange:1.0");
            orb->unregister_value_factory (
                "IDL:thalesgroup.com/CdmwPlatformMngt/SystemStatusChange:1.0");
            orb->unregister_value_factory (
                "IDL:thalesgroup.com/CdmwPlatformMngt/SystemModeChange:1.0");
            orb->unregister_value_factory (
                "IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgentDeath:1.0");
            orb->unregister_value_factory (
                "IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationStatusChange:1.0");        
            orb->unregister_value_factory (
                "IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationModeChange:1.0");                        
            orb->unregister_value_factory (
                "IDL:thalesgroup.com/CdmwPlatformMngt/ProcessStatusChange:1.0");
            orb->unregister_value_factory (
                "IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessage:1.0");
            orb->unregister_value_factory (
                "IDL:thalesgroup.com/CdmwPlatformMngt/SystemEntityStatusChange:1.0");
            orb->unregister_value_factory (
                "IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationEntityStatusChange:1.0");
            orb->unregister_value_factory (
                "IDL:thalesgroup.com/CdmwPlatformMngt/ProcessEntityStatusChange:1.0");               
            orb->unregister_value_factory (
                "IDL:thalesgroup.com/CdmwPlatformMngt/GraphElement:1.0");
            orb->unregister_value_factory (
                "IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDef:1.0");                
            orb->unregister_value_factory (
                "IDL:thalesgroup.com/CdmwPlatformMngt/ManagedProcessDef:1.0");
            #endif
            
            
            if (exitType == ORB_SHUTDOWN_AND_DESTROY)
            {
                orb->shutdown(false);
            }
                               
            // stops the ORB
	    OrbSupport::OrbSupport::ORB_cleanup(orb.in());
	    orb->destroy();

        }
    }
    catch (...)
    {
    }

    // destroy states
    System_impl::destroyStates();
    
    if (centralised)
    {
        // destroy application states
        Application_impl::destroyStates();
    }

    // destroy factory
    BoundSyncCallFactory::destroy();
    
    // destroy log manager
    LogMngr::destroy();
    
    std::cout << std::endl << "Platform Supervision : stopped and exited" << std::endl;
    
    
    return ret_code;

}

