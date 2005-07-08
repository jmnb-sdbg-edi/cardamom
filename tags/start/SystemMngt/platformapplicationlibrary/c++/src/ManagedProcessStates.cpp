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


#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "platformapplicationlibrary/ManagedProcessProxy_impl.hpp"
#include "SystemMngt/platformlibrary/BoundSyncCall.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/ApplicationLogMessageId.hpp"
#include "platformapplicationlibrary/ServiceBroker_impl.hpp"
#include "platformapplicationlibrary/EntityObserver_impl.hpp"

#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"
#include "SystemMngt/platformvaluetypes/ProcessStatusChange_impl.hpp"


#include <sstream>



namespace Cdmw
{
namespace PlatformMngt
{

// ----------------------------------------------------------------------
// ManagedProcessState class.
// ----------------------------------------------------------------------
char* ManagedProcessState::status_info(
    ManagedProcessStateMachine* stateMachine )
{
    CORBA::String_var statusInfo = CORBA::string_dup(
        (stateMachine->getContext()->m_status_info).in() );

    return statusInfo._retn();
}

// ----------------------------------------------------------------------
// ManagedProcessInitial class.
// ----------------------------------------------------------------------
ManagedProcessInitial::ManagedProcessInitial()
{
}

CdmwPlatformMngt::ProcessStatus ManagedProcessInitial::status()
{
    return CdmwPlatformMngt::PROCESS_INITIAL;
}

void ManagedProcessInitial::set_autoending(
    ManagedProcessStateMachine* stateMachine )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending();

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessInitial::set_host(
    ManagedProcessStateMachine* stateMachine,
    const char* host_name )
throw( CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setHost( host_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessInitial::set_process_info(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::ProcessInfo& process_info )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setProcessInfo( process_info );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessInitial::set_life_cycle(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::LifeCycleDef& life_cycle )
throw( CdmwPlatformMngt::DuplicateStep,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setLifeCycle( life_cycle );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessInitial::set_monitoring_info(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::CompleteMonitoringInfo& monitoring_info)
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setMonitoringInfo( monitoring_info );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessInitial::set_activity_point_defs(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::ActivityPointDefs& point_defs )
throw( CdmwPlatformMngt::DuplicateActivityPoint,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setActivityPointDefs( point_defs );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

CdmwPlatformMngt::RequestStatus ManagedProcessInitial::initialise(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    try
    {
        ManagedProcessProxy_impl* processProxy = stateMachine->getContext();

        // Reset the context
        processProxy->m_startupKind = startup_kind;
        processProxy->m_stepCounter = 0;

        // Change to the state Initialising
        stateMachine->setState( "Initialising" );

        return stateMachine->getContext()->m_lastRequestStatus;
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

// ----------------------------------------------------------------------
// ManagedProcessInitialising class.
// ----------------------------------------------------------------------
ManagedProcessInitialising::ManagedProcessInitialising()
{
}

void ManagedProcessInitialising::entryAction(
    ManagedProcessStateMachine* stateMachine )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();

    // Log and notify the change of status
    processProxy->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    processProxy->notifyStatusChangeEvent(
            header,
            CdmwPlatformMngtBase::PFMMNG );
}

void ManagedProcessInitialising::doActivity(
    ManagedProcessStateMachine* stateMachine )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();

    unsigned long int stepCounter = processProxy->m_stepCounter;
    CORBA::ULong stepTimeout
        = processProxy->m_lifeCycle->init_steps[stepCounter].step_timeout;

    processProxy->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_ERROR;
    
    const char* p_applicationName = processProxy->m_application->get_applicationName();
    const char* p_processName = processProxy->get_processName();
    CdmwPlatformMngt::HostProxy_var hostProxy = processProxy->get_host();
    CORBA::String_var processHostName = hostProxy->name();

    try
    {
        BoundSyncCallReturn result = EXCEPTION;
        BoundSyncCallFactory* factory = BoundSyncCallFactory::getFactory();

        if( stepCounter == 0 )
        {
            // Create a managed process within the defined timeout
            CdmwPlatformMngt::ApplicationAgent_var agent
                = processProxy->getApplicationAgent();

            OsSupport::OS::Timeval startTime = OsSupport::OS::get_time();

            CdmwPlatformMngt::ProcessInfo_var processInfo = 
                               processProxy->get_process_info();
                              
            processProxy->m_process = agent->create_managed_process(
                p_processName,
                processInfo.in(),
                stepTimeout,
                processProxy->m_entityObserver.in(),
                processProxy->m_serviceBroker.in() );
                

            OsSupport::OS::Timeval endTime = OsSupport::OS::get_time();

            // Check the consistence of the number of steps
            // try 3 times to call process in case of exception
            int counter = 3;
            CORBA::ULong nb_of_steps = 0L;
            
            while (counter > 0)
            {
                try
                {
                    nb_of_steps = processProxy->m_process->nb_steps();
                    
                    counter = 0;
                }
                catch (const CORBA::SystemException& e)
                {
                    counter--;
                    
                    // object cannot be reached, throw exception
                    if (counter == 0)
                    {
                        throw;
                    }
                    
                    OsSupport::OS::sleep (50);
                }
            }
            
            if (nb_of_steps !=
                processProxy->m_lifeCycle->init_steps.length())
            {
                // Set the status info's message
                processProxy->m_status_info = CORBA::string_dup(
                    LogMngr::getFormattedMessage(
                        MSG_ID_PROCESS_INVALID_STEP_NUMBER).c_str());

                // Store the info about the issue of this error event.
                processProxy->m_lastErrorHeader =
                    EventHeaderFactory::createHeader(
                        CdmwPlatformMngtBase::ERR);

                // Change to the state FailedDiscrepancy
                stateMachine->setState ("FailedDiscrepancy");
                
                // State has been changed so return
                return;
            }

            // Compute the remaining timeout (in ms)
            unsigned long int elapsedTime
                = (endTime.seconds - startTime.seconds) * 1000000;
            elapsedTime += (endTime.microseconds - startTime.microseconds);
            elapsedTime = elapsedTime / 1000;

            if( elapsedTime > stepTimeout )
                throw CdmwPlatformMngt::CreationTimeout();

            unsigned long int remainingTimeout
                = stepTimeout - elapsedTime;

            // Initialise it within the remaing timeout
            std::auto_ptr< ProcessInitCall > initCall(
                factory->createProcessInitCall(
                    processProxy->m_process.in(),
                    processProxy->m_startupKind,
                    remainingTimeout ) );

            result = initCall->execute();
            if( result == EXCEPTION )
            {
                // Set the status info's message
                processProxy->m_status_info = CORBA::string_dup(
                    LogMngr::getFormattedMessage(
                        MSG_ID_PROCESS_EXCEPTION,
                        initCall->getExceptionName() ).c_str() );

                // Store the info about the issue of this error event.
                processProxy->m_lastErrorHeader =
                    EventHeaderFactory::createHeader(
                        CdmwPlatformMngtBase::ERR );
                processProxy->m_lastErrorIssuer = CdmwPlatformMngtBase::PFMMNG;

                // Change to the state FailedError
                stateMachine->setState( "FailedError" );
            }
        }
        else
        {
            // Perform one step
            std::auto_ptr< ProcessStepCall > stepCall(
                factory->createProcessStepCall(
                    processProxy->m_process.in(),
                    stepTimeout ) );

            result = stepCall->execute();
            if( result == EXCEPTION )
            {
                // Set the status info's message
                processProxy->m_status_info = CORBA::string_dup(
                    LogMngr::getFormattedMessage(
                        MSG_ID_PROCESS_EXCEPTION,
                        stepCall->getExceptionName() ).c_str() );

                // Store the info about the issue of this error event.
                processProxy->m_lastErrorHeader =
                    EventHeaderFactory::createHeader(
                        CdmwPlatformMngtBase::ERR );
                processProxy->m_lastErrorIssuer = CdmwPlatformMngtBase::PFMMNG;

                // Change to the state FailedError
                stateMachine->setState( "FailedError" );
            }
        }

        if( result == SUCCEED )
        {
            processProxy->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_SUCCEED;

            if( (processProxy->m_stepCounter + 1) < 
                processProxy->m_lifeCycle->init_steps.length() )
            {
                // Change to the state StepPerformed
                stateMachine->setState( "StepPerformed" );
            }
            else
            {
                // Change to the state Initialised
                stateMachine->setState( "Initialised" );
            }
        }
        else if( result == TIMEOUT )
        {
            // Set the status info's message
            processProxy->m_status_info = CORBA::string_dup(
                LogMngr::getFormattedMessage(
                    MSG_ID_PROCESS_TIMEOUT,
                    stepTimeout ).c_str() );

            // Store the info about the issue of this error event.
            processProxy->m_lastErrorHeader =
                EventHeaderFactory::createHeader(
                    CdmwPlatformMngtBase::ERR );

            // Change to the state FailedTimeout
            stateMachine->setState( "FailedTimeout" );
        }
    }
    catch( const std::bad_alloc& )
    {
        // Log the error
        LogMngr::logMessage( ERR, "Run out of memory" );

        // TODO: do an assert violation till a policy is decided
        CDMW_ASSERT( false );
    }
    catch( const OutOfMemoryException& e )
    {
        // Log the error
        LogMngr::logMessage( ERR, "Run out of memory" );

        // TODO: do an assert violation till a policy is decided
        CDMW_ASSERT( false );
    }
    catch( const CdmwPlatformMngt::HostNotFound& e )
    {
        // Set the info about the issue of this error event.
        std::string error_info =
            LogMngr::getFormattedMessage(
                MSG_ID_HOST_NOT_FOUND,
                processHostName.in() );
        processProxy->m_status_info = CORBA::string_dup( error_info.c_str() );

        // Store the info about the issue of this error event.
        processProxy->m_lastErrorHeader = 
            EventHeaderFactory::createHeader( CdmwPlatformMngtBase::ERR );

        // Change to state INVALID
        stateMachine->setState( "FailedInvalid" );
    }
    catch( const CdmwPlatformMngt::HostNotReachable& e )
    {
        // Set the info about the issue of this error event.
        std::string error_info =
            LogMngr::getFormattedMessage(
                MSG_ID_HOST_UNREACHABLE,
                processHostName.in() );
        processProxy->m_status_info = CORBA::string_dup( error_info.c_str() );

        // Store the info about the issue of this error event.
        processProxy->m_lastErrorHeader = 
            EventHeaderFactory::createHeader( CdmwPlatformMngtBase::ERR );

        // Change to state INVALID
        stateMachine->setState( "FailedInvalid" );
    }
    catch( const CdmwPlatformMngt::CreationError& e )
    {
        // Set the info about the issue of this error event.
        processProxy->m_status_info = CORBA::string_dup( e.error_info.in() );

        // Store the info about the issue of this error event.
        processProxy->m_lastErrorHeader = 
            EventHeaderFactory::createHeader( CdmwPlatformMngtBase::ERR );

        // Change to state INVALID
        stateMachine->setState( "FailedInvalid" );
    }
    catch( const CdmwPlatformMngt::ApplicationAgentAlreadyExists& e )
    {
        // Set the info about the issue of this error event.
        std::string error_info =
            LogMngr::getFormattedMessage(
                MSG_ID_AGENT_EXISTS,
                p_applicationName,
                processHostName.in() );
        processProxy->m_status_info = CORBA::string_dup( error_info.c_str() );

        // Store the info about the issue of this error event.
        processProxy->m_lastErrorHeader = 
            EventHeaderFactory::createHeader( CdmwPlatformMngtBase::ERR );

        // Change to state INVALID
        stateMachine->setState( "FailedInvalid" );
    }
    catch( const CdmwPlatformMngt::InvalidInfo& e )
    {
        // Set the info about the issue of this error event.
        processProxy->m_status_info = CORBA::string_dup( e.reason.in() );

        // Store the info about the issue of this error event.
        processProxy->m_lastErrorHeader
            = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::ERR );

        // Change to state INVALID
        stateMachine->setState( "FailedInvalid" );
    }
    catch( const CdmwPlatformMngt::CreationTimeout& e )
    {
        // Set the info about the issue of this error event.
        processProxy->m_status_info
            = CORBA::string_dup(
                "the process has not acknowledged its "
                "creation during the allowed timeout" );

        // Store the info about the issue of this error event.
        processProxy->m_lastErrorHeader
            = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::ERR );

        // Change to state INVALID
        stateMachine->setState( "FailedTimeout" );
    }

    catch( const CdmwPlatformMngt::CreationErrorExt& e )
    {
        // Set the info about the issue of this error event.
        processProxy->m_status_info = CORBA::string_dup( e.error_info.in() );

        // Store the info about the issue of this error event.
        processProxy->m_lastErrorHeader
            = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::FTL );
        processProxy->m_lastErrorIssuer = e.issuer.in();

        // Change to state FailedError
        stateMachine->setState( "FailedError" );
    }
    catch( const CdmwPlatformMngt::ProcessAlreadyExists& e )
    {
        // Set the info about the issue of this error event.
        std::string error_info =
            LogMngr::getFormattedMessage(
                MSG_ID_AGENT_PROCESS_EXISTS,
                p_processName,
                p_applicationName,
                processHostName.in() );
        processProxy->m_status_info = CORBA::string_dup( error_info.c_str() );

        // Store the info about the issue of this error event.
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

CdmwPlatformMngt::ProcessStatus ManagedProcessInitialising::status()
{
    return CdmwPlatformMngt::PROCESS_INITIALISING;
}

void ManagedProcessInitialising::ending_event(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::EventHeader& header )
{
    stateMachine->getContext()->endingEvent( header );
}

void ManagedProcessInitialising::invalidate_event(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* reason )
{
    stateMachine->getContext()->invalidateEvent( header, reason );
}

// ----------------------------------------------------------------------
// ManagedProcessStepPerformed class.
// ----------------------------------------------------------------------
ManagedProcessStepPerformed::ManagedProcessStepPerformed()
{
}

void ManagedProcessStepPerformed::entryAction(
    ManagedProcessStateMachine* stateMachine )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();

    // Log and notify the change of status
    processProxy->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    processProxy->notifyStatusChangeEvent(
            header,
            CdmwPlatformMngtBase::PFMMNG );
}

CdmwPlatformMngt::ProcessStatus
ManagedProcessStepPerformed::status()
{
    return CdmwPlatformMngt::PROCESS_STEP_PERFORMED;
}

CdmwPlatformMngt::RequestStatus ManagedProcessStepPerformed::next_step(
    ManagedProcessStateMachine* stateMachine )
throw( CdmwPlatformMngt::InvalidStep,
       CdmwPlatformMngt::IncompatibleType,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->m_stepCounter++;

    // Continue with the next step initialisation
    stateMachine->setState( "Initialising" );

    return stateMachine->getContext()->m_lastRequestStatus;
}

CdmwPlatformMngt::RequestStatus ManagedProcessStepPerformed::stop(
    ManagedProcessStateMachine* stateMachine,
    CORBA::Boolean emergency )
throw( CORBA::SystemException )
{
    if( emergency )
    {
        // Change to the state Stopped
        stateMachine->setState( "Stopped" );
    }
    else
    {
        // Change to the state Stopping
        stateMachine->setState( "Stopping" );
    }

    return stateMachine->getContext()->m_lastRequestStatus;
}

void ManagedProcessStepPerformed::error_event(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* issuer,
    const char* error_info )
{
    stateMachine->getContext()->errorEvent( header, issuer, error_info );
}

void ManagedProcessStepPerformed::ending_event(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::EventHeader& header )
{
    stateMachine->getContext()->endingEvent( header );
}

void ManagedProcessStepPerformed::invalidate_event(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* reason )
{
    stateMachine->getContext()->invalidateEvent( header, reason );
}

// ----------------------------------------------------------------------
// ManagedProcessInitialised class.
// ----------------------------------------------------------------------
ManagedProcessInitialised::ManagedProcessInitialised()
{
}

void ManagedProcessInitialised::entryAction(
    ManagedProcessStateMachine* stateMachine )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();

    // Log and notify the change of status
    processProxy->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    processProxy->notifyStatusChangeEvent(
            header,
            CdmwPlatformMngtBase::PFMMNG );
}

CdmwPlatformMngt::ProcessStatus
ManagedProcessInitialised::status()
{
    return CdmwPlatformMngt::PROCESS_INITIALISED;
}

CdmwPlatformMngt::RequestStatus ManagedProcessInitialised::run(
    ManagedProcessStateMachine* stateMachine )
throw( CdmwPlatformMngt::NotReadyToRun,
       CdmwPlatformMngt::IncompatibleType,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    // Change to the state RunRequest
    stateMachine->setState( "RunRequest" );

    return stateMachine->getContext()->m_lastRequestStatus;
}

CdmwPlatformMngt::RequestStatus ManagedProcessInitialised::stop(
    ManagedProcessStateMachine* stateMachine,
    CORBA::Boolean emergency )
throw( CORBA::SystemException )
{
    if( emergency )
    {
        // Change to the state Stopped
        stateMachine->setState( "Stopped" );
    }
    else
    {
        // Change to the state Stopping
        stateMachine->setState( "Stopping" );
    }

    return stateMachine->getContext()->m_lastRequestStatus;
}

void ManagedProcessInitialised::error_event(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* issuer,
    const char* error_info )
{
    stateMachine->getContext()->errorEvent( header, issuer, error_info );
}

void ManagedProcessInitialised::ending_event(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::EventHeader& header )
{
    stateMachine->getContext()->endingEvent( header );
}

void ManagedProcessInitialised::invalidate_event(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* reason )
{
    stateMachine->getContext()->invalidateEvent( header, reason );
}

// ----------------------------------------------------------------------
// ManagedProcessRunRequest class.
// ----------------------------------------------------------------------
ManagedProcessRunRequest::ManagedProcessRunRequest()
{
}

void ManagedProcessRunRequest::entryAction(
    ManagedProcessStateMachine* stateMachine )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();

    // Log and notify the change of status
    processProxy->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    processProxy->notifyStatusChangeEvent(
            header,
            CdmwPlatformMngtBase::PFMMNG );
}

void ManagedProcessRunRequest::doActivity(
    ManagedProcessStateMachine* stateMachine )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();
    processProxy->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_ERROR;

    try
    {
        BoundSyncCallReturn result;
        BoundSyncCallFactory* factory = BoundSyncCallFactory::getFactory();

        // Perform the process's run
        std::auto_ptr< ProcessRunCall > runCall(
            factory->createProcessRunCall(
                processProxy->m_process.in(),
                processProxy->m_lifeCycle->run_timeout ) );

        result = runCall->execute();
        if( result == SUCCEED )
        {
            processProxy->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_SUCCEED;
                       
            // Change to the state ManagedProcessRunning
            stateMachine->setState( "Running" );
        }
        else if( result == TIMEOUT )
        {
            // Set the status info's message
            processProxy->m_status_info = CORBA::string_dup(
                LogMngr::getFormattedMessage(
                    MSG_ID_PROCESS_TIMEOUT,
                    processProxy->m_lifeCycle->run_timeout ).c_str() );

            // Store the info about the issue of this error event.
            processProxy->m_lastErrorHeader =
                EventHeaderFactory::createHeader( CdmwPlatformMngtBase::ERR );

            // Change to the state FailedTimeout
            stateMachine->setState( "FailedTimeout" );
        }
        else if( result == EXCEPTION )
        {
            CdmwPlatformMngt::Process::NotReadyToRun notReadyToRunException;
            
            if( strcmp(
                    runCall->getExceptionName(),
                    notReadyToRunException._name() ) == 0 )
            {
                // Set the status info's message
                processProxy->m_status_info = CORBA::string_dup(
                    LogMngr::getFormattedMessage(
                        MSG_ID_PROCESS_NOT_READY_TO_RUN ).c_str() );

                // Store the info about the issue of this error event.
                processProxy->m_lastErrorHeader =
                    EventHeaderFactory::createHeader(
                        CdmwPlatformMngtBase::ERR );

                // Change to the state FailedDiscrepancy
                stateMachine->setState( "FailedDiscrepancy" );
            }
            else
            {
                // Set the status info's message
                processProxy->m_status_info = CORBA::string_dup(
                    LogMngr::getFormattedMessage(
                        MSG_ID_PROCESS_EXCEPTION,
                        runCall->getExceptionName() ).c_str() );

                // Store the info about the issue of this error event.
                processProxy->m_lastErrorHeader =
                    EventHeaderFactory::createHeader(
                        CdmwPlatformMngtBase::ERR );
                processProxy->m_lastErrorIssuer = CdmwPlatformMngtBase::PFMMNG;

                // Change to the state FailedError
                stateMachine->setState( "FailedError" );
            }
        }
    }
    catch( const OutOfMemoryException& e )
    {
        // Log the error
        LogMngr::logMessage( ERR, "Run out of memory" );

        // TODO: do an assert violation while no policy is decided
        CDMW_ASSERT( false );
    }

}

CdmwPlatformMngt::ProcessStatus
ManagedProcessRunRequest::status()
{
    return CdmwPlatformMngt::PROCESS_RUN_REQUEST;
}

void ManagedProcessRunRequest::ending_event(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::EventHeader& header )
{
    stateMachine->getContext()->endingEvent( header );
}

void ManagedProcessRunRequest::invalidate_event(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* reason )
{
    stateMachine->getContext()->invalidateEvent( header, reason );
}

// ----------------------------------------------------------------------
// ManagedProcessRunning class.
// ----------------------------------------------------------------------
ManagedProcessRunning::ManagedProcessRunning()
{
}

void ManagedProcessRunning::entryAction(
    ManagedProcessStateMachine* stateMachine )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();

