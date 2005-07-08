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


#include "Foundation/orbsupport/OrbSupport.hpp"
#include "platformapplicationlibrary/ProcessStop.hpp"
#include "platformapplicationlibrary/ProcessProxy_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/ApplicationLogMessageId.hpp"
#include "SystemMngt/platformlibrary/GraphUtility.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"

#include <sstream>

namespace Cdmw
{
namespace PlatformMngt
{

// ----------------------------------------------------------------------
// StopProcessCompletionCallback class.
// ----------------------------------------------------------------------
StopProcessCompletionCallback::StopProcessCompletionCallback(
    Application_impl* application )
: Callback()
{
    m_application = application;
}

void StopProcessCompletionCallback::execute() throw()
{
    m_application->completion_event();
}

// ----------------------------------------------------------------------
// StopProcessTask class.
// ----------------------------------------------------------------------
StopProcessTask::StopProcessTask(
    const std::string& taskId,
    Sequencer* sequencer,
    TaskObserver* observer,
    size_t predecessorCount,
    ProcessProxy_impl* processProxy,
    CORBA::Boolean emergency )
: SynchronisableTask( taskId, sequencer, observer, predecessorCount )
{
    m_processProxy = processProxy;
    m_emergency = emergency;
}

bool StopProcessTask::execute() throw()
{
    bool ret = false;
    const char* p_appliName = m_processProxy->m_application->get_applicationName();
    const char* p_procName = m_processProxy->get_processName();

    try
    {
        LogMngr::logMessage(INF, MSG_ID_APP_PROC_STOP,
                            p_procName,p_appliName);
                            
        CdmwPlatformMngt::RequestStatus status;

        status = m_processProxy->stop( m_emergency );

        if( status == CdmwPlatformMngt::REQUEST_SUCCEED )
        {
            LogMngr::logMessage(INF, MSG_ID_APP_PROC_STOP_STATUS_OK,
                                p_procName,p_appliName);
                                
            ret = true;
        }
    }
    catch (const CORBA::SystemException& e)
    {
        std::ostringstream exceptionInfo;
        exceptionInfo << e;
        
        LogMngr::logMessage(ERR, MSG_ID_APP_PROC_STOP_SYSTEM_ERR,
                            exceptionInfo.str().c_str(),
                            p_procName,p_appliName);
    }
    catch (...)
    {
        LogMngr::logMessage(INF, MSG_ID_APP_UNEXPECTED_EXCEPTION,
                            "StopProcessTask::execute()",
                            p_procName,p_appliName);
    }

    return ret;
}

// ----------------------------------------------------------------------
// StopProcessTaskFactory class.
// ----------------------------------------------------------------------
StopProcessTaskFactory::StopProcessTaskFactory(
    Application_impl* application,
    CORBA::Boolean emergency,
    const SynchronisableTaskDefMap& taskDefMap,
    const TaskIdList& rootTasks )
: SynchronisableTaskFactory( taskDefMap, rootTasks )
{
    m_application = application;
    m_emergency = emergency;
}

SynchronisableTaskFactory* StopProcessTaskFactory::createFactory(
    Application_impl* application,
    CORBA::Boolean emergency,
    const CdmwPlatformMngt::GraphElements& graphElements )
throw( OutOfMemoryException,
       CORBA::SystemException )
{
    try
    {
        SynchronisableTaskDefMap taskDefMap;
        TaskIdList rootTasks;

        GraphUtility::getTaskDefinitions(
            graphElements,
            true,
            taskDefMap,
            rootTasks );

        std::auto_ptr<StopProcessTaskFactory> syncTaskFactory(
            new StopProcessTaskFactory(
                application, emergency, taskDefMap, rootTasks ) );

        return syncTaskFactory.release();
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
}

SynchronisableTask* StopProcessTaskFactory::createTask(
    const std::string& taskId,
    Sequencer* sequencer,
    TaskObserver* taskObserver )
throw( InvalidTaskIdException,
       OutOfMemoryException )
{
    const char* p_appliName = m_application->get_applicationName();
    std::string processName = "";
    
    try
    {
        SynchronisableTaskDef syncTaskDef = getTaskDef( taskId );

        // Search for the process
        ProcessProxy_impl* process
            = m_application->getProcessProxy( taskId.c_str() );
            
        // get process name
        processName = process->get_processName();
        

        std::auto_ptr<StopProcessTask> syncTask(
            new StopProcessTask(
                taskId,
                sequencer,
                taskObserver,
                syncTaskDef.m_nbPredecessors,
                process,
                m_emergency ) );

        return syncTask.release();
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
    catch (...)
    {
        LogMngr::logMessage(INF, MSG_ID_APP_UNEXPECTED_EXCEPTION,
                            "StopProcessTaskFactory::createTask",
                            processName.c_str(),p_appliName);
                            
        CDMW_THROW1( InvalidTaskIdException, taskId );
    }
}

} // End namespace PlatformMngt
} // End namespace Cdmw

