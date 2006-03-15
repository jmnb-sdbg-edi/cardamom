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


#ifndef INCL_CDMW_PLATFORMMNGT_PROXY_SYSTEM_PROXY_HPP
#define INCL_CDMW_PLATFORMMNGT_PROXY_SYSTEM_PROXY_HPP

#include <Foundation/orbsupport/CORBASystemExceptionWrapper.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp>
#include "platformftproxy/AccessorLookupService.hpp"
#include "platformftproxy/ProxyCollection.hpp"

namespace Cdmw {
namespace PlatformMngt {
namespace Proxy {

    class SystemProxy
    {
    private:
	/**
         * Purpose:
         * <p> True if deactivate has been called otherwise false.
         */
	bool m_deactivated;

    public:        
	/**
         * Purpose:
         * <p> Constructor of CDMW SystemMngt Proxy servant.
         * 
         *@param orb A reference to the ORB pseudo-object
         *
         *@param parent_poa The parent POA used by the Proxy to spawn sub-POAs
         *
         *@param proxy_name The unique name assigned to system in config file
	 *
         *@param lookup_service The service to lookup the accessor
         *object references; used to startup proxy asynchronously from
         *SystemMngt application providing the accessors. If Accessors
         *are not present, clients receive TRANSIENT errors on
         *requests. On destruction of proxy this object will be
         *deleted.
         *
         *@exception Cdmw::OrbSupport::CORBASystemExceptionWrapper
         */        

	SystemProxy(CORBA::ORB_ptr orb,
		    PortableServer::POA_ptr parent_poa,
		    const std::string &proxy_name,
		    AccessorLookupService *lookup_service)
	    throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper);
	/**
         * Purpose: <p> Deactivation of SystemMngt Proxy; it support
         * blocking and non-blocking mode.
	 *
	 * Deactivates the proxy; all POAs are going to be
	 * destroyed. This operation is mainly used to realise
	 * specific unit_tests; This oepration is not thread safe.
         * 
	 * @param wait_for_completion if true operation blocks
	 * until all pending requests have been processed; otherwise
	 * the operation returns immediatly; In any case all pending
	 * requests are processed completly.
	 */
	void deactivate(bool wait_for_completion) throw();
	
	/**
         * Purpose: <p> Destructor of CDMW SystemMngt Proxy servant;
         * may block until all pending requests have been handled by
         * proxy (blocking/nonblocking depends on implementation)
         * 
	 * Deactivates the proxy; all POAs and servants are going to
	 * be destroyed; this may happen asynchronously.
	 */
	~SystemProxy() throw();

        /**
         * Purpose:
         * <p> returns a CDMW System Proxy object.
         * 
         *@return A reference to a CDMW PlatformMngt proxy
         *
         *@exception Cdmw::OrbSupport::CORBASystemExceptionWrapper
         */        
        CdmwPlatformMngt::System_ptr
	getReference()
            throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper);
	
    private:
	/** The proxy implementation */
	Cdmw::PlatformMngt::Proxy::ProxyCollection *m_proxy_collection; 
    }; // End class SystemProxy

}; // namespace Proxy
}; // namespace PlatformMngt
}; // namespace Cdmw

#endif // INCL_CDMW_PLATFORMMNGT_PROXY_SYSTEM_PROXY_HPP

