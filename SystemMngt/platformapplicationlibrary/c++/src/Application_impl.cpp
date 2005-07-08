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


#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/osthreads/ReaderLockGuard.hpp"
#include "Foundation/osthreads/WriterLockGuard.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtDaemon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp"

#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "SystemMngt/platformlibrary/EntityContainer_impl.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"
#include "SystemMngt/platformlibrary/GraphChecker.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformlibrary/RWEntityContainer_impl.hpp"
#include "SystemMngt/platformlibrary/RWServiceDefContainer_impl.hpp"
#include "SystemMngt/platformlibrary/ServiceDefContainer_impl.hpp"
#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"
#include "SystemMngt/platformlibrary/SynchronisableTaskDef.hpp"
#include "SystemMngt/platformlibrary/TaskFactory.hpp"
#include "SystemMngt/platformlibrary/TaskSequencer.hpp"
#include "SystemMngt/platformlibrary/TaskStarter.hpp"

#include "SystemMngt/platformvaluetypes/EntityStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationAgentDeath_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationModeChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/GraphElement_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/ApplicationLogMessageId.hpp"

#include "platformapplicationlibrary/ProcessProxy_impl.hpp"
#include "platformapplicationlibrary/ManagedProcessProxy_impl.hpp"
#include "platformapplicationlibrary/UnmanagedProcessProxy_impl.hpp"
#include "platformapplicationlibrary/ProcessObserver_impl.hpp"
#include "platformapplicationlibrary/ServiceBroker_impl.hpp"
#include "platformapplicationlibrary/EntityObserver_impl.hpp"
#include "platformapplicationlibrary/ProcessProxiesIterator_impl.hpp"
#include "platformapplicationlibrary/ProcessInit.hpp"
#include "platformapplicationlibrary/ProcessRun.hpp"
#include "platformapplicationlibrary/ProcessStop.hpp"
#include "platformapplicationlibrary/ApplicationAgentObserver_impl.hpp"



