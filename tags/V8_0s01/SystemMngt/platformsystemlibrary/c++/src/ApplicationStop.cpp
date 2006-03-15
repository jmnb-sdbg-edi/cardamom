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
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformlibrary/GraphUtility.hpp"

#include "platformsystemlibrary/ApplicationStop.hpp"
#include "platformsystemlibrary/System_impl.hpp"
#include "platformsystemlibrary/SystemLogMessageId.hpp"

#include <sstream>

namespace Cdmw
{
namespace PlatformMngt
{

// ----------------------------------------------------------------------
// StopApplicationCompletionCallback class.
// ----------------------------------------------------------------------
StopApplicationCompletionCallback::StopApplicationCompletionCallback (
                 System_impl* system)
    : Callback()
{
    m_system = system;
}

StopApplicationCompletionCallback::~StopApplicationCompletionCallback ()
{
}


void StopApplicationCompletionCallback::execute()
  throw()
{
    m_system->completion_event();
}


// ----------------------------------------------------------------------
// StopApplicationTask class.
// ----------------------------------------------------------------------
StopApplicationTask::StopApplicationTask (const std::string& taskId,
                                          Sequencer* sequencer,
                                          TaskObserver* observer,
                                          size_t predecessorCount,
                                          CdmwPlatformMngt::Application_ptr application,
                                          CORBA::Boolean emergency)
            throw (OutOfMemoryException)
            : SynchronisableTask (taskId, sequencer, observer, predecessorCount)
{
    m_application = CdmwPlatformMngt::Application::_duplicate(application);
    m_emergency = emergency;
}

StopApplicationTask::~StopApplicationTask ()
  throw()
{
}


bool StopApplicationTask::execute()
  throw()
{
    bool ret = false;
    CORBA::String_var appliName = m_application->name();
    
    try
    {
        LogMngr::logMessage(INF, MSG_ID_SYS_APP_STOP,
                            appliName.in());
                            
        CdmwPlatformMngt::LongRequestStatus status;

        status = m_application->stop_and_wait (m_emergency);

        if (status == CdmwPlatformMngt::LONG_REQUEST_COMPLETED)
        {
            LogMngr::logMessage(INF, MSG_ID_SYS_APP_STOP_STATUS_OK,
                                appliName.in());
                            
            ret = true;
        }
        else
        {
            std::cout << appliName.in() 
                      << " <<ERROR>> stop and wait status not LONG_REQUEST_COMPLETED"
                      << std::endl
                      << "           status is " << status
                      << std::endl;
        }
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus &)
    {
        LogMngr::logMessage(INF, MSG_ID_SYS_APP_STOP_STATUS_ERR,
                            appliName.in());
    }
    catch (const CORBA::SystemException& e)
    {
        std::ostringstream exceptionInfo;
        exceptionInfo << e;
        
        LogMngr::logMessage(INF, MSG_ID_SYS_APP_STOP_SYSTEM_ERR,
                            exceptionInfo.str().c_str(),
                            appliName.in());
    }
    catch (...)
    {
        LogMngr::logMessage(INF, MSG_ID_SYS_UNEXPECTED_EXCEPTION,
                            "StopApplicationTask::execute()",
                            appliName.in());
    }

    return ret;
}


// ----------------------------------------------------------------------
// StopApplicationTaskFactory class.
// ----------------------------------------------------------------------


StopApplicationTaskFactory::StopApplicationTaskFactory (
                                System_impl* system,        
                                CORBA::Boolean emergency,
                                const SynchronisableTaskDefMap& taskDefMap,
                                const TaskIdList& rootTasks)
         : SynchronisableTaskFactory (taskDefMap, rootTasks)
{
    m_system = system;
    m_emergency = emergency;
}


StopApplicationTaskFactory::~StopApplicationTaskFactory ()
{
}


SynchronisableTaskFactory* StopApplicationTaskFactory::createFactory (
                                System_impl* system,
                                CORBA::Boolean emergency,
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

        std::auto_ptr <StopApplicationTaskFactory> syncTaskFactory (
                                new StopApplicationTaskFactory (
                                           system, emergency, 
                                           taskDefMap, rootTasks));

        // return the factory reference
        return syncTaskFactory.release();
    }
    catch (const std::bad_alloc&)
    {
        CDMW_THROW (OutOfMemoryException);
    }
}


SynchronisableTask* StopApplicationTaskFactory::createTask(
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
        std::auto_ptr <StopApplicationTask> syncTask (
                                new StopApplicationTask (
                                           taskId,
                                           sequencer,
                                           taskObserver,
                                           syncTaskDef.m_nbPredecessors,
                                           application.in(),
                                           m_emergency));

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
                            "StopApplicationTaskFactory::createTask",
                            appliName.in());
                            
        CDMW_THROW1( InvalidTaskIdException, taskId );
    } 
}

} // End namespace PlatformMngt
} // End namespace Cdmw

