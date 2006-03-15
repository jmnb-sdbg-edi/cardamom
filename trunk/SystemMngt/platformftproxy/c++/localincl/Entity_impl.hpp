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


#ifndef INCL_PLATFORMMNGT_PROXY_ENTITY_IMPL_HPP
#define INCL_PLATFORMMNGT_PROXY_ENTITY_IMPL_HPP


#include <Foundation/orbsupport/OrbSupport.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtEntity.skel.hpp>

#include "platformftproxy/ProxyUtil.hpp"
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
 *<p> Provides the Entity.
 */
class Entity_impl : virtual public POA_CdmwPlatformMngt::Entity,
		     virtual public PortableServer::RefCountServantBase,
		    public ProxyUtil
                    
{

private:
   /**
     * Purpose: <p>
     *
     * The accessor interface provides all operations to be mapped onto.
     */
    CdmwFTSystMngt::EntityAccessor_var           m_accessor;

    /** Purpose: <p> 
     *
     * Protects access to  m_system_accessor */
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
     *
     * The lookup service 
     */ 
    AccessorLookupService *m_lookup_service;

private:
    
    /**
     * Purpose: <p> 
     *
     @return Fetches the accessor from lokoup service and returns the object reference
     *@exception CORBA::TRANSIENT Raised if accessor not defined. 
     */ 
    const CdmwFTSystMngt::EntityAccessor_ptr accessor() 
	throw(CORBA::TRANSIENT);
    
public:

    /**
     *Purpose:
     *<p> Constructor.
     *  
     *@param orb                The ORB 
     *@param m_proxy_collection The proxy collection provides operations for object reference creation
     *@param lookup_service     The lookup service provides access to accessor object references
     *
     *@exception BadParameterException One parameter is nil
     *@exception OutOfMemoryException Lack of memory.
     *
     */
     Entity_impl (CORBA::ORB_ptr orb,
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
     *
     */
     Entity_impl ()     
	 throw (Cdmw::BadParameterException, 
		Cdmw::OutOfMemoryException)   ;
                    
public:           

    /**
     *Purpose:
     *<p> Destructor.
     */
    ~Entity_impl() throw();


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
     * IDL:thalesgroup.com/CdmwPlatformMngt/Entity/get_status:1.0
     * operation
     */
    char* get_status(CORBA::String_out entity_info)
             throw (CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Entity/set_status:1.0
     * operation
     */
    void set_status(const char* entity_status, const char* entity_info)
             throw (CdmwPlatformMngt::EntityStatusNotAllowed, 
                    CORBA::SystemException);
        

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Entity/set_status_values:1.0
     * operation
     */
    CdmwPlatformMngt::EntityStatusValues* get_status_values()
             throw (CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Entity/get_snapshot:1.0
     * operation
     */
     CdmwPlatformMngt::EntitySnapshot* get_snapshot()
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
    void add_command (
        const char * command_name,
        ::CdmwPlatformMngt::Timeout time_out,
        const ::CdmwPlatformMngt::CmdParameterDescriptors & command_args
      )
        throw (CORBA::SystemException,
	       ::CdmwPlatformMngt::CommandAlreadyExists,
	       ::CdmwPlatformMngt::CommandArgumentAlreadyExists);
    
    // TAO_IDL - Generated from
    // be/be_visitor_operation/operation_ch.cpp:46
    
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
    void remove_command (
        const char * command_name
      )
	throw ( CORBA::SystemException,
		::CdmwPlatformMngt::CommandNotFound );

};

} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_ENTITY_IMPL_HPP

