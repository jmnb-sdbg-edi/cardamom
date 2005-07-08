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

#ifndef CARDAMOM_CLOCK_UTIL_HPP
#define CARDAMOM_CLOCK_UTIL_HPP

/**
 * @file 
 * @brief
 * 
 * @author Fabrizio Morciano <fmorciano@amsjv.it>
 */

#include <Time/clockservice/config.hpp>
#include <Time/clockservice/CdmwTime.skel.hpp>

#include <stdexcept>

namespace Cdmw { namespace clock { namespace util {

    CLOCK_API 
    double  mult(TimeBase::TimeT, 
                 double );

    CLOCK_API 
    double  add(TimeBase::TimeT, 
                double );

    CLOCK_API 
    double  divide(TimeBase::TimeT,
                   TimeBase::TimeT );

    CLOCK_API 
    double  convert(TimeBase::TimeT);

    CLOCK_API 
    void    sleep(unsigned int seconds);

    /*  
     *
     */
    CLOCK_API 
    TimeBase::TimeT seconds2TimeT(double seconds) 
        throw  (std::range_error);

    /*  
     *
     */
    CLOCK_API 
    double TimeT2seconds(TimeBase::TimeT utc) 
        throw  (std::range_error);

} /* util */ } /* clock */ } /* Cdmw */

#endif /* CARDAMOM_CLOCK_UTIL_HPP */
