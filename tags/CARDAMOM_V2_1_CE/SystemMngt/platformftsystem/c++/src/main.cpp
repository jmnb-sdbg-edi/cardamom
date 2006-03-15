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
#include <string>
#include <sstream>

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/common/Exception.hpp"

#include "Foundation/idllib/FTPullMonitorable.skel.hpp"


#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"
#include "SystemMngt/platformlibrary/BoundSyncCall.hpp"
#include "SystemMngt/platformlibrary/Iterator.hpp"
#include "SystemMngt/platformlibrary/RWElementRepository_impl.hpp"

#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"

#include "platformsystemlibrary/SystemLogMngr.hpp"
#include "platformsystemlibrary/SystemLogMessageId.hpp"
#include "platformsystemlibrary/CentralisedSupervisionSystem_impl.hpp"

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
#include "SystemMngt/idllib/CdmwFTSystMngtSystem.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtPullMonitorableRegistration.stub.hpp"

#include "SystemMngt/platformlibrary/ClientIDGenerator.hpp"
#include "FaultTolerance/ftinit/FTServiceInit.hpp"
#include "FaultTolerance/ftstatemanager/DataStoreBase.hpp"

#include "platformftsystem/CdmwInterface.hpp"
#include "platformftsystem/ApplicationDataStoreObserver.hpp"
#include "platformftsystem/EntityDataStoreObserver.hpp"
#include "platformftsystem/HostDataStoreObserver.hpp"
#include "platformftsystem/ProcessDataStoreObserver.hpp"
#include "platformftsystem/RunSetDataStoreObserver.hpp"
#include "platformftsystem/ServiceDataStoreObserver.hpp"
#include "platformftsystem/SystemDataStoreObserver.hpp"
#include "platformftsystem/SystemObsvDataStoreObserver.hpp"
#include "platformftsystem/SystemDataStoreOnInit.hpp"

#include "platformftsystem/PrimaryActivationHandler_impl.hpp"

#include "SystemMngt/platformftaccessorlibrary/SystemAccessor_impl.hpp"


namespace Cdmw
{
namespace PlatformMngt
{

/**
*Purpose: SystemMngtPullMonitorable implementation
*<p>
*
*
*/
class SystemMngtPullMonitorable_impl : virtual public POA_FT::PullMonitorable,
                                    virtual public PortableServer::RefCountServantBase
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        *
        */
        SystemMngtPullMonitorable_impl()
                throw()
        {
        }

        /**
        * Purpose:
        * <p> Destructor
        *
        */
        virtual
        ~SystemMngtPullMonitorable_impl()
                throw()
        {
        }

        /**
        * Purpose:
        * <p> to call to check if object is still living
        *
        *@exception CORBA::SystemException
        *
        */
        virtual CORBA::Boolean is_alive ()
                throw(CORBA::SystemException)
        {
             return true;
        }

}; // End class SystemMngtPullMonitorable_impl

} // End namespace PlatformMngt
} // End namespace Cdmw



namespace
{
    const std::string CENTRALISED_OPTION = "--centralised";
    const std::string DISTRIBUTED_OPTION = "--distributed";
    const std::string EVENT_TIMEOUT_OPTION = "--event-timeout";
    const std::string CREATION_TIMEOUT_OPTION = "--creation-timeout";
    const std::string DAEMON_PORT_OPTION = "--DaemonPort";
    const std::string PULLMON_REGISTER_IOR_OPTION = "--PullMonRegisterIor";
    const std::string FAULT_MANAGER_CORBALOC = "--FaultManagerRegistration";
}; // End anonymous namespace


enum ExitType
{
    ORB_DESTROY,                // the ORB will be destroyed
    ORB_SHUTDOWN_AND_DESTROY    // the ORB will be shutdown and destroyed
};


const int POA_THREAD_POOL_SIZE = 5;   

CORBA::ORB_var orb = CORBA::ORB::_nil(); 

