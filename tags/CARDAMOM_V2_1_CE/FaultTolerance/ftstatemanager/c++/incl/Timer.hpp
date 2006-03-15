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

#ifndef _CDMW_FT_TIMER_HPP_
#define _CDMW_FT_TIMER_HPP_

#include <Foundation/osthreads/ThreadHandle.hpp>
#include <Foundation/osthreads/Mutex.hpp>
#include <Foundation/osthreads/Condition.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <FaultTolerance/idllib/CdmwFTTPCCommon.stub.hpp>
#include <ace/Reactor.h>
#include <ace/Task.h>
#include <ace/TP_Reactor.h>
#include <ace/Timer_Hash.h>
#include <ace/Timer_List.h>

namespace Cdmw {
namespace FT {

    template <typename Identification>
    /**
     * Timer_Event_Handler 
     */
    class Timer_Event_Handler : public ACE_Event_Handler
    {
    public:
        
        virtual int handle_timeout(const Identification* tid) = 0;

    protected:
        int handle_timeout(const ACE_Time_Value &tv, const void* arg)
        {
            const Identification* tid =
        static_cast< const Identification* > (arg);
            return this->handle_timeout(tid);
        }
        
        
    };


   /**
   * TimerBase 
   */
    class TimerBase : public ACE_Task_Base
    {
    public:
        TimerBase();

        ~TimerBase() throw ();

        void stop();

        int cancel_timer (int timer_id, const void** act = 0);

        //int cancel_timer (int timer_id);

        virtual int start (void);

        virtual int svc (void);

    protected:
        ACE_Timer_Queue * m_timer_queue;

    private:
        bool m_stop;

    };    
    

  /**
   * Timer 
   */
    template <typename Identification>
    class Timer : public TimerBase
    {
    public:
        typedef int TimerId;

        TimerId start_timer(
            Timer_Event_Handler<Identification> *event_handler, 
            Identification* id, 
            OsSupport::OS::Timeval time);
    };

    template <typename Identification>
    typename Timer<Identification>::TimerId
    Timer<Identification>::start_timer(
        Timer_Event_Handler<Identification> *event_handler,
        Identification* id, OsSupport::OS::Timeval time)
    {
       
        ACE_Time_Value current = ACE_OS::gettimeofday();
        ACE_Time_Value ace_time(time.seconds, time.microseconds);
        ACE_Time_Value new_time = current + ace_time;
        int res = m_timer_queue->schedule (event_handler, (const void*)id,
                       new_time);
        return res;
    }


}; // namespace FT
}; // namespace Cdmw

#endif // _CDMW_FT_TIMER_HPP_
