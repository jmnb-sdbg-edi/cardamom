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


#include "testbasic/SIM_ProcessAccessor_impl.hpp"
#include "testbasic/TestSamples.hpp"

#include "testbasic/TestModus.hpp"
#include <Foundation/common/Assert.hpp> 
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{


SIM_ProcessAccessor_impl::SIM_ProcessAccessor_impl(const SIM_ProcessAccessor_impl& rhs) 
{
    CDMW_NEVER_HERE();
}

 
SIM_ProcessAccessor_impl& 
SIM_ProcessAccessor_impl::operator=(const SIM_ProcessAccessor_impl& rhs) 
{
    CDMW_NEVER_HERE();
    return *this;
}


SIM_ProcessAccessor_impl::SIM_ProcessAccessor_impl()
    throw() 
{
}

SIM_ProcessAccessor_impl::~SIM_ProcessAccessor_impl()
{
}

     /**
     *Purpose:
     *<p> facade to get an attribute
     *
     */

     void SIM_ProcessAccessor_impl::get_attribute( const char * target_element_path, const char * attr_name, CORBA::Any_out ret_attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{
throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

    /**
     *Purpose:
     *<p> facade to set an attribute
     *
     */

     void SIM_ProcessAccessor_impl::set_attribute( const char * target_element_path, const char * attr_name, const CORBA::Any & attr, CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{
throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


CdmwPlatformMngt::ProcessMode SIM_ProcessAccessor_impl::get_process_mode(const char* path)throw(CORBA::SystemException)
{
throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

CORBA::Boolean SIM_ProcessAccessor_impl::get_memory_resident(const char* path)throw(CORBA::SystemException)
{
throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

CORBA::Long SIM_ProcessAccessor_impl::get_core_dump_size(const char* path)throw(CORBA::SystemException)
{
throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


void SIM_ProcessAccessor_impl::set_memory_resident(const char* path,bool val)throw(CORBA::SystemException)
{
throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

CdmwPlatformMngt::ProcessType 
SIM_ProcessAccessor_impl::get_type (const char* target_element_path)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CORBA::SystemException) 
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getProcessType();
}



CORBA::Object_ptr 
SIM_ProcessAccessor_impl::get_embedded_service(const char* target_element_path)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getEmbeddedService();
}
              

CdmwPlatformMngt::ProcessPID 
SIM_ProcessAccessor_impl::get_pid(const char* target_element_path)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CORBA::SystemException) 
{  
   raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getProcessPID();
}
    

CORBA::Boolean 
SIM_ProcessAccessor_impl::get_created_dynamically
(const char* target_element_path)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getCreatedDynamically();
}
     
     
CdmwPlatformMngt::RestartType 
SIM_ProcessAccessor_impl::get_restart_type(const char* target_element_path)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getRestartType();
}

    
    
CORBA::Long 
SIM_ProcessAccessor_impl::get_restart_attempt(const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getRestartAttempt();
}

    
    
CdmwPlatformMngtBase::TimeStamp 
SIM_ProcessAccessor_impl::get_start_time(const char* target_element_path)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getTimeStamp();
}

    
CdmwPlatformMngtBase::TimeStamp 
SIM_ProcessAccessor_impl::get_stop_time(const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getTimeStamp();
}
        

CORBA::Boolean 
SIM_ProcessAccessor_impl::is_manual_starting(const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getManualStarting();
}

    
void 
SIM_ProcessAccessor_impl::set_manual_starting
(const char* target_element_path,
 CORBA::Boolean manual_starting)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CdmwPlatformMngt::IncompatibleStatus,
	  CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path) ||
			   TestSamples::getManualStarting() != manual_starting);}

    
CdmwPlatformMngt::ProcessStatus
SIM_ProcessAccessor_impl::get_status(const char* target_element_path,
	   CORBA::String_out status_info)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getProcessStatus();
}

              
CdmwPlatformMngt::ElementName 
SIM_ProcessAccessor_impl::get_host_name(const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return CORBA::string_dup(target_element_path);
}

char* 
SIM_ProcessAccessor_impl::get_host(const char* target_element_path)
    throw(CdmwPlatformMngt::ManagedElementNotFound,
	  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getElementPath();
}
              
 

CORBA::Boolean 
SIM_ProcessAccessor_impl::is_auto_ending(const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getAutoEnding();
}

              
              
void 
SIM_ProcessAccessor_impl::set_auto_ending(const char* target_element_path,
					  CORBA::Boolean auto_ending)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CdmwPlatformMngt::IncompatibleStatus,
                    CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path) ||
			   TestSamples::getAutoEnding() != auto_ending);
}




CdmwPlatformMngt::ProcessInfo* 
SIM_ProcessAccessor_impl::get_process_info(const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException)
{  
     raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getProcessInfo();  
}

void 
SIM_ProcessAccessor_impl::set_process_info(const char* target_element_path,
                          const CdmwPlatformMngt::ProcessInfo& process_info )
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    CdmwPlatformMngt::ProcessInfo_var check = 
	TestSamples::getProcessInfo();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path) );
}


CdmwPlatformMngt::LifeCycleDef* 
SIM_ProcessAccessor_impl::get_life_cycle(const char* target_element_path)
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::IncompatibleType,
                   CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getLifeCycleDef();  
}

void 
SIM_ProcessAccessor_impl::set_life_cycle(const char* target_element_path,
                        const CdmwPlatformMngt::LifeCycleDef& life_cycle)
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::IncompatibleType,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    CdmwPlatformMngt::LifeCycleDef_var check = 
	TestSamples::getLifeCycleDef();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
}
    

CdmwPlatformMngt::CompleteMonitoringInfo 
SIM_ProcessAccessor_impl::get_monitoring_info(
                         const char* target_element_path)
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::IncompatibleType,
                   CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getCompleteMonitoringInfo(); 
}


void 
SIM_ProcessAccessor_impl::set_monitoring_info(
                         const char* target_element_path,
                         const CdmwPlatformMngt::CompleteMonitoringInfo& monitoring_info)
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::IncompatibleType,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    CdmwPlatformMngt::CompleteMonitoringInfo_var check = 
	TestSamples::getCompleteMonitoringInfo();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
}

                   
CdmwPlatformMngt::ExitAutoRestartInfo* 
SIM_ProcessAccessor_impl::get_exit_autorestart_info(
                         const char* target_element_path)
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getExitAutoRestartInfo(); 
}



 void 
SIM_ProcessAccessor_impl::set_exit_autorestart_info(
                         const char* target_element_path,
                         const CdmwPlatformMngt::ExitAutoRestartInfo& exit_autorestart_info)
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException)
{  
     raiseExceptionIfExceptionModus();
    
    CdmwPlatformMngt::ExitAutoRestartInfo_var check = 
	TestSamples::getExitAutoRestartInfo();

    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
}


CdmwPlatformMngt::ProcessSnapshot* 
SIM_ProcessAccessor_impl::get_snapshot(const char* target_element_path)
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getProcessSnapshot(); 
}
             
CdmwPlatformMngt::RequestStatus 
SIM_ProcessAccessor_impl::initialise(
                          const char* target_element_path,
                          const CdmwPlatformMngtBase::StartupKind& startup_kind)
             throw (CdmwPlatformMngt::ManagedElementNotFound,
                    CdmwPlatformMngt::IncompatibleStatus,
                    CORBA::SystemException)
{  
     raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getRequestStatus(); 
}


CdmwPlatformMngt::RequestStatus 
SIM_ProcessAccessor_impl::next_step(const char* target_element_path)
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::InvalidStep,
                   CdmwPlatformMngt::IncompatibleType,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getRequestStatus(); 
}



CdmwPlatformMngt::RequestStatus 
SIM_ProcessAccessor_impl::run(const char* target_element_path)
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::NotReadyToRun,
                   CdmwPlatformMngt::IncompatibleType,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getRequestStatus(); 
}


CdmwPlatformMngt::RequestStatus 
SIM_ProcessAccessor_impl::stop(const char* target_element_path,
                                         CORBA::Boolean emergency)
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getRequestStatus(); 
}

             
             
CdmwPlatformMngt::RequestStatus 
SIM_ProcessAccessor_impl::cold_restart(
                          const char* target_element_path,
                          const CdmwPlatformMngtBase::StartupKind& startup_kind)
             throw (CdmwPlatformMngt::ManagedElementNotFound,
                    CdmwPlatformMngt::IncompatibleStatus,
                    CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getRequestStatus(); 
}

             

CdmwPlatformMngt::RequestStatus 
SIM_ProcessAccessor_impl::hot_restart(
                          const char* target_element_path,
                          const CdmwPlatformMngtBase::StartupKind& startup_kind)
             throw (CdmwPlatformMngt::ManagedElementNotFound,
                    CdmwPlatformMngt::IncompatibleStatus,
                    CORBA::SystemException)
{  
    raiseExceptionIfExceptionModus();
    
    raiseSystemExceptionIf(! TestSamples::hasElementPath(target_element_path));
    // FIXME verify the porperty list
    
    return TestSamples::getRequestStatus(); 
}

} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw

