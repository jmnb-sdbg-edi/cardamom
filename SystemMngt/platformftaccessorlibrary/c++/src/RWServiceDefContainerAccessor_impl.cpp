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

#include "platformftaccessorlibrary/RWServiceDefContainerAccessor_impl.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtServiceContainer.stub.hpp"

#include "SystemMngt/platformlibrary/ManagedElementKey.hpp"


/**
* Root namespace for CDMW runtime.
*/
namespace Cdmw
{
/**
* Namespace for CDMW PlatformMngt
*/
namespace PlatformMngt
{

//
// Constructor
//
RWServiceDefContainerAccessor_impl::RWServiceDefContainerAccessor_impl(
     CdmwPlatformMngt::ElementRepository_ptr element_repository)
      throw()    
     : ServiceDefContainerAccessor_impl(element_repository)   
{
}


//
// Destructor
// 
RWServiceDefContainerAccessor_impl::~RWServiceDefContainerAccessor_impl()
{
} 

CdmwPlatformMngtService::RWServiceDefContainer_ptr 
RWServiceDefContainerAccessor_impl::get_rw_servicedef_container (
          const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound)
{
    CdmwPlatformMngtService::RWServiceDefContainer_var rwservicedef_container_obj;
    
    try
    {
        CORBA::Object_var element_obj =
              m_service_element_repository->get_element(target_element_path);
                           
        rwservicedef_container_obj = 
              CdmwPlatformMngtService::RWServiceDefContainer::_narrow (element_obj.in());
    }
    catch (const CdmwPlatformMngt::ElementPathInvalid&)
    {
        throw CdmwPlatformMngt::ManagedElementNotFound(target_element_path);
    }
    catch(const CORBA::SystemException&)
    {
        throw CdmwPlatformMngt::ManagedElementNotFound(target_element_path);
    }
    
    return rwservicedef_container_obj._retn();                   
}

void RWServiceDefContainerAccessor_impl::add_service_def (
      const char* target_element_path,
      const CdmwPlatformMngtService::ServiceDef& service_def)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngtService::ServiceAlreadyExists,
	  CORBA::SystemException)
    
{
	CdmwPlatformMngtService::RWServiceDefContainer_var rwservicedef_container_obj = 
                        get_rw_servicedef_container (target_element_path);
                        
    rwservicedef_container_obj->add_service_def(service_def);
} 


void RWServiceDefContainerAccessor_impl::remove_service_def (
      const char* target_element_path,
      const char* service_name)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngtService::ServiceNotFound,
      CORBA::SystemException)
{
    CdmwPlatformMngtService::RWServiceDefContainer_var rwservicedef_container_obj = 
                        get_rw_servicedef_container (target_element_path);
                        
    rwservicedef_container_obj->remove_service_def(service_name);
} 

void RWServiceDefContainerAccessor_impl::remove_all_service_defs (
      const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
    CdmwPlatformMngtService::RWServiceDefContainer_var rwservicedef_container_obj = 
                        get_rw_servicedef_container (target_element_path);
                        
    rwservicedef_container_obj->remove_all_service_defs();
} 
                      
                      

} // End namespace PlatformMngt
} // End namespace Cdmw



