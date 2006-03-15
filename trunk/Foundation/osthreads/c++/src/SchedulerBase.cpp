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


#include "Foundation/osthreads/SchedulerBase.hpp"
#include <Foundation/osthreads/MutexGuard.hpp>
#include <Foundation/common/Assert.hpp>
#include <memory>


namespace 
{


class WorkerThread : public Cdmw::OsSupport::Thread
{
public:
    
    WorkerThread(ACE_Activation_Queue & activation_queue)
        : m_activation_queue(activation_queue)
    {
    };
    
    ~WorkerThread()
        throw()  
    {
    }

    virtual void run()
        throw()
    {
        for (;;)
        {
            // Dequeue the next method object (we use an auto pointer in
            // case an exception is thrown in the <call>).
            std::auto_ptr<ACE_Method_Request> mo (this->m_activation_queue.dequeue ());
                
            // Call it.
            if (mo->call () == -1)
                break;
                
            // Destructor automatically deletes it.
        }
    }
    
private:
    ACE_Activation_Queue & m_activation_queue;
};


class RunnableTaskHolder : public ACE_Method_Request
{
    
public:
    RunnableTaskHolder(Cdmw::OsSupport::Runnable * task)
        : m_task(task)
    {
        CDMW_ASSERT(task != 0);
    }
    
    ~RunnableTaskHolder()
    {
    }

    virtual int call (void)
    {
        m_task->run();
        return 0;
    }
    
private:
    std::auto_ptr<Cdmw::OsSupport::Runnable> m_task;
};

class StopRequest : public ACE_Method_Request
{
public:

    virtual int call (void)
    {
        return -1;
    }
};

};// Anonymous namespace


namespace Cdmw {
namespace OsSupport {


//
// Implementation part
//
SchedulerBase::SchedulerBase(unsigned short nbthreads)
    : m_status(SCHEDULER_CREATED)
{
    // Create nbthreads
    for (unsigned short i = 0; i < nbthreads; ++i) 
    {
        m_worker_threads.push_back(new WorkerThread(m_activation_queue));
    }
}

SchedulerBase::~SchedulerBase()
    throw()
{
    CDMW_MUTEX_GUARD (m_mutex);
    
    if (m_status == SCHEDULER_RUNNING)
        do_stop(true);
    else if (m_status == SCHEDULER_STOPPING)
        join_all();

    for (std::list<Cdmw::OsSupport::Thread*>::iterator it = m_worker_threads.begin();
         it != m_worker_threads.end(); ++it)
        delete (*it);
}



void SchedulerBase::start()
{
    CDMW_MUTEX_GUARD (m_mutex);
    
    if (m_status == SCHEDULER_CREATED)
    {
        for (std::list<Cdmw::OsSupport::Thread*>::iterator it = m_worker_threads.begin();
             it != m_worker_threads.end(); ++it)
            (*it)->start();
            
        m_status = SCHEDULER_RUNNING;
    }
}


bool SchedulerBase::enqueue(Cdmw::OsSupport::Runnable * task)
{
    int result = -1;
    CDMW_MUTEX_GUARD (m_mutex);
    
    if (m_status == SCHEDULER_RUNNING)        
        result = this->m_activation_queue.enqueue(new RunnableTaskHolder(task));
        
    return (result == 0);
}


bool SchedulerBase::add_thread()
{
    bool result = false;
    
    try 
    {
        CDMW_MUTEX_GUARD (m_mutex);
        
        if (m_status == SCHEDULER_RUNNING) 
        {
            std::auto_ptr<Cdmw::OsSupport::Thread> worker(new WorkerThread(m_activation_queue));
            worker->start();
            m_worker_threads.push_back(worker.release());
            result = true;
        }
    } 
    catch (...) 
    {
        // ignore
    }
    
    return result;
}
    
    
void SchedulerBase::stop(bool wait_for_completion)
{
    CDMW_MUTEX_GUARD (m_mutex);
    
    if (m_status == SCHEDULER_RUNNING)
        do_stop(wait_for_completion);
}   

void SchedulerBase::do_stop(bool wait_for_completion)
{
    std::size_t size = m_worker_threads.size();
    
    for (std::size_t i = 0; i < size; ++i)
        this->m_activation_queue.enqueue(new StopRequest());
        
    m_status = SCHEDULER_STOPPING;
    
    if (wait_for_completion) 
    {
        join_all();
        m_status = SCHEDULER_STOPPED;
    }
}

void SchedulerBase::join_all()
{
    for (std::list<Cdmw::OsSupport::Thread*>::iterator it = m_worker_threads.begin();
         it != m_worker_threads.end(); ++it)
        (*it)->join();
}


}; // namespace OsSupport
}; // namespace Cdmw
