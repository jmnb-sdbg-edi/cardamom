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


#include "platformsystem/System_impl.hpp"
#include "platformsystem/ApplicationInit.hpp"
#include "platformsystem/ApplicationStop.hpp"
#include "platformsystem/RunSetManager.hpp"
#include "platformsystem/SystemLogMessageId.hpp"
#include "SystemMngt/platformlibrary/TaskSequencer.hpp"
#include "SystemMngt/platformlibrary/GraphUtility.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/common/Assert.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

// ----------------------------------------------------------------------
// SystemState class.
// ----------------------------------------------------------------------

void SystemState::setInfo (SystemStateMachine* stateMachine,
                           const char* messageInfo)
{
     stateMachine->getContext()->m_statusInfo = CORBA::string_dup (messageInfo);
}


// ----------------------------------------------------------------------
// SystemInitial class.
// ----------------------------------------------------------------------

SystemInitial::SystemInitial ()
{
}


CdmwPlatformMngt::SystemStatus SystemInitial::status()
{
    return CdmwPlatformMngt::SYSTEM_INITIAL;
}


void SystemInitial::define (SystemStateMachine* stateMachine,
                            const CdmwPlatformMngt::SystemDef& system_def)
    throw (CdmwPlatformMngt::DuplicateEntity,
           CdmwPlatformMngt::DuplicateService,
           CdmwPlatformMngt::DuplicateHost,
           CdmwPlatformMngt::AlreadyDone,
           CdmwPlatformMngt::IncompatibleStatus,
           CORBA::SystemException)
{
    stateMachine->getContext()->defineSystem (system_def);

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager

}   
        
                   
                   
                   
CdmwPlatformMngt::Application_ptr SystemInitial::add_application (
                              SystemStateMachine* stateMachine,
                              const CdmwPlatformMngt::PlatformApplicationDef& platform_application_def,
                              const CdmwPlatformMngt::ApplicationDef& application_def)
    throw (CdmwPlatformMngt::ApplicationAlreadyExists,
           CdmwPlatformMngt::HostNotFound,
           CdmwPlatformMngt::DuplicateEntity,
           CdmwPlatformMngt::DuplicateService,
           CdmwPlatformMngt::DuplicateProcess,
           CdmwPlatformMngt::ProcessHostNotFound,
           CdmwPlatformMngt::DuplicateProcessEntity,
           CdmwPlatformMngt::DuplicateProcessService,
           CdmwPlatformMngt::DuplicateProcessStep,
           CdmwPlatformMngt::DuplicateProcessActivityPoint,
           CdmwPlatformMngt::IncompatibleStatus,
           CORBA::SystemException)
{
    CdmwPlatformMngt::Application_ptr application =
            stateMachine->getContext()->addApplication (platform_application_def,
                                                        application_def);

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager

    return application;
}

void SystemInitial::remove_application (SystemStateMachine* stateMachine,
                                        const char* application_name )    
    throw (CdmwPlatformMngt::ApplicationNotFound,
           CdmwPlatformMngt::ApplicationStillActive,
           CdmwPlatformMngt::IncompatibleStatus,
           CORBA::SystemException)
{
    stateMachine->getContext()->removeApplication (application_name);

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void SystemInitial::set_init_graph (
                               SystemStateMachine* stateMachine,   
                               const CdmwPlatformMngt::GraphElements& application_graph)
     throw (OutOfMemoryException,
            CdmwPlatformMngt::ReferenceNotFound,
            CdmwPlatformMngt::CircularReference,
            CdmwPlatformMngt::IncompatibleStatus,
            CORBA::SystemException)
{
    stateMachine->getContext()->setInitGraph (application_graph);
}



void SystemInitial::set_stop_graph (
                               SystemStateMachine* stateMachine,   
                               const CdmwPlatformMngt::GraphElements& application_graph)
     throw (OutOfMemoryException,
            CdmwPlatformMngt::ReferenceNotFound,
            CdmwPlatformMngt::CircularReference,
            CdmwPlatformMngt::IncompatibleStatus,
            CORBA::SystemException)
{
    stateMachine->getContext()->setStopGraph (application_graph);
}

void SystemInitial::set_run_set (SystemStateMachine* stateMachine,
                                 const CdmwPlatformMngt::RunSetDef& run_set_def)
      throw (CdmwPlatformMngt::DuplicateRunSetElement,
             CdmwPlatformMngt::IncompatibleStatus,
             CORBA::SystemException)
{
    stateMachine->getContext()->setRunSet (run_set_def);
}

 

void SystemInitial::remove_run_set (SystemStateMachine* stateMachine,
                                    const char* run_set_name)
      throw (CdmwPlatformMngt::RunSetNotFound,
             CdmwPlatformMngt::IncompatibleStatus,
             CORBA::SystemException)
{
    stateMachine->getContext()->removeRunSet (run_set_name);
}


void SystemInitial::remove_all_run_sets(SystemStateMachine* stateMachine)
      throw (CdmwPlatformMngt::IncompatibleStatus,
             CORBA::SystemException)
{
    stateMachine->getContext()->removeAllRunSets ();
}



CdmwPlatformMngt::HostProxy_ptr 
    SystemInitial::add_host (SystemStateMachine* stateMachine,
                             const CdmwPlatformMngt::HostDef& host_def)
      throw (CdmwPlatformMngt::HostAlreadyExists,
             CdmwPlatformMngt::IncompatibleStatus,
             CORBA::SystemException)
{
    return stateMachine->getContext()->addHost (host_def);
}



void SystemInitial::remove_host (SystemStateMachine* stateMachine,
                                 const char* host_name)
      throw (CdmwPlatformMngt::HostNotFound,
             CdmwPlatformMngt::IncompatibleStatus,
             CORBA::SystemException)
{
    stateMachine->getContext()->removeHost (host_name);
}                 










void SystemInitial::start (SystemStateMachine* stateMachine,   
                           const CdmwPlatformMngtBase::StartupKind& startup_kind)
       throw (CdmwPlatformMngt::IncompatibleStatus,
              CORBA::SystemException)
{
    try
    {
        System_impl* system = stateMachine->getContext();
        
        stateMachine->getContext()->m_newGraph = true;
        stateMachine->getContext()->m_startup_kind =
            new CdmwPlatformMngtBase::StartupKind (startup_kind);

        // Change to the state Starting
        stateMachine->setState (System_impl::M_startingStateName);
        
        // start RunSetManager thread
        system->m_runSetManager->start();
        
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
}






// ----------------------------------------------------------------------
// SystemStarting class.
// ----------------------------------------------------------------------

SystemStarting::SystemStarting ()
{
}

void SystemStarting::entryAction (SystemStateMachine* stateMachine)   
{
    System_impl* system = stateMachine->getContext();

    // Notify the change of status
    setInfo (stateMachine, "");
    system->notifyStatusChangeEvent();
    
    // Stop and destroy the stop sequencer 
    system->destroyStopSequencer();
}



void SystemStarting::exitAction (SystemStateMachine* stateMachine)
{   
    stateMachine->getContext()->m_newGraph = false;
}


void SystemStarting::doActivity (SystemStateMachine* stateMachine)   
{
    try
    {
        System_impl* system = stateMachine->getContext();
        
        // check if graph is new to create application sequencer
        if (system->m_newGraph)
        {
            // Create the application init sequencer
            std::auto_ptr <InitApplicationSuspensionCallback> suspensionCallback (
                     new InitApplicationSuspensionCallback (system));

            std::auto_ptr <InitApplicationCompletionCallback> completionCallback (
                     new InitApplicationCompletionCallback (system));

            system->m_initSyncTaskFactory =
                    InitApplicationTaskFactory::createFactory (
                                                  system,
                                                  system->m_startup_kind.in(),
                                                  system->m_applicationInitGraph.in());

            system->m_initSequencer = new Sequencer (system->m_initSyncTaskFactory);               
            system->m_initSequencer->setSuspensionCallback (suspensionCallback.release());               
            system->m_initSequencer->setCompletionCallback (completionCallback.release());
                
            // Start the init sequencer thread
            LogMngr::logMessage(INF, MSG_ID_START_APP_INIT_SEQ);
            system->m_initSequencer->start();
        }
        else
        {
            // Resume the sequencer
            LogMngr::logMessage(INF, MSG_ID_RESUME_APP_INIT_SEQ);
            system->m_initSequencer->resume();
        }
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch (const OutOfMemoryException&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch (...)
    {
        throw CORBA::INTERNAL (OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}


CdmwPlatformMngt::SystemStatus SystemStarting::status()
{
    return CdmwPlatformMngt::SYSTEM_STARTING;
}


void SystemStarting::stop (SystemStateMachine* stateMachine,    
                           CORBA::Boolean emergency)
       throw (CdmwPlatformMngt::IncompatibleStatus,
              CORBA::SystemException)
{
    // Change to the state Stopping
    stateMachine->setState(System_impl::M_stoppingStateName);
}


void SystemStarting::completion_event (SystemStateMachine* stateMachine)
       throw (CdmwPlatformMngt::IncompatibleStatus,
              CORBA::SystemException)    
{
    // Change to the state Initialised
    stateMachine->setState(System_impl::M_startedStateName);
}



void SystemStarting::suspension_event (SystemStateMachine* stateMachine) 
       throw (CdmwPlatformMngt::IncompatibleStatus,
              CORBA::SystemException)   
{
    // Change to the state StartupSuspended
    stateMachine->setState (System_impl::M_startupSuspendStateName);
}






// ----------------------------------------------------------------------
// SystemInitialisationSuspended class.
// ----------------------------------------------------------------------

SystemStartupSuspended::SystemStartupSuspended ()
{
}

void SystemStartupSuspended::entryAction (SystemStateMachine* stateMachine)    
{
    System_impl* system = stateMachine->getContext();

    // Notify the change of status
    setInfo (stateMachine, "");
    system->notifyStatusChangeEvent();    
}

CdmwPlatformMngt::SystemStatus SystemStartupSuspended::status()
{
    return CdmwPlatformMngt::SYSTEM_STARTUP_SUSPENDED;
}


void SystemStartupSuspended::set_init_graph (
                    SystemStateMachine* stateMachine,   
                    const CdmwPlatformMngt::GraphElements& application_graph)
     throw (OutOfMemoryException,
            CdmwPlatformMngt::ReferenceNotFound,
            CdmwPlatformMngt::CircularReference,
            CdmwPlatformMngt::IncompatibleStatus,
            CORBA::SystemException)
{
    System_impl* system = stateMachine->getContext();

    // Stop and destroy the init sequencer 
    system->destroyInitSequencer();

    system->setInitGraph (application_graph);
    system->m_newGraph = true;
}


void SystemStartupSuspended::set_stop_graph (
                               SystemStateMachine* stateMachine,   
                               const CdmwPlatformMngt::GraphElements& application_graph)
     throw (OutOfMemoryException,
            CdmwPlatformMngt::ReferenceNotFound,
            CdmwPlatformMngt::CircularReference,
            CdmwPlatformMngt::IncompatibleStatus,
            CORBA::SystemException)
{
    stateMachine->getContext()->setStopGraph (application_graph);
}

void SystemStartupSuspended::resume (SystemStateMachine* stateMachine) 
     throw (CdmwPlatformMngt::IncompatibleStatus,
            CORBA::SystemException)
{
    // Change to the state Starting
    stateMachine->setState (System_impl::M_startingStateName);
}


void SystemStartupSuspended::stop (SystemStateMachine* stateMachine,    
                                   CORBA::Boolean emergency)
     throw (CdmwPlatformMngt::IncompatibleStatus,
            CORBA::SystemException)
{
    // Change to the state Stopping
    stateMachine->setState(System_impl::M_stoppingStateName);
}





// ----------------------------------------------------------------------
// SystemStarted class.
// ----------------------------------------------------------------------

SystemStarted::SystemStarted ()
{    
}

void SystemStarted::entryAction (SystemStateMachine* stateMachine)    
{
    System_impl* system = stateMachine->getContext();

    // Notify the change of status
    setInfo (stateMachine, "");
    system->notifyStatusChangeEvent();   
}



void SystemStarted::doActivity (SystemStateMachine* stateMachine)   
{
    try
    {
        System_impl* system = stateMachine->getContext();
   
        // create application list not defined in a run set 
        RunSetManager::ApplicationList appList;

        // Get the list of all element of the init graph
        ElementNames elementNames =
               GraphUtility::getElementNames ((system->m_applicationInitGraph).in());
               
        ElementNames::iterator elementIt;
        
        for (elementIt = elementNames.begin();
             elementIt != elementNames.end();
             elementIt++)
        {
            if (!(system->m_runSetManager->isInRunSet (*elementIt)))
            {
                // insert application name in Application list
                appList.push_back (*elementIt);
            }
        }
        
        // start built RunSet with application list
        LogMngr::logMessage(INF, MSG_ID_START_RUNSET, "");
        system->m_runSetManager->startRunSet ("", appList);
    }
    
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch( const OutOfMemoryException& )
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch (...)
    {
        throw CORBA::INTERNAL (OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}



CdmwPlatformMngt::SystemStatus SystemStarted::status()
{
    return CdmwPlatformMngt::SYSTEM_STARTED;
}



CdmwPlatformMngt::Application_ptr SystemStarted::add_application (
                              SystemStateMachine* stateMachine,
                              const CdmwPlatformMngt::PlatformApplicationDef& platform_application_def,
                              const CdmwPlatformMngt::ApplicationDef& application_def)
    throw (CdmwPlatformMngt::ApplicationAlreadyExists,
           CdmwPlatformMngt::HostNotFound,
           CdmwPlatformMngt::DuplicateEntity,
           CdmwPlatformMngt::DuplicateService,
           CdmwPlatformMngt::DuplicateProcess,
           CdmwPlatformMngt::ProcessHostNotFound,
           CdmwPlatformMngt::DuplicateProcessEntity,
           CdmwPlatformMngt::DuplicateProcessService,
           CdmwPlatformMngt::DuplicateProcessStep,
           CdmwPlatformMngt::DuplicateProcessActivityPoint,
           CdmwPlatformMngt::IncompatibleStatus,
           CORBA::SystemException)
{
    CdmwPlatformMngt::Application_ptr application =
            stateMachine->getContext()->addApplication (platform_application_def,
                                                        application_def);

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager

    return application;
}

void SystemStarted::remove_application (SystemStateMachine* stateMachine,
                                        const char* application_name )    
    throw (CdmwPlatformMngt::ApplicationNotFound,
           CdmwPlatformMngt::ApplicationStillActive,
           CdmwPlatformMngt::IncompatibleStatus,
           CORBA::SystemException)
{
    stateMachine->getContext()->removeApplication (application_name);

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}


void SystemStarted::set_stop_graph (
                               SystemStateMachine* stateMachine,   
                               const CdmwPlatformMngt::GraphElements& application_graph)
     throw (OutOfMemoryException,
            CdmwPlatformMngt::ReferenceNotFound,
            CdmwPlatformMngt::CircularReference,
            CdmwPlatformMngt::IncompatibleStatus,
            CORBA::SystemException)
{
    stateMachine->getContext()->setStopGraph (application_graph);
}


void SystemStarted::stop (SystemStateMachine* stateMachine,   
                          CORBA::Boolean emergency)
     throw (CdmwPlatformMngt::IncompatibleStatus,
            CORBA::SystemException)
{
    // Change to the state Stopping
    stateMachine->setState(System_impl::M_stoppingStateName);
}


CdmwPlatformMngt::HostProxy_ptr 
    SystemStarted::add_host (SystemStateMachine* stateMachine,
                             const CdmwPlatformMngt::HostDef& host_def)
      throw (CdmwPlatformMngt::HostAlreadyExists,
             CdmwPlatformMngt::IncompatibleStatus,
             CORBA::SystemException)
{
    return stateMachine->getContext()->addHost (host_def);
}



void SystemStarted::remove_host (SystemStateMachine* stateMachine,
                                 const char* host_name)
      throw (CdmwPlatformMngt::HostNotFound,
             CdmwPlatformMngt::IncompatibleStatus,
             CORBA::SystemException)
{
    stateMachine->getContext()->removeHost (host_name);
}                 





// ----------------------------------------------------------------------
// SystemStopping class.
// ----------------------------------------------------------------------

SystemStopping::SystemStopping ()
{
}

void SystemStopping::entryAction (SystemStateMachine* stateMachine)   
{
    System_impl* system = stateMachine->getContext();

    // Notify the change of status
    setInfo (stateMachine, "");
    system->notifyStatusChangeEvent();
    
    // Stop and destroy the init sequencer 
    system->destroyInitSequencer();
    
    // stop RunSetManager starters
    system->m_runSetManager->stopAllRunSets();
    
    // reset all runset acknowledged elements
    system->m_runSetManager->reset();
}



void SystemStopping::doActivity (SystemStateMachine* stateMachine)    
{
    try
    {
        System_impl* system = stateMachine->getContext();

        // Create the application stop sequencer
        std::auto_ptr <StopApplicationCompletionCallback> completionCallback (
                    new StopApplicationCompletionCallback (system));

    
        // create Task factory and sequencer
        system->m_stopSyncTaskFactory = 
                       StopApplicationTaskFactory::createFactory (system,                                                         
                                                                  system->m_emergency,
                                                                  system->m_stopGraph.in());
            
        system->m_stopSequencer = new Sequencer (system->m_stopSyncTaskFactory);
        system->m_stopSequencer->setCompletionCallback (completionCallback.release());
                
        // Start the stop sequencer thread
        LogMngr::logMessage(INF, MSG_ID_START_APP_STOP_SEQ);
        system->m_stopSequencer->start();
    }
    catch(const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch (const OutOfMemoryException&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch (...)
    {
        throw CORBA::INTERNAL (OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}



CdmwPlatformMngt::SystemStatus SystemStopping::status()
{
    return CdmwPlatformMngt::SYSTEM_STOPPING;
}



void SystemStopping::completion_event (SystemStateMachine* stateMachine) 
     throw (CdmwPlatformMngt::IncompatibleStatus,
            CORBA::SystemException)  
{
    // Change to the state Stopped
    stateMachine->setState(System_impl::M_stoppedStateName);
}




// ----------------------------------------------------------------------
// SystemStopped class.
// ----------------------------------------------------------------------

SystemStopped::SystemStopped ()
{
}


void SystemStopped::entryAction (SystemStateMachine* stateMachine)    
{
    System_impl* system = stateMachine->getContext();

    // Notify the change of status
    setInfo (stateMachine, "");
    system->notifyStatusChangeEvent();     
}



CdmwPlatformMngt::SystemStatus SystemStopped::status()
{
    return CdmwPlatformMngt::SYSTEM_STOPPED;
}

CdmwPlatformMngt::Application_ptr SystemStopped::add_application (
                              SystemStateMachine* stateMachine,
                              const CdmwPlatformMngt::PlatformApplicationDef& platform_application_def,
                              const CdmwPlatformMngt::ApplicationDef& application_def)
    throw (CdmwPlatformMngt::ApplicationAlreadyExists,
           CdmwPlatformMngt::HostNotFound,
           CdmwPlatformMngt::DuplicateEntity,
           CdmwPlatformMngt::DuplicateService,
           CdmwPlatformMngt::DuplicateProcess,
           CdmwPlatformMngt::ProcessHostNotFound,
           CdmwPlatformMngt::DuplicateProcessEntity,
           CdmwPlatformMngt::DuplicateProcessService,
           CdmwPlatformMngt::DuplicateProcessStep,
           CdmwPlatformMngt::DuplicateProcessActivityPoint,
           CdmwPlatformMngt::IncompatibleStatus,
           CORBA::SystemException)
{
    CdmwPlatformMngt::Application_ptr application =
            stateMachine->getContext()->addApplication (platform_application_def,
                                                        application_def);

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager

    return application;
}

void SystemStopped::remove_application (SystemStateMachine* stateMachine,
                                        const char* application_name )    
    throw (CdmwPlatformMngt::ApplicationNotFound,
           CdmwPlatformMngt::ApplicationStillActive,
           CdmwPlatformMngt::IncompatibleStatus,
           CORBA::SystemException)
{
    stateMachine->getContext()->removeApplication (application_name);

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}


void SystemStopped::set_init_graph (
                               SystemStateMachine* stateMachine,   
                               const CdmwPlatformMngt::GraphElements& application_graph)
     throw (OutOfMemoryException,
            CdmwPlatformMngt::ReferenceNotFound,
            CdmwPlatformMngt::CircularReference,
            CdmwPlatformMngt::IncompatibleStatus,
            CORBA::SystemException)
{
    stateMachine->getContext()->setInitGraph (application_graph);
}


CdmwPlatformMngt::HostProxy_ptr 
    SystemStopped::add_host (SystemStateMachine* stateMachine,
                             const CdmwPlatformMngt::HostDef& host_def)
      throw (CdmwPlatformMngt::HostAlreadyExists,
             CdmwPlatformMngt::IncompatibleStatus,
             CORBA::SystemException)
{
    return stateMachine->getContext()->addHost (host_def);
}



void SystemStopped::remove_host (SystemStateMachine* stateMachine,
                                 const char* host_name)
      throw (CdmwPlatformMngt::HostNotFound,
             CdmwPlatformMngt::IncompatibleStatus,
             CORBA::SystemException)
{
    stateMachine->getContext()->removeHost (host_name);
}                 



void SystemStopped::start (SystemStateMachine* stateMachine,   
                           const CdmwPlatformMngtBase::StartupKind& startup_kind)
     throw (CdmwPlatformMngt::IncompatibleStatus,
            CORBA::SystemException)
{
    stateMachine->getContext()->m_newGraph = true;
    stateMachine->getContext()->m_startup_kind =
            new CdmwPlatformMngtBase::StartupKind (startup_kind);


    // Change to the state Starting
    stateMachine->setState(System_impl::M_startingStateName);
};



} // End namespace PlatformMngt
} // End namespace Cdmw

