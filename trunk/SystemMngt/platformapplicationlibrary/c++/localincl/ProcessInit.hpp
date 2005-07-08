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


#ifndef INCL_PLATFORMMNGT_INIT_PROCESS_HPP
#define INCL_PLATFORMMNGT_INIT_PROCESS_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/platformlibrary/Callback.hpp"
#include "SystemMngt/platformlibrary/SynchronisableTaskFactory.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class ProcessProxy_impl;
class Application_impl;

/**
 * The callback for the suspension of the initialisation of a process.
 */
class InitProcessSuspensionCallback : public virtual Callback
{
private:
    /**
     * The application managing the process.
     */
    Application_impl* m_application;

public:
    /**
     *Purpose:
     *<p> Constructor.
     */
    InitProcessSuspensionCallback(
        Application_impl* application );

    /**
     *Purpose:
     *<p> Callback execution. notifies the application the process
     * has been suspended.
     */
    void execute()
        throw();
};

/**
 * The callback for the completion of the initialisation of a process.
 */
class InitProcessCompletionCallback : public virtual Callback
{
private:
    /**
     * The application managing the process.
     */
    Application_impl* m_application;

public:
    /**
     *Purpose:
     *<p> Constructor.
     */
    InitProcessCompletionCallback(
        Application_impl* application );

    /**
     *Purpose:
     *<p> Callback execution. Notifies the application the process
     * has been completely initialised.
     */
    void execute()
        throw();
};

/**
 * The task for the initialisation of a process.
 */
class InitProcessTask : public virtual SynchronisableTask
{
private:
    /**
     * The process proxy.
     */
    ProcessProxy_impl* m_processProxy;

    /**
     * The startup kind.
     */
    CdmwPlatformMngtBase::StartupKind_var m_startupKind;
    unsigned int m_step;

public:
    /**
     *Purpose:
     *<p> Constructor.
     */
    InitProcessTask(
        const std::string& taskId,
        Sequencer* sequencer,
        TaskObserver* observer,
        size_t predecessorCount,
        ProcessProxy_impl* processProxy,
        const CdmwPlatformMngtBase::StartupKind& startup_kind,
        unsigned int step )
            throw( OutOfMemoryException );

    /**
     *Purpose:
     *<p> Destructor.
     */

     ~InitProcessTask() throw() {} ;

    /**
     *Purpose:
     *<p> Goes through all the steps of initialisation of a process.
     *
     *@return the initialisation status of the execution.
     */
    bool execute()
        throw();
};

/**
 * The initialisation task factory.
 */
class InitProcessTaskFactory : public virtual SynchronisableTaskFactory
{
private:
    /**
     * The application managing the process.
     */
    Application_impl* m_application;

    /**
     * The startup kind.
     */
    CdmwPlatformMngtBase::StartupKind_var m_startupKind;

protected:
    /**
     *Purpose:
     *<p> Constructor.
     */
    InitProcessTaskFactory(
        Application_impl* application,
        const CdmwPlatformMngtBase::StartupKind& startup_kind,
        const SynchronisableTaskDefMap& taskDefMap,
        const TaskIdList& rootTasks )
            throw( OutOfMemoryException );

public:
    /**
     *Purpose:
     *<p> Gets a new synchronisable task factory instance.
     *
     *@param  application the application managing the process.
     *@param  startup_kind the process's startup mode.
     *@param  graphElements the dependency graph of processes' initialisation.
     */
    static
    SynchronisableTaskFactory* createFactory(
        Application_impl* application,
        const CdmwPlatformMngtBase::StartupKind& startup_kind,
        const CdmwPlatformMngt::GraphElements& graphElements )
            throw( OutOfMemoryException,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p> Creates a new synchronisable task by retrieving the process name
     * and the step from the task id.
     *
     *@param taskId the process's name.
     *@param sequencer the sequencer of the process's execution.
     *@param taskObserver the process's observer.
     */
    SynchronisableTask* createTask(
        const std::string& taskId,
        Sequencer* sequencer,
        TaskObserver* taskObserver )
            throw( InvalidTaskIdException,
                   OutOfMemoryException );
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_INIT_PROCESS_HPP

