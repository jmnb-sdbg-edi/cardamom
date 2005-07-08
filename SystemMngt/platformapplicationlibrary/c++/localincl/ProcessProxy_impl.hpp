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


#ifndef INCL_PLATFORMMNGT_PROCESS_PROXY_HPP
#define INCL_PLATFORMMNGT_PROCESS_PROXY_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessProxy.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtServiceContainer.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtApplicationAgent.stub.hpp"
#include "SystemMngt/platformlibrary/DeactivableServant_impl.hpp"
#include "SystemMngt/platformlibrary/EntityContainer_impl.hpp"
#include "SystemMngt/platformlibrary/ServiceDefContainer_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"

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
class ProcessProxy_impl : virtual public DeactivableServant_impl,
                          virtual public POA_CdmwPlatformMngt::ProcessProxy,
                          virtual public PortableServer::RefCountServantBase
{
    
private:
    /**
     * Indicates whether the process is autoending or not.
     */
    CORBA::Boolean m_autoending;

    /**
     * The name of the process.
     */
    CdmwPlatformMngt::ProcessName_var m_name;

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
     * The host executing the process.
     */
    CdmwPlatformMngt::HostProxy_var m_host;

    /**
     * Tha process entity status change factory.
     */
    ProcessEntityStatusChangeFactory* m_procEntityStatusChangeFactory;

    /**
     * The process status change factory.
     */
    ProcessStatusChangeFactory* m_procStatusChangeFactory;

public:
    /**
     * The application managing this process.
     */
    Application_impl* m_application;

    /**
     * The life cycle of this process.
     */
    CdmwPlatformMngt::LifeCycleDef_var m_lifeCycle;

    /**
     * The monitoring info.
     */
    CdmwPlatformMngt::CompleteMonitoringInfo_var m_monitoring_info;

    /**
     * The activity points of this process.
     */
    CdmwPlatformMngt::ActivityPointDefs_var m_activity_point_defs;


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
    ProcessProxy_impl(const ProcessProxy_impl& rhs );

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    ProcessProxy_impl& operator=( const ProcessProxy_impl& rhs );

protected:
    /**
     *Purpose:
     *<p> constructor.
     */
    ProcessProxy_impl(
        PortableServer::POA_ptr poa,
        CdmwPlatformMngt::ProcessDef* process_def,
        Application_impl* application );

    /**
     *Purpose:
     *<p> Gets the process proxy's states.
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
    ~ProcessProxy_impl();

public:
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
     *<p> Called when a technical error has occured.
     *
     *@param error_info the information about the error.
     */
    virtual
    void invalidate_event(
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* reason ) = 0;

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/name:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::ProcessName name()
  throw( CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/type:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::ProcessType type()
  throw( CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/get_status:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::ProcessStatus get_status(
        CORBA::String_out status_info )
            throw( CORBA::SystemException ) = 0;

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/embedded_service:1.0
     * operation
     */
    virtual CORBA::Object_ptr embedded_service()
  throw( CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/is_autoending:1.0
     * operation
     */
    virtual
    CORBA::Boolean is_autoending()
      throw( CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/set_autoending:1.0
     * operation
     */
    virtual
    void set_autoending()
        throw( CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException ) = 0;

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/get_host:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::HostProxy_ptr get_host()
      throw( CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/set_host:1.0
     * operation
     */
    virtual
    void set_host(
        const char* host_name)
            throw( CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException ) = 0;

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/get_process_info:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::ProcessInfo* get_process_info()
      throw( CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/set_process_info:1.0
     * operation
     */
    virtual
    void set_process_info(
        const CdmwPlatformMngt::ProcessInfo& process_info )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException ) = 0;

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/get_life_cycle:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::LifeCycleDef* get_life_cycle()
            throw( CdmwPlatformMngt::IncompatibleType,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/set_life_cycle:1.0
     * operation
     */
    virtual
    void set_life_cycle(
        const CdmwPlatformMngt::LifeCycleDef& life_cycle )
            throw( CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::IncompatibleType,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/get_monitoring_info:1.0
     * operation
     */
    virtual
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
    virtual
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
    virtual
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
    virtual
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/get_snapshot:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::ProcessSnapshot* get_snapshot()
      throw( CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/initialise:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::RequestStatus initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException ) = 0;

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessProxy/next_step:1.0
     * operation
     */
    virtual
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
    virtual
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
    virtual
    CdmwPlatformMngt::RequestStatus stop(
        CORBA::Boolean emergency )
      throw( CORBA::SystemException ) = 0;
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngtEntity/EntityContainer/get_number_of_entities:1.0
     * operation
     */
    virtual
    CORBA::ULong get_number_of_entities()
        throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngtEntity/EntityContainer/get_all_entity_names:1.0
     * operation
     */
    virtual
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
    virtual
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
    virtual
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
    virtual
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
    virtual
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
    virtual
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
    virtual
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
    virtual
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
    virtual
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
      throw( CORBA::SystemException );
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
      throw( CORBA::SystemException );
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
      throw( CORBA::SystemException );
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
      throw( CORBA::SystemException );
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
      throw( CORBA::SystemException );
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
    void is_alive();

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ActivityPointContainer/get_number_of_activity_points:1.0
     * operation
     */
    CORBA::ULong get_number_of_activity_points()
      throw( CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ActivityPointContainer/get_all_activity_point_names:1.0
     * operation
     */
    void get_all_activity_point_names(
        CORBA::ULong how_many,
        CdmwPlatformMngt::ActivityPointNames_out point_names,
        CdmwPlatformMngt::ActivityPointNamesIterator_out rest )
      throw( CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ActivityPointContainer/get_activity_point_status:1.0
     * operation
     */
    CdmwPlatformMngt::ActivityPointStatus get_activity_point_status(
        const char* point_name )
            throw( CdmwPlatformMngt::ActivityPointNotFound,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ActivityPointContainer/get_activity_points:1.0
     * operation
     */
    CORBA::Boolean get_activity_points(
        const CdmwPlatformMngt::ActivityPointNames& point_names,
        CdmwPlatformMngt::ActivityPoints_out points )
      throw( CORBA::SystemException );
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ActivityPointContainer/get_all_activity_points:1.0
     * operation
     */
    void get_all_activity_points(
        CORBA::ULong how_many,
        CdmwPlatformMngt::ActivityPoints_out points,
        CdmwPlatformMngt::ActivityPointsIterator_out rest )
      throw( CORBA::SystemException );


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
        throw( CdmwPlatformMngt::HostNotFound,
               CdmwPlatformMngt::HostNotReachable,
               CdmwPlatformMngt::CreationError,
               CdmwPlatformMngt::ApplicationAgentAlreadyExists,
               CdmwPlatformMngt::InvalidInfo,
               CdmwPlatformMngt::CreationTimeout,
               AgentNotFoundException,
               CORBA::SystemException );


    
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
     *<p> Actually sets the process as ending itself.
     */
     
    void setAutoending();

    /**
     *Purpose:
     *<p> Actually changes the host where the process will be running.
     *
     *@param host_name the name of the host.
     *
     *@exception HostNotFound if the host where the process will be run is
     * not found.
     */
    void setHost(
        const char* host_name)
            throw( CdmwPlatformMngt::HostNotFound,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p> Actually changes the information needed for the process creation.
     *
     *@param process_info the information of the process.
     */
    virtual
    void setProcessInfo(
        const CdmwPlatformMngt::ProcessInfo& process_info );

    /**
     *Purpose:
     *<p> Actually changes the information about the monitoring of the process.
     *
     *@param monitoring_info the new information about the monitoring of the
     * process.
     */
    void setMonitoringInfo(
        const CdmwPlatformMngt::CompleteMonitoringInfo& monitoring_info );

    /**
     *Purpose:
     *<p> Called when an error has occured. This method is called within
     * the state machine.
     */
    void errorEvent(
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* issuer,
        const char* error_info );

    /**
     *Purpose:
     *<p> Called when the process has ended its running step. This method
     * is called within the state machine.
     */
    void endingEvent(
        const CdmwPlatformMngtBase::EventHeader& header );

    /**
     *Purpose:
     *<p> Called when a technical error has occured. This method is called
     * within the state machine.
     */
    void invalidateEvent(
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* reason );

    /**
     * Moves the state machine to specified state.
     */
    virtual
    void setState(
        const char* stateName ) = 0;
        
        
    /**
     *Purpose:
     *<p> return the process name
     *
     */
    const char* get_processName();
    
    
    
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_PROCESS_PROXY_HPP

