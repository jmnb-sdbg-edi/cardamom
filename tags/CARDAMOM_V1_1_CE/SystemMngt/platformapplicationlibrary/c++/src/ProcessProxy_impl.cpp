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


#include "platformapplicationlibrary/ProcessProxy_impl.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtDaemon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"
#include "SystemMngt/platformvaluetypes/EntityStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ProcessStatusChange_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/ApplicationLogMessageId.hpp"
#include <set>

namespace Cdmw
{
namespace PlatformMngt
{

ProcessProxy_impl::ProcessProxy_impl(
    PortableServer::POA_ptr poa,
    CdmwPlatformMngt::ProcessDef* process_def,
    Application_impl* application )
: DeactivableServant_impl( poa )
{
    if( application == NULL )
        CDMW_THROW2( BadParameterException, "application", "NULL" );

    m_procEntityStatusChangeFactory = NULL;
    m_procStatusChangeFactory = NULL;
    
    try
    {
        // Sets the process proxy's internal data
        std::auto_ptr< ProcessEntityStatusChangeFactory > procEntityStatusChangeFactory(
            new ProcessEntityStatusChangeFactory() );
        std::auto_ptr< ProcessStatusChangeFactory > procStatusChangeFactory(
            new ProcessStatusChangeFactory() );
            
        m_procEntityStatusChangeFactory = procEntityStatusChangeFactory.get();
        m_procStatusChangeFactory = procStatusChangeFactory.get();
        
        m_application = application;
        m_name = CORBA::string_dup(process_def->name());
        m_type = process_def->type();
        m_autoending = process_def->autoending();
        m_host = CdmwPlatformMngt::HostProxy::_nil();
        
        m_process_info = new CdmwPlatformMngt::ProcessInfo();
            m_process_info->process_exe = CORBA::string_dup( "" );
            m_process_info->process_port = 0;
            m_process_info->working_directory = CORBA::string_dup( "" );
            m_process_info->process_args = CORBA::string_dup( "" );
            
        m_lifeCycle = new CdmwPlatformMngt::LifeCycleDef();
            m_lifeCycle->init_steps.length( 0 );
            m_lifeCycle->run_timeout = 0;
            m_lifeCycle->stop_timeout = 0;
            
        m_monitoring_info = new CdmwPlatformMngt::CompleteMonitoringInfo();
            m_monitoring_info->monitoring_model = CdmwPlatformMngt::NO_MONITORING;
            m_monitoring_info->pull_monitoring_interval = 0;
            m_monitoring_info->pull_monitoring_timeout = 0;
            m_monitoring_info->push_monitoring_interval = 0;
            m_monitoring_info->push_monitoring_timeout = 0;
            
        m_activity_point_defs = new CdmwPlatformMngt::ActivityPointDefs();
        m_status_info = CORBA::string_dup( "" );
        m_stepCounter = 0;

        setProcessInfo( process_def->process_info() );

        // Release the ownership of the temporary objects
        procEntityStatusChangeFactory.release();
        procStatusChangeFactory.release();
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
}


ProcessProxy_impl::~ProcessProxy_impl()
{
    if( m_procEntityStatusChangeFactory != NULL )
    {
        m_procEntityStatusChangeFactory->_remove_ref();
    }

    if( m_procStatusChangeFactory != NULL )
    {
        m_procStatusChangeFactory->_remove_ref();
    }
    
#ifdef CDMW_USE_FAULTTOLERANCE
    // unregister the process observer from the ftconverter
    std::string applicationname(m_application->get_applicationName());
    std::string processname(get_processName());
    std::string appliProcName = applicationname + "-" + processname;
    m_application->m_ftConverter->unregister_process_observer(appliProcName.c_str());
#endif
}



CdmwPlatformMngt::ProcessName ProcessProxy_impl::name()
throw(  CORBA::SystemException )
{
    CORBA::String_var name = m_name;

    return name._retn();
}

CdmwPlatformMngt::ProcessType ProcessProxy_impl::type()
throw(  CORBA::SystemException )
{
    return m_type;
}

CORBA::Object_ptr ProcessProxy_impl::embedded_service()
throw(  CORBA::SystemException )
{
    return CORBA::Object::_nil();
}

CORBA::Boolean ProcessProxy_impl::is_autoending()
throw(  CORBA::SystemException )
{
    return m_autoending;
}

CdmwPlatformMngt::HostProxy_ptr ProcessProxy_impl::get_host()
throw(  CORBA::SystemException )
{
    CdmwPlatformMngt::HostProxy_var hostProxy = m_host;
        
    return hostProxy._retn();
}

CdmwPlatformMngt::ProcessInfo* ProcessProxy_impl::get_process_info()
throw(  CORBA::SystemException )
{
    try
    {
        CdmwPlatformMngt::ProcessInfo_var processInfo
            = new CdmwPlatformMngt::ProcessInfo( m_process_info.in() );

        return processInfo._retn();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

CdmwPlatformMngt::LifeCycleDef* ProcessProxy_impl::get_life_cycle()
throw( CdmwPlatformMngt::IncompatibleType,
       CORBA::SystemException )
{
    throw CdmwPlatformMngt::IncompatibleType();
}

void ProcessProxy_impl::set_life_cycle(
    const CdmwPlatformMngt::LifeCycleDef& life_cycle )
throw( CdmwPlatformMngt::DuplicateStep,
       CdmwPlatformMngt::IncompatibleType,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    throw CdmwPlatformMngt::IncompatibleType();
}

CdmwPlatformMngt::CompleteMonitoringInfo ProcessProxy_impl::get_monitoring_info()
throw( CdmwPlatformMngt::IncompatibleType,
       CORBA::SystemException )
{
    throw CdmwPlatformMngt::IncompatibleType();
}

void ProcessProxy_impl::set_monitoring_info(
    const CdmwPlatformMngt::CompleteMonitoringInfo& monitoring_info)
throw( CdmwPlatformMngt::IncompatibleType,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    throw CdmwPlatformMngt::IncompatibleType();
}

CdmwPlatformMngt::ActivityPointDefs*
ProcessProxy_impl::get_activity_point_defs()
throw( CdmwPlatformMngt::IncompatibleType,
       CORBA::SystemException )
{
    throw CdmwPlatformMngt::IncompatibleType();
}

void ProcessProxy_impl::set_activity_point_defs(
    const CdmwPlatformMngt::ActivityPointDefs& point_defs )
throw( CdmwPlatformMngt::DuplicateActivityPoint,
       CdmwPlatformMngt::IncompatibleType,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    throw CdmwPlatformMngt::IncompatibleType();
}

CdmwPlatformMngt::ProcessSnapshot* ProcessProxy_impl::get_snapshot()
throw(  CORBA::SystemException )
{
    try
    {
        CdmwPlatformMngtEntity::Entities_var entities
            = new CdmwPlatformMngtEntity::Entities();
        CdmwPlatformMngt::ActivityPoints_var points
            = new CdmwPlatformMngt::ActivityPoints();
        
        // If the process is managed
        if( type() != CdmwPlatformMngt::UNMANAGED_PROCESS )
        {
            // Gets all the entities
            CdmwPlatformMngtEntity::EntitiesIterator_var entitiesIt;

            get_all_entities(
                get_number_of_entities(),
                entities.out(),
                entitiesIt.out() );

            /*
            // Gets all the monitoring points
            CdmwPlatformMngt::ActivityPointsIterator_var pointsIt;

            get_all_activity_points(
                get_number_of_activity_points(),
                points.out(),
                pointsIt.out() );
            */
        }

        // Create a process snapshot
        CORBA::String_var statusInfo;
        CdmwPlatformMngt::ProcessSnapshot_var snapshot
            = new CdmwPlatformMngt::ProcessSnapshot;

        snapshot->process_name = CORBA::string_dup( m_name.in() );
        snapshot->process_status = get_status( statusInfo );
        snapshot->process_status_info = statusInfo;
        snapshot->process_entities = entities.in();
        snapshot->activity_points = points.in();


        return snapshot._retn();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

CdmwPlatformMngt::RequestStatus ProcessProxy_impl::next_step()
throw( CdmwPlatformMngt::InvalidStep,
       CdmwPlatformMngt::IncompatibleType,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    throw CdmwPlatformMngt::IncompatibleType();
}

CdmwPlatformMngt::RequestStatus ProcessProxy_impl::run()
throw( CdmwPlatformMngt::NotReadyToRun,
       CdmwPlatformMngt::IncompatibleType,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    throw CdmwPlatformMngt::IncompatibleType();
}

CORBA::ULong ProcessProxy_impl::get_number_of_entities()
throw( CORBA::SystemException )
{
    throw CORBA::BAD_OPERATION(
        OrbSupport::BAD_OPERATION,
        CORBA::COMPLETED_NO );
}

void ProcessProxy_impl::get_all_entity_names(
    CORBA::ULong how_many,
    CdmwPlatformMngtEntity::EntityNames_out entity_names,
    CdmwPlatformMngtEntity::EntityNamesIterator_out rest )
throw( CORBA::SystemException )
{
    throw CORBA::BAD_OPERATION(
        OrbSupport::BAD_OPERATION,
        CORBA::COMPLETED_NO );
}

CdmwPlatformMngtEntity::EntityStatus ProcessProxy_impl::get_entity_status(
    const char* entity_name,
    CORBA::String_out entity_info )
throw( CdmwPlatformMngtEntity::EntityNotFound, CORBA::SystemException )
{
    throw CORBA::BAD_OPERATION(
        OrbSupport::BAD_OPERATION,
        CORBA::COMPLETED_NO );
}

CORBA::Boolean ProcessProxy_impl::get_entities(
    const CdmwPlatformMngtEntity::EntityNames& entity_names,
    CdmwPlatformMngtEntity::Entities_out nentities )
throw( CORBA::SystemException )
{
    throw CORBA::BAD_OPERATION(
        OrbSupport::BAD_OPERATION,
        CORBA::COMPLETED_NO );
}

void ProcessProxy_impl::get_all_entities(
    CORBA::ULong how_many,
    CdmwPlatformMngtEntity::Entities_out nentities,
    CdmwPlatformMngtEntity::EntitiesIterator_out rest )
throw( CORBA::SystemException )
{
    throw CORBA::BAD_OPERATION(
        OrbSupport::BAD_OPERATION,
        CORBA::COMPLETED_NO );
}

void ProcessProxy_impl::add_entity(
    const char* entity_name )
throw( CdmwPlatformMngtEntity::EntityAlreadyExists, CORBA::SystemException )
{
    throw CORBA::BAD_OPERATION(
        OrbSupport::BAD_OPERATION,
        CORBA::COMPLETED_NO );
}

void ProcessProxy_impl::remove_entity(
    const char* entity_name )
throw( CdmwPlatformMngtEntity::EntityNotFound, CORBA::SystemException )
{
    throw CORBA::BAD_OPERATION(
        OrbSupport::BAD_OPERATION,
        CORBA::COMPLETED_NO );
}

CdmwPlatformMngtService::ServiceProviderID*
ProcessProxy_impl::get_service_def(
    const char* service_name )
throw( CdmwPlatformMngtService::ServiceNotFound, CORBA::SystemException )
{
    throw CORBA::BAD_OPERATION(
        OrbSupport::BAD_OPERATION,
        CORBA::COMPLETED_NO );
}

void ProcessProxy_impl::add_service_def(
    const CdmwPlatformMngtService::ServiceDef& service_def )
throw( CdmwPlatformMngtService::ServiceAlreadyExists, CORBA::SystemException )
{
    throw CORBA::BAD_OPERATION(
        OrbSupport::BAD_OPERATION,
        CORBA::COMPLETED_NO );
}

void ProcessProxy_impl::remove_service_def(
    const char* service_name )
throw( CdmwPlatformMngtService::ServiceNotFound, CORBA::SystemException )
{
    throw CORBA::BAD_OPERATION(
        OrbSupport::BAD_OPERATION,
        CORBA::COMPLETED_NO );
}

CORBA::ULong ProcessProxy_impl::get_number_of_properties()
throw(  CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );
}

void ProcessProxy_impl::get_all_property_names(
    CORBA::ULong how_many,
    CosPropertyService::PropertyNames_out property_names,
    CosPropertyService::PropertyNamesIterator_out rest )
throw(  CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );
}

CORBA::Any* ProcessProxy_impl::get_property_value(
    const char* property_name )
throw( CosPropertyService::PropertyNotFound,
       CosPropertyService::InvalidPropertyName,
       CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );
}

CORBA::Boolean ProcessProxy_impl::get_properties(
    const CosPropertyService::PropertyNames& property_names,
    CosPropertyService::Properties_out nproperties )
throw(  CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );
}

void ProcessProxy_impl::get_all_properties(
    CORBA::ULong how_many,
    CosPropertyService::Properties_out nproperties,
    CosPropertyService::PropertiesIterator_out rest )
throw(  CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );
}

void ProcessProxy_impl::set_property(
    const char* property_name,
    const CORBA::Any& property_value )
throw(  CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ProcessProxy_impl::remove_property(
    const char* property_name )
throw( CosPropertyService::PropertyNotFound,
       CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ProcessProxy_impl::is_alive()
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );
}

CORBA::ULong ProcessProxy_impl::get_number_of_activity_points()
throw(  CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );
}

void ProcessProxy_impl::get_all_activity_point_names(
    CORBA::ULong how_many,
    CdmwPlatformMngt::ActivityPointNames_out point_names,
    CdmwPlatformMngt::ActivityPointNamesIterator_out rest )
throw(  CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );
}

CdmwPlatformMngt::ActivityPointStatus
ProcessProxy_impl::get_activity_point_status(
    const char* point_name)
throw( CdmwPlatformMngt::ActivityPointNotFound,
       CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );
}

CORBA::Boolean ProcessProxy_impl::get_activity_points(
    const CdmwPlatformMngt::ActivityPointNames& point_names,
    CdmwPlatformMngt::ActivityPoints_out points )
throw(  CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );
}

void ProcessProxy_impl::get_all_activity_points(
    CORBA::ULong how_many,
    CdmwPlatformMngt::ActivityPoints_out points,
    CdmwPlatformMngt::ActivityPointsIterator_out rest )
throw(  CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );
}

CdmwPlatformMngt::ApplicationAgent_ptr
ProcessProxy_impl::getApplicationAgent()
throw( CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::HostNotReachable,
       CdmwPlatformMngt::CreationError,
       CdmwPlatformMngt::ApplicationAgentAlreadyExists,
       CdmwPlatformMngt::InvalidInfo,
       CdmwPlatformMngt::CreationTimeout,
       AgentNotFoundException,
       CORBA::SystemException )
{
    CdmwPlatformMngt::ApplicationAgent_var agent;
    
    CORBA::String_var processHostName = m_host->name();
    
    // Get the application agent on the host of this process
    // if does not exist start a new one
    agent = m_application->getAgent (processHostName.in());

    return agent._retn();
}




void ProcessProxy_impl::setAutoending()
{
    m_autoending = 1;
}

void ProcessProxy_impl::notifyEntityStatusChangeEvent(
    const char* entity_name,
    CdmwPlatformMngtEntity::EntityStatus entity_status,
    const char* entity_info )
{
    try
    {
        CORBA::String_var processHostName = m_host->name();
        
        CdmwPlatformMngtBase::EventHeader header
            = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

        CdmwPlatformMngt::ProcessEntityStatusChange_var event
            = m_procEntityStatusChangeFactory->create(
                header,
                m_application->m_applicationAck->system_name.in(),
                m_application->get_applicationName(),
                get_processName(),
                processHostName.in(),
                entity_name,
                entity_status,
                entity_info );

        CORBA::String_var eventStr = event->to_string();
        LogMngr::logMessage(eventStr.in());

        m_application->m_eventNotifier->addEvent( event );
        event._retn();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

void ProcessProxy_impl::setHost( const char* host_name)
throw( CdmwPlatformMngt::HostNotFound,
       CORBA::SystemException )
{
    m_host = m_application->m_hostContainer->get_host( host_name );
}

void ProcessProxy_impl::setProcessInfo(
    const CdmwPlatformMngt::ProcessInfo& process_info )
{
    try
    {
        m_process_info = new CdmwPlatformMngt::ProcessInfo( process_info );
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

void ProcessProxy_impl::setMonitoringInfo(
    const CdmwPlatformMngt::CompleteMonitoringInfo& monitoring_info )
{
    try
    {
        m_monitoring_info = new CdmwPlatformMngt::CompleteMonitoringInfo(
            monitoring_info );
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

void ProcessProxy_impl::errorEvent(
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* issuer,
    const char* error_info )
{
    // Do nothing if in a failed status
    CdmwPlatformMngt::ProcessStatus status = getStatus();

    if( (status == CdmwPlatformMngt::PROCESS_FAILED_ERROR) ||
        (status == CdmwPlatformMngt::PROCESS_FAILED_TIMEOUT) ||
        (status == CdmwPlatformMngt::PROCESS_FAILED_DISCREPANCY) ||
        (status == CdmwPlatformMngt::PROCESS_FAILED_INVALID) ||
        (status == CdmwPlatformMngt::PROCESS_FAILED_NO_RESPONSE) ||
        (status == CdmwPlatformMngt::PROCESS_FAILED_DEATH) )
        return;

    // Set the status info's message
    m_status_info = CORBA::string_dup( error_info );

    // Store the info about the issuance of this error event.
    m_lastErrorHeader = header;
    m_lastErrorIssuer = issuer;
    
    // Change to the state FailedError
    setState( "FailedError" );
}

void ProcessProxy_impl::endingEvent(
    const CdmwPlatformMngtBase::EventHeader& header )
{
    // Do nothing if in a failed status
    CdmwPlatformMngt::ProcessStatus status = getStatus();

    if( (status == CdmwPlatformMngt::PROCESS_FAILED_ERROR) ||
        (status == CdmwPlatformMngt::PROCESS_FAILED_TIMEOUT) ||
        (status == CdmwPlatformMngt::PROCESS_FAILED_DISCREPANCY) ||
        (status == CdmwPlatformMngt::PROCESS_FAILED_INVALID) ||
        (status == CdmwPlatformMngt::PROCESS_FAILED_NO_RESPONSE) ||
        (status == CdmwPlatformMngt::PROCESS_FAILED_DEATH) )
        return;

    // Set the status info's message
    m_status_info = CORBA::string_dup(
        LogMngr::getFormattedMessage( MSG_ID_PROCESS_ENDING_ERROR ).c_str() );

    // Store the info about the issuance of this event.
    m_lastErrorHeader = header;
    
    // Change to the state FailedDeath
    setState( "FailedDeath" );
}

void ProcessProxy_impl::invalidateEvent(
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* reason )
{
    // Do nothing if in a failed status
    CdmwPlatformMngt::ProcessStatus status = getStatus();

    if( (status == CdmwPlatformMngt::PROCESS_FAILED_ERROR) ||
        (status == CdmwPlatformMngt::PROCESS_FAILED_TIMEOUT) ||
        (status == CdmwPlatformMngt::PROCESS_FAILED_DISCREPANCY) ||
        (status == CdmwPlatformMngt::PROCESS_FAILED_INVALID) ||
        (status == CdmwPlatformMngt::PROCESS_FAILED_NO_RESPONSE) ||
        (status == CdmwPlatformMngt::PROCESS_FAILED_DEATH) )
        return;

    // Set the status info's message
    m_status_info = CORBA::string_dup( reason );

    // Store the info about the issuance of this event.
    m_lastErrorHeader = header;
    
    // Change to the state FailedInvalid
    setState( "FailedInvalid" );
}



const char* ProcessProxy_impl::get_processName()
{
    return m_name.in();
}



} // End namespace PlatformMngt
} // End namespace Cdmw