    // Log and notify the change of status
    processProxy->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    processProxy->notifyStatusChangeEvent(
            header,
            CdmwPlatformMngtBase::PFMMNG );
}

CdmwPlatformMngt::ProcessStatus
ManagedProcessRunning::status()
{
    return CdmwPlatformMngt::PROCESS_RUNNING;
}

CdmwPlatformMngt::RequestStatus ManagedProcessRunning::stop(
    ManagedProcessStateMachine* stateMachine,
    CORBA::Boolean emergency )
throw( CORBA::SystemException )
{
    if( emergency )
    {
        // Change to the state Stopped
        stateMachine->setState( "Stopped" );
    }
    else
    {
        // Change to the state Stopping
        stateMachine->setState( "Stopping" );
    }

    return stateMachine->getContext()->m_lastRequestStatus;
}

void ManagedProcessRunning::error_event(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* issuer,
    const char* error_info )
{
    stateMachine->getContext()->errorEvent( header, issuer, error_info );
}

void ManagedProcessRunning::monitoringFailure_event(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::EventHeader& header )
throw( CdmwPlatformMngt::IncompatibleType,
       CORBA::SystemException )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();

    // Set the status info's message
    processProxy->m_status_info = CORBA::string_dup(
        LogMngr::getFormattedMessage(
            MSG_ID_PROCESS_MONITORING_FAILURE ).c_str() );

    // Store the info about the issue of this event.
    processProxy->m_lastErrorHeader = header;
    
    // Change to the state FailedMonitoringFailure
    stateMachine->setState( "FailedMonitoringFailure" );
}

