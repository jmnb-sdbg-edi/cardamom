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


#ifndef INCL_PLATFORMMNGT_MANAGED_PROCESS_PROXY_HPP
#define INCL_PLATFORMMNGT_MANAGED_PROCESS_PROXY_HPP

#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "platformapplicationlibrary/ProcessProxy_impl.hpp"
#include "platformapplicationlibrary/ManagedProcessStates.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcess.stub.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class RWServiceDefContainer_impl;
class RWEntityContainer_impl;

/**
 *Purpose:
 *<p> The process proxy managing a managed process.
 */
class ManagedProcessProxy_impl : public virtual ProcessProxy_impl
{
    
private:

    /**
     * The associated state machine.
     */
    ManagedProcessStateMachine* m_stateMachine;

    /**
     * The actual entity container.
     */
    EntityContainer_impl::EntityContainer* m_actualEntityContainer;

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

protected:
    /**
     *Purpose:
     *<p> Gets the process proxy's states.
     * This method must exclusivaly be used by the state machine.
     */
    CdmwPlatformMngt::ProcessStatus getStatus()
        throw( CORBA::SystemException );

public:
    /**
     * The read/write entity container of this process.
     */
    RWEntityContainer_impl* m_rwEntityContainer;

    /**
     * The entity observer.
     */
    CdmwPlatformMngtEntity::EntityObserver_var m_entityObserver;

    /**
     * The services' brocker.
     */
    CdmwPlatformMngtService::ServiceBroker_var m_serviceBroker;

    /**
     * The associated OS process.
     */
    CdmwPlatformMngt::Process_var m_process;

    /**
     * The startup kind.
     */
    CdmwPlatformMngtBase::StartupKind m_startupKind;

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

private:

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    ManagedProcessProxy_impl(
        const ManagedProcessProxy_impl& rhs );

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    ManagedProcessProxy_impl& operator=(
        const ManagedProcessProxy_impl& rhs );

public:
    /**
     *Purpose:
     *<p> constructor.
     */
    ManagedProcessProxy_impl(
        PortableServer::POA_ptr poa,
        CdmwPlatformMngt::ManagedProcessDef* process_def,
        Application_impl* application )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual ~ManagedProcessProxy_impl();

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
    void invalidate_event(
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* reason );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/embedded_service:1.0
     * operation
     */
    CORBA::Object_ptr embedded_service()
            throw( CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/get_status:1.0
     * operation
     */
    CdmwPlatformMngt::ProcessStatus get_status(
        CORBA::String_out status_info )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/status_info:1.0
     * operation
     */
    char* status_info();

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/set_autoending:1.0
     * operation
     */
    void set_autoending()
        throw( CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/set_host:1.0
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/set_process_info:1.0
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/get_life_cycle:1.0
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/get_monitoring_info:1.0
     * operation
     */
    CdmwPlatformMngt::CompleteMonitoringInfo get_monitoring_info()
        throw( CdmwPlatformMngt::IncompatibleType,
               CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/set_monitoring_info:1.0
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/get_activity_point_defs:1.0
     * operation
     */
    CdmwPlatformMngt::ActivityPointDefs* get_activity_point_defs()
        throw( CdmwPlatformMngt::IncompatibleType,
               CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/set_activity_point_defs:1.0
     * operation
     */
    void set_activity_point_defs(
        const CdmwPlatformMngt::ActivityPointDefs& point_defs )
            throw( CdmwPlatformMngt::DuplicateActivityPoint,
                   CdmwPlatformMngt::IncompatibleType,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/initialise:1.0
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/next_step:1.0
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/run:1.0
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/stop:1.0
     * operation
     */
    CdmwPlatformMngt::RequestStatus stop(
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
     *<p> Actually changes the definition of activity points.
     *
     *@param point_defs the new activity point definitions of the process.
     *
     *@exception DuplicateActivityPoint if there is a duplicate name
     * among the definition of activity points.
     */
    void setActivityPointDefs(
        const CdmwPlatformMngt::ActivityPointDefs& point_defs )
            throw( CdmwPlatformMngt::DuplicateActivityPoint,
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
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_MANAGED_PROCESS_PROXY_HPP

