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


#include "testbasic/SIM_EntityContainerAccessor_impl.hpp"
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


SIM_EntityContainerAccessor_impl::SIM_EntityContainerAccessor_impl()
                throw()
{
}


SIM_EntityContainerAccessor_impl::~SIM_EntityContainerAccessor_impl()
{}

      
CORBA::ULong 
SIM_EntityContainerAccessor_impl::get_number_of_entities(
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
SIM_EntityContainerAccessor_impl::get_all_entity_names (
				   const char* target_element_path,
		                   CORBA::ULong how_many,
                                   CdmwPlatformMngt::ElementNames_out entity_names,
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
    CdmwPlatformMngt::ElementNames_var tail =
	new CdmwPlatformMngt::ElementNames();
	
    split(elems, how_many, head, tail);

    // create the path iterator
    CdmwPlatformMngt::ElementNamesIterator_var elementsRemainingIt =
	ElementNamesIterator_impl::create (seq2list(tail));

    entity_names = head._retn();
    rest         = elementsRemainingIt._retn(); 
}



char*
SIM_EntityContainerAccessor_impl::get_entity(const char* target_element_path,
		                const char* entity_name)
                throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CdmwPlatformMngt::EntityNotFound,
                      CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    
    return TestSamples::getElementName();
}

CORBA::Boolean 
SIM_EntityContainerAccessor_impl::get_entities (
		      const char * target_element_path,
		      const ::CdmwPlatformMngt::ElementNames & entity_names,
		      ::CdmwPlatformMngt::ElementPaths_out entities
		      ) 
	throw (CORBA::SystemException,
	       ::CdmwPlatformMngt::ManagedElementNotFound)
{
    raiseExceptionIfExceptionModus();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    
    entities = TestSamples::getElementPaths();

    return true;
}

void 
SIM_EntityContainerAccessor_impl::get_all_entities(
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




SIM_EntityContainerAccessor_impl&
SIM_EntityContainerAccessor_impl::operator=(const SIM_EntityContainerAccessor_impl& rhs)
    throw()
{
     throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO); // FIXME tbd
     return *this;
}



}  // End namespace Proxy
}  // End namespace PlatformMngt
}  // End namespace Cdmw


