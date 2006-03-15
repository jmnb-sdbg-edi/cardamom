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


#ifndef INCL_PLATFORMMNGT_PROXY_PROXY_COLLECTION_HPP
#define INCL_PLATFORMMNGT_PROXY_PROXY_COLLECTION_HPP

#include "platformftproxy/AccessorLookupService.hpp"

#include <SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtApplicationBase.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtApplication.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtService.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtEntity.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtHost.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtProcessObserver.stub.hpp>
#include <SystemMngt/idllib/CdmwFTSystMngtSystem.stub.hpp>

#include <Foundation/osthreads/Mutex.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/orbsupport/ObjectTraits.hpp>
#include <sstream>
#include <list>


namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{

/**
 *Purpose <p> Provides the ProxyCollection
 */
class ProxyCollection {

public:   
    /**
     *Purpose: <p>  constructor

     *The ref_counter is initialised by one; call operation remove_ref
     *to delete object
     *
     *@param orb
     *@param parent_poa
     *@param lookup_service  The lookup service object will be deleted on destruction 
     *@param proxy_name The unique name of proxy.
     *
     *@exception Cdmw::BadParameterException   If invalid parameter.
     *@exception Cdmw::OutOfResourcesException If creating POA policy fails.
     *@exception Cdmw::OutOfMemoryException    Lack of memory
     */
    ProxyCollection (CORBA::ORB_ptr orb, 
		   PortableServer::POA_ptr parent_poa,
		     AccessorLookupService *lookup_service,
		   std::string proxy_name) 
	throw (Cdmw::BadParameterException,
	       Cdmw::OutOfResourcesException,
	       Cdmw::OutOfMemoryException);
private: 
    /**
     *Purpose: <p>  Default constructor not allowed
     *
     *@exception Cdmw::BadParameterException   If invalid parameter.
     *@exception Cdmw::OutOfResourcesException If creating POA policy fails.
     *@exception Cdmw::OutOfMemoryException    Lack of memory
     */
    ProxyCollection() 
	throw (Cdmw::BadParameterException,
	       Cdmw::OutOfResourcesException,
	       Cdmw::OutOfMemoryException);
 
protected:   
    /**
     *Purpose: <p> Destructor not allowed, use operation
     *"destroy(bool)" instead.
     */
    virtual ~ProxyCollection() 
	throw();

public:    
    /**
     *Purpose: <p> 

     * Terminates operation of proxy; all further requests are blocked
     * and resoruces are free-d including sub-POAs. This operation
     * must be called at most once. The operation decrements the
     * ref_counter by one.
     * 
     * @parameter wait_for_completion if true the function returns after
     * all pending requests have been processed, otherwise it returns
     * immediatly. Pending requests are processed in any case.
     * 
     */ 
    void destroy(bool wait_for_completion) 
	throw();

    /**
     *Purpose: <p> Increments ref counter by one
     */
    void add_ref() throw ();

    /**
     *Purpose: <p> Decrements ref counter by one, if ref_counter
     *equals 0 the the object will be deleted.
     */
    void remove_ref() throw ();

    
public:

    /**
     *Purpose:
     *<p> create object reference 
     */
    CdmwPlatformMngt::System_ptr createSystemRef() 
	throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p> create object reference 
     *
     *@param element_path The element path to be encoded into object-id
     */
    CdmwPlatformMngt::Host_ptr createHostRef(const char* element_path)
	throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p> create object reference 
     *
     *@param element_path The element path to be encoded into object-id
     */
    CdmwPlatformMngt::Process_ptr createProcessRef(const char* element_path) 
	throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p> create object reference 
     *
     *@param element_path The element path to be encoded into object-id
     */
    CdmwPlatformMngt::Application_ptr createApplicationRef(const char* element_path)
	throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p> create object reference 
     *
     *@param element_path The element path to be encoded into object-id
     */
    CdmwPlatformMngt::Entity_ptr createEntityRef(const char* element_path)
	throw (CORBA::SystemException);
    
     /* ************************************* */
   
    
    /**
     *Purpose:
     *<p> create object reference 
     *
     *@param ref The iterators object reference to be encoded into object-id
     */
    CdmwPlatformMngt::HostsIterator_ptr 
    createHostsIteratorRef(CdmwPlatformMngt::ElementPathsIterator_ptr ref) 
	throw (CORBA::SystemException);
    
     /**
     *Purpose:
     *<p> create object reference 
     *
     *@param ref The iterators object reference to be encoded into object-id
     */
   CdmwPlatformMngt::EntitiesIterator_ptr 
    createEntitiesIteratorRef(CdmwPlatformMngt::ElementPathsIterator_ptr ref) 
	throw (CORBA::SystemException);

     /**
     *Purpose:
     *<p> create object reference 
     *
     *@param ref The iterators object reference to be encoded into object-id
     */
   CdmwPlatformMngt::ProcessesIterator_ptr 
    createProcessesIteratorRef(CdmwPlatformMngt::ElementPathsIterator_ptr ref)
	throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p> create object reference 
     *
     *@param ref The iterators object reference to be encoded into object-id
     */
    CdmwPlatformMngt::ApplicationsIterator_ptr 
    createApplicationsIteratorRef(CdmwPlatformMngt::ElementPathsIterator_ptr ref) 
	throw (CORBA::SystemException);
 
    /* ************************************* */
 
    /**
     *Purpose:
     *<p> create sequence of object references 
     *
     *@param paths The sequence of element paths, each one transformed into object reference 
     */
    CdmwPlatformMngt::Entities*
    createEntitiesFromSeq(const CdmwPlatformMngt::ElementPaths* paths) 
	throw (CORBA::SystemException);
    
    /**
     *Purpose:
     *<p> create sequence of object references 
     *
     *@param paths The sequence of element paths, each one transformed into object reference     
     */
    CdmwPlatformMngt::Applications*
    createApplicationsFromSeq(const CdmwPlatformMngt::ElementPaths* paths) 
	throw (CORBA::SystemException);
    
    /**
     *Purpose:
     *<p> create sequence of object references 
     *
     *@param paths The sequence of element paths, each one transformed into object reference      
     */
    CdmwPlatformMngt::Processes*
    createProcessesFromSeq(const CdmwPlatformMngt::ElementPaths* paths) 
	throw (CORBA::SystemException);
    
    /**
     *Purpose:
     *<p> create sequence of object references 
     *
     *@param paths The sequence of element paths, each one transformed into object reference      
     */
    CdmwPlatformMngt::Hosts*
    createHostsFromSeq(const CdmwPlatformMngt::ElementPaths* paths) 
	throw (CORBA::SystemException);
    
    /* ************************************* */
 
    /**
     *Purpose:
     *<p> create object reference of specified type
     *
     *@param type The specialized type of the object reference
     *@param element_path The path to be encoded into object-id
     */
    CdmwPlatformMngt::ManagedElement_ptr
    createManagedElement(const char *element_path, 
			 CdmwPlatformMngt::ElementType type)
	throw (CORBA::SystemException);

private:
    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    ProxyCollection (ProxyCollection& rhs);

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    ProxyCollection& operator= (const ProxyCollection& rhs);

private:
    /**
     * Purpose:
     * <p> The ORB
     */
    CORBA::ORB_var          m_orb;

    /**
     * Purpose:
     * <p> The POA
     */
    PortableServer::POA_var m_host_poa;
    /**
     * Purpose:
     * <p> The POA
     */
    PortableServer::POA_var m_proc_poa;
    /**
     * Purpose:
     * <p> The POA
     */
    PortableServer::POA_var m_appl_poa;
    /**
     * Purpose:
     * <p> The POA
     */
    PortableServer::POA_var m_entity_poa;
    /**
     * Purpose:
     * <p> The POA
     */
    PortableServer::POA_var m_host_iter_poa;
    /**
     * Purpose:
     * <p> The POA
     */
    PortableServer::POA_var m_proc_iter_poa;
    /**
     * Purpose:
     * <p> The POA
     */
    PortableServer::POA_var m_appl_iter_poa;
    /**
     * Purpose:
     * <p> The POA
     */
    PortableServer::POA_var m_entity_iter_poa;	
    /**
     * Purpose:
     * <p> The POA
     */
    PortableServer::POA_var m_system_poa;
    
private:
    /**
     * Purpose:
     * <p> The lookup service will be deleted on destruction
     */ 
    AccessorLookupService *m_lookup_service;   

    /**
     * Purpose: <p> The ref counter indicates the number of living
     * servants, it is needed to realize async destruction. 
     */
    long                                         m_ref_counter;
    /**
     * Purpose: <p> The ref counter mutex protects ref_counter
     * variable.
     */
    Cdmw::OsSupport::Mutex                       m_ref_counter_mutex; 
};

} // End of namespace Proxy
} // End of namespace NamingAndRepository
} // End of namespace Cdmw

#endif
