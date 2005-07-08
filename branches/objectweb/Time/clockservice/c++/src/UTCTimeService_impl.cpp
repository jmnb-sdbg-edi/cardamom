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

#include <Time/clockservice/UTCTimeService_impl.hpp>
#include <Time/clockservice/UTC_impl.hpp>
#include <Time/clockservice/Clock_impl.hpp>
#include <Time/clockservice/config.hpp>

#include <stdexcept>
#include <limits>
#include <ctime>
#include <iostream>
#include "ace/OS.h"

using namespace std;
using namespace CosClockService;
using namespace CosPropertyService;
using namespace Cdmw::clock;

////////////////////////////////////////////////////////////////////////////////

UtcTimeService_impl::UtcTimeService_impl (void)
{
    factory_ = new UTC_impl_init;  
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

UtcTimeService_impl::~UtcTimeService_impl (void)
{
    delete factory_;
}

////////////////////////////////////////////////////////////////////////////////

/** 
 *  \fn ::CosClockService::UTC* UtcTimeService_impl::universal_time();
 *  \brief  Return the current time and an estimate of inaccuracy in a UTC.
 *  \return a pointer to UTC.
 *  \exception  TimeUnavalaible, indicate a failure in underlying time provider
 *  
 */
::CosClockService::UTC * 
UtcTimeService_impl::universal_time()
    throw (CORBA::SystemException, 
           CosClockService::TimeUnavailable)
{
    TimeBase::TimeT time = this->current_time();
    
    CORBA::ULong inacclo = 0;
    CORBA::UShort inacchi = 0;
    TimeBase::TdfT tdf = 0;
    
    return factory_->compose(time,
                             inacclo,
                             inacchi,
                             tdf);
}

////////////////////////////////////////////////////////////////////////////////

::CosClockService::UTC * 
UtcTimeService_impl::secure_universal_time ()
    throw (CORBA::SystemException,
            CosClockService::TimeUnavailable)
{
    //throw CosClockService::TimeUnavailable();
    //  ACE_THROW_RETURN(CORBA::NO_IMPLEMENT (),CosClockService::UTC::_nil());
    throw CORBA::NO_IMPLEMENT ();
}
////////////////////////////////////////////////////////////////////////////////

::CosClockService::UTC * 
UtcTimeService_impl::absolute_time (CosClockService::UTC * with_offset )
    throw (CORBA::SystemException, 
           CosClockService::TimeUnavailable )
{
    static TimeBase::TimeT max_val = std::numeric_limits<TimeBase::TimeT>::max();
    
    TimeBase::TimeT delta = max_val - with_offset->time();
    
    TimeBase::TimeT time = this->current_time(); 
    
    if (time > delta) throw CORBA::DATA_CONVERSION();
    
    CORBA::ULong inacclo = 0;	
    CORBA::UShort inacchi = 0;	
    TimeBase::TdfT tdf = 0;     
    
    inacclo = (inacclo > with_offset->inacclo()) ? inacclo : with_offset->inacclo();
    inacchi = (inacchi > with_offset->inacchi()) ? inacchi : with_offset->inacchi();
    
    return factory_->compose(time+delta,
                             inacclo,
                             inacchi,
                             tdf);
}
////////////////////////////////////////////////////////////////////////////////

::CosPropertyService::PropertySet_ptr 
UtcTimeService_impl::properties ()
    throw (CORBA::SystemException)
{
    return  PropertySet::_duplicate(property_.in());
}

////////////////////////////////////////////////////////////////////////////////
/*
*/
TimeBase::TimeT UtcTimeService_impl::current_time ()
    throw (CORBA::SystemException, 
           CosClockService::TimeUnavailable)
{
    return Clock_impl::compute_current_time();
}
////////////////////////////////////////////////////////////////////////////////
