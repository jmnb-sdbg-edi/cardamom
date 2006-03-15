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



#include "testbasic/SIM_HostAccessor_impl.hpp"
#include "testbasic/TestModus.hpp"
#include "testbasic/TestSamples.hpp"
#include <SystemMngt/platformlibrary/ElementPathsIterator_impl.hpp>
#include <Foundation/common/Assert.hpp> 

namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{

SIM_HostAccessor_impl::SIM_HostAccessor_impl(
        SIM_HostAccessor_impl& rhs)
{
     CDMW_NEVER_HERE(); // FIXME tbd
}


SIM_HostAccessor_impl& SIM_HostAccessor_impl::operator=(
		    const SIM_HostAccessor_impl& rhs) 
{
    CDMW_NEVER_HERE(); // FIXME tbd
    return *this;
}
        
SIM_HostAccessor_impl::SIM_HostAccessor_impl()
    throw()
{
}

SIM_HostAccessor_impl::~SIM_HostAccessor_impl() 
{
}
    
    
///
/*
virtual char * get_host_status_info(const char* path)throw(CORBA::SystemException)
{
throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}
*/

/*
CdmwPlatformMngt::CompleteMonitoringInfo get_monitoring_parameters(const char* path)throw(CORBA::SystemException)
{
throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

void set_monitoring_parameters(const CdmwPlatformMngt::CompleteMonitoringInfo&,const char* path)throw(CORBA::SystemException)
{
throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

CORBA::Boolean get_global_supervision_indicator(const char* path)throw(CORBA::SystemException)
{
throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

void set_global_supervision_indicator(bool val,const char* path)throw(CORBA::SystemException)
{
throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

*/

CdmwPlatformMngt::HostStatus 
SIM_HostAccessor_impl::get_status
(const char* target_element_path)
             throw (CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();


//     enum HostStatus
//     {
//         /**
//         * The host is responding
//         */
//         HOST_RESPONDING,

//         /**
//         * The host is not responding
//         */
//         HOST_UNREACHABLE
//     };

    return CdmwPlatformMngt::HOST_RESPONDING;
}

    /**
     *Purpose:
     *<p> facade to get an attribute
     *
     */

     void SIM_HostAccessor_impl::get_attribute( const char * target_element_path, const char * attr_name, CORBA::Any_out ret_attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{
throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

    /**
     *Purpose:
     *<p> facade to set an attribute
     *
     */

     void SIM_HostAccessor_impl::set_attribute( const char * target_element_path, const char * attr_name, const CORBA::Any & attr, CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{
throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


char* 
SIM_HostAccessor_impl::get_platform_application_exe
(const char* target_element_path)
    throw (CdmwPlatformMngt::ManagedElementNotFound,
	   CORBA::SystemException) 
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementName(target_element_path));

    return TestSamples::getPlatformApplicationExe();
}
    
    
CdmwPlatformMngt::ElementPaths* 
SIM_HostAccessor_impl::get_processes
(const char* target_element_path,
 CORBA::ULong how_many,
 CdmwPlatformMngt::ElementPathsIterator_out processes_iterator)
    throw (CdmwPlatformMngt::ManagedElementNotFound,
	   CORBA::SystemException)
{
    raiseExceptionIfExceptionModus(); 
   
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
  
    raiseExceptionIfExceptionModus();

    CdmwPlatformMngt::ElementPaths_var elems = 
	TestSamples::getElementPaths();

    CdmwPlatformMngt::ElementPaths_var head =
	new CdmwPlatformMngt::ElementPaths();
    CdmwPlatformMngt::ElementPaths_var tail =
	new CdmwPlatformMngt::ElementPaths();
	
    split(elems, how_many, head, tail);

    // create the path iterator
    CdmwPlatformMngt::ElementPathsIterator_var elementsRemainingIt =
	ElementPathsIterator_impl::create (seq2list(tail));

    processes_iterator = elementsRemainingIt;

    return head._retn();
}

  
   
void 
SIM_HostAccessor_impl::shutdown (const char* target_element_path)
    throw (CdmwPlatformMngt::ManagedElementNotFound,
	   CORBA::SystemException) 
{
    raiseExceptionIfExceptionModus();
   
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
}


void 
SIM_HostAccessor_impl::cold_reboot (const char* target_element_path)
    throw (CdmwPlatformMngt::ManagedElementNotFound,
	   CORBA::SystemException) 
{
    raiseExceptionIfExceptionModus();
   
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
}


void 
SIM_HostAccessor_impl::hot_reboot (const char* target_element_path)
    throw (CdmwPlatformMngt::ManagedElementNotFound,
	   CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
   
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
}
    


} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw
