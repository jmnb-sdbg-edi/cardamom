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


#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/ossupport/OS.hpp>

#include "platformftproxy/DefaultAccessorLookupService.hpp"

namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{

DefaultAccessorLookupService::DefaultAccessorLookupService
(CdmwFTSystMngt::SystemAccessor_ptr system_accessor) 
    throw (Cdmw::BadParameterException,  
	   Cdmw::OutOfMemoryException)
{
    try {
	m_system_accessor = 
	    CdmwFTSystMngt::SystemAccessor::_duplicate(system_accessor);
    }
    catch (const std::bad_alloc &) {
	CDMW_THROW(Cdmw::OutOfMemoryException);
    }
    catch (...) {
	CDMW_THROW2(Cdmw::BadParameterException, "system_accessor", "");
    }
}   
    
DefaultAccessorLookupService::~DefaultAccessorLookupService() 
    throw () 
{
}
    
CdmwFTSystMngt::SystemAccessor_ptr          
DefaultAccessorLookupService::getSystemAccessor()
    throw (AccessorUndefined) 
{
    try {
	return CdmwFTSystMngt::SystemAccessor::_duplicate(m_system_accessor.in());
    }
    catch (...) {
	throw AccessorUndefined();
    }
}

CdmwFTSystMngt::ApplicationAccessor_ptr     
DefaultAccessorLookupService::getApplicationAccessor()
    throw (AccessorUndefined) 
{
    try {
	return m_system_accessor->get_application_accessor();
    }
    catch (...) {
	throw AccessorUndefined();
    }
}

CdmwFTSystMngt::ProcessAccessor_ptr         
DefaultAccessorLookupService::getProcessAccessor() 
    throw (AccessorUndefined) 
{
    try {
	return m_system_accessor->get_process_accessor();
    }
    catch (...) {
	throw AccessorUndefined();
    }
}

CdmwFTSystMngt::HostAccessor_ptr            
DefaultAccessorLookupService::getHostAccessor()
    throw (AccessorUndefined) 
{
    try {
	return m_system_accessor->get_host_accessor();
    }
    catch (...) {
	throw AccessorUndefined();
    }
}

CdmwFTSystMngt::EntityAccessor_ptr          
DefaultAccessorLookupService::getEntityAccessor() 
    throw (AccessorUndefined)
{
    try {
	return m_system_accessor->get_entity_accessor();
    }
    catch (...) {
	throw AccessorUndefined();
    }
}

} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw


