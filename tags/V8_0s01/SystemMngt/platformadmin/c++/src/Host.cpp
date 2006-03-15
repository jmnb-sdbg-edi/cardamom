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


#include "platformadmin/Host.hpp"
#include "platformadmin/ConfigFileParser.hpp"
#include "platformadmin/SystemParser.hpp"
#include "platformadmin/MonitoringParser.hpp"
#include "platformadmin/HostParser.hpp"
#include "platformadmin/EntityParser.hpp"
#include "platformadmin/Property.hpp"
#include "platformadmin/PropertyParser.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "platformadmin/AdminLogMessageId.hpp"



namespace Cdmw {         // Begin namespace Cdmw
    namespace PlatformAdmin { // Begin namespace PlatformAdmin

	Host::Host(
	    CdmwPlatformMngt::Host_ptr host )
	{
	    m_host = CdmwPlatformMngt::Host::_duplicate( host );
	}

	Host::~Host()
	{
	}

	CdmwPlatformMngt::HostDef* Host::get_definition(
	    const HostParser& hostParser )
	{
	    try
	    {
		CdmwPlatformMngt::HostDef_var hostDef
		    = new CdmwPlatformMngt::HostDef();

		// Get the host's name
		hostDef->host_name = CORBA::string_dup( hostParser.name().c_str() );

		// Set the application process's info
		hostDef->platform_application_exe
		    = CORBA::string_dup( hostParser.app_exec_path().c_str() );

		// Set the monitoring's info
		hostDef->daemon_monitoring_info.monitoring_interval
		    = hostParser.get_monitoring().pull_interval();
		hostDef->daemon_monitoring_info.monitoring_timeout
		    = hostParser.get_monitoring().pull_timeout();
            
		// Add all the entity definitions to the host definition
		unsigned int entityCount = hostParser.get_entity_count();
		hostDef->entities.length( entityCount );
		for( unsigned int entityIndex=0; entityIndex<entityCount; entityIndex++ )
		{
		    // Create an EntityDef
		    CdmwPlatformMngt::EntityDef_var entityDef = Entity::get_definition(
			hostParser.get_entity( entityIndex ) );
        	
		    std::string newEntityName = entityDef->entity_name.in();

		    // Checks there is no entity redefinition
		    for( unsigned int l_entityIndex = 0;
			 l_entityIndex < entityIndex;
			 l_entityIndex++ )
		    {
			std::string entityName
			    = hostDef->entities[ l_entityIndex ].entity_name.in();
                
			if( newEntityName == entityName )
			    throw CdmwPlatformMngt::DuplicateEntity(
				newEntityName.c_str() );
		    }
		    hostDef->entities[ entityIndex ] =  entityDef.in();
		}
	
		// Add Properties to host
		unsigned int propertyCount = hostParser.get_property_count();
		hostDef->properties.length( propertyCount );
		for( unsigned int propIndex=0; propIndex<propertyCount; propIndex++)
		{
		    CdmwPlatformMngt::PropertyDef_var prop = Property::get_definition(hostParser.get_property(propIndex));
		    hostDef->properties[propIndex] = prop;
		}

		return hostDef._retn();
	    }
	    catch( const std::bad_alloc& )
	    {
		CDMW_THROW( OutOfMemoryException );
	    }
	}

      
	void Host::add_property(const char* propertyConfigFileName) 
	{
	    try
	    {
                ConfigFileParser parser( propertyConfigFileName );
		PropertyParser propertyparser = parser.get_property();
		CdmwPlatformMngt::PropertyDefs_var propertydefs ;
		CosPropertyService::PropertyDef_var cos_prop = new CosPropertyService::PropertyDef;
		CosPropertyService::PropertyModeType prop_mode_type = CosPropertyService::PropertyModeType(0);
		CdmwPlatformMngt::PropertyDef_var prop = Property::get_definition(propertyparser);
                cos_prop->property_name = prop->property_name;
		cos_prop->property_value = prop->initial_value;
		if (prop->mode_type==1) prop_mode_type = CosPropertyService::PropertyModeType(1);
		cos_prop->property_mode = prop_mode_type;
		m_host -> define_property_with_mode(prop->property_name.in(),prop->initial_value, prop_mode_type);
	    }
	    catch( const CdmwPlatformMngt::IncompatibleStatus& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_INCOMPATIBLE_STATUS );
		
		throw LoggedException();
	    }
	}
	
	void Host::remove_property(const char* propertyConfigFileName)
	{
           try
            {
                ConfigFileParser parser( propertyConfigFileName );
                PropertyParser propertyparser = parser.get_property();
                CdmwPlatformMngt::PropertyDefs_var propertydefs ;
                CdmwPlatformMngt::PropertyDef_var prop = Property::get_definition(propertyparser);
		m_host -> delete_property (prop -> property_name);
            }
            catch( const CdmwPlatformMngt::IncompatibleStatus& e )
            {
                PlatformMngt::LogMngr::logMessage(
                    PlatformMngt::ERR,
                    MSG_ID_ADMIN_INCOMPATIBLE_STATUS );

                throw LoggedException();
            }
	}
 
    } // End namespace PlatformAdmin
} // End namespace Cdmw

