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


#include "clockservice/TimeSpan_impl.hpp"
#include <Time/clockservice/CosClockService.stub.hpp>
#include "clockservice/UTC_impl.hpp"
#include <Time/clockservice/Macro.hpp>

#include "ace/CORBA_macros.h"
#include <algorithm>
#include <functional>
#include <Foundation/common/Assert.hpp>

using namespace CosClockService;
using namespace Cdmw::clock;

using Cdmw::clock::macro::Mask16;
using Cdmw::clock::macro::Mask32;
using Cdmw::clock::macro::Mask48;



///////////////////////////////////////////////////////////////////////////////

static  OverlapType 
overlap_impl(CosClockService::TimeSpan *A,
             CosClockService::TimeSpan *B,
             CosClockService::TimeSpan_out& overlap)
{
    CDMW_ASSERT( overlap.ptr()==0 );
    overlap = new TimeSpan_impl;
    // return the gap
    if( A->lower_bound() > B->upper_bound() )
    {
        overlap->lower_bound( B->upper_bound() );
        overlap->upper_bound( A->lower_bound() );
        return OTNoOverlap;
    }
    
    if( A->upper_bound() < B->lower_bound() )
    {
        overlap->lower_bound( A->upper_bound() );
        overlap->upper_bound( B->lower_bound() );
        return OTNoOverlap;
    }
    
    if( A->lower_bound() <= B->lower_bound() )
    {    
        overlap->lower_bound( B->lower_bound() );
        if( B->upper_bound() <= A->upper_bound() )
        {
            overlap->upper_bound( B->upper_bound() );
            return OTContainer;
        }
    }
    else // then A_lower is greater than B_lower
    {
        overlap->lower_bound( A->lower_bound() );
        if( A->upper_bound() <= B->upper_bound() )
        {
            overlap->upper_bound( A->upper_bound() );
            return OTContained;
        }
    }
    
    CDMW_ASSERT(std::max(A->lower_bound(), B->lower_bound()) == 
           overlap->lower_bound() );
    
    TimeBase::TimeT min_val = 
        std::min(A->upper_bound(), B->upper_bound());
    
    // then A_upper is greater then B_upper
    overlap->upper_bound(min_val);
    return OTOverlap;
}

///////////////////////////////////////////////////////////////////////////////

TimeSpan_impl::TimeSpan_impl()
{
    //  No-op
}

///////////////////////////////////////////////////////////////////////////////

TimeBase::IntervalT 
TimeSpan_impl::time_interval()
{
  TimeBase::IntervalT ret_val;
  ret_val.lower_bound = this->lower_bound();
  ret_val.upper_bound = this->upper_bound();  
  return ret_val;
}

///////////////////////////////////////////////////////////////////////////////

OverlapType 
TimeSpan_impl::spans(CosClockService::UTC * time,
                     CosClockService::TimeSpan_out overlap) 
{
  // just to rember to add code for check time parameter validity
  //    but when is it not valid?????
  bool is_valid = true;
  if(!is_valid)
    throw CORBA::BAD_PARAM();

  return overlap_impl(this, 
                      time->interval(), 
                      overlap );
}

///////////////////////////////////////////////////////////////////////////////

OverlapType 
TimeSpan_impl::overlaps(CosClockService::TimeSpan *other, 
                        CosClockService::TimeSpan_out overlap)
{
  // check for validity
  bool is_valid = true;
  if( !is_valid )
    throw CORBA::BAD_PARAM();

  return overlap_impl(this, 
                      other, 
                      overlap );
}

///////////////////////////////////////////////////////////////////////////////

UTC* 
TimeSpan_impl::time()
{
    TimeBase::TimeT time_ = 
        (this->upper_bound() + 
         this->lower_bound())/2L;
    
    TimeBase::TimeT inaccuracy_ = 
        time_ - this->lower_bound();
    
    TimeBase::TimeT res = inaccuracy_ & Mask48;
    res&=Mask32;
    
    unsigned long inacclo_ = static_cast<unsigned long>(res);
    res >>= 32;
    res &= Mask16;
    
    unsigned short inacchi_ = static_cast<unsigned short>(res);
    
    CosClockService::UTC*   ret_val 
        = new  UTC_impl;
    
    ret_val->time(time_);
    ret_val->inacclo(inacclo_);
    ret_val->inacchi(inacchi_);
    ret_val->tdf(0);
    return ret_val;
}

///////////////////////////////////////////////////////////////////////////////

TimeSpan_impl_init::TimeSpan_impl_init()
{
    // No-op
}

///////////////////////////////////////////////////////////////////////////////

CosClockService::TimeSpan* 
TimeSpan_impl_init::init(const TimeBase::IntervalT & from)
    throw (CORBA::SystemException)
{
  CosClockService::TimeSpan* 
    ret_val = new  TimeSpan_impl;
  CDMW_ASSERT( ret_val );
  ret_val->lower_bound(from.lower_bound);
  ret_val->upper_bound(from.upper_bound);
  return ret_val;  
}
    
///////////////////////////////////////////////////////////////////////////////

CosClockService::TimeSpan* 
TimeSpan_impl_init::compose(TimeBase::TimeT lower_bound,
                           TimeBase::TimeT upper_bound)
    throw (CORBA::SystemException)
{
  CosClockService::TimeSpan* 
    ret_val = new  TimeSpan_impl;
  CDMW_ASSERT( ret_val );
  ret_val->lower_bound(lower_bound);
  ret_val->upper_bound(upper_bound);
  return ret_val;  
}
      
///////////////////////////////////////////////////////////////////////////////

CORBA::ValueBase*  
TimeSpan_impl_init::create_for_unmarshal()
{
  return new  TimeSpan_impl();
}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
