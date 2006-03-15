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


#ifndef INCL_PLATFORMMNGT_MANAGED_ELEMENT_IMPL_HPP
#define INCL_PLATFORMMNGT_MANAGED_ELEMENT_IMPL_HPP


#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtManagedElement.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtPropertyManager.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp"

#include "SystemMngt/platformelementlibrary/DataStoreDefinition.hpp"


#include <string>


#if CDMW_ORB_VDR == tao

#include "orbsvcs/orbsvcs/Property/CosPropertyService_i.h"
                                                                                                                  
#else
#error "Unsupported ORB"
#endif


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



/**
 *Purpose:
 *<p>
 */
	class ManagedElement_impl : virtual public POA_CdmwPlatformMngt::ManagedElement,
				    virtual public PortableServer::RefCountServantBase
	{

	public:


	    /**
	     *Purpose:
	     *<p> Constructor.
	     *
	     * @param element_repository the element repository object reference
	     *     
	     */
	    ManagedElement_impl (
		CdmwPlatformMngt::RWElementRepository_ptr element_repository)    
		throw ();

	    /**
	     *Purpose:
	     *<p> Default destructor.
	     */
	    virtual ~ManagedElement_impl();
    
   
      
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
	     *  Get the name of element
	     *
	     * @return the element name (!! do not desallocate !!)
	     */       
	    const char* get_element_name();
    
	    /**
	     *Purpose:
	     *<p>
	     *  Get the path of element
	     *
	     * @return the element path (!! do not desallocate !!)
	     */       
	    const char* get_element_path();
    
    
    
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
    
    
	    /**
	     * Purpose :
	     * <p>
	     * Implements the recovering of userdefined attributes default values
	     *
	     */
	    void recover_default_properties();
    

	    /*
	     * Purpose :
	     * <p>
	     * Implements the operation to store userdefined attributes in a class variable to allow correct restore of default values
	     * after restart of Supervise Object 
	     */
	    void store_default_properties();

	    
	    
	  
	    /**
	     * Purpose:
	     *<p>
	     *Implements the 
	     *
	     */


	protected:

	    /**
	     * The name of the element.
	     */
	    CdmwPlatformMngt::ElementName_var m_element_name;
    
	    /**
	     * The full name of the element.
	     */
	    CdmwPlatformMngt::ElementPath_var m_element_path;
    
	    /**
	     * The repository of the managed elements.
	     */
	    CdmwPlatformMngt::RWElementRepository_var m_element_repository;

   
	private:    
	    /*
	     * The PropertySet used for representing User Defined Attributes   
	     */
   
	    TAO_PropertySetDef* m_property_set;                     
	
	    CosPropertyService::PropertySetDef_var  m_property_set_var;	

	    /*
	     * The PropertyDefs var used as the container of default properties
	     */
	  
	    CosPropertyService::PropertyDefs* m_user_defined_attributes;	
                       
	protected:

	    /**
	     *Purpose:
	     *<p> Copy constructor is not allowed.
	     */
	    ManagedElement_impl (ManagedElement_impl& rhs)
		{};

	    /**
	     * Purpose:
	     * <p> Assignment operator is not allowed.
	     */ 
	    ManagedElement_impl& operator = (const ManagedElement_impl& rhs)

		{
		    return *this;
		};
    
    
	    /**
	     * Purpose:
	     * <p> Set the managed element full pathname
	     * 
	     * @param element_path         the full path name of element
	     * @param element_name         the name of element
	     *
	     */
	    void set_element_path (const char* element_path,   
				   const char* element_name);
                            
	    /**
	     *Purpose:
	     *<p> Set the managed element record in data store
	     *
	     */    
	    void set_element_in_datastore();
         

	    /**
	     *Purpose:
	     *<p> Get the managed element record from data store
	     *
	     */
	    CdmwPlatformMngt::ManagedElementRecord* get_managed_element_record_from_datastore()
		throw (Common::NotFoundException);


	    /**
	     *Purpose:
	     *<p> Synchronise with datastore
	     *
	     */
	    void synchro_with_datastore();

 
	private:

	    /*
	     * The datastore where m_element_name is stored with a m_element_path oid
	     */
	    ManagedElementDataStore* m_managed_element_ds;
    
       
	    /**
	     *Purpose:
	     *<p> Remove the host record in data store
	     *
	     */
	    void remove_element_in_datastore();
    
	};

    } // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_MANAGED_ELEMENT_IMPL_HPP

