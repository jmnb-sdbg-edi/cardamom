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


#ifndef INCL_PLATFORMMNGT_DAEMON_IMPL_HPP 
#define INCL_PLATFORMMNGT_DAEMON_IMPL_HPP 

#include "Foundation/common/System.hpp"

#include <map>
#include <string>

#include "Foundation/ossupport/OS.hpp"

#include "Foundation/orbsupport/CORBA.hpp"

#include "idllib/CdmwPlatformMngtDaemon.skel.hpp"

#include "idllib/CdmwPlatformMngtMonitoringObserver.stub.hpp"
#include "idllib/CdmwPlatformMngtApplication.stub.hpp"
#include "idllib/CdmwPlatformMngtApplicationAgent.stub.hpp"
#include "idllib/CdmwPlatformMngtHostProxy.stub.hpp"

#include "platformdaemon/MonitorManager.hpp"
#include "platformdaemon/PlatformNotifier_impl.hpp"

#include "SystemMngt/platformlibrary/ProcessLauncher.hpp"

namespace Cdmw
{
namespace PlatformMngt
{


class ProcessCreationCallback;
class ProcessEndingCallback;


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

    friend class ProcessCreationCallback;
    friend class ProcessEndingCallback;

public:

    /**
    * Purpose:
    * <p>
    * Constructor.
    */
    Daemon_impl(CORBA::ORB_ptr orb, 
                const std::string& systemPortString,
                unsigned long notificationCallTimeout, 
                ProcessLauncher* processLauncher,
                size_t duration_time);


    /**
    * Purpose:
    * <p>
    * Activates this daemon servant and returns the associated CORBA reference.
    */
    CdmwPlatformMngt::Daemon_ptr activate();


    /**
    * Purpose:
    * <p> Returns the snapshot of the processes known by the daemon.
    */    
    CdmwPlatformMngt::PlatformSnapshot* createSnapshot()
        throw(CORBA::SystemException);
        

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
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/platform_notifier:1.0
    * attribute
    */
    virtual CdmwPlatformMngt::PlatformNotifier_ptr platform_notifier()
        throw(CORBA::SystemException);


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/create_process:1.0
    * operation
    */
    virtual void create_process(const char* process_name,
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
    virtual void kill_process(const char* process_name)
        throw(CdmwPlatformMngt::ProcessNotFound,
              CORBA::SystemException);


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
            const CdmwPlatformMngt::ApplicationAgentInfo& agent_info,
            CdmwPlatformMngt::Timeout creation_timeout)
        throw(CdmwPlatformMngt::CreationError,
            CdmwPlatformMngt::ApplicationAgentAlreadyExists,
            CdmwPlatformMngt::InvalidInfo,
            CdmwPlatformMngt::CreationTimeout,
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
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/create_host_probe:1.0
    * operation
    */
    virtual CdmwPlatformMngt::HostProbe_ptr create_host_probe(
            const CdmwPlatformMngt::HostProbeInfo& probe_info,
            CdmwPlatformMngt::Timeout creation_timeout,
            CORBA::Boolean respawn)
        throw(CdmwPlatformMngt::CreationError,
            CdmwPlatformMngt::HostProbeAlreadyExists,
            CdmwPlatformMngt::InvalidInfo,
            CdmwPlatformMngt::CreationTimeout,
            CORBA::SystemException);


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/search_host_probe:1.0
    * operation
    */
    virtual CdmwPlatformMngt::HostProbe_ptr search_host_probe()
        throw(CORBA::SystemException);


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/kill_host_probe:1.0
    * operation
    */
    virtual void kill_host_probe()
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
    * IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/reboot:1.0
    * operation
    */
    virtual void reboot()
        throw(CORBA::SystemException);


    /**
    * Purpose:
    * <p> kill all processes which have been created by the agent of the specified application.
    * The agent is not killed. 
    */
    virtual void kill_application_processes(const std::string& applicationName)
        throw(OutOfMemoryException,
              InternalErrorException);
        
private:


    /**
    * Purpose:
    * <p> Kills the specified process.
    */    
    void killProcess(OsSupport::OS::ProcessId processId);


    /**
    * Purpose:
    * <p> Indicates whether the daemon manages the specified simple process.
    */    
    bool hasSimpleProcess(OsSupport::OS::ProcessId processId);


    /**
    * Purpose:
    * <p> Indicates whether the daemon manages the specified simple process.
    *
    * @param processId contains the id of the process if true is returned.
    * @return true if the process is managed by the daemon.
    * 
    */    
    bool hasSimpleProcess(const std::string& processName,
        OsSupport::OS::ProcessId& processId);

    /**
    * Purpose:
    * <p> Stores the information about the process and notify the creation.
    */    
    void addSimpleProcess(OsSupport::OS::ProcessId processId,
        const std::string& processName)
        throw(OutOfMemoryException, InternalErrorException);


    /**
    * Purpose:
    * <p> Removes the information about the process and returns the process name.
    */    
    std::string removeSimpleProcess(OsSupport::OS::ProcessId processId);



    /**
    * Purpose:
    * <p> Kills the specified simple process.
    * If the process was already killed, removes the information
    * about the process.
    */    
    void killSimpleProcess(OsSupport::OS::ProcessId processId);


    /**
    * Purpose:
    * <p>
    */    
    CdmwPlatformMngt::PlatformInfo* createPlatformInfo(
            const std::string& processName,
            CdmwPlatformMngt::OSProcessStatus processStatus)
        throw(OutOfMemoryException);


private:

    typedef std::map<OsSupport::OS::ProcessId, std::string> SimpleProcesses;

    /**
    * The processes managed by this daemon.
    */
    SimpleProcesses m_simpleProcesses;

    /**
    * The mutex protecting concurrent access to m_processes.
    */
    OsSupport::Mutex m_mutex;

    /**
    * The process ending callback.
    */
    ProcessEndingCallback *m_processEndingCallback;

    /**
    * The associated monitor manager.
    */
    MonitorManager m_monitorManager;

    /**
    * The associated platform notifier.
    */
    PlatformNotifier_impl *m_platformNotifier;

    /**
    * The CORBA reference to the associated platform notifier.
    */
    CdmwPlatformMngt::PlatformNotifier_var m_platformNotifierRef;

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

    /*
     * request duration time (used only for Fault Tolerance)
     */
    size_t m_duration_time;
    
};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_DAEMON_IMPL_HPP

