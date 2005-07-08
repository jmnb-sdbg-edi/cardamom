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

#include "platformsystem/ServiceBinding_impl.hpp"
#include "platformsystem/SystemEntityObserver_impl.hpp"
#include "platformsystem/HostContainer_impl.hpp"
#include "platformsystem/HostContainer.hpp"
#include "platformsystem/ApplicationObserver_impl.hpp"
#include "platformsystem/ApplicationIterator_impl.hpp"

#include "platformsystem/RunSetManager.hpp"
#include "platformsystem/SystemLogMessageId.hpp"

#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "SystemMngt/platformlibrary/RWServiceDefContainer_impl.hpp"
#include "SystemMngt/platformlibrary/RWEntityContainer_impl.hpp"
#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"
#include "SystemMngt/platformlibrary/GraphChecker.hpp"
#include "SystemMngt/platformlibrary/GraphUtility.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformlibrary/TaskFactory.hpp"
#include "SystemMngt/platformlibrary/TaskSequencer.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"

#include "SystemMngt/platformvaluetypes/GraphElement_impl.hpp"
#include "SystemMngt/platformvaluetypes/SystemStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/SystemModeChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/EntityStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/HostStatusChange_impl.hpp"

#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/osthreads/ReaderLockGuard.hpp"
#include "Foundation/osthreads/WriterLockGuard.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp"

#include "platformsystem/ApplicationInit.hpp"
#include "platformsystem/ApplicationRun.hpp"
#include "platformsystem/ApplicationStop.hpp"


namespace {
    
    const int MAX_RETRY_ON_FTMGR = 4;
    const int SLEEP_RETRY_ON_FTMGR_MS = 1000;
}




