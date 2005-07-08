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


#include "Foundation/common/Assert.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"
#include "SystemMngt/platformlibrary/GraphUtility.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"
#include "SystemMngt/platformlibrary/SynchronisableTaskDef.hpp"
#include "SystemMngt/platformlibrary/TaskSequencer.hpp"
#include "SystemMngt/platformlibrary/TaskStarter.hpp"
#include "SystemMngt/platformvaluetypes/GraphElement_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/ApplicationLogMessageId.hpp"
#include "platformapplicationlibrary/ProcessProxy_impl.hpp"
#include "platformapplicationlibrary/ProcessInit.hpp"
#include "platformapplicationlibrary/ProcessRun.hpp"
#include "platformapplicationlibrary/ProcessStop.hpp"


namespace Cdmw
{
namespace PlatformMngt
{

// ----------------------------------------------------------------------
// ApplicationInitial class.
// ----------------------------------------------------------------------
ApplicationInitial::ApplicationInitial()
{
}

CdmwPlatformMngt::ApplicationStatus ApplicationInitial::status()
{
    return CdmwPlatformMngt::APPLICATION_INITIAL;
}

void ApplicationInitial::define(
    ApplicationStateMachine* stateMachine,
    const CdmwPlatformMngt::ApplicationDef& application_def )
throw(CdmwPlatformMngt::DuplicateEntity,
      CdmwPlatformMngt::DuplicateService,
      CdmwPlatformMngt::DuplicateProcess,
      CdmwPlatformMngt::ProcessHostNotFound,
      CdmwPlatformMngt::DuplicateProcessEntity,
      CdmwPlatformMngt::DuplicateProcessService,
      CdmwPlatformMngt::DuplicateProcessStep,
      CdmwPlatformMngt::DuplicateProcessActivityPoint,
      CdmwPlatformMngt::AlreadyDone,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException )
{
    stateMachine->getContext()->defineApplication( application_def );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

CdmwPlatformMngt::ProcessProxy_ptr ApplicationInitial::add_process(
    ApplicationStateMachine* stateMachine,
    CdmwPlatformMngt::ProcessDef* process_def )
throw( CdmwPlatformMngt::ProcessAlreadyExists,
       CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::DuplicateEntity,
       CdmwPlatformMngt::DuplicateService,
       CdmwPlatformMngt::DuplicateStep,
       CdmwPlatformMngt::DuplicateActivityPoint,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    CdmwPlatformMngt::ProcessProxy_ptr processProxy
        = stateMachine->getContext()->addProcess( process_def );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager

    return processProxy;
}

void ApplicationInitial::remove_process(
    ApplicationStateMachine* stateMachine,
    const char* process_name )
throw( CdmwPlatformMngt::ProcessNotFound,
       CdmwPlatformMngt::ProcessStillActive,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->removeProcess( process_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ApplicationInitial::set_init_graph(
    ApplicationStateMachine* stateMachine,
    const CdmwPlatformMngt::GraphElements& process_graph)
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setInitGraph( process_graph );
}

void ApplicationInitial::set_stop_graph(
    ApplicationStateMachine* stateMachine,
    const CdmwPlatformMngt::GraphElements& process_graph)
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setStopGraph( process_graph );
}

void ApplicationInitial::initialise(
    ApplicationStateMachine* stateMachine,
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    try
    {
        stateMachine->getContext()->m_newInitGraph = true;
        stateMachine->getContext()->m_startup_kind
            = new CdmwPlatformMngtBase::StartupKind( startup_kind );

        // Change to the state Initialising
        stateMachine->setState( "Initialising" );
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

// ----------------------------------------------------------------------
// ApplicationInitialising class.
// ----------------------------------------------------------------------
ApplicationInitialising::ApplicationInitialising()
{
}

void ApplicationInitialising::entryAction(
    ApplicationStateMachine* stateMachine )
{
    Application_impl* application = stateMachine->getContext();

    // Log and notify the change of status
    application->m_statusInfo = CORBA::string_dup( "" );
    application->notifyStatusChangeEvent();

    // Stop and destroy the stop sequence 
    application->destroyStopSequencer();
}

void ApplicationInitialising::exitAction(
    ApplicationStateMachine* stateMachine )
{
    stateMachine->getContext()->m_newInitGraph = false;
}

void ApplicationInitialising::doActivity(
    ApplicationStateMachine* stateMachine )
{
    try
    {
        Application_impl* application = stateMachine->getContext();
        if( application->m_newInitGraph )
        {
            // Create the process init sequencer
            std::auto_ptr< InitProcessSuspensionCallback > suspensionCallback(
                new InitProcessSuspensionCallback( application ) );

            std::auto_ptr< InitProcessCompletionCallback > completionCallback(
                new InitProcessCompletionCallback( application ) );

            application->m_initSyncTaskFactory
                = InitProcessTaskFactory::createFactory(
                    application,
                    application->m_startup_kind.in(),
                    application->m_initProcessGraph.in() );

            application->m_initSequencer = new Sequencer(
                application->m_initSyncTaskFactory );
            application->m_initSequencer->setSuspensionCallback(
                suspensionCallback.release() );
            application->m_initSequencer->setCompletionCallback(
                completionCallback.release() );

            // Start the init sequencer
            LogMngr::logMessage(
                INF,
                MSG_ID_APPLICATION_START_INIT_SEQUENCER,
                application->m_applicationAck->application_name.in() );

            application->m_initSequencer->start();
        }
        else
        {
            // Resume the init sequencer
            LogMngr::logMessage(
                INF,
                MSG_ID_APPLICATION_RESUME_INIT_SEQUENCER,
                application->m_applicationAck->application_name.in() );

            application->m_initSequencer->resume();
        }
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
    catch( const OutOfMemoryException& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
    catch( const BadParameterException& )
    {
        throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );
    }
    catch (...)
    {
        throw CORBA::INTERNAL (OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}

CdmwPlatformMngt::ApplicationStatus
ApplicationInitialising::status()
{
    return CdmwPlatformMngt::APPLICATION_INITIALISING;
}

void ApplicationInitialising::set_stop_graph(
    ApplicationStateMachine* stateMachine,
    const CdmwPlatformMngt::GraphElements& process_graph )
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setStopGraph( process_graph );
}

void ApplicationInitialising::stop(
    ApplicationStateMachine* stateMachine,
    CORBA::Boolean emergency )
throw( CORBA::SystemException )
{
    // Change to the state Stopping
    stateMachine->setState( "Stopping" );
}

void ApplicationInitialising::completion_event(
    ApplicationStateMachine* stateMachine )
{
    // Change to the state Initialised
    stateMachine->setState( "Initialised" );
}

void ApplicationInitialising::non_completion_event(
    ApplicationStateMachine* stateMachine )
{
    // Change to the state InitialisationIncomplete
    stateMachine->setState( "InitialisationIncomplete" );
}

void ApplicationInitialising::suspension_event(
    ApplicationStateMachine* stateMachine )
{
    // Change to the state InitialisationSuspended
    stateMachine->setState( "InitialisationSuspended" );
}

// ----------------------------------------------------------------------
// ApplicationInitialisationSuspended class.
// ----------------------------------------------------------------------
ApplicationInitialisationSuspended::ApplicationInitialisationSuspended()
{
}

void ApplicationInitialisationSuspended::entryAction(
    ApplicationStateMachine* stateMachine )
{
    Application_impl* application = stateMachine->getContext();

    application->setInitialisationStatus(
        CdmwPlatformMngt::LONG_REQUEST_SUSPENDED );

    // Log and notify the change of status
    application->m_statusInfo = CORBA::string_dup( "" );
    application->notifyStatusChangeEvent();
}

CdmwPlatformMngt::ApplicationStatus
ApplicationInitialisationSuspended::status()
{
    return CdmwPlatformMngt::APPLICATION_INITIALISATION_SUSPENDED;
}

void ApplicationInitialisationSuspended::set_init_graph(
    ApplicationStateMachine* stateMachine,
    const CdmwPlatformMngt::GraphElements& process_graph)
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    Application_impl* application = stateMachine->getContext();

    // Stop and destroy the init sequencer 
    application->destroyInitSequencer();

    // Set the init process graph
    application->setInitGraph( process_graph );
    application->m_newInitGraph = true;
}

void ApplicationInitialisationSuspended::set_stop_graph(
    ApplicationStateMachine* stateMachine,
    const CdmwPlatformMngt::GraphElements& process_graph)
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setStopGraph( process_graph );
}

void ApplicationInitialisationSuspended::resume(
    ApplicationStateMachine* stateMachine )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    // Change to the state Initialising
    stateMachine->setState( "Initialising" );
}

void ApplicationInitialisationSuspended::stop(
    ApplicationStateMachine* stateMachine,
    CORBA::Boolean emergency )
throw( CORBA::SystemException )
{
    // Change to the state Stopping
    stateMachine->setState( "Stopping" );
}

// ----------------------------------------------------------------------
// ApplicationInitialisationIncomplete class.
// ----------------------------------------------------------------------
ApplicationInitialisationIncomplete::ApplicationInitialisationIncomplete()
{
}

void ApplicationInitialisationIncomplete::entryAction(
    ApplicationStateMachine* stateMachine )
{
    Application_impl* application = stateMachine->getContext();

    application->setInitialisationStatus(
        CdmwPlatformMngt::LONG_REQUEST_NOT_COMPLETED );

    // Log and notify the change of status
    application->m_statusInfo = CORBA::string_dup( "" );
    application->notifyStatusChangeEvent();
}

CdmwPlatformMngt::ApplicationStatus
ApplicationInitialisationIncomplete::status()
{
    return CdmwPlatformMngt::APPLICATION_INITIALISATION_INCOMPLETE;
}

void ApplicationInitialisationIncomplete::set_init_graph(
    ApplicationStateMachine* stateMachine,
    const CdmwPlatformMngt::GraphElements& process_graph)
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setInitGraph( process_graph );
}

void ApplicationInitialisationIncomplete::set_stop_graph(
    ApplicationStateMachine* stateMachine,
    const CdmwPlatformMngt::GraphElements& process_graph)
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setStopGraph( process_graph );
}

void ApplicationInitialisationIncomplete::stop(
    ApplicationStateMachine* stateMachine,
    CORBA::Boolean emergency )
throw( CORBA::SystemException )
{
    // Change to the state Stopping
    stateMachine->setState( "Stopping" );
}

// ----------------------------------------------------------------------
// ApplicationInitialised class.
// ----------------------------------------------------------------------
ApplicationInitialised::ApplicationInitialised()
{
}

void ApplicationInitialised::entryAction(
    ApplicationStateMachine* stateMachine )
{
    Application_impl* application = stateMachine->getContext();

    application->setInitialisationStatus( 
        CdmwPlatformMngt::LONG_REQUEST_COMPLETED );

    // Log and notify the change of status
    application->m_statusInfo = CORBA::string_dup( "" );
    application->notifyStatusChangeEvent();

    try
    {

        // Notify the initialisation event to the application observers
        application->m_applicationAck->application_observer->notify_initialisation(
            application->m_applicationAck->application_name.in() );
    }
    catch( const CORBA::Exception& e )
    {
        // TODO: What to do with the raised exception ?!!
        std::string errorName = e._name();
        LogMngr::logMessage(
            INF,
            (errorName + " exception raised by app observer notification" ).c_str() );
    }
}

CdmwPlatformMngt::ApplicationStatus
ApplicationInitialised::status()
{
    return CdmwPlatformMngt::APPLICATION_INITIALISED;
}

CdmwPlatformMngt::ProcessProxy_ptr ApplicationInitialised::add_process(
    ApplicationStateMachine* stateMachine,
    CdmwPlatformMngt::ProcessDef* process_def )
throw( CdmwPlatformMngt::ProcessAlreadyExists,
       CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::DuplicateEntity,
       CdmwPlatformMngt::DuplicateService,
       CdmwPlatformMngt::DuplicateStep,
       CdmwPlatformMngt::DuplicateActivityPoint,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    CdmwPlatformMngt::ProcessProxy_ptr processProxy
        = stateMachine->getContext()->addProcess( process_def );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager

    return processProxy;
}

void ApplicationInitialised::remove_process(
    ApplicationStateMachine* stateMachine,
    const char* process_name )
throw( CdmwPlatformMngt::ProcessNotFound,
       CdmwPlatformMngt::ProcessStillActive,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->removeProcess( process_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ApplicationInitialised::set_init_graph(
    ApplicationStateMachine* stateMachine,
    const CdmwPlatformMngt::GraphElements& process_graph)
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setInitGraph( process_graph );
}

void ApplicationInitialised::set_stop_graph(
    ApplicationStateMachine* stateMachine,
    const CdmwPlatformMngt::GraphElements& process_graph)
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setStopGraph( process_graph );
}

void ApplicationInitialised::run(
    ApplicationStateMachine* stateMachine )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    // Change to the state RunRequest
    stateMachine->setState( "RunRequest" );
}

void ApplicationInitialised::stop(
    ApplicationStateMachine* stateMachine,
    CORBA::Boolean emergency )
throw( CORBA::SystemException )
{
    // Change to the state Stopping
    stateMachine->setState( "Stopping" );
}

// ----------------------------------------------------------------------
// ApplicationRunRequest class.
// ----------------------------------------------------------------------
ApplicationRunRequest::ApplicationRunRequest()
{
}

void ApplicationRunRequest::entryAction(
    ApplicationStateMachine* stateMachine )
{
    Application_impl* application = stateMachine->getContext();

    // Log and notify the change of status
    application->m_statusInfo = CORBA::string_dup( "" );
    application->notifyStatusChangeEvent();
}

void ApplicationRunRequest::doActivity(
    ApplicationStateMachine* stateMachine )
{
    try
    {
        Application_impl* application = stateMachine->getContext();

        // Create the run starter
        std::auto_ptr< RunProcessCompletionCallback > completionCallback(
            new RunProcessCompletionCallback( application ) );

        application->m_taskFactory
            = RunProcessTaskFactory::createFactory( application );

        application->m_starter = new Starter( application->m_taskFactory );
        application->m_starter->setCompletionCallback(
            completionCallback.release() );

        // Get the list of task to be run concurently
        std::list< std::string > taskList;
        Cdmw::PlatformMngt::Application_impl::ProcessProxies::iterator processIt;
        for( processIt = application->m_processProxies.begin();
             processIt != application->m_processProxies.end();
             processIt++ )
        {
            ProcessProxy_impl* process = processIt->second;
            const char* p_processName = process->get_processName();
            CORBA::String_var statusInfo;
            CdmwPlatformMngt::ProcessStatus status
                = process->get_status( statusInfo.out() );

            if( status == CdmwPlatformMngt::PROCESS_INITIALISED )
                taskList.push_back( p_processName );
        }
        
        
        application->m_starter->addTasksToStart( taskList );
        
        // Start the run starter
        application->m_starter->start();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
    catch( const OutOfMemoryException& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
    catch( const BadParameterException& )
    {
        throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );
    }
    catch( const CdmwPlatformMngt::ProcessNotFound& )
    {
        throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );
    }
    catch (...)
    {
        throw CORBA::INTERNAL (OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}

CdmwPlatformMngt::ApplicationStatus
ApplicationRunRequest::status()
{
    return CdmwPlatformMngt::APPLICATION_RUN_REQUEST;
}

void ApplicationRunRequest::set_init_graph(
    ApplicationStateMachine* stateMachine,
    const CdmwPlatformMngt::GraphElements& process_graph)
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setInitGraph( process_graph );
}

void ApplicationRunRequest::set_stop_graph(
    ApplicationStateMachine* stateMachine,
    const CdmwPlatformMngt::GraphElements& process_graph)
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setStopGraph( process_graph );
}

void ApplicationRunRequest::stop(
    ApplicationStateMachine* stateMachine,
    CORBA::Boolean emergency )
throw( CORBA::SystemException )
{
    // Change to the state Stopping
    stateMachine->setState( "Stopping" );
}

void ApplicationRunRequest::completion_event(
    ApplicationStateMachine* stateMachine )
{
    // Change to the state Running
    stateMachine->setState( "Running" );
}

// ----------------------------------------------------------------------
// ApplicationRunning class.
// ----------------------------------------------------------------------
ApplicationRunning::ApplicationRunning()
{
}

void ApplicationRunning::entryAction(
    ApplicationStateMachine* stateMachine )
{
    Application_impl* application = stateMachine->getContext();

    // Log and notify the change of status
    application->m_statusInfo = CORBA::string_dup( "" );
    application->notifyStatusChangeEvent();
}

CdmwPlatformMngt::ApplicationStatus
ApplicationRunning::status()
{
    return CdmwPlatformMngt::APPLICATION_RUNNING;
}

CdmwPlatformMngt::ProcessProxy_ptr ApplicationRunning::add_process(
    ApplicationStateMachine* stateMachine,
    CdmwPlatformMngt::ProcessDef* process_def )
throw( CdmwPlatformMngt::ProcessAlreadyExists,
       CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::DuplicateEntity,
       CdmwPlatformMngt::DuplicateService,
       CdmwPlatformMngt::DuplicateStep,
       CdmwPlatformMngt::DuplicateActivityPoint,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    CdmwPlatformMngt::ProcessProxy_ptr processProxy
        = stateMachine->getContext()->addProcess( process_def );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager

    return processProxy;
}

void ApplicationRunning::remove_process(
    ApplicationStateMachine* stateMachine,
    const char* process_name )
throw( CdmwPlatformMngt::ProcessNotFound,
       CdmwPlatformMngt::ProcessStillActive,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->removeProcess( process_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ApplicationRunning::set_init_graph(
    ApplicationStateMachine* stateMachine,
    const CdmwPlatformMngt::GraphElements& process_graph)
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setInitGraph( process_graph );
}

void ApplicationRunning::set_stop_graph(
    ApplicationStateMachine* stateMachine,
    const CdmwPlatformMngt::GraphElements& process_graph)
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setStopGraph( process_graph );
}

void ApplicationRunning::stop(
    ApplicationStateMachine* stateMachine,
    CORBA::Boolean emergency )
throw( CORBA::SystemException )
{
    // Change to the state Stopping
    stateMachine->setState( "Stopping" );
}

// ----------------------------------------------------------------------
// ApplicationStopping class.
// ----------------------------------------------------------------------
ApplicationStopping::ApplicationStopping()
{
}

void ApplicationStopping::entryAction(
    ApplicationStateMachine* stateMachine )
{
    Application_impl* application = stateMachine->getContext();

    // Log and notify the change of status
    application->m_statusInfo = CORBA::string_dup( "" );
    application->notifyStatusChangeEvent();

    // Stop and destroy the init sequence 
    application->destroyInitSequencer();

    // Stop and destroy the run starter 
    application->destroyStarter();
}

void ApplicationStopping::doActivity(
    ApplicationStateMachine* stateMachine )
{
    try
    {
        Application_impl* application = stateMachine->getContext();
        CdmwPlatformMngt::GraphElements_var stopProcessGraph
            = new CdmwPlatformMngt::GraphElements();

        // If regular stop, consider the process stop graph
        if( !application->m_emergency )
        {
            stopProcessGraph = application->m_stopProcessGraph;
        }

        // Get the list of all element of the stop graph
        ElementNames elementNames
            = GraphUtility::getElementNames( stopProcessGraph.in() );

        // Add all the active process proxies as roots of the stop graph
        // If not present in the stop graph.
        Application_impl::ProcessProxies::iterator processIt;

        for( processIt =  application->m_processProxies.begin();
             processIt != application->m_processProxies.end();
             processIt++ )
        {
            ProcessProxy_impl* process = processIt->second;
            const char* p_processName = process->get_processName();
            CORBA::String_var statusInfo;
            CdmwPlatformMngt::ProcessStatus status
                = process->get_status( statusInfo.out() );

            if( ( elementNames.find( p_processName ) == elementNames.end() ) &&
                ( ( status == CdmwPlatformMngt::PROCESS_INITIALISING ) ||
                  ( status == CdmwPlatformMngt::PROCESS_STEP_PERFORMED ) ||
                  ( status == CdmwPlatformMngt::PROCESS_INITIALISED ) ||
                  ( status == CdmwPlatformMngt::PROCESS_RUN_REQUEST ) ||
                  ( status == CdmwPlatformMngt::PROCESS_RUNNING ) ||
                  ( status == CdmwPlatformMngt::PROCESS_STOPPING ) ) )
            {
                CdmwPlatformMngt::GraphElement_var element
                    = application->m_graphElementFactory->create(
                        p_processName,
                        0 );

                // insert new element in graph
                int graphlen =  stopProcessGraph->length();                     
                stopProcessGraph->length (graphlen + 1);
                stopProcessGraph->operator[] (graphlen) = element;
            }
        }

        // Create the stop sequencer
        std::auto_ptr< StopProcessCompletionCallback > completionCallback(
            new StopProcessCompletionCallback( application ) );

        application->m_stopSyncTaskFactory
            = StopProcessTaskFactory::createFactory(
                application,
                application->m_emergency,
                stopProcessGraph.in() );

        application->m_stopSequencer = new Sequencer(
            application->m_stopSyncTaskFactory );
        application->m_stopSequencer->setCompletionCallback(
            completionCallback.release() );

        // Start the stop sequencer
        LogMngr::logMessage(
            INF,
            MSG_ID_APPLICATION_START_STOP_SEQUENCER,
            application->m_applicationAck->application_name.in() );

        application->m_stopSequencer->start();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
    catch( const OutOfMemoryException& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
    catch( const BadParameterException& )
    {
        throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );
    }
    catch (...)
    {
        throw CORBA::INTERNAL (OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}

CdmwPlatformMngt::ApplicationStatus ApplicationStopping::status()
{
    return CdmwPlatformMngt::APPLICATION_STOPPING;
}

void ApplicationStopping::set_init_graph(
    ApplicationStateMachine* stateMachine,
    const CdmwPlatformMngt::GraphElements& process_graph)
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setInitGraph( process_graph );
}

void ApplicationStopping::completion_event(
    ApplicationStateMachine* stateMachine )
{
    Application_impl* application = stateMachine->getContext();
    
    // kill all application agents

    LogMngr::logMessage(
            INF,
            MSG_ID_APPLICATION_START_KILL_AGENTS,
            application->m_applicationAck->application_name.in() );            
    
    application->killAllAgents();
    
    // Change to the state Stopped
    stateMachine->setState( "Stopped" );
}

// ----------------------------------------------------------------------
// ApplicationStopped class.
// ----------------------------------------------------------------------
ApplicationStopped::ApplicationStopped()
{
}

void ApplicationStopped::entryAction(
    ApplicationStateMachine* stateMachine )
{
    Application_impl* application = stateMachine->getContext();

    application->setStopStatus(
        CdmwPlatformMngt::LONG_REQUEST_COMPLETED );

    // Log and notify the change of status
    application->m_statusInfo = CORBA::string_dup( "" );
    application->notifyStatusChangeEvent();
}

CdmwPlatformMngt::ApplicationStatus ApplicationStopped::status()
{
    return CdmwPlatformMngt::APPLICATION_STOPPED;
}

CdmwPlatformMngt::ProcessProxy_ptr ApplicationStopped::add_process(
    ApplicationStateMachine* stateMachine,
    CdmwPlatformMngt::ProcessDef* process_def )
throw( CdmwPlatformMngt::ProcessAlreadyExists,
       CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::DuplicateEntity,
       CdmwPlatformMngt::DuplicateService,
       CdmwPlatformMngt::DuplicateStep,
       CdmwPlatformMngt::DuplicateActivityPoint,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    CdmwPlatformMngt::ProcessProxy_ptr processProxy
        = stateMachine->getContext()->addProcess( process_def );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager

    return processProxy;
}

void ApplicationStopped::remove_process(
    ApplicationStateMachine* stateMachine,
    const char* process_name )
throw( CdmwPlatformMngt::ProcessNotFound,
       CdmwPlatformMngt::ProcessStillActive,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->removeProcess( process_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void ApplicationStopped::set_init_graph(
    ApplicationStateMachine* stateMachine,
    const CdmwPlatformMngt::GraphElements& process_graph)
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setInitGraph( process_graph );
}

void ApplicationStopped::set_stop_graph(
    ApplicationStateMachine* stateMachine,
    const CdmwPlatformMngt::GraphElements& process_graph)
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->setStopGraph( process_graph );
}

void ApplicationStopped::initialise(
    ApplicationStateMachine* stateMachine,
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    stateMachine->getContext()->m_newInitGraph = true;

    // Change to the state Initialising
    stateMachine->setState( "Initialising" );
}

} // End namespace PlatformMngt
} // End namespace Cdmw


