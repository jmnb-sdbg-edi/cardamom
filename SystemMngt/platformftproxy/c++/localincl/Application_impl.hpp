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


#ifndef INCL_PLATFORMMNGT_PROXY_APPLICATION_IMPL_HPP
#define INCL_PLATFORMMNGT_PROXY_APPLICATION_IMPL_HPP

// #include <SystemMngt/platformapplicationlibrary/ApplicationStates.hpp>

#include <Foundation/common/Exception.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>

#include <SystemMngt/idllib/CdmwPlatformMngtApplication.skel.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtApplicationAgent.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtEntity.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtHost.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtService.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtDaemon.stub.hpp>

#include "platformftproxy/ProxyUtil.hpp"
#include "platformftproxy/AccessorLookupService.hpp"
#include "platformftproxy/ProxyCollection.hpp"

#include <string>
#include <set>
#include <map>

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
class Application_impl : public virtual POA_CdmwPlatformMngt::Application,
			 virtual public PortableServer::RefCountServantBase,
			 public ProxyUtil
{
private:
    /**
     * Purpose: <p>
     *
     * The accessor interface provides all operations to be mapped onto.
     */
    CdmwFTSystMngt::ApplicationAccessor_var m_accessor;

    /** Purpose: <p> protects access to  m_system_accessor */
    Cdmw::OsSupport::Mutex                       m_accessor_mutex; 

    /** Purpose: <p> True if m_system_target_path is valid obejct
	reference, otherwise false */
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
    const CdmwFTSystMngt::ApplicationAccessor_ptr accessor() 
	throw(CORBA::TRANSIENT);

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
     *<p> Default constructor is not allowed.
     *  
     *
     *@exception BadParameterException
     *@exception OutOfMemoryException
     *
     */
    Application_impl() 
	throw (Cdmw::BadParameterException, 
	       Cdmw::OutOfMemoryException);
    
public:
    /**
     *Purpose:
     *<p> Default constructor.
     *
     *@param orb                The ORB 
     *@param m_proxy_collection The proxy collection provides operations for object reference creation
     *@param lookup_service     The lookup service provides access to accessor object references
     *
     *@exception BadParameterException if the parameter is invalid.
     *@exception OutOfMemoryException Lack of memory.
     */
    Application_impl(CORBA::ORB_ptr orb,
		     ProxyCollection *proxy_collection,
		     AccessorLookupService *lookup_service)
	throw (Cdmw::BadParameterException,
	       Cdmw::OutOfMemoryException);


    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual ~Application_impl() throw ();


public:
    

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
     *<p> facade to get an attribute
     *
     */

     void get_attribute(const char * attr_name, CORBA::Any_out ret_attr , CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);

    /**
     *Purpose:
     *<p> facade to set an attribute
     *
     */

     void set_attribute(const char * attr_name, const CORBA::Any & attr,CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);


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
    CORBA::ULong get_number_of_processes() throw(CORBA::SystemException);
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
    virtual 
    void remove_all_service_defs ()
	 throw (CORBA::SystemException);
    


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PullMonitorable/is_alive:1.0
     * operation
     *
     * <p> Not iplemented - raises NO_IMPLEMENT exception. The
     * operation is deprecated.
     */
    void is_alive()
              throw(CORBA::SystemException );


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Observable/register_observer:1.0
     * operation
     *
     * <p> Not iplemented - raises NO_IMPLEMENT exception. The
     * operation is deprecated.
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
     *
     * <p> Not iplemented - raises NO_IMPLEMENT exception. The
     * operation is deprecated.
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
     *
     * <p> Not iplemented - raises NO_IMPLEMENT exception. The
     * operation is deprecated.
     */
    CdmwPlatformMngt::SupervisionObserver_ptr unregister_observer(
        const char* observer_name)
            throw (CdmwPlatformMngt::ObserverNotFound,
                   CORBA::SystemException);
                   
    // ManagedObject operations
   
     
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ManagedElement/name:1.0
     * operation
     */
    char* name()
          throw (CORBA::SystemException);
          
                     
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ManagedElement/element_path:1.0
     * operation
     */
    char* element_path()
          throw (CORBA::SystemException);
        
        
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ManagedElement/get_parent:1.0
     * operation
     */   
    CdmwPlatformMngt::ManagedElement_ptr get_parent()
          throw (CORBA::SystemException);
          
     
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/define_property:1.0
     * operation
     */
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
    CORBA::ULong get_number_of_properties()
          throw (CORBA::SystemException);
      
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/get_all_property_names:1.0
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
     * IDL:omg.org/CosPropertyService/PropertySet/get_property_value:1.0
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
     * IDL:omg.org/CosPropertyService/PropertySet//get_properties:1.0
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
     * IDL:omg.org/CosPropertyService/PropertySet/get_all_properties:1.0
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
     * IDL:omg.org/CosPropertyService/PropertySet/delete_property:1.0
     * operation
     */  
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
    CORBA::Boolean delete_all_properties()
          throw (CORBA::SystemException);
    
    
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/is_property_defined:1.0
     * operation
     */
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
    void get_allowed_property_types(CosPropertyService::PropertyTypes_out property_types)
          throw (CORBA::SystemException);
          
          
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/get_allowed_properties:1.0
     * operation
     */
    void get_allowed_properties(CosPropertyService::PropertyDefs_out property_defs)
          throw (CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/define_property_with_mode:1.0
     * operation
     */
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
    void unregister_property_observer (
            const char* observer_name,
            const CosPropertyService::PropertyNames& property_names)
            throw (CdmwPlatformMngt::PropertyInListNotFound,
                   CdmwPlatformMngt::PropertyObserverNotFound,
                   CORBA::SystemException);
          
                                      
 


    // ManagedObject operations end

};

} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_APPLICATION_IMPL_HPP

