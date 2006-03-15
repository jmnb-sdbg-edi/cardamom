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



#include "testbasic/SIM_RWServiceDefContainerAccessor_impl.hpp"
#include <Foundation/common/Assert.hpp> 

#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp>
#include <Foundation/common/Assert.hpp> 
#include "testbasic/TestSamples.hpp"
#include "testbasic/TestModus.hpp"
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


SIM_RWServiceDefContainerAccessor_impl::SIM_RWServiceDefContainerAccessor_impl()
                throw()
{
}


 SIM_RWServiceDefContainerAccessor_impl::~SIM_RWServiceDefContainerAccessor_impl()
 {
 }

void 
SIM_RWServiceDefContainerAccessor_impl::add_service_def (
		     const char* target_element_path,
		     const CdmwPlatformMngtService::ServiceDef& service_def)
		        throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CdmwPlatformMngtService::ServiceAlreadyExists,
		              CORBA::SystemException)
 {
     raiseExceptionIfExceptionModus();
      
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
 
 }

void 
SIM_RWServiceDefContainerAccessor_impl::remove_service_def (
		     const char* target_element_path,
		     const char* service_name)
                throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CdmwPlatformMngtService::ServiceNotFound,
                      CORBA::SystemException)
 {
     raiseExceptionIfExceptionModus();
      
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
 
 }

                      
                      
	
void 
SIM_RWServiceDefContainerAccessor_impl::remove_all_service_defs (
		     const char* target_element_path)
                throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CORBA::SystemException)
 {
     raiseExceptionIfExceptionModus();
      
     raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
 }

                      
                      
                      
                      

SIM_RWServiceDefContainerAccessor_impl::SIM_RWServiceDefContainerAccessor_impl (const SIM_RWServiceDefContainerAccessor_impl& rhs)
                throw() 
{
      	// TODO
    CDMW_NEVER_HERE();
}

    




SIM_RWServiceDefContainerAccessor_impl&
        SIM_RWServiceDefContainerAccessor_impl::operator=(const SIM_RWServiceDefContainerAccessor_impl& rhs)
                throw()
{
     	// TODO
     CDMW_NEVER_HERE();
    return *this;
}



}  // End namespace Proxy
}  // End namespace PlatformMngt
}  // End namespace Cdmw

