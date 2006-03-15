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




#include "testbasic/SIM_HostContainerAccessor_impl.hpp"
#include <SystemMngt/platformlibrary/ElementPathsIterator_impl.hpp>
#include <SystemMngt/platformlibrary/ElementNamesIterator_impl.hpp>

#include "testbasic/TestModus.hpp"
#include "testbasic/TestSamples.hpp"
#include <Foundation/common/Assert.hpp> 
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

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



SIM_HostContainerAccessor_impl::SIM_HostContainerAccessor_impl()
    throw() 
{
}
        


SIM_HostContainerAccessor_impl::~SIM_HostContainerAccessor_impl()
{
}


CORBA::ULong 
SIM_HostContainerAccessor_impl::get_number_of_hosts(
	        const char* target_element_path)
                throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CORBA::SystemException) 
{
   raiseExceptionIfExceptionModus();
   
   raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));

   CdmwPlatformMngt::ElementPaths_var elems = 
       TestSamples::getElementPaths();
   
   return elems->length();
}


void 
SIM_HostContainerAccessor_impl::get_all_host_names (
                                 const char* target_element_path,
		                 CORBA::ULong how_many,
                                 CdmwPlatformMngt::ElementNames_out host_names,
                                 CdmwPlatformMngt::ElementNamesIterator_out rest)
                throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CORBA::SystemException) 
{
    raiseExceptionIfExceptionModus();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    
    CdmwPlatformMngt::ElementNames_var elems = 
	TestSamples::getElementNames();

    CdmwPlatformMngt::ElementNames_var head = 
	new CdmwPlatformMngt::ElementNames();
    CdmwPlatformMngt::ElementNames_var tail= 
	new CdmwPlatformMngt::ElementNames();
	
    split(elems, how_many, head, tail);

    // create the path iterator
    CdmwPlatformMngt::ElementNamesIterator_var elementsRemainingIt =
	ElementNamesIterator_impl::create (seq2list(tail));

    host_names = head._retn();
    rest       = elementsRemainingIt._retn();
}




char*
SIM_HostContainerAccessor_impl::get_host(const char* target_element_path,
					 const char* host_name)
                throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CdmwPlatformMngt::HostNotFound,
                      CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    
    return TestSamples::getElementPath();
}
               
CORBA::Boolean 
SIM_HostContainerAccessor_impl::get_hosts (
	       const char* target_element_path,
	       const CdmwPlatformMngt::ElementNames& host_names,
	       CdmwPlatformMngt::ElementPaths_out hosts)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    
    hosts = TestSamples::getElementPaths();
    
    return true;
}                    

void  
SIM_HostContainerAccessor_impl::get_all_hosts(
                           const char* target_element_path,
		           CORBA::ULong how_many,
                           CdmwPlatformMngt::ElementPaths_out hosts,
                           CdmwPlatformMngt::ElementPathsIterator_out rest)
                throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CORBA::SystemException) 
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    
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

    hosts  = head._retn();
    rest   = elementsRemainingIt._retn();
}


        
 
SIM_HostContainerAccessor_impl::SIM_HostContainerAccessor_impl (
                         const SIM_HostContainerAccessor_impl& rhs)
    throw() 
{ 
    CDMW_NEVER_HERE();
} 



SIM_HostContainerAccessor_impl&
SIM_HostContainerAccessor_impl::operator=(
		          const SIM_HostContainerAccessor_impl& rhs)
    throw() 
{
    CDMW_NEVER_HERE();
    return *this;
}


//         CdmwPlatformMngt::ElementRepository_var SIM_HostContainerAccessor_impl::m_element_repository;
        
}   // End namespace Proxy
}   // End namespace PlatformMngt
}   // End namespace Cdmw


