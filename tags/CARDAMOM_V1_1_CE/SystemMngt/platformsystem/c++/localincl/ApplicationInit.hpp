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


#ifndef INCL_PLATFORMMNGT_APPLICATION_INIT_HPP
#define INCL_PLATFORMMNGT_APPLICATION_INIT_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/platformlibrary/Callback.hpp"
#include "SystemMngt/platformlibrary/SynchronisableTaskFactory.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtApplication.stub.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class System_impl;
class Application_impl;

/**
 * The callback for the suspension of the initialisation of an application.
 */
class InitApplicationSuspensionCallback : public virtual Callback
{
    

public:

    /**
     *Purpose:
     *<p> Constructor.
     *
     *@param  system  the system managing the applications.
     */
    InitApplicationSuspensionCallback (System_impl* system);
        

    /**
     *Purpose:
     *<p> Destructor.
     *
     */
    ~InitApplicationSuspensionCallback ();
    
    
    /**
     *Purpose:
     *<p> Callback execution. notifies the system the application
     * has been suspended.
     */
    void execute() throw();
 
    
private:

    /**
     * The system managing the application.
     */
    System_impl* m_system;

};



/**
 * The callback for the completion of the initialisation of an application.
 */
class InitApplicationCompletionCallback : public virtual Callback
{

public:

    /**
     *Purpose:
     *<p> Constructor.
     *
     *@param  system  the system managing the applications.
     */
    InitApplicationCompletionCallback (System_impl* system);


    /**
     *Purpose:
     *<p> Destructor.
     *
     */
    ~InitApplicationCompletionCallback ();
    
    
    /**
     *Purpose:
     *<p> Callback execution. Notifies the system the application
     * has been completely initialised.
     */
    void execute() throw();
    
private:

    /**
     * The system managing the application.
     */
    System_impl* m_system;

};

/**
 * The task for the initialisation of an application.
 */
class InitApplicationTask : public virtual SynchronisableTask
{

public:

    /**
     *Purpose:
     *<p> Constructor.
     *
     *@param  taskId           the identifier of the application task.
     *@param  sequencer        the task sequencer
     *@param  observer         the task observer
     *@param  predecessorCount the number of predecessor tasks
     *@param  application      the application object
     *@param  startup_kind     the kind of start up
     */
    InitApplicationTask (const std::string& taskId,
                         Sequencer* sequencer,
                         TaskObserver* observer,
                         size_t predecessorCount,
                         CdmwPlatformMngt::Application_ptr application,
                         const CdmwPlatformMngtBase::StartupKind& startup_kind)
            throw (OutOfMemoryException);


    /**
     *Purpose:
     *<p> Destructor.
     *
     */
    ~InitApplicationTask () throw();
    
    
    /**
     *Purpose:
     *<p> Goes through initialisation of an application.
     *
     *@return the initialisation status of the execution.
     */
    bool execute() throw();
    
private:

    /**
     * The application.
     */
    CdmwPlatformMngt::Application_var m_application;

    /**
     * The startup kind.
     */
    CdmwPlatformMngtBase::StartupKind_var m_startupKind;
};

/**
 * The initialisation task factory.
 */
class InitApplicationTaskFactory : public virtual SynchronisableTaskFactory
{


public:


    /**
     *Purpose:
     *<p> Destructor.
     *
     */
    ~InitApplicationTaskFactory ();
    
    
    
    /**
     *Purpose:
     *<p> Gets a new synchronisable task factory instance.
     *
     *@param  system the system managing the application.
     *@param  startup_kind the application's startup mode.
     *@param  graphElements the dependency graph of application initialisation.
     */
    static
    SynchronisableTaskFactory* createFactory (
                     System_impl* system,
                     const CdmwPlatformMngtBase::StartupKind& startup_kind,
                     const CdmwPlatformMngt::GraphElements& graphElements)
            throw (OutOfMemoryException);

    /**
     *Purpose:
     *<p> Creates a new application synchronisable task 
     * 
     *@param taskId       the application task's name.
     *@param sequencer    the sequencer of the application's execution.
     *@param taskObserver the application's observer.
     */
    SynchronisableTask* createTask (const std::string& taskId,       
                                    Sequencer* sequencer,
                                    TaskObserver* taskObserver)
            throw (InvalidTaskIdException, OutOfMemoryException);
                   
  
  
protected:

    /**
     *Purpose:
     *<p> Constructor.
     *
     *@param  system         the system managing the application.
     *@param  startup_kind   the kind of start up
     *@param  taskDefMap     the synchro TaskDef map
     *@param  rootTasks      the root tasks list
     */
    InitApplicationTaskFactory (System_impl* system,        
                                const CdmwPlatformMngtBase::StartupKind& startup_kind,
                                const SynchronisableTaskDefMap& taskDefMap,
                                const TaskIdList& rootTasks)
            throw (OutOfMemoryException);


                 
private:

    /**
     * The system managing the applications.
     */
    System_impl* m_system;

    /**
     * The startup kind.
     */
    CdmwPlatformMngtBase::StartupKind_var m_startupKind;

};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_APPLICATION_INIT_HPP

