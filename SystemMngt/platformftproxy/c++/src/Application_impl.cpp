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



#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

#include <SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtDaemon.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp>

#include "platformftproxy/Application_impl.hpp"

namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{


// ----------------------------------------------------------------------
// Application_impl class.
// ----------------------------------------------------------------------
Application_impl::Application_impl(CORBA::ORB_ptr orb,
				   ProxyCollection *proxy_collection,
				   AccessorLookupService *lookup_service)
    throw (Cdmw::BadParameterException, 
	   Cdmw::OutOfMemoryException)                 
    : ProxyUtil(orb),
      m_accessor(CdmwFTSystMngt::ApplicationAccessor::_nil()),
      m_accessor_ok(false),
      m_proxy_collection(proxy_collection),
      m_lookup_service(lookup_service)
{    
    m_proxy_collection->add_ref();
}

const CdmwFTSystMngt::ApplicationAccessor_ptr 
Application_impl::accessor() 
    throw(CORBA::TRANSIENT) 
{
    if (!m_accessor_ok) {
	
	CDMW_MUTEX_GUARD(m_accessor_mutex);
	// leaving mutex other thread might have fetched the data
	// already

	try {

	    if (CORBA::is_nil(m_accessor.in())) {
		m_accessor = 
		    m_lookup_service->getApplicationAccessor(); // raises AccessorUndefined
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


Application_impl::~Application_impl() throw ()
{
    m_proxy_collection->remove_ref();
}

CdmwPlatformMngt::RestartType Application_impl::restart_type ()
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
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}
    
   
CORBA::Long Application_impl::restart_attempt ()
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
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}
        
    
    
CORBA::Boolean Application_impl::is_manual_starting()
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
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}
     
     
  
void Application_impl::set_manual_starting(
    CORBA::Boolean manual_starting)
throw(CORBA::SystemException,
      CdmwPlatformMngt::IncompatibleStatus)
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
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}

void Application_impl::get_attribute(const char * attr_name, CORBA::Any_out ret_attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{

    try {
	std::string target = this->getObjectId();

	accessor()->get_attribute(target.c_str(), 
					attr_name, ret_attr, flag);
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
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


void Application_impl::set_attribute(const char * attr_name, const CORBA::Any & attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{

    try {
	std::string target = this->getObjectId();

	accessor()->set_attribute(target.c_str(), 
					attr_name, attr, flag);
    }
    catch (const CdmwPlatformMngt::IncompatibleStatus & ex) {
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

CdmwPlatformMngt::ApplicationStatus Application_impl::get_status(
                   CORBA::String_out status_info)
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
    catch (const  CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}

CdmwPlatformMngt::FunctioningMode Application_impl::get_mode(
    CORBA::String_out mode_info)
throw(CORBA::SystemException)
{
    try {
	std::string target = this->getObjectId();

	return accessor()->get_mode(target.c_str(), mode_info);
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

CdmwPlatformMngt::ApplicationSnapshot* Application_impl::get_snapshot()
throw(CORBA::SystemException)
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


CORBA::ULong Application_impl::get_number_of_processes()
throw(CORBA::SystemException)
{
    try {
	std::string target = this->getObjectId();
	
	return accessor()->get_number_of_processes(target.c_str());
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

CdmwPlatformMngt::Processes* Application_impl::get_processes(
    CORBA::ULong how_many,
    CdmwPlatformMngt::ProcessesIterator_out processes_iterator)
throw(CORBA::SystemException)
{
    try { 
	std::string target = this->getObjectId();
	 

	CdmwPlatformMngt::ElementPathsIterator_var iter;

	CdmwPlatformMngt::ElementPaths_var paths = 
	    accessor()->get_processes(target.c_str(), 
				      how_many,
				      iter.out());
	
	CdmwPlatformMngt::ProcessesIterator_var tmp_processes_iterator = 
	    this->m_proxy_collection->createProcessesIteratorRef(iter.in());
	
	CdmwPlatformMngt::Processes_var tmp_processes =
	    this->m_proxy_collection->createProcessesFromSeq(paths.ptr());

	processes_iterator = tmp_processes_iterator._retn();

	return tmp_processes._retn();
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

CdmwPlatformMngt::Process_ptr Application_impl::get_process(
                const char* process_name,
                const char* host_name)
throw(CdmwPlatformMngt::ProcessNotFound,
      CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	CORBA::String_var proc_target = 
	    accessor()->get_process (target.c_str(),
				     process_name,
				     host_name);
	
	return this->m_proxy_collection->createProcessRef(proc_target);
    }
    catch (const CdmwPlatformMngt::ProcessNotFound & ex) {
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

void Application_impl::define(
    const CdmwPlatformMngt::ApplicationDef& application_def)
throw(CdmwPlatformMngt::DuplicateEntity,
      CdmwPlatformMngt::DuplicateService,
      CdmwPlatformMngt::DuplicateProcess,
      CdmwPlatformMngt::ProcessHostNotFound,
      CdmwPlatformMngt::DuplicateProcessEntity,
      CdmwPlatformMngt::DuplicateProcessService,
      CdmwPlatformMngt::DuplicateProcessStep,
      CdmwPlatformMngt::AlreadyDone,
      CdmwPlatformMngt::IncompatibleStatus,
      CosPropertyService::MultipleExceptions,
      CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	accessor()->define (target.c_str(),
			    application_def);
    }
    catch (const CdmwPlatformMngt::DuplicateEntity & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::DuplicateService & ex) {
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
    catch (const CdmwPlatformMngt::AlreadyDone & ex) {
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

CdmwPlatformMngt::Process_ptr Application_impl::add_process(
    CdmwPlatformMngt::ProcessDef* process_def)
throw (CdmwPlatformMngt::ProcessAlreadyExists,
       CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::DuplicateEntity,
       CdmwPlatformMngt::DuplicateService,
       CdmwPlatformMngt::DuplicateStep,
       CdmwPlatformMngt::IncompatibleStatus,
       CosPropertyService::MultipleExceptions,
       CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	CORBA::String_var proc_target = 
	    accessor()->add_process (target.c_str(),
				     process_def);
	
	return this->m_proxy_collection->createProcessRef(proc_target);
    }
    catch (const CdmwPlatformMngt::ProcessAlreadyExists & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::HostNotFound & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::DuplicateEntity & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::DuplicateService & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::DuplicateStep & ex) {
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

void Application_impl::remove_process(const char* process_name,
                                      const char* host_name)
throw (CdmwPlatformMngt::ProcessNotFound,
       CdmwPlatformMngt::ProcessStillActive,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	accessor()->remove_process (target.c_str(),
				    process_name,
				    host_name);
    }
    catch (const CdmwPlatformMngt::ProcessNotFound & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::ProcessStillActive & ex) {
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

void Application_impl::set_init_graph(
    const CdmwPlatformMngt::GraphElements& process_graph)
throw (CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::StepOutOfBound,
       CdmwPlatformMngt::StepJump,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	accessor()->set_init_graph (target.c_str(),
				    process_graph);
    }
    catch (const CdmwPlatformMngt::ReferenceNotFound & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::CircularReference & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::StepOutOfBound & ex) {
	throw;
    }
    catch (const CdmwPlatformMngt::StepJump & ex) {
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

void Application_impl::set_stop_graph(
    const CdmwPlatformMngt::GraphElements& process_graph)
throw( CdmwPlatformMngt::ReferenceNotFound,
       CdmwPlatformMngt::CircularReference,
       CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	accessor()->set_stop_graph (target.c_str(),
				    process_graph);
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


CdmwPlatformMngt::ApplicationAutoRestartInfo Application_impl::get_autorestart_info()
throw(CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->get_autorestart_info (target.c_str());
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



void Application_impl::set_autorestart_info(
     const CdmwPlatformMngt::ApplicationAutoRestartInfo& autorestart_info)
throw(CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	accessor()->set_autorestart_info (target.c_str(),
					  autorestart_info);
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



void Application_impl::initialise(
    const CdmwPlatformMngtBase::StartupKind& startup_kind)
throw (CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	accessor()->initialise (target.c_str(),
				startup_kind);
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

CdmwPlatformMngt::LongRequestStatus Application_impl::initialise_and_wait(
    const CdmwPlatformMngtBase::StartupKind& startup_kind)
throw (CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException )
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->initialise_and_wait(target.c_str(),
					       startup_kind);
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

void Application_impl::resume()
throw (CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	accessor()->resume(target.c_str());
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

CdmwPlatformMngt::LongRequestStatus Application_impl::resume_and_wait()
throw (CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->resume_and_wait(target.c_str());
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

void Application_impl::acknowledge()
throw (CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	accessor()->acknowledge(target.c_str());
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

void Application_impl::run()
throw (CdmwPlatformMngt::IncompatibleStatus,
       CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	accessor()->run(target.c_str());
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

void Application_impl::stop(CORBA::Boolean emergency)
throw(CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	accessor()->stop(target.c_str(), emergency);
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

CdmwPlatformMngt::LongRequestStatus Application_impl::stop_and_wait(
    CORBA::Boolean emergency)
throw(CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	return accessor()->stop_and_wait(target.c_str(), emergency);
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

void Application_impl::cold_restart(
    const CdmwPlatformMngtBase::StartupKind& startup_kind)
throw(CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	accessor()->cold_restart(target.c_str(), startup_kind);
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
      
            


void Application_impl::hot_restart(
    const CdmwPlatformMngtBase::StartupKind& startup_kind)
throw(CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
    try {  
	std::string target = this->getObjectId();
	
	accessor()->hot_restart(target.c_str(), startup_kind);
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
 
 
 
 

CORBA::ULong Application_impl::get_number_of_entities()
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

void Application_impl::get_all_entity_names(
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

CdmwPlatformMngt::Entity_ptr Application_impl::get_entity (
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

CORBA::Boolean Application_impl::get_entities(
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

void Application_impl::get_all_entities(
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

    
CdmwPlatformMngt::Entity_ptr Application_impl::add_entity (
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

void Application_impl::remove_entity(
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

CdmwPlatformMngtService::ServiceProviderID* Application_impl::get_service_def(
    const char* service_name)
throw (CdmwPlatformMngtService::ServiceNotFound,
       CORBA::SystemException)
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

void Application_impl::add_service_def(
    const CdmwPlatformMngtService::ServiceDef& service_def)
throw (CdmwPlatformMngtService::ServiceAlreadyExists,
       CORBA::SystemException)
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

void Application_impl::remove_service_def(
    const char* service_name)
throw (CdmwPlatformMngtService::ServiceNotFound,
       CORBA::SystemException)
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
Application_impl::remove_all_service_defs ()
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

void Application_impl::is_alive()
  throw(CORBA::SystemException)
{
    /*
     * Not iplemented - raises NO_IMPLEMENT exception. The
     * operation is deprecated.
     */
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

CdmwPlatformMngt::SupervisionObserver_ptr Application_impl::register_observer(
                     const char* observer_name,
                     CdmwPlatformMngt::SupervisionObserver_ptr observer)
throw(CORBA::SystemException)
{
    /*
     * Not iplemented - raises NO_IMPLEMENT exception. The
     * operation is deprecated.
     */
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);

}

CdmwPlatformMngt::SupervisionObserver_ptr Application_impl::register_proxy_observer(
                      const char* observer_name,
                      const char* host_name,
                      CdmwPlatformMngt::SupervisionObserver_ptr observer)
throw (CORBA::SystemException)
{
    /*
     * Not iplemented - raises NO_IMPLEMENT exception. The
     * operation is deprecated.
     */
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

CdmwPlatformMngt::SupervisionObserver_ptr Application_impl::unregister_observer(
    const char* observer_name)
throw (CdmwPlatformMngt::ObserverNotFound,
       CORBA::SystemException)
{
    /*
     * Not iplemented - raises NO_IMPLEMENT exception. The
     * operation is deprecated.
     */
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}



char* 
Application_impl::name()
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
Application_impl::element_path()
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
Application_impl::get_parent()
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
Application_impl::define_property(const char* property_name,
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
Application_impl::define_properties(const CosPropertyService::Properties & nproperties)
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
Application_impl::get_number_of_properties()
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
Application_impl::get_all_property_names (CORBA::ULong how_many,       
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

      
CORBA::Any* Application_impl::get_property_value (const char* property_name)
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
Application_impl::get_properties (const CosPropertyService::PropertyNames& property_names,
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
Application_impl::get_all_properties (CORBA::ULong how_many,      
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
Application_impl::delete_property(const char* property_name)
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
Application_impl::delete_properties(const CosPropertyService::PropertyNames& property_names)
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
Application_impl::delete_all_properties()
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
Application_impl::is_property_defined(const char* property_name)
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
Application_impl::get_allowed_property_types(CosPropertyService::PropertyTypes_out property_types)
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
Application_impl::get_allowed_properties(CosPropertyService::PropertyDefs_out property_defs)
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
Application_impl::define_property_with_mode(const char* property_name,
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

    
void Application_impl::define_properties_with_modes(const CosPropertyService::PropertyDefs & property_defs)
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



CosPropertyService::PropertyModeType Application_impl::get_property_mode(const char* property_name)
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
Application_impl::get_property_modes(const CosPropertyService::PropertyNames& property_names,
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


void Application_impl::set_property_mode(const char* property_name,
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
Application_impl::set_property_modes(const CosPropertyService::PropertyModes & property_modes)
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
Application_impl::register_all_properties_observer (const char* observer_name,
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
Application_impl::unregister_all_properties_observer (const char* observer_name)
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
Application_impl::register_property_observer (
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



void Application_impl::unregister_property_observer (const char* observer_name,
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

