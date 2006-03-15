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

#ifndef CARDAMOM_CLOCK_FT_CLOCK_IMPL_HPP
#define CARDAMOM_CLOCK_FT_CLOCK_IMPL_HPP

// -- Cdmw Time Includes --
#include <Time/clockservice/ClockStateDataStore.hpp>
#include "clockservice/Clock_impl.hpp"
#include "clockservice/Executor_impl.hpp"

extern "C++" {
    namespace Cdmw {
        namespace clock {
            class FTClockActivationHandler;
        }
    }
}

namespace Cdmw {
    namespace clock {
        class FTClock_impl;
    }
}

            
/**
 * @brief This class provides a fault-tolerant implementation of the
 * <code>Clock</code> and <code>Executor</code> interface.
 *
 * @author Angelo Corsaro <acorsaro@selex-si.com>
 */
class Cdmw::clock::FTClock_impl
    : public Cdmw::clock::Executor_impl	
{
public:
    /**
     * @brief Create an <code>FTClock_impl</code>.
     *
     * @param orb the orb
     * @param reactor     the reactor that will be used by this instance.
     */
    FTClock_impl(CORBA::ORB_ptr orb,
                 ACE_Reactor* reactor = ACE_Reactor::instance());

    virtual ~FTClock_impl();
    
public:
    // -- Methods from the Executor Interface --
    
    /**
     * @brief Register an instance of the Periodic interface for periodic execution.
     * 
     * @return the <code>Controller</code> to be used to control 
     *         the execution of the <code>Periodic</code> instance.
     */
    virtual ::CosClockService::PeriodicExecution::Controller_ptr
    enable_periodic_execution(CosClockService::PeriodicExecution::Periodic_ptr on)
        throw (CORBA::SystemException);
    
protected:
    /**
     * @brief This method is called by the framework to activate the clock.
     */ 
    virtual void activate();
    friend class ::Cdmw::clock::FTClockActivationHandler;
    
private:
    FTClock_impl(const FTClock_impl&);
    FTClock_impl& operator=(const FTClock_impl&);
    
protected:
    Cdmw::clock::ControllerDataStorageHome*  m_controllerDataStorageHome; 
    Cdmw::clock::ControllerDataStore*        m_controllerDataStore;
    
    Cdmw::clock::Ident m_controllerIds;
};

#endif /* CARDAMOM_CLOCK_FT_CLOCK_IMPL_HPP */
