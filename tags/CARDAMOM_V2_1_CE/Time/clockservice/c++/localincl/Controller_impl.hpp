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

#ifndef CARDAMOM_CLOCK_CONTROLLER_IMPL_HPP
#define CARDAMOM_CLOCK_CONTROLLER_IMPL_HPP

#include "Foundation/orbsupport/RefCountServantBase.hpp"

#include <Time/clockservice/CdmwTime.skel.hpp>
#include <Time/clockservice/config.hpp>

#include "clockservice/ClockStatus.hpp"

#include "ace/Reactor.h"
#include "ace/Synch.h"
#include "ace/Event_Handler.h"
#include "clockservice/TimerAtomicCount.hpp"

namespace Cdmw { namespace clock { namespace policy {
    class ControllerUpdate_impl;
} /* policy */ } /* clock */ } /* Cdmw */


namespace Cdmw { namespace clock {
    class Executor_impl;
    class IClock;
} /* clock */ } /* Cdmw */


namespace Cdmw { namespace clock {

/**
 * @brief This class implements the basic Controller functionalities
 * described in the EVoT specification.  
 *
 * Allows control of periodic 
 * execution after the appropriate object has been registered with 
 * the clock.
 *
 * @author Angelo Corsaro <acorsaro@selex-si.com>
 * @author Fabrizio Morciano<fmorciano@selex-si.com>
 */
class CLOCK_API  Controller_impl 
:   public virtual POA_Cdmw::clock::ControllerEx
,   public virtual Cdmw::clock::ControllerEx
,   public virtual Cdmw::OrbSupport::RefCountServantBase
{
public:
    /**
     * Creates a <code>Controller_impl</code> with an associated 
     * <code>Periodic</code> and <code>Reactor</code>
     *
     * @param clock The clock used by this controller to measure time.
     *
     * @param periodic The <code>Periodic</code> instance that will
     *         be notified when the programmed time interval expires.
     *
     * @param reactor The reactor that will be used to register timeout 
     *         handlers.
     */
    Controller_impl(Cdmw::clock::Executor_impl& clock,
                    CosClockService::PeriodicExecution::Periodic_ptr periodic,
                    ACE_Reactor* reactor);
    
    /**
     * Releases resources associated with the instance.
     */
   virtual ~Controller_impl();

   /**
    * Initiates periodic execution with a specified period for a 
    * specified count of executions. Specifying an execution limit 
    * of 0 is interpreted as an unbounded number of executions. 
    *
    * 
    * @param  period the period after which this controller will
    *         schedule the execution of the associated 
    *         <code>Periodic</code>
    *
    * @param with_offset this parameter may be used to delay the start 
    *        of the first execution. 
    *
    * @param execution_limit the maximum number of periodic executions
    *        that have to be scheduled. an execution limit 
    *        of 0 is interpreted as an unbounded number of executions.
    *
    * @param params This parameter will be passed to each invocation.
    */
    virtual void start(TimeBase::TimeT    period, 
                       TimeBase::TimeT    with_offset,
                       CORBA::ULong       execution_limit,
                       const CORBA::Any&  params)
        throw (CORBA::SystemException);

    /**
     * Identical to the start operation except that the 
     * <code>at_time</code> parameter specifies an absolute 
     * time for the start of the first execution.
     *
     * @param  period the period after which this controller will
     *         schedule the execution of the associated 
     *         <code>Periodic</code>
     *
     * @param at_time specifies an absolute time for the start 
     *        of the first execution.
     *
     * @param execution_limit the maximum number of periodic executions
     *        that have to be scheduled. an execution limit 
     *        of 0 is interpreted as an unbounded number of executions.
     *
     * @param params This parameter will be passed to each invocation.
     */
    virtual void start_at(TimeBase::TimeT    period,
                          TimeBase::TimeT    at_time,
                          CORBA::ULong       execution_limit,
                          const CORBA::Any&  params)
        throw (CORBA::SystemException,
               CosClockService::PeriodicExecution::Controller::TimePast);

    /**
     * Pauses periodic execution.
     */ 
    virtual void pause()
        throw (CORBA::SystemException);

    /**
     * Resumes periodic execution.
     */
    virtual void resume ()
        throw (CORBA::SystemException);


    /**
     * Resumes periodic execution at a particular time.
     *
     * @param at_time time at which the execution has to
     *        be resumed.
     *
     * @throw CosClockService::PeriodicExecution::Controller::TimePast
     */
    virtual void resume_at (TimeBase::TimeT at_time)
        throw (CORBA::SystemException,
               CosClockService::PeriodicExecution::Controller::TimePast);

    /**
     * Terminates periodic execution.
     */
    virtual void terminate()
        throw (CORBA::SystemException);


    /**
     * Reports the number of executions that have already been initiated.
     */
    virtual CORBA::ULong executions()
        throw (CORBA::SystemException);
        

    /**
     * Override the update policy.
     */
    virtual void set_update_policy (::Cdmw::clock::policy::ControllerUpdateID id )
        throw (CORBA::SystemException,
               ::Cdmw::clock::policy::ControllerUpdateAdmin::Unavailable);

    /**
     * Get the update policy.
     */
    virtual  Cdmw::clock::policy::ControllerUpdateID get_update_policy ()
        throw (CORBA::SystemException);

    virtual TimeBase::TimeT get_controller_time_origin ()
        throw (CORBA::SystemException);
      
    virtual void set_controller_time_origin (TimeBase::TimeT time_origin) 
        throw (CORBA::SystemException);
      
    CORBA::ULong                execution_limit() const;
    CORBA::Any                  get_parameter() const;
    TimeBase::TimeT             get_period() const; 
    Cdmw::clock::Executor_impl* retrieve_executor();

    typedef Cdmw::clock::Status ControllerState_t;
    ControllerState_t get_status() const;
    
private:

     //////////////////////////////////////////////////////////////////////
    /**
     * This class implements an <code>ACE_Event_Handler</code>
     * and takes care of notifying the registered <code>Periodic</code>
     * instance once the period ellapses.
     */ 
    class TimeoutHandler : public ACE_Event_Handler {
    public:
      enum  {INF_EXEC_LIMIT = 0};
        
    public:
        TimeoutHandler(Controller_impl* controller,
                       CosClockService::PeriodicExecution::Periodic_ptr periodic,
                       ACE_Recursive_Thread_Mutex&);
        
        virtual ~TimeoutHandler();
        
    public:

        virtual int handle_timeout (const ACE_Time_Value&,
                                    const void* act = 0);
            
    public:
        long& executionLimit();
        const long& executionLimit() const;
        TimerAtomicCount& executions();
    public:
        inline CosClockService::PeriodicExecution::Periodic_ptr periodic() {
            return periodic_.in();
        }
    private:
        Controller_impl* controller_;
        CosClockService::PeriodicExecution::Periodic_var periodic_;
        TimerAtomicCount executions_;
        long executionsLimit_;
        ACE_Recursive_Thread_Mutex  &m_mutex;
    };

    //////////////////////////////////////////////////////////////////////

    // retrieve the correct period for a Controller with
    //  a rate != 1
    void update_period(TimeBase::TimeT period);

    // verify that offset is good and return an absolute
    //  offset good for positive and negative time controller
    TimeBase::TimeT checked_offset(TimeBase::TimeT offset) const;

    // change the offset adapting for internal timer manager
    void update_offset(TimeBase::TimeT offset);

    void cancelTimer();
    void cancelTimerLockFree();

    /**
     * This method freezes the timer without actually changing its
     * state. It also computes the remaining time to the next fire
     * time.
     */
    void freezeTimer();
    void freezeTimerLockFree();

    void scheduleTimerLockFree(TimeBase::TimeT period, 
                               TimeBase::TimeT offset); 

    // -- Disable Copy Ctor and Assignment Operator --
    Controller_impl(Controller_impl&);
    Controller_impl& operator=(const Controller_impl&);
    
    friend class TimeoutHandler;
    friend class policy::ControllerUpdate_impl;

protected:

    CosClockService::PeriodicExecution::Periodic_ptr periodic();
    
    Cdmw::clock::Executor_impl& m_clock;

    ACE_Reactor*    m_reactor;
    TimeoutHandler  m_handler;
    long            m_timer_key;
    CORBA::Any      m_any;
    
    ACE_Recursive_Thread_Mutex m_mutex;
    
    ACE_Time_Value  m_period;
    ACE_Time_Value  m_delay;

    Cdmw::clock::policy::ControllerUpdateID m_ID;

    TimeBase::TimeT m_real_period; 
    TimeBase::TimeT m_controller_time_origin;
    TimeBase::TimeT m_delta2end;
    
    ControllerState_t  m_status;
    
};

TimeBase::TimeT compute_delta2end(const IClock&          clock,
                                  const TimeBase::TimeT& period,
                                  const TimeBase::TimeT& time_origin);

} /* clock */ } /* Cdmw */

#endif /* CARDAMOM_CLOCK_CONTROLLER_IMPL_HPP */
