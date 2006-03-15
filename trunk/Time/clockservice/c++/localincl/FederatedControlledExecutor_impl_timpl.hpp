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

#ifndef _CDMW_TIME_FEDERATED_CONTROLLED_EXECUTOR_IMPL_TIMPL_HPP_
#define _CDMW_TIME_FEDERATED_CONTROLLED_EXECUTOR_IMPL_TIMPL_HPP_

#include "clockservice/FederatedControlledClockSvc.hpp"

template<typename LOCK, template <typename T> class GUARD>
Cdmw::clock::FederatedControlledExecutor_impl<LOCK, GUARD>::FederatedControlledExecutor_impl(int          federation_id,
                                                                                             ACE_Reactor* reactor,
                                                                                             bool         align_at_startup)
    : Cdmw::clock::Executor_impl(reactor),
      Cdmw::clock::ControlledExecutor_impl< ::Cdmw::Common::Void >(reactor),
      m_federation_id(federation_id),
      m_set_time_cmd(*this),
      m_set_rate_cmd(*this),
      m_pause_cmd(*this),
      m_resume_cmd(*this),
      m_terminate_cmd(*this),
      m_align_request_cmd(*this),
      m_align_response_cmd(*this),
      m_update_handler(::Cdmw::clock::FederatedControlledClockSvc::instance().get_codec()),
      m_update_manager(::Cdmw::CommonSvcs::LocalTopicUpdateManager::instance()),
      m_encoder(::Cdmw::clock::FederatedControlledClockSvc::instance().get_codec()),
      m_aligned(false)
{
    // -- Register commands with handler
    m_update_handler.register_command(::Cdmw::clock::SET_TIME,     &m_set_time_cmd);
    m_update_handler.register_command(::Cdmw::clock::SET_RATE,     &m_set_rate_cmd);
    m_update_handler.register_command(::Cdmw::clock::PAUSE,        &m_pause_cmd);
    m_update_handler.register_command(::Cdmw::clock::RESUME,       &m_resume_cmd);
    m_update_handler.register_command(::Cdmw::clock::TERMINATE,    &m_terminate_cmd);
    m_update_handler.register_command(Cdmw::clock::ALIGN_REQUEST,  &m_align_request_cmd);
    m_update_handler.register_command(Cdmw::clock::ALIGN_RESPONSE, &m_align_response_cmd);

    // Register Topic
    m_update_manager.set_topic_handler(federation_id, &m_update_handler);

    if (align_at_startup == true)
    {
        // -- Fill up the Message
        m_encoder.data().cmd = Cdmw::clock::ALIGN_REQUEST;

        // -- Send the message
        //std::cerr << "FederatedControlledExecutor_impl: sending align request" << std::endl;
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


template<typename LOCK, template <typename T> class GUARD>
Cdmw::clock::FederatedControlledExecutor_impl<LOCK, GUARD>::~FederatedControlledExecutor_impl()
    throw()
{
    // No-Op
}

template<typename LOCK, template <typename T> class GUARD>
void
Cdmw::clock::FederatedControlledExecutor_impl<LOCK, GUARD>::set(TimeBase::TimeT to)
    throw (CORBA::SystemException,
           CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex); 
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::set(to);

    if (m_aligned == true)
    {
        // -- Fill up the Message
        m_encoder.data().cmd          = Cdmw::clock::SET_TIME;
        m_encoder.data().clock_status = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::m_status;
        m_encoder.data().x0           = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::X0();
        m_encoder.data().y0           = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::Y0();
        m_encoder.data().frozen_time  = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::frozen_time();
        m_encoder.data().rate         = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::ratio();

        // -- Send the message
        m_update_manager.update_topic(m_federation_id, m_encoder, Cdmw::CommonSvcs::ACCEPT_NEWER);
    }
}

template<typename LOCK, template <typename T> class GUARD>
void
Cdmw::clock::FederatedControlledExecutor_impl<LOCK, GUARD>::set_rate(CORBA::Float ratio)
    throw (CORBA::SystemException,
           CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex); 
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::set_rate(ratio);

    if (m_aligned == true)
    {
        // -- Fill up the Message
        m_encoder.data().cmd          = Cdmw::clock::SET_RATE;
        m_encoder.data().clock_status = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::m_status;
        m_encoder.data().x0           = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::X0();
        m_encoder.data().y0           = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::Y0();
        m_encoder.data().frozen_time  = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::frozen_time();
        m_encoder.data().rate         = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::ratio();

        // -- Send the message
        m_update_manager.update_topic(m_federation_id, m_encoder, Cdmw::CommonSvcs::ACCEPT_NEWER);
    }
}


template<typename LOCK, template <typename T> class GUARD>
void
Cdmw::clock::FederatedControlledExecutor_impl<LOCK, GUARD>::pause()
    throw (CORBA::SystemException,
           CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex); 
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::pause();

    if (m_aligned == true)
    {
        // -- Fill up the Message
        m_encoder.data().cmd          = Cdmw::clock::PAUSE;
        m_encoder.data().clock_status = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::m_status;
        m_encoder.data().x0           = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::X0();
        m_encoder.data().y0           = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::Y0();
        m_encoder.data().frozen_time  = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::frozen_time();
        m_encoder.data().rate         = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::ratio();

        // -- Send the message
        m_update_manager.update_topic(m_federation_id, m_encoder, Cdmw::CommonSvcs::ACCEPT_NEWER);
    }
}


template<typename LOCK, template <typename T> class GUARD>
void
Cdmw::clock::FederatedControlledExecutor_impl<LOCK, GUARD>::resume()
    throw (CORBA::SystemException,
           CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex); 
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::resume();

    if (m_aligned == true)
    {
        // -- Fill up the Message
        m_encoder.data().cmd          = Cdmw::clock::RESUME;
        m_encoder.data().clock_status = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::m_status;
        m_encoder.data().x0           = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::X0();
        m_encoder.data().y0           = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::Y0();
        m_encoder.data().frozen_time  = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::frozen_time();
        m_encoder.data().rate         = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::ratio();

        // -- Send the message
        m_update_manager.update_topic(m_federation_id, m_encoder, Cdmw::CommonSvcs::ACCEPT_NEWER);
    }
}


template<typename LOCK, template <typename T> class GUARD>
void
Cdmw::clock::FederatedControlledExecutor_impl<LOCK, GUARD>::terminate()
    throw (CORBA::SystemException ,
           CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex); 
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::terminate();

    if (m_aligned == true)
    {
        // -- Fill up the Message
        m_encoder.data().cmd          = Cdmw::clock::TERMINATE;
        m_encoder.data().clock_status = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::m_status;

        // -- Send the message
        m_update_manager.update_topic(m_federation_id, m_encoder, Cdmw::CommonSvcs::ACCEPT_NEWER);
    }
}


template<typename LOCK, template <typename T> class GUARD>
::CosClockService::PeriodicExecution::Controller_ptr
Cdmw::clock::FederatedControlledExecutor_impl<LOCK, GUARD>::enable_periodic_execution
(CosClockService::PeriodicExecution::Periodic_ptr on)
    throw (CORBA::SystemException)
{
    Guard_t guard(m_mutex); 
    return this->ControlledExecutor_impl< ::Cdmw::Common::Void >::enable_periodic_execution(on);
}

//////////////////////////////////////////////////////////////////////////////

template<typename LOCK, template <typename T> class GUARD>
void
Cdmw::clock::FederatedControlledExecutor_impl<LOCK, GUARD>::handle_set(Cdmw::clock::Status status,
                                                                       TimeBase::TimeT     X0,
                                                                       TimeBase::TimeT     Y0,
                                                                       TimeBase::TimeT     frozen_time,
								       CORBA::Float        ratio)
    throw (CORBA::SystemException,
           CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex); 
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::m_status = status;
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::set_time_origins(X0, Y0);
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::set_frozen_time(frozen_time);
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::ratio() = ratio;
    m_aligned = true;

    // apply policy to attached controllers
    Executor_impl::PolicyFunctorManager pos(this, &Cdmw::clock::policy::ControllerUpdate::on_set);
    Executor_impl::apply_policy_to_controllers_lock_free(pos);
}

template<typename LOCK, template <typename T> class GUARD>
void
Cdmw::clock::FederatedControlledExecutor_impl<LOCK, GUARD>::handle_set_rate(Cdmw::clock::Status status,
                                                                            TimeBase::TimeT     X0,
                                                                            TimeBase::TimeT     Y0,
                                                                            TimeBase::TimeT     frozen_time,
                                                                            CORBA::Float        ratio)
    throw (CORBA::SystemException,
           CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex); 
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::m_status = status;
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::set_time_origins(X0, Y0);
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::set_frozen_time(frozen_time);
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::ratio() = ratio;
    m_aligned = true;

    // apply policy to attached controllers
    Executor_impl::PolicyFunctorManager pos(this, &Cdmw::clock::policy::ControllerUpdate::on_set_rate);
    Executor_impl::apply_policy_to_controllers_lock_free(pos);
}

template<typename LOCK, template <typename T> class GUARD>
void
Cdmw::clock::FederatedControlledExecutor_impl<LOCK, GUARD>::handle_pause(Cdmw::clock::Status status,
                                                                         TimeBase::TimeT     X0,
                                                                         TimeBase::TimeT     Y0,
                                                                         TimeBase::TimeT     frozen_time,
                                                                         CORBA::Float        ratio)
    throw (CORBA::SystemException,
           CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex); 
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::m_status = status;
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::set_time_origins(X0, Y0);
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::set_frozen_time(frozen_time);
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::ratio() = ratio;
    m_aligned = true;

    // apply policy to attached controllers
    Executor_impl::PolicyFunctorManager pos(this, &Cdmw::clock::policy::ControllerUpdate::on_pause);
    Executor_impl::apply_policy_to_controllers_lock_free(pos);
}

template<typename LOCK, template <typename T> class GUARD>
void
Cdmw::clock::FederatedControlledExecutor_impl<LOCK, GUARD>::handle_resume(Cdmw::clock::Status status,
                                                                          TimeBase::TimeT     X0,
                                                                          TimeBase::TimeT     Y0,
                                                                          TimeBase::TimeT     frozen_time,
                                                                          CORBA::Float        ratio)
    throw (CORBA::SystemException,
           CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex); 
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::m_status = status;
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::set_time_origins(X0, Y0);
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::set_frozen_time(frozen_time);
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::ratio() = ratio;
    m_aligned = true;

    // apply policy to attached controllers
    Executor_impl::PolicyFunctorManager pos(this, &Cdmw::clock::policy::ControllerUpdate::on_resume);
    Executor_impl::apply_policy_to_controllers_lock_free(pos);
}

template<typename LOCK, template <typename T> class GUARD>
void
Cdmw::clock::FederatedControlledExecutor_impl<LOCK, GUARD>::handle_terminate()
    throw (CORBA::SystemException ,
           CosClockService::ControlledClock::NotSupported)
{
    // NB: calling ControlledExecutor_impl::terminate() has the effect to
    // automatically apply the correct policy to all attached controllers.
    Guard_t guard(m_mutex);
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::terminate(); 
    m_aligned = true;
}


template<typename LOCK, template <typename T> class GUARD>
void
Cdmw::clock::FederatedControlledExecutor_impl< LOCK, GUARD>::handle_align_request()
    throw (CORBA::SystemException ,
           CosClockService::ControlledClock::NotSupported)
{
    if (m_aligned == true)
    {
        // -- Fill up the Message
        m_encoder.data().cmd          = Cdmw::clock::ALIGN_RESPONSE;
        m_encoder.data().clock_status = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::m_status;
        m_encoder.data().x0           = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::X0();
        m_encoder.data().y0           = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::Y0();
        m_encoder.data().frozen_time  = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::frozen_time();
        m_encoder.data().rate         = this->ControlledExecutor_impl< ::Cdmw::Common::Void >::ratio();

        // -- Send the message
        //std::cerr << "FederatedControlledExecutor_impl: sending align response" << std::endl;
        m_update_manager.update_topic(m_federation_id, m_encoder, Cdmw::CommonSvcs::ACCEPT_NEWER);
    }
}


template<typename LOCK, template <typename T> class GUARD>
void
Cdmw::clock::FederatedControlledExecutor_impl< LOCK, GUARD>::handle_align_response(Cdmw::clock::Status status,
                                                                                   TimeBase::TimeT     X0,
                                                                                   TimeBase::TimeT     Y0,
                                                                                   TimeBase::TimeT     frozen_time,
                                                                                   CORBA::Float        ratio)
    throw (CORBA::SystemException ,
           CosClockService::ControlledClock::NotSupported)
{
    //std::cerr << "FederatedControlledExecutor_impl: received align response" << std::endl;
    Guard_t guard(m_mutex);
    this->ControlledExecutor_impl< ::Cdmw::Common::Void >::m_status = status;
    if (this->ControlledExecutor_impl< ::Cdmw::Common::Void >::m_status != Cdmw::clock::TERMINATED)
    {
        this->ControlledExecutor_impl< ::Cdmw::Common::Void >::set_time_origins(X0, Y0);
        this->ControlledExecutor_impl< ::Cdmw::Common::Void >::set_frozen_time(frozen_time);
        this->ControlledExecutor_impl< ::Cdmw::Common::Void >::ratio() = ratio;
    }
    m_aligned = true;
}


template<typename LOCK, template <typename T> class GUARD>
void
Cdmw::clock::FederatedControlledExecutor_impl< LOCK, GUARD>::detach_terminated_controller(Cdmw::clock::Controller_impl* ctrl)
{
    Guard_t guard(m_mutex);
    Cdmw::clock::Executor_impl::detach_terminated_controller(ctrl);
}


#endif /* _CDMW_TIME_FEDERATED_CONTROLLED_EXECUTOR_IMPL_TIMPL_HPP_ */
