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

#include "LocalClock.h"
#include "ClockDef_.inl"

#include <sys/time.h>
#include <iostream>

/**
 *@file LocalClock.cc
 *@brief some useful function for calculate the current time in gregorian format.
 *@version 0.1 - 26/11/04
 *@author Raffaele Mele <rmele@progesi.it>
 */

#ifndef  WIN32

ctools::localclock::TimeT 
ctools::localclock::compute_current_time()
{
    timeval timeofday_;
    if( gettimeofday(&timeofday_,0 ) < 0 )
    {
        std::cerr << "ERR : Unable to read time!";
        exit(-1);
    }
    return static_cast<ctools::localclock::TimeT>(ctools::localclock::base_time_ 
        + static_cast<ctools::localclock::TimeT>(timeofday_.tv_sec ) * 10000000
        + static_cast<ctools::localclock::TimeT>(timeofday_.tv_usec) * 10 );
}

#else

ctools::localclock::TimeT 
ctools::localclock::compute_current_time()
{
    ctools::localclock::TimeT time;
    ctools::localclock::TimeT local_time;
    
    // return value since 1,Jan,1601 
    GetSystemTimeAsFileTime( (FILETIME*)&time );
    if( FileTimeToLocalFileTime( 
        (FILETIME*)&time, 
        (FILETIME*)&local_time ) == 0 )
    {
        std::cerr << "ERR : Unable to read time!";
        exit(-1);
    }
    
    return ctools::localclock::base_time_ + local_time;
}

#endif
