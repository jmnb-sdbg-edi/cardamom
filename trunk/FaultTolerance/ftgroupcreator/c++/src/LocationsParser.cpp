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
#include "ftgroupcreator/LocationsParser.hpp"
#include "ftgroupcreator/LocationParser.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace FT { // Begin namespace FT

LocationsParser::LocationsParser(
    const DOM_Node& locationsParser )
: ConfigElementParser( locationsParser )
{
}


unsigned int LocationsParser::get_location_count() const
{
    return get_child_count( "location" );
}

LocationParser LocationsParser::get_location(unsigned int locationIndex ) const
{
    DOM_Node location_parser(
        get_child_element( "location", locationIndex ) );

    return LocationParser (location_parser );
}



} // End namespace FT
} // End namespace Cdmw

