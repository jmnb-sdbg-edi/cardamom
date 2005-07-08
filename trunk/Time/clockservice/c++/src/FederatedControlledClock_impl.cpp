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

#include "Foundation/osthreads/MutexGuard.hpp"

#include "Time/clockservice/FederatedControlledClock_impl.hpp"
#include "Time/clockservice/ControlledClock_impl.hpp"
#include "Time/clockservice/FederatedControlledClockSvc.hpp"

Cdmw::clock::FederatedControlledClock_impl::FederatedControlledClock_impl
(int federation_id)
    throw()
    
    : m_federation_id(federation_id),
      m_set_time_cmd(*this),
      m_set_rate_cmd(*this),
      m_pause_cmd(*this),
      m_resume_cmd(*this),
      m_terminate_cmd(*this),
      m_update_handler(
          ::Cdmw::clock::FederatedControlledClockSvc::instance().get_codec()),
      m_update_manager(::Cdmw::CommonSvcs::LocalTopicUpdateManager::instance()),
      m_encoder(::Cdmw::clock::FederatedControlledClockSvc::instance().get_codec())
{
    // -- Register commands with handler
    m_update_handler.register_command(static_cast<int>(::Cdmw::clock::SET_TIME),
                                      &m_set_time_cmd);
    
    m_update_handler.register_command(::Cdmw::clock::SET_RATE,
                                      &m_set_rate_cmd);
    
    m_update_handler.register_command(::Cdmw::clock::PAUSE,
                                      &m_pause_cmd);
    
    m_update_handler.register_command(::Cdmw::clock::RESUME,
                                      &m_resume_cmd);
    
    m_update_handler.register_command(::Cdmw::clock::TERMINATE,
                                      &m_terminate_cmd);

    // Register Topic
    m_update_manager.set_topic_handler(federation_id,
                                       &m_update_handler);
}

Cdmw::clock::FederatedControlledClock_impl::~FederatedControlledClock_impl()
    throw()
{
    // No-Op
}

void
Cdmw::clock::FederatedControlledClock_impl::set(TimeBase::TimeT to)
    throw (CORBA::SystemException,
           CosClockService::ControlledClock::NotSupported)
{
    ::Cdmw::OsSupport::MutexGuard guard(m_mutex); 
    this->ControlledClock_impl::set(to);
    m_encoder.data().cmd = ::Cdmw::clock::SET_TIME;
    m_encoder.data().X0 = this->ControlledClock_impl::X0();
    m_encoder.data().Y0 = this->ControlledClock_impl::Y0();
    m_encoder.data().frozen_time =
        this->ControlledClock_impl::frozen_time();
    m_update_manager.update_topic(m_federation_id, m_encoder);
}

void
Cdmw::clock::FederatedControlledClock_impl::set_rate(CORBA::Float ratio)
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported)
{
    ::Cdmw::OsSupport::MutexGuard guard(m_mutex); 
    this->ControlledClock_impl::set_rate(ratio);
    m_encoder.data().cmd = ::Cdmw::clock::SET_RATE;
    m_encoder.data().rate = this->ControlledClock_impl::ratio();
    m_update_manager.update_topic(m_federation_id, m_encoder); 
}

void
Cdmw::clock::FederatedControlledClock_impl::pause()
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported)

{
    ::Cdmw::OsSupport::MutexGuard guard(m_mutex); 
    this->ControlledClock_impl::pause();
    m_encoder.data().cmd = ::Cdmw::clock::PAUSE;
    m_encoder.data().frozen_time = this->ControlledClock_impl::frozen_time();
    m_update_manager.update_topic(m_federation_id, m_encoder); 
}

void
Cdmw::clock::FederatedControlledClock_impl::resume()
    throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported)
{
    ::Cdmw::OsSupport::MutexGuard guard(m_mutex); 
    this->ControlledClock_impl::resume();
    m_encoder.data().cmd = ::Cdmw::clock::RESUME;
    m_update_manager.update_topic(m_federation_id, m_encoder); 
}

void
Cdmw::clock::FederatedControlledClock_impl::terminate()
    throw (CORBA::SystemException ,
           CosClockService::ControlledClock::NotSupported)
{
    ::Cdmw::OsSupport::MutexGuard guard(m_mutex);
    this->ControlledClock_impl::terminate();
    m_encoder.data().cmd = ::Cdmw::clock::TERMINATE;
    m_update_manager.update_topic(m_federation_id, m_encoder); 
}

void
Cdmw::clock::FederatedControlledClock_impl::set_(TimeBase::TimeT X0,
                                                 TimeBase::TimeT Y0,
                                                 TimeBase::TimeT frozen_time)
    throw (CORBA::SystemException,
           CosClockService::ControlledClock::NotSupported)
{
    ::Cdmw::OsSupport::MutexGuard guard(m_mutex); 
    this->ControlledClock_impl::set_time_origins(X0, Y0);
    this->ControlledClock_impl::set_frozen_time(frozen_time);
}

void
Cdmw::clock::FederatedControlledClock_impl::set_rate_(CORBA::Float ratio)
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported)
{
    this->ControlledClock_impl::set_rate(ratio);
}

void
Cdmw::clock::FederatedControlledClock_impl::pause_(::TimeBase::TimeT frozen_time)
    throw (CORBA::SystemException,
           CosClockService::ControlledClock::NotSupported)
    
{
    ::Cdmw::OsSupport::MutexGuard guard(m_mutex); 
    this->ControlledClock_impl::pause();
    // Set the frozen time to the the one of the originating clock.
    this->ControlledClock_impl::set_frozen_time(frozen_time);

}

void
Cdmw::clock::FederatedControlledClock_impl::resume_()
    throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported)
{
    this->ControlledClock_impl::resume();
}

void
Cdmw::clock::FederatedControlledClock_impl::terminate_()
    throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported)
{
    this->ControlledClock_impl::terminate();
}