namespace Cdmw
{
namespace PlatformMngt
{

ApplicationInitial*
    Application_impl::M_initial = NULL;
ApplicationInitialising*
    Application_impl::M_initialising = NULL;
ApplicationInitialisationSuspended*
    Application_impl::M_initialisationSuspended = NULL;
ApplicationInitialised*
    Application_impl::M_initialised = NULL;
ApplicationRunRequest*
    Application_impl::M_runRequest = NULL;
ApplicationRunning*
    Application_impl::M_running = NULL;
ApplicationStopping*
    Application_impl::M_stopping = NULL;
ApplicationStopped*
    Application_impl::M_stopped = NULL;
ApplicationInitialisationIncomplete*
    Application_impl::M_initialisationIncomplete = NULL;

// ----------------------------------------------------------------------
// Application_impl class.
// ----------------------------------------------------------------------
#ifdef CDMW_USE_FAULTTOLERANCE
Application_impl::Application_impl(
    PortableServer::POA_ptr poa,
    SupervisionEventNotifier* eventNotifier,
    std::string replicationManagerIOGR,
    CdmwPlatformMngt::FTConverter_ptr ftConverter)
    throw( BadParameterException, AlreadyDoneException,
           UnknownStateException, OutOfMemoryException )
    : DeactivableServant_impl( poa ),
      m_initialisation_condition( m_mutex ),
      m_stop_condition( m_mutex ),
      m_applicationDefined( false ),
      m_replicationManagerIOGR(replicationManagerIOGR),
      m_newInitGraph( true )
#else
Application_impl::Application_impl(
    PortableServer::POA_ptr poa,
    SupervisionEventNotifier* eventNotifier )
    throw( BadParameterException, AlreadyDoneException,
           UnknownStateException, OutOfMemoryException )
    : DeactivableServant_impl( poa ),
      m_initialisation_condition( m_mutex ),
      m_stop_condition( m_mutex ),
      m_applicationDefined( false ),      
      m_newInitGraph( true )
#endif
{
    if( eventNotifier == NULL )
        CDMW_THROW2( BadParameterException, "eventNotifier", "NULL" );

    m_graphElementFactory = NULL;
    m_appEntityStatusChangeFactory = NULL;
    m_appStatusChangeFactory = NULL;
    m_appModeChangeFactory = NULL;
    m_stateMachine = NULL;
    m_actualEntityContainer = NULL;
    m_actualServiceDefContainer = NULL;
    m_serviceBroker = NULL;
    m_entityObserver = NULL;
    m_processObserver = NULL;
    m_agentObserver = NULL;
    m_serviceDefContainer = NULL;
    
    try
    {
        std::auto_ptr< GraphElementFactory > graphElementFactory(
            new GraphElementFactory() );
            
        std::auto_ptr< ApplicationEntityStatusChangeFactory > appEntityStatusChangeFactory(
            new ApplicationEntityStatusChangeFactory() );
            
        std::auto_ptr< ApplicationStatusChangeFactory > appStatusChangeFactory(
            new ApplicationStatusChangeFactory() );
            
        std::auto_ptr< ApplicationModeChangeFactory > appModeChangeFactory(
            new ApplicationModeChangeFactory() );
            
        std::auto_ptr< ApplicationStateMachine > stateMachine(
            new ApplicationStateMachine( this, "Application" ) );
            
        m_agentObserverServant = new ApplicationAgentObserver_impl( poa, this );
            
        m_processObserverServant = new ProcessObserver_impl(  poa, this );
            
        // Set the internal data
#ifdef CDMW_USE_FAULTTOLERANCE
        m_ftConverter = CdmwPlatformMngt::FTConverter::_duplicate(ftConverter);        
#endif
        m_poa = PortableServer::POA::_duplicate( poa );
        m_graphElementFactory = graphElementFactory.get();
        m_appEntityStatusChangeFactory = appEntityStatusChangeFactory.get();
        m_appStatusChangeFactory = appStatusChangeFactory.get();
        m_appModeChangeFactory = appModeChangeFactory.get();
        m_stateMachine = stateMachine.get();
        
        m_agentObserver
            = dynamic_cast< ApplicationAgentObserver_impl* >( m_agentObserverServant.in() );
            
        m_processObserver
            = dynamic_cast< ProcessObserver_impl* >( m_processObserverServant.in() );

        m_mode = CdmwPlatformMngt::NORMAL_MODE;
        m_modeInfo = CORBA::string_dup( "" );
        m_statusInfo = CORBA::string_dup( "" );
        m_eventNotifier = eventNotifier;
        m_taskFactory = NULL;
        m_starter = NULL;
        m_initSyncTaskFactory = NULL;
        m_stopSyncTaskFactory = NULL;
        m_initSequencer = NULL;
        m_stopSequencer = NULL;
        m_actualEntityContainer = NULL;
        m_actualServiceDefContainer = NULL;
        m_initProcessGraph = new CdmwPlatformMngt::GraphElements();
        m_stopProcessGraph = new CdmwPlatformMngt::GraphElements();
        m_processProxies.clear();

        // Start the event handler
        m_eventHandler.start();

        // Activate objects
        m_agentObserver->activate();
        m_processObserver->activate();

        // Add the states to the state machine
        m_stateMachine->addState( "Initial", M_initial );
        m_stateMachine->addState( "Initialising", M_initialising );
        m_stateMachine->addState(
            "InitialisationSuspended", M_initialisationSuspended );
        m_stateMachine->addState( "Initialised", M_initialised );
        m_stateMachine->addState( "RunRequest", M_runRequest );
        m_stateMachine->addState( "Running", M_running );
        m_stateMachine->addState( "Stopping", M_stopping );
        m_stateMachine->addState( "Stopped", M_stopped );
        m_stateMachine->addState(
            "InitialisationIncomplete", M_initialisationIncomplete );

        // Set the initial state
        m_stateMachine->setState( "Initial" );
   
        // Release the ownership of the temporary objects
        graphElementFactory.release();
        appEntityStatusChangeFactory.release();
        appStatusChangeFactory.release();
        appModeChangeFactory.release();
        stateMachine.release();
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
}

Application_impl::~Application_impl()
{

#ifdef CDMW_USE_FAULTTOLERANCE
    std::string appliProcessName(get_applicationName());
    m_ftConverter->unregister_application_agent_observer(appliProcessName.c_str());
    
#endif

    destroyStarter();
    destroyInitSequencer();
    destroyStopSequencer();

    m_eventHandler.stop();

    if( m_graphElementFactory != NULL )
    {
        m_graphElementFactory->_remove_ref();
    }
        
    if( m_appEntityStatusChangeFactory != NULL )
    {
        m_appEntityStatusChangeFactory->_remove_ref();
    }
       
    if( m_appStatusChangeFactory != NULL )
    {
        m_appStatusChangeFactory->_remove_ref();
    }
        
    if( m_appModeChangeFactory != NULL )
    {
        m_appModeChangeFactory->_remove_ref();
    }
        
    if( m_stateMachine != NULL )
    {
        delete m_stateMachine;
    }
    
    
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

    try
    {
        // note : servant are deleted by the corresponding _var attributes
        //        destruction
        if( m_processObserver != NULL )
        {
            m_processObserver->deactivate();
        }
        
        if( m_agentObserver != NULL )
        {
            m_agentObserver->deactivate();
        }
        
        if( m_serviceDefContainer != NULL )
        {
            m_serviceDefContainer->deactivate();
        }

        if( m_serviceBroker != NULL )
        {
            m_serviceBroker->destroy();
        }

        if( m_entityObserver != NULL )
        { 
            m_entityObserver->destroy();
        }
    }
    catch (...)
    {
    }
}



void Application_impl::endCreation(
    const CdmwPlatformMngt::ApplicationAck& applicationAck )
throw( OutOfMemoryException, CORBA::SystemException )
{
    try
    {
        m_applicationAck
            = new CdmwPlatformMngt::ApplicationAck( applicationAck );
            
        m_actualEntityContainer =
            EntityContainer_impl::EntityContainer::createApplicationContainer(
                applicationAck.application_name.in() );
        m_actualServiceDefContainer =
            ServiceDefContainer_impl::ServiceDefContainer::createApplicationContainer(
                applicationAck.application_name.in() );

        m_rwEntityContainerServant = new RWEntityContainer_impl( m_poa.in(), m_actualEntityContainer );

        m_serviceDefContainerServant = new ServiceDefContainer_impl(
                m_poa.in(),
                m_actualServiceDefContainer,
                applicationAck.system_service_def_container );

        m_rwServiceDefContainerServant = new RWServiceDefContainer_impl(
                m_poa.in(),
                m_actualServiceDefContainer,
                applicationAck.system_service_def_container );

        m_entityObserverServant = new EntityObserver_impl( this );

        m_serviceBrokerServant = new ServiceBroker_impl( this, applicationAck.service_binding );

        // Set the additional internal data with the acknowledgement data
        m_rwEntityContainer
            = dynamic_cast< RWEntityContainer_impl* >( m_rwEntityContainerServant.in() );
            
        m_serviceDefContainer
            = dynamic_cast< ServiceDefContainer_impl* >( m_serviceDefContainerServant.in() );
            
        m_rwServiceDefContainer
            = dynamic_cast< RWServiceDefContainer_impl* >( m_rwServiceDefContainerServant.in() );
            
        m_entityObserver
            = dynamic_cast< EntityObserver_impl* >( m_entityObserverServant.in() );

        m_serviceBroker
            = dynamic_cast< ServiceBroker_impl* >( m_serviceBrokerServant.in() );
            
        m_hostContainer = CdmwPlatformMngt::HostContainer::_duplicate(
            applicationAck.host_container );
            
        m_systemEntityObserver
            = CdmwPlatformMngtEntity::SystemEntityObserver::_duplicate(
                applicationAck.system_entity_observer );
                
        m_eventNotifier->registerObserver(
            m_applicationAck->application_name.in(),
            m_applicationAck->supervision_observer.in() );

        // Activate objects
        m_serviceDefContainer->activate();
    }
    catch( const std::bad_alloc& )
    {
        if( m_actualEntityContainer != NULL )
            EntityContainer_impl::EntityContainer::destroy( m_actualEntityContainer );

        if( m_actualServiceDefContainer != NULL )
            ServiceDefContainer_impl::ServiceDefContainer::destroy(
                m_actualServiceDefContainer );

        CDMW_THROW( OutOfMemoryException );
    }
}

void Application_impl::createStates()
throw( OutOfMemoryException )
{
    try
    {
        std::auto_ptr< ApplicationInitial > initial(
            new ApplicationInitial() );
        std::auto_ptr< ApplicationInitialising > initialising(
            new ApplicationInitialising() );
        std::auto_ptr< ApplicationInitialisationSuspended >
            initialisationSuspended(
                new ApplicationInitialisationSuspended() );
        std::auto_ptr< ApplicationInitialised > initialised(
            new ApplicationInitialised() );
        std::auto_ptr< ApplicationRunRequest > runRequest(
            new ApplicationRunRequest() );
        std::auto_ptr< ApplicationRunning > running(
            new ApplicationRunning() );
        std::auto_ptr< ApplicationStopping > stopping(
            new ApplicationStopping() );
        std::auto_ptr< ApplicationStopped > stopped(
            new ApplicationStopped() );
        std::auto_ptr< ApplicationInitialisationIncomplete >
            initialisationIncomplete(
                new ApplicationInitialisationIncomplete() );

        if( M_initial == NULL)
            M_initial = initial.get();
        if( M_initialising == NULL)
            M_initialising = initialising.get();
        if( M_initialisationSuspended == NULL)
            M_initialisationSuspended = initialisationSuspended.get();
        if( M_initialised == NULL)
            M_initialised = initialised.get();
        if( M_runRequest == NULL)
            M_runRequest = runRequest.get();
        if( M_running == NULL)
            M_running = running.get();
        if( M_stopping == NULL)
            M_stopping = stopping.get();
        if( M_stopped == NULL)
            M_stopped = stopped.get();
        if( M_initialisationIncomplete == NULL)
            M_initialisationIncomplete = initialisationIncomplete.get();

        // Create the processes states
        ManagedProcessProxy_impl::createStates();
        UnmanagedProcessProxy_impl::createStates();

        // Release the ownership of the temporary objects
        initial.release();
        initialising.release();
        initialisationSuspended.release();
        initialised.release();
        runRequest.release();
        running.release();
        stopping.release();
        stopped.release();
        initialisationIncomplete.release();

    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
}

void Application_impl::destroyStates()
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

    if( M_initialisationSuspended != NULL )
    {
        delete M_initialisationSuspended;
        M_initialisationSuspended = NULL;
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

    if( M_initialisationIncomplete != NULL )
    {
        delete M_initialisationIncomplete;
        M_initialisationIncomplete = NULL;
    }

    // Destroy the processes states
    ManagedProcessProxy_impl::destroyStates();
    UnmanagedProcessProxy_impl::destroyStates();
}

// TODO: - Activer une seule fois ProcessObserver
CdmwPlatformMngt::ApplicationAgent_ptr Application_impl::getAgent(
    const char* hostName )
throw( CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::HostNotReachable,
       CdmwPlatformMngt::CreationError,
       CdmwPlatformMngt::ApplicationAgentAlreadyExists,
       CdmwPlatformMngt::InvalidInfo,
       CdmwPlatformMngt::CreationTimeout,
       AgentNotFoundException,
       CORBA::SystemException )
{
    CdmwPlatformMngt::ApplicationAgent_var applicationAgent;

    try
    {
        const char *p_appName = m_applicationAck->application_name.in();
        CdmwPlatformMngt::Daemon_var daemon;

        // try to find host name in host set for the application
        bool agentFound = false;
        std::string hostName_str = hostName;
        
        {
            CDMW_READER_LOCK_GUARD(m_rwAgentHosts_lock);
            
            AgentHosts::iterator hostSetIt = m_agentHosts.find(hostName_str);

            if (hostSetIt != m_agentHosts.end())
            {
                // agent has been already created
                agentFound = true;
                
                // get the daemon
                daemon = (hostSetIt->second)->daemonObj;
            }
        }

        // If application agent not found, create a new one
        if (!agentFound)
        {
            // protect with the WRITER lock
            CDMW_WRITER_LOCK_GUARD(m_rwAgentHosts_lock);

            // check again if agent has not been created by another thread
            // in the meantime
            AgentHosts::iterator hostSetIt = m_agentHosts.find(hostName_str);
            
            if (hostSetIt != m_agentHosts.end())
            {
                // the agent has been already created by another thread
                agentFound = true;
                
                // get the daemon
                daemon = (hostSetIt->second)->daemonObj;
            }
            else
            {
                // agent does not exist fot the host
                
                // Get the daemon running on the specified host
                daemon =
                   Configuration::getDaemon (hostName, m_applicationAck->system_port);
            
                CdmwPlatformMngt::ApplicationAgentAck_var agentAck
                              = new CdmwPlatformMngt::ApplicationAgentAck();
                agentAck->application_name = CORBA::string_dup (p_appName);
                agentAck->process_observer = m_processObserver->_this();
#ifdef CDMW_USE_FAULTTOLERANCE
                agentAck->replication_manager_iogr = CORBA::string_dup (m_replicationManagerIOGR.c_str());
#endif
                
                CdmwPlatformMngt::HostProxy_var host
                              = m_hostContainer->get_host (hostName);
                    
                CdmwPlatformMngt::ProcessLightInfo* p_agentProcessInfo
                              = &(m_applicationAck->application_agent_process_info);
    
                CdmwPlatformMngt::ApplicationAgentInfo_var agentInfo
                              = new CdmwPlatformMngt::ApplicationAgentInfo();
                agentInfo->application_agent_ack = agentAck.in();
                agentInfo->application_agent_exe = host->application_agent_exe();
                agentInfo->application_agent_port = p_agentProcessInfo->process_port;
                agentInfo->application_agent_args = p_agentProcessInfo->process_args;
                agentInfo->application_agent_monitoring_info =
                                m_applicationAck->application_agent_monitoring_info;
                agentInfo->application_agent_observer = m_agentObserver->_this();
    
                applicationAgent = daemon->create_application_agent (
                                                         agentInfo.in(),
                                                         Configuration::M_timeout);
#ifdef CDMW_USE_FAULTTOLERANCE
                m_ftConverter->register_application_agent_observer(CORBA::string_dup (p_appName) , m_agentObserver->_this());
#endif
                // store host name with daemon in map
                std::auto_ptr <HostData> hostData (new HostData);
                                 
                (hostData.get())->daemonObj = daemon;
                
                m_agentHosts[hostName_str] = hostData.get();
                hostData.release();
            }
        }

        // if host name has been found, application agent should exist        
        if (agentFound)
        {            
            // Get the application agent from the daemon
            applicationAgent =
                daemon->search_application_agent (p_appName);

            if( CORBA::is_nil (applicationAgent.in()))
            {
                // If application agent not found by daemon, throw exception
                CDMW_THROW (AgentNotFoundException);
            }
        }
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }

    return applicationAgent._retn();
}


void Application_impl::killAllAgents ()
{
    // Kill all agents create by daemons for application
    {
        CDMW_WRITER_LOCK_GUARD (m_rwAgentHosts_lock);
        
        bool errorSet = true;
        
        const char *p_appName = m_applicationAck->application_name.in();
        
        AgentHosts::iterator hostSetIt;

        for (hostSetIt = m_agentHosts.begin(); 
             hostSetIt != m_agentHosts.end(); 
             hostSetIt++)
        {
            try
            {
                LogMngr::logMessage (
                               INF,
                               MSG_ID_APP_START_KILL_AGENT_HOST,
                               p_appName,
                               (hostSetIt->first).c_str());
            
                // Get the daemon running on the specified host                
                CdmwPlatformMngt::Daemon_var daemon =
                             (hostSetIt->second)->daemonObj;    
            
                // request to daemon to kill agent
                daemon->kill_application_agent (p_appName);
                
                // kill is ok
                errorSet = false;
            }
            catch (const CdmwPlatformMngt::HostNotFound& e)
            {
                // nothing to do
            }
            catch (const CdmwPlatformMngt::HostNotReachable& e)
            {
                // nothing to do
            }
            catch (const CdmwPlatformMngt::ApplicationAgentNotFound& e)
            {
                // nothing to do
            }
            catch (...)
            {
                // nothing to do
            }
            
            long messageId;
            
            if (errorSet)
            {
                messageId = MSG_ID_APP_KILL_AGENT_FAIL;
            }
            else
            {
                messageId = MSG_ID_APP_KILL_AGENT_OK;
            }
            
            LogMngr::logMessage (
                               INF,
                               messageId,
                               p_appName,
                               (hostSetIt->first).c_str());
        }
        
        // clear the map
        for (hostSetIt = m_agentHosts.begin(); 
             hostSetIt != m_agentHosts.end(); 
             hostSetIt++)
        {
            HostData* p_hostData = hostSetIt->second;
            delete p_hostData;
        }
              
        m_agentHosts.clear();
    }    
}

void Application_impl::invalidateProcesses(
    CdmwPlatformMngtBase::EventHeader header,
    const char* hostName,
    InvalidationType reason )
{
    std::string faultyHostName = hostName;
    
    // remove invalidated host from application hosts set
    {
        CDMW_WRITER_LOCK_GUARD(m_rwAgentHosts_lock);
            
        AgentHosts::iterator hostSetIt = m_agentHosts.find(faultyHostName);

        if (hostSetIt != m_agentHosts.end())
        {
            HostData* p_hostData = hostSetIt->second;
            delete p_hostData;
            m_agentHosts.erase (hostSetIt);           
        }
    }
    
    CDMW_READER_LOCK_GUARD( m_rwApplicationLock );

    // Invalidate all the activ processes
    Application_impl::ProcessProxies::iterator processIt;

    for( processIt =  m_processProxies.begin();
         processIt != m_processProxies.end();
         processIt++ )
    {
        ProcessProxy_impl* process = processIt->second;
        
        CdmwPlatformMngt::HostProxy_var hostProxy = process->get_host();
        CORBA::String_var processHostName = hostProxy->name();
        
        CORBA::String_var statusInfo;
        CdmwPlatformMngt::ProcessStatus status =
              process->get_status (statusInfo.out());

        if ((faultyHostName == processHostName.in()) &&
            ((status == CdmwPlatformMngt::PROCESS_INITIALISING) ||
             (status == CdmwPlatformMngt::PROCESS_STEP_PERFORMED) ||
             (status == CdmwPlatformMngt::PROCESS_INITIALISED) ||
             (status == CdmwPlatformMngt::PROCESS_RUN_REQUEST) ||
             (status == CdmwPlatformMngt::PROCESS_RUNNING) ||
             (status == CdmwPlatformMngt::PROCESS_STOPPING)))
        {
            switch( reason )
            {
                case AGENT_DEATH:
                    process->invalidate_event(
                            header,
                            LogMngr::getFormattedMessage(
                                 MSG_ID_AGENT_CRASHED,
                                 m_applicationAck->application_name.in(),
                                 hostName ).c_str() );
                    break;
                case HOST_UNAVAILABLE:
                    process->invalidate_event(
                            header,
                            LogMngr::getFormattedMessage(
                                MSG_ID_HOST_CRASHED,
                                hostName ).c_str() );
                    break;
            }

            // Enter the degraded mode
            degradation_event();
        }
    }
}

void Application_impl::notifyAgentDeathEvent(
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* hostName )
{
    try
    {
        std::auto_ptr< ApplicationAgentDeathFactory > appAgentDeathFactory(
            new ApplicationAgentDeathFactory() );

        CdmwPlatformMngt::ApplicationAgentDeath_var event
            = appAgentDeathFactory->create(
                header,
                m_applicationAck->system_name.in(),
                hostName,
                m_applicationAck->application_name.in() );

        CORBA::String_var eventStr = event->to_string();
        LogMngr::logMessage( eventStr.in() );

        m_eventNotifier->addEvent( event );
        event._retn();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

void Application_impl::notifyModeChangeEvent()
{
    try
    {
        CdmwPlatformMngtBase::EventHeader header
            = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

        CdmwPlatformMngt::ApplicationModeChange_var event
            = m_appModeChangeFactory->create(
                header,
                m_applicationAck->system_name.in(),
                m_applicationAck->application_name.in(),
                m_mode,
                m_modeInfo.in() );

        CORBA::String_var eventStr = event->to_string();
        LogMngr::logMessage(eventStr.in());

        m_eventNotifier->addEvent( event );
        event._retn();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

void Application_impl::notifyStatusChangeEvent()
{
    try
    {
        CdmwPlatformMngtBase::EventHeader header
            = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

        CdmwPlatformMngt::ApplicationStatusChange_var event
            = m_appStatusChangeFactory->create(
                header,
                m_applicationAck->system_name.in(),
                m_applicationAck->application_name.in(),
                m_stateMachine->getState()->status(),
                m_statusInfo.in() );

        CORBA::String_var eventStr = event->to_string();
        LogMngr::logMessage(eventStr.in());

        m_eventNotifier->addEvent( event );
        event._retn();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

void Application_impl::notifyEntityStatusChangeEvent(
    const char* entity_name,
    CdmwPlatformMngtEntity::EntityStatus entity_status,
    const char* entity_info )
{
    try
    {
        CdmwPlatformMngtBase::EventHeader header
            = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF );

        CdmwPlatformMngt::ApplicationEntityStatusChange_var event
            = m_appEntityStatusChangeFactory->create(
                header,
                m_applicationAck->system_name.in(),
                m_applicationAck->application_name.in(),
                entity_name,
                entity_status,
                entity_info );

        CORBA::String_var eventStr = event->to_string();
        LogMngr::logMessage(eventStr.in());

        m_eventNotifier->addEvent( event );
        event._retn();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

void Application_impl::setInitialisationStatus(
    CdmwPlatformMngt::LongRequestStatus status )
{
    m_mutex.lock();
    m_initialisation_status = status;
    m_initialisation_condition.broadcast();
    m_mutex.unlock();    
}

void Application_impl::setStopStatus(
    CdmwPlatformMngt::LongRequestStatus status )
{
    m_mutex.lock();
    m_stop_status = status;
    m_stop_condition.broadcast();
    m_mutex.unlock();  
}

void Application_impl::setInitGraph(
    const CdmwPlatformMngt::GraphElements& process_graph )
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CORBA::SystemException )
{
    try
    {
        CDMW_WRITER_LOCK_GUARD( m_rwApplicationLock );

        // Build the map of exisiting elements
        ExistingElementMap existingElements;
        ProcessProxies::iterator processIt;

        for( processIt =  m_processProxies.begin();
             processIt != m_processProxies.end();
             processIt++ )
        {
            ProcessProxy_impl* process = processIt->second;
            unsigned long initStepCount = 1;
            
            if( process->type() != CdmwPlatformMngt::UNMANAGED_PROCESS )
            {
                CdmwPlatformMngt::LifeCycleDef_var lifeCycleDef =
                    process->get_life_cycle();
                    
                initStepCount = lifeCycleDef->init_steps.length();
            }

            existingElements.insert(
                PlatformMngt::ExistingElementMap::value_type(
                    process->get_processName(),
                    initStepCount ) );
        }

        // Check the validity of the graph of processes
        GraphChecker processGraph( process_graph, &existingElements, false );

        // Actually sets the graph of processes to this application
        m_initProcessGraph = new CdmwPlatformMngt::GraphElements( process_graph );

        // TODO: Notify the configuration change

        // TODO: Trace to the log manager
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

void Application_impl::setStopGraph(
    const CdmwPlatformMngt::GraphElements& process_graph )
throw( OutOfMemoryException,
       CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CORBA::SystemException )
{
    try
    {
        CDMW_WRITER_LOCK_GUARD( m_rwApplicationLock );

        // Build the map of existing elements
        ExistingElementMap existingElements;
        ProcessProxies::iterator processIt;

        for( processIt =  m_processProxies.begin();
             processIt != m_processProxies.end();
             processIt++ )
        {   
            const char* p_processName = processIt->second->get_processName();
                 
            existingElements.insert(
                PlatformMngt::ExistingElementMap::value_type(
                    p_processName,
                    0 ) );
        }

        // Check the validity of the graph of processes
        GraphChecker processGraph( process_graph, &existingElements, true );

        // Actually sets the graph of processes to this application
        m_stopProcessGraph = new CdmwPlatformMngt::GraphElements( process_graph );

        // TODO: Notify the configuration change

        // TODO: Trace to the log manager
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

void Application_impl::defineApplication(
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
    {
        CDMW_READER_LOCK_GUARD( m_rwApplicationLock );

        if( m_applicationDefined )
            throw CdmwPlatformMngt::AlreadyDone();
    }

    // Check the validity of all the process definitions
    // regarding the context of this application
    unsigned int processCount = application_def.processes.length();

    for( unsigned int processIndex=0;
         processIndex < processCount;
         processIndex++ )
    { 
        CdmwPlatformMngt::ProcessDef_var processDef
                = application_def.processes[ processIndex ];
                
        const char* p_hostName = processDef->host_name();
        const char* p_processName = processDef->name();
    
        try
        {
            // Check the host is known from the system
            CdmwPlatformMngt::HostProxy_var host =
                m_hostContainer->get_host(p_hostName);

            // TODO: Check there is no duplicate property

            if( processDef->type() != CdmwPlatformMngt::UNMANAGED_PROCESS )
            {
                CdmwPlatformMngt::ManagedProcessDef* managedProcessDef
                    = CdmwPlatformMngt::ManagedProcessDef::_downcast(
                        processDef );

                // Check there is no duplicate entity
                CdmwPlatformMngtEntity::EntityNames& entities
                    = managedProcessDef->entities();

                for( unsigned int entityIndex = 0;
                     entityIndex < entities.length();
                     entityIndex++ )
                 {
                    std::string entityName = entities[entityIndex].in();
                    
                    for( unsigned int entityIndex2 = entityIndex + 1;
                         entityIndex2 < entities.length();
                         entityIndex2++ )
                    {
                        std::string entityName2 = entities[entityIndex2].in();
                        
                        if( entityName == entityName2 )
                            throw CdmwPlatformMngt::DuplicateProcessEntity(
                                p_processName,
                                entityName.c_str() );
                    }
                 }
                    
                // Check there is no duplicate service
                CdmwPlatformMngtService::ServiceDefs& services
                    = managedProcessDef->service_definitions();

                for( unsigned int serviceIndex = 0;
                     serviceIndex < services.length();
                     serviceIndex++ )
                {
                    std::string serviceName
                        = services[serviceIndex].service_name.in();
                        
                    for( unsigned int serviceIndex2 = serviceIndex + 1;
                         serviceIndex2 < services.length();
                         serviceIndex2++ )
                    {
                        std::string serviceName2
                            = services[serviceIndex2].service_name.in();
                            
                        if( serviceName == serviceName2 )
                            throw CdmwPlatformMngt::DuplicateProcessService(
                                p_processName,
                                serviceName.c_str() );
                    }
                }
                    
                // Check there is no duplicate step
                CdmwPlatformMngt::Steps& initSteps
                    = managedProcessDef->life_cycle().init_steps;

                unsigned int initStepCount = initSteps.length();
                for( unsigned int initStepIndex = 0;
                     initStepIndex < initStepCount;
                     initStepIndex++ )
                {
                    std::string initStepName
                        = initSteps[ initStepIndex ].step_label.in();

                    for( unsigned int initStepIndex2 = initStepIndex + 1;
                         initStepIndex2 < initStepCount;
                         initStepIndex2++ )
                    {
                        std::string initStepName2
                            = initSteps[ initStepIndex2 ].step_label.in();

                        if( initStepName == initStepName2 )
                            throw CdmwPlatformMngt::DuplicateProcessStep(
                                p_processName,
                                initStepName.c_str() );
                    }
                }

                // TODO: Check there is no duplicate activity point
            }
        }
        catch( const CdmwPlatformMngt::HostNotFound& )
        {
            throw CdmwPlatformMngt::ProcessHostNotFound (p_processName, p_hostName);
        }
    }

    // Add all the processes
    for( unsigned int processIndex=0;
         processIndex < processCount;
         processIndex++ )
    { 
        CdmwPlatformMngt::ProcessDef_var processDef
                = application_def.processes[ processIndex ];
                
        const char* p_hostName = processDef->host_name();
        const char* p_processName = processDef->name();
                
        try
        {
            CdmwPlatformMngt::ProcessProxy_var processProxy
                = addProcess( processDef.in() );
        }
        catch( const CdmwPlatformMngt::HostNotFound& )
        {
            throw CdmwPlatformMngt::ProcessHostNotFound (p_processName, p_hostName);
        }
        catch( const CdmwPlatformMngt::ProcessAlreadyExists& )
        {
            throw CdmwPlatformMngt::DuplicateProcess (p_processName);
        }
    }

    // Add all the entities
    unsigned int entityCount = application_def.entities.length();
    
    for( unsigned int entityIndex=0;
         entityIndex < entityCount;
         entityIndex++ )
    {     
        try
        {
            add_entity (application_def.entities[entityIndex]);
        }
        catch( const CdmwPlatformMngtEntity::EntityAlreadyExists& )
        {
            throw CdmwPlatformMngt::DuplicateEntity (
                               application_def.entities[entityIndex]);
        }
    }

    // Add all the services
    
    unsigned int serviceCount
        = application_def.service_definitions.length();
        
    for( unsigned int serviceIndex=0;
         serviceIndex < serviceCount;
         serviceIndex++ )
    {
        try
        {
            add_service_def(
                application_def.service_definitions[ serviceIndex ] );
        }
        catch( const CdmwPlatformMngtService::ServiceAlreadyExists& )
        {
            throw CdmwPlatformMngt::DuplicateService(
                application_def.service_definitions[serviceIndex].service_name.in() );
        }
    }

    m_applicationDefined = true;
}

CdmwPlatformMngt::ProcessProxy_ptr Application_impl::addProcess(
    CdmwPlatformMngt::ProcessDef* process_def )
throw( CdmwPlatformMngt::ProcessAlreadyExists,
       CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::DuplicateEntity,
       CdmwPlatformMngt::DuplicateService,
       CdmwPlatformMngt::DuplicateStep,
       CdmwPlatformMngt::DuplicateActivityPoint,
       CORBA::SystemException )
{
    const char* p_hostName = process_def->host_name();
    const char* p_processName = process_def->name();

    {
        CDMW_READER_LOCK_GUARD( m_rwApplicationLock );

        ProcessProxies::iterator processIt = 
                 m_processProxies.find (p_processName);

        if( processIt != m_processProxies.end() )
            throw CdmwPlatformMngt::ProcessAlreadyExists();
    }

    // Create a process proxy
    ProcessProxy_impl* processProxyImpl = NULL;
    
    // Servant var to take pointer ownership and to release
    // ref counter from 2 to 1 after object activation
    PortableServer::ServantBase_var processProxyServant;
    
    try
    {
        if( process_def->type() == CdmwPlatformMngt::UNMANAGED_PROCESS )
        {
            UnmanagedProcessProxy_impl* unmanagedProcessProxy =
                new UnmanagedProcessProxy_impl (m_poa.in(), process_def, this);
            
            // take pointer ownership
            processProxyServant = unmanagedProcessProxy;

            // Set the host proxy of the process
            unmanagedProcessProxy->setHost(p_hostName);

            // Add it to the map of process proxies
            CDMW_WRITER_LOCK_GUARD (m_rwApplicationLock);
            
            m_processProxies.insert (ProcessProxies::value_type (             
                                                       p_processName,
                                                       unmanagedProcessProxy));
            processProxyImpl = unmanagedProcessProxy;
        }
        else
        {
            CdmwPlatformMngt::ManagedProcessDef* managedProcessDef
                = CdmwPlatformMngt::ManagedProcessDef::_downcast (process_def);
            
            ManagedProcessProxy_impl* managedProcessProxy =
                new ManagedProcessProxy_impl (m_poa.in(), managedProcessDef, this);
                
            // take pointer ownership
            processProxyServant = managedProcessProxy;

            // Set the host proxy of the process
            managedProcessProxy->setHost(p_hostName);

            // Add the entities of the process
            CdmwPlatformMngtEntity::EntityNames& entities
                = managedProcessDef->entities();

            for (unsigned int entityIndex = 0;
                 entityIndex < entities.length();
                 entityIndex++)
            {
                try
                {
                    managedProcessProxy->add_entity (entities[entityIndex]);
                }
                catch (const CdmwPlatformMngtEntity::EntityAlreadyExists&)
                {
                    throw CdmwPlatformMngt::DuplicateEntity(
                        entities[entityIndex]);
                }
             }

            // Add the services of the process
            CdmwPlatformMngtService::ServiceDefs& services
                = managedProcessDef->service_definitions();

            for (unsigned int serviceIndex = 0;
                 serviceIndex < services.length();
                 serviceIndex++)
            {
                try
                {
                    managedProcessProxy->add_service_def (services[serviceIndex]);
                }
                catch (const CdmwPlatformMngtService::ServiceAlreadyExists&)
                {
                    throw CdmwPlatformMngt::DuplicateService(
                        services[serviceIndex].service_name.in());
                }
            }

            // TODO: Add the properties of the process

            // Add the init steps of the process
            managedProcessProxy->setLifeCycle (managedProcessDef->life_cycle());

            /*
            // TODO: Add the activity point defs of the process
            setActivityPointDefs( process_def->activity_points() );
            */

            // Add it to the map of process proxies
            CDMW_WRITER_LOCK_GUARD (m_rwApplicationLock);

            m_processProxies.insert (ProcessProxies::value_type (                
                                              p_processName,
                                              managedProcessProxy));

            processProxyImpl = managedProcessProxy;
        }
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }

#ifdef CDMW_USE_FAULTTOLERANCE
    std::string appliProcessName(get_applicationName());
    appliProcessName+="-";    
    appliProcessName+=p_processName;

    m_ftConverter->register_process_observer(appliProcessName.c_str(), m_processObserver->_this());
#endif

    // activate processProxy servant
    processProxyImpl->activate();
    CdmwPlatformMngt::ProcessProxy_var processProxy =
                                            processProxyImpl->_this();

    return processProxy._retn();
}

void Application_impl::removeProcess(
    const char* process_name )
throw( CdmwPlatformMngt::ProcessNotFound,
       CdmwPlatformMngt::ProcessStillActive,
       CORBA::SystemException )
{
    CDMW_WRITER_LOCK_GUARD( m_rwApplicationLock );

    ProcessProxies::iterator processIt = m_processProxies.find( process_name );
    if( processIt == m_processProxies.end() )
        throw CdmwPlatformMngt::ProcessNotFound();

    ProcessProxy_impl* processProxy = processIt->second;
     
    CORBA::String_var statusInfo;
    CdmwPlatformMngt::ProcessStatus status
        = processProxy->get_status( statusInfo.out() );

    if( ( status == CdmwPlatformMngt::PROCESS_INITIALISING ) ||
        ( status == CdmwPlatformMngt::PROCESS_STEP_PERFORMED ) ||
        ( status == CdmwPlatformMngt::PROCESS_INITIALISED ) ||
        ( status == CdmwPlatformMngt::PROCESS_RUN_REQUEST ) ||
        ( status == CdmwPlatformMngt::PROCESS_RUNNING ) ||
        ( status == CdmwPlatformMngt::PROCESS_STOPPING ) )
        throw CdmwPlatformMngt::ProcessStillActive();

    // supress processProxy servant from map
    m_processProxies.erase( processIt );
    
    // deactivate processProxy servant
    processProxy->deactivate();
}

void Application_impl::removeProcesses()
{
    try
    {
        // deactivate processProxy object referenced in map
        ProcessProxies::iterator processIt;
            
        for (processIt = m_processProxies.begin(); 
             processIt != m_processProxies.end(); 
             processIt++)
        {
            ProcessProxy_impl* processProxy = processIt->second;
      
            // deactivate processProxy servant
            processProxy->deactivate();
        }
    }
    catch(...)
    {
    }
}
    
ProcessProxy_impl* Application_impl::getProcessProxy(
    const char* process_name )
throw( CdmwPlatformMngt::ProcessNotFound,
       CORBA::SystemException )
{
    CDMW_READER_LOCK_GUARD( m_rwApplicationLock );

    ProcessProxies::iterator processIt = m_processProxies.find( process_name );
    if( processIt == m_processProxies.end() )
        throw CdmwPlatformMngt::ProcessNotFound();

    return processIt->second;
}

void Application_impl::destroyInitSequencer()
{
    // Stop and destroy the init sequencer 
    if( m_initSequencer != NULL )
    {
        m_initSequencer->stop();

        delete m_initSequencer;
        m_initSequencer = NULL;
    }
    
    if( m_initSyncTaskFactory != NULL )
    {
        delete m_initSyncTaskFactory;
        m_initSyncTaskFactory = NULL;
    }
}

void Application_impl::destroyStopSequencer()
{
    // Stop and destroy the stop sequencer 
    if( m_stopSequencer != NULL )
    {
        m_stopSequencer->stop();

        delete m_stopSequencer;
        m_stopSequencer = NULL;
    }
    
    if( m_stopSyncTaskFactory != NULL )
    {
        delete m_stopSyncTaskFactory;
        m_stopSyncTaskFactory = NULL;
    }
}

void Application_impl::destroyStarter()
{
    // Stop and destroy the run starter 
    if( m_starter != NULL )
    {
        m_starter->stop();
        
        delete m_starter;
        m_starter = NULL;
    }
    
    if( m_taskFactory != NULL )
    {
        delete m_taskFactory;
        m_taskFactory = NULL;
    }
}

void Application_impl::degradation_event()
{
    bool changed = false;
    
    CDMW_MUTEX_GUARD( m_mutexMode );

    if( m_mode == CdmwPlatformMngt::NORMAL_MODE )
    {
        m_mode = CdmwPlatformMngt::DEGRADED_MODE;
        changed = true;
    }

    if( changed )
    {
        // Log and notify the change of mode
        m_modeInfo = CORBA::string_dup( "" );
        notifyModeChangeEvent();

        try
        {
            // Notify the degradation event to the application observers
            (m_applicationAck->application_observer)->notify_degradation(
                m_applicationAck->application_name.in() );
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

}

void Application_impl::suspension_event()
{
    CDMW_SM_WRITER_LOCK_GUARD( *m_stateMachine );
    
    m_stateMachine->getState()->suspension_event( m_stateMachine );
}

void Application_impl::completion_event()
{
    CDMW_SM_WRITER_LOCK_GUARD( *m_stateMachine );
    
    m_stateMachine->getState()->completion_event( m_stateMachine );
}

void Application_impl::non_completion_event()
{
    CDMW_SM_WRITER_LOCK_GUARD( *m_stateMachine );
    
    m_stateMachine->getState()->non_completion_event( m_stateMachine );
}

CdmwPlatformMngt::ApplicationName Application_impl::name()
throw( CORBA::SystemException )
{
    CORBA::String_var name = m_applicationAck->application_name.in();

    return name._retn();
}

CdmwPlatformMngt::ApplicationStatus Application_impl::get_status(
    CORBA::String_out status_info )
throw( CORBA::SystemException )    
{
    CDMW_SM_READER_LOCK_GUARD( *m_stateMachine );
    
    CORBA::String_var statusInfo = CORBA::string_dup( m_statusInfo.in() );
    status_info = statusInfo._retn();

    return m_stateMachine->getState()->status();
}

CdmwPlatformMngt::FunctioningMode Application_impl::get_mode(
    CORBA::String_out mode_info )
throw( CORBA::SystemException )
{
    CDMW_MUTEX_GUARD( m_mutexMode );

    CORBA::String_var modeInfo = m_modeInfo;
    mode_info = modeInfo._retn();

    return m_mode;
}

CdmwPlatformMngt::ApplicationSnapshot* Application_impl::get_snapshot()
throw( CORBA::SystemException )
{
    try
    {
        CDMW_READER_LOCK_GUARD( m_rwApplicationLock );

        CdmwPlatformMngt::ProcessesSnapshot_var processesSnapshot
            = new CdmwPlatformMngt::ProcessesSnapshot();

        // Get the snapshot of all the processes
        ProcessProxies::iterator processIt;
        unsigned int i;

        processesSnapshot->length( m_processProxies.size() );
        for( processIt = m_processProxies.begin(), i = 0;
             processIt != m_processProxies.end();
             processIt++, i++ )
        {
            processesSnapshot[i]
                = *(processIt->second->get_snapshot());
        }

        // Gets all the entities
        CdmwPlatformMngtEntity::Entities_var entities;
        CdmwPlatformMngtEntity::EntitiesIterator_var entitiesIt;

        get_all_entities(
            get_number_of_entities(),
            entities.out(),
            entitiesIt.out() );

        // Create an application snapshot
        CORBA::String_var modeInfo;
        CORBA::String_var statusInfo;
        
        CdmwPlatformMngt::ApplicationSnapshot_var snapshot
            = new CdmwPlatformMngt::ApplicationSnapshot;

        snapshot->application_name
            = CORBA::string_dup( m_applicationAck->application_name.in() );
        snapshot->application_status = get_status( statusInfo );
        snapshot->status_info = statusInfo;
        snapshot->application_mode = get_mode( modeInfo );
        snapshot->mode_info = modeInfo;
        snapshot->application_processes = processesSnapshot.in();
        snapshot->application_entities = entities.in();


        return snapshot._retn();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

CdmwPlatformMngt::ProcessProxies* Application_impl::get_processes(
    CORBA::ULong how_many,
    CdmwPlatformMngt::ProcessProxiesIterator_out processes_iterator )
throw( CORBA::SystemException )
{
    try
    {
        CDMW_READER_LOCK_GUARD( m_rwApplicationLock );

        ProcessProxies::iterator processProxiesIt;

        // Get the how_many application references   
        size_t len = m_processProxies.size();

        // set length of requested sequence and rest of sequence
        size_t requested_len = len;
        size_t rest_len = 0;

        // adjust to how_many requested       
        if (len > how_many)
        {
            requested_len = how_many;
            rest_len = len - how_many;
        }

        // Get the how_many process proxies references
        unsigned int count;
        CdmwPlatformMngt::ProcessProxies_var processProxies
            = new CdmwPlatformMngt::ProcessProxies();

        processProxies->length( requested_len );
        for( processProxiesIt=m_processProxies.begin(), count=0;
             count < requested_len;
             processProxiesIt++, count++ )
        {
            processProxies[ count ]
                = processProxiesIt->second->_this();
        }

        // Get an iterator on the rest of the process proxies
        std::list< CdmwPlatformMngt::ProcessProxy_var > processProxiesRemaining;
        for( ; processProxiesIt != m_processProxies.end(); processProxiesIt++ )
        {
            CdmwPlatformMngt::ProcessProxy_var processProxy
                = processProxiesIt->second->_this();
                    
            processProxiesRemaining.push_back( processProxy );
        }

        CdmwPlatformMngt::ProcessProxiesIterator_var processProxiesRemainingIt
            = ProcessProxiesIterator_impl::create( processProxiesRemaining );

        // Assign the iterator to the processes_iterator
        processes_iterator = processProxiesRemainingIt._retn();

        return processProxies._retn();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

CdmwPlatformMngt::ProcessProxy_ptr Application_impl::get_process(
    const char* process_name )
throw( CdmwPlatformMngt::ProcessNotFound,
       CORBA::SystemException )
{
    CDMW_READER_LOCK_GUARD( m_rwApplicationLock );

    ProcessProxies::iterator processIt = m_processProxies.find(
        process_name );

    if( processIt == m_processProxies.end() )
        throw CdmwPlatformMngt::ProcessNotFound();

    return processIt->second->_this();
}

void Application_impl::define(
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
    LogMngr::logMessage(
        INF,
        MSG_ID_APPLICATION_DEFINE,
        m_applicationAck->application_name.in() );

    CDMW_SM_READER_LOCK_GUARD( *m_stateMachine );
    
    m_stateMachine->getState()->define(
        m_stateMachine,
        application_def );
}

CdmwPlatformMngt::ProcessProxy_ptr Application_impl::add_process(
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
    LogMngr::logMessage(
        INF,
        MSG_ID_PROCESS_ADD,
        process_def->name(),
        m_applicationAck->application_name.in() );

    CDMW_SM_READER_LOCK_GUARD( *m_stateMachine );
    
    return m_stateMachine->getState()->add_process(
        m_stateMachine,
        process_def );
}

void Application_impl::remove_process(
    const char* process_name )
throw( CdmwPlatformMngt::ProcessNotFound,
       CdmwPlatformMngt::ProcessStillActive,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    LogMngr::logMessage(
        INF,
        MSG_ID_PROCESS_REMOVE,
        process_name,
        m_applicationAck->application_name.in() );

    CDMW_SM_READER_LOCK_GUARD( *m_stateMachine );
    
    m_stateMachine->getState()->remove_process(
        m_stateMachine,
        process_name );
}

void Application_impl::set_init_graph(
    const CdmwPlatformMngt::GraphElements& process_graph )
throw( CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    LogMngr::logMessage(
        INF,
        MSG_ID_APPLICATION_SET_INIT_GRAPH,
        m_applicationAck->application_name.in() );

    CDMW_SM_READER_LOCK_GUARD( *m_stateMachine );

    try{

    m_stateMachine->getState()->set_init_graph( m_stateMachine, process_graph );
   
       }
    catch( OutOfMemoryException )
      {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
      }

   

}

void Application_impl::set_stop_graph(
    const CdmwPlatformMngt::GraphElements& process_graph )
throw( CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    LogMngr::logMessage(
        INF,
        MSG_ID_APPLICATION_SET_STOP_GRAPH,
        m_applicationAck->application_name.in() );

    CDMW_SM_READER_LOCK_GUARD( *m_stateMachine );
    try 
      {
      
    m_stateMachine->getState()->set_stop_graph( m_stateMachine, process_graph );
   
      }
    catch( OutOfMemoryException )
      {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
      }

   

}

void Application_impl::initialise(
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    LogMngr::logMessage(
        INF,
        MSG_ID_APPLICATION_INITIALISE,
        m_applicationAck->application_name.in() );

    CDMW_SM_WRITER_LOCK_GUARD( *m_stateMachine );
    
    m_stateMachine->getState()->initialise( m_stateMachine, startup_kind );
}

CdmwPlatformMngt::LongRequestStatus Application_impl::initialise_and_wait(
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    m_mutex.lock();
    
    initialise( startup_kind );
    
    m_initialisation_condition.wait();
    m_mutex.unlock();

    return m_initialisation_status;
}

void Application_impl::resume()
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    LogMngr::logMessage(
        INF,
        MSG_ID_APPLICATION_RESUME,
        m_applicationAck->application_name.in() );

    CDMW_SM_WRITER_LOCK_GUARD( *m_stateMachine );
    
    m_stateMachine->getState()->resume( m_stateMachine );
}

CdmwPlatformMngt::LongRequestStatus Application_impl::resume_and_wait()
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    m_mutex.lock();
    
    resume();
    
    m_initialisation_condition.wait();
    m_mutex.unlock();

    return m_initialisation_status;
}

void Application_impl::acknowledge()
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    LogMngr::logMessage(
        INF,
        MSG_ID_APPLICATION_ACKNOWLEDGE,
        m_applicationAck->application_name.in() );

    CDMW_MUTEX_GUARD( m_mutexMode );

    m_mode = CdmwPlatformMngt::NORMAL_MODE;

    // Log and notify the change of mode
    m_modeInfo = CORBA::string_dup( "" );
    notifyModeChangeEvent();
}

void Application_impl::run()
throw( CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    LogMngr::logMessage(
        INF,
        MSG_ID_APPLICATION_RUN,
        m_applicationAck->application_name.in() );

    CDMW_SM_WRITER_LOCK_GUARD( *m_stateMachine );
    
    m_stateMachine->getState()->run( m_stateMachine );
}

void Application_impl::stop( CORBA::Boolean emergency )
throw( CORBA::SystemException )
{
    LogMngr::logMessage(
        INF,
        MSG_ID_APPLICATION_STOP,
        m_applicationAck->application_name.in() );

    CDMW_SM_WRITER_LOCK_GUARD( *m_stateMachine );
    
    m_emergency = emergency;
    m_stateMachine->getState()->stop( m_stateMachine, emergency );
}

CdmwPlatformMngt::LongRequestStatus Application_impl::stop_and_wait(
    CORBA::Boolean emergency )
throw( CORBA::SystemException )
{
    {
        CDMW_SM_READER_LOCK_GUARD( *m_stateMachine );
       
        CdmwPlatformMngt::ApplicationStatus appStatus = 
                            m_stateMachine->getState()->status();
                            
        if (appStatus == CdmwPlatformMngt::APPLICATION_INITIAL ||
            appStatus == CdmwPlatformMngt::APPLICATION_STOPPED)
        {
            return CdmwPlatformMngt::LONG_REQUEST_COMPLETED;
        }
    }
       
    m_mutex.lock();
         
    stop( emergency );
    
    m_stop_condition.wait();

    CdmwPlatformMngt::LongRequestStatus status = m_stop_status;
    m_mutex.unlock();

    return status;
}

CORBA::ULong Application_impl::get_number_of_entities()
throw( CORBA::SystemException )
{
    return m_rwEntityContainer->get_number_of_entities();
}

void Application_impl::get_all_entity_names(
    CORBA::ULong how_many,
    CdmwPlatformMngtEntity::EntityNames_out entity_names,
    CdmwPlatformMngtEntity::EntityNamesIterator_out rest )
throw( CORBA::SystemException )
{
    m_rwEntityContainer->get_all_entity_names(
        how_many, entity_names, rest );
}

CdmwPlatformMngtEntity::EntityStatus Application_impl::get_entity_status(
    const char* entity_name,
    CORBA::String_out entity_info )
throw( CdmwPlatformMngtEntity::EntityNotFound,
       CORBA::SystemException )
{
    return m_rwEntityContainer->get_entity_status( entity_name, entity_info );
}

CORBA::Boolean Application_impl::get_entities(
    const CdmwPlatformMngtEntity::EntityNames& entity_names,
    CdmwPlatformMngtEntity::Entities_out nentities )
throw( CORBA::SystemException )
{
    return m_rwEntityContainer->get_entities(
        entity_names, nentities );
}

void Application_impl::get_all_entities(
    CORBA::ULong how_many,
    CdmwPlatformMngtEntity::Entities_out nentities,
    CdmwPlatformMngtEntity::EntitiesIterator_out rest )
throw( CORBA::SystemException )
{
    m_rwEntityContainer->get_all_entities(
        how_many, nentities, rest );
}

void Application_impl::add_entity(
    const char* entity_name )
throw( CdmwPlatformMngtEntity::EntityAlreadyExists,
       CORBA::SystemException)
{
    m_rwEntityContainer->add_entity( entity_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void Application_impl::remove_entity(
    const char* entity_name )
throw( CdmwPlatformMngtEntity::EntityNotFound,
       CORBA::SystemException )
{
    m_rwEntityContainer->remove_entity( entity_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

CdmwPlatformMngtService::ServiceProviderID* Application_impl::get_service_def(
    const char* service_name )
throw( CdmwPlatformMngtService::ServiceNotFound,
       CORBA::SystemException )
{
    return m_rwServiceDefContainer->get_service_def( service_name );
}

void Application_impl::add_service_def(
    const CdmwPlatformMngtService::ServiceDef& service_def )
throw( CdmwPlatformMngtService::ServiceAlreadyExists,
       CORBA::SystemException )
{
    m_rwServiceDefContainer->add_service_def( service_def );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void Application_impl::remove_service_def(
    const char* service_name )
throw( CdmwPlatformMngtService::ServiceNotFound,
       CORBA::SystemException )
{
    m_rwServiceDefContainer->remove_service_def( service_name );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

CORBA::ULong Application_impl::get_number_of_properties()
  throw( CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );
}

void Application_impl::get_all_property_names(
    CORBA::ULong how_many,
    CosPropertyService::PropertyNames_out property_names,
    CosPropertyService::PropertyNamesIterator_out rest )
  throw( CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );
}

CORBA::Any* Application_impl::get_property_value(
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

CORBA::Boolean Application_impl::get_properties(
    const CosPropertyService::PropertyNames& property_names,
    CosPropertyService::Properties_out nproperties )
throw( CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );
}

void Application_impl::get_all_properties(
    CORBA::ULong how_many,
    CosPropertyService::Properties_out nproperties,
    CosPropertyService::PropertiesIterator_out rest )
throw( CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );
}

void Application_impl::set_property(
    const char* property_name,
    const CORBA::Any& property_value )
  throw( CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );

    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void Application_impl::remove_property(
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

void Application_impl::is_alive()
  throw( CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );
}

CdmwPlatformMngt::SupervisionObserver_ptr Application_impl::register_observer(
    const char* observer_name,
    CdmwPlatformMngt::SupervisionObserver_ptr observer )
throw( CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );
}

CdmwPlatformMngt::SupervisionObserver_ptr Application_impl::unregister_observer(
    const char* observer_name )
throw( CdmwPlatformMngt::ObserverNotFound,
       CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );
}



const char* Application_impl::get_applicationName()
{
    return (m_applicationAck->application_name).in();
}




} // End namespace PlatformMngt
} // End namespace Cdmw

