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
#include "SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/ApplicationLogMessageId.hpp"


#include <sstream>



namespace Cdmw
{
namespace PlatformMngt
{

// ----------------------------------------------------------------------
// UnmanagedProcessState class.
// ----------------------------------------------------------------------
char* UnmanagedProcessState::status_info(
    UnmanagedProcessStateMachine* stateMachine )
{
    CORBA::String_var statusInfo = CORBA::string_dup(
        (stateMachine->getContext()->m_status_info).in() );

    return statusInfo._retn();
}

void UnmanagedProcessState::invalidate_event(
    UnmanagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* reason )
{
    stateMachine->getContext()->invalidateEvent( header, reason );
}

// ----------------------------------------------------------------------
// UnmanagedProcessInitial class.
// ----------------------------------------------------------------------
UnmanagedProcessInitial::UnmanagedProcessInitial()
{
}

CdmwPlatformMngt::ProcessStatus UnmanagedProcessInitial::status()
{
    return CdmwPlatformMngt::PROCESS_INITIAL;
}

void UnmanagedProcessInitial::set_autoending(
    UnmanagedProcessStateMachine* stateMachine )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending();

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void UnmanagedProcessInitial::set_host(
    UnmanagedProcessStateMachine* stateMachine,
    const char* host_name )
throw( CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setHost( host_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void UnmanagedProcessInitial::set_process_info(
    UnmanagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::ProcessInfo& process_info )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setProcessInfo( process_info );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

CdmwPlatformMngt::RequestStatus UnmanagedProcessInitial::initialise(
    UnmanagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CdmwPlatformMngt::DuplicateActivityPoint,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    // Change to the state Initialising
    stateMachine->setState( "Initialising" );

    return stateMachine->getContext()->m_lastRequestStatus;
}

// ----------------------------------------------------------------------
// UnmanagedProcessInitialising class.
// ----------------------------------------------------------------------
UnmanagedProcessInitialising::UnmanagedProcessInitialising()
{
}

void UnmanagedProcessInitialising::entryAction(
    UnmanagedProcessStateMachine* stateMachine )
{
    UnmanagedProcessProxy_impl* processProxy = stateMachine->getContext();

    // Log and notify the change of status
    processProxy->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    processProxy->notifyStatusChangeEvent(
            header,
            CdmwPlatformMngtBase::PFMMNG );
}

void UnmanagedProcessInitialising::doActivity(
    UnmanagedProcessStateMachine* stateMachine )
{
    UnmanagedProcessProxy_impl* processProxy = stateMachine->getContext();

    processProxy->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_ERROR;
    
    const char* p_applicationName = processProxy->m_application->get_applicationName();
    const char* p_processName = processProxy->get_processName();
    CdmwPlatformMngt::HostProxy_var hostProxy = processProxy->get_host();
    CORBA::String_var processHostName = hostProxy->name();

    try
    {
        // Create the unmanaged process
        CdmwPlatformMngt::ApplicationAgent_var agent
            = processProxy->getApplicationAgent();

        CdmwPlatformMngt::ProcessInfo_var processInfo = 
                               processProxy->get_process_info();
             
        agent->create_process (p_processName, processInfo.in());

        // Change to state RUNNING
        stateMachine->setState( "Running" );

        processProxy->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_SUCCEED;
    }
    catch( const CdmwPlatformMngt::HostNotFound& e )
    {
        // Set the info about the issuance of this error event.
        std::string error_info =
            LogMngr::getFormattedMessage(
                MSG_ID_HOST_NOT_FOUND,
                processHostName.in() );
        processProxy->m_status_info = CORBA::string_dup( error_info.c_str() );

        // Store the info about the issuance of this error event.
        processProxy->m_lastErrorHeader = 
            EventHeaderFactory::createHeader( CdmwPlatformMngtBase::ERR );

        // Change to state INVALID
        stateMachine->setState( "FailedInvalid" );
    }
    catch( const CdmwPlatformMngt::HostNotReachable& e )
    {
        // Set the info about the issuance of this error event.
        std::string error_info =
            LogMngr::getFormattedMessage(
                MSG_ID_HOST_UNREACHABLE,
                processHostName.in() );
        processProxy->m_status_info = CORBA::string_dup( error_info.c_str() );

        // Store the info about the issuance of this error event.
        processProxy->m_lastErrorHeader = 
            EventHeaderFactory::createHeader( CdmwPlatformMngtBase::ERR );

        // Change to state INVALID
        stateMachine->setState( "FailedInvalid" );
    }
    catch( const CdmwPlatformMngt::CreationError& e )
    {
        // Set the info about the issuance of this error event.
        processProxy->m_status_info = CORBA::string_dup( e.error_info.in() );

        // Store the info about the issuance of this error event.
        processProxy->m_lastErrorHeader = 
            EventHeaderFactory::createHeader( CdmwPlatformMngtBase::ERR );

        // Change to state INVALID
        stateMachine->setState( "FailedInvalid" );
    }
    catch( const CdmwPlatformMngt::ApplicationAgentAlreadyExists& e )
    {
        // Set the info about the issuance of this error event.
        std::string error_info =
            LogMngr::getFormattedMessage(
                MSG_ID_AGENT_EXISTS,
                p_applicationName,
                processHostName.in() );
        processProxy->m_status_info = CORBA::string_dup( error_info.c_str() );

        // Store the info about the issuance of this error event.
        processProxy->m_lastErrorHeader = 
            EventHeaderFactory::createHeader( CdmwPlatformMngtBase::ERR );

        // Change to state INVALID
        stateMachine->setState( "FailedInvalid" );
    }
    catch( const CdmwPlatformMngt::InvalidInfo& e )
    {
        // Set the info about the issuance of this error event.
        processProxy->m_status_info = CORBA::string_dup( e.reason.in() );

        // Store the info about the issuance of this error event.
        processProxy->m_lastErrorHeader
            = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::ERR );

        // Change to state INVALID
        stateMachine->setState( "FailedInvalid" );
    }
    catch( const CdmwPlatformMngt::ProcessAlreadyExists& e )
    {
        // Set the info about the issuance of this error event.
        processProxy->m_status_info
            = CORBA::string_dup( "the process already exists" );

        // Store the info about the issuance of this error event.
        processProxy->m_lastErrorHeader
            = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::ERR );

        // Change to state INVALID
        stateMachine->setState( "FailedInvalid" );
    }
    catch( const AgentNotFoundException& e )
    {
        // Set the info about the issue of this error event.
        processProxy->m_status_info = CORBA::string_dup( e.what() );

        // Store the info about the issue of this error event.
        processProxy->m_lastErrorHeader = 
            EventHeaderFactory::createHeader( CdmwPlatformMngtBase::ERR );

        // Change to state INVALID
        stateMachine->setState( "FailedInvalid" );
    }
    
    catch( const CORBA::SystemException& e )
    {
        std::ostringstream exceptionInfo;
        exceptionInfo << e;
        
        // Set the info about the issue of this error event.
        processProxy->m_status_info = CORBA::string_dup( exceptionInfo.str().c_str() );

        // Store the info about the issue of this error event.
        processProxy->m_lastErrorHeader = 
            EventHeaderFactory::createHeader( CdmwPlatformMngtBase::ERR );

        // Change to state INVALID
        stateMachine->setState( "FailedInvalid" );
    }
}

CdmwPlatformMngt::ProcessStatus UnmanagedProcessInitialising::status()
{
    return CdmwPlatformMngt::PROCESS_INITIALISING;
}

// ----------------------------------------------------------------------
// UnmanagedProcessRunning class.
// ----------------------------------------------------------------------
UnmanagedProcessRunning::UnmanagedProcessRunning()
{
}

void UnmanagedProcessRunning::entryAction(
    UnmanagedProcessStateMachine* stateMachine )
{
    UnmanagedProcessProxy_impl* processProxy = stateMachine->getContext();

    // Log and notify the change of status
    processProxy->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    processProxy->notifyStatusChangeEvent(
            header,
            CdmwPlatformMngtBase::PFMMNG );
}

CdmwPlatformMngt::ProcessStatus
UnmanagedProcessRunning::status()
{
    return CdmwPlatformMngt::PROCESS_RUNNING;
}

CdmwPlatformMngt::RequestStatus UnmanagedProcessRunning::stop(
    UnmanagedProcessStateMachine* stateMachine )
throw( CORBA::SystemException )
{
    UnmanagedProcessProxy_impl* processProxy = stateMachine->getContext();

    processProxy->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_SUCCEED;
    
    CdmwPlatformMngt::HostProxy_var hostProxy = processProxy->get_host();
    CORBA::String_var processHostName = hostProxy->name();

    try
    {
        // Kill the OS process
        CdmwPlatformMngt::ApplicationAgent_var agent
            = processProxy->getApplicationAgent();

        agent->kill_process( processProxy->get_processName() );
    }
    catch( const CdmwPlatformMngt::HostNotFound& )
    {
    }
    catch( const CdmwPlatformMngt::HostNotReachable& )
    {
    }
    catch( const CdmwPlatformMngt::ProcessNotFound& )
    {
    }
    catch( ... )
    {
        // Set the info about the issuance of this error event.
        std::string error_info =
            LogMngr::getFormattedMessage(
                MSG_ID_PROCESS_KILL_ERROR,
                processProxy->get_processName(),
                processHostName.in() );

        // Log the error
        LogMngr::logMessage( ERR, error_info.c_str() );
    }

    // Change to the state Stopped
    stateMachine->setState( "Stopped" );

    return processProxy->m_lastRequestStatus;
}

void UnmanagedProcessRunning::ending_event(
    UnmanagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::EventHeader& header )
{
    UnmanagedProcessProxy_impl* processProxy = stateMachine->getContext();

    if( processProxy->is_autoending() )
        stateMachine->setState( "Ended" );
    else
        stateMachine->getContext()->endingEvent( header );
}

// ----------------------------------------------------------------------
// UnmanagedProcessStopped class.
// ----------------------------------------------------------------------
UnmanagedProcessStopped::UnmanagedProcessStopped()
{
}

void UnmanagedProcessStopped::entryAction(
    UnmanagedProcessStateMachine* stateMachine )
{
    UnmanagedProcessProxy_impl* processProxy = stateMachine->getContext();

    // Log and notify the change of status
    processProxy->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    processProxy->notifyStatusChangeEvent(
            header,
            CdmwPlatformMngtBase::PFMMNG );
}

CdmwPlatformMngt::ProcessStatus
UnmanagedProcessStopped::status()
{
    return CdmwPlatformMngt::PROCESS_STOPPED;
}

void UnmanagedProcessStopped::set_autoending(
    UnmanagedProcessStateMachine* stateMachine )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending();

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void UnmanagedProcessStopped::set_host(
    UnmanagedProcessStateMachine* stateMachine,
    const char* host_name )
throw( CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setHost( host_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void UnmanagedProcessStopped::set_process_info(
    UnmanagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::ProcessInfo& process_info )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setProcessInfo( process_info );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

CdmwPlatformMngt::RequestStatus UnmanagedProcessStopped::initialise(
    UnmanagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CdmwPlatformMngt::DuplicateActivityPoint,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    // Change to the state Initialising
    stateMachine->setState( "Initialising" );

    return stateMachine->getContext()->m_lastRequestStatus;
}

// ----------------------------------------------------------------------
// UnmanagedProcessEnded class.
// ----------------------------------------------------------------------
UnmanagedProcessEnded::UnmanagedProcessEnded()
{
}

void UnmanagedProcessEnded::entryAction(
    UnmanagedProcessStateMachine* stateMachine )
{
    UnmanagedProcessProxy_impl* processProxy = stateMachine->getContext();

    // Log and notify the change of status
    processProxy->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    processProxy->notifyStatusChangeEvent(
            header,
            CdmwPlatformMngtBase::PFMMNG );
}

CdmwPlatformMngt::ProcessStatus
UnmanagedProcessEnded::status()
{
    return CdmwPlatformMngt::PROCESS_ENDED;
}

void UnmanagedProcessEnded::set_autoending(
    UnmanagedProcessStateMachine* stateMachine )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending();

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void UnmanagedProcessEnded::set_host(
    UnmanagedProcessStateMachine* stateMachine,
    const char* host_name )
throw( CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setHost( host_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void UnmanagedProcessEnded::set_process_info(
    UnmanagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::ProcessInfo& process_info )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setProcessInfo( process_info );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

CdmwPlatformMngt::RequestStatus UnmanagedProcessEnded::initialise(
    UnmanagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CdmwPlatformMngt::DuplicateActivityPoint,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    // Change to the state Initialising
    stateMachine->setState( "Initialising" );

    return stateMachine->getContext()->m_lastRequestStatus;
}

// ----------------------------------------------------------------------
// UnmanagedProcessFailedDeath class.
// ----------------------------------------------------------------------
UnmanagedProcessFailedDeath::UnmanagedProcessFailedDeath()
{
}

void UnmanagedProcessFailedDeath::entryAction(
    UnmanagedProcessStateMachine* stateMachine )
{
    UnmanagedProcessProxy_impl* processProxy = stateMachine->getContext();

    // Log and notify the change of status
    processProxy->notifyStatusChangeEvent(
        processProxy->m_lastErrorHeader,
        CdmwPlatformMngtBase::PFMMNG );

    // Change the application's mode to degraded
    processProxy->m_application->degradation_event();
}

CdmwPlatformMngt::ProcessStatus
UnmanagedProcessFailedDeath::status()
{
    return CdmwPlatformMngt::PROCESS_FAILED_DEATH;
}

void UnmanagedProcessFailedDeath::set_autoending(
    UnmanagedProcessStateMachine* stateMachine )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending();

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void UnmanagedProcessFailedDeath::set_host(
    UnmanagedProcessStateMachine* stateMachine,
    const char* host_name )
throw( CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setHost( host_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void UnmanagedProcessFailedDeath::set_process_info(
    UnmanagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::ProcessInfo& process_info )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setProcessInfo( process_info );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

CdmwPlatformMngt::RequestStatus
UnmanagedProcessFailedDeath::initialise(
    UnmanagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CdmwPlatformMngt::DuplicateActivityPoint,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    // Change to the state Initialising
    stateMachine->setState( "Initialising" );

    return stateMachine->getContext()->m_lastRequestStatus;
}

// ----------------------------------------------------------------------
// UnmanagedProcessFailedInvalid class.
// ----------------------------------------------------------------------
UnmanagedProcessFailedInvalid::UnmanagedProcessFailedInvalid()
{
}

void UnmanagedProcessFailedInvalid::entryAction(
    UnmanagedProcessStateMachine* stateMachine )
{
    UnmanagedProcessProxy_impl* processProxy = stateMachine->getContext();
        
    // Log and notify the change of status
    processProxy->notifyStatusChangeEvent(
        processProxy->m_lastErrorHeader,
        CdmwPlatformMngtBase::PFMMNG );

    // Change the application's mode to degraded
    processProxy->m_application->degradation_event();
}

CdmwPlatformMngt::ProcessStatus
UnmanagedProcessFailedInvalid::status()
{
    return CdmwPlatformMngt::PROCESS_FAILED_INVALID;
}

void UnmanagedProcessFailedInvalid::set_autoending(
    UnmanagedProcessStateMachine* stateMachine )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending();

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void UnmanagedProcessFailedInvalid::set_host(
    UnmanagedProcessStateMachine* stateMachine,
    const char* host_name )
throw( CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setHost( host_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void UnmanagedProcessFailedInvalid::set_process_info(
    UnmanagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::ProcessInfo& process_info )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setProcessInfo( process_info );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

CdmwPlatformMngt::RequestStatus UnmanagedProcessFailedInvalid::initialise(
    UnmanagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CdmwPlatformMngt::DuplicateActivityPoint,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->setState( "Initialising" );

    return stateMachine->getContext()->m_lastRequestStatus;
}

} // End namespace PlatformMngt
} // End namespace Cdmw

