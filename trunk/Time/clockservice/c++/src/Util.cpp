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

#include "Time/clockservice/Time.hpp"
#include "Time/clockservice/Util.hpp"
#include "Time/clockservice/Macro.hpp"
#include "clockservice/Clock_impl.hpp"
#include "clockservice/ControlledClock_impl.hpp"

#include <Foundation/osthreads/Mutex.hpp>
#include <Foundation/common/Assert.hpp>

#include <sstream>

#include <math.h>

#ifndef  WIN32
#   include <unistd.h>
#endif

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Return the current gregorian time
TimeBase::TimeT
Cdmw::clock::compute_current_time()
{
    TimeBase::TimeT out;
    Cdmw::OsSupport::OS::Timeval  tv;

    try
    {    
        tv=Cdmw::OsSupport::OS::get_time();        
        Cdmw::clock::util::CdmwTimeval2TimeT(tv, out);
    }
    catch(...)
    {
        out=(TimeBase::TimeT)(-1);
    }
    return out;
}

#if 0
#ifndef  WIN32

TimeBase::TimeT
Cdmw::clock::compute_current_time()
{
    TimeBase::TimeT out;
    struct      timeval  tv;
  //struct timezone tz;
    
    if (gettimeofday(&tv, 0) < 0)
        throw CosClockService::TimeUnavailable();
        
    if (Cdmw::clock::util::timeval2TimeT(tv, out) != 0)
        throw CosClockService::TimeUnavailable();    

/*
        // this value is minutes for West
    TimeBase::TimeT
        timezone_offset = abs( tz.tz_minuteswest );
        timezone_offset*=600000000;
        
        if(tz.tz_minuteswest>0)
        out-=timezone_offset; // West
    else
        out+=timezone_offset; // East
*/
        return out;
}

#else

TimeBase::TimeT 
Cdmw::clock::compute_current_time()
{
    TimeBase::TimeT 
        time;
    
    TimeBase::TimeT 
        local_time;
    
    // return value since 1,Jan,1601 
    GetSystemTimeAsFileTime( (FILETIME*)&time );
    if( FileTimeToLocalFileTime( 
        (FILETIME*)&time, 
        (FILETIME*)&local_time ) == 0 )
        throw CosClockService::TimeUnavailable();
    
    return TIME_BASE + local_time;
}

#endif
#endif
///////////////////////////////////////////////////////////////////////////////

int 
compute_time_displacement_factor()
{
    return -1;
}

///////////////////////////////////////////////////////////////////////////////

int 
compute_daylight()
{
    return -1;
}

///////////////////////////////////////////////////////////////////////////////

double 
Cdmw::clock::util::convert(TimeBase::TimeT value)
{
#if defined(WIN32) && (_MSC_VER < 1300)
    return (unsigned long)(value & clock::macro::Mask32);
#else
    return (double)value;
#endif
}

///////////////////////////////////////////////////////////////////////////////

double  
Cdmw::clock::util::mult(TimeBase::TimeT val1,
                        double val2)
{
    double out = 0;
    out = convert(val1) * val2;
    return out;
}
///////////////////////////////////////////////////////////////////////////////

double  
Cdmw::clock::util::add(TimeBase::TimeT val1, 
                       double val2)
{
    double out = 0;
    out = convert(val1)+val2;
    return out;
}

///////////////////////////////////////////////////////////////////////////////


double  
Cdmw::clock::util::divide(TimeBase::TimeT val1,
                    TimeBase::TimeT val2)
{
    double num = convert(val1);
    double den = convert(val2);
    return num/den;
}

///////////////////////////////////////////////////////////////////////////////

TimeBase::TimeT 
Cdmw::clock::util::seconds2TimeT(double seconds) 
throw(std::range_error)
{
    if( seconds < 0 || seconds > 1.8e11 )
        throw   std::range_error("Error: negative seconds!");

    TimeBase::TimeT
        out = (TimeBase::TimeT)(roundl(seconds * 1.0e7));
    return  out;
}


///////////////////////////////////////////////////////////////////////////////

double 
Cdmw::clock::util::TimeT2seconds(TimeBase::TimeT utc) 
{
    double  
        out = Cdmw::clock::util::convert(utc);
    out*=1.0e-7;
    return out;  
}

///////////////////////////////////////////////////////////////////////////////

TimeBase::TimeT
Cdmw::clock::util::real2controlled (CosClockService::ControlledClock_ptr clock_ptr, 
                                    TimeBase::TimeT x) 
    throw (CosClockService::TimeUnavailable)
{
    // assuming that time equation is:
    //
    //    y - y0 = rate * ( x - x0 )
    //
    
    Cdmw::clock::ControlledClockEx_ptr 
        clock_val = Cdmw::clock::ControlledClockEx::_narrow(clock_ptr);
    CDMW_ASSERT(clock_val);
    if( !clock_val )
        throw CosClockService::TimeUnavailable();

    TimeBase::TimeT 
        x0, y0;

    clock_val->get_time_origins (x0, 
                                 y0);
    
    if (x0 > x) 
        throw CosClockService::TimeUnavailable();

    CORBA::Float 
        rate = clock_val->get_rate(),
        abs_rate = fabs(rate);

    TimeBase::TimeT 
        delta = x - x0;

    TimeBase::TimeT 
        out = 0;
        
    if(rate>=0.0)
        out = (TimeBase::TimeT) ( abs_rate * delta) + y0;
    else
    {
        out = (TimeBase::TimeT) ( abs_rate * delta);
        if( out > y0 )
            throw CosClockService::TimeUnavailable();        
        out = y0 - out;
    }
    return out;
}

///////////////////////////////////////////////////////////////////////////////

TimeBase::TimeT
Cdmw::clock::util::controlled2real(CosClockService::ControlledClock_ptr clock_ptr, 
                                   TimeBase::TimeT y) 
    throw (CosClockService::TimeUnavailable)
{
    // assuming that time equation is:
    //
    //    y - y0 = rate * ( x - x0 )
    //
    
    Cdmw::clock::ControlledClockEx_ptr 
        clock_val = Cdmw::clock::ControlledClockEx::_narrow(clock_ptr);
    CDMW_ASSERT(clock_val);
    if( !clock_val )
        throw CosClockService::TimeUnavailable();

    TimeBase::TimeT 
        x0, y0;

    clock_val->get_time_origins (x0, 
                                 y0);

    CORBA::Float 
        rate = clock_val->get_rate();
    
    if (rate == 0.0) 
        throw CosClockService::TimeUnavailable();

    TimeBase::TimeT 
        delta = 0;
        
    if (rate >= 0.0)
    {
      if ( y < y0 ) 
        throw CosClockService::TimeUnavailable();
      else
        delta = y - y0;
    }
    else
    {
      if ( y > y0 ) 
        throw CosClockService::TimeUnavailable();
      else
        delta = y0 - y;
    }

    TimeBase::TimeT 
        out = (TimeBase::TimeT) (delta / rate) + x0;

    return out;
}

///////////////////////////////////////////////////////////////////////////////

std::string
Cdmw::clock::util::TimeT2string(const TimeBase::TimeT& tmp )
{
    cdmw_tm tm;
    Cdmw::clock::util::TimeT2cdmw_tm(tmp,tm);
    char buffer[100];
    Cdmw::clock::util::asctime_r(tm,buffer);
    return std::string(buffer);
}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
