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


#ifndef INCL_PLATFORMMNGT_APPLICATION_IMPL_HPP
#define INCL_PLATFORMMNGT_APPLICATION_IMPL_HPP

#include "SystemMngt/platformapplicationlibrary/ApplicationStates.hpp"

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/osthreads/Condition.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/ReaderWriterLock.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtApplicationBase.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtApplication.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtApplicationAgent.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntity.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtHost.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtService.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtDaemon.stub.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp"

#ifdef CDMW_USE_FAULTTOLERANCE
#include "SystemMngt/platformapplicationlibrary/CdmwPlatformMngtFTConverter.stub.hpp"
#endif

#include "SystemMngt/platformlibrary/StateMachineTemplate.hpp"
#include "SystemMngt/platformlibrary/DeactivableServant_impl.hpp"
#include "SystemMngt/platformlibrary/EventHandler.hpp"
#include "SystemMngt/platformlibrary/ServiceDefContainer_impl.hpp"
#include "SystemMngt/platformelementlibrary/ManagedElement_impl.hpp"
#include "SystemMngt/platformelementlibrary/EntityContainer.hpp"
#include "SystemMngt/platformelementlibrary/DataStoreDefinition.hpp"

#include <string>
#include <set>
#include <map>

namespace Cdmw
{
namespace PlatformMngt
{

class EventNotifierStoppedException;
class Process_impl;
class ProcessObserver_impl;
class RWEntityContainer_impl;
class RWServiceDefContainer_impl;
class ServiceBroker_impl;
class SupervisionEventNotifier;
class TaskFactory;
class Starter;
class SynchronisableTaskFactory;
class Sequencer;
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
 * Purpose:
 * Thrown when the process name has been found more than one time
 * (Process name is not an unique key id).
 */
class ProcessNameNotUnique : public Exception
{
public:
    ProcessNameNotUnique()
        : Exception (Exception::SAFE, "Process name is not unique")
    {
    };
};


/**
 *Purpose:
 *<p>
 */
class Application_impl : public virtual POA_CdmwPlatformMngt::Application,
                         public DeactivableServant_impl,
                         public ManagedElement_impl
{
	friend class ApplicationControl;
    friend class ApplicationInitialising;
    friend class ApplicationRunRequest;
    friend class ApplicationStopping;

public:

    /**
     * The type of process invalidation.
     */
    enum InvalidationType
    {
        HOST_UNAVAILABLE
    };
    
     
private:
    
    /**
     * The POA.
     */
    PortableServer::POA_var m_poa;
    
    
    /**
     * Indicates whether the application has been defined.
     */
    bool m_applicationDefined;
    
    /**
     * The read/write lock to protect access to the internal data.
     */
    OsSupport::ReaderWriterLock m_rwApplicationLock;

    /**
     * The mutex used with the initialisation condition and the stop condition.
     */
    OsSupport::Mutex m_mutex;    

    /**
     * The initialisation variable condition for long operation.
     */
    OsSupport::Condition m_initialisation_condition;

    /**
     * The stop variable condition for long operation.
     */
    OsSupport::Condition m_stop_condition;
    
    /**
     * The initialisation condition flag for long operation.
     */
    bool m_init_condition_flag;
    
    /**
     * The stop condition flag for long operation.
     */
    bool m_stop_condition_flag;

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
     * The Service Broker object var to take pointer ownership
     */
    PortableServer::ServantBase_var m_serviceBrokerServant;
    
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
     * The actual entity container.
     */
    PlatformMngt::EntityContainer* m_actualEntityContainer;

    /**
     * The actual service def container.
     */
    ServiceDefContainer_impl::ServiceDefContainer* m_actualServiceDefContainer;

    /**
     * The read/write service container.
     */
    RWServiceDefContainer_impl* m_rwServiceDefContainer;
    
    

    /**
     * The process observer.
     */
    ProcessObserver_impl* m_processObserver;
    
    /**
     * The system process observer.
     */
    CdmwPlatformMngt::ProcessObserver_var m_syst_processObserver;
        
    /**
     * The services' broker.
     */
    ServiceBroker_impl* m_serviceBroker;
    
    /**
     * The system services' broker reference.
     */
    CdmwPlatformMngtService::ServiceBroker_var m_syst_serviceBroker;
    
    /**
     * The service def container servant.
     */
    ServiceDefContainer_impl* m_serviceDefContainer;
        
    /**
     * The read/write entity container.
     */
    RWEntityContainer_impl* m_rwEntityContainer;
  
    /**
     * The entityDef to restore entity default values
     */
    CdmwPlatformMngt::EntityDefs* m_default_entitydefs;
  
    
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
     * The reference to the system host container.
     */
    CdmwPlatformMngt::HostContainer_var m_hostContainer;
    
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
     * The application acknowledgement data.
     */
    CdmwPlatformMngt::ApplicationStartingData_var m_applicationStartData;
    
    /**
     * The list of process managed by this application.
     */
    typedef std::map <std::string, Process_impl*> ProcessServants;
    
    ProcessServants m_process_servants;
    
    /**
     * The associated state machine.
     */
    ApplicationStateMachine* m_stateMachine;
    
    /**
     * The supervision event notifier.
     */
    SupervisionEventNotifier* m_eventNotifier;
    
    /**
     * The handler of events.
     */
    EventHandler m_eventHandler;
    
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
     * The system name.
     */
    CORBA::String_var m_system_name;
    
    /**
     * Application datastore
     */
    ApplicationDataStore* m_application_ds;
    

    
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
    
    

public:
      

    /**
     * Indicates whether the init processes graph has changed or not.
     */
    bool m_newInitGraph;
    
    /**
     * The startup kind.
     */
    CdmwPlatformMngtBase::StartupKind_var m_startup_kind;
    
    /**
     * Indicates whether to stop the application normaly or with emergency.
     */
    CORBA::Boolean m_emergency;
    
    
    /**
     * The information about the status this application.
     */
    CORBA::String_var m_statusInfo;
    
    /**
     * The functioning mode.
     */
    CdmwPlatformMngt::FunctioningMode m_mode;
        
    /**
     * The information about the mode of this system.
     */
    CORBA::String_var m_modeInfo;
    
    
    /**
     * The dependency graph of processes used for the initialisation of the application.
     */
    CdmwPlatformMngt::GraphElements_var m_processInitGraph;

    /**
     * The dependency graph of processes used for the stop of the application.
     */
    CdmwPlatformMngt::GraphElements_var m_processStopGraph;


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
        
        
    /**
     *Purpose:
     *<p> Set the application record in data store
     *
     */
    void set_record_in_datastore();
            
    /**
     *Purpose:
     *<p> Remove the application record in datastore
     *
     */
    void remove_record_in_datastore();
    
    /**
     *Purpose:
     *<p> Get the application record from data store
     *
     */
    CdmwPlatformMngt::ApplicationRecord* get_record_from_datastore();
    

public:
    /**
     *Purpose:
     *<p> Default constructor.
     *
     *@param poa                the poa in charge of servants.
     *@param eventNotifier      the supervision observer to be notified of events.
     *@param element_repository the element repository 
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
        CdmwPlatformMngt::RWElementRepository_ptr element_repository,
        const std::string& replicationManagerIOGR,
        CdmwPlatformMngt::FTConverter_ptr ftConverter)
            throw( BadParameterException,
                   AlreadyDoneException,
                   UnknownStateException,
                   OutOfMemoryException );
#else
    Application_impl(
        PortableServer::POA_ptr poa,
        SupervisionEventNotifier* eventNotifier,
        CdmwPlatformMngt::RWElementRepository_ptr element_repository)
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
    
    /**
     *Purpose:
     *<p> Synchronise with datastore
     *
     */
    void synchro_with_datastore();
    
    /**
     *Purpose:
     *<p> Synchronise the processes with their datastore
     *
     */
    void synchro_processes_with_datastore();
    
    /**
     *Purpose:
     *<p> Synchronise the processing
     *
     */
    void synchro_processing();
    
    /**
     *Purpose:
     *<p> Synchronise the processing of processes
     *
     */
    void synchro_processing_of_processes();


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
     *@param applicationStartData the data for application servant to start.
     *@param application_path     the path of the Application.
     *
     *@exception OutOfMemoryException Lack of memory.
     */
    void end_creation(
        const CdmwPlatformMngt::ApplicationStartingData& applicationStartData,
        const char* application_path)
            throw (OutOfMemoryException,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p> Gets the application agent representing this application on the
     * specified host. If not existing, It creates one onto the host.
     *
     *@param hostName the host running a part of this application.
     *
     *@return the application agent
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
            throw (CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::HostNotReachable,
                   CdmwPlatformMngt::CreationError,
                   CdmwPlatformMngt::ApplicationAgentAlreadyExists,
                   CdmwPlatformMngt::InvalidInfo,
                   CdmwPlatformMngt::CreationTimeout,
                   AgentNotFoundException,
                   CORBA::SystemException);
                   
    /**
     *Purpose:
     *<p> Gets the application agent representing this application on the
     * specified host from the daemon.
     *
     *@param hostName the host running a part of this application.
     *
     *@return the application agent or nil if agent does not exist
     *
     *@exception HostNotFound if the host cannot be found.
     *@exception HostNotReachable if the host doesn't respond.
     *@exception CORBA::SystemException
     */               
    CdmwPlatformMngt::ApplicationAgent_ptr searchAgentFromDaemon (
        const char* hostName)
            throw (CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::HostNotReachable,
                   CORBA::SystemException);
                   
                   
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
     *<p> Notifies a mode change events to the supervision event notifier.
     *
     * @param functioning_mode the functionning mode
     * @param mode_info the associated info
     */
    void notifyModeChangeEvent(CdmwPlatformMngt::FunctioningMode functioning_mode,
                               const char* mode_info);

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
        const char* entity_status,
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
     *<p> Check the number of processes having the same name.
     *
     * @param process_name the process name to check.
     *
     */               
    int checkNbrOfProcessNames (const char* process_name);
    
    /**
     *Purpose:
     *<p> Get the host name of the process specified by its name.
     *
     * @param process_name the process name.
     *
     * @param host_name the host name.
     *
     * @return the process servant
     *
     * @exception ProcessNameNotUnique thrown if name is not unique
     * @exception CdmwPlatformMngt::ProcessNotFound thrown if process not found
     */                  
    Process_impl* getProcessHostName (
                                 const std::string& process_name,
                                 std::string& host_name)
        throw (ProcessNameNotUnique,
               CdmwPlatformMngt::ProcessNotFound);

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
                  CdmwPlatformMngt::AlreadyDone,
                  CdmwPlatformMngt::IncompatibleStatus,
           	  CosPropertyService::MultipleExceptions,
		  CORBA::SystemException );

    /**
     *Purpose:
     *<p> Actually adds the specified process.
     *
     *@param process_def the definition of the process.
     *
     *@return the CORBA reference of the process.
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
     *@exception IncompatibleStatus if the status of the application doesn't
     * allow the operation.
     */
    CdmwPlatformMngt::Process_ptr addProcess(
        CdmwPlatformMngt::ProcessDef* process_def )
            throw( CdmwPlatformMngt::ProcessAlreadyExists,
                   CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateStep,
		   CosPropertyService::MultipleExceptions,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p> Actually removes the specified process.
     *
     *@param process_key the key name identifying the process.
     *
     *@exception ProcessNotFound if process_name doesn't denote
     * an existing process.
     *@exception ProcessStillActive if the process is still active
     */
    void removeProcess(const char* process_key)
            throw (CdmwPlatformMngt::ProcessNotFound,
                   CdmwPlatformMngt::ProcessStillActive,
                   CORBA::SystemException);
                   
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
     *<p> facade to get an attribute
     *
     */

     void get_attribute(const char * attr_name, CORBA::Any_out ret_attr , CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);

    /**
     *Purpose:
     *<p> facade to set an attribute
     *
     */
    void set_attribute(const char * attr_name, const CORBA::Any & attr , CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);



    /**
     *Purpose:
     *<p> Get the process_impl associated to the identified process.
     *
     *@param process_key the key name identifying the process.
     *
     *@return the process_impl.
     *
     *@exception ProcessNotFound if process_name doesn't denote
     * an existing process.
     *@exception ElementPathInvalid if key is invalid
     */
    Process_impl* getProcess(const char* process_key)
            throw (CdmwPlatformMngt::ProcessNotFound,
                   CdmwPlatformMngt::ElementPathInvalid,
                   CORBA::SystemException);
    
    /**
     *Purpose:
     *<p> Get the process_impl associated to the identified process.
     *
     *@param process_name the name of the process.
     *@param host_name    the name of the host.
     *
     *@return the process_impl.
     *
     *@exception ProcessNotFound if process_name doesn't denote
     * an existing process.
     *@exception ElementPathInvalid if key is invalid
     */               
    Process_impl* getProcess(const char* process_name,
                             const char* host_name)
            throw (CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException);
    
    
    /**
     *Purpose:
     *<p> Get the host container (associated to system).
     *
     *@return the host container.
     *
     */
    CdmwPlatformMngt::HostContainer_ptr get_host_container();
    
    
    /**
     *Purpose:
     *<p> Get the application observer (associated to system).
     *
     *@return the application observer.
     *
     */               
    CdmwPlatformMngt::ApplicationObserver_ptr get_application_observer();
    
    
    /**
     *Purpose:
     *<p> Get the rw service def container for application.
     *
     *@return the rw service def container.
     *
     */     
    RWServiceDefContainer_impl* get_rwservice_defcontainer();
                
    /**
     *Purpose:
     *<p> Get the supervision event notifier to send notification (reroute to System).
     *
     *@return the supervision event notifier.
     *
     */      
    SupervisionEventNotifier* get_event_notifier();  
    
    /**
     *Purpose:
     *<p> Get the event handler to manage internal event.
     *
     *@return the event handler.
     *
     */
    EventHandler*  get_event_handler();
    
    
    /**
     *Purpose:
     *<p> Get the process observer.
     *
     *@return the process observer.
     *
     */                                                  
    CdmwPlatformMngt::ProcessObserver_ptr get_process_observer()
            throw(CORBA::SystemException);
    
    /**
     *Purpose:
     *<p> Get the process observer servant.
     *
     *@return the process observer servant.
     *
     */        
    ProcessObserver_impl* get_process_observer_servant();
            
    /**
     *Purpose:
     *<p> Get the service broker.
     *
     *@return the service broker.
     *
     */                                                   
    CdmwPlatformMngtService::ServiceBroker_ptr get_service_broker()
            throw(CORBA::SystemException);
            
            
            
    
    

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
     *<p> Get the internal status from state machine.
     */
    CdmwPlatformMngt::ApplicationStatus get_internal_status();





    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/restart_type:1.0
     * operation
     */         
    CdmwPlatformMngt::RestartType restart_type ()
              throw(CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/restart_attempt:1.0
     * operation
     */    
    CORBA::Long restart_attempt ()
              throw(CORBA::SystemException);
        
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/is_manual_starting:1.0
     * operation
     */    
    CORBA::Boolean is_manual_starting()
              throw(CORBA::SystemException);
     
     
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/set_manual_starting:1.0
     * operation
     */   
    void set_manual_starting(CORBA::Boolean manual_starting)
              throw(CORBA::SystemException,
                    CdmwPlatformMngt::IncompatibleStatus);
                    
                    
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
     * Set the functionning mode with info
     * @param functioning_mode the functionning mode
     * @param mode_info the info about mode
     */  
    void set_mode(CdmwPlatformMngt::FunctioningMode functioning_mode,
                  const char* mode_info);

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
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/get_number_of_processes:1.0
     * operation
     */
    CORBA::ULong get_number_of_processes() throw(CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/get_processes:1.0
     * operation
     */
    CdmwPlatformMngt::Processes* get_processes(
        CORBA::ULong how_many,
        CdmwPlatformMngt::ProcessesIterator_out processes_iterator )
              throw(CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/get_process:1.0
     * operation
     */
    CdmwPlatformMngt::Process_ptr get_process(const char* process_name,
                                              const char* host_name)
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
                   CdmwPlatformMngt::AlreadyDone,
                   CdmwPlatformMngt::IncompatibleStatus,
		   CosPropertyService::MultipleExceptions,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/add_process:1.0
     * operation
     */
    CdmwPlatformMngt::Process_ptr add_process(
        CdmwPlatformMngt::ProcessDef* process_def )
            throw( CdmwPlatformMngt::ProcessAlreadyExists,
                   CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::IncompatibleStatus,
		   CosPropertyService::MultipleExceptions,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/remove_process:1.0
     * operation
     */
    void remove_process(const char* process_name, const char* host_name)
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
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/get_autorestart_info:1.0
     * operation
     */              
    CdmwPlatformMngt::ApplicationAutoRestartInfo get_autorestart_info()
            throw(CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/set_autorestart_info:1.0
     * operation
     */
    void set_autorestart_info(
         const CdmwPlatformMngt::ApplicationAutoRestartInfo& autorestart_info)
            throw(CdmwPlatformMngt::IncompatibleStatus,
                  CORBA::SystemException);
                  
                  
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
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/cold_restart:1.0
     * operation
     */
    void cold_restart(const CdmwPlatformMngtBase::StartupKind& startup_kind)
            throw(CdmwPlatformMngt::IncompatibleStatus,
                  CORBA::SystemException);        
            

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/hot_restart:1.0
     * operation
     */
    void hot_restart(const CdmwPlatformMngtBase::StartupKind& startup_kind)
            throw(CdmwPlatformMngt::IncompatibleStatus,
                  CORBA::SystemException); 

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/EntityContainer/get_number_of_entities:1.0
     * operation
     */
    CORBA::ULong get_number_of_entities()
        throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/EntityContainer/get_all_entity_names:1.0
     * operation
     */
    void get_all_entity_names(
        CORBA::ULong how_many,
        CdmwPlatformMngt::ElementNames_out entity_names,
        CdmwPlatformMngt::ElementNamesIterator_out rest )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/EntityContainer/get_entity:1.0
     * operation
     */
    CdmwPlatformMngt::Entity_ptr get_entity (const char* entity_name)
            throw( CdmwPlatformMngt::EntityNotFound,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/EntityContainer/get_entities:1.0
     * operation
     */
    CORBA::Boolean get_entities(
        const CdmwPlatformMngt::ElementNames& entity_names,
        CdmwPlatformMngt::Entities_out nentities )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/EntityContainer/get_all_entities:1.0
     * operation
     */
    void get_all_entities(
        CORBA::ULong how_many,
        CdmwPlatformMngt::Entities_out nentities,
        CdmwPlatformMngt::EntitiesIterator_out rest )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/RWEntityContainer/add_entity:1.0
     * operation
     */
    CdmwPlatformMngt::Entity_ptr add_entity (
                const CdmwPlatformMngt::EntityDef & entity_def)
                throw(CdmwPlatformMngt::EntityAlreadyExists,
		              CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/RWEntityContainer/remove_entity:1.0
     * operation
     */
    void remove_entity(
        const char* entity_name )
            throw( CdmwPlatformMngt::EntityNotFound,
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/RWServiceDefContainer/remove_all_service_defs:1.0
     * operation
     */               
    void remove_all_service_defs()
		    throw (CORBA::SystemException);

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
        CdmwPlatformMngt::SupervisionObserver_ptr observer)
            throw (CORBA::SystemException);
            
            
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/register_observer:1.0
     * operation
     */
    CdmwPlatformMngt::SupervisionObserver_ptr register_proxy_observer(
        const char* observer_name,
        const char* host_name,
        CdmwPlatformMngt::SupervisionObserver_ptr observer)
            throw (CORBA::SystemException);
      

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Observable/unregister_observer:1.0
     * operation
     */
    CdmwPlatformMngt::SupervisionObserver_ptr unregister_observer(
        const char* observer_name)
            throw (CdmwPlatformMngt::ObserverNotFound,
                   CORBA::SystemException);
                   
                   
    /**
     *Purpose:
     *<p> return the system name
     *   !! do not desallocate !!
     *
     */
    const char* get_system_name();
    

};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_APPLICATION_IMPL_HPP

