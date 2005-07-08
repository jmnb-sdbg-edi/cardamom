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


#include "platformdaemon/Daemon_impl.hpp"
#include "platformdaemon/Callback_impl.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/ossupport/ProcessCallback.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformlibrary/BoundSyncCall.hpp"
#include "platformdaemon/DaemonLogMessageId.hpp"

#include <sstream>
#include <memory>


namespace Cdmw
{
namespace PlatformMngt
{


class ProcessCreationCallback : public OsSupport::ProcessCallback
{
public:

    ProcessCreationCallback(Daemon_impl* daemon, const std::string& processName)
        : m_daemon(daemon), m_processName(processName)
    {
    }

    virtual ~ProcessCreationCallback() { }

    void execute(OsSupport::OS::ProcessId processId) throw()
    {
        m_daemon->addSimpleProcess(processId, m_processName);
        LogMngr::logMessage(INF, MSG_ID_DAEMON_PROCESS_CREATED, m_processName);
    }

private:
    Daemon_impl *m_daemon;
    std::string m_processName;

};


class ProcessEndingCallback : public OsSupport::ProcessCallback
{
public:

    ProcessEndingCallback(Daemon_impl* daemon)
        : m_daemon(daemon)
    {
    }

    virtual ~ProcessEndingCallback() { }

