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

#include "Time/clockservice/config.hpp"
#include "clockservice/Clock_impl.hpp"
#include "clockservice/Controller_impl.hpp"
#include "Time/clockservice/Util.hpp"

#include <iostream>
#include <Foundation/common/Assert.hpp>

using namespace std;
using namespace Cdmw::clock;
using namespace CosPropertyService;
using namespace CosClockService;

////////////////////////////////////////////////////////////////////////////////

// _implementation skeleton constructor
Clock_impl::Clock_impl()
    : propset_factory_(0),
      property_(0)
{
    TAO_PropertySetFactory* propset_factory = new TAO_PropertySetFactory();
    propset_factory_ = propset_factory->_this();
    property_ = propset_factory_->create_propertyset();
    property_->delete_all_properties();

    {
        // resolution
        PropertyName name = "Resolution";
        // in windows is possible to have millisecond resolution
        CORBA::Any value;
        value <<= (CORBA::ULong)(Cdmw::clock::RESOLUTION);
        property_->define_property( name, value );
    }

    {
        // Precision
        PropertyName name = "Precision";
        CORBA::Any value;
        value<<= (CORBA::Short)24;
        property_->define_property( name, value );
    }
    
    {
        // Width
        PropertyName name = "Width";
        CORBA::Any value;
        value<<= (CORBA::Short)64;
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
        value<<= (CORBA::Short)0;
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
    CosPropertyService::PropertySet_var property_set(PropertySet::_duplicate(property_.in()));
    return property_set._retn();
}

////////////////////////////////////////////////////////////////////////////////

TimeBase::TimeT 
Clock_impl::current_time()
    throw(CORBA::SystemException,
          CosClockService::TimeUnavailable )
{
    // Add your implementation here
    return Cdmw::clock::compute_current_time();
}

////////////////////////////////////////////////////////////////////////////////
//  End Of File
////////////////////////////////////////////////////////////////////////////////

