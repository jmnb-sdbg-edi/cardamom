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

#include <Time/clockservice/Util.hpp>
#include <Time/clockservice/Macro.hpp>

#ifndef  WIN32
#   include <unistd.h>
#endif

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


void
Cdmw::clock::util::sleep(unsigned int seconds)
{
#ifndef WIN32
    ::sleep(seconds);
#else
    Sleep(seconds*1000);
#endif
}


///////////////////////////////////////////////////////////////////////////////

TimeBase::TimeT 
Cdmw::clock::util::seconds2TimeT(double seconds) 
throw(std::range_error)
{
    TimeBase::TimeT out = 0;
    if(seconds < 0)
        throw   std::range_error("Error: negative seconds!");

    long to_TimeT = (long)seconds;
    out = to_TimeT * 10000000;

    // remove integer part
    seconds-=(double)to_TimeT;

    // milli seconds
    seconds*=1000;
    to_TimeT = (long)(seconds);

    if(to_TimeT)
    {
        out += to_TimeT*10000;

        // remove integer part
        seconds-=(double)to_TimeT;

        // micro seconds
        seconds*=1000;
        to_TimeT = (long)(seconds);

        if(to_TimeT)
            out += to_TimeT*10;
    }

    return out;    
}


///////////////////////////////////////////////////////////////////////////////

double 
Cdmw::clock::util::TimeT2seconds(TimeBase::TimeT utc) 
throw  (std::range_error)
{
  return 0.0;  
}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
