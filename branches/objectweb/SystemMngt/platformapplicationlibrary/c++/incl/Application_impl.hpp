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


#ifndef INCL_PLATFORMMNGT_APPLICATION_IMPL_HPP
#define INCL_PLATFORMMNGT_APPLICATION_IMPL_HPP

#include "SystemMngt/platformapplicationlibrary/ApplicationStates.hpp"

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/osthreads/Condition.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/ReaderWriterLock.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtApplication.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtApplicationAgent.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntity.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtHostProxy.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtService.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtDaemon.stub.hpp"

#ifdef CDMW_USE_FAULTTOLERANCE
#include "SystemMngt/platformapplicationlibrary/CdmwPlatformMngtFTConverter.stub.hpp"
#endif

#include "SystemMngt/platformlibrary/StateMachineTemplate.hpp"
#include "SystemMngt/platformlibrary/DeactivableServant_impl.hpp"
#include "SystemMngt/platformlibrary/EventHandler.hpp"
#include "SystemMngt/platformlibrary/EntityContainer_impl.hpp"
#include "SystemMngt/platformlibrary/ServiceDefContainer_impl.hpp"

#include <string>
#include <set>
#include <map>

namespace Cdmw
{
namespace PlatformMngt
{

class EntityObserver_impl;
class EventNotifierStoppedException;
class ProcessProxy_impl;
class ProcessObserver_impl;
class RWEntityContainer_impl;
class RWServiceDefContainer_impl;
class ServiceBroker_impl;
class SupervisionEventNotifier;
class TaskFactory;
class Starter;
class SynchronisableTaskFactory;
class Sequencer;
class ApplicationAgentObserver_impl;
class GraphElementFactory;
class ApplicationEntityStatusChangeFactory;
class ApplicationStatusChangeFactory;
class ApplicationModeChangeFactory;


/**
 * Purpose:
 * Thrown when the created application agent cannot be found.
 */
class AgentNotFoundException : public Exception
{
public:
    AgentNotFoundException()
        : Exception (Exception::SAFE, "Created Application Agent cannot be found")
    {
    };
};


/**
 *Purpose:
 *<p>
 */
class Application_impl : public virtual DeactivableServant_impl,
                         public virtual POA_CdmwPlatformMngt::Application,                         
                         public virtual PortableServer::RefCountServantBase
{

public:
    friend class ProcessProxy_impl;
  
    typedef std::map< std::string, ProcessProxy_impl* > ProcessProxies;


    /**
     * The type of process invalidation.
     */
    enum InvalidationType
    {
        AGENT_DEATH,
        HOST_UNAVAILABLE
    };

protected:
    /**
     * The mutex used with the initialisation condition and the stop condition.
     */
    OsSupport::Mutex m_mutex;

private:
        
    /**
     * The POA.
     */
    PortableServer::POA_var m_poa;

    /**
     * The read/write lock to protect access to the internal data.
     */
    OsSupport::ReaderWriterLock m_rwApplicationLock;

    /**
     * The application agent observer.
     */
    ApplicationAgentObserver_impl* m_agentObserver;

    /**
     * The functioning mode.
     */
    CdmwPlatformMngt::FunctioningMode m_mode;

    /**
     * The information about the mode this application.
     */
    CORBA::String_var m_modeInfo;

    /**
     * The initialisation variable condition for long operation.
     */
    OsSupport::Condition m_initialisation_condition;

    /**
     * The stop variable condition for long operation.
     */
    OsSupport::Condition m_stop_condition;

    /**
     * The status of a long initialisation operation.
     */
    CdmwPlatformMngt::LongRequestStatus m_initialisation_status;

    /**
     * The status of a long stop operation.
     */
    CdmwPlatformMngt::LongRequestStatus m_stop_status;

    /**
     * The mutex used for changing the functioning mode.
     */
    OsSupport::Mutex m_mutexMode;

    /**
     * The actual entity container.
     */
    EntityContainer_impl::EntityContainer* m_actualEntityContainer;

    /**
     * The actual service def container.
     */
    ServiceDefContainer_impl::ServiceDefContainer* m_actualServiceDefContainer;

    /**
     * The read/write service container.
     */
    RWServiceDefContainer_impl* m_rwServiceDefContainer;

    /**
     * Indicates whether the application has been defined.
     */
    bool m_applicationDefined;
    
    /**
     * The reader/writer lock used to manage concurrent thread safe access to
     * RunSet.
     */
    Cdmw::OsSupport::ReaderWriterLock m_rwAgentHosts_lock;
    
    /**
     *Purpose:
     *<p> Structure used to contain host information.
     */
    struct HostData
    {
        /**
        * The Host daemon persistent object reference
        */
        CdmwPlatformMngt::Daemon_var daemonObj;            
    };
                
    /**
     *Purpose:
     *<p> The type used to contain the host names where application agent is created.
     */
    typedef std::map<std::string, HostData*> AgentHosts;
    
    /**
     *Purpose:
     *<p> The set containing the host names where application agent is created.
     */
    AgentHosts m_agentHosts;
    
    
    /**
     * The Process Observer object var to take pointer ownership
     */
    PortableServer::ServantBase_var m_processObserverServant;
    
    /**
     * The Agent Observer object var to take pointer ownership
     */
    PortableServer::ServantBase_var m_agentObserverServant;
    
    /**
     * The Entity Observer object var to take pointer ownership
     */
    PortableServer::ServantBase_var m_entityObserverServant;

    /**
     * The Service Def Container object var to take pointer ownership
     */
    PortableServer::ServantBase_var m_serviceDefContainerServant;
    
    /**
     * The RW Service Def Container object var to take pointer ownership
     */
    PortableServer::ServantBase_var m_rwServiceDefContainerServant;
    
    /**
     * The RW Entity Container object var to take pointer ownership
     */
    PortableServer::ServantBase_var m_rwEntityContainerServant;
    
    /**
     * The Service Broker object var to take pointer ownership
     */
    PortableServer::ServantBase_var m_serviceBrokerServant;
    
    

public:
    /**
     * The information about the status this application.
     */
    CORBA::String_var m_statusInfo;

    /**
     * The list of process proxies managed by this application.
     */
    ProcessProxies m_processProxies;

    /**
     * The process observer.
     */
    ProcessObserver_impl* m_processObserver;

#ifdef CDMW_USE_FAULTTOLERANCE
    /**
     * The replicationManagerIOGR
     */
    std::string m_replicationManagerIOGR;    
    
    /**
     * The reference of the faut tolerance converter (fault report to systemmngt event)
     */
    CdmwPlatformMngt::FTConverter_var m_ftConverter;    
#endif    
    /**
     * The entity observer.
     */
    EntityObserver_impl* m_entityObserver;

    /**
     * The services' broker.
     */
    ServiceBroker_impl* m_serviceBroker;

    /**
     * The application acknowledgement data.
     */
    CdmwPlatformMngt::ApplicationAck_var m_applicationAck;

    /**
     * The host container.
     */
    CdmwPlatformMngt::HostContainer_var m_hostContainer;

    /**
     * The supervision event notifier.
     */
    SupervisionEventNotifier* m_eventNotifier;

    /**
     * The associated state machine.
     */
    ApplicationStateMachine* m_stateMachine;

    /**
     * The observer of the system entities.
     */
    CdmwPlatformMngtEntity::SystemEntityObserver_var m_systemEntityObserver;

    /**
     * The service def container servant.
     */
    ServiceDefContainer_impl* m_serviceDefContainer;
    
    /**
     * The read/write entity container.
     */
    RWEntityContainer_impl* m_rwEntityContainer;

    /**
     * The handler of events.
     */
    EventHandler m_eventHandler;

    /**
     * The startup kind.
     */
    CdmwPlatformMngtBase::StartupKind_var m_startup_kind;

    /**
     * Indicates whether to stop the application normaly or with emergency.
     */
    CORBA::Boolean m_emergency;

    /**
     * The dependency graph of processes used for the initialisation of the application.
     */
    CdmwPlatformMngt::GraphElements_var m_initProcessGraph;

    /**
     * The dependency graph of processes used for the stop of the application.
     */
    CdmwPlatformMngt::GraphElements_var m_stopProcessGraph;

    /**
     * Indicates whether the init processes graph has changed or not.
     */
    bool m_newInitGraph;

    /**
     * The asynchronous task factory.
     */
    TaskFactory* m_taskFactory;

    /**
     * The synchronisable init task factory.
     */
    SynchronisableTaskFactory* m_initSyncTaskFactory;

    /**
     * The synchronisable stop task factory.
     */
    SynchronisableTaskFactory* m_stopSyncTaskFactory;

    /**
     * The task sequencer for processes initialisation.
     */
    Sequencer* m_initSequencer;

    /**
     * The task sequencer for processes stopping.
     */
    Sequencer* m_stopSequencer;

    /**
     * The task starter for running concurently processes.
     */
    Starter* m_starter;

    /**
     * The graph element factory.
     */
    GraphElementFactory* m_graphElementFactory;

    /**
     * The application entity status change factory.
     */
    ApplicationEntityStatusChangeFactory* m_appEntityStatusChangeFactory;

    /**
     * The application status change factory.
     */
    ApplicationStatusChangeFactory* m_appStatusChangeFactory;

    /**
     * The application mode change factory.
     */
    ApplicationModeChangeFactory* m_appModeChangeFactory;

    /**
     * The states of an application.
     */
    static ApplicationInitial* M_initial;
    static ApplicationInitialising* M_initialising;
    static ApplicationInitialisationSuspended* M_initialisationSuspended;
    static ApplicationInitialised* M_initialised;
    static ApplicationRunRequest* M_runRequest;
    static ApplicationRunning* M_running;
    static ApplicationStopping* M_stopping;
    static ApplicationStopped* M_stopped;
    static ApplicationInitialisationIncomplete* M_initialisationIncomplete;

private:
    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    Application_impl(
        Application_impl& rhs );

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    Application_impl& operator=(
        const Application_impl& rhs );

public:
    /**
     *Purpose:
     *<p> Default constructor.
     *
     *@param eventNotifier the supervision observer to be notified of events.
     *
     *@exception BadParameterException if the parameter is invalid.
     *@exception AlreadyDoneException if the state machine is already running.
     *@exception UnknownStateException if stateName doesn't refer to a known
     * state.
     *@exception OutOfMemoryException Lack of memory.
     */
#ifdef CDMW_USE_FAULTTOLERANCE
    Application_impl(
        PortableServer::POA_ptr poa,
        SupervisionEventNotifier* eventNotifier,
        std::string replicationManagerIOGR,
        CdmwPlatformMngt::FTConverter_ptr ftConverter)
            throw( BadParameterException,
                   AlreadyDoneException,
                   UnknownStateException,
                   OutOfMemoryException );
#else
    Application_impl(
        PortableServer::POA_ptr poa,
        SupervisionEventNotifier* eventNotifier )
            throw( BadParameterException,
                   AlreadyDoneException,
                   UnknownStateException,
                   OutOfMemoryException );
#endif

    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual ~Application_impl();

public:
    /**
     *Purpose:
     *<p> Creates all the state instances (states are singleton).
     *
     *@exception OutOfMemoryException Lack of memory.
     */
    static void createStates()
        throw( OutOfMemoryException );

    /**
     *Purpose:
     *<p> Destroys all the states.
     */
    static void destroyStates();

    /**
     *Purpose:
     *<p> Ends the creation of the application servant by taking in additional
     * information provided by the application acknowledgement data.
     *
     *@param applicationAck the acknowledgement data.
     *
     *@exception OutOfMemoryException Lack of memory.
     */
    void endCreation(
        const CdmwPlatformMngt::ApplicationAck& applicationAck )
            throw( OutOfMemoryException,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p> Gets the application agent representing this application on the
     * specified host. If not existing, It creates one onto the host.
     *
     *@param hostName the host running a part of this application.
     *
     *@return the application agent or a NIL reference if there is no agent
     * for this application.
     *
     *@exception HostNotFound if the host cannot be found.
     *@exception HostNotReachable if the host doesn't respond.
     *@exception CreationError if the application agent returned a fatal error
     * and aborts its creation.
     *@exception ApplicationAgentAlreadyExists if the application agent
     * already exists.
     *@exception InvalidInfo if agent_info contains invalid information.
     *@exception CreationTimeout if the agent has not acknowledged its
     * creation during the allowed timeout.
     *@exception AgentNotFoundException if created agent cannot be found
     *@exception CORBA::SystemException
     */
    CdmwPlatformMngt::ApplicationAgent_ptr getAgent(
        const char* hostName )
            throw( CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::HostNotReachable,
                   CdmwPlatformMngt::CreationError,
                   CdmwPlatformMngt::ApplicationAgentAlreadyExists,
                   CdmwPlatformMngt::InvalidInfo,
                   CdmwPlatformMngt::CreationTimeout,
                   AgentNotFoundException,
                   CORBA::SystemException );
                   
                   
    /**
     *Purpose:
     *<p> Kill all application agents created by host daemons for this application
     *
     */
    void killAllAgents ();


    /**
     *Purpose:
     *<p> If there active processes running on the specified host, the
     * application enters the degraded mode and invalidates these processes.
     */
    void invalidateProcesses(
        CdmwPlatformMngtBase::EventHeader header,
        const char* hostName,
        InvalidationType reason );

    /**
     *Purpose:
     *<p> Creates an application agent death event. This method is
     * designed to be used outside the state machine.
     */
    void notifyAgentDeathEvent(
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* hostName );

    /**
     *Purpose:
     *<p> Creates a process status change event. This method is
     * designed to be used outside the state machine.
     */
    void notifyModeChangeEvent();

    /**
     *Purpose:
     *<p> Notifies the application's status has changed. This method is
     * designed to be used inside the state machine.
     */
    void notifyStatusChangeEvent();

    /**
     *<p> Notifies the supervision observer that an entity of this
     * process has its status changed.
     *
     *@param entity_name the name of the entity.
     *@param entity_status the status of the entity.
     *@param entity_info some additional information.
     */
    void notifyEntityStatusChangeEvent(
        const char* entity_name,
        CdmwPlatformMngtEntity::EntityStatus entity_status,
        const char* entity_info );

    /**
     *Purpose:
     *<p> Sets the initialisation status for long operation.
     *
     *@param status the initialisation status.
     */
    void setInitialisationStatus(
        CdmwPlatformMngt::LongRequestStatus status );

    /**
     *Purpose:
     *<p> Sets the stop status for long operation.
     *
     *@param status the initialisation status.
     */
    void setStopStatus(
        CdmwPlatformMngt::LongRequestStatus status );

    /**
     *Purpose:
     *<p> Actually sets the init graph with the specified graph.
     *
     *@param process_graph the graph of process.
     *
     *@exception OutOfMemoryException Lack of memory.
     *@exception ReferenceNotFound if an element of the graph refers to an
     * unknown process
     *@exception CircularReference if a circular reference has been detected
     * in the graph
     *@exception StepOutOfBound if the step defined in an element graph is
     * out of bound for the corresponding process.
     *@exception StepJump if a jump has been detected between steps of the
     * same process.
     *@exception IncompatibleStatus if the status of the application doesn't
     * allow the operation.
     */
    void setInitGraph(
        const CdmwPlatformMngt::GraphElements& process_graph )
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p> Actually sets the stop graph with the specified graph.
     *
     *@param process_graph the graph of process.
     *
     *@exception OutOfMemoryException Lack of memory.
     *@exception ReferenceNotFound if an element of the graph refers to an
     * unknown process
     *@exception CircularReference if a circular reference has been detected
     * in the graph
     *@exception StepOutOfBound if the step defined in an element graph is
     * out of bound for the corresponding process.
     *@exception StepJump if a jump has been detected between steps of the
     * same process.
     *@exception IncompatibleStatus if the status of the application doesn't
     * allow the operation.
     */
    void setStopGraph(
        const CdmwPlatformMngt::GraphElements& process_graph )
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p> Defines the initial configuration of the application.
     *
     * @param application_def the definition of the application.
     *
     * @exception DuplicateEntity if there is a duplicate entity name among
     *  the application's entities
     * @exception DuplicateService if there is a duplicate service name among
     *  the application's service definitions
     * @exception DuplicateProcess if there is a duplicate process name among
     *  the application's processes
     * @exception ProcessHostNotFound if an unknown host is used by one of
     *  the application's processes
     * @exception DuplicateProcessEntity if there is a duplicate entity name
     *  among the entities of one of the application processes
     * @exception DuplicateProcessService if there is a duplicate service
     *  name among the services of one of the application processes
     * @exception DuplicateProcessStep if there is a duplicate step label
     *  among the steps of one of the application processes
     * @exception DuplicateProcessActivityPoint if there is a duplicate
     *  name among the activity points of one of the application processes
     */
    void defineApplication(
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
                  CORBA::SystemException );

    /**
     *Purpose:
     *<p> Actually adds the specified process.
     *
     *@param process_def the definition of the process.
     *
     *@return the CORBA reference of the process proxy.
     *
     *@exception ProcessAlreadyExists if the process name is already used
     * within the application.
     *@exception HostNotFound if the host where the process will be run is
     * not found.
     *@exception DuplicateEntity if there is a duplicate entity name among
     * the entities of the process
     *@exception DuplicateService if there is a duplicate service name among
     * the service definitions of the process
     *@exception DuplicateStep if there is a duplicate step label among
     * the steps of the process
     *@exception DuplicateActivityPoint if there is a duplicate name
     * among the activity point definitions of the process
     *@exception IncompatibleStatus if the status of the application doesn't
     * allow the operation.
     */
    CdmwPlatformMngt::ProcessProxy_ptr addProcess(
        CdmwPlatformMngt::ProcessDef* process_def )
            throw( CdmwPlatformMngt::ProcessAlreadyExists,
                   CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::DuplicateActivityPoint,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p> Actually removes the specified process.
     *
     *@param process_name the name identifying the process.
     *
     *@exception ProcessNotFound if process_name doesn't denote
     * an existing process.
     *@exception ProcessStillActive if the process is still active
     */
    void removeProcess(
        const char* process_name )
            throw( CdmwPlatformMngt::ProcessNotFound,
                   CdmwPlatformMngt::ProcessStillActive,
                   CORBA::SystemException );
                   
    /**
     *Purpose:
     *<p> Actually removes all processes of application.
     *
     */
    void removeProcesses();

    /**
     *Purpose:
     *<p> Stops and destroys the init sequencer.
     */
    void destroyInitSequencer();

    /**
     *Purpose:
     *<p> Stops and destroys the stop sequencer.
     */
    void destroyStopSequencer();

    /**
     *Purpose:
     *<p> Stops and destroys the run starter.
     */
    void destroyStarter();

    /**
     *Purpose:
     *<p> Get the process proxy associated to the identified process.
     *
     *@param process_name the name identifying the process.
     *
     *@return the process proxy.
     *
     *@exception ProcessNotFound if process_name doesn't denote
     * an existing process.
     */
    ProcessProxy_impl* getProcessProxy(
        const char* process_name )
            throw( CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p> Puts the application into the degraded mode.
     */
    void degradation_event();

    /**
     *Purpose:
     *<p> Indicates the application has to suspend its job.
     */
    void suspension_event();

    /**
     *Purpose:
     *<p> Indicates the application has completed its job.
     */
    void completion_event();

    /**
     *Purpose:
     *<p> Indicates the application has not completed its job.
     */
    void non_completion_event();

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/name:1.0
     * operation
     */
    CdmwPlatformMngt::ApplicationName name()
              throw(CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/status:1.0
     * operation
     */
    CdmwPlatformMngt::ApplicationStatus status();

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/get_status:1.0
     * operation
     */
    CdmwPlatformMngt::ApplicationStatus get_status(
        CORBA::String_out status_info )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/get_mode:1.0
     * operation
     */
    CdmwPlatformMngt::FunctioningMode get_mode(
        CORBA::String_out mode_info )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/get_snapshot:1.0
     * operation
     */
    CdmwPlatformMngt::ApplicationSnapshot* get_snapshot()
              throw(CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/get_processes:1.0
     * operation
     */
    CdmwPlatformMngt::ProcessProxies* get_processes(
        CORBA::ULong how_many,
        CdmwPlatformMngt::ProcessProxiesIterator_out processes_iterator )
              throw(CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/get_process:1.0
     * operation
     */
    CdmwPlatformMngt::ProcessProxy_ptr get_process(
        const char* process_name )
            throw( CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/define:1.0
     * operation
     */
    void define(
        const CdmwPlatformMngt::ApplicationDef& application_def )
            throw( CdmwPlatformMngt::DuplicateEntity,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateProcess,
                   CdmwPlatformMngt::ProcessHostNotFound,
                   CdmwPlatformMngt::DuplicateProcessEntity,
                   CdmwPlatformMngt::DuplicateProcessService,
                   CdmwPlatformMngt::DuplicateProcessStep,
                   CdmwPlatformMngt::DuplicateProcessActivityPoint,
                   CdmwPlatformMngt::AlreadyDone,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/add_process:1.0
     * operation
     */
    CdmwPlatformMngt::ProcessProxy_ptr add_process(
        CdmwPlatformMngt::ProcessDef* process_def )
            throw( CdmwPlatformMngt::ProcessAlreadyExists,
                   CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::DuplicateActivityPoint,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/remove_process:1.0
     * operation
     */
    void remove_process(
        const char* process_name )
            throw( CdmwPlatformMngt::ProcessNotFound,
                   CdmwPlatformMngt::ProcessStillActive,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/set_init_graph:1.0
     * operation
     */
    void set_init_graph(
        const CdmwPlatformMngt::GraphElements& process_graph )
            throw( CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/set_stop_graph:1.0
     * operation
     */
    void set_stop_graph(
        const CdmwPlatformMngt::GraphElements& process_graph )
            throw( CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,

                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/initialise:1.0
     * operation
     */
    void initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/initialise_and_wait:1.0
     * operation
     */
    CdmwPlatformMngt::LongRequestStatus initialise_and_wait(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/resume:1.0
     * operation
     */
    void resume()
        throw( CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/resume_and_wait:1.0
     * operation
     */
    CdmwPlatformMngt::LongRequestStatus resume_and_wait()
        throw( CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/acknowledge:1.0
     * operation
     */
    void acknowledge()
        throw( CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/run:1.0
     * operation
     */
    void run()
        throw( CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/stop:1.0
     * operation
     */
    void stop(
        CORBA::Boolean emergency )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/stop_and_wait:1.0
     * operation
     */
    CdmwPlatformMngt::LongRequestStatus stop_and_wait(
        CORBA::Boolean emergency )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngtEntity/EntityContainer/get_number_of_entities:1.0
     * operation
     */
    CORBA::ULong get_number_of_entities()
        throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngtEntity/EntityContainer/get_all_entity_names:1.0
     * operation
     */
    void get_all_entity_names(
        CORBA::ULong how_many,
        CdmwPlatformMngtEntity::EntityNames_out entity_names,
        CdmwPlatformMngtEntity::EntityNamesIterator_out rest )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngtEntity/EntityContainer/get_entity_status:1.0
     * operation
     */
    CdmwPlatformMngtEntity::EntityStatus get_entity_status(
        const char* entity_name,
        CORBA::String_out entity_info )
            throw( CdmwPlatformMngtEntity::EntityNotFound,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngtEntity/EntityContainer/get_entities:1.0
     * operation
     */
    CORBA::Boolean get_entities(
        const CdmwPlatformMngtEntity::EntityNames& entity_names,
        CdmwPlatformMngtEntity::Entities_out nentities )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngtEntity/EntityContainer/get_all_entities:1.0
     * operation
     */
    void get_all_entities(
        CORBA::ULong how_many,
        CdmwPlatformMngtEntity::Entities_out nentities,
        CdmwPlatformMngtEntity::EntitiesIterator_out rest )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngtEntity/RWEntityContainer/add_entity:1.0
     * operation
     */
    void add_entity(
        const char* entity_name )
            throw( CdmwPlatformMngtEntity::EntityAlreadyExists,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngtEntity/RWEntityContainer/remove_entity:1.0
     * operation
     */
    void remove_entity(
        const char* entity_name )
            throw( CdmwPlatformMngtEntity::EntityNotFound,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngtService/ServiceDefContainer/get_service_def:1.0
     * operation
     */
    CdmwPlatformMngtService::ServiceProviderID* get_service_def(
        const char* service_name )
            throw( CdmwPlatformMngtService::ServiceNotFound,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngtService/RWServiceDefContainer/add_service_def:1.0
     * operation
     */
    void add_service_def(
        const CdmwPlatformMngtService::ServiceDef& service_def )
            throw( CdmwPlatformMngtService::ServiceAlreadyExists,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngtService/RWServiceDefContainer/remove_service_def:1.0
     * operation
     */
    void remove_service_def(
        const char* service_name )
            throw( CdmwPlatformMngtService::ServiceNotFound,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyContainer/get_number_of_properties:1.0
     * operation
     */
    CORBA::ULong get_number_of_properties()
              throw(CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyContainer/get_all_property_names:1.0
     * operation
     */
    void get_all_property_names(
        CORBA::ULong how_many,
        CosPropertyService::PropertyNames_out property_names,
        CosPropertyService::PropertyNamesIterator_out rest )
              throw(CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyContainer/get_property_value:1.0
     * operation
     */
    CORBA::Any* get_property_value(
        const char* property_name )
            throw( CosPropertyService::PropertyNotFound,
                   CosPropertyService::InvalidPropertyName,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyContainer/get_properties:1.0
     * operation
     */
    CORBA::Boolean get_properties(
        const CosPropertyService::PropertyNames& property_names,
        CosPropertyService::Properties_out nproperties )
              throw(CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyContainer/get_all_properties:1.0
     * operation
     */
    void get_all_properties(
        CORBA::ULong how_many,
        CosPropertyService::Properties_out nproperties,
        CosPropertyService::PropertiesIterator_out rest )
              throw(CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/RWPropertyContainer/set_property:1.0
     * operation
     */
    void set_property(
        const char* property_name,
        const CORBA::Any& property_value )
              throw(CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/RWPropertyContainer/remove_property:1.0
     * operation
     */
    void remove_property(
        const char* property_name )
            throw( CosPropertyService::PropertyNotFound,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PullMonitorable/is_alive:1.0
     * operation
     */
  void is_alive()
              throw(CORBA::SystemException );


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Observable/register_observer:1.0
     * operation
     */
    CdmwPlatformMngt::SupervisionObserver_ptr register_observer(
        const char* observer_name,
        CdmwPlatformMngt::SupervisionObserver_ptr observer )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Observable/unregister_observer:1.0
     * operation
     */
    CdmwPlatformMngt::SupervisionObserver_ptr unregister_observer(
        const char* observer_name )
            throw( CdmwPlatformMngt::ObserverNotFound,
                   CORBA::SystemException );
                   
                   
                   
    /**
     *Purpose:
     *<p> return the application name
     *
     */
    const char* get_applicationName();
    

};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_APPLICATION_IMPL_HPP

