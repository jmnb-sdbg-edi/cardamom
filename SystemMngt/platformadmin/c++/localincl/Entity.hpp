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


#ifndef INCL_PLATFORMADMIN_ENTITY_HPP
#define INCL_PLATFORMADMIN_ENTITY_HPP

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntity.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntityContainer.stub.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

class ConfigException;
class EntityParser;

class Entity
{
public:
    static CdmwPlatformMngt::EntityDef* get_definition(
        const EntityParser& entityParser );
    
    Entity(CdmwPlatformMngt::Entity_ptr entity);
    ~Entity();
    void add_property(const char* propertyConfigFileName) ;
    void remove_property(const char* propertyConfigFileName);
    CdmwPlatformMngt::Entity_var m_entity;	
};

class EntityContainer
{
private:
    CdmwPlatformMngt::RWEntityContainer_var m_entity_container;

protected:
    EntityContainer(
        CdmwPlatformMngt::RWEntityContainer_ptr entity_container );

public:
    void add_entity(
        const char* entityConfigFileName );

    void remove_entity(
        const char* entityName );
};

} // End namespace PlatformAdmin
} // End namespace Cdmw

#endif //INCL_PLATFORMADMIN_ENTITY_HPP

