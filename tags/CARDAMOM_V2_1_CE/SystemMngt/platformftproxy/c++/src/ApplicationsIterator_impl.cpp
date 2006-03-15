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


#include "platformftproxy/ApplicationsIterator_impl.hpp"

#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>


namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{


// ----------------------------------------------------------------------
// Application_impl class.
// ----------------------------------------------------------------------


ApplicationsIterator_impl::ApplicationsIterator_impl(CORBA::ORB_ptr orb,
						     ProxyCollection *proxy_collection)
    throw (Cdmw::BadParameterException, 
	   Cdmw::OutOfMemoryException)                  
    : ProxyUtil(orb),
      m_proxy_collection(proxy_collection)
{        
    m_proxy_collection->add_ref();
}


ApplicationsIterator_impl::~ApplicationsIterator_impl () throw ()
{        
    m_proxy_collection->remove_ref();
}

CdmwPlatformMngt::ElementPathsIterator_ptr
ApplicationsIterator_impl::getTarget() 
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
 * IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationsIterator/reset:1.0
 * operation
 */
void 
ApplicationsIterator_impl::reset()
    throw(CORBA::SystemException) 
{
    CdmwPlatformMngt::ElementPathsIterator_var target = this->getTarget();
    
    target->reset();
}
    


/**
 * Purpose:
 * <p>
 * implements the
 * IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationsIterator/next_one:1.0
 * operation
 */
CORBA::Boolean 
ApplicationsIterator_impl::next_one(CdmwPlatformMngt::Application_out entity)
    throw(CORBA::SystemException)
{
    CdmwPlatformMngt::ElementPathsIterator_var target = this->getTarget();
 
    CORBA::String_var element_path;

    CORBA::Boolean    succeed =
	target->next_one(element_path.out());

    if (succeed) {
	entity = m_proxy_collection->createApplicationRef(element_path.ptr());
    }

    return succeed; 
}


/**
 * Purpose:
 * <p>
 * implements the
 * IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationsIterator/next_n:1.0
 * operation
 */
CORBA::Boolean 
ApplicationsIterator_impl::next_n(CORBA::ULong how_many,
			      CdmwPlatformMngt::Applications_out entities)
    throw(CORBA::SystemException)
{
    CdmwPlatformMngt::ElementPathsIterator_var target = this->getTarget();
 
    ::CdmwPlatformMngt::ElementPaths_var element_paths;

    CORBA::Boolean    succeed =
	target->next_n(how_many, element_paths.out());

    if (succeed) {
	entities = m_proxy_collection->createApplicationsFromSeq(element_paths.ptr());
    }

    return succeed; 
}


/**
 * Purpose:
 * <p>
 * implements the
 * IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationsIterator/destroy:1.0
 * operation
 */
void 
ApplicationsIterator_impl::destroy()
    throw(CORBA::SystemException) 
{
    CdmwPlatformMngt::ElementPathsIterator_var target = this->getTarget();
    
    target->destroy(); // throws OBJECT_NOT_EXIST in case of multiple
		       // invocations
}




          
} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw


