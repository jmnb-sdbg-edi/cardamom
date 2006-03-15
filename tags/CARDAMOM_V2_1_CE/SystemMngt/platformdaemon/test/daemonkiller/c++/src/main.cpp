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


#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/String.hpp"

#include "platformdaemonlibrary/DaemonLogMngr.hpp"
#include "platformdaemonlibrary/DaemonLogMessageId.hpp"

#include "platformdaemonlibrary/ConfigurationMngr.hpp"

#include "platformdaemon/Daemon_impl.hpp"

#include "SystemMngt/platformlibrary/ProcessLauncher.hpp"
#include "SystemMngt/platformlibrary/ProcessKillMngr.hpp"

#ifdef CDMW_USE_FAULTTOLERANCE
#include "SystemMngt/platformlibrary/Configuration.hpp"

#include "SystemMngt/platformlibrary/ClientIDGenerator.hpp"
#include "platformdaemon/AccessorLookupService_impl.hpp"
#include "SystemMngt/platformftproxy/InitUtils.hpp"
#include "SystemMngt/platformftproxy/SystemProxy.hpp"
#include "SystemMngt/platformvaluetypes/GraphElement_impl.hpp"
#include "SystemMngt/platformvaluetypes/ProcessDef_impl.hpp"

#include "FaultTolerance/ftmonitoring/InitUtils.hpp"
#include "FaultTolerance/ftinit/FTServiceInit.hpp"
#include "FaultTolerance/idllib/FT.stub.hpp"
#include <FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTManager.stub.hpp>
#include "FaultTolerance/ftmonitoring/MonitoringLibraryInterface_impl.hpp"
#include <tao/ORB_Core.h>
#endif

#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/common/Assert.hpp"
#include <daemonkiller/TestInterceptors.hpp>

#if CDMW_ORB_VDR == tao && CDMW_ORB_VER > 13
#include <tao/ORBInitializer_Registry.h>
#endif



const int POA_THREAD_POOL_SIZE = 20;    

// Daemon must be launched by the following script.
const char* CDMW_START_DAEMON_SCRIPT="cdmw_platform_daemon.sh";

const char * CDMW_SYSTEM_MNGT_PROXY = "SystemMngtProxy";
const int MAX_RETRY_ON_FTMGR = 4;
const int SLEEP_RETRY_ON_FTMGR_MS = 1000;


// This option is to check if daemon has been started by the script
// (only the script should use it !!) 
const char* STARTED_BY_SCRIPT_CHECKING_OPTION = "--isStartedByScript";

const char* FAULT_MANAGER_CORBALOC_OPTION = "--FaultManagerRegistration";

const char* METHOD_TO_INTERCEPT_OPTION="--method-to-intercept";
const char* WHERE_OPTION="--where";
const char* COUNT_OPTION="--count";
const char* SUPERVISION_PID_OPTION="--sup-pid";


void print_syntax(char* exe)
{
    std::cout << "Syntax is :" << std::endl << exe;
    std::cout << " [ " << METHOD_TO_INTERCEPT_OPTION << "=<initialise|run|stop>";
    std::cout << " [ " << WHERE_OPTION << "=<receive|reply>";
    std::cout << " [ " << COUNT_OPTION << "=when";
    std::cout << " [ " 
              << Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION 
              << "=PORT";
    std::cout << " | " 
              << Cdmw::Common::Options::PROCESS_XML_FILE_OPTION 
              << "=XMLConfigFile";
    std::cout << " [" 
              << Cdmw::Common::Options::VALIDATE_XML_OPTION 
              << "] ]";
    std::cout << " [" 
              << FAULT_MANAGER_CORBALOC_OPTION 
              << "=fault_manager_corbaloc]";
    std::cout << std::endl;
}


class TestORBInitializer : public PortableInterceptor::ORBInitializer,
                                                       public Cdmw::OrbSupport::RefCountLocalObject
{
public:

    // constructor
    TestORBInitializer( int count, const char* method
                        , const char* where, const char* process)
        : m_expected_event_count(count)
        , m_method(method)
        , m_where(where)
        , m_process(process){ }

    //destructor
    ~TestORBInitializer() { }

    //
    // IDL to C++ Mapping
    //
    virtual void
    pre_init(PortableInterceptor::ORBInitInfo_ptr info) 
        throw (CORBA::SystemException) { }

    
    //
    // Note: the interceptors are registered in post_init()
    //       if their constructors require
    //       ORBInitInfo::resolve_initial_reference(), which XXX
    //       cannot be called in pre_init().
    //
    virtual void
    post_init(PortableInterceptor::ORBInitInfo_ptr info) 
        throw (CORBA::SystemException)
    {

        //
        // Create and register the interceptors
        //
        
        PortableInterceptor::ServerRequestInterceptor_var server_interceptor
        = new Cdmw::TestServerInterceptor( m_expected_event_count, m_method.c_str()
                                           , m_where.c_str(), m_process.c_str());
        
        try {
            info->add_server_request_interceptor(server_interceptor.in());
        }
        catch(const PortableInterceptor::ORBInitInfo::DuplicateName&) {
            // can't happened while interceptor is not named!
            CDMW_ASSERT(false);
        }
        
    }
    
private:
    
    int m_expected_event_count;
    std::string m_method;
    std::string m_where;
    std::string m_process;
    int m_sup_pid;
};




