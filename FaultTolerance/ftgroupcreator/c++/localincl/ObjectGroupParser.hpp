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


#ifndef INCL_FTGROUPCREATOR_OBJECTGROUPPARSER_HPP
#define INCL_FTGROUPCREATOR_OBJECTGROUPPARSER_HPP

#include "ftgroupcreator/ConfigElementParser.hpp"


namespace Cdmw {         // Begin namespace Cdmw
namespace FT { // Begin namespace FT

class LocationsParser;
class DatastoreIDsParser;

class ObjectGroupParser : public virtual ConfigElementParser
{
public:
    ObjectGroupParser( const DOM_Node& objectGroupParser );

    std::string name() const;

    std::string type_id() const;

    unsigned int min_num_replicas() const;

    LocationsParser get_locations(  ) const;

    DatastoreIDsParser get_datastoreIDs(  ) const;

    bool contains_datastoreIDs() const;
    
};

} // End namespace FT
} // End namespace Cdmw

#endif //INCL_FTGROUPCREATOR_OBJECTGROUPPARSER_HPP

