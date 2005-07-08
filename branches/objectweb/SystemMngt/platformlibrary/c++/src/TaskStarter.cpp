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


#include "SystemMngt/platformlibrary/TaskStarter.hpp"
#include "SystemMngt/platformlibrary/TaskFactory.hpp"
#include "SystemMngt/platformlibrary/Callback.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"

#include "Foundation/common/Assert.hpp"

#include <memory>
#include <sstream>

namespace Cdmw
{
namespace PlatformMngt
{


//=========================================================================
// Starter
//=========================================================================


Starter::Starter(TaskFactory* taskFactory) : m_status(STARTER_CREATED)
{
    if (taskFactory == NULL)
    {
        CDMW_THROW2(BadParameterException, "taskFactory", "NULL");
    }

    m_taskObserver = new TaskObserver(this);

    m_taskFactory = taskFactory;
    m_completionCallback = NULL;
    m_abortCallback = NULL;

}


Starter::Starter() : m_status(STARTER_CREATED)
{

    m_taskObserver = NULL;
    m_taskFactory = NULL;
    m_completionCallback = NULL;
    m_abortCallback = NULL;

}


Starter::~Starter()
  throw()
{

    if (m_taskObserver != NULL)
    {
        delete m_taskObserver;
    }

    if (m_completionCallback != NULL)
    {
        delete m_completionCallback;
    }

    if (m_abortCallback != NULL)
    {
        delete m_abortCallback;
    }

}


void Starter::setCompletionCallback(Callback* callback)
    throw (BadParameterException)
{

    if (callback == NULL)
    {
        CDMW_THROW2(BadParameterException, "callback", "NULL");
    }

    m_completionCallback = callback;

}


void Starter::setAbortCallback(Callback* callback)
    throw (BadParameterException)
{

    if (callback == NULL)
    {
        CDMW_THROW2(BadParameterException, "callback", "NULL");
    }

    m_abortCallback = callback;

}


void freeListContents(std::list<std::string*>& theList)
{

    // deletes each allocated string in taskPointers
    for (std::list<std::string*>::const_iterator it =
        theList.begin(); it != theList.end(); ++it)
    {
        delete *it;
    }

}


void Starter::addTasksToStart(const std::list<std::string>& tasks)
    throw(IncompatibleStatusException, OutOfMemoryException)
{


    {
        CDMW_MUTEX_GUARD(m_mutex);

        if (m_status == STARTER_COMPLETED || m_status == STARTER_COMPLETION_SUSPENDED
                || m_status == STARTER_STOPPED)
            CDMW_THROW(IncompatibleStatusException);
       
    }


    try
    {

        // creates the list of pointers to the copy of the tasks
        std::list<std::string*> taskPointers;

        for (std::list<std::string>::const_iterator it = tasks.begin();
            it != tasks.end(); ++it)
        {

            try
            {
                std::auto_ptr<std::string> taskPtr(new std::string(*it));

                taskPointers.push_back(taskPtr.get());

                taskPtr.release();
            }
            catch(const std::bad_alloc&)
            {
                // deletes each allocated string in taskPointers
                freeListContents(taskPointers);

                CDMW_THROW(OutOfMemoryException);
            }

        }

        try
        {
            // pushes the list of pointers
            m_tasksToStart.push(taskPointers);

            // the message queue has taken ownership of each task copy

        }
        catch(const OutOfMemoryException&)
        {
            // deletes each allocated string in taskPointers
            freeListContents(taskPointers);

            throw;
        }

    }
    catch(const std::bad_alloc&)
    {
        CDMW_THROW(OutOfMemoryException);
    }

}


std::string* Starter::popTaskToStart()
{
    return m_tasksToStart.pop();
}


void Starter::stop()
    throw ()
{

    bool stop;

    m_mutex.lock();

    stop = (get_status() != OsSupport::THREAD_CREATED) && (m_status != STARTER_STOPPED);
    m_status = STARTER_STOPPED;

    m_mutex.unlock();

    if (stop)
    {
        doStop();
    }


}


void Starter::doStop()
    throw ()
{

    // Stops the task observer
    m_taskObserver->stop();

    // Destroys the message queue in order to release the starter thread
    m_tasksToStart.destroy();

    // Waits for the ending of the starter thread
    this->join();

    // Waits for the ending of each actually started task
    // and deletes each task.
    CDMW_MUTEX_GUARD(m_startedTasks_mutex);
    
    for (std::map<std::string, Task*>::iterator it = m_startedTasks.begin();
        it != m_startedTasks.end(); ++it)
    {
        Task* task = it->second;
        if (task->get_status() != OsSupport::THREAD_CREATED)
        {
            task->join();
        }

        delete task;

    }

    // destroy all elements in started Task map
    m_startedTasks.clear();

}


Task* Starter::getStartedTask(const std::string& taskId)
{
    Task* task = NULL;

    CDMW_MUTEX_GUARD(m_startedTasks_mutex);

    std::map<std::string, Task*>::iterator it = m_startedTasks.find(taskId);

    if (it != m_startedTasks.end())
    {
        task = it->second;
    }

    return task;

}


void Starter::addStartedTask(const std::string& taskId, Task* task)
    throw(BadParameterException, OutOfMemoryException)
{

    if (task == NULL)
    {
        CDMW_THROW2(BadParameterException, "task", "NULL");
    }

    CDMW_MUTEX_GUARD(m_startedTasks_mutex);

    try
    {
        m_startedTasks[taskId] = task;
    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfMemoryException);
    }

}


void Starter::removeStartedTask(const std::string& taskId)
{

    Task* task = NULL;

    m_startedTasks_mutex.lock();

    std::map<std::string, Task*>::iterator it = m_startedTasks.find(taskId);

    if (it != m_startedTasks.end())
    {
        task = it->second;

        m_startedTasks.erase(it);
    }

    m_startedTasks_mutex.unlock();

    if (task != NULL)
    {
        // join the task thread and deletes the task
        try
        {
            task->join();

            delete task;
        }
        catch(...)
        {
            std::ostringstream msg;

            msg << "Unexpected exception to join started task " << taskId.c_str() << " in Starter" << std::endl;

            LogMngr::logMessage(FTL, msg.str().c_str()); 
        }
    }
}


void Starter::run()
  throw()
{

    try
    {

        bool end = false;

        m_mutex.lock();

        if (m_status == STARTER_STOPPED)
            end = true;
        else
            m_status = STARTER_RUNNING;

        m_mutex.unlock();

        if (end)
        {
            // The starter has been stopped prematurely
            return;
        }

        size_t nbTasks = m_tasksToStart.size(); 

        if (nbTasks > 0)
        {
            m_taskObserver->setNumberOfTasksToObserve(nbTasks);
            m_taskObserver->start();
        }
        else
        {
            complete();
            end = true;
        }

        while(!end)
        {
            std::auto_ptr<std::string> pTaskId(m_tasksToStart.pop());

            m_mutex.lock();
            end = (m_status == STARTER_STOPPED);
            m_mutex.unlock();

            if (end)
                break;

            try
            {

                std::auto_ptr<Task> pTask(
                    m_taskFactory->createTask(*pTaskId, m_taskObserver));

                addStartedTask(*pTaskId, pTask.get());

                pTask->startTask();
                pTask.release();

            }
            catch(const InvalidTaskIdException&)
            {
                LogMngr::logMessage(FTL, "Task creation error due to an invalid task id"); 
                this->abort();
                break;
            }
            catch(const OutOfMemoryException&)
            {
                LogMngr::logMessage(FTL, "Starter runs out of memory"); 
                this->abort();  // avoids conflict with ::abort()
                break;
            }
            catch(...)
            {
                LogMngr::logMessage(FTL, "Unexpected exception in while loop of Starter"); 
                this->abort();  // avoids conflict with ::abort()
                break;
            }

            if (m_tasksToStart.isEmpty())
                break;

        }

    }
    catch(const MessageQueueDestroyedException&)
    {
    }
    catch(...)
    {
        LogMngr::logMessage(FTL, "Unexpected exception in Starter"); 
    }

}


void Starter::complete() throw ()
{

    bool completed = false;

    m_mutex.lock();

    if (m_status == STARTER_RUNNING)
    {
        m_status = STARTER_COMPLETED;
        completed = true;
    }
    else
    {
        // the starter was suspended
        m_status = STARTER_COMPLETION_SUSPENDED;
    }

    m_mutex.unlock();

    if (completed)
    {
        doComplete();
    }

}


void Starter::doComplete() throw ()
{

    try
    {
        if (m_completionCallback != NULL)
        {
            m_completionCallback->execute();
        }
    }
    catch(...)
    {
        LogMngr::logMessage(FTL, "Unexpected exception in Starter completion callback"); 
    }

}


void Starter::abort() throw ()
{

    if (m_abortCallback != NULL)
    {
        try
        {

            m_abortCallback->execute();

        }
        catch(...)
        {
            LogMngr::logMessage(FTL, "Unexpected exception in Starter abort callback"); 
        }
    }

}


//=========================================================================
// TaskObserver
//=========================================================================

TaskObserver::TaskObserver(Starter* starter) : m_stopped(false)
{
    if (starter == NULL)
    {
        CDMW_THROW2(BadParameterException, "starter", "NULL");
    }

    m_starter = starter;
    m_totalNbOfTasks = 0;

}


void TaskObserver::setNumberOfTasksToObserve(size_t numberOfTasks)
    throw(BadParameterException)
{
    if (numberOfTasks == 0)
    {
        CDMW_THROW2(BadParameterException, "numberOfTasks", "0");
    }

    m_totalNbOfTasks = numberOfTasks;
}


TaskObserver::~TaskObserver()
  throw()
{
}


void TaskObserver::addEndedTask(const std::string& taskId)
    throw()
{

    try
    {

        std::auto_ptr<std::string> pTaskId(new std::string(taskId));

        m_endedTasks.push(pTaskId.get());

        pTaskId.release();

    }
    catch(const MessageQueueDestroyedException&)
    {
        // The task observer has been stopped
        // Nothing to do
    }
    catch(...)
    {
        LogMngr::logMessage(FTL, "TaskObserver adding ended task unexpected exception"); 
        m_starter->abort();
    }

}


bool TaskObserver::runExtensionPoint(const std::string& taskId)
{
    return true;
}


void TaskObserver::stop()
    throw ()
{

    bool stop;

    m_mutex.lock();

    stop = (get_status() != OsSupport::THREAD_CREATED) && (!m_stopped);
    m_stopped = true;

    m_mutex.unlock();

    if (stop)
    {
        // Destroys the message queue in order to release the observer thread
        m_endedTasks.destroy();

        // Waits for the ending of the observer thread
        this->join();
    }

}


void TaskObserver::run()
  throw()
{

    bool stopped = false;

    CDMW_ASSERT(m_totalNbOfTasks != 0);

    try
    {
        size_t nbAckEndedTasks = 0;

        while(!stopped)
        {
            std::auto_ptr<std::string> pTaskId(m_endedTasks.pop());

            m_mutex.lock();
            stopped = m_stopped;
            m_mutex.unlock();

            if (stopped)
                break;

            m_starter->removeStartedTask(*pTaskId);
            ++nbAckEndedTasks;

            if (nbAckEndedTasks == m_totalNbOfTasks)
            {
                m_starter->complete();
                break;
            }

            if (!runExtensionPoint(*pTaskId))
            {
                m_starter->abort();
                break;
            }

        }

    }
    catch(const MessageQueueDestroyedException&)
    {
    }
    catch(...)
    {
        LogMngr::logMessage(FTL, "TaskObserver Unexpected exception"); 
    }

}


} // End namespace PlatformMngt
} // End namespace Cdmw

