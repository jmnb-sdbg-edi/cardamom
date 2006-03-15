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


#ifndef INCL_PLATFORMMNGT_MANAGED_PROCESS_PROXY_HPP
#define INCL_PLATFORMMNGT_MANAGED_PROCESS_PROXY_HPP

#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "platformapplicationlibrary/Process_impl.hpp"
#include "platformapplicationlibrary/ManagedProcessStates.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessDelegateWrapper.stub.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class RWServiceDefContainer_impl;
class RWEntityContainer_impl;

/**
 *Purpose:
 *<p> The process managing a managed process.
 */
class ManagedProcess_impl : public Process_impl
{
	friend class ManagedProcessInitialising;
	friend class ManagedProcessRunRequest;
	friend class ManagedProcessStopping;
	friend class ManagedProcessStopped;
	friend class ManagedProcessEnded;
	friend class ManagedProcessFailedDeath;
	friend class ManagedProcessFailedError;
	friend class ManagedProcessFailedMonitoringFailure;
	friend class ManagedProcessFailedDiscrepancy;
	friend class ManagedProcessFailedInvalid;
	friend class ManagedProcessFailedTimeout;
    
private:

    /**
     * The associated state machine.
     */
    ManagedProcessStateMachine* m_stateMachine;

    /**
     * The actual entity container.
     */
    PlatformMngt::EntityContainer* m_actualEntityContainer;

    /**
     * The actual service def container.
     */
    ServiceDefContainer_impl::ServiceDefContainer* m_actualServiceDefContainer;
    
    /**
     * The read/write service container of this process.
     */
    RWServiceDefContainer_impl* m_rwServiceDefContainer;
    
    
    /**
     * The RW Service Def Container object var to take pointer ownership
     */
    PortableServer::ServantBase_var m_rwServiceDefContainerServant;
    
    /**
     * The RW Entity Container object var to take pointer ownership
     */
    PortableServer::ServantBase_var m_rwEntityContainerServant;
    
    /**
     * The read/write entity container of this process.
     */
    RWEntityContainer_impl* m_rwEntityContainer;
    
    /**
     * The associated ProcessDelegateWrapper.
     */
    CdmwPlatformMngt::ProcessDelegateWrapper_var m_processDelegateWrap;
    
    /**
     * The step number realised.
     */
    CORBA::ULong m_step_nbr_done;
      
    /**
     * The entityDef to restore entity default values
     */
    CdmwPlatformMngt::EntityDefs* m_default_entitydefs;
  

    
    
    
    /**
     * The states of a managed process.
     */
    static ManagedProcessInitial* M_initial;
    static ManagedProcessInitialising* M_initialising;
    static ManagedProcessStepPerformed* M_stepPerformed;
    static ManagedProcessInitialised* M_initialised;
    static ManagedProcessRunRequest* M_runRequest;
    static ManagedProcessRunning* M_running;
    static ManagedProcessStopping* M_stopping;
    static ManagedProcessStopped* M_stopped;
    static ManagedProcessEnded* M_ended;
    static ManagedProcessFailedError* M_failedError;
    static ManagedProcessFailedMonitoringFailure* M_failedMonitoringFailure;
    static ManagedProcessFailedDeath* M_failedDeath;
    static ManagedProcessFailedDiscrepancy* M_failedDiscrepancy;
    static ManagedProcessFailedTimeout* M_failedTimeout;
    static ManagedProcessFailedInvalid* M_failedInvalid;

protected:

    /**
     *Purpose:
     *<p> Gets the process's states.
     * This method must exclusivaly be used by the state machine.
     */
    CdmwPlatformMngt::ProcessStatus getStatus()
        throw( CORBA::SystemException );

public:
    
    /**
     * The startup kind.
     */
    CdmwPlatformMngtBase::StartupKind m_startupKind;

    

private:

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    ManagedProcess_impl(
        const ManagedProcess_impl& rhs );

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    ManagedProcess_impl& operator=(
        const ManagedProcess_impl& rhs );
     
    /**
     *Purpose:
     *<p> Get the current state name of state machine
     *
     */    
    const char* get_state_machine_state_name();
    
    /**
     *Purpose:
     *<p> Set the current state name of state machine
     *
     */
    void set_state_machine_state_name(const char* state_name);
        
              

public:
    /**
     *Purpose:
     *<p> constructor.
     */
    ManagedProcess_impl(
        PortableServer::POA_ptr poa,
        CdmwPlatformMngt::ManagedProcessDef* process_def,
        const char* process_key, const char* process_path,
        Application_impl* application,
        CdmwPlatformMngt::RWElementRepository_ptr element_repository)
            throw(CORBA::SystemException);

    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual ~ManagedProcess_impl();
    
    
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
    void end_creation(CdmwPlatformMngt::ProcessDef* process_def)
            throw (CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateStep,
                   OutOfMemoryException,
         	   CosPropertyService::MultipleExceptions,
	           CORBA::SystemException);
                   
                   
    /**
     *Purpose:
     *<p> Synchronise with datastore
     *
     */
    void synchro_with_datastore();
    
