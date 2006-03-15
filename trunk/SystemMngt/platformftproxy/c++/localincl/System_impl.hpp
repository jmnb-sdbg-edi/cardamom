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


#ifndef INCL_PLATFORMMNGT_PROXY_SYSTEM_IMPL_HPP
#define INCL_PLATFORMMNGT_PROXY_SYSTEM_IMPL_HPP

#include "platformftproxy/ProxyUtil.hpp"
#include "platformftproxy/AccessorLookupService.hpp"
#include "platformftproxy/ProxyCollection.hpp"

#include <Foundation/osthreads/Mutex.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>

#include <SystemMngt/idllib/CdmwPlatformMngtSystem.skel.hpp>

#include <SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtApplicationBase.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtApplication.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtService.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtEntity.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtHost.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtProcessObserver.stub.hpp>
#include <SystemMngt/idllib/CdmwFTSystMngtSystem.stub.hpp>

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

namespace Proxy
{

    class AccessorLookupService;

/**
 *Purpose:
 *<p>
 */
class System_impl : virtual public POA_CdmwPlatformMngt::System,
		    virtual public PortableServer::RefCountServantBase,
                    public ProxyUtil
{
private:
    /**Purpose:  
     *<p> 
     *The proxy collection  */
    Cdmw::PlatformMngt::Proxy::ProxyCollection   *m_proxy_collection;

    /**Purpose:   
     *<p> 
     *The lookup service  */
    AccessorLookupService                        *m_lookup_service;

    /**Purpose: 
     *<p> 
     *System accessor object reference all operations are mapped
     *onto  */
    CdmwFTSystMngt::SystemAccessor_var           m_system_accessor; 

    /** Purpose:  
     *<p> 
     *Protects access to  m_system_accessor */
    Cdmw::OsSupport::Mutex                       m_system_accessor_mutex; 

    /**Purpose:  
     *<p> 
     *True if m_system_target_path is valid obejct reference,
     *otherwise false */
    bool                                         m_system_accessor_ok;

    /**Purpose:  
     *<p> 
     *Used as identifier for SystemAccessor requests on
     *EntityContainerAccessor operations, must match the name
     *specified in system.xml
     */
    ::CdmwPlatformMngt::ElementPath_var          m_system_target_path;

    /** Purpose: 
     *<p>
     * Protects access to m_system_target_path */
    Cdmw::OsSupport::Mutex                       m_system_target_path_mutex;
    
    /** Purpose: 
     *<p> 
     *True if m_system_target_path is valid string, otherwise false */
    bool                                         m_system_target_path_ok;
private:
    /**
     *Purpose:   
     *<p> 
<p> 
     *
     * Looks-up the system target path needed to access singleton
     * System object behind SystemAccessor interface. It throws
     * exception if the system_target_path has not been fetched and
     * SystemAccessor service is not accessible; otherwise it returns
     * the target path.
     */
    const char* lookupSystemTargetPath() 
	throw (CORBA::SystemException); 

    const CdmwFTSystMngt::SystemAccessor_ptr accessor() 
	throw(CORBA::TRANSIENT);

public:

    /**
     *Purpose:
     *<p> Constructor.
     *     
     *@param eventNotifier the supervision observer to be notified of events. 
     *@param orb           the orb reference
     *@param parent_poa    the poa in charge of servants  
     *@param proxy_name    the name identifies the proxy, it may be any string but must be unique within application. 
     *
     *@exception BadParameterException if the parameter is invalid.
     *@exception OutOfMemoryException Lack of ressource.
     */

    System_impl (CORBA::ORB_ptr orb,
		 ProxyCollection *proxy_collection,
		 AccessorLookupService *lookup_service) 
        throw (Cdmw::BadParameterException, 
	       Cdmw::OutOfMemoryException);
    

	/**
     *Purpose:
     *<p> Destructor.
     */
    virtual ~System_impl() throw();


public:
                   
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
     *<p> facade to get an attribute
     *
     */

     void get_attribute(const char * attr_name, CORBA::Any_out ret_attr , CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);

    /**
     *Purpose:
     *<p> facade to set an attribute
     *
     */

     void set_attribute(const char * attr_name, const CORBA::Any & attr, CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);


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
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/get_number_of_applications:1.0
     * operation
     */
    CORBA::ULong  get_number_of_applications()
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/RWServiceDefContainer/remove_all_service_defs:1.0
     * operation
     */               
    void remove_all_service_defs()
                    throw (CORBA::SystemException);




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
    
 
        
};

} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_PROXY_SYSTEM_IMPL_HPP

