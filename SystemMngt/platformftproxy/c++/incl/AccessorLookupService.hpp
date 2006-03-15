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


#ifndef INCL_CDMW_PLATFORMMNGT_PROXY_ACCESSOR_LOOKUP_SERVICE_HPP
#define INCL_CDMW_PLATFORMMNGT_PROXY_ACCESSOR_LOOKUP_SERVICE_HPP

#include <Foundation/orbsupport/CORBASystemExceptionWrapper.hpp>
#include <SystemMngt/idllib/CdmwFTSystMngtSystem.stub.hpp> 
#include <SystemMngt/idllib/CdmwFTSystMngtApplication.stub.hpp> 
#include <SystemMngt/idllib/CdmwFTSystMngtProcess.stub.hpp> 
#include <SystemMngt/idllib/CdmwFTSystMngtEntity.stub.hpp> 
#include <SystemMngt/idllib/CdmwFTSystMngtHost.stub.hpp> 

namespace Cdmw {
namespace PlatformMngt {
namespace Proxy {
    /**
     * Purpose:
     * <p> Raised if the requested Accessor is not present currently 
     */
    class AccessorUndefined : Cdmw::Exception {
    public:
	AccessorUndefined() 
	    :  Cdmw::Exception(Cdmw::Exception::SAFE, "accessor undefined")
	{
	}     

	virtual ~AccessorUndefined() throw() {}
    };
    
    /**
     * Purpose: <p> Declares methods the proxy is using to fetch
     * specific accessors it operates on. The proxy is instanciated
     * with object that implements this interface. This allows async
     * creation of proxy and SystemMngt process. The first time the
     * proxy receives request from client it will invoke one of these
     * operations. On AccessorUndefined exception the proxy will throw
     * CORBA::TRANSIENT exception back to client.  Note: the
     * implementation of these methods must be thread safe!
     */
    class AccessorLookupService
    {
    public:  
	/**
	 * Purpose:
	 * <p> Destructor, enforcing declaration as virtual.
	 */     
	virtual ~AccessorLookupService() throw() {}

	/**
	 * Purpose:
	 * <p> Returns the accessor if present, otherwise throws
	 * AccessorUndefined exception 
	 *
	 * @exception AccessorUndefined the object reference is not
	 * known yet
	 */      
	virtual CdmwFTSystMngt::SystemAccessor_ptr      getSystemAccessor()
	    throw (AccessorUndefined) = 0;
    	/**
	 * Purpose:
	 * <p> Returns the accessor if present, otherwise throws
	 * AccessorUndefined exception 
	 *
	 * @exception AccessorUndefined the object reference is not
	 * known yet
	 */ 
	virtual CdmwFTSystMngt::ApplicationAccessor_ptr getApplicationAccessor()
	    throw (AccessorUndefined) = 0;
	/**
	 * Purpose:
	 * <p> Returns the accessor if present, otherwise throws
	 * AccessorUndefined exception 
	 *
	 * @exception AccessorUndefined the object reference is not
	 * known yet
	 */ 
	virtual CdmwFTSystMngt::ProcessAccessor_ptr     getProcessAccessor() 
	    throw (AccessorUndefined) = 0;
	/**
	 * Purpose:
	 * <p> Returns the accessor if present, otherwise throws
	 * AccessorUndefined exception 
	 *
	 * @exception AccessorUndefined the object reference is not
	 * known yet
	 */ 
	virtual CdmwFTSystMngt::HostAccessor_ptr        getHostAccessor()
	    throw (AccessorUndefined) = 0;
	/**
	 * Purpose:
	 * <p> Returns the accessor if present, otherwise throws
	 * AccessorUndefined exception 
	 *
	 * @exception AccessorUndefined the object reference is not
	 * known yet
	 */ 
	virtual CdmwFTSystMngt::EntityAccessor_ptr      getEntityAccessor() 
	    throw (AccessorUndefined) = 0;

    }; // End class AccessorLookupService

}; // namespace Proxy
}; // namespace PlatformMngt
}; // namespace Cdmw

#endif // INCL_CDMW_PLATFORMMNGT_PROXY_ACCESSOR_LOOKUP_SERVICE_HPP

