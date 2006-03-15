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


#include "testbasic/SIM_ApplicationAccessor_impl.hpp"
#include "testbasic/TestSamples.hpp"

#include "testbasic/TestModus.hpp"
#include <Foundation/common/Assert.hpp> 
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <SystemMngt/platformlibrary/ElementPathsIterator_impl.hpp>

namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{


   

SIM_ApplicationAccessor_impl::SIM_ApplicationAccessor_impl(
        SIM_ApplicationAccessor_impl& rhs)
{
    CDMW_NEVER_HERE();
}

SIM_ApplicationAccessor_impl& 
SIM_ApplicationAccessor_impl::operator=(
					const SIM_ApplicationAccessor_impl& rhs)
{ 
    CDMW_NEVER_HERE();
    return *this;
}

SIM_ApplicationAccessor_impl::SIM_ApplicationAccessor_impl()
    throw()
{
}


SIM_ApplicationAccessor_impl::~SIM_ApplicationAccessor_impl()
{
}


CdmwPlatformMngt::RestartType 
SIM_ApplicationAccessor_impl::get_restart_type (const char* target_element_path)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));

    return TestSamples::getRestartType();
}

void SIM_ApplicationAccessor_impl::get_attribute( const char * target_element_path, const char * attr_name, CORBA::Any_out ret_attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{
}

void SIM_ApplicationAccessor_impl::set_attribute( const char * target_element_path, const char * attr_name, const CORBA::Any & attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{
}


CORBA::Long 
SIM_ApplicationAccessor_impl::get_restart_attempt (const char* target_element_path)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));

    return TestSamples::getRestartAttempt();
}

        
CORBA::Boolean 
SIM_ApplicationAccessor_impl::is_manual_starting(const char* target_element_path)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));

    return TestSamples::getManualStarting();
}
     
     
 
void 
SIM_ApplicationAccessor_impl::set_manual_starting(const char* target_element_path,
						  CORBA::Boolean manual_starting)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CdmwPlatformMngt::IncompatibleStatus,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path) ||
			   TestSamples::getManualStarting() != manual_starting);
}

CdmwPlatformMngt::ApplicationStatus 
SIM_ApplicationAccessor_impl::get_status(
					 const char* target_element_path,
					 CORBA::String_out status_info)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));

    return  TestSamples::getApplicationStatus(); 
}


CdmwPlatformMngt::FunctioningMode 
SIM_ApplicationAccessor_impl::get_mode(
				       const char* target_element_path,
				       CORBA::String_out mode_info)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));

    return  TestSamples::getFunctioningMode(); 
}


CdmwPlatformMngt::ApplicationSnapshot* 
SIM_ApplicationAccessor_impl::get_snapshot(
					   const char* target_element_path)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));

    return  TestSamples::getApplicationSnapshot(); 
}

CdmwPlatformMngt::ElementPaths* 
SIM_ApplicationAccessor_impl::get_processes(
					    const char* target_element_path,
					    CORBA::ULong how_many,
					    CdmwPlatformMngt::ElementPathsIterator_out processes_iterator)
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

    processes_iterator = elementsRemainingIt;

    return head._retn();
}


char*
SIM_ApplicationAccessor_impl::get_process(const char* target_element_path,              
					  const char* process_name,
					  const char* host_name)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CdmwPlatformMngt::ProcessNotFound,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));

    CdmwPlatformMngt::ElementPaths_var element_paths =
	TestSamples::getElementPaths(); 

    return CORBA::string_dup((*element_paths)[0]);
}


void 
SIM_ApplicationAccessor_impl::define (const char* target_element_path,            
				      const CdmwPlatformMngt::ApplicationDef& application_def)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CdmwPlatformMngt::DuplicateEntity,
	  CdmwPlatformMngt::DuplicateService,
	  CdmwPlatformMngt::DuplicateProcess,
	  CdmwPlatformMngt::ProcessHostNotFound,
	  CdmwPlatformMngt::DuplicateProcessEntity,
	  CdmwPlatformMngt::DuplicateProcessService,
	  CdmwPlatformMngt::DuplicateProcessStep,
	  CdmwPlatformMngt::AlreadyDone,
	  CdmwPlatformMngt::IncompatibleStatus,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path) );
    // FIXME verify arg
}


char*
SIM_ApplicationAccessor_impl::add_process(const char* target_element_path,
					  CdmwPlatformMngt::ProcessDef* process_def)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CdmwPlatformMngt::ProcessAlreadyExists,
	  CdmwPlatformMngt::HostNotFound,
	  CdmwPlatformMngt::DuplicateEntity,
	  CdmwPlatformMngt::DuplicateService,
	  CdmwPlatformMngt::DuplicateStep,
	  CdmwPlatformMngt::IncompatibleStatus,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path) );
    
    return TestSamples::getElementPath();
}


void 
SIM_ApplicationAccessor_impl::remove_process(
					     const char* target_element_path,
					     const char* process_name,
					     const char* host_name)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CdmwPlatformMngt::ProcessNotFound,
	  CdmwPlatformMngt::ProcessStillActive,
	  CdmwPlatformMngt::IncompatibleStatus,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path) );
    // FIXME verify arg
}


void 
SIM_ApplicationAccessor_impl::set_init_graph(
					     const char* target_element_path,
					     const CdmwPlatformMngt::GraphElements& process_graph)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CdmwPlatformMngt::ReferenceNotFound,
	  CdmwPlatformMngt::CircularReference,
	  CdmwPlatformMngt::StepOutOfBound,
	  CdmwPlatformMngt::StepJump,
	  CdmwPlatformMngt::IncompatibleStatus,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path) );
    // FIXME verify arg
}

void 
SIM_ApplicationAccessor_impl::set_stop_graph(
					     const char* target_element_path,
					     const CdmwPlatformMngt::GraphElements& process_graph)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CdmwPlatformMngt::ReferenceNotFound,
	  CdmwPlatformMngt::CircularReference,
	  CdmwPlatformMngt::IncompatibleStatus,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path) );
    // FIXME verify arg
}


CdmwPlatformMngt::ApplicationAutoRestartInfo 
SIM_ApplicationAccessor_impl::get_autorestart_info(
						   const char* target_element_path)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path) );
    return TestSamples::getApplicationAutoRestartInfo();
}



void 
SIM_ApplicationAccessor_impl::set_autorestart_info(
						   const char* target_element_path,
						   const CdmwPlatformMngt::ApplicationAutoRestartInfo& autorestart_info)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CdmwPlatformMngt::IncompatibleStatus,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path) );   
    // FIXME verify arg
}


void 
SIM_ApplicationAccessor_impl::initialise(const char* target_element_path,
					 const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CdmwPlatformMngt::IncompatibleStatus,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path) );   
    // FIXME verify arg
}


CdmwPlatformMngt::LongRequestStatus 
SIM_ApplicationAccessor_impl::initialise_and_wait(
						  const char* target_element_path,
						  const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CdmwPlatformMngt::IncompatibleStatus,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path) );
    return TestSamples::getLongRequestStatus();
}


void 
SIM_ApplicationAccessor_impl::resume(const char* target_element_path)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CdmwPlatformMngt::IncompatibleStatus,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path) );
}


CdmwPlatformMngt::LongRequestStatus 
SIM_ApplicationAccessor_impl::resume_and_wait(
					      const char* target_element_path)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CdmwPlatformMngt::IncompatibleStatus,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path) );
    return TestSamples::getLongRequestStatus();
}


void 
SIM_ApplicationAccessor_impl::acknowledge(const char* target_element_path)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CdmwPlatformMngt::IncompatibleStatus,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path) );
}


void 
SIM_ApplicationAccessor_impl::run(const char* target_element_path)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CdmwPlatformMngt::IncompatibleStatus,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path) );
}


void 
SIM_ApplicationAccessor_impl::stop(const char* target_element_path,
				   CORBA::Boolean emergency)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path) );
}


CdmwPlatformMngt::LongRequestStatus 
SIM_ApplicationAccessor_impl::stop_and_wait(
					    const char* target_element_path,
					    CORBA::Boolean emergency)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path) );
    return TestSamples::getLongRequestStatus();
}

            
void 
SIM_ApplicationAccessor_impl::cold_restart(const char* target_element_path,
					   const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CdmwPlatformMngt::IncompatibleStatus,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path) );
}
        
CORBA::ULong SIM_ApplicationAccessor_impl::get_number_of_processes( const char* target_element_path) throw(CdmwPlatformMngt::ManagedElementNotFound, CORBA::SystemException)
  {
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
  }
            

void 
SIM_ApplicationAccessor_impl::hot_restart(const char* target_element_path,
					  const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CdmwPlatformMngt::IncompatibleStatus,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path) );
} 

} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw

