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

#include "clockservice/ControllerUpdateAdmin_impl.hpp"
#include "clockservice/ControllerUpdate_impl.hpp"
#include <algorithm>
#include <functional>

///////////////////////////////////////////////////////////////////////////////

template <typename LOCK, template <typename T> class GUARD >
Cdmw::clock::ControlledExecutor_impl<LOCK, GUARD>::ControlledExecutor_impl(ACE_Reactor* reactor)
    : Cdmw::clock::Executor_impl(reactor),
      m_ID(Cdmw::clock::policy::CANCEL_ALL)
{
}

///////////////////////////////////////////////////////////////////////////////

template <typename LOCK, template <typename T> class GUARD>
Cdmw::clock::ControlledExecutor_impl<LOCK, GUARD>::~ControlledExecutor_impl ()
    throw()
{
}

///////////////////////////////////////////////////////////////////////////////

template <typename LOCK, template <typename T> class GUARD>
CosClockService::PeriodicExecution::Controller_ptr
Cdmw::clock::ControlledExecutor_impl<LOCK, GUARD>::enable_periodic_execution
(CosClockService::PeriodicExecution::Periodic_ptr on)
    throw (CORBA::SystemException)
{
    Guard_t guard(m_mutex);
    CDMW_ASSERT(!CORBA::is_nil(on));
    Cdmw::clock::Controller_impl* ctrl = 
        new Cdmw::clock::Controller_impl(*this, on, Executor_impl::get_reactor_lock_free());
    Executor_impl::attach_controller_lock_free(ctrl);

    /* ----- DEBUG CODE [BEGIN] ----- *
    std::cerr << std::endl << "Attached controller: " << ctrl << std::endl;
    Executor_impl::print_attached_controllers_lock_free();
     * ----- DEBUG CODE [ END ] ----- */

    return ctrl->_this();
}

///////////////////////////////////////////////////////////////////////////////

template <typename LOCK, template <typename T> class GUARD>
void 
Cdmw::clock::ControlledExecutor_impl<LOCK, GUARD>::set(TimeBase::TimeT to)
    throw (CORBA::SystemException,
           CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex);
    this->ControlledClock_impl< ::Cdmw::Common::Void >::set(to);
    Executor_impl::PolicyFunctorManager pos(this, &Cdmw::clock::policy::ControllerUpdate::on_set);
    Executor_impl::apply_policy_to_controllers_lock_free(pos);
}

///////////////////////////////////////////////////////////////////////////////

template <typename LOCK, template <typename T> class GUARD>
void 
Cdmw::clock::ControlledExecutor_impl<LOCK, GUARD>::set_rate (CORBA::Float ratio)
    throw (CORBA::SystemException,
           CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex);
    this->ControlledClock_impl< ::Cdmw::Common::Void >::set_rate(ratio);
    Executor_impl::PolicyFunctorManager pos(this, &Cdmw::clock::policy::ControllerUpdate::on_set_rate);
    Executor_impl::apply_policy_to_controllers_lock_free(pos);
}

///////////////////////////////////////////////////////////////////////////////
template <typename LOCK, template <typename T> class GUARD>
void 
Cdmw::clock::ControlledExecutor_impl<LOCK, GUARD>::pause ()
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex);
    this->ControlledClock_impl< ::Cdmw::Common::Void >::pause();
    Executor_impl::PolicyFunctorManager pos(this, &Cdmw::clock::policy::ControllerUpdate::on_pause);
    Executor_impl::apply_policy_to_controllers_lock_free(pos);
}

///////////////////////////////////////////////////////////////////////////////

template <typename LOCK, template <typename T> class GUARD>
void 
Cdmw::clock::ControlledExecutor_impl<LOCK, GUARD>::resume ()
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex);
    this->ControlledClock_impl< ::Cdmw::Common::Void >::resume();
    Executor_impl::PolicyFunctorManager pos(this, &Cdmw::clock::policy::ControllerUpdate::on_resume);
    Executor_impl::apply_policy_to_controllers_lock_free(pos);
}

///////////////////////////////////////////////////////////////////////////////

