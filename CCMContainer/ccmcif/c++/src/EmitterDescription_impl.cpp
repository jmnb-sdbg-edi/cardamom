/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
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


// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>

#include <CCMContainer/ccmcif/EmitterDescription_impl.hpp>

namespace {


} // End anonymous space

namespace Cdmw {

namespace CCM {

namespace CIF {

    //
    // IDL:omg.org/Components/EmitterDescription:1.0
    //
    EmitterDescription_impl::EmitterDescription_impl ()
    {
        consumer (Components::EventConsumerBase::_nil ());
    }
 
    EmitterDescription_impl::~EmitterDescription_impl()
    {
    }
    
    CORBA::ValueBase*
        EmitterDescription_impl::_copy_value()
    {
        EmitterDescription_impl* result = new EmitterDescription_impl;
        result->name(name());
        result->type_id(type_id());
        result->consumer(consumer());

        return result;
    }


    /******************************/
    /*         Factory            */
    /******************************/
    EmitterDescriptionFactory::EmitterDescriptionFactory()
    {
    }
    
    EmitterDescriptionFactory::~EmitterDescriptionFactory()
    {
    }

    CORBA::ValueBase *
        EmitterDescriptionFactory::create_for_unmarshal ()
    {
        return new EmitterDescription_impl;
    }

} // End namespace CIF
} // End namespace CCM
} // End namespace Cdmw


