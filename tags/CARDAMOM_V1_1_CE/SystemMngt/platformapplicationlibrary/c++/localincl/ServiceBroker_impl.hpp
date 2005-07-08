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


#ifndef INCL_PLATFORMMNGT_SERVICE_BROKER_HPP
#define INCL_PLATFORMMNGT_SERVICE_BROKER_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtServiceBroker.skel.hpp"
#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "SystemMngt/platformlibrary/MultipleIdServant_impl.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class Application_impl;

/**
 * The service broker of an application.
 */
class ServiceBroker_impl : public virtual POA_CdmwPlatformMngtService::ServiceBroker,
                           public virtual MultipleIdServant_impl,
                           public virtual PortableServer::RefCountServantBase
{
private:
    /**
     * The application managing this process observer.
     */
    Application_impl* m_application;

    /**
     * The service binding.
     */
    CdmwPlatformMngtService::ServiceBinding_var m_serviceBinding;

#ifdef CDMW_USE_FAULTTOLERANCE
    /*
     * 
     */
    typedef std::map< std::string,  CORBA::Object_var> GlobalsServices;
    
    GlobalsServices m_globalServices;
#endif


private:
    /**
     *Purpose:
     *<p> Default constructor is not allowed.
     */
    ServiceBroker_impl()
    : MultipleIdServant_impl( Configuration::M_orb.in(), "ServiceBrokerPOA" )
    {};

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    ServiceBroker_impl( ServiceBroker_impl& rhs )
    : MultipleIdServant_impl( Configuration::M_orb.in(), "ServiceBrokerPOA" )
    {};

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    ServiceBroker_impl& operator=(
        const ServiceBroker_impl& rhs )
    {
        return *this;
    };

public:
    /**
     *Purpose:
     *<p> Constructor.
     */
    ServiceBroker_impl(
        Application_impl* application,
        CdmwPlatformMngtService::ServiceBinding_ptr serviceBinding )
            throw( BadParameterException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ServiceBroker/get_service:1.0
     * operator
     */
    CORBA::Object_ptr get_service(
        const char* service_name )
        throw( CdmwPlatformMngtService::ServiceNotFound,
               CdmwPlatformMngtService::ServiceUnavailable,
               CORBA::SystemException );

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

#endif //INCL_PLATFORMMNGT_SERVICE_BROKER_HPP

