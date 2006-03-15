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


#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

#include "platformsystemlibrary/System_impl.hpp"
#include "platformsystemlibrary/Host_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "platformapplicationlibrary/Process_impl.hpp"
#include "platformapplicationlibrary/ManagedProcess_impl.hpp"

#include "SystemMngt/platformelementlibrary/Entity_impl.hpp"
#include "SystemMngt/platformvaluetypes/ProcessStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/UserEvent_impl.hpp"
#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"
#include "SystemMngt/platformlibrary/ManagedElementKey.hpp"
#include "SystemMngt/platformlibrary/ElementRepository_impl.hpp"

#include "platformftaccessorlibrary/FTProcessObserver_impl.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

// ----------------------------------------------------------------------
// FTProcessObserver_impl class.
// ----------------------------------------------------------------------
FTProcessObserver_impl::FTProcessObserver_impl(
    PortableServer::POA_ptr poa,
    System_impl* system,
    CdmwPlatformMngt::ElementRepository_ptr element_repository)
: DeactivableServant_impl (poa)
{
    m_system = system;
    m_element_repository = 
           CdmwPlatformMngt::ElementRepository::_duplicate(element_repository);
}

void FTProcessObserver_impl::notify_message(
    const char* application_name,
    const char* process_name,
    const char* host_name,
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* issuer,
    const char* message)
throw (CdmwPlatformMngt::ProcessNotFound,
       CORBA::SystemException)
{
    try
    {
        // search the application process observer and relay the command
        CdmwPlatformMngt::ProcessObserver_var app_proc_observer = 
                               m_system->get_process_observer (application_name);
                               
        app_proc_observer->notify_message(application_name, process_name,
                                          host_name, header, issuer, message);
    }
    catch (CdmwPlatformMngt::ApplicationNotFound)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }    
    catch (...)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}

void FTProcessObserver_impl::notify_error(
    const char* application_name,
    const char* process_name,
    const char* host_name,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp,
    const char* issuer,
    const char* error_info)
throw (CdmwPlatformMngt::ProcessNotFound,
       CORBA::SystemException)
{
    try
    {
        // search the application process observer and relay the command
        CdmwPlatformMngt::ProcessObserver_var app_proc_observer = 
                               m_system->get_process_observer (application_name);
                               
        app_proc_observer->notify_error (application_name, process_name,
                                         host_name, time_stamp, issuer, error_info);
    }
    catch (CdmwPlatformMngt::ApplicationNotFound)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }
    catch(...)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}


void FTProcessObserver_impl::notify_starting(
    const char* application_name,
    const char* process_name,
    const char* host_name,
    CdmwPlatformMngt::ProcessPID pid,
    const CdmwPlatformMngtBase::TimeStamp & time_stamp,
    CORBA::Long restart_attempt)
            throw (CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException)
{
	// TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}



void FTProcessObserver_impl::notify_ending(
    const char* application_name,
    const char* process_name,
    const char* host_name,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp,
    CORBA::Long exit_status)
throw (CdmwPlatformMngt::ProcessNotFound,
       CORBA::SystemException)
{
    try
    {
        // search the application process observer and relay the command
        CdmwPlatformMngt::ProcessObserver_var app_proc_observer = 
                               m_system->get_process_observer (application_name);
                               
        app_proc_observer->notify_ending (application_name, process_name,
                                          host_name, time_stamp, exit_status);
    }
    catch (CdmwPlatformMngt::ApplicationNotFound)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }
    catch (...)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}

void FTProcessObserver_impl::notify_monitoring_failure(
    const char* application_name,
    const char* process_name,
    const char* host_name,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp)
throw (CdmwPlatformMngt::ProcessNotFound,
       CORBA::SystemException)
{
    try
    {
        // search the application process observer and relay the command
        CdmwPlatformMngt::ProcessObserver_var app_proc_observer = 
                               m_system->get_process_observer (application_name);
                               
        app_proc_observer->notify_monitoring_failure (application_name, process_name,
                                                      host_name, time_stamp);
    }
    catch (CdmwPlatformMngt::ApplicationNotFound)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }
    catch (...)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}


