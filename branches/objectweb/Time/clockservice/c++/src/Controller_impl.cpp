/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * ========================================================================= */

#include <Time/clockservice/Controller_impl.hpp>
#include <Time/clockservice/Executor_impl.hpp>
#include <Time/clockservice/Util.hpp>
#include <Foundation/common/Assert.hpp>

#include <iostream>
#include <cmath>
#include <ace/Log_Msg.h>


using namespace Cdmw::clock;
using namespace Cdmw::clock::util;

namespace 
{
    double
    checked_rate( const IClock& ref )
    throw( CORBA::BAD_PARAM )
    {
        double out = ref.retrieve_rate();
        if(out == 0.0)
            throw CORBA::BAD_PARAM();
        return out;
    }

    
    struct SimpleTrace
    {
        static int counter_;
        std::string m_name;
        int m_line;
        std::ostream& m_os;
        SimpleTrace(const std::string& name,
                    int line,
                    std::ostream& os=std::cerr)  
           :m_name (name) 
           ,m_line (line)
           ,m_os   (os)
        {
            tab();
            m_os<<m_name<<" Begin  Line: "<<m_line<<std::endl; 
            ++counter_;
        }

        void tab()
        {
            int tmp = counter_;
            CDMW_ASSERT(tmp>=0);
            while(2*(tmp--))
              m_os<<' ';
        }

        ~SimpleTrace()
        {
            --counter_;
            tab();
            m_os<<m_name<<" End"<<std::endl;
        }
    };
    int SimpleTrace::counter_ = 0;
}

///////////////////////////////////////////////////////////////////////////
//                 -- Controller_impl _implementation --

Controller_impl::Controller_impl(
    IClock& clock,
    CosClockService::PeriodicExecution::Periodic_ptr periodic,
    ACE_Reactor* reactor)
    :  clock_(clock),
       reactor_(reactor),
       handler_(this, periodic,mutex_),
       timerKey_(-1) 
{
}

///////////////////////////////////////////////////////////////////////////////

Controller_impl::~Controller_impl()
{
    // No-Op
    this->cancelTimer();
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
    //   SimpleTrace st("start",__LINE__);
 
    ACE_Guard<ACE_Mutex> guard(this->mutex_);

    this->cancelTimerLockFree();
    any_ = params;

    this->update_period(period);
    this->update_offset(with_offset);

    handler_.executionLimit() = execution_limit;
    handler_.executions() = 0;
    timerKey_ = reactor_->schedule_timer(&handler_, 
                                         0, 
                                         delay_, 
                                         period_);
    CDMW_ASSERT( timerKey_!=-1 &&
                "Not valid handler available!");
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
    // SimpleTrace st("start_at",__LINE__);

    TimeBase::TimeT 
      chekedOffset = this->checked_offset(at_time);
    TimeBase::TimeT
      now = Clock_impl::compute_current_time();
    
    // if chekedOffset if >> 1 then probably can assert 
    CDMW_ASSERT( now+chekedOffset >= now );

    this->start(period,
                chekedOffset,
                execution_limit,
                params);
}

///////////////////////////////////////////////////////////////////////////////

void 
Controller_impl::pause()
    throw (CORBA::SystemException)
{
    // NOTE: The current assumption under pause is that once the timer will
    // be resumed it will start counting a full period.
    //
    // TODO: This should be changed to make sure that we take into
    // account the current time from the right clock and consider the time
    // left.
     this->cancelTimer();
}

///////////////////////////////////////////////////////////////////////////////

void 
Controller_impl::resume ()
    throw (CORBA::SystemException)
{
    timerKey_ = reactor_->schedule_timer(&handler_,
                                         0,
                                         delay_,
                                         period_);
    CDMW_ASSERT( timerKey_!=-1 &&
                "Not valid handler available!");
}

///////////////////////////////////////////////////////////////////////////////

void 
Controller_impl::resume_at (TimeBase::TimeT at_time)
    throw (CORBA::SystemException,
           CosClockService::PeriodicExecution::Controller::TimePast)
{
    TimeBase::TimeT 
        chekedOffset = this->checked_offset(at_time);
    this->update_offset(chekedOffset);

    timerKey_ = reactor_->schedule_timer(&handler_, 
                                         0, 
                                         delay_, 
                                         period_); 
    CDMW_ASSERT(timerKey_!=-1 &&
                "Not valid handler available!");
}

///////////////////////////////////////////////////////////////////////////////

void 
Controller_impl::terminate()
    throw (CORBA::SystemException)
{
    this->cancelTimer();
}

///////////////////////////////////////////////////////////////////////////////

CORBA::ULong
Controller_impl::executions()
    throw (CORBA::SystemException)
{
    return handler_.executions();
}

///////////////////////////////////////////////////////////////////////////////

void
Controller_impl::cancelTimer() 
{
    ACE_Guard<ACE_Mutex> guard(this->mutex_);

    this->cancelTimerLockFree();
}
    
///////////////////////////////////////////////////////////////////////////////////

void
Controller_impl::cancelTimerLockFree()
{
    // No need to lock since this method is always called by method
    // that have already aquired the mutex.

    // This check can occur because if an exception is thrown 
    //  timerKey_ is -1
    if (timerKey_ != -1) 
    {
        int res;
        res  = reactor_->cancel_timer(timerKey_);
        CDMW_ASSERT( res == 1 && 
                    "Timer was not canceled properly!");

        timerKey_ = -1;
    }
}

///////////////////////////////////////////////////////////////////////////////

CosClockService::PeriodicExecution::Periodic_ptr
Controller_impl::periodic()
{
    return handler_.periodic();
}

///////////////////////////////////////////////////////////////////////////////
//                 -- Timeout Handler _implementation --
///////////////////////////////////////////////////////////////////////////////

Controller_impl::TimeoutHandler::TimeoutHandler
(Controller_impl* controller,
 CosClockService::PeriodicExecution::Periodic_ptr periodic,
 ACE_Mutex& mutex)
:mutex_(mutex)
{
    controller_ = controller;
    periodic_ = 
        CosClockService::PeriodicExecution::Periodic::_duplicate(periodic);
}

///////////////////////////////////////////////////////////////////////////////

Controller_impl::TimeoutHandler::~TimeoutHandler() 
{
    // No-Op
}

///////////////////////////////////////////////////////////////////////////////
int
Controller_impl::TimeoutHandler::handle_close(ACE_HANDLE,
                                              ACE_Reactor_Mask)
{
   return 0;
}

///////////////////////////////////////////////////////////////////////////////

int 
Controller_impl::TimeoutHandler::handle_timeout(const ACE_Time_Value& /*current_time*/,
                                                const void* /* act */)
{
    ACE_Guard<ACE_Mutex> guard(this->mutex_);
    // SimpleTrace st("handle_timeout",__LINE__);

    size_t currentExecution = executions_;
    
    // until timer is valid callback is called
    if (executionsLimit_ == INF_EXEC_LIMIT || 
        currentExecution <  executionsLimit_) 
    {
        ++executions_;
        try
        {
            CORBA::Boolean on =
                periodic_->do_work(controller_->any_);

            if (on)
                return 0;
        }
        catch (const CORBA::SystemException& e)
        {
            std::cerr << "Controller:>> Exception raised while "
                      << " delivering timeout!\n"
                      << e 
                      << std::endl; 
            throw;
        }
    }
    
    // else timer will be removed
    controller_->cancelTimerLockFree();
    return -1;
}

///////////////////////////////////////////////////////////////////////////////            

CORBA::ULong& 
Controller_impl::TimeoutHandler::executionLimit()
{
    return executionsLimit_;   
}

///////////////////////////////////////////////////////////////////////////////

CORBA::ULong&
Controller_impl::TimeoutHandler::executions()
{
    return executions_;   
}

////////////////////////////////////////////////////////////////////////////////

TimeBase::TimeT
Controller_impl::checked_offset(TimeBase::TimeT at_time)
{
    //SimpleTrace st("Controller_impl::checked_offset",__LINE__);
    TimeBase::TimeT now = Clock_impl::compute_current_time(); 
    TimeBase::TimeT offset =  0;

    // controlled clock can have a negative rate
    double  tmp_rate = checked_rate(clock_);
    if (tmp_rate>0)
    {
        if (now < at_time)
           offset = at_time - now;
        else
           throw CosClockService::PeriodicExecution::Controller::TimePast();
    }
    else
    {
       if (at_time < now)
           offset = now - at_time;
       else
           throw CosClockService::PeriodicExecution::Controller::TimePast();
    }
    return offset;
}

////////////////////////////////////////////////////////////////////////////////

void    
Controller_impl::update_offset(TimeBase::TimeT   offset)
{
    double rate = fabs(checked_rate(clock_));
    double tmp_offset = convert(offset);
    tmp_offset *= rate;
    delay_.set(0, (unsigned long)(tmp_offset / 10.0));
}

////////////////////////////////////////////////////////////////////////////////

void    
Controller_impl::update_period(TimeBase::TimeT   period)
{
    double rate;
    rate = fabs(checked_rate(clock_));
    double tmp_period;
    tmp_period = convert(period)/(10.0 * rate);
    unsigned long ulong_period;
    ulong_period = (unsigned long)tmp_period;

    // workaround for ACE problem
    if( period < 1 ||
        tmp_period < 1.0 )
        ulong_period = 1;
    period_.set(0, ulong_period );
}

////////////////////////////////////////////////////////////////////////////////
