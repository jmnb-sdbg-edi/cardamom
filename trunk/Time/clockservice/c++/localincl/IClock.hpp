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

#ifndef CARDAMOM_CLOCK_ICLOCK_HPP
#define CARDAMOM_CLOCK_ICLOCK_HPP

#include <Time/clockservice/config.hpp>
#include <Time/clockservice/CosClockService.skel.hpp>
#include "clockservice/ClockStatus.hpp"

namespace Cdmw { namespace clock {
    class Controller_impl;
    
/**
 * @brief This interface provide the basic service through which the time can
 * be accessed.
 * @todo Split IClock in IClock, IControlledClock, IControlledExecutor.
 *
 * @author Angelo Corsaro <acorsaro@selex-si.com
 * @author Fabrizio Morciano <fmorciano@selex-si.com>
 * @author Marco Macellari <mmacellari@progesi.it>
 */
class CLOCK_API  IClock
{
public:

    typedef Cdmw::clock::Status ControlledClockState_t;

    IClock();
    virtual ~IClock();
    
    /**
     * @brief Return the current time, as measured by this clock. 
     * 
     * This method is guaranteed to be thread safe.
     */
    virtual ::TimeBase::TimeT current_time() = 0;

    /**
     * @brief Retrieve rate of used clock.
     */
    virtual CORBA::Float retrieve_rate() const;

    /**
     * @brief Change rate of used clock.
     */
    void set_rate(CORBA::Float);

    /**
     * @brief Return the state of clock.
     * 
     * @note  System clock have just a single state RUNNING,
     *        only controlledclock can have other states.
     */
    ControlledClockState_t get_state() const;
    
protected:
    CORBA::Float            m_rate;
    ControlledClockState_t  m_status;
};
    
}  /* clock */ } /* Cdmw */
        
#endif  /*  CARDAMOM_CLOCK_ICLOCK_HPP */
