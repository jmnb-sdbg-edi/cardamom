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

#ifndef CARDAMOM_CLOCK_FT_CONTROLLER_IMPL_HPP
#define CARDAMOM_CLOCK_FT_CONTROLLER_IMPL_HPP

#include <Time/clockservice/Controller_impl.hpp>
#include <Time/clockservice/ClockStateDataStore.hpp>

namespace Cdmw {
    namespace clock {
        class FTController_impl;
    }
}

/**
 * This class implements the fault-tolerant Controller
 * functionalities. To this end it adds fault-tolerance capabilities
 * to the <code>Controller</code> interface described in the EVoT
 * specification. Allows control of periodic execution after the
 * appropriate object has been registered with the clock.
 *
 * @author Angelo Corsaro <acorsaro@amsjv.it>
 */
class Cdmw::clock::FTController_impl :
    public virtual Cdmw::clock::Controller_impl
{
public:
    /**
     * Creates a <code>FTController_impl</code> with an associated 
     * <code>Periodic</code> and <code>Reactor</code>
     *
     * @param controllerId Identifyer of internal controller.
     *
     * @param clock The clock used by this controller to measure time.
     *
     * @param periodic The <code>Periodic</code> instance that will
     *         be notified when the programmed time interval expires.
     *
     * @param controllerDataStorageHome information to manage state transfert.
     *
     * @param controllerDataStore information to manage state transfert.
     *
     * @param reactor The reactor that will be used to register timeout 
     *         handlers.
     */
    FTController_impl(Cdmw::clock::Ident controllerId,
                      Cdmw::clock::IClock& clock,
                      CosClockService::PeriodicExecution::Periodic_ptr periodic, 
                      Cdmw::clock::ControllerDataStorageHome& controllerDataStorageHome,
                      Cdmw::clock::ControllerDataStore& controllerDataStore, 
                      ACE_Reactor* reactor = ACE_Reactor::instance());
    
    /**
     * Releases resources associated with the instance.
     */
   virtual ~FTController_impl();

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

#if 0    
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
#endif 
protected:
    Cdmw::clock::Ident m_id;
    
    Cdmw::clock::ControllerDataStorageHome&  m_controllerDataStorageHome;
    Cdmw::clock::ControllerDataStore&        m_controllerDataStore; 
    Cdmw::clock::ControllerData              m_statusData;

};


#endif /* CARDAMOM_CLOCK_FT_CONTROLLER_IMPL_HPP */
