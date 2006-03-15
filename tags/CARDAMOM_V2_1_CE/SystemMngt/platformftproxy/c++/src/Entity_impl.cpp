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


#include "platformftproxy/Entity_impl.hpp"

#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>


namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{


// ----------------------------------------------------------------------
// Entity_impl class.
// ----------------------------------------------------------------------


Entity_impl::Entity_impl (CORBA::ORB_ptr orb,
			  ProxyCollection *proxy_collection,
			  AccessorLookupService *lookup_service)
    throw (Cdmw::BadParameterException, 
	   Cdmw::OutOfMemoryException)                  
    : ProxyUtil(orb),
      m_accessor(CdmwFTSystMngt::EntityAccessor::_nil()),
      m_accessor_ok(false),
      m_proxy_collection(proxy_collection),
      m_lookup_service(lookup_service)
{
    m_proxy_collection->add_ref();
}

const CdmwFTSystMngt::EntityAccessor_ptr 
Entity_impl::accessor() 
    throw(CORBA::TRANSIENT) 
{
    if (!m_accessor_ok) {
	
	CDMW_MUTEX_GUARD(m_accessor_mutex);
	// leaving mutex other thread might have fetched the data
	// already

	try {

	    if (CORBA::is_nil(m_accessor.in())) {
		m_accessor = 
		    m_lookup_service->getEntityAccessor(); // raises AccessorUndefined
		// update the flag
		m_accessor_ok = ! CORBA::is_nil(m_accessor.in());
	    }

	}
	catch (...) {
	    throw CORBA::TRANSIENT(::Cdmw::OrbSupport::TRANSIENT,
				   CORBA::COMPLETED_NO);
	}
	
	// paranoid testing
	if (CORBA::is_nil(m_accessor.in())) {
	    throw CORBA::TRANSIENT(::Cdmw::OrbSupport::TRANSIENT,
				   CORBA::COMPLETED_NO);
	}

	// releasing mutex here
    }

    CDMW_ASSERT(m_accessor_ok &&
		! CORBA::is_nil(m_accessor.in()));
    

    return m_accessor.ptr(); // return const ptr
}


Entity_impl::~Entity_impl () throw ()
{
    m_proxy_collection->remove_ref();
}

void Entity_impl::get_attribute(const char * attr_name, CORBA::Any_out ret_attr, CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{


    try {
	std::string target = this->getObjectId();
	
	accessor()->get_attribute(target.c_str(), attr_name, ret_attr, flag);
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}

void Entity_impl::set_attribute(const char * attr_name, const CORBA::Any & attr, CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{


    try {
	std::string target = this->getObjectId();
	
	accessor()->set_attribute(target.c_str(), attr_name, attr, flag);
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}

/**
 *Purpose:
 *<p>
 * Implements the
 * IDL:thalesgroup.com/CdmwPlatformMngt/Entity/get_status:1.0
 * operation
 */
char* 
Entity_impl::get_status(CORBA::String_out entity_info)
    throw (CORBA::SystemException)
{
    try {
	std::string target = this->getObjectId();
	
	return accessor()->get_status(target.c_str(), entity_info);
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}

/**
 *Purpose:
 *<p>
 * Implements the
 * IDL:thalesgroup.com/CdmwPlatformMngt/Entity/set_status:1.0
 * operation
 */
void 
Entity_impl::set_status(const char* entity_status, const char* entity_info)
    throw (CdmwPlatformMngt::EntityStatusNotAllowed, 
                CORBA::SystemException)
{
    try {
	std::string target = this->getObjectId();
	
	accessor()->set_status(target.c_str(), entity_status, entity_info);
    }
    catch (const  CdmwPlatformMngt::EntityStatusNotAllowed & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}

    

/**
 *Purpose:
 *<p>
 * Implements the
 * IDL:thalesgroup.com/CdmwPlatformMngt/Entity/get_status_values:1.0
 * operation
 */
CdmwPlatformMngt::EntityStatusValues* 
Entity_impl::get_status_values()
    throw (CORBA::SystemException)
{
    try {
	std::string target = this->getObjectId();
	
	return accessor()->get_status_values(target.c_str());
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}


/**
 *Purpose:
 *<p>
 * Implements the
 * IDL:thalesgroup.com/CdmwPlatformMngt/Entity/get_snapshot:1.0
 * operation
 */
CdmwPlatformMngt::EntitySnapshot* 
Entity_impl::get_snapshot()
    throw (CORBA::SystemException)
{
    try {
	std::string target = this->getObjectId();
	
	return accessor()->get_snapshot(target.c_str());
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}
     
char* 
Entity_impl::name()
    throw (CORBA::SystemException)
{
    try {
	std::string target = this->getObjectId();
	
	return accessor()->get_name(target.c_str());
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}

          
                     
char* 
Entity_impl::element_path()
    throw (CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();

	// verify the target-object is accessible
	CORBA::String_var name = 
	    accessor()->get_name(target.c_str());
	
	return CORBA::string_dup(target.c_str());
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}

        
        
CdmwPlatformMngt::ManagedElement_ptr 
Entity_impl::get_parent()
    throw (CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();

	CORBA::String_var uid;

	CdmwPlatformMngt::ElementType type = 
	    accessor()->get_parent(target.c_str(), uid.out());
	
	return this->m_proxy_collection->createManagedElement(uid.in(), type);
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}

          
void 
Entity_impl::define_property(const char* property_name,
			     const CORBA::Any& property_value)
    throw (CosPropertyService::InvalidPropertyName,
	   CosPropertyService::ConflictingProperty,
	   CosPropertyService::UnsupportedTypeCode,
	   CosPropertyService::UnsupportedProperty,
	   CosPropertyService::ReadOnlyProperty,
	   CORBA::SystemException)
{
    try {   
	std::string target = this->getObjectId();
	
	accessor()->define_property(target.c_str(),
				    property_name,
				    property_value);
    }
    catch (const CosPropertyService::InvalidPropertyName & ex) {
	throw;
    }
    catch (const CosPropertyService::ConflictingProperty & ex) {
	throw;
    }
    catch (const CosPropertyService::UnsupportedTypeCode & ex) {
	throw;
    }
    catch (const CosPropertyService::UnsupportedProperty & ex) {
	throw;
    }
    catch (const CosPropertyService::ReadOnlyProperty & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}

                 
void 
Entity_impl::define_properties(const CosPropertyService::Properties & nproperties)
    throw (CosPropertyService::MultipleExceptions,
	   CORBA::SystemException)
{
    try {   
	std::string target = this->getObjectId();
	 
	accessor()->define_properties(target.c_str(),
				      nproperties);
    }
    catch (const CosPropertyService::MultipleExceptions & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}

 

CORBA::ULong 
Entity_impl::get_number_of_properties()
    throw (CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	 
	return accessor()->get_number_of_properties(target.c_str());	
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}

      
      
void 
Entity_impl::get_all_property_names (CORBA::ULong how_many,       
				     CosPropertyService::PropertyNames_out property_names,
				     CosPropertyService::PropertyNamesIterator_out rest)
    throw (CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	  
	accessor()->get_all_property_names(target.c_str(),
					   how_many,
					   property_names,
					   rest);	
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}

      
CORBA::Any* Entity_impl::get_property_value (const char* property_name)
    throw (CosPropertyService::PropertyNotFound,
	   CosPropertyService::InvalidPropertyName,
	   CORBA::SystemException)
{
    try {   
	std::string target = this->getObjectId();
	  
	return accessor()->get_property_value(target.c_str(), property_name);
    }
    catch (const CosPropertyService::PropertyNotFound & ex) {
	throw;
    }
    catch (const CosPropertyService::InvalidPropertyName & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}


CORBA::Boolean 
Entity_impl::get_properties (const CosPropertyService::PropertyNames& property_names,
			     CosPropertyService::Properties_out nproperties)
    throw (CORBA::SystemException)
{
    try {    
	std::string target = this->getObjectId();
	  
	return accessor()->get_properties(target.c_str(), 
					  property_names,
					  nproperties);
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}


void 
Entity_impl::get_all_properties (CORBA::ULong how_many,      
				 CosPropertyService::Properties_out nproperties,
				 CosPropertyService::PropertiesIterator_out rest)
    throw (CORBA::SystemException)
{
    try {
	std::string target = this->getObjectId();
	    
	accessor()->get_all_properties(target.c_str(),
				       how_many,
				       nproperties,
				       rest);
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}

      
void 
Entity_impl::delete_property(const char* property_name)
    throw (CosPropertyService::PropertyNotFound,
	   CosPropertyService::InvalidPropertyName,
	   CosPropertyService::FixedProperty,
	   CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	    
	accessor()->delete_property(target.c_str(), property_name);	
    }
    catch (const CosPropertyService::PropertyNotFound & ex) {
	throw;
    }
    catch (const CosPropertyService::InvalidPropertyName & ex) {
	throw;
    }
    catch (const CosPropertyService::FixedProperty & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}

    
void 
Entity_impl::delete_properties(const CosPropertyService::PropertyNames& property_names)
    throw (CosPropertyService::MultipleExceptions,
	   CORBA::SystemException)
{
    try {   
	std::string target = this->getObjectId();
	    
	accessor()->delete_properties(target.c_str(), 
				      property_names);
    }  
    catch (const CosPropertyService::MultipleExceptions & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}


CORBA::Boolean 
Entity_impl::delete_all_properties()
    throw (CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	    
	return accessor()->delete_all_properties(target.c_str());
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}

    
    
CORBA::Boolean 
Entity_impl::is_property_defined(const char* property_name)
    throw (CosPropertyService::InvalidPropertyName,
                 CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	    
	return accessor()->is_property_defined(target.c_str(),
					       property_name);
    }
    catch (const CosPropertyService::InvalidPropertyName & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}

    
    
void 
Entity_impl::get_allowed_property_types(CosPropertyService::PropertyTypes_out property_types)
    throw (CORBA::SystemException)
{
    try { 
	std::string target = this->getObjectId();
	     
	accessor()->get_allowed_property_types(target.c_str(),
					       property_types); 
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}

          
void
Entity_impl::get_allowed_properties(CosPropertyService::PropertyDefs_out property_defs)
    throw (CORBA::SystemException)
{ 
    try {  
	std::string target = this->getObjectId();
	     
	accessor()->get_allowed_properties(target.c_str(),
					   property_defs); 
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}


void 
Entity_impl::define_property_with_mode(const char* property_name,
				       const CORBA::Any& property_value,
				       CosPropertyService::PropertyModeType property_mode)
    throw (CosPropertyService::InvalidPropertyName,
	   CosPropertyService::ConflictingProperty,
	   CosPropertyService::UnsupportedTypeCode,
	   CosPropertyService::UnsupportedProperty,
	   CosPropertyService::UnsupportedMode,
	   CosPropertyService::ReadOnlyProperty,
	   CORBA::SystemException)
{

    try {   
	std::string target = this->getObjectId();
	    
	accessor()->define_property_with_mode(target.c_str(), 
					      property_name,
					      property_value,
					      property_mode); 	
    }
    catch (const CosPropertyService::InvalidPropertyName & ex) {
	throw;
    }
    catch (const CosPropertyService::ConflictingProperty & ex) {
	throw;
    }
    catch (const CosPropertyService::UnsupportedTypeCode & ex) {
	throw;
    }
    catch (const CosPropertyService::UnsupportedProperty & ex) {
	throw;
    }
    catch (const CosPropertyService::UnsupportedMode & ex) {
	throw;
    }
    catch (const CosPropertyService::ReadOnlyProperty & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}

    
void Entity_impl::define_properties_with_modes(const CosPropertyService::PropertyDefs & property_defs)
    throw (CosPropertyService::MultipleExceptions,
	   CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	    
	accessor()->define_properties_with_modes(target.c_str(), 
						 property_defs); 
    }
    catch (const CosPropertyService::MultipleExceptions & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}



CosPropertyService::PropertyModeType Entity_impl::get_property_mode(const char* property_name)
    throw (CosPropertyService::PropertyNotFound,
	   CosPropertyService::InvalidPropertyName,
	   CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	    
	return accessor()->get_property_mode(target.c_str(), 
					     property_name); 
    }
    catch (const CosPropertyService::PropertyNotFound & ex) {
	throw;
    }
    catch (const CosPropertyService::InvalidPropertyName & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}


CORBA::Boolean 
Entity_impl::get_property_modes(const CosPropertyService::PropertyNames& property_names,
				CosPropertyService::PropertyModes_out property_modes)
    throw (CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	    
	return accessor()->get_property_modes(target.c_str(), 
					      property_names,
					      property_modes); 
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}


void Entity_impl::set_property_mode(const char* property_name,
				    CosPropertyService::PropertyModeType property_mode)
    throw (CosPropertyService::InvalidPropertyName,
	   CosPropertyService::PropertyNotFound,
	   CosPropertyService::UnsupportedMode,
	   CORBA::SystemException)
{
    try { 
	std::string target = this->getObjectId();
	
	accessor()->set_property_mode(target.c_str(), 
				      property_name,
				      property_mode); 	
    }
    catch (const CosPropertyService::InvalidPropertyName & ex) {
	throw;
    }
    catch (const CosPropertyService::PropertyNotFound & ex) {
	throw;
    }
    catch (const CosPropertyService::UnsupportedMode & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}



void 
Entity_impl::set_property_modes(const CosPropertyService::PropertyModes & property_modes)
    throw (CosPropertyService::MultipleExceptions,
	   CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	accessor()->set_property_modes(target.c_str(),
				       property_modes); 
    }
    catch (const CosPropertyService::MultipleExceptions & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}

    

CdmwPlatformMngt::SupervisionObserver_ptr 
Entity_impl::register_all_properties_observer (const char* observer_name,
					       CdmwPlatformMngt::SupervisionObserver_ptr observer,
					       CORBA::Boolean deeply)   
    throw (CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->register_all_properties_observer(target.c_str(),
							    observer_name,
							    observer,
							    deeply); 	
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}


CdmwPlatformMngt::SupervisionObserver_ptr 
Entity_impl::unregister_all_properties_observer (const char* observer_name)
    throw (CdmwPlatformMngt::ObserverNotFound,
	   CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->unregister_all_properties_observer(target.c_str(),
							      observer_name);
    }
    catch (const CdmwPlatformMngt::ObserverNotFound & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}

   
   
void 
Entity_impl::register_property_observer (
					 const char* observer_name,
					 CdmwPlatformMngt::SupervisionObserver_ptr observer,
					 const CosPropertyService::PropertyNames& property_names)
    throw (CdmwPlatformMngt::PropertyInListNotFound,
	   CORBA::SystemException)
{
    try { 
	std::string target = this->getObjectId();
	 
	accessor()->register_property_observer(target.c_str(),
					       observer_name,
					       observer,
					       property_names);
    }
    catch (const CdmwPlatformMngt::PropertyInListNotFound & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}



void Entity_impl::unregister_property_observer (const char* observer_name,
						const CosPropertyService::PropertyNames& property_names)
    throw (CdmwPlatformMngt::PropertyInListNotFound,
	   CdmwPlatformMngt::PropertyObserverNotFound,
	   CORBA::SystemException)
{
    try { 
	std::string target = this->getObjectId();
	  
	accessor()->unregister_property_observer(target.c_str(),
						 observer_name,
						 property_names);
    }
    catch (const CdmwPlatformMngt::PropertyInListNotFound & ex) {
	throw;
    }    
    catch (const CdmwPlatformMngt::PropertyObserverNotFound & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}



::CdmwPlatformMngt::CommandStatus 
Entity_impl::get_command_status (CORBA::String_out status_info)
    throw (CORBA::SystemException)
{
   
//     try { 
// 	std::string target = this->getObjectId();
	  
// 	return accessor()->get_command_status(target.c_str(), 
// 					      status_info);
//     }
//     catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
// 	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
// 				      CORBA::COMPLETED_NO);
//     }
//     catch (const CORBA::SystemException & ex) {
// 	throw;
//     }
//     catch (...) {
// 	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
// 			      CORBA::COMPLETED_MAYBE);
//     }	
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);

}
   

void 
Entity_impl::call_command (const char * command_name,
	      const ::CdmwPlatformMngt::CmdParameterValues & command_args,
	      ::CdmwPlatformMngt::CommandResponseCallback_ptr response_callback
	      )
    throw  (CORBA::SystemException,
	    ::CdmwPlatformMngt::CommandNotFound,
	    ::CdmwPlatformMngt::CommandArgumentInvalid,
	    ::CdmwPlatformMngt::CommandRejected)
{
//     try { 
// 	std::string target = this->getObjectId();
	  
// 	accessor()->call_command(target.c_str(), 
// 				 command_name,
// 				 command_args, 
// 				 response_callback);
//     }
//     catch (const ::CdmwPlatformMngt::CommandNotFound & ex) {
// 	throw;
//     }  
//     catch (const ::CdmwPlatformMngt::CommandArgumentInvalid & ex) {
// 	throw;
//     }
//     catch (const ::CdmwPlatformMngt::CommandRejected & ex) {
// 	throw;
//     }
//     catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
// 	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
// 				      CORBA::COMPLETED_NO);
//     }
//     catch (const CORBA::SystemException & ex) {
// 	throw;
//     }
//     catch (...) {
// 	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
// 			      CORBA::COMPLETED_MAYBE);
//     }	

    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}
   


void 
Entity_impl::add_command (
        const char * command_name,
        ::CdmwPlatformMngt::Timeout time_out,
        const ::CdmwPlatformMngt::CmdParameterDescriptors & command_args
      )
        throw (CORBA::SystemException,
	       ::CdmwPlatformMngt::CommandAlreadyExists,
	       ::CdmwPlatformMngt::CommandArgumentAlreadyExists)
{  
//     try { 
// 	std::string target = this->getObjectId();
	  
// 	accessor()->add_command(target.c_str(), 
// 				command_name,
// 				time_out,
// 				command_args);
//     }
//     catch (const ::CdmwPlatformMngt::CommandArgumentAlreadyExists & ex) {
// 	throw;
//     }  
//     catch (const ::CdmwPlatformMngt::CommandAlreadyExists & ex) {
// 	throw;
//     }
//     catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
// 	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
// 				      CORBA::COMPLETED_NO);
//     }
//     catch (const CORBA::SystemException & ex) {
// 	throw;
//     }
//     catch (...) {
// 	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
// 			      CORBA::COMPLETED_MAYBE);
//     }	

    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);

}
   
    

void 
Entity_impl::remove_command (const char * command_name)
    throw ( CORBA::SystemException,
	    ::CdmwPlatformMngt::CommandNotFound )
{ 
//     try { 
// 	std::string target = this->getObjectId();
	  
// 	accessor()->remove_command(target.c_str(), 
// 				   command_name);
//     }
//     catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
// 	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
// 				      CORBA::COMPLETED_NO);
//     }
//     catch (const CORBA::SystemException & ex) {
// 	throw;
//     }
//     catch (...) {
// 	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
// 			      CORBA::COMPLETED_MAYBE);
//     }	

    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);

}
   


          
} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw


