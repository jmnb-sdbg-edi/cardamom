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

#include <memory> // auto_ptr 

#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

#include <SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtHost.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtEntity.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtProcess.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtApplication.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtEntityContainer.stub.hpp>

#include <Foundation/common/Assert.hpp> 
#include "platformftproxy/ProxyUtil.hpp"
#include "platformftproxy/ProxyCollection.hpp"

#include "platformftproxy/System_impl.hpp"
#include "platformftproxy/Host_impl.hpp"
#include "platformftproxy/Application_impl.hpp"
#include "platformftproxy/Entity_impl.hpp"
#include "platformftproxy/Process_impl.hpp"

#include "platformftproxy/HostsIterator_impl.hpp"
#include "platformftproxy/ApplicationsIterator_impl.hpp"
#include "platformftproxy/EntitiesIterator_impl.hpp"
#include "platformftproxy/ProcessesIterator_impl.hpp"



namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{

class Host_impl;
class Entity_impl;
class Process_impl;
class Application_impl;

class HostsIterator_impl;
class ApplicationsIterator_impl;
class EntitiesIterator_impl;
class ProcessesIterator_impl;

ProxyCollection::ProxyCollection (CORBA::ORB_ptr orb, 
				  PortableServer::POA_ptr parent_poa,
				  AccessorLookupService *lookup_service,
				  std::string proxy_name) 
    throw (Cdmw::BadParameterException,
	   Cdmw::OutOfResourcesException,
	   Cdmw::OutOfMemoryException)
    : m_lookup_service(lookup_service),
      m_ref_counter(1)
{
    CDMW_ASSERT(m_lookup_service!=NULL);
    CDMW_ASSERT(m_ref_counter==1);

    if (lookup_service==NULL) {
	CDMW_THROW2(Cdmw::BadParameterException, "lookup_service", "NULL");
    }

    try { // FIXME verify exceptions raised
	m_orb = CORBA::ORB::_duplicate(orb);
	
	m_system_poa = ProxyUtil::createDefaultServantPOA(orb, 
							  parent_poa,
							  proxy_name);
    } catch (...) { 
	CDMW_THROW(Cdmw::OutOfResourcesException); 
    }

    try {
	try {
	    PortableServer::POA_var host_poa = 
		ProxyUtil::createDefaultServantPOA(orb, 
						   m_system_poa.in(),
						   proxy_name + "H");
	    
	    PortableServer::POA_var appl_poa = 
		ProxyUtil::createDefaultServantPOA(orb, 
						   m_system_poa.in(),
						   proxy_name + "A");
	    
	    PortableServer::POA_var entity_poa = 
		ProxyUtil::createDefaultServantPOA(orb, 
						   m_system_poa.in(),
						   proxy_name + "E");
	    
	    PortableServer::POA_var proc_poa = 
		ProxyUtil::createDefaultServantPOA(orb, 
						   m_system_poa.in(),
						   proxy_name + "P");
	    
	    PortableServer::POA_var host_iter_poa = 
		ProxyUtil::createDefaultServantPOA(orb, 
						   m_system_poa.in(),
						   proxy_name + "Hi");
	    
	    PortableServer::POA_var appl_iter_poa = 
		ProxyUtil::createDefaultServantPOA(orb, 
						   m_system_poa.in(),
						   proxy_name + "Ai");
	    
	    PortableServer::POA_var entity_iter_poa = 
		ProxyUtil::createDefaultServantPOA(orb, 
						   m_system_poa.in(),
						   proxy_name + "Ei");
	    
	    PortableServer::POA_var proc_iter_poa = 
		ProxyUtil::createDefaultServantPOA(orb, 
						   m_system_poa.in(),
						   proxy_name + "Pi");
	    m_host_poa = host_poa;
	    m_proc_poa = proc_poa;
	    m_appl_poa = appl_poa;
	    m_entity_poa = entity_poa;
	    m_host_iter_poa = host_iter_poa;
	    m_proc_iter_poa = proc_iter_poa;
	    m_appl_iter_poa = appl_iter_poa;
	    m_entity_iter_poa = entity_iter_poa;	    
	} catch (...) {
	    CDMW_THROW(Cdmw::OutOfResourcesException); 
	}
	
	//
	// instanciate servants
	//
	try {
	    auto_ptr<Cdmw::PlatformMngt::Proxy::System_impl> servant(
		new System_impl(orb,
				this,
				lookup_service));
	    
	    m_system_poa->set_servant(servant.get());
	    
	    servant.get()->_remove_ref();
	    
	    servant.release();
	}
	catch (...) {
	    CDMW_THROW(Cdmw::OutOfResourcesException); 
	}
	
	/* ******************* */
	
	try {
	    auto_ptr<Cdmw::PlatformMngt::Proxy::Host_impl> servant(
		new Host_impl(orb,
			      this,
			      lookup_service));
	    
	    m_host_poa->set_servant(servant.get());
	    
	    servant.get()->_remove_ref();
	    
	    servant.release();
	}
	catch (...) {
	    CDMW_THROW(Cdmw::OutOfResourcesException); 
	}
	
	/* ******************* */
	
	try {
	    auto_ptr<Cdmw::PlatformMngt::Proxy::Process_impl> servant(
		new Process_impl(orb,
				 this,
				 lookup_service));
	    
	    m_proc_poa->set_servant(servant.get());
	    
	    servant.get()->_remove_ref();
	    
	    servant.release();
	}
	catch (...) {
	    CDMW_THROW(Cdmw::OutOfResourcesException); 
	}

	/* ******************* */
	
	try {
	    auto_ptr<Cdmw::PlatformMngt::Proxy::Application_impl> servant(
		new Application_impl(orb,
				     this,
				     lookup_service));
	    
	    m_appl_poa->set_servant(servant.get());
	    
	    servant.get()->_remove_ref();
	    
	    servant.release();
	}
	catch (...) {
	    CDMW_THROW(Cdmw::OutOfResourcesException); 
	}

	/* ******************* */
	
	try {
	    auto_ptr<Cdmw::PlatformMngt::Proxy::Entity_impl> servant(
		new Entity_impl(orb,
				this,
				lookup_service));
	    
	    m_entity_poa->set_servant(servant.get());
	    
	    servant.get()->_remove_ref();
	    
	    servant.release();
	}
	catch (...) {
	    CDMW_THROW(Cdmw::OutOfResourcesException); 
	}

	/* ******************* */
	
	try {
	    auto_ptr<Cdmw::PlatformMngt::Proxy::HostsIterator_impl> servant(
	        new HostsIterator_impl(orb,
				       this));
	    
	    m_host_iter_poa->set_servant(servant.get());
	    
	    servant.get()->_remove_ref();
	    
	    servant.release();
	}
	catch (...) {
	    CDMW_THROW(Cdmw::OutOfResourcesException); 
	}

	/* ******************* */
	
	try {
	    auto_ptr<Cdmw::PlatformMngt::Proxy::EntitiesIterator_impl> servant(
		new EntitiesIterator_impl(orb,
					  this));
	    
	    m_entity_iter_poa->set_servant(servant.get());
	    
	    servant.get()->_remove_ref();
	    
	    servant.release();
	}
	catch (...) {
	    CDMW_THROW(Cdmw::OutOfResourcesException); 
	}

	/* ******************* */
	
	try {
	    auto_ptr<Cdmw::PlatformMngt::Proxy::ApplicationsIterator_impl> servant( 
		new ApplicationsIterator_impl(orb,
					      this));
	    
	    m_appl_iter_poa->set_servant(servant.get());
	    
	    servant.get()->_remove_ref();
	    
	    servant.release();
	}
	catch (...) {
	    CDMW_THROW(Cdmw::OutOfResourcesException); 
	}

	/* ******************* */
	
	try {
	    auto_ptr<Cdmw::PlatformMngt::Proxy::ProcessesIterator_impl> servant(
		new ProcessesIterator_impl(orb,
					   this));
	    
	    m_proc_iter_poa->set_servant(servant.get());
	    
	    servant.get()->_remove_ref();
	    
	    servant.release();
	}
	catch (...) {
	    CDMW_THROW(Cdmw::OutOfResourcesException); 
	}
    } 
    catch (...) {
	// on error, destroy the root POA syncronous
	m_system_poa->destroy(true,  /* etheralize objects  */  
			      true); /* wait for completion */
	throw; // re-throw
    }
}
    
    
void 
ProxyCollection::add_ref() throw () 
{
    CDMW_MUTEX_GUARD(m_ref_counter_mutex);
    ++m_ref_counter; 
}
    
void 
ProxyCollection::remove_ref() throw ()
{
    long current_ref_counter;

    { 
	CDMW_MUTEX_GUARD(m_ref_counter_mutex);

	current_ref_counter = --m_ref_counter; 

	//release mutex at end of block
    }

    if (current_ref_counter==0) {
	delete this;
    }
}

void 
ProxyCollection::destroy(bool wait_for_completion)  // FIXME tbd
    throw()
{
    try {
       this->remove_ref();
 
	m_system_poa->destroy(true,                 /* etheralize objects  */  
			      wait_for_completion); /* wait for completion */

    } 
    catch (...) {
	// ignore
    }
}

ProxyCollection::ProxyCollection() 
    throw (Cdmw::BadParameterException,
	   Cdmw::OutOfResourcesException,
	   Cdmw::OutOfMemoryException)
{
    CDMW_NEVER_HERE();
}


ProxyCollection::ProxyCollection (ProxyCollection& rhs)
{
    CDMW_NEVER_HERE();
}


ProxyCollection& 
ProxyCollection::operator= (const ProxyCollection& rhs)
{
    CDMW_NEVER_HERE();
}

ProxyCollection::~ProxyCollection() throw() 
{
    delete m_lookup_service;
}

CdmwPlatformMngt::System_ptr 
ProxyCollection::createSystemRef() 
	throw (CORBA::SystemException)
{ 
    try {
	std::string targetid("singleton"); 
	
	PortableServer::ObjectId_var oid =
	    PortableServer::string_to_ObjectId(targetid.c_str());
	
	CORBA::Object_var obj = 
	    m_system_poa->create_reference_with_id
	    (oid, "IDL:thalesgroup.com/CdmwPlatformMngt/System:1.0");
	
	return CdmwPlatformMngt::System::_narrow(obj.in());
    }
    catch (const CORBA::SystemException &ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}

CdmwPlatformMngt::Host_ptr 
ProxyCollection::createHostRef(const char* uid)
	throw (CORBA::SystemException) 
{ 
    try {
	PortableServer::ObjectId_var oid =
	    PortableServer::string_to_ObjectId(uid);
	
	CORBA::Object_var obj = 
	    m_host_poa->create_reference_with_id
	    (oid, "IDL:thalesgroup.com/CdmwPlatformMngt/Host:1.0");
	
	return CdmwPlatformMngt::Host::_narrow(obj.in());
    }
    catch (const CORBA::SystemException &ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}

CdmwPlatformMngt::Process_ptr 
ProxyCollection::createProcessRef(const char* uid)
	throw (CORBA::SystemException)
{ 
    try {
	PortableServer::ObjectId_var oid =
	    PortableServer::string_to_ObjectId(uid);
	
	CORBA::Object_var obj = 
	    m_proc_poa->create_reference_with_id
	    (oid, "IDL:thalesgroup.com/CdmwPlatformMngt/Process:1.0");
	
	return CdmwPlatformMngt::Process::_narrow(obj.in());
    }
    catch (const CORBA::SystemException &ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}  

CdmwPlatformMngt::Application_ptr 
ProxyCollection::createApplicationRef(const char* uid) 
	throw (CORBA::SystemException) 
{ 
    try {
	PortableServer::ObjectId_var oid =
	    PortableServer::string_to_ObjectId(uid);
	
	CORBA::Object_var obj = 
	    m_appl_poa->create_reference_with_id
	    (oid, "IDL:thalesgroup.com/CdmwPlatformMngt/Application:1.0");
	
	return CdmwPlatformMngt::Application::_narrow(obj.in());
    }
    catch (const CORBA::SystemException &ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}  

CdmwPlatformMngt::Entity_ptr 
ProxyCollection::createEntityRef(const char* uid) 
	throw (CORBA::SystemException) 
{ 
    try {
	PortableServer::ObjectId_var oid =
	    PortableServer::string_to_ObjectId(uid);
	
	CORBA::Object_var obj = 
	    m_entity_poa->create_reference_with_id
	    (oid, "IDL:thalesgroup.com/CdmwPlatformMngt/Entity:1.0");
	
	return CdmwPlatformMngt::Entity::_narrow(obj.in());
    }
    catch (const CORBA::SystemException &ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}  

    /* ************************************* */
    
    
CdmwPlatformMngt::HostsIterator_ptr 
ProxyCollection::createHostsIteratorRef(CdmwPlatformMngt::ElementPathsIterator_ptr ref) 
	throw (CORBA::SystemException) 
{ 
    try {
	CORBA::String_var stringified_ref = m_orb->object_to_string(ref);
	
	PortableServer::ObjectId_var oid =
	    PortableServer::string_to_ObjectId(stringified_ref.in());
	
	CORBA::Object_var obj = 
	    m_host_iter_poa->create_reference_with_id
	    (oid, "IDL:thalesgroup.com/CdmwPlatformMngt/HostsIterator:1.0");
	
	return CdmwPlatformMngt::HostsIterator::_narrow(obj.in());
    }
    catch (const CORBA::SystemException &ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}

    
CdmwPlatformMngt::EntitiesIterator_ptr 
ProxyCollection::createEntitiesIteratorRef(CdmwPlatformMngt::ElementPathsIterator_ptr ref) 
	throw (CORBA::SystemException) 
{ 
    try {
	CORBA::String_var stringified_ref = m_orb->object_to_string(ref);
	
	PortableServer::ObjectId_var oid =
	    PortableServer::string_to_ObjectId(stringified_ref.in());
	
	CORBA::Object_var obj = 
	    m_entity_iter_poa->create_reference_with_id
	    (oid, "IDL:thalesgroup.com/CdmwPlatformMngt/EntitiesIterator:1.0");
	
	return CdmwPlatformMngt::EntitiesIterator::_narrow(obj.in());
    }
    catch (const CORBA::SystemException &ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}


CdmwPlatformMngt::ProcessesIterator_ptr 
ProxyCollection::createProcessesIteratorRef(CdmwPlatformMngt::ElementPathsIterator_ptr ref)  
	throw (CORBA::SystemException) 
{ 
    try {
	CORBA::String_var stringified_ref = m_orb->object_to_string(ref);
	
	PortableServer::ObjectId_var oid =
	    PortableServer::string_to_ObjectId(stringified_ref.in());
	
	CORBA::Object_var obj = 
	    m_proc_iter_poa->create_reference_with_id
	    (oid, "IDL:thalesgroup.com/CdmwPlatformMngt/ProcessesIterator:1.0");
	
	return CdmwPlatformMngt::ProcessesIterator::_narrow(obj.in());
    }
    catch (const CORBA::SystemException &ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}


CdmwPlatformMngt::ApplicationsIterator_ptr 
ProxyCollection::createApplicationsIteratorRef(CdmwPlatformMngt::ElementPathsIterator_ptr ref)  
	throw (CORBA::SystemException) 
{ 
    try {
	CORBA::String_var stringified_ref = m_orb->object_to_string(ref);
	
	PortableServer::ObjectId_var oid =
	    PortableServer::string_to_ObjectId(stringified_ref.in());
	
	CORBA::Object_var obj = 
	    m_appl_iter_poa->create_reference_with_id
	    (oid, "IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationsIterator:1.0");
	
	return CdmwPlatformMngt::ApplicationsIterator::_narrow(obj.in());
    }
    catch (const CORBA::SystemException &ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}

    /* ************************************* */

CdmwPlatformMngt::Entities*
ProxyCollection::createEntitiesFromSeq(const CdmwPlatformMngt::ElementPaths* paths) 
    throw (CORBA::SystemException)
{
    const CORBA::ULong N = paths->length();

    CdmwPlatformMngt::Entities_var lst = 
	new CdmwPlatformMngt::Entities(N);

    lst->length(N);
    
    for (CORBA::ULong i=0; i<N; ++i) {
	(lst.in())[i] = 
	    this->createEntityRef((*paths)[i]);
    }

    return lst._retn();
}

CdmwPlatformMngt::Processes*
ProxyCollection::createProcessesFromSeq(const CdmwPlatformMngt::ElementPaths* paths) 
    throw (CORBA::SystemException)
{
    const CORBA::ULong N = paths->length();

    CdmwPlatformMngt::Processes_var lst = 
	new CdmwPlatformMngt::Processes(N);

    lst->length(N);
       
    for (CORBA::ULong i=0; i<N; ++i) {
	(lst.in())[i] = 
	    this->createProcessRef((*paths)[i]);
    }

    return lst._retn();
}

CdmwPlatformMngt::Hosts*
ProxyCollection::createHostsFromSeq(const CdmwPlatformMngt::ElementPaths* paths) 
    throw (CORBA::SystemException)
{
    const CORBA::ULong N = paths->length();

    CdmwPlatformMngt::Hosts_var lst = 
	new CdmwPlatformMngt::Hosts(N);

    lst->length(N);
       
    for (CORBA::ULong i=0; i<N; ++i) {
	(lst.in())[i] = 
	    this->createHostRef((*paths)[i]);
    }

    return lst._retn();
}

CdmwPlatformMngt::Applications*
ProxyCollection::createApplicationsFromSeq(const CdmwPlatformMngt::ElementPaths* paths) 
    throw (CORBA::SystemException)
{
    const CORBA::ULong N = paths->length();

    CdmwPlatformMngt::Applications_var lst = 
	new CdmwPlatformMngt::Applications(N);

    lst->length(N);
       
    for (CORBA::ULong i=0; i<N; ++i) {
	(lst.in())[i] = 
	    this->createApplicationRef((*paths)[i]);
    }

    return lst._retn();
}

CdmwPlatformMngt::ManagedElement_ptr
ProxyCollection::createManagedElement(const char *element_path, 
				      CdmwPlatformMngt::ElementType type)
    throw (CORBA::SystemException) 
{
    try {
	switch (type) {
	case CdmwPlatformMngt::SYSTEM_TYPE: {
	    return this->createSystemRef(); // singleton
	    break;
	}
	case CdmwPlatformMngt::HOST_TYPE: {
	    return this->createHostRef(element_path);
	    break;
	}
	case CdmwPlatformMngt::APPLICATION_TYPE: {
	    return this->createApplicationRef(element_path);
	    break;
	}
	case CdmwPlatformMngt::PROCESS_TYPE: {
	    return this->createProcessRef(element_path);
	    break;
	}
	case CdmwPlatformMngt::ENTITY_TYPE: {
	    return this->createEntityRef(element_path);
	    break;
	}
	default:
	    return CdmwPlatformMngt::ManagedElement::_nil();
	} // end switch  
    }
    catch (const CORBA::SystemException & ex) {
	throw;
    }
    catch (...) {
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_MAYBE);
    }
}  


} // End of namespace Proxy
} // End of namespace NamingAndRepository
} // End of namespace Cdmw
