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


#ifndef INCL_PLATFORMMNGT_DAEMON_IMPL_HPP 
#define INCL_PLATFORMMNGT_DAEMON_IMPL_HPP 


#include <map>
#include <string>

#include "Foundation/common/System.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/osthreads/SchedulerBase.hpp"

#include "idllib/CdmwPlatformMngtDaemon.skel.hpp"

#include "idllib/CdmwPlatformMngtMonitoringObserver.stub.hpp"
#include "idllib/CdmwPlatformMngtApplication.stub.hpp"
#include "idllib/CdmwPlatformMngtApplicationAgent.stub.hpp"
#include "idllib/CdmwPlatformMngtHost.stub.hpp"
#include "idllib/CdmwPlatformMngtLocalFaultDetector.stub.hpp"

#include "platformagent/ApplicationAgent_impl.hpp"
#include "platformdaemon/ServiceRegistration_impl.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


class ProcessLauncher;
class SimpleProcessCreationCallback;
class SimpleProcessEndingCallback;


/**
* Purpose:
* <p>
* implements the
* IDL:thalesgroup.com/CdmwPlatformMngt/Daemon:1.0
* interface.
*/
class Daemon_impl : virtual public POA_CdmwPlatformMngt::Daemon,
    virtual public PortableServer::RefCountServantBase
{
    
    friend class SimpleProcessCreationCallback;
    friend class SimpleProcessEndingCallback;

public:

    /**
    * Purpose:
    * <p>
    * Constructor.
    */
    Daemon_impl(CORBA::ORB_ptr orb, 
                const std::string& systemPortString,
                unsigned long notificationCallTimeout, 
                unsigned long pullInterval,
                unsigned long pullTimeout,
                ProcessLauncher* processLauncher,
                ServiceRegistration_impl* serviceRegistration,
                size_t duration_time,
                const std::string& fault_manager_corbaloc);


    /**
    * Purpose:
    * <p>
    * Activates this daemon servant and returns the associated CORBA reference.
    */
    CdmwPlatformMngt::Daemon_ptr activate();


    /**
    * Purpose:
    * <p>
    * Destructor.
    */
    virtual ~Daemon_impl();


    /**
    * Purpose:
    * <p>
    * Returns the specific POA associated with the daemon.
    */
    virtual PortableServer::POA_ptr _default_POA();


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/PullMonitorable/is_alive:1.0
    * operation
    */
    virtual void is_alive()
        throw(CORBA::SystemException);



    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/resolve_corbaloc:1.0
    * operation
    */
    virtual CORBA::Object_ptr resolve_corbaloc(const char* corbaloc)
        throw(CdmwPlatformMngt::InvalidCorbaloc,
              CORBA::SystemException);


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/get_remote_daemon:1.0
    * operation
    */
    virtual CdmwPlatformMngt::Daemon_ptr get_remote_daemon(const char* host_name)
        throw(CdmwPlatformMngt::HostNotFound,
              CdmwPlatformMngt::HostNotReachable,
              CORBA::SystemException);
              
    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/create_process:1.0
    * operation
    */          
    virtual void create_process (const char * process_name,
                                 const CdmwPlatformMngt::ProcessInfo& process_info)
        throw(CdmwPlatformMngt::ProcessAlreadyExists,
              CdmwPlatformMngt::InvalidInfo,
              CORBA::SystemException);
              
    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/kill_process:1.0
    * operation
    */          
    virtual void kill_process (const char * process_name)
        throw(CdmwPlatformMngt::ProcessNotFound,
              CORBA::SystemException);


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/create_application:1.0
    * operation
    */
    virtual CdmwPlatformMngt::Application_ptr create_application(
            const CdmwPlatformMngt::ApplicationInfo& application_info,
            CdmwPlatformMngt::Timeout creation_timeout,
            CdmwPlatformMngt::ApplicationControl_out application_control)
        throw(CdmwPlatformMngt::CreationError,
            CdmwPlatformMngt::ApplicationAlreadyExists,
            CdmwPlatformMngt::InvalidInfo,
            CdmwPlatformMngt::CreationTimeout,
            CORBA::SystemException);


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/kill_application:1.0
    * operation
    */
    virtual void kill_application(const char* application_name)
        throw(CdmwPlatformMngt::ApplicationNotFound, CORBA::SystemException);


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/create_application_agent:1.0
    * operation
    */
    virtual CdmwPlatformMngt::ApplicationAgent_ptr create_application_agent(
            const CdmwPlatformMngt::ApplicationAgentInfo& agent_info)
        throw(CdmwPlatformMngt::CreationError,
            CdmwPlatformMngt::ApplicationAgentAlreadyExists,
            CdmwPlatformMngt::InvalidInfo,
            CORBA::SystemException);


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/search_application_agent:1.0
    * operation
    */

    virtual CdmwPlatformMngt::ApplicationAgent_ptr search_application_agent(
            const char* application_name)
        throw(CORBA::SystemException);


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/kill_application_agent:1.0
    * operation
    */
    virtual void kill_application_agent(const char* application_name)
        throw(CdmwPlatformMngt::ApplicationAgentNotFound,
              CORBA::SystemException);
              
    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/register_local_service:1.0
    * operation
    */    
    virtual CORBA::Object_ptr register_local_service (const char * service_name,
                                                      CORBA::Object_ptr service_ref)
        throw(CORBA::SystemException);
        
    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/unregister_local_service:1.0
    * operation
    */    
    virtual CORBA::Object_ptr unregister_local_service (const char * service_name)
        throw(CORBA::SystemException);
        
    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/resolve_local_service:1.0
    * operation
    */    
    virtual CORBA::Object_ptr resolve_local_service (const char * service_name)
        throw(CORBA::SystemException);
        
    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/discover_global_service:1.0
    * operation
    */    
    virtual CORBA::Object_ptr discover_global_service (const char * service_name)
        throw(CORBA::SystemException);
                   

    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/shutdown:1.0
    * operation
    */
    virtual void shutdown()
        throw(CORBA::SystemException);


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/cold_reboot:1.0
    * operation
    */
    virtual void cold_reboot()
        throw(CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/hot_reboot:1.0
    * operation
    */
    virtual void hot_reboot()
        throw(CORBA::SystemException);



    /**
    * Purpose:
    * <p> kill all processes which have been created by the agent of the specified application.
    * The agent is not killed. 
    */
    virtual void kill_application_processes(const std::string& applicationName)
        throw(OutOfMemoryException,
              InternalErrorException);
              
    /**
    * Purpose:
    * <p> Sets the process monitoring Id of the process specified by its name.
    * 
    * @exception BadParameterException if name doesn't denote an existing
    * OS Process.
    */    
    void set_process_monitoringId (const std::string& processName,
                                   ::CdmwPlatformMngt::MonitoringId id)
        throw(BadParameterException);
        
              
              
#ifdef CDMW_USE_FAULTTOLERANCE

    /**
    * Purpose:
    * <p> Get the Fault Notifier reference.
    * 
    */
    FT::FaultNotifier_ptr get_fault_notifier();  
    
#endif
        
private:

    /**
    * Purpose:
    * <p> Indicates whether the daemon manages the specified simple process.
    *
    * @param processId the id of the process to find
    */    
    bool hasSimpleProcess(OsSupport::OS::ProcessId processId);


    /**
    * Purpose:
    * <p> Indicates whether the daemon manages the specified simple process.
    *
    * @param processName the name of the process to find
    * @param processId contains the id of the process if true is returned.
    * @return true if the process is managed by the daemon.
    * 
    */    
    bool hasSimpleProcess(const std::string& processName,
                          OsSupport::OS::ProcessId& processId);

    /**
    * Purpose:
    * <p> Stores the information about the process.
    */    
    void addSimpleProcess(OsSupport::OS::ProcessId processId,
                          const std::string& processName)
        throw(OutOfMemoryException, InternalErrorException);


    /**
    * Purpose:
    * <p> Removes the information about the process and returns the process name.
    *
    * @param processId the id of the process to remove.
    * @param monitoringId contains the id of the monitoring is returned.
    * @return the name of removed process ("" if not found).
    */    
    std::string removeSimpleProcess(OsSupport::OS::ProcessId processId,
                                    ::CdmwPlatformMngt::MonitoringId& monitoringId);



    /**
    * Purpose:
    * <p> Removes the information about the process and returns the process id.
    *
    * @param processName the name of the process to remove
    * @param processId contains the id of the process if true is returned.
    * @param monitoringId contains the id of the monitoring if true is returned.
    * @return true if the process is managed by the daemon.
    */     
    bool removeSimpleProcess(const std::string& processName,
                             OsSupport::OS::ProcessId& processId,
                             ::CdmwPlatformMngt::MonitoringId& monitoringId);
    
    
    /**
    * Purpose:
    * <p> Indicates whether the daemon manages the specified application agent.
    */    
    bool hasApplicationAgent(const char* applicationName);


    /**
    * Purpose:
    * <p> Stores the information about the application agent.
    */    
    void addApplicationAgent(const char* applicationName,
			     ApplicationAgent_impl* agentImpl);

    /**
    * Purpose:
    * <p> Removes the application agent.
    */    
    ApplicationAgent_impl* removeApplicationAgent(const char* applicationName);

private:

    struct SimpleProcessInfo 
    {
        std::string processName;                             // process name
        ::CdmwPlatformMngt::MonitoringId monitoringId;       // monitoring id
    };

    typedef std::map<OsSupport::OS::ProcessId, SimpleProcessInfo> SimpleProcesses;

    /**
    * The processes managed by this daemon.
    */
    SimpleProcesses m_simpleProcesses;

    /**
    * The mutex protecting concurrent access to m_simpleProcesses.
    */
    OsSupport::Mutex m_simpleProcessesMutex;
    
    /**
    * The simple process ending callback.
    */
    SimpleProcessEndingCallback *m_simpleProcessEndingCallback;
    
    
    
    

    typedef std::map<std::string, ApplicationAgent_impl*> Agents;

    /**
    * The application agent implementations managed by this daemon.
    */
    Agents m_agents;
    
    /**
    * The mutex protecting concurrent access to m_processes.
    */
    OsSupport::Mutex m_agentsMutex;


    /**
    * The local fault detector.
    */
    CdmwPlatformMngt::LocalFaultDetector_var m_localFaultDetector;

    LocalFaultDetector_impl* m_localFaultDetector_impl;

#ifdef CDMW_USE_FAULTTOLERANCE

    /**
     * The fault notifier
     */
    FT::FaultNotifier_var m_faultNotifier; 

#endif

    /**
    * The ORB.
    */
    CORBA::ORB_var m_orb;
        
    /**
    * The root POA.
    */
    PortableServer::POA_var m_rootPoa;

    /**
    * The specific POA managing the daemon.
    */
    PortableServer::POA_var m_poa;

    /**
    * The port of the CDMW system and consequently the port of the daemon.
    */
    std::string m_systemPortString;

    /**
    * The name of the host where the daemon is running.
    */
    std::string m_hostName;

    /**
    * The process launcher.
    */
    ProcessLauncher* m_processLauncher;

    /**
     * the servide Registration.
     */
    ServiceRegistration_impl* m_serviceRegistration;


    /*
     * request duration time (used only for Fault Tolerance)
     */
    size_t m_duration_time;
    
    /**
     * The specific POA in charge of the PullMonitorableRegistration servant.
     */
    PortableServer::POA_var m_pullMonitorRegistrationPoa;
    
    
    typedef std::map <std::string, CORBA::Object_var> Services;
    
    /**
     * The local specific services.
     */
    Services m_localServices;
    
    /**
    * The mutex protecting concurrent access to m_localServices.
    */
    OsSupport::Mutex m_localServicesMutex;
    
    /**
    * The scheduler base used to create process.
    */
    Cdmw::OsSupport::SchedulerBase m_scheduler;
    
};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_DAEMON_IMPL_HPP

