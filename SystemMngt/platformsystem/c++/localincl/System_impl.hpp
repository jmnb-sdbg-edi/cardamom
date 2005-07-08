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


#ifndef INCL_PLATFORMMNGT_SYSTEM_IMPL_HPP
#define INCL_PLATFORMMNGT_SYSTEM_IMPL_HPP

#include "platformsystem/SystemStates.hpp"

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/osthreads/Condition.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/ReaderWriterLock.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtSystem.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtApplication.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtService.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntity.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtHostProxy.stub.hpp"
#include "SystemMngt/platformlibrary/StateMachineTemplate.hpp"
#include "SystemMngt/platformlibrary/EventHandler.hpp"
#include "SystemMngt/platformlibrary/ServiceDefContainer_impl.hpp"
#include "SystemMngt/platformlibrary/EntityContainer_impl.hpp"

#ifdef CDMW_USE_FAULTTOLERANCE
#include "platformsystem/FTConverter_impl.hpp"
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

class SupervisionEventNotifier;
class RunSetManager;

class SynchronisableTaskFactory;
class Sequencer;

class SystemStatusChangeFactory;
class SystemModeChangeFactory;
class SystemEntityStatusChangeFactory;
class HostStatusChangeFactory;

class SystemEntityObserver_impl;
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
     *Purpose:
     *<p> Default constructor.
     *
     *@param application         reference of application object.
     *@param applicationControl  reference of application control object.
     *
     */
    ApplicationRef (CdmwPlatformMngt::Application* application,
                    CdmwPlatformMngt::ApplicationControl* applicationControl)
    {
        m_application = CdmwPlatformMngt::Application::_duplicate(application);
        m_applicationControl = 
              CdmwPlatformMngt::ApplicationControl::_duplicate(applicationControl);
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
    };

    /**
     * Purpose:
     * <p> Assignment operator.
     */ 
    ApplicationRef& operator = (const ApplicationRef& rhs)

    {
        m_application = rhs.m_application;
        m_applicationControl = rhs.m_applicationControl;
        
        return *this;
    };

};

/**
 *Purpose:
 *<p>
 */
