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


#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/platformelementlibrary/RWEntityContainer_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "platformapplicationlibrary/ServiceBroker_impl.hpp"
#include "platformapplicationlibrary/ManagedProcess_impl.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

ServiceBroker_impl::ServiceBroker_impl(
    PortableServer::POA_ptr poa,
    Application_impl* application,
    CdmwPlatformMngtService::ServiceBinding_ptr serviceBinding)
throw (BadParameterException)
: DeactivableServant_impl (poa)
{
    if (application == NULL)
        CDMW_THROW2(BadParameterException, "application", "NULL");
    if (CORBA::is_nil(serviceBinding))
        CDMW_THROW2(BadParameterException, "serviceBinding", "NULL");

    m_application = application;
    m_serviceBinding = 
        CdmwPlatformMngtService::ServiceBinding::_duplicate (serviceBinding);
}

CORBA::Object_ptr ServiceBroker_impl::get_service(
    const char* application_name,
    const char* process_name,
    const char* host_name,
    const char* service_name)
throw (CdmwPlatformMngtService::ServiceNotFound,
       CdmwPlatformMngtService::ServiceUnavailable,
       CORBA::SystemException)
{
    try
    {
        CORBA::Object_var service;
        
#ifdef CDMW_USE_FAULTTOLERANCE

        GlobalsServices::iterator it = m_globalServices.find(service_name);

        
        if (it != m_globalServices.end())
        {
            service = CORBA::Object::_duplicate(it->second.in());            
            
        } 
        else
#endif  

        {         
            ManagedProcess_impl* managedProcess
                = dynamic_cast <ManagedProcess_impl*> (
                    m_application->getProcess (process_name, host_name));
                    
            if (managedProcess == NULL)
                throw CORBA::BAD_OPERATION (OrbSupport::BAD_OPERATION,
                                            CORBA::COMPLETED_NO);
            
            CdmwPlatformMngtService::ServiceProviderID_var serviceProvider =
                 managedProcess->get_service_def (service_name);
                 
            service = m_serviceBinding->get_service (serviceProvider.in());
        }
       
        return service._retn();
    }
    catch (const CdmwPlatformMngt::ProcessNotFound&)
    {
        throw CdmwPlatformMngtService::ServiceUnavailable();
    }
    catch (const OutOfMemoryException&)
    {
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch (const InternalErrorException&)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}

#ifdef CDMW_USE_FAULTTOLERANCE

void ServiceBroker_impl::set_global_service(const char* service_name, CORBA::Object_ptr service_object)
    throw (CdmwPlatformMngtService::ServiceAlreadyExists,
           CORBA::SystemException)
{
    //to do :throw the ServiceAlreadyExists
    GlobalsServices::iterator it = m_globalServices.find(service_name);
    if (it != m_globalServices.end())
    {
        throw CdmwPlatformMngtService::ServiceAlreadyExists();
    } 

    m_globalServices[service_name] = CORBA::Object::_duplicate(service_object);

}

#endif

} // End namespace PlatformMngt
} // End namespace Cdmw

