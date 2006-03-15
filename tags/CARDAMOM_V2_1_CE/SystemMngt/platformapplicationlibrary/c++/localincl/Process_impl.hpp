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


#ifndef INCL_PLATFORMMNGT_PROCESS_HPP
#define INCL_PLATFORMMNGT_PROCESS_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcess.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtServiceContainer.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtApplicationAgent.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessDelegateWrapper.stub.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp"

#include "SystemMngt/platformlibrary/DeactivableServant_impl.hpp"
#include "SystemMngt/platformlibrary/ServiceDefContainer_impl.hpp"

#include "SystemMngt/platformelementlibrary/EntityContainer_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "SystemMngt/platformelementlibrary/ManagedElement_impl.hpp"
#include "SystemMngt/platformelementlibrary/CommandedElement_impl.hpp"
#include "SystemMngt/platformelementlibrary/DataStoreDefinition.hpp"

#include "platformapplicationlibrary/CommandCallback.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class Application_impl;
class ServiceBroker_impl;
class SupervisionEventNotifier;
class ProcessEntityStatusChangeFactory;
class ProcessStatusChangeFactory;

/**
 *Purpose:
 *<p>
 */
class Process_impl : virtual public POA_CdmwPlatformMngt::Process,
                     public DeactivableServant_impl,
                     public ManagedElement_impl,
                     public CommandContainer_impl
{
    
private:

    /**
     * Indicates whether the process is autoending or not.
     */
    CORBA::Boolean m_autoending;

    /**
     * The type of the process.
     */
    CdmwPlatformMngt::ProcessType m_type;

    /**
     * The process info.
     */
    CdmwPlatformMngt::ProcessInfo_var m_process_info;

protected: 
    
    /**
     * The process host name.
     */
    CORBA::String_var m_proc_host_name;

    /**
     * The process application name.
     */
    CORBA::String_var m_proc_appli_name;
    
    /**
     * The process system name.
     */
    CORBA::String_var m_proc_syst_name;

	/**
     * The process key.
     */
    CORBA::String_var m_proc_key;
    

    /**
     * The host executing the process.
     */
    CdmwPlatformMngt::Host_var m_host;
    
    /**
     * The application managing this process.
     */
    Application_impl* m_application;

    /**
     * Tha process entity status change factory.
     */
    ProcessEntityStatusChangeFactory* m_procEntityStatusChangeFactory;

    /**
     * The process status change factory.
     */
    ProcessStatusChangeFactory* m_procStatusChangeFactory;
    
    /**
     * The command callback for process starting.
     */
    CommandCallback* m_process_start_cbk;
    
    /**
     * The acknowledgement callback for process stopping.
     */
    CommandCallback* m_process_stop_cbk;
    
    /**
     * The process starting command result.
     */
    CdmwPlatformMngt::ProcessCommandResult m_starting_result;
    
    /**
     * The process stopping command result.
     */
    CdmwPlatformMngt::ProcessCommandResult m_stopping_result;
    
    /**
     * The process command command error info.
     */
    CORBA::String_var m_creation_error_info;
    
    /**
     * The process command command error issuer.
     */
    CORBA::String_var m_creation_error_issuer;
    
    /**
     * Process datastore
     */
    ProcessDataStore* m_process_ds;
    
    /**
     * The status of the process in datastore
     */
    CdmwPlatformMngt::ProcessStatus m_status_in_ds;
    

public:

    

    /**
     * The life cycle of this process.
     */
    CdmwPlatformMngt::LifeCycleDef_var m_lifeCycle;

    /**
     * The monitoring info.
     */
    CdmwPlatformMngt::CompleteMonitoringInfo_var m_monitoring_info;

    /**
     * The information about the status of this process.
     */
    CORBA::String_var m_status_info;

    /**
     * The date and the time of the last error event.
     */
    CdmwPlatformMngtBase::EventHeader m_lastErrorHeader;

    /**
     * The issuer of the last error event.
     */
    std::string m_lastErrorIssuer;

    /**
     * The counter of init steps.
     */
    unsigned int m_stepCounter;

    /**
     * The status of the last action.
     */
    CdmwPlatformMngt::RequestStatus m_lastRequestStatus;
    

private:

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    Process_impl(const Process_impl& rhs);

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    Process_impl& operator=(const Process_impl& rhs);

protected:

    /**
     *Purpose:
     *<p> constructor.
     */
    Process_impl(
        PortableServer::POA_ptr poa,
        CdmwPlatformMngt::ProcessDef* process_def,
        const char* process_key, const char* process_path,
        Application_impl* application,
        CdmwPlatformMngt::RWElementRepository_ptr element_repository);

 
    
    /**
     *Purpose:
     *<p> Set the process record in data store
     *
     */
    void set_record_in_datastore();
            
    /**
     *Purpose:
     *<p> Remove the process record in datastore
     *
     */
    void remove_record_in_datastore();
    
    /**
     *Purpose:
     *<p> Get the process record from data store
     *
     */
    CdmwPlatformMngt::ProcessRecord* get_record_from_datastore();
    
    /**
     *Purpose:
     *<p> Get the current state name of state machine
     *
     */
    virtual
    const char* get_state_machine_state_name() = 0;
    
    /**
     *Purpose:
     *<p> Set the current state name of state machine
     *
     */
    virtual
    void set_state_machine_state_name(const char* state_name) = 0;
    

    /**
     *Purpose:
     *<p> Gets the process's states.
     * This method must exclusivaly be used by the state machine.
     */
    virtual
    CdmwPlatformMngt::ProcessStatus getStatus()
        throw( CORBA::SystemException ) = 0;
        
       

public:
    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual
    ~Process_impl();

public:

    /**
     *Purpose:
     *<p> Ends the creation of the process servant
     *
     *@exception HostNotFound if the host where the process will be run is
     * not found.
     *@exception DuplicateEntity if there is a duplicate entity name among
     * the entities of the process
     *@exception DuplicateService if there is a duplicate service name among
     * the service definitions of the process
     *@exception DuplicateStep if there is a duplicate step label among
     * the steps of the process
     */
    virtual
    void end_creation(CdmwPlatformMngt::ProcessDef* process_def)
            throw (CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateStep,
                   OutOfMemoryException,
  	    	   CosPropertyService::MultipleExceptions,
                   CORBA::SystemException) = 0;
                   
    /**
     *Purpose:
     *<p> Synchronise with datastore
     *
     */
    virtual
    void synchro_with_datastore();
    
    /**
     *Purpose:
     *<p> Synchronise the processing
     *
     */
    void synchro_processing();
    
    /**
     *Purpose:
     *<p> Synchronise the processing to initial state
     *
     */
    virtual
    void synchro_processing_to_initial () = 0;
    
    /**
     *Purpose:
     *<p> Synchronise the processing to initialised state
     *
     */
    virtual
    void synchro_processing_to_initialised () = 0;
    
    /**
     *Purpose:
     *<p> Synchronise the processing to running state
     *
     */
    virtual
    void synchro_processing_to_running () = 0;
    

    /**
     *Purpose:
     *<p> Called when a user application error has occured.
     *
     *@param error_info the information about the error.
     */
    virtual
    void error_event(
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* issuer,
        const char* error_info ) = 0;

    /**
     *Purpose:
     *<p> Called when the process has ended its running step.
     */
    virtual
    void ending_event(
        const CdmwPlatformMngtBase::EventHeader& header ) = 0;
        
    /**
     *Purpose:
     *<p> Called when a monitoring failure event has happened.
     *
     *@exception IncompatibleType if the process is not a managed process.
     */
    virtual
    void monitoringFailure_event(
        const CdmwPlatformMngtBase::EventHeader& header )
            throw( CdmwPlatformMngt::IncompatibleType,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p> Called when a technical error has occured.
     *
     *@param error_info the information about the error.
     */
    virtual
    void invalidate_event(
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* reason) = 0;
        
        
    /**
     *Purpose:
     *<p> Get the internal status from state machine.
     */
     virtual
     CdmwPlatformMngt::ProcessStatus get_internal_status() = 0;
     
     /**
     *Purpose:
     *<p> Get the status stored in datastore.
     */
     CdmwPlatformMngt::ProcessStatus get_status_in_ds();
       

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/type:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::ProcessType type()
            throw(CORBA::SystemException);
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

  
    /*const *
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_status:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::ProcessStatus get_status(
        CORBA::String_out status_info)
            throw(CORBA::SystemException) = 0;

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/embedded_service:1.0
     * operation
     */
    virtual CORBA::Object_ptr embedded_service()
            throw(CORBA::SystemException);
     
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/pid:1.0
     * operation
     */       
    virtual CdmwPlatformMngt::ProcessPID pid()
            throw(CORBA::SystemException);
                
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/created_dynamically:1.0
     * operation
     */
    virtual CORBA::Boolean created_dynamically()
            throw(CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/restart_type:1.0
     * operation
     */    
    virtual CdmwPlatformMngt::RestartType restart_type()
            throw(CORBA::SystemException);
    
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/restart_attempt:1.0
     * operation
     */      
    virtual CORBA::Long restart_attempt()
            throw(CORBA::SystemException);
     
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/start_time:1.0
     * operation
     */     
    virtual CdmwPlatformMngtBase::TimeStamp start_time()
            throw(CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop_time:1.0
     * operation
     */    
    virtual CdmwPlatformMngtBase::TimeStamp stop_time()
            throw(CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/is_manual_starting:1.0
     * operation
     */
    virtual CORBA::Boolean is_manual_starting()
            throw(CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_manual_starting:1.0
     * operation
     */
    virtual void set_manual_starting (CORBA::Boolean manual_starting)
       throw(CdmwPlatformMngt::IncompatibleStatus,
             CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/is_auto_ending:1.0
     * operation
     */
    virtual
    CORBA::Boolean is_auto_ending()
      throw(CORBA::SystemException);
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_auto_ending:1.0
     * operation
     */
    virtual
    void set_auto_ending(CORBA::Boolean auto_ending)
        throw (CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException) = 0;

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_parent:1.0
     * operation
     */
    CdmwPlatformMngt::ManagedElement_ptr Process_impl::get_parent() 
        throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_host:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::Host_ptr get_host()
      throw (CORBA::SystemException);
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_host:1.0
     * operation
     */
    virtual
    void set_host(const char* host_name)
            throw (CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException) = 0;

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_process_info:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::ProcessInfo* get_process_info()
      throw (CORBA::SystemException);
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_process_info:1.0
     * operation
     */
    virtual
    void set_process_info(const CdmwPlatformMngt::ProcessInfo& process_info)
            throw (CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException) = 0;

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_process_mode:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::ProcessMode  get_process_mode()
      throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_memory_resident:1.0
     * operation
     */
    virtual
    bool get_memory_resident()
      throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_memory_resident:1.0
     * operation
     */
    virtual void set_memory_resident( bool memory_resident)
      throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_core_dump_size:1.0
     * operation
     */
    virtual
    CORBA::Long get_core_dump_size()
      throw (CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_life_cycle:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::LifeCycleDef* get_life_cycle()
            throw (CdmwPlatformMngt::IncompatibleType,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_life_cycle:1.0
     * operation
     */
    virtual
    void set_life_cycle (const CdmwPlatformMngt::LifeCycleDef& life_cycle)
            throw (CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::IncompatibleType,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_monitoring_info:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::CompleteMonitoringInfo get_monitoring_info()
            throw (CdmwPlatformMngt::IncompatibleType,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_monitoring_info:1.0
     * operation
     */
    virtual
    void set_monitoring_info(
        const CdmwPlatformMngt::CompleteMonitoringInfo& monitoring_info)
            throw (CdmwPlatformMngt::IncompatibleType,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_exit_autorestart_info:1.0
     * operation
     */
    virtual CdmwPlatformMngt::ExitAutoRestartInfo* get_exit_autorestart_info ()
             throw (CORBA::SystemException);
             
             
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_exit_autorestart_info:1.0
     * operation
     */
    virtual void set_exit_autorestart_info (
        const CdmwPlatformMngt::ExitAutoRestartInfo & exit_autorestart_info)
             throw (CdmwPlatformMngt::IncompatibleStatus,
                    CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_snapshot:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::ProcessSnapshot* get_snapshot()
      throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_run_as_root:1.0
     * operation
     */
    virtual
    bool get_run_as_root()
      throw (CORBA::SystemException);
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::RequestStatus initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind)
            throw (CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException) = 0;

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/next_step:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::RequestStatus next_step()
            throw (CdmwPlatformMngt::InvalidStep,
                   CdmwPlatformMngt::IncompatibleType,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::RequestStatus run()
            throw (CdmwPlatformMngt::NotReadyToRun,
                   CdmwPlatformMngt::IncompatibleType,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::RequestStatus stop(CORBA::Boolean emergency)
      throw (CORBA::SystemException) = 0;
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/cold_restart:1.0
     * operation
     */  
    virtual CdmwPlatformMngt::RequestStatus cold_restart (
        const CdmwPlatformMngtBase::StartupKind & startup_kind)
      throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/hot_restart:1.0
     * operation
     */
    virtual CdmwPlatformMngt::RequestStatus hot_restart (
        const CdmwPlatformMngtBase::StartupKind & startup_kind)
      throw (CORBA::SystemException);
      
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/EntityContainer/get_number_of_entities:1.0
     * operation
     */
    virtual
    CORBA::ULong get_number_of_entities()
        throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/EntityContainer/get_all_entity_names:1.0
     * operation
     */
    virtual
    void get_all_entity_names(
        CORBA::ULong how_many,
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
    virtual
    CdmwPlatformMngt::Entity_ptr get_entity(const char* entity_name)
            throw (CdmwPlatformMngt::EntityNotFound,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/EntityContainer/get_entities:1.0
     * operation
     */
    virtual
    CORBA::Boolean get_entities(
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
    virtual
    void get_all_entities(
        CORBA::ULong how_many,
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
    virtual
    CdmwPlatformMngt::Entity_ptr add_entity(const CdmwPlatformMngt::EntityDef & entity_def)
            throw (CdmwPlatformMngt::EntityAlreadyExists,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/RWEntityContainer/remove_entity:1.0
     * operation
     */
    virtual
    void remove_entity(const char* entity_name)
            throw (CdmwPlatformMngt::EntityNotFound,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngtService/ServiceDefContainer/get_service_def:1.0
     * operation
     */
    virtual
    CdmwPlatformMngtService::ServiceProviderID* get_service_def(
        const char* service_name)
            throw (CdmwPlatformMngtService::ServiceNotFound,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngtService/RWServiceDefContainer/add_service_def:1.0
     * operation
     */
    virtual
    void add_service_def(
        const CdmwPlatformMngtService::ServiceDef& service_def)
            throw (CdmwPlatformMngtService::ServiceAlreadyExists,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngtService/RWServiceDefContainer/remove_service_def:1.0
     * operation
     */
    virtual
    void remove_service_def(const char* service_name)
            throw (CdmwPlatformMngtService::ServiceNotFound,
                   CORBA::SystemException);
                   
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/RWServiceDefContainer/remove_all_service_defs:1.0
     * operation
     */ 
    virtual              
    void remove_all_service_defs()
		    throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PullMonitorable/is_alive:1.0
     * operation
     */
    void is_alive();

    /**
     *Purpose:
     *<p> Gets the application agent in charge of this process. This method
     * is called within the state machine.
     *
     *@return the application agent or a NIL reference if there is no agent
     * for the application in charge of this process.
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
    CdmwPlatformMngt::ApplicationAgent_ptr getApplicationAgent()
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
     *<p> Search from daemon the application agent in charge of this process.
     *
     *@return the application agent or a NIL reference if there is no agent
     *
     *@exception HostNotFound if the host cannot be found.
     *@exception HostNotReachable if the host doesn't respond.
     *@exception CORBA::SystemException
     */
    CdmwPlatformMngt::ApplicationAgent_ptr searchApplicationAgent()
        throw (CdmwPlatformMngt::HostNotFound,
               CdmwPlatformMngt::HostNotReachable,
               CORBA::SystemException);
               
  
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
        const char* entity_info);

    /**
     *Purpose:
     *<p> Actually sets the process as ending itself.
     *
     *@param auto_ending set if process can end itself.
     */    
    void setAutoending(CORBA::Boolean auto_ending);

    /**
     *Purpose:
     *<p> Actually changes the host where the process will be running.
     *
     *@param host_name the name of the host.
     *
     *@exception HostNotFound if the host where the process will be run is
     * not found.
     */
    void setHost(const char* host_name)
            throw (CdmwPlatformMngt::HostNotFound,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p> Actually changes the information needed for the process creation.
     *
     *@param process_info the information of the process.
     */
    virtual
    void setProcessInfo(
        const CdmwPlatformMngt::ProcessInfo& process_info);

    /**
     *Purpose:
     *<p> Actually changes the information about the monitoring of the process.
     *
     *@param monitoring_info the new information about the monitoring of the
     * process.
     */
    void setMonitoringInfo(
        const CdmwPlatformMngt::CompleteMonitoringInfo& monitoring_info);

    /**
     *Purpose:
     *<p> Called when an error has occured. This method is called within
     * the state machine.
     */
    void errorEvent(
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* issuer,
        const char* error_info);

    /**
     *Purpose:
     *<p> Called when the process has ended its running step. This method
     * is called within the state machine.
     */
    void endingEvent(
        const CdmwPlatformMngtBase::EventHeader& header);

    /**
     *Purpose:
     *<p> Called when a technical error has occured. This method is called
     * within the state machine.
     */
    void invalidateEvent(
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* reason);

    /**
     * Moves the state machine to specified state.
     */
    virtual
    void setState(const char* stateName) = 0;
        
    
    /**
     *Purpose:
     *<p> return the application managing the process
     *
     */
    Application_impl* get_application();
    
        
    /**
     *Purpose:
     *<p> return the host name where process is running
     *   !! do not desallocate !!
     *
     */
    const char* get_process_host_name();
    
    /**
     *Purpose:
     *<p> return the process key (host + process)
     *   !! do not desallocate !!
     *
     */
    const char* get_process_key();
    
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_create_process_result:1.0
     * operator
     */
     virtual
     void set_create_process_result (
        CdmwPlatformMngt::ProcessCommandResult result,
        const char* error_info,
        const char* error_issuer,
        CdmwPlatformMngt::ProcessDelegateWrapper_ptr process_delegate_wrap)
            throw (CORBA::SystemException);

     
     /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_initialisation_result:1.0
     * operator
     */
     virtual
     void set_initialisation_result (CdmwPlatformMngt::ProcessCommandResult result)
            throw (CORBA::SystemException);
     
     /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_next_step_result:1.0
     * operator
     */
     virtual
     void set_next_step_result (CdmwPlatformMngt::ProcessCommandResult result,
                                CORBA::ULong step_nbr)
            throw (CORBA::SystemException);
     
     /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_run_result:1.0
     * operator
     */
     virtual
     void set_run_result (CdmwPlatformMngt::ProcessCommandResult result)
            throw (CORBA::SystemException);
     
     /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_stop_result:1.0
     * operator
     */
     virtual
     void set_stop_result (CdmwPlatformMngt::ProcessCommandResult result)
            throw (CORBA::SystemException);
        
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_PROCESS_HPP

