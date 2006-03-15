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


#ifndef INCL_PLATFORMMNGT_SYSTEM_IMPL_HPP
#define INCL_PLATFORMMNGT_SYSTEM_IMPL_HPP


#include "platformsystemlibrary/SystemStates.hpp"

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/osthreads/Condition.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/ReaderWriterLock.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtSystem.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtApplicationBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtApplication.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtService.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntity.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtHost.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessObserver.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtServiceBroker.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp"

#include "SystemMngt/platformlibrary/StateMachineTemplate.hpp"
#include "SystemMngt/platformlibrary/EventHandler.hpp"
#include "SystemMngt/platformlibrary/ServiceDefContainer_impl.hpp"

#include "SystemMngt/platformelementlibrary/EntityContainer_impl.hpp"
#include "SystemMngt/platformelementlibrary/ManagedElement_impl.hpp"
#include "SystemMngt/platformelementlibrary/DataStoreDefinition.hpp"



#ifdef CDMW_USE_FAULTTOLERANCE


#include "SystemMngt/platformapplicationlibrary/CdmwPlatformMngtFTConverter.stub.hpp"
#include "FaultTolerance/idllib/FT.stub.hpp"
#include "FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp"
#include "FaultTolerance/idllib/CdmwFTFaultDetector.stub.hpp"
#include "FaultTolerance/idllib/CdmwFTManager.stub.hpp"

#endif

#include <string>

/**
* Root namespace for CDMW runtime.
*/
namespace Cdmw
{
/**
* Namespace for CDMW PlatformMngt
*/
namespace PlatformMngt
{

class RWServiceDefContainer_impl;
class ServiceBinding_impl;
class RWEntityContainer_impl;
class HostContainer_impl;
class HostContainer;
class Host_impl;

class SupervisionEventNotifier;
class RunSetManager;

class SynchronisableTaskFactory;
class Sequencer;

class SystemStatusChangeFactory;
class SystemModeChangeFactory;
class SystemEntityStatusChangeFactory;
class HostStatusChangeFactory;

class ApplicationObserver_impl;


/**
 *Purpose:
 *<p>
 */
class ApplicationRef                   
{
    
public:
   
    /**
     * The application object reference.
     */
    CdmwPlatformMngt::Application_var m_application;
    
    /**
     * The application control object reference.
     */
    CdmwPlatformMngt::ApplicationControl_var m_applicationControl;
    
    /**
     * The process observer object reference.
     */
    CdmwPlatformMngt::ProcessObserver_var m_processObserver;
     
    
    /**
     *Purpose:
     *<p> Default constructor.
     *
     *@param application         reference of application object.
     *@param applicationControl  reference of application control object.
     *@param processObserver     reference of application process observer object.
     *
     */
    ApplicationRef (CdmwPlatformMngt::Application* application,
                    CdmwPlatformMngt::ApplicationControl* applicationControl,
                    CdmwPlatformMngt::ProcessObserver* processObserver)
    {
        m_application = CdmwPlatformMngt::Application::_duplicate(application);
        m_applicationControl = 
              CdmwPlatformMngt::ApplicationControl::_duplicate(applicationControl);
        m_processObserver = 
              CdmwPlatformMngt::ProcessObserver::_duplicate(processObserver);
    };       


    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual ~ApplicationRef()
    {
    };
    

    /**
     *Purpose:
     *<p> Copy constructor.
     */
    ApplicationRef (const ApplicationRef& rhs)
    {
        m_application = rhs.m_application;
        m_applicationControl = rhs.m_applicationControl;
        m_processObserver = rhs.m_processObserver;
    };

    /**
     * Purpose:
     * <p> Assignment operator.
     */ 
    ApplicationRef& operator = (const ApplicationRef& rhs)

