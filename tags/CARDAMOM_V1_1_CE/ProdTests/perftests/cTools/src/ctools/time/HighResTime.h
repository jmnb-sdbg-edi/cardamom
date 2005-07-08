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
                                                                                                 
#ifndef __CTOOLS_TIME_HIGHRESTIME__
#define __CTOOLS_TIME_HIGHRESTIME__

#include "ctools/time/SysInfo.h"
#include <iostream>

namespace ctools { namespace time {
    
class HighResTime {
public:
    typedef unsigned long long time_t;
    
public:
    /**
     * Creates a <code>HighResTime</code> and initializes it with the
     * time represented by the clockTicks times the machine clock
     * period. 
     *
     * @param clockTicks the number of hardware clock tics that have
     * to be used in order to initialize the timer.
     */ 
    explicit HighResTime(time_t clockTicks = 0);

    /**
     * Creates a HighResTime object and initializes it with a given
     * time expressed in milliseconds and nanoseconds.
     *
     * @param msec the millisecond component of the time.
     * @param nsec the nanosecond compomnent of the time.
     */
    HighResTime(time_t msec, time_t nsec);
    
    /**
     * Creates a HighResTime object and initializes it with a given
     * time object;
     */
    HighResTime(const HighResTime& time);
    
    ~HighResTime();

public:
    time_t millisec() const;
    void   millisec(time_t msec);

    time_t nanosec() const;
    void nanosec(time_t nsec);
    
    void set(time_t msec, time_t nsec);
    void set(time_t clockTicks); 
    void reset();
    
    long double toMillisec() const;
    long double toMicrosec() const;
    long double toNanosec() const;

    HighResTime operator+(const HighResTime& rhs);
    HighResTime operator-(const HighResTime& rhs);
    const HighResTime& operator=(const HighResTime& rhs);
    
private:
    time_t msec_;
    time_t nsec_;
};
        
} /* ctools */ } /* time */

std::ostream&
operator<<(std::ostream& os, const  ctools::time::HighResTime& time);

#endif /* __CTOOLS_TIME_HIGHRESTIME__ */
