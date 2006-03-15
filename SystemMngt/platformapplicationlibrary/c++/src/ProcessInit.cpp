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


#include "Foundation/orbsupport/OrbSupport.hpp"
#include "platformapplicationlibrary/ProcessInit.hpp"
#include "platformapplicationlibrary/Process_impl.hpp"
#include "platformapplicationlibrary/ManagedProcess_impl.hpp"
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
// InitProcessSuspensionCallback class.
// ----------------------------------------------------------------------
InitProcessSuspensionCallback::InitProcessSuspensionCallback(
    Application_impl* application )
: Callback()
{
    m_application = application;
}

void InitProcessSuspensionCallback::execute() throw()
{
    m_application->suspension_event();
}

// ----------------------------------------------------------------------
// InitProcessCompletionCallback class.
// ----------------------------------------------------------------------
InitProcessCompletionCallback::InitProcessCompletionCallback(
    Application_impl* application )
: Callback()
{
    m_application = application;
}

void InitProcessCompletionCallback::execute() throw()
{
    // Check all the processes have been initialised
    GraphElementNames elementNames
        = GraphUtility::getElementNames( (m_application->m_processInitGraph).in() );

    GraphElementNames::iterator elementNameIt;
    for( elementNameIt =  elementNames.begin();
         elementNameIt != elementNames.end();
         elementNameIt++ )
    {
        Process_impl* process
            = m_application->getProcess( elementNameIt->c_str() );
            
        CdmwPlatformMngt::ProcessType processType
            = process->type();

        CdmwPlatformMngt::ProcessStatus processStatus
            = process->get_internal_status();
            
        CORBA::Boolean processAutoended = process->is_auto_ending();

        if( ((processType != CdmwPlatformMngt::UNMANAGED_PROCESS) &&
             (processStatus != CdmwPlatformMngt::PROCESS_INITIALISED)) ||
            ((processType == CdmwPlatformMngt::UNMANAGED_PROCESS) && (!processAutoended) &&
             (processStatus != CdmwPlatformMngt::PROCESS_RUNNING)) )
        {
            m_application->non_completion_event();
            return;
        }
    }

    m_application->completion_event();
}

// ----------------------------------------------------------------------
// InitProcessTask class.
// ----------------------------------------------------------------------
InitProcessTask::InitProcessTask(
    const std::string& taskId,
    Sequencer* sequencer,
    TaskObserver* observer,
    size_t predecessorCount,
    Process_impl* process,
    const CdmwPlatformMngtBase::StartupKind& startup_kind,
    unsigned int step )
throw( OutOfMemoryException )
: SynchronisableTask( taskId, sequencer, observer, predecessorCount )
{
    try
    {
        m_process = process;
        m_startupKind = new CdmwPlatformMngtBase::StartupKind( startup_kind );
        m_step = step;
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
}

bool InitProcessTask::execute() throw()
{
    bool ret = false;
    const char* p_appliName = m_process->get_application()->get_element_name();
    const char* p_procKey = m_process->get_process_key();

    try
    {
        CdmwPlatformMngt::RequestStatus status;

        if( m_step > 0 )
        {
            LogMngr::logMessage(INF, MSG_ID_APP_PROC_NEXTSTEP,
                                p_procKey,p_appliName);
                                
            ManagedProcess_impl* managedProcess
                = dynamic_cast< ManagedProcess_impl* >( m_process );

            status = managedProcess->nextStep( m_step );
        }
        else
        {
            LogMngr::logMessage(INF, MSG_ID_APP_PROC_INIT,
                                p_procKey,p_appliName);
                            
            status = m_process->initialise( m_startupKind.in() );
        }
            
        if( status == CdmwPlatformMngt::REQUEST_SUCCEED )
        {
            LogMngr::logMessage(INF, MSG_ID_APP_PROC_INIT_STATUS_OK,
                            p_procKey,p_appliName);
                            
            ret = true;   
        }     

    }
    catch (const CdmwPlatformMngt::IncompatibleStatus &)
    {
        LogMngr::logMessage(INF, MSG_ID_APP_PROC_INIT_STATUS_ERR,
                            p_procKey,p_appliName);
    }
    catch (const CdmwPlatformMngt::InvalidStep &)
    {
        LogMngr::logMessage(INF, MSG_ID_APP_PROC_INIT_STEP_ERR,
                            p_procKey,p_appliName);
    }
    catch (const CdmwPlatformMngt::IncompatibleType &)
    {
        LogMngr::logMessage(INF, MSG_ID_APP_PROC_INIT_TYPE_ERR,
                            p_procKey,p_appliName);
    }
    catch (const CORBA::SystemException& e)
    {
        std::ostringstream exceptionInfo;
        exceptionInfo << e;
        
        LogMngr::logMessage(INF, MSG_ID_APP_PROC_INIT_SYSTEM_ERR,
                            exceptionInfo.str().c_str(),
                            p_procKey,p_appliName);
    }
    catch (...)
    {
        LogMngr::logMessage(INF, MSG_ID_APP_UNEXPECTED_EXCEPTION,
                            "InitProcessTask::execute()",
                            p_procKey,p_appliName);
    }
    
    return ret;
}

// ----------------------------------------------------------------------
// InitProcessTaskFactory class.
// ----------------------------------------------------------------------
InitProcessTaskFactory::InitProcessTaskFactory(
    Application_impl* application,
    const CdmwPlatformMngtBase::StartupKind& startup_kind,
    const SynchronisableTaskDefMap& taskDefMap,
    const TaskIdList& rootTasks )
throw( OutOfMemoryException )
: SynchronisableTaskFactory( taskDefMap, rootTasks )
{
    try
    {
        m_application = application;
        m_startupKind = new CdmwPlatformMngtBase::StartupKind( startup_kind );
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
}

SynchronisableTaskFactory* InitProcessTaskFactory::createFactory(
    Application_impl* application,
    const CdmwPlatformMngtBase::StartupKind& startup_kind,
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
            false,
            taskDefMap,
            rootTasks );

        std::auto_ptr<InitProcessTaskFactory> syncTaskFactory(
            new InitProcessTaskFactory(
                application, startup_kind, taskDefMap, rootTasks ) );

        return syncTaskFactory.release();
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
}

SynchronisableTask* InitProcessTaskFactory::createTask(
    const std::string& taskId,
    Sequencer* sequencer,
    TaskObserver* taskObserver )
throw( InvalidTaskIdException,
       OutOfMemoryException )
{
    
    const char* p_appliName = m_application->get_element_name();
    std::string processName;
    

    try
    {
        SynchronisableTaskDef syncTaskDef = getTaskDef( taskId );

        // Get the name and the step from the taskId
        
        unsigned int processStep;
        size_t separatorPos = taskId.find( "\\" );
        processName = taskId.substr( 0, separatorPos );
        processStep = atol( (taskId.substr( separatorPos+1 )).c_str() );
        

        // Search for the process
        Process_impl* process
            = m_application->getProcess( processName.c_str() );
        
        std::auto_ptr<InitProcessTask> syncTask(
            new InitProcessTask(
                taskId,
                sequencer,
                taskObserver,
                syncTaskDef.m_nbPredecessors,
                process,
                m_startupKind.in(),
                processStep ) );

    
        return syncTask.release();
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
                            "InitProcessTaskFactory::createTask",
                            processName.c_str(),p_appliName);
                            
        CDMW_THROW1( InvalidTaskIdException, taskId );
    }
}

} // End namespace PlatformMngt
} // End namespace Cdmw