void FTProcessObserver_impl::notify_user_event(
    const char* application_name,
    const char* process_name,
    const char* host_name,
    const char* issuer,
    const CORBA::Any& data)
throw (CdmwPlatformMngt::ProcessNotFound,
       CORBA::SystemException)
{
    try
    {
        // search the application process observer and relay the command
        CdmwPlatformMngt::ProcessObserver_var app_proc_observer = 
                               m_system->get_process_observer (application_name);
                               
        app_proc_observer->notify_user_event (application_name, process_name,
                                              host_name, issuer, data);
    }
    catch (CdmwPlatformMngt::ApplicationNotFound)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }
}

CdmwPlatformMngt::ManagedElement_ptr FTProcessObserver_impl::get_managed_element (
     const char * element_path)
throw (CdmwPlatformMngt::ElementPathInvalid,
       CdmwPlatformMngt::ManagedElementNotFound,
       CORBA::SystemException)
{
    // search the element in repository
	CdmwPlatformMngt::ManagedElement_var element = 
	             m_element_repository->get_element(element_path);
	             
    return element._retn();
}


char* FTProcessObserver_impl::get_system_entity_status (
        const char* entity_name,
        CORBA::String_out entity_info)
throw (CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    // get the system entity
    CdmwPlatformMngt::Entity_var entity = m_system->get_entity (entity_name);
    
    // get the status of entity and return
    return entity->get_status(entity_info);
}

char* FTProcessObserver_impl::get_host_entity_status (
        const char* host_name,
        const char* entity_name,
        CORBA::String_out entity_info)
throw (CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    // get the host object
    CdmwPlatformMngt::Host_var host_obj = 
                               m_system->get_host (host_name);
                               
    // get the host entity
    CdmwPlatformMngt::Entity_var entity = host_obj->get_entity (entity_name);
    
    // get the status of entity and return
    return entity->get_status(entity_info);
}

               
char* FTProcessObserver_impl::get_application_entity_status (
        const char* application_name,
        const char* entity_name,
        CORBA::String_out entity_info)
throw (CdmwPlatformMngt::ApplicationNotFound,
       CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    // search the application process observer and relay the command
    CdmwPlatformMngt::ProcessObserver_var app_proc_observer = 
                               m_system->get_process_observer (application_name);
                               
    return app_proc_observer->get_application_entity_status (
                                                application_name, entity_name,     
                                                entity_info);
}

               

char* FTProcessObserver_impl::get_process_entity_status (
        const char* application_name,
        const char* process_name,
        const char* host_name,
        const char* entity_name,
        CORBA::String_out entity_info)
throw (CdmwPlatformMngt::ProcessNotFound,
       CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    try
    {
        // search the application process observer and relay the command
        CdmwPlatformMngt::ProcessObserver_var app_proc_observer = 
                               m_system->get_process_observer (application_name);
                               
        return app_proc_observer->get_process_entity_status (
                                              application_name, process_name,
                                              host_name, entity_name, 
                                              entity_info);
    }
    catch (CdmwPlatformMngt::ApplicationNotFound)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }
}


void FTProcessObserver_impl::set_system_entity_status (
        const char* entity_name,
        const char* entity_status,
        const char* entity_info)
throw (CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    // get the system entity
    CdmwPlatformMngt::Entity_var entity = m_system->get_entity (entity_name);
    
    // get the current status of entity
    CORBA::String_var current_entity_info;
    CORBA::String_var current_entity_status;
        
    current_entity_status = entity->get_status(current_entity_info.out());
    
    // if entity status change
    if (strcmp (entity_status, current_entity_status.in()))
    {    
        // set the status of entity
        entity->set_status(entity_status, entity_info);
        
        // notify the entity status change
        m_system->notifyEntityStatusChangeEvent (
                               entity_name,
                               entity_status,
                               entity_info);
    }
}



void FTProcessObserver_impl::set_host_entity_status (
        const char* host_name,
        const char* entity_name,
        const char* entity_status,
        const char* entity_info)
throw (CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    // get the host
    Host_impl* host = m_system->getHost (host_name);
                               
    // get the host entity
    CdmwPlatformMngt::Entity_var entity = host->get_entity (entity_name);
    
    // get the current status of entity
    CORBA::String_var current_entity_info;
    CORBA::String_var current_entity_status;
        
    current_entity_status = entity->get_status(current_entity_info.out());
    
    // if entity status change
    if (strcmp (entity_status, current_entity_status.in()))
    {    
        // set the status of entity
        entity->set_status(entity_status, entity_info);
        
        // notify the entity status change
        host->notifyEntityStatusChangeEvent (
                               entity_name,
                               entity_status,
                               entity_info);
    }
}
               

void FTProcessObserver_impl::set_application_entity_status (
        const char* application_name,
        const char* entity_name,
        const char* entity_status,
        const char* entity_info)
throw (CdmwPlatformMngt::ApplicationNotFound,
       CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    // search the application process observer and relay the command
    CdmwPlatformMngt::ProcessObserver_var app_proc_observer = 
                               m_system->get_process_observer (application_name);
                               
    app_proc_observer->set_application_entity_status (
                                                application_name, entity_name,     
                                                entity_status, entity_info);
}

       

void FTProcessObserver_impl::set_process_entity_status (
        const char * application_name,
        const char * process_name,
        const char * host_name,
        const char * entity_name,
        const char * entity_status,
        const char * entity_info)
throw (CdmwPlatformMngt::ProcessNotFound,
       CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    try
    {
        // search the application process observer and relay the command
        CdmwPlatformMngt::ProcessObserver_var app_proc_observer = 
                               m_system->get_process_observer (application_name);
                               
        app_proc_observer->set_process_entity_status (
                                              application_name, process_name,
                                              host_name, entity_name, 
                                              entity_status, entity_info);
    }
    catch (CdmwPlatformMngt::ApplicationNotFound)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }
}


void FTProcessObserver_impl::set_create_process_result (
        const char * application_name,
        const char * process_name,
        const char * host_name,
        CdmwPlatformMngt::ProcessCommandResult result,
        const char* error_info,
        const char* error_issuer,
        CdmwPlatformMngt::ProcessDelegateWrapper_ptr process_delegate_wrap)
            throw (CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException)
{
    /***
    try
    {
	    // search the application process observer and relay the command
        CdmwPlatformMngt::ProcessObserver_var app_proc_observer = 
                               m_system->get_process_observer (application_name);
                               
        app_proc_observer->set_create_process_result (application_name, process_name,
                                                      host_name, result,
                                                      error_info, error_issuer,
                                                      process_delegate_wrap);
    }
    catch (CdmwPlatformMngt::ApplicationNotFound)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }
    ***/
    
    // build the process path
    std::string process_path = ManagedElementKey::Get_process_path ("",
                                    application_name, host_name, process_name);
    try
    {
	    // search the process in repository and relay the command
	    CdmwPlatformMngt::ManagedElement_var element = 
	             m_element_repository->get_element(process_path.c_str());
	             
        CdmwPlatformMngt::Process_var process = 
                 CdmwPlatformMngt::Process::_narrow(element.in());

        process->set_create_process_result(result, error_info, error_issuer,
                                           process_delegate_wrap);
    }
    catch (CdmwPlatformMngt::ElementPathInvalid)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }
    catch (CdmwPlatformMngt::ManagedElementNotFound)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }      
}


     

void FTProcessObserver_impl::set_initialisation_result (
        const char * application_name,
        const char * process_name,
        const char * host_name,
        CdmwPlatformMngt::ProcessCommandResult result)
            throw (CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException)
{
    /****
    try
    {
	    // search the application process observer and relay the command
        CdmwPlatformMngt::ProcessObserver_var app_proc_observer = 
                               m_system->get_process_observer (application_name);
                               
        app_proc_observer->set_initialisation_result(application_name, process_name,
                                                     host_name, result);
    }
    catch (CdmwPlatformMngt::ApplicationNotFound)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }
    ***/
    
    // build the process path
    std::string process_path = ManagedElementKey::Get_process_path ("",
                            application_name, host_name, process_name);
    try
    {
	    // search the process in repository and relay the command
	    CdmwPlatformMngt::ManagedElement_var element = 
	             m_element_repository->get_element(process_path.c_str());
	             
        CdmwPlatformMngt::Process_var process = 
                 CdmwPlatformMngt::Process::_narrow(element.in());

        process->set_initialisation_result(result);
    }
    catch (CdmwPlatformMngt::ElementPathInvalid)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }
    catch (CdmwPlatformMngt::ManagedElementNotFound)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }        
}
     

void FTProcessObserver_impl::set_next_step_result (
        const char * application_name,
        const char * process_name,
        const char * host_name,
        CdmwPlatformMngt::ProcessCommandResult result,
        CORBA::ULong step_nbr)
            throw (CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException)
{
    /***
	try
    {
	    // search the application process observer and relay the command
        CdmwPlatformMngt::ProcessObserver_var app_proc_observer = 
                               m_system->get_process_observer (application_name);
                               
        app_proc_observer->set_next_step_result (application_name, process_name,
                                                 host_name, result, step_nbr);
    }
    catch (CdmwPlatformMngt::ApplicationNotFound)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }
    ***/
    
    // build the process path
    std::string process_path = ManagedElementKey::Get_process_path ("",
                            application_name, host_name, process_name);
    try
    {
	    // search the process in repository and relay the command
	    CdmwPlatformMngt::ManagedElement_var element = 
	             m_element_repository->get_element(process_path.c_str());
	             
        CdmwPlatformMngt::Process_var process = 
                 CdmwPlatformMngt::Process::_narrow(element.in());

        process->set_next_step_result(result, step_nbr);
    }
    catch (CdmwPlatformMngt::ElementPathInvalid)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }
    catch (CdmwPlatformMngt::ManagedElementNotFound)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    } 
}
     

void FTProcessObserver_impl::set_run_result (
        const char * application_name,
        const char * process_name,
        const char * host_name,
        CdmwPlatformMngt::ProcessCommandResult result)
            throw (CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException)
{
    /***
	try
    {
	    // search the application process observer and relay the command
        CdmwPlatformMngt::ProcessObserver_var app_proc_observer = 
                               m_system->get_process_observer (application_name);
                               
        app_proc_observer->set_run_result (application_name, process_name,
                                           host_name, result);
    }
    catch (CdmwPlatformMngt::ApplicationNotFound)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }
    ***/
    
    // build the process path
    std::string process_path = ManagedElementKey::Get_process_path ("",
                            application_name, host_name, process_name);
    try
    {
	    // search the process in repository and relay the command
	    CdmwPlatformMngt::ManagedElement_var element = 
	             m_element_repository->get_element(process_path.c_str());
	             
        CdmwPlatformMngt::Process_var process = 
                 CdmwPlatformMngt::Process::_narrow(element.in());

        process->set_run_result(result);
    }
    catch (CdmwPlatformMngt::ElementPathInvalid)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }
    catch (CdmwPlatformMngt::ManagedElementNotFound)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    } 
}
     

void FTProcessObserver_impl::set_stop_result (
        const char * application_name,
        const char * process_name,
        const char * host_name,
        CdmwPlatformMngt::ProcessCommandResult result)
            throw (CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException)
{
    /***
	try
    {
	    // search the application process observer and relay the command
        CdmwPlatformMngt::ProcessObserver_var app_proc_observer = 
                               m_system->get_process_observer (application_name);
                               
        app_proc_observer->set_stop_result (application_name, process_name,
                                            host_name, result);
    }
    catch (CdmwPlatformMngt::ApplicationNotFound)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }
    ***/
    
    // build the process path
    std::string process_path = ManagedElementKey::Get_process_path ("",
                            application_name, host_name, process_name);
    try
    {
	    // search the process in repository and relay the command
	    CdmwPlatformMngt::ManagedElement_var element = 
	             m_element_repository->get_element(process_path.c_str());
	             
        CdmwPlatformMngt::Process_var process = 
                 CdmwPlatformMngt::Process::_narrow(element.in());

        process->set_stop_result(result);
    }
    catch (CdmwPlatformMngt::ElementPathInvalid)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }
    catch (CdmwPlatformMngt::ManagedElementNotFound)
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    } 
}




} // End namespace PlatformMngt
} // End namespace Cdmw

