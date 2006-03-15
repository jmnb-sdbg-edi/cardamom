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


#include "platformapplicationlibrary/UnmanagedProcess_impl.hpp"
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
    UnmanagedProcessStateMachine* stateMachine,
    CORBA::Boolean auto_ending )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending(auto_ending);

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
throw( CdmwPlatformMngt::IncompatibleStatus,
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
    UnmanagedProcess_impl* process = stateMachine->getContext();

    // Log and notify the change of status
    process->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    process->notifyStatusChangeEvent(
            header,
            CdmwPlatformMngtBase::PFMMNG );
}

void UnmanagedProcessInitialising::doActivity(
    UnmanagedProcessStateMachine* stateMachine )
{
    UnmanagedProcess_impl* process = stateMachine->getContext();

    process->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_ERROR;
    
    const char* p_applicationName = process->get_application()->get_element_name();
    const char* p_processName = process->get_element_name();
    CdmwPlatformMngt::Host_var host = process->get_host();
    CORBA::String_var processHostName = host->name();

    try
    {
        // Create the unmanaged process
        CdmwPlatformMngt::ApplicationAgent_var agent
            = process->getApplicationAgent();

        CdmwPlatformMngt::ProcessInfo_var processInfo = 
                               process->get_process_info();
        
        CORBA::String_var result_info;
        
        CdmwPlatformMngt::ProcessCommandResult creation_result =     
            agent->create_process (p_processName, processInfo.in(), result_info.out());
            
        if (creation_result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED ||
            creation_result == CdmwPlatformMngt::PROCESS_ALREADY_EXIST)
        {
            // Change to state RUNNING
            stateMachine->setState( "Running" );

            process->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_SUCCEED;
        }
        else if (creation_result == CdmwPlatformMngt::PROCESS_INVALID_INFO)
        {
            // Set the info about the issuance of this error event.
            process->m_status_info = CORBA::string_dup (result_info.in());

            // Store the info about the issuance of this error event.
            process->m_lastErrorHeader =
                EventHeaderFactory::createHeader (CdmwPlatformMngtBase::ERR);

            // Change to state INVALID
            stateMachine->setState( "FailedInvalid" );
        }
        
    }
    catch( const CdmwPlatformMngt::HostNotFound& e )
    {
        // Set the info about the issuance of this error event.
        std::string error_info =
            LogMngr::getFormattedMessage(
                MSG_ID_HOST_NOT_FOUND,
                processHostName.in() );
        process->m_status_info = CORBA::string_dup( error_info.c_str() );

        // Store the info about the issuance of this error event.
        process->m_lastErrorHeader = 
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
        process->m_status_info = CORBA::string_dup( error_info.c_str() );

        // Store the info about the issuance of this error event.
        process->m_lastErrorHeader = 
            EventHeaderFactory::createHeader( CdmwPlatformMngtBase::ERR );

        // Change to state INVALID
        stateMachine->setState( "FailedInvalid" );
    }
    catch( const CdmwPlatformMngt::CreationError& e )
    {
        // Set the info about the issuance of this error event.
        process->m_status_info = CORBA::string_dup( e.error_info.in() );

        // Store the info about the issuance of this error event.
        process->m_lastErrorHeader = 
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
        process->m_status_info = CORBA::string_dup( error_info.c_str() );

        // Store the info about the issuance of this error event.
        process->m_lastErrorHeader = 
            EventHeaderFactory::createHeader( CdmwPlatformMngtBase::ERR );

        // Change to state INVALID
        stateMachine->setState( "FailedInvalid" );
    }
    catch( const AgentNotFoundException& e )
    {
        // Set the info about the issue of this error event.
        process->m_status_info = CORBA::string_dup( e.what() );

        // Store the info about the issue of this error event.
        process->m_lastErrorHeader = 
            EventHeaderFactory::createHeader( CdmwPlatformMngtBase::ERR );

        // Change to state INVALID
        stateMachine->setState( "FailedInvalid" );
    }    
    catch( const CORBA::SystemException& e )
    {
        std::ostringstream exceptionInfo;
        exceptionInfo << e;
        
        // Set the info about the issue of this error event.
        process->m_status_info = CORBA::string_dup( exceptionInfo.str().c_str() );

        // Store the info about the issue of this error event.
        process->m_lastErrorHeader = 
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
    UnmanagedProcess_impl* process = stateMachine->getContext();

    // Log and notify the change of status
    process->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    process->notifyStatusChangeEvent(
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
    UnmanagedProcess_impl* process = stateMachine->getContext();

    // Change to the state Stopped
    stateMachine->setState( "Stopped" );

    return process->m_lastRequestStatus;
}

void UnmanagedProcessRunning::ending_event(
    UnmanagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::EventHeader& header )
{
    UnmanagedProcess_impl* process = stateMachine->getContext();
    
    process->endingEvent (header);
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
    UnmanagedProcess_impl* process = stateMachine->getContext();

    // Log and notify the change of status
    process->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    process->notifyStatusChangeEvent(
            header,
            CdmwPlatformMngtBase::PFMMNG );
            
    process->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_SUCCEED;
    
    CdmwPlatformMngt::Host_var host = process->get_host();
    CORBA::String_var processHostName = host->name();

    try
    {
        // Kill the OS process
        CdmwPlatformMngt::ApplicationAgent_var agent
            = process->getApplicationAgent();

        agent->kill_process( process->get_element_name() );
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
                process->get_element_name(),
                processHostName.in() );

        // Log the error
        LogMngr::logMessage( ERR, error_info.c_str() );
    }
}

CdmwPlatformMngt::ProcessStatus
UnmanagedProcessStopped::status()
{
    return CdmwPlatformMngt::PROCESS_STOPPED;
}

void UnmanagedProcessStopped::set_autoending(
    UnmanagedProcessStateMachine* stateMachine,
    CORBA::Boolean auto_ending )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending(auto_ending);

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
throw( CdmwPlatformMngt::IncompatibleStatus,
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
    UnmanagedProcess_impl* process = stateMachine->getContext();

    // Log and notify the change of status
    process->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    process->notifyStatusChangeEvent(
            header,
            CdmwPlatformMngtBase::PFMMNG );
}

CdmwPlatformMngt::ProcessStatus
UnmanagedProcessEnded::status()
{
    return CdmwPlatformMngt::PROCESS_ENDED;
}

void UnmanagedProcessEnded::set_autoending(
    UnmanagedProcessStateMachine* stateMachine,
    CORBA::Boolean auto_ending )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending(auto_ending);

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

CdmwPlatformMngt::RequestStatus UnmanagedProcessEnded::stop(
    UnmanagedProcessStateMachine* stateMachine)
throw( CORBA::SystemException )
{
    UnmanagedProcess_impl* process = stateMachine->getContext();

    process->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_SUCCEED;

    try
    {
        // call kill_process to clean all process info in agent
        CdmwPlatformMngt::ApplicationAgent_var agent
            = process->getApplicationAgent();

        agent->kill_process(process->get_element_name());        
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
    }

    return CdmwPlatformMngt::REQUEST_SUCCEED;
}

CdmwPlatformMngt::RequestStatus UnmanagedProcessEnded::initialise(
    UnmanagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CdmwPlatformMngt::IncompatibleStatus,
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
    UnmanagedProcess_impl* process = stateMachine->getContext();

    // Log and notify the change of status
    process->notifyStatusChangeEvent(
        process->m_lastErrorHeader,
        CdmwPlatformMngtBase::PFMMNG );

    // Change the application's mode to degraded
    process->get_application()->degradation_event();
    
    try
    {
        // call kill_process to clean all process info in agent
        CdmwPlatformMngt::ApplicationAgent_var agent
            = process->getApplicationAgent();

        agent->kill_process(process->get_element_name());        
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
    }
}

CdmwPlatformMngt::ProcessStatus
UnmanagedProcessFailedDeath::status()
{
    return CdmwPlatformMngt::PROCESS_FAILED_DEATH;
}

void UnmanagedProcessFailedDeath::set_autoending(
    UnmanagedProcessStateMachine* stateMachine,
    CORBA::Boolean auto_ending )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending(auto_ending);

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
throw( CdmwPlatformMngt::IncompatibleStatus,
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
    UnmanagedProcess_impl* process = stateMachine->getContext();
        
    // Log and notify the change of status
    process->notifyStatusChangeEvent(
        process->m_lastErrorHeader,
        CdmwPlatformMngtBase::PFMMNG );

    // Change the application's mode to degraded
    process->get_application()->degradation_event();
}

CdmwPlatformMngt::ProcessStatus
UnmanagedProcessFailedInvalid::status()
{
    return CdmwPlatformMngt::PROCESS_FAILED_INVALID;
}

void UnmanagedProcessFailedInvalid::set_autoending(
    UnmanagedProcessStateMachine* stateMachine,
    CORBA::Boolean auto_ending )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending(auto_ending);

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
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->setState( "Initialising" );

    return stateMachine->getContext()->m_lastRequestStatus;
}

} // End namespace PlatformMngt
} // End namespace Cdmw

