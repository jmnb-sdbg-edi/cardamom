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

#include <CCMContainer/ccmcif/PortDescription_impl.hpp>

namespace {


} // End anonymous space

namespace Cdmw {

namespace CCM {

namespace CIF {

    //
    // IDL:omg.org/Components/PortDescription:1.0
    //
    PortDescription_impl::PortDescription_impl()
    {
        name ((const char *) "");
        type_id ((const char *) "");
    }
    
    PortDescription_impl::~PortDescription_impl()
    {
    }
    
    CORBA::ValueBase*
        PortDescription_impl::_copy_value()
    {
        PortDescription_impl* result = new PortDescription_impl;
        result->name(name());
        result->type_id(type_id());
        
        return result;
    }
    
    
    /******************************/
    /*         Factory            */
    /******************************/
    
    PortDescriptionFactory::PortDescriptionFactory()
    {
    }
    
    PortDescriptionFactory::~PortDescriptionFactory()
    {
    }
    
    CORBA::ValueBase *
        PortDescriptionFactory::create_for_unmarshal ()
    {
        return new PortDescription_impl;
    }

} // End namespace CIF
} // End namespace CCM
} // End namespace Cdmw