template <typename LOCK, template <typename T> class GUARD>
void Cdmw::clock::ControlledExecutor_impl<LOCK, GUARD>::terminate ()
        throw (CORBA::SystemException ,
               CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard(m_mutex);
    this->ControlledClock_impl< ::Cdmw::Common::Void >::terminate();
    Executor_impl::PolicyFunctorManager pos(this, &Cdmw::clock::policy::ControllerUpdate::on_terminate);
    Executor_impl::apply_policy_to_controllers_lock_free(pos);
    assert(Executor_impl::num_of_attached_controllers_lock_free() == 0);
}

///////////////////////////////////////////////////////////////////////////////
template <typename LOCK,
          template <typename T> class GUARD>
void 
Cdmw::clock::ControlledExecutor_impl<LOCK, GUARD>::set_time_origins(::TimeBase::TimeT X0,
                                                                    ::TimeBase::TimeT Y0)
    throw()
{
    Guard_t guard(m_mutex); 
    this->ControlledClock_impl< ::Cdmw::Common::Void >::set_time_origins(X0, Y0);
}

///////////////////////////////////////////////////////////////////////////////

template <typename LOCK,
          template <typename T> class GUARD>
CosClockService::PeriodicExecution::Controller_ptr 
Cdmw::clock::ControlledExecutor_impl<LOCK, GUARD>::enable_periodic_execution_with_policy (
	  CosClockService::PeriodicExecution::Periodic_ptr on,
      Cdmw::clock::policy::ControllerUpdateID id)
    throw (CORBA::SystemException,
           Cdmw::clock::policy::ControllerUpdateAdmin::Unavailable)
{
    Guard_t guard(m_mutex);

    Cdmw::clock::Controller_impl* ctrl = 
        new Cdmw::clock::Controller_impl(*this, on, Executor_impl::get_reactor_lock_free());
    ctrl->set_update_policy(id); // can raise an Unavailable exception
    Executor_impl::attach_controller_lock_free(ctrl);

    /* ----- DEBUG CODE [BEGIN] ----- *
    std::cerr << std::endl << "Attached controller: " << ctrl << std::endl;
    Executor_impl::print_attached_controllers_lock_free();
     * ----- DEBUG CODE [ END ] ----- */

    return ctrl->_this();               
}

////////////////////////////////////////////////////////////////////////////////

template <typename LOCK,
          template <typename T> class GUARD>
void 
Cdmw::clock::ControlledExecutor_impl<LOCK, GUARD>::set_controller_update_policy (
    Cdmw::clock::policy::ControllerUpdateID id)
    throw (CORBA::SystemException,
           Cdmw::clock::policy::ControllerUpdateAdmin::Unavailable)
{   
    Guard_t guard(m_mutex);

    this->m_ID = id;
    // if is not ok, exception is raised
    Cdmw::clock::policy::ControllerUpdateAdmin_impl::instance()->get_policy(this->m_ID); 
}
  
////////////////////////////////////////////////////////////////////////////////

template <typename LOCK,
          template <typename T> class GUARD>
Cdmw::clock::policy::ControllerUpdateID 
Cdmw::clock::ControlledExecutor_impl<LOCK, GUARD>::get_controller_update_policy ()
    throw (CORBA::SystemException)
{
        return this->m_ID;
}

////////////////////////////////////////////////////////////////////////////////

template < typename LOCK,  
           template < typename T >  class GUARD>
TimeBase::TimeT
Cdmw::clock::ControlledExecutor_impl < LOCK, GUARD >::current_time ()
throw (CORBA::SystemException,
       CosClockService::TimeUnavailable)
{
   return Cdmw::clock::ControlledClock_impl< ::Cdmw::Common::Void >::current_time();    
}

////////////////////////////////////////////////////////////////////////////////

template < typename LOCK,
           template < typename T >  class GUARD>
void
Cdmw::clock::ControlledExecutor_impl < LOCK, GUARD >::detach_terminated_controller(Cdmw::clock::Controller_impl* ctrl)
{
    Guard_t guard(m_mutex);
    Cdmw::clock::Executor_impl::detach_terminated_controller(ctrl);
}

////////////////////////////////////////////////////////////////////////////////
//  End Of File
////////////////////////////////////////////////////////////////////////////////
