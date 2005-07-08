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
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

#include "Foundation/ossupport/OS.hpp"

#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/String.hpp"
#include "SystemMngt/platformlibrary/ProcessLauncher.hpp"

#include "platformagent/ApplicationAgentLogMngr.hpp"
#include "platformagent/ApplicationAgentLogMessageId.hpp"

#include "platformagent/ApplicationAgent_impl.hpp"

#include "idllib/CdmwPlatformMngtDaemon.stub.hpp"

#ifdef CDMW_USE_FAULTTOLERANCE
#include "SystemMngt/platformlibrary/ClientIDGenerator.hpp"
#include "FaultTolerance/ftinit/FTServiceInit.hpp"
#include <tao/ORB_Core.h>
#endif 

using namespace Cdmw;
using namespace Cdmw::PlatformMngt;

const int POA_THREAD_POOL_SIZE = 5;    

class BadConfiguration : public Exception
{
    
    public:
        BadConfiguration() : Exception(Exception::FATAL, "Bad configuration") {}
};


void print_syntax(char* exe)
{
    std::cout << "Syntax is :" << std::endl << exe;
    std::cout << " " << Common::Options::CALLBACK_IOR_OPTION << "=IOR";
#ifdef CDMW_USE_FAULTTOLERANCE
    std::cout << " [" << Cdmw::Common::Options::REQUEST_DURATION_TIME << "=time]"; 
#endif
    std::cout << " [" << Common::Options::LOCALISATION_SERVICE_OPTION << "=PORT]";
    std::cout << " [" << Common::Options::LOG_FILE_OPTION << "=full_path_to_log_file]";
    std::cout << " [" << ProcessLauncher::PIDS_DIRECTORY_OPTION << "=directory_to_save_pids_of_launched_processes";
    std::cout << std::endl;
}


void activateRootPOAManager(CORBA::ORB_ptr orb)
{


    CORBA::Object_var poaObj = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(poaObj.in());
    
    // Activate the POA manager
    PortableServer::POAManager_var manager = poa->the_POAManager();
    manager->activate();

}