class System_impl : virtual public POA_CdmwPlatformMngt::System,
                    virtual public PortableServer::RefCountServantBase
{




public:

    /**
     * Define the applications map
     */
    typedef std::map <std::string, ApplicationRef> Applications;
        
     
     
    /**
     * Read/Writer lock guard tp protect system data
     */
    OsSupport::ReaderWriterLock m_rwSystemLock;
        
    /**
     * The list of applications managed by this system.
     */
    Applications m_applications;      
        
    /**
     * The associated state machine.
     */
    SystemStateMachine* m_stateMachine;

    /**
     * The runset manager
     */
    RunSetManager* m_runSetManager;

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
     * The information about the status of this system.
     */
    CORBA::String_var m_statusInfo;



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

    /**
     *Purpose:
     *<p> Default constructor.
     *     
     *@param eventNotifier the supervision observer to be notified of events. 
     *@param orb           the orb reference
     *@param poa           the poa in charge of servants    
     *
     *@exception BadParameterException if the parameter is invalid.
     *@exception AlreadyDoneException if the state machine is already running.
     *@exception UnknownStateException if stateName doesn't refer to a known
     * state.
     *@exception AlreadyExistException if the system container already exist.
     *@exception OutOfRessourcesException Lack of ressource.
     *@exception OutOfMemoryException Lack of memory.
     */
#ifdef CDMW_USE_FAULTTOLERANCE
    System_impl (SupervisionEventNotifier* eventNotifier,
                 CORBA::ORB_ptr orb,
                 PortableServer::POA_ptr poa,
                 std::string faultManCorbaloc)    
#else
    System_impl (SupervisionEventNotifier* eventNotifier,
                 CORBA::ORB_ptr orb,
                 PortableServer::POA_ptr poa)    
        
#endif
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
                      CdmwPlatformMngtEntity::EntityStatus entity_status,
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
                      CdmwPlatformMngt::HostStatus host_status);


    /**
     *Purpose:
     *<p> Defines the initial configuration of the system.
     *
     *@param system_def the definition of the system.
     *    
     *@exception DuplicateEntity if there is a duplicate entity name among
     *           the system's entities
     *@exception DuplicateService if there is a duplicate service name among
     *           the system's service definitions
     *@exception DuplicateHost if there is a duplicate host name among
     *           the system's host definitions
     *@exception AlreadyDone if system has been already defined
     */
    void defineSystem (const CdmwPlatformMngt::SystemDef& system_def)
        throw (CdmwPlatformMngt::DuplicateEntity,
               CdmwPlatformMngt::DuplicateService,
               CdmwPlatformMngt::DuplicateHost,
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
     *@return the proxy to the added host
     *
     *@exception HostAlreadyExists if the host name is already used
     *           within the system.
     *
     */
    CdmwPlatformMngt::HostProxy_ptr 
          addHost(const CdmwPlatformMngt::HostDef& host_def)
           throw (CdmwPlatformMngt::HostAlreadyExists,
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
     * @exception DuplicateService if there is a duplicate service name among
     *            the application's service definitions
     * @exception DuplicateProcess if there is a duplicate process name among
     *            the application's processes
     * @exception ProcessHostNotFound if an unknown host is used by one of
     *            the application's processes
     * @exception DuplicateProcessEntity if there is a duplicate entity name
     *            among the entities of one of the application processes
     * @exception DuplicateProcessService if there is a duplicate service
     *            name among the services of one of the application processes
     * @exception DuplicateProcessStep if there is a duplicate step label
     *            among the steps of one of the application processes
     * @exception DuplicateProcessActivityPoint if there is a duplicate
     *            name among the activity points of one of the application processes
     */
    CdmwPlatformMngt::Application_ptr 
            addApplication(
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
     * @exception DuplicateService if there is a duplicate service name among
     *            the application's service definitions
     * @exception DuplicateProcess if there is a duplicate process name among
     *            the application's processes
     * @exception ProcessHostNotFound if an unknown host is used by one of
     *            the application's processes
     * @exception DuplicateProcessEntity if there is a duplicate entity name
     *            among the entities of one of the application processes
     * @exception DuplicateProcessService if there is a duplicate service
     *            name among the services of one of the application processes
     * @exception DuplicateProcessStep if there is a duplicate step label
     *            among the steps of one of the application processes
     * @exception DuplicateProcessActivityPoint if there is a duplicate
     *            name among the activity points of one of the application processes
     * @exception IncompatibleStatus if application status incompatible
     */
    virtual
    CdmwPlatformMngt::Application_ptr 
            doAddApplication(
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
     *<p> Get event handler.
     */
    EventHandler* getEventHandler();

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
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/name:1.0
     * operation
     */
    CdmwPlatformMngt::SystemName name()
      throw (CORBA::SystemException);

    
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
           register_observer(const char* observer_name,
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
           unregister_observer(const char* observer_name)
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
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateHost,
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
    CdmwPlatformMngt::HostProxy_ptr 
          add_host(const CdmwPlatformMngt::HostDef& host_def)
           throw (CdmwPlatformMngt::HostAlreadyExists,
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
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateProcess,
                   CdmwPlatformMngt::ProcessHostNotFound,
                   CdmwPlatformMngt::DuplicateProcessEntity,
                   CdmwPlatformMngt::DuplicateProcessService,
                   CdmwPlatformMngt::DuplicateProcessStep,
                   CdmwPlatformMngt::DuplicateProcessActivityPoint,
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
                              CdmwPlatformMngtEntity::EntityNames_out entity_names,
                              CdmwPlatformMngtEntity::EntityNamesIterator_out rest)
      throw (CORBA::SystemException);
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/EntityContainer/get_entity_status:1.0
     * operation
     */
    CdmwPlatformMngtEntity::EntityStatus get_entity_status (
                              const char* entity_name,
                              CORBA::String_out entity_info)
         throw (CdmwPlatformMngtEntity::EntityNotFound,
                CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/EntityContainer/get_entities:1.0
     * operation
     */
    CORBA::Boolean get_entities (
                              const CdmwPlatformMngtEntity::EntityNames& entity_names,
                              CdmwPlatformMngtEntity::Entities_out nentities)
      throw (CORBA::SystemException);
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/EntityContainer/get_all_entities:1.0
     * operation
     */
    void get_all_entities (CORBA::ULong how_many,     
                           CdmwPlatformMngtEntity::Entities_out nentities,
                           CdmwPlatformMngtEntity::EntitiesIterator_out rest)
      throw (CORBA::SystemException);
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/RWEntityContainer/add_entity:1.0
     * operation
     */
    void add_entity (const char* entity_name) 
          throw (CdmwPlatformMngtEntity::EntityAlreadyExists,
                 CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/RWEntityContainer/remove_entity:1.0
     * operation
     */
    void remove_entity (const char* entity_name)       
          throw (CdmwPlatformMngtEntity::EntityNotFound,
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyContainer/get_number_of_properties:1.0
     * operation
     */
    CORBA::ULong get_number_of_properties()
      throw (CORBA::SystemException);
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyContainer/get_all_property_names:1.0
     * operation
     */
    void get_all_property_names (CORBA::ULong how_many,       
                                 CosPropertyService::PropertyNames_out property_names,
                                 CosPropertyService::PropertyNamesIterator_out rest)
      throw (CORBA::SystemException);
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyContainer/get_property_value:1.0
     * operation
     */
    CORBA::Any* get_property_value (const char* property_name)
            throw (CosPropertyService::PropertyNotFound,
                   CosPropertyService::InvalidPropertyName,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyContainer/get_properties:1.0
     * operation
     */
    CORBA::Boolean get_properties (
                          const CosPropertyService::PropertyNames& property_names,
                          CosPropertyService::Properties_out nproperties)
      throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyContainer/get_all_properties:1.0
     * operation
     */
    void get_all_properties (CORBA::ULong how_many,      
                             CosPropertyService::Properties_out nproperties,
                             CosPropertyService::PropertiesIterator_out rest)
      throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/RWPropertyContainer/set_property:1.0
     * operation
     */
    void set_property (const char* property_name,      
                       const CORBA::Any& property_value)
      throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/RWPropertyContainer/remove_property:1.0
     * operation
     */
    void remove_property (const char* property_name)
            throw (CosPropertyService::PropertyNotFound,
                   CORBA::SystemException);




    
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
                            CdmwPlatformMngt::HostNames_out host_names,
                            CdmwPlatformMngt::HostNamesIterator_out rest)
      throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/HostContainer/get_host:1.0
     * operation
     */
    CdmwPlatformMngt::HostProxy_ptr get_host(const char* host_name)
            throw (CdmwPlatformMngt::HostNotFound,
                   CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/HostContainer/get_hosts:1.0
     * operation
     */
    CORBA::Boolean get_hosts(const CdmwPlatformMngt::HostNames& host_names,
                             CdmwPlatformMngt::HostProxies_out hosts)
      throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/HostContainer/get_all_hosts:1.0
     * operation
     */
    void get_all_hosts(CORBA::ULong how_many,
                       CdmwPlatformMngt::HostProxies_out hosts,
                       CdmwPlatformMngt::HostProxiesIterator_out rest)
       throw (CORBA::SystemException);




    /**
     *Purpose:
     *<p> return the system name
     *
     */
    const char* get_systemName();
    
    

 
protected:

    /**
     * The supervision event notifier.
     */
    SupervisionEventNotifier* m_eventNotifier;
    
    /**
    * ORB reference
    */ 
    CORBA::ORB_var m_orb;
    
    /**
     * The specific POA in charge of servants.
     */
    PortableServer::POA_var m_poa;  
    
#ifdef CDMW_USE_FAULTTOLERANCE
    /**
     * The fault notifier
     */
    FT::FaultNotifier_var m_faultNotifier;
    
    /**
     * The fault detector
     **/
    CdmwFT::FaultManagement::FaultDetector_var m_faultDetector;    

    /**
     * The replication manager
     */
    CdmwFT::ReplicationManager_var m_replicationMngt;
    
    /**
     * the replication manager IOGR
     */
    std::string m_replicationManagerIOGR;

    /**
     * The fault manager corbaloc
     */
    std::string m_faultManagerCorbaloc;

    /**
     * The fault tolerance converter object.
     */
    CdmwPlatformMngt::FTConverter_var m_FTConverterObject;
#endif    

    /**
     * The system is defined
     */
    bool m_systemDefined;
    
    /**
     * The system name.
     */
    CdmwPlatformMngt::SystemName_var m_systemName;

    /**
     * The system port.
     */
    CdmwPlatformMngt::Port m_systemPort;
    
    /**
     * The service defs container object.
     */
    CdmwPlatformMngtService::ServiceDefContainer_var m_serviceDefContainerObject;
    
    /**
     * The service binding object
     */
    CdmwPlatformMngtService::ServiceBinding_var m_serviceBindingObject;
    
    /**
     * The entity observer object.
     */
    CdmwPlatformMngtEntity::SystemEntityObserver_var m_entityObserverObject;
    
    /**
     * The host container object (read only).
     */
    CdmwPlatformMngt::HostContainer_var m_hostContainerObject;
    
    /**
     * The application observer object.
     */
    CdmwPlatformMngt::ApplicationObserver_var m_applicationObserverObject;                               
                       
private:

    /**
     *Purpose:
     *<p> Default constructor is not allowed.
     */
    System_impl()
    {};

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    System_impl (System_impl& rhs)
    {};

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    System_impl& operator = (const System_impl& rhs)

    {
        return *this;
    };
    
     
     
    /**
     *Purpose:
     *<p> Notifies a mode change events to the supervision event notifier.
     */
    void notifyModeChangeEvent(); 
      
    
    /**
     * The functioning mode.
     */
    CdmwPlatformMngt::FunctioningMode m_mode;
        
    /**
     * The information about the mode of this system.
     */
    CORBA::String_var m_modeInfo;
    
    /**
     * The mutex used for changing the functioning mode.
     */
    OsSupport::Mutex m_mutexMode;



    /**
     * The actual service defs container.
     */  
    ServiceDefContainer_impl::ServiceDefContainer* m_actualServiceDefContainer;
    
    /**
     * The rw service defs container.
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
     * The system entity container.
     */  
    EntityContainer_impl::EntityContainer* m_actualEntityContainer;
    
    /**
     * The entity container.
     */
    RWEntityContainer_impl* m_rwEntityContainer;
       
    /**
     * The entity observer servant.
     */    
    SystemEntityObserver_impl* m_entityObserver;        

        
    /**
     * The host container.
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
    
#ifdef CDMW_USE_FAULTTOLERANCE
    /**
     * The fault tolerance converter servant.
     */ 
    FTConverter_impl*  m_FTConverter;           
#endif
    
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
        
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_SYSTEM_IMPL_HPP

