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


#include "platformftproxy/System_impl.hpp"
#include "platformftproxy/Host_impl.hpp"
// #include "platformftproxy/EntitiesIterator_impl.hpp"

#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp>
#include <Foundation/common/Assert.hpp> 


namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{

const char* 
System_impl::lookupSystemTargetPath() 
	throw (CORBA::SystemException) 
{
    if (!m_system_target_path_ok) {

	CDMW_MUTEX_GUARD(m_system_target_path_mutex);
	// leaving mutex other thread might have fetched the data
	// already
	
	if (m_system_target_path.in()==NULL ||
	    strlen(m_system_target_path.in())==0) {

	    try {
		// might return NULL string to indicate error, only
		// throws CORBA::SystemException
		m_system_target_path = 
		    accessor()->get_system_element_path(); 
	    }
	    catch (const CORBA::SystemException &ex) {
		// eg. the accessor can not be resolved, etc
		throw;
	    }
	    catch (...) {
		// eg. out of memory
		throw CORBA::INTERNAL(::Cdmw::OrbSupport::INTERNAL,
				      CORBA::COMPLETED_NO);
	    }
	}
	
	// verify the return value and, update the flag
	m_system_target_path_ok = 
	    m_system_target_path.in()!=NULL  &&
	    strlen(m_system_target_path.in())>0;

	if ( ! m_system_target_path_ok) {
	    // systemMngt returned invalid value
	    throw CORBA::TRANSIENT(::Cdmw::OrbSupport::TRANSIENT,
				   CORBA::COMPLETED_NO);
	}

	// releasing mutex here
    }

    // m_system_target path is defined, readonly  
    
    return m_system_target_path.in();
}

const CdmwFTSystMngt::SystemAccessor_ptr 
System_impl::accessor() 
    throw(CORBA::TRANSIENT) 
{
    if ( ! m_system_accessor_ok) {
	
	CDMW_MUTEX_GUARD(m_system_accessor_mutex);
	// leaving mutex other thread might have fetched the data
	// already
	
	try {

	    if (CORBA::is_nil(m_system_accessor.in())) {
		m_system_accessor = 
		    m_lookup_service->getSystemAccessor(); // raises AccessorUndefined
		// update the flag
		m_system_accessor_ok = 
		    ( ! CORBA::is_nil(m_system_accessor.in()) );
	    }


	}
	catch (...) {
	    throw CORBA::TRANSIENT(::Cdmw::OrbSupport::TRANSIENT,
				   CORBA::COMPLETED_NO);
	}
	
	// verify the value returned is valid
	if (CORBA::is_nil(m_system_accessor.in())) {
	    throw CORBA::TRANSIENT(::Cdmw::OrbSupport::TRANSIENT,
				   CORBA::COMPLETED_NO);
	}
	
	// releasing mutex here
    }

    CDMW_ASSERT(m_system_accessor_ok &&
		! CORBA::is_nil(m_system_accessor.in()));

    return m_system_accessor.ptr(); // return const ptr
}
// ----------------------------------------------------------------------
// System_impl class.
// ----------------------------------------------------------------------

System_impl::System_impl (CORBA::ORB_ptr orb,
			  ProxyCollection *proxy_collection,
			  AccessorLookupService *lookup_service) 
    throw (Cdmw::BadParameterException, 
	   Cdmw::OutOfMemoryException) 
    : ProxyUtil(orb),
      m_proxy_collection(proxy_collection),
      m_lookup_service(lookup_service),
      m_system_accessor(CdmwFTSystMngt::SystemAccessor::_nil()),
      m_system_accessor_ok(false),
      m_system_target_path((const char*) 0),
      m_system_target_path_ok(false)
{        
    m_proxy_collection->add_ref();
}

System_impl::~System_impl() throw()
{
    m_proxy_collection->remove_ref();
}

void System_impl::get_attribute(const char * attr_name, CORBA::Any_out ret_attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{


    try {
	CdmwFTSystMngt::SystemAccessor_ptr acc = 
	    accessor();

	CDMW_ASSERT( ! CORBA::is_nil(acc));

	acc->get_attribute(NULL, attr_name , ret_attr, flag);
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
  }
}

void System_impl::set_attribute(const char * attr_name, const CORBA::Any & attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{


    try {
	CdmwFTSystMngt::SystemAccessor_ptr acc = 
	    accessor();

	CDMW_ASSERT( ! CORBA::is_nil(acc));

	acc->set_attribute(NULL, attr_name , attr, flag);
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
  }
}


CdmwPlatformMngt::SystemStatus 
System_impl::get_status(CORBA::String_out status_info)
    throw( CORBA::SystemException )
{
    try {
	CdmwFTSystMngt::SystemAccessor_ptr acc = 
	    accessor();

	CDMW_ASSERT( ! CORBA::is_nil(acc));

	return acc->get_status(status_info);
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}

//NEW function added on 15-7-2005 by Selex
char * System_impl::get_system_mission()  throw (CORBA::SystemException)
{

    try {
	CdmwFTSystMngt::SystemAccessor_ptr acc = 
	    accessor();

	CDMW_ASSERT( ! CORBA::is_nil(acc));

	return acc->get_system_mission();
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }

}


//NEW function added on 15-7-2005 by Selex
void System_impl::set_system_mission(  const char * system_mission)  throw (CORBA::SystemException)
{

    try {
	CdmwFTSystMngt::SystemAccessor_ptr acc = 
	    accessor();

	CDMW_ASSERT( ! CORBA::is_nil(acc));

	return acc->set_system_mission(system_mission);
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }

}


 CdmwPlatformMngt::FunctioningMode System_impl::get_mode(CORBA::String_out mode_info)
    throw( CORBA::SystemException )
{
    try {
        return accessor()->get_mode(mode_info);
    }
    catch (const  CORBA::SystemException & ex) {
        throw;
    }
    catch (...) {
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                              CORBA::COMPLETED_MAYBE);
    }   
}


//NEW function added on 15-7-2005 by Selex
void System_impl::set_system_name(  const char * system_name)  throw (CORBA::SystemException)
{

    try {
	CdmwFTSystMngt::SystemAccessor_ptr acc = accessor();

	CDMW_ASSERT( ! CORBA::is_nil(acc));

	return acc->set_system_name(system_name);
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }

}

 char * System_impl::get_system_name()
    throw( CORBA::SystemException )
{
    try {
	return accessor()->get_system_name();
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}



 char * System_impl::get_master_host()
    throw( CORBA::SystemException )
{
    try {
	return accessor()->get_master_host();
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}

CdmwPlatformMngt::SystemSnapshot* 
System_impl::get_snapshot()
    throw( CORBA::SystemException )
{
    try {
	return accessor()->get_snapshot();
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }		
}


CORBA::ULong System_impl::get_number_of_applications()
    throw( CORBA::SystemException )
{
    try {
	return accessor()->get_number_of_applications();
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }		
}


CdmwPlatformMngt::Applications* 
System_impl::get_applications 
       (CORBA::ULong how_many,
	CdmwPlatformMngt::ApplicationsIterator_out applications_iterator)
    throw( CORBA::SystemException )
{
    try {
	CdmwPlatformMngt::ElementPathsIterator_var elem_iter;
	
	CdmwPlatformMngt::ElementPaths_var paths = 
	    accessor()->get_applications(how_many, elem_iter.out());
	
	applications_iterator =
	    this->m_proxy_collection->createApplicationsIteratorRef(elem_iter.in());
	
	CdmwPlatformMngt::Applications_var apps_seq =
	    this->m_proxy_collection->createApplicationsFromSeq(paths.ptr());
	
	return apps_seq._retn();
	    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}





CdmwPlatformMngt::Application_ptr System_impl::get_application (
                          const char* application_name)
        throw (CdmwPlatformMngt::ApplicationNotFound,
               CORBA::SystemException)
{
    try {
	CORBA::String_var uid = 
	    accessor()->get_application(application_name);
	
	return this->m_proxy_collection->createApplicationRef(uid.in());
    }
    catch (const CdmwPlatformMngt::ApplicationNotFound &ex) {
	throw;
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}


CdmwPlatformMngt::SupervisionObserver_ptr 
       System_impl::register_observer(const char* observer_name,
                                      CdmwPlatformMngt::SupervisionObserver_ptr observer)
throw( CORBA::SystemException )
{
    try {
	return accessor()->register_observer(observer_name, observer);
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}

CdmwPlatformMngt::SupervisionObserver_ptr 
       System_impl::register_proxy_observer(const char* observer_name,
                                            const char* host_name,
                                            CdmwPlatformMngt::SupervisionObserver_ptr observer)
throw (CORBA::SystemException)
{
    try {
	return accessor()->register_proxy_observer(observer_name, host_name, observer);
	    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}

CdmwPlatformMngt::SupervisionObserver_ptr 
       System_impl::unregister_observer(const char* observer_name)
        throw (CdmwPlatformMngt::ObserverNotFound,
               CORBA::SystemException)
{
    try {
	return accessor()->unregister_observer(observer_name);
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}


void System_impl::define(const CdmwPlatformMngt::SystemDef& system_def)
        throw (CdmwPlatformMngt::DuplicateEntity,
               CosPropertyService::MultipleExceptions,
               CdmwPlatformMngt::DuplicateService,
               CdmwPlatformMngt::DuplicateHost,
               CdmwPlatformMngt::DuplicateHostEntity,
               CdmwPlatformMngt::HostPropertyExceptions,
               CdmwPlatformMngt::AlreadyDone,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    try {
	accessor()->define(system_def);
    }
    catch (const CdmwPlatformMngt::DuplicateEntity & ex) {
	throw;
    }    
    catch (const CdmwPlatformMngt::DuplicateService & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::DuplicateHost & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::AlreadyDone & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
	throw;
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}


void System_impl::set_init_graph (const CdmwPlatformMngt::GraphElements& application_graph)                  
        throw (CdmwPlatformMngt::ReferenceNotFound,
               CdmwPlatformMngt::CircularReference,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    try {
	accessor()->set_init_graph(application_graph);
    }
    catch (const CdmwPlatformMngt::ReferenceNotFound & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::CircularReference & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
	throw;
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}


void System_impl::set_stop_graph (const CdmwPlatformMngt::GraphElements& application_graph)                  
        throw (CdmwPlatformMngt::ReferenceNotFound,
               CdmwPlatformMngt::CircularReference,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    try {
	accessor()->set_stop_graph(application_graph);
    } 
    catch (const CdmwPlatformMngt::ReferenceNotFound & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::CircularReference & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
	throw;
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
			      CORBA::COMPLETED_MAYBE);
    }	
}



void System_impl::set_run_set(const CdmwPlatformMngt::RunSetDef& run_set_def)
        throw (CdmwPlatformMngt::DuplicateRunSetElement,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    try {
	accessor()->set_run_set(run_set_def);
    }
    catch (const CdmwPlatformMngt::DuplicateRunSetElement & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
	throw;
    }
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }		
}

void System_impl::remove_run_set(const char* run_set_name)
        throw (CdmwPlatformMngt::RunSetNotFound,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    try {
	accessor()->remove_run_set(run_set_name);
    } 
    catch (const CdmwPlatformMngt::RunSetNotFound & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
	throw;
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}

void System_impl::remove_all_run_sets()
        throw (CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    try {
	accessor()->remove_all_run_sets();
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
	throw;
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
			      CORBA::COMPLETED_MAYBE);
    }		
}



CdmwPlatformMngt::Host_ptr 
      System_impl::add_host(const CdmwPlatformMngt::HostDef& host_def)
        throw (CdmwPlatformMngt::HostAlreadyExists,
               CdmwPlatformMngt::DuplicateEntity, 
               CosPropertyService::MultipleExceptions,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    try {
	CORBA::String_var host_id = 
	    accessor()->add_host(host_def);
	
	return m_proxy_collection->createHostRef(host_id.in());
    }  
    catch (const CdmwPlatformMngt::HostAlreadyExists & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
	throw;
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}

void System_impl::remove_host(const char* host_name)
        throw (CdmwPlatformMngt::HostNotFound,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    try {
	accessor()->remove_host(host_name);
    } 
    catch (const CdmwPlatformMngt::HostNotFound & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
	throw;
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
    
}                  


CdmwPlatformMngt::Application_ptr 
      System_impl::add_application(const CdmwPlatformMngt::PlatformApplicationDef& platform_application_def,
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
               CORBA::SystemException)
{
    try {
	CORBA::String_var uid = 
	    accessor()->add_application(platform_application_def,
					       application_def);	
	
	return m_proxy_collection->createApplicationRef(uid.in());
    } 
    catch (const CdmwPlatformMngt::ApplicationAlreadyExists & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::HostNotFound & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::DuplicateEntity & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::DuplicateProcess & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::ProcessHostNotFound & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::DuplicateProcessEntity & ex) {
	throw;
    }    
    catch (const CdmwPlatformMngt::DuplicateProcessService & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::DuplicateProcessStep & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
	throw;
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}

void System_impl::remove_application(const char* application_name)
        throw (CdmwPlatformMngt::ApplicationNotFound,
               CdmwPlatformMngt::ApplicationStillActive,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    try {
	accessor()->remove_application(application_name);
    } 
    catch (const CdmwPlatformMngt::ApplicationNotFound & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::ApplicationStillActive & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
	throw;
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
} 

void System_impl::start (const CdmwPlatformMngtBase::StartupKind& startup_kind)      
        throw (CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    try {
	accessor()->start(startup_kind);
    } 
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
	throw;
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}
    

void System_impl::resume()
        throw (CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    try {
	accessor()->resume();
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
	throw;
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}


void System_impl::acknowledge()
        throw (CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    try {
	accessor()->acknowledge();
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
	throw;
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}


void System_impl::stop (CORBA::Boolean emergency)
         throw( CORBA::SystemException )      
{
    try {
	accessor()->stop(emergency);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}


void System_impl::cold_restart (const CdmwPlatformMngtBase::StartupKind& startup_kind)      
throw (CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
    try {
	accessor()->cold_restart(startup_kind);
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
	throw;
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
System_impl::hot_restart (const CdmwPlatformMngtBase::StartupKind& startup_kind)      
throw (CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
    try {
	accessor()->hot_restart(startup_kind);
    } 
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
	throw;
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
System_impl::get_number_of_entities()
throw( CORBA::SystemException )
{
    try {
	return accessor()->get_number_of_entities(lookupSystemTargetPath()); 
    } 
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}

void 
System_impl::get_all_entity_names (
                         CORBA::ULong how_many,
                         CdmwPlatformMngt::ElementNames_out entity_names,
                         CdmwPlatformMngt::ElementNamesIterator_out rest)
throw( CORBA::SystemException )
{
    try {
	accessor()->get_all_entity_names(lookupSystemTargetPath(),
						how_many,
						entity_names,
						rest);
    } 
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }	
}



CdmwPlatformMngt::Entity_ptr 
System_impl::get_entity (const char* entity_name)
throw (CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{  
    try {  
	CORBA::String_var uid = 
	    accessor()->get_entity(lookupSystemTargetPath(),
					  entity_name);
	
	return m_proxy_collection->createEntityRef(uid);
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

CORBA::Boolean System_impl::get_entities (
                        const CdmwPlatformMngt::ElementNames& entity_names,
                        CdmwPlatformMngt::Entities_out nentities)
throw( CORBA::SystemException )
{
    try {  
	::CdmwPlatformMngt::ElementPaths_var paths = 
	      new ::CdmwPlatformMngt::ElementPaths();

	CORBA::Boolean succeed = 
	    accessor()->get_entities(lookupSystemTargetPath(),
					    entity_names,
					    paths.out());
	if (succeed) {
	    nentities = m_proxy_collection->createEntitiesFromSeq(paths.ptr());
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


void 
System_impl::get_all_entities (
			       CORBA::ULong how_many,
			       CdmwPlatformMngt::Entities_out nentities,
			       CdmwPlatformMngt::EntitiesIterator_out rest)
    throw( CORBA::SystemException )
{
    try {  
	::CdmwPlatformMngt::ElementPaths_var         paths;
	::CdmwPlatformMngt::ElementPathsIterator_var iter;
	
	accessor()->get_all_entities(lookupSystemTargetPath(),
					    how_many,
					    paths.out(),
					    iter.out());
	
	CdmwPlatformMngt::Entities_var tmp_nentities = 
	    m_proxy_collection->createEntitiesFromSeq(paths.ptr());
	
	CdmwPlatformMngt::EntitiesIterator_var tmp_rest =
	    m_proxy_collection->createEntitiesIteratorRef(iter.in());
	
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


CdmwPlatformMngt::Entity_ptr 
System_impl::add_entity (const CdmwPlatformMngt::EntityDef & entity_def)
    throw(CdmwPlatformMngt::EntityAlreadyExists,
	  CORBA::SystemException)
{
    try {  
	CORBA::String_var uid = 
	    accessor()->add_entity(lookupSystemTargetPath(),
					  entity_def);
	
	return m_proxy_collection->createEntityRef(uid);
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


void 
System_impl::remove_entity (const char* entity_name)    
       throw (CdmwPlatformMngt::EntityNotFound,
              CORBA::SystemException)
{  
    try {  
	accessor()->remove_entity(lookupSystemTargetPath(),
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



void 
System_impl::remove_all_service_defs()
    throw (CORBA::SystemException)
{ 
    try {  
	accessor()->remove_all_service_defs(lookupSystemTargetPath());
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


CdmwPlatformMngtService::ServiceProviderID* 
System_impl::get_service_def (
                        const char* service_name)
       throw (CdmwPlatformMngtService::ServiceNotFound,
              CORBA::SystemException)
{
    try {  
	return accessor()->get_service_def (lookupSystemTargetPath(), service_name);
    }
    catch (const CdmwPlatformMngtService::ServiceNotFound & ex) {
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

void System_impl::add_service_def (const CdmwPlatformMngtService::ServiceDef& service_def)  
       throw (CdmwPlatformMngtService::ServiceAlreadyExists,
              CORBA::SystemException)
{
    try {  
	accessor()->add_service_def(lookupSystemTargetPath(), service_def);
    }
    catch (const CdmwPlatformMngtService::ServiceAlreadyExists & ex) {
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

void System_impl::remove_service_def (const char* service_name)  
       throw (CdmwPlatformMngtService::ServiceNotFound,
              CORBA::SystemException)
{
   try {  
       accessor()->remove_service_def(lookupSystemTargetPath(), service_name);
    }
    catch (const CdmwPlatformMngtService::ServiceNotFound & ex) {
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




CORBA::ULong System_impl::get_number_of_hosts()
throw( CORBA::SystemException )
{
    try {  
	return accessor()->get_number_of_hosts(lookupSystemTargetPath());
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



void System_impl::get_all_host_names (CORBA::ULong how_many,
                                      CdmwPlatformMngt::ElementNames_out host_names,
                                      CdmwPlatformMngt::ElementNamesIterator_out rest)
throw( CORBA::SystemException )
{
    try {  
	accessor()->get_all_host_names (lookupSystemTargetPath(),
					       how_many,
					       host_names,
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



CdmwPlatformMngt::Host_ptr System_impl::get_host (const char* host_name)
        throw (CdmwPlatformMngt::HostNotFound,
               CORBA::SystemException)
{
    try {  
	CORBA::String_var uid = 
	    accessor()->get_host (lookupSystemTargetPath(),
					 host_name);
	
	return m_proxy_collection->createHostRef(uid);
    }
    catch (const CdmwPlatformMngt::HostNotFound & ex) {
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



CORBA::Boolean System_impl::get_hosts (const CdmwPlatformMngt::ElementNames& host_names,
                                       CdmwPlatformMngt::Hosts_out hosts)
throw( CORBA::SystemException )
{
    try {  
	CdmwPlatformMngt::ElementPaths_var paths;

	CORBA::Boolean succeed =
	    accessor()->get_hosts(lookupSystemTargetPath(), 
					 host_names, 
					 paths.out());
	
	if (succeed) {
	    hosts = m_proxy_collection->createHostsFromSeq(paths.ptr());
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



void System_impl::get_all_hosts (CORBA::ULong how_many,
                                 CdmwPlatformMngt::Hosts_out hosts,
                                 CdmwPlatformMngt::HostsIterator_out rest)
throw( CORBA::SystemException )
{
    try {  
	CdmwPlatformMngt::ElementPaths_var paths;
	CdmwPlatformMngt::ElementPathsIterator_var iter;

	accessor()->get_all_hosts(lookupSystemTargetPath(), 
				  how_many,
				  paths.out(),
				  iter.out());
	
	hosts = m_proxy_collection->createHostsFromSeq(paths.ptr());
	rest  = m_proxy_collection->createHostsIteratorRef(iter.in());
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
System_impl::name()
    throw (CORBA::SystemException)
{
    try {  
	return accessor()->get_name(lookupSystemTargetPath());
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
System_impl::element_path()
    throw (CORBA::SystemException)
{
    try {  
	std::string target = lookupSystemTargetPath();

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
System_impl::get_parent()
    throw (CORBA::SystemException)
{
    try {  
	CORBA::String_var uid;

	CdmwPlatformMngt::ElementType type = 
	    accessor()->get_parent(lookupSystemTargetPath(), uid.out());
	
	return m_proxy_collection->createManagedElement(uid.in(), type);
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
System_impl::define_property(const char* property_name,
			     const CORBA::Any& property_value)
    throw (CosPropertyService::InvalidPropertyName,
	   CosPropertyService::ConflictingProperty,
	   CosPropertyService::UnsupportedTypeCode,
	   CosPropertyService::UnsupportedProperty,
	   CosPropertyService::ReadOnlyProperty,
	   CORBA::SystemException)
{
    try {  
	accessor()->define_property(lookupSystemTargetPath(),
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
System_impl::define_properties(const CosPropertyService::Properties & nproperties)
    throw (CosPropertyService::MultipleExceptions,
	   CORBA::SystemException)
{
    try {  
	accessor()->define_properties(lookupSystemTargetPath(),
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
System_impl::get_number_of_properties()
    throw (CORBA::SystemException)
{
    try {  
	return accessor()->get_number_of_properties(lookupSystemTargetPath());	
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
System_impl::get_all_property_names (CORBA::ULong how_many,       
				     CosPropertyService::PropertyNames_out property_names,
				     CosPropertyService::PropertyNamesIterator_out rest)
    throw (CORBA::SystemException)
{
    try {  
	accessor()->get_all_property_names(lookupSystemTargetPath(),
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

      
CORBA::Any* System_impl::get_property_value (const char* property_name)
    throw (CosPropertyService::PropertyNotFound,
	   CosPropertyService::InvalidPropertyName,
	   CORBA::SystemException)
{
    try {  
	return accessor()->get_property_value(lookupSystemTargetPath(), property_name);
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
System_impl::get_properties (const CosPropertyService::PropertyNames& property_names,
			     CosPropertyService::Properties_out nproperties)
    throw (CORBA::SystemException)
{
    try {  
	return accessor()->get_properties(lookupSystemTargetPath(), 
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
System_impl::get_all_properties (CORBA::ULong how_many,      
				 CosPropertyService::Properties_out nproperties,
				 CosPropertyService::PropertiesIterator_out rest)
    throw (CORBA::SystemException)
{
    try {  
	accessor()->get_all_properties(lookupSystemTargetPath(),
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
System_impl::delete_property(const char* property_name)
    throw (CosPropertyService::PropertyNotFound,
	   CosPropertyService::InvalidPropertyName,
	   CosPropertyService::FixedProperty,
	   CORBA::SystemException)
{
    try {  
	accessor()->delete_property(lookupSystemTargetPath(), property_name);	
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
System_impl::delete_properties(const CosPropertyService::PropertyNames& property_names)
    throw (CosPropertyService::MultipleExceptions,
	   CORBA::SystemException)
{
    try {  
	accessor()->delete_properties(lookupSystemTargetPath(), 
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
System_impl::delete_all_properties()
    throw (CORBA::SystemException)
{
    try {  
	return accessor()->delete_all_properties(lookupSystemTargetPath());
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
System_impl::is_property_defined(const char* property_name)
    throw (CosPropertyService::InvalidPropertyName,
	   CORBA::SystemException)
{
    try {  
	return accessor()->is_property_defined(lookupSystemTargetPath(),
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
System_impl::get_allowed_property_types(CosPropertyService::PropertyTypes_out property_types)
    throw (CORBA::SystemException)
{
    try {  
	accessor()->get_allowed_property_types(lookupSystemTargetPath(),
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
System_impl::get_allowed_properties(CosPropertyService::PropertyDefs_out property_defs)
    throw (CORBA::SystemException)
{
    try {  
	accessor()->get_allowed_properties(lookupSystemTargetPath(),
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
System_impl::define_property_with_mode(const char* property_name,
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
	accessor()->define_property_with_mode(lookupSystemTargetPath(), 
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

    
void System_impl::define_properties_with_modes(const CosPropertyService::PropertyDefs & property_defs)
    throw (CosPropertyService::MultipleExceptions,
	   CORBA::SystemException)
{
    try {  
	accessor()->define_properties_with_modes(lookupSystemTargetPath(), 
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



CosPropertyService::PropertyModeType System_impl::get_property_mode(const char* property_name)
    throw (CosPropertyService::PropertyNotFound,
	   CosPropertyService::InvalidPropertyName,
	   CORBA::SystemException)
{
    try {  
	return accessor()->get_property_mode(lookupSystemTargetPath(), 
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
System_impl::get_property_modes(const CosPropertyService::PropertyNames& property_names,
				CosPropertyService::PropertyModes_out property_modes)
    throw (CORBA::SystemException)
{
    try {  
	return accessor()->get_property_modes(lookupSystemTargetPath(), 
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


void System_impl::set_property_mode(const char* property_name,
				    CosPropertyService::PropertyModeType property_mode)
    throw (CosPropertyService::InvalidPropertyName,
	   CosPropertyService::PropertyNotFound,
	   CosPropertyService::UnsupportedMode,
	   CORBA::SystemException)
{
    try {  
	accessor()->set_property_mode(lookupSystemTargetPath(), 
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
System_impl::set_property_modes(const CosPropertyService::PropertyModes & property_modes)
    throw (CosPropertyService::MultipleExceptions,
	   CORBA::SystemException)
{
    try {  
	accessor()->set_property_modes(lookupSystemTargetPath(),
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
System_impl::register_all_properties_observer (const char* observer_name,
					       CdmwPlatformMngt::SupervisionObserver_ptr observer,
					       CORBA::Boolean deeply)   
    throw (CORBA::SystemException)
{
    try {  
	return accessor()->register_all_properties_observer(lookupSystemTargetPath(),
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
System_impl::unregister_all_properties_observer (const char* observer_name)
    throw (CdmwPlatformMngt::ObserverNotFound,
	   CORBA::SystemException)
{
    try {  
	return accessor()->unregister_all_properties_observer(lookupSystemTargetPath(),
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
System_impl::register_property_observer (
					 const char* observer_name,
					 CdmwPlatformMngt::SupervisionObserver_ptr observer,
					 const CosPropertyService::PropertyNames& property_names)
    throw (CdmwPlatformMngt::PropertyInListNotFound,
	   CORBA::SystemException)
{
    try {  
	accessor()->register_property_observer(lookupSystemTargetPath(),
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



void System_impl::unregister_property_observer (const char* observer_name,
						const CosPropertyService::PropertyNames& property_names)
    throw (CdmwPlatformMngt::PropertyInListNotFound,
	   CdmwPlatformMngt::PropertyObserverNotFound,
	   CORBA::SystemException)
{
    try {  
	accessor()->unregister_property_observer(lookupSystemTargetPath(),
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

