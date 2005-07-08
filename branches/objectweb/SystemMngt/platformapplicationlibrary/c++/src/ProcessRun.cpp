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
#include "platformapplicationlibrary/ProcessRun.hpp"
#include "platformapplicationlibrary/ProcessProxy_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/ApplicationLogMessageId.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"

#include <sstream>

namespace Cdmw
{
namespace PlatformMngt
{

// ----------------------------------------------------------------------
// RunProcessCompletionCallback class.
// ----------------------------------------------------------------------
RunProcessCompletionCallback::RunProcessCompletionCallback(
    Application_impl* application )
: Callback()
{
    m_application = application;
}

void RunProcessCompletionCallback::execute() throw()
{
    m_application->completion_event();
}

// ----------------------------------------------------------------------
// RunProcessTask class.
// ----------------------------------------------------------------------
RunProcessTask::RunProcessTask(
    const std::string& taskId,
    TaskObserver* observer,
    ProcessProxy_impl* processProxy )
: Task( taskId, observer )
{
    m_processProxy = processProxy;
}

bool RunProcessTask::execute() throw()
{
    bool ret = false;
    const char* p_appliName = m_processProxy->m_application->get_applicationName();
    const char* p_procName = m_processProxy->get_processName();

    try
    {
        LogMngr::logMessage(INF, MSG_ID_APP_PROC_RUN,
                            p_procName,p_appliName);
                                
        CdmwPlatformMngt::RequestStatus status;
        
        status = m_processProxy->run();
        
        if( status == CdmwPlatformMngt::REQUEST_SUCCEED )
        {
            LogMngr::logMessage(INF, MSG_ID_APP_PROC_RUN_STATUS_OK,
                                p_procName,p_appliName);
                            
            ret = true;
        }
            
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus &)
    {
        LogMngr::logMessage(INF, MSG_ID_APP_PROC_RUN_STATUS_ERR,
                            p_procName,p_appliName);
    }
    catch (const CdmwPlatformMngt::NotReadyToRun &)
    {
        LogMngr::logMessage(INF, MSG_ID_APP_PROC_RUN_NOT_READY_ERR,
                            p_procName,p_appliName);
    }
    catch (const CdmwPlatformMngt::IncompatibleType &)
    {
        LogMngr::logMessage(INF, MSG_ID_APP_PROC_RUN_TYPE_ERR,
                            p_procName,p_appliName);
    }
    catch (const CORBA::SystemException& e)
    {
        std::ostringstream exceptionInfo;
        exceptionInfo << e;
        
        LogMngr::logMessage(INF, MSG_ID_APP_PROC_RUN_SYSTEM_ERR,
                            exceptionInfo.str().c_str(),
                            p_procName,p_appliName);
    }
    catch (...)
    {
        LogMngr::logMessage(INF, MSG_ID_APP_UNEXPECTED_EXCEPTION,
                            "RunProcessTask::execute()",
                            p_procName,p_appliName);
    }

    return ret;
}

// ----------------------------------------------------------------------
// RunProcessTaskFactory class.
// ----------------------------------------------------------------------
RunProcessTaskFactory::RunProcessTaskFactory(
    Application_impl* application)
: TaskFactory()
{
    m_application = application;
}

TaskFactory* RunProcessTaskFactory::createFactory(
    Application_impl* application )
throw( OutOfMemoryException )
{
    try
    {
        std::auto_ptr<RunProcessTaskFactory> taskFactory(
            new RunProcessTaskFactory( application ) );

        return taskFactory.release();
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
}

Task* RunProcessTaskFactory::createTask(
    const std::string& taskId,
    TaskObserver* taskObserver )
throw( InvalidTaskIdException,
       OutOfMemoryException )    
{
    const char* p_appliName = m_application->get_applicationName();
    std::string processName = "";
    
    try
    {
        ProcessProxy_impl* process
            = m_application->getProcessProxy( taskId.c_str() );
            
        // get process name
        processName = process->get_processName();
        
        std::auto_ptr<RunProcessTask> task(
            new RunProcessTask(
                taskId,
                taskObserver,
                process ) );

        return task.release();
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
    catch( const CdmwPlatformMngt::ProcessNotFound& )
    {
        CDMW_THROW1( InvalidTaskIdException, taskId );
    }
    catch (...)
    {
        LogMngr::logMessage(INF, MSG_ID_APP_UNEXPECTED_EXCEPTION,
                            "RunProcessTaskFactory::createTask",
                            processName.c_str(),p_appliName);
                            
        CDMW_THROW1( InvalidTaskIdException, taskId );
    }
}

} // End namespace PlatformMngt
} // End namespace Cdmw

