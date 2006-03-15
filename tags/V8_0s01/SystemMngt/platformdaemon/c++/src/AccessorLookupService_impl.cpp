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


#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"

#include "platformdaemon/AccessorLookupService_impl.hpp"
#include "SystemMngt/platformlibrary/Configuration.hpp"

#include "FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp"

namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{

AccessorLookupService_impl::AccessorLookupService_impl(ServiceRegistration_impl* serviceRegistration) 
    throw (Cdmw::BadParameterException,  
	   Cdmw::OutOfMemoryException)
    : m_serviceRegistration(serviceRegistration),
      m_is_initialised(false)
{

}   
    
AccessorLookupService_impl::~AccessorLookupService_impl() 
    throw () 
{
}
    
CdmwFTSystMngt::SystemAccessor_ptr          
AccessorLookupService_impl::getSystemAccessor()
    throw (AccessorUndefined) 
{
    // first check if accessors are ready
    initialise();
            
    try
    {
        return CdmwFTSystMngt::SystemAccessor::_duplicate(m_system_accessor.in());
    }
    catch (...) 
    {
        throw AccessorUndefined();
    }
}

CdmwFTSystMngt::ApplicationAccessor_ptr     
AccessorLookupService_impl::getApplicationAccessor()
    throw (AccessorUndefined) 
{
    // first check if accessors are ready
    initialise();
     
    try 
    {
        return  CdmwFTSystMngt::ApplicationAccessor::_duplicate(m_application_accessor.in());
    }
    catch (...)
    {
        throw AccessorUndefined();
    }
}

CdmwFTSystMngt::ProcessAccessor_ptr         
AccessorLookupService_impl::getProcessAccessor() 
    throw (AccessorUndefined) 
{
    // first check if accessors are ready
    initialise();
    
    try 
    {
        return CdmwFTSystMngt::ProcessAccessor::_duplicate(m_process_accessor.in());
    }
    catch (...)
    {
        throw AccessorUndefined();
    }
}

CdmwFTSystMngt::HostAccessor_ptr            
AccessorLookupService_impl::getHostAccessor()
    throw (AccessorUndefined) 
{
    // first check if accessors are ready
    initialise();
    
    try 
    {
        return CdmwFTSystMngt::HostAccessor::_duplicate(m_host_accessor.in());
    }
    catch (...) 
    {
        throw AccessorUndefined();
    } 
}

CdmwFTSystMngt::EntityAccessor_ptr          
AccessorLookupService_impl::getEntityAccessor() 
    throw (AccessorUndefined)
{
    // first check if accessors are ready
    initialise();
    
    try 
    {
        return CdmwFTSystMngt::EntityAccessor::_duplicate(m_entity_accessor.in());
    }
    catch (...) 
    {
        throw AccessorUndefined();
    }
}

void AccessorLookupService_impl::initialise()
    throw (AccessorUndefined)
{
    CDMW_MUTEX_GUARD(m_init_mutex);
    
    if (m_is_initialised == false)
    {
        try
        {
            // get the ReplicationManager service
            CORBA::Object_var object = 
                 m_serviceRegistration->get_service("ReplicationManager");
                 
            if (CORBA::is_nil(object.in()))
                throw AccessorUndefined();
        
            CdmwFT::ReplicationManager_var replication_manager = 
                 CdmwFT::ReplicationManager::_narrow(object.in());

            // get from the replication manager the system accessor group reference
            // from its group name
            ::FT::Name group_name;
            group_name.length(1);
            group_name[0].id =
                Cdmw::PlatformMngt::Configuration::Get_SystemAccessor_group_name().c_str();
        
            CORBA::Object_var system_accessor_group_obj = 
                replication_manager->get_object_group_ref_from_name(group_name);
                
            m_system_accessor = 
                CdmwFTSystMngt::SystemAccessor::_narrow(system_accessor_group_obj.in());
        
            // get the other accessor from system accessor
            m_application_accessor = CdmwFTSystMngt::ApplicationAccessor::_duplicate(m_system_accessor->get_application_accessor());
            m_process_accessor = CdmwFTSystMngt::ProcessAccessor::_duplicate(m_system_accessor->get_process_accessor());
            m_host_accessor = CdmwFTSystMngt::HostAccessor::_duplicate(m_system_accessor->get_host_accessor());
            m_entity_accessor = CdmwFTSystMngt::EntityAccessor::_duplicate(m_system_accessor->get_entity_accessor());

            m_is_initialised = true;
        
         }
         catch(...)
         {
            throw AccessorUndefined();
         }
     }
}


} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw


