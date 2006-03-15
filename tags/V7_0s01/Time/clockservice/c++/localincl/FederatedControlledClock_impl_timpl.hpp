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

#ifndef _CDMW_TIME_FEDERATED_CONTROLLED_CLOCK_IMPL_TIMPL_HPP_
#define _CDMW_TIME_FEDERATED_CONTROLLED_CLOCK_IMPL_TIMPL_HPP_



template <template <typename L, template <typename> class G > class CTRL_CLOCK,
          typename LOCK,
          template <typename T> class GUARD >
Cdmw::clock::FederatedControlledClock_impl< CTRL_CLOCK,LOCK,GUARD>::FederatedControlledClock_impl(int federation_id, bool align_at_startup)
    throw()
    : m_federation_id(federation_id),
      m_set_time_cmd(*this),
      m_set_rate_cmd(*this),
      m_pause_cmd(*this),
      m_resume_cmd(*this),
      m_terminate_cmd(*this),
      m_align_request_cmd(*this),
      m_align_response_cmd(*this),
      m_update_handler(
      Cdmw::clock::FederatedControlledClockSvc::instance().get_codec()),
      m_update_manager(Cdmw::CommonSvcs::LocalTopicUpdateManager::instance()),
      m_encoder(Cdmw::clock::FederatedControlledClockSvc::instance().get_codec()),
      m_aligned(false)
{
    // -- Register commands with handler
    m_update_handler.register_command(Cdmw::clock::SET_TIME,       &m_set_time_cmd);
    m_update_handler.register_command(Cdmw::clock::SET_RATE,       &m_set_rate_cmd);
    m_update_handler.register_command(Cdmw::clock::PAUSE,          &m_pause_cmd);
    m_update_handler.register_command(Cdmw::clock::RESUME,         &m_resume_cmd);
    m_update_handler.register_command(Cdmw::clock::TERMINATE,      &m_terminate_cmd);
    m_update_handler.register_command(Cdmw::clock::ALIGN_REQUEST,  &m_align_request_cmd);
    m_update_handler.register_command(Cdmw::clock::ALIGN_RESPONSE, &m_align_response_cmd);

    // Register Topic
    m_update_manager.set_topic_handler(federation_id, &m_update_handler);

    if (align_at_startup == true)
    {
        // -- Fill up the Message
        m_encoder.data().cmd = Cdmw::clock::ALIGN_REQUEST;

        // -- Send the message
        //std::cerr << "FederatedControlledClock_impl: sending align request" << std::endl;
        m_update_manager.update_topic(m_federation_id, m_encoder, Cdmw::CommonSvcs::ACCEPT_ALL);

        // -- If it's not the 1-st in the federation, it must align before any dispatching
        // -- of controls, (set, set_rate, pause, resume, terminate), acting upon it;
        // -- so wait for possible align responses
        Cdmw::OsSupport::OS::sleep(1, 0);

        // -- It it's the 1-st in the federation, it must consider itself aligned
        // -- even if it will not receive align responses!
        m_aligned = true;
    }
} 

template <template <typename L, template <typename> class G > class CTRL_CLOCK,
          typename LOCK,
          template <typename T> class GUARD >
Cdmw::clock::FederatedControlledClock_impl<CTRL_CLOCK,LOCK,GUARD>::~FederatedControlledClock_impl()  throw()
{
    // No-Op
}

template <template <typename L, template <typename> class G > class CTRL_CLOCK,
          typename LOCK,
          template <typename T> class GUARD >
void Cdmw::clock::FederatedControlledClock_impl<CTRL_CLOCK,LOCK,GUARD>::set(TimeBase::TimeT to)
    throw (CORBA::SystemException, CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex); 
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::set(to);

    if (m_aligned == true)
    {
        // -- Fill up the Message
        m_encoder.data().cmd          = Cdmw::clock::SET_TIME;
        m_encoder.data().clock_status = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::m_status;
        m_encoder.data().x0           = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::X0();
        m_encoder.data().y0           = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::Y0();
        m_encoder.data().frozen_time  = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::frozen_time();
        m_encoder.data().rate         = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::ratio();

        // -- Send the message
        m_update_manager.update_topic(m_federation_id, m_encoder, Cdmw::CommonSvcs::ACCEPT_NEWER);
    }
}

template <template <typename L, template <typename> class G > class CTRL_CLOCK,
          typename LOCK,
          template <typename T> class GUARD >
