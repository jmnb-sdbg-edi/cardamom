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


#include "platformagent/ApplicationAgent_impl.hpp"
#include "platformagent/ProcessCallback_impl.hpp"
#include "platformdaemonlibrary/DaemonLogMessageId.hpp"
#include "platformagent/ProcessDelegateWrapper_impl.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Options.hpp"

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "Foundation/ossupport/ProcessCallback.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/osthreads/SchedulerBase.hpp"

#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"

#include "SystemMngt/platformlibrary/EndingCallback.hpp"

#include "SystemMngt/platformlibrary/ProcessLauncher.hpp"
#include "SystemMngt/platformlibrary/ProcessKillMngr.hpp"

#include "idllib/CdmwPlatformMngtProcessObserver.stub.hpp"
#include "idllib/CdmwPlatformMngtProcessCallback.stub.hpp"
#include "idllib/CdmwPlatformMngtProcessDelegateWrapper.stub.hpp"



namespace Cdmw
{
namespace PlatformMngt
{

/**
*Implementation notes: none 
*<p>
*Portability issues: none 
*<p>
*/


class UnmanagedProcessCreationCallback : public OsSupport::ProcessCallback
{
public:

    UnmanagedProcessCreationCallback(ApplicationAgent_impl* agent, 
                                     const std::string& process_name)
        : m_agent(agent), m_process_name(process_name)
    {
    }

    virtual ~UnmanagedProcessCreationCallback() { }

    void execute(OsSupport::OS::ProcessId processId) throw()
    {
        // Add process in map (write locked)
        m_agent->fill_created_process (m_process_name, processId);
        
        // register the process to killer
        ProcessKillMngr::Instance().register_process (m_agent->m_application_name.c_str(),
                                                      m_process_name.c_str(), processId);

        LogMngr::logMessage(INF, MSG_ID_AGENT_PROCESS_CREATED, m_process_name);
    }

private:
    ApplicationAgent_impl *m_agent;
    std::string m_process_name;

};


class ProcessEndingCallback : public EndingCallback
{
public:

    ProcessEndingCallback(ApplicationAgent_impl* agent)
        : m_agent(agent),
          m_local_host_name(OsSupport::OS::get_hostname())
    {
    }



    virtual ~ProcessEndingCallback() throw()
    { 
        // associated agent can be deactivated and destroyed by orb
        const char* application_name = m_agent->m_application_name.c_str();
        
        m_agent->deactivate(application_name);
    }
    
    

    void execute(OsSupport::OS::ProcessId processId) throw()
    {
        try
        {
            std::string process_name = m_agent->reset_process_info_by_end(processId);

            if (!process_name.empty())
            {
                LogMngr::logMessage(INF, MSG_ID_AGENT_PROCESS_ENDED, process_name.c_str());

                CdmwPlatformMngtBase::TimeStamp time_stamp =
                                 EventHeaderFactory::createTimeStamp();
                    
#ifdef CDMW_USE_FAULTTOLERANCE

                if (!CORBA::is_nil((m_agent->m_fault_notifier).in()))
                {
                    ::FT::Location location;
              
                    location.length(3);
                    location[0].kind = CORBA::string_dup ("hostname");
                    location[0].id = CORBA::string_dup (m_local_host_name.c_str());
                    location[1].kind = CORBA::string_dup ("applicationname");
                    location[1].id = CORBA::string_dup (m_agent->m_application_name.c_str());
                    location[2].kind = CORBA::string_dup ("processname");
                    location[2].id = CORBA::string_dup (process_name.c_str());
                    
                    // Prepare the event for the fault notification
                    CosNotification::StructuredEvent fault_event;
                    
                    fault_event.header.fixed_header.event_type.domain_name = CORBA::string_dup ("FT_CORBA");
                    fault_event.header.fixed_header.event_type.type_name = CORBA::string_dup ("ObjectCrashFault");
                    
                    fault_event.filterable_data.length (3);
                    fault_event.filterable_data[0].name = CORBA::string_dup ("FTDomainId");
                    fault_event.filterable_data[0].value <<= "thalesgroup.com";
                    fault_event.filterable_data[1].name = CORBA::string_dup ("Location");
                    fault_event.filterable_data[1].value <<= location;
                    CdmwPlatformMngtBase::TimeStamp time_stamp =
                    PlatformMngt::EventHeaderFactory::createTimeStamp();
                    fault_event.filterable_data[2].name = CORBA::string_dup( "TimeStamp" );
                    fault_event.filterable_data[2].value <<= time_stamp;                

                    // notify the FT notifier
                    try
                    {
                        (m_agent->m_fault_notifier)->push_structured_fault (fault_event);
                    }
                    catch (CORBA::SystemException& e)
                    {
                        // std::cerr << "ApplicationAgent_impl push_structured_fault calling exception : " 
                        //           << e._name() << std::endl;
                    }
                    catch (...)
                    {
                        // nothing to do
                    }
                    
                }
#else

                // notify the supervision process observer
                (m_agent->m_process_observer)->notify_ending (
                           m_agent->m_application_name.c_str(), 
                           process_name.c_str(), 
                           m_local_host_name.c_str(), 
                           time_stamp, 0); 
#endif

                
            }
            
            // remove the process from killer
            ProcessKillMngr::Instance().remove_process (processId);
                
        }
        catch(const std::bad_alloc&)
        {
            LogMngr::logMessage(ERR, MSG_ID_AGENT_NO_MEMORY);
        }
        catch(const CORBA::SystemException& ex)
        {
            std::cout<<ex._info().c_str()<<std::endl;            
        }
        catch(...)
        {
            LogMngr::logMessage(ERR, MSG_ID_AGENT_INTERNAL);            
        }
    }
    
private:

    /**
    * The application agent.
    */
    ApplicationAgent_impl *m_agent;
    
    /**
     * The name of the host
     */
    std::string m_local_host_name;
};


class UnmanagedProcessCreationRun : public Cdmw::OsSupport::Runnable
{
public:

    UnmanagedProcessCreationRun (ApplicationAgent_impl* agent,
                                 ProcessLauncher* process_launcher,                               
                                 ProcessEndingCallback *process_ending_callback,
                                 const std::string& application_name,
                                 const std::string& process_name,
                                 const CdmwPlatformMngt::ProcessInfo& process_info)
       : m_agent(agent),
         m_process_launcher(process_launcher),
         m_process_ending_callback(process_ending_callback),
         m_application_name(application_name),
         m_process_name(process_name),
         m_process_info (process_info)
    {
    }

    virtual ~UnmanagedProcessCreationRun (void) throw()
    {
    }
    

    virtual void run() throw()
    {
        try
        {      
            // creates the arguments for the OS process
            std::string args;

            if (m_process_info.process_port > 0)
            {
                args += Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION;
                args += "=";
                std::ostringstream port_str;
                port_str << m_process_info.process_port;
                args += port_str.str();
            }

            args += " ";
            args += m_process_info.process_args.in();

            OsSupport::OS::ProcessId processID;

            // creates the OS process
            LogMngr::logMessage(INF, MSG_ID_AGENT_CREATE_PROCESS, m_process_name);
                
            UnmanagedProcessCreationCallback processCreationCallback(m_agent, m_process_name);

            if (m_process_launcher != NULL) 
            {
                processID = m_process_launcher->create_process(
                        m_process_name,
                        m_process_info.process_exe.in(),
                        args,
                        m_process_info.working_directory.in(),
                        &processCreationCallback, m_process_ending_callback);

            } 
            else 
            {
                processID = OsSupport::OS::create_process(
                        m_process_info.process_exe.in(),
                        args,
                        m_process_info.working_directory.in(),
                        &processCreationCallback, m_process_ending_callback);
            }
        }
        catch (const BadParameterException& e)
        { // We catch the BadParameterException which is, normaly, only (re)thrown after create_process
            LogMngr::logMessage(WRN, MSG_ID_AGENT_INVALID_PROCESS_INFO, m_process_name,
                                e.m_parameterValue);   

            // fill process with error info in map (write locked)
            m_agent->fill_faulty_process (m_process_name,
                                          ::CdmwPlatformMngt::PROCESS_INVALID_INFO, 
                                          "", (e.m_parameterValue).c_str());
        }
        catch(const std::bad_alloc&)
        {
            LogMngr::logMessage(ERR, MSG_ID_AGENT_NO_MEMORY);
            
            // fill process with exception info in map (write locked)
            m_agent->fill_faulty_exception_process (m_process_name,
                                                    ApplicationAgent_impl::NO_MEMORY_RAISED);
        }
        catch(const OutOfMemoryException&)
        {
            LogMngr::logMessage(ERR, MSG_ID_AGENT_NO_MEMORY);
            
            // fill process with exception info in map (write locked)
            m_agent->fill_faulty_exception_process (m_process_name,
                                                    ApplicationAgent_impl::NO_MEMORY_RAISED);
        }
        catch(const InternalErrorException& ex)
        {
            LogMngr::logMessage(ERR, MSG_ID_AGENT_INTERNAL, ex.get_info());
            
            // fill process with exception info in map (write locked)
            m_agent->fill_faulty_exception_process (m_process_name,
                                                    ApplicationAgent_impl::INTERNAL_RAISED);
        }
        catch(...)
        {
            LogMngr::logMessage(ERR, MSG_ID_AGENT_INTERNAL, "Unexpected exception");
            
            // fill process with exception info in map (write locked)
            m_agent->fill_faulty_exception_process (m_process_name,
                                                    ApplicationAgent_impl::INTERNAL_RAISED);
        }
    }
    
private:

    /**
    * The application agent.
    */
    ApplicationAgent_impl *m_agent;
    
    /**
     * The ProcessLauncher
     */
    ProcessLauncher* m_process_launcher;
    
    /**
    * The process ending callback.
    */
    ProcessEndingCallback *m_process_ending_callback;
    
    /**
    * The name of the application.
    */
    std::string m_application_name;
    
    /**
    * The name of the process.
    */
    std::string m_process_name;
    
    /**
    * The process info.
    */
    CdmwPlatformMngt::ProcessInfo m_process_info;

};

class ManagedProcessCreationRun : public Cdmw::OsSupport::Runnable
{
public:

    ManagedProcessCreationRun (CORBA::ORB_ptr orb,
                               PortableServer::POA_ptr root_poa,
                               ApplicationAgent_impl* agent,
                               ProcessMessageBroker_impl* process_message_broker,
                               ProcessLauncher* process_launcher,                               
                               ProcessEndingCallback* process_ending_callback,
                               const std::string& application_name,
                               const std::string& process_name,
                               const CdmwPlatformMngt::ProcessInfo& process_info,
                               CdmwPlatformMngt::Timeout creation_timeout)
       : m_agent(agent),
         m_process_message_broker(process_message_broker),
         m_process_launcher(process_launcher),
         m_process_ending_callback(process_ending_callback),
         m_application_name(application_name),
         m_process_name(process_name),
         m_process_info (process_info),
         m_creation_timeout(creation_timeout)
    {
        m_orb = CORBA::ORB::_duplicate (orb);
        m_root_poa = PortableServer::POA::_duplicate (root_poa);
    }

    virtual ~ManagedProcessCreationRun (void) throw()
    {
    }
    

    virtual void run() throw()
    {
        try
        {
            // creates the process starting data
            CdmwPlatformMngt::ProcessCallback::ProcessStartingData 
                       process_starting_data;

            process_starting_data.application_name = 
                       CORBA::string_dup(m_application_name.c_str());
            process_starting_data.process_name = CORBA::string_dup(m_process_name.c_str());

            // creates a specific message broker for the process
            CORBA::Object_var message_broker_base =
                       m_process_message_broker->activate(m_process_name);
            process_starting_data.process_message_broker =
                       CdmwPlatformMngt::ProcessMessageBroker::_narrow(message_broker_base.in());
            
            // set the process configuration data           
            process_starting_data.process_configuration_data =
                       m_process_info.configuration_data;
            
            
            // creates the acknowledgement callback
            PortableServer::ServantBase_var callback_servant =
                       new ProcessCallback_impl(m_root_poa.in(), process_starting_data);

            ProcessCallback_impl *callback_servant_impl =
                       dynamic_cast<ProcessCallback_impl*>(callback_servant.in());

            // activate servant
            callback_servant_impl->activate();
        
            // get object reference
            CdmwPlatformMngt::ProcessCallback_var callback = callback_servant_impl->_this();
               
            // creates the arguments for the OS process
            std::string args;

            args += Cdmw::Common::Options::CALLBACK_IOR_OPTION;
            args += "=";
            std::string callbackIOR = m_orb->object_to_string(callback.in());
            args += callbackIOR;
        
            if (m_process_info.process_port > 0)
            {
                args += " ";
                args += Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION;
                args += "=";
                std::ostringstream port_str;
                port_str << m_process_info.process_port;
                args += port_str.str();
            }

            args += " ";
            args += m_process_info.process_args.in();

            OsSupport::OS::ProcessId processID;
        
            // first lock the callback mutex
            callback_servant_impl->lock();

            // creates the OS process
            try
            {

                LogMngr::logMessage(INF, MSG_ID_AGENT_CREATE_MANAGED_PROCESS, m_process_name);

                if (m_process_launcher != NULL) 
                {
                    processID = m_process_launcher->create_process(
                        m_process_name,
                        m_process_info.process_exe.in(),
                        args,
                        m_process_info.working_directory.in(),
                        NULL, m_process_ending_callback);

                } 
                else 
                {
                    processID = OsSupport::OS::create_process(
                        m_process_info.process_exe.in(),
                        args,
                        m_process_info.working_directory.in(),
                        NULL, m_process_ending_callback);
                }
            }
            catch (const BadParameterException& e)
            {
                // cleans up
                try
                {
                    m_process_message_broker->deactivate(m_process_name);
                
                    // unlock the callback mutex
                    callback_servant_impl->unlock();
                    callback_servant_impl->deactivate();
                }
                catch(...)
                {
                }
            
                // It is catched again at end of the method
                throw e;
            }
            
            
            // waits for the end of starting (acknowledgement)
            AcknowledgementStatus ack = callback_servant_impl->wait(m_creation_timeout);
            
            CdmwPlatformMngt::ProcessDelegateWrapper_var process_delegate_wrapper =
                       CdmwPlatformMngt::ProcessDelegateWrapper::_nil();
            CdmwPlatformMngt::ProcessDelegate_var process_delegate = 
                       CdmwPlatformMngt::ProcessDelegate::_nil();
    

            if (ack == ACK_SUCCEED)
            {
                // get process delegate
                process_delegate = callback_servant_impl->getProcess();
                
                // create a process delegate wrapper
                // Reference counter is 1
                ProcessDelegateWrapper_impl* p_delegate_wrapper_impl =
                    new ProcessDelegateWrapper_impl (process_delegate.in(), 
                                                     m_process_name, m_agent);
                    
                // This affectation will decrement ref counter from 2 to 1 at end of scope
                PortableServer::ServantBase_var delegate_wrapper_servant = p_delegate_wrapper_impl;
                
                // activates the servant and creates the CORBA object. Ref counter is 2.
                process_delegate_wrapper = p_delegate_wrapper_impl->_this();
                
                // Add process in map (write locked)
                m_agent->fill_created_managed_process (
                                               m_process_name, processID,  
                                               process_delegate_wrapper.in());
                                               
                // register the process to killer
                ProcessKillMngr::Instance().register_process (
                                                      m_agent->m_application_name.c_str(),
                                                      m_process_name.c_str(), processID);

                LogMngr::logMessage(INF, MSG_ID_AGENT_MANAGED_PROCESS_CREATED, m_process_name);
                
                
                // deactivate the callback servant not useful as process is created
                callback_servant_impl->deactivate(); 
            }
            
            else if (ack == ACK_ERROR)
            {
                // The process has reported an error
                LogMngr::logMessage(ERR, MSG_ID_AGENT_MANAGED_PROCESS_CREATION_ERROR, m_process_name,
                                    callback_servant_impl->getErrorInfo());
                  
                std::string error_issuer = callback_servant_impl->getErrorIssuer();
                std::string error_info = callback_servant_impl->getErrorInfo();
                            
                // fill process with error info in map (write locked)
                m_agent->fill_faulty_process (m_process_name,
                                              ::CdmwPlatformMngt::PROCESS_CREATION_ERR, 
                                              error_issuer, error_info);                   

                // kills the OS process and cleans up
                try
                {
                    m_process_message_broker->deactivate(m_process_name);
                    callback_servant_impl->deactivate();

                    LogMngr::logMessage(ERR, MSG_ID_AGENT_KILL_PROCESS, m_process_name);
                    OsSupport::OS::kill_process(processID);
                }
                catch(...)
                {
                }
            }
            else
            {

                // The timeout has expired
                LogMngr::logMessage(ERR, MSG_ID_AGENT_MANAGED_PROCESS_CREATION_TIMEOUT, m_process_name);
                
                // fill process with error info in map (write locked)
                m_agent->fill_faulty_process (m_process_name,
                                              ::CdmwPlatformMngt::PROCESS_CMD_TIMEOUT, 
                                              "", "");
                                                         
                // kills the OS process and cleans up
                try
                {
                    m_process_message_broker->deactivate(m_process_name);
                    callback_servant_impl->deactivate();

                    LogMngr::logMessage(ERR, MSG_ID_AGENT_KILL_PROCESS, m_process_name);
                    OsSupport::OS::kill_process(processID);
                }
                catch(...)
                {
                }
            }
        }
        catch (const BadParameterException& e)
        { // We catch the BadParameterException which is, normaly, only (re)thrown after create_process
            LogMngr::logMessage(WRN, MSG_ID_AGENT_INVALID_PROCESS_INFO, m_process_name,
                                e.m_parameterValue);   

            // fill process with error info in map (write locked)
            m_agent->fill_faulty_process (m_process_name,
                                          ::CdmwPlatformMngt::PROCESS_INVALID_INFO, 
                                          "", (e.m_parameterValue).c_str());
        }
        catch(const std::bad_alloc&)
        {
            LogMngr::logMessage(ERR, MSG_ID_AGENT_NO_MEMORY);
            
            // fill process with exception info in map (write locked)
            m_agent->fill_faulty_exception_process (m_process_name,
                                                    ApplicationAgent_impl::NO_MEMORY_RAISED);
        }
        catch(const OutOfMemoryException&)
        {
            LogMngr::logMessage(ERR, MSG_ID_AGENT_NO_MEMORY);
            
            // fill process with exception info in map (write locked)
            m_agent->fill_faulty_exception_process (m_process_name,
                                                    ApplicationAgent_impl::NO_MEMORY_RAISED);
        }
        catch(const InternalErrorException& ex)
        {
            LogMngr::logMessage(ERR, MSG_ID_AGENT_INTERNAL, ex.get_info());
            
            // fill process with exception info in map (write locked)
            m_agent->fill_faulty_exception_process (m_process_name,
                                                    ApplicationAgent_impl::INTERNAL_RAISED);
        }
        catch (CORBA::SystemException& e)
        {
            LogMngr::logMessage(ERR, MSG_ID_AGENT_INTERNAL, e._info().c_str());
            // fill process with exception info in map (write locked)
            m_agent->fill_faulty_exception_process (m_process_name,
                                                    ApplicationAgent_impl::INTERNAL_RAISED);
        }
        catch(...)
        {
            LogMngr::logMessage(ERR, MSG_ID_AGENT_INTERNAL, "Unexpected exception");
            
            // fill process with exception info in map (write locked)
            m_agent->fill_faulty_exception_process (m_process_name,
                                                    ApplicationAgent_impl::INTERNAL_RAISED);
        }
    }
    
private:

    /**
    * The ORB.
    */
    CORBA::ORB_var m_orb;
    
    /**
    * The root POA.
    */
    PortableServer::POA_var m_root_poa;
    
    /**
    * The application agent.
    */
    ApplicationAgent_impl *m_agent;
    
    /**
    * The associated process message broker servant.
    */
    ProcessMessageBroker_impl *m_process_message_broker;
    
    /**
     * The ProcessLauncher
     */
    ProcessLauncher* m_process_launcher;
    
    /**
    * The process ending callback.
    */
    ProcessEndingCallback *m_process_ending_callback;
    
    /**
    * The name of the application.
    */
    std::string m_application_name;
    
    /**
    * The name of the process.
    */
    std::string m_process_name;
    
    /**
    * The process info.
    */
    CdmwPlatformMngt::ProcessInfo m_process_info;
    
    /**
    * The creation timeout.
    */
    CdmwPlatformMngt::Timeout m_creation_timeout;


};


//
// Constructor.
//
ApplicationAgent_impl::ApplicationAgent_impl(CORBA::ORB_ptr orb,
                                             CdmwPlatformMngt::Daemon_ptr daemon,
                                             LocalFaultDetector_impl* faultDetector,
                                             ProcessLauncher* processLauncher,
                                             OsSupport::SchedulerBase* scheduler,
                                             const char* applicationName)
       : m_creation_condition(m_processes_mutex),
         m_scheduler(scheduler),
         m_local_fault_detector(faultDetector)
{
    if (CORBA::is_nil(orb))
        CDMW_THROW2(BadParameterException, "orb", "nil");

    m_local_host_name = OsSupport::OS::get_hostname();

    std::string appl_pid_directory(processLauncher->getPidDirectory());
    appl_pid_directory += "/" ;
    appl_pid_directory += applicationName;
    m_process_launcher = new ProcessLauncher(appl_pid_directory.c_str());

    
    
    m_daemon = CdmwPlatformMngt::Daemon::_duplicate(daemon);

    // creates the specific POA
    CORBA::Object_var object = orb->resolve_initial_references("RootPOA");
    m_root_poa = PortableServer::POA::_narrow(object.in());

    PortableServer::POAManager_var manager = m_root_poa->the_POAManager();

    CORBA::PolicyList policies;
    OrbSupport::StrategyList poaStrategyList;

    policies.length(2);
    policies[0] = m_root_poa->create_id_assignment_policy(PortableServer::USER_ID);
    policies[1] = m_root_poa->create_lifespan_policy(PortableServer::PERSISTENT);

#if CDMW_ORB_VDR == orbacus
    poaStrategyList.add_PoaThreadPerRequest();

#elif CDMW_ORB_VDR == tao
    poaStrategyList.add_PoaThreadPerConnection();
#endif

    std::string agentPoaName = "CdmwApplicationAgentPOA";
    std::string name(applicationName);

    agentPoaName += name;

    try 
    {
      m_poa = m_root_poa->find_POA(agentPoaName.c_str(), false);
    }
    catch (const PortableServer::POA::AdapterNonExistent&)
    {
       m_poa = OrbSupport::OrbSupport::create_POA(m_root_poa.in(), agentPoaName.c_str(), 
          manager.in(), policies, poaStrategyList);
    }

    m_orb = CORBA::ORB::_duplicate(orb);

    // creates the process ending callback
    m_process_ending_callback = new ProcessEndingCallback(this);
    
    // increase the ref counter
    m_process_ending_callback->increaseCounter();

    std::string messageBrokerPoaName = "ProcessMessageBrokerPOA";
    messageBrokerPoaName += name;

    // creates the process message broker
    m_process_message_broker_servant = 
        new ProcessMessageBroker_impl(orb, messageBrokerPoaName.c_str(), m_daemon.in());

    m_process_message_broker =
        dynamic_cast<ProcessMessageBroker_impl*>(m_process_message_broker_servant.in()); 
        
#ifdef CDMW_USE_FAULTTOLERANCE

    m_fault_notifier = FT::FaultNotifier::_nil();

    
#endif
   
}


CdmwPlatformMngt::ApplicationAgent_ptr ApplicationAgent_impl::activate(const char* applicationName)
{

    std::string agent_name = "CdmwApplicationAgent";
    std::string name(applicationName);

    agent_name += name;
    
    PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(agent_name.c_str());
    
    m_poa->activate_object_with_id(oid.in(), this);

    return _this();

}


void ApplicationAgent_impl::deactivate(const char* applicationName)
{

    std::string agent_name = "CdmwApplicationAgent";
    std::string name(applicationName);

    agent_name += name;
    
    PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(agent_name.c_str());
    
    m_poa->deactivate_object(oid.in());

}


bool ApplicationAgent_impl::endCreation(
    const CdmwPlatformMngt::ApplicationAgentInfo& creationData)
{

    bool ret = true;

    try
    {
        m_application_name = creationData.application_name.in();

        m_process_observer = creationData.process_observer;

        m_process_message_broker->setProcessObserver(m_process_observer.in());
              
        m_process_message_broker->setServiceBroker(creationData.service_broker.in());
        
        m_process_message_broker->setApplicationName (m_application_name.c_str());
        
    
#ifdef CDMW_USE_FAULTTOLERANCE 

        // if fault notifier not yet defined by daemon directly
        if (CORBA::is_nil (m_fault_notifier.in()))
        {       
            // Get the replication manager
            CORBA::Object_var rm_obj =
                  m_orb->string_to_object (creationData.replication_manager_iogr);

            if (CORBA::is_nil (rm_obj.in()))
            {
                CDMW_THROW2(Cdmw::Exception,
                            Cdmw::Exception::FATAL,
                            "Invalid replication manager iogr");
            }
                        
            ::CdmwFT::ReplicationManager_var rm =
                  ::CdmwFT::ReplicationManager::_narrow(rm_obj.in());

        
            if (CORBA::is_nil (rm.in()))
            {
                CDMW_THROW2(Cdmw::Exception,
                        Cdmw::Exception::FATAL,
                        "Invalid replication manager reference");
            }
        
            CORBA::Object_var fn_obj = 
                  CORBA::Object::_duplicate (rm->get_fault_notifier());
        
            if (CORBA::is_nil (fn_obj.in()))
            {
                CDMW_THROW2(Cdmw::Exception,
                            Cdmw::Exception::FATAL,
                            "Invalid fault notifier reference");
            }

            m_fault_notifier = FT::FaultNotifier::_narrow(fn_obj.in());
        }

        m_local_fault_detector->set_fault_notifier (m_fault_notifier.in());

#else

        m_local_fault_detector->set_process_observer (m_process_observer.in());

#endif  
      
    }
    catch(...)
    {
        ret = false;
    }

    return ret;

}

//
// Destructor.
//
ApplicationAgent_impl::~ApplicationAgent_impl()
    throw()
{
    // delete the process launcher
    delete m_process_launcher;
}


PortableServer::POA_ptr ApplicationAgent_impl::_default_POA()
{

    return PortableServer::POA::_duplicate(m_poa.in());

}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/PullMonitorable/is_alive:1.0
//
/*
void ApplicationAgent_impl::is_alive()
    throw(CORBA::SystemException)
{
    //TODO : check the liveliness deeper
}
*/

CdmwPlatformMngt::ProcessCommandResult
ApplicationAgent_impl::create_process(const char* process_name,
        const CdmwPlatformMngt::ProcessInfo& process_info,
        CORBA::String_out error_info)
    throw(CORBA::SystemException)
{
    CdmwPlatformMngt::ProcessCommandResult result;
    
    bool process_exist = false;
    
    error_info = CORBA::string_dup("");  
    
    
    // first lock the mutex of map
    CDMW_MUTEX_GUARD(m_processes_mutex);
    
    try
    {
        // Add blank entry with process name in map, only if this name was not present.
        add_blank_process(process_name,::CdmwPlatformMngt::UNMANAGED_PROCESS);
        
        // search process name in map (must be found)
        OsProcesses::iterator iter = m_processes.find(process_name);    

        // if process element not locked (no creation in progress)
        if (!(iter->second).lock)
        {
            // if process id undefined and not auto end itself : request the creation
            if ((iter->second).processId == 0 && 
                (iter->second).process_auto_ended == false)
            {
                // lock the process element in map (creation in progress)
                (iter->second).lock = true;
               
                // enqueue the process creation
                m_scheduler->enqueue(new UnmanagedProcessCreationRun (
                               this,
                               m_process_launcher,                               
                               m_process_ending_callback,                               
                               m_application_name,
                               process_name,
                               process_info));                       
            }            
            else
            {
                // set process already created
                process_exist = true;
            }            
        }
        
        // if process already created
        if (process_exist)
        {
            result = CdmwPlatformMngt::PROCESS_ALREADY_EXIST;
        }
        else
        {
            // wait the end of the process creation
            while (m_processes[process_name].lock)
            {
                m_creation_condition.wait();
            }
        
            // search process name in map (must be found)
            OsProcesses::iterator iter = m_processes.find(process_name);   
            
            // get the result of creation 

            if ((iter->second).exceptionRaised != NO_EXCEPTION_RAISED)
            {
                if ((iter->second).exceptionRaised == NO_MEMORY_RAISED)
                {
                    throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
                }
                else
                {
                    throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
                }
            }
       
            error_info = CORBA::string_dup((iter->second).errorInfo.c_str());
            
            result = (iter->second).commandResult;
        }
                      
        return result;
    }
    catch(...)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}

CdmwPlatformMngt::ProcessCommandResult
ApplicationAgent_impl::create_managed_process(
        const char* process_name,
        const CdmwPlatformMngt::ProcessInfo& process_info,
        CdmwPlatformMngt::Timeout creation_timeout,
        CdmwPlatformMngt::ProcessDelegateWrapper_out process_wrapper,
        CORBA::String_out error_info,
        CORBA::String_out error_issuer)
    throw(CORBA::SystemException)
{
    CdmwPlatformMngt::ProcessCommandResult result;
    
    bool process_exist = false;
    
    error_info = CORBA::string_dup("");  
    error_issuer = CORBA::string_dup("");
    
    CdmwPlatformMngt::ProcessDelegateWrapper_var process_delegate_wrapper =
                CdmwPlatformMngt::ProcessDelegateWrapper::_nil();
    
    // first lock the mutex of map
    CDMW_MUTEX_GUARD(m_processes_mutex);
    
    try
    {
        // Add blank entry with process name in map, only if this name was not present.
        add_blank_process(process_name,::CdmwPlatformMngt::MANAGED_PROCESS);
        
        // search process name in map (must be found)
        OsProcesses::iterator iter = m_processes.find(process_name);    

        // get process delegate wrapper from map (can be nil)
        process_delegate_wrapper = (iter->second).processWrapper;
        
        // if process element not locked (no creation in progress)
        if (!(iter->second).lock)
        {
            // if process delegate wrapper undefined and not auto end itself : 
            //     request the creation
            if (CORBA::is_nil(process_delegate_wrapper.in()) &&
                (iter->second).process_auto_ended == false)
            {
                // lock the process element in map (creation in progress)
                (iter->second).lock = true;
               
                // enqueue the process creation
                m_scheduler->enqueue(new ManagedProcessCreationRun (
                               m_orb.in(),
                               m_root_poa.in(),
                               this,
                               m_process_message_broker,
                               m_process_launcher,                               
                               m_process_ending_callback,                               
                               m_application_name,
                               process_name,
                               process_info,
                               creation_timeout));                        
            }            
            else
            {
                // set process already created
                process_exist = true;
            }            
        }
        
        // if process already created
        if (process_exist)
        {
            result = CdmwPlatformMngt::PROCESS_ALREADY_EXIST;
        }
        else
        {
            // wait the end of the process creation
            while (m_processes[process_name].lock)
            {
                m_creation_condition.wait();
            }
        
            // search process name in map (must be found)
            OsProcesses::iterator iter = m_processes.find(process_name);   
            
            // get the result of creation 

            if ((iter->second).exceptionRaised != NO_EXCEPTION_RAISED)
            {
                if ((iter->second).exceptionRaised == NO_MEMORY_RAISED)
                {
                    throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
                }
                else
                {
                    throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
                }
            }
       
            error_issuer = CORBA::string_dup((iter->second).errorIssuer.c_str());
            error_info = CORBA::string_dup((iter->second).errorInfo.c_str());
            
            result = (iter->second).commandResult;
        
            process_delegate_wrapper = (iter->second).processWrapper;
        }
        
        // return the process delegate wrapper to the caller    
        process_wrapper = process_delegate_wrapper._retn();
                        
        return result;
    }
    catch(...)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}



CdmwPlatformMngt::ProcessDelegateWrapper_ptr
ApplicationAgent_impl::search_process_delegate_wrapper(const char* process_name)
    throw(CORBA::SystemException)
{
    CDMW_MUTEX_GUARD(m_processes_mutex);

    CdmwPlatformMngt::ProcessDelegateWrapper_var process_delegate_wrapper =
        CdmwPlatformMngt::ProcessDelegateWrapper::_nil();


    std::string name(process_name);

    OsProcesses::iterator iter = m_processes.find(name);
    
    if (iter != m_processes.end())
    {
        process_delegate_wrapper = (iter->second).processWrapper;
    }

    return process_delegate_wrapper._retn();
}
    


void ApplicationAgent_impl::kill_process(const char* process_name)
    throw(CdmwPlatformMngt::ProcessNotFound, CORBA::SystemException)
{

    try
    {        
        OsSupport::OS::ProcessId processId = find_OS_processId(process_name);
        
        // first reset process info from map
        reset_process_info_by_kill (process_name);
        
        if (processId == 0)
        {
            throw CdmwPlatformMngt::ProcessNotFound();
        }

        else
        {
            LogMngr::logMessage(INF, MSG_ID_AGENT_KILL_PROCESS, process_name);
        
            // kills the process
            try
            {
                if (m_process_launcher != NULL) 
                {
                    m_process_launcher->kill_process(process_name);
                } 
                else
                {
                    OsSupport::OS::kill_process(processId);
                }
            }
            catch(...)
            {
            }
       } 
    }
    catch(const BadParameterException &)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }
}

CdmwPlatformMngt::ProcessPID ApplicationAgent_impl::get_pid(const char* process_name)
    throw(CdmwPlatformMngt::ProcessNotFound, CORBA::SystemException)
{
    CdmwPlatformMngt::ProcessPID pid;
    try
    {
        pid = find_OS_processId(process_name);
        
        if (pid == 0)
        {
            throw CdmwPlatformMngt::ProcessNotFound();
        }
    }
    catch(const BadParameterException &)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }
   
    return pid;
}

void ApplicationAgent_impl::prepare_agent_removal()
{
    // set ending callback can be deletable
    m_process_ending_callback->setDeleteCallback();
    
    // decrease the ref counter of ending callback
    bool is_deletable = m_process_ending_callback->decreaseCounter();
    
    // if it can be deleted (no process use it)   
    if (is_deletable)
    {
        // delete callback (agent will be deactivated by the callback)
        delete m_process_ending_callback;
    }
}


void ApplicationAgent_impl::start_process_monitoring(const char* process_name,
                                                     const CdmwPlatformMngt::CompleteMonitoringInfo& process_monitoring_info)
    throw(CdmwPlatformMngt::ProcessNotFound,
              CdmwPlatformMngt::UnsupportedMonitoringModel,
              CdmwPlatformMngt::IncompatibleType,
              CdmwPlatformMngt::IncompatibleMonitoringCommand,
              CdmwPlatformMngt::IncompatibleMonitoringValue,
          CORBA::SystemException)
{
    try
    {
        std::string name(process_name);

        if (process_monitoring_info.monitoring_model != ::CdmwPlatformMngt::PULL_MONITORING_MODEL)
        {
            throw CdmwPlatformMngt::UnsupportedMonitoringModel();
        }
        else
        {
            ::FT::Location location;                
            location.length(3);
            location[0].kind = CORBA::string_dup ("hostname");
            location[0].id = CORBA::string_dup (m_local_host_name.c_str());
            location[1].kind = CORBA::string_dup ("applicationname");
            location[1].id = CORBA::string_dup (m_application_name.c_str());
            location[2].kind = CORBA::string_dup ("processname");
            location[2].id = CORBA::string_dup (process_name);

            FT::FaultMonitoringIntervalAndTimeoutValue timeout;
            timeout.monitoring_interval = process_monitoring_info.pull_monitoring_interval;
            timeout.timeout = process_monitoring_info.pull_monitoring_timeout;
        
            ::FT::PullMonitorable_ptr pull_monitorable = get_pull_monitorable(name);

            if (!CORBA::is_nil(pull_monitorable))
            {
                ::CdmwPlatformMngt::MonitoringId id = 
                      m_local_fault_detector->start_monitoring(pull_monitorable,
                                                               location, 
                                                               timeout); 
        
                set_process_monitoringId(name, id);
            }
            else
            {
                throw CdmwPlatformMngt::ProcessNotFound();
            }
        }
    }    
    catch(const CORBA::BAD_PARAM &)
    {
        throw CdmwPlatformMngt::IncompatibleMonitoringValue();
    }

}


void ApplicationAgent_impl::stop_process_monitoring(const char* process_name)
    throw(CdmwPlatformMngt::ProcessNotFound,
          CdmwPlatformMngt::IncompatibleType,
          CdmwPlatformMngt::IncompatibleMonitoringCommand,
          CORBA::SystemException)
{
    try
    {
       std::string name(process_name);

       ::CdmwPlatformMngt::MonitoringId id = find_process_monitoringId(name);

       if (id == 0) throw CdmwPlatformMngt::IncompatibleMonitoringCommand();
       
       m_local_fault_detector->stop_monitoring(id);

       set_process_monitoringId(name, 0);
    }
    catch(const CdmwPlatformMngt::InvalidMonitoringId &)
    {
       throw CdmwPlatformMngt::IncompatibleMonitoringCommand();
    }  
    catch(const BadParameterException &)
    {
       throw CdmwPlatformMngt::ProcessNotFound();
    }  
}


void ApplicationAgent_impl::destroy()
    throw(CORBA::SystemException)
{
 
    // TODO

}


void ApplicationAgent_impl::add_blank_process (
                           const std::string& name,
                           CdmwPlatformMngt::ProcessType process_type)
{
    // The process map must be locked by the caller
    
    // Warning : do not throw a BadParameterException in case process name already exist !
    
    if (!exists_process_unlock(name))
    {
        ProcessInfo info;
        
        info.processId = 0;
        info.processType = process_type;
        info.monitoringId = 0;        
        info.processWrapper = CdmwPlatformMngt::ProcessDelegateWrapper::_nil();
        info.commandResult = CdmwPlatformMngt::PROCESS_CMD_SUCCEED;
        info.errorIssuer = "";
        info.errorInfo = "";
        info.exceptionRaised = NO_EXCEPTION_RAISED;
        info.process_auto_ended = false;
        info.process_killed = false;
        info.lock = false;
        
        // store in map
        m_processes[name] = info;
    }
}




void ApplicationAgent_impl::fill_created_process(const std::string& name,
                                                 OsSupport::OS::ProcessId processId)
    throw(BadParameterException)
{
    CDMW_MUTEX_GUARD(m_processes_mutex);
    
    if (exists_process_unlock(name))
    {
        ProcessInfo& process_info = m_processes[name];
        
        process_info.processId = processId;
        process_info.commandResult = CdmwPlatformMngt::PROCESS_CMD_SUCCEED;
        process_info.errorIssuer = "";
        process_info.errorInfo = "";
        process_info.process_auto_ended = false;
        process_info.lock = false;

        // increase the ref counter of ending callback
        m_process_ending_callback->increaseCounter();
    }
    else
    {
        CDMW_THROW2(BadParameterException, name, "not found");
    }
    
    // broadcast to the waiting creation caller
    m_creation_condition.broadcast();
}

void ApplicationAgent_impl::fill_created_managed_process (
                                       const std::string& name,
                                       OsSupport::OS::ProcessId processId,
                                       CdmwPlatformMngt::ProcessDelegateWrapper_ptr process_wrapper)
    throw(BadParameterException)
{
    CDMW_MUTEX_GUARD(m_processes_mutex);
    
    if (exists_process_unlock(name))
    {
        ProcessInfo& process_info = m_processes[name];
        
        process_info.processId = processId;
        process_info.processWrapper = CdmwPlatformMngt::ProcessDelegateWrapper::_duplicate(process_wrapper);
        process_info.commandResult = CdmwPlatformMngt::PROCESS_CMD_SUCCEED;
        process_info.errorIssuer = "";
        process_info.errorInfo = "";
        process_info.process_auto_ended = false;
        process_info.lock = false;

        // increase the ref counter of ending callback
        m_process_ending_callback->increaseCounter();
    }
    else
    {
        CDMW_THROW2(BadParameterException, name, "not found");
    }
    
    // broadcast to the waiting creation caller
    m_creation_condition.broadcast();
}


void ApplicationAgent_impl::fill_faulty_process (const std::string& name,
                                                 CdmwPlatformMngt::ProcessCommandResult command_result,
                                                 const std::string & error_issuer,
                                                 const std::string & error_info)
    throw(BadParameterException)
{
    CDMW_MUTEX_GUARD(m_processes_mutex);
    
    if (exists_process_unlock(name))
    {
        ProcessInfo& process_info = m_processes[name];
        
        process_info.commandResult = command_result;
        process_info.errorIssuer = error_issuer;
        process_info.errorInfo = error_info;
        process_info.processId = 0;
        process_info.processWrapper = CdmwPlatformMngt::ProcessDelegateWrapper::_nil();
        process_info.process_auto_ended = false;
        process_info.lock = false;
    }
    else
    {
        CDMW_THROW2(BadParameterException, name, "not found");
    }
    
    // broadcast to the waiting creation caller
    m_creation_condition.broadcast();
}



void ApplicationAgent_impl::fill_faulty_exception_process (
                                                 const std::string& name,
                                                 ExceptionRaisedType exception_raised)
    throw(BadParameterException)
{
    CDMW_MUTEX_GUARD(m_processes_mutex);
    
    if (exists_process_unlock(name))
    {
        ProcessInfo& process_info = m_processes[name];
        
        process_info.exceptionRaised = exception_raised;
        process_info.processId = 0;
        process_info.processWrapper = CdmwPlatformMngt::ProcessDelegateWrapper::_nil();
        process_info.process_auto_ended = false;
        process_info.lock = false;
    }
    else
    {
        CDMW_THROW2(BadParameterException, name, "not found");
    }
    
    // broadcast to the waiting creation caller
    m_creation_condition.broadcast();
}

std::string ApplicationAgent_impl::reset_process_info_by_end (OsSupport::OS::ProcessId processId)
{
    std::string name = "";

    CDMW_MUTEX_GUARD(m_processes_mutex);
    
    OsProcesses::iterator it = find_process_iterator_unlock(processId);
    
    if (it != m_processes.end())
    {
        name = it->first;
        
        ProcessInfo& process_info = m_processes[name];
        
        // get monitoring id
        CdmwPlatformMngt::MonitoringId monitoring_id = process_info.monitoringId;
        if (monitoring_id != 0)
        {
            try
            {
                // stop the monitoring
                m_local_fault_detector->stop_monitoring(monitoring_id);
            }
            catch (...)
            {
                    // do nothing as monitoring is already stopped
            }
        }

        // if process killed by the supervision
        if (process_info.process_killed)
        {
            // erase element from map
            // (current pullMonitorable and processWrapper references are released)
            m_processes.erase(it);
        }
        // if process ended itself
        else
        {
            process_info.processId = 0;
            
            // processWrapper references are released
            process_info.processWrapper = CdmwPlatformMngt::ProcessDelegateWrapper::_nil();
            
            process_info.monitoringId = 0;
            
            // set process ended itself (or crash itself)
            process_info.process_auto_ended = true;    
        }
        
        // deactivates the specific process message broker
        try
        {
            m_process_message_broker->deactivate(name.c_str());
        }
        catch(const BadParameterException &)
        {
            // Not a managed process => ignored
        }
    }

    return name;
}

void ApplicationAgent_impl::reset_process_info_by_kill (const std::string& name)
{

    CDMW_MUTEX_GUARD(m_processes_mutex);

    OsProcesses::iterator it = m_processes.find(name);
    
    if (it != m_processes.end())
    {
        // if process already ended 
        if (it->second.processId == 0)
        {
            // erase element from map
            // (current pullMonitorable and processWrapper references are released)
            m_processes.erase(it);
        }
        // if process to kill
        else
        {
            it->second.process_killed = true;
        }
    }
}


void ApplicationAgent_impl::remove_process_info (const std::string& name)
{
    CDMW_MUTEX_GUARD(m_processes_mutex);
    
    OsProcesses::iterator it = m_processes.find(name);
    
    if (it != m_processes.end())
    {
        // erase element from map
        // (current pullMonitorable and processWrapper references are released)
        m_processes.erase(it);
        
        // deactivates the specific process message broker
        try
        {
            m_process_message_broker->deactivate(name.c_str());
        }
        catch(const BadParameterException &)
        {
            // Not a managed process => ignored
        }
    } 
}

bool ApplicationAgent_impl::exists_process_unlock(const std::string& name)
{
    bool found = false;

    OsProcesses::iterator it = m_processes.find(name);
    
    if (it != m_processes.end())
    {
        found = true;
    }    
    
    return found;
}

bool ApplicationAgent_impl::exists_process(const std::string& name)
{
    bool found = false;

    CDMW_MUTEX_GUARD(m_processes_mutex);
    found = exists_process_unlock(name);

    return found;
}


bool ApplicationAgent_impl::exists_process(OsSupport::OS::ProcessId processId)
{

    bool found = false;

    CDMW_MUTEX_GUARD(m_processes_mutex);

    OsProcesses::iterator it = find_process_iterator_unlock(processId);
    
    if (it != m_processes.end())
    {
        found = true;
    }

    return found;
}


OsSupport::OS::ProcessId ApplicationAgent_impl::find_OS_processId (
          const std::string& name)
    throw(BadParameterException)
{
    bool found = false;

    CDMW_MUTEX_GUARD(m_processes_mutex);
    
    OsProcesses::iterator it = m_processes.find(name);
    
    if (it != m_processes.end())
    {
        found = true;
    }    

    if (!found)
    {
        CDMW_THROW2(BadParameterException, name, "not found");
    }

    return it->second.processId;
}

ApplicationAgent_impl::OsProcesses::iterator ApplicationAgent_impl::find_process_iterator_unlock(
        OsSupport::OS::ProcessId processId )
{
    OsProcesses::iterator it;

    for (it = m_processes.begin(); it != m_processes.end(); ++it)
    {
        if (processId == it->second.processId)
        {
            break;
        }
    }
    
    return it;
}

::CdmwPlatformMngt::MonitoringId ApplicationAgent_impl::find_process_monitoringId(
              const std::string& name)
        throw(BadParameterException)
{
    bool found = false;

    CDMW_MUTEX_GUARD(m_processes_mutex);
    
    OsProcesses::iterator it = m_processes.find(name);
    
    if (it != m_processes.end())
    {
        found = true;
    }    

    if (!found)
    {
        CDMW_THROW2(BadParameterException, name, "not found");
    }

    return it->second.monitoringId;
}


void ApplicationAgent_impl::set_process_monitoringId(const std::string& name,
                                                     ::CdmwPlatformMngt::MonitoringId id)
        throw(BadParameterException)
{
    bool found = false;

    CDMW_MUTEX_GUARD(m_processes_mutex);
    
    OsProcesses::iterator it = m_processes.find(name);
    
    if (it != m_processes.end())
    {
        found = true;
        it->second.monitoringId = id;
    }    

    if (!found)
    {
        CDMW_THROW2(BadParameterException, name, "not found");
    }
}


::FT::PullMonitorable_ptr ApplicationAgent_impl::get_pull_monitorable(const std::string& name)
        throw(CdmwPlatformMngt::ProcessNotFound, 
              CdmwPlatformMngt::IncompatibleType)
{
    bool found = false;

    CDMW_MUTEX_GUARD(m_processes_mutex);
    
    OsProcesses::iterator it = m_processes.find(name);
    
    if (it != m_processes.end())
    {
        if (it->second.processType != ::CdmwPlatformMngt::MANAGED_PROCESS)
        {
            throw CdmwPlatformMngt::IncompatibleType();
        }    
            
        found = true;
    }    

    if (!found)
    {
       throw CdmwPlatformMngt::ProcessNotFound();
    }

    return ::FT::PullMonitorable::_duplicate(it->second.processWrapper->get_pull_monitorable());
}

#ifdef CDMW_USE_FAULTTOLERANCE

//
// Set the Fault Notifier reference.
//
void ApplicationAgent_impl::set_fault_notifier (FT::FaultNotifier_ptr fault_notifier)
{
    m_fault_notifier = FT::FaultNotifier::_duplicate (fault_notifier);
}
  
    
#endif

} // End namespace PlatformMngt

} // End namespace Cdmw


