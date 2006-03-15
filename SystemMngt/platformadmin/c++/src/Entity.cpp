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


#include "platformadmin/Entity.hpp"
#include "platformadmin/EntityParser.hpp"
#include "platformadmin/ConfigFileParser.hpp"
#include "platformadmin/AdminLogMessageId.hpp"
#include "platformadmin/Property.hpp"
#include "platformadmin/PropertyParser.hpp"

#include "SystemMngt/platformlibrary/LogMngr.hpp"

extern bool check_only;

namespace Cdmw {         // Begin namespace Cdmw
    namespace PlatformAdmin { // Begin namespace PlatformAdmin

/**
 * class Entity
 */
	
	Entity::Entity( CdmwPlatformMngt::Entity_ptr entity )
        {
            if( !check_only )
                m_entity = CdmwPlatformMngt::Entity::_duplicate( entity);
        }
                                                                                                                             
        Entity::~Entity()
        {
        }

	CdmwPlatformMngt::EntityDef* Entity::get_definition(
	    const EntityParser& entityParser )
	{
	    try
	    {
		CdmwPlatformMngt::EntityDef_var entityDef
		    = new CdmwPlatformMngt::EntityDef();

		// Get the entity's name
		entityDef->entity_name = CORBA::string_dup( entityParser.name().c_str() );

		//Add Properties to Entity                                                                                                                              
		unsigned int propertyCount = entityParser.get_property_count();
		entityDef->properties.length( propertyCount );
                                                                                                                             
		for( unsigned int propIndex=0; propIndex<propertyCount; propIndex++)
		{
		    CdmwPlatformMngt::PropertyDef_var prop = Property::get_definition(entityParser.get_property(propIndex));
		    entityDef->properties[propIndex] = prop;
		}

		return entityDef._retn();
	    }
	    catch( const std::bad_alloc& )
	    {
		CDMW_THROW( OutOfMemoryException );
	    }
	}
      
      void Entity::add_property(const char* propertyConfigFileName) 
      {
	try
	  {
	    ConfigFileParser parser( propertyConfigFileName );
	    PropertyParser propertyparser = parser.get_property();
	    CosPropertyService::PropertyDef_var cos_prop = new CosPropertyService::PropertyDef;
	    CosPropertyService::PropertyModeType prop_mode_type = CosPropertyService::PropertyModeType(0);
	    CdmwPlatformMngt::PropertyDef_var prop = Property::get_definition(propertyparser);
	    cos_prop->property_name = prop->property_name;
	    cos_prop->property_value = prop->initial_value;
	    if (prop->mode_type==1) prop_mode_type = CosPropertyService::PropertyModeType(1);
	    cos_prop->property_mode = prop_mode_type;
	    m_entity -> define_property_with_mode(prop->property_name.in(),prop->initial_value, prop_mode_type);
	  }
	catch( const CdmwPlatformMngt::IncompatibleStatus& e )
	  {
	    PlatformMngt::LogMngr::logMessage(
					      PlatformMngt::ERR,
					      MSG_ID_ADMIN_INCOMPATIBLE_STATUS );
	    
	    throw LoggedException();
	  }
      }
      
      
      void Entity::remove_property(const char* propertyConfigFileName)
      {
	try
	  {
	    ConfigFileParser parser( propertyConfigFileName );
	    PropertyParser propertyparser = parser.get_property();
	    CdmwPlatformMngt::PropertyDef_var prop = Property::get_definition(propertyparser);
	    m_entity -> delete_property (prop -> property_name);
	  }
	catch( const CdmwPlatformMngt::IncompatibleStatus& e )
	  {
	    PlatformMngt::LogMngr::logMessage(
					      PlatformMngt::ERR,
					      MSG_ID_ADMIN_INCOMPATIBLE_STATUS );
	    
	    throw LoggedException();
	  }
	
      }
      
      
      

/**
 * class EntityContainer
 */
	EntityContainer::EntityContainer(
	    CdmwPlatformMngt::RWEntityContainer_ptr entityContainer )
	{
	    if( !check_only )
		m_entity_container = CdmwPlatformMngt::RWEntityContainer::_duplicate(
		    entityContainer );
	}

	void EntityContainer::add_entity(
	    const char* entityConfigFileName )
	{
	    // Open the entity configuration file
	    ConfigFileParser parser( entityConfigFileName );

	    // Get a entity definition
	    CdmwPlatformMngt::EntityDef_var entityDef
		= Entity::get_definition( parser.get_entity() );

	    try
	    {
		if( !check_only )
		    m_entity_container->add_entity( entityDef.in() );
	    }
	    catch( const CdmwPlatformMngt::EntityAlreadyExists& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_ENTITY_ALREADY_EXISTS,
		    entityDef->entity_name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::IncompatibleStatus& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_INCOMPATIBLE_STATUS );

		throw LoggedException();
	    }
	}

	void EntityContainer::remove_entity(
	    const char* entityName )
	{
	    if( !check_only )
		m_entity_container->remove_entity( entityName );
	}

    } // End namespace PlatformAdmin
} // End namespace Cdmw

