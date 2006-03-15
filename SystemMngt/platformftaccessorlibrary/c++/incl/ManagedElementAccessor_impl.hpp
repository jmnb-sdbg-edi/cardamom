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


#ifndef INCL_FTSYSTMNGT_MANAGED_ELEMENT_ACCESSOR_IMPL_HPP
#define INCL_FTSYSTMNGT_MANAGED_ELEMENT_ACCESSOR_IMPL_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtPropertyManager.skel.hpp"
#include "SystemMngt/idllib/CdmwFTSystMngtManagedElement.skel.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtManagedElement.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp"


#include <string>

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



/**
 *Purpose:
 *<p>
 */
class ManagedElementAccessor_impl : public virtual POA_CdmwFTSystMngt::ManagedElementAccessor,                       
                                    public virtual PortableServer::RefCountServantBase
{

public:


    /**
     *Purpose:
     *<p> Constructor.
     *     
     */
    ManagedElementAccessor_impl (
        CdmwPlatformMngt::ElementRepository_ptr element_repository)    
            throw ();

    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual ~ManagedElementAccessor_impl();

      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ManagedElement/name:1.0
     * operation
     */
    CdmwPlatformMngt::ElementName get_name(const char* target_element_path)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException);
     
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ManagedElement/get_parent:1.0
     * operation
     */             
    CdmwPlatformMngt::ElementType get_parent (const char * target_element_path,
                                              CORBA::String_out parent_element_path)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException);
          
          
          
          
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/define_property:1.0
     * operation
     */
    void define_property(const char* target_element_path,
                         const char* property_name,
                         const CORBA::Any& property_value)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::InvalidPropertyName,
                 CosPropertyService::ConflictingProperty,
                 CosPropertyService::UnsupportedTypeCode,
                 CosPropertyService::UnsupportedProperty,
                 CosPropertyService::ReadOnlyProperty,
                 CORBA::SystemException);
                 
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/define_properties:1.0
     * operation
     */ 
    void define_properties(const char* target_element_path,
                           const CosPropertyService::Properties & nproperties)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::MultipleExceptions,
                 CORBA::SystemException);
 

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/get_number_of_properties:1.0
     * operation
     */
    CORBA::ULong get_number_of_properties(const char* target_element_path)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException);
      
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/get_all_property_names:1.0
     * operation
     */
    void get_all_property_names (const char* target_element_path,
                                 CORBA::ULong how_many,       
                                 CosPropertyService::PropertyNames_out property_names,
                                 CosPropertyService::PropertyNamesIterator_out rest)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException);
      
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/get_property_value:1.0
     * operation
     */
    CORBA::Any* get_property_value (const char* target_element_path,
                                    const char* property_name)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::PropertyNotFound,
                 CosPropertyService::InvalidPropertyName,
                 CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet//get_properties:1.0
     * operation
     */
    CORBA::Boolean get_properties (
                          const char* target_element_path,
                          const CosPropertyService::PropertyNames& property_names,
                          CosPropertyService::Properties_out nproperties)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/get_all_properties:1.0
     * operation
     */
    void get_all_properties (const char* target_element_path,
                             CORBA::ULong how_many,      
                             CosPropertyService::Properties_out nproperties,
                             CosPropertyService::PropertiesIterator_out rest)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException);
      
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/delete_property:1.0
     * operation
     */  
    void delete_property(const char* target_element_path,
                         const char* property_name)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::PropertyNotFound,
                 CosPropertyService::InvalidPropertyName,
                 CosPropertyService::FixedProperty,
                 CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/delete_properties:1.0
     * operation
     */              
    void delete_properties(const char* target_element_path,
                           const CosPropertyService::PropertyNames& property_names)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::MultipleExceptions,
                 CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/delete_all_properties:1.0
     * operation
     */
    CORBA::Boolean delete_all_properties(const char* target_element_path)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException);
    
    
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/is_property_defined:1.0
     * operation
     */
    CORBA::Boolean is_property_defined(
                             const char* target_element_path,
                             const char* property_name)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::InvalidPropertyName,
                 CORBA::SystemException);
    
    
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/get_allowed_property_types:1.0
     * operation
     */
    void get_allowed_property_types(
                               const char* target_element_path,
                               CosPropertyService::PropertyTypes_out property_types)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException);
          
          
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/get_allowed_properties:1.0
     * operation
     */
    void get_allowed_properties(const char* target_element_path,
                                CosPropertyService::PropertyDefs_out property_defs)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/define_property_with_mode:1.0
     * operation
     */
    void define_property_with_mode(const char* target_element_path,
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
                 CORBA::SystemException);
    

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/define_properties_with_mode:1.0
     * operation
     */
    void define_properties_with_modes(
                            const char* target_element_path,
                            const CosPropertyService::PropertyDefs & property_defs)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::MultipleExceptions,
                 CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/get_property_mode:1.0
     * operation
     */
    CosPropertyService::PropertyModeType get_property_mode(
                             const char* target_element_path,
                             const char* property_name)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::PropertyNotFound,
                 CosPropertyService::InvalidPropertyName,
                 CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/get_property_modes:1.0
     * operation
     */
    CORBA::Boolean get_property_modes(const char* target_element_path,
                                      const CosPropertyService::PropertyNames& property_names,
                                      CosPropertyService::PropertyModes_out property_modes)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/set_property_mode:1.0
     * operation
     */
    void set_property_mode(const char* target_element_path,
                           const char* property_name,
                           CosPropertyService::PropertyModeType property_mode)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::InvalidPropertyName,
                 CosPropertyService::PropertyNotFound,
                 CosPropertyService::UnsupportedMode,
                 CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:omg.org/CosPropertyService/PropertySet/set_property_modes:1.0
     * operation
     */
    void set_property_modes(const char* target_element_path,
                            const CosPropertyService::PropertyModes & property_modes)
          throw (CdmwPlatformMngt::ManagedElementNotFound,
                 CosPropertyService::MultipleExceptions,
                 CORBA::SystemException);
    

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyManager/register_all_properties_observer:1.0
     * operation
     */
    CdmwPlatformMngt::SupervisionObserver_ptr register_all_properties_observer (
            const char* target_element_path,
            const char* observer_name,
            CdmwPlatformMngt::SupervisionObserver_ptr observer,
            CORBA::Boolean deeply)
            throw (CdmwPlatformMngt::ManagedElementNotFound,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyManager/unregister_all_properties_observer:1.0
     * operation
     */
    CdmwPlatformMngt::SupervisionObserver_ptr unregister_all_properties_observer (
            const char* target_element_path,
            const char* observer_name)
            throw (CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::ObserverNotFound,
                   CORBA::SystemException);
   
   
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyManager/register_property_observer:1.0
     * operation
     */         
    void register_property_observer (
            const char* target_element_path,
            const char* observer_name,
            CdmwPlatformMngt::SupervisionObserver_ptr observer,
            const CosPropertyService::PropertyNames& property_names)
            throw (CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::PropertyInListNotFound,
                   CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PropertyManager/unregister_property_observer:1.0
     * operation
     */ 
    void unregister_property_observer (
            const char* target_element_path,
            const char* observer_name,
            const CosPropertyService::PropertyNames& property_names)
            throw (CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::PropertyInListNotFound,
                   CdmwPlatformMngt::PropertyObserverNotFound,
                   CORBA::SystemException);
          
                     

 
protected:

    /**
     * The repository of the managed elements.
     */
    CdmwPlatformMngt::ElementRepository_var m_element_repository;

                                  
                       
protected:

    /**
     *Purpose:
     *<p> Get the ManagedElement object.
     */
    CdmwPlatformMngt::ManagedElement_ptr get_managed_element (const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound);

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    ManagedElementAccessor_impl (ManagedElementAccessor_impl& rhs)
    {};

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    ManagedElementAccessor_impl& operator = (const ManagedElementAccessor_impl& rhs)

    {
        return *this;
    };
          
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_FTSYSTMNGT_MANAGED_ELEMENT_ACCESSOR_IMPL_HPP

