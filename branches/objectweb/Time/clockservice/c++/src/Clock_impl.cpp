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

#include <Time/clockservice/Clock_impl.hpp>
#include <Time/clockservice/config.hpp>
#include <Time/clockservice/Controller_impl.hpp>

#include <iostream>
#include <Foundation/common/Assert.hpp>

using namespace std;
using namespace Cdmw::clock;
using namespace CosPropertyService;
using namespace CosClockService;


// Return the current gregorian time
#ifndef  WIN32

TimeBase::TimeT 
Clock_impl::compute_current_time()
{
    timeval timeofday_;
    if( gettimeofday(&timeofday_,0 ) < 0 )
        throw CosClockService::TimeUnavailable();
    
    return static_cast<TimeBase::TimeT>
        (TIME_BASE 
         + static_cast<TimeBase::TimeT>(timeofday_.tv_sec ) * 10000000
         + static_cast<TimeBase::TimeT>(timeofday_.tv_usec) * 10 );
}

#else

TimeBase::TimeT 
Clock_impl::compute_current_time()
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

////////////////////////////////////////////////////////////////////////////////

// _implementation skeleton constructor
Clock_impl::Clock_impl() 
{
    property_ = propset_factory_.create_propertyset();
    
    {
        // resolution
        PropertyName name = "Resolution";
        // in windows is possible to have millisecond resolution
        CORBA::Any value;
        value <<= (CORBA::ULong)(Cdmw::clock::RESOLUTION);
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
        PropertyName name = "Width";
        CORBA::Any value;
        value<<= 64;
        property_->define_property( name, value );
    }
    
    {
        // Stability_Description
        PropertyName name = "Stability_Description";
        CORBA::Any value;
        value<<= "Not Available";
        property_->define_property( name, value );
    }
    
    {
        // Coordination
        PropertyName name = "Coordination";
        CORBA::Any value;
        value<<= 0;
        property_->define_property( name, value );
    }
    
    {
    /*
    // Measurement
    PropertyName name = "Measurement";
    CORBA::Any value;
    value<<= 0;
    property_->define_property( name, value );
        */
    }
    
    {
        // TimeScale
        PropertyName name = "TimeScale";
        CORBA::Any value;
        value<<= ClockProperty::Local_;
        property_->define_property( name, value );
    }
}

////////////////////////////////////////////////////////////////////////////////

Clock_impl::~Clock_impl()
{
    // No-Op
}

////////////////////////////////////////////////////////////////////////////////

PropertySet_ptr 
Clock_impl::properties()
    throw(CORBA::SystemException)
{
    return  PropertySet::_duplicate(property_.in());
}

////////////////////////////////////////////////////////////////////////////////

TimeBase::TimeT 
Clock_impl::current_time()
    throw(CORBA::SystemException,
          CosClockService::TimeUnavailable )
{
    // Add your implementation here
    return Clock_impl::compute_current_time();
}

////////////////////////////////////////////////////////////////////////////////
//  End Of File
////////////////////////////////////////////////////////////////////////////////

