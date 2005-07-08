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


#ifndef INCL_PLATFORMMNGT_MONITOR_MANAGER_HPP 
#define INCL_PLATFORMMNGT_MONITOR_MANAGER_HPP 

#include "Foundation/common/System.hpp"

#include <map>
#include <string>

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/ossupport/ProcessCallback.hpp"

#include "Foundation/orbsupport/CORBA.hpp"

#ifdef CDMW_USE_FAULTTOLERANCE
#include "FaultTolerance/idllib/FT.stub.hpp"
#include "FaultTolerance/idllib/CdmwFTManager.stub.hpp"
#endif

#include "SystemMngt/platformlibrary/MessageQueue.hpp"
#include "SystemMngt/platformlibrary/PullMonitor.hpp"

#include "idllib/CdmwPlatformMngtDaemon.skel.hpp"

#include "idllib/CdmwPlatformMngtMonitoringObserver.stub.hpp"
#include "idllib/CdmwPlatformMngtApplication.stub.hpp"
#include "idllib/CdmwPlatformMngtApplicationAgent.stub.hpp"
#include "idllib/CdmwPlatformMngtHostProxy.stub.hpp"

#include "platformdaemon/PlatformNotifier_impl.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class Daemon_impl;


enum MonitorableProcessType
{
    APPLICATION,
    APPLICATION_AGENT,
    HOST_PROBE
};


class MonitorableProcessInfo
{

public:

    std::string m_name;

    MonitorableProcessType m_processType;

    std::string m_featureName;

    CORBA::Object_var m_processObject;

    CdmwPlatformMngt::MonitorableProcessObserver_var m_processObserver;

    PullMonitor *m_monitor;

#ifdef CDMW_USE_FAULTTOLERANCE
    std::string m_replicationManagerIOGR;
#endif

public:

    /**
    * Purpose:
    * <p>
    * Constructor.
    * 
    */ 
    MonitorableProcessInfo();

    /**
    * Purpose:
    * <p>
    * Constructor.
    */ 
#ifdef CDMW_USE_FAULTTOLERANCE
    MonitorableProcessInfo(const std::string& name,
        MonitorableProcessType processType,
        const std::string& featureName,
        CORBA::Object_ptr processObject,
        CdmwPlatformMngt::MonitorableProcessObserver_ptr processObserver,
        const std::string faultManagerCorbaloc);
#else
    MonitorableProcessInfo(const std::string& name,
        MonitorableProcessType processType,
        const std::string& featureName,
        CORBA::Object_ptr processObject,
        CdmwPlatformMngt::MonitorableProcessObserver_ptr processObserver);
#endif    

    /**
    * Purpose:
    * <p>
    * Copy constructor.
    *
    */ 
    MonitorableProcessInfo(const MonitorableProcessInfo& rhs);

    /**
    * Purpose:
    * <p>
    * Destructor.
    */ 
    virtual ~MonitorableProcessInfo();

    /**
    * Purpose:
    * <p>
    * Assignment operator.
    * 
    */ 
    MonitorableProcessInfo& operator=(const MonitorableProcessInfo& rhs);

};


class MonitorManager;


/**
* Class that handles the ending of the monitorable process :
* the process is removed, the monitoring is stopped, the ending is notified
* to the observer.
*/
class MonitorableProcessEndingCallback : public OsSupport::ProcessCallback
{
public:

    MonitorableProcessEndingCallback(MonitorManager* monitorManager, Daemon_impl* daemon);

    virtual ~MonitorableProcessEndingCallback();

    virtual void execute(OsSupport::OS::ProcessId processId) throw();

private:

    MonitorManager *m_monitorManager;
    
    Daemon_impl *m_daemon;

};


/**
* Purpose:
* <p>
*/
class MonitorManager : public OsSupport::Thread
{

    friend class MonitorableProcessEndingCallback;

public:


    /**
    * Purpose:
    * <p>
    * Constructor.
    */ 
    MonitorManager(Daemon_impl* daemon);


    /**
    * Purpose:
    * <p>
    * Destructor.
    */ 
    virtual ~MonitorManager() throw();

    /**
    * Returns the callback that handles the ending of monitorable processes.
    */
    MonitorableProcessEndingCallback* getProcessEndingCallback();

    /**
    * Adds the monitorable process.
    *
    * @param processId the identifier of the process.
    * @param info the information about the monitorable process.
    */
    void addMonitorableProcess(OsSupport::OS::ProcessId processId,
            const MonitorableProcessInfo& info)
        throw(OutOfMemoryException);

    /**
    * Starts the monitoring of the specified process.
    *
    * @param processId the identifier of the process.
    * @param monitoringInfo the information about the monitoring to perform.
    * @exception BadParameterException if processId doesn't denote an existing monitorable process.
    */
    void startMonitoring(OsSupport::OS::ProcessId processId,
            const CdmwPlatformMngt::MonitoringInfo& monitoringInfo)
        throw(BadParameterException);

    /**
    * Removes the monitorable process and stops its monitoring.
    * Used before the kill of the process.
    *
    * @param processId the identifier of the process.
    */
    void stopMonitoring(OsSupport::OS::ProcessId processId);


    /**
    */
    void stop();


    /**
    * Purpose:
    * <p> Indicates whether the MonitorManager manages the specified process.
    */    
    bool hasMonitorableProcess(OsSupport::OS::ProcessId processId);


    /**
    * Purpose:
    * <p> Indicates whether the MonitorManager manages the process
    * identified by the specified name and type.
    *
    * @param processId contains the id of the process if true is returned.
    * @param processObject contains the process object if true is returned.
    * @return true if the process is managed by the MonitorManager.
    */    
    bool hasMonitorableProcess(const std::string& name,
            MonitorableProcessType processType,
            OsSupport::OS::ProcessId& processId,
            CORBA::Object_out processObject);

protected:
    /**
     *Purpose:
     *<p> MonitorManager's activity.
     */
    void run() throw();

private:

    /**
    * Removes the monitorable process.
    *
    * @param processId the identifier of the process.
    * @exception BadParameterException if processId doesn't denote an existing monitorable process.
    */
    MonitorableProcessInfo removeMonitorableProcess(OsSupport::OS::ProcessId processId)
        throw(BadParameterException);


    /**
    * Stops the monitor.
    */
    void stopMonitoring(PullMonitor* monitor);


//    /**
//    * Purpose:
//    * <p> Stops the monitoring for the specified process.
//    */    
//    void addMonitorToStop(OsSupport::OS::ProcessId processId);


private:


    typedef std::map<OsSupport::OS::ProcessId, MonitorableProcessInfo> MonitorableProcesses;


    /**
    * The processes managed by this daemon.
    */
    MonitorableProcesses m_monitorableProcesses;

    /**
    * The process ending callback.
    */
    MonitorableProcessEndingCallback *m_processEndingCallback;

    /**
    * The processes managed by this daemon.
    */
    bool m_stopped;

    /**
    * The mutex protecting concurrent access to m_monitorableProcesses.
    */
    OsSupport::Mutex m_mutex;

//    /**
//    * The monitors to stop.
//    */
//    MessageQueue<OsSupport::OS::ProcessId> m_monitorsToStop;

};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_MONITOR_MANAGER_HPP