void print_syntax(char* program_name)
{
    std::cout << "Syntax :\n";
    std::cout << program_name 
              << Cdmw::Common::Options::PROCESS_XML_FILE_OPTION << "=XMLConfigFile\n";
    std::cout << "\n";
    std::cout << "Options:\n";
    std::cout << " [" << Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION << "=PORT]";
    std::cout << " [" << CENTRALISED_OPTION << "]";
    std::cout << " [" << EVENT_TIMEOUT_OPTION << "=time_out]";
    std::cout << " [" << CREATION_TIMEOUT_OPTION << "=time_out]";
    std::cout << " [" << Cdmw::Common::Options::LOG_FILE_OPTION << "=full_path_to_log_file]";
    std::cout << " [" << Cdmw::Common::Options::PROCESS_NAME_OPTION << "=process_name]";
    std::cout << " [" << PULLMON_REGISTER_IOR_OPTION << "=pullmonitor_registration_callback_ior]";
    std::cout << " [" << DAEMON_PORT_OPTION << "=daemon_port]";
    std::cout << " [" << FAULT_MANAGER_CORBALOC << "=fault_manager_corbaloc]";
    std::cout << std::endl;
}


void exceptionHandler(void)
{
    std::cout << "Platform Supervision : UNEXPECTED EXCEPTION HANDLER" << std::endl;
}


