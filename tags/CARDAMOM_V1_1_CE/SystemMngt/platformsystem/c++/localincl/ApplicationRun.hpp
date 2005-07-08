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


#ifndef INCL_PLATFORMMNGT_APPLICATION_RUN_HPP
#define INCL_PLATFORMMNGT_APPLICATION_RUN_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/platformlibrary/Callback.hpp"
#include "SystemMngt/platformlibrary/TaskFactory.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtApplication.stub.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class System_impl;
class Application_impl;


/**
 * The task for the run of a process.
 */
class RunApplicationTask : public virtual Task
{

public:

    /**
     *Purpose:
     *<p> Constructor.
     *
     *@param  taskId           the identifier of the application task.
     *@param  observer         the task observer
     *@param  application      the application object
     */
    RunApplicationTask (const std::string& taskId,        
                        TaskObserver* observer,
                        CdmwPlatformMngt::Application_ptr application);


    /**
     *Purpose:
     *<p> Destructor.
     *
     */
    ~RunApplicationTask () throw();
    
    
    /**
     *Purpose:
     *<p> Launches the application's execution.
     *
     *@return the status of the execution.
     */
    bool execute() throw();


private:
    
    /**
     * The application.
     */
    CdmwPlatformMngt::Application_var m_application;
    
};


/**
 * The run task factory.
 */
class RunApplicationTaskFactory : public virtual TaskFactory
{


public:

    /**
     *Purpose:
     *<p> Destructor.
     *
     */
    ~RunApplicationTaskFactory ();
    
    
    /**
     *Purpose:
     *<p> Gets a new task factory instance.
     *
     *@param  system the system managing the applications.
     */
    static
    TaskFactory* createFactory (System_impl* system)
            throw (OutOfMemoryException);


    /**
     *Purpose:
     *<p> Creates a new task.
     *
     *@param taskId       the application's name.
     *@param taskObserver the application's observer.
     */
    Task* createTask (const std::string& taskId,
                      TaskObserver* taskObserver)      
            throw (InvalidTaskIdException, OutOfMemoryException);
  
  
protected:

    /**
     *Purpose:
     *<p> Constructor.
     *
     *@param  system the system managing the applications.
     */
    RunApplicationTaskFactory (System_impl* system);


                 
private:

    /**
     * The system managing the application.
     */
    System_impl* m_system;

};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_APPLICATION_RUN_HPP

