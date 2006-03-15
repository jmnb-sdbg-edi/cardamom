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


#include "platformftproxy/Process_impl.hpp"
#include <SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtDaemon.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{




Process_impl::Process_impl (CORBA::ORB_ptr orb,
			    ProxyCollection *proxy_collection,
			    AccessorLookupService *lookup_service)
    throw (Cdmw::BadParameterException, 
	   Cdmw::OutOfMemoryException)                  
    : ProxyUtil(orb),
      m_accessor(CdmwFTSystMngt::ProcessAccessor::_nil()),
      m_accessor_ok(false),
      m_proxy_collection(proxy_collection),
      m_lookup_service(lookup_service)
 {
     m_proxy_collection->add_ref();
 }

const CdmwFTSystMngt::ProcessAccessor_ptr 
Process_impl::accessor() 
    throw(CORBA::TRANSIENT) 
{
    if (!m_accessor_ok) {
	
	CDMW_MUTEX_GUARD(m_accessor_mutex);
	// leaving mutex other thread might have fetched the data
	// already

	try {

	    if (CORBA::is_nil(m_accessor.in())) {
		m_accessor = 
		    m_lookup_service->getProcessAccessor(); // raises AccessorUndefined
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


Process_impl::~Process_impl() throw()
{
    m_proxy_collection->remove_ref();
}


CdmwPlatformMngt::ProcessType Process_impl::type()
throw(  CORBA::SystemException )
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->get_type(target.c_str());
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

CORBA::Object_ptr Process_impl::embedded_service()
throw(CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->get_embedded_service(target.c_str());
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


CdmwPlatformMngt::ProcessPID Process_impl::pid()
throw(CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->get_pid(target.c_str());
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

                
CORBA::Boolean Process_impl::created_dynamically()
throw(CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->get_created_dynamically(target.c_str());
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
        
CdmwPlatformMngt::RestartType Process_impl::restart_type()
throw(CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->get_restart_type(target.c_str());
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
         
CORBA::Long Process_impl::restart_attempt()
throw(CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->get_restart_attempt(target.c_str());
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


bool Process_impl::get_run_as_root()
throw(CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->get_run_as_root(target.c_str());
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
    
CdmwPlatformMngtBase::TimeStamp Process_impl::start_time()
throw(CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->get_start_time(target.c_str());
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
       
CdmwPlatformMngtBase::TimeStamp Process_impl::stop_time()
throw(CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->get_stop_time(target.c_str());
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
    
CORBA::Boolean Process_impl::is_manual_starting()
throw(CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->is_manual_starting(target.c_str());
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
    
void Process_impl::set_manual_starting (CORBA::Boolean manual_starting)
throw(CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	accessor()->set_manual_starting(target.c_str(),
					manual_starting);
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
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
             
             
CORBA::Boolean Process_impl::is_auto_ending()
throw(  CORBA::SystemException )
{  
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->is_auto_ending(target.c_str());
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

CdmwPlatformMngt::Host_ptr Process_impl::get_host()
throw(  CORBA::SystemException )
{  
    try {  
	std::string target = this->getObjectId();
	
	CORBA::String_var uid =
	    accessor()->get_host(target.c_str());
	
	return this->m_proxy_collection->createHostRef(uid);
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


CdmwPlatformMngt::ProcessInfo* Process_impl::get_process_info()
throw(  CORBA::SystemException )
{ 
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->get_process_info(target.c_str());
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

CdmwPlatformMngt::LifeCycleDef* Process_impl::get_life_cycle()
throw( CdmwPlatformMngt::IncompatibleType,
       CORBA::SystemException )
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->get_life_cycle(target.c_str());
    }
     catch (const CdmwPlatformMngt::IncompatibleType & ex) {
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

void Process_impl::set_life_cycle(
    const CdmwPlatformMngt::LifeCycleDef& life_cycle )
throw( CdmwPlatformMngt::DuplicateStep,
       CdmwPlatformMngt::IncompatibleType,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    try {  
	std::string target = this->getObjectId();
	
	accessor()->set_life_cycle(target.c_str(),
				   life_cycle);
    }
    catch (const CdmwPlatformMngt::DuplicateStep  & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::IncompatibleType & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
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

CdmwPlatformMngt::CompleteMonitoringInfo Process_impl::get_monitoring_info()
throw( CdmwPlatformMngt::IncompatibleType,
       CORBA::SystemException )
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->get_monitoring_info(target.c_str());
    }
    catch (const CdmwPlatformMngt::IncompatibleType & ex) {
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

void Process_impl::set_monitoring_info(
    const CdmwPlatformMngt::CompleteMonitoringInfo& monitoring_info)
throw( CdmwPlatformMngt::IncompatibleType,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    try {  
	std::string target = this->getObjectId();
	
	accessor()->set_monitoring_info(target.c_str(),
					monitoring_info);
    }
    catch (const CdmwPlatformMngt::IncompatibleType & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
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

CdmwPlatformMngt::ExitAutoRestartInfo* Process_impl::get_exit_autorestart_info ()
throw (CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->get_exit_autorestart_info(target.c_str());
    }
    catch (const CdmwPlatformMngt::IncompatibleType & ex) {
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

             
             
void Process_impl::set_exit_autorestart_info (
        const CdmwPlatformMngt::ExitAutoRestartInfo & exit_autorestart_info)
throw (CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	accessor()->set_exit_autorestart_info(target.c_str(),
					      exit_autorestart_info);
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
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



CdmwPlatformMngt::ProcessSnapshot* Process_impl::get_snapshot()
throw(  CORBA::SystemException )
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->get_snapshot(target.c_str());
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

CdmwPlatformMngt::RequestStatus Process_impl::next_step()
throw( CdmwPlatformMngt::InvalidStep,
       CdmwPlatformMngt::IncompatibleType,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->next_step(target.c_str());
    }
    catch (const CdmwPlatformMngt::InvalidStep & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::IncompatibleType & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
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

CdmwPlatformMngt::RequestStatus Process_impl::run()
throw( CdmwPlatformMngt::NotReadyToRun,
       CdmwPlatformMngt::IncompatibleType,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->run(target.c_str());
    }
    catch (const CdmwPlatformMngt::NotReadyToRun & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::IncompatibleType & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
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

CdmwPlatformMngt::RequestStatus Process_impl::cold_restart (
        const CdmwPlatformMngtBase::StartupKind & startup_kind)
      throw (CORBA::SystemException,
	     CdmwPlatformMngt::IncompatibleStatus)
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->cold_restart(target.c_str(), startup_kind);
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
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


CdmwPlatformMngt::RequestStatus Process_impl::hot_restart (
        const CdmwPlatformMngtBase::StartupKind & startup_kind)
      throw (CORBA::SystemException,
	     CdmwPlatformMngt::IncompatibleStatus)
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->hot_restart(target.c_str(), startup_kind);
    }
    catch (const CdmwPlatformMngt::ManagedElementNotFound & ex) {
	throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
				      CORBA::COMPLETED_NO);
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}



CORBA::ULong Process_impl::get_number_of_entities()
throw( CORBA::SystemException )
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

void Process_impl::get_all_entity_names(
    CORBA::ULong how_many,
    CdmwPlatformMngt::ElementNames_out entity_names,
    CdmwPlatformMngt::ElementNamesIterator_out rest )
throw( CORBA::SystemException )
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

CdmwPlatformMngt::Entity_ptr Process_impl::get_entity (const char* entity_name)
throw(CdmwPlatformMngt::EntityNotFound, CORBA::SystemException)
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

CORBA::Boolean Process_impl::get_entities(
    const CdmwPlatformMngt::ElementNames& entity_names,
    CdmwPlatformMngt::Entities_out nentities )
throw( CORBA::SystemException )
{
    try {   
	std::string target = this->getObjectId();
	  
	::CdmwPlatformMngt::ElementPaths_var paths = 
	      new ::CdmwPlatformMngt::ElementPaths();
	
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

void Process_impl::get_all_entities(
    CORBA::ULong how_many,
    CdmwPlatformMngt::Entities_out nentities,
    CdmwPlatformMngt::EntitiesIterator_out rest )
throw( CORBA::SystemException )
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

CdmwPlatformMngt::Entity_ptr Process_impl::add_entity(
    const CdmwPlatformMngt::EntityDef & entity_def)
throw( CdmwPlatformMngt::EntityAlreadyExists, CORBA::SystemException )
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

void Process_impl::remove_entity(
    const char* entity_name )
throw( CdmwPlatformMngt::EntityNotFound, CORBA::SystemException )
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

CdmwPlatformMngtService::ServiceProviderID*
Process_impl::get_service_def(
    const char* service_name )
throw( CdmwPlatformMngtService::ServiceNotFound, 
       CORBA::SystemException )
{
    try {    
	std::string target = this->getObjectId();

	return accessor()->get_service_def(target.c_str(),
					   service_name);
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

void Process_impl::add_service_def(
    const CdmwPlatformMngtService::ServiceDef& service_def )
throw( CdmwPlatformMngtService::ServiceAlreadyExists, CORBA::SystemException )
{
    try {    
	std::string target = this->getObjectId();

	accessor()->add_service_def(target.c_str(),
				    service_def);
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

void 
Process_impl::remove_service_def(
    const char* service_name )
throw( CdmwPlatformMngtService::ServiceNotFound, CORBA::SystemException )
{
    try {    
	std::string target = this->getObjectId();

	accessor()->remove_service_def(target.c_str(),
				       service_name);
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

void 
Process_impl::remove_all_service_defs ()
	 throw (CORBA::SystemException)
{
    try {    
	std::string target = this->getObjectId();

	accessor()->remove_all_service_defs(target.c_str());
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


    
void Process_impl::is_alive()
  throw(CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


void Process_impl::set_create_process_result (
        CdmwPlatformMngt::ProcessCommandResult result,
        const char* result_info,
        CdmwPlatformMngt::ProcessDelegateWrapper_ptr process_delegate_wrap)
            throw (CORBA::SystemException)
{ 
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

     

void Process_impl::set_initialisation_result (
        CdmwPlatformMngt::ProcessCommandResult result)
            throw (CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}
     

void Process_impl::set_next_step_result (
        CdmwPlatformMngt::ProcessCommandResult result,
        CORBA::ULong step_nbr)
            throw (CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}
     

void Process_impl::set_run_result (
        CdmwPlatformMngt::ProcessCommandResult result)
            throw (CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}
     

void Process_impl::set_stop_result (
        CdmwPlatformMngt::ProcessCommandResult result)
            throw (CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

 
     
char* 
Process_impl::name()
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
Process_impl::element_path()
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
Process_impl::get_parent()
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
Process_impl::define_property(const char* property_name,
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
Process_impl::define_properties(const CosPropertyService::Properties & nproperties)
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
Process_impl::get_number_of_properties()
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
Process_impl::get_all_property_names (CORBA::ULong how_many,       
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

      
CORBA::Any* Process_impl::get_property_value (const char* property_name)
    throw (CosPropertyService::PropertyNotFound,
	   CosPropertyService::InvalidPropertyName,
	   CORBA::SystemException)
{
    try {   
	std::string target = this->getObjectId();
	
	return accessor()->get_property_value(target.c_str(), 
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
Process_impl::get_properties (const CosPropertyService::PropertyNames& property_names,
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
Process_impl::get_all_properties (CORBA::ULong how_many,      
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
Process_impl::delete_property(const char* property_name)
    throw (CosPropertyService::PropertyNotFound,
	   CosPropertyService::InvalidPropertyName,
	   CosPropertyService::FixedProperty,
	   CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();

	accessor()->delete_property(target.c_str(), 
				    property_name);	
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
Process_impl::delete_properties(const CosPropertyService::PropertyNames& property_names)
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
Process_impl::delete_all_properties()
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
Process_impl::is_property_defined(const char* property_name)
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
Process_impl::get_allowed_property_types(CosPropertyService::PropertyTypes_out property_types)
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
Process_impl::get_allowed_properties(CosPropertyService::PropertyDefs_out property_defs)
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
Process_impl::define_property_with_mode(const char* property_name,
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

    
void Process_impl::define_properties_with_modes(const CosPropertyService::PropertyDefs & property_defs)
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



CosPropertyService::PropertyModeType Process_impl::get_property_mode(const char* property_name)
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
Process_impl::get_property_modes(const CosPropertyService::PropertyNames& property_names,
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


void Process_impl::set_property_mode(const char* property_name,
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
Process_impl::set_property_modes(const CosPropertyService::PropertyModes & property_modes)
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
Process_impl::register_all_properties_observer (const char* observer_name,
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
Process_impl::unregister_all_properties_observer (const char* observer_name)
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
Process_impl::set_create_process_result (
	 ::CdmwPlatformMngt::ProcessCommandResult result,
	 const char * error_info,
	 const char * error_issuer,
	 ::CdmwPlatformMngt::ProcessDelegateWrapper_ptr process_delegate_wrap)
	throw (CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

   
void 
Process_impl::register_property_observer (
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



void Process_impl::unregister_property_observer (const char* observer_name,
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
Process_impl::get_command_status (CORBA::String_out status_info)
    throw (CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);

}
   

void 
Process_impl::call_command (const char * command_name,
	      const ::CdmwPlatformMngt::CmdParameterValues & command_args,
	      ::CdmwPlatformMngt::CommandResponseCallback_ptr response_callback
	      )
    throw  (CORBA::SystemException,
	    ::CdmwPlatformMngt::CommandNotFound,
	    ::CdmwPlatformMngt::CommandArgumentInvalid,
	    ::CdmwPlatformMngt::CommandRejected)
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}
 

void Process_impl::get_attribute(const char * attr_name, CORBA::Any_out ret_attr, CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{

    try {     
	std::string target = this->getObjectId();
	
	accessor()->get_attribute(target.c_str(), attr_name, ret_attr, flag);
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

void Process_impl::set_attribute(const char * attr_name, const
CORBA::Any & attr, CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{


    try {     
	std::string target = this->getObjectId();
	
	accessor()->set_attribute(target.c_str(), attr_name, attr, flag);
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

  
CdmwPlatformMngt::ProcessStatus
Process_impl::get_status( CORBA::String_out status_info)
    throw(CORBA::SystemException)
{
    try {     
	std::string target = this->getObjectId();
	
	return accessor()->get_status(target.c_str(), status_info);
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

///get_process_mode
CdmwPlatformMngt::ProcessMode
Process_impl::get_process_mode( )
    throw(CORBA::SystemException)
{
    try {     
	std::string target = this->getObjectId();
	
	return accessor()->get_process_mode(target.c_str() );
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

///get_memory_resident
bool
Process_impl::get_memory_resident()
    throw(CORBA::SystemException)
{
    try {     
	std::string target = this->getObjectId();
	
	return accessor()->get_memory_resident(target.c_str() );
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

///set_memory_resident
void
Process_impl::set_memory_resident(bool memory_resident)
    throw(CORBA::SystemException)
{
    try {     
	std::string target = this->getObjectId();
	
	return accessor()->set_memory_resident(target.c_str(), memory_resident);
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

///get_core_dump_size
 CORBA::Long
Process_impl::get_core_dump_size()
    throw(CORBA::SystemException)
{
    try {     
	std::string target = this->getObjectId();
	
	return accessor()->get_core_dump_size(target.c_str() );
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
Process_impl::set_auto_ending(CORBA::Boolean auto_ending)
    throw (CdmwPlatformMngt::IncompatibleStatus,
	   CORBA::SystemException)
{
    try {     
	std::string target = this->getObjectId();
	
	 accessor()->set_auto_ending(target.c_str(), auto_ending);
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
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
Process_impl::set_process_info(const CdmwPlatformMngt::ProcessInfo& process_info)
    throw (CdmwPlatformMngt::IncompatibleStatus,
	   CORBA::SystemException)
{
    try {     
	std::string target = this->getObjectId();
	
	 accessor()->set_process_info(target.c_str(), process_info);
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
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


CdmwPlatformMngt::RequestStatus 
Process_impl::initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw (CdmwPlatformMngt::IncompatibleStatus,
	   CORBA::SystemException)
{
    try {     
	std::string target = this->getObjectId();
	
	 return accessor()->initialise(target.c_str(), startup_kind);
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
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


CdmwPlatformMngt::RequestStatus 
Process_impl::stop(CORBA::Boolean emergency)
    throw (CORBA::SystemException)
{
    try {     
	std::string target = this->getObjectId();
	
	 return accessor()->stop(target.c_str(), emergency);
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
Process_impl::add_command (
        const char * command_name,
        ::CdmwPlatformMngt::Timeout time_out,
        const ::CdmwPlatformMngt::CmdParameterDescriptors & command_args
      )
        throw (CORBA::SystemException,
	       ::CdmwPlatformMngt::CommandAlreadyExists,
	       ::CdmwPlatformMngt::CommandArgumentAlreadyExists)
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}
   
 
void 
Process_impl::remove_command (const char * command_name)
    throw ( CORBA::SystemException,
	    ::CdmwPlatformMngt::CommandNotFound )
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}
   


} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw

