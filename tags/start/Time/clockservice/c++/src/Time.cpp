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

#include <Time/clockservice/Time.hpp>

#include <iostream>
#include <sstream>
#include <limits>
#include <Foundation/common/Assert.hpp>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Local utility function
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
std::string 
Cdmw::clock::print(CORBA::ULongLong time_in_utc)
#ifdef WIN32
{
    struct LongLongType {
        unsigned long L;
        unsigned long H;
    };
    LongLongType* pllt;

    ostringstream oss;
    pllt= (LongLongType* )&time_in_utc;
    const char * mult_factor = "4294967295";
    oss<<pllt->H<<" * ";
    oss<<mult_factor;
    oss<<" + "<<pllt->L;
    return oss.str();
}
#else
{
    ostringstream oss;
    oss<<time_in_utc;
    return oss.str();
}
#endif

///////////////////////////////////////////////////////////////////////////////

std::string 
to_seconds(CORBA::ULongLong time_in_utc)
{
    time_in_utc/=10000000;
    ostringstream oss;
    if( time_in_utc < Cdmw::clock::macro::MAX_TIME_VAL )
        oss<<(long)time_in_utc;
    else
        oss<<Cdmw::clock::print(time_in_utc);
    oss<<" s";
    return oss.str();
}


///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
