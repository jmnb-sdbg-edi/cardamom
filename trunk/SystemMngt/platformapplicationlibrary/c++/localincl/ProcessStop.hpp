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


#ifndef INCL_PLATFORMMNGT_STOP_PROCESS_HPP
#define INCL_PLATFORMMNGT_STOP_PROCESS_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/platformlibrary/Callback.hpp"
#include "SystemMngt/platformlibrary/SynchronisableTaskFactory.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class Process_impl;
class Application_impl;

/**
 * The callback for the completion of the stop of a process.
 */
class StopProcessCompletionCallback : public virtual Callback
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
    StopProcessCompletionCallback(
        Application_impl* application );

    /**
     *Purpose:
     *<p> Callback execution. Notifies the application the process
     * has been completely stopped.
     */
    void execute()
        throw();
};

/**
 * The task for the stop of a process.
 */
class StopProcessTask : public virtual SynchronisableTask
{
private:
    /**
     * The process impl.
     */
    Process_impl* m_process;

    /**
     * Indicates whether to performed an emergency stop or not.
     */
    CORBA::Boolean m_emergency;

public:
    /**
     *Purpose:
     *<p> Constructor.
     */
    StopProcessTask(
        const std::string& taskId,
        Sequencer* sequencer,
        TaskObserver* observer,
        size_t predecessorCount,
        Process_impl* process,
        CORBA::Boolean emergency );

    /**
     *Purpose:
     *<p> Stops the process.
     *
     *@return the status of the stop execution.
     */
    bool execute()
        throw();
};

/**
 * The stop task factory.
 */
class StopProcessTaskFactory : public virtual SynchronisableTaskFactory
{
private:
    /**
     * The application managing the process.
     */
    Application_impl* m_application;

    /**
     * Indicates whether to performed an emergency stop or not.
     */
    CORBA::Boolean m_emergency;

protected:
    /**
     *Purpose:
     *<p> Constructor.
     */
    StopProcessTaskFactory(
        Application_impl* application,
        CORBA::Boolean emergency,
        const SynchronisableTaskDefMap& taskDefMap,
        const TaskIdList& rootTasks );

public:
    /**
     *Purpose:
     *<p> Gets a new synchronisable task factory instance.
     *
     *@param  application the application managing the process.
     *@param  emergency the process's stop mode.
     *@param  graphElements the dependency graph of processes' stop.
     */
    static
    SynchronisableTaskFactory* createFactory(
        Application_impl* application,
        CORBA::Boolean emergency,
        const CdmwPlatformMngt::GraphElements& graphElements )
            throw( OutOfMemoryException,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p> Creates a new synchronisable task.
     *
     *@param taskId the process's name.
     *@param sequencer the sequencer of the process's stop.
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

#endif //INCL_PLATFORMMNGT_STOP_PROCESS_HPP

