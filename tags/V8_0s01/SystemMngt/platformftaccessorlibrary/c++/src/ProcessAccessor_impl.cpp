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

#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "platformftaccessorlibrary/ProcessAccessor_impl.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcess.stub.hpp"

#include "SystemMngt/platformlibrary/ManagedElementKey.hpp"


namespace Cdmw
{
namespace PlatformMngt
{

ProcessAccessor_impl::ProcessAccessor_impl(
    CdmwPlatformMngt::ElementRepository_ptr element_repository)
throw()
    : ManagedElementAccessor_impl(element_repository),
      RWEntityContainerAccessor_impl(element_repository),
      RWServiceDefContainerAccessor_impl(element_repository)
{        
}


ProcessAccessor_impl::~ProcessAccessor_impl()
{
}

CdmwPlatformMngt::Process_ptr ProcessAccessor_impl::get_process (
          const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound)
{
    CdmwPlatformMngt::Process_var process_obj;
    
    try
    {
        CdmwPlatformMngt::ManagedElement_var element_obj =
              m_element_repository->get_element(target_element_path);
              
        process_obj = CdmwPlatformMngt::Process::_narrow (element_obj.in());
    }
    catch (const CdmwPlatformMngt::ElementPathInvalid&)
    {
        throw CdmwPlatformMngt::ManagedElementNotFound(target_element_path);
    }
    catch(const CORBA::SystemException&)
    {
        throw CdmwPlatformMngt::ManagedElementNotFound(target_element_path);
    }
    
    return process_obj._retn();                   
}

CdmwPlatformMngt::ProcessType ProcessAccessor_impl::get_type(
                             const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
    CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->type();
}

CORBA::Object_ptr ProcessAccessor_impl::get_embedded_service(
                             const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->embedded_service();
}

CdmwPlatformMngt::ProcessPID ProcessAccessor_impl::get_pid(
                             const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->pid();
}

CORBA::Boolean ProcessAccessor_impl::get_created_dynamically(
                             const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->created_dynamically();
}
   
CdmwPlatformMngt::RestartType ProcessAccessor_impl::get_restart_type(
                             const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->restart_type();
}

       
CORBA::Long ProcessAccessor_impl::get_restart_attempt(
                             const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->restart_attempt();
}

void ProcessAccessor_impl::get_attribute( const char * target_element_path, const char * attr_name, CORBA::Any_out ret_attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    process_obj->get_attribute(attr_name, ret_attr, flag);
}

void ProcessAccessor_impl::set_attribute(  const char * target_element_path, const char * attr_name, const CORBA::Any & attr, CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
 {

	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    process_obj->set_attribute(attr_name, attr, flag);

 }

bool ProcessAccessor_impl::get_run_as_root(
                             const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->get_run_as_root();
}

////
CdmwPlatformMngt::ProcessMode ProcessAccessor_impl::get_process_mode(
                             const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = get_process (target_element_path);
                        
    return process_obj->get_process_mode();
}

///
bool ProcessAccessor_impl::get_memory_resident(
                             const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = get_process (target_element_path);
                        
    return process_obj->get_memory_resident();
}

///
void ProcessAccessor_impl::set_memory_resident( const char* target_element_path, bool memory_resident)
throw(CdmwPlatformMngt::ManagedElementNotFound, CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = get_process (target_element_path);
                        
    process_obj->set_memory_resident(memory_resident);
}
///
CORBA::Long ProcessAccessor_impl::get_core_dump_size( const char* target_element_path )
throw(CdmwPlatformMngt::ManagedElementNotFound, CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = get_process (target_element_path);
                        
    return(process_obj->get_core_dump_size());
}



CdmwPlatformMngtBase::TimeStamp ProcessAccessor_impl::get_start_time(
                             const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->start_time();
}

CdmwPlatformMngtBase::TimeStamp ProcessAccessor_impl::get_stop_time(
                             const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->stop_time();
}

        
CORBA::Boolean ProcessAccessor_impl::is_manual_starting(
                             const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->is_manual_starting();
}

        
void ProcessAccessor_impl::set_manual_starting(
                             const char* target_element_path,
                             CORBA::Boolean manual_starting)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    process_obj->set_manual_starting(manual_starting);
}

                    
CdmwPlatformMngt::ProcessStatus ProcessAccessor_impl::get_status(
                              const char* target_element_path,
                              CORBA::String_out status_info)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->get_status(status_info);
}

                       

CORBA::Boolean ProcessAccessor_impl::is_auto_ending(
                              const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->is_auto_ending();
}

void ProcessAccessor_impl::set_auto_ending(
                              const char* target_element_path,
                              CORBA::Boolean auto_ending)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    process_obj->set_auto_ending(auto_ending);
}

char* ProcessAccessor_impl::get_host (const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    CdmwPlatformMngt::Host_var host = process_obj->get_host();
        
    return host->element_path();
}


CdmwPlatformMngt::ProcessInfo* ProcessAccessor_impl::get_process_info(
                              const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->get_process_info();
}

void ProcessAccessor_impl::set_process_info(
                              const char* target_element_path,
                              const CdmwPlatformMngt::ProcessInfo& process_info )
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    process_obj->set_process_info(process_info);
}


CdmwPlatformMngt::LifeCycleDef* ProcessAccessor_impl::get_life_cycle(
                              const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::IncompatibleType,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->get_life_cycle();
}

void ProcessAccessor_impl::set_life_cycle(
                              const char* target_element_path,
                              const CdmwPlatformMngt::LifeCycleDef& life_cycle)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::DuplicateStep,
      CdmwPlatformMngt::IncompatibleType,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    process_obj->set_life_cycle(life_cycle);
}

CdmwPlatformMngt::CompleteMonitoringInfo ProcessAccessor_impl::get_monitoring_info(
                               const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::IncompatibleType,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->get_monitoring_info();
}

void ProcessAccessor_impl::set_monitoring_info(
                               const char* target_element_path,
                               const CdmwPlatformMngt::CompleteMonitoringInfo& monitoring_info)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::IncompatibleType,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    process_obj->set_monitoring_info(monitoring_info);
}

CdmwPlatformMngt::ExitAutoRestartInfo* ProcessAccessor_impl::get_exit_autorestart_info(
                               const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->get_exit_autorestart_info();
}

void ProcessAccessor_impl::set_exit_autorestart_info(
                               const char* target_element_path,
                               const CdmwPlatformMngt::ExitAutoRestartInfo& exit_autorestart_info)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    process_obj->set_exit_autorestart_info(exit_autorestart_info);
}



CdmwPlatformMngt::ProcessSnapshot* ProcessAccessor_impl::get_snapshot(
                               const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->get_snapshot();
}

CdmwPlatformMngt::RequestStatus ProcessAccessor_impl::initialise(
                               const char* target_element_path,
                               const CdmwPlatformMngtBase::StartupKind& startup_kind)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->initialise(startup_kind);
}



CdmwPlatformMngt::RequestStatus ProcessAccessor_impl::next_step(
                                const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::InvalidStep,
      CdmwPlatformMngt::IncompatibleType,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->next_step();
}

CdmwPlatformMngt::RequestStatus ProcessAccessor_impl::run(
                                const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::NotReadyToRun,
      CdmwPlatformMngt::IncompatibleType,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->run();
}

CdmwPlatformMngt::RequestStatus ProcessAccessor_impl::stop(
                                const char* target_element_path,
                                CORBA::Boolean emergency)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->stop(emergency);
}

             
             

CdmwPlatformMngt::RequestStatus ProcessAccessor_impl::cold_restart(
                                const char* target_element_path,
                                const CdmwPlatformMngtBase::StartupKind& startup_kind)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->cold_restart(startup_kind);
}

             


CdmwPlatformMngt::RequestStatus ProcessAccessor_impl::hot_restart(
                               const char* target_element_path,
                               const CdmwPlatformMngtBase::StartupKind& startup_kind)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Process_var process_obj = 
                        get_process (target_element_path);
                        
    return process_obj->hot_restart(startup_kind);
}





} // End namespace PlatformMngt
} // End namespace Cdmw

