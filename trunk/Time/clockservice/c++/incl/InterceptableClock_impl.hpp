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

#ifndef CARDAMOM_CLOCK_INTERCEPTABLE_CLOCK_IMPL_HPP
#define CARDAMOM_CLOCK_INTERCEPTABLE_CLOCK_IMPL_HPP

#include <Time/clockservice/Clock_impl.hpp>

//////////////////////////////////////////////////////////////////////////////
namespace Cdmw {
    namespace clock {        
        template <typename Interceptor>
        class InterceptableClock_impl;
        
        template <>
        class InterceptableClock_impl<void>;
    }
}
//////////////////////////////////////////////////////////////////////////////

/**
 *  This class provides an interceptable implementation of the Local
 *  Clock. This clock is not synchronized with any external source,
 *  unless the machine clock is not kept synchronized with a master
 *  clock by using some external mean such as NTP.

  * @brief This class implement base clock service.
  * @author Angelo Corsaro <acorsaro@amsjv.it>
  */
template <typename Interceptor>
class ::Cdmw::clock::InterceptableClock_impl
    : public ::Cdmw::clock::Clock_impl
{
public:
    /**
     * Create an <code>InterceptableClock_impl</code>.
     *
     * @param interceptor the interceptor for this instance.
     */
    InterceptableClock_impl(const Interceptor& interceptor)
        : interceptor_(interceptor)
        {
            // No-Op
        }

    /**
     * Create an <code>InterceptableClock_impl</code>.
     *
     * @param interceptor the interceptor for this instance.
     * @param reactor     the reactor that will be used by this instance.
     */
    InterceptableClock_impl(const Interceptor& interceptor, ACE_Reactor* reactor)
        : InterceptableClock_impl::Clock_impl(reactor),
          interceptor_(interceptor)
        {
            // No-Op
        }

    /**
     * Create an <code>InterceptableClock_impl</code>. Default
     * interceptor and reactor instances will be used.
     *
     */
    InterceptableClock_impl() 
        {
            // No-Op
        }
public:
    /**
     * @brief The known properties of the clock.
     * @return reference to clock's properties.
     * @exception CORBA::SystemException if error occurs in setting properties
     */
    virtual ::CosPropertyService::PropertySet_ptr properties() 
        throw(CORBA::SystemException)
    {
        interceptor_.on_properties_enter();
        this->Clock_impl::properties();
        return interceptor_.on_properties_exit();
    }

    /**
     * @brief Provides a measure of the current time. The time unit is 100
     * nanosecond e.g. 10e-7 seconds.
     *
     * @return TimeT with current time
     * @exception CORBA::SystemException if error occurs in setting properties
     * @exception CosClockService::TimeUnavailable if time is not available
     *  with required security assurance.
     */
    virtual TimeBase::TimeT current_time()
        throw(CORBA::SystemException, 
              CosClockService::TimeUnavailable)
    {
        interceptor_.on_current_time_enter();
        return interceptor_.on_current_time_exit
            (this->Clock_impl::current_time());
    }


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
        throw (CORBA::SystemException)
    {
        interceptor_.on_enable_periodic_execution_enter(on);
        return interceptor_.on_enable_periodic_execution_exit
            (this->Clock_impl::enable_periodic_execution());
    }    

public:
    /**
     * This method is called by the framework to activate the clock.
     */ 
    virtual void activate() {
        interceptor_.on_activate_enter();
        interceptor_.on_activate_exit();
    }
    
protected:
    Interceptor interceptor_;
};


template <>
class ::Cdmw::clock::InterceptableClock_impl<void>
    : public ::Cdmw::clock::Clock_impl
{
public:
    InterceptableClock_impl(ACE_Reactor* reactor = ACE_Reactor::instance());
    virtual ~InterceptableClock_impl();
};

#endif  /* CARDAMOM_CLOCK_INTERCEPTABLE_CLOCK_IMPL_HPP */
