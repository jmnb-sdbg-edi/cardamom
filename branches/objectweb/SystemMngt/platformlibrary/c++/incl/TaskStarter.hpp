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


#ifndef INCL_PLATFORMMNGT_TASKSTARTER_HPP 
#define INCL_PLATFORMMNGT_TASKSTARTER_HPP 

#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "SystemMngt/platformlibrary/MessageQueue.hpp"

#include <string>
#include <list>
#include <map>

namespace Cdmw
{
namespace PlatformMngt
{


class Task;
class TaskObserver;
class TaskFactory;
class Callback;



/**
 * Purpose:
 * Thrown when the status of the receiving object doesn't allow the operation.
 */
class IncompatibleStatusException  : public Exception
{
public:
    IncompatibleStatusException()
        : Exception( Exception::SAFE, "Incompatible Status" )
    {
    };
};



/**
 * Purpose:
 * The different status of a Starter.
 */
enum StarterStatus
{
    /**
     * The starter has been created.
     */
    STARTER_CREATED,

    /**
     * The starter is running.
     */
    STARTER_RUNNING,

    /**
     * The starter is suspended by an error.
     */
    STARTER_SUSPENDED,

    /**
     * The starter has completed its work but has been suspended by an error.
     */
    STARTER_COMPLETION_SUSPENDED,

    /**
     * The starter has completed its work.
     */
    STARTER_COMPLETED,

    /**
     * The starter has been stopped.
     */
    STARTER_STOPPED
};



/**
*Purpose:
* The concurrent tasks starter.
*<p> 
*
*Features:
*<p> Thread safe. Exception safe.
*/
class Starter : public OsSupport::Thread
{

    friend class TaskObserver;

    public:


        /**
        * Purpose:
        * <p> Constructor.
        *
        * @exception BadParameterException if taskFactory is NULL.
        */ 
        Starter(TaskFactory* taskFactory);


        /**
        * Purpose:
        * <p> Destructor.
        */ 
        virtual ~Starter() throw();


        /**
         *Purpose:
         *<p> Sets the abort callback.
         * If successful, this method takes ownership of the callback.
         *
         *@param callback the callback.
         *
         * @exception BadParameterException if the callback is NULL.
         */
        void setAbortCallback(Callback* callback)
            throw (BadParameterException);


        /**
         *Purpose:
         *<p> Sets the completion callback.
         * If successful, this method takes ownership of the callback.
         *
         *@param callback the callback.
         *
         * @exception BadParameterException if the callback is NULL.
         */
        void setCompletionCallback(Callback* callback)
            throw (BadParameterException);


        /**
         *Purpose:
         *<p> Adds the specified tasks for starting.
         *
         * @param tasks the tasks to start.
         *
         * @exception IncompatibleStatusException if the starter is either
         * completed or stopped.
         * @exception OutOfMemoryException
         */
        void addTasksToStart(const std::list<std::string>& tasks)
            throw(IncompatibleStatusException, OutOfMemoryException);


        /**
         *Purpose:
         *<p> Stops the starter.
         */
        virtual void stop() throw();



    protected:

        /**
        * Purpose:
        * <p> Constructor.
        */ 
        Starter();

        /**
         *Purpose:
         *<p> Pops a task to start.
         */
        std::string* popTaskToStart();

        /**
         *Purpose:
         *<p> Returns the specified started task.
         * Returns NULL if the specified task has not been started yet.
         *
         * @param taskId the identifier of the task.
         */
        Task* getStartedTask(const std::string& taskId);

        /**
         *Purpose:
         *<p> Adds the specified task to the list of started tasks.
         *
         * @param taskId the identifier of the task.
         * @param task the task.
         * @exception BadParameterException if task is NULL.
         */
        void addStartedTask(const std::string& taskId, Task* task)
            throw(BadParameterException, OutOfMemoryException);


        /**
         *Purpose:
         *<p> The starter's activity.
         */
        virtual void run() throw();


        /**
         *Purpose:
         *<p> Calls the abort callback if any.
         */
        void abort() throw();


