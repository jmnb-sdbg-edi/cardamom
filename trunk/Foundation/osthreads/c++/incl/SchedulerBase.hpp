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

#ifndef _CDMW_OSSUPPORT_SCHEDULER_BASE_HPP_
#define _CDMW_OSSUPPORT_SCHEDULER_BASE_HPP_

#include <Foundation/osthreads/Runnable.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <ace/Activation_Queue.h>
#include <memory> // for std::auto_ptr ???
#include <list>
#include <cstddef>


namespace Cdmw {
namespace OsSupport {

  /**
   * This class defines a scheduler responsible for state transfer
   */

class SchedulerBase
{
public:
    SchedulerBase(unsigned short nbthreads);
    
    ~SchedulerBase()
        throw();

    void start();

    bool enqueue(Cdmw::OsSupport::Runnable * task);

    bool add_thread();
    
    
    void stop(bool wait_for_completion = false);

    std::size_t nb_threads() const
    {
        return  m_worker_threads.size();
    }

private:
    // No copying allowed
    SchedulerBase(const SchedulerBase & );
    SchedulerBase& operator=(const SchedulerBase & );

    // This does lock the list of worker threads!
    void join_all();
    void do_stop(bool wait_for_completion);
    
    
    enum StatusType {
        SCHEDULER_CREATED  = 0,
        SCHEDULER_RUNNING  = 1,
        SCHEDULER_STOPPING = 2,
        SCHEDULER_STOPPED  = 3
    };

    std::list<Cdmw::OsSupport::Thread*> m_worker_threads;
    Cdmw::OsSupport::Mutex              m_mutex;
    ACE_Activation_Queue                m_activation_queue;
    StatusType                          m_status;
};
 

}; // namespace OsSupport
}; // namespace Cdmw

#endif // _CDMW_FT_SCXHEDULER_BASE_HPP_
