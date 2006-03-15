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


#ifndef INCL_PLATFORMMNGT_FTSERVICE_BROKER_HPP
#define INCL_PLATFORMMNGT_FTSERVICE_BROKER_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtServiceBroker.skel.hpp"
#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "SystemMngt/platformlibrary/DeactivableServant_impl.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class System_impl;

/**
 * The service broker of an application.
 */
class FTServiceBroker_impl : public virtual POA_CdmwPlatformMngtService::ServiceBroker,                           
                             public virtual PortableServer::RefCountServantBase,
                             public DeactivableServant_impl
{
private:
    /**
     * The associated system.
     */
    System_impl* m_system;

    /**
     * The service binding.
     */
    CdmwPlatformMngtService::ServiceBinding_var m_serviceBinding;

#ifdef CDMW_USE_FAULTTOLERANCE

    /*
     * 
     */
    typedef std::map <std::string, CORBA::Object_var> GlobalsServices;
    
    GlobalsServices m_globalServices;
    
#endif


private:

    /**
     *Purpose:
     *<p> Default constructor is not allowed.
     */
    FTServiceBroker_impl();

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    FTServiceBroker_impl(FTServiceBroker_impl& rhs);

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    FTServiceBroker_impl& operator=(
        const FTServiceBroker_impl& rhs);

public:
    /**
     *Purpose:
     *<p> Constructor.
     */
    FTServiceBroker_impl(
        PortableServer::POA_ptr poa,
        System_impl* system,
        CdmwPlatformMngtService::ServiceBinding_ptr serviceBinding)
            throw (BadParameterException);

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
        const char* service_name)
        throw (CdmwPlatformMngtService::ServiceNotFound,
               CdmwPlatformMngtService::ServiceUnavailable,
               CORBA::SystemException);

#ifdef CDMW_USE_FAULTTOLERANCE
    /**
     *Purpose:
     *<p>
     * Implements the 
     * IDL:thalesgroup.com/CdmwPlatformMngt/ServiceBroker/get_global_service:1.0
     * operator
     */
    void set_global_service(const char* service_name, CORBA::Object_ptr service_object)
        throw( CdmwPlatformMngtService::ServiceAlreadyExists,
               CORBA::SystemException );
#endif
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_FTSERVICE_BROKER_HPP