    /**
     *Purpose:
     *<p> Synchronise the processing to initial state
     *
     */
    void synchro_processing_to_initial();
    
    /**
     *Purpose:
     *<p> Synchronise the processing to initialised state
     *
     */
    void synchro_processing_to_initialised();
    
    /**
     *Purpose:
     *<p> Synchronise the processing to running state
     *
     */
    void synchro_processing_to_running();
                   

public:
    /**
     *Purpose:
     *<p> Called when an error has occured.
     */
    void error_event(
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* issuer,
        const char* error_info );

    /**
     *Purpose:
     *<p> Called when the process has ended its running step.
     */
    void ending_event(
        const CdmwPlatformMngtBase::EventHeader& header );

    /**
     *Purpose:
     *<p> Called when a monitoring failure event has happened.
     *
     *@exception IncompatibleType if the process is not a managed process.
     */
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
    void invalidate_event(
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* reason );
        
    /**
     *Purpose:
     *<p> Get the internal status from state machine.
     */
     CdmwPlatformMngt::ProcessStatus get_internal_status();

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/embedded_service:1.0
     * operation
     */
    CORBA::Object_ptr embedded_service()
            throw( CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_status:1.0
     * operation
     */
    CdmwPlatformMngt::ProcessStatus get_status(
        CORBA::String_out status_info )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/status_info:1.0
     * operation
     */
    char* status_info();

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_auto_ending:1.0
     * operation
     */
    void set_auto_ending(CORBA::Boolean auto_ending)
        throw( CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_host:1.0
     * operation
     */
    void set_host(
        const char* host_name )
            throw( CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_process_info:1.0
     * operation
     */
    void set_process_info(
        const CdmwPlatformMngt::ProcessInfo& process_info )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     */
    CdmwPlatformMngt::LifeCycleDef* get_life_cycle()
        throw( CdmwPlatformMngt::IncompatibleType,
               CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_life_cycle:1.0
     * operation
     */
    void set_life_cycle(
        const CdmwPlatformMngt::LifeCycleDef& life_cycle )
            throw( CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::IncompatibleType,
                   CdmwPlatformMngt::IncompatibleStatus ,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_monitoring_info:1.0
     * operation
     */
    CdmwPlatformMngt::CompleteMonitoringInfo get_monitoring_info()
        throw( CdmwPlatformMngt::IncompatibleType,
               CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_monitoring_info:1.0
     * operation
     */
    void set_monitoring_info(
        const CdmwPlatformMngt::CompleteMonitoringInfo& monitoring_info)
            throw( CdmwPlatformMngt::IncompatibleType,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
     * operation
     */
    CdmwPlatformMngt::RequestStatus initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/next_step:1.0
     * operation
     */
    CdmwPlatformMngt::RequestStatus next_step()
        throw( CdmwPlatformMngt::InvalidStep,
               CdmwPlatformMngt::IncompatibleType,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
     * operation
     */
    CdmwPlatformMngt::RequestStatus run()
        throw( CdmwPlatformMngt::NotReadyToRun,
               CdmwPlatformMngt::IncompatibleType,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
     * operation
     */
    CdmwPlatformMngt::RequestStatus stop(
        CORBA::Boolean emergency )
            throw( CORBA::SystemException );
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
    CdmwPlatformMngt::Entity_ptr get_entity(const char* entity_name)
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
    CdmwPlatformMngt::Entity_ptr add_entity(
        const CdmwPlatformMngt::EntityDef & entity_def)
            throw( CdmwPlatformMngt::EntityAlreadyExists,
                   CORBA::SystemException );

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
     * Moves the state machine to specified state.
     */
    void setState(
        const char* stateName );

    /**
     *Purpose:
     *<p> Actually changes the lifecycle of the process.
     *
     *@param life_cycle the new lifecycle.
     *
     *@exception DuplicateStep if the definition of the lifecycle
     * has a duplicate step.
     */
    void setLifeCycle(
        const CdmwPlatformMngt::LifeCycleDef& life_cycle )
            throw( CdmwPlatformMngt::DuplicateStep,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p> Same as the method next_step except that it checks
     * the consistency of the step number.
     */
    CdmwPlatformMngt::RequestStatus nextStep(
        unsigned int targetStep )
    throw( CdmwPlatformMngt::InvalidStep,
           CdmwPlatformMngt::IncompatibleType,
           CdmwPlatformMngt::IncompatibleStatus,
           CORBA::SystemException );

    /**
     *Purpose:
     *<p> Creates a process status change event. This method
     * is called within the state machine.
     */
    void notifyStatusChangeEvent(
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* issuer );
        
        
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_create_process_result:1.0
     * operator
     */
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
     void set_initialisation_result (CdmwPlatformMngt::ProcessCommandResult result)
            throw (CORBA::SystemException);
     
     /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_next_step_result:1.0
     * operator
     */
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
     void set_run_result (CdmwPlatformMngt::ProcessCommandResult result)
            throw (CORBA::SystemException);
     
     /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_stop_result:1.0
     * operator
     */
     void set_stop_result (CdmwPlatformMngt::ProcessCommandResult result)
            throw (CORBA::SystemException);
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_MANAGED_PROCESS_PROXY_HPP

