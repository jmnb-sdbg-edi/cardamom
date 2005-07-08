/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your 
 * option) any later version. 
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public 
 * License for more details. 
 * 
 * You should have received a copy of the GNU Library General 
 * Public License along with CARDAMOM; see the file COPYING. If not, write to 
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


#ifndef INCL_PLATFORMMNGT_SERVICE_BINDING_HPP
#define INCL_PLATFORMMNGT_SERVICE_BINDING_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtServiceContainer.skel.hpp"
#include "SystemMngt/platformlibrary/Configuration.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class System_impl;

/**
 * The service binding of system.
 */
class ServiceBinding_impl : virtual public POA_CdmwPlatformMngtService::ServiceBinding,
                            virtual public PortableServer::RefCountServantBase
{



public:

    /**
     *Purpose:
     *<p> Constructor.
     */
    ServiceBinding_impl (System_impl* system)
            throw (BadParameterException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ServiceBinding/get_service:1.0
     * operator
     */
    CORBA::Object_ptr get_service (const CdmwPlatformMngtService::ServiceProviderID& service_provider)
            throw (CdmwPlatformMngtService::ServiceProviderNotFound,
                   CdmwPlatformMngtService::ServiceUnavailable,
                   CORBA::SystemException);
                   
                   
private:

    /**
     *Purpose:
     *<p> Default constructor is not allowed.
     */
    ServiceBinding_impl()
    {};

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    ServiceBinding_impl (ServiceBinding_impl& rhs)
    {};

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    ServiceBinding_impl& operator= (const ServiceBinding_impl& rhs)       
    {
        return *this;
    };
    
    
    /**
     * The system managing this process observer.
     */
    System_impl* m_system;


};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_SERVICE_BINDING_HPP

