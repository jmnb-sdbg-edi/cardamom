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


#include "SystemMngt/platformlibrary/TaskSequencer.hpp"
#include "SystemMngt/platformlibrary/SynchronisableTaskFactory.hpp"
#include "SystemMngt/platformlibrary/Callback.hpp"
#include "SystemMngt/platformlibrary/CommonLogMessageId.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"

#include "Foundation/common/Assert.hpp"

namespace Cdmw
{
namespace PlatformMngt
{


//=========================================================================
// Sequencer
//=========================================================================

Sequencer::Sequencer(SynchronisableTaskFactory* taskFactory)
    : m_resumption_condition(m_mutex)
{
    if (taskFactory == NULL)
    {
        CDMW_THROW2(BadParameterException, "taskFactory", "NULL");
    }

    m_taskObserver = new SynchronisableTaskObserver(this, taskFactory);

    m_taskFactory = taskFactory;
    m_suspensionCallback = NULL;
    m_completionResumer = NULL;

}


Sequencer::~Sequencer()
  throw()
{

    if (m_suspensionCallback != NULL)
    {
        delete m_suspensionCallback;
    }
    
    if (m_completionResumer != NULL)
    {
        delete m_completionResumer;
    }

}


void Sequencer::setSuspensionCallback(Callback* callback)
    throw (BadParameterException)
{

    if (callback == NULL)
    {
        CDMW_THROW2(BadParameterException, "callback", "NULL");
    }

    m_suspensionCallback = callback;

}


void Sequencer::run()
  throw()
{

    try
    {

        LogMngr::logMessage(INF, "Sequencer is running"); 

        bool end = false;

        m_mutex.lock();

        if (m_status == STARTER_STOPPED)
            end = true;
        else
            m_status = STARTER_RUNNING;

        m_mutex.unlock();

        if (end)
        {
            // The sequencer has been stopped prematurely
            return;
        }

        size_t nbTasks = m_taskFactory->getNumberOfTasks();

        if (nbTasks > 0)
        {
            m_taskObserver->setNumberOfTasksToObserve(nbTasks);
            m_taskObserver->start();

            try
            {
                TaskIdList rootTasksId = m_taskFactory->getRootTasksId();
                addTasksToStart(rootTasksId);
            }
            catch(const IncompatibleStatusException&)
            {
                // The sequencer has been stopped prematurely
                this->abort();  // avoids conflict with ::abort()
                end = true;
            }
            catch(const OutOfMemoryException&)
            {
                LogMngr::logMessage(FTL, "Sequencer runs out of memory"); 
                this->abort();  // avoids conflict with ::abort()
                end = true;
            }
        }
        else
        {
            complete();
            end = true;
        }


        while(!end)
        {
            std::auto_ptr<std::string> pTaskId(popTaskToStart());

            m_mutex.lock();

            while (m_status == STARTER_SUSPENDED)
            {
                m_resumption_condition.wait();
            }

            end = (m_status == STARTER_STOPPED);
            m_mutex.unlock();

            if (end)
                break;

            try
            {
                Task* task = getStartedTask(*pTaskId);

                if (task != NULL)
                {
                    task->startTask();
                }
                else
                {
                    try
                    {

                        std::auto_ptr<SynchronisableTask> pTask(
                            m_taskFactory->createTask(*pTaskId, this, m_taskObserver));

                        addStartedTask(*pTaskId, pTask.get());

                        pTask->startTask();
                        pTask.release();

                    }
                    catch(const InvalidTaskIdException&)
                    {
                        LogMngr::logMessage(FTL, "Task creation error due to an invalid task id"); 
                        LogMngr::logMessage(FTL, MSG_ID_INVALID_TASK, (*pTaskId).c_str());
                        this->abort();
                        break;
                    }
                    catch(const OutOfMemoryException&)
                    {
                        LogMngr::logMessage(FTL, "Sequencer runs out of memory"); 
                        this->abort();
                        break;
                    }
                }
            }

            catch(...)
            {
                LogMngr::logMessage(FTL, "Unexpected exception in while loop of Sequencer"); 
                this->abort();  // avoids conflict with ::abort()
                break;
            }

        }

    }
    catch(const MessageQueueDestroyedException&)
    {
    }
    catch(...)
    {
        LogMngr::logMessage(FTL, "Unexpected exception in Sequencer"); 
    }

}


void Sequencer::stop()
    throw ()
{

    bool stopped;
    bool suspended;

    m_mutex.lock();

    stopped = (get_status() != OsSupport::THREAD_CREATED) && (m_status != STARTER_STOPPED);
    suspended = (m_status == STARTER_SUSPENDED);
    m_status = STARTER_STOPPED;

    m_mutex.unlock();

    if (stopped)
    {
        if (suspended)
        {
            // release the sequencer thread
            m_resumption_condition.broadcast();
        }


        if (m_completionResumer != NULL)
        {
            // join the completion resumer thread
            m_completionResumer->join();
        }
        
        doStop();
    }

}


void Sequencer::resume()
    throw (IncompatibleStatusException)
{
    bool resumption = false;

    m_mutex.lock();

    if (m_status != STARTER_SUSPENDED && m_status != STARTER_COMPLETION_SUSPENDED)
    {
        m_mutex.unlock();
        CDMW_THROW(IncompatibleStatusException);
    }

    if (m_status == STARTER_SUSPENDED)
    {
        resumption = true;
        m_status = STARTER_RUNNING;
    }
    else
    {
        m_status = STARTER_COMPLETED;
    }

    m_mutex.unlock();


    if (resumption)
    {
        m_resumption_condition.broadcast();
    }
    else
    {  
        // create and start new thread to complete the resume action         
        m_completionResumer = new CompletionResumer (this);
        m_completionResumer->start();
    }

}


void Sequencer::suspend()
    throw ()
{

    bool suspension = false;

    m_mutex.lock();
    if (m_status == STARTER_RUNNING)
    {
        m_status = STARTER_SUSPENDED;
        suspension = true;
    }
    m_mutex.unlock();

    if (suspension && m_suspensionCallback != NULL)
    {
        m_suspensionCallback->execute();
    }
}


//=========================================================================
// SynchronisableTaskObserver
//=========================================================================

SynchronisableTaskObserver::SynchronisableTaskObserver(Starter* starter,
    SynchronisableTaskFactory* taskFactory)
    : TaskObserver(starter)
{

    if (taskFactory == NULL)
    {
        CDMW_THROW2(BadParameterException, "taskFactory", "NULL");
    }

    m_taskFactory = taskFactory;
    m_totalNbOfTasks = taskFactory->getNumberOfTasks();

}


SynchronisableTaskObserver::~SynchronisableTaskObserver()
  throw()
{

}


bool SynchronisableTaskObserver::runExtensionPoint(const std::string& taskId)
{

    bool ret = true;

    CDMW_ASSERT(m_taskFactory != NULL);

    try
    {
        if (m_taskFactory->hasSuccessors(taskId))
        {
            TaskIdList successorsId = m_taskFactory->getSuccessorsId(taskId);
            m_starter->addTasksToStart(successorsId);
        }
    }
    catch(const InvalidTaskIdException&)
    {
        LogMngr::logMessage(FTL,
                "runExtension Point error in SynchronisableTaskObserver due to invalid task id exception"); 
        LogMngr::logMessage(FTL, MSG_ID_INVALID_TASK, taskId.c_str());

        ret = false;
    }
    catch(...)
    {
        LogMngr::logMessage(FTL,
                "SynchronisableTaskObserver unexpected exception"); 
        ret = false;
    }

    return ret;

}



//=========================================================================
// CompletionResumer
//=========================================================================


//
// Ctor
//    
CompletionResumer::CompletionResumer (Sequencer* p_sequencer)
    : m_sequencer (p_sequencer)

{

}

//
// Dtor
//
CompletionResumer::~CompletionResumer()
    throw()
{

}

//
// Call in a new thread by the Thread system
//
void CompletionResumer::run()
    throw()
{
    try 
    {
        // complete the sequencer resuming
        m_sequencer->doComplete();
    }
    
    // catch any exception
    catch (...)
    {
    }
}

} // End namespace PlatformMngt
} // End namespace Cdmw

