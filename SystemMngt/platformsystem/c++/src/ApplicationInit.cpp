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
#include "SystemMngt/platformlibrary/GraphUtility.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "platformsystem/ApplicationInit.hpp"
#include "platformsystem/System_impl.hpp"
#include "platformsystem/SystemLogMessageId.hpp"

#include <sstream>

namespace Cdmw
{
namespace PlatformMngt
{

// ----------------------------------------------------------------------
// InitApplicationSuspensionCallback class.
// ----------------------------------------------------------------------
InitApplicationSuspensionCallback::InitApplicationSuspensionCallback (
                                       System_impl* system)
    : Callback()
{
    m_system = system;
}


InitApplicationSuspensionCallback::~InitApplicationSuspensionCallback ()
{
}


void InitApplicationSuspensionCallback::execute()
  throw()
{
    m_system->suspension_event();
}

// ----------------------------------------------------------------------
// InitApplicationCompletionCallback class.
// ----------------------------------------------------------------------
InitApplicationCompletionCallback::InitApplicationCompletionCallback (
                                       System_impl* system)
    : Callback()
{
    m_system = system;
}


InitApplicationCompletionCallback::~InitApplicationCompletionCallback ()
{
}


void InitApplicationCompletionCallback::execute()
  throw()
{
    m_system->completion_event();
}



// ----------------------------------------------------------------------
// InitApplicationTask class.
// ----------------------------------------------------------------------
InitApplicationTask::InitApplicationTask (const std::string& taskId,
                                          Sequencer* sequencer,
                                          TaskObserver* observer,
                                          size_t predecessorCount,
                                          CdmwPlatformMngt::Application_ptr application,
                                          const CdmwPlatformMngtBase::StartupKind& startup_kind)
             throw (OutOfMemoryException)
             : SynchronisableTask (taskId, sequencer, observer, predecessorCount)
{
    try
    {
        m_application = CdmwPlatformMngt::Application::_duplicate(application);
        m_startupKind = new CdmwPlatformMngtBase::StartupKind (startup_kind);
    }
    catch (const std::bad_alloc&)
    {
        CDMW_THROW (OutOfMemoryException);
    }
}


InitApplicationTask::~InitApplicationTask ()
  throw()
{
}


bool InitApplicationTask::execute()
  throw()
{
    bool ret = false;
    CORBA::String_var appliName = m_application->name();
    
    try
    {
        LogMngr::logMessage(INF, MSG_ID_SYS_APP_INIT,
                            appliName.in());
                            
        CdmwPlatformMngt::LongRequestStatus status;

        status = m_application->initialise_and_wait (m_startupKind.in());

        if (status == CdmwPlatformMngt::LONG_REQUEST_COMPLETED)
        {
            LogMngr::logMessage(INF, MSG_ID_SYS_APP_INIT_STATUS_OK,
                                appliName.in());
                                
            ret = true;
        }
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus &)
    {
        LogMngr::logMessage(INF, MSG_ID_SYS_APP_INIT_STATUS_ERR,
                            appliName.in());
    }
    catch (const CORBA::SystemException& e)
    {
        std::ostringstream exceptionInfo;
        exceptionInfo << e;
        
        LogMngr::logMessage(INF, MSG_ID_SYS_APP_INIT_SYSTEM_ERR,
                            exceptionInfo.str().c_str(),
                            appliName.in());
    }
    catch (...)
    {
        LogMngr::logMessage(INF, MSG_ID_SYS_UNEXPECTED_EXCEPTION,
                            "InitApplicationTask::execute()",
                            appliName.in());
    }
    
    
    return ret;
}



// ----------------------------------------------------------------------
// InitApplicationTaskFactory class.
// ----------------------------------------------------------------------
InitApplicationTaskFactory::InitApplicationTaskFactory (
                                System_impl* system,        
                                const CdmwPlatformMngtBase::StartupKind& startup_kind,
                                const SynchronisableTaskDefMap& taskDefMap,
                                const TaskIdList& rootTasks)
             throw (OutOfMemoryException)
             : SynchronisableTaskFactory (taskDefMap, rootTasks)
{
    try
    {
        m_system = system;
        m_startupKind = new CdmwPlatformMngtBase::StartupKind (startup_kind);
    }
    catch (const std::bad_alloc&)
    {
        CDMW_THROW (OutOfMemoryException);
    }
}


InitApplicationTaskFactory::~InitApplicationTaskFactory ()
{
}


SynchronisableTaskFactory* InitApplicationTaskFactory::createFactory (
                               System_impl* system,
                               const CdmwPlatformMngtBase::StartupKind& startup_kind,
                               const CdmwPlatformMngt::GraphElements& graphElements)
             throw (OutOfMemoryException)
{
    try
    {
        // create TaskDef map and root task list from graph
        SynchronisableTaskDefMap taskDefMap;
        TaskIdList rootTasks;

        GraphUtility::getTaskDefinitions (graphElements,            
                                          true,
                                          taskDefMap,
                                          rootTasks);

        std::auto_ptr <InitApplicationTaskFactory> syncTaskFactory (
                                 new InitApplicationTaskFactory (
                                            system, startup_kind, 
                                            taskDefMap, rootTasks));

        // return the factory reference
        return syncTaskFactory.release();
    }
    catch (const std::bad_alloc&)
    {
        CDMW_THROW (OutOfMemoryException);
    }
}



SynchronisableTask* InitApplicationTaskFactory::createTask (
                        const std::string& taskId,
                        Sequencer* sequencer,
                        TaskObserver* taskObserver)
             throw (InvalidTaskIdException, 
                    OutOfMemoryException)    
{
    CORBA::String_var appliName = CORBA::string_dup ("");
    
    try
    {
        // get synchronisable task definition
        SynchronisableTaskDef syncTaskDef = getTaskDef (taskId);
        
        // get application
        CdmwPlatformMngt::Application_var application =
              m_system->get_application (taskId.c_str());
              
        // get application name
        appliName = application->name();
        
        // create synchronisable task
        std::auto_ptr <InitApplicationTask> syncTask (
                                 new InitApplicationTask (
                                            taskId,
                                            sequencer,
                                            taskObserver,
                                            syncTaskDef.m_nbPredecessors,
                                            application.in(),
                                            m_startupKind.in()));

        // return synchronisable task
        return syncTask.release();
    }
    catch (const CdmwPlatformMngt::ApplicationNotFound&)
    {
        CDMW_THROW1(InvalidTaskIdException, taskId);
    }
    catch (const std::bad_alloc&)
    {
        CDMW_THROW (OutOfMemoryException);
    }
    catch (...)
    {
        LogMngr::logMessage(INF, MSG_ID_SYS_UNEXPECTED_EXCEPTION,
                            "InitApplicationTaskFactory::createTask",
                            appliName.in());
                            
        CDMW_THROW1( InvalidTaskIdException, taskId );
    }   
}

} // End namespace PlatformMngt
} // End namespace Cdmw

