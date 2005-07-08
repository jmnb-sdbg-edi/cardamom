/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
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
*/
/* ===================================================================== */


// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>

#include <CCMContainer/ccmcif/FacetDescription_impl.hpp>

namespace {


} // End anonymous space

namespace Cdmw {

namespace CCM {

namespace CIF {


    //
    // IDL:omg.org/Components/FacetDescription:1.0
    //
    FacetDescription_impl::FacetDescription_impl()
    {
        facet_ref(CORBA::Object::_nil ());
    }
    
    FacetDescription_impl::~FacetDescription_impl()
    {
    }
    
    CORBA::ValueBase*
        FacetDescription_impl::_copy_value()
    {
        FacetDescription_impl* result = new FacetDescription_impl;
        result->name(name());
        result->type_id(type_id());
        CORBA::Object_var obj(CORBA::Object::_duplicate(facet_ref()));
        result->facet_ref(obj.in());
        
        return result;
    }
    
    
    /******************************/
    /*         Factory            */
    /******************************/
    
    FacetDescriptionFactory::FacetDescriptionFactory()
    {
    }
    
    FacetDescriptionFactory::~FacetDescriptionFactory()
    {
    }
    
    CORBA::ValueBase *
        FacetDescriptionFactory::create_for_unmarshal ()
    {
        return new FacetDescription_impl;
    }

} // End namespace CIF
} // End namespace CCM
} // End namespace Cdmw



