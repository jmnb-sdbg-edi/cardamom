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


#include "platformadmin/EntityParser.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

EntityContainerParser::EntityContainerParser(
    const DOM_Node& entityContainerParser )
: ConfigElementParser( entityContainerParser )
{
}

unsigned int EntityContainerParser::get_entity_count() const
{
    return get_child_count( "Entity" );
}

EntityParser EntityContainerParser::get_entity(
    unsigned int entityIndex ) const
{
    DOM_Node entity_Parser(
        get_child_element( "Entity", entityIndex ) );

    return EntityParser( entity_Parser );
}

EntityParser::EntityParser( const DOM_Node& entityParser )
: ConfigElementParser( entityParser )
{
}

std::string EntityParser::name() const
{
    return get_attribute_value( "Name" );
}

} // End namespace PlatformAdmin
} // End namespace Cdmw

