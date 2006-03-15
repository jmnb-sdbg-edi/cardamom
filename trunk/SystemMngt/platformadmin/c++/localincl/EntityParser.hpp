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


#ifndef INCL_PLATFORMADMIN_ENTITYPARSER_HPP
#define INCL_PLATFORMADMIN_ENTITYPARSER_HPP

#include "platformadmin/ConfigElementParser.hpp"
#include "platformadmin/PropertyParser.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

class PropertyParser;

class EntityParser : public virtual ConfigElementParser
{
public:
    EntityParser( const DOM_Node& entityParser );
    std::string name() const;
    PropertyParser get_property(unsigned int propertyIndex) const;
    unsigned int get_property_count() const;
};

class EntityContainerParser : public virtual ConfigElementParser
{
protected:
    EntityContainerParser( const DOM_Node& entityContainerParser );

public:
    unsigned int get_entity_count() const;

    EntityParser get_entity( unsigned int entityIndex ) const;
};

} // End namespace PlatformAdmin
} // End namespace Cdmw

#endif //INCL_PLATFORMADMIN_ENTITYPARSER_HPP

