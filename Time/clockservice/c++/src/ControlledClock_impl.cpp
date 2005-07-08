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

#include <Time/clockservice/ControlledClock_impl.hpp>
#include <Time/clockservice/Clock_impl.hpp>
#include <Time/clockservice/config.hpp>

#include <Time/clockservice/Macro.hpp>
#include <Time/clockservice/Util.hpp>

#include <Foundation/common/Assert.hpp>

#include "ace/Synch.h"
#include <limits>

using namespace CosPropertyService;
using namespace Cdmw::clock;

///////////////////////////////////////////////////////////////////////////////

ControlledClock_impl::ControlledClock_impl()
    : ratio_(1),
      status_(RUNNING),
      X0_(0),
      Y0_(0),
      frozenTime_(0)
{
    property_ = propset_factory_.create_propertyset();
    
    {
        // resolution
        CosPropertyService::PropertyName name = "Resolution";
        // in windows is possible to have millisecond resolution
        CORBA::Any value;
        value <<= (CORBA::ULong)(RESOLUTION);
        property_->define_property( name, value );
    }
    
    {
    /*
    // Precision
    PropertyName name = "Precision";
    CORBA::Any value;
    value<<= ;
    property_->define_property( name, value );
        */
    }
    
    {
        // Width
        CosPropertyService::PropertyName name = "Width";
        CORBA::Any value;
        value<<= 64;
        property_->define_property( name, value );
    }
    
    {
        // Stability_Description
        CosPropertyService::PropertyName name = "Stability_Description";
        CORBA::Any value;
        value<<= "Not Available";
        property_->define_property( name, value );
    }
    
    {
        // Coordination
        CosPropertyService::PropertyName name = "Coordination";
        CORBA::Any value;
        value<<= 0;
        property_->define_property( name, value );
    }
    
    {
    /*
    // Measurement
    CosPropertyService::PropertyName name = "Measurement";
    CORBA::Any value;
    value<<= 0;
    property_->define_property( name, value );
        */
    }
    
    {
        // TimeScale
        CosPropertyService::PropertyName name = "TimeScale";
        CORBA::Any value;
        value<<= CosClockService::ClockProperty::Local_;
        property_->define_property( name, value );
    }
}

///////////////////////////////////////////////////////////////////////////////

ControlledClock_impl::~ControlledClock_impl()
    throw()
{
    // No-Op.
}

///////////////////////////////////////////////////////////////////////////////

::CosPropertyService::PropertySet_ptr
ControlledClock_impl::properties()
    throw (CORBA::SystemException)
{
    return  PropertySet::_duplicate(property_.in());
}

///////////////////////////////////////////////////////////////////////////////

::TimeBase::TimeT
ControlledClock_impl::current_time()
    throw (CORBA::SystemException,
           CosClockService::TimeUnavailable)
{
    ACE_Guard<ACE_Mutex> guard(mutex_);
    ::TimeBase::TimeT retVal = frozenTime_;
    
    if (status_ == RUNNING)
    {
        ::TimeBase::TimeT tmp = Clock_impl::compute_current_time();
        retVal =  this->adjusted_virtual_time(tmp);
    }
    return retVal;
}

///////////////////////////////////////////////////////////////////////////////

void
ControlledClock_impl::set(TimeBase::TimeT virtualNow)
    throw (CORBA::SystemException,
           CosClockService::ControlledClock::NotSupported)
{
    ACE_Guard<ACE_Mutex> guard(mutex_);

    ::TimeBase::TimeT realNow =
          Clock_impl::compute_current_time(); 
    this->updateTimeOrigin(realNow, virtualNow);

    frozenTime_ = virtualNow;
}

///////////////////////////////////////////////////////////////////////////////

CORBA::Float 
ControlledClock_impl::get_rate ()
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported)
{
    ACE_Guard<ACE_Mutex> guard(mutex_);
    return ratio_;    
}               

///////////////////////////////////////////////////////////////////////////////

void
ControlledClock_impl::set_rate (CORBA::Float ratio)
    throw (CORBA::SystemException,
           CosClockService::ControlledClock::NotSupported)
{
    ACE_Guard<ACE_Mutex> guard(mutex_);
    ::TimeBase::TimeT realNow =
          Clock_impl::compute_current_time();
    ::TimeBase::TimeT virtualNow =
          this->adjusted_virtual_time(realNow);
    // update is done with old rate to have a correct
    //  virtual origin for time
    this->updateTimeOrigin(realNow, virtualNow);
    ratio_ = ratio;
}


