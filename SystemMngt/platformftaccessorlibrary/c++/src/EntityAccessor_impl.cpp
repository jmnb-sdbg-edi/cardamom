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

#include "platformftaccessorlibrary/EntityAccessor_impl.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntity.stub.hpp"

#include "SystemMngt/platformlibrary/ManagedElementKey.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


// ----------------------------------------------------------------------
// Entity accessor class.
// ----------------------------------------------------------------------

EntityAccessor_impl::EntityAccessor_impl(
    CdmwPlatformMngt::ElementRepository_ptr element_repository)
    throw()  
    : ManagedElementAccessor_impl(element_repository)     
{
}

EntityAccessor_impl::~EntityAccessor_impl ()
{
}

CdmwPlatformMngt::Entity_ptr EntityAccessor_impl::get_entity (
          const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound)
{
    CdmwPlatformMngt::Entity_var entity_obj;
    
    try
    {
        CdmwPlatformMngt::ManagedElement_var element_obj =
              m_element_repository->get_element(target_element_path);
              
        entity_obj = CdmwPlatformMngt::Entity::_narrow (element_obj.in());
    }
    catch (const CdmwPlatformMngt::ElementPathInvalid&)
    {
        throw CdmwPlatformMngt::ManagedElementNotFound(target_element_path);
    }
    catch(const CORBA::SystemException&)
    {
        throw CdmwPlatformMngt::ManagedElementNotFound(target_element_path);
    }
    
    return entity_obj._retn();                   
}


char* 
EntityAccessor_impl::get_status(const char* target_element_path,
                                CORBA::String_out entity_info)
throw (CdmwPlatformMngt::ManagedElementNotFound,
       CORBA::SystemException)
{
	CdmwPlatformMngt::Entity_var entity_obj = 
                        get_entity (target_element_path);
                        
    return entity_obj->get_status(entity_info);
}

void EntityAccessor_impl::get_attribute( const char *target_element_path, const char * attr_name, CORBA::Any_out ret_attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{
	CdmwPlatformMngt::Entity_var entity_obj = 
                        get_entity (target_element_path);
                        
    entity_obj->get_attribute(attr_name, ret_attr,flag);


}

void EntityAccessor_impl::set_attribute( const char *target_element_path, const char * attr_name, const CORBA::Any & attr, CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{
	CdmwPlatformMngt::Entity_var entity_obj = 
                        get_entity (target_element_path);
                        
    entity_obj->set_attribute(attr_name, attr,flag);


}

void 
EntityAccessor_impl::set_status(const char* target_element_path,
                                const char* entity_status, 
                                const char* entity_info)
throw (CdmwPlatformMngt::ManagedElementNotFound,
       CdmwPlatformMngt::EntityStatusNotAllowed, 
       CORBA::SystemException)
{
	CdmwPlatformMngt::Entity_var entity_obj = 
                        get_entity (target_element_path);
                        
    entity_obj->set_status(entity_status, entity_info);
}

    


CdmwPlatformMngt::EntityStatusValues* 
EntityAccessor_impl::get_status_values(const char* target_element_path)
throw (CdmwPlatformMngt::ManagedElementNotFound,
       CORBA::SystemException)
{
	CdmwPlatformMngt::Entity_var entity_obj = 
                        get_entity (target_element_path);
                        
    return entity_obj->get_status_values();
}



CdmwPlatformMngt::EntitySnapshot* 
EntityAccessor_impl::get_snapshot(const char* target_element_path)
throw (CdmwPlatformMngt::ManagedElementNotFound,
       CORBA::SystemException)
{
	CdmwPlatformMngt::Entity_var entity_obj = 
                        get_entity (target_element_path);
                        
    return entity_obj->get_snapshot();
}

} // End namespace PlatformMngt
} // End namespace Cdmw


