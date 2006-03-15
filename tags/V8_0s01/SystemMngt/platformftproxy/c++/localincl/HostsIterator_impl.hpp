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


#ifndef INCL_PLATFORMMNGT_PROXY_HOSTS_ITERATOR_IMPL_HPP
#define INCL_PLATFORMMNGT_PROXY_HOSTS_ITERATOR_IMPL_HPP


#include <Foundation/orbsupport/OrbSupport.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtHost.skel.hpp>

#include "platformftproxy/ProxyUtil.hpp"
#include "platformftproxy/ProxyCollection.hpp"

namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{

    class Host_impl;

/**
 *Purpose:
 *<p> Provides the HostsIterator
 */
class HostsIterator_impl : virtual public POA_CdmwPlatformMngt::HostsIterator,
			   virtual public PortableServer::RefCountServantBase,
			   public ProxyUtil
 
{
private:
    /**
     *Purpose:
     *<p> The proxy collection
     */
    Cdmw::PlatformMngt::Proxy::ProxyCollection *m_proxy_collection;

public:

    /**
     *Purpose:
     *<p> Constructor.
     *  
     *@param orb                The ORB 
     *@param m_proxy_collection The proxy collection provides operations for object reference creation
     *
     *@exception BadParameterException If one argument is invalid.
     *@exception OutOfMemoryException  Lack of Memory.
     *
     */
     HostsIterator_impl (CORBA::ORB_ptr orb,
			 Cdmw::PlatformMngt::Proxy::ProxyCollection *proxy_collection)
	 throw (Cdmw::BadParameterException, 
		Cdmw::OutOfMemoryException);

private:
     /**
     *Purpose:
     *<p> Default constructor is not allowed.
     *  
     *
     *@exception BadParameterException
     *@exception OutOfMemoryException
     *
     */
     HostsIterator_impl ()     
	 throw (Cdmw::BadParameterException,
		Cdmw::OutOfMemoryException);

      
public:           

    /**
     *Purpose:
     *<p> Destructor.
     */
    virtual 
    ~HostsIterator_impl() throw();
    
	
    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/HostsIterator/reset:1.0
    * operation
    */
    virtual void reset()
        throw(CORBA::SystemException);


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/HostsIterator/next_one:1.0
    * operation
    */
    virtual CORBA::Boolean next_one(CdmwPlatformMngt::Host_out entity)
        throw(CORBA::SystemException);


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/HostsIterator/next_n:1.0
    * operation
    */
    virtual CORBA::Boolean next_n(CORBA::ULong how_many,
                                  CdmwPlatformMngt::Hosts_out entities)
        throw(CORBA::SystemException);
    

    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/HostsIterator/destroy:1.0
    * operation
    */
    virtual void destroy()
        throw(CORBA::SystemException);

private:
    /**
     *Purpose:
     *<p> convert delivered object id into target object reference
     */
    CdmwPlatformMngt::ElementPathsIterator_ptr getTarget() 
	throw (CORBA::SystemException);
};

} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_PROXY_HOSTS_ITERATOR_IMPL_HPP

