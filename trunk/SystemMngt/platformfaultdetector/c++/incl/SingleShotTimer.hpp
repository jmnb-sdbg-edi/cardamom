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

#ifndef INCL_CDMW_PLATFORMMNGT_SINGLESHOTTIMER_HPP
#define INCL_CDMW_PLATFORMMNGT_SINGLESHOTTIMER_HPP

// private include files
#include "platformfaultdetector/TimeoutObserver.hpp"

// include files
#include <string>

#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Condition.hpp"

/**
* Namespace for Supervision
*/
namespace Cdmw {
namespace PlatformMngt 
{


/**
*Purpose:
*<p>    This class implements a cancelable single shot timer
*       wich can be started and stopped multiple times and
*       with a modifiable timeout
*
*Features:
*<p> Thread safe, exception safe
*
*
*/
class SingleShotTimer : public Cdmw::OsSupport::Thread
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
        *@param observer Observer whose wake_up method will be called
        * at timeout of the timer
        *@param timeoutValue timeout of the timer
        *
        */ 
        SingleShotTimer (TimeoutObserver* p_observer,
                         unsigned long timeoutValue);

        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~SingleShotTimer()
                throw();


        /**
        * Purpose:
        * <p> Ask to the internal thread to stop its job, 
        * When this function return the internal thread have stopped.
        *
        *
        */ 
        void stop()
            throw (Cdmw::InternalErrorException);


                
        /**
        * Purpose:
        * <p> Change the timeout value
        *
        *@param unsigned long timeoutValue
        *
        *@exception BadOrderException if timer is already started
        */ 
        void setup (unsigned long timeoutValue)
            throw (Cdmw::BadOrderException);
        
        /**
        * Purpose:
        * <p> Start the timer
        *     It will wait with the current timeout
        *
        *@exception AlreadyDoneException if timer is already started
        */ 
        void start_timer ()
            throw (Cdmw::AlreadyDoneException);
        
        /**
        * Purpose:
        * <p> Cancel the timer
        *     Then the timer is ready to be started again
        *
        *@exception AlreadyDoneException if timer is already canceled
        */ 
        void cancel_timer ()
            throw (Cdmw::AlreadyDoneException);
        
  
    protected:


        /**
        * Purpose:
        * <p> Call by the underlying thread system in a new thread
        * 
        */ 
        virtual
        void run()
            throw();        
        
    private:

        //
        // Copy ctor
        //
        SingleShotTimer (const SingleShotTimer& rhs)
                throw();


        //
        // Assignement operator
        //
        SingleShotTimer& operator=(const SingleShotTimer& rhs)
                 throw();


        /**
        * At each iteration the thread test this variable, to
        * see if it must stop it's processing. If false
        * the thread terminates.
        */
        bool m_threadRunning;    
        

        /**
        * Used to forbid a timeout change if timer is started
        * Set at the start and stop command and after timeout
        */
        bool m_timerStarted;
        
        /**
        * Mutex protecting stop flag and associated to the following
        * two conditions
        */
        Cdmw::OsSupport::Mutex m_mutex;


        /**
        * Condition on which the thread is sleeping to wait 
        * either the start of the timer, either the end of the timer
        * (with a timout or a cancel timer).
        */
        Cdmw::OsSupport::Condition m_condition;


        /**
        * monitoring timeout used by PullMonitoringCall object
        * for polling in msec
        */
        unsigned long m_timeoutValue;

        
        /**
        * associated TimeoutObserver whose wake_up method will be called
        * at timeout
        */
        TimeoutObserver* m_pObserver;
                
}; // End class SingleShotTimer 

} // End namespace PlatformMngt 
}  // End namespace Cdmw

#endif // INCL_CDMW_PLATFORMMNGT_SINGLESHOTTIMER_HPP
