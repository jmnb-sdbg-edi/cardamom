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


#ifndef INCL_PLATFORMMNGT_PROXY_PROCESS_HPP
#define INCL_PLATFORMMNGT_PROXY_PROCESS_HPP

#include <Foundation/common/Exception.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtProcess.skel.hpp>

#include "platformftproxy/ProxyUtil.hpp"
// #include "platformftproxy/System_impl.hpp"
#include "platformftproxy/AccessorLookupService.hpp"
#include "platformftproxy/ProxyCollection.hpp"

namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{

class System_impl;

/**
 *Purpose:
 *<p>
 */
class Process_impl : virtual public POA_CdmwPlatformMngt::Process,
		     virtual public PortableServer::RefCountServantBase,
                     public ProxyUtil
{
private:
    /**
     * Purpose: <p>
     *
     * The accessor interface provides all operations to be mapped onto.
     */
    CdmwFTSystMngt::ProcessAccessor_var          m_accessor;

    /** Purpose: <p> 
     *
     * Protects access to  m_system_accessor */
    Cdmw::OsSupport::Mutex                       m_accessor_mutex; 

    /** Purpose: <p> 
     *
     * True if m_system_target_path is valid obejct reference,
     * otherwise false */
    bool                                         m_accessor_ok;

    /**
     * Purpose: <p> 
     *
     * The proxy collection
     */ 
    Cdmw::PlatformMngt::Proxy::ProxyCollection *m_proxy_collection;

    /**
     * Purpose: <p> 
     */ 
    AccessorLookupService *m_lookup_service;

private:
        
    /**
     * Purpose: <p> 
     *
     @return Fetches the accessor from lokoup service and returns the object reference
     *@exception CORBA::TRANSIENT Raised if accessor not defined. 
     */ 
    const CdmwFTSystMngt::ProcessAccessor_ptr accessor() 
	throw(CORBA::TRANSIENT);
    
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

 
public:

    /**
     *Purpose:
     *<p> Constructor.
     *  
     *@param orb                The ORB 
     *@param m_proxy_collection The proxy collection provides operations for object reference creation
     *@param lookup_service     The lookup service provides access to accessor object references
     *
     *@exception BadParameterException One parameter is nil.
     *@exception OutOfMemoryException  Lack of memory.
     *
     */
    Process_impl (CORBA::ORB_ptr orb,
		  ProxyCollection *proxy_collection,
		  AccessorLookupService *lookup_service)
	throw (Cdmw::BadParameterException, 
	       Cdmw::OutOfMemoryException);

private:
     /**
     *Purpose:
     *<p> Default constructor is not allowed.
     *
     *@exception BadParameterException
     *@exception OutOfMemoryException
     */
     Process_impl ()     
          throw (Cdmw::BadParameterException,
		 Cdmw::OutOfMemoryException);
                    
public:           
  

    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual
    ~Process_impl() throw();

public:


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

     void get_attribute(const char * attr_name, CORBA::Any_out ret_attr, CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);

    /**
     *Purpose:
     *<p> facade to set an attribute
     *
     */

     void set_attribute(const char * attr_name, const CORBA::Any & attr, CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_status:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::ProcessStatus get_status(
        CORBA::String_out status_info)
            throw(CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_process_mode:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::ProcessMode get_process_mode()
            throw(CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_memory_resident:1.0
     * operation
     */
    virtual
    bool get_memory_resident()
            throw(CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_memory_resident:1.0
     * operation
     */
    virtual
    void set_memory_resident(bool memory_resident)
            throw(CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_core_dump_size:1.0
     * operation
     */
    virtual
    CORBA::Long get_core_dump_size()
            throw(CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_run_as_root:1.0
     * operation
     */
    virtual
    bool get_run_as_root()
            throw(CORBA::SystemException);




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
               CORBA::SystemException);

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
//     virtual
//     void set_host(const char* host_name)
//             throw (CdmwPlatformMngt::HostNotFound,
//                    CdmwPlatformMngt::IncompatibleStatus,
//                    CORBA::SystemException);

   /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_create_process_result:1.0
     * operation
     */
    virtual 
    void set_create_process_result (
	 ::CdmwPlatformMngt::ProcessCommandResult result,
	 const char * error_info,
	 const char * error_issuer,
	 ::CdmwPlatformMngt::ProcessDelegateWrapper_ptr process_delegate_wrap)
	throw (CORBA::SystemException);

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
                   CORBA::SystemException);

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
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
     * operation
     */
    virtual
    CdmwPlatformMngt::RequestStatus initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind)
            throw (CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);

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
      throw (CORBA::SystemException);
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/cold_restart:1.0
     * operation
     */  
    virtual CdmwPlatformMngt::RequestStatus cold_restart (
        const CdmwPlatformMngtBase::StartupKind & startup_kind)
      throw (CORBA::SystemException, CdmwPlatformMngt::IncompatibleStatus);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/hot_restart:1.0
     * operation
     */
    virtual CdmwPlatformMngt::RequestStatus hot_restart (
        const CdmwPlatformMngtBase::StartupKind & startup_kind)
      throw (CORBA::SystemException, CdmwPlatformMngt::IncompatibleStatus);
      
      
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
    void remove_all_service_defs ()
	 throw (CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PullMonitorable/is_alive:1.0
     * operation
     */
    virtual 
     void is_alive() throw (CORBA::SystemException);

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
        const char* result_info,
        CdmwPlatformMngt::ProcessDelegateWrapper_ptr process_delegate_wrap)
            throw (CORBA::SystemException);

     
     /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_initialisation_result:1.0
     * operator
     *
     * <p> Not iplemented - raises NO_IMPLEMENT exception. The
     * operation is for internal use in SystemMngt application only.
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
     *
     * <p> Not iplemented - raises NO_IMPLEMENT exception. The
     * operation is for internal use in SystemMngt application only.
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
     *
     * <p> Not iplemented - raises NO_IMPLEMENT exception. The
     * operation is for internal use in SystemMngt application only.
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
     *
     * <p> Not iplemented - raises NO_IMPLEMENT exception. The
     * operation is for internal use in SystemMngt application only.
     */
     virtual
     void set_stop_result (CdmwPlatformMngt::ProcessCommandResult result)
            throw (CORBA::SystemException);
        
    // ManagedObject operations
   
     
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ManagedElement/name:1.0
     * operation
     */
    virtual 
     char* name()
          throw (CORBA::SystemException);
          
                     
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ManagedElement/element_path:1.0
     * operation
     */
    virtual 
     char* element_path()
          throw (CORBA::SystemException);
        
        
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ManagedElement/get_parent:1.0
     * operation
     */   
     virtual 
    CdmwPlatformMngt::ManagedElement_ptr get_parent()
          throw (CORBA::SystemException);
          
     
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/define_property:1.0
     * operation
     */
     virtual 
    void define_property(const char* property_name,
                         const CORBA::Any& property_value)
          throw (CosPropertyService::InvalidPropertyName,
                 CosPropertyService::ConflictingProperty,
                 CosPropertyService::UnsupportedTypeCode,
                 CosPropertyService::UnsupportedProperty,
                 CosPropertyService::ReadOnlyProperty,
                 CORBA::SystemException);
                 
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/define_properties:1.0
     * operation
     */ 
    virtual 
     void define_properties(const CosPropertyService::Properties & nproperties)
          throw (CosPropertyService::MultipleExceptions,
                 CORBA::SystemException);
 

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/get_number_of_properties:1.0
     * operation
     */
    virtual 
     CORBA::ULong get_number_of_properties()
          throw (CORBA::SystemException);
      
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/get_all_property_names:1.0
     * operation
     */
    virtual 
     void get_all_property_names (CORBA::ULong how_many,       
                                 CosPropertyService::PropertyNames_out property_names,
                                 CosPropertyService::PropertyNamesIterator_out rest)
          throw (CORBA::SystemException);
      
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/get_property_value:1.0
     * operation
     */
    virtual 
     CORBA::Any* get_property_value (const char* property_name)
          throw (CosPropertyService::PropertyNotFound,
                 CosPropertyService::InvalidPropertyName,
                 CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet//get_properties:1.0
     * operation
     */
    virtual 
     CORBA::Boolean get_properties (
                          const CosPropertyService::PropertyNames& property_names,
                          CosPropertyService::Properties_out nproperties)
          throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/get_all_properties:1.0
     * operation
     */
     virtual 
    void get_all_properties (CORBA::ULong how_many,      
                             CosPropertyService::Properties_out nproperties,
                             CosPropertyService::PropertiesIterator_out rest)
          throw (CORBA::SystemException);
      
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/delete_property:1.0
     * operation
     */  
    virtual 
     void delete_property(const char* property_name)
          throw (CosPropertyService::PropertyNotFound,
                 CosPropertyService::InvalidPropertyName,
                 CosPropertyService::FixedProperty,
                 CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/delete_properties:1.0
     * operation
     */              
    virtual 
     void delete_properties(const CosPropertyService::PropertyNames& property_names)
          throw (CosPropertyService::MultipleExceptions,
                 CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/delete_all_properties:1.0
     * operation
     */
    virtual 
     CORBA::Boolean delete_all_properties()
          throw (CORBA::SystemException);
    
    
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/is_property_defined:1.0
     * operation
     */
    virtual 
     CORBA::Boolean is_property_defined(const char* property_name)
          throw (CosPropertyService::InvalidPropertyName,
                 CORBA::SystemException);
    
    
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/get_allowed_property_types:1.0
     * operation
     */
    virtual 
     void get_allowed_property_types(CosPropertyService::PropertyTypes_out property_types)
          throw (CORBA::SystemException);
          
          
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/get_allowed_properties:1.0
     * operation
     */
    virtual 
     void get_allowed_properties(CosPropertyService::PropertyDefs_out property_defs)
          throw (CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/define_property_with_mode:1.0
     * operation
     */
    virtual 
     void define_property_with_mode(const char* property_name,
                                   const CORBA::Any& property_value,
                                   CosPropertyService::PropertyModeType property_mode)
          throw (CosPropertyService::InvalidPropertyName,
                 CosPropertyService::ConflictingProperty,
                 CosPropertyService::UnsupportedTypeCode,
                 CosPropertyService::UnsupportedProperty,
                 CosPropertyService::UnsupportedMode,
                 CosPropertyService::ReadOnlyProperty,
                 CORBA::SystemException);
    

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/define_properties_with_mode:1.0
     * operation
     */
     virtual 
    void define_properties_with_modes(const CosPropertyService::PropertyDefs & property_defs)
          throw (CosPropertyService::MultipleExceptions,
                 CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/get_property_mode:1.0
     * operation
     */
    virtual 
     CosPropertyService::PropertyModeType get_property_mode(const char* property_name)
          throw (CosPropertyService::PropertyNotFound,
                 CosPropertyService::InvalidPropertyName,
                 CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/get_property_modes:1.0
     * operation
     */
    virtual 
     CORBA::Boolean get_property_modes(const CosPropertyService::PropertyNames& property_names,
                                      CosPropertyService::PropertyModes_out property_modes)
          throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/set_property_mode:1.0
     * operation
     */
    virtual 
     void set_property_mode(const char* property_name,
                           CosPropertyService::PropertyModeType property_mode)
          throw (CosPropertyService::InvalidPropertyName,
                 CosPropertyService::PropertyNotFound,
                 CosPropertyService::UnsupportedMode,
                 CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/set_property_modes:1.0
     * operation
     */
    virtual 
     void set_property_modes(const CosPropertyService::PropertyModes & property_modes)
          throw (CosPropertyService::MultipleExceptions,
                 CORBA::SystemException);
    

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyManager/register_all_properties_observer:1.0
     * operation
     */
    virtual 
     CdmwPlatformMngt::SupervisionObserver_ptr register_all_properties_observer (
            const char* observer_name,
            CdmwPlatformMngt::SupervisionObserver_ptr observer,
            CORBA::Boolean deeply)
            throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyManager/unregister_all_properties_observer:1.0
     * operation
     */
    virtual 
     CdmwPlatformMngt::SupervisionObserver_ptr unregister_all_properties_observer (
            const char* observer_name)
            throw (CdmwPlatformMngt::ObserverNotFound,
                   CORBA::SystemException);
   
   
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyManager/register_property_observer:1.0
     * operation
     */         
    virtual 
     void register_property_observer (
            const char* observer_name,
            CdmwPlatformMngt::SupervisionObserver_ptr observer,
            const CosPropertyService::PropertyNames& property_names)
            throw (CdmwPlatformMngt::PropertyInListNotFound,
                   CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyManager/unregister_property_observer:1.0
     * operation
     */ 
    virtual 
     void unregister_property_observer (
            const char* observer_name,
            const CosPropertyService::PropertyNames& property_names)
            throw (CdmwPlatformMngt::PropertyInListNotFound,
                   CdmwPlatformMngt::PropertyObserverNotFound,
                   CORBA::SystemException);
          
                                      
 

    //
    // CommandContainer operations
    //

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/CommandedElement/get_command_status:1.0
     * operation
     *
     * <p> Not iplemented - raises NO_IMPLEMENT exception. The
     * operation is declared for future use.
     */   
    virtual 
     ::CdmwPlatformMngt::CommandStatus get_command_status (
        CORBA::String_out status_info
      )
	throw (CORBA::SystemException);
    

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/CommandedElement/call_command:1.0
     * operation
     *
     * <p> Not iplemented - raises NO_IMPLEMENT exception. The
     * operation is declared for future use.
     */       
    virtual 
     void call_command (
        const char * command_name,
        const ::CdmwPlatformMngt::CmdParameterValues & command_args,
        ::CdmwPlatformMngt::CommandResponseCallback_ptr response_callback
      )
	throw  (CORBA::SystemException,
		::CdmwPlatformMngt::CommandNotFound,
		::CdmwPlatformMngt::CommandArgumentInvalid,
		::CdmwPlatformMngt::CommandRejected);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/CommandContainer/add_command:1.0
     * operation
     *
     * <p> Not iplemented - raises NO_IMPLEMENT exception. The
     * operation is declared for future use.
     */    
    virtual 
     void add_command (
        const char * command_name,
        ::CdmwPlatformMngt::Timeout time_out,
        const ::CdmwPlatformMngt::CmdParameterDescriptors & command_args
      )
        throw (CORBA::SystemException,
	       ::CdmwPlatformMngt::CommandAlreadyExists,
	       ::CdmwPlatformMngt::CommandArgumentAlreadyExists);
    
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/CommandContainer/remove_command:1.0
     * operation
     *
     * <p> Not iplemented - raises NO_IMPLEMENT exception. The
     * operation is declared for future use.
     */  
    virtual 
     void remove_command (
        const char * command_name
      )
	throw ( CORBA::SystemException,
		::CdmwPlatformMngt::CommandNotFound );


 

};

} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_PROCESS_HPP

