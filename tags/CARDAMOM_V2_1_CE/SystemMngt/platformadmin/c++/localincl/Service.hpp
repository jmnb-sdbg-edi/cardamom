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


#ifndef INCL_PLATFORMADMIN_SERVICE_HPP
#define INCL_PLATFORMADMIN_SERVICE_HPP

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtService.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtServiceContainer.stub.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

class ConfigException;
class ServiceParser;

class Service
{
public:
    static
    CdmwPlatformMngtService::ServiceDef* get_definition(
        const ServiceParser& serviceParser );
};

class ServiceContainer
{
private:
    CdmwPlatformMngtService::RWServiceDefContainer_var m_service_container;

protected:
    ServiceContainer(
        CdmwPlatformMngtService::RWServiceDefContainer_ptr service_container );

public:
    void add_service(
        const char* serviceConfigFileName );

    void remove_service(
        const char* serviceName );
};

} // End namespace PlatformAdmin
} // End namespace Cdmw

#endif //INCL_PLATFORMADMIN_SERVICE_HPP

