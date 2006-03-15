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

#ifndef CARDAMOM_CLOCK_UTIL_HPP
#define CARDAMOM_CLOCK_UTIL_HPP

/**
 * @author Fabrizio Morciano <fmorciano@selex-si.com>
 */

#include "Time/clockservice/config.hpp"
#include "Time/clockservice/CdmwTime.skel.hpp"
#include "Foundation/ossupport/OS.hpp"

#include <stdexcept>
#include <string>

namespace Cdmw { namespace clock {

    /**
     * @brief Auxililary function to compute current machine time.
     */
    CLOCK_API TimeBase::TimeT 
    compute_current_time();

    /**
     * @brief Auxililary function to retrieve offset 
     * from GMT (+ if East,- if West).
     */
    CLOCK_API int 
    compute_time_displacement_factor();

    /**
     * 
     */
    CLOCK_API int 
    compute_daylight();
    
/**
 * @namespace Cdmw::clock::util
 * @brief Namespace that contains a lot of useful functions to work with
 * TimeBase::TimeT.
 * 
 * The namespace @code util @endcode maintains some functions that are 
 * not strictly related with CORBA Enhanced View of Time implementation.
 */
namespace util {

    /**
     * @brief Retrieve the "virtual" time related to a Controlled clock.
     * @param theControlledClock The reference ControlledClock.
     * @param when Time to convert.
     * @return The time in ControlledClock base.
     * @exception CosClockService::TimeUnavailable This exception can be raised
     *            if operation is unvailable (time in the past or in the future).
     */
    CLOCK_API  TimeBase::TimeT
    real2controlled (CosClockService::ControlledClock_ptr theControlledClock, 
                     TimeBase::TimeT when) 
        throw (CosClockService::TimeUnavailable);

    /**
     * @brief Retrieve the actual time related to a Controlled clock.
     * @param theControlledClock The reference ControlledClock.
     * @param when Time to convert.
     * @return The time in GMT base.
     * @exception CosClockService::TimeUnavailable This exception can be raised
     *            if operation is unvailable (time in the past or in the future).
     */
    CLOCK_API  TimeBase::TimeT
    controlled2real(CosClockService::ControlledClock_ptr, 
                    TimeBase::TimeT) 
        throw (CosClockService::TimeUnavailable);

    /**
     * @brief mult a TimeT with a double and return a double.
     */
    CLOCK_API   double  
    mult(TimeBase::TimeT, 
         double );

    /**
     * @brief add a TimeT with a double and return a double.
     */
    CLOCK_API double  
    add(TimeBase::TimeT, 
        double );

    /**
     * @brief divide a TimeT with a double and return a double.
     */
    CLOCK_API   double  
    divide(TimeBase::TimeT,
           TimeBase::TimeT );

    /**
     * @brief conversion TimeT vs double.
     */
    CLOCK_API   double  
    convert(TimeBase::TimeT);

    /** 
     * @brief Conversion from seconds to TimetT.
     * @exception range_error The exception is thrown if seconds are negatives.
     */
    CLOCK_API   TimeBase::TimeT 
    seconds2TimeT(double seconds) 
        throw  (std::range_error);

    /** 
     * @brief Conversion from TimetT to seconds.
     * @param val The value in TimeT.
     */
    CLOCK_API   double 
    TimeT2seconds(TimeBase::TimeT val);


    CLOCK_API   std::string
    TimeT2string(const TimeBase::TimeT& time2convert);

} /* util */ } /* clock */ } /* Cdmw */

#endif /* CARDAMOM_CLOCK_UTIL_HPP */
