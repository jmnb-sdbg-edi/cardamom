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

#include "platformftaccessorlibrary/RWEntityContainerAccessor_impl.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntityContainer.stub.hpp"

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

RWEntityContainerAccessor_impl::RWEntityContainerAccessor_impl(
     CdmwPlatformMngt::ElementRepository_ptr element_repository)
      throw()    
     : EntityContainerAccessor_impl(element_repository)   
{
}


//
// Destructor
// 
RWEntityContainerAccessor_impl::~RWEntityContainerAccessor_impl()
{
} 

CdmwPlatformMngt::RWEntityContainer_ptr RWEntityContainerAccessor_impl::get_rw_entity_container (
          const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound)
{
    CdmwPlatformMngt::RWEntityContainer_var rwentity_container_obj;
    
    try
    {
        CORBA::Object_var element_obj =
              m_entity_element_repository->get_element(target_element_path);
                 
        rwentity_container_obj = 
               CdmwPlatformMngt::RWEntityContainer::_narrow (element_obj.in());
    }
    catch (const CdmwPlatformMngt::ElementPathInvalid&)
    {
        throw CdmwPlatformMngt::ManagedElementNotFound(target_element_path);
    }
    catch(const CORBA::SystemException&)
    {
        throw CdmwPlatformMngt::ManagedElementNotFound(target_element_path);
    }
    
    return rwentity_container_obj._retn();                   
}

char* RWEntityContainerAccessor_impl::add_entity(
                      const char* target_element_path,
                      const CdmwPlatformMngt::EntityDef & entity_def)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::EntityAlreadyExists,CosPropertyService::MultipleExceptions,
      CORBA::SystemException)
{
    CdmwPlatformMngt::RWEntityContainer_var rwentity_container_obj = 
                        get_rw_entity_container (target_element_path);
    
    CdmwPlatformMngt::Entity_var entityObject =
        rwentity_container_obj->add_entity(entity_def);
    
    return entityObject->element_path(); 
} 



void RWEntityContainerAccessor_impl::remove_entity(
                      const char* target_element_path,
                      const char* entity_name)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::EntityNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::RWEntityContainer_var rwentity_container_obj = 
                        get_rw_entity_container (target_element_path);
                        
    rwentity_container_obj->remove_entity(entity_name);
} 
                      
                      
                      

} // End namespace PlatformMngt
} // End namespace Cdmw



