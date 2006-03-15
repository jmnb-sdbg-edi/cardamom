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

#include "platformftaccessorlibrary/ApplicationAccessor_impl.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtApplication.stub.hpp"

#include "SystemMngt/platformlibrary/ManagedElementKey.hpp"

#include "SystemMngt/platformlibrary/ElementPathsIterator_impl.hpp"



namespace Cdmw
{
namespace PlatformMngt
{

// ----------------------------------------------------------------------
// ApplicationAccessor_impl class.
// ----------------------------------------------------------------------

ApplicationAccessor_impl::ApplicationAccessor_impl(
    CdmwPlatformMngt::ElementRepository_ptr element_repository)
    throw()
    : ManagedElementAccessor_impl(element_repository),
      RWEntityContainerAccessor_impl(element_repository),
      RWServiceDefContainerAccessor_impl(element_repository)
{   
}

ApplicationAccessor_impl::~ApplicationAccessor_impl()
{
}



void ApplicationAccessor_impl::get_attribute( const char * target_element_path, const char * attr_name, CORBA::Any_out ret_attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{

    CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    application_obj->get_attribute(attr_name, ret_attr, flag);

}

void ApplicationAccessor_impl::set_attribute( const char * target_element_path, const char * attr_name, const CORBA::Any & attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{

    CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    application_obj->set_attribute(attr_name, attr, flag);

}


CdmwPlatformMngt::Application_ptr ApplicationAccessor_impl::get_application (
          const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound)
{
    CdmwPlatformMngt::Application_var application_obj;
    
    try
    {
        CdmwPlatformMngt::ManagedElement_var element_obj =
              m_element_repository->get_element(target_element_path);
              
        application_obj = CdmwPlatformMngt::Application::_narrow (element_obj.in());
    }
    catch (const CdmwPlatformMngt::ElementPathInvalid&)
    {
        throw CdmwPlatformMngt::ManagedElementNotFound(target_element_path);
    }
    catch(const CORBA::SystemException&)
    {
        throw CdmwPlatformMngt::ManagedElementNotFound(target_element_path);
    }
    
    return application_obj._retn();                   
}


CdmwPlatformMngt::RestartType ApplicationAccessor_impl::get_restart_type (
      const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
    CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    return application_obj->restart_type();
}
    
   
CORBA::Long ApplicationAccessor_impl::get_restart_attempt (
     const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    return application_obj->restart_attempt();
}
        
    
    
CORBA::Boolean ApplicationAccessor_impl::is_manual_starting(
     const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    return application_obj->is_manual_starting();
}
     
     
  
void ApplicationAccessor_impl::set_manual_starting(
    const char* target_element_path,
    CORBA::Boolean manual_starting)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException,
      CdmwPlatformMngt::IncompatibleStatus)
{
    CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    application_obj->set_manual_starting(manual_starting);
}


CdmwPlatformMngt::ApplicationStatus ApplicationAccessor_impl::get_status(
    const char* target_element_path,
    CORBA::String_out status_info)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)    
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    return application_obj->get_status(status_info);
}

CdmwPlatformMngt::FunctioningMode ApplicationAccessor_impl::get_mode(
    const char* target_element_path,
    CORBA::String_out mode_info)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    return application_obj->get_mode(mode_info);
}

CdmwPlatformMngt::ApplicationSnapshot* ApplicationAccessor_impl::get_snapshot(
    const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    return application_obj->get_snapshot();
}


 CORBA::ULong ApplicationAccessor_impl::get_number_of_processes( const char* target_element_path) 
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    return application_obj->get_number_of_processes();
}

CdmwPlatformMngt::ElementPaths* ApplicationAccessor_impl::get_processes(
    const char* target_element_path,
    CORBA::ULong how_many,
    CdmwPlatformMngt::ElementPathsIterator_out processes_iterator)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
    CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    // get the list of process references
    CdmwPlatformMngt::ProcessesIterator_var processesRemainingIt;
    
	CdmwPlatformMngt::Processes_var processes_seq =
	            application_obj->get_processes (how_many, processesRemainingIt.out());
	            
    // convert each process reference in element path
    
    // create sequence of element path
    CdmwPlatformMngt::ElementPaths_var elements =
                          new CdmwPlatformMngt::ElementPaths();
    
    // set element sequence length 
    size_t seq_len = processes_seq->length();
    elements->length(seq_len);
    
    // fill element sequence
    unsigned int count;
    for (count=0 ; count < seq_len ; count++)
    {
        // Store process path in sequence
        // path is allocated by element_path()
        elements[count] = (processes_seq[count])->element_path();                                           
    }
        
    // Get an iterator on the rest of the elements
    std::list<CdmwPlatformMngt::ElementPath_var> elementsRemaining;
    CdmwPlatformMngt::Process_var process;
    
    // fore each element contained in reference iterator
    while (processesRemainingIt->next_one (process.out()))
    { 
        elementsRemaining.push_back (process->element_path());
    }
    
    // create the path iterator
    CdmwPlatformMngt::ElementPathsIterator_var elementsRemainingIt =
              ElementPathsIterator_impl::create (elementsRemaining);

    // Assign the iterator to the processes_iterator
    processes_iterator = elementsRemainingIt._retn();

    return elements._retn();
}

