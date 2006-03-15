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


#include "testbasic/SIM_ManagedElementAccessor_impl.hpp"
#include "testbasic/TestSamples.hpp"
#include "testbasic/TestModus.hpp"
#include <string>
#include <Foundation/common/Assert.hpp> 

#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp>
#include <Foundation/common/Assert.hpp> 

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

namespace Proxy
{

// CORBA::ULong SIM_ManagedElementAccessor_impl::TEST_NUMBER_OF_PROPERTIES = 42;
// char *SIM_ManagedElementAccessor_impl::TEST_PROPERTY_NAME = "FOO";
// char *SIM_ManagedElementAccessor_impl::TEST_PROPERTY_VALUE_STRING = "BAR";

    CdmwPlatformMngt::ElementType SIM_ManagedElementAccessor_impl::M_parent_type = CdmwPlatformMngt::SYSTEM_TYPE;

void 
SIM_ManagedElementAccessor_impl::setParentType
(CdmwPlatformMngt::ElementType type) throw() 
{
    M_parent_type = type;
}

SIM_ManagedElementAccessor_impl::SIM_ManagedElementAccessor_impl ()    
            throw ()
{
}

    /**
     *Purpose:
     *<p> Default destructor.
     */
SIM_ManagedElementAccessor_impl::~SIM_ManagedElementAccessor_impl()
{
}

      
CdmwPlatformMngt::ElementName 
SIM_ManagedElementAccessor_impl::get_name(const char* target_element_path)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException) 
{
	return CORBA::string_dup(target_element_path);
}
           
          
CdmwPlatformMngt::ElementType 
SIM_ManagedElementAccessor_impl::get_parent 
(
 const char * target_element_path,
 CORBA::String_out parent_element_path)
    throw (CdmwPlatformMngt::ManagedElementNotFound,
	   CORBA::SystemException)
{
    parent_element_path = TestSamples::getElementPath();

    return M_parent_type;
}

/**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/define_property:1.0
     * operation
     */
void 
SIM_ManagedElementAccessor_impl::define_property(
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
    raiseExceptionIfExceptionModus();

    raiseSystemExceptionIf(! TestSamples::hasElementName(target_element_path) ||
			   ! TestSamples::hasPropertyName(property_name) ||
			   ! TestSamples::hasPropertyValue(property_value) );
}
                 
void 
SIM_ManagedElementAccessor_impl::define_properties(
			   const char* target_element_path,
                           const CosPropertyService::Properties & nproperties)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::MultipleExceptions,
                 CORBA::SystemException) 
{   
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementName(target_element_path));
    // FIXME verify the porperty list
}
 
CORBA::ULong 
SIM_ManagedElementAccessor_impl::get_number_of_properties(
                           const char* target_element_path)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException) 
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementName(target_element_path) );
    CosPropertyService::Properties_var properties = 
        TestSamples::getProperties();
    
    return properties->length();
}
      
      
void 
SIM_ManagedElementAccessor_impl::get_all_property_names (
                           const char* target_element_path,
			   CORBA::ULong how_many,       
			   CosPropertyService::PropertyNames_out property_names,
			   CosPropertyService::PropertyNamesIterator_out rest)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException)
{
    // TODO was not implemented in supervision
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
    
    raiseExceptionIfExceptionModus();
   
    raiseSystemExceptionIf( ! TestSamples::hasElementPath(target_element_path));    
    //     CosPropertyService::PropertyNames_var elems =
    // 	TestSamples::getPropertyNames();
    //     CosPropertyService::PropertyNames_var head =
    // 	new CosPropertyService::PropertyNames();
    //     CosPropertyService::PropertyNames_var tail =
    // 	new CosPropertyService::PropertyNames();
    
    //     split (elems, how_many, head, tail);
    
    //     CosPropertyService::PropertyNamesIterator_var elementsRemainingIt =
    //      	PropertNamesIterator_impl::create (seq2list(tail.in()));    
    
    //     property_names = head._retn();
    //     rest = elementsRemainingIt._retn();
}

 

      

CORBA::Any* 
SIM_ManagedElementAccessor_impl::get_property_value (
                                    const char* target_element_path,
                                    const char* property_name)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::PropertyNotFound,
                 CosPropertyService::InvalidPropertyName,
                 CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf( ! TestSamples::hasElementPath(target_element_path));   
    return TestSamples::getPropertyValue();
}


CORBA::Boolean 
SIM_ManagedElementAccessor_impl::get_properties (
                          const char* target_element_path,
                          const CosPropertyService::PropertyNames& property_names,
                          CosPropertyService::Properties_out nproperties)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException) 
{    
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf( ! TestSamples::hasElementPath(target_element_path));
    nproperties = TestSamples::getProperties();

    return true;
}


void 
SIM_ManagedElementAccessor_impl::get_all_properties (
                             const char* target_element_path,
                             CORBA::ULong how_many,      
                             CosPropertyService::Properties_out nproperties,
                             CosPropertyService::PropertiesIterator_out rest)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException)
{
  // TODO has not been defined in supervision 20050603
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);

    raiseExceptionIfExceptionModus();
   
    raiseSystemExceptionIf( ! TestSamples::hasElementPath(target_element_path));    
    //     CosPropertyService::Properties_var elems =
    //         TestSamples::getProperties();
    
    //     CosPropertyService::Properties_var head =
    // new CosPropertyService::Properties();
    //     CosPropertyService::Properties_var tail =
    // new CosPropertyService::Properties();
    
    //     split(elems, how_many, head, tail);
    
    //     // create the path iterator
    //     CosPropertyService::PropertiesIterator_var elementsRemainingIt =
    // 	PropertiesIterator_impl::create (seq2list(tail.in()));
    
    //     nproperties = head._retn();  
    //     rest        = elementsRemainingIt._retn(); 
}

      
      
void 
SIM_ManagedElementAccessor_impl::delete_property(
                         const char* target_element_path,
                         const char* property_name)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::PropertyNotFound,
                 CosPropertyService::InvalidPropertyName,
                 CosPropertyService::FixedProperty,
                 CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf( ! TestSamples::hasElementPath(target_element_path));}
    
void 
SIM_ManagedElementAccessor_impl::delete_properties(
                           const char* target_element_path,
                           const CosPropertyService::PropertyNames& property_names)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::MultipleExceptions,
                 CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf( ! TestSamples::hasElementPath(target_element_path));
    // FIXME - test property
}


CORBA::Boolean 
SIM_ManagedElementAccessor_impl::delete_all_properties(
                           const char* target_element_path)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf( ! TestSamples::hasElementPath(target_element_path));      
    return true;
}
    
    

CORBA::Boolean 
SIM_ManagedElementAccessor_impl::is_property_defined(
                             const char* target_element_path,
                             const char* property_name)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::InvalidPropertyName,
                 CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf( ! TestSamples::hasElementPath(target_element_path));      
    return false;
}
    
    
    

void 
SIM_ManagedElementAccessor_impl::get_allowed_property_types(
                               const char* target_element_path,
                               CosPropertyService::PropertyTypes_out property_types)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException)
{ 
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf( ! TestSamples::hasElementPath(target_element_path));   
    property_types = TestSamples::getPropertyTypes();
}

          
          
void 
SIM_ManagedElementAccessor_impl::get_allowed_properties(const char* target_element_path,
                                CosPropertyService::PropertyDefs_out property_defs)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf( ! TestSamples::hasElementPath(target_element_path));
    
    property_defs = TestSamples::getPropertyDefs();
}


void 
SIM_ManagedElementAccessor_impl::define_property_with_mode(
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
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf( ! TestSamples::hasElementPath(target_element_path));}

    
void 
SIM_ManagedElementAccessor_impl::define_properties_with_modes(
                            const char* target_element_path,
                            const CosPropertyService::PropertyDefs & property_defs)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::MultipleExceptions,
                 CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(std::string(target_element_path).empty());
}


CosPropertyService::PropertyModeType 
SIM_ManagedElementAccessor_impl::get_property_mode(
                             const char* target_element_path,
                             const char* property_name)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::PropertyNotFound,
                 CosPropertyService::InvalidPropertyName,
                 CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf( ! TestSamples::hasElementPath(target_element_path));    
    return CosPropertyService::undefined;
}



CORBA::Boolean 
SIM_ManagedElementAccessor_impl::get_property_modes(
                                      const char* target_element_path,
                                      const CosPropertyService::PropertyNames& property_names,
                                      CosPropertyService::PropertyModes_out property_modes)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf( ! TestSamples::hasElementPath(target_element_path));        
    return true;
}


void 
SIM_ManagedElementAccessor_impl::set_property_mode(
                           const char* target_element_path,
                           const char* property_name,
                           CosPropertyService::PropertyModeType property_mode)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::InvalidPropertyName,
                 CosPropertyService::PropertyNotFound,
                 CosPropertyService::UnsupportedMode,
                 CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf( ! TestSamples::hasElementPath(target_element_path));    
}


void 
SIM_ManagedElementAccessor_impl::set_property_modes(
                            const char* target_element_path,
                            const CosPropertyService::PropertyModes & property_modes)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::MultipleExceptions,
                 CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf( ! TestSamples::hasElementPath(target_element_path));}

    

 
CdmwPlatformMngt::SupervisionObserver_ptr 
SIM_ManagedElementAccessor_impl::register_all_properties_observer (
            const char* target_element_path,
            const char* observer_name,
            CdmwPlatformMngt::SupervisionObserver_ptr observer,
            CORBA::Boolean deeply)
            throw (CdmwPlatformMngt::ManagedElementNotFound,
                   CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf( ! TestSamples::hasElementPath(target_element_path));
    return CdmwPlatformMngt::SupervisionObserver::_nil();
}



CdmwPlatformMngt::SupervisionObserver_ptr  
SIM_ManagedElementAccessor_impl::unregister_all_properties_observer (
            const char* target_element_path,
            const char* observer_name)
            throw (CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::ObserverNotFound,
                   CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf( ! TestSamples::hasElementPath(target_element_path));
    return CdmwPlatformMngt::SupervisionObserver::_nil();
}


   
   
     
void  
SIM_ManagedElementAccessor_impl::register_property_observer (
            const char* target_element_path,
            const char* observer_name,
            CdmwPlatformMngt::SupervisionObserver_ptr observer,
            const CosPropertyService::PropertyNames& property_names)
            throw (CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::PropertyInListNotFound,
                   CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf( ! TestSamples::hasElementPath(target_element_path));
}




void  
SIM_ManagedElementAccessor_impl::unregister_property_observer (
            const char* target_element_path,
            const char* observer_name,
            const CosPropertyService::PropertyNames& property_names)
            throw (CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::PropertyInListNotFound,
                   CdmwPlatformMngt::PropertyObserverNotFound,
                   CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf( ! TestSamples::hasElementPath(target_element_path));
    
}





} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw

