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


#include "platformagent/ApplicationAgent_impl.hpp"
#include "platformagent/ProcessCallback_impl.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Options.hpp"

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/ossupport/ProcessCallback.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"

#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "platformagent/ApplicationAgentLogMessageId.hpp"


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


class ProcessCreationCallback : public OsSupport::ProcessCallback
{
public:

    ProcessCreationCallback(ApplicationAgent_impl* agent, const std::string& processName)
        : m_agent(agent), m_processName(processName)
    {
    }

    virtual ~ProcessCreationCallback() { }

    void execute(OsSupport::OS::ProcessId processId) throw()
    {
        m_agent->addProcess(processId, m_processName);
        LogMngr::logMessage(INF, MSG_ID_AGENT_PROCESS_CREATED, m_processName);
    }

private:
    ApplicationAgent_impl *m_agent;
    std::string m_processName;

};


class ProcessEndingCallback : public OsSupport::ProcessCallback
{
public:

    ProcessEndingCallback(ApplicationAgent_impl* agent)
        : m_agent(agent)
    {
    }



    virtual ~ProcessEndingCallback() 
    { 
    }
    
    

    void execute(OsSupport::OS::ProcessId processId) throw()
    {
        try
        {
            std::string processName = m_agent->removeProcess(processId);

            if (!processName.empty())
            {
                LogMngr::logMessage(INF, MSG_ID_AGENT_PROCESS_ENDED, processName.c_str());

                CdmwPlatformMngtBase::TimeStamp timeStamp =
                    EventHeaderFactory::createTimeStamp();
#ifdef CDMW_USE_FAULTTOLERANCE
                if (!CORBA::is_nil((m_agent->m_faultNotifier).in()))
                {
                    ::FT::Location location;
                    std::string my_host_name = OsSupport::OS::get_hostname();                
                    location.length(3);
                    location[0].kind = CORBA::string_dup( "hostname" );
                    location[0].id = CORBA::string_dup( OsSupport::OS::get_hostname().c_str() );
                    location[1].kind = CORBA::string_dup( "applicationname" );
                    location[1].id = CORBA::string_dup( m_agent->m_applicationName.c_str() );
                    location[2].kind = CORBA::string_dup( "processname" );
                    location[2].id = CORBA::string_dup( processName.c_str() );
                    
                    // Prepare the event for the fault notification
                    CosNotification::StructuredEvent fault_event;
                    fault_event.header.fixed_header.event_type.domain_name = CORBA::string_dup( "FT_CORBA" );
                    fault_event.header.fixed_header.event_type.type_name = CORBA::string_dup( "ObjectCrashFault" );
                    fault_event.filterable_data.length( 2 );
                    fault_event.filterable_data[0].name = CORBA::string_dup( "FTDomainId" );
                    fault_event.filterable_data[0].value <<= "thalesgroup.com";
                    fault_event.filterable_data[1].name = CORBA::string_dup( "Location" );
                    fault_event.filterable_data[1].value <<= location;
                    (m_agent->m_faultNotifier)->push_structured_fault( fault_event );
                    
                }
#else
                (m_agent->m_processObserver)->notify_ending(processName.c_str(), timeStamp); 
#endif
                
            }

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
    ApplicationAgent_impl *m_agent;

};


//
// Constructor.
//
ApplicationAgent_impl::ApplicationAgent_impl(CORBA::ORB_ptr orb, ProcessLauncher* processLauncher):
    m_processLauncher(processLauncher),
    m_init_exception_raised(false),
    m_init_done(false)
{

    if (CORBA::is_nil(orb))
        CDMW_THROW2(BadParameterException, "orb", "nil");

    // creates the specific POA
    CORBA::Object_var object = orb->resolve_initial_references("RootPOA");
    m_rootPoa = PortableServer::POA::_narrow(object.in());

    PortableServer::POAManager_var manager = m_rootPoa->the_POAManager();
	    
    CORBA::PolicyList policies;
    OrbSupport::StrategyList poaStrategyList;

    policies.length(2);
    policies[0] = m_rootPoa->create_id_assignment_policy(PortableServer::USER_ID);
    policies[1] = m_rootPoa->create_lifespan_policy(PortableServer::PERSISTENT);

#if CDMW_ORB_VDR == orbacus
    poaStrategyList.add_PoaThreadPerRequest();

#elif CDMW_ORB_VDR == tao
    poaStrategyList.add_PoaThreadPerConnection();
#endif


    m_poa = OrbSupport::OrbSupport::create_POA(m_rootPoa.in(), "CdmwApplicationAgentPOA", 
        manager.in(), policies, poaStrategyList);

    m_orb = CORBA::ORB::_duplicate(orb);

    // creates the process ending callback
    m_processEndingCallback = new ProcessEndingCallback(this);

    // creates the process message broker
#ifdef CDMW_USE_FAULTTOLERANCE
    m_processMessageBroker_servantBase = new ProcessMessageBroker_impl(this, orb);
#else
    m_processMessageBroker_servantBase = new ProcessMessageBroker_impl(orb);
#endif
    m_processMessageBroker =
        dynamic_cast<ProcessMessageBroker_impl*>(m_processMessageBroker_servantBase.in());
        
        
    // Creating a Condition associated with the member Mutex
    m_pinit_condition = new OsSupport::Condition(m_init_mutex);    
    
}


CdmwPlatformMngt::ApplicationAgent_ptr ApplicationAgent_impl::activate()
{

    PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId("CdmwApplicationAgent");
    
    m_poa->activate_object_with_id(oid.in(), this);

    return _this();

}


bool ApplicationAgent_impl::endCreation(
    const CdmwPlatformMngt::ApplicationAgentAck& acknowledgementData)
{

    bool ret = true;

    try
    {
        m_applicationName = acknowledgementData.application_name;

        m_processObserver =
            CdmwPlatformMngt::ProcessObserver::_duplicate(acknowledgementData.process_observer);

        m_processMessageBroker->setProcessObserver(m_processObserver.in());
        
        
        // agent initialisation is done
        set_init_done();

#ifdef CDMW_USE_FAULTTOLERANCE        
        // Get the replication manager
        CORBA::Object_var rm_obj
        = m_orb->string_to_object( acknowledgementData.replication_manager_iogr);

        if( CORBA::is_nil( rm_obj.in() ) )
            CDMW_THROW2(Cdmw::Exception,
                        Cdmw::Exception::FATAL,
                        "Invalid replication manager iogr" );
        ::CdmwFT::ReplicationManager_var rm =
          ::CdmwFT::ReplicationManager::_narrow(rm_obj.in());

        
        if( CORBA::is_nil( rm.in() ) )
            CDMW_THROW2(Cdmw::Exception,
                        Cdmw::Exception::FATAL,
                        "Invalid replication manager reference" );
        
        CORBA::Object_var fn_obj = CORBA::Object::_duplicate( rm->get_fault_notifier());
        
        if( CORBA::is_nil( fn_obj.in() ) )
            CDMW_THROW2(Cdmw::Exception,
                        Cdmw::Exception::FATAL,
                        "Invalid fault notifier reference" );

        m_faultNotifier = FT::FaultNotifier::_narrow(fn_obj.in());

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
    delete m_processEndingCallback;
    if (m_processLauncher != NULL)
        delete m_processLauncher;
        
    delete m_pinit_condition;
}


PortableServer::POA_ptr ApplicationAgent_impl::_default_POA()
{

    return PortableServer::POA::_duplicate(m_poa.in());

}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/PullMonitorable/is_alive:1.0
//
void ApplicationAgent_impl::is_alive()
    throw(CORBA::SystemException)
{
    //TODO : check the liveliness deeper
}


void ApplicationAgent_impl::create_process(const char* process_name,
        const CdmwPlatformMngt::ProcessInfo& process_info)
    throw(CdmwPlatformMngt::ProcessAlreadyExists,
          CdmwPlatformMngt::InvalidInfo,
          CORBA::SystemException)
{

    if (existsProcess(process_name))
    {
        throw CdmwPlatformMngt::ProcessAlreadyExists();
    }

    try
    {

        // creates the arguments for the OS process
        std::string args;

        if (process_info.process_port > 0)
        {
            args += Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION;
            args += "=";
            std::ostringstream port_str;
            port_str << process_info.process_port;
            args += port_str.str();
        }

        args += " ";
        args += process_info.process_args.in();

        OsSupport::OS::ProcessId processID;

        // creates the OS process
        try
        {

            ProcessCreationCallback processCreationCallback(this, process_name);

            LogMngr::logMessage(INF, MSG_ID_AGENT_CREATE_PROCESS, process_name);

            if (m_processLauncher != NULL) {
                processID = m_processLauncher->create_process(
                    process_name,
                    process_info.process_exe.in(),
                    args,
                    process_info.working_directory.in(),
                    &processCreationCallback, m_processEndingCallback);

            } else {
                processID = OsSupport::OS::create_process(
                    process_info.process_exe.in(),
                    args,
                    process_info.working_directory.in(),
                    &processCreationCallback, m_processEndingCallback);
            }
        }
        catch (const BadParameterException& e)
        {
            LogMngr::logMessage(WRN, MSG_ID_AGENT_INVALID_PROCESS_INFO, process_name,
                    e.m_parameterValue);

            CdmwPlatformMngt::InvalidInfo exception;
            exception.reason = CORBA::string_dup((e.m_parameterValue).c_str());
            throw exception;
        }

    }
    catch(const std::bad_alloc&)
    {
        LogMngr::logMessage(ERR, MSG_ID_AGENT_NO_MEMORY);
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch(const OutOfMemoryException&)
    {
        LogMngr::logMessage(ERR, MSG_ID_AGENT_NO_MEMORY);
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch(const InternalErrorException& ex )
    {
        LogMngr::logMessage(ERR, MSG_ID_AGENT_INTERNAL, ex.get_info());
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }

}


CdmwPlatformMngt::Process_ptr ApplicationAgent_impl::create_managed_process(const char* process_name,
        const CdmwPlatformMngt::ProcessInfo& process_info,
        CdmwPlatformMngt::Timeout creation_timeout,
        CdmwPlatformMngtEntity::EntityObserver_ptr entity_observer,
        CdmwPlatformMngtService::ServiceBroker_ptr service_broker)
    throw(CdmwPlatformMngt::CreationErrorExt,
        CdmwPlatformMngt::ProcessAlreadyExists,
        CdmwPlatformMngt::InvalidInfo,
        CdmwPlatformMngt::CreationTimeout,
        CORBA::SystemException)
{

    CdmwPlatformMngt::Process_var process;

    if (CORBA::is_nil(entity_observer) || CORBA::is_nil(service_broker))
    {
        throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);
    }

    if (existsProcess(process_name))
    {
        throw CdmwPlatformMngt::ProcessAlreadyExists();
    }

        
    // before to create the managed process wait the agent initialisation
    // to be completed to get all the parameters as Application name
    
    
    // first lock the mutex
    m_init_mutex.lock();
    
    // if initialisation not terminated (with exception or not)
    while (!(m_init_exception_raised || m_init_done))
    {
       // Wait for agent init end
       m_pinit_condition->wait();
    }
       
    // agent initialisation is normally terminated and condition broadcasted
    
    // if exception raised during initialisation
    if (m_init_exception_raised)
    {
        m_init_mutex.unlock();
        
        LogMngr::logMessage(ERR, MSG_ID_AGENT_EXCEPTION_DURING_INIT);
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
    
    // if initialisation not terminated (unexpected statement)
    if (!m_init_done)
    {
        m_init_mutex.unlock();
        
        LogMngr::logMessage(ERR, MSG_ID_AGENT_UNEXPECTED_STATEMENT);
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }

    // all is ok : unlock the mutex
    m_init_mutex.unlock();
    
    
    
    
    try
    {
        // creates the acknowledgement data
        CdmwPlatformMngt::ProcessCallback::ProcessAck processAck;

        processAck.application_name = CORBA::string_dup(m_applicationName.c_str());
        processAck.process_name = CORBA::string_dup(process_name);

        processAck.entity_observer =
            CdmwPlatformMngtEntity::EntityObserver::_duplicate(entity_observer);
        processAck.service_broker =
            CdmwPlatformMngtService::ServiceBroker::_duplicate(service_broker);

        // creates a specific message broker for the process
        CORBA::Object_var messageBrokerBase =
            m_processMessageBroker->activate(process_name);
        processAck.process_message_broker =
            CdmwPlatformMngt::ProcessMessageBroker::_narrow(messageBrokerBase.in());
            
            
        // creates the acknowledgement callback
        PortableServer::ServantBase_var callbackServant =
            new ProcessCallback_impl(m_rootPoa.in(), processAck);

        ProcessCallback_impl *callbackServant_impl =
            dynamic_cast<ProcessCallback_impl*>(callbackServant.in());

        // activate servant
        callbackServant_impl->activate();
        
        // get object reference
        CdmwPlatformMngt::ProcessCallback_var callback = callbackServant_impl->_this();
        
       
        // creates the arguments for the OS process
        std::string args;

        args += Cdmw::Common::Options::CALLBACK_IOR_OPTION;
        args += "=";
        std::string callbackIOR = m_orb->object_to_string(callback.in());
        args += callbackIOR;
        

        if (process_info.process_port > 0)
        {
            args += " ";
            args += Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION;
            args += "=";
            std::ostringstream port_str;
            port_str << process_info.process_port;
            args += port_str.str();
        }

        args += " ";
        args += process_info.process_args.in();

        OsSupport::OS::ProcessId processID;
        
        // first lock the callback mutex
        callbackServant_impl->lock();

        // creates the OS process
        try
        {

            LogMngr::logMessage(INF, MSG_ID_AGENT_CREATE_MANAGED_PROCESS, process_name);

            if (m_processLauncher != NULL) {
                processID = m_processLauncher->create_process(
                    process_name,
                    process_info.process_exe.in(),
                    args,
                    process_info.working_directory.in(),
                    NULL, m_processEndingCallback);

            } else {
                processID = OsSupport::OS::create_process(
                    process_info.process_exe.in(),
                    args,
                    process_info.working_directory.in(),
                    NULL, m_processEndingCallback);
                
            }
        }
        catch (const BadParameterException& e)
        {
            LogMngr::logMessage(WRN, MSG_ID_AGENT_INVALID_PROCESS_INFO, process_name,
                    e.m_parameterValue);
                    
            // cleans up
            try
            {
                m_processMessageBroker->deactivate(process_name);
                
                // unlock the callback mutex
                callbackServant_impl->unlock();
                callbackServant_impl->deactivate();
            }
            catch(...)
            {

            }

            CdmwPlatformMngt::InvalidInfo exception;
            exception.reason = CORBA::string_dup((e.m_parameterValue).c_str());
            throw exception;
        }

        // waits for the acknowledgement
        AcknowledgementStatus ack = callbackServant_impl->wait(creation_timeout);

        if (ack == ACK_SUCCEED)
        {
            process = callbackServant_impl->getProcess(); 

            addProcess(processID, process_name);

            LogMngr::logMessage(INF, MSG_ID_AGENT_MANAGED_PROCESS_CREATED, process_name);
        }
        else if (ack == ACK_ERROR)
        {
            // The process has reported an error
            LogMngr::logMessage(ERR, MSG_ID_AGENT_MANAGED_PROCESS_ERROR, process_name,
                callbackServant_impl->getErrorInfo());
            
            // kills the OS process and cleans up
            try
            {
                m_processMessageBroker->deactivate(process_name);
                callbackServant_impl->deactivate();

                LogMngr::logMessage(ERR, MSG_ID_AGENT_KILL_PROCESS, process_name);
                OsSupport::OS::kill_process(processID);
            }
            catch(...)
            {

            }

            CdmwPlatformMngt::CreationErrorExt errorException;
            errorException.issuer = CORBA::string_dup(callbackServant_impl->getErrorIssuer());
            errorException.error_info = CORBA::string_dup(callbackServant_impl->getErrorInfo());

            throw errorException;

        }
        else
        {
            // The timeout has expired
            LogMngr::logMessage(ERR, MSG_ID_AGENT_MANAGED_PROCESS_TIMEOUT, process_name);
            
            // kills the OS process and cleans up
            try
            {
                m_processMessageBroker->deactivate(process_name);
                callbackServant_impl->deactivate();

                LogMngr::logMessage(ERR, MSG_ID_AGENT_KILL_PROCESS, process_name);
                OsSupport::OS::kill_process(processID);
            }
            catch(...)
            {

            }

            throw CdmwPlatformMngt::CreationTimeout();

        }
    
        // deactivate the callback servant not useful as process is created
        callbackServant_impl->deactivate();

        return process._retn();

    }
    catch(const std::bad_alloc&)
    {
        LogMngr::logMessage(ERR, MSG_ID_AGENT_NO_MEMORY);
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch(const OutOfMemoryException&)
    {
        LogMngr::logMessage(ERR, MSG_ID_AGENT_NO_MEMORY);
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch(const InternalErrorException& ex)
    {
        LogMngr::logMessage(ERR, MSG_ID_AGENT_INTERNAL,ex.get_info());
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }

}


void ApplicationAgent_impl::kill_process(const char* process_name)
    throw(CdmwPlatformMngt::ProcessNotFound, CORBA::SystemException)
{

    try
    {

        OsSupport::OS::ProcessId processId = findOSProcessId(process_name);

        LogMngr::logMessage(INF, MSG_ID_AGENT_KILL_PROCESS, process_name);

        // kills the process
        try
        {
            if (m_processLauncher != NULL) {
                m_processLauncher->kill_process(process_name);
            } else {
                OsSupport::OS::kill_process(processId);
            }
        }
        catch(const BadParameterException &)
        {
            // the OS process is already dead 

            // cleans up the list of processes

            try
            {
                CDMW_MUTEX_GUARD(m_mutex);
                m_processes.erase(processId);
            }
            catch(...)
            {

            }
        }
        catch(...)
        {

        }

        // deactivates the specific process message broker
        try
        {
            m_processMessageBroker->deactivate(process_name);
        }
        catch(const BadParameterException &)
        {
            // Not a managed process => ignored
        }

    }
    catch(const BadParameterException &)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }

}

void ApplicationAgent_impl::start_process_monitoring(const char* process_name,
                                      const CdmwPlatformMngt::CompleteMonitoringInfo& process_monitoring_info,
                                      const CdmwPlatformMngt::MonitoringInfos& activity_point_monitoring_infos)
    throw(CdmwPlatformMngt::ProcessNotFound,
          CdmwPlatformMngt::UnsupportedMonitoringModel,
          CdmwPlatformMngt::OutOfActivityPoints,
          CdmwPlatformMngt::IncompatibleType,
          CORBA::SystemException)
{

    // TODO

}


void ApplicationAgent_impl::stop_process_monitoring(const char* process_name)
    throw(CdmwPlatformMngt::ProcessNotFound,
          CdmwPlatformMngt::IncompatibleType,
          CORBA::SystemException)
{

    // TODO

}


void ApplicationAgent_impl::destroy()
    throw(CORBA::SystemException)
{

    // TODO

}



void ApplicationAgent_impl::addProcess(OsSupport::OS::ProcessId processId,
        const std::string& name)
{

    CDMW_MUTEX_GUARD(m_mutex);
    m_processes[processId] = name;

}


std::string ApplicationAgent_impl::removeProcess(OsSupport::OS::ProcessId processId)
{

    std::string name;

    CDMW_MUTEX_GUARD(m_mutex);

    OsProcesses::iterator it = m_processes.find(processId);
    
    if (it != m_processes.end())
    {
        name = it->second;
        m_processes.erase(it);
        
        
        // deactivates the specific process message broker
        try
        {
            m_processMessageBroker->deactivate(name.c_str());
        }
        catch(const BadParameterException &)
        {
            // Not a managed process => ignored
        }
    }

    return name;

}


bool ApplicationAgent_impl::existsProcess(const std::string& name)
{

    bool found = false;

    CDMW_MUTEX_GUARD(m_mutex);

    OsProcesses::iterator it;

    for (it = m_processes.begin(); it != m_processes.end(); ++it)
    {

        if (!name.compare(it->second))
        {
            found = true;
            break;
        }

    }

    return found;

}


bool ApplicationAgent_impl::existsProcess(OsSupport::OS::ProcessId processId)
{

    bool found = false;

    CDMW_MUTEX_GUARD(m_mutex);

    OsProcesses::iterator it = m_processes.find(processId);
    
    if (it != m_processes.end())
    {
        found = true;
    }

    return found;

}


OsSupport::OS::ProcessId ApplicationAgent_impl::findOSProcessId(
        const std::string& name)
    throw(BadParameterException)
{

    bool found = false;

    CDMW_MUTEX_GUARD(m_mutex);

    OsProcesses::iterator it;

    for (it = m_processes.begin(); it != m_processes.end(); ++it)
    {

        if (!name.compare(it->second))
        {
            found = true;
            break;
        }

    }

    if (!found)
    {
        CDMW_THROW2(BadParameterException, "name", "not found");
    }

    return it->first;

}


void 
ApplicationAgent_impl::set_init_exit_failure() 
{
    m_init_mutex.lock();
    m_init_exception_raised = true;    
    m_init_mutex.unlock();
    
    m_pinit_condition->broadcast();
}

void 
ApplicationAgent_impl::set_init_done() 
{
    m_init_mutex.lock();
    m_init_done = true;
    m_init_mutex.unlock();
    
    m_pinit_condition->broadcast();
}


} // End namespace PlatformMngt

} // End namespace Cdmw


