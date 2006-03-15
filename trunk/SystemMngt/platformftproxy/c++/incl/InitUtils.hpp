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


#ifndef INCL_CDMW_PLATFORMMNGT_PROXY_INIT_UTILS_HPP
#define INCL_CDMW_PLATFORMMNGT_PROXY_INIT_UTILS_HPP

#include <Foundation/orbsupport/CORBASystemExceptionWrapper.hpp>
#include "platformftproxy/SystemProxy.hpp"
#include "platformftproxy/AccessorLookupService.hpp"

namespace Cdmw {
namespace PlatformMngt {
namespace Proxy {

    class InitUtils
    {
    public:
        /**
         * Purpose:
         * <p> Creates a CDMW Repository Proxy object.
         * 
         *@param orb A reference to the ORB pseudo-object
         *
         *@param parent_poa The parent POA used by the Proxy
         *
         *@param proxy_name The unique name assigned to sub-POA
	 *
         *@param lookup_service The service to lookup the accessor
         *object references; used to startup proxy asynchronously from
         *SystemMngt application providing the accessors. The proxy
         *takes over responsibility for the object and deletes it on
         *destruction.
	 *
         *@return A reference to a CDMW PlatformMngt proxy
         *
         *@exception Cdmw::OrbSupport::CORBASystemExceptionWrapper
         */        
        static 
	Cdmw::PlatformMngt::Proxy::SystemProxy*
        create_platformmngt_proxy
        (CORBA::ORB_ptr orb,
         PortableServer::POA_ptr parent_poa,
	 const std::string &proxy_name,
	 AccessorLookupService *lookup_service)
            throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper);

    }; // End class InitUtils   

}; // namespace Proxy
}; // namespace PlatformMngt
}; // namespace Cdmw

#endif // INCL_CDMW_PLATFORMMNGT_PROXY_INIT_UTILS_HPP

