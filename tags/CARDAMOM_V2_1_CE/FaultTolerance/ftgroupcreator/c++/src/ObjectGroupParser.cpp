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


#include "ftgroupcreator/ObjectGroupParser.hpp"
#include "ftgroupcreator/LocationsParser.hpp"
#include "ftgroupcreator/DatastoreIDsParser.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace FT { // Begin namespace FT

ObjectGroupParser::ObjectGroupParser(
    const DOM_Node& objectGroupParser )
: ConfigElementParser( objectGroupParser )
{
}

std::string ObjectGroupParser::name() const
{
    return get_attribute_value( "name" ).c_str();
}

std::string ObjectGroupParser::type_id() const
{
    return get_attribute_value( "type-id" ).c_str();
}

unsigned int ObjectGroupParser::min_num_replicas() const
{
    return atol (get_attribute_value( "minimum-number-replicas" ).c_str() );
}


LocationsParser ObjectGroupParser::get_locations(  ) const
{
    DOM_Node locations_parser(get_child_element( "locations", 0 ));
    
    return LocationsParser ( locations_parser );
}

DatastoreIDsParser ObjectGroupParser::get_datastoreIDs(  ) const
{
    DOM_Node datastoreids_parser(get_child_element( "datastoreIDs" , 0));

    return DatastoreIDsParser ( datastoreids_parser );
}


bool ObjectGroupParser::contains_datastoreIDs() const
{
    if (get_child_count("datastoreIDs") == 0)
        return false;
    else
        return true;
}




} // End namespace FT
} // End namespace Cdmw

