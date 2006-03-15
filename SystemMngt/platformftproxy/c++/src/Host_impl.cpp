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


#include "platformftproxy/Host_impl.hpp"

#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "SystemMngt/platformlibrary/ManagedElementKey.hpp"
#include "SystemMngt/platformftaccessorlibrary/HostAccessor_impl.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"


namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{

// class Host_impl;
// class System_impl;



// ----------------------------------------------------------------------
// HostRespondingCallback class.
// ----------------------------------------------------------------------
// HostRespondingCallback::HostRespondingCallback (Host_impl* host)
//     : Callback()
// {
//     m_host = host;
// }


// HostRespondingCallback::~HostRespondingCallback ()
// {
// }


// // ----------------------------------------------------------------------
// // HostNotRespondingCallback class.
// // ----------------------------------------------------------------------
// HostNotRespondingCallback::HostNotRespondingCallback (Host_impl* host)
//     : Callback()
// {



// ----------------------------------------------------------------------
// Host_impl class.
// ----------------------------------------------------------------------


Host_impl::Host_impl (CORBA::ORB_ptr orb,
		      ProxyCollection *proxy_collection,
		      AccessorLookupService *lookup_service)
    throw (Cdmw::BadParameterException, 
	   Cdmw::OutOfMemoryException)                  
    : ProxyUtil(orb),
      m_accessor(CdmwFTSystMngt::HostAccessor::_nil()),
      m_accessor_ok(false),
      m_proxy_collection(proxy_collection),
      m_lookup_service(lookup_service)
{
    m_proxy_collection->add_ref();
}



const CdmwFTSystMngt::HostAccessor_ptr 
Host_impl::accessor() 
    throw(CORBA::TRANSIENT) 
{
    if (!m_accessor_ok) {
	
	CDMW_MUTEX_GUARD(m_accessor_mutex);
	// leaving mutex other thread might have fetched the data
	// already

	try {

	    if (CORBA::is_nil(m_accessor.in())) {
		m_accessor = 
		    m_lookup_service->getHostAccessor(); // raises AccessorUndefined
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

Host_impl::~Host_impl () throw ()
{  
    m_proxy_collection->remove_ref();
}

void Host_impl::set_attribute(const char * attr_name, const CORBA::Any & attr, CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
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


void Host_impl::get_attribute(const char * attr_name, CORBA::Any_out ret_attr, CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
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

CdmwPlatformMngt::HostStatus Host_impl::status()
throw( CORBA::SystemException )
{
    try {
	std::string target = this->getObjectId();
	
	return accessor()->get_status(target.c_str());
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


char * Host_impl::host_status_info()
throw( CORBA::SystemException )
{
	std::string target = this->getObjectId();
	
	return accessor()->get_host_status_info(target.c_str());
}
    

CdmwPlatformMngt::CompleteMonitoringInfo Host_impl::get_monitoring_parameters() throw (CORBA::SystemException)
{
std::string target = this->getObjectId();
return accessor()->get_monitoring_parameters(target.c_str()) ;
}

void Host_impl::set_monitoring_parameters(const CdmwPlatformMngt::CompleteMonitoringInfo& monitoring_parameters) throw (CORBA::SystemException)
{
std::string target = this->getObjectId();
accessor()->set_monitoring_parameters(monitoring_parameters, target.c_str()) ;
}
    
bool Host_impl::get_global_supervision_indicator()  throw (CORBA::SystemException)
{
std::string target = this->getObjectId();
return accessor()->get_global_supervision_indicator(target.c_str()) ;
}


void Host_impl::set_global_supervision_indicator( bool global_supervision_indicator)  throw (CORBA::SystemException)
{
std::string target = this->getObjectId();
accessor()->set_global_supervision_indicator(global_supervision_indicator, target.c_str()) ;
}

char * Host_impl::get_host_name()  throw (CORBA::SystemException)
{
std::string target = this->getObjectId();
return accessor()->get_host_name(target.c_str()) ;
}

CdmwPlatformMngt::Processes * Host_impl::get_processes (
                     CORBA::ULong how_many,
                     CdmwPlatformMngt::ProcessesIterator_out processes_iterator)
    throw (CORBA::SystemException)
{   
    try {
	std::string target = this->getObjectId();

	CdmwPlatformMngt::ElementPathsIterator_var iter;
	CdmwPlatformMngt::ElementPaths_var paths = 
	    accessor()->get_processes(target.c_str(), 
				      how_many,
				      iter.out());
	
	processes_iterator = 
	    this->m_proxy_collection->createProcessesIteratorRef(iter.ptr());
	
	return this->m_proxy_collection->createProcessesFromSeq(paths.ptr());
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

CdmwPlatformMngt::Processes * Host_impl::getprocesses () throw (CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

 CdmwPlatformMngt::Process_ptr Host_impl::get_process(const char *process_name, const char *application_name ) throw (CdmwPlatformMngt::ProcessNotFound, CORBA::SystemException)
   {
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

          
          
        
void Host_impl::shutdown ()
throw (CORBA::SystemException)
{  
    try {
	std::string target = this->getObjectId();

	accessor()->shutdown(target.c_str());
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


 
void Host_impl::cold_reboot ()
throw (CORBA::SystemException)
{ 
    try {
	std::string target = this->getObjectId();

	accessor()->cold_reboot(target.c_str());
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

               
void Host_impl::hot_reboot ()
throw (CORBA::SystemException)
{
    try {
	std::string target = this->getObjectId();

	accessor()->hot_reboot(target.c_str());
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

    

char* Host_impl::platform_application_exe()
throw( CORBA::SystemException )
{
    try {
	std::string target = this->getObjectId();

	return accessor()->get_platform_application_exe(target.c_str());
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


CORBA::ULong Host_impl::get_number_of_entities()
throw(CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	 
	return accessor()->get_number_of_entities(target.c_str());	
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

void Host_impl::get_all_entity_names(
    CORBA::ULong how_many,
    CdmwPlatformMngt::ElementNames_out entity_names,
    CdmwPlatformMngt::ElementNamesIterator_out rest)
throw(CORBA::SystemException)
{  
    try {  
	std::string target = this->getObjectId();
	  
	accessor()->get_all_entity_names(target.c_str(),
					 how_many,
					 entity_names,
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

CdmwPlatformMngt::Entity_ptr Host_impl::get_entity (
    const char* entity_name)
throw (CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	  
	CORBA::String_var entity_target =
	    accessor()->get_entity(target.c_str(), entity_name);
	
	return this->m_proxy_collection->createEntityRef(entity_target);
    }
    catch (const CdmwPlatformMngt::EntityNotFound & ex) {
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

CORBA::Boolean Host_impl::get_entities(
    const CdmwPlatformMngt::ElementNames& entity_names,
    CdmwPlatformMngt::Entities_out nentities)
throw(CORBA::SystemException)
{
    try {   
	std::string target = this->getObjectId();
	  
	::CdmwPlatformMngt::ElementPaths_var paths = 
	      new ::CdmwPlatformMngt::ElementPaths();
	
	CdmwPlatformMngt::ElementNames_var tmp_entity_names;

	CORBA::Boolean succeed = 
	    accessor()->get_entities(target.c_str(),
				     entity_names,
				     paths.out());

	if (succeed) {
	    nentities =
		this->m_proxy_collection->createEntitiesFromSeq(paths.ptr());
	}
		
	return succeed;
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

void Host_impl::get_all_entities(
    CORBA::ULong how_many,
    CdmwPlatformMngt::Entities_out nentities,
    CdmwPlatformMngt::EntitiesIterator_out rest)
throw(CORBA::SystemException)
{  
    try {  
	std::string target = this->getObjectId();
	  
	::CdmwPlatformMngt::ElementPaths_var         paths;
	::CdmwPlatformMngt::ElementPathsIterator_var iter;
	
	accessor()->get_all_entities(target.c_str(),
				     how_many,
				     paths.out(),
				     iter.out());
	
	CdmwPlatformMngt::Entities_var tmp_nentities = 
	    this->m_proxy_collection->createEntitiesFromSeq(paths.ptr());
	
	CdmwPlatformMngt::EntitiesIterator_var tmp_rest =
	    this->m_proxy_collection->createEntitiesIteratorRef(iter.in());
	
	nentities = tmp_nentities._retn();
	rest      = tmp_rest._retn();
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

    
CdmwPlatformMngt::Entity_ptr Host_impl::add_entity (
     const CdmwPlatformMngt::EntityDef & entity_def)
throw(CdmwPlatformMngt::EntityAlreadyExists,
      CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();

	CORBA::String_var uid = 
	    accessor()->add_entity(target.c_str(),
				   entity_def);
	
	return this->m_proxy_collection->createEntityRef(uid);
    }
    catch (const CdmwPlatformMngt::EntityAlreadyExists & ex) {
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

void Host_impl::remove_entity(
    const char* entity_name)
throw (CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{  
    try {  
	std::string target = this->getObjectId();

	accessor()->remove_entity(target.c_str(),
				  entity_name);
    }
    catch (const CdmwPlatformMngt::EntityNotFound & ex) {
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


char* 
Host_impl::name()
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
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}

          
                     
char* 
Host_impl::element_path()
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
Host_impl::get_parent()
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
Host_impl::define_property(const char* property_name,
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
Host_impl::define_properties(const CosPropertyService::Properties & nproperties)
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
Host_impl::get_number_of_properties()
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
Host_impl::get_all_property_names (CORBA::ULong how_many,       
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

      
CORBA::Any* Host_impl::get_property_value (const char* property_name)
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
Host_impl::get_properties (const CosPropertyService::PropertyNames& property_names,
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
Host_impl::get_all_properties (CORBA::ULong how_many,      
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
Host_impl::delete_property(const char* property_name)
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
Host_impl::delete_properties(const CosPropertyService::PropertyNames& property_names)
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
Host_impl::delete_all_properties()
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
Host_impl::is_property_defined(const char* property_name)
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
Host_impl::get_allowed_property_types(CosPropertyService::PropertyTypes_out property_types)
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
Host_impl::get_allowed_properties(CosPropertyService::PropertyDefs_out property_defs)
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
Host_impl::define_property_with_mode(const char* property_name,
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

    
void Host_impl::define_properties_with_modes(const CosPropertyService::PropertyDefs & property_defs)
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



CosPropertyService::PropertyModeType Host_impl::get_property_mode(const char* property_name)
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
Host_impl::get_property_modes(const CosPropertyService::PropertyNames& property_names,
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


void Host_impl::set_property_mode(const char* property_name,
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
Host_impl::set_property_modes(const CosPropertyService::PropertyModes & property_modes)
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
Host_impl::register_all_properties_observer (const char* observer_name,
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
Host_impl::unregister_all_properties_observer (const char* observer_name)
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
Host_impl::register_property_observer (
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



void Host_impl::unregister_property_observer (const char* observer_name,
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

          
} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw


