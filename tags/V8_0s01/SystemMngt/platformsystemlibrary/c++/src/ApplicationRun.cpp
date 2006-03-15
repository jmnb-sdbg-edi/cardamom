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

#include "platformsystemlibrary/ApplicationRun.hpp"
#include "platformsystemlibrary/System_impl.hpp"
#include "platformsystemlibrary/SystemLogMessageId.hpp"

#include <sstream>

namespace Cdmw
{
namespace PlatformMngt
{



// ----------------------------------------------------------------------
// RunApplicationTask class.
// ----------------------------------------------------------------------
RunApplicationTask::RunApplicationTask (const std::string& taskId,        
                                        TaskObserver* observer,
                                        CdmwPlatformMngt::ApplicationControl_ptr app_control,
                                        CdmwPlatformMngt::Application_ptr application)
   : Task (taskId, observer)
{
    m_app_control = CdmwPlatformMngt::ApplicationControl::_duplicate(app_control);
    m_application = CdmwPlatformMngt::Application::_duplicate(application);
}

RunApplicationTask::~RunApplicationTask ()
  throw()
{
}

bool RunApplicationTask::execute()
  throw()
{
    bool ret = false;
    CORBA::String_var appliName = m_application->name();

    try
    {
        // get the application current status
        
        /***
        CORBA::String_var statusInfo;
        CdmwPlatformMngt::ApplicationStatus appStatus =
                      m_application->get_status(statusInfo.out());
        ***/
        
        CdmwPlatformMngt::ApplicationStatus appStatus =              
                      m_app_control->get_internal_status();

        // if application is already running, do nothing
        if (appStatus == CdmwPlatformMngt::APPLICATION_RUNNING)
        {
            ret = true;
        }
        
        else
        {        
            LogMngr::logMessage(INF, MSG_ID_SYS_APP_START,
                            appliName.in());
                            
            m_application->run();
            ret = true;
        
            LogMngr::logMessage(INF, MSG_ID_SYS_APP_START_STATUS_OK,
                                appliName.in());
        }
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus &)
    {
        LogMngr::logMessage(INF, MSG_ID_SYS_APP_START_STATUS_ERR,
                            appliName.in());
    }
    catch (const CORBA::SystemException& e)
    {
        std::ostringstream exceptionInfo;
        exceptionInfo << e;
        
        LogMngr::logMessage(INF, MSG_ID_SYS_APP_START_SYSTEM_ERR,
                            exceptionInfo.str().c_str(),
                            appliName.in());
    }
    catch (...)
    {
        LogMngr::logMessage(INF, MSG_ID_SYS_UNEXPECTED_EXCEPTION,
                            "RunApplicationTask::execute()",
                            appliName.in());
    }
    
    return ret;
}

// ----------------------------------------------------------------------
// RunApplicationTaskFactory class.
// ----------------------------------------------------------------------
RunApplicationTaskFactory::RunApplicationTaskFactory (System_impl* system)
   : TaskFactory()
{
    m_system = system;
}


RunApplicationTaskFactory::~RunApplicationTaskFactory ()
{
}


TaskFactory* RunApplicationTaskFactory::createFactory (System_impl* system)
            throw (OutOfMemoryException)
{
    try
    {
        std::auto_ptr <RunApplicationTaskFactory> taskFactory (
                          new RunApplicationTaskFactory (system));

        return taskFactory.release();
    }
    catch (const std::bad_alloc&)
    {
        CDMW_THROW (OutOfMemoryException);
    }
}


Task* RunApplicationTaskFactory::createTask (
                        const std::string& taskId,
                        TaskObserver* taskObserver) 
             throw (InvalidTaskIdException, 
                    OutOfMemoryException)
{
    CORBA::String_var appliName = CORBA::string_dup ("");
    
    try
    {
        // get application control
        CdmwPlatformMngt::ApplicationControl_var application_control =
            m_system->get_application_control (taskId.c_str());
            
        // get application
        CdmwPlatformMngt::Application_var application =
            m_system->get_application (taskId.c_str());
    
        // get application name
        appliName = application->name();
        
        
        
    
        std::auto_ptr <RunApplicationTask> task (
                               new RunApplicationTask (
                                         taskId,
                                         taskObserver,
                                         application_control.in(),
                                         application.in()));

        return task.release();
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
                            "RunApplicationTaskFactory::createTask",
                            appliName.in());
                            
        CDMW_THROW1( InvalidTaskIdException, taskId );
    } 
}

} // End namespace PlatformMngt
} // End namespace Cdmw

