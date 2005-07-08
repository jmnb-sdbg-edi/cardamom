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


#include "platformapplicationlibrary/UnmanagedProcessProxy_impl.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"
#include "SystemMngt/platformvaluetypes/ProcessStatusChange_impl.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/ApplicationLogMessageId.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

UnmanagedProcessInitial* UnmanagedProcessProxy_impl::M_initial = NULL;
UnmanagedProcessInitialising* UnmanagedProcessProxy_impl::M_initialising = NULL;
UnmanagedProcessRunning* UnmanagedProcessProxy_impl::M_running = NULL;
UnmanagedProcessStopped* UnmanagedProcessProxy_impl::M_stopped = NULL;
UnmanagedProcessEnded* UnmanagedProcessProxy_impl::M_ended = NULL;
UnmanagedProcessFailedDeath* UnmanagedProcessProxy_impl::M_failedDeath = NULL;
UnmanagedProcessFailedInvalid* UnmanagedProcessProxy_impl::M_failedInvalid = NULL;

// ----------------------------------------------------------------------
// UnmanagedProcessProxy_impl class.
// ----------------------------------------------------------------------
UnmanagedProcessProxy_impl::UnmanagedProcessProxy_impl(
    PortableServer::POA_ptr poa,
    CdmwPlatformMngt::ProcessDef* process_def,
    Application_impl* application )
throw( CORBA::SystemException )
: DeactivableServant_impl( poa ), ProcessProxy_impl( poa, process_def, application )
{
    try
    {
        std::auto_ptr< UnmanagedProcessStateMachine > stateMachine(
            new UnmanagedProcessStateMachine(
                this, "UnmanagedProcessProxy_impl" ) );

        m_stateMachine = stateMachine.get();

        // Add the states to the state machine
        m_stateMachine->addState( "Initial", M_initial );
        m_stateMachine->addState( "Initialising", M_initialising );
        m_stateMachine->addState( "Running", M_running );
        m_stateMachine->addState( "Stopped", M_stopped );
        m_stateMachine->addState( "Ended", M_ended );
        m_stateMachine->addState( "FailedDeath", M_failedDeath );
        m_stateMachine->addState( "FailedInvalid", M_failedInvalid );

        // Set the initial state
        m_stateMachine->setState( "Initial" );

        // Release the ownership of the temporary objects
        stateMachine.release();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

UnmanagedProcessProxy_impl::~UnmanagedProcessProxy_impl()
{
    if( m_stateMachine != NULL )
        delete m_stateMachine;
}

CdmwPlatformMngt::ProcessStatus UnmanagedProcessProxy_impl::get_status(
    CORBA::String_out status_info )
throw( CORBA::SystemException )
{
    CDMW_SM_READER_LOCK_GUARD( *m_stateMachine );

    CORBA::String_var statusInfo = m_status_info;
    status_info = statusInfo._retn();

    return m_stateMachine->getState()->status();
}

void UnmanagedProcessProxy_impl::set_autoending()
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    CDMW_SM_READER_LOCK_GUARD( *m_stateMachine );

    m_stateMachine->getState()->set_autoending( m_stateMachine );
}

void UnmanagedProcessProxy_impl::set_host(
    const char* host_name )
throw( CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    CDMW_SM_READER_LOCK_GUARD( *m_stateMachine );

    m_stateMachine->getState()->set_host( m_stateMachine, host_name );
}

void UnmanagedProcessProxy_impl::set_process_info(
    const CdmwPlatformMngt::ProcessInfo& process_info )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    CDMW_SM_READER_LOCK_GUARD( *m_stateMachine );

    m_stateMachine->getState()->set_process_info(
        m_stateMachine,
        process_info );
}

CdmwPlatformMngt::RequestStatus UnmanagedProcessProxy_impl::initialise(
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

CdmwPlatformMngt::RequestStatus UnmanagedProcessProxy_impl::stop(
    CORBA::Boolean emergency )
throw( CORBA::SystemException )
{
    LogMngr::logMessage(
        INF,
        MSG_ID_PROCESS_STOP,
        get_processName(),
        m_application->get_applicationName() );

    CDMW_SM_WRITER_LOCK_GUARD( *m_stateMachine );

    return m_stateMachine->getState()->stop( m_stateMachine );
}

void UnmanagedProcessProxy_impl::error_event(
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* issuer,
    const char* error_info )
{
}

void UnmanagedProcessProxy_impl::ending_event(
    const CdmwPlatformMngtBase::EventHeader& header )
{
    CDMW_SM_WRITER_LOCK_GUARD( *m_stateMachine );

    m_stateMachine->getState()->ending_event(
        m_stateMachine,
        header );
}

void UnmanagedProcessProxy_impl::invalidate_event(
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* reason )
{
    CDMW_SM_WRITER_LOCK_GUARD( *m_stateMachine );

    m_stateMachine->getState()->invalidate_event(
        m_stateMachine,
        header,
        reason );
}

void UnmanagedProcessProxy_impl::createStates()
throw( OutOfMemoryException )
{
    try
    {
        std::auto_ptr< UnmanagedProcessInitial > initial(
            new UnmanagedProcessInitial() );
        std::auto_ptr< UnmanagedProcessInitialising > initialising(
            new UnmanagedProcessInitialising() );
        std::auto_ptr< UnmanagedProcessRunning > running(
            new UnmanagedProcessRunning() );
        std::auto_ptr< UnmanagedProcessStopped > stopped(
            new UnmanagedProcessStopped() );
        std::auto_ptr< UnmanagedProcessEnded > ended(
            new UnmanagedProcessEnded() );
        std::auto_ptr< UnmanagedProcessFailedDeath > failedDeath(
            new UnmanagedProcessFailedDeath() );
        std::auto_ptr< UnmanagedProcessFailedInvalid > failedInvalid(
            new UnmanagedProcessFailedInvalid() );

        if( M_initial == NULL )
            M_initial = initial.release();
        if( M_initialising == NULL )
            M_initialising = initialising.release();
        if( M_running == NULL )
            M_running = running.release();
        if( M_stopped == NULL )
            M_stopped = stopped.release();
        if( M_ended == NULL )
            M_ended = ended.release();
        if( M_failedDeath == NULL )
            M_failedDeath = failedDeath.release();
        if( M_failedInvalid == NULL )
            M_failedInvalid = failedInvalid.release();
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
}

void UnmanagedProcessProxy_impl::destroyStates()
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

    if( M_running != NULL )
    {
        delete M_running;
        M_running = NULL;
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

    if( M_failedDeath != NULL )
    {
        delete M_failedDeath;
        M_failedDeath = NULL;
    }

    if( M_failedInvalid != NULL )
    {
        delete M_failedInvalid;
        M_failedInvalid = NULL;
    }
}

void UnmanagedProcessProxy_impl::setState( const char* stateName )
{
    m_stateMachine->setState( stateName );
}

CdmwPlatformMngt::ProcessStatus UnmanagedProcessProxy_impl::getStatus()
throw( CORBA::SystemException )    
{
    return m_stateMachine->getState()->status();
}

void UnmanagedProcessProxy_impl::notifyStatusChangeEvent(
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

