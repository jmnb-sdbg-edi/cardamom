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

// private include files
#include "common/SingleShotTimer.hpp"

// include files
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"

#include "ftmonitoring/common/CommonTypeDefinitions.hpp"

#include <memory>
#include "ftmonitoring/common/MacroDefs.hpp"


namespace Cdmw {
namespace FT {
namespace Supervision
{

/**
*Implementation notes: 
*<p>
*Portability issues:  none 
*<p>
*/


//
// Ctor
//    
SingleShotTimer::SingleShotTimer (TimeoutObserver* p_observer,
                                  unsigned long timeoutValue)
    : m_threadRunning (false),
      m_timerStarted (false),
      m_condition (m_mutex),
      m_timeoutValue (timeoutValue),
      m_pObserver(p_observer)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"timeoutValue="<<timeoutValue);
}



//
// Dtor
//
SingleShotTimer::~SingleShotTimer()
    throw()
{
    try {
        // set observer to NULL before stop. 
        // Thus, avoid problem if timer expire before stop done
        m_pObserver = NULL;
        stop();
    } catch(Cdmw::InternalErrorException) {
        std::cerr << "stop of timer raise InternalErrorException in destructor" << std::endl;
    }
}



//
// Ask to the internal thread for stopping
//
void SingleShotTimer::stop()
    throw (Cdmw::InternalErrorException)
{
    if (m_threadRunning) {
        try {
            {
                CDMW_MUTEX_GUARD(m_mutex);

                // cancel timer
                m_timerStarted = false;
                // stop thread
                m_threadRunning = false;

                // awake the thread, if it is sleeping
                m_condition.broadcast();
            }
            
            // Wait the termination of the internal thread
            // outside of the mutex guard to let the thread to finish
            this->join();
        
        } catch (const Cdmw::OsSupport::DeadlockException&) {
            // No explanation, how is it possible : corrupted
            CDMW_THROW (Cdmw::InternalErrorException);
        } catch (...) {
            CDMW_THROW (Cdmw::InternalErrorException);
        }
    }
 }


//
// Change the timeout value
//
void SingleShotTimer::setup (unsigned long timeoutValue)
    throw (Cdmw::BadOrderException)
{
    CDMW_MUTEX_GUARD(m_mutex);
    if (m_timerStarted) {
        // timer already started, cannot change timeout value
        CDMW_THROW(Cdmw::BadOrderException);
    }
    
    m_timeoutValue = timeoutValue;
}

//
// Start the timer 
//
void SingleShotTimer::start_timer ()
  throw (Cdmw::AlreadyDoneException)
{
    CDMW_MUTEX_GUARD (m_mutex);

    if (m_timerStarted) {
        // timer already started, cannot start it twice
        CDMW_THROW(Cdmw::AlreadyDoneException);
    }
    // start timer
    m_timerStarted = true;
    // awake the thread which is waiting on condition 
    // (see m_condition.wait() without timeout argument)
    m_condition.broadcast();
}

//
// Cancel the timer 
//
void SingleShotTimer::cancel_timer ()
  throw (Cdmw::AlreadyDoneException)
{
    CDMW_MUTEX_GUARD (m_mutex);
    if (!m_timerStarted) {
        // timer not started, nothing to cancel
        CDMW_THROW(Cdmw::AlreadyDoneException);
    }
    // stop timer
    m_timerStarted = false;
    // awake the thread which is waiting on condition 
    // (see m_condition.wait(m_timeoutValue) )
    m_condition.broadcast();
}


//
// Call in a new thread by the Thread system
//
void SingleShotTimer::run()
    throw()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    // Keep mutex. 
    // It will be released during m_condition.wait() calls
    // because m_condition use this mutex.
    CDMW_MUTEX_GUARD(m_mutex);
    
    // Status returned by the m_condition.wait(m_timeoutValue) call.
    // If TIMEOUT, then timer has been fired.
    Cdmw::OsSupport::Condition::WaitReturn status;	

    // the thread is running
    m_threadRunning = true;

    // Infinite loop, stopped by call to stop() operation
    // which set m_threadRunning to false
    while (m_threadRunning) {

        // loop while thread hasn't been stopped
        // and timer hasn't been started
        while (m_threadRunning && !m_timerStarted) {
            try {
                // wait until wake by start_timer() or stop()
                m_condition.wait();
            } catch (const InternalErrorException& e) {
                // shouldn't happen
                CDMW_FATAL(FTLogger::GetLogger(), "Caught exception '" << e.what() << "'");
                CDMW_ASSERT(false);
            }
        }
        
        // initialize status to something different than TIMEDOUT
        status = Cdmw::OsSupport::Condition::SIGNALLED;
        
        // loop while thread hasn't been stopped,
        // or timer hasn't been canceled
        // or timer hasn't expired
        while (m_threadRunning && m_timerStarted 
               && status != Cdmw::OsSupport::Condition::TIMEDOUT)
        {
            try {
                // wait until wake by stop(), cancel_timer()
                // or timer expiration
                status = m_condition.wait(m_timeoutValue);
            } catch (const InternalErrorException& e) {
                // shouldn't happen
                CDMW_FATAL(FTLogger::GetLogger(), "Caught exception '" << e.what() << "'");
                CDMW_ASSERT(false);
            }
        }
        
        // if thread hasn't been stopped
        // and timer has expired
        if (m_threadRunning && 
            status == Cdmw::OsSupport::Condition::TIMEDOUT)
        {
            // timer has expired, it won't be restarted automatically
            m_timerStarted = false;
            
            // wake observer up, if exists
            if (m_pObserver != NULL) {
                // Unlock m_mutex just for wake_up() call, to allow re-entrant calls.
                // Note: wake_up() doesn't raise any exception.
                m_mutex.unlock();
                m_pObserver->wake_up();
                m_mutex.lock();
            }
        }
        
    } // end of while(m_threadRunning)

    // thread ended, cancel timer which could have been set
    // during wake_up
    m_timerStarted = false;

}


} // End namespace Supervision
}  // End namespace FT
}  // End namespace Cdmw
