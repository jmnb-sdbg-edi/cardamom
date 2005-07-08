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


#include "platformadmin/PropertyParser.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

PropertyContainerParser::PropertyContainerParser(
    const DOM_Node& propertyContainerParser )
: ConfigElementParser( propertyContainerParser )
{
}

unsigned int PropertyContainerParser::get_property_count() const
{
    return get_child_count( "Property" );
}

PropertyParser PropertyContainerParser::get_property(
    unsigned int propertyIndex ) const
{
    DOM_Node property_Parser(
        get_child_element( "Property", propertyIndex ) );

    return PropertyParser( property_Parser );
}

PropertyParser::PropertyParser(
    const DOM_Node& propertyParser )
: ConfigElementParser( propertyParser )
{
}

std::string PropertyParser::name() const
{
    return get_attribute_value( "Name" );
}

} // End namespace PlatformAdmin
} // End namespace Cdmw