void Cdmw::clock::FederatedControlledClock_impl<CTRL_CLOCK,LOCK,GUARD>::set_rate(CORBA::Float ratio)
    throw (CORBA::SystemException, CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex);
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::set_rate(ratio);

    if (m_aligned == true)
    {
        // -- Fill up the message
        m_encoder.data().cmd          = Cdmw::clock::SET_RATE; 
        m_encoder.data().clock_status = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::m_status;
        m_encoder.data().x0           = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::X0();
        m_encoder.data().y0           = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::Y0();
        m_encoder.data().frozen_time  = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::frozen_time();
        m_encoder.data().rate         = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::ratio();

        // -- Send the Message 
        m_update_manager.update_topic(m_federation_id, m_encoder, Cdmw::CommonSvcs::ACCEPT_NEWER);
    }
}

template <template <typename L, template <typename> class G > class CTRL_CLOCK,
          typename LOCK,
          template <typename T> class GUARD >
void Cdmw::clock::FederatedControlledClock_impl<CTRL_CLOCK,LOCK,GUARD>::pause()
    throw (CORBA::SystemException, CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex);
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::pause();

    if (m_aligned == true)
    {
        // -- Fill-up the Message
        m_encoder.data().cmd          = Cdmw::clock::PAUSE;
        m_encoder.data().clock_status = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::m_status;
        m_encoder.data().x0           = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::X0();
        m_encoder.data().y0           = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::Y0();
        m_encoder.data().frozen_time  = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::frozen_time();
        m_encoder.data().rate         = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::ratio();
 
        // -- Send the Message
        m_update_manager.update_topic(m_federation_id, m_encoder, Cdmw::CommonSvcs::ACCEPT_NEWER);
    }
}


template <template <typename L, template <typename> class G > class CTRL_CLOCK,
          typename LOCK,
          template <typename T> class GUARD >
void Cdmw::clock::FederatedControlledClock_impl<CTRL_CLOCK,LOCK,GUARD>::resume()
    throw (CORBA::SystemException, CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex); 
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::resume();

    if (m_aligned == true)
    {
        // -- Fill up the Message
        m_encoder.data().cmd          = Cdmw::clock::RESUME;
        m_encoder.data().clock_status = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::m_status;
        m_encoder.data().x0           = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::X0();
        m_encoder.data().y0           = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::Y0();
        m_encoder.data().frozen_time  = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::frozen_time();
        m_encoder.data().rate         = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::ratio();

        // -- Send the Message
        m_update_manager.update_topic(m_federation_id, m_encoder, Cdmw::CommonSvcs::ACCEPT_NEWER);
    }
}

template <template <typename L, template <typename> class G > class CTRL_CLOCK,
          typename LOCK,
          template <typename T> class GUARD >
void Cdmw::clock::FederatedControlledClock_impl<CTRL_CLOCK,LOCK,GUARD>::terminate()
    throw (CORBA::SystemException, CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex);
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::terminate();

    if (m_aligned == true)
    {
        // -- Fill up the Message
        m_encoder.data().cmd          = Cdmw::clock::TERMINATE;
        m_encoder.data().clock_status = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::m_status;

        // -- Send the Message
        m_update_manager.update_topic(m_federation_id, m_encoder, Cdmw::CommonSvcs::ACCEPT_NEWER);
    }
}

template <template <typename L, template <typename> class G > class CTRL_CLOCK,
          typename LOCK,
          template <typename T> class GUARD >
void Cdmw::clock::FederatedControlledClock_impl<CTRL_CLOCK,LOCK,GUARD>::handle_set(Cdmw::clock::Status status,
                                                                                   TimeBase::TimeT     X0,
                                                                                   TimeBase::TimeT     Y0,
                                                                                   TimeBase::TimeT     frozen_time,
                                                                                   CORBA::Float        ratio)
    throw (CORBA::SystemException, CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex);
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::m_status = status;
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::set_time_origins(X0, Y0);
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::set_frozen_time(frozen_time);
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::ratio() = ratio;
    m_aligned = true;
}

template <template <typename L, template <typename> class G > class CTRL_CLOCK,
          typename LOCK,
          template <typename T> class GUARD >
void Cdmw::clock::FederatedControlledClock_impl<CTRL_CLOCK,LOCK,GUARD>::handle_set_rate(Cdmw::clock::Status status,
                                                                                        TimeBase::TimeT     X0,
                                                                                        TimeBase::TimeT     Y0,
                                                                                        TimeBase::TimeT     frozen_time,
                                                                                        CORBA::Float        ratio)
    throw (CORBA::SystemException, CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex);
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::m_status = status;
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::set_time_origins(X0, Y0);
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::set_frozen_time(frozen_time);
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::ratio() = ratio;
    m_aligned = true;
}

template <template <typename L, template <typename> class G > class CTRL_CLOCK,
          typename LOCK,
          template <typename T> class GUARD >
void Cdmw::clock::FederatedControlledClock_impl<CTRL_CLOCK,LOCK,GUARD>::handle_pause(Cdmw::clock::Status status,
                                                                                     TimeBase::TimeT     X0,
                                                                                     TimeBase::TimeT     Y0,
                                                                                     TimeBase::TimeT     frozen_time,
                                                                                     CORBA::Float        ratio)
    throw (CORBA::SystemException, CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex);
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::m_status = status;
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::set_time_origins(X0, Y0);
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::set_frozen_time(frozen_time);
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::ratio() = ratio;
    m_aligned = true;
}

template <template <typename L, template <typename> class G > class CTRL_CLOCK,
          typename LOCK,
          template <typename T> class GUARD >
void Cdmw::clock::FederatedControlledClock_impl<CTRL_CLOCK,LOCK,GUARD>::handle_resume(Cdmw::clock::Status status,
                                                                                      TimeBase::TimeT     X0,
                                                                                      TimeBase::TimeT     Y0,
                                                                                      TimeBase::TimeT     frozen_time,
                                                                                      CORBA::Float        ratio)
    throw (CORBA::SystemException, CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex);
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::m_status = status;
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::set_time_origins(X0, Y0);
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::set_frozen_time(frozen_time);
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::ratio() = ratio;
    m_aligned = true;
}


template <template <typename L, template <typename> class G > class CTRL_CLOCK,
          typename LOCK,
          template <typename T> class GUARD >
void Cdmw::clock::FederatedControlledClock_impl<CTRL_CLOCK,LOCK,GUARD>::handle_terminate()
    throw (CORBA::SystemException, CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex);
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::terminate();
    m_aligned = true;
}

template <template <typename L, template <typename> class G > class CTRL_CLOCK,
          typename LOCK,
          template <typename T> class GUARD >
void Cdmw::clock::FederatedControlledClock_impl<CTRL_CLOCK,LOCK,GUARD>::handle_align_request()
    throw (CORBA::SystemException, CosClockService::ControlledClock::NotSupported)
{
    if (m_aligned == true)
    {
        // -- Fill up the message
        m_encoder.data().cmd          = Cdmw::clock::ALIGN_RESPONSE;
        m_encoder.data().clock_status = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::m_status;
        m_encoder.data().x0           = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::X0();
        m_encoder.data().y0           = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::Y0();
        m_encoder.data().frozen_time  = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::frozen_time();
        m_encoder.data().rate         = this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::ratio();

        // -- Send the Message
        //std::cerr << "FederatedControlledClock_impl: sending align response" << std::endl;
        m_update_manager.update_topic(m_federation_id, m_encoder, Cdmw::CommonSvcs::ACCEPT_NEWER);
    }
}

template <template <typename L, template <typename> class G > class CTRL_CLOCK,
          typename LOCK,
          template <typename T> class GUARD >
void Cdmw::clock::FederatedControlledClock_impl<CTRL_CLOCK,LOCK,GUARD>::handle_align_response(Cdmw::clock::Status status,
                                                                                              TimeBase::TimeT     X0,
                                                                                              TimeBase::TimeT     Y0,
                                                                                              TimeBase::TimeT     frozen_time,
                                                                                              CORBA::Float        ratio)
    throw (CORBA::SystemException, CosClockService::ControlledClock::NotSupported)
{
    //std::cerr << "FederatedControlledClock_impl: received align response" << std::endl;
    Guard_t guard(m_mutex);
    this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::m_status = status;
    if (this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::m_status != Cdmw::clock::TERMINATED)
    {
        this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::set_time_origins(X0, Y0);
        this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::set_frozen_time(frozen_time);
        this->CTRL_CLOCK< Cdmw::Common::Void, Cdmw::OsSupport::Guard >::ratio() = ratio;
    }
    m_aligned = true;
}


#endif // _CDMW_TIME_FEDERATED_CONTROLLED_CLOCK_IMPL_TIMPL_HPP_
 
