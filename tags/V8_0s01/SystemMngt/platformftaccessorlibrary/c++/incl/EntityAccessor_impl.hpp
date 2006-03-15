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


#ifndef INCL_FTSYSTMNGT_ENTITY_ACCESSOR_IMPL_HPP
#define INCL_FTSYSTMNGT_ENTITY_ACCESSOR_IMPL_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

#include "SystemMngt/platformftaccessorlibrary/ManagedElementAccessor_impl.hpp"

#include "SystemMngt/idllib/CdmwFTSystMngtEntity.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntity.stub.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


/**
 *Purpose:
 *<p> Provides the Entity accessor.
 */
class EntityAccessor_impl : virtual public POA_CdmwFTSystMngt::EntityAccessor,
                            public ManagedElementAccessor_impl
{

public:

    /**
     *Purpose:
     *<p> Constructor.
     *
     */
     EntityAccessor_impl(
            CdmwPlatformMngt::ElementRepository_ptr element_repository)
                throw();
                    

    /**
     *Purpose:
     *<p> Destructor.
     */
    ~EntityAccessor_impl();
    
     /**
     *Purpose:
     *<p> facade to get an attribute
     *
     */

     void get_attribute( const char *target_element_path, const char * attr_name, CORBA::Any_out ret_attr  , CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);

    /**
     *Purpose:
     *<p> facade to set an attribute
     *
     */

     void set_attribute( const char * target_element_path, const char * attr_name, const CORBA::Any & attr, CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Entity/get_status:1.0
     * operation
     */
    char* get_status(
          const char* target_element_path,
          CORBA::String_out entity_info)
        throw (CdmwPlatformMngt::ManagedElementNotFound,
               CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Entity/set_status:1.0
     * operation
     */
    void set_status(
          const char* target_element_path,
          const char* entity_status, 
          const char* entity_info)
             throw (CdmwPlatformMngt::ManagedElementNotFound,
                    CdmwPlatformMngt::EntityStatusNotAllowed, 
                    CORBA::SystemException);
        

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Entity/get_status_values:1.0
     * operation
     */
    CdmwPlatformMngt::EntityStatusValues* get_status_values(
           const char* target_element_path)
             throw (CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Entity/get_snapshot:1.0
     * operation
     */
    CdmwPlatformMngt::EntitySnapshot* get_snapshot(
            const char* target_element_path)
             throw (CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);

private:

    /**
     *Purpose:
     *<p> Get the Entity object.
     */
    CdmwPlatformMngt::Entity_ptr get_entity (const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound);

    /**
     *Purpose:
     *<p> Default constructor is not allowed.
     */
    EntityAccessor_impl();

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    EntityAccessor_impl (EntityAccessor_impl& rhs);

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    EntityAccessor_impl& operator= (const EntityAccessor_impl& rhs);

};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_FTSYSTMNGT_ENTITY_ACCESSOR_IMPL_HPP

