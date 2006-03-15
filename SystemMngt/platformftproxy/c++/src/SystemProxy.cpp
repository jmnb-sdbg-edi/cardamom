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

#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "platformftproxy/InitUtils.hpp"
#include "platformftproxy/System_impl.hpp"

#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{

//  CdmwPlatformMngt::System_ptr
SystemProxy::SystemProxy
(CORBA::ORB_ptr orb,
 PortableServer::POA_ptr parent_poa,
 const std::string &proxy_name,
 AccessorLookupService *lookup_service)
    throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper)
    : m_deactivated(false)
{
    try {
	m_proxy_collection = new ProxyCollection(orb, 
						 parent_poa, 
						 lookup_service,
						 proxy_name);
    }
    catch (const Cdmw::BadParameterException & ex) {
	// FIXME through propper CORBASystemExceptionWrapper exception 
	throw CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAM, 
			       CORBA::COMPLETED_NO);    
    }
    catch (...) {  // FIXME verify exceptions raised
	throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
			      CORBA::COMPLETED_NO);
    }
}
  
CdmwPlatformMngt::System_ptr
SystemProxy::getReference() 
    throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper)
{
    try {
	// FIXME verify exceptions raised
	CdmwPlatformMngt::System_ptr result = 
	    this->m_proxy_collection->createSystemRef();
   
	return result; 
    } catch (...) { // FIXME verify exceptions raised
	 throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
     }

}

// Not thread safe
void SystemProxy::deactivate(bool wait_for_completion) 
    throw()
{
    if (!m_deactivated) { //FIXME protect with mutex
	try {
	    m_deactivated = true;
	    m_proxy_collection->destroy(wait_for_completion);
	}
	catch (...) {
	    // ignore
	}
    }
}
	
SystemProxy::~SystemProxy() throw() {
    try {
	deactivate (true); // blocking 
    } 
    catch (...) {
	// ignore
    }
}
    
} // End of namespace Proxy
} // End of namespace PlatformMngt
} // End of namespace Cdmw
