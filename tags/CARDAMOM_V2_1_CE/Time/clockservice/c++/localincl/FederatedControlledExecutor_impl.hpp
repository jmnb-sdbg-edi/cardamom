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

#ifndef _CDMW_TIME_FEDERATED_CONTROLLED_EXECUTOR_IMPL_HPP_
#define _CDMW_TIME_FEDERATED_CONTROLLED_EXECUTOR_IMPL_HPP_

#include <Foundation/common/Config.hpp>

#include "clockservice/ClockUpdateCommands.hpp"
#include "clockservice/FederatedClockUpdateHandler.hpp"
#include "clockservice/FederatedControlledClockDataEncoder.hpp"
#include "Foundation/commonsvcs/federation/LocalTopicUpdateManager.hpp"
#include <clockservice/ControlledExecutor_impl.hpp>
#include <clockservice/ControlledClockCommandHandler.hpp>

namespace Cdmw { namespace clock {

/**
 * @brief This class allows to federate controlled executors so that their
 * behaviour will keep consistent regardless of the clock location
 * (i.e. distribution across multiple hosts). 
 * 
 * The different clock are
 * federated in a declarative manner, by simply creating them with the
 * same logical clock id.<br>
 *
 * @note The consistency model provided by the current
 * implementation is eventual consistency.
 *
 * @author <a href="mailto:corsaro@cse.wustl.edu">Angelo Corsaro</a>
 * @version 1.0 
 * 
 */    
CDMW_TEMPLATE_EXPORT
template <typename LOCK,
          template <typename T> class GUARD = ::Cdmw::OsSupport::Guard>
class FederatedControlledExecutor_impl
    :   public virtual Cdmw::clock::ControlledExecutor_impl< ::Cdmw::Common::Void >
    ,   protected virtual Cdmw::clock::ControlledClockCommandHandler
{
public:
    typedef LOCK        Lock_t; 
    typedef GUARD<LOCK> Guard_t;

public:
    FederatedControlledExecutor_impl(int federation_id, ACE_Reactor* reactor, bool align_at_startup = true);
    virtual ~FederatedControlledExecutor_impl() throw();

public:    
    // -- Methods from ControlledClock Interface --

    /**
     * Set the time for this clock. Extend functionality resetting controller.
     *
     * @param to the time to which the clock will be set.
     */
    virtual void set(TimeBase::TimeT to)
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);
        
    /**
     * Set the rate for this clock. Extend functionality resetting controller.
     *
     * @param ration the rate of clock will be set.
     */
    virtual void set_rate (CORBA::Float ratio)
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);

    /**
     * Pause the clock, i.e., the clock stops advancing.
     *
     * @exception CORBA::BAD_INV_ORDER, if the clock is already paused.
     */
    virtual void pause ()
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);

    /**
     * Resume the clock, i.e., the clock restarts measuring the time
     * that elapses.
     *
     * @exception CORBA::BAD_INV_ORDER, if the clock is not paused.
     */ 
    virtual void resume ()
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);

    /**
     * Stop the clock.
     */
    virtual void terminate ()
        throw (CORBA::SystemException ,
               CosClockService::ControlledClock::NotSupported);

public:    
    // -- Methods from the Executor Interface --
    
    /**
     * Register an instance of the Periodic interface for periodic execution.
     * 
     * @return the <code>Controller</code> to be used to control 
     *         the execution of the <code>Periodic</code> instance.
     */
    virtual ::CosClockService::PeriodicExecution::Controller_ptr
    enable_periodic_execution(CosClockService::PeriodicExecution::Periodic_ptr on)
        throw (CORBA::SystemException);    

public:
    virtual void detach_terminated_controller(Cdmw::clock::Controller_impl* ctrl);

private:
    FederatedControlledExecutor_impl(const FederatedControlledExecutor_impl&);
    FederatedControlledExecutor_impl& operator=(const FederatedControlledExecutor_impl&);

protected:
    /**
     * These operations are used by the framework to update the
     * Controlled Clock status without having the change distributed
     * to the other member of the federation. The purpose of these
     * methods is that of establishing a private protocol between the
     * FederatedControlledClock_impl and the command class that dispatch
     * the updates.
     */ 
    virtual void handle_set(Cdmw::clock::Status status,
                            TimeBase::TimeT     X0,
                            TimeBase::TimeT     Y0,
                            TimeBase::TimeT     frozen_time,
			    CORBA::Float        ratio)
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);
    
    virtual void handle_set_rate(Cdmw::clock::Status status,
                                 TimeBase::TimeT     X0,
                                 TimeBase::TimeT     Y0,
                                 TimeBase::TimeT     frozen_time,
                                 CORBA::Float        ratio)
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);
    
    virtual void handle_pause(Cdmw::clock::Status status,
                              TimeBase::TimeT     X0,
			      TimeBase::TimeT     Y0,
			      TimeBase::TimeT     frozen_time,
			      CORBA::Float        ratio)
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);
    
    virtual void handle_resume(Cdmw::clock::Status status,
                               TimeBase::TimeT     X0,
                               TimeBase::TimeT     Y0,
                               TimeBase::TimeT     frozen_time,
                               CORBA::Float        ratio)
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);
    
    virtual void handle_terminate()
        throw (CORBA::SystemException ,
               CosClockService::ControlledClock::NotSupported);

    virtual void handle_align_request()
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);

    virtual void handle_align_response(Cdmw::clock::Status status,
                                       TimeBase::TimeT     X0,
                                       TimeBase::TimeT     Y0,
                                       TimeBase::TimeT     frozen_time,
                                       CORBA::Float        ratio)
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);

private:
    // -- let commands class access the update methods
    friend class Cdmw::clock::SetTimeCommand;
    friend class Cdmw::clock::SetRateCommand;
    friend class Cdmw::clock::PauseCommand;
    friend class Cdmw::clock::ResumeCommand;
    friend class Cdmw::clock::TerminateCommand;
    friend class Cdmw::clock::AlignRequestCommand;
    friend class Cdmw::clock::AlignResponseCommand;

private:
    int                                 m_federation_id; 
    ::Cdmw::clock::SetTimeCommand       m_set_time_cmd;  
    ::Cdmw::clock::SetRateCommand       m_set_rate_cmd;  
    ::Cdmw::clock::PauseCommand         m_pause_cmd;    
    ::Cdmw::clock::ResumeCommand        m_resume_cmd;   
    ::Cdmw::clock::TerminateCommand     m_terminate_cmd;
    ::Cdmw::clock::AlignRequestCommand  m_align_request_cmd;
    ::Cdmw::clock::AlignResponseCommand m_align_response_cmd;
    
    ::Cdmw::clock::ControlledClockDataTopicUpdateHandler_t  m_update_handler;
    ::Cdmw::CommonSvcs::LocalTopicUpdateManager&            m_update_manager;
    ::Cdmw::clock::FederatedControlledClockDataEncoder      m_encoder;
    Lock_t                                                  m_mutex; 
    bool                                                    m_aligned;
};

} /* clock */ } /* Cdmw */

#if (CDMW_USES_TEMPLATE_INCLUSION_MODEL == 1)
#   include <clockservice/FederatedControlledExecutor_impl_timpl.hpp>
#endif /* CDMW_USES_TEMPLATE_INCLUSION_MODEL == 1 */

#endif /* _CDMW_TIME_FEDERATED_CONTROLLED_EXECUTOR_IMPL_HPP_ */
