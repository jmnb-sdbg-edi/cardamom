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

#include "platformftaccessorlibrary/EntityContainerAccessor_impl.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntityContainer.stub.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtApplication.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcess.stub.hpp"

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
EntityContainerAccessor_impl::EntityContainerAccessor_impl(
    CdmwPlatformMngt::ElementRepository_ptr element_repository)
    throw()       
{
	m_entity_element_repository = 
           CdmwPlatformMngt::ElementRepository::_duplicate(element_repository);
}



//
// Destructor
// 
EntityContainerAccessor_impl::~EntityContainerAccessor_impl()
{
}

CdmwPlatformMngt::EntityContainer_ptr EntityContainerAccessor_impl::get_entity_container (
          const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound)
{
    CdmwPlatformMngt::EntityContainer_var entity_container_obj;
    
    try
    {                           
        CORBA::Object_var element_obj =
              m_entity_element_repository->get_element(target_element_path);
              
        entity_container_obj = 
               CdmwPlatformMngt::EntityContainer::_narrow (element_obj.in());
    }
    catch (const CdmwPlatformMngt::ElementPathInvalid&)
    {
        throw CdmwPlatformMngt::ManagedElementNotFound(target_element_path);
    }
    catch(const CORBA::SystemException&)
    {
        throw CdmwPlatformMngt::ManagedElementNotFound(target_element_path);
    }
    
    return entity_container_obj._retn();                   
}



CORBA::ULong EntityContainerAccessor_impl::get_number_of_entities(
                      const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::EntityContainer_var entity_container_obj = 
                        get_entity_container (target_element_path);
                        
    return entity_container_obj->get_number_of_entities();
}


void EntityContainerAccessor_impl::get_all_entity_names(
                      const char* target_element_path,
                      CORBA::ULong how_many,
                      CdmwPlatformMngt::ElementNames_out entity_names,
                      CdmwPlatformMngt::ElementNamesIterator_out rest)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	// TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


char* EntityContainerAccessor_impl::get_entity(
                      const char* target_element_path,
                      const char* entity_name)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::EntityNotFound,
      CORBA::SystemException)
{
    CdmwPlatformMngt::EntityContainer_var entity_container_obj = 
                        get_entity_container (target_element_path);
    
    CdmwPlatformMngt::Entity_var entityObject =
        entity_container_obj->get_entity(entity_name);
    
    return entityObject->element_path(); 
}
    	
CORBA::Boolean EntityContainerAccessor_impl::get_entities (
		                const char* target_element_path,
                        const CdmwPlatformMngt::ElementNames& entity_names,
                        CdmwPlatformMngt::ElementPaths_out entities)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	// TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}    	

void EntityContainerAccessor_impl::get_all_entities (
                      const char* target_element_path,
                      CORBA::ULong how_many,
                      CdmwPlatformMngt::ElementPaths_out nentities,
                      CdmwPlatformMngt::ElementPathsIterator_out rest)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{   	
	// TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}
     

} // End namespace PlatformMngt
} // End namespace Cdmw



