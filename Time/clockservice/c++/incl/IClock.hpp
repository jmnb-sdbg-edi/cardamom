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

#ifndef CARDAMOM_CLOCK_ICLOCK_HPP
#define CARDAMOM_CLOCK_ICLOCK_HPP

#include <Time/clockservice/config.hpp>
#include <Time/clockservice/CosClockService.skel.hpp>

namespace Cdmw { namespace clock {
    
/**
 * @brief This interface provide the basic service through which the time can
 * be accessed.
 *
 * @author Angelo Corsaro <acorsaro@amsjv.it>
 */
class CLOCK_API  IClock
{
public:
    virtual ~IClock();
    
public:
    /**
     * @brief Return the current time, as measured by this clock. 
     * 
     * This method is guaranteed to be thread safe.
     */
    virtual ::TimeBase::TimeT 
        current_time() = 0;

    /**
     * @brief Retrieve rate of used clock.
     */
    virtual double  
        retrieve_rate() const;

    /**
     * @brief   
     * Convert virtual time in real time. Occur to manage virtual clock.
     *
     * @param to the time to which the clock will be set.
     */
    virtual ::TimeBase::TimeT 
        virtual_to_real(::TimeBase::TimeT time_val) const;
};
    
}  /* clock */ } /* Cdmw */
        
#endif  /*  CARDAMOM_CLOCK_ICLOCK_HPP */
