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


#include "platformapplicationlibrary/ManagedProcessProxy_impl.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformlibrary/RWEntityContainer_impl.hpp"
#include "SystemMngt/platformlibrary/RWServiceDefContainer_impl.hpp"
#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"
#include "SystemMngt/platformvaluetypes/ProcessStatusChange_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/ApplicationLogMessageId.hpp"
#include "platformapplicationlibrary/ServiceBroker_impl.hpp"
#include "platformapplicationlibrary/EntityObserver_impl.hpp"
#include <set>

namespace Cdmw
{
namespace PlatformMngt
{

ManagedProcessInitial*
    ManagedProcessProxy_impl::M_initial = NULL;
ManagedProcessInitialising*
    ManagedProcessProxy_impl::M_initialising = NULL;
ManagedProcessStepPerformed*
    ManagedProcessProxy_impl::M_stepPerformed = NULL;
ManagedProcessInitialised*
    ManagedProcessProxy_impl::M_initialised = NULL;
ManagedProcessRunRequest*
    ManagedProcessProxy_impl::M_runRequest = NULL;
ManagedProcessRunning*
    ManagedProcessProxy_impl::M_running = NULL;
ManagedProcessStopping*
    ManagedProcessProxy_impl::M_stopping = NULL;
ManagedProcessStopped*
    ManagedProcessProxy_impl::M_stopped = NULL;
ManagedProcessEnded*
    ManagedProcessProxy_impl::M_ended = NULL;
ManagedProcessFailedError*
    ManagedProcessProxy_impl::M_failedError = NULL;
ManagedProcessFailedMonitoringFailure*
    ManagedProcessProxy_impl::M_failedMonitoringFailure = NULL;
ManagedProcessFailedDeath*
    ManagedProcessProxy_impl::M_failedDeath = NULL;
ManagedProcessFailedDiscrepancy*
    ManagedProcessProxy_impl::M_failedDiscrepancy = NULL;
ManagedProcessFailedTimeout*
    ManagedProcessProxy_impl::M_failedTimeout = NULL;
ManagedProcessFailedInvalid*
    ManagedProcessProxy_impl::M_failedInvalid = NULL;

// ----------------------------------------------------------------------
// ManagedProcessProxy_impl class.
// ----------------------------------------------------------------------
ManagedProcessProxy_impl::ManagedProcessProxy_impl(
    PortableServer::POA_ptr poa,
    CdmwPlatformMngt::ManagedProcessDef* process_def,
    Application_impl* application )
throw( CORBA::SystemException )
: DeactivableServant_impl( poa ), ProcessProxy_impl( poa, process_def, application )
{
    m_stateMachine = NULL;
    m_actualEntityContainer = NULL;
    m_actualServiceDefContainer = NULL;
    
    try
    {
        setMonitoringInfo( process_def->monitoring_info() );

        m_actualEntityContainer
            = EntityContainer_impl::EntityContainer::createProcessContainer(
                application->get_applicationName(),
                process_def->name() );

        m_actualServiceDefContainer
            = ServiceDefContainer_impl::ServiceDefContainer::createProcessContainer(
                application->get_applicationName(),
                process_def->name() );
    
        m_rwEntityContainerServant = 
                new RWEntityContainer_impl( poa, m_actualEntityContainer );

        CdmwPlatformMngtService::ServiceDefContainer_var appliServiceDefContainer =
                application->m_serviceDefContainer->_this();
        
        m_rwServiceDefContainerServant = new RWServiceDefContainer_impl (
                poa,
                m_actualServiceDefContainer,
                appliServiceDefContainer.in());

        std::auto_ptr <ManagedProcessStateMachine> stateMachine (
                                       new ManagedProcessStateMachine (
                                                this, "ManagedProcessProxy_impl"));
    
        // Sets the process proxy's internal data
        m_rwServiceDefContainer
            = dynamic_cast< RWServiceDefContainer_impl* >( m_rwServiceDefContainerServant.in() );
            
        m_rwEntityContainer
            = dynamic_cast< RWEntityContainer_impl* >( m_rwEntityContainerServant.in() );


        CORBA::Object_var entityObserverObj
            = application->m_entityObserver->activate( process_def->name() );

        m_entityObserver
            = CdmwPlatformMngtEntity::EntityObserver::_narrow( entityObserverObj.in() );

        CORBA::Object_var serviceBrokerObj
            = application->m_serviceBroker->activate( process_def->name() );

        m_serviceBroker
            = CdmwPlatformMngtService::ServiceBroker::_narrow( serviceBrokerObj.in() );


        m_stateMachine = stateMachine.get();

        // Add the states to the state machine
        m_stateMachine->addState( "Initial", M_initial );
        m_stateMachine->addState( "Initialising", M_initialising );
        m_stateMachine->addState( "StepPerformed", M_stepPerformed );
        m_stateMachine->addState( "Initialised", M_initialised );
        m_stateMachine->addState( "RunRequest", M_runRequest );
        m_stateMachine->addState( "Running", M_running );
        m_stateMachine->addState( "Stopping", M_stopping );
        m_stateMachine->addState( "Stopped", M_stopped );
        m_stateMachine->addState( "Ended", M_ended );
        m_stateMachine->addState( "FailedError", M_failedError );
        m_stateMachine->addState( "FailedMonitoringFailure",
            M_failedMonitoringFailure );
        m_stateMachine->addState( "FailedDeath", M_failedDeath );
        m_stateMachine->addState( "FailedDiscrepancy", M_failedDiscrepancy );
        m_stateMachine->addState( "FailedTimeout", M_failedTimeout );
        m_stateMachine->addState( "FailedInvalid", M_failedInvalid );

        // Release the ownership of the temporary objects
        stateMachine.release();

        // Set the initial state
        m_stateMachine->setState( "Initial" );
    }
    catch( const std::bad_alloc& )
    {
        if( m_actualEntityContainer != NULL )
        {
            EntityContainer_impl::EntityContainer::destroy(
                m_actualEntityContainer );
        }

        if( m_actualServiceDefContainer != NULL )
        {
            ServiceDefContainer_impl::ServiceDefContainer::destroy(
                m_actualServiceDefContainer );
        }

        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

ManagedProcessProxy_impl::~ManagedProcessProxy_impl()
{
    if( m_stateMachine != NULL ) 
        delete m_stateMachine;

    if( m_actualEntityContainer != NULL )
    {
        EntityContainer_impl::EntityContainer::destroy(
            m_actualEntityContainer );
    }

    if( m_actualServiceDefContainer != NULL )
    {
        ServiceDefContainer_impl::ServiceDefContainer::destroy(
            m_actualServiceDefContainer );
    }
}

void ManagedProcessProxy_impl::error_event(
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* issuer,
    const char* error_info )
{
    CDMW_SM_WRITER_LOCK_GUARD( *m_stateMachine );

    m_stateMachine->getState()->error_event(
        m_stateMachine,
        header,
        issuer,
        error_info );
}

void ManagedProcessProxy_impl::ending_event(
    const CdmwPlatformMngtBase::EventHeader& header )
{
    CDMW_SM_WRITER_LOCK_GUARD( *m_stateMachine );

    m_stateMachine->getState()->ending_event(
        m_stateMachine,
        header );
}

void ManagedProcessProxy_impl::monitoringFailure_event(
    const CdmwPlatformMngtBase::EventHeader& header )
throw( CdmwPlatformMngt::IncompatibleType,
       CORBA::SystemException )
{
    CDMW_SM_WRITER_LOCK_GUARD( *m_stateMachine );

    m_stateMachine->getState()->monitoringFailure_event(
        m_stateMachine,
        header );
}

void ManagedProcessProxy_impl::invalidate_event(
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* reason )
{
    CDMW_SM_WRITER_LOCK_GUARD( *m_stateMachine );

    m_stateMachine->getState()->invalidate_event(
        m_stateMachine,
        header,
        reason );
}

CORBA::Object_ptr ManagedProcessProxy_impl::embedded_service()
throw( CORBA::SystemException ) 
{
    if( type() != CdmwPlatformMngt::SERVICE_PROVIDER )
        return CORBA::Object::_nil();

    return m_process->get_service();
}

CdmwPlatformMngt::ProcessStatus ManagedProcessProxy_impl::get_status(
    CORBA::String_out status_info )
throw( CORBA::SystemException )    
{
    CDMW_SM_READER_LOCK_GUARD( *m_stateMachine );

    CORBA::String_var statusInfo = m_status_info;
    status_info = statusInfo._retn();

    return m_stateMachine->getState()->status();
}

void ManagedProcessProxy_impl::set_autoending()
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    CDMW_SM_READER_LOCK_GUARD( *m_stateMachine );

    m_stateMachine->getState()->set_autoending( m_stateMachine );
}

void ManagedProcessProxy_impl::set_host(
    const char* host_name )
throw( CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    CDMW_SM_READER_LOCK_GUARD( *m_stateMachine );

    m_stateMachine->getState()->set_host( m_stateMachine, host_name );
}

void ManagedProcessProxy_impl::set_process_info(
    const CdmwPlatformMngt::ProcessInfo& process_info )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    CDMW_SM_READER_LOCK_GUARD( *m_stateMachine );

    m_stateMachine->getState()->set_process_info(
        m_stateMachine,
        process_info );
}

CdmwPlatformMngt::LifeCycleDef* ManagedProcessProxy_impl::get_life_cycle()
throw( CdmwPlatformMngt::IncompatibleType,
       CORBA::SystemException )
{
    try
    {
        CdmwPlatformMngt::LifeCycleDef_var lifeCycleDef
            = new CdmwPlatformMngt::LifeCycleDef( m_lifeCycle.in() );
            
        return lifeCycleDef._retn();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

void ManagedProcessProxy_impl::set_life_cycle(
    const CdmwPlatformMngt::LifeCycleDef& life_cycle )
throw( CdmwPlatformMngt::DuplicateStep,
       CdmwPlatformMngt::IncompatibleType,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    CDMW_SM_READER_LOCK_GUARD( *m_stateMachine );

    m_stateMachine->getState()->set_life_cycle(
        m_stateMachine,
        life_cycle );
}

CdmwPlatformMngt::CompleteMonitoringInfo ManagedProcessProxy_impl::get_monitoring_info()
throw( CdmwPlatformMngt::IncompatibleType,
       CORBA::SystemException )
{
    return m_monitoring_info.in();
}

void ManagedProcessProxy_impl::set_monitoring_info(
    const CdmwPlatformMngt::CompleteMonitoringInfo& monitoring_info )
throw( CdmwPlatformMngt::IncompatibleType,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    CDMW_SM_READER_LOCK_GUARD( *m_stateMachine );

    m_stateMachine->getState()->set_monitoring_info(
        m_stateMachine,
        monitoring_info );
}

CdmwPlatformMngt::ActivityPointDefs*
ManagedProcessProxy_impl::get_activity_point_defs()
throw( CdmwPlatformMngt::IncompatibleType,
       CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );
}

void ManagedProcessProxy_impl::set_activity_point_defs(
    const CdmwPlatformMngt::ActivityPointDefs& point_defs )
throw( CdmwPlatformMngt::DuplicateActivityPoint,
       CdmwPlatformMngt::IncompatibleType,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    CDMW_SM_READER_LOCK_GUARD( *m_stateMachine );

    m_stateMachine->getState()->set_activity_point_defs(
        m_stateMachine,
        point_defs );
}

CdmwPlatformMngt::RequestStatus ManagedProcessProxy_impl::initialise(
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    LogMngr::logMessage(
        INF,
        MSG_ID_PROCESS_INITALISE,
        get_processName(),
        m_application->get_applicationName() );

    CDMW_SM_WRITER_LOCK_GUARD( *m_stateMachine );

    return m_stateMachine->getState()->initialise(
        m_stateMachine,
        startup_kind );
}

CdmwPlatformMngt::RequestStatus ManagedProcessProxy_impl::next_step()
throw( CdmwPlatformMngt::InvalidStep,
       CdmwPlatformMngt::IncompatibleType,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    CDMW_SM_WRITER_LOCK_GUARD( *m_stateMachine );

    return m_stateMachine->getState()->next_step( m_stateMachine );
}

CdmwPlatformMngt::RequestStatus ManagedProcessProxy_impl::nextStep(
    unsigned int targetStep )
throw( CdmwPlatformMngt::InvalidStep,
       CdmwPlatformMngt::IncompatibleType,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    CDMW_SM_WRITER_LOCK_GUARD( *m_stateMachine );

    if( targetStep != (m_stepCounter+1) )
    {
        LogMngr::logMessage(
            ERR,
            MSG_ID_PROCESS_UNEXPECTED_STEP,
            targetStep,
            get_processName() );

        return CdmwPlatformMngt::REQUEST_ERROR;
    }

    return m_stateMachine->getState()->next_step( m_stateMachine );
}

CdmwPlatformMngt::RequestStatus ManagedProcessProxy_impl::run()
throw( CdmwPlatformMngt::NotReadyToRun,
       CdmwPlatformMngt::IncompatibleType,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    LogMngr::logMessage(
        INF,
        MSG_ID_PROCESS_RUN,
        get_processName(),
        m_application->get_applicationName() );

    CDMW_SM_WRITER_LOCK_GUARD( *m_stateMachine );

    return m_stateMachine->getState()->run( m_stateMachine );
}

CdmwPlatformMngt::RequestStatus ManagedProcessProxy_impl::stop(
    CORBA::Boolean emergency )
throw( CORBA::SystemException ) 
{
    LogMngr::logMessage(
        INF,
        MSG_ID_PROCESS_STOP,
        get_processName(),
        m_application->get_applicationName() );

    CDMW_SM_WRITER_LOCK_GUARD( *m_stateMachine );

    return m_stateMachine->getState()->stop( m_stateMachine, emergency );
}

CORBA::ULong ManagedProcessProxy_impl::get_number_of_entities()
throw( CORBA::SystemException )
{
    return m_rwEntityContainer->get_number_of_entities();
}

void ManagedProcessProxy_impl::get_all_entity_names(
    CORBA::ULong how_many,
    CdmwPlatformMngtEntity::EntityNames_out entity_names,
    CdmwPlatformMngtEntity::EntityNamesIterator_out rest )
throw( CORBA::SystemException )
{
    m_rwEntityContainer->get_all_entity_names(
        how_many, entity_names, rest );
}

CdmwPlatformMngtEntity::EntityStatus ManagedProcessProxy_impl::get_entity_status(
    const char* entity_name,
    CORBA::String_out entity_info )
throw( CdmwPlatformMngtEntity::EntityNotFound, CORBA::SystemException )
{
    return m_rwEntityContainer->get_entity_status( entity_name, entity_info );
}

CORBA::Boolean ManagedProcessProxy_impl::get_entities(
    const CdmwPlatformMngtEntity::EntityNames& entity_names,
    CdmwPlatformMngtEntity::Entities_out nentities )
throw( CORBA::SystemException )
{
    return m_rwEntityContainer->get_entities(
        entity_names, nentities );
}

void ManagedProcessProxy_impl::get_all_entities(
    CORBA::ULong how_many,
    CdmwPlatformMngtEntity::Entities_out nentities,
    CdmwPlatformMngtEntity::EntitiesIterator_out rest )
throw( CORBA::SystemException )
{
    m_rwEntityContainer->get_all_entities(
        how_many, nentities, rest );
}

void ManagedProcessProxy_impl::add_entity(
    const char* entity_name )
throw( CdmwPlatformMngtEntity::EntityAlreadyExists, CORBA::SystemException )
{
    m_rwEntityContainer->add_entity( entity_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessProxy_impl::remove_entity(
    const char* entity_name )
throw( CdmwPlatformMngtEntity::EntityNotFound, CORBA::SystemException )
{
    m_rwEntityContainer->remove_entity( entity_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

CdmwPlatformMngtService::ServiceProviderID*
ManagedProcessProxy_impl::get_service_def(
    const char* service_name )
throw( CdmwPlatformMngtService::ServiceNotFound, CORBA::SystemException )
{
    return m_rwServiceDefContainer->get_service_def( service_name );
}

void ManagedProcessProxy_impl::add_service_def(
    const CdmwPlatformMngtService::ServiceDef& service_def )
throw( CdmwPlatformMngtService::ServiceAlreadyExists, CORBA::SystemException )
{
    m_rwServiceDefContainer->add_service_def( service_def );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessProxy_impl::remove_service_def(
    const char* service_name )
throw( CdmwPlatformMngtService::ServiceNotFound, CORBA::SystemException )
{
    m_rwServiceDefContainer->remove_service_def( service_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessProxy_impl::createStates()
throw( OutOfMemoryException )
{
    try
    {
        std::auto_ptr< ManagedProcessInitial > initial(
            new ManagedProcessInitial() );
        std::auto_ptr< ManagedProcessInitialising > initialising(
            new ManagedProcessInitialising() );
        std::auto_ptr< ManagedProcessStepPerformed > stepPerformed(
            new ManagedProcessStepPerformed() );
        std::auto_ptr< ManagedProcessInitialised > initialised(
            new ManagedProcessInitialised() );
        std::auto_ptr< ManagedProcessRunRequest > runRequest(
            new ManagedProcessRunRequest() );
        std::auto_ptr< ManagedProcessRunning > running(
            new ManagedProcessRunning() );
        std::auto_ptr< ManagedProcessStopping > stopping(
            new ManagedProcessStopping() );
        std::auto_ptr< ManagedProcessStopped > stopped(
            new ManagedProcessStopped() );
        std::auto_ptr< ManagedProcessEnded > ended(
            new ManagedProcessEnded() );
        std::auto_ptr< ManagedProcessFailedError > failedError(
            new ManagedProcessFailedError() );
        std::auto_ptr< ManagedProcessFailedMonitoringFailure > failedMonitoringFailure(
            new ManagedProcessFailedMonitoringFailure() );
        std::auto_ptr< ManagedProcessFailedDeath > failedDeath(
            new ManagedProcessFailedDeath() );
        std::auto_ptr< ManagedProcessFailedDiscrepancy > failedDiscrepancy(
            new ManagedProcessFailedDiscrepancy() );
        std::auto_ptr< ManagedProcessFailedTimeout > failedTimeout(
            new ManagedProcessFailedTimeout() );
        std::auto_ptr< ManagedProcessFailedInvalid > failedInvalid(
            new ManagedProcessFailedInvalid() );

        if( M_initial == NULL )
            M_initial = initial.release();
        if( M_initialising == NULL )
            M_initialising = initialising.release();
        if( M_stepPerformed == NULL )
            M_stepPerformed = stepPerformed.release();
        if( M_initialised == NULL )
            M_initialised = initialised.release();
        if( M_runRequest == NULL )
            M_runRequest = runRequest.release();
        if( M_running == NULL )
            M_running = running.release();
        if( M_stopping == NULL )
            M_stopping = stopping.release();
        if( M_stopped == NULL )
            M_stopped = stopped.release();
        if( M_ended == NULL )
            M_ended = ended.release();
        if( M_failedError == NULL )
            M_failedError = failedError.release();
        if( M_failedMonitoringFailure == NULL )
            M_failedMonitoringFailure = failedMonitoringFailure.release();
        if( M_failedDeath == NULL )
            M_failedDeath = failedDeath.release();
        if( M_failedDiscrepancy == NULL )
            M_failedDiscrepancy = failedDiscrepancy.release();
        if( M_failedTimeout == NULL )
            M_failedTimeout = failedTimeout.release();
        if( M_failedInvalid == NULL )
            M_failedInvalid = failedInvalid.release();
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
}

void ManagedProcessProxy_impl::destroyStates()
{
    if( M_initial != NULL )
    {
        delete M_initial;
        M_initial = NULL;
    }

    if( M_initialising != NULL )
    {
        delete M_initialising;
        M_initialising = NULL;
    }

    if( M_stepPerformed != NULL )
    {
        delete M_stepPerformed;
        M_stepPerformed = NULL;
    }

    if( M_initialised != NULL )
    {
        delete M_initialised;
        M_initialised = NULL;
    }

    if( M_runRequest != NULL )
    {
        delete M_runRequest;
        M_runRequest = NULL;
    }

    if( M_running != NULL )
    {
        delete M_running;
        M_running = NULL;
    }

    if( M_stopping != NULL )
    {
        delete M_stopping;
        M_stopping = NULL;
    }

    if( M_stopped != NULL )
    {
        delete M_stopped;
        M_stopped = NULL;
    }

    if( M_ended != NULL )
    {
        delete M_ended;
        M_ended = NULL;
    }

    if( M_failedError != NULL )
    {
        delete M_failedError;
        M_failedError = NULL;
    }

    if( M_failedMonitoringFailure != NULL )
    {
        delete M_failedMonitoringFailure;
        M_failedMonitoringFailure = NULL;
    }

    if( M_failedDeath != NULL )
    {
        delete M_failedDeath;
        M_failedDeath = NULL;
    }

    if( M_failedDiscrepancy != NULL )
    {
        delete M_failedDiscrepancy;
        M_failedDiscrepancy = NULL;
    }

    if( M_failedTimeout != NULL )
    {
        delete M_failedTimeout;
        M_failedTimeout = NULL;
    }

    if( M_failedInvalid != NULL )
    {
        delete M_failedInvalid;
        M_failedInvalid = NULL;
    }
}

void ManagedProcessProxy_impl::setState( const char* stateName )
{
    m_stateMachine->setState( stateName );
}

CdmwPlatformMngt::ProcessStatus ManagedProcessProxy_impl::getStatus()
throw( CORBA::SystemException )    
{
    return m_stateMachine->getState()->status();
}

void ManagedProcessProxy_impl::setLifeCycle(
    const CdmwPlatformMngt::LifeCycleDef& life_cycle )
throw( CdmwPlatformMngt::DuplicateStep,
       CORBA::SystemException )
{
    try
    {
        std::set< std::string > steps;
        for( unsigned int i=0; i<life_cycle.init_steps.length(); i++ )
        {
            std::string stepName = life_cycle.init_steps[i].step_label.in();
            if( steps.find( stepName ) != steps.end() )
                throw CdmwPlatformMngt::DuplicateStep( stepName.c_str() );
                
            steps.insert( stepName );
        }
    
        m_lifeCycle = new CdmwPlatformMngt::LifeCycleDef( life_cycle );
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

void ManagedProcessProxy_impl::setActivityPointDefs(
    const CdmwPlatformMngt::ActivityPointDefs& point_defs )
throw( CdmwPlatformMngt::DuplicateActivityPoint,
       CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );
}

void ManagedProcessProxy_impl::notifyStatusChangeEvent(
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* issuer )
{
    try
    {
        // Get the status
        CdmwPlatformMngt::ProcessStatus status
            = m_stateMachine->getState()->status();
            
        // Create the event
        CdmwPlatformMngt::ProcessID_var processId
            = new CdmwPlatformMngt::ProcessID;
        processId->application_name = m_application->name();
        processId->process_name = name();
        
        CORBA::String_var processHostName = m_host->name();
        
        const char* p_stepLabel = 
                     (m_lifeCycle->init_steps.length() == 0) ?
                           "" :
                           m_lifeCycle->init_steps[m_stepCounter].step_label.in(); 

        CdmwPlatformMngt::ProcessStatusChange_var event
            = m_procStatusChangeFactory->create(
                header,
                m_application->m_applicationAck->system_name.in(),
                processId.in(),
                processHostName.in(),
                issuer,
                status,
                m_stepCounter,
                p_stepLabel,
                m_status_info.in() );


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

} // End namespace PlatformMngt
} // End namespace Cdmw