int main(int argc, char* argv[])
{

    // the return code 
    int ret_code = 0;


    std::string callbackIor;
    long specific_port = -1;
#ifdef CDMW_USE_FAULTTOLERANCE
    size_t duration_time=20000000;
#endif
    std::string configurationErrorMsg;
    
    // initialise log manager
    ApplicationAgentLogMngr::initialise();


    // except for the callback argument, errors are reported
    // to the daemon via CORBA


    // checks the log file option
    std::string logFile;
    std::string logFile_option =
        OsSupport::OS::get_option_value(argc, argv, Common::Options::LOG_FILE_OPTION);

    if (logFile_option != "no")
    {
        if (logFile_option != "yes")
        {
            logFile = logFile_option; 
        }
        else
        {
            configurationErrorMsg =
                LogMngr::getFormattedMessage(MSG_ID_AGENT_LOG_FILE_NOT_SPECIFIED);

            LogMngr::logMessage(FTL, configurationErrorMsg.c_str());
        }
    }


    // If a log file has been specified, use it
    if (!logFile.empty())
    {
        LogMngr::logMessage(INF, MSG_ID_AGENT_LOG_FILE, logFile);

        LogMngr::open(logFile);
        // next messages will be logged in the file
    }


    // checks the mandatory callback argument
    std::string callbackIor_option =
        OsSupport::OS::get_option_value(argc, argv, Common::Options::CALLBACK_IOR_OPTION);

    if (callbackIor_option == "no" || callbackIor_option == "yes")
    {
        LogMngr::logMessage(FTL, MSG_ID_AGENT_CALLBACK_NOT_SPECIFIED);
        print_syntax(argv[0]);

        ::exit(EXIT_FAILURE);
    }
    else
    {
        callbackIor = callbackIor_option;
    }


    // checks the port option
    std::string port_option =
        OsSupport::OS::get_option_value(argc, argv, Common::Options::LOCALISATION_SERVICE_OPTION);

    if (port_option != "no")
    {
        if (port_option != "yes")
        {
            specific_port = ::atoi(port_option.c_str());

            if (specific_port <= 0)
            {
                configurationErrorMsg =
                    LogMngr::getFormattedMessage(MSG_ID_AGENT_INVALID_PORT, port_option)
                    + "\n" + configurationErrorMsg;
            }
        }
        else
        {
            configurationErrorMsg =
                LogMngr::getFormattedMessage(MSG_ID_AGENT_PORT_NOT_SPECIFIED)
                + "\n" + configurationErrorMsg;
        }
    }


    // check the pids directory option
    ProcessLauncher* processLauncher = NULL;
    std::string pidsDirectory =
        OsSupport::OS::get_option_value(argc, argv, ProcessLauncher::PIDS_DIRECTORY_OPTION);
    
    if (pidsDirectory != "no") {
        try {
            processLauncher = new ProcessLauncher(pidsDirectory.c_str());
        } catch (const CannotCreatePidDirectory& ex) {
            configurationErrorMsg =
                LogMngr::getFormattedMessage(MSG_ID_AGENT_CANNOT_CREATE_PID_DIR, 
                                             pidsDirectory, ex.get_info())
                + "\n" + configurationErrorMsg;
        }
    }
    
    
#ifdef CDMW_USE_FAULTTOLERANCE
    //check the request duration time option
    std::string request_duration_time_value =
        OsSupport::OS::get_option_value(argc, argv, Cdmw::Common::Options::REQUEST_DURATION_TIME);
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

    
    CORBA::ORB_var orb;
    
    PortableServer::ServantBase_var agent_servant;
    ApplicationAgent_impl *agent_imp = 0;

    try
    {

        // initialises the ORB

        OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        //strategyList.add_PoaThreadPerRequest();
        strategyList.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        if (specific_port < 0)
        {
            // no specific port
            orb = OrbSupport::OrbSupport::ORB_init( argc, argv, strategyList);
            activateRootPOAManager(orb.in());
        }
        else
        {

            try
            {
                // nothing special to do :
                // the specific port is already on the command line
                orb = OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);
                activateRootPOAManager(orb.in());
            }

#if CDMW_ORB_VDR == tao
            catch( const CORBA::BAD_PARAM& ex ) 
#elif CDMW_ORB_VDR == orbacus
            catch (const CORBA::COMM_FAILURE& ex)
#endif
            {
                // the specific port is already used
                configurationErrorMsg =
                    LogMngr::getFormattedMessage(MSG_ID_AGENT_UNAVAILABLE_PORT, specific_port)
                    + "\n" + configurationErrorMsg;


                // tries to reinitialise the ORB with a default port in order
                // to report the problem
                std::string str_opt(argv[0]);

                char** argv__ = Cdmw::Common::String::to_char_array
                    (Cdmw::Common::String::to_strings(str_opt));
                int argc__(1);

                try
                {
#if CDMW_ORB_VDR == orbacus
                    orb->shutdown(true);
		            OrbSupport::OrbSupport::ORB_cleanup(orb.in());
                    orb->destroy();
#endif

                    orb = CORBA::ORB_init(argc__, argv__);
                    // activation of the root POA's manager is
                    // unnecessary as we only want to report the problem
                }
                catch(...)
                {
                    configurationErrorMsg =
                        LogMngr::getFormattedMessage(MSG_ID_AGENT_CANNOT_REPORT_PROBLEM)
                        + "\n" + configurationErrorMsg;

                    // logs all errors
                    LogMngr::logMessage(FTL, configurationErrorMsg.c_str());

                    CDMW_THROW(BadConfiguration);
                }

                for (int i=0; i < argc__; ++i)
                {
                    delete[] argv__[i];
                }
                delete[] argv__;

            }

        }

        // gets the acknowledgement callback
        CdmwPlatformMngt::ApplicationAgentCallback_var callback;
        try
        {
            CORBA::Object_var callback_obj = orb->string_to_object(callbackIor.c_str());

            callback = CdmwPlatformMngt::ApplicationAgentCallback::_narrow(callback_obj.in());

            if (CORBA::is_nil(callback.in()))
            {
                throw CORBA::BAD_PARAM();
            }

        }
        catch(const CORBA::BAD_PARAM&)
        {
            // the acknowledgement callback is invalid
            configurationErrorMsg =
                LogMngr::getFormattedMessage(MSG_ID_AGENT_INVALID_CALLBACK)
                + "\n" + configurationErrorMsg;

            // logs all errors
            LogMngr::logMessage(FTL, configurationErrorMsg.c_str());

            CDMW_THROW(BadConfiguration);
        }


        // reports all errors to the daemon if any
        if (!configurationErrorMsg.empty())
        {
            LogMngr::logMessage(FTL, configurationErrorMsg.c_str());
            callback->notify_error(configurationErrorMsg.c_str());
            CDMW_THROW(BadConfiguration);
        }


        // creates the application agent servant
        agent_servant =
              new ApplicationAgent_impl(orb.in(), processLauncher);

        // creates the application agent CORBA object and activates it
        agent_imp =
            dynamic_cast<ApplicationAgent_impl*>(agent_servant.in());

        CdmwPlatformMngt::ApplicationAgent_var agent = agent_imp->activate();

        // acknowledges to the daemon
        CdmwPlatformMngt::ApplicationAgentAck_var ack =
            callback->set_ready(agent.in());

        std::string applicationName = (ack->application_name).in();

        // ends the creation of the application agent
        if (!agent_imp->endCreation(ack.in()))
        {
            CDMW_THROW(BadConfiguration);
        }

        // get agent processId
        OsSupport::OS::ProcessId processId = OsSupport::OS::get_processId();

        // logs the startup info
        if (specific_port > 0)
        {
            LogMngr::logMessage(INF, MSG_ID_AGENT_STARTUP_WITH_PORT, applicationName, 
                                specific_port, processId);
        }
        else
        {
            LogMngr::logMessage(INF, MSG_ID_AGENT_STARTUP, applicationName, processId);
        }

        if (processLauncher != NULL) {
            LogMngr::logMessage(INF, MSG_ID_AGENT_PID_DIR_CREATED, 
                                applicationName, processLauncher->getPidDirectory());
        }
        
        
#ifdef CDMW_USE_FAULTTOLERANCE
        TAO_ORB_Core::set_endpoint_selector_factory ("FT_Endpoint_Selector_Factory");
        
        orb->_tao_ft_client_id(Cdmw::PlatformMngt::ClientIDGenerator::generate_agent_client_id(OsSupport::OS::get_hostname(), applicationName).c_str());
        
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
        // runs the ORB loop
        orb->run();
	
    }
    catch (const CORBA::Exception& ex)
    {
        ret_code = EXIT_FAILURE;
        LogMngr::logMessage(FTL, MSG_ID_AGENT_UNEXPECTED_CORBA_EXCEPTION, ex._name());
	}
    catch (const BadConfiguration&)
    {
        ret_code = EXIT_FAILURE;
	}

    if (ret_code == EXIT_FAILURE)
    {
        if (agent_imp != 0)
        {
            agent_imp->set_init_exit_failure();
        }        
    }
    
    
    if (!CORBA::is_nil(orb.in()))
    {
        // stops the ORB
        orb->shutdown(true);
	    OrbSupport::OrbSupport::ORB_cleanup(orb.in());
        orb->destroy();
    }
    
    // destroy log manager
    LogMngr::destroy();

    return ret_code;

}