void exit_handler(int sig) {
    if (!CORBA::is_nil(orb.in())) {
        try {
            orb->shutdown(false);
            std::cout << " was trapped. Supervision abort..." << std::endl;
            Cdmw::OsSupport::OS::Timeval time = Cdmw::OsSupport::OS::get_time();
            std::cout << "########## Supervision Abort date: " << time.seconds << "sec " << time.microseconds << "microsec" << std::endl;
            abort();
        } catch (const CORBA::SystemException& e) {
            std::cerr << "Error while shuting ORB down in exit_handler:\n"
                      << e << " - minor code: " << e.minor() << std::endl;
        }
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
    long daemonPort = -1;
    bool centralised = true;
    std::string logFile;
    unsigned long eventTimeout;
    unsigned long creationTimeout;
    std::string faultManagerCorbaloc = "";
    std::string supervisionProcessName = "";
    std::string pullMonitorRegistrationIOR = "";

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

    if (argc < 3)
    {
       print_syntax (argv[0]);
       return ret_code;
    }

    // checks the number of options
    if (argc > 10)
    {
        LogMngr::logMessage(FTL, MSG_ID_SUPERVISION_INVALID_OPTIONS);
        print_syntax(argv[0]);
        ::exit(EXIT_FAILURE);
    }

    // checks the platform management supervision port option
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
        // !! distributed option is not compatible with FT supervision !!
        // centralised = false;
        LogMngr::logMessage (FTL, MSG_ID_SUPERVISION_INVALID_OPTIONS);
        print_syntax(argv[0]);
        ::exit(EXIT_FAILURE);
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
                std::string msg = "Daemon port option invalid ";
                msg += daemon_port_option;
                LogMngr::logMessage (FTL, msg.c_str());
                ::exit(EXIT_FAILURE);
            }
        }
        else
        {
            LogMngr::logMessage (FTL, "Daemon port option not specified");
            print_syntax(argv[0]);
            ::exit(EXIT_FAILURE);
        }
    }
    
    // get daemon default port
    else
    {
        daemonPort = CdmwPlatformMngt::DEFAULT_SYSTEM_PORT;
    }
    
    // store the daemon port
    Cdmw::PlatformMngt::Configuration::Set_daemon_port(daemonPort);

    // checks the FaultManager corbaloc
    std::string faultManagerCorbaloc_option =
        OS::get_option_value( argc, argv, FAULT_MANAGER_CORBALOC);
        
    if (faultManagerCorbaloc_option!= "no")
    {
        if (faultManagerCorbaloc_option != "yes")
        {
            faultManagerCorbaloc = faultManagerCorbaloc_option;           
        }
        else
        {
            LogMngr::logMessage (FTL, "Fault manager corbaloc not specified");
            print_syntax(argv[0]);
            ::exit(EXIT_FAILURE);
        }
    }
    
    // checks the supervision process name option
    std::string processName_option =
        OS::get_option_value( argc, argv, Cdmw::Common::Options::PROCESS_NAME_OPTION);

    supervisionProcessName = "SMG_SUPERVISION_PROCESS";
    
    if (processName_option != "no")
    {
        if (processName_option != "yes")
        {
            supervisionProcessName = processName_option; 
        }
        else
        {
            LogMngr::logMessage (FTL, MSG_ID_SUPERVISION_PROCESS_NAME_NOT_SPECIFIED);
            print_syntax(argv[0]);
            ::exit(EXIT_FAILURE);
        }
    }  
    
    // checks the pullmonitoring register callback ior option
    std::string callbackIor_option =
        OS::get_option_value( argc, argv, PULLMON_REGISTER_IOR_OPTION);

    if (callbackIor_option != "no")
    {
        if (callbackIor_option != "yes")
        {
            pullMonitorRegistrationIOR = callbackIor_option; 
        }
        else
        {
            LogMngr::logMessage (FTL, MSG_ID_SUPERVISION_CALLBACK_IOR_NOT_SPECIFIED);
            print_syntax(argv[0]);
            ::exit(EXIT_FAILURE);
        }
    }
    
    // init configuration static members
    Configuration::Set_timeout(creationTimeout);
    Configuration::Set_fault_tolerant();

    // Initialise FT service
    Cdmw::FT::FTServiceInit::Init(argc, argv);

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
        
        // init configuration static members
        Configuration::Set_orb(orb.in());


        // delete temporary arguments
        for (int i=0; i <= argc__; ++i)
        {
            delete[] argv__[i];
        }
        delete[] argv__;
        
        
        // get root poa
        CORBA::Object_var poaObj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = 
           PortableServer::POA::_narrow(poaObj.in());
       
        // activate the POA manager
        PortableServer::POAManager_var manager = rootPOA->the_POAManager();
        manager->activate();

        // init Cdmw
        Cdmw::PlatformMngt::CDMW_init(orb.in(), argc, argv, 
                                      supervisionProcessName.c_str(),
                                      faultManagerCorbaloc.c_str());

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
            
        factory = new HostEntityStatusChangeFactory;
        old_factory = orb->register_value_factory(
            "IDL:thalesgroup.com/CdmwPlatformMngt/HostEntityStatusChange:1.0",
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
        
        // create element repository servant
        PortableServer::ServantBase_var elementRepositoryServant;
        CdmwPlatformMngt::RWElementRepository_var elementRepositoryObject;
        
        RWElementRepository_impl* p_elementRepository =
               new RWElementRepository_impl();
               
        // create an object var to take pointer ownership
        elementRepositoryServant = p_elementRepository;
        
        // activate object
        elementRepositoryObject = p_elementRepository->_this();

        // create system states
        System_impl::createStates();
            
        // create system servant
        PortableServer::ServantBase_var systemServant;
        CdmwPlatformMngt::System_var systemObject;

        CentralisedSupervisionSystem_impl* p_system = 0;


        // create application states
        Application_impl::createStates();
        
        // create the centralised system servant
        p_system = 
                new CentralisedSupervisionSystem_impl(eventNotifier.get(), 
                                                      orb.in(), 
                                                      rootPOA.in(), 
                                                      elementRepositoryObject.in());

        // create an object var to take pointer ownership
        systemServant = p_system;
                  
        // activate object
        systemObject = p_system->_this();

        // publishes the supervision system corbaloc
        OrbSupport::OrbSupport::bind_object_to_corbaloc (
                     orb.in(), CdmwPlatformMngt::SUPERVISION_NAME, systemObject.in());

        
        //
        // Register a datastore observer to create SystemMngt tree in backup process
        // 
        // 1) Entity DS
        EntityDataStoreObserver entity_ds_observer(elementRepositoryObject.in());
        CORBA::ULong ds_id = 
           Cdmw::PlatformMngt::Configuration::Get_entity_datastore_id();
        Cdmw::FT::DataStoreBase* dsBase = 
           Cdmw::FT::DataStoreBase::Get_dataStore(ds_id);
        EntityDataStore* entity_datastore = 
           dynamic_cast<EntityDataStore*>(dsBase);
        entity_datastore->register_observer(&entity_ds_observer);

        // 2) Application DS
        ApplicationDataStoreObserver application_ds_observer(p_system);
        ds_id = 
           Cdmw::PlatformMngt::Configuration::Get_application_datastore_id();
        dsBase = Cdmw::FT::DataStoreBase::Get_dataStore(ds_id);
        ApplicationDataStore* application_datastore = 
           dynamic_cast<ApplicationDataStore*>(dsBase);
        application_datastore->register_observer(&application_ds_observer);

        // 3) Host DS
        HostDataStoreObserver host_ds_observer(p_system);
        ds_id = 
           Cdmw::PlatformMngt::Configuration::Get_host_datastore_id();
        dsBase = Cdmw::FT::DataStoreBase::Get_dataStore(ds_id);
        HostDataStore* host_datastore = 
           dynamic_cast<HostDataStore*>(dsBase);
        host_datastore->register_observer(&host_ds_observer);

        // 4)  Process DS
        ProcessDataStoreObserver process_ds_observer(elementRepositoryObject.in());
        ds_id = 
           Cdmw::PlatformMngt::Configuration::Get_process_datastore_id();
        dsBase = Cdmw::FT::DataStoreBase::Get_dataStore(ds_id);
        ProcessDataStore* process_datastore = 
           dynamic_cast<ProcessDataStore*>(dsBase);
        process_datastore->register_observer(&process_ds_observer);

        // 5) RunSet DS
        RunSetDataStoreObserver runset_ds_observer(p_system);
        ds_id = 
           Cdmw::PlatformMngt::Configuration::Get_runset_datastore_id();
        dsBase = Cdmw::FT::DataStoreBase::Get_dataStore(ds_id);
        RunSetDataStore* runset_datastore = 
           dynamic_cast<RunSetDataStore*>(dsBase);
        runset_datastore->register_observer(&runset_ds_observer);

        // 6) Service DS
        ServiceDataStoreObserver service_ds_observer(elementRepositoryObject.in());
        ds_id = 
           Cdmw::PlatformMngt::Configuration::Get_service_defs_datastore_id();
        dsBase = Cdmw::FT::DataStoreBase::Get_dataStore(ds_id);
        ServiceDefsDataStore* service_datastore = 
           dynamic_cast<ServiceDefsDataStore*>(dsBase);
        service_datastore->register_observer(&service_ds_observer);

        // 7) System DS
        SystemDataStoreObserver system_ds_observer(p_system);
        ds_id = 
           Cdmw::PlatformMngt::Configuration::Get_system_datastore_id();
        dsBase = Cdmw::FT::DataStoreBase::Get_dataStore(ds_id);
        SystemDataStore* system_datastore = 
           dynamic_cast<SystemDataStore*>(dsBase);
        system_datastore->register_observer(&system_ds_observer);
        
        // 8) System Observer DS
        SystemObsvDataStoreObserver system_obsv_ds_observer(p_system);
        ds_id = 
           Cdmw::PlatformMngt::Configuration::Get_system_observer_registration_datastore_id();
        dsBase = Cdmw::FT::DataStoreBase::Get_dataStore(ds_id);
        SystemObserverRegistrationDataStore* system_obsv_datastore = 
           dynamic_cast<SystemObserverRegistrationDataStore*>(dsBase);
        system_obsv_datastore->register_observer(&system_obsv_ds_observer);
        
        

        //    
        // Create the system accessor
        //
                  
        // Create the system accessor servant
        SystemAccessor_impl* system_accessor = 
           new SystemAccessor_impl(orb.in(),
                                   rootPOA.in(), 
                                   p_system,
                                   elementRepositoryObject.in(),
                                   supervisionProcessName);   
        
        // create an object var to take pointer ownership
        PortableServer::ServantBase_var system_accessor_servant = 
           system_accessor;        
        
        // activate object
        CdmwFTSystMngt::SystemAccessor_var system_accessor_ref = 
           system_accessor->_this();
           
        //
        // Register a datastore initialiser observer for system datastore
        // 
        SystemDataStoreOnInit system_ds_oninit (system_accessor);
        SystemFTDataStore* system_ft_datastore = 
           dynamic_cast<SystemFTDataStore*>(system_datastore);
        system_ft_datastore->register_initialiser(&system_ds_oninit);

            
        // Set System Accessor object in ft group
        ::FT::ObjectGroup_var system_accessor_group;

        // get replication manager
        CORBA::Object_var replication_mngr_obj = 
           orb->resolve_initial_references("ReplicationManager");
        CdmwFT::ReplicationManager_var replication_mngr =
           CdmwFT::ReplicationManager::_narrow(replication_mngr_obj.in());

                
        // get System Accessor Group Id
        try
        {
            ::FT::Name group_name;
            group_name.length(1);
            group_name[0].id =
               Cdmw::PlatformMngt::Configuration::Get_SystemAccessor_group_name(
                     ).c_str();
                
            system_accessor_group = 
               replication_mngr->get_object_group_ref_from_name(group_name);
        }
        catch(const ::FT::ObjectGroupNotFound&)
        {
            CDMW_THROW2 (Cdmw::BadParameterException, 
                         "SystemAccessor", 
                         "group undefined");
        }
                
        //
        // Register the primary activation handler
        //
        
        // resolve the Activation Manager        
        CORBA::Object_var activation_mngr_obj = orb->resolve_initial_references("FTActivationManager");

        CdmwFT::Location::ActivationManager_var activation_manager = 
                  CdmwFT::Location::ActivationManager::_narrow(activation_mngr_obj.in());
        
        PrimaryActivationHandler_impl* activation_handler =
                          new PrimaryActivationHandler_impl(orb.in(), 
                                                            system_accessor_ref.in(), 
                                                            p_system);
        
        // register the handler into the Activation Manager
        ::CdmwFT::Location::HandlerId handler_id;
        
        handler_id = activation_manager->register_activation_handler(
                      system_accessor_group.in(), activation_handler);

        // set System Management location
        ::FT::Location smg_location;
        smg_location.length(2);
            
        smg_location[0].id = Cdmw::OsSupport::OS::get_hostname().c_str();
        smg_location[0].kind = "hostname";    
        smg_location[1].id = supervisionProcessName.c_str();
        smg_location[1].kind = "processname"; 

        // add the System Accessor object in group
        std::cout << "call add_member for system_accessor_group" << std::endl;
        system_accessor_group = 
           replication_mngr->add_member(system_accessor_group.in(),
                                        smg_location,
                                        system_accessor_ref.in());
        


        
        // create the Pull Monitorable Object to be polled by daemon
        SystemMngtPullMonitorable_impl* pull_monitorable =
             new SystemMngtPullMonitorable_impl();

        // create an object var to take pointer ownership
        PortableServer::ServantBase_var pull_monitorable_servant = 
             pull_monitorable;

        // create the CORBA reference
        FT::PullMonitorable_var pull_monitorable_ref = 
             pull_monitorable->_this();

        // register Pull Monitorable in daemon if callback is defined
        if (!pullMonitorRegistrationIOR.empty())
        {
            // gets the daemon callback for pull monitorable registration
            CORBA::Object_var callback_obj = orb->string_to_object(pullMonitorRegistrationIOR.c_str());

            CdmwPlatformMngt::PullMonitorableRegistration_var callback = 
                CdmwPlatformMngt::PullMonitorableRegistration::_narrow(callback_obj.in());

            if (CORBA::is_nil(callback.in()))
            {
                CDMW_THROW2(Cdmw::BadParameterException, "argv", 
                            "doesn't contain a valid callback IOR");
            }
            
            // register the pull monitorable 
            callback->register_pull_monitorable (pull_monitorable_ref.in());
        }
        


        // logs the startup info
        LogMngr::logMessage (INF, MSG_ID_SUPERVISION_STARTUP, specificPortStr);

        LogMngr::logMessage (INF, MSG_ID_SUPERVISION_CORBALOC, OS::get_hostname(),
                             specificPortStr, CdmwPlatformMngt::SUPERVISION_NAME);


        struct sigaction action;
        action.sa_handler=exit_handler;
        sigemptyset(&action.sa_mask);
        sigaction(SIGTERM, &action, NULL);
        sigaction(SIGINT, &action, NULL);
        sigaction(SIGQUIT, &action, NULL);
        sigaction(SIGABRT, &action, NULL);

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

