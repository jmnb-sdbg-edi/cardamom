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

#include "ClockTime.hpp"

clockTime_t ClockHighResTime::getTime()
{
    return ctools::time::HighResClock::getTime().toMicrosec();
}

clockTime_t ClockCpuTime::getTime()
{
    static clockid_t clk_id = CLOCK_THREAD_CPUTIME_ID;
    static struct timespec tp;

    clock_gettime(clk_id, &tp);

    // return Thread-specific CPU-time clock in usec.
    return static_cast<clockTime_t >(tp.tv_nsec/1000);
}

clockTime_t ClockCpuTick::getTime()
{
    return ctools::time::HighResClock::getClockTickCount();
}
