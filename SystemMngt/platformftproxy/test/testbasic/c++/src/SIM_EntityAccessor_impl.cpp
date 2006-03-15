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


#include "testbasic/SIM_EntityAccessor_impl.hpp"

#include "testbasic/TestModus.hpp"
#include "testbasic/TestSamples.hpp"
#include <Foundation/common/Assert.hpp> 
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{



SIM_EntityAccessor_impl::SIM_EntityAccessor_impl()
                throw()
{
}
                    
SIM_EntityAccessor_impl::~SIM_EntityAccessor_impl()
{
}
    
     /**
     *Purpose:
     *<p> facade to get an attribute
     *
     */

     void SIM_EntityAccessor_impl::get_attribute( const char * target_element_path, const char * attr_name, CORBA::Any_out ret_attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{
throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);

}

    /**
     *Purpose:
     *<p> facade to set an attribute
     *
     */

     void SIM_EntityAccessor_impl::set_attribute( const char * target_element_path, const char * attr_name, const CORBA::Any & attr, CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{
throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);

}

 
char*
SIM_EntityAccessor_impl::get_status(
          const char* target_element_path,
          CORBA::String_out entity_info)
    throw (CdmwPlatformMngt::ManagedElementNotFound,
	   CORBA::SystemException)
{
     raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));

    return TestSamples::getEntityStatus();
}

    
void 
SIM_EntityAccessor_impl::set_status(
          const char* target_element_path,
          const char* entity_status, 
          const char* entity_info)
             throw (CdmwPlatformMngt::ManagedElementNotFound,
                    CdmwPlatformMngt::EntityStatusNotAllowed, 
                    CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
}


CdmwPlatformMngt::EntityStatusValues* 
SIM_EntityAccessor_impl::get_status_values(
           const char* target_element_path)
             throw (CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException)
{
     raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));

    return TestSamples::getEntityStatusValues();
}


CdmwPlatformMngt::EntitySnapshot* 
SIM_EntityAccessor_impl::get_snapshot(
            const char* target_element_path)
             throw (CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));

    return TestSamples::getEntitySnapshot();
}


SIM_EntityAccessor_impl::SIM_EntityAccessor_impl (SIM_EntityAccessor_impl& rhs)
{
    CDMW_NEVER_HERE();
}

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
SIM_EntityAccessor_impl& 
SIM_EntityAccessor_impl::operator= (const SIM_EntityAccessor_impl& rhs)
{
    CDMW_NEVER_HERE();
    return *this;
}


} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw

