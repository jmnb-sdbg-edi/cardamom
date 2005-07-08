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

#include "platformdaemon/DaemonLogMngr.hpp"
#include "platformdaemon/DaemonLogMessageId.hpp"

#include "platformdaemon/ConfigurationMngr.hpp"

#include "platformdaemon/Daemon_impl.hpp"

#include "SystemMngt/platformlibrary/ProcessLauncher.hpp"

#ifdef CDMW_USE_FAULTTOLERANCE
#include "SystemMngt/platformlibrary/ClientIDGenerator.hpp"
#include "FaultTolerance/ftmonitoring/InitUtils.hpp"
#include "FaultTolerance/ftinit/FTServiceInit.hpp"
#include "FaultTolerance/idllib/FT.stub.hpp"
#include "FaultTolerance/ftmonitoring/MonitoringLibraryInterface_impl.hpp"
#include <tao/ORB_Core.h>
#endif

const int POA_THREAD_POOL_SIZE = 5;    

// Daemon must be launched by the following script.
const char* CDMW_START_DAEMON_SCRIPT="cdmw_platform_daemon.sh";

// This option is to check if daemon has been started by the script
// (only the script should use it !!) 
const char* STARTED_BY_SCRIPT_CHECKING_OPTION="--isStartedByScript";

void print_syntax(char* exe)
{
    std::cout << "Syntax is :" << std::endl << exe;
    std::cout << " [ " << Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION << "=PORT";
    std::cout << " | " << Cdmw::Common::Options::PROCESS_XML_FILE_OPTION << "=XMLConfigFile";
    std::cout << " [" << Cdmw::Common::Options::VALIDATE_XML_OPTION << "] ]";
    std::cout << std::endl;
}


int main(int argc, char* argv[])
{
    using namespace Cdmw::PlatformMngt;
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::OrbSupport;
    using namespace Cdmw::Common;
    
    // the return code 
    int ret_code = 0;

    // initialise log manager
    DaemonLogMngr::initialise();
    
    // checks the arguments
    long specific_port = -1;
    std::string xml_conf;
    bool validate = false;
    // check if daemon has been started by cdmw_platform_start_daemon.sh
    std::string isStartedByScript_value =
        OS::get_option_value(argc, argv, STARTED_BY_SCRIPT_CHECKING_OPTION);
    if (isStartedByScript_value == "no") {
        std::cout << "You should not directly use " << argv[0] << "." << std::endl;
        std::cout << "Use " << CDMW_START_DAEMON_SCRIPT << " instead." << std::endl;
        ::exit(EXIT_FAILURE);
    }
 
    
    if (argc > 2 )
    {

        // checks the number of options
        if (argc > 5)
        {
            LogMngr::logMessage(FTL, MSG_ID_DAEMON_INVALID_OPTIONS);
            print_syntax(argv[0]);
            ::exit(EXIT_FAILURE);
        }

        // checks the port option
        std::string port_option =
            OS::get_option_value(argc, argv, Options::LOCALISATION_SERVICE_OPTION);

        if (port_option != "no")
        {
            // if the port option is used, no other options is allowed
            if (argc > 4)
            {
                LogMngr::logMessage(FTL, MSG_ID_DAEMON_INVALID_OPTIONS);
                print_syntax(argv[0]);

                ::exit(EXIT_FAILURE);
            }

            if (port_option != "yes")
            {
                specific_port = ::atoi(port_option.c_str());

                if (specific_port <= 0)
                {
                    LogMngr::logMessage(FTL, MSG_ID_DAEMON_INVALID_PORT, port_option);
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
            OS::get_option_value( argc, argv, Options::PROCESS_XML_FILE_OPTION);

        if ( xml_conf_option != "no" )
        {
            if (xml_conf_option != "yes")
            {
                if (OS::file_exists(xml_conf_option))
                {
                    xml_conf = xml_conf_option;  		

                    // checks the optional validate option
                    std::string validateXML_option =
                        OS::get_option_value( argc, argv, Cdmw::Common::Options::VALIDATE_XML_OPTION);

                    if (argc == 5 && validateXML_option == "no")
                    {
                        LogMngr::logMessage(FTL, MSG_ID_DAEMON_INVALID_OPTIONS);

                        print_syntax(argv[0]);

                        ::exit(EXIT_FAILURE);
                    }

                    if (validateXML_option != "no")
                    {
                        validate = true;
                    }

                }
                else
                {
                    LogMngr::logMessage(FTL, MSG_ID_DAEMON_CONF_FILE_NOT_FOUND, xml_conf_option);
                    ::exit(EXIT_FAILURE);
                }
                
            }
            else
            {
                LogMngr::logMessage(FTL, MSG_ID_DAEMON_CONF_FILE_NOT_SPECIFIED);
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
            LogMngr::logMessage(INF, MSG_ID_DAEMON_LOG_FILE, conf.getLogFile());

	        LogMngr::open(conf.getLogFile());
            // next messages will be logged in the file
	    }

#ifdef CDMW_USE_FAULTTOLERANCE  
              
        // Initialise FT service
        Cdmw::FT::FTServiceInit::init( argc, argv, true );
        
#endif

        // initialises the ORB

        StrategyList strategyList;
        strategyList.add_OrbThreaded();
        //strategyList.add_PoaThreadPerRequest();
        strategyList.add_PoaThreadPool(POA_THREAD_POOL_SIZE);
#ifdef CDMW_USE_FAULTTOLERANCE  
        strategyList.add_multicast();
#endif
   
        std::string str_opt(argv[0]);
        str_opt += " ";
        str_opt += Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION;
        str_opt += "=";
        str_opt += conf.getSystemPortString();

        char** argv__ = Cdmw::Common::String::to_char_array
            ( Cdmw::Common::String::to_strings(str_opt) );
        int argc__(2);
        
        orb = OrbSupport::OrbSupport::ORB_init( argc__, argv__, strategyList);
        for (int i=0; i < argc__; ++i)
        {
            delete[] argv__[i];
        }
        delete[] argv__;
    
        CORBA::Object_var poaObj = orb -> resolve_initial_references("RootPOA");
        PortableServer::POA_var poa = PortableServer::POA::_narrow(poaObj.in());



#ifdef CDMW_USE_FAULTTOLERANCE
        TimeBase::TimeT duration_time = 0;

        TAO_ORB_Core::set_endpoint_selector_factory ("FT_Endpoint_Selector_Factory");
        
        orb->_tao_ft_client_id(Cdmw::PlatformMngt::ClientIDGenerator::generate_daemon_client_id(OS::get_hostname()).c_str());

        CORBA::Object_var obj;
        obj = orb->resolve_initial_references("ORBPolicyManager");

        CORBA::PolicyManager_var policy_manager 
            = CORBA::PolicyManager::_narrow(obj.in());

        // Create and apply an ORB-wide Routed policy        
        duration_time = conf.getRequestDurationTime();   
        
        if (duration_time == 0)
        {
            CDMW_THROW2(ConfigurationException,"no request duration time defined", __LINE__);
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

#ifdef CDMW_USE_FAULTTOLERANCE
        // set orb arguments for the monitoring library
	    if (conf.getPullCorbaloc() == "no_corbaloc")
        {
            CDMW_THROW2(ConfigurationException,"no simple pull corbaloc defined", __LINE__);
        }
	    if (conf.getPushCorbaloc() == "no_corbaloc")
        {
            CDMW_THROW2(ConfigurationException,"no simple pull corbaloc defined", __LINE__);
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
        CdmwFT::MonitoringLibrary::MonitoringLibraryInterface_var monitoringLibraryInterface
            = Cdmw::FT::InitUtils::create_monitoring_library(argc_mt, argv_mt, orb.in());  
    
        // create acknoledgingSimpleMonitoring
        CdmwFT::MonitoringLibrary::SimpleMonitoring::AcknowledgingSimpleMonitoring_var acknoledgingSimpleMonitoring 
            = monitoringLibraryInterface->create_acknowledging_simple_monitoring();
        // activate the monitoring 
       acknoledgingSimpleMonitoring->start_monitoring();
#endif        



        // create ProcessesLauncher
        ProcessLauncher* processLauncher = new ProcessLauncher(conf.getProcessesIdDirectory());

        // creates the daemon servant
        PortableServer::ServantBase_var daemon_servant = new Daemon_impl(orb.in(),
                conf.getSystemPortString(), conf.getNotificationCallTimeout(), processLauncher, duration_time);

        // creates the daemon CORBA object and activates it
        CdmwPlatformMngt::Daemon_var daemon =
            (dynamic_cast<Daemon_impl*>(daemon_servant.in()))->activate();

        // publishes the daemon corbaloc
        OrbSupport::OrbSupport::bind_object_to_corbaloc
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

        LogMngr::logMessage(INF, MSG_ID_DAEMON_IOR_FILE, conf.getIorFile());

        LogMngr::logMessage(INF, MSG_ID_DAEMON_PID_DIR_CREATED,
            conf.getProcessesIdDirectory());
        
        // runs the ORB loop
        orb->run();
        
#ifdef CDMW_USE_FAULTTOLERANCE
        // deactivate the monitoring 
        acknoledgingSimpleMonitoring->stop_monitoring();
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
        OrbSupport::OrbSupport::ORB_cleanup(orb.in());
        orb->destroy();
     }
 
 
    // destro log manager
    LogMngr::destroy();

    return ret_code;

}

