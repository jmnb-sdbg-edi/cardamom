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


#ifndef INCL_CDMW_PLATFORMMNGT_PROXY_DEFAULT_ACCESSOR_LOOKUP_SERVICE_HPP
#define INCL_CDMW_PLATFORMMNGT_PROXY_DEFAULT_ACCESSOR_LOOKUP_SERVICE_HPP

#include <SystemMngt/idllib/CdmwFTSystMngtSystem.stub.hpp> 
#include <SystemMngt/idllib/CdmwFTSystMngtApplication.stub.hpp> 
#include <SystemMngt/idllib/CdmwFTSystMngtProcess.stub.hpp> 
#include <SystemMngt/idllib/CdmwFTSystMngtEntity.stub.hpp> 
#include <SystemMngt/idllib/CdmwFTSystMngtHost.stub.hpp> 

#include "platformftproxy/AccessorLookupService.hpp"

namespace Cdmw {
namespace PlatformMngt {
namespace Proxy {

/**
 * Purpose: <p> Provides default implementation of
 * AccessorLookupService
 */
class DefaultAccessorLookupService: public AccessorLookupService
{
public:  
    /**
     * Purpose:
     * <p> Constructor
     *
     *@param system_accessor The system_accessor
     * 
     *@exception  Cdmw::BadParameterException If _duplicate() on argument fails.
     */     
    DefaultAccessorLookupService(CdmwFTSystMngt::SystemAccessor_ptr system_accessor) 
	throw (Cdmw::BadParameterException,
	       Cdmw::OutOfMemoryException);
    
    /**
     * Purpose:
     * <p> Destructor, enforcing declaration as virtual.
     */     
    virtual ~DefaultAccessorLookupService() throw();
    
    /**
     * Purpose:
     * <p> Returns the accessor if present, otherwise throws
     * AccessorUndefined exception 
     *
     * @exception AccessorUndefined the object reference is not
     * known yet
     */      
    virtual CdmwFTSystMngt::SystemAccessor_ptr      getSystemAccessor()
	throw (AccessorUndefined);

    /**
     * Purpose:
     * <p> Returns the accessor if present, otherwise throws
     * AccessorUndefined exception 
     *
     * @exception AccessorUndefined the object reference is not
     * known yet
     */ 
    virtual CdmwFTSystMngt::ApplicationAccessor_ptr getApplicationAccessor()
	throw (AccessorUndefined);

    /**
     * Purpose:
     * <p> Returns the accessor if present, otherwise throws
     * AccessorUndefined exception 
     *
     * @exception AccessorUndefined the object reference is not
     * known yet
     */ 
    virtual CdmwFTSystMngt::ProcessAccessor_ptr     getProcessAccessor() 
	throw (AccessorUndefined);

    /**
     * Purpose:
     * <p> Returns the accessor if present, otherwise throws
     * AccessorUndefined exception 
     *
     * @exception AccessorUndefined the object reference is not
     * known yet
     */ 
    virtual CdmwFTSystMngt::HostAccessor_ptr        getHostAccessor()
	throw (AccessorUndefined);

    /**
     * Purpose:
     * <p> Returns the accessor if present, otherwise throws
     * AccessorUndefined exception 
     *
     * @exception AccessorUndefined the object reference is not
     * known yet
     */ 
    virtual CdmwFTSystMngt::EntityAccessor_ptr      getEntityAccessor() 
	throw (AccessorUndefined);
    
private:
    /**
     * Purpose:
     * <p> Default Constructor not allowed
     */    
    DefaultAccessorLookupService(); 
    
private:
    /**
     * Purpose:
     * <p> The system accessor
     */    
    CdmwFTSystMngt::SystemAccessor_var m_system_accessor;
    
    
}; // End class DefaultAccessorLookupService

}; // namespace Proxy
}; // namespace PlatformMngt
}; // namespace Cdmw

#endif // INCL_CDMW_PLATFORMMNGT_PROXY_DEFAULT_ACCESSOR_LOOKUP_SERVICE_HPP

