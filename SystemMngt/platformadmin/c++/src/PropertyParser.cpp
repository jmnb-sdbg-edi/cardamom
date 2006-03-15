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


#include "platformadmin/PropertyParser.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

/**
 * Property class.
 */
PropertyParser::PropertyParser(
    const DOM_Node& propertyParser )
: ConfigElementParser( propertyParser )
{
}

std::string PropertyParser::name() const
{
    return get_attribute_value( "Name" );
}

std::string PropertyParser::get_name() const
{
     std::string name = get_attribute_value("Name");	
     return name;	
}                                                                                                                             
std::string PropertyParser::get_type() const
{
    std::string type_name = get_attribute_value("Type");
    return type_name;
}      
               
std::string PropertyParser::get_value() const                                                                               
{
     std::string value = get_attribute_value("Value");
     return value;
}

std::string PropertyParser::get_mode() const
{
     std::string mode = get_attribute_value("Mode");	
     return mode;
}


} // End namespace PlatformAdmin
} // End namespace Cdmw

