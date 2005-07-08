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


#include "platformdaemon/MonitorManager.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/ossupport/ProcessCallback.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"
#include "platformdaemon/DaemonLogMessageId.hpp"

#include "platformdaemon/Daemon_impl.hpp"

#include <sstream>
#include <memory>

namespace Cdmw
{
namespace PlatformMngt
{


// MonitorableProcessInfo implementation

MonitorableProcessInfo::MonitorableProcessInfo()
{
    m_monitor = NULL;
}


#ifdef CDMW_USE_FAULTTOLERANCE
MonitorableProcessInfo::MonitorableProcessInfo(const std::string& name,
    MonitorableProcessType processType,
        const std::string& featureName,
        CORBA::Object_ptr processObject,
        CdmwPlatformMngt::MonitorableProcessObserver_ptr processObserver,
        const std::string replicationManagerIOGR)
    : m_name(name), m_processType(processType), m_featureName(featureName), m_replicationManagerIOGR(replicationManagerIOGR)
#else
MonitorableProcessInfo::MonitorableProcessInfo(const std::string& name,
    MonitorableProcessType processType,
        const std::string& featureName,
        CORBA::Object_ptr processObject,
        CdmwPlatformMngt::MonitorableProcessObserver_ptr processObserver)
    : m_name(name), m_processType(processType), m_featureName(featureName)
#endif
{
    m_processObject = CORBA::Object::_duplicate(processObject);
    m_processObserver =
        CdmwPlatformMngt::MonitorableProcessObserver::_duplicate(processObserver);
    m_monitor = NULL;
}


MonitorableProcessInfo::MonitorableProcessInfo(const MonitorableProcessInfo& rhs)
{

    m_name = rhs.m_name;
    m_featureName = rhs.m_featureName;
    m_processType = rhs.m_processType;
    m_processObject = CORBA::Object::_duplicate(rhs.m_processObject);
    m_processObserver =
        CdmwPlatformMngt::MonitorableProcessObserver::_duplicate(rhs.m_processObserver);
#ifdef CDMW_USE_FAULTTOLERANCE
    m_replicationManagerIOGR = rhs.m_replicationManagerIOGR;
#endif
    m_monitor = rhs.m_monitor;
}


MonitorableProcessInfo::~MonitorableProcessInfo()
{

}


MonitorableProcessInfo& MonitorableProcessInfo::operator=(const MonitorableProcessInfo& rhs)
{

    std::string tempName(rhs.m_name);
    std::string tempfeatureName(rhs.m_featureName);

    m_name.swap(tempName);
    m_featureName.swap(tempfeatureName);
    m_processType = rhs.m_processType;
    m_processObject = CORBA::Object::_duplicate(rhs.m_processObject);
    m_processObserver =
        CdmwPlatformMngt::MonitorableProcessObserver::_duplicate(rhs.m_processObserver);
#ifdef CDMW_USE_FAULTTOLERANCE
    m_replicationManagerIOGR = rhs.m_replicationManagerIOGR;
#endif
    m_monitor = rhs.m_monitor;

    return *this;

}


MonitorableProcessEndingCallback::MonitorableProcessEndingCallback(MonitorManager* monitorManager, Daemon_impl* daemon)
    : m_monitorManager(monitorManager), m_daemon(daemon)
{
}

MonitorableProcessEndingCallback::~MonitorableProcessEndingCallback()
{
}

void MonitorableProcessEndingCallback::execute(OsSupport::OS::ProcessId processId) throw()
{
    try
    {

        MonitorableProcessInfo info =
            m_monitorManager->removeMonitorableProcess(processId);

        // logs the process ending
        switch(info.m_processType)
        {
            case APPLICATION:
                LogMngr::logMessage(INF, MSG_ID_DAEMON_PLATFORM_APPLICATION_ENDED, info.m_name);
            break;

            case APPLICATION_AGENT:
                LogMngr::logMessage(INF, MSG_ID_DAEMON_APPLICATION_AGENT_ENDED, info.m_name);
            break;

            case HOST_PROBE:
                LogMngr::logMessage(INF, MSG_ID_DAEMON_HOST_PROBE_ENDED);
            break;
        }

        m_monitorManager->stopMonitoring(processId);

#ifdef CDMW_USE_FAULTTOLERANCE
        // Get the replication manager
        CORBA::Object_var rm_obj = m_daemon->resolve_corbaloc( info.m_replicationManagerIOGR.c_str() );
        
        if( CORBA::is_nil( rm_obj.in() ) )
            CDMW_THROW2(Cdmw::Exception,
                        Cdmw::Exception::FATAL,
                        "Invalid fault manager corbaloc" );

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
        
        FT::FaultNotifier_var faultNotifier = FT::FaultNotifier::_narrow( fn_obj.in() );        
        
        if (!CORBA::is_nil(faultNotifier.in()))
        {
            if (info.m_processType == APPLICATION_AGENT) { 

                ::FT::Location location;
                std::string my_host_name = OsSupport::OS::get_hostname();                
                location.length( 2 );
                location[0].kind = CORBA::string_dup( "hostname" );
                location[0].id = CORBA::string_dup( my_host_name.c_str() );
                location[1].kind = CORBA::string_dup( "applicationname" );
                location[1].id = CORBA::string_dup( info.m_name.c_str() );

                // Prepare the event for the fault notification
                CosNotification::StructuredEvent fault_event;
                fault_event.header.fixed_header.event_type.domain_name = CORBA::string_dup( "FT_CORBA" );
                fault_event.header.fixed_header.event_type.type_name = CORBA::string_dup( "ObjectCrashFault" );
                fault_event.filterable_data.length( 2 );
                fault_event.filterable_data[0].name = CORBA::string_dup( "FTDomainId" );
                fault_event.filterable_data[0].value <<= "thalesgroup.com";
                fault_event.filterable_data[1].name = CORBA::string_dup( "Location" );
                fault_event.filterable_data[1].value <<= location;
                
                faultNotifier->push_structured_fault( fault_event );

            } else {
                
                std::cout<<__FILE__<<" "<<__LINE__<<std::endl;
                
            }

        } else {
            
            std::cout<<__FILE__<<" "<<__LINE__<<std::endl;
            
        }

#else
        CdmwPlatformMngt::MonitorableProcessObserver_var processObserver =
            info.m_processObserver;
        
        if (!CORBA::is_nil(processObserver.in()))
        {
            CdmwPlatformMngtBase::TimeStamp timeStamp =
                EventHeaderFactory::createTimeStamp();
            processObserver->notify_ending((info.m_featureName).c_str(), timeStamp);
        }
#endif



        if (info.m_processType == APPLICATION_AGENT) { 
            // Agent is dead. Kill all processes it has created.
            m_daemon->kill_application_processes(info.m_name);
        }
        
    }
    catch(...)
    {

    }

}


MonitorManager::MonitorManager(Daemon_impl* daemon)
    : m_stopped(false)
{
    m_processEndingCallback = new MonitorableProcessEndingCallback(this, daemon);
}



MonitorManager::~MonitorManager()
  throw()
{
    delete m_processEndingCallback;
}


void MonitorManager::run()
  throw()
{
    // TODO
    // manages the actual monitoring
}


MonitorableProcessEndingCallback* MonitorManager::getProcessEndingCallback()
{
    CDMW_ASSERT(m_processEndingCallback != NULL);
    return m_processEndingCallback;
}


bool MonitorManager::hasMonitorableProcess(OsSupport::OS::ProcessId processId)
{

    bool ret = false;

    CDMW_MUTEX_GUARD(m_mutex);

    MonitorableProcesses::iterator it = m_monitorableProcesses.find(processId);
    
    if (it != m_monitorableProcesses.end())
    {
        ret = true;
    }

    return ret;

}


bool MonitorManager::hasMonitorableProcess(const std::string& name,
    MonitorableProcessType processType,
    OsSupport::OS::ProcessId& processId,
    CORBA::Object_out processObject)
{

    bool ret = false;

    for (MonitorableProcesses::iterator it = m_monitorableProcesses.begin(); it != m_monitorableProcesses.end(); ++it)
    {

        if (processType == (it->second).m_processType)
        {
            if (!name.compare((it->second).m_name))
            {
                ret = true;
                processId = it->first;
                processObject = CORBA::Object::_duplicate((it->second).m_processObject.in());
                break;
            }
        }

    }

    if (!ret)
        processObject = CORBA::Object::_nil();

    return ret;

}


void MonitorManager::addMonitorableProcess(OsSupport::OS::ProcessId processId,
    const MonitorableProcessInfo& info)
    throw(OutOfMemoryException)
{

    CDMW_MUTEX_GUARD(m_mutex);

    try
    {
        m_monitorableProcesses[processId] = info;
    }
    catch(const std::bad_alloc&)
    {
        CDMW_THROW(OutOfMemoryException);
    }

}


void MonitorManager::startMonitoring(OsSupport::OS::ProcessId processId,
        const CdmwPlatformMngt::MonitoringInfo& monitoringInfo)
    throw(BadParameterException)
{
    // TODO

}


void MonitorManager::stopMonitoring(OsSupport::OS::ProcessId processId)
{

    CDMW_MUTEX_GUARD(m_mutex);

    MonitorableProcesses::iterator it = m_monitorableProcesses.find(processId);
    
    if (it != m_monitorableProcesses.end())
    {
        PullMonitor *monitor = (it->second).m_monitor;
        stopMonitoring(monitor);
    }

}


MonitorableProcessInfo MonitorManager::removeMonitorableProcess(OsSupport::OS::ProcessId processId)
    throw(BadParameterException)
{

    MonitorableProcessInfo info;

    {

        CDMW_MUTEX_GUARD(m_mutex);

        MonitorableProcesses::iterator it = m_monitorableProcesses.find(processId);
        
        if (it == m_monitorableProcesses.end())
        {
            CDMW_THROW2(BadParameterException, "processId", "not found");
        }

        info = it->second;

        m_monitorableProcesses.erase(processId);

    }

    return info;

}


void MonitorManager::stopMonitoring(PullMonitor* monitor)
{
    // TODO

}


} // End namespace PlatformMngt
} // End namespace Cdmw