namespace Cdmw
{
namespace PlatformMngt
{

// set static members

SystemInitial* System_impl::M_initial = NULL;    
SystemStarting* System_impl::M_starting = NULL;   
SystemStartupSuspended* System_impl::M_startupSuspended = NULL;   
SystemStarted* System_impl::M_started = NULL;    
SystemStopping* System_impl::M_stopping = NULL;  
SystemStopped* System_impl::M_stopped = NULL;
    
const std::string  System_impl::M_initialStateName = "initial";
const std::string  System_impl::M_startingStateName = "starting";
const std::string  System_impl::M_startupSuspendStateName = "startupSuspend";
const std::string  System_impl::M_startedStateName = "started";
const std::string  System_impl::M_stoppingStateName = "stopping";
const std::string  System_impl::M_stoppedStateName = "stopped";
    
    
    

// ----------------------------------------------------------------------
// System_impl class.
// ----------------------------------------------------------------------
#ifdef CDMW_USE_FAULTTOLERANCE
System_impl::System_impl (SupervisionEventNotifier* eventNotifier,
                          CORBA::ORB_ptr orb,
                          PortableServer::POA_ptr poa,
                          std::string faultManCorbaloc)
          throw (BadParameterException, AlreadyDoneException,
                 UnknownStateException, AlreadyExistsException,
                 OutOfResourcesException, OutOfMemoryException)
    : m_newGraph (true), m_faultManagerCorbaloc(faultManCorbaloc)
#else
System_impl::System_impl (SupervisionEventNotifier* eventNotifier,
                          CORBA::ORB_ptr orb,
                          PortableServer::POA_ptr poa)
          throw (BadParameterException, AlreadyDoneException,
                 UnknownStateException, AlreadyExistsException,
                 OutOfResourcesException, OutOfMemoryException)
          : m_newGraph (true)
#endif
{
    if (eventNotifier == NULL)
    {
        CDMW_THROW2 (BadParameterException, "eventNotifier", "NULL");
    }
    
    if (CORBA::is_nil(poa))
    {
        CDMW_THROW2 (BadParameterException, "poa", "nil");
    }

    // Set the internal data
    m_eventNotifier = eventNotifier;
    m_orb = CORBA::ORB::_duplicate(orb);
    m_poa = PortableServer::POA::_duplicate(poa);
    
#ifdef CDMW_USE_FAULTTOLERANCE
    // get the FT manager
    CORBA::Object_var fm_obj = orb->string_to_object( faultManCorbaloc.c_str() );
    if( CORBA::is_nil( fm_obj.in() ) )
        CDMW_THROW2(Cdmw::Exception,
                    Cdmw::Exception::FATAL,
                    "Invalid fault manager corbaloc" );    

    
    CdmwFT::FTManager_var fm;
    try {
        fm = ::CdmwFT::FTManager::_narrow(fm_obj.in() );
    } catch (const CORBA::SystemException & e) {
        std::cerr << "Exception while contacting FTManager: "
                  << e << "\n"
                  << faultManCorbaloc << " is probably invalid"
                  << std::endl;
        CDMW_THROW2(Cdmw::Exception,
                    Cdmw::Exception::FATAL,
                    "Invalid fault manager corbaloc" );
    }

    if( CORBA::is_nil( fm.in() ) )
        CDMW_THROW2(Cdmw::Exception,
                    Cdmw::Exception::FATAL,
                    "Invalid fault manager reference" );
    try{
        
        bool ftmgr_not_active = true;
        int nbRetries = 0;
        
        do {
            try {
                // get the replication manager from the FTManager
                CORBA::Object_var rm_obj = fm->get_replication_manager();
                m_replicationMngt = CdmwFT::ReplicationManager::_narrow(rm_obj.in() );
                ftmgr_not_active = false;
            } catch (const CdmwFT::FTManager::NoAlreadyActivated&) {
                if (++nbRetries > MAX_RETRY_ON_FTMGR) {
                    std::cerr << "ERROR: FTManager not yet activated." << std::endl;
                    throw;
                }
                std::cerr << "Retry to contact FTManager in " << SLEEP_RETRY_ON_FTMGR_MS 
                          << " ms ..." << std::endl;
                Cdmw::OsSupport::OS::sleep(SLEEP_RETRY_ON_FTMGR_MS);
            }
        } while (ftmgr_not_active);
        
       
        m_replicationManagerIOGR = orb->object_to_string(m_replicationMngt.in());

        // get the fault notifier from the FTManager
        CORBA::Object_var fn_obj = fm->get_fault_notifier();
        
        m_faultNotifier = FT::FaultNotifier::_narrow(fn_obj.in());
        
        // get the fault detector from the FTManager
        CORBA::Object_var fd_obj = fm->get_fault_detector();
        
        m_faultDetector = CdmwFT::FaultManagement::FaultDetector::_narrow(fd_obj.in());
    }
    catch(const CdmwFT::FTManager::NoAlreadyActivated&)
    {
        CDMW_THROW2 (BadParameterException, "fault manager", "Not the primary");
    }
    

#endif

    m_statusInfo = CORBA::string_dup ("");
    
    m_mode = CdmwPlatformMngt::NORMAL_MODE;
    m_modeInfo = CORBA::string_dup ("");
    
    m_systemDefined = false;
    m_systemName = CORBA::string_dup ("");
   
    m_emergency = false;
     
    m_actualServiceDefContainer = NULL;
    m_actualEntityContainer = NULL;
           
    m_initSyncTaskFactory = NULL;
    m_initSequencer = NULL;  
    
    m_stopSyncTaskFactory = NULL;
    m_stopSequencer = NULL; 
         
    m_stateMachine = NULL;
    m_rwServiceDefContainer = NULL;
    m_rwEntityContainer = NULL;             
    m_actualHostContainer = NULL;    
    m_runSetManager = NULL;
    
    m_sysStatusChangeFactory = NULL;
    m_sysModeChangeFactory = NULL;
    m_sysEntityStatusChangeFactory = NULL;
    m_hostStatusChangeFactory = NULL;
        
    try
    {       
        // create an empty init application graph
        std::auto_ptr <CdmwPlatformMngt::GraphElements> applicationInitGraph (
                                 new CdmwPlatformMngt::GraphElements);
                
        // create an empty stop application graph
        std::auto_ptr <CdmwPlatformMngt::GraphElements> applicationStopGraph (
                                 new CdmwPlatformMngt::GraphElements);
                                 
        // create an empty stop emergency graph
        std::auto_ptr <CdmwPlatformMngt::GraphElements> stopGraph (
                                 new CdmwPlatformMngt::GraphElements);
                
        
        
        // create local objects        
        std::auto_ptr <SystemStateMachine > stateMachine (
                                new SystemStateMachine (this, "System"));
                                                                                 
#ifdef CDMW_USE_FAULTTOLERANCE
        std::auto_ptr <PlatformMngt::HostContainer> actualHostContainer (
            new PlatformMngt::HostContainer(this, poa, m_faultDetector.in()));     
#else
        std::auto_ptr <PlatformMngt::HostContainer> actualHostContainer (
            new PlatformMngt::HostContainer(this, poa));        
#endif   
        std::auto_ptr <RunSetManager> runSetManager (
            new RunSetManager(this));
            
            
        std::auto_ptr <SystemStatusChangeFactory> sysStatusChangeFactory (
            new SystemStatusChangeFactory());
        std::auto_ptr <SystemModeChangeFactory> sysModeChangeFactory (
            new SystemModeChangeFactory());    
        std::auto_ptr <SystemEntityStatusChangeFactory> sysEntityStatusChangeFactory (
            new SystemEntityStatusChangeFactory());
        std::auto_ptr <HostStatusChangeFactory> hostStatusChangeFactory (
            new HostStatusChangeFactory());

      
        // create system service def and entity containers
        m_actualServiceDefContainer = 
                   ServiceDefContainer_impl::ServiceDefContainer::createSystemContainer();
                    
        m_actualEntityContainer = 
                   EntityContainer_impl::EntityContainer::createSystemContainer();
                   
     
        // Create the local RWServiceDefContainer
        PortableServer::ServantBase_var rwServiceDefContainer = 
	           new RWServiceDefContainer_impl (m_poa.in(), m_actualServiceDefContainer);
	    
	    // Create the local RWEntityContainer       
	    PortableServer::ServantBase_var rwEntityContainer =
               new RWEntityContainer_impl (m_poa.in(), m_actualEntityContainer);
        RWEntityContainer_impl* entityContainer =
               dynamic_cast<RWEntityContainer_impl*>(rwEntityContainer.in());
               
        
        // Create Object servants
            
        // Create the ServiceDefContainer servant
        m_serviceDefContainer = 
	           new ServiceDefContainer_impl (m_poa.in(), m_actualServiceDefContainer);
        // create an object var to take pointer ownership
        PortableServer::ServantBase_var serviceDefContainerServant = m_serviceDefContainer;        
        // activate object
        m_serviceDefContainer->activate();
        m_serviceDefContainerObject = m_serviceDefContainer->_this();
        
        
        // Create the ServiceBinding servant
        m_serviceBinding = new ServiceBinding_impl (this);			             
        // create an object var to take pointer ownership
        PortableServer::ServantBase_var serviceBindingServant = m_serviceBinding;        
        // activate object
        m_serviceBindingObject = m_serviceBinding->_this();
        
        
        // Create the SystemEntityObserver servant
        m_entityObserver = new SystemEntityObserver_impl (this, entityContainer);				             
        // create an object var to take pointer ownership
        PortableServer::ServantBase_var entityObserverServant = m_entityObserver;        
        // activate object
        m_entityObserverObject = m_entityObserver->_this();
        
        
        // Create the HostContainer servant
        m_hostContainer = new HostContainer_impl (actualHostContainer.get());				             
        // create an object var to take pointer ownership
        PortableServer::ServantBase_var hostContainerServant = m_hostContainer;        
        // activate object
        m_hostContainerObject = m_hostContainer->_this();
        
        
        // Create the ApplicationObserver servant
        m_applicationObserver = new ApplicationObserver_impl (this);				             
        // create an object var to take pointer ownership
        PortableServer::ServantBase_var applicationObserverServant = m_applicationObserver;        
        // activate object
        m_applicationObserverObject = m_applicationObserver->_this();
        
#ifdef CDMW_USE_FAULTTOLERANCE
        // Create the FTConverter servant
        m_FTConverter = new FTConverter_impl (m_hostContainer);				             
        // create an object var to take pointer ownership
        PortableServer::ServantBase_var FTConverter = m_FTConverter;        
        // activate object
        m_FTConverterObject = m_FTConverter->_this();
        
        CosNotifyComm::StructuredPushConsumer_var system_consumer
            = CosNotifyComm::StructuredPushConsumer::_narrow(m_FTConverterObject.in());
        m_faultNotifier->connect_structured_fault_consumer(system_consumer.in());
#endif

        // Set the internal data   
        m_applicationInitGraph = applicationInitGraph.get(); 
        m_applicationStopGraph = applicationStopGraph.get();
        m_stopGraph = stopGraph.get();
        m_stateMachine = stateMachine.get();                         
        m_actualHostContainer = actualHostContainer.get();       
        m_runSetManager = runSetManager.get();  
        m_sysStatusChangeFactory = sysStatusChangeFactory.get();
        m_sysModeChangeFactory = sysModeChangeFactory.get();
        m_sysEntityStatusChangeFactory = sysEntityStatusChangeFactory.get(); 
        m_hostStatusChangeFactory = hostStatusChangeFactory.get();  
        
        m_rwServiceDefContainer = 
            dynamic_cast<RWServiceDefContainer_impl*>(rwServiceDefContainer.in()); 
        m_rwEntityContainer =
            dynamic_cast<RWEntityContainer_impl*>(rwEntityContainer.in());


        // Add the states to the state machine
        m_stateMachine->addState (M_initialStateName, M_initial);
        m_stateMachine->addState (M_startingStateName, M_starting);
        m_stateMachine->addState (M_startupSuspendStateName, M_startupSuspended);            
        m_stateMachine->addState (M_startedStateName, M_started);
        m_stateMachine->addState (M_stoppingStateName, M_stopping);
        m_stateMachine->addState (M_stoppedStateName, M_stopped);

        // Set the initial state
        m_stateMachine->setState (M_initialStateName);
        
         
        // start event handler
        m_eventHandler.start();
 
        
        // Release the temporary objects
        applicationInitGraph.release(); 
        applicationStopGraph.release();
        stopGraph.release();
        stateMachine.release();                      
        actualHostContainer.release();           
        runSetManager.release(); 
        sysStatusChangeFactory.release();
        sysModeChangeFactory.release();
        sysEntityStatusChangeFactory.release();
        hostStatusChangeFactory.release();
        
        rwServiceDefContainer._retn();
        rwEntityContainer._retn();
            
    }
    catch (const std::bad_alloc&)
    {
        // destroy containers
        try
        {
            ServiceDefContainer_impl::ServiceDefContainer::destroy (
                                                  m_actualServiceDefContainer);
            EntityContainer_impl::EntityContainer::destroy (
                                                  m_actualEntityContainer);
        }
        catch (...)
        {
        }
        
        CDMW_THROW (OutOfMemoryException);
    }
}



System_impl::~System_impl()
{
    // stop event handler
    m_eventHandler.stop();
    
    // destroy init and stop sequencers
    destroyInitSequencer();
    destroyStopSequencer();
    
    // destroy RunSet manager
    destroyRunSetManager();
    
    // destroy containers
    try
    {
        ServiceDefContainer_impl::ServiceDefContainer::destroy (
                                                  m_actualServiceDefContainer);
        EntityContainer_impl::EntityContainer::destroy (
                                                  m_actualEntityContainer);
    }
    catch (...)
    {
    }
 
                  
    if (m_stateMachine != NULL)
    {
        delete m_stateMachine;
    }
        
    if (m_rwServiceDefContainer != NULL)
    {
        // remove reference before deletion
        m_rwServiceDefContainer->_remove_ref();
    }        
        
    if (m_rwEntityContainer != NULL)
    {
        // remove reference before deletion
        m_rwEntityContainer->_remove_ref();
    }
    
    if (m_actualHostContainer != NULL)
    {
        delete m_actualHostContainer;
    }   
        
    if (m_sysStatusChangeFactory != NULL)
    {
        m_sysStatusChangeFactory->_remove_ref();
    }
    
    if (m_sysModeChangeFactory != NULL)
    {
        m_sysModeChangeFactory->_remove_ref();
    }
    
    if (m_sysEntityStatusChangeFactory != NULL)
    {
        m_sysEntityStatusChangeFactory->_remove_ref();
    }   
    
    if (m_hostStatusChangeFactory != NULL)
    {
        m_hostStatusChangeFactory->_remove_ref();
    } 
}


void System_impl::destroyInitSequencer()
{
    // Stop and destroy the init sequencer 
    if (m_initSequencer != NULL)
    {
        m_initSequencer->stop();

        delete m_initSequencer;
        m_initSequencer = NULL;
    }
    
    if(m_initSyncTaskFactory != NULL)
    {
        delete m_initSyncTaskFactory;
        m_initSyncTaskFactory = NULL;
    }
}

void System_impl::destroyStopSequencer()
{
    // Stop and destroy the stop sequencer 
    if (m_stopSequencer != NULL)
    {
        m_stopSequencer->stop();

        delete m_stopSequencer;
        m_stopSequencer = NULL;
    }
    
    if(m_stopSyncTaskFactory != NULL)
    {
        delete m_stopSyncTaskFactory;
        m_stopSyncTaskFactory = NULL;
    }
}


void System_impl::destroyRunSetManager()
{
    if (m_runSetManager != NULL)
    {
        // stop RunSetManager starters
        m_runSetManager->stopAllRunSets();
        
        // stop thread
        m_runSetManager->stop();
        
        delete m_runSetManager;
        m_runSetManager = NULL;
    } 
}


void System_impl::createStates()
               throw (OutOfMemoryException)
{
    try
    {
        std::auto_ptr <SystemInitial> initial (new SystemInitial());           
        std::auto_ptr <SystemStarting> starting (new SystemStarting());            
        std::auto_ptr <SystemStartupSuspended> startupSuspended (
                       new SystemStartupSuspended());               
        std::auto_ptr <SystemStarted> started (new SystemStarted());            
        std::auto_ptr <SystemStopping> stopping (new SystemStopping());           
        std::auto_ptr <SystemStopped> stopped (new SystemStopped());
            


        if (M_initial == NULL)
        {
            M_initial = initial.get();
        }
        if (M_starting == NULL)
        {
            M_starting = starting.get();
        }
        if (M_startupSuspended == NULL)
        {
            M_startupSuspended = startupSuspended.get();
        }
        if (M_started == NULL)
        {
            M_started = started.get();
        }
        if (M_stopping == NULL)
        {
            M_stopping = stopping.get();
        }
        if (M_stopped == NULL)
        {
            M_stopped = stopped.get();
        }

        initial.release();
        starting.release();
        startupSuspended.release();
        started.release();
        stopping.release();
        stopped.release();

    }
    catch (const std::bad_alloc&)
    {
        CDMW_THROW (OutOfMemoryException);
    }
}

void System_impl::destroyStates()
{
    if (M_initial != NULL)
    {
        delete M_initial;
        M_initial = NULL;
    }

    if (M_starting != NULL)
    {
        delete M_starting;
        M_starting = NULL;
    }

    if (M_startupSuspended != NULL)
    {
        delete M_startupSuspended;
        M_startupSuspended = NULL;
    }

    if (M_started != NULL)
    {
        delete M_started;
        M_started = NULL;
    }

    if (M_stopping != NULL)
    {
        delete M_stopping;
        M_stopping = NULL;
    }

    if (M_stopped != NULL)
    {
        delete M_stopped;
        M_stopped = NULL;
    }

}

void System_impl::notifyStatusChangeEvent()
{
    try
    {
        // Note : lock must be set by state machine caller
        
        // create header
        CdmwPlatformMngtBase::EventHeader header = 
                EventHeaderFactory::createHeader(CdmwPlatformMngtBase::INF);

        // get status
        CdmwPlatformMngt::SystemStatus systemStatus = 
                            m_stateMachine->getState()->status();
                            
        // create event value type using its factory        
        CdmwPlatformMngt::SystemStatusChange_var event = 
               m_sysStatusChangeFactory->create (header, m_systemName.in(), 
                                                 systemStatus, m_statusInfo.in());
                                                 
        // Trace to the log manager (must be done before addEvent)
        CORBA::String_var eventStr = event->to_string();
        LogMngr::logMessage(eventStr.in());
        
        // Perform the status change notification
        m_eventNotifier->addEvent (event);
        
        // release owner ship if no error
        event._retn();
         
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
}

void System_impl::notifyModeChangeEvent()
{
    try
    {
        // Note : lock on mode must be set by caller
        
        // create header
        CdmwPlatformMngtBase::EventHeader header = 
                EventHeaderFactory::createHeader(CdmwPlatformMngtBase::INF);

        // create event value type using its factory
        CdmwPlatformMngt::SystemModeChange_var event = 
               m_sysModeChangeFactory->create (header, m_systemName.in(), 
                                               m_mode, m_modeInfo.in());
                                                          
        // Trace to the log manager (must be done before addEvent)
        CORBA::String_var eventStr = event->to_string();
        LogMngr::logMessage(eventStr.in());
        
        // Perform the status change notification
        m_eventNotifier->addEvent (event);
        
        // release owner ship if no error
        event._retn();
         
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
}



void System_impl::notifyEntityStatusChangeEvent (
                     const char* entity_name,
                     CdmwPlatformMngtEntity::EntityStatus entity_status,
                     const char* entity_info)
{
    try
    {
        // create header
        CdmwPlatformMngtBase::EventHeader header = 
                EventHeaderFactory::createHeader(CdmwPlatformMngtBase::WRN);

        // create event value type using its factory
        CdmwPlatformMngt::SystemEntityStatusChange_var event = 
               m_sysEntityStatusChangeFactory->create (
                             header, m_systemName.in(), 
                             entity_name, entity_status, entity_info);

        // Trace to the log manager (must be done before addEvent)
        CORBA::String_var eventStr = event->to_string();
        LogMngr::logMessage(eventStr.in());  
        
        // Perform the status change notification
        m_eventNotifier->addEvent (event);
        
        // release owner ship if no error
        event._retn();
            
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
}


void System_impl::notifyHostStatusChangeEvent (
                     const char* host_name,
                     CdmwPlatformMngt::HostStatus host_status)
{
    try
    {
        // create header
        CdmwPlatformMngtBase::EventHeader header = 
                EventHeaderFactory::createHeader(CdmwPlatformMngtBase::WRN);

        // create event value type using its factory
        CdmwPlatformMngt::HostStatusChange_var event = 
               m_hostStatusChangeFactory->create (
                             header, m_systemName.in(), 
                             host_name, host_status);
     
        // Trace to the log manager (must be done before addEvent)
        CORBA::String_var eventStr = event->to_string();
        LogMngr::logMessage(eventStr.in());     
        
        // Perform the status change notification
        m_eventNotifier->addEvent (event);
        
        // release owner ship if no error
        event._retn();
                
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
}

void System_impl::defineSystem (const CdmwPlatformMngt::SystemDef& system_def)
        throw (CdmwPlatformMngt::DuplicateEntity,
               CdmwPlatformMngt::DuplicateService,
               CdmwPlatformMngt::DuplicateHost,
               CdmwPlatformMngt::AlreadyDone,
               CORBA::SystemException)
{
    CDMW_WRITER_LOCK_GUARD (m_rwSystemLock);
    
    if (!m_systemDefined)
    {     
        CORBA::ULong i;

        // get system name
        m_systemName = system_def.system_name;
    
        // get system port
        m_systemPort = system_def.system_port;
    
    
        for (i=0 ; i < system_def.entities.length() ; i++)
        {
            try
            {
                // add entity in container
                m_rwEntityContainer->add_entity(system_def.entities[i]);
            }
            catch (const CdmwPlatformMngtEntity::EntityAlreadyExists &)
            {
                throw CdmwPlatformMngt::DuplicateEntity();
            }
        }
    
        for (i=0 ; i < system_def.service_definitions.length() ; i++)
        {
            try
            {
                // add service def in container
                m_rwServiceDefContainer->add_service_def(system_def.service_definitions[i]);
            }
            catch (const CdmwPlatformMngtService::ServiceAlreadyExists &)
            {
                throw CdmwPlatformMngt::DuplicateService();
            }
        }
    
        for (i=0 ; i < system_def.hosts.length() ; i++)
        {
            try
            {
                // add host in container
                CdmwPlatformMngt::HostProxy_var hostProxy = addHost (system_def.hosts[i]);
            }
            catch (const CdmwPlatformMngt::HostAlreadyExists &)
            {
                throw CdmwPlatformMngt::DuplicateHost();
            }
        }
    
        // set system is defined
        m_systemDefined = true;
    }
    else
    {
        throw CdmwPlatformMngt::AlreadyDone ();
    }
}


void System_impl::setInitGraph (const CdmwPlatformMngt::GraphElements& application_graph)
            throw (OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CORBA::SystemException)
{
    try
    {
        CDMW_WRITER_LOCK_GUARD (m_rwSystemLock);
        
        // Check the validity of the init graph of applications
        
        // fill existing element map with application name (step is not used)
        ExistingElementMap existingElementMap;        
        Applications::iterator applicationIt;
        
        {            
            for (applicationIt =  m_applications.begin();
                 applicationIt != m_applications.end();
                 applicationIt++)
            {
                // insert in map the application name
                existingElementMap.insert (PlatformMngt::ExistingElementMap::value_type (
                                           applicationIt->first, 0));
         
            }
        }

        GraphChecker applicationGraph (application_graph, &existingElementMap, true);

        // Actually sets the graph of applications to this system
        // (the previous init graph is deleted by var mechanism)
        m_applicationInitGraph = new CdmwPlatformMngt::GraphElements (application_graph);

        // TODO: Notify the configuration change

        // TODO: Trace to the log manager
    }
    catch (const CdmwPlatformMngt::StepOutOfBound&)
    {
        // nothing to do
    }
    catch (const CdmwPlatformMngt::StepJump&)
    {
        // nothing to do
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
}


void System_impl::setStopGraph (const CdmwPlatformMngt::GraphElements& application_graph)
            throw (OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CORBA::SystemException)
{
    try
    {
        CDMW_WRITER_LOCK_GUARD (m_rwSystemLock);
        
        // Check the validity of the stop graph of applications
        
        // fill existing element map with application name (step is not used)
        ExistingElementMap existingElementMap;        
        Applications::iterator applicationIt;
        
        {
            for (applicationIt =  m_applications.begin();
                 applicationIt != m_applications.end();
                 applicationIt++)
            {
                // insert in map the application name
                existingElementMap.insert (PlatformMngt::ExistingElementMap::value_type (
                                           applicationIt->first, 0));
         
            }
        }

        GraphChecker applicationGraph (application_graph, &existingElementMap, true);

        // Actually sets the graph of applications to this system
        // (the previous stop graph is deleted by var mechanism)
        m_applicationStopGraph = new CdmwPlatformMngt::GraphElements (application_graph);

        // TODO: Notify the configuration change

        // TODO: Trace to the log manager
    }
    catch (const CdmwPlatformMngt::StepOutOfBound&)
    {
        // nothing to do
    }
    catch (const CdmwPlatformMngt::StepJump&)
    {
        // nothing to do
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
}


void System_impl::setRunSet(const CdmwPlatformMngt::RunSetDef& run_set_def)
      throw (CdmwPlatformMngt::DuplicateRunSetElement,
             CORBA::SystemException)
{
    try
    {
        m_runSetManager->setRunSet (run_set_def);
    }
    catch (Cdmw::OutOfResourcesException&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
}
     

void System_impl::removeRunSet(const char* run_set_name)
      throw (CdmwPlatformMngt::RunSetNotFound,
             CORBA::SystemException)
{
    m_runSetManager->removeRunSet (run_set_name);
}


void System_impl::removeAllRunSets()
{
    m_runSetManager->removeAllRunSets ();
}


CdmwPlatformMngt::HostProxy_ptr 
      System_impl::addHost(const CdmwPlatformMngt::HostDef& host_def)
       throw (CdmwPlatformMngt::HostAlreadyExists,
              CORBA::SystemException)
{
    // add host in container
    return m_actualHostContainer->addHost (host_def);
}


void System_impl::removeHost(const char* host_name)
       throw (CdmwPlatformMngt::HostNotFound,
              CORBA::SystemException)
{
    // remove host from container
    m_actualHostContainer->removeHost (host_name);
}
                  


CdmwPlatformMngt::Application_ptr 
      System_impl::addApplication(
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
                  CORBA::SystemException)
{ 
    // add application according to centralised/distributed supervision
    return doAddApplication (platform_application_def, application_def);  
}
    

void System_impl::removeApplication (const char* application_name)
            throw (CdmwPlatformMngt::ApplicationNotFound,
                   CdmwPlatformMngt::ApplicationStillActive,
                   CORBA::SystemException)
{
    // remove application according to centralised/distributed supervision
    doRemoveApplication (application_name);
}



    
void System_impl::updateApplications (const char* host_name)
            throw ()
{   
    Applications::iterator applicationIt;
    
    CDMW_READER_LOCK_GUARD (m_rwSystemLock);
    
    // update each application with faulty host name
    for (applicationIt=m_applications.begin();
         applicationIt != m_applications.end();
         applicationIt++)
    {
        try
        {
            ((applicationIt->second).m_applicationControl)->update_application(host_name);
        }
        catch (...)
        {
        }
    }   
}            
           
            
            
            
            
EventHandler* System_impl::getEventHandler()
{
    return &m_eventHandler;
}



void System_impl::degradation_event()
{
    CDMW_MUTEX_GUARD (m_mutexMode);
    
    m_mode = CdmwPlatformMngt::DEGRADED_MODE;
    m_modeInfo = CORBA::string_dup ("");
    
    // Notify the change of mode
    notifyModeChangeEvent();
}


void System_impl::suspension_event()
{
    CDMW_SM_WRITER_LOCK_GUARD(*m_stateMachine);
    
    // send event to state machine for transition
    m_stateMachine->getState()->suspension_event (m_stateMachine);
}

void System_impl::completion_event()
{
    CDMW_SM_WRITER_LOCK_GUARD(*m_stateMachine);
    
    // send event to state machine for transition
    m_stateMachine->getState()->completion_event (m_stateMachine);
}
    
    

CdmwPlatformMngt::Port System_impl::systemPort()
{
    return m_systemPort;
}

    
    
CdmwPlatformMngt::SystemName System_impl::name()
throw (CORBA::SystemException)
{
    CORBA::String_var name = m_systemName;

    return name._retn();
}

CdmwPlatformMngt::SystemStatus System_impl::get_status(CORBA::String_out status_info)
throw( CORBA::SystemException )
{
    try
    {
        CDMW_SM_READER_LOCK_GUARD(*m_stateMachine);
    
        // get status
        CdmwPlatformMngt::SystemStatus systemStatus = 
                            m_stateMachine->getState()->status();
                            
        
        // get info associated to status
        CORBA::String_var info = m_statusInfo;    
        status_info = info._retn();

        return systemStatus;
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
}



CdmwPlatformMngt::FunctioningMode System_impl::get_mode(CORBA::String_out mode_info)
throw( CORBA::SystemException )
{
    try
    {
        CDMW_MUTEX_GUARD (m_mutexMode);
    
        // get mode
        CdmwPlatformMngt::FunctioningMode functioningMode = m_mode;
    
        // get info associated to mode
        CORBA::String_var info = m_modeInfo;    
        mode_info = info._retn();

        return functioningMode;
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
}



CdmwPlatformMngt::SystemSnapshot* System_impl::get_snapshot()
throw( CORBA::SystemException )
{
    // Create a system snapshot
    try
    {
    
        CdmwPlatformMngt::SystemSnapshot_var snapshot =
                   new CdmwPlatformMngt::SystemSnapshot;

        snapshot->system_status = get_status ((snapshot->status_info).out());
        snapshot->system_mode = get_mode ((snapshot->mode_info).out());
        
        // Get the snapshot of all hosts
        CdmwPlatformMngt::HostsSnapshot_var hostsSnapshot =
                   m_actualHostContainer->getSnapshot();
        snapshot->system_hosts = hostsSnapshot.in();

        // Get the snapshot of all applications
        {
            CDMW_READER_LOCK_GUARD (m_rwSystemLock);
            snapshot->system_applications.length (m_applications.size());
            Applications::iterator applicationIt;
            unsigned int i;
        
            for (applicationIt=m_applications.begin(), i=0;
                 applicationIt != m_applications.end();
                 applicationIt++, i++)
            {
                snapshot->system_applications[i] =
                          *(((applicationIt->second).m_application)->get_snapshot());
            }
        }

        
        // Gets all entities
        CdmwPlatformMngtEntity::Entities_var entities;
        CdmwPlatformMngtEntity::EntitiesIterator_var entitiesIt;
    
        get_all_entities (get_number_of_entities(),       
                          entities.out(),
                          entitiesIt.out());
        
        snapshot->system_entities = entities.in();
                     
        return snapshot._retn();
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch (const Cdmw::OutOfResourcesException&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
}


CdmwPlatformMngt::Applications* System_impl::get_applications (
                          CORBA::ULong how_many,
                          CdmwPlatformMngt::ApplicationsIterator_out applications_iterator)
throw( CORBA::SystemException )
{
    try
    {
        CDMW_READER_LOCK_GUARD (m_rwSystemLock);
        
        Applications::iterator applicationIt;

        // create sequence
        CdmwPlatformMngt::Applications_var applications =
                          new CdmwPlatformMngt::Applications();
      
        // Get the how_many application references   
        size_t len = m_applications.size();
                
        // set length of requested sequence and rest of sequence
        size_t requested_len = len;
        size_t rest_len = 0;
         
        // adjust to how_many requested       
        if (len > how_many)
        {
            requested_len = how_many;
            rest_len = len - how_many;
        }
      
        // set sequence length 
        applications->length (requested_len);  
          
        // store requested application reference objects         
        unsigned int count;
        for (applicationIt=m_applications.begin(), count=0 ;
             count < requested_len ;
             applicationIt++, count++)
        {
            // Store application reference in sequence (duplicate is processed 
            // during var copy)
            applications[count] = (applicationIt->second).m_application;                                           
        }

        // Get an iterator on the rest of the applications
        std::list<CdmwPlatformMngt::Application_var> applicationsRemaining;
        for (; applicationIt != m_applications.end(); applicationIt++)
        {
            // Store remaining application reference in list (duplicate is processed 
            // during var copy)
                
            applicationsRemaining.push_back ((applicationIt->second).m_application);
        }

        CdmwPlatformMngt::ApplicationsIterator_var applicationsRemainingIt =
              ApplicationsIterator_impl::create (applicationsRemaining);

        // Assign the iterator to the processes_iterator
        applications_iterator = applicationsRemainingIt._retn();

        return applications._retn();   
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
}





CdmwPlatformMngt::Application_ptr System_impl::get_application (
                          const char* application_name)
        throw (CdmwPlatformMngt::ApplicationNotFound,
               CORBA::SystemException)
{
    CDMW_READER_LOCK_GUARD (m_rwSystemLock);
    
    // search application in map
    Applications::iterator applicationIt = 
                 m_applications.find (application_name);

    // if application not found
    if (applicationIt == m_applications.end())
    {
        throw CdmwPlatformMngt::ApplicationNotFound();
    }

    // return a duplicated object reference
    // (duplicate is implicit in var copy)
    CdmwPlatformMngt::Application_var application = (applicationIt->second).m_application;
    return application._retn();
}


CdmwPlatformMngt::SupervisionObserver_ptr 
       System_impl::register_observer(const char* observer_name,
                                      CdmwPlatformMngt::SupervisionObserver_ptr observer)
throw( CORBA::SystemException )
{
    try
    {
        LogMngr::logMessage(INF, MSG_ID_SYS_REG_OBSERVER, 
                            observer_name, m_systemName.in());
        
        CdmwPlatformMngt::SupervisionObserver_var previousObserver =
            m_eventNotifier->registerObserver (observer_name, observer);
            
        return previousObserver._retn();
    }
    catch (const BadParameterException &)
    {
        throw CORBA::BAD_PARAM (OrbSupport::BAD_PARAM, CORBA::COMPLETED_NO);
    }
    catch (const EventNotifierStoppedException&)
    {
        throw CORBA::INTERNAL (OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
    catch (const OutOfMemoryException&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
}

CdmwPlatformMngt::SupervisionObserver_ptr 
       System_impl::unregister_observer(const char* observer_name)
        throw (CdmwPlatformMngt::ObserverNotFound,
               CORBA::SystemException)
{
    try
    {
        LogMngr::logMessage(INF, MSG_ID_SYS_UNREG_OBSERVER, 
                            observer_name, m_systemName.in());
                            
        CdmwPlatformMngt::SupervisionObserver_var observer =
            m_eventNotifier->unregisterObserver (observer_name);
            
        return observer._retn();
    }
    catch (const EventNotifierStoppedException&)
    {
        throw CORBA::INTERNAL (OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}


void System_impl::define(const CdmwPlatformMngt::SystemDef& system_def)
        throw (CdmwPlatformMngt::DuplicateEntity,
               CdmwPlatformMngt::DuplicateService,
               CdmwPlatformMngt::DuplicateHost,
               CdmwPlatformMngt::AlreadyDone,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    LogMngr::logMessage(INF, MSG_ID_DEFINE_SYSTEM, system_def.system_name.in());
    
    CDMW_SM_READER_LOCK_GUARD(*m_stateMachine);
    
    m_stateMachine->getState()->define (m_stateMachine, system_def);
}


void System_impl::set_init_graph (const CdmwPlatformMngt::GraphElements& application_graph)                  
        throw (CdmwPlatformMngt::ReferenceNotFound,
               CdmwPlatformMngt::CircularReference,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    LogMngr::logMessage(INF, MSG_ID_SET_INITGRAPH, m_systemName.in());
    
    CDMW_SM_READER_LOCK_GUARD(*m_stateMachine);
            
    m_stateMachine->getState()->set_init_graph (m_stateMachine, application_graph);
}


void System_impl::set_stop_graph (const CdmwPlatformMngt::GraphElements& application_graph)                  
        throw (CdmwPlatformMngt::ReferenceNotFound,
               CdmwPlatformMngt::CircularReference,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    LogMngr::logMessage(INF, MSG_ID_SET_STOPGRAPH, m_systemName.in());
    
    CDMW_SM_READER_LOCK_GUARD(*m_stateMachine);
              
    m_stateMachine->getState()->set_stop_graph (m_stateMachine, application_graph);
}



void System_impl::set_run_set(const CdmwPlatformMngt::RunSetDef& run_set_def)
        throw (CdmwPlatformMngt::DuplicateRunSetElement,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    LogMngr::logMessage(INF, MSG_ID_SET_RUNSET, m_systemName.in());
    
    CDMW_SM_READER_LOCK_GUARD(*m_stateMachine);
    
    m_stateMachine->getState()->set_run_set (m_stateMachine,
                                             run_set_def);
}

void System_impl::remove_run_set(const char* run_set_name)
        throw (CdmwPlatformMngt::RunSetNotFound,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    LogMngr::logMessage(INF, MSG_ID_REMOVE_RUNSET, m_systemName.in());
    
    CDMW_SM_READER_LOCK_GUARD(*m_stateMachine);
    
    m_stateMachine->getState()->remove_run_set (m_stateMachine,
                                                run_set_name);
}

void System_impl::remove_all_run_sets()
        throw (CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    LogMngr::logMessage(INF, MSG_ID_REMOVE_ALL_RUNSET, m_systemName.in());
    
    CDMW_SM_READER_LOCK_GUARD(*m_stateMachine);
    
    m_stateMachine->getState()->remove_all_run_sets (m_stateMachine);
}

CdmwPlatformMngt::HostProxy_ptr 
      System_impl::add_host(const CdmwPlatformMngt::HostDef& host_def)
        throw (CdmwPlatformMngt::HostAlreadyExists,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    LogMngr::logMessage(INF, MSG_ID_ADD_HOST, m_systemName.in());
    
    CDMW_SM_READER_LOCK_GUARD(*m_stateMachine);
    
    return m_stateMachine->getState()->add_host (m_stateMachine,
                                                 host_def);
}

void System_impl::remove_host(const char* host_name)
        throw (CdmwPlatformMngt::HostNotFound,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    LogMngr::logMessage(INF, MSG_ID_REMOVE_HOST, m_systemName.in());
    
    CDMW_SM_READER_LOCK_GUARD(*m_stateMachine);
    
    m_stateMachine->getState()->remove_host (m_stateMachine,
                                             host_name);
}                  


CdmwPlatformMngt::Application_ptr 
      System_impl::add_application(const CdmwPlatformMngt::PlatformApplicationDef& platform_application_def,
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
    std::string applicationName = platform_application_def.application_name.in();    
    LogMngr::logMessage(INF, MSG_ID_ADD_APPLICATION, 
                        applicationName.c_str(), m_systemName.in());
    
    bool systemDefined = false;   
    {   
        CDMW_READER_LOCK_GUARD (m_rwSystemLock);
        systemDefined = m_systemDefined;
    }
    
    // check if system is defined
    if (systemDefined)
    {
        CDMW_SM_READER_LOCK_GUARD(*m_stateMachine);
        
        return m_stateMachine->getState()->add_application (m_stateMachine,
                                                            platform_application_def,
                                                            application_def);
    }
    else
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    } 
}

void System_impl::remove_application(const char* application_name)
        throw (CdmwPlatformMngt::ApplicationNotFound,
               CdmwPlatformMngt::ApplicationStillActive,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    LogMngr::logMessage(INF, MSG_ID_REMOVE_APPLICATION, 
                        application_name, m_systemName.in());
    
    CDMW_SM_READER_LOCK_GUARD(*m_stateMachine);
    
    return m_stateMachine->getState()->remove_application (m_stateMachine,
                                                           application_name);
} 

void System_impl::start (const CdmwPlatformMngtBase::StartupKind& startup_kind)      
        throw (CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    LogMngr::logMessage(INF, MSG_ID_SYS_START, m_systemName.in());
    
    // check if system is defined
    bool systemDefined = false;   
    {   
        CDMW_READER_LOCK_GUARD (m_rwSystemLock);
        systemDefined = m_systemDefined;
    }
    
    if (systemDefined)
    {
        CDMW_SM_WRITER_LOCK_GUARD(*m_stateMachine);
        
        m_stateMachine->getState()->start (m_stateMachine, startup_kind);
    }
    else
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    }    
}
    

void System_impl::resume()
        throw (CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    LogMngr::logMessage(INF, MSG_ID_SYS_RESUME, m_systemName.in());
    
    CDMW_SM_WRITER_LOCK_GUARD(*m_stateMachine);
    
    m_stateMachine->getState()->resume (m_stateMachine);
}


void System_impl::acknowledge()
        throw (CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    LogMngr::logMessage(INF, MSG_ID_SYS_ACK, m_systemName.in());
    
    CDMW_MUTEX_GUARD (m_mutexMode);
    
    m_mode = CdmwPlatformMngt::NORMAL_MODE;
    m_modeInfo = CORBA::string_dup ("");

    // Notify the change of mode
    notifyModeChangeEvent();
}


void System_impl::stop (CORBA::Boolean emergency)
         throw( CORBA::SystemException )      
{
    try
    {
        LogMngr::logMessage(INF, MSG_ID_SYS_STOP, m_systemName.in());
                
        // create graph element factory
        #if CDMW_ORB_VDR == tao
        GraphElementFactory factory;
        #else
        CORBA::ValueFactoryBase_var factory = new GraphElementFactory();
        #endif
 
        CDMW_SM_WRITER_LOCK_GUARD(*m_stateMachine);
                
        // emergency stop
        if (emergency)
        {           
            // empty the actual emergency stop graph
            m_stopGraph->length(0); 
            
            // set reader lock for application protection
            CDMW_READER_LOCK_GUARD (m_rwSystemLock);
            
            // Add all the active applications as roots of the stop graph          
            Applications::iterator applicationIt; 
        
            for (applicationIt =  m_applications.begin() ;
                 applicationIt != m_applications.end() ;
                 applicationIt++)
            {
                CdmwPlatformMngt::Application_var application = 
                             (applicationIt->second).m_application;
                
                // get application status
                CORBA::String_var info;
                CdmwPlatformMngt::ApplicationStatus appStatus = 
                                 application->get_status(info.out());
                                          
                if ((appStatus == CdmwPlatformMngt::APPLICATION_INITIALISING) ||
                    (appStatus == CdmwPlatformMngt::APPLICATION_INITIALISED) ||
                    (appStatus == CdmwPlatformMngt::APPLICATION_INITIALISATION_INCOMPLETE) ||
                    (appStatus == CdmwPlatformMngt::APPLICATION_INITIALISATION_SUSPENDED) ||
                    (appStatus == CdmwPlatformMngt::APPLICATION_RUN_REQUEST) ||
                    (appStatus == CdmwPlatformMngt::APPLICATION_RUNNING))
                {
                    // create graph element using factory
                    CORBA::String_var appliName = application->name();
                    #if CDMW_ORB_VDR == tao
                    CdmwPlatformMngt::GraphElement_var element = 
                          factory.create (appliName.in(), 0);
                    #else
                    CdmwPlatformMngt::GraphElement_var element = 
                          dynamic_cast <GraphElementFactory*> (factory.in())->
                                                    create (appliName.in(), 0);
                    #endif
                 
                    // insert new element in graph
                    int graphlen =  m_stopGraph->length();                     
                    m_stopGraph->length (graphlen + 1);
                    m_stopGraph->operator[](graphlen) = element;
                }
            }
        }
    
        else
        {           
            // Get the list of all element of the stop graph
            ElementNames elementNames =
               GraphUtility::getElementNames (m_applicationStopGraph.in());


            // create a new stop graph copy of the original
            m_stopGraph = m_applicationStopGraph;

            // set reader lock for application protection
            CDMW_READER_LOCK_GUARD (m_rwSystemLock);
            
            // Add all the active applications as roots of the stop graph
            // if not present in the stop graph.
            Applications::iterator applicationIt; 
        
            for (applicationIt =  m_applications.begin() ;
                 applicationIt != m_applications.end() ;
                 applicationIt++)
            {
                CdmwPlatformMngt::Application_var application = 
                             (applicationIt->second).m_application;
                
                // if application not found in the stop graph  
                if (elementNames.find (applicationIt->first) == elementNames.end())
                { 
                    // get application status
                    CORBA::String_var info;
                    CdmwPlatformMngt::ApplicationStatus appStatus = 
                                 application->get_status(info.out());
                                 
                    // if application candidate to be stopping        
                    if ((appStatus == CdmwPlatformMngt::APPLICATION_INITIALISING) ||
                        (appStatus == CdmwPlatformMngt::APPLICATION_INITIALISED) ||
                        (appStatus == CdmwPlatformMngt::APPLICATION_INITIALISATION_INCOMPLETE) ||
                        (appStatus == CdmwPlatformMngt::APPLICATION_INITIALISATION_SUSPENDED) ||
                        (appStatus == CdmwPlatformMngt::APPLICATION_RUN_REQUEST) ||
                        (appStatus == CdmwPlatformMngt::APPLICATION_RUNNING))
                    {
                        // create graph element from application name using factory
                        CORBA::String_var appliName = application->name();
                        #if CDMW_ORB_VDR == tao
                        CdmwPlatformMngt::GraphElement_var element = 
                              factory.create (appliName.in(), 0);
                        #else
                        CdmwPlatformMngt::GraphElement_var element = 
                              dynamic_cast <GraphElementFactory*> (factory.in())->
                                                    create (appliName.in(), 0);
                        #endif
                 
                        // insert new application element in graph
                        int graphlen =  m_stopGraph->length();                     
                        m_stopGraph->length (graphlen + 1);
                        m_stopGraph->operator[](graphlen) = element;
                    }
                }
            }
        }

        // request stop        
        m_emergency = emergency;
        m_stateMachine->getState()->stop (m_stateMachine, emergency);
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
}





CORBA::ULong System_impl::get_number_of_entities()
throw( CORBA::SystemException )
{
    return m_rwEntityContainer->get_number_of_entities();
}

void System_impl::get_all_entity_names (
                         CORBA::ULong how_many,
                         CdmwPlatformMngtEntity::EntityNames_out entity_names,
                         CdmwPlatformMngtEntity::EntityNamesIterator_out rest)
throw( CORBA::SystemException )
{
    
    m_rwEntityContainer->get_all_entity_names (how_many, entity_names, rest);
        
}

CdmwPlatformMngtEntity::EntityStatus System_impl::get_entity_status (
                        const char* entity_name,
                        CORBA::String_out entity_info)
       throw (CdmwPlatformMngtEntity::EntityNotFound,
              CORBA::SystemException)
{
    return m_rwEntityContainer->get_entity_status (entity_name, entity_info);
}

CORBA::Boolean System_impl::get_entities (
                        const CdmwPlatformMngtEntity::EntityNames& entity_names,
                        CdmwPlatformMngtEntity::Entities_out nentities)
throw( CORBA::SystemException )
{
    
    return m_rwEntityContainer->get_entities (entity_names, nentities);
        
}

void System_impl::get_all_entities (
                        CORBA::ULong how_many,
                        CdmwPlatformMngtEntity::Entities_out nentities,
                        CdmwPlatformMngtEntity::EntitiesIterator_out rest)
throw( CORBA::SystemException )
{
    
    m_rwEntityContainer->get_all_entities (how_many, nentities, rest);
        
}

void System_impl::add_entity (const char* entity_name)    
       throw (CdmwPlatformMngtEntity::EntityAlreadyExists,
              CORBA::SystemException)
{
    m_rwEntityContainer->add_entity (entity_name);

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void System_impl::remove_entity (const char* entity_name)    
       throw (CdmwPlatformMngtEntity::EntityNotFound,
              CORBA::SystemException)
{
    m_rwEntityContainer->remove_entity (entity_name);

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}



CdmwPlatformMngtService::ServiceProviderID* System_impl::get_service_def (
                        const char* service_name)
       throw (CdmwPlatformMngtService::ServiceNotFound,
              CORBA::SystemException)
{
    return m_rwServiceDefContainer->get_service_def (service_name);
}

void System_impl::add_service_def (const CdmwPlatformMngtService::ServiceDef& service_def)  
       throw (CdmwPlatformMngtService::ServiceAlreadyExists,
              CORBA::SystemException)
{
    m_rwServiceDefContainer->add_service_def (service_def);

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void System_impl::remove_service_def (const char* service_name)  
       throw (CdmwPlatformMngtService::ServiceNotFound,
              CORBA::SystemException)
{
    m_rwServiceDefContainer->remove_service_def (service_name);

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}




CORBA::ULong System_impl::get_number_of_properties()
throw( CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);       
}

void System_impl::get_all_property_names (
                        CORBA::ULong how_many,
                        CosPropertyService::PropertyNames_out property_names,
                        CosPropertyService::PropertyNamesIterator_out rest)
throw( CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

CORBA::Any* System_impl::get_property_value (const char* property_name)    
        throw (CosPropertyService::PropertyNotFound,
               CosPropertyService::InvalidPropertyName,
               CORBA::SystemException)
{
    // TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

CORBA::Boolean System_impl::get_properties (
                         const CosPropertyService::PropertyNames& property_names,
                         CosPropertyService::Properties_out nproperties)
throw( CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

void System_impl::get_all_properties (
                         CORBA::ULong how_many,
                         CosPropertyService::Properties_out nproperties,
                         CosPropertyService::PropertiesIterator_out rest)
throw( CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

void System_impl::set_property (
                         const char* property_name,
                         const CORBA::Any& property_value)
throw( CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);


    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void System_impl::remove_property (const char* property_name)    
        throw (CosPropertyService::PropertyNotFound,
               CORBA::SystemException)
{
    // TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);


    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}





CORBA::ULong System_impl::get_number_of_hosts()
throw( CORBA::SystemException )
{
    return m_hostContainer->get_number_of_hosts();
}



void System_impl::get_all_host_names (CORBA::ULong how_many,
                                      CdmwPlatformMngt::HostNames_out host_names,
                                      CdmwPlatformMngt::HostNamesIterator_out rest)
throw( CORBA::SystemException )
{
    m_hostContainer->get_all_host_names (how_many, host_names, rest);
}



CdmwPlatformMngt::HostProxy_ptr System_impl::get_host (const char* host_name)
        throw (CdmwPlatformMngt::HostNotFound,
               CORBA::SystemException)
{
    return m_hostContainer->get_host (host_name);
}



CORBA::Boolean System_impl::get_hosts (const CdmwPlatformMngt::HostNames& host_names,
                                       CdmwPlatformMngt::HostProxies_out hosts)
throw( CORBA::SystemException )
{
    return m_hostContainer->get_hosts (host_names, hosts);
}



void System_impl::get_all_hosts (CORBA::ULong how_many,
                                 CdmwPlatformMngt::HostProxies_out hosts,
                                 CdmwPlatformMngt::HostProxiesIterator_out rest)
throw( CORBA::SystemException )
{
    m_hostContainer->get_all_hosts (how_many, hosts, rest);
}



const char* System_impl::get_systemName()
{
    return m_systemName.in();
}

} // End namespace PlatformMngt
} // End namespace Cdmw

