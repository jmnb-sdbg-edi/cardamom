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


#include "platformftproxy/HostsIterator_impl.hpp"

#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>


namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{


// ----------------------------------------------------------------------
// Host_impl class.
// ----------------------------------------------------------------------


HostsIterator_impl::HostsIterator_impl(CORBA::ORB_ptr orb,
				       ProxyCollection *proxy_collection)
    throw (Cdmw::BadParameterException, 
	   Cdmw::OutOfMemoryException)                  
    : ProxyUtil(orb),
      m_proxy_collection(proxy_collection)
{        
    m_proxy_collection->add_ref();
}


HostsIterator_impl::~HostsIterator_impl () throw ()
{        
    m_proxy_collection->remove_ref();
}

CdmwPlatformMngt::ElementPathsIterator_ptr
HostsIterator_impl::getTarget() 
    throw (CORBA::SystemException) 
{
    std::string oid = this->getObjectId();

    CORBA::Object_var obj = m_orb->string_to_object(oid.c_str());
    
    return CdmwPlatformMngt::ElementPathsIterator::_narrow(obj.in());
}


/**
 * Purpose:
 * <p>
 * implements the
 * IDL:thalesgroup.com/CdmwPlatformMngt/HostsIterator/reset:1.0
 * operation
 */
void 
HostsIterator_impl::reset()
    throw(CORBA::SystemException) 
{
    CdmwPlatformMngt::ElementPathsIterator_var target = this->getTarget();
    
    target->reset();
}
    


/**
 * Purpose:
 * <p>
 * implements the
 * IDL:thalesgroup.com/CdmwPlatformMngt/HostsIterator/next_one:1.0
 * operation
 */
CORBA::Boolean 
HostsIterator_impl::next_one(CdmwPlatformMngt::Host_out entity)
    throw(CORBA::SystemException)
{
    CdmwPlatformMngt::ElementPathsIterator_var target = this->getTarget();
 
    CORBA::String_var element_path;

    CORBA::Boolean    succeed =
	target->next_one(element_path.out());

    if (succeed) {
	entity = m_proxy_collection->createHostRef(element_path.ptr());
    }

    return succeed; 
}


/**
 * Purpose:
 * <p>
 * implements the
 * IDL:thalesgroup.com/CdmwPlatformMngt/HostsIterator/next_n:1.0
 * operation
 */
CORBA::Boolean 
HostsIterator_impl::next_n(CORBA::ULong how_many,
			      CdmwPlatformMngt::Hosts_out entities)
    throw(CORBA::SystemException)
{
    CdmwPlatformMngt::ElementPathsIterator_var target = this->getTarget();
 
    ::CdmwPlatformMngt::ElementPaths_var element_paths;

    CORBA::Boolean    succeed =
	target->next_n(how_many, element_paths.out());

    if (succeed) {
	entities = m_proxy_collection->createHostsFromSeq(element_paths.ptr());
    }

    return succeed; 
}


/**
 * Purpose:
 * <p>
 * implements the
 * IDL:thalesgroup.com/CdmwPlatformMngt/HostsIterator/destroy:1.0
 * operation
 */
void 
HostsIterator_impl::destroy()
    throw(CORBA::SystemException) 
{
    CdmwPlatformMngt::ElementPathsIterator_var target = this->getTarget();
    
    target->destroy(); // throws OBJECT_NOT_EXIST in case of multiple
		       // invocations
}




          
} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw


