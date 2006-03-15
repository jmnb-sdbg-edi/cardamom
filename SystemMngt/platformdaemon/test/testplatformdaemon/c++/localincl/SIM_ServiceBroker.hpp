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


#ifndef INCL_PLATFORMMNGT_SIM_SERVICE_BROKER_HPP
#define INCL_PLATFORMMNGT_SIM_SERVICE_BROKER_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtServiceBroker.skel.hpp"

namespace Cdmw
{
namespace PlatformMngt
{


class SIM_ServiceBroker : public virtual POA_CdmwPlatformMngtService::ServiceBroker,
                           public virtual PortableServer::RefCountServantBase
{

public:

    /**
     *Purpose:
     *<p> Constructor.
     */
    SIM_ServiceBroker()
    {
    }

    /**
     *Purpose:
     *<p> Destructor.
     */
    virtual ~SIM_ServiceBroker()
    {
    }

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ServiceBroker/get_service:1.0
     * operator
     */
    CORBA::Object_ptr get_service(
        const char* application_name,
        const char* process_name,
        const char* host_name,
        const char* service_name )
        throw( CdmwPlatformMngtService::ServiceNotFound,
               CdmwPlatformMngtService::ServiceUnavailable,
               CORBA::SystemException )
    {
        return CORBA::Object::_nil();
    }


#ifdef CDMW_USE_FAULTTOLERANCE
    void set_global_service(const char* service_name, CORBA::Object_ptr service_object)
        throw( CdmwPlatformMngtService::ServiceAlreadyExists,
               CORBA::SystemException )
    {
    }
#endif

};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_SIM_SERVICE_BROKER_HPP