    void execute(OsSupport::OS::ProcessId processId) throw()
    {

        try
        {
            std::string processName = m_daemon->removeSimpleProcess(processId);

            // logs the process ending
            LogMngr::logMessage(INF, MSG_ID_DAEMON_PROCESS_ENDED, processName);

            // creates the platform info
            std::auto_ptr<CdmwPlatformMngt::PlatformInfo> pPlatformInfo(
                m_daemon->createPlatformInfo(processName, CdmwPlatformMngt::ENDED));

            // adds the platform info for notifying
            (m_daemon->m_platformNotifier)->notify(pPlatformInfo.get());

            pPlatformInfo.release();
        }
        catch(const std::bad_alloc&)
        {
            LogMngr::logMessage(ERR, MSG_ID_DAEMON_NO_MEMORY);
        }
        catch(...)
        {
            LogMngr::logMessage(ERR, MSG_ID_DAEMON_INTERNAL, "no more information !");
        }
        
    }

private:
    Daemon_impl *m_daemon;

};


//
// Constructor.
//
Daemon_impl::Daemon_impl(CORBA::ORB_ptr orb,
                         const std::string& systemPortString,
                         unsigned long notificationCallTimeout,
                         ProcessLauncher* processLauncher,
                         size_t duration_time) 
    :m_monitorManager(this),
     m_systemPortString(systemPortString), 
     m_processLauncher(processLauncher),
     m_duration_time(duration_time)

{

    if (CORBA::is_nil(orb))
        CDMW_THROW2(BadParameterException, "orb", "nil");

    m_hostName = OsSupport::OS::get_hostname();

    // creates the specific POA
    CORBA::Object_var object = orb->resolve_initial_references("RootPOA");
    m_rootPoa = PortableServer::POA::_narrow(object.in());

    PortableServer::POAManager_var manager = m_rootPoa->the_POAManager();
	    
    CORBA::PolicyList policies;
    OrbSupport::StrategyList poaStrategyList;

    policies.length(2);
    policies[0] = m_rootPoa->create_id_assignment_policy(PortableServer::USER_ID);
    policies[1] = m_rootPoa->create_lifespan_policy(PortableServer::PERSISTENT);

// the PERSISTENT policy is incompatible with the THREADPOOL policy for TAO
#if CDMW_ORB_VDR == orbacus
    poaStrategyList.add_PoaThreadPerRequest();

#elif CDMW_ORB_VDR == tao
    poaStrategyList.add_PoaThreadPerConnection();
#endif

    m_poa = OrbSupport::OrbSupport::create_POA(m_rootPoa.in(), "CdmwDaemonPOA", 
        manager.in(), policies, poaStrategyList);

    m_orb = CORBA::ORB::_duplicate(orb);

    // creates the process ending callback
    m_processEndingCallback = new ProcessEndingCallback(this);

    // initialise the BoundSyncCallFactory used by the PlatformNotifier.
    BoundSyncCallFactory::initialise(orb);

    // creates the PlatformNotifier.
    PortableServer::ServantBase_var platformNotifier_servant =
        new PlatformNotifier_impl(this, notificationCallTimeout);

    m_platformNotifier =
        dynamic_cast<PlatformNotifier_impl*>(platformNotifier_servant.in());

    m_platformNotifierRef = m_platformNotifier->_this();

}


CdmwPlatformMngt::Daemon_ptr Daemon_impl::activate()
{

    PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId("CdmwDaemon");
    
    m_poa->activate_object_with_id(oid.in(), this);

    return _this();

}


CdmwPlatformMngt::PlatformInfo* Daemon_impl::createPlatformInfo(
    const std::string& processName,
    CdmwPlatformMngt::OSProcessStatus processStatus)
    throw(OutOfMemoryException)
{

    try
    {

        CdmwPlatformMngt::PlatformInfo_var platformInfo =
            new CdmwPlatformMngt::PlatformInfo;

        platformInfo->host_name = CORBA::string_dup(m_hostName.c_str());
        platformInfo->process_name = CORBA::string_dup(processName.c_str());
        platformInfo->process_status = processStatus;

        return platformInfo._retn();

    }
    catch(const std::bad_alloc&)
    {
        CDMW_THROW(OutOfMemoryException);
    }

}


CdmwPlatformMngt::PlatformSnapshot* Daemon_impl::createSnapshot()
    throw(CORBA::SystemException)
{

    try
    {

        CDMW_MUTEX_GUARD(m_mutex);

        CdmwPlatformMngt::PlatformSnapshot_var snapshot =
            new CdmwPlatformMngt::PlatformSnapshot;

        snapshot->length(m_simpleProcesses.size());

        CORBA::ULong index = 0;

        for (SimpleProcesses::iterator it = m_simpleProcesses.begin();
            it != m_simpleProcesses.end(); ++it)
        {

            CdmwPlatformMngt::PlatformInfo_var platformInfo =
                createPlatformInfo(it->second, CdmwPlatformMngt::STARTED);

            snapshot[index] = *platformInfo;

            index++;

        }

        return snapshot._retn();

    }
    catch(const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }

}


//
// Destructor.
//
Daemon_impl::~Daemon_impl()
{
    delete m_processEndingCallback;
    delete m_processLauncher;
    m_platformNotifier->stop();
}


PortableServer::POA_ptr Daemon_impl::_default_POA()
{

    return PortableServer::POA::_duplicate(m_poa.in());

}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/PullMonitorable/is_alive:1.0
//
void Daemon_impl::is_alive()
    throw(CORBA::SystemException)
{
    //TODO : check the liveliness deeper
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/platform_notifier:1.0
//
CdmwPlatformMngt::PlatformNotifier_ptr Daemon_impl::platform_notifier()
    throw(CORBA::SystemException)
{
    return CdmwPlatformMngt::PlatformNotifier::_duplicate(m_platformNotifierRef.in());
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/create_process:1.0
//
void Daemon_impl::create_process(const char* process_name,
                                 const CdmwPlatformMngt::ProcessInfo& process_info)
    throw(CdmwPlatformMngt::ProcessAlreadyExists,
          CdmwPlatformMngt::InvalidInfo,
          CORBA::SystemException)
{

    OsSupport::OS::ProcessId processId;

    if (hasSimpleProcess(process_name, processId))
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

        // creates the OS process
        try
        {
            ProcessCreationCallback processCreationCallback(this, process_name);

            LogMngr::logMessage(INF, MSG_ID_DAEMON_CREATE_PROCESS, process_name);

            processId = OsSupport::OS::create_process(
                process_info.process_exe.in(),
                args,
                process_info.working_directory.in(),
                &processCreationCallback, m_processEndingCallback);

        }
        catch (const BadParameterException& e)
        {

            LogMngr::logMessage(WRN, MSG_ID_DAEMON_INVALID_PROCESS_INFO, process_name,
                    e.m_parameterValue);

            CdmwPlatformMngt::InvalidInfo exception;
            exception.reason = CORBA::string_dup((e.m_parameterValue).c_str());
            throw exception;
        }

    }
    catch(const std::bad_alloc&)
    {
        LogMngr::logMessage(ERR, MSG_ID_DAEMON_NO_MEMORY);
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch(const OutOfMemoryException&)
    {
        LogMngr::logMessage(ERR, MSG_ID_DAEMON_NO_MEMORY);
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch(const InternalErrorException& ex)
    {
        LogMngr::logMessage(ERR, MSG_ID_DAEMON_INTERNAL, ex.get_info());
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }

}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/kill_process:1.0
//
void Daemon_impl::kill_process(const char* process_name)
    throw(CdmwPlatformMngt::ProcessNotFound,
          CORBA::SystemException)
{

    OsSupport::OS::ProcessId processId;

    if (!hasSimpleProcess(process_name, processId))
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }

    LogMngr::logMessage(INF, MSG_ID_DAEMON_KILL_PROCESS, process_name);
    killSimpleProcess(processId);

}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/resolve_corbaloc:1.0
//
CORBA::Object_ptr Daemon_impl::resolve_corbaloc(const char* corbaloc)
    throw(CdmwPlatformMngt::InvalidCorbaloc,
          CORBA::SystemException)
{

    try
    {
        return m_orb->string_to_object(corbaloc);
    }
    catch(const CORBA::BAD_PARAM&)
    {
        throw CdmwPlatformMngt::InvalidCorbaloc();
    }
    
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/get_remote_daemon:1.0
//
CdmwPlatformMngt::Daemon_ptr Daemon_impl::get_remote_daemon(const char* host_name)
    throw(CdmwPlatformMngt::HostNotFound,
          CdmwPlatformMngt::HostNotReachable,
          CORBA::SystemException)
{

    try
    {
        std::string remoteDaemonCorbaloc("corbaloc::");
        remoteDaemonCorbaloc += host_name;
        remoteDaemonCorbaloc += ":";
        remoteDaemonCorbaloc += m_systemPortString + "/" + CdmwPlatformMngt::DAEMON_NAME;

        CORBA::Object_var object = m_orb->string_to_object(remoteDaemonCorbaloc.c_str());
        CdmwPlatformMngt::Daemon_var daemon = CdmwPlatformMngt::Daemon::_narrow(object.in());

        if (CORBA::is_nil(daemon.in()))
        {
            throw CdmwPlatformMngt::HostNotFound();
        }

        return daemon._retn();

    }
    catch(const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch(const CORBA::BAD_PARAM&)
    {
        throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAM, CORBA::COMPLETED_NO);
    }
    catch(const CORBA::SystemException&)
    {
        throw CdmwPlatformMngt::HostNotReachable();
    }

}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/create_application:1.0
//
CdmwPlatformMngt::Application_ptr Daemon_impl::create_application(
        const CdmwPlatformMngt::ApplicationInfo& application_info,
        CdmwPlatformMngt::Timeout creation_timeout,
        CdmwPlatformMngt::ApplicationControl_out application_control)
    throw(CdmwPlatformMngt::CreationError,
        CdmwPlatformMngt::ApplicationAlreadyExists,
        CdmwPlatformMngt::InvalidInfo,
        CdmwPlatformMngt::CreationTimeout,
        CORBA::SystemException)
{
    //TODO
    throw CORBA::NO_IMPLEMENT(OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/kill_application:1.0
//
void Daemon_impl::kill_application(const char* application_name)
    throw(CdmwPlatformMngt::ApplicationNotFound, CORBA::SystemException)
{
    //TODO
    throw CORBA::NO_IMPLEMENT(OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/create_application_agent:1.0
//
CdmwPlatformMngt::ApplicationAgent_ptr Daemon_impl::create_application_agent(
        const CdmwPlatformMngt::ApplicationAgentInfo& agent_info,
        CdmwPlatformMngt::Timeout creation_timeout)
    throw(CdmwPlatformMngt::CreationError,
        CdmwPlatformMngt::ApplicationAgentAlreadyExists,
        CdmwPlatformMngt::InvalidInfo, CdmwPlatformMngt::CreationTimeout,
        CORBA::SystemException)
{

    CdmwPlatformMngt::ApplicationAgent_var applicationAgent;

    const char* applicationName = agent_info.application_agent_ack.application_name.in();

    OsSupport::OS::ProcessId processId;
    CORBA::Object_var processObject;

    if (m_monitorManager.hasMonitorableProcess(applicationName, APPLICATION_AGENT,
        processId, processObject))
    {
        throw CdmwPlatformMngt::ApplicationAgentAlreadyExists();
    }

    try
    {
        // creates the acknowledgement callback
        PortableServer::ServantBase_var callbackServant =
            new ApplicationAgentCallback_impl(m_rootPoa.in(),
                                              agent_info.application_agent_ack);

        ApplicationAgentCallback_impl *callbackServant_impl =
            dynamic_cast<ApplicationAgentCallback_impl*>(callbackServant.in());
            
        // activate servant
        callbackServant_impl->activate();

        CdmwPlatformMngt::ApplicationAgentCallback_var callback = callbackServant_impl->_this();

        // creates the arguments for the OS process
        std::string args;

        args += Cdmw::Common::Options::CALLBACK_IOR_OPTION;
        args += "=";
        std::string callbackIOR = m_orb->object_to_string(callback.in());
        args += callbackIOR;

        if (agent_info.application_agent_port > 0)
        {
            args += " ";
            args += Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION;
            args += "=";
            std::ostringstream port_str;
            port_str << agent_info.application_agent_port;
            args += port_str.str();
        }

        args += " ";
        
        // give agent its pidDirectory: <daemonPidDir>/<applicationName>
        args += ProcessLauncher::PIDS_DIRECTORY_OPTION;
        args += "=";
        args += m_processLauncher->getPidDirectory();
        args += "/";
        args += applicationName;
        
        args += " ";
        args += agent_info.application_agent_args.in();

        args += " ";
        args += Cdmw::Common::Options::REQUEST_DURATION_TIME;
        args += "=";

        std::ostringstream duration_time_str;
        duration_time_str << m_duration_time;
        
        args += duration_time_str.str();
        
        OsSupport::OS::ProcessId agentProcessID;
        
        // first lock the callback mutex
        callbackServant_impl->lock();

        // creates the OS process
        std::string agentName (applicationName);
        agentName += "_agent";
        try
        {

            MonitorableProcessEndingCallback *processEndingCallback =
                m_monitorManager.getProcessEndingCallback();

            LogMngr::logMessage(INF, MSG_ID_DAEMON_CREATE_APPLICATION_AGENT, applicationName);

            agentProcessID = m_processLauncher->create_process(
                agentName, 
                agent_info.application_agent_exe.in(),
                args,
                agent_info.application_agent_working_directory.in(),
                NULL, processEndingCallback);

        }
        catch (const BadParameterException& e)
        {

            LogMngr::logMessage(WRN, MSG_ID_DAEMON_INVALID_APPLICATION_AGENT_INFO, applicationName,
                    e.m_parameterValue);
                    
            // cleans up
            try
            {
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
            applicationAgent = callbackServant_impl->getApplicationAgent(); 
#ifdef CDMW_USE_FAULTTOLERANCE
                    
            MonitorableProcessInfo info(applicationName, 
                                        APPLICATION_AGENT,
                                        m_hostName, 
                                        applicationAgent.in(), 
                                        agent_info.application_agent_observer.in(),
                                        CORBA::string_dup(agent_info.application_agent_ack.replication_manager_iogr) );
#else
            MonitorableProcessInfo info(applicationName, 
                                        APPLICATION_AGENT,
                                        m_hostName, 
                                        applicationAgent.in(), 
                                        agent_info.application_agent_observer.in());
#endif            


            m_monitorManager.addMonitorableProcess(agentProcessID, info);
            LogMngr::logMessage(INF, MSG_ID_DAEMON_APPLICATION_AGENT_CREATED, applicationName);

            m_monitorManager.startMonitoring(agentProcessID,
                agent_info.application_agent_monitoring_info);
        }
        else if (ack == ACK_ERROR)
        {
            // The application agent has reported an error
            LogMngr::logMessage(ERR, MSG_ID_DAEMON_APPLICATION_AGENT_ERROR, applicationName,
                callbackServant_impl->getErrorInfo());
            // kills the OS process
            LogMngr::logMessage(INF, MSG_ID_DAEMON_KILL_APPLICATION_AGENT, applicationName);
            m_processLauncher->kill_process(agentName);

            CdmwPlatformMngt::CreationError errorException;
            errorException.error_info = CORBA::string_dup(callbackServant_impl->getErrorInfo());

            throw errorException;

        }
        else
        {
            // The timeout has expired
            LogMngr::logMessage(ERR, MSG_ID_DAEMON_APPLICATION_AGENT_TIMEOUT, applicationName);
            
            // kills the OS process
            LogMngr::logMessage(INF, MSG_ID_DAEMON_KILL_APPLICATION_AGENT, applicationName);
            m_processLauncher->kill_process(agentName);

            throw CdmwPlatformMngt::CreationTimeout();

        }
        // deactivate the callback servant not useful as agent is created
        callbackServant_impl->deactivate();
        
        return applicationAgent._retn();
    }
    catch(const std::bad_alloc&)
    {
        LogMngr::logMessage(ERR, MSG_ID_DAEMON_NO_MEMORY);
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch(const OutOfMemoryException&)
    {
        LogMngr::logMessage(ERR, MSG_ID_DAEMON_NO_MEMORY);
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch(const InternalErrorException& ex)
    {
        LogMngr::logMessage(ERR, MSG_ID_DAEMON_INTERNAL, ex.get_info());
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }

}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/search_application_agent:1.0
//
CdmwPlatformMngt::ApplicationAgent_ptr Daemon_impl::search_application_agent(
        const char* application_name)
    throw(CORBA::SystemException)
{

    CdmwPlatformMngt::ApplicationAgent_ptr applicationAgent =
        CdmwPlatformMngt::ApplicationAgent::_nil();

    OsSupport::OS::ProcessId processId;
    CORBA::Object_var processObject;

    if (m_monitorManager.hasMonitorableProcess(application_name, APPLICATION_AGENT,
        processId, processObject))
    {

        applicationAgent = CdmwPlatformMngt::ApplicationAgent::_narrow(processObject.in());

        if (CORBA::is_nil(applicationAgent))
        {
            // the returned processObject is not actually an application agent !
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
    }

    return applicationAgent;

}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/kill_application_agent:1.0
//
void Daemon_impl::kill_application_agent(
        const char* application_name)
    throw(CdmwPlatformMngt::ApplicationAgentNotFound, CORBA::SystemException)
{

    OsSupport::OS::ProcessId processId;
    CORBA::Object_var processObject;

    if (!m_monitorManager.hasMonitorableProcess(application_name, APPLICATION_AGENT,
        processId, processObject))
    {
        throw CdmwPlatformMngt::ApplicationAgentNotFound();
    }

    LogMngr::logMessage(INF, MSG_ID_DAEMON_KILL_APPLICATION_AGENT, application_name);

    m_monitorManager.stopMonitoring(processId);

    std::string agentName(application_name);
    agentName += "_agent";
    
    try {
        m_processLauncher->kill_process(agentName);
        
    } catch(const OutOfMemoryException&) {
        LogMngr::logMessage(ERR, MSG_ID_DAEMON_NO_MEMORY);
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    } catch (const InternalErrorException& ex) {
        LogMngr::logMessage(ERR, MSG_ID_DAEMON_INTERNAL, ex.get_info());
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/create_host_probe:1.0
//
CdmwPlatformMngt::HostProbe_ptr Daemon_impl::create_host_probe(
        const CdmwPlatformMngt::HostProbeInfo& probe_info,
        CdmwPlatformMngt::Timeout creation_timeout,
        CORBA::Boolean respawn)
    throw(CdmwPlatformMngt::CreationError,
        CdmwPlatformMngt::HostProbeAlreadyExists,
        CdmwPlatformMngt::InvalidInfo,
        CdmwPlatformMngt::CreationTimeout,
        CORBA::SystemException)
{
    //TODO
    throw CORBA::NO_IMPLEMENT(OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/search_host_probe:1.0
//
CdmwPlatformMngt::HostProbe_ptr Daemon_impl::search_host_probe()
    throw(CORBA::SystemException)
{
    //TODO
    throw CORBA::NO_IMPLEMENT(OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/kill_host_probe:1.0
//
void Daemon_impl::kill_host_probe()
    throw(CORBA::SystemException)
{
    //TODO
    throw CORBA::NO_IMPLEMENT(OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/shutdown:1.0
//
void Daemon_impl::shutdown()
    throw(CORBA::SystemException)
{

#ifdef CDMW_POSIX

    // resets the process attributes to remain root
    OsSupport::OS::set_process_attributes(0, 0);

#endif

    try
    {
        OsSupport::OS::create_process("cdmw_host_shutdown.sh", "", "", NULL, NULL);
        LogMngr::logMessage(INF, MSG_ID_DAEMON_SHUTDOWN);
    }
    catch (...)
    {
        // the deployment of the daemon doesn't allow the operation
        LogMngr::logMessage("cdmw_host_shutdown.sh failed");
        throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);
    }

}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/reboot:1.0
//
void Daemon_impl::reboot()
    throw(CORBA::SystemException)
{

#ifdef CDMW_POSIX

    // resets the process attributes to remain root
    OsSupport::OS::set_process_attributes(0, 0);

#endif

    try
    {
        OsSupport::OS::create_process("cdmw_host_reboot.sh", "", "", NULL, NULL);
        LogMngr::logMessage(INF, MSG_ID_DAEMON_REBOOT);
    }
    catch (...)
    {
        // the deployment of the daemon doesn't allow the operation
        LogMngr::logMessage("cdmw_host_reboot.sh failed");
        throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);
    }

}


void Daemon_impl::kill_application_processes(const std::string& applicationName) 
    throw (OutOfMemoryException,
           InternalErrorException)
{
    try {
        m_processLauncher->kill_all_processes(applicationName);

    } catch(const OutOfMemoryException&) {
        LogMngr::logMessage(ERR, MSG_ID_DAEMON_NO_MEMORY);
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    } catch (const InternalErrorException& ex) {
        LogMngr::logMessage(ERR, MSG_ID_DAEMON_INTERNAL, ex.get_info());
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }        
}



bool Daemon_impl::hasSimpleProcess(OsSupport::OS::ProcessId processId)
{

    bool ret = false;

    CDMW_MUTEX_GUARD(m_mutex);

    SimpleProcesses::iterator it = m_simpleProcesses.find(processId);
    
    if (it != m_simpleProcesses.end())
    {
        ret = true;
    }

    return ret;

}


bool Daemon_impl::hasSimpleProcess(const std::string& processName,
    OsSupport::OS::ProcessId& processId)
{

    bool ret = false;

    CDMW_MUTEX_GUARD(m_mutex);

    for (SimpleProcesses::iterator it = m_simpleProcesses.begin(); it != m_simpleProcesses.end(); ++it)
    {

        if (!processName.compare(it->second))
        {
            ret = true;
            processId = it->first;
            break;
        }

    }

    return ret;

}


void Daemon_impl::addSimpleProcess(OsSupport::OS::ProcessId processId,
        const std::string& processName)
    throw(OutOfMemoryException, InternalErrorException)
{

    CDMW_MUTEX_GUARD(m_mutex);

    try
    {

        m_simpleProcesses[processId] = processName;

        try
        {

            // creates the platform info
            std::auto_ptr<CdmwPlatformMngt::PlatformInfo> pPlatformInfo(
                createPlatformInfo(processName, CdmwPlatformMngt::STARTED));

            // adds the platform info for notifying
            m_platformNotifier->notify(pPlatformInfo.get());

            pPlatformInfo.release();

        }
        catch(...)
        {
            m_simpleProcesses.erase(processId);
            throw;
        }

    }
    catch(const std::bad_alloc&)
    {
        CDMW_THROW(OutOfMemoryException);
    }

}


std::string Daemon_impl::removeSimpleProcess(OsSupport::OS::ProcessId processId)
{

    std::string processName;

    CDMW_MUTEX_GUARD(m_mutex);

    SimpleProcesses::iterator it = m_simpleProcesses.find(processId);
    
    if (it != m_simpleProcesses.end())
    {
        processName = it->second;
        m_simpleProcesses.erase(processId);
    }

    return processName;

}


void Daemon_impl::killProcess(OsSupport::OS::ProcessId processId)
{

    try
    {
        OsSupport::OS::kill_process(processId);
    }
    catch(...)
    {

    }
}


void Daemon_impl::killSimpleProcess(OsSupport::OS::ProcessId processId)
{

    try
    {
        OsSupport::OS::kill_process(processId);
    }
    catch(const BadParameterException &)
    {
        // the OS process is already dead 

        // cleans up the list of processes
        try
        {
            CDMW_MUTEX_GUARD(m_mutex);
            m_simpleProcesses.erase(processId);
        }
        catch(...)
        {

        }
    }
    catch(...)
    {

    }
}


} // End namespace PlatformMngt
} // End namespace Cdmw


