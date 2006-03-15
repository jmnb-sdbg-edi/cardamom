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


#include "platformsystemlibrary/ServiceBinding_impl.hpp"
#include "platformsystemlibrary/System_impl.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtApplication.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcess.stub.hpp"

#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

ServiceBinding_impl::ServiceBinding_impl (System_impl* system)
        throw (BadParameterException)
{
    if (system == NULL)
    {
        CDMW_THROW2 (BadParameterException, "system", "NULL");
    }
            
    m_system = system;
}

CORBA::Object_ptr ServiceBinding_impl::get_service (
                                       const CdmwPlatformMngtService::ServiceProviderID& service_provider)
        throw (CdmwPlatformMngtService::ServiceProviderNotFound,
               CdmwPlatformMngtService::ServiceUnavailable,
               CORBA::SystemException)
{
    try
    {
        CdmwPlatformMngt::Application_var application = 
                    m_system->get_application (service_provider.application_name.in());
                    
        CdmwPlatformMngt::Process_var process =
                    application->get_process (service_provider.process_name.in(),
                                              service_provider.host_name.in());
                    
        CORBA::Object_var serviceObject =  process->embedded_service();
        
        if (serviceObject.in() == CORBA::Object::_nil())
        {
            throw CdmwPlatformMngtService::ServiceUnavailable();
        }
        
        return serviceObject._retn();
        
    }
    catch (const CdmwPlatformMngt::ApplicationNotFound&)
    {    
        throw CdmwPlatformMngtService::ServiceProviderNotFound();
    }
    catch (const CdmwPlatformMngt::ProcessNotFound&)
    {    
        throw CdmwPlatformMngtService::ServiceProviderNotFound();
    }
}

} // End namespace PlatformMngt
} // End namespace Cdmw