    {
        m_application = rhs.m_application;
        m_applicationControl = rhs.m_applicationControl;
        m_processObserver = rhs.m_processObserver;
        
        return *this;
    };

};

class System_impl;

/**
 *Purpose: Dumper of supervision observer from datastore
 *<p>
 */
class ObserverDumper : 
   public std::unary_function <std::pair <
              OrbSupport::type_traits <std::string>::_var_type, 
              OrbSupport::type_traits <CdmwPlatformMngt::SystemObserverRegistrationRecord>::_var_type> , void>
{
    
public:

    typedef std::pair <
              OrbSupport::type_traits <std::string>::_var_type, 
              OrbSupport::type_traits <CdmwPlatformMngt::SystemObserverRegistrationRecord>::_var_type> 
                     PairType;
              
    /**
     *Purpose:
     *<p> constructor.
     *
     *@param system         reference of system.
     */
    ObserverDumper(System_impl* system) : m_system(system)
    { 
    }

    /**
     *Purpose:
     *<p> operator () called by datastore interface.
     *
     *@param pair of <oid, record>
     */
    void operator()(const PairType &elt);

private:

    System_impl* m_system;
};

/**
 *Purpose:
 *<p>
 */
class System_impl : virtual public POA_CdmwPlatformMngt::System,
                    public ManagedElement_impl
{
    friend class SystemInitial;
    friend class SystemStarting;
    friend class SystemStarted;
    friend class SystemStopping;
    friend class SystemStopped;
    friend class SystemStartupSuspended;


public:

    /**
     * The system port.
     */
    CdmwPlatformMngt::Port m_systemPort;

    /**
     * The startup kind.
     */
    CdmwPlatformMngtBase::StartupKind_var m_startup_kind;

    /**
     * Indicates whether to stop the application normaly or with emergency.
     */
    CORBA::Boolean m_emergency;

    /**
     * The dependency init graph of applications.
     */
    CdmwPlatformMngt::GraphElements_var m_applicationInitGraph;

    /**
     * The dependency stop graph of applications.
     */
    CdmwPlatformMngt::GraphElements_var m_applicationStopGraph;
    
    /**
     * The normal/emergency stop graph of applications.
     */
    CdmwPlatformMngt::GraphElements_var m_stopGraph;
    
    
    /**
     * Indicates whether the applications graph has changed or not.
     */
    bool m_newGraph;

    /**
     * The information about the status of this system.
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
        
       
    
protected:

    /**
     * Define the applications map
     */
    typedef std::map <std::string, ApplicationRef> Applications;
    
    /**
     * The list of applications managed by this system.
     */
    Applications m_applications; 
    
    
    
    /**
    * ORB reference
    */ 
    CORBA::ORB_var m_orb;
    
    /**
     * The specific POA in charge of servants.
     */
    PortableServer::POA_var m_poa;  
    
    
    /**
     * The supervision event notifier (notify to external observers).
     */
    SupervisionEventNotifier* m_eventNotifier;
    
    
    
#ifdef CDMW_USE_FAULTTOLERANCE

    /**
     * The replication manager
     */
    CdmwFT::ReplicationManager_var m_replicationMngt;
    
    /**
     * the replication manager IOGR
     */
    std::string m_replicationManagerIOGR;

    /**
     * The fault tolerance converter object (ior).
     */
    CdmwPlatformMngt::FTConverter_var m_FTConverterObject;
    
    /**
     * The fault tolerance process observer object (iogr).
     */
    CdmwPlatformMngt::ProcessObserver_var m_FTProcessObserverGroup;
    
    /**
     * The fault tolerance service broker object (iogr).
     */
    CdmwPlatformMngtService::ServiceBroker_var m_FTServiceBrokerGroup;
    
    
#endif    

    /**
     * The system is defined
     */
    bool m_systemDefined;
   
    /**
     * The service defs container object.
     */
    CdmwPlatformMngtService::ServiceDefContainer_var m_serviceDefContainerObject;
    
    /**
     * The service binding object
     */
    CdmwPlatformMngtService::ServiceBinding_var m_serviceBindingObject;
    

    /**
     * The host container object (read only).
     */
    CdmwPlatformMngt::HostContainer_var m_hostContainerObject;
    
    /**
     * The application observer object (receive events from applications).
     */
    CdmwPlatformMngt::ApplicationObserver_var m_applicationObserverObject;
    
    
    /**
     * Read/Writer lock guard tp protect system data
     */
    OsSupport::ReaderWriterLock m_rwSystemLock;
    
    
    /**
     * System datastore
     */
    SystemDataStore* m_system_ds;
    
    /**
     * System Observer datastore
     */
    SystemObserverRegistrationDataStore* m_system_observer_ds;
    
    
private:

    /**
     * The system name.
     */
    CORBA::String_var m_sys_name;
    
    
    /**
     * The mutex used for changing the functioning mode.
     */
    OsSupport::Mutex m_mutexMode;
        
    /**
     * The init sync task factory.
     */
    SynchronisableTaskFactory* m_initSyncTaskFactory;
    
    /**
     * The task sequencer for initialising applications accordingly to a dependency
     * graph.
     */
    Sequencer* m_initSequencer;
        
    /**
     * The stop sync task factory.
     */
    SynchronisableTaskFactory* m_stopSyncTaskFactory;
    
    /**
     * The task sequencer for stopping applications accordingly to a dependency
     * graph.
     */
    Sequencer* m_stopSequencer;


    /**
     * The actual service defs container.
     */  
    ServiceDefContainer_impl::ServiceDefContainer* m_actualServiceDefContainer;
    
    /**
     * The rw service defs container servant.
     */
    RWServiceDefContainer_impl* m_rwServiceDefContainer;
    
    /**
     * The service def container servant.
     */
    ServiceDefContainer_impl* m_serviceDefContainer;
    
    /**
     * The service binding servant.
     */
    ServiceBinding_impl* m_serviceBinding;        
     
      
    /**
     * The actual system entity container.
     */  
    PlatformMngt::EntityContainer* m_actualEntityContainer;
    
    /**
     * The rw entity container servant.
     */
    RWEntityContainer_impl* m_rwEntityContainer;

    /**
     * The entityDef to restore entity default values
     */

  CdmwPlatformMngt::EntityDefs* m_default_entitydefs;

  //    CdmwPlatformMngt::EntityDefs_var m_default_entitydefs;

         
    /**
     * The host container object.
     */
    PlatformMngt::HostContainer* m_actualHostContainer;
    
    /**
     * The host container servant.
     */    
    HostContainer_impl* m_hostContainer;        

    /**
     * The application observer servant.
     */ 
    ApplicationObserver_impl*  m_applicationObserver;
    
    
    /**
     * The runset manager
     */
    RunSetManager* m_runSetManager;

    /**
     * The handler of events.
     */
    EventHandler m_eventHandler;
    
    
    /**
     * The system status change factory.
     */
    SystemStatusChangeFactory* m_sysStatusChangeFactory;
    
    /**
     * The system mode change factory.
     */
    SystemModeChangeFactory* m_sysModeChangeFactory;
    
    /**
     * The system entity status change factory.
     */
    SystemEntityStatusChangeFactory* m_sysEntityStatusChangeFactory;
    
    /**
     * The host status change factory.
     */
    HostStatusChangeFactory* m_hostStatusChangeFactory;
    
    /**
     * The associated state machine.
     */
    SystemStateMachine* m_stateMachine;
    
    /**
     * The states of system.
     */
    static SystemInitial*          M_initial;
    static SystemStarting*         M_starting;
    static SystemStartupSuspended* M_startupSuspended;
    static SystemStarted*          M_started;
    static SystemStopping*         M_stopping;
    static SystemStopped*          M_stopped;

    static const std::string  M_initialStateName;
    static const std::string  M_startingStateName;
    static const std::string  M_startupSuspendStateName;
    static const std::string  M_startedStateName;
    static const std::string  M_stoppingStateName;
    static const std::string  M_stoppedStateName;


public:
   

    /**
     *Purpose:
     *<p> Default constructor.
     *     
     *@param eventNotifier the supervision observer to be notified of events. 
     *@param orb           the orb reference
     *@param poa           the poa in charge of servants   
     *@param element_repository the element repository  
     *
     *@exception BadParameterException if the parameter is invalid.
     *@exception AlreadyDoneException if the state machine is already running.
     *@exception UnknownStateException if stateName doesn't refer to a known
     * state.
     *@exception AlreadyExistException if the system container already exist.
     *@exception OutOfRessourcesException Lack of ressource.
     *@exception OutOfMemoryException Lack of memory.
     */

    System_impl (SupervisionEventNotifier* eventNotifier,
                 CORBA::ORB_ptr orb,
                 PortableServer::POA_ptr poa,
                 CdmwPlatformMngt::RWElementRepository_ptr element_repository)    
        throw (BadParameterException,
               AlreadyDoneException,
               UnknownStateException,
               AlreadyExistsException,
               OutOfResourcesException,
               OutOfMemoryException);

    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual ~System_impl();

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
     *<p> Returns the system_mission
     *
     */

     char * get_system_mission()  throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p> to set the system_mission attribute
     *
     */

     void set_system_mission( const char * system_mission )  throw (CORBA::SystemException);

     /**     
     *Purpose:
     *<p> to get system_name
     *
     */

     char * System_impl::get_system_name(  )  throw (CORBA::SystemException) ;


     /**
     *Purpose:
     *<p> to set system_name
     *
     */
     void System_impl::set_system_name( const char * system_name )  throw (CORBA::SystemException);

     /**
     *Purpose:
     *<p> to get master_host
     *
     */

     char * System_impl::get_master_host(  )  throw (CORBA::SystemException);



    /**
     *Purpose:
     *<p> Synchronise with datastore
     *
     */
    void synchro_with_datastore();

    
    /**
     *Purpose:
     *<p> Synchronise the processing
     *
     */
    void synchro_processing();
     
     


    /**
     *Purpose:
     *<p> Stop and destroy the init sequencer.
     */
    void destroyInitSequencer();

    /**
     *Purpose:
     *<p> Stop and destroy the stop sequencer.
     */
    void destroyStopSequencer();
    
    /**
     *Purpose:
     *<p> Stop and destroy the runset manager.
     */
    void destroyRunSetManager();

    /**
     *Purpose:
     *<p> Creates all the state instances (only once).
     *
     *@exception OutOfMemoryException Lack of memory.
     */
    static void createStates()
        throw (OutOfMemoryException);

    /**
     *Purpose:
     *<p> Destroys all the System_impl's states.
     */
    static void destroyStates();

    /**
     *Purpose:
     *<p> Notifies a status change events to the supervision event notifier.
     */
    void notifyStatusChangeEvent();

    /**
     *Purpose:
     *<p> Notifies the supervision observer that an entity of this
     * system has its status changed.
     *
     *@param entity_name the name of the entity.
     *@param entity_status the status of the entity.
     *@param entity_info some additional information.
     */
    void notifyEntityStatusChangeEvent (
                      const char* entity_name,
                      const char* entity_status,
                      const char* entity_info);
                      
                      
    /**
     *Purpose:
     *<p> Notifies the supervision observer that an host of this
     * system has its status changed.
     *
     *@param host_name the name of the host.
     *@param host_status the status of the host.
     *
     */
    void notifyHostStatusChangeEvent (
                      const char* host_name,
                      CdmwPlatformMngt::HostStatus host_status,
                      CdmwPlatformMngtBase::TimeStamp time_stamp);


    /**
     *Purpose:
     *<p> Defines the initial configuration of the system.
     *
     *@param system_def the definition of the system.
     *    
     *@exception DuplicateEntity if there is a duplicate entity name among
     *           the system's entities
     *@exception CosPropertyService::MultipleExceptions if there is a bad property
     *@exception DuplicateService if there is a duplicate service name among
     *           the system's service definitions
     *@exception DuplicateHost if there is a duplicate host name among
     *           the system's host definitions
     *@exception DuplicateHostEntity if there is a duplicate entity name among
     *           the host's entities
     *@exception HostPropertyExceptions if there is a bad host property
     *@exception AlreadyDone if system has been already defined
     */
    void defineSystem (const CdmwPlatformMngt::SystemDef& system_def)
        throw (CdmwPlatformMngt::DuplicateEntity,
               CosPropertyService::MultipleExceptions,
               CdmwPlatformMngt::DuplicateService,
               CdmwPlatformMngt::DuplicateHost,
               CdmwPlatformMngt::DuplicateHostEntity,
               CdmwPlatformMngt::HostPropertyExceptions,
               CdmwPlatformMngt::AlreadyDone,
               CORBA::SystemException);
               
    /**
     *Purpose:
     *<p> Actually sets the specified init graph.
     *
     *@param application_graph the init graph of application.
     *
     *@exception OutOfMemoryException Lack of memory.
     *@exception ReferenceNotFound if an element of the graph refers to an
     *           unknown application
     *@exception CircularReference if a circular reference has been detected
     *           in the graph
     */
    void setInitGraph (const CdmwPlatformMngt::GraphElements& application_graph)
            throw (OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CORBA::SystemException);

    
    /**
     *Purpose:
     *<p> Actually sets the specified stop graph.
     *
     *@param application_graph the stop graph of application.
     *
     *@exception OutOfMemoryException Lack of memory.
     *@exception ReferenceNotFound if an element of the graph refers to an
     *           unknown application
     *@exception CircularReference if a circular reference has been detected
     *           in the graph
     */
    void setStopGraph (const CdmwPlatformMngt::GraphElements& application_graph)
            throw (OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CORBA::SystemException);


    /**
     *Purpose:
     *<p> Actually adds a new runset or modify an existing runset.
     *
     *@param run_set_def the definition of the runset.
     *
     *@exception DuplicateRunSetElement if an element already exists in the
     *           runset or among existing runsets.
     */
    void setRunSet(const CdmwPlatformMngt::RunSetDef& run_set_def)
          throw (CdmwPlatformMngt::DuplicateRunSetElement,
                 CORBA::SystemException);

     
    /**
     *Purpose:
     *<p> Actually removes the specified runset.
     *
     *@param run_set_name the name identifying the runset to remove.
     *
     *@exception RunSetNotFound if run_set_name doesn't denote
     *           an existing runset.
     */
    void removeRunSet(const char* run_set_name)
          throw (CdmwPlatformMngt::RunSetNotFound,
                 CORBA::SystemException);


    /**
     *Purpose:
     *<p> Actually removes all runsets.
     *
     */
    void removeAllRunSets();


    /**
     *Purpose:
     *<p> Actally adds a host to the system.
     *
     *@param host_def the definition of the host to add.
     *
     *@return the reference of the added host
     *
     *@exception HostAlreadyExists if the host name is already used
     *           within the system.
     *@exception DuplicateEntity if entity is duplicated
     *@exception CosPropertyService::MultipleExceptions if multiple exception in property
     *
     */
    CdmwPlatformMngt::Host_ptr 
          addHost(const CdmwPlatformMngt::HostDef& host_def)
           throw (CdmwPlatformMngt::HostAlreadyExists,
                  CdmwPlatformMngt::DuplicateEntity,
                  CosPropertyService::MultipleExceptions,
                  CORBA::SystemException);
    
    /**
     *Purpose:
     *<p> Get a host implementation.
     *
     *@param host_name the name identifying the host.
     *
     *@exception HostNotFound if host_name doesn't denote
     *           an existing host.
     */              
    Host_impl* getHost (const char* host_name)
           throw (CdmwPlatformMngt::HostNotFound,
                  CORBA::SystemException);

    /**
     *Purpose:
     *<p> Removes a host from the system.
     *
     *@param host_name the name identifying the host to remove.
     *
     *@exception HostNotFound if host_name doesn't denote
     *           an existing host.
     */
    void removeHost(const char* host_name)
           throw (CdmwPlatformMngt::HostNotFound,
                  CORBA::SystemException);
                  

    
    

    /**
     *Purpose:
     *<p> Actually adds the specified application.
     *
     *@param platform_application_def the definition of platform application
     *@param application_def          the definition of the application.
     *
     *@return the CORBA reference of the application.
     *
     * @exception ApplicationAlreadyExists if the application name is already
     *            used within the system.
     * @exception HostNotFound if the host where the application should run
     *            is unknown.
     * @exception DuplicateEntity if there is a duplicate entity name among
     *            the application's entities
     * @exception CosPropertyService::MultipleExceptions if there is a bad property among
     *            the application's properties
     * @exception DuplicateService if there is a duplicate service name among
     *            the application's service definitions
     * @exception DuplicateProcess if there is a duplicate process name among
     *            the application's processes
     * @exception ProcessHostNotFound if an unknown host is used by one of
     *            the application's processes
     * @exception DuplicateProcessEntity if there is a duplicate entity name
     *            among the entities of one of the application processes
     * @exception ProcessPropertyExceptions if there is a bad property among
     *            the properties of one of the application processes
     * @exception DuplicateProcessService if there is a duplicate service
     *            name among the services of one of the application processes
     * @exception DuplicateProcessStep if there is a duplicate step label
     *            among the steps of one of the application processes
     * @exception DuplicateProcessCommand if there is a duplicate command name
     *            among the commands of one of the application processes        
     * @exception DuplicateProcessCommandArg if there is a duplicate command arg name
     *            among the commands of one of the application processes
     * @exception DuplicateEntityCommand if there is a duplicate command name
     *            among the commands of one of the entities of the application processes
     * @exception DuplicateEntityCommandArg if there is a duplicate command arg name
     *            among the commands of one of the entities of the application processes
     * @exception IncompatibleStatus if the status of the system doesn't
     *            allow the operation.
     */
    CdmwPlatformMngt::Application_ptr 
            addApplication(
                   const CdmwPlatformMngt::PlatformApplicationDef& platform_application_def,
                   const CdmwPlatformMngt::ApplicationDef& application_def)
            throw (CdmwPlatformMngt::ApplicationAlreadyExists,
                   CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CosPropertyService::MultipleExceptions,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateProcess,
                   CdmwPlatformMngt::ProcessHostNotFound,
                   CdmwPlatformMngt::DuplicateProcessEntity,
                   CdmwPlatformMngt::ProcessPropertyExceptions,
                   CdmwPlatformMngt::DuplicateProcessService,
                   CdmwPlatformMngt::DuplicateProcessStep,
                   CdmwPlatformMngt::DuplicateProcessCommand, 
                   CdmwPlatformMngt::DuplicateProcessCommandArg,
                   CdmwPlatformMngt::DuplicateEntityCommand, 
                   CdmwPlatformMngt::DuplicateEntityCommandArg,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);
    
    /**
     *Purpose:
     *<p> Actually removes the specified application.
     *
     *@param application_name the name identifying the application.
     *
     *@exception ApplicationNotFound if application_name doesn't denote
     *           an existing application.
     *@exception ApplicationStillActive if the application is still active
     */
    void removeApplication (const char* application_name)
            throw (CdmwPlatformMngt::ApplicationNotFound,
                   CdmwPlatformMngt::ApplicationStillActive,
                   CORBA::SystemException);
    
    
    
    /**
     *Purpose:
     *<p> Actually adds the specified application according to 
     *    centralised/distributed supervision.
     *
     *@param platform_application_def the definition of platform application
     *@param application_def          the definition of the application.
     *
     *@return the CORBA reference of the application.
     *
     * @exception ApplicationAlreadyExists if the application name is already
     *            used within the system.
     * @exception HostNotFound if the host where the application should run
     *            is unknown.
     * @exception DuplicateEntity if there is a duplicate entity name among
     *            the application's entities
     * @exception CosPropertyService::MultipleExceptions if there is a bad property among
     *            the application's properties
     * @exception DuplicateService if there is a duplicate service name among
     *            the application's service definitions
     * @exception DuplicateProcess if there is a duplicate process name among
     *            the application's processes
     * @exception ProcessHostNotFound if an unknown host is used by one of
     *            the application's processes
     * @exception DuplicateProcessEntity if there is a duplicate entity name
     *            among the entities of one of the application processes
     * @exception ProcessPropertyExceptions if there is a bad property among
     *            the properties of one of the application processes
     * @exception DuplicateProcessService if there is a duplicate service
     *            name among the services of one of the application processes
     * @exception DuplicateProcessStep if there is a duplicate step label
     *            among the steps of one of the application processes
     * @exception DuplicateProcessCommand if there is a duplicate command name
     *            among the commands of one of the application processes        
     * @exception DuplicateProcessCommandArg if there is a duplicate command arg name
     *            among the commands of one of the application processes
     * @exception DuplicateEntityCommand if there is a duplicate command name
     *            among the commands of one of the entities of the application processes
     * @exception DuplicateEntityCommandArg if there is a duplicate command arg name
     *            among the commands of one of the entities of the application processes
     * @exception IncompatibleStatus if the status of the system doesn't
     *            allow the operation.
     */
    virtual
    CdmwPlatformMngt::Application_ptr 
            doAddApplication(
                   const CdmwPlatformMngt::PlatformApplicationDef& platform_application_def,
                   const CdmwPlatformMngt::ApplicationDef& application_def)
            throw (CdmwPlatformMngt::ApplicationAlreadyExists,
                   CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CosPropertyService::MultipleExceptions,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateProcess,
                   CdmwPlatformMngt::ProcessHostNotFound,
                   CdmwPlatformMngt::DuplicateProcessEntity,
                   CdmwPlatformMngt::ProcessPropertyExceptions,
                   CdmwPlatformMngt::DuplicateProcessService,
                   CdmwPlatformMngt::DuplicateProcessStep,
                   CdmwPlatformMngt::DuplicateProcessCommand, 
                   CdmwPlatformMngt::DuplicateProcessCommandArg,
                   CdmwPlatformMngt::DuplicateEntityCommand, 
                   CdmwPlatformMngt::DuplicateEntityCommandArg,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException) = 0;
    
    /**
     *Purpose:
     *<p> Actually removes the specified application according to
     *    centralised/distributed supervision.
     *
     *@param application_name the name identifying the application.
     *
     *@exception ApplicationNotFound if application_name doesn't denote
     *           an existing application.
     *@exception ApplicationStillActive if the application is still active
     */
    virtual
    void doRemoveApplication (const char* application_name)
            throw (CdmwPlatformMngt::ApplicationNotFound,
                   CdmwPlatformMngt::ApplicationStillActive,
                   CORBA::SystemException) = 0;
    
    
    /**
     *Purpose:
     *<p> update application.
     *
     *@param host_name the name identifying the faulty host.
     *
     */
    void updateApplications (const char* host_name)
            throw ();
                   
                   
    /**
     *Purpose:
     *<p> get the application control
     *
     *@param application_name the name identifying the application.
     *
     */
    CdmwPlatformMngt::ApplicationControl_ptr get_application_control (
                              const char* application_name)
            throw (CdmwPlatformMngt::ApplicationNotFound);
            
    /**
     *Purpose:
     *<p> get the application process observer
     *
     *@param application_name the name identifying the application.
     *
     */
    CdmwPlatformMngt::ProcessObserver_ptr get_process_observer (
                              const char* application_name)
            throw (CdmwPlatformMngt::ApplicationNotFound);

    /**
     *Purpose:
     *<p> Puts the system into the degraded mode.
     */
    void degradation_event();

    /**
     *Purpose:
     *<p> Indicates the system is suspended.
     */
    void suspension_event();
    
    /**
     *Purpose:
     *<p> Indicates the system is completed.
     */
    void completion_event();

    
    /**
     *Purpose:
     *<p> Get the system port.
     */
     CdmwPlatformMngt::Port systemPort();
     
     /**
     *Purpose:
     *<p> Get the internal status from state machine.
     */
     CdmwPlatformMngt::SystemStatus get_internal_status();
     
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/get_status:1.0
     * operation
     */
    CdmwPlatformMngt::SystemStatus get_status(CORBA::String_out status_info)
          throw (CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/get_mode:1.0
     * operation
     */
    CdmwPlatformMngt::FunctioningMode get_mode(CORBA::String_out mode_info)
      throw (CORBA::SystemException);
    
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/get_snapshot:1.0
     * operation
     */
    CdmwPlatformMngt::SystemSnapshot* get_snapshot()
      throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/get_applications:1.0
     * operation
     */
    CdmwPlatformMngt::Applications* get_applications (
                              CORBA::ULong how_many,
                              CdmwPlatformMngt::ApplicationsIterator_out applications_iterator )
      throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/get_number_of_applications:1.0
     * operation
     */
    CORBA::ULong  get_number_of_applications()
          throw (CORBA::SystemException);
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/get_application:1.0
     * operation
     */
    CdmwPlatformMngt::Application_ptr get_application (
                              const char* application_name)
            throw (CdmwPlatformMngt::ApplicationNotFound,
                   CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/register_observer:1.0
     * operation
     */
    CdmwPlatformMngt::SupervisionObserver_ptr 
           register_observer (const char* observer_name,
                              CdmwPlatformMngt::SupervisionObserver_ptr observer)
      throw (CORBA::SystemException);
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/SystemAccessor/register_observer:1.0
     * operation
     */
    CdmwPlatformMngt::SupervisionObserver_ptr 
           register_proxy_observer (const char* observer_name,
                                    const char* host_name,
                                    CdmwPlatformMngt::SupervisionObserver_ptr observer)
      throw (CORBA::SystemException);
            
            
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/unregister_observer:1.0
     * operation
     */
    CdmwPlatformMngt::SupervisionObserver_ptr 
           unregister_observer (const char* observer_name)
            throw (CdmwPlatformMngt::ObserverNotFound,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/define:1.0
     * operation
     */
    void define(const CdmwPlatformMngt::SystemDef& system_def)
            throw (
                   CdmwPlatformMngt::DuplicateEntity,
                   CosPropertyService::MultipleExceptions,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateHost,
                   CdmwPlatformMngt::DuplicateHostEntity,
                   CdmwPlatformMngt::HostPropertyExceptions,
                   CdmwPlatformMngt::AlreadyDone,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/set_init_graph:1.0
     * operation
     */
    void set_init_graph (const CdmwPlatformMngt::GraphElements& application_graph)                  
            throw (CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/set_stop_graph:1.0
     * operation
     */
    void set_stop_graph (const CdmwPlatformMngt::GraphElements& application_graph)                  
            throw (CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/set_run_set:1.0
     * operation
     */
    void set_run_set(const CdmwPlatformMngt::RunSetDef& run_set_def)
          throw (CdmwPlatformMngt::DuplicateRunSetElement,
                 CdmwPlatformMngt::IncompatibleStatus,
                 CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/remove_run_set:1.0
     * operation
     */
    void remove_run_set(const char* run_set_name)
          throw (CdmwPlatformMngt::RunSetNotFound,
                 CdmwPlatformMngt::IncompatibleStatus,
                 CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/remove_all_run_sets:1.0
     * operation
     */
    void remove_all_run_sets()
          throw (CdmwPlatformMngt::IncompatibleStatus,
                 CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/add_host:1.0
     * operation
     */
    CdmwPlatformMngt::Host_ptr 
          add_host(const CdmwPlatformMngt::HostDef& host_def)
           throw (CdmwPlatformMngt::HostAlreadyExists,
                  CdmwPlatformMngt::DuplicateEntity,
                  CosPropertyService::MultipleExceptions,
                  CdmwPlatformMngt::IncompatibleStatus,
                  CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/remove_host:1.0
     * operation
     */
    void remove_host(const char* host_name)
           throw (CdmwPlatformMngt::HostNotFound,
                  CdmwPlatformMngt::IncompatibleStatus,
                  CORBA::SystemException);
                  

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/add_application:1.0
     * operation
     */
    CdmwPlatformMngt::Application_ptr 
            add_application(const CdmwPlatformMngt::PlatformApplicationDef& platform_application_def,
                            const CdmwPlatformMngt::ApplicationDef& application_def)
            throw (CdmwPlatformMngt::ApplicationAlreadyExists,
                   CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CosPropertyService::MultipleExceptions,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateProcess,
                   CdmwPlatformMngt::ProcessHostNotFound,
                   CdmwPlatformMngt::DuplicateProcessEntity,
                   CdmwPlatformMngt::ProcessPropertyExceptions,
                   CdmwPlatformMngt::DuplicateProcessService,
                   CdmwPlatformMngt::DuplicateProcessStep,
                   CdmwPlatformMngt::DuplicateProcessCommand, 
                   CdmwPlatformMngt::DuplicateProcessCommandArg,
                   CdmwPlatformMngt::DuplicateEntityCommand, 
                   CdmwPlatformMngt::DuplicateEntityCommandArg,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/remove_application:1.0
     * operation
     */
    void remove_application(const char* application_name)
            throw (CdmwPlatformMngt::ApplicationNotFound,
                   CdmwPlatformMngt::ApplicationStillActive,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);
 
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/start:1.0
     * operation
     */
    void start (const CdmwPlatformMngtBase::StartupKind& startup_kind)      
            throw (CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);

    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/resume:1.0
     * operation
     */
    void resume()
        throw (CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/acknowledge:1.0
     * operation
     */
    void acknowledge()
        throw (CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/stop:1.0
     * operation
     */
    void stop (CORBA::Boolean emergency)
            throw (CORBA::SystemException);
            
            
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/cold_restart:1.0
     * operation
     */
    void cold_restart (const CdmwPlatformMngtBase::StartupKind& startup_kind)      
            throw (CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);
                   
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/hot_restart:1.0
     * operation
     */
    void hot_restart (const CdmwPlatformMngtBase::StartupKind& startup_kind)      
            throw (CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);



    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/EntityContainer/get_number_of_entities:1.0
     * operation
     */
    CORBA::ULong get_number_of_entities()
      throw (CORBA::SystemException);
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/EntityContainer/get_all_entity_names:1.0
     * operation
     */
    void get_all_entity_names(CORBA::ULong how_many,               
                              CdmwPlatformMngt::ElementNames_out entity_names,
                              CdmwPlatformMngt::ElementNamesIterator_out rest)
      throw (CORBA::SystemException);
      
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
    CORBA::Boolean get_entities (
                              const CdmwPlatformMngt::ElementNames& entity_names,
                              CdmwPlatformMngt::Entities_out nentities)
      throw (CORBA::SystemException);
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/EntityContainer/get_all_entities:1.0
     * operation
     */
    void get_all_entities (CORBA::ULong how_many,     
                           CdmwPlatformMngt::Entities_out nentities,
                           CdmwPlatformMngt::EntitiesIterator_out rest)
      throw (CORBA::SystemException);
    
    
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
    void remove_entity (const char* entity_name)       
          throw (CdmwPlatformMngt::EntityNotFound,
                 CORBA::SystemException);






    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ServiceDefContainer/get_service_def:1.0
     * operation
     */
    CdmwPlatformMngtService::ServiceProviderID* get_service_def (const char* service_name)      
          throw (CdmwPlatformMngtService::ServiceNotFound,
                 CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/RWServiceDefContainer/add_service_def:1.0
     * operation
     */
    void add_service_def (const CdmwPlatformMngtService::ServiceDef& service_def)
           throw (CdmwPlatformMngtService::ServiceAlreadyExists,
                  CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/RWServiceDefContainer/remove_service_def:1.0
     * operation
     */
    void remove_service_def (const char* service_name)
            throw (CdmwPlatformMngtService::ServiceNotFound,
                   CORBA::SystemException);
    
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/HostContainer/get_number_of_hosts:1.0
     * operation
     */
    CORBA::ULong get_number_of_hosts()
      throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/HostContainer/get_all_host_names:1.0
     * operation
     */
    void get_all_host_names(CORBA::ULong how_many,
                            CdmwPlatformMngt::ElementNames_out host_names,
                            CdmwPlatformMngt::ElementNamesIterator_out rest)
      throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/HostContainer/get_host:1.0
     * operation
     */
    CdmwPlatformMngt::Host_ptr get_host(const char* host_name)
            throw (CdmwPlatformMngt::HostNotFound,
                   CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/HostContainer/get_hosts:1.0
     * operation
     */
    CORBA::Boolean get_hosts(const CdmwPlatformMngt::ElementNames& host_names,
                             CdmwPlatformMngt::Hosts_out hosts)
      throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/HostContainer/get_all_hosts:1.0
     * operation
     */
    void get_all_hosts(CORBA::ULong how_many,
                       CdmwPlatformMngt::Hosts_out hosts,
                       CdmwPlatformMngt::HostsIterator_out rest)
       throw (CORBA::SystemException);


    /**
     *Purpose:
     *<p> Get the runset manager.
     *
     *@return the runset manager.
     *
     */
    RunSetManager*  get_runset_manager();
    
    
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
     *<p> Get the event notifier to notify to external.
     *
     *@return the event notifer.
     *
     */
    SupervisionEventNotifier* get_event_notifier();
    
    /**
     *Purpose:
     *<p> Get the host_container.
     *
     */
    HostContainer_impl* get_host_container_impl();
    
    /**
     *Purpose:
     *<p> Get the actual host_container.
     *
     */
    PlatformMngt::HostContainer* get_actual_host_container();
    
    /**
     *Purpose:
     *<p> Get the service binding. (!!do not desallocate!!)
     *
     */
    const CdmwPlatformMngtService::ServiceBinding_ptr get_service_binding();

    
    
#ifdef CDMW_USE_FAULTTOLERANCE

    /**
     *Purpose:
     *<p> Set the ft converter object (iogr).
     *
     */
    void set_replication_mngr_iogr (const std::string& replication_mngr_iogr);
    
     /**
     *Purpose:
     *<p> Set the ft converter object (iogr).
     *
     */
    void set_replication_mngr (CdmwFT::ReplicationManager_ptr replication_mngr);
    
    /**
     *Purpose:
     *<p> Set the ft converter object (iogr).
     *
     */
    void set_ft_converter_object (CdmwPlatformMngt::FTConverter_ptr ft_converter_obj);
        
    /**
     *Purpose:
     *<p> Set the fault tolerance process observer object (iogr).
     *
     */
    void set_ft_process_observer_group (CdmwPlatformMngt::ProcessObserver_ptr ft_process_observer_group);
    
    /**
     *Purpose:
     *<p> Set the fault tolerance service broker object (iogr).
     *
     */
    void set_ft_service_broker_group (CdmwPlatformMngtService::ServiceBroker_ptr ft_service_broker_group);
    
#endif
    
 
protected:

                                   
                       
private:

    /**
     *Purpose:
     *<p> Default constructor is not allowed.
     */
    System_impl();

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    System_impl (System_impl& rhs);

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    System_impl& operator = (const System_impl& rhs);
    
      
    /**
     *Purpose:
     *<p> Set the system record in data store
     *
     */
    void set_system_record_in_datastore();
            
    /**
     *Purpose:
     *<p> Remove the system record in datastore
     *
     */
    void remove_system_record_in_datastore();
    
    
    /**
     *Purpose:
     *<p> Get the system record from data store
     *
     */
    CdmwPlatformMngt::SystemRecord* get_system_record_from_datastore()
       throw (Common::NotFoundException);
    
    
    /**
     *Purpose:
     *<p> Set the system observer record in data store
     *
     * @param observer_name the name of observer to insert
     * @param host_name the host name where the observer is running
     * @param observer the observer reference
     */
    void set_observer_record_in_datastore(const char* observer_name,
                                          const char* host_name,
                                          CdmwPlatformMngt::SupervisionObserver_ptr observer);
            
    /**
     *Purpose:
     *<p> Remove the system observer_record in datastore
     *
     * @param observer_name the name of observer to remove
     *
     */
    void remove_observer_record_in_datastore(const char* observer_name);
    
    

    /**
     *Purpose:
     *<p> Notifies a mode change events to the supervision event notifier.
     *
     * @param functioning_mode the functionning mode
     * @param mode_info the associated info
     */
    void notifyModeChangeEvent(CdmwPlatformMngt::FunctioningMode functioning_mode,
                               const char* mode_info); 
      
    
    
        
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_SYSTEM_IMPL_HPP