///////////////////////////////////////////////////////////////////////////////

void
ControlledClock_impl::pause ()
    throw (CORBA::SystemException,
           CosClockService::ControlledClock::NotSupported)
{
    ACE_Guard<ACE_Mutex> guard(mutex_);
    if (status_ != RUNNING) 
        throw CORBA::BAD_INV_ORDER();
    
    ::TimeBase::TimeT tmp = Clock_impl::compute_current_time();
    frozenTime_ = this->adjusted_virtual_time(tmp);
    status_ = PAUSED;
}

///////////////////////////////////////////////////////////////////////////////

void
ControlledClock_impl::resume ()
    throw (CORBA::SystemException,
           CosClockService::ControlledClock::NotSupported)
{
    if (status_ != PAUSED) 
        throw CORBA::BAD_INV_ORDER();
    status_ = RUNNING;
    ::TimeBase::TimeT realNow =
          Clock_impl::compute_current_time();
    this->updateTimeOrigin(realNow, frozenTime_);
    frozenTime_ = 0;
}

///////////////////////////////////////////////////////////////////////////////

void
ControlledClock_impl::terminate ()
    throw (CORBA::SystemException ,
           CosClockService::ControlledClock::NotSupported)
{
    status_ = STOPPED;
}


///////////////////////////////////////////////////////////////////////////////

::TimeBase::TimeT
ControlledClock_impl::adjusted_virtual_time
(::TimeBase::TimeT time) const
{
    ::TimeBase::TimeT retVal = time - X0_;
    if (ratio_ >= 0 ) 
    {
        // rest of TimeBase::TimeT interval
        ::TimeBase::TimeT iv_disp = Cdmw::clock::macro::MAX_TIME_VAL - Y0_; 

        // Overflow: retVal is too big
        if (retVal > iv_disp)
            throw CORBA::BAD_PARAM();

        // common case has a rapid solution 
        if (ratio_ == 1.0)
            return Y0_ + retVal;

        // Overflow: rate is too big
        double  num = util::convert(iv_disp);
        double  den = util::convert(retVal);
        if (num/den < ratio_) // ratio_ > 1.0
            throw CORBA::BAD_PARAM();
    } 
    else
    {
        // Underflow: retVal is too big
        if (retVal > Y0_)
            throw CORBA::BAD_PARAM();

        // common case has a rapid solution 
        // if (ratio_ == -1)
        //    return Y0_ - retVal;
        
        // Underflow: rate is too big 
        double  num = util::convert(Y0_);
        double  den = util::convert(retVal);
        if (num/den < (- ratio_)) // check for ratio_ < -1.0
            throw CORBA::BAD_PARAM();        
    }

    ::TimeBase::TimeT intRatio =
            static_cast< ::TimeBase::TimeT >(ratio_ * DECIMAL_PRECISION);
    retVal = Y0_ + ((retVal / DECIMAL_PRECISION) * intRatio);
    return retVal;
}

////////////////////////////////////////////////////////////////////////////////

double  
ControlledClock_impl::retrieve_rate()
{
    return this->get_rate();
}

////////////////////////////////////////////////////////////////////////////////

::TimeBase::TimeT 
ControlledClock_impl::virtual_to_real(::TimeBase::TimeT time_val)
{
    // assuming that time equation is:
    // 
    //    y-Y0_ = ratio_ * ( x - X0_)
    //

    CDMW_ASSERT( time_val >= this->X0_ );

    ::TimeBase::TimeT 
        delta = time_val-X0_;

    double a1 = util::convert(delta);
    double a2 = util::convert(Y0_);
    ::TimeBase::TimeT 
    out = (::TimeBase::TimeT)(ratio_ * a1 + a2);
    return out;
}

////////////////////////////////////////////////////////////////////////////////

void
ControlledClock_impl::set_time_origins(::TimeBase::TimeT X0,
                                       ::TimeBase::TimeT Y0)
    throw()
{
    ACE_Guard<ACE_Mutex> guard(mutex_);
    X0_ = X0;
    Y0_ = Y0;
}

////////////////////////////////////////////////////////////////////////////////

void
ControlledClock_impl::set_frozen_time(::TimeBase::TimeT time)
    throw()
{
    ACE_Guard<ACE_Mutex> guard(mutex_);
    frozenTime_ = time;
}
////////////////////////////////////////////////////////////////////////////////
//  End Of File
////////////////////////////////////////////////////////////////////////////////
