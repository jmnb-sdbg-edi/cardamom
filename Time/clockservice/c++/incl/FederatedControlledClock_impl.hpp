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

#ifndef _CDMW_TIME_FEDERATED_CONTROLLED_CLOCK_IMPL_HPP_
#define _CDMW_TIME_FEDERATED_CONTROLLED_CLOCK_IMPL_HPP_

#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/commonsvcs/LocalTopicUpdateManager.hpp"

#include "Time/clockservice/ControlledClock_impl.hpp"
#include "Time/clockservice/FederatedControlledClockDataEncoder.hpp"
#include "Time/clockservice/FederatedClockUpdateHandler.hpp"
#include "Time/clockservice/ClockUpdateCommands.hpp"


namespace Cdmw {
    namespace clock {
        class FederatedControlledClock_impl;
    }
}

/**
 * This class allows to federate controlled clocks so that their
 * behaviour will keep consistent regardless of the clock location
 * (i.e. distribution across multiple hosts). The different clock are
 * federated in a declarative manner, by simply creating them with the
 * same logical clock id.
 *
 * @author <a href="mailto:corsaro@cse.wustl.edu">Angelo Corsaro</a>
 * @version 1.0 
 * 
 */
class Cdmw::clock::FederatedControlledClock_impl
    : public virtual Cdmw::clock::ControlledClock_impl
{
public:
    /**
     * Creates a <code>FederatedControlledClock_impl</code> instance.
     *
     * @param federation_id the id representing the federation to
     * which the clock belongs.
     */
    FederatedControlledClock_impl(int federation_id) throw();

    /**
     * Finalize a <code>FederatedControlledClock_impl</code>.
     */
    virtual ~FederatedControlledClock_impl() throw();

public:
    /**
     * Set  the  time for  this  clock. The  change  of  time will  be
     * trasparently distributed to all the member of the federation.
     *
     * @param to the time to which the clock will be set.
     */
    virtual void set(TimeBase::TimeT to)
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);

    /**
     * Set the rate at which the clock measures time. The rate is
     * expressed as a ratio of the controlled clock speed relative to
     * the real-time clock speed. The change of rate will be
     * trasparently distributed to all the member of the federation.
     *
     * @param ratio the ratio for this clock. 
     */
    virtual void set_rate(CORBA::Float ratio)
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);

    
    /**
     * Pause the clock, e.g., the clock stops advancing. The change of
     * status will be trasparently distributed to all the member of
     * the federation.
     *
     * @exception CORBA::BAD_INV_ORDER, if the clock is already paused.
     */
    virtual void pause()
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);

    /**
     * Resume the clock, e.g., the clock restarts measuring the time
     * that elapses. The change of status will be trasparently
     * distributed to all the member of the federation.
     *
     * @exception CORBA::BAD_INV_ORDER, if the clock is not paused.
     */ 
    virtual void resume()
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);

    /**
     * Stop the clock.
     */
    virtual void terminate()
        throw (CORBA::SystemException ,
               CosClockService::ControlledClock::NotSupported);

private:
    // Forbid Copy Ctor and Assignment Operator.
    FederatedControlledClock_impl(const FederatedControlledClock_impl&);
    FederatedControlledClock_impl& operator=(const FederatedControlledClock_impl&);
    
protected:
    /**
     * These operations are used by the framework to update the
     * Controlled Clock status without having the change distributed
     * to the other member of the federation. The purpose of these
     * methods is that of establishing a private protocol between the
     * FederatedControlledClock_impl and the command class that dispatch
     * the updates.
     */ 
    void set_(TimeBase::TimeT X0,
              TimeBase::TimeT Y0,
              TimeBase::TimeT frozen_time)
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);
    
    void set_rate_(CORBA::Float ratio)
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);

    void pause_(::TimeBase::TimeT frozen_time)
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);
    
    void resume_()
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);

    void terminate_()
        throw (CORBA::SystemException ,
               CosClockService::ControlledClock::NotSupported);

    // -- let commands class access the update methods
    friend class Cdmw::clock::SetTimeCommand;
    friend class Cdmw::clock::SetRateCommand;
    friend class Cdmw::clock::PauseCommand;
    friend class Cdmw::clock::ResumeCommand;
    friend class Cdmw::clock::TerminateCommand;
    
private:
    int                              m_federation_id; 
    ::Cdmw::clock::SetTimeCommand    m_set_time_cmd;  
    ::Cdmw::clock::SetRateCommand    m_set_rate_cmd;  
    ::Cdmw::clock::PauseCommand      m_pause_cmd;    
    ::Cdmw::clock::ResumeCommand     m_resume_cmd;   
    ::Cdmw::clock::TerminateCommand  m_terminate_cmd;

    ::Cdmw::clock::ControlledClockDataTopicUpdateHandler_t  m_update_handler;
    ::Cdmw::CommonSvcs::LocalTopicUpdateManager&            m_update_manager;
    ::Cdmw::clock::FederatedControlledClockDataEncoder      m_encoder;
    ::Cdmw::OsSupport::Mutex                                m_mutex;
};

#endif /* _CDMW_TIME_FEDERATED_CONTROLLED_CLOCK_IMPL_HPP_ */
