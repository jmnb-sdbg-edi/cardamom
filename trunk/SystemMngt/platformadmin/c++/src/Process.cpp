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
#include "SystemMngt/platformvaluetypes/ProcessDef_impl.hpp"

#include "platformadmin/Process.hpp"
#include "platformadmin/Service.hpp"
#include "platformadmin/ConfigFileParser.hpp"
#include "platformadmin/TimeoutParser.hpp"
#include "platformadmin/ServiceParser.hpp"
#include "platformadmin/ProcessParser.hpp"
#include "platformadmin/EntityParser.hpp"
#include "platformadmin/LifeCycleParser.hpp"
#include "platformadmin/AdminLogMessageId.hpp"
#include "platformadmin/Property.hpp"
#include "platformadmin/PropertyParser.hpp"



extern bool check_only;

namespace Cdmw {         // Begin namespace Cdmw
    namespace PlatformAdmin { // Begin namespace PlatformAdmin

	Process::Process( CdmwPlatformMngt::Process_ptr process )
	    : ServiceContainer( process ),
	      EntityContainer( process )
	{
	    if( !check_only )
		m_process = CdmwPlatformMngt::Process::_duplicate( process );
	}

	Process::~Process()
	{
	}

	CdmwPlatformMngt::ProcessSnapshot* Process::get_snapshot()
	{
	    CdmwPlatformMngt::ProcessSnapshot_var snapshot;
    
	    if( !check_only )
		snapshot = m_process->get_snapshot();

	    return snapshot._retn();
	}

	void Process::initialise( const CdmwPlatformMngtBase::StartupKind& startup_kind )
	{
	    try
	    {
		if( !check_only )
		    m_process->initialise( startup_kind );
	    }
	    catch( const CdmwPlatformMngt::IncompatibleStatus& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_INCOMPATIBLE_STATUS );

		throw LoggedException();
	    }
	}

	void Process::next_step()
	{
	    try
	    {
		if( !check_only )
		    m_process->next_step();
	    }
	    catch( const CdmwPlatformMngt::InvalidStep& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_INVALID_STEP );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::IncompatibleType& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_INCOMPATIBLE_TYPE );

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

	void Process::run()
	{
	    try
	    {
		if( !check_only )
		    m_process->run();
	    }
	    catch( const CdmwPlatformMngt::NotReadyToRun& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_NOT_READY_TO_RUN,
		    m_process->name() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::IncompatibleType& )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_INCOMPATIBLE_TYPE );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::IncompatibleStatus& )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_INCOMPATIBLE_STATUS );

		throw LoggedException();
	    }
	}

	void Process::stop( bool emergency )
	{
	    if( !check_only )
		m_process->stop( emergency );
	}

	CdmwPlatformMngt::ProcessDef* Process::get_definition(
	    const ProcessParser& processParser )
	{
	    try
	    {
		// Create the process definition
		CdmwPlatformMngt::ProcessDef_var processDef;

		if( processParser.type() == CdmwPlatformMngt::UNMANAGED_PROCESS )
		{
#if CDMW_ORB_VDR == tao
		    PlatformMngt::ProcessDefFactory processDefFactory;
		    processDef = processDefFactory.create(
			processParser.name().c_str() );
#else
		    CORBA::ValueFactoryBase_var processDefFactory
			= new PlatformMngt::ProcessDefFactory();
		    processDef = dynamic_cast< PlatformMngt::ProcessDefFactory* >(
			processDefFactory.in() )->create(
			    processParser.name().c_str() ); 
#endif
		}
		else
		{
#if CDMW_ORB_VDR == tao
		    PlatformMngt::ManagedProcessDefFactory managedProcessDefFactory;
		    processDef
			= managedProcessDefFactory.create(
			    processParser.name().c_str(),
			    processParser.type() );
#else
		    CORBA::ValueFactoryBase_var managedProcessDefFactory
			= new PlatformMngt::ManagedProcessDefFactory();

		    processDef
			= dynamic_cast< PlatformMngt::ManagedProcessDefFactory* >(
			    managedProcessDefFactory.in() )->create(
				processParser.name().c_str(),
				processParser.type() );
#endif
		}
		// Set the process definition
		processDef->auto_ending( processParser.is_autoended() );
		processDef->host_name( processParser.host_name().c_str() );

		// Set the process info
		CdmwPlatformMngt::ProcessInfo_var processInfo
		    = new CdmwPlatformMngt::ProcessInfo();

		processInfo->process_exe = CORBA::string_dup(
		    processParser.executable_path().c_str() );
		processInfo->process_port = processParser.port();
		processInfo->working_directory = CORBA::string_dup(
		    processParser.run_directory().c_str() );
		processInfo->process_args = CORBA::string_dup(
		    processParser.arguments().c_str() );

		processDef->process_info( processInfo.in() );
	
		//Add Properties to Process                                                                                                                               
		unsigned int propertyCount = processParser.get_property_count();
		processDef->properties().length( propertyCount );
                                                                                                                             
		for( unsigned int propIndex=0; propIndex<propertyCount; propIndex++)
		{
		    CdmwPlatformMngt::PropertyDef_var prop = Property::get_definition(processParser.get_property(propIndex));
		    processDef->properties()[propIndex] = prop.in();
		}
                   
		// Set the managed process definition
		if( processDef->type() != CdmwPlatformMngt::UNMANAGED_PROCESS )
		{
		    CdmwPlatformMngt::ManagedProcessDef* managedProcessDef
			= CdmwPlatformMngt::ManagedProcessDef::_downcast(processDef.in());

		    // Add all the entity definitions to the process definition
		    unsigned int entityCount = processParser.get_entity_count();
		    managedProcessDef->entities().length( entityCount );
		    for( unsigned int entityIndex=0;
			 entityIndex<entityCount;
			 entityIndex++ )
		    {
			// Create an EntityDef
			CdmwPlatformMngt::EntityDef_var entityDef = Entity::get_definition(
			    processParser.get_entity( entityIndex ) );
                       
			std::string newEntityName = entityDef->entity_name.in();

			// Checks there is no entity redefinition
			for( unsigned int l_entityIndex = 0;
			     l_entityIndex < entityIndex;
			     l_entityIndex++ )
			{
			    std::string entityName
				= managedProcessDef->entities()[ l_entityIndex ].entity_name.in();
                    
			    if( newEntityName == entityName )
				throw CdmwPlatformMngt::DuplicateProcessEntity(
				    managedProcessDef->name(),
				    managedProcessDef->host_name(),
				    newEntityName.c_str() );
			}

			managedProcessDef->entities()[ entityIndex ] =  entityDef.in();
		    }

		    // Add all the service definitions to the process definition
		    unsigned int serviceCount = processParser.get_service_count();
		    managedProcessDef->service_definitions().length( serviceCount );
		    for( unsigned int serviceIndex=0;
			 serviceIndex<serviceCount;
			 serviceIndex++ )
		    {
			CdmwPlatformMngtService::ServiceDef_var serviceDef
			    = Service::get_definition(
				processParser.get_service( serviceIndex ) );

			// Checks there is no service redefinition
			for( unsigned int l_serviceIndex = 0;
			     l_serviceIndex < serviceIndex;
			     l_serviceIndex++ )
			{
			    std::string serviceName =
				managedProcessDef->service_definitions()[ l_serviceIndex ].service_name.in();

			    if( serviceName == serviceDef->service_name.in() )
				throw CdmwPlatformMngt::DuplicateProcessService(
				    managedProcessDef->name(),
				    managedProcessDef->host_name(),
				    serviceDef->service_name.in() );
			}

			managedProcessDef->service_definitions()[ serviceIndex ]
			    = serviceDef.in();
		    }

		    // Set the life cycle
		    CdmwPlatformMngt::LifeCycleDef_var lifeCycleDef
			= new CdmwPlatformMngt::LifeCycleDef();
		    {
			// Add all the init step
			unsigned int initStepCount
			    = processParser.get_init_step_count();
			lifeCycleDef->init_steps.length( initStepCount );
			for( unsigned int initStepIndex=0;
			     initStepIndex<initStepCount;
			     initStepIndex++ )
			{
			    // Get an init step
			    CdmwPlatformMngt::Step_var initStep
				= new CdmwPlatformMngt::Step();
                        
			    initStep->step_label = CORBA::string_dup(
				processParser.get_init_step_name(
				    initStepIndex ).c_str() );
			    initStep->step_timeout
				= processParser.get_init_step_timeout( initStepIndex );

			    // Checks there is no step redefinition
			    for( unsigned int l_initStepIndex = 0;
				 l_initStepIndex < initStepIndex;
				 l_initStepIndex++ )
			    {
				std::string initStepName
				    = lifeCycleDef->init_steps[ l_initStepIndex ].step_label.in();
                        
				if( initStepName == initStep->step_label.in() )
				    throw CdmwPlatformMngt::DuplicateProcessStep(
					managedProcessDef->name(),
					managedProcessDef->host_name(),
					initStep->step_label.in() );
			    }

			    // Add this init step
			    lifeCycleDef->init_steps[ initStepIndex ] = initStep.in();
			}

			// Set the startup and stop timeout
			lifeCycleDef->run_timeout
			    = processParser.get_run_timeout();
			lifeCycleDef->stop_timeout
			    = processParser.get_stop_timeout();
		    }
            
		    managedProcessDef->life_cycle( lifeCycleDef.in() );

		    // Set the monitoring info
		    CdmwPlatformMngt::CompleteMonitoringInfo_var monitoringInfo
			= new CdmwPlatformMngt::CompleteMonitoringInfo();
            
		    try
		    {
			// get the monitoring parser
			// if not found exception has been raised
			ProcessMonitoringParser monitor_parser = 
			    processParser.get_monitoring();
                              
			monitoringInfo->monitoring_model = 
			    monitor_parser.model();
			monitoringInfo->pull_monitoring_interval =
			    monitor_parser.pull_interval();
			monitoringInfo->pull_monitoring_timeout =
			    monitor_parser.pull_timeout();
			monitoringInfo->push_monitoring_interval =
			    monitor_parser.push_interval();
			monitoringInfo->push_monitoring_timeout =
			    monitor_parser.push_timeout();
		    }
		    catch (const NotFoundException&)
		    {
			monitoringInfo->monitoring_model = CdmwPlatformMngt::NO_MONITORING;
			monitoringInfo->pull_monitoring_interval = 0;
			monitoringInfo->pull_monitoring_timeout  = 0;
			monitoringInfo->push_monitoring_interval = 0;
			monitoringInfo->push_monitoring_timeout  = 0;
		    }

		    managedProcessDef->monitoring_info( monitoringInfo.in() ); 

		}
		return processDef._retn();
	    }
	    catch( const std::bad_alloc& )
	    {
		CDMW_THROW( OutOfMemoryException );
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
	}

	void Process::set_info(
	    const char* process_exe,
	    CdmwPlatformMngt::Port process_port,
	    const char* working_directory,
	    const char* process_args )
	{
	    try
	    {
		// Set the process info
		CdmwPlatformMngt::ProcessInfo_var processInfo
		    = new CdmwPlatformMngt::ProcessInfo();

		processInfo->process_exe = CORBA::string_dup( process_exe );
		processInfo->process_port = process_port;
		processInfo->working_directory = CORBA::string_dup( working_directory);
		processInfo->process_args = CORBA::string_dup( process_args );

		if( !check_only )
		    m_process->set_process_info( processInfo.in() );
	    }
	    catch( const CdmwPlatformMngt::IncompatibleStatus& )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_INCOMPATIBLE_STATUS );

		throw LoggedException();
	    }
	}

	void Process::set_autoending(bool autoending)
	{
	    try
	    {
		if( !check_only )
		{
		    CORBA::Boolean flag;
        	
		    if (autoending)
		    {
        		flag = 1;
		    }
		    else
		    {
			flag = 0;
		    }
            
		    m_process->set_auto_ending(flag);
		}
	    }
	    catch( const CdmwPlatformMngt::IncompatibleStatus& )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_INCOMPATIBLE_STATUS );

		throw LoggedException();
	    }
	}


	void Process::set_life_cycle( const char* lifeCycleFileName )
	{
	    ConfigFileParser parser( lifeCycleFileName );
	    LifeCycleParser lifeCycleParser( parser.get_life_cycle() );

	    try
	    {

		CdmwPlatformMngt::LifeCycleDef_var lifeCycleDef
		    = new CdmwPlatformMngt::LifeCycleDef();

		// Add all the init step
		unsigned int initStepCount
		    = lifeCycleParser.get_init_step_count();

		lifeCycleDef->init_steps.length( initStepCount );
		for( unsigned int initStepIndex=0;
		     initStepIndex<initStepCount;
		     initStepIndex++ )
		{
		    // Get an init step
		    CdmwPlatformMngt::Step_var initStep
			= new CdmwPlatformMngt::Step();
                
		    initStep->step_label = CORBA::string_dup(
			lifeCycleParser.get_init_step_name(
			    initStepIndex ).c_str() );
		    initStep->step_timeout
			= lifeCycleParser.get_init_step_timeout( initStepIndex );

		    // Checks there is no step redefinition
		    for( unsigned int l_initStepIndex = 0;
			 l_initStepIndex < initStepIndex;
			 l_initStepIndex++ )
		    {
			std::string initStepName
			    = lifeCycleDef->init_steps[ l_initStepIndex ].step_label.in();
                
			if( initStepName == initStep->step_label.in() )
			    throw CdmwPlatformMngt::DuplicateStep(
				initStep->step_label.in() );
		    }

		    // Add this init step
		    lifeCycleDef->init_steps[ initStepIndex ] = initStep.in();
		}

		// Set the startup and stop timeout
		lifeCycleDef->run_timeout = lifeCycleParser.get_run_timeout();
		lifeCycleDef->stop_timeout = lifeCycleParser.get_stop_timeout();

		// Set the life cycle
		if( !check_only )
		    m_process->set_life_cycle( lifeCycleDef.in() );
	    }
	    catch( const CdmwPlatformMngt::DuplicateStep& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_DUPLICATE_STEP,
		    e.step_label.in() );

		throw LoggedException();
	    }
	    catch( const CdmwPlatformMngt::IncompatibleType& )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_INCOMPATIBLE_TYPE );

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

      	void Process::add_property(const char* propertyConfigFileName) 
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
		m_process -> define_property_with_mode(prop->property_name.in(),prop->initial_value, prop_mode_type);
	    }
	    catch( const CdmwPlatformMngt::IncompatibleStatus& e )
	    {
		PlatformMngt::LogMngr::logMessage(
		    PlatformMngt::ERR,
		    MSG_ID_ADMIN_INCOMPATIBLE_STATUS );
		
		throw LoggedException();
	    }
	}
	

	void Process::remove_property(const char* propertyConfigFileName)
	{
	    try
            {
                ConfigFileParser parser( propertyConfigFileName );
                PropertyParser propertyparser = parser.get_property();
                CdmwPlatformMngt::PropertyDef_var prop = Property::get_definition(propertyparser);
		m_process -> delete_property (prop -> property_name);
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