int main(int argc, char* argv[])
{
    using namespace Cdmw::PlatformMngt;
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::Common;

    int ret_code = 0;

    // initialise log manager
    DaemonLogMngr::initialise();
    
    // checks the arguments
    long specific_port = -1;
    std::string xml_conf;
    bool validate = false;
    std::string fault_manager_corbaloc = "";
    

    std::string method_to_intercept;
    std::string where;
    long count = 0;

    // check if daemon has been started by cdmw_platform_start_daemon.sh
    std::string isStartedByScript_value =
    OS::get_option_value(argc, argv, STARTED_BY_SCRIPT_CHECKING_OPTION);
    if (isStartedByScript_value == "no") {
        std::cout << "You should not directly use " 
                  << argv[0] << "." 
                  << std::endl;
        std::cout << "Use " 
                  << CDMW_START_DAEMON_SCRIPT 
                  << " instead." 
                  << std::endl;
        ::exit(EXIT_FAILURE);
    }
 
    
    if (argc > 2 )
    {

        // checks the number of options
        if (argc > 10)
        {
            LogMngr::logMessage(FTL, MSG_ID_DAEMON_INVALID_OPTIONS);
            print_syntax(argv[0]);
            ::exit(EXIT_FAILURE);
        }

        // checks the port option
        std::string port_option =
        OS::get_option_value(argc, 
                             argv, 
                             Options::LOCALISATION_SERVICE_OPTION);

        if (port_option != "no")
        {
            if (port_option != "yes")
            {
                specific_port = ::atoi(port_option.c_str());

                if (specific_port <= 0)
                {
                    LogMngr::logMessage(FTL, 
                                        MSG_ID_DAEMON_INVALID_PORT, 
                                        port_option);
                    ::exit(EXIT_FAILURE);
                }
            }
            else
            {
                LogMngr::logMessage(FTL, MSG_ID_DAEMON_PORT_NOT_SPECIFIED);
                ::exit(EXIT_FAILURE);
            }

        }

        // checks the xml configuration file option
        std::string xml_conf_option =
        OS::get_option_value( argc, 
                              argv, 
                              Options::PROCESS_XML_FILE_OPTION);

        if (xml_conf_option != "no")
        {
            // if the port option is used, xml option is not allowed
            if (port_option != "no")
            {
                LogMngr::logMessage(FTL, MSG_ID_DAEMON_INVALID_OPTIONS);
                print_syntax(argv[0]);

                ::exit(EXIT_FAILURE);
            }
            
            if (xml_conf_option != "yes")
            {
                if (OS::file_exists(xml_conf_option))
                {
                    xml_conf = xml_conf_option;  		

                    // checks the optional validate option
                    std::string validateXML_option =
                    OS::get_option_value( 
                                         argc, 
                                         argv, 
                                         Cdmw::Common::Options::VALIDATE_XML_OPTION);

                    if (validateXML_option != "no")
                    {
                        validate = true;
                    }

                }
                else
                {
                    LogMngr::logMessage(FTL, 
                                        MSG_ID_DAEMON_CONF_FILE_NOT_FOUND,
                                        xml_conf_option);
                    ::exit(EXIT_FAILURE);
                }
                
            }
            else
            {
                LogMngr::logMessage(FTL, 
                                    MSG_ID_DAEMON_CONF_FILE_NOT_SPECIFIED);
                ::exit(EXIT_FAILURE);
            }
        }

        // display the usage
        if (specific_port < 0 && xml_conf.empty())
        {
            LogMngr::logMessage(FTL, MSG_ID_DAEMON_INVALID_OPTIONS);
            print_syntax(argv[0]);

            ::exit(EXIT_FAILURE);
        }

        // checks the FaultManager corbaloc
        std::string FaultManCorbaloc_option =
        OS::get_option_value( argc, argv, FAULT_MANAGER_CORBALOC_OPTION);
        
        if (FaultManCorbaloc_option!= "no")
        {
            if (FaultManCorbaloc_option != "yes")
            {
                fault_manager_corbaloc = FaultManCorbaloc_option;           
            }
            //             else
            //             {
            //                 LogMngr::logMessage(FTL, 
            //                                     MSG_ID_DAEMON_FAULT_MANAGER_NOT_SPECIFIED);
            //                 ::exit(EXIT_FAILURE);
            //             }
            //         }
            //         else
            //         {
            //             LogMngr::logMessage(FTL, 
            //                                 MSG_ID_DAEMON_FAULT_MANAGER_NOT_SPECIFIED);
            //             ::exit(EXIT_FAILURE);
        }

        // checks the xml configuration file option
        method_to_intercept =
        OS::get_option_value( argc, argv, METHOD_TO_INTERCEPT_OPTION);
        std::cout<<"method_to_intercept"<<method_to_intercept<<std::endl;
        if ( method_to_intercept != "no" )
        {
            if ( method_to_intercept == "yes")
            {
                LogMngr::logMessage(FTL, "no parameter for the option --method-to-intercept");
                ::exit(EXIT_FAILURE);
            }
        }
        else
        {
            LogMngr::logMessage(FTL, "no option --method-to-intercept");
            ::exit(EXIT_FAILURE);
        }

        // checks the xml configuration file option
        where =
        OS::get_option_value( argc, argv, WHERE_OPTION);

        if ( where != "no" )
        {
            if ( where == "yes")
            {
                LogMngr::logMessage(FTL, "no parameter for the option --where");
                ::exit(EXIT_FAILURE);
            }
        }
        else
        {
            LogMngr::logMessage(FTL, "no option --where");
            ::exit(EXIT_FAILURE);
        }

        // checks the xml configuration file option
        std::string count_str =
        OS::get_option_value( argc, argv, COUNT_OPTION);
        
        if ( count_str != "no" )
        {
            if ( count_str != "yes")
            {
                count = ::atoi(count_str.c_str());
            }
            else
            {
                LogMngr::logMessage(FTL, "no parameter for the option --count");
                ::exit(EXIT_FAILURE);
            }
        }
        else
        {
            count = 1;
        }
    }


    // The default configuration
    ConfigurationMngr conf(specific_port);

    CORBA::ORB_var orb;

    try
    {

 
        // If an XML configuration file has been transmit, we use it
        if (!xml_conf.empty())
        {
            conf.parse(xml_conf, validate);
        }


#ifdef CDMW_POSIX

        if (conf.getUID() > 0 && conf.getGID() > 0)
        {

            uid_t uid = conf.getUID();
            gid_t gid = conf.getGID();

            OS::set_process_attributes(uid, gid);

        }

#endif


        // If a log file has been specified we create a file to log data
        if (conf.getLogFile() != NULL )
        {
            LogMngr::logMessage(INF, 
                                MSG_ID_DAEMON_LOG_FILE, 
                                conf.getLogFile());

            LogMngr::open(conf.getLogFile());
            // next messages will be logged in the file
        }

#ifdef CDMW_USE_FAULTTOLERANCE  
              
        // Initialise FT service
        Cdmw::FT::FTServiceInit::Init( argc, argv, true );
        
#endif
        std::string host_name = OS::get_hostname();
        PortableInterceptor::ORBInitializer_var initializer
        = new TestORBInitializer(count, method_to_intercept.c_str(), where.c_str(), host_name.c_str());
        PortableInterceptor::register_orb_initializer(initializer.in());

        // initialises the ORB

        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        //strategyList.add_PoaThreadPerRequest();
        strategyList.add_PoaThreadPool(POA_THREAD_POOL_SIZE);
        strategyList.add_multicast();
   
        std::string str_opt(argv[0]);
        str_opt += " ";
        //str_opt += "-ORBDebug -ORBDebugLevel 10 ";
        str_opt += Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION;
        str_opt += "=";
        str_opt += conf.getSystemPortString();

        char** argv__ = Cdmw::Common::String::to_char_array
        ( Cdmw::Common::String::to_strings(str_opt) );
        int argc__(2);
        //int argc__(4);
        
        orb = Cdmw::OrbSupport::OrbSupport::ORB_init( argc__, 
                                                      argv__, 
                                                      strategyList);
        for (int i=0; i < argc__; ++i)
        {
            delete[] argv__[i];
        }
        delete[] argv__;
    
        CORBA::Object_var poaObj = 
        orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var poa = 
        PortableServer::POA::_narrow(poaObj.in());



#ifdef CDMW_USE_FAULTTOLERANCE

        TimeBase::TimeT duration_time = 0;

        TAO_ORB_Core::set_endpoint_selector_factory (
                                                     "FT_Endpoint_Selector_Factory");
        
        orb->_tao_ft_client_id(ClientIDGenerator::generate_daemon_client_id(
                                                                            OS::get_hostname()).c_str());

        CORBA::Object_var obj;
        obj = orb->resolve_initial_references("ORBPolicyManager");

        CORBA::PolicyManager_var policy_manager 
        = CORBA::PolicyManager::_narrow(obj.in());

        // Create and apply an ORB-wide Routed policy        
        duration_time = conf.getRequestDurationTime();   
        
        if (duration_time == 0)
        {
            CDMW_THROW2(ConfigurationException,
                        "no request duration time defined", 
                        __LINE__);
        }     

        CORBA::Any any;
        any <<= duration_time;
        CORBA::PolicyList policies(1);
        policies.length(1);
        policies[0] =
        orb->create_policy(::FT::REQUEST_DURATION_POLICY, any);
        
        policy_manager->set_policy_overrides(policies, CORBA::SET_OVERRIDE);
        
#else
        size_t duration_time = 0;
#endif

        // Activate the POA manager
        PortableServer::POAManager_var manager = poa->the_POAManager();
        manager->activate();


        //create service registration implementation
        ServiceRegistration_impl* service_registration_impl 
        = new ServiceRegistration_impl(orb.in(),
                                       poa.in(),
                                       conf.getLocationServiceAddress(),
                                       conf.getRegistrationServiceAddress(),
                                       conf.getLocationServiceRetries(),
                                       conf.getLocationServiceTimeInterval());


#ifdef CDMW_USE_FAULTTOLERANCE

        // set orb arguments for the monitoring library
        if (conf.getPullCorbaloc() == "no_corbaloc")
        {
            CDMW_THROW2(ConfigurationException,
                        "no simple pull corbaloc defined", 
                        __LINE__);
        }
        if (conf.getPushCorbaloc() == "no_corbaloc")
        {
            CDMW_THROW2(ConfigurationException,
                        "no simple pull corbaloc defined", 
                        __LINE__);
        }
        
        std::string str_opt_mt("");
        str_opt_mt += "--SIMPLE_PULL_CORBALOC ";
        str_opt_mt += conf.getPullCorbaloc();        
        str_opt_mt += " --SIMPLE_PUSH_CORBALOC ";
        str_opt_mt += conf.getPushCorbaloc();        

        char** argv_mt = Cdmw::Common::String::to_char_array
        (Cdmw::Common::String::to_strings(str_opt_mt));
        int argc_mt(4);


        // create monitoring library
        CdmwFT::MonitoringLibrary::MonitoringLibraryInterface_var 
        monitoringLibraryInterface
        = Cdmw::FT::InitUtils::Create_monitoring_library(
                                                         argc_mt, argv_mt, orb.in());  
    
        // create acknowledgingSimpleMonitoring
        CdmwFT::MonitoringLibrary::SimpleMonitoring::AcknowledgingSimpleMonitoring_var acknowledgingSimpleMonitoring 
        = monitoringLibraryInterface->create_acknowledging_simple_monitoring();
        // activate the monitoring 
        acknowledgingSimpleMonitoring->start_monitoring();

        // create SystemMngt proxy

        CORBA::Object_var objref;
             
        Cdmw::PlatformMngt::Proxy::SystemProxy *the_system_proxy_handle = NULL;

        CdmwPlatformMngt::System_var the_system_proxy = 
        CdmwPlatformMngt::System::_nil();
           
        try
        {
            auto_ptr<Cdmw::PlatformMngt::Proxy::AccessorLookupService> 
            lookup_service
            (new Cdmw::PlatformMngt::Proxy::AccessorLookupService_impl(service_registration_impl)); 
        
            the_system_proxy_handle= 
            Cdmw::PlatformMngt::Proxy::InitUtils::create_platformmngt_proxy
            (orb.in(),
             poa.in(),
             CDMW_SYSTEM_MNGT_PROXY,
             lookup_service.get()); /* proxy takes over responsibility
                                       for lookup service object */ 

            lookup_service.release(); 
 
            the_system_proxy = the_system_proxy_handle->getReference();
        }
        catch (const Cdmw::BadParameterException &ex) 
        {
            ret_code = EXIT_FAILURE;

            LogMngr::logMessage(FTL, 
                                MSG_ID_DAEMON_CANNOT_CREATE_SYSTEM_MNGT_PROXY,
                                ex.get_info());

            return ret_code;
        }
        catch (const Cdmw::OutOfMemoryException &ex) 
        {
            ret_code = EXIT_FAILURE;

            LogMngr::logMessage(FTL, 
                                MSG_ID_DAEMON_CANNOT_CREATE_SYSTEM_MNGT_PROXY,
                                ex.get_info());

            return ret_code;
        }

        // publishes the supervision system proxy corbaloc
        Cdmw::OrbSupport::OrbSupport::bind_object_to_corbaloc (
                                                               orb.in(), 
                                                               CdmwPlatformMngt::SUPERVISION_NAME, 
                                                               the_system_proxy.in());

        // register event factories (value type factory) needed by the proxy
        CORBA::ValueFactoryBase_var factory, old_factory;

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

#endif        

        // create ProcessesLauncher
        ProcessLauncher* processLauncher = 
        new ProcessLauncher(conf.getProcessesIdDirectory());

        // creates the daemon servant
        PortableServer::ServantBase_var daemon_servant = 
        new Daemon_impl(orb.in(),
                        conf.getSystemPortString(), 
                        conf.getNotificationCallTimeout(), 
                        conf.getPullInterval(), 
                        conf.getPullTimeout(),
                        processLauncher, 
                        service_registration_impl,
                        duration_time, 
                        fault_manager_corbaloc);

        // create the ProcessKillMngr singleton
        ProcessKillMngr::Init (argc, argv);

        // creates the daemon CORBA object and activates it
        CdmwPlatformMngt::Daemon_var daemon =
        (dynamic_cast<Daemon_impl*>(daemon_servant.in()))->activate();

        // publishes the daemon corbaloc
        Cdmw::OrbSupport::OrbSupport::bind_object_to_corbaloc
        (orb.in(), CdmwPlatformMngt::DAEMON_NAME, daemon.in() );
    
        // writes the daemon IOR in the IOR file
        {
            std::ofstream iorFile(conf.getIorFile());
            std::string ior = orb->object_to_string(daemon.in());
            iorFile << ior;
        }

        // logs the startup info
        LogMngr::logMessage(INF, MSG_ID_DAEMON_STARTUP,
                            conf.getSystemPortString(), conf.getNotificationCallTimeout());

        LogMngr::logMessage(INF, MSG_ID_DAEMON_CORBALOC, OS::get_hostname(),
                            conf.getSystemPortString(), CdmwPlatformMngt::DAEMON_NAME);
        
#ifdef CDMW_USE_FAULTTOLERANCE
        LogMngr::logMessage(INF, MSG_ID_DAEMON_CORBALOC, OS::get_hostname(),
                            conf.getSystemPortString(), CdmwPlatformMngt::SUPERVISION_NAME);
#endif
        LogMngr::logMessage(INF, MSG_ID_DAEMON_IOR_FILE, conf.getIorFile());

        LogMngr::logMessage(INF, MSG_ID_DAEMON_PID_DIR_CREATED,
                            conf.getProcessesIdDirectory());
        
        // runs the ORB loop
        orb->run();
        
#ifdef CDMW_USE_FAULTTOLERANCE

        // deactivate the monitoring 
        acknowledgingSimpleMonitoring->stop_monitoring();

        /* control the moment, the proxy is deactivated (sync or async);
           otherwise the following delete statement would block indefinetly
           until all pending request have been processed */
        the_system_proxy_handle->deactivate(false);

        /* if not deactivated, the proxy is going to be deactivated now with
           "wait_for_completion=true", blocking mode until all pending
           requests have been processed. If the proxy has been deactivated
           before, only the remaining handler will be free-ed,
           without any blocking. */
        delete the_system_proxy_handle;
#endif        
 
	
    }
    catch (const CannotCreatePidDirectory& ex)
    {
        ret_code = EXIT_FAILURE;

        LogMngr::logMessage(FTL, MSG_ID_DAEMON_CANNOT_CREATE_DIR,
                            conf.getProcessesIdDirectory(), ex.get_info());

    }
    catch (const CORBA::COMM_FAILURE& ex)
    {
        ret_code = EXIT_FAILURE;

        LogMngr::logMessage(FTL, MSG_ID_DAEMON_UNAVAILABLE_PORT,
                            conf.getSystemPortString());

    }
    catch ( const ConfigurationException& ex )
    {
        ret_code = EXIT_FAILURE;

        LogMngr::logMessage(FTL, MSG_ID_DAEMON_INVALID_CONF_FILE,
                            xml_conf, ex.getLineNumber(), ex.getMessage()); 
    }

    if (!CORBA::is_nil(orb.in()))
    {
        // stops the ORB
        orb->shutdown(true);
        Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
        orb->destroy();
    }
 
 
    // destro log manager
    LogMngr::destroy();

    return ret_code;

}

