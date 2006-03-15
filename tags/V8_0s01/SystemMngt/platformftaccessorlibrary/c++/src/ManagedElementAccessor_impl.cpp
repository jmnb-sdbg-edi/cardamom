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

#include "platformftaccessorlibrary/ManagedElementAccessor_impl.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"

#include "SystemMngt/platformlibrary/ManagedElementKey.hpp"


namespace Cdmw
{
namespace PlatformMngt
{



// ----------------------------------------------------------------------
// ManagedElementAccessor_impl class.
// ----------------------------------------------------------------------

ManagedElementAccessor_impl::ManagedElementAccessor_impl (
       CdmwPlatformMngt::ElementRepository_ptr element_repository)
throw ()
{
    m_element_repository = 
           CdmwPlatformMngt::ElementRepository::_duplicate(element_repository);
}


ManagedElementAccessor_impl::~ManagedElementAccessor_impl()
{
}

CdmwPlatformMngt::ManagedElement_ptr ManagedElementAccessor_impl::get_managed_element (
          const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound)
{
    CdmwPlatformMngt::ManagedElement_var element_obj;
    
    try
    {
        element_obj = m_element_repository->get_element(target_element_path);
    }
    catch (const CdmwPlatformMngt::ElementPathInvalid&)
    {
        throw CdmwPlatformMngt::ManagedElementNotFound(target_element_path);
    }
    catch(const CORBA::SystemException&)
    {
        throw CdmwPlatformMngt::ManagedElementNotFound(target_element_path);
    }
    
    return element_obj._retn();                   
}


CdmwPlatformMngt::ElementName ManagedElementAccessor_impl::get_name(
       const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    return element_obj->name();
}


CdmwPlatformMngt::ElementType ManagedElementAccessor_impl::get_parent (
                                          const char * target_element_path,
                                          CORBA::String_out parent_element_path)
throw (CdmwPlatformMngt::ManagedElementNotFound,
       CORBA::SystemException)
{
    CdmwPlatformMngt::ElementType parent_type;
    
    // get the target element object
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
    
    // get the parent object (nil if element is root)
    CdmwPlatformMngt::ManagedElement_var parent_obj = element_obj->get_parent();
    
    // check parent is defined
    if (CORBA::is_nil(parent_obj.in()))
    {
        // parent is undefined, return empty path
        parent_element_path = CORBA::string_dup ("");
        parent_type = CdmwPlatformMngt::SYSTEM_TYPE;
    }
    else
    {
        // get the element path of the parent
        CORBA::String_var path = parent_obj->element_path();
        
        // get the type of parent
        parent_type = ManagedElementKey::Get_element_type (path.in());
        
        parent_element_path = path._retn();
    }
        
    return parent_type;
}



void ManagedElementAccessor_impl::define_property(
                         const char* target_element_path,
                         const char* property_name,
                         const CORBA::Any& property_value)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::InvalidPropertyName,
                 CosPropertyService::ConflictingProperty,
                 CosPropertyService::UnsupportedTypeCode,
                 CosPropertyService::UnsupportedProperty,
                 CosPropertyService::ReadOnlyProperty,
                 CORBA::SystemException)
{
	CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    element_obj->define_property(property_name, property_value);
}

                 

void ManagedElementAccessor_impl::define_properties(
                          const char* target_element_path,
                          const CosPropertyService::Properties & nproperties)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::MultipleExceptions,
                 CORBA::SystemException)
{
	CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    element_obj->define_properties(nproperties);
}



CORBA::ULong ManagedElementAccessor_impl::get_number_of_properties(
                           const char* target_element_path)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException )
{
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    return element_obj->get_number_of_properties();    
}

void ManagedElementAccessor_impl::get_all_property_names (
                        const char* target_element_path,
                        CORBA::ULong how_many,
                        CosPropertyService::PropertyNames_out property_names,
                        CosPropertyService::PropertyNamesIterator_out rest)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException )
{
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    element_obj->get_all_property_names(how_many, property_names, rest);
}

CORBA::Any* ManagedElementAccessor_impl::get_property_value (
                        const char* target_element_path,
                        const char* property_name)    
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::PropertyNotFound,
                 CosPropertyService::InvalidPropertyName,
                 CORBA::SystemException)
{
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    return element_obj->get_property_value(property_name);
}

CORBA::Boolean ManagedElementAccessor_impl::get_properties (
                         const char* target_element_path,
                         const CosPropertyService::PropertyNames& property_names,
                         CosPropertyService::Properties_out nproperties)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException )
{
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    return element_obj->get_properties(property_names, nproperties);
}

void ManagedElementAccessor_impl::get_all_properties (
                         const char* target_element_path,
                         CORBA::ULong how_many,
                         CosPropertyService::Properties_out nproperties,
                         CosPropertyService::PropertiesIterator_out rest)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException )
{
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    element_obj->get_all_properties(how_many, nproperties, rest);
}



void ManagedElementAccessor_impl::delete_property(
                         const char* target_element_path,
                         const char* property_name)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::PropertyNotFound,
                 CosPropertyService::InvalidPropertyName,
                 CosPropertyService::FixedProperty,
                 CORBA::SystemException)
{
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    element_obj->delete_property(property_name);
}
    
              
void ManagedElementAccessor_impl::delete_properties(
                         const char* target_element_path,
                         const CosPropertyService::PropertyNames& property_names)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::MultipleExceptions,
                 CORBA::SystemException)
{
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    element_obj->delete_properties(property_names);
}


CORBA::Boolean ManagedElementAccessor_impl::delete_all_properties(
                          const char* target_element_path)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException)
{
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    return element_obj->delete_all_properties();
}
    
    
    
CORBA::Boolean ManagedElementAccessor_impl::is_property_defined(
                          const char* target_element_path,
                          const char* property_name)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::InvalidPropertyName,
                 CORBA::SystemException)
{
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    return element_obj->is_property_defined(property_name);
}
    
    
    

void ManagedElementAccessor_impl::get_allowed_property_types(
                         const char* target_element_path,
                         CosPropertyService::PropertyTypes_out property_types)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException)
{
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    element_obj->get_allowed_property_types(property_types);
}
          
          

void ManagedElementAccessor_impl::get_allowed_properties(
                         const char* target_element_path,
                         CosPropertyService::PropertyDefs_out property_defs)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException)
{
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    element_obj->get_allowed_properties(property_defs);
}



void ManagedElementAccessor_impl::define_property_with_mode(
                         const char* target_element_path,
                         const char* property_name,
                         const CORBA::Any& property_value,
                         CosPropertyService::PropertyModeType property_mode)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::InvalidPropertyName,
                 CosPropertyService::ConflictingProperty,
                 CosPropertyService::UnsupportedTypeCode,
                 CosPropertyService::UnsupportedProperty,
                 CosPropertyService::UnsupportedMode,
                 CosPropertyService::ReadOnlyProperty,
                 CORBA::SystemException)
{
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    element_obj->define_property_with_mode(property_name, property_value, property_mode);
}
    


void ManagedElementAccessor_impl::define_properties_with_modes(
                         const char* target_element_path,
                         const CosPropertyService::PropertyDefs & property_defs)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::MultipleExceptions,
                 CORBA::SystemException)
{
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    element_obj->define_properties_with_modes(property_defs);
}


CosPropertyService::PropertyModeType ManagedElementAccessor_impl::get_property_mode(
                         const char* target_element_path,
                         const char* property_name)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::PropertyNotFound,
                 CosPropertyService::InvalidPropertyName,
                 CORBA::SystemException)
{
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    return element_obj->get_property_mode(property_name);
}



CORBA::Boolean ManagedElementAccessor_impl::get_property_modes(
                         const char* target_element_path,
                         const CosPropertyService::PropertyNames& property_names,
                         CosPropertyService::PropertyModes_out property_modes)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException)
{
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    return element_obj->get_property_modes(property_names, property_modes);
}


void ManagedElementAccessor_impl::set_property_mode(
                         const char* target_element_path,
                         const char* property_name,
                         CosPropertyService::PropertyModeType property_mode)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::InvalidPropertyName,
                 CosPropertyService::PropertyNotFound,
                 CosPropertyService::UnsupportedMode,
                 CORBA::SystemException)
{
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    element_obj->set_property_mode(property_name, property_mode);
}



void ManagedElementAccessor_impl::set_property_modes(
                         const char* target_element_path,
                         const CosPropertyService::PropertyModes & property_modes)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::MultipleExceptions,
                 CORBA::SystemException)
{
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    element_obj->set_property_modes(property_modes);
}
    



CdmwPlatformMngt::SupervisionObserver_ptr 
   ManagedElementAccessor_impl::register_all_properties_observer (
            const char* target_element_path,
            const char* observer_name,
            CdmwPlatformMngt::SupervisionObserver_ptr observer,
            CORBA::Boolean deeply)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException)
{
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    return element_obj->register_all_properties_observer(
                              observer_name, observer, deeply);
}


CdmwPlatformMngt::SupervisionObserver_ptr 
   ManagedElementAccessor_impl::unregister_all_properties_observer (
            const char* target_element_path,
            const char* observer_name)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CdmwPlatformMngt::ObserverNotFound,
                 CORBA::SystemException)
{
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    return element_obj->unregister_all_properties_observer(observer_name);
}
   
   
       
void ManagedElementAccessor_impl::register_property_observer (
            const char* target_element_path,
            const char* observer_name,
            CdmwPlatformMngt::SupervisionObserver_ptr observer,
            const CosPropertyService::PropertyNames& property_names)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CdmwPlatformMngt::PropertyInListNotFound,
                 CORBA::SystemException)
{
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    element_obj->register_property_observer(observer_name, observer, property_names);
}



void ManagedElementAccessor_impl::unregister_property_observer (
            const char* target_element_path,
            const char* observer_name,
            const CosPropertyService::PropertyNames& property_names)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CdmwPlatformMngt::PropertyInListNotFound,
                 CdmwPlatformMngt::PropertyObserverNotFound,
                 CORBA::SystemException)
{
    CdmwPlatformMngt::ManagedElement_var element_obj = 
                        get_managed_element (target_element_path);
                        
    element_obj->unregister_property_observer(observer_name, property_names);
}
          

} // End namespace PlatformMngt
} // End namespace Cdmw