        /**
         *Purpose:
         *<p> Signals the completion to the starter.
         * Used only by TaskObserver.
         */
        void complete() throw();


        /**
         *Purpose:
         *<p> Stops actually the starter and its auxiliary active objects.
         */
        void doStop() throw();


        /**
         *Purpose:
         *<p> Calls the completion callback if any.
         */
        void doComplete() throw();

    private:

        /**
        * Purpose:
        * <p>
        * Copy constructor is not allowed.
        */ 
        Starter(const Starter& rhs);


        /**
        * Purpose:
        * <p>
        * Assignment operator is not allowed.
        */ 
        Starter& operator=(const Starter& rhs);


        /**
         *Purpose:
         *<p> Removes the specified task from the list of started tasks.
         * Used only by TaskObserver.
         *
         * @param taskId the identifier of the task.
         */
        void removeStartedTask(const std::string& taskId);


    protected:

        /**
         *Purpose:
         *<p> The status of the starter.
         */
        StarterStatus m_status;

        /**
         * The mutex protecting the state of the starter.
         * The state includes the m_status member.
         * For a derived class, the state may include other members of this
         * derived class.
         */
        OsSupport::Mutex m_mutex;

        /**
         * The associated task observer.
         */
        TaskObserver* m_taskObserver;

    private:

        /**
        * The tasks to start.
        */
        MessageQueue<std::string> m_tasksToStart;

        /**
         * The map of started tasks.
         */
        std::map<std::string, Task*> m_startedTasks;

        /**
         * The mutex protecting concurrent access to m_startedTasks.
         */
        OsSupport::Mutex m_startedTasks_mutex;
 
        /**
        * The associated task factory.
        */
        TaskFactory* m_taskFactory;

        /**
         * The callback executed when the starter has completed its work.
         */
        Callback* m_completionCallback;

        /**
         * The callback executed when the starter encounters a fatal error.
         */
        Callback* m_abortCallback;

}; // End class Starter 



/**
*Purpose:
* The observer of task completion.
*<p> 
*
*Features:
*<p> Thread safe. Exception safe.
*/
class TaskObserver : public OsSupport::Thread
{

    friend class Task;


    public:


        /**
        * Purpose:
        * <p> Constructor.
        */ 
        TaskObserver(Starter* starter);


        /**
        * Purpose:
        * <p> Sets the number of tasks to observe.
        *
        * @exception BadParameterException if numberOfTasks equals 0.
        */ 
        void setNumberOfTasksToObserve(size_t numberOfTasks)
            throw(BadParameterException);


        /**
        * Purpose:
        * <p> Destructor.
        */ 
        virtual ~TaskObserver() throw();


        /**
         *Purpose:
         *<p> Stops the task observer.
         */
        void stop() throw();


    protected:

        /**
         *Purpose:
         *<p> The task observer's activity.
         */
        virtual void run() throw();

        /**
         *Purpose:
         *<p> The extension point of the run.
         * By default, returns true.
         */
        virtual bool runExtensionPoint(const std::string& taskId);

    private:

        /**
        * Purpose:
        * <p>
        * Copy constructor is not allowed.
        */ 
        TaskObserver(const TaskObserver& rhs);


        /**
        * Purpose:
        * <p>
        * Assignment operator is not allowed.
        */ 
        TaskObserver& operator=(const TaskObserver& rhs);


        /**
         *Purpose:
         *<p> Adds the specified task in the ended tasks list.
         * Used only by Task.
         *
         * @param taskId the identifier of the ended task.
         */
        void addEndedTask(const std::string& taskId)
            throw();



    protected:

        /**
         * The associated starter.
         */
        Starter* m_starter;

        /**
         *Purpose:
         *<p> Indicates whether the task observer has been requested to stop.
         */
        bool m_stopped;

        /**
         * The mutex protecting the m_stopped member.
         */
        OsSupport::Mutex m_mutex;

        /**
        * The total number of tasks to observe.
        */
        size_t m_totalNbOfTasks;


    private:

        /**
        * The ended tasks message queue.
        */
        MessageQueue<std::string> m_endedTasks;


}; // End class TaskObserver 

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_TASKSTARTER_HPP

