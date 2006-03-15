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


#ifndef INCL_PLATFORMMNGT_APPLICATION_STOP_HPP
#define INCL_PLATFORMMNGT_APPLICATION_STOP_HPP

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
 * The callback for the completion of the stop of an application.
 */
class StopApplicationCompletionCallback : public virtual Callback
{

public:

    /**
     *Purpose:
     *<p> Constructor.
     *
     *@param  system  the system managing the applications.
     */
    StopApplicationCompletionCallback (System_impl* system);


    /**
     *Purpose:
     *<p> Destructor.
     *
     */
    ~StopApplicationCompletionCallback ();
    
    
    /**
     *Purpose:
     *<p> Callback execution. Notifies the system the application
     *    has been completely stopped.
     */
    void execute() throw();
    
    
private:

    /**
     * The system managing the application.
     */
    System_impl* m_system;
    
};

/**
 * The task for the stop of a process.
 */
class StopApplicationTask : public virtual SynchronisableTask
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
     *@param  emergency        the stopping emergency
     */
    StopApplicationTask (const std::string& taskId,
                         Sequencer* sequencer,
                         TaskObserver* observer,
                         size_t predecessorCount,
                         CdmwPlatformMngt::Application_ptr application,
                         CORBA::Boolean emergency)
            throw (OutOfMemoryException);


    /**
     *Purpose:
     *<p> Destructor.
     *
     */
    ~StopApplicationTask () throw();
    
    
    /**
     *Purpose:
     *<p> Stops the applications.
     *
     *@return the status of the stop execution.
     */
    bool execute() throw();
    
private:

    /**
     * The application.
     */
    CdmwPlatformMngt::Application_var m_application;
    
    /**
     * Indicates whether to performed an emergency stop or not.
     */
    CORBA::Boolean m_emergency;
    
};

/**
 * The stop task factory.
 */
class StopApplicationTaskFactory : public virtual SynchronisableTaskFactory
{


public:

    /**
     *Purpose:
     *<p> Destructor.
     *
     */
    ~StopApplicationTaskFactory ();
    
    
    /**
     *Purpose:
     *<p> Gets a new synchronisable task factory instance.
     *
     *@param  system        the system managing the applications.
     *@param  emergency     the application's stop mode.
     *@param  graphElements the dependency graph of application stopping.
     */
    static
    SynchronisableTaskFactory* createFactory (
                     System_impl* system,
                     CORBA::Boolean emergency,
                     const CdmwPlatformMngt::GraphElements& graphElements)
            throw (OutOfMemoryException);

    /**
     *Purpose:
     *<p> Creates a new application synchronisable task.
     *
     *@param taskId       the application's name.
     *@param sequencer    the sequencer of the application's stop.
     *@param taskObserver the application's observer.
     */
    SynchronisableTask* createTask (const std::string& taskId,                      
                                    Sequencer* sequencer,
                                    TaskObserver* taskObserver )
            throw (InvalidTaskIdException, OutOfMemoryException);

                   
protected:

    /**
     *Purpose:
     *<p> Constructor.
     *
     *@param  system         the system managing the application.
     *@param  emergency      the emergency of stopping
     *@param  taskDefMap     the synchro TaskDef map
     *@param  rootTasks      the root tasks list
     */
    StopApplicationTaskFactory (System_impl* system,        
                                CORBA::Boolean emergency,
                                const SynchronisableTaskDefMap& taskDefMap,
                                const TaskIdList& rootTasks);


                   
private:

    /**
     * The system managing the application.
     */
    System_impl* m_system;

    /**
     * Indicates whether to performed an emergency stop or not.
     */
    CORBA::Boolean m_emergency;

};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_APPLICATION_STOP_HPP

