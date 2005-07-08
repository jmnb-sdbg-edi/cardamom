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

#ifndef CARDAMOM_CLOCK_CONTROLLER_IMPL_HPP
#define CARDAMOM_CLOCK_CONTROLLER_IMPL_HPP

#include <Time/clockservice/CdmwTime.skel.hpp>
#include <Time/clockservice/config.hpp>
#include <Time/clockservice/IClock.hpp>

#include "ace/Reactor.h"
#include "ace/Synch.h"
#include "ace/Event_Handler.h"

namespace Cdmw { namespace clock {

/**
 * @brief This class implements the basic Controller functionalities
 * described in the EVoT specification.  
 *
 * Allows control of periodic 
 * execution after the appropriate object has been registered with 
 * the clock.
 *
 * @author Angelo Corsaro <acorsaro@amsjv.it>
 * @author Fabrizio Morciano<fmorciano@amsjv.it>
 */
class CLOCK_API  Controller_impl :
    public virtual POA_CosClockService::PeriodicExecution::Controller
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
    Controller_impl(Cdmw::clock::IClock& clock,
                    CosClockService::PeriodicExecution::Periodic_ptr periodic,
                    ACE_Reactor* reactor = ACE_Reactor::instance());
    
    /**
     * Releases resources associated with the instance.
     */
   virtual ~Controller_impl();

public:
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
     * <code>at_time parameter</code> specifies an absolute 
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
        
private:

     //////////////////////////////////////////////////////////////////////
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
                       ACE_Mutex& mutex);
        
        virtual ~TimeoutHandler();
        
    public:

        virtual int handle_close(ACE_HANDLE,  
                                 ACE_Reactor_Mask);

        virtual int handle_timeout (const ACE_Time_Value&,
                                    const void* act = 0);
            
    public:
        CORBA::ULong& executionLimit();
        CORBA::ULong& executions();
    public:
        inline CosClockService::PeriodicExecution::Periodic_ptr periodic() {
            return periodic_.in();
        }
    private:
        Controller_impl* controller_;
        CosClockService::PeriodicExecution::Periodic_var periodic_;
        CORBA::ULong executions_;
        CORBA::ULong executionsLimit_;
        ACE_Mutex&    mutex_;
    };

    //////////////////////////////////////////////////////////////////////

    // retrieve the correct period for a Controller with
    //  a rate != 1
    void update_period(TimeBase::TimeT period);

    // verify that offset is good and return an absolute
    //  offset good for positive and negative time controller
    TimeBase::TimeT checked_offset(TimeBase::TimeT offset);

    // change the offset adapting for internal timer manager
    void update_offset(TimeBase::TimeT offset);

    //////////////////////////////////////////////////////////////////////
protected:
    CosClockService::PeriodicExecution::Periodic_ptr periodic();
        
private:
    void cancelTimer();
    void cancelTimerLockFree();

private:
    // -- Disable Copy Ctor and Assignment Operator --
    Controller_impl(Controller_impl&);
    Controller_impl& operator=(const Controller_impl&);
    
protected:
    Cdmw::clock::IClock& clock_;
    
    ACE_Reactor*    reactor_;
    TimeoutHandler  handler_;
    long            timerKey_;
    CORBA::Any      any_;

    ACE_Mutex       mutex_;
    
    ACE_Time_Value  period_;
    ACE_Time_Value  delay_;
    
    friend  class TimeoutHandler;
};

} /* clock */ } /* Cdmw */

#endif /* CARDAMOM_CLOCK_CONTROLLER_IMPL_HPP */
