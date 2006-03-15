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


#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "platformapplicationlibrary/ManagedProcess_impl.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/ApplicationLogMessageId.hpp"

#include "platformapplicationlibrary/ServiceBroker_impl.hpp"

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
    ManagedProcessStateMachine* stateMachine,
    CORBA::Boolean auto_ending )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending(auto_ending);

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

CdmwPlatformMngt::RequestStatus ManagedProcessInitial::initialise(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    try
    {
        ManagedProcess_impl* process = stateMachine->getContext();

        // Reset the context
        process->m_startupKind = startup_kind;
        process->m_stepCounter = 0;

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
    ManagedProcess_impl* process = stateMachine->getContext();

    // Log and notify the change of status
    process->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    process->notifyStatusChangeEvent(
            header,
            CdmwPlatformMngtBase::PFMMNG );
}

void ManagedProcessInitialising::doActivity(
    ManagedProcessStateMachine* stateMachine )
{
    ManagedProcess_impl* process = stateMachine->getContext();

    unsigned long int stepCounter = process->m_stepCounter;
    CORBA::ULong stepTimeout
        = process->m_lifeCycle->init_steps[stepCounter].step_timeout;

    process->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_ERROR;
    
    const char* p_applicationName = process->get_application()->get_element_name();
    const char* p_processName = process->get_element_name();
    CdmwPlatformMngt::Host_var host = process->get_host();
    CORBA::String_var processHostName = host->name();
    
    try
    {
        CdmwPlatformMngt::ProcessCommandResult cmd_result =
                          CdmwPlatformMngt::PROCESS_CMD_SUCCEED;
        CORBA::String_var error_info = CORBA::string_dup("");
        CORBA::String_var error_issuer = CORBA::string_dup("");
        
        // if initialisation phase
        if (stepCounter == 0)
        {
            // Create a managed process within the defined timeout
            
            // get application agent
            CdmwPlatformMngt::ApplicationAgent_var agent =
                           process->getApplicationAgent();

            OsSupport::OS::Timeval startTime = OsSupport::OS::get_time();

            CdmwPlatformMngt::ProcessInfo_var processInfo = 
                               process->get_process_info();
        
            cmd_result =     
                agent->create_managed_process (
                        p_processName, processInfo.in(), 
                        stepTimeout, 
                        (process->m_processDelegateWrap).out(), 
                        error_info.out(), error_issuer.out());
            
            // if creation succeeded
            if (cmd_result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED ||
                cmd_result == CdmwPlatformMngt::PROCESS_ALREADY_EXIST)
            {
                OsSupport::OS::Timeval endTime = OsSupport::OS::get_time();
                // Check the consistence of the number of steps
                // try 3 times to call process in case of exception
                int counter = 3;
                CORBA::ULong nb_of_steps = 0L;
            
                while (counter > 0)
                {
                    try
                    {
                        nb_of_steps = process->m_processDelegateWrap->nb_steps();
                    
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
            
                // if nbr of steps is different from the configuration
                if (nb_of_steps !=
                    process->m_lifeCycle->init_steps.length())
                {
                    // Set the status info's message
                    process->m_status_info = CORBA::string_dup(
                        LogMngr::getFormattedMessage(
                            MSG_ID_PROCESS_INVALID_STEP_NUMBER).c_str());

                    // Store the info about the issue of this error event.
                    process->m_lastErrorHeader =
                        EventHeaderFactory::createHeader(CdmwPlatformMngtBase::ERR);

                    // Change to the state FailedDiscrepancy
                    stateMachine->setState ("FailedDiscrepancy");
                }
                
                // nbr of steps is ok
                else
                {
                    // Compute the remaining timeout (in ms)
                    unsigned long int elapsedTime =
                       (endTime.seconds - startTime.seconds) * 1000000;
                    elapsedTime += (endTime.microseconds - startTime.microseconds);
                    elapsedTime = elapsedTime / 1000;

                    if (elapsedTime > stepTimeout)
                    {
                        std::string errorMsg = "<<Insufficient Time>> for process initialisation A:'" ;
                        errorMsg += p_applicationName;
                        errorMsg += "' H:'";
                        errorMsg += processHostName.in();
                        errorMsg += "' P:'";
                        errorMsg += p_processName;
                        errorMsg += "'";
                                                
                        LogMngr::logMessage (INF, errorMsg.c_str());
                                        
                        // set time out
                        cmd_result = CdmwPlatformMngt::PROCESS_CMD_TIMEOUT;
                    }
                    else
                    {
                        unsigned long int remainingTimeout = stepTimeout - elapsedTime;

                        // Call Process Initialise within the remaing timeout
                        cmd_result = process->m_processDelegateWrap->initialise_process (
                                                  process->m_startupKind,
                                                  remainingTimeout);  
                    }
                }            
            }            
        }
        
        // if next step phase
        else
        {
            // Perform one step
            cmd_result = 
              process->m_processDelegateWrap->next_step_process (
                                                     stepCounter, stepTimeout);            
        }

        if (cmd_result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED || 
            cmd_result == CdmwPlatformMngt::PROCESS_INIT_ALREADY_DONE || 
            cmd_result == CdmwPlatformMngt::PROCESS_STEP_ALREADY_DONE)
        {
            process->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_SUCCEED;

            // if step still to perform
            if ((process->m_stepCounter + 1) < 
                 process->m_lifeCycle->init_steps.length())
            {
                // Change to the state StepPerformed
                stateMachine->setState ("StepPerformed");
            }
            
            // else all steps performed
            else
            {
                // Change to the state Initialised
                stateMachine->setState ("Initialised");
            }
        }
                
        else if (cmd_result == CdmwPlatformMngt::PROCESS_INVALID_INFO)
        {
            // Set the info about the issuance of this error event.
            process->m_status_info = CORBA::string_dup (error_info.in());
            // Store the info about the issuance of this error event.
            process->m_lastErrorHeader =
                    EventHeaderFactory::createHeader (CdmwPlatformMngtBase::ERR);

            // Change to state INVALID
            stateMachine->setState ("FailedInvalid");
        }
        else if (cmd_result == CdmwPlatformMngt::PROCESS_CREATION_ERR)
        {
            // Set the info about the issuance of this error event.
            process->m_status_info = CORBA::string_dup (error_info.in());

            // Store the info about the issuance of this error event.
            process->m_lastErrorHeader =
                    EventHeaderFactory::createHeader (CdmwPlatformMngtBase::FTL);
            process->m_lastErrorIssuer = CORBA::string_dup (error_issuer.in());

            // Change to state FailedError
            stateMachine->setState ("FailedError");
        }
        else if (cmd_result == CdmwPlatformMngt::PROCESS_CMD_TIMEOUT)
        {
            // if initialisation phase
            if (stepCounter == 0)
            {
                // Set the info about the issue of this error event.
                process->m_status_info = CORBA::string_dup(
                            "the process has not acknowledged its "
                            "creation during the allowed timeout");                            
            }
            
            // else next step phase
            else
            {
                // Set the status info's message
                process->m_status_info = CORBA::string_dup(
                    LogMngr::getFormattedMessage (MSG_ID_PROCESS_TIMEOUT,stepTimeout).c_str());
            }
            
            // Store the info about the issue of this error event.
            process->m_lastErrorHeader =
                    EventHeaderFactory::createHeader(CdmwPlatformMngtBase::ERR);

            // Change to state INVALID
            stateMachine->setState ("FailedTimeout");
        }
        else if (cmd_result == CdmwPlatformMngt::PROCESS_CMD_EXCEPTION)
        {
            // Set the status info's message
            error_info = process->m_processDelegateWrap->get_command_result_info();
                          
            process->m_status_info = CORBA::string_dup(
                         LogMngr::getFormattedMessage(MSG_ID_PROCESS_EXCEPTION, 
                                                      error_info.in()).c_str());
          
            // Store the info about the issue of this error event.
            process->m_lastErrorHeader = 
                         EventHeaderFactory::createHeader(CdmwPlatformMngtBase::ERR);
            process->m_lastErrorIssuer = CdmwPlatformMngtBase::PFMMNG;

            // Change to the state FailedError
            stateMachine->setState ("FailedError");
        }
    }
    
    catch(const std::bad_alloc&)
    {
        // Log the error
        LogMngr::logMessage (ERR, "Run out of memory");

        // TODO: do an assert violation till a policy is decided
        CDMW_ASSERT(false);
    }
    catch(const OutOfMemoryException& e)
    {
        // Log the error
        LogMngr::logMessage (ERR, "Run out of memory");

        // TODO: do an assert violation till a policy is decided
        CDMW_ASSERT(false);
    }
    catch(const CdmwPlatformMngt::HostNotFound& e)
    {
        // Set the info about the issue of this error event.
        std::string error_info =
            LogMngr::getFormattedMessage (MSG_ID_HOST_NOT_FOUND,
                                          processHostName.in() );
        process->m_status_info = CORBA::string_dup (error_info.c_str());

        // Store the info about the issue of this error event.
        process->m_lastErrorHeader = 
            EventHeaderFactory::createHeader (CdmwPlatformMngtBase::ERR);

        // Change to state INVALID
        stateMachine->setState ("FailedInvalid");
    }
    catch(const CdmwPlatformMngt::HostNotReachable& e)
    {
        // Set the info about the issue of this error event.
        std::string error_info =
            LogMngr::getFormattedMessage (MSG_ID_HOST_UNREACHABLE,
                                          processHostName.in() );
        process->m_status_info = CORBA::string_dup (error_info.c_str());

        // Store the info about the issue of this error event.
        process->m_lastErrorHeader = 
            EventHeaderFactory::createHeader (CdmwPlatformMngtBase::ERR);

        // Change to state INVALID
        stateMachine->setState ("FailedInvalid");
    }
    catch(const CdmwPlatformMngt::CreationError& e)
    {
        // Set the info about the issue of this error event.
        process->m_status_info = CORBA::string_dup (e.error_info.in());

        // Store the info about the issue of this error event.
        process->m_lastErrorHeader = 
            EventHeaderFactory::createHeader (CdmwPlatformMngtBase::ERR);

        // Change to state INVALID
        stateMachine->setState ("FailedInvalid");
    }
    catch(const CdmwPlatformMngt::ApplicationAgentAlreadyExists& e)
    {
        // Set the info about the issue of this error event.
        std::string error_info =
            LogMngr::getFormattedMessage (MSG_ID_AGENT_EXISTS,
                                          p_applicationName,
                                          processHostName.in());
        process->m_status_info = CORBA::string_dup (error_info.c_str());

        // Store the info about the issue of this error event.
        process->m_lastErrorHeader = 
            EventHeaderFactory::createHeader (CdmwPlatformMngtBase::ERR);

        // Change to state INVALID
        stateMachine->setState("FailedInvalid");
    }
    catch(const AgentNotFoundException& e)
    {
        // Set the info about the issue of this error event.
        process->m_status_info = CORBA::string_dup (e.what());

        // Store the info about the issue of this error event.
        process->m_lastErrorHeader = 
            EventHeaderFactory::createHeader (CdmwPlatformMngtBase::ERR);

        // Change to state INVALID
        stateMachine->setState ("FailedInvalid");
    }
    catch(const CORBA::SystemException& e)
    {
        std::ostringstream exceptionInfo;
        exceptionInfo << e;
        
        // Set the info about the issue of this error event.
        process->m_status_info = CORBA::string_dup (exceptionInfo.str().c_str());

        // Store the info about the issue of this error event.
        process->m_lastErrorHeader = 
            EventHeaderFactory::createHeader (CdmwPlatformMngtBase::ERR);

        // Change to state INVALID
        stateMachine->setState ("FailedInvalid");
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
    ManagedProcess_impl* process = stateMachine->getContext();

    // Log and notify the change of status
    process->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    process->notifyStatusChangeEvent(
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
    ManagedProcess_impl* process = stateMachine->getContext();

    // Log and notify the change of status
    process->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    process->notifyStatusChangeEvent(
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
    ManagedProcess_impl* process = stateMachine->getContext();

    // Log and notify the change of status
    process->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    process->notifyStatusChangeEvent(
            header,
            CdmwPlatformMngtBase::PFMMNG );
}

void ManagedProcessRunRequest::doActivity(
    ManagedProcessStateMachine* stateMachine )
{
    ManagedProcess_impl* process = stateMachine->getContext();
    process->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_ERROR;
    
    const char* p_processName = process->get_element_name();

    try
    {
        CdmwPlatformMngt::ProcessCommandResult cmd_result =
                          CdmwPlatformMngt::PROCESS_CMD_SUCCEED;
        CORBA::String_var error_info  = CORBA::string_dup("");
        
        // Perform the process's run
        
        // if process delegate wrapper has nil reference
        if (CORBA::is_nil (process->m_processDelegateWrap.in()))
        {
            std::string err_msg = "<<ProcessDelegateWrapper>> is nil : ";
            err_msg += p_processName;
            
            // Log the error
            LogMngr::logMessage(ERR, err_msg.c_str());
            
            // force command result to not ready to run
            cmd_result = CdmwPlatformMngt::PROCESS_NOT_READY_TO_RUN;
        }
        else
        {
            cmd_result = 
              process->m_processDelegateWrap->run_process (
                                                     process->m_lifeCycle->run_timeout);
        }

        if (cmd_result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED)
        {
            // if process monitoring requested
            if (process->m_monitoring_info->monitoring_model == CdmwPlatformMngt::PULL_MONITORING_MODEL)
            {
                try
                {
                    // get application agent
                    CdmwPlatformMngt::ApplicationAgent_var agent = process->getApplicationAgent();

                    // request the agent to monitor the managed process
                    agent->start_process_monitoring(p_processName,
                                                    (process->m_monitoring_info).in());
                }
                catch (CdmwPlatformMngt::ProcessNotFound &)
                {
                    // Set the info about the issue of this error event.
                    std::string error_info =
                        LogMngr::getFormattedMessage(
                           MSG_ID_PULL_MONITORING_NIL_ERROR,
                                p_processName);

                    // Log the error
                    LogMngr::logMessage( ERR, error_info.c_str() );
                }
                catch (CdmwPlatformMngt::IncompatibleMonitoringValue &)
                {
                    // Set the info about the issue of this error event.
                    std::string error_info =
                        LogMngr::getFormattedMessage(
                           MSG_ID_PULL_MONITORING_VALUE_ERROR,
                                p_processName);

                    // Log the error
                    LogMngr::logMessage( ERR, error_info.c_str() );
                }
                catch (...)
                {
                    // Set the info about the issue of this error event.
                    std::string error_info =
                        LogMngr::getFormattedMessage(
                           MSG_ID_PULL_MONITORING_UNEXPECTED_ERROR,
                                p_processName);

                    // Log the error
                    LogMngr::logMessage( ERR, error_info.c_str() );
                }
            }
            
            process->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_SUCCEED;
                       
            // Change to the state ManagedProcessRunning
            stateMachine->setState ("Running");
        }
        
        else if (cmd_result == CdmwPlatformMngt::PROCESS_RUN_ALREADY_DONE)
        {
            process->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_SUCCEED;
                       
            // Change to the state ManagedProcessRunning
            stateMachine->setState ("Running");
            
        }
        
        else if (cmd_result == CdmwPlatformMngt::PROCESS_CMD_TIMEOUT)
        {
            // Set the status info's message
            process->m_status_info = CORBA::string_dup(
                    LogMngr::getFormattedMessage (MSG_ID_PROCESS_TIMEOUT,
                                                  process->m_lifeCycle->run_timeout).c_str());

            // Store the info about the issue of this error event.
            process->m_lastErrorHeader =
                    EventHeaderFactory::createHeader(CdmwPlatformMngtBase::ERR);

            // Change to state INVALID
            stateMachine->setState ("FailedTimeout");
        }
        else if (cmd_result == CdmwPlatformMngt::PROCESS_NOT_READY_TO_RUN)
        {
            // Set the status info's message
            process->m_status_info = CORBA::string_dup(
                         LogMngr::getFormattedMessage(MSG_ID_PROCESS_NOT_READY_TO_RUN).c_str());

            // Store the info about the issue of this error event.
            process->m_lastErrorHeader =
                    EventHeaderFactory::createHeader(CdmwPlatformMngtBase::ERR);

            // Change to the state FailedDiscrepancy
            stateMachine->setState ("FailedDiscrepancy");
        }
        else if (cmd_result == CdmwPlatformMngt::PROCESS_CMD_EXCEPTION)
        {
            CdmwPlatformMngt::ProcessDelegate::NotReadyToRun notReadyToRunException;
            
            // Set the status info's message
            
            // Store the info about the issue of this error event.
            process->m_lastErrorHeader =
                    EventHeaderFactory::createHeader(CdmwPlatformMngtBase::ERR);
            process->m_lastErrorIssuer = CdmwPlatformMngtBase::PFMMNG;
                
            error_info = process->m_processDelegateWrap->get_command_result_info();
            
            // if not ready to run exception
            if (strcmp (error_info.in(), notReadyToRunException._name()) == 0)
            {   
                process->m_status_info = CORBA::string_dup(
                         LogMngr::getFormattedMessage(MSG_ID_PROCESS_NOT_READY_TO_RUN).c_str());
                         
                // Change to the state FailedDiscrepancy
                stateMachine->setState ("FailedDiscrepancy");
            }
            else
            {                            
                process->m_status_info = CORBA::string_dup(
                         LogMngr::getFormattedMessage(MSG_ID_PROCESS_EXCEPTION, error_info.in()).c_str());
          
                // Change to the state FailedError
                stateMachine->setState ("FailedError");
            }
        }
    }
    catch(const OutOfMemoryException& e)
    {
        // Log the error
        LogMngr::logMessage(ERR, "Run out of memory");

        // TODO: do an assert violation while no policy is decided
        CDMW_ASSERT(false);
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
    ManagedProcess_impl* process = stateMachine->getContext();

    // Log and notify the change of status
    process->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    process->notifyStatusChangeEvent(
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
    ManagedProcess_impl* process = stateMachine->getContext();

    // Set the status info's message
    process->m_status_info = CORBA::string_dup(
        LogMngr::getFormattedMessage(
            MSG_ID_PROCESS_MONITORING_FAILURE ).c_str() );

    // Store the info about the issue of this event.
    process->m_lastErrorHeader = header;
    
    // Change to the state FailedMonitoringFailure
    stateMachine->setState( "FailedMonitoringFailure" );
}

void ManagedProcessRunning::ending_event(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::EventHeader& header )
{
    stateMachine->getContext()->endingEvent(header);
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
    ManagedProcess_impl* process = stateMachine->getContext();

    // Log and notify the change of status
    process->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    process->notifyStatusChangeEvent(
            header,
            CdmwPlatformMngtBase::PFMMNG );
}

void ManagedProcessStopping::doActivity(
    ManagedProcessStateMachine* stateMachine )
{
    ManagedProcess_impl* process = stateMachine->getContext();

    process->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_ERROR;
    const char* p_processName = process->get_element_name();

    try
    {
        CdmwPlatformMngt::ProcessCommandResult cmd_result =
                          CdmwPlatformMngt::PROCESS_CMD_SUCCEED;
        CORBA::String_var error_info = CORBA::string_dup("");
        
        // if process monitoring requested
        if (process->m_monitoring_info->monitoring_model == CdmwPlatformMngt::PULL_MONITORING_MODEL)
        {
            try
            {
                // get application agent
                CdmwPlatformMngt::ApplicationAgent_var agent = process->getApplicationAgent();

                // request the agent to stop monitor the managed process
                agent->stop_process_monitoring(p_processName);
            }
            catch (CdmwPlatformMngt::ProcessNotFound &)
            {
                // Set the info about the issue of this error event.
                std::string error_info =
                    LogMngr::getFormattedMessage(
                        MSG_ID_PULL_MONITORING_NIL_ERROR,
                        p_processName);

                // Log the error
                LogMngr::logMessage( ERR, error_info.c_str() );
            }
            catch (...)
            {
                // Set the info about the issue of this error event.
                std::string error_info =
                    LogMngr::getFormattedMessage(
                        MSG_ID_PULL_MONITORING_UNEXPECTED_ERROR,
                        p_processName);

                // Log the error
                LogMngr::logMessage( ERR, error_info.c_str() );
            }
        }

        // Stop the managed process
        
        // if process delegate wrapper has nil reference
        if (CORBA::is_nil (process->m_processDelegateWrap.in()))
        {
            std::string err_msg = "<<ProcessDelegateWrapper>> is nil : ";
            err_msg += p_processName;
            
            // Log the error
            LogMngr::logMessage(ERR, err_msg.c_str());
            
            // force result to stop already done
            cmd_result = CdmwPlatformMngt::PROCESS_STOP_ALREADY_DONE;
        }
        else
        {
            cmd_result = 
              process->m_processDelegateWrap->stop_process (
                                                     process->m_lifeCycle->stop_timeout);
        }

        if (cmd_result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED ||
            cmd_result == CdmwPlatformMngt::PROCESS_STOP_ALREADY_DONE)
        {
            process->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_SUCCEED;
        }
        
        else if (cmd_result == CdmwPlatformMngt::PROCESS_CMD_TIMEOUT)
        {
            // Set the status info's message
            process->m_status_info = CORBA::string_dup(
                    LogMngr::getFormattedMessage (MSG_ID_PROCESS_TIMEOUT,
                                                  process->m_lifeCycle->stop_timeout).c_str());

            // Store the info about the issue of this error event.
            process->m_lastErrorHeader =
                    EventHeaderFactory::createHeader (CdmwPlatformMngtBase::ERR);
        }
        else if (cmd_result == CdmwPlatformMngt::PROCESS_CMD_EXCEPTION)
        {
            process->m_status_info = CORBA::string_dup ("");
        }

        // Change to the state Stopped
        stateMachine->setState ("Stopped");
    }
    catch(const OutOfMemoryException& e)
    {
        // Log the error
        LogMngr::logMessage(ERR, "Run out of memory");

        // TODO: do an assert violation while no policy is decided
        CDMW_ASSERT(false);
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
    ManagedProcess_impl* process = stateMachine->getContext();
    
    CdmwPlatformMngt::Host_var host = process->get_host();
    CORBA::String_var processHostName = host->name();
    
    process->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_SUCCEED;

    // Log and notify the change of status
    process->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    process->notifyStatusChangeEvent(
            header,
            CdmwPlatformMngtBase::PFMMNG );
    
    // reset the delegate wrapper reference
    process->m_processDelegateWrap = CdmwPlatformMngt::ProcessDelegateWrapper::_nil();
    
    try
    {
        // Kill the managed process (OS process)
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
        // Set the info about the issue of this error event.
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
ManagedProcessStopped::status()
{
    return CdmwPlatformMngt::PROCESS_STOPPED;
}

void ManagedProcessStopped::set_autoending(
    ManagedProcessStateMachine* stateMachine,
    CORBA::Boolean auto_ending )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending(auto_ending);

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
    ManagedProcess_impl* process = stateMachine->getContext();

    // Reset the context
    process->m_startupKind = startup_kind;
    process->m_stepCounter = 0;

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
    ManagedProcess_impl* process = stateMachine->getContext();

    // Log and notify the change of status
    process->m_status_info = CORBA::string_dup( "" );

    CdmwPlatformMngtBase::EventHeader header
        = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

    process->notifyStatusChangeEvent(
            header,
            CdmwPlatformMngtBase::PFMMNG );
}

CdmwPlatformMngt::ProcessStatus
ManagedProcessEnded::status()
{
    return CdmwPlatformMngt::PROCESS_ENDED;
}

void ManagedProcessEnded::set_autoending(
    ManagedProcessStateMachine* stateMachine,
    CORBA::Boolean auto_ending )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending(auto_ending);

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

CdmwPlatformMngt::RequestStatus ManagedProcessEnded::stop(
    ManagedProcessStateMachine* stateMachine,
    CORBA::Boolean emergency )
throw( CORBA::SystemException )
{
    ManagedProcess_impl* process = stateMachine->getContext();
    
    process->m_lastRequestStatus = CdmwPlatformMngt::REQUEST_SUCCEED;
    process->m_status_info = CORBA::string_dup( "" );

    // reset the delegate wrapper reference
    process->m_processDelegateWrap = CdmwPlatformMngt::ProcessDelegateWrapper::_nil();
    
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

CdmwPlatformMngt::RequestStatus ManagedProcessEnded::initialise(
    ManagedProcessStateMachine* stateMachine,
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw(CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    ManagedProcess_impl* process = stateMachine->getContext();

    // Reset the context
    process->m_startupKind = startup_kind;
    process->m_stepCounter = 0;

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
    ManagedProcess_impl* process = stateMachine->getContext();
    
    CdmwPlatformMngt::Host_var host = process->get_host();
    CORBA::String_var processHostName = host->name();
        
    // Log and notify the change of status
    process->notifyStatusChangeEvent(
            process->m_lastErrorHeader,
            process->m_lastErrorIssuer.c_str() );

    // Change the application's mode to degraded
    process->get_application()->degradation_event();
    
    // reset the delegate wrapper reference
    process->m_processDelegateWrap = CdmwPlatformMngt::ProcessDelegateWrapper::_nil();

    try
    {
        // Kill the managed process (OS process)
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
        // Set the info about the issue of this error event.
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
ManagedProcessFailedError::status()
{
    return CdmwPlatformMngt::PROCESS_FAILED_ERROR;
}

void ManagedProcessFailedError::set_autoending(
    ManagedProcessStateMachine* stateMachine,
    CORBA::Boolean auto_ending )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending(auto_ending);

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
    ManagedProcess_impl* process = stateMachine->getContext();

    // Reset the context
    process->m_startupKind = startup_kind;
    process->m_stepCounter = 0;

    stateMachine->setState( "Initialising" );

    return stateMachine->getContext()->m_lastRequestStatus;
}

void ManagedProcessFailedError::error_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* issuer,
        const char* error_info )
{
    ManagedProcess_impl* process = stateMachine->getContext();
    
    CdmwPlatformMngt::Host_var host = process->get_host();
    CORBA::String_var processHostName = host->name();

    // update the status info
    process->m_status_info = CORBA::string_dup( error_info );

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
        processId->application_name = process->get_application()->name();
        processId->process_name = process->name();
        processId->host_name = host->name();

        #if CDMW_ORB_VDR == tao
        CdmwPlatformMngt::ProcessMessage_var event
            = eventFactory.create(
                header,
                process->get_application()->get_system_name(),
                processId.in(),
                issuer,
                error_info );
        #else
        CdmwPlatformMngt::ProcessMessage_var event
            = dynamic_cast< ProcessMessageFactory* >(
                eventFactory.in() )->create(
                    header,
                    process->get_application()->get_system_name(),
                    processId.in(),
                    issuer,
                    error_info );
        #endif

        // Log the error message
        CORBA::String_var eventStr = event->to_string();
        LogMngr::logMessage( eventStr.in() );

        // notify the error message
        process->get_application()->get_event_notifier()->addEvent(event);
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
    ManagedProcess_impl* process = stateMachine->getContext();
    
    CdmwPlatformMngt::Host_var host = process->get_host();
    CORBA::String_var processHostName = host->name();
        
    // Log and notify the change of status
    process->notifyStatusChangeEvent(
        process->m_lastErrorHeader,
        CdmwPlatformMngtBase::PFMMNG );

    // Change the application's mode to degraded
    process->get_application()->degradation_event();
    
    // reset the delegate wrapper reference
    process->m_processDelegateWrap = CdmwPlatformMngt::ProcessDelegateWrapper::_nil();

    try
    {
        // Kill the managed process (OS process)
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
        // Set the info about the issue of this error event.
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
ManagedProcessFailedMonitoringFailure::status()
{
    return CdmwPlatformMngt::PROCESS_FAILED_NO_RESPONSE;
}

void ManagedProcessFailedMonitoringFailure::set_autoending(
    ManagedProcessStateMachine* stateMachine,
    CORBA::Boolean auto_ending )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending(auto_ending);

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
    ManagedProcess_impl* process = stateMachine->getContext();

    // Reset the context
    process->m_startupKind = startup_kind;
    process->m_stepCounter = 0;

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
    ManagedProcess_impl* process = stateMachine->getContext();
        
    // Log and notify the change of status
    process->notifyStatusChangeEvent(
        process->m_lastErrorHeader,
        CdmwPlatformMngtBase::PFMMNG );

    // Change the application's mode to degraded
    process->get_application()->degradation_event();
    
    // reset the delegate wrapper reference
    process->m_processDelegateWrap = CdmwPlatformMngt::ProcessDelegateWrapper::_nil();
    
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
ManagedProcessFailedDeath::status()
{
    return CdmwPlatformMngt::PROCESS_FAILED_DEATH;
}

void ManagedProcessFailedDeath::set_autoending(
    ManagedProcessStateMachine* stateMachine,
    CORBA::Boolean auto_ending )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending(auto_ending);

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
    ManagedProcess_impl* process = stateMachine->getContext();

    // Reset the context
    process->m_startupKind = startup_kind;
    process->m_stepCounter = 0;

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
    ManagedProcess_impl* process = stateMachine->getContext();
    
    CdmwPlatformMngt::Host_var host = process->get_host();
    CORBA::String_var processHostName = host->name();
        
    // Log and notify the change of status
    process->notifyStatusChangeEvent(
        process->m_lastErrorHeader,
        CdmwPlatformMngtBase::PFMMNG );

    // Change the application's mode to degraded
    process->get_application()->degradation_event();
    
    // reset the delegate wrapper reference
    process->m_processDelegateWrap = CdmwPlatformMngt::ProcessDelegateWrapper::_nil();

    try
    {
        // Kill the managed process (OS process)
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
        // Set the info about the issue of this error event.
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
ManagedProcessFailedDiscrepancy::status()
{
    return CdmwPlatformMngt::PROCESS_FAILED_DISCREPANCY;
}

void ManagedProcessFailedDiscrepancy::set_autoending(
    ManagedProcessStateMachine* stateMachine,
    CORBA::Boolean auto_ending )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending(auto_ending);

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
    ManagedProcess_impl* process = stateMachine->getContext();

    // Reset the context
    process->m_startupKind = startup_kind;
    process->m_stepCounter = 0;

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
    ManagedProcess_impl* process = stateMachine->getContext();
    
    CdmwPlatformMngt::Host_var host = process->get_host();
    CORBA::String_var processHostName = host->name();
        
    // Log and notify the change of status
    process->notifyStatusChangeEvent(
        process->m_lastErrorHeader,
        CdmwPlatformMngtBase::PFMMNG );

    // Change the application's mode to degraded
    process->get_application()->degradation_event();
    
    // reset the delegate wrapper reference
    process->m_processDelegateWrap = CdmwPlatformMngt::ProcessDelegateWrapper::_nil();

    try
    {
        // Kill the managed process (OS process)
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
        // Set the info about the issue of this error event.
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
ManagedProcessFailedTimeout::status()
{
    return CdmwPlatformMngt::PROCESS_FAILED_TIMEOUT;
}

void ManagedProcessFailedTimeout::set_autoending(
    ManagedProcessStateMachine* stateMachine,
    CORBA::Boolean auto_ending )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending(auto_ending);

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
    ManagedProcess_impl* process = stateMachine->getContext();

    // Reset the context
    process->m_startupKind = startup_kind;
    process->m_stepCounter = 0;

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
    ManagedProcess_impl* process = stateMachine->getContext();
        
    // Log and notify the change of status
    process->notifyStatusChangeEvent(
        process->m_lastErrorHeader,
        CdmwPlatformMngtBase::PFMMNG );

    // Change the application's mode to degraded
    process->get_application()->degradation_event();
    
    // reset the delegate wrapper reference
    process->m_processDelegateWrap = CdmwPlatformMngt::ProcessDelegateWrapper::_nil();
}

CdmwPlatformMngt::ProcessStatus
ManagedProcessFailedInvalid::status()
{
    return CdmwPlatformMngt::PROCESS_FAILED_INVALID;
}

void ManagedProcessFailedInvalid::set_autoending(
    ManagedProcessStateMachine* stateMachine,
    CORBA::Boolean auto_ending )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setAutoending(auto_ending);

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
    ManagedProcess_impl* process = stateMachine->getContext();

    // Reset the context
    process->m_startupKind = startup_kind;
    process->m_stepCounter = 0;

    stateMachine->setState( "Initialising" );

    return stateMachine->getContext()->m_lastRequestStatus;
}

} // End namespace PlatformMngt
} // End namespace Cdmw

