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


#ifndef INCL_PLATFORMMNGT_RUN_PROCESS_HPP
#define INCL_PLATFORMMNGT_RUN_PROCESS_HPP

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
 * The callback for the completion of the run of a process.
 */
class RunProcessCompletionCallback : public virtual Callback
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
    RunProcessCompletionCallback(
        Application_impl* application );

    /**
     *Purpose:
     *<p> Callback execution. Notifies the application a process
     * has finished to run.
     */
    void execute()
        throw();
};

/**
 * The task for the run of a process.
 */
class RunProcessTask : public virtual Task
{
private:
    /**
     * The process impl.
     */
    Process_impl* m_process;

public:
    /**
     *Purpose:
     *<p> Constructor.
     */
    RunProcessTask(
        const std::string& taskId,
        TaskObserver* observer,
        Process_impl* process );

    /**
     *Purpose:
     *<p> Launches the process's execution.
     *
     *@return the status of the execution.
     */
    bool execute()
        throw();
};

/**
 * The run task factory.
 */
class RunProcessTaskFactory : public virtual TaskFactory
{
private:
    /**
     * The application managing the process.
     */
    Application_impl* m_application;

protected:
    /**
     *Purpose:
     *<p> Constructor.
     */
    RunProcessTaskFactory(
        Application_impl* application);

public:
    /**
     *Purpose:
     *<p> Gets a new synchronisable task factory instance.
     */
    static
    TaskFactory* createFactory(
        Application_impl* application )
            throw( OutOfMemoryException );

    /**
     *Purpose:
     *<p> Creates a new synchronisable task.
     */
    Task* createTask(
        const std::string& taskId,
        TaskObserver* taskObserver )
            throw( InvalidTaskIdException,
                   OutOfMemoryException );
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_RUN_PROCESS_HPP

