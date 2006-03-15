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


#include "Foundation/orbsupport/OrbSupport.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"

#include "SystemMngt/platformlibrary/LogMngr.hpp"

#include "platformadmin/ConfigFileParser.hpp"
#include "platformadmin/SystemParser.hpp"
#include "platformadmin/HostParser.hpp"
#include "platformadmin/ApplicationParser.hpp"
#include "platformadmin/ServiceParser.hpp"
#include "platformadmin/RunSetParser.hpp"
#include "platformadmin/GraphParser.hpp"
#include "platformadmin/Host.hpp"
#include "platformadmin/Application.hpp"
#include "platformadmin/Service.hpp"
#include "platformadmin/RunSet.hpp"
#include "platformadmin/Graph.hpp"
#include "platformadmin/System.hpp"
#include "platformadmin/AdminLogMessageId.hpp"
#include "platformadmin/Property.hpp"
#include "platformadmin/PropertyParser.hpp"
extern bool check_only;

namespace Cdmw {         // Begin namespace Cdmw
    namespace PlatformAdmin { // Begin namespace PlatformAdmin

	System::System( CdmwPlatformMngt::System_ptr system )
	    : EntityContainer( system ),
	      ServiceContainer( system )
	{
	    if( !check_only )
		m_system = CdmwPlatformMngt::System::_duplicate( system );
	}

	System::~System()
	{
	}

	CdmwPlatformMngt::SystemSnapshot* System::get_snapshot()
	{
	    CdmwPlatformMngt::SystemSnapshot_var snapshot;
    
	    if( !check_only )
		snapshot = m_system->get_snapshot();

	    return snapshot._retn();
	}

	void System::start(
	    const CdmwPlatformMngtBase::StartupKind& startup_kind )
	{
	    try
	    {
		if( !check_only )
		    m_system->start( startup_kind );
	    }
	    catch( const CdmwPlatformMngt::IncompatibleStatus& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_INCOMPATIBLE_STATUS );

		throw LoggedException();
	    }
	}

	void System::stop( bool emergency )
	{
	    if( !check_only )
		m_system->stop( emergency );
	}

	void System::resume()
	{
	    try
	    {
		if( !check_only )
		    m_system->resume();
	    }
	    catch( const CdmwPlatformMngt::IncompatibleStatus& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_INCOMPATIBLE_STATUS );

		throw LoggedException();
	    }
	}

	void System::acknowledge()
	{
	    try
	    {
		if( !check_only )
		    m_system->acknowledge();
	    }
	    catch( const CdmwPlatformMngt::IncompatibleStatus& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_INCOMPATIBLE_STATUS );

		throw LoggedException();
	    }
	}

	CdmwPlatformMngt::SystemDef* System::get_definition(
	    const SystemParser& systemParser )
	{
	    try
	    {
		// Get a system definition
		CdmwPlatformMngt::SystemDef_var systemDef
		    = new CdmwPlatformMngt::SystemDef();

		systemDef->system_name
		    = CORBA::string_dup( systemParser.name().c_str() );
		systemDef->system_port = systemParser.port();

		// Add all the entity definitions to the system definition
		unsigned int entityCount = systemParser.get_entity_count();

		systemDef->entities.length( entityCount );
		for( unsigned int entityIndex=0; entityIndex<entityCount; entityIndex++ )
		{
		    // Create an EntityDef
		    CdmwPlatformMngt::EntityDef_var entityDef = Entity::get_definition(
			systemParser.get_entity( entityIndex ) );
                       
		    std::string newEntityName = entityDef->entity_name.in();

		    // Checks there is no entity redefinition
		    for( unsigned int l_entityIndex = 0;
			 l_entityIndex < entityIndex;
			 l_entityIndex++ )
		    {
			std::string entityName
			    = systemDef->entities[ l_entityIndex ].entity_name.in();
                
			if( newEntityName == entityName )
			    throw CdmwPlatformMngt::DuplicateEntity(
				newEntityName.c_str() );
		    }

		    systemDef->entities[ entityIndex ] =  entityDef.in();
		}

		// Add all the service definitions to the system definition
		unsigned int serviceCount = systemParser.get_service_count();

		systemDef->service_definitions.length( serviceCount );
		for( unsigned int serviceIndex=0;
		     serviceIndex<serviceCount;
		     serviceIndex++ )
		{
		    CdmwPlatformMngtService::ServiceDef_var serviceDef
			= Service::get_definition(
			    systemParser.get_service( serviceIndex ) );

		    // Checks there is no service redefinition
		    std::string newServiceName = serviceDef->service_name.in();

		    for( unsigned int l_serviceIndex = 0;
			 l_serviceIndex < serviceIndex;
			 l_serviceIndex++ )
		    {
			std::string serviceName =
			    systemDef->service_definitions[ l_serviceIndex ].service_name.in();

			if( newServiceName == serviceName )
			    throw CdmwPlatformMngt::DuplicateService(
				newServiceName.c_str() );
		    }

		    systemDef->service_definitions[ serviceIndex ] = serviceDef.in();
		}

		// Add all the host definitions to the system definition
		unsigned int hostCount = systemParser.get_host_count();

		systemDef->hosts.length( hostCount );
		for( unsigned int hostIndex=0; hostIndex<hostCount; hostIndex++ )
		{
		    CdmwPlatformMngt::HostDef_var hostDef = Host::get_definition(
			systemParser.get_host( hostIndex ) );

		    // Checks there is no host redefinition
		    std::string newHostName = hostDef->host_name.in();

		    for( unsigned int l_hostIndex = 0;
			 l_hostIndex < hostIndex;
			 l_hostIndex++ )
		    {
			std::string hostName =
			    systemDef->hosts[ l_hostIndex ].host_name.in();

			if( newHostName == hostName )
			    throw CdmwPlatformMngt::DuplicateHost(
				newHostName.c_str() );
		    }

		    systemDef->hosts[ hostIndex ] = hostDef.in();
		}
	
		unsigned int propertyCount = systemParser.get_property_count();
		systemDef->properties.length( propertyCount );
	
		for( unsigned int propIndex=0; propIndex<propertyCount; propIndex++)
		{
		    CdmwPlatformMngt::PropertyDef_var prop = Property::get_definition(systemParser.get_property(propIndex));
		    systemDef->properties[propIndex] = prop;		
		}

		return systemDef._retn();
	    }
	    catch( const std::bad_alloc& )
	    {
		CDMW_THROW( OutOfMemoryException );
	    }
	    catch( const CdmwPlatformMngt::DuplicateEntity& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_DUPLICATE_ENTITY,
		    e.entity_name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::DuplicateService& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_DUPLICATE_SERVICE,
		    e.service_name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::DuplicateHost& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_DUPLICATE_HOST,
		    e.host_name.in() );

		throw LoggedException();
	    }
	}

	void System::define( const char* systemConfigFileName )
	{
	    // Open the system configuration file
	    ConfigFileParser  parser( systemConfigFileName );
	    SystemParser      systemParser = parser.get_system();

	    try
	    {
		// define the system onto the platform supervision

		CdmwPlatformMngt::SystemDef_var systemDef = System::get_definition(
		    systemParser );
		if( !check_only )
		    m_system->define( systemDef.in() );
	
		// Add all the applications to the system
		unsigned int appCount = systemParser.get_application_count();
		for( unsigned int appIndex=0; appIndex<appCount; appIndex++ )
		{
		    // Create and add each application to the system onto the supervision
		    ApplicationParser appParser = systemParser.get_application( appIndex );
		    CdmwPlatformMngt::PlatformApplicationDef_var platformApplicationDef;
		    CdmwPlatformMngt::ApplicationDef_var applicationDef
			= Application::get_definition(
			    appParser,
			    platformApplicationDef.out() );
		    // Checks there is no application redefinition
		    for( unsigned int l_appIndex = 0;
			 l_appIndex < appIndex;
			 l_appIndex++ )
		    {
			std::string applicationName
			    = systemParser.get_application( l_appIndex ).name();
                    
			if( applicationName == platformApplicationDef->application_name.in() )
			{
			    PlatformMngt::LogMngr::logMessage(
				PlatformMngt::ERR,
				MSG_ID_ADMIN_DUPLICATE_APPLICATION,
				platformApplicationDef->application_name.in() );

			    throw LoggedException();
			}
		    }

		    // Actualy add the application to the system
		    CdmwPlatformMngt::Application_var application;
		    try
		    {
			if( !check_only )
			    application = m_system->add_application(
				platformApplicationDef.in(),
				applicationDef.in() );
		    }
		    catch( const CdmwPlatformMngt::ApplicationAlreadyExists& e )
		    {
			PlatformMngt::LogMngr::logMessage(
			    PlatformMngt::ERR,
			    MSG_ID_ADMIN_APPLICATION_ALREADY_EXISTS,
			    platformApplicationDef->application_name.in() );

			throw LoggedException();
		    }

		    try
		    {
			// Set each application's init process graph
			ProcessGraph initProcessGraph(
			    appParser,
			    appParser.get_init_graph(),
			    false );
			CdmwPlatformMngt::GraphElements_var rootGraphElements
			    = initProcessGraph.get_roots();

			if( !check_only )
			    application->set_init_graph( rootGraphElements.in() );
		    }
		    catch( const NotFoundException& )
		    {
		    }

		    try
		    {
			// Set each application's stop process graph
			ProcessGraph stopProcessGraph(
			    appParser,
			    appParser.get_stop_graph(),
			    true );
			CdmwPlatformMngt::GraphElements_var rootGraphElements
			    = stopProcessGraph.get_roots();

			if( !check_only )
			    application->set_stop_graph( rootGraphElements.in() );
		    }
		    catch( const NotFoundException& )
		    {
		    }
		}

		// Set the application run sets
		unsigned int runSetCount = systemParser.get_run_set_count();
		for( unsigned int runSetIndex = 0;
		     runSetIndex < runSetCount;
		     runSetIndex++ )
		{
		    RunSetParser runSetParser = systemParser.get_run_set( runSetIndex );
		    CdmwPlatformMngt::RunSetDef_var runSetDef
			= RunSet::get_definition( runSetParser );

		    // Checks there is no run set redefinition
		    for( unsigned int l_runSetIndex = 0;
			 l_runSetIndex < runSetIndex;
			 l_runSetIndex++ )
		    {
			std::string runSetName
			    = systemParser.get_run_set( l_runSetIndex ).name();
                    
			if( runSetName == runSetDef->set_name.in() )
			{
			    PlatformMngt::LogMngr::logMessage(
				PlatformMngt::ERR,
				MSG_ID_ADMIN_DUPLICATE_RUNSET,
				runSetDef->set_name.in() );

			    throw LoggedException();
			}
		    }

		    if( !check_only )
			m_system->set_run_set( runSetDef.in() );
		}

		try
		{
		    // Set the system's application init graph onto the platform supervision
		    CdmwPlatformMngt::GraphElements_var rootGraphElements;
		    ApplicationGraph initApplicationGraph(
			systemParser,
			systemParser.get_init_graph() );
		    rootGraphElements = initApplicationGraph.get_roots();

		    if( !check_only )
			m_system->set_init_graph( rootGraphElements.in() );
		}
		catch( const NotFoundException& )
		{
		}

		try
		{
		    // Set the system's application stop graph onto the platform supervision
		    ApplicationGraph stopApplicationGraph(
			systemParser,
			systemParser.get_stop_graph() );
		    CdmwPlatformMngt::GraphElements_var rootGraphElements
			= stopApplicationGraph.get_roots();

		    if( !check_only )
			m_system->set_stop_graph( rootGraphElements.in() );
		}
		catch( const NotFoundException& )
		{
		}
	    }
	    catch( const CdmwPlatformMngt::HostNotFound& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_HOST_NOT_FOUND );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::DuplicateEntity& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_DUPLICATE_ENTITY,
		    e.entity_name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::DuplicateService& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_DUPLICATE_SERVICE,
		    e.service_name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::DuplicateProcess& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_DUPLICATE_PROCESS,
		    e.process_name.in(),
		    e.host_name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::ProcessHostNotFound& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_PROCESS_HOST_NOT_FOUND,
		    e.host_name.in(),
		    e.process_name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::DuplicateProcessEntity& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_DUPLICATE_PROCESS_ENTITY,
		    e.entity_name.in(),
		    e.process_name.in(),
		    e.host_name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::DuplicateProcessService& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_DUPLICATE_PROCESS_SERVICE,
		    e.service_name.in(),
		    e.process_name.in(),
		    e.host_name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::DuplicateProcessStep& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_DUPLICATE_PROCESS_STEP,
		    e.step_label.in(),
		    e.process_name.in(),
		    e.host_name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::ReferenceNotFound& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_REFERENCE_NOT_FOUND,
		    e.name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::CircularReference& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_CIRCULAR_REFERENCE,
		    e.name.in(),
		    e.step );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::StepOutOfBound& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_STEP_OUT_OF_BOUND,
		    e.step,
		    e.name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::StepJump& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_STEP_JUMP,
		    e.name.in(),
		    e.origin_step,
		    e.target_step );

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

	void System::add_host( const char* hostConfigFileName )
	{
	    // Open the host configuration file
	    ConfigFileParser parser( hostConfigFileName );

	    // Get a host definition
	    CdmwPlatformMngt::HostDef_var hostDef
		= Host::get_definition( parser.get_host() );

	    try
	    {
		if( !check_only )
		    m_system->add_host( hostDef.in() );
	    }
	    catch( const CdmwPlatformMngt::HostAlreadyExists& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_HOST_ALREADY_EXISTS,
		    hostDef->host_name.in() );

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

	void System::remove_host( const char* hostConfigFileName )
	{
	    try
	    {
		if( !check_only )
		    m_system->remove_host( hostConfigFileName );
	    }
	    catch( const CdmwPlatformMngt::HostNotFound& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_HOST_NOT_FOUND,
		    hostConfigFileName );

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

	void System::add_application( const char* applicationConfigFileName )
	{
	    // Open the application's configuration file
	    ConfigFileParser  parser( applicationConfigFileName );
	    ApplicationParser appParser = parser.get_application();

	    // Add the application's definition
	    CdmwPlatformMngt::PlatformApplicationDef_var platformApplicationDef;
	    CdmwPlatformMngt::ApplicationDef_var applicationDef
		= Application::get_definition(
		    appParser,
		    platformApplicationDef.out() );
	    CdmwPlatformMngt::Application_var application;

	    try
	    {
		if( !check_only )
		    application = m_system->add_application(
			platformApplicationDef.in(),
			applicationDef.in() );

		try
		{
		    // Set the application's init process graph
		    CdmwPlatformMngt::GraphElements_var rootGraphElements;
		    ProcessGraph initProcessGraph(
			appParser,
			appParser.get_init_graph(),
			false );
		    rootGraphElements = initProcessGraph.get_roots();

		    if( !check_only )
			application->set_init_graph( rootGraphElements.in() );
		}
		catch( const NotFoundException& )
		{
		}

		try
		{
		    // Set the application's stop process graph
		    ProcessGraph stopProcessGraph(
			appParser,
			appParser.get_stop_graph(),
			true );
		    CdmwPlatformMngt::GraphElements_var rootGraphElements
			= stopProcessGraph.get_roots();

		    if( !check_only )
			application->set_stop_graph( rootGraphElements.in() );
		}
		catch( const NotFoundException& )
		{
		}
	    }
	    catch( const CdmwPlatformMngt::HostNotFound& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_HOST_NOT_FOUND );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::DuplicateEntity& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_DUPLICATE_ENTITY,
		    e.entity_name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::DuplicateService& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_DUPLICATE_SERVICE,
		    e.service_name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::DuplicateProcess& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_DUPLICATE_PROCESS,
		    e.process_name.in(),
		    e.host_name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::ProcessHostNotFound& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_PROCESS_HOST_NOT_FOUND,
		    e.host_name.in(),
		    e.process_name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::DuplicateProcessEntity& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_DUPLICATE_PROCESS_ENTITY,
		    e.process_name.in(),
		    e.entity_name.in(),
		    e.host_name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::DuplicateProcessService& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_DUPLICATE_PROCESS_SERVICE,
		    e.service_name.in(),
		    e.process_name.in(),
		    e.host_name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::DuplicateProcessStep& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_DUPLICATE_PROCESS_STEP,
		    e.step_label.in(),
		    e.process_name.in(),
		    e.host_name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::ReferenceNotFound& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_REFERENCE_NOT_FOUND,
		    e.name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::CircularReference& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_CIRCULAR_REFERENCE,
		    e.name.in(),
		    e.step );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::StepOutOfBound& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_STEP_OUT_OF_BOUND,
		    e.step,
		    e.name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::StepJump& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_STEP_JUMP,
		    e.name.in(),
		    e.origin_step,
		    e.target_step );

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

	void System::remove_application( const char* applicationName )
	{
	    try
	    {
		if( !check_only )
		    m_system->remove_application( applicationName );
	    }
	    catch( const CdmwPlatformMngt::ApplicationNotFound& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_APPLICATION_NOT_FOUND,
		    applicationName );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::ApplicationStillActive& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_APPLICATION_STILL_ACTIVE,
		    applicationName );

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

	void System::set_init_graph( const char* graphConfigFileName )
	{
	    // Open the graph configuration file
	    ConfigFileParser parser( graphConfigFileName );

	    try
	    {
		// Set the init graph sequence to the system onto the platform supervision
		ApplicationGraph applicationGraph( parser.get_init_graph() );
		CdmwPlatformMngt::GraphElements_var rootGraphElements
		    = applicationGraph.get_roots();

		if( !check_only )
		    m_system->set_init_graph( rootGraphElements.in() );
	    }
	    catch( const CdmwPlatformMngt::ReferenceNotFound& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_REFERENCE_NOT_FOUND,
		    e.name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::CircularReference& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_CIRCULAR_REFERENCE,
		    e.name.in(),
		    e.step );

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

	void System::set_stop_graph( const char* graphConfigFileName )
	{
	    // Open the graph configuration file
	    ConfigFileParser parser( graphConfigFileName );

	    try
	    {
		// Set the stop graph sequence to the system onto the platform supervision
		ApplicationGraph applicationGraph( parser.get_stop_graph() );
		CdmwPlatformMngt::GraphElements_var rootGraphElements
		    = applicationGraph.get_roots();

		if( !check_only )
		    m_system->set_stop_graph( rootGraphElements.in() );
	    }
	    catch( const CdmwPlatformMngt::ReferenceNotFound& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_REFERENCE_NOT_FOUND,
		    e.name.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::CircularReference& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_CIRCULAR_REFERENCE,
		    e.name.in(),
		    e.step );

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
	
	void System::add_property(const char* propertyConfigFileName) 
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
		m_system -> define_property_with_mode(prop->property_name.in(),prop->initial_value, prop_mode_type);
	    }
	    catch( const CdmwPlatformMngt::IncompatibleStatus& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_INCOMPATIBLE_STATUS );
		
		throw LoggedException();
	    }
	}
	
	void System::remove_property(const char* propertyConfigFileName)
	{
           try
            {
                ConfigFileParser parser( propertyConfigFileName );
                PropertyParser propertyparser = parser.get_property();
                CdmwPlatformMngt::PropertyDefs_var propertydefs ;
                CdmwPlatformMngt::PropertyDef_var prop = Property::get_definition(propertyparser);
		m_system -> delete_property (prop -> property_name);
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

