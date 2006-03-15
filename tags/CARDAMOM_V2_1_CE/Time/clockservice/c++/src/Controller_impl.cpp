/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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


#include "clockservice/Controller_impl.hpp"
#include "clockservice/Executor_impl.hpp"
#include "clockservice/ControllerUpdateAdmin_impl.hpp"
#include "Time/clockservice/Util.hpp"

#include <Foundation/common/Assert.hpp>

#include <iostream>
#include <cmath>

using namespace Cdmw::clock;
using namespace Cdmw::clock::util;

namespace 
{
    const   TimeBase::TimeT
        S_invalid_TimeT = (TimeBase::TimeT)-1;
        
    double
    checked_rate( const IClock& ref )
    throw( CORBA::BAD_PARAM )
    {
        double 
            out = ref.retrieve_rate();
        if(out == 0.0)
            throw CORBA::BAD_PARAM();
        return out;
    }
}

TimeBase::TimeT Cdmw::clock::compute_delta2end(const IClock&          clock,
                                               const TimeBase::TimeT& period,
                                               const TimeBase::TimeT& time_origin)
{
    TimeBase::TimeT delta2end = 0;
    long double d_rate = (long double) clock.retrieve_rate();
    CDMW_ASSERT(d_rate != 0);
    if (d_rate == 0.0)
    {
        return delta2end;
    }
    TimeBase::TimeT vt = const_cast<IClock&>(clock).current_time();
    TimeBase::TimeT vx, v1_vx, vx_v0;
    if (vt > time_origin)
    {
        vx = vt - time_origin;
        vx_v0 = vx % period;
        v1_vx = period - vx_v0;
    }
    else
    {
        vx = time_origin - vt;
        v1_vx = vx % period;
        vx_v0 = period - v1_vx;
    }
    if (d_rate > 0)
        delta2end = (TimeBase::TimeT) (v1_vx / d_rate);  // NB: converting directly to TimeBase::TimeT ...
    else
        delta2end = (TimeBase::TimeT) (vx_v0 / -d_rate); // NB: converting directly to TimeBase::TimeT ...
    return delta2end;
}

///////////////////////////////////////////////////////////////////////////
//                 -- Controller_impl _implementation --

Controller_impl::Controller_impl(Executor_impl&                                   clock,
                                 CosClockService::PeriodicExecution::Periodic_ptr periodic,
                                 ACE_Reactor*                                     reactor)
    : m_clock(clock)
    , m_reactor(reactor)
    , m_handler(this, periodic, m_mutex)
    , m_timer_key(-1)
    , m_ID(Cdmw::clock::policy::UNDEFINED) 
    , m_status(CREATED)
{
    m_controller_time_origin = m_clock.current_time();
}

///////////////////////////////////////////////////////////////////////////////

Controller_impl::~Controller_impl()
{
    // No-Op
    this->cancelTimer();
}

///////////////////////////////////////////////////////////////////////////////

void  
Controller_impl::scheduleTimerLockFree(TimeBase::TimeT period, 
                                       TimeBase::TimeT with_offset)
{
    this->update_offset(with_offset);
    this->update_period(period);

    m_timer_key = m_reactor->schedule_timer(&m_handler, 0, m_delay, m_period);
    CDMW_ASSERT(m_timer_key != -1);
}

///////////////////////////////////////////////////////////////////////////////

void
Controller_impl::start(TimeBase::TimeT period, 
                       TimeBase::TimeT with_offset,
                       CORBA::ULong execution_limit,
                       const CORBA::Any & params)
    throw (CORBA::SystemException)
{
    // If the timer had already been programmed, this is 
    // interpreted as a re-programmation. Thus, the former 
    // alarm is reset to this.
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->m_mutex);

    if (m_status != CREATED && m_status != STARTED)
    {
        std::cerr << "ERROR: Controller is neither CREATED nor STARTED" << std::endl;
        throw CORBA::BAD_INV_ORDER();
    }
    
    this->cancelTimerLockFree();
    m_real_period = period;
    m_any = params;
    m_handler.executionLimit() = (long) execution_limit;
    m_handler.executions() = TimerAtomicCount(0);
    
    this->scheduleTimerLockFree(period, with_offset);
    set_controller_time_origin( m_clock.current_time() + with_offset);
    m_status = STARTED;        
}

///////////////////////////////////////////////////////////////////////////////

void 
Controller_impl::start_at(TimeBase::TimeT   period ,
                          TimeBase::TimeT   at_time,
                          CORBA::ULong      execution_limit,
                          const CORBA::Any& params)

    throw (CORBA::SystemException,
           CosClockService::PeriodicExecution::Controller::TimePast)
{
   TimeBase::TimeT 
      checkedOffset = this->checked_offset(at_time);
    TimeBase::TimeT 
      now = m_clock.current_time();
    
    // if checkedOffset is >> 1 then probably can assert 
    CDMW_ASSERT( now+checkedOffset >= now );
    if( now+checkedOffset < now )
        throw CosClockService::PeriodicExecution::Controller::TimePast();

    this->start(period,
                checkedOffset,
                execution_limit,
                params);
}

///////////////////////////////////////////////////////////////////////////////

void 
Controller_impl::pause()
    throw (CORBA::SystemException)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->m_mutex);
    // NOTE: The current assumption under pause is that once the timer will
    // be resumed it will start counting a full period.
    //
    // TODO: This should be changed to make sure that we take into
    // account the current time from the right clock and consider the time
    // left.
    if (m_status != STARTED) 
    { 
	std::cerr<<"ERROR: Controller is not STARTED "<<std::endl; 
        throw CORBA::BAD_INV_ORDER ();
    }
    this->cancelTimerLockFree();

    m_delta2end = compute_delta2end(m_clock, m_real_period, m_controller_time_origin);
    m_status    = PAUSED;
}

///////////////////////////////////////////////////////////////////////////////

void 
Controller_impl::resume()
    throw (CORBA::SystemException)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->m_mutex);

    /*
    std::cerr << "<delta2end, period> = <"
              << Cdmw::clock::util::TimeT2seconds(m_delta2end)
              << ", "
              << Cdmw::clock::util::TimeT2seconds(m_real_period)
              << ">"
              << std::endl;
    */

    if (m_status != PAUSED) 
    { 
	std::cerr<<"ERROR: Controller is not PAUSED "<<std::endl; 
        throw CORBA::BAD_INV_ORDER ();
    }

    this->scheduleTimerLockFree(m_real_period, m_delta2end);
    m_status = STARTED;
}

///////////////////////////////////////////////////////////////////////////////

void 
Controller_impl::resume_at (TimeBase::TimeT at_time)
    throw (CORBA::SystemException,
           CosClockService::PeriodicExecution::Controller::TimePast)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->m_mutex);
    
    if (m_status != PAUSED)
    { 
	std::cerr<<"ERROR: Controller is not PAUSED "<<std::endl; 
        throw CORBA::BAD_INV_ORDER ();
    }

    TimeBase::TimeT 
        checkedOffset = this->checked_offset(at_time);
        
    this->scheduleTimerLockFree(m_real_period,
                                checkedOffset);
    m_status = STARTED;
}

///////////////////////////////////////////////////////////////////////////////

void 
Controller_impl::terminate()
    throw (CORBA::SystemException)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->m_mutex);
    m_status = TERMINATED;
    this->cancelTimerLockFree();
    m_clock.detach_terminated_controller(this);
}

///////////////////////////////////////////////////////////////////////////////

CORBA::ULong
Controller_impl::executions()
    throw (CORBA::SystemException)
{
    return (CORBA::ULong) ((long) m_handler.executions());
}

///////////////////////////////////////////////////////////////////////////////

void
Controller_impl::cancelTimer() 
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->m_mutex);
    this->cancelTimerLockFree();
}

///////////////////////////////////////////////////////////////////////////////
    
void
Controller_impl::freezeTimer() 
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->m_mutex);
    this->freezeTimerLockFree();
}

///////////////////////////////////////////////////////////////////////////////

void
Controller_impl::cancelTimerLockFree()
{
    // No need to lock since this method is always called by method
    // that have already aquired the mutex.

    // This check can occur because if an exception is thrown 
    //  m_timer_key is -1
    if (m_timer_key != -1) 
    {
        try 
        {
            int 
                res = m_reactor->cancel_timer(m_timer_key);
            if( res != 1 )
               std::cerr<<"Timer was not canceled properly!"<<std::endl;

        } catch (...) 
        {
            std::cerr << "Exception during  m_reactor->cancel_timer (" <<m_timer_key <<")"<< std::endl; 
        }
        m_timer_key = -1;
    }
}

///////////////////////////////////////////////////////////////////////////////////

void
Controller_impl::freezeTimerLockFree()
{
    // No need to lock since this method is always called by method
    // that have already aquired the mutex.

    // This check can occur because if an exception is thrown 
    //  m_timer_key is -1
    if (m_timer_key != -1) 
    {
        try 
        {
            int 
                res  = m_reactor->cancel_timer(m_timer_key);
            if( res != 1 )
               std::cerr<<"Timer was not canceled properly!"<<std::endl;              
//            CDMW_ASSERT( res == 1 && 
//                        "Timer was not canceled properly!");
        } 
        catch (...) 
        {
            std::cerr << "Exception during  m_reactor->cancel_timer (" <<m_timer_key <<")"<< std::endl; 
        }

        m_timer_key = -1;
    }
}
///////////////////////////////////////////////////////////////////////////////

CosClockService::PeriodicExecution::Periodic_ptr
Controller_impl::periodic()
{
    return m_handler.periodic();
}

///////////////////////////////////////////////////////////////////////////////
//                 -- Timeout Handler _implementation --
///////////////////////////////////////////////////////////////////////////////

Controller_impl::TimeoutHandler::TimeoutHandler
(Controller_impl* controller,
 CosClockService::PeriodicExecution::Periodic_ptr periodic,
 ACE_Recursive_Thread_Mutex& mutex)
:executions_(0)
,m_mutex(mutex)
{
    CDMW_ASSERT(controller);
    controller_ = controller;
    CDMW_ASSERT(!CORBA::is_nil(periodic));
    periodic_ = 
        CosClockService::PeriodicExecution::Periodic::_duplicate(periodic);
    CDMW_ASSERT(!CORBA::is_nil(periodic_.in()));
}

///////////////////////////////////////////////////////////////////////////////

Controller_impl::TimeoutHandler::~TimeoutHandler() 
{
    // No-Op
}

///////////////////////////////////////////////////////////////////////////////

int 
Controller_impl::TimeoutHandler::handle_timeout(const ACE_Time_Value& /*current_time*/,
                                                const void* /* act */)
{
    int  return_value = 0;
    long executions = executions_++;
    bool condition  = (executionsLimit_ == INF_EXEC_LIMIT || executions < executionsLimit_);
    executions = (condition ? 0 : executions_--); // NB: just to correct executions_ in case of overflow

    // handle the case of non-overflowing execution (call to periodic callback)
    if (condition)
    {
        try
        {
            CORBA::Boolean on = periodic_->do_work(controller_->m_any);
            if (!on) return_value = -1;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Controller:>> std exception caught: " << e.what() << std::endl;
        }
        catch (const CORBA::SystemException& e)
        {
            std::cerr << "Controller:>> Exception raised while "
                      << " delivering timeout!\n"
                      << e 
                      << std::endl;
        }
    }

    // handle the case of overflowing execution or execution failure
    if (!condition || return_value == -1)
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->m_mutex);
        controller_->cancelTimerLockFree();
        return_value = -1;
    }

    // return to caller
    return return_value;
}

///////////////////////////////////////////////////////////////////////////////            

long& 
Controller_impl::TimeoutHandler::executionLimit()
{
    return executionsLimit_;   
}

///////////////////////////////////////////////////////////////////////////////

const long& 
Controller_impl::TimeoutHandler::executionLimit() const
{
    return executionsLimit_;   
}

///////////////////////////////////////////////////////////////////////////////

TimerAtomicCount&
Controller_impl::TimeoutHandler::executions()
{
    return executions_;   
}

////////////////////////////////////////////////////////////////////////////////

TimeBase::TimeT
Controller_impl::checked_offset(TimeBase::TimeT at_time) const
{
    TimeBase::TimeT 
        now = m_clock.current_time(),
        offset =  0;

    // controlled clock can have a negative rate
    double  
        tmp_rate = checked_rate(m_clock);
    if (tmp_rate>0)
    {
        if (now < at_time)
            offset = at_time - now;
        else
        {
            /*
            std::cerr << "Controller_impl::checked_offset: now - at_time (sec): "
                      << Cdmw::clock::util::TimeT2seconds(now - at_time)
                      << std::endl;
            */
            throw CosClockService::PeriodicExecution::Controller::TimePast();
        }
    }
    else
    {
        if (at_time < now)
            offset = now - at_time;
        else
        {
            /*
            std::cerr << "Controller_impl::checked_offset: at_time - now (sec): "
                      << Cdmw::clock::util::TimeT2seconds(at_time - now)
                      << std::endl;
            */
            throw CosClockService::PeriodicExecution::Controller::TimePast();
        }
    }
    return offset;
}

////////////////////////////////////////////////////////////////////////////////

void    
Controller_impl::update_offset(TimeBase::TimeT   offset)
{
    long double 
        rate = fabs(checked_rate(m_clock)),
        tmp_offset;
    tmp_offset = offset /(10.0 * rate);;
    // std::cerr<<"Controller_impl::update_offset: "<<TimeT2seconds((TimeBase::TimeT)(offset*rate))<<std::endl;    
    m_delay.set(0, (unsigned long)(tmp_offset));
}

////////////////////////////////////////////////////////////////////////////////C

Controller_impl::ControllerState_t
Controller_impl::get_status() const
{
    return m_status; 
}

////////////////////////////////////////////////////////////////////////////////

void    
Controller_impl::update_period(TimeBase::TimeT   period)
{
    //CDMW_ASSERT(S_invalid_TimeT != period);            
    long double 
        rate = fabs(checked_rate(m_clock)),
    // std::cerr<<"Controller_impl::update_period, rate: "<<rate<<std::endl;
        tmp_period = period/(10.0 * rate);
    unsigned long 
        ulong_period = (unsigned long)tmp_period;

    // workaround for ACE problem
    if( period < 1 ||
        tmp_period < 1.0 )
        ulong_period = 1;
    m_period.set(0, ulong_period );
}

////////////////////////////////////////////////////////////////////////////////

void 
Controller_impl::set_update_policy (::Cdmw::clock::policy::ControllerUpdateID id )
        throw (CORBA::SystemException,
               ::Cdmw::clock::policy::ControllerUpdateAdmin::Unavailable)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->m_mutex);
    this->m_ID = id;
    
    Cdmw::clock::policy::ControllerUpdateAdmin_impl::instance()->get_policy(this->m_ID); 
}
////////////////////////////////////////////////////////////////////////////////

Cdmw::clock::policy::ControllerUpdateID 
Controller_impl::get_update_policy ()
        throw (CORBA::SystemException)
{
    return this->m_ID;
}

////////////////////////////////////////////////////////////////////////////////

Cdmw::clock::Executor_impl*
Controller_impl::retrieve_executor()
{
    return &m_clock;
}

////////////////////////////////////////////////////////////////////////////////

CORBA::Any
Controller_impl::get_parameter() const
{
    return m_any;  
}

////////////////////////////////////////////////////////////////////////////////

TimeBase::TimeT 
Controller_impl::get_period() const
{
   return  m_real_period;
}


////////////////////////////////////////////////////////////////////////////////

CORBA::ULong
Controller_impl::execution_limit() const
{
   return (CORBA::ULong) (m_handler.executionLimit());
}

////////////////////////////////////////////////////////////////////////////////

TimeBase::TimeT Controller_impl::get_controller_time_origin ()
        throw (CORBA::SystemException)
{
    //CDMW_ASSERT(m_controller_time_origin != S_invalid_TimeT);
    return m_controller_time_origin;
}      
////////////////////////////////////////////////////////////////////////////////

void Controller_impl::set_controller_time_origin (TimeBase::TimeT time_origin) 
        throw (CORBA::SystemException)
{
    // CDMW_ASSERT(time_origin != S_invalid_TimeT);    
	m_controller_time_origin = time_origin;
}
////////////////////////////////////////////////////////////////////////////////