char* ApplicationAccessor_impl::get_process(
    const char* target_element_path,
    const char* process_name,
    const char* host_name)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::ProcessNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    CdmwPlatformMngt::Process_var process =
             application_obj->get_process(process_name, host_name);
             
    return process->element_path();
}

void ApplicationAccessor_impl::define(
    const char* target_element_path,
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
      CosPropertyService::MultipleExceptions,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    application_obj->define(application_def);
}

char* ApplicationAccessor_impl::add_process(
    const char* target_element_path,
    CdmwPlatformMngt::ProcessDef* process_def)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::ProcessAlreadyExists,
      CdmwPlatformMngt::HostNotFound,
      CdmwPlatformMngt::DuplicateEntity,
      CdmwPlatformMngt::DuplicateService,
      CdmwPlatformMngt::DuplicateStep,
      CdmwPlatformMngt::IncompatibleStatus,
      CosPropertyService::MultipleExceptions, 
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    CdmwPlatformMngt::Process_var process =
             application_obj->add_process(process_def);
             
    return process->element_path();
}

void ApplicationAccessor_impl::remove_process(
    const char* target_element_path,
    const char* process_name,
    const char* host_name)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::ProcessNotFound,
      CdmwPlatformMngt::ProcessStillActive,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    application_obj->remove_process(process_name, host_name);
}

void ApplicationAccessor_impl::set_init_graph(
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
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    application_obj->set_init_graph(process_graph);
}

void ApplicationAccessor_impl::set_stop_graph(
    const char* target_element_path,
    const CdmwPlatformMngt::GraphElements& process_graph)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::ReferenceNotFound,
      CdmwPlatformMngt::CircularReference,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    application_obj->set_stop_graph(process_graph);
}

            
CdmwPlatformMngt::ApplicationAutoRestartInfo ApplicationAccessor_impl::get_autorestart_info(
    const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    return application_obj->get_autorestart_info();
}



void ApplicationAccessor_impl::set_autorestart_info(
    const char* target_element_path,
    const CdmwPlatformMngt::ApplicationAutoRestartInfo& autorestart_info)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    application_obj->set_autorestart_info(autorestart_info);
}
      
      
      
      
      

void ApplicationAccessor_impl::initialise(
    const char* target_element_path,
    const CdmwPlatformMngtBase::StartupKind& startup_kind)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    application_obj->initialise(startup_kind);
}

CdmwPlatformMngt::LongRequestStatus ApplicationAccessor_impl::initialise_and_wait(
    const char* target_element_path,
    const CdmwPlatformMngtBase::StartupKind& startup_kind)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    return application_obj->initialise_and_wait(startup_kind);
}

void ApplicationAccessor_impl::resume(const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    application_obj->resume();
}

CdmwPlatformMngt::LongRequestStatus ApplicationAccessor_impl::resume_and_wait(
     const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    return application_obj->resume_and_wait();
}

void ApplicationAccessor_impl::acknowledge(const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    application_obj->acknowledge();
}

void ApplicationAccessor_impl::run(const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    application_obj->run();
}

void ApplicationAccessor_impl::stop(
     const char* target_element_path,
     CORBA::Boolean emergency)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    application_obj->stop(emergency);
}

CdmwPlatformMngt::LongRequestStatus ApplicationAccessor_impl::stop_and_wait(
    const char* target_element_path,
    CORBA::Boolean emergency)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    return application_obj->stop_and_wait(emergency);
}


void ApplicationAccessor_impl::cold_restart(
    const char* target_element_path,
    const CdmwPlatformMngtBase::StartupKind& startup_kind)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    application_obj->cold_restart(startup_kind);
}
      
            


void ApplicationAccessor_impl::hot_restart(
    const char* target_element_path,
    const CdmwPlatformMngtBase::StartupKind& startup_kind)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::IncompatibleStatus,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Application_var application_obj = 
                        get_application (target_element_path);
                        
    application_obj->hot_restart(startup_kind);
}
 


} // End namespace PlatformMngt
} // End namespace Cdmw