void ManagedProcessRunning::ending_event(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::EventHeader& header )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();

    if( processProxy->is_autoending() )
        stateMachine->setState( "Ended" );
    else
        stateMachine->getContext()->endingEvent( header );
}

void ManagedProcessRunning::invalidate_event(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* reason )
{
    stateMachine->getContext()->invalidateEvent( header, reason );
}

// ----------------------------------------------------------------------
// ManagedProcessStopping class.
// ----------------------------------------------------------------------
ManagedProcessStopping::ManagedProcessStopping()
{
}

void ManagedProcessStopping::entryAction(
    ManagedProcessStateMachine* stateMachine )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();

    // Log and notify the change of status
    processProxy->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    processProxy->notifyStatusChangeEvent(
            header,
            CdmwPlatformMngtBase::PFMMNG );
}

void ManagedProcessStopping::doActivity(
    ManagedProcessStateMachine* stateMachine )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();

    processProxy->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_ERROR;

    try
    {
        // Stop the managed process
        BoundSyncCallFactory* factory = BoundSyncCallFactory::getFactory();
        std::auto_ptr< ProcessStopCall > stopCall(
            factory->createProcessStopCall(
                processProxy->m_process.in(),
                processProxy->m_lifeCycle->stop_timeout ) );

        BoundSyncCallReturn result = stopCall->execute();
        if( result == SUCCEED )
        {
            processProxy->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_SUCCEED;
        }
        else if( result == TIMEOUT )
        {
            // Set the status info's message
            processProxy->m_status_info = CORBA::string_dup(
                LogMngr::getFormattedMessage(
                    MSG_ID_PROCESS_TIMEOUT,
                    processProxy->m_lifeCycle->stop_timeout ).c_str() );

            // Store the info about the issue of this error event.
            processProxy->m_lastErrorHeader =
                EventHeaderFactory::createHeader( CdmwPlatformMngtBase::ERR );
        }
        else if( result == EXCEPTION )
        {
            processProxy->m_status_info = CORBA::string_dup( "" );
        }

        // Change to the state Stopped
        stateMachine->setState( "Stopped" );
    }
    catch( const OutOfMemoryException& e )
    {
        // Log the error
        LogMngr::logMessage( ERR, "Run out of memory" );

        // TODO: do an assert violation while no policy is decided
        CDMW_ASSERT( false );
    }
}

CdmwPlatformMngt::ProcessStatus
ManagedProcessStopping::status()
{
    return CdmwPlatformMngt::PROCESS_STOPPING;
}

void ManagedProcessStopping::invalidate_event(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* reason )
{
    stateMachine->getContext()->invalidateEvent( header, reason );
}

// ----------------------------------------------------------------------
// ManagedProcessStopped class.
// ----------------------------------------------------------------------
ManagedProcessStopped::ManagedProcessStopped()
{
}

void ManagedProcessStopped::entryAction(
    ManagedProcessStateMachine* stateMachine )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();
    
    CdmwPlatformMngt::HostProxy_var hostProxy = processProxy->get_host();
    CORBA::String_var processHostName = hostProxy->name();
    
    processProxy->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_SUCCEED;

    // Log and notify the change of status
    processProxy->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    processProxy->notifyStatusChangeEvent(
            header,
            CdmwPlatformMngtBase::PFMMNG );

    try
    {
        // Kill the managed process (OS process)
        CdmwPlatformMngt::ApplicationAgent_var agent
            = processProxy->getApplicationAgent();

        agent->kill_process(processProxy->get_processName());

        processProxy->m_process = CdmwPlatformMngt::Process::_nil();
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
        // Set the info about the issue of this error event.
        std::string error_info =
            LogMngr::getFormattedMessage(
                MSG_ID_PROCESS_KILL_ERROR,
                processProxy->get_processName(),
                processHostName.in() );

        // Log the error
        LogMngr::logMessage( ERR, error_info.c_str() );
    }
}

CdmwPlatformMngt::ProcessStatus
ManagedProcessStopped::status()
{
    return CdmwPlatformMngt::PROCESS_STOPPED;
}

void ManagedProcessStopped::set_autoending(
    ManagedProcessStateMachine* stateMachine )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending();

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessStopped::set_host(
    ManagedProcessStateMachine* stateMachine,
    const char* host_name )
throw( CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setHost( host_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessStopped::set_process_info(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::ProcessInfo& process_info )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setProcessInfo( process_info );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessStopped::set_life_cycle(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::LifeCycleDef& life_cycle )
throw( CdmwPlatformMngt::DuplicateStep,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setLifeCycle( life_cycle );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

CdmwPlatformMngt::RequestStatus ManagedProcessStopped::initialise(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();

    // Reset the context
    processProxy->m_startupKind = startup_kind;
    processProxy->m_stepCounter = 0;

    stateMachine->setState( "Initialising" );

    return stateMachine->getContext()->m_lastRequestStatus;
}

// ----------------------------------------------------------------------
// ManagedProcessEnded class.
// ----------------------------------------------------------------------
ManagedProcessEnded::ManagedProcessEnded()
{
}

void ManagedProcessEnded::entryAction(
    ManagedProcessStateMachine* stateMachine )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();

    // Log and notify the change of status
    processProxy->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    processProxy->notifyStatusChangeEvent(
            header,
            CdmwPlatformMngtBase::PFMMNG );
}

CdmwPlatformMngt::ProcessStatus
ManagedProcessEnded::status()
{
    return CdmwPlatformMngt::PROCESS_ENDED;
}

void ManagedProcessEnded::set_autoending(
    ManagedProcessStateMachine* stateMachine )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending();

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessEnded::set_host(
    ManagedProcessStateMachine* stateMachine,
    const char* host_name )
throw( CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setHost( host_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessEnded::set_process_info(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::ProcessInfo& process_info )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setProcessInfo( process_info );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessEnded::set_life_cycle(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::LifeCycleDef& life_cycle )
throw( CdmwPlatformMngt::DuplicateStep,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setLifeCycle( life_cycle );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

CdmwPlatformMngt::RequestStatus ManagedProcessEnded::initialise(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw(CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();

    // Reset the context
    processProxy->m_startupKind = startup_kind;
    processProxy->m_stepCounter = 0;

    stateMachine->setState( "Initialising" );

    return stateMachine->getContext()->m_lastRequestStatus;
}

// ----------------------------------------------------------------------
// ManagedProcessFailedError class.
// ----------------------------------------------------------------------
ManagedProcessFailedError::ManagedProcessFailedError()
{
}

void ManagedProcessFailedError::entryAction(
    ManagedProcessStateMachine* stateMachine )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();
    
    CdmwPlatformMngt::HostProxy_var hostProxy = processProxy->get_host();
    CORBA::String_var processHostName = hostProxy->name();
        
    // Log and notify the change of status
    processProxy->notifyStatusChangeEvent(
            processProxy->m_lastErrorHeader,
            processProxy->m_lastErrorIssuer.c_str() );

    // Change the application's mode to degraded
    processProxy->m_application->degradation_event();

    try
    {
        // Kill the managed process (OS process)
        CdmwPlatformMngt::ApplicationAgent_var agent
            = processProxy->getApplicationAgent();

        agent->kill_process(processProxy->get_processName());

        processProxy->m_process = CdmwPlatformMngt::Process::_nil();
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
        // Set the info about the issue of this error event.
        std::string error_info =
            LogMngr::getFormattedMessage(
                MSG_ID_PROCESS_KILL_ERROR,
                processProxy->get_processName(),
                processHostName.in() );

        // Log the error
        LogMngr::logMessage( ERR, error_info.c_str() );
    }
}

CdmwPlatformMngt::ProcessStatus
ManagedProcessFailedError::status()
{
    return CdmwPlatformMngt::PROCESS_FAILED_ERROR;
}

void ManagedProcessFailedError::set_autoending(
    ManagedProcessStateMachine* stateMachine )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending();

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessFailedError::set_host(
    ManagedProcessStateMachine* stateMachine,
    const char* host_name )
throw( CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setHost( host_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessFailedError::set_process_info(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::ProcessInfo& process_info )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setProcessInfo( process_info );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessFailedError::set_life_cycle(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::LifeCycleDef& life_cycle )
throw( CdmwPlatformMngt::DuplicateStep,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setLifeCycle( life_cycle );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

CdmwPlatformMngt::RequestStatus ManagedProcessFailedError::initialise(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();

    // Reset the context
    processProxy->m_startupKind = startup_kind;
    processProxy->m_stepCounter = 0;

    stateMachine->setState( "Initialising" );

    return stateMachine->getContext()->m_lastRequestStatus;
}

void ManagedProcessFailedError::error_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* issuer,
        const char* error_info )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();
    
    CdmwPlatformMngt::HostProxy_var hostProxy = processProxy->get_host();
    CORBA::String_var processHostName = hostProxy->name();

    // update the status info
    processProxy->m_status_info = CORBA::string_dup( error_info );

    // just log and notify the error
    try
    {
        #if CDMW_ORB_VDR == tao
        ProcessMessageFactory eventFactory;
        #else
        CORBA::ValueFactoryBase_var eventFactory
            = new ProcessMessageFactory();
        #endif

        CdmwPlatformMngt::ProcessID_var processId
            = new CdmwPlatformMngt::ProcessID;
        processId->application_name = processProxy->m_application->name();
        processId->process_name = processProxy->name();
            
        #if CDMW_ORB_VDR == tao
        CdmwPlatformMngt::ProcessMessage_var event
            = eventFactory.create(
                header,
                processProxy->m_application->m_applicationAck->system_name.in(),
                processId.in(),
                processHostName.in(),
                issuer,
                error_info );
        #else
        CdmwPlatformMngt::ProcessMessage_var event
            = dynamic_cast< ProcessMessageFactory* >(
                eventFactory.in() )->create(
                    header,
                    processProxy->m_application->m_applicationAck->system_name.in(),
                    processId.in(),
                    processHostName.in(),
                    issuer,
                    error_info );
        #endif

        // Log the error message
        CORBA::String_var eventStr = event->to_string();
        LogMngr::logMessage( eventStr.in() );

        // notify the error message
        processProxy->m_application->m_eventNotifier->addEvent( event );
        event._retn();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
    catch( ... )
    {
        throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );
    }
    
}

// ----------------------------------------------------------------------
// ManagedProcessFailedMonitoringFailure class.
// ----------------------------------------------------------------------
ManagedProcessFailedMonitoringFailure::ManagedProcessFailedMonitoringFailure()
{
}

void ManagedProcessFailedMonitoringFailure::entryAction(
    ManagedProcessStateMachine* stateMachine )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();
    
    CdmwPlatformMngt::HostProxy_var hostProxy = processProxy->get_host();
    CORBA::String_var processHostName = hostProxy->name();
        
    // Log and notify the change of status
    processProxy->notifyStatusChangeEvent(
        processProxy->m_lastErrorHeader,
        CdmwPlatformMngtBase::PFMMNG );

    // Change the application's mode to degraded
    processProxy->m_application->degradation_event();

    try
    {
        // Kill the managed process (OS process)
        CdmwPlatformMngt::ApplicationAgent_var agent
            = processProxy->getApplicationAgent();

        agent->kill_process(processProxy->get_processName());

        processProxy->m_process = CdmwPlatformMngt::Process::_nil();
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
        // Set the info about the issue of this error event.
        std::string error_info =
            LogMngr::getFormattedMessage(
                MSG_ID_PROCESS_KILL_ERROR,
                processProxy->get_processName(),
                processHostName.in() );

        // Log the error
        LogMngr::logMessage( ERR, error_info.c_str() );
    }
}

CdmwPlatformMngt::ProcessStatus
ManagedProcessFailedMonitoringFailure::status()
{
    return CdmwPlatformMngt::PROCESS_FAILED_NO_RESPONSE;
}

void ManagedProcessFailedMonitoringFailure::set_autoending(
    ManagedProcessStateMachine* stateMachine )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending();

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessFailedMonitoringFailure::set_host(
    ManagedProcessStateMachine* stateMachine,
    const char* host_name )
throw( CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setHost( host_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessFailedMonitoringFailure::set_process_info(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::ProcessInfo& process_info )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setProcessInfo( process_info );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessFailedMonitoringFailure::set_life_cycle(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::LifeCycleDef& life_cycle )
throw( CdmwPlatformMngt::DuplicateStep,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setLifeCycle( life_cycle );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

CdmwPlatformMngt::RequestStatus
ManagedProcessFailedMonitoringFailure::initialise(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();

    // Reset the context
    processProxy->m_startupKind = startup_kind;
    processProxy->m_stepCounter = 0;

    stateMachine->setState( "Initialising" );

    return stateMachine->getContext()->m_lastRequestStatus;
}

// ----------------------------------------------------------------------
// ManagedProcessFailedDeath class.
// ----------------------------------------------------------------------
ManagedProcessFailedDeath::ManagedProcessFailedDeath()
{
}

void ManagedProcessFailedDeath::entryAction(
    ManagedProcessStateMachine* stateMachine )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();
        
    // Log and notify the change of status
    processProxy->notifyStatusChangeEvent(
        processProxy->m_lastErrorHeader,
        CdmwPlatformMngtBase::PFMMNG );

    // Change the application's mode to degraded
    processProxy->m_application->degradation_event();
}

CdmwPlatformMngt::ProcessStatus
ManagedProcessFailedDeath::status()
{
    return CdmwPlatformMngt::PROCESS_FAILED_DEATH;
}

void ManagedProcessFailedDeath::set_autoending(
    ManagedProcessStateMachine* stateMachine )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending();

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessFailedDeath::set_host(
    ManagedProcessStateMachine* stateMachine,
    const char* host_name )
throw( CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setHost( host_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessFailedDeath::set_process_info(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::ProcessInfo& process_info )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setProcessInfo( process_info );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessFailedDeath::set_life_cycle(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::LifeCycleDef& life_cycle )
throw( CdmwPlatformMngt::DuplicateStep,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setLifeCycle( life_cycle );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

CdmwPlatformMngt::RequestStatus
ManagedProcessFailedDeath::initialise(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();

    // Reset the context
    processProxy->m_startupKind = startup_kind;
    processProxy->m_stepCounter = 0;

    stateMachine->setState( "Initialising" );

    return stateMachine->getContext()->m_lastRequestStatus;
}

// ----------------------------------------------------------------------
// ManagedProcessFailedDiscrepancy class.
// ----------------------------------------------------------------------
ManagedProcessFailedDiscrepancy::ManagedProcessFailedDiscrepancy()
{
}

void ManagedProcessFailedDiscrepancy::entryAction(
    ManagedProcessStateMachine* stateMachine )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();
    
    CdmwPlatformMngt::HostProxy_var hostProxy = processProxy->get_host();
    CORBA::String_var processHostName = hostProxy->name();
        
    // Log and notify the change of status
    processProxy->notifyStatusChangeEvent(
        processProxy->m_lastErrorHeader,
        CdmwPlatformMngtBase::PFMMNG );

    // Change the application's mode to degraded
    processProxy->m_application->degradation_event();

    try
    {
        // Kill the managed process (OS process)
        CdmwPlatformMngt::ApplicationAgent_var agent
            = processProxy->getApplicationAgent();

        agent->kill_process(processProxy->get_processName());

        processProxy->m_process = CdmwPlatformMngt::Process::_nil();
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
        // Set the info about the issue of this error event.
        std::string error_info =
            LogMngr::getFormattedMessage(
                MSG_ID_PROCESS_KILL_ERROR,
                processProxy->get_processName(),
                processHostName.in() );

        // Log the error
        LogMngr::logMessage( ERR, error_info.c_str() );
    }
}

CdmwPlatformMngt::ProcessStatus
ManagedProcessFailedDiscrepancy::status()
{
    return CdmwPlatformMngt::PROCESS_FAILED_DISCREPANCY;
}

void ManagedProcessFailedDiscrepancy::set_autoending(
    ManagedProcessStateMachine* stateMachine )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending();

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessFailedDiscrepancy::set_host(
    ManagedProcessStateMachine* stateMachine,
    const char* host_name )
throw( CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setHost( host_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessFailedDiscrepancy::set_process_info(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::ProcessInfo& process_info )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setProcessInfo( process_info );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessFailedDiscrepancy::set_life_cycle(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::LifeCycleDef& life_cycle )
throw( CdmwPlatformMngt::DuplicateStep,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setLifeCycle( life_cycle );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

CdmwPlatformMngt::RequestStatus
ManagedProcessFailedDiscrepancy::initialise(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();

    // Reset the context
    processProxy->m_startupKind = startup_kind;
    processProxy->m_stepCounter = 0;

    stateMachine->setState( "Initialising" );

    return stateMachine->getContext()->m_lastRequestStatus;
}

// ----------------------------------------------------------------------
// ManagedProcessFailedTimeout class.
// ----------------------------------------------------------------------
ManagedProcessFailedTimeout::ManagedProcessFailedTimeout()
{
}

void ManagedProcessFailedTimeout::entryAction(
    ManagedProcessStateMachine* stateMachine )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();
    
    CdmwPlatformMngt::HostProxy_var hostProxy = processProxy->get_host();
    CORBA::String_var processHostName = hostProxy->name();
        
    // Log and notify the change of status
    processProxy->notifyStatusChangeEvent(
        processProxy->m_lastErrorHeader,
        CdmwPlatformMngtBase::PFMMNG );

    // Change the application's mode to degraded
    processProxy->m_application->degradation_event();

    try
    {
        // Kill the managed process (OS process)
        CdmwPlatformMngt::ApplicationAgent_var agent
            = processProxy->getApplicationAgent();

        agent->kill_process(processProxy->get_processName());

        processProxy->m_process = CdmwPlatformMngt::Process::_nil();
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
        // Set the info about the issue of this error event.
        std::string error_info =
            LogMngr::getFormattedMessage(
                MSG_ID_PROCESS_KILL_ERROR,
                processProxy->get_processName(),
                processHostName.in() );

        // Log the error
        LogMngr::logMessage( ERR, error_info.c_str() );
    }
}

CdmwPlatformMngt::ProcessStatus
ManagedProcessFailedTimeout::status()
{
    return CdmwPlatformMngt::PROCESS_FAILED_TIMEOUT;
}

void ManagedProcessFailedTimeout::set_autoending(
    ManagedProcessStateMachine* stateMachine )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending();

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessFailedTimeout::set_host(
    ManagedProcessStateMachine* stateMachine,
    const char* host_name )
throw( CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setHost( host_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessFailedTimeout::set_process_info(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::ProcessInfo& process_info )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setProcessInfo( process_info );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessFailedTimeout::set_life_cycle(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::LifeCycleDef& life_cycle )
throw( CdmwPlatformMngt::DuplicateStep,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setLifeCycle( life_cycle );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

CdmwPlatformMngt::RequestStatus
ManagedProcessFailedTimeout::initialise(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();

    // Reset the context
    processProxy->m_startupKind = startup_kind;
    processProxy->m_stepCounter = 0;

    stateMachine->setState( "Initialising" );

    return stateMachine->getContext()->m_lastRequestStatus;
}

// ----------------------------------------------------------------------
// ManagedProcessFailedInvalid class.
// ----------------------------------------------------------------------
ManagedProcessFailedInvalid::ManagedProcessFailedInvalid()
{
}

void ManagedProcessFailedInvalid::entryAction(
    ManagedProcessStateMachine* stateMachine )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();
        
    // Log and notify the change of status
    processProxy->notifyStatusChangeEvent(
        processProxy->m_lastErrorHeader,
        CdmwPlatformMngtBase::PFMMNG );

    // Change the application's mode to degraded
    processProxy->m_application->degradation_event();
}

CdmwPlatformMngt::ProcessStatus
ManagedProcessFailedInvalid::status()
{
    return CdmwPlatformMngt::PROCESS_FAILED_INVALID;
}

void ManagedProcessFailedInvalid::set_autoending(
    ManagedProcessStateMachine* stateMachine )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending();

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessFailedInvalid::set_host(
    ManagedProcessStateMachine* stateMachine,
    const char* host_name )
throw( CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setHost( host_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessFailedInvalid::set_process_info(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::ProcessInfo& process_info )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setProcessInfo( process_info );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ManagedProcessFailedInvalid::set_life_cycle(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngt::LifeCycleDef& life_cycle )
throw( CdmwPlatformMngt::DuplicateStep,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setLifeCycle( life_cycle );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

CdmwPlatformMngt::RequestStatus ManagedProcessFailedInvalid::initialise(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    ManagedProcessProxy_impl* processProxy = stateMachine->getContext();

    // Reset the context
    processProxy->m_startupKind = startup_kind;
    processProxy->m_stepCounter = 0;

    stateMachine->setState( "Initialising" );

    return stateMachine->getContext()->m_lastRequestStatus;
}

} // End namespace PlatformMngt
} // End namespace Cdmw

