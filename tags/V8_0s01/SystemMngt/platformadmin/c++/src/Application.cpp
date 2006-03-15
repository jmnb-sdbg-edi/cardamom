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

#include "platformadmin/Application.hpp"
#include "platformadmin/Process.hpp"
#include "platformadmin/Service.hpp"
#include "platformadmin/Graph.hpp"
#include "platformadmin/ConfigFileParser.hpp"
#include "platformadmin/MonitoringParser.hpp"
#include "platformadmin/ServiceParser.hpp"
#include "platformadmin/ApplicationParser.hpp"
#include "platformadmin/ProcessParser.hpp"
#include "platformadmin/EntityParser.hpp"
#include "platformadmin/GraphParser.hpp"
#include "platformadmin/AdminLogMessageId.hpp"
#include "platformadmin/Property.hpp"
#include "platformadmin/PropertyParser.hpp"

extern bool check_only;

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

Application::Application(
    CdmwPlatformMngt::Application_ptr application )
: ServiceContainer( application ),
  EntityContainer( application )
{
    if( !check_only )
        m_application = CdmwPlatformMngt::Application::_duplicate(
            application );
}

Application::~Application()
{
}

CdmwPlatformMngt::ApplicationSnapshot* Application::get_snapshot()
{
    CdmwPlatformMngt::ApplicationSnapshot_var snapshot;

    if( !check_only )
        snapshot = m_application->get_snapshot();

    return snapshot._retn();
}

void Application::initialise(
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
{
    try
    {
        if( !check_only )
            m_application->initialise( startup_kind );
    }
    catch( const CdmwPlatformMngt::IncompatibleStatus& e )
    {
        PlatformMngt::LogMngr::logMessage(
            PlatformMngt::ERR,
            MSG_ID_ADMIN_INCOMPATIBLE_STATUS );

        throw LoggedException();
    }
}

CdmwPlatformMngt::LongRequestStatus Application::initialise_and_wait(
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
{
    CdmwPlatformMngt::LongRequestStatus requestStatus =
        CdmwPlatformMngt::LONG_REQUEST_NOT_COMPLETED;

    try
    {
        if( !check_only )
            requestStatus = m_application->initialise_and_wait( startup_kind );
    }
    catch( const CdmwPlatformMngt::IncompatibleStatus& e )
    {
        PlatformMngt::LogMngr::logMessage(
            PlatformMngt::ERR,
            MSG_ID_ADMIN_INCOMPATIBLE_STATUS );

        throw LoggedException();
    }

    return requestStatus;
}

void Application::run()
{
    try
    {
        if( !check_only )
            m_application->run();
    }
    catch( const CdmwPlatformMngt::IncompatibleStatus& e )
    {
        PlatformMngt::LogMngr::logMessage(
            PlatformMngt::ERR,
            MSG_ID_ADMIN_INCOMPATIBLE_STATUS );

        throw LoggedException();
    }
}

void Application::stop(
    bool emergency )
{
    if( !check_only )
        m_application->stop( emergency );
}

void Application::resume()
{
    try
    {
        if( !check_only )
            m_application->resume();
    }
    catch( const CdmwPlatformMngt::IncompatibleStatus& e )
    {
        PlatformMngt::LogMngr::logMessage(
            PlatformMngt::ERR,
            MSG_ID_ADMIN_INCOMPATIBLE_STATUS );

        throw LoggedException();
    }
}

CdmwPlatformMngt::LongRequestStatus Application::resume_and_wait()
{
    CdmwPlatformMngt::LongRequestStatus requestStatus =
	CdmwPlatformMngt::LONG_REQUEST_NOT_COMPLETED;

    try
    {
        if( !check_only )
            requestStatus = m_application->resume_and_wait();
    }
    catch( const CdmwPlatformMngt::IncompatibleStatus& e )
    {
        PlatformMngt::LogMngr::logMessage(
            PlatformMngt::ERR,
            MSG_ID_ADMIN_INCOMPATIBLE_STATUS );

        throw LoggedException();
    }
    
    return requestStatus;
}

void Application::acknowledge()
{
    try
    {
        if( !check_only )
            m_application->acknowledge();
    }
    catch( const CdmwPlatformMngt::IncompatibleStatus& e )
    {
        PlatformMngt::LogMngr::logMessage(
            PlatformMngt::ERR,
            MSG_ID_ADMIN_INCOMPATIBLE_STATUS );

        throw LoggedException();
    }
}

CdmwPlatformMngt::ApplicationDef* Application::get_definition(
    const ApplicationParser& applicationParser,
    CdmwPlatformMngt::PlatformApplicationDef_out platformApplicationDef )
{
    try
    {
         
	// Create an ApplicationDef
        CdmwPlatformMngt::ApplicationDef_var applicationDef
            = new CdmwPlatformMngt::ApplicationDef();


        // Add all the managed process definitions to the application definition
        unsigned int managedProcessCount = applicationParser.get_managed_process_count();

	applicationDef->processes.length( managedProcessCount );

        for( unsigned int managedProcessIndex = 0;
             managedProcessIndex < managedProcessCount;
             managedProcessIndex++ )
        {
	
            CdmwPlatformMngt::ProcessDef_var managedProcessDef
                = Process::get_definition( applicationParser.get_managed_process(
                    managedProcessIndex ) );
            // Checks there is no managed process redefinition
            for( unsigned int l_managedProcessIndex = 0;
                 l_managedProcessIndex < managedProcessIndex;
                 l_managedProcessIndex++ )
            {
                std::string managedProcessName =
                     applicationDef->processes[ l_managedProcessIndex ]->name();

                if( managedProcessName == managedProcessDef->name() )
                    throw CdmwPlatformMngt::DuplicateProcess(
                        managedProcessDef->name(),
                        managedProcessDef->host_name() );
            }
	    applicationDef->processes[ managedProcessIndex ] = managedProcessDef;

	}

        // Add all the unmanaged process definitions to the application definition
        unsigned int unmanagedProcessCount = applicationParser.get_unmanaged_process_count();
        applicationDef->processes.length( managedProcessCount + unmanagedProcessCount );
        for( unsigned int unmanagedProcessIndex = 0;
             unmanagedProcessIndex < unmanagedProcessCount;
             unmanagedProcessIndex++ )
        {
            CdmwPlatformMngt::ProcessDef_var unmanagedProcessDef
                = Process::get_definition( applicationParser.get_unmanaged_process(
                    unmanagedProcessIndex ) );

            // Checks there is no unmanaged process redefinition
            for( unsigned int l_unmanagedProcessIndex = 0;
                 l_unmanagedProcessIndex < managedProcessCount + unmanagedProcessIndex;
                 l_unmanagedProcessIndex++ )
            {
                std::string unmanagedProcessName =
                     applicationDef->processes[ l_unmanagedProcessIndex ]->name();
                if( unmanagedProcessName == unmanagedProcessDef->name() )
                    throw CdmwPlatformMngt::DuplicateProcess(
                        unmanagedProcessDef->name(),
                        unmanagedProcessDef->host_name() );
            }

            applicationDef->processes[ managedProcessCount + unmanagedProcessIndex ]
                = unmanagedProcessDef;
        }

        // Add all the entity definitions to the application definition
        unsigned int entityCount = applicationParser.get_entity_count();
        applicationDef->entities.length( entityCount );
        for( unsigned int entityIndex=0; entityIndex<entityCount; entityIndex++ )
        {
        	// Create an EntityDef
        	CdmwPlatformMngt::EntityDef_var entityDef = Entity::get_definition(
                applicationParser.get_entity( entityIndex ) );
        	
            std::string newEntityName = entityDef->entity_name.in();

            // Checks there is no entity redefinition
            for( unsigned int l_entityIndex = 0;
                 l_entityIndex < entityIndex;
                 l_entityIndex++ )
            {
                std::string entityName
                    = applicationDef->entities[ l_entityIndex ].entity_name.in();
                
                if( newEntityName == entityName )
                    throw CdmwPlatformMngt::DuplicateEntity(
                        newEntityName.c_str() );
            }

            applicationDef->entities[ entityIndex ] =  entityDef.in();
        }

        // Add all the service definitions to the application definition
        unsigned int serviceCount = applicationParser.get_service_count();
        applicationDef->service_definitions.length( serviceCount );
        for( unsigned int serviceIndex=0;
             serviceIndex<serviceCount;
             serviceIndex++ )
        {
            CdmwPlatformMngtService::ServiceDef_var serviceDef
                = Service::get_definition(
                    applicationParser.get_service( serviceIndex ) );

            // Checks there is no service redefinition
            for( unsigned int l_serviceIndex = 0;
                 l_serviceIndex < serviceIndex;
                 l_serviceIndex++ )
            {
                std::string serviceName =
                     applicationDef->service_definitions[ l_serviceIndex ].service_name.in();

                if( serviceName == serviceDef->service_name.in() )
                    throw CdmwPlatformMngt::DuplicateService(
                        serviceDef->service_name.in() );
            }

            applicationDef->service_definitions[ serviceIndex ] = serviceDef.in();
        }

        // Set the platform application definition
        CdmwPlatformMngt::PlatformApplicationDef_var l_platformApplicationDef
            = new CdmwPlatformMngt::PlatformApplicationDef();

        l_platformApplicationDef->application_name
            = CORBA::string_dup( applicationParser.name().c_str() );
        l_platformApplicationDef->host_name
            = CORBA::string_dup( applicationParser.host_name().c_str() );

        // Set the application process info
        CdmwPlatformMngt::ProcessLightInfo_var applicationProcessInfo
            = new CdmwPlatformMngt::ProcessLightInfo();

        applicationProcessInfo->process_port
            = applicationParser.app_port();
        applicationProcessInfo->working_directory
            = CORBA::string_dup( applicationParser.app_run_dir().c_str() );
        applicationProcessInfo->process_args
            = CORBA::string_dup( applicationParser.app_args().c_str() );

        l_platformApplicationDef->platform_application_info = applicationProcessInfo.in();
        
        // Set the monitoring info
        CdmwPlatformMngt::MonitoringInfo_var monitoringInfo
            = new CdmwPlatformMngt::MonitoringInfo();

        monitoringInfo->monitoring_interval
            = applicationParser.get_monitoring().pull_interval();
        monitoringInfo->monitoring_timeout
            = applicationParser.get_monitoring().pull_timeout();

        l_platformApplicationDef->monitoring_info = monitoringInfo.in();

        // Set the out parameter
        platformApplicationDef = l_platformApplicationDef._retn();

	// Set the Properties for Application 

	 
        unsigned int propertyCount = applicationParser.get_property_count();
        applicationDef->properties.length( propertyCount );
                                               	                                                        
	for( unsigned int propIndex=0; propIndex<propertyCount; propIndex++)
        {

		CdmwPlatformMngt::PropertyDef_var prop = Property::get_definition(applicationParser.get_property(propIndex));
   	        applicationDef->properties[propIndex] = prop;
        }
	
        return applicationDef._retn();
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
    catch( const CdmwPlatformMngt::DuplicateProcess& e )
    {
        PlatformMngt::LogMngr::logMessage(
            PlatformMngt::ERR,
            MSG_ID_ADMIN_DUPLICATE_PROCESS,
            e.process_name.in(),
            e.host_name.in(),
            applicationParser.name().c_str() );

        throw LoggedException();
    }
    catch( const CdmwPlatformMngt::DuplicateEntity& e )
    {
        PlatformMngt::LogMngr::logMessage(
            PlatformMngt::ERR,
            MSG_ID_ADMIN_DUPLICATE_APPLICATION_ENTITY,
            e.entity_name.in(),
            applicationParser.name().c_str() );

        throw LoggedException();
    }
    catch( const CdmwPlatformMngt::DuplicateService& e )
    {
        PlatformMngt::LogMngr::logMessage(
            PlatformMngt::ERR,
            MSG_ID_ADMIN_DUPLICATE_APPLICATION_SERVICE,
            e.service_name.in(),
            applicationParser.name().c_str() );

        throw LoggedException();
    }
}

void Application::add_process(
    const char* processConfigFileName )
{
    // Open the process configuration file
    ConfigFileParser parser( processConfigFileName );

    // Add a process definition
    CdmwPlatformMngt::ProcessDef_var processDef
        = Process::get_definition( parser.get_process() );

    try
    {
        if( !check_only )
            m_application->add_process( processDef );
    }
    catch( const CdmwPlatformMngt::ProcessAlreadyExists& e )
    {
        PlatformMngt::LogMngr::logMessage(
            PlatformMngt::ERR,
            MSG_ID_ADMIN_PROCESS_ALREADY_EXISTS,
            processDef->name() );

        throw LoggedException();
    }
    catch( const CdmwPlatformMngt::HostNotFound& e )
    {
        PlatformMngt::LogMngr::logMessage(
            PlatformMngt::ERR,
            MSG_ID_ADMIN_HOST_NOT_FOUND,
            processDef->host_name() );

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
    catch( const CdmwPlatformMngt::DuplicateStep& e )
    {
        PlatformMngt::LogMngr::logMessage(
            PlatformMngt::ERR,
            MSG_ID_ADMIN_DUPLICATE_STEP,
            e.step_label.in() );

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

void Application::remove_process(
    const char* processName,
    const char* hostName )
{
    try
    {
        if( !check_only )
            m_application->remove_process( processName, hostName );
    }
    catch( const CdmwPlatformMngt::ProcessNotFound& e )
    {
        PlatformMngt::LogMngr::logMessage(
            PlatformMngt::ERR,
            MSG_ID_ADMIN_PROCESS_NOT_FOUND,
            processName );

        throw LoggedException();
    }
    catch( const CdmwPlatformMngt::ProcessStillActive& e )
    {
        PlatformMngt::LogMngr::logMessage(
            PlatformMngt::ERR,
            MSG_ID_ADMIN_PROCESS_STILL_ACTIVE,
            processName );

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

void Application::set_init_graph(
    const char* graphConfigFileName )
{
    // Open the graph configuration file
    ConfigFileParser parser( graphConfigFileName );

    try
    {
        // Set the init process graph sequence of the
        // application onto the platform supervision
        ProcessGraph processGraph( parser.get_init_graph(), false );
        CdmwPlatformMngt::GraphElements_var rootGraphElements
            = processGraph.get_roots();

        if( !check_only )
            m_application->set_init_graph( rootGraphElements.in() );
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

void Application::set_stop_graph(
    const char* graphConfigFileName )
{
    // Open the graph configuration file
    ConfigFileParser parser( graphConfigFileName );

    try
    {
        // Set the stop process graph sequence of the
        // application onto the platform supervision
        ProcessGraph processGraph( parser.get_stop_graph(), true );
        CdmwPlatformMngt::GraphElements_var rootGraphElements
            = processGraph.get_roots();

        if( !check_only )
            m_application->set_stop_graph( rootGraphElements.in() );
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


	void Application::add_property(const char* propertyConfigFileName) 
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
		m_application -> define_property_with_mode(prop->property_name.in(),prop->initial_value, prop_mode_type);
                
	    }
	    catch( const CdmwPlatformMngt::IncompatibleStatus& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_INCOMPATIBLE_STATUS );
		
		throw LoggedException();
	    }
	}

	void Application::remove_property(const char* propertyConfigFileName)
	{
           try
            {
                ConfigFileParser parser( propertyConfigFileName );
                PropertyParser propertyparser = parser.get_property();
                CdmwPlatformMngt::PropertyDef_var prop = Property::get_definition(propertyparser);
		m_application -> delete_property (prop -> property_name);
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

