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


#include "platformadmin/Entity.hpp"
#include "platformadmin/EntityParser.hpp"
#include "platformadmin/ConfigFileParser.hpp"

extern bool check_only;

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

/**
 * class Entity
 */
char* Entity::get_definition(
    const EntityParser& entityParser )
{
    CdmwPlatformMngtEntity::EntityName_var entityName
        = CORBA::string_dup( entityParser.name().c_str() );

    return entityName._retn();
}

/**
 * class EntityContainer
 */
EntityContainer::EntityContainer(
    CdmwPlatformMngtEntity::RWEntityContainer_ptr entityContainer )
{
    if( !check_only )
        m_entity_container = CdmwPlatformMngtEntity::RWEntityContainer::_duplicate(
            entityContainer );
}

void EntityContainer::add_entity(
    const char* entityName )
{
    if( !check_only )
        m_entity_container->add_entity( entityName );
}

void EntityContainer::remove_entity(
    const char* entityName )
{
    if( !check_only )
        m_entity_container->remove_entity( entityName );
}

} // End namespace PlatformAdmin
} // End namespace Cdmw

