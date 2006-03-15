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


#include "platformadmin/Service.hpp"
#include "platformadmin/ServiceParser.hpp"
#include "platformadmin/ConfigFileParser.hpp"

extern bool check_only;

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

/**
 * class Service
 */
CdmwPlatformMngtService::ServiceDef* Service::get_definition(
    const ServiceParser& serviceParser )
{
    try
    {
        // Get a service's service provider identifier definition
        CdmwPlatformMngtService::ServiceProviderID_var serviceProviderId
            = new CdmwPlatformMngtService::ServiceProviderID();

        serviceProviderId->application_name = CORBA::string_dup(
            serviceParser.application_name().c_str() );
        serviceProviderId->process_name = CORBA::string_dup(
            serviceParser.process_name().c_str() );
        serviceProviderId->host_name = CORBA::string_dup(
            serviceParser.host_name().c_str() );
        
        // Get the service definition
        CdmwPlatformMngtService::ServiceDef_var serviceDef
            = new CdmwPlatformMngtService::ServiceDef();
            
        serviceDef->service_name = CORBA::string_dup(
            serviceParser.name().c_str() );
        serviceDef->service_provider = serviceProviderId.in();

        return serviceDef._retn();
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
}

/**
 * class ServiceContainer
 */
ServiceContainer::ServiceContainer(
    CdmwPlatformMngtService::RWServiceDefContainer_ptr serviceContainer )
{
    if( !check_only )
        m_service_container
            = CdmwPlatformMngtService::RWServiceDefContainer::_duplicate(
                serviceContainer );
}

void ServiceContainer::add_service(
    const char* serviceConfigFileName )
{
    // Open the service configuration file
    ConfigFileParser parser( serviceConfigFileName );

    // Get a service definition
    CdmwPlatformMngtService::ServiceDef_var serviceDef
        = Service::get_definition( parser.get_service() );

    if( !check_only )
        m_service_container->add_service_def( serviceDef.in() );
}

void ServiceContainer::remove_service(
    const char* serviceName )
{
    if( !check_only )
        m_service_container->remove_service_def( serviceName );
}

} // End namespace PlatformAdmin
} // End namespace Cdmw

