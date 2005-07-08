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

#include <Time/clockservice/UTC_impl.hpp>
#include <Time/clockservice/TimeSpan_impl.hpp>
#include <Foundation/common/Assert.hpp>

using namespace std;
using namespace CosClockService;
using namespace Cdmw::clock;

///////////////////////////////////////////////////////////////////////////////

UTC_impl::UTC_impl()
{
    // No-op
}

///////////////////////////////////////////////////////////////////////////////

TimeBase::InaccuracyT 
UTC_impl::inaccuracy()
{
    // Construct the Inaccuracy from the
    // inacchi and inacclo.
    
    TimeBase::InaccuracyT inaccuracy = inacchi();
    inaccuracy <<= 32;
    inaccuracy |= inacclo();
    return inaccuracy;
    
}

///////////////////////////////////////////////////////////////////////////////

TimeBase::UtcT  
UTC_impl::utc_time()
{
    TimeBase::UtcT  ret_val;
    ret_val.time    = time();
    ret_val.inacclo = inacclo();
    ret_val.inacchi = inacchi();
    ret_val.tdf     = tdf();  
    return ret_val;
}

///////////////////////////////////////////////////////////////////////////////


TimeComparison 
UTC_impl::compare_time(ComparisonType comparison_type,
                       CosClockService::UTC* with_utc)
{
    if (comparison_type == IntervalC)
    {
        if(with_utc->time() == time() &&
           with_utc->inaccuracy() == inaccuracy() &&
           inaccuracy()  == 0)
            return TCEqualTo;
        
        try
        {
            TimeSpan_var ret;    
            switch(UTC_impl::interval()->overlaps( with_utc->interval(),ret))
            {
            case OTContainer:
                return TCGreaterThan;
                break;
            case OTContained:
                return TCLessThan;
                break;
            case OTOverlap:
            case OTNoOverlap:
                return TCIndeterminate;
                break;
            }
        }
        catch(...)
        {
            CDMW_ASSERT(false);
        }
        
    } 
    else if (comparison_type == MidC)
    {
        if(with_utc->time() == time())
            return TCEqualTo;
        if(time() < with_utc->time())
            return TCLessThan;
        else
            return TCGreaterThan;
    }
    return TCIndeterminate;
}

///////////////////////////////////////////////////////////////////////////////


TimeSpan* 
UTC_impl::interval()
{
    TimeSpan* ret_val = new TimeSpan_impl;
    CDMW_ASSERT(ret_val);
    TimeBase::InaccuracyT inacc = UTC_impl::inaccuracy();  
    TimeBase::TimeT tmp_time = time();
    ret_val->lower_bound( tmp_time - inacc);
    ret_val->upper_bound( tmp_time + inacc);    
    return ret_val;
}

///////////////////////////////////////////////////////////////////////////////

UTC_impl_init::UTC_impl_init()
{
    // No-op
}

///////////////////////////////////////////////////////////////////////////////

UTC* 
UTC_impl_init::init(const TimeBase::UtcT & from) 
    throw (CORBA::SystemException)
{
    CosClockService::UTC* 
        ret_val =  new  UTC_impl;
    CDMW_ASSERT( ret_val );
    ret_val->time(from.time);
    ret_val->inacclo(from.inacclo);
    ret_val->inacchi(from.inacchi);
    ret_val->tdf(from.tdf);
    return ret_val;
}

///////////////////////////////////////////////////////////////////////////////


UTC* UTC_impl_init::compose (TimeBase::TimeT time,
                            CORBA::ULong inacclo,
                            CORBA::UShort inacchi,
                            TimeBase::TdfT tdf ) 
                            throw ( CORBA::SystemException) 
{
    CosClockService::UTC* 
        ret_val = new  UTC_impl;
    CDMW_ASSERT( ret_val );
    ret_val->time(time);
    ret_val->inacclo(inacclo);
    ret_val->inacchi(inacchi);
    ret_val->tdf(tdf);
    
    return ret_val;
}

///////////////////////////////////////////////////////////////////////////////


CORBA::ValueBase* 
UTC_impl_init::create_for_unmarshal()
{
    CosClockService::UTC* 
        ret_val = new  UTC_impl;
    CDMW_ASSERT( ret_val );
    return ret_val;
}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
