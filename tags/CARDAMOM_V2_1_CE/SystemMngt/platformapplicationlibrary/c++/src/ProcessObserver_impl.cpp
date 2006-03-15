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

#include "SystemMngt/platformelementlibrary/Entity_impl.hpp"
#include "platformapplicationlibrary/ProcessObserver_impl.hpp"
#include "platformapplicationlibrary/Process_impl.hpp"
#include "platformapplicationlibrary/ManagedProcess_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "SystemMngt/platformvaluetypes/ProcessStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/UserEvent_impl.hpp"
#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

// ----------------------------------------------------------------------
// ProcessObserver_impl class.
// ----------------------------------------------------------------------
ProcessObserver_impl::ProcessObserver_impl(
    PortableServer::POA_ptr poa,
    Application_impl* application)
: DeactivableServant_impl (poa)
{
    m_application = application;
}

void ProcessObserver_impl::notify_message(
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
        std::auto_ptr <ProcessMessageEvent> event(
            new ProcessMessageEvent(
                m_application->getProcess (process_name, host_name),
                header,
                issuer,
                message));

        m_application->get_event_handler()->addEvent (event.get());
        event.release();
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch (...)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}

void ProcessObserver_impl::notify_error(
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
        std::auto_ptr <ProcessErrorEvent> event(
            new ProcessErrorEvent(
                m_application->getProcess (process_name, host_name),
                time_stamp,
                issuer,
                error_info));

        m_application->get_event_handler()->addEvent (event.get());
        event.release();
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch(...)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}


void ProcessObserver_impl::notify_starting(
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



void ProcessObserver_impl::notify_ending(
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
        std::auto_ptr <ProcessEndingEvent> event(
            new ProcessEndingEvent(
                m_application->getProcess(process_name, host_name),
                time_stamp));

        m_application->get_event_handler()->addEvent(event.get());
        event.release();
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch (...)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}

void ProcessObserver_impl::notify_monitoring_failure(
    const char* application_name,
    const char* process_name,
    const char* host_name,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp)
throw (CdmwPlatformMngt::ProcessNotFound,
       CORBA::SystemException)
{
    try
    {
        ManagedProcess_impl* managedProcess
            = dynamic_cast <ManagedProcess_impl*>(
                m_application->getProcess (process_name, host_name));
        if (managedProcess == NULL)
            throw CORBA::BAD_OPERATION (
                      OrbSupport::BAD_OPERATION, CORBA::COMPLETED_NO);

        std::auto_ptr <ProcessMonitoringFailure> event(
            new ProcessMonitoringFailure(
                managedProcess,
                time_stamp)) ;

        m_application->get_event_handler()->addEvent (event.get());
        event.release();
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch (...)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}


void ProcessObserver_impl::notify_user_event(
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
        std::auto_ptr <ProcessUserEvent> event(
            new ProcessUserEvent(
                m_application->getProcess(process_name, host_name),
                issuer,
                data));

        m_application->get_event_handler()->addEvent (event.get());
        event.release();
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
}


CdmwPlatformMngt::ManagedElement_ptr ProcessObserver_impl::get_managed_element (
     const char * element_path)
throw (CdmwPlatformMngt::ElementPathInvalid,
       CdmwPlatformMngt::ManagedElementNotFound,
       CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

           
char* ProcessObserver_impl::get_system_entity_status (
        const char* entity_name,
        CORBA::String_out entity_info)
throw (CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


char* ProcessObserver_impl::get_host_entity_status (
        const char* host_name,
        const char* entity_name,
        CORBA::String_out entity_info)
throw (CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
        throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

               
char* ProcessObserver_impl::get_application_entity_status (
        const char* application_name,
        const char* entity_name,
        CORBA::String_out entity_info)
throw (CdmwPlatformMngt::ApplicationNotFound,
       CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    // get the application entity
    CdmwPlatformMngt::Entity_var entity = m_application->get_entity (entity_name);
    
    // get the status of entity and return
    return entity->get_status(entity_info);
}

               

char* ProcessObserver_impl::get_process_entity_status (
        const char* application_name,
        const char* process_name,
        const char* host_name,
        const char* entity_name,
        CORBA::String_out entity_info)
throw (CdmwPlatformMngt::ProcessNotFound,
       CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    // get the process object
    Process_impl* process = 
             m_application->getProcess (process_name, host_name);
             
    // get the process entity
    CdmwPlatformMngt::Entity_var entity = process->get_entity (entity_name);
    
    // get the status of entity and return
    return entity->get_status(entity_info);
}


void ProcessObserver_impl::set_system_entity_status (
        const char* entity_name,
        const char* entity_status,
        const char* entity_info)
throw (CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}
               
void ProcessObserver_impl::set_host_entity_status (
        const char* host_name,
        const char* entity_name,
        const char* entity_status,
        const char* entity_info)
throw (CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
        throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}
 
  
void ProcessObserver_impl::set_application_entity_status (
        const char* application_name,
        const char* entity_name,
        const char* entity_status,
        const char* entity_info)
throw (CdmwPlatformMngt::ApplicationNotFound,
       CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{        
    // get the application entity
    CdmwPlatformMngt::Entity_var entity = m_application->get_entity (entity_name);
    
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
        m_application->notifyEntityStatusChangeEvent (
                               entity_name,
                               entity_status,
                               entity_info);
    }
}

       

void ProcessObserver_impl::set_process_entity_status (
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
    // get the process object
    Process_impl* process = 
             m_application->getProcess (process_name, host_name);
             
    // get the process entity
    CdmwPlatformMngt::Entity_var entity = process->get_entity (entity_name);
    
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
        process->notifyEntityStatusChangeEvent (
                               entity_name,
                               entity_status,
                               entity_info);
    }
}



void ProcessObserver_impl::set_create_process_result (
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
    Process_impl* process = m_application->getProcess (process_name, host_name);
    
    process->set_create_process_result (result, error_info, error_issuer,
                                        process_delegate_wrap);
}


     

void ProcessObserver_impl::set_initialisation_result (
        const char * application_name,
        const char * process_name,
        const char * host_name,
        CdmwPlatformMngt::ProcessCommandResult result)
            throw (CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException)
{
	Process_impl* process = m_application->getProcess (process_name, host_name);
    
    process->set_initialisation_result (result);
}
     

void ProcessObserver_impl::set_next_step_result (
        const char * application_name,
        const char * process_name,
        const char * host_name,
        CdmwPlatformMngt::ProcessCommandResult result,
        CORBA::ULong step_nbr)
            throw (CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException)
{
	Process_impl* process = m_application->getProcess (process_name, host_name);
    
    process->set_next_step_result (result, step_nbr);
}
     

void ProcessObserver_impl::set_run_result (
        const char * application_name,
        const char * process_name,
        const char * host_name,
        CdmwPlatformMngt::ProcessCommandResult result)
            throw (CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException)
{
	Process_impl* process = m_application->getProcess (process_name, host_name);
    
    process->set_run_result (result);
}
     

void ProcessObserver_impl::set_stop_result (
        const char * application_name,
        const char * process_name,
        const char * host_name,
        CdmwPlatformMngt::ProcessCommandResult result)
            throw (CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException)
{
	Process_impl* process = m_application->getProcess (process_name, host_name);
    
    process->set_stop_result (result);
}


void ProcessObserver_impl::synchro_event (const CdmwPlatformMngt::EventReportRecord& event_record)
{
    try
    {
         // first get the managed process from event report
         Process_impl* p_managed_process = 
              m_application->getProcess (event_record.process_name.in(), 
                                         event_record.host_name.in());
                                    
         std::cout << "<<ProcessObserver_impl::synchro_event() : " 
                   << event_record.process_name.in()
                   << " - "
                   << event_record.host_name.in() << ">>" << std::endl;
                                    
 
        if (event_record.event_type == CdmwPlatformMngt::PROCESS_MESSAGE_EVENT)
        {
            std::cout << "<<ProcessObserver_impl::synchro_event() PROCESS_MESSAGE_EVENT>>" << std::endl;
            
            std::auto_ptr <ProcessMessageEvent> event(
            new ProcessMessageEvent(
                p_managed_process, event_record));

            // execute the event now
            event->execute();
        }
        
        else if (event_record.event_type == CdmwPlatformMngt::PROCESS_ERROR_EVENT)
        {
            std::cout << "<<ProcessObserver_impl::synchro_event() PROCESS_ERROR_EVENT>>" << std::endl;
                      
            std::auto_ptr <ProcessErrorEvent> event(
            new ProcessErrorEvent(
                p_managed_process, event_record));

            // execute the event now
            event->execute();
        }
        
        else if (event_record.event_type == CdmwPlatformMngt::PROCESS_ENDING_EVENT)
        {
            std::cout << "<<ProcessObserver_impl::synchro_event() PROCESS_ENDING_EVENT>>" << std::endl;
                      
            std::auto_ptr <ProcessEndingEvent> event(
            new ProcessEndingEvent(
                p_managed_process, event_record));

            // execute the event now
            event->execute();
        }
        
        else if (event_record.event_type == CdmwPlatformMngt::PROCESS_MONITORING_FAILURE)
        {
            std::cout << "<<ProcessObserver_impl::synchro_event() PROCESS_MONITORING_FAILURE>>" << std::endl;
                      
            std::auto_ptr <ProcessMonitoringFailure> event(
            new ProcessMonitoringFailure(
                p_managed_process, event_record));

            // execute the event now
            event->execute();
        }
        
        else if (event_record.event_type == CdmwPlatformMngt::PROCESS_USER_EVENT)
        {
            std::cout << "<<ProcessObserver_impl::synchro_event() PROCESS_USER_EVENT>>" << std::endl;
              
            std::auto_ptr <ProcessUserEvent> event(
            new ProcessUserEvent(
                p_managed_process, event_record));

            // execute the event now
            event->execute();
        }
    }
    catch(const CdmwPlatformMngt::ProcessNotFound&)
    {
        std::cout << "<<ProcessObserver_impl::synchro_event() Process Not Found>>" << std::endl;
    }
    catch(const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch(...)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}



// ----------------------------------------------------------------------
// ProcessMessageEvent class.
// ----------------------------------------------------------------------
ProcessMessageEvent::ProcessMessageEvent(
    Process_impl* process,
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* issuer,
    const char* message)
:Event()
{
    m_application = process->get_application();
    m_process = process;
    
    m_record.event_type = CdmwPlatformMngt::PROCESS_MESSAGE_EVENT;
    m_record.header = header;    
    m_record.issuer = CORBA::string_dup (issuer);
    m_record.message = CORBA::string_dup (message);
    m_record.application_name = process->get_application()->name();
    m_record.process_name = process->name();
    m_record.host_name = CORBA::string_dup (process->get_process_host_name());
}

ProcessMessageEvent::ProcessMessageEvent(    
        Process_impl* process,
        const CdmwPlatformMngt::EventReportRecord& event_record)
:Event()
{
    m_application = process->get_application();
    m_process = process;
    
    m_record.event_type = CdmwPlatformMngt::PROCESS_MESSAGE_EVENT;
    m_record.header = event_record.header;
    m_record.issuer = event_record.issuer;
    m_record.message = event_record.message;
    m_record.application_name = event_record.application_name;
    m_record.process_name = event_record.process_name;
    m_record.host_name = event_record.host_name;
}

void ProcessMessageEvent::execute()
{
    try
    {
        #if CDMW_ORB_VDR == tao
        ProcessMessageFactory eventFactory;
        #else
        CORBA::ValueFactoryBase_var eventFactory
            = new ProcessMessageFactory();
        #endif

        CdmwPlatformMngt::ProcessID_var processId
            = new CdmwPlatformMngt::ProcessID;
            
        processId->application_name =
                    CORBA::string_dup(m_application->get_element_name());
        processId->process_name = CORBA::string_dup(m_process->get_element_name());
        processId->host_name = CORBA::string_dup(m_process->get_process_host_name());
        
            
        #if CDMW_ORB_VDR == tao
        CdmwPlatformMngt::ProcessMessage_var event
            = eventFactory.create(
                m_record.header,
                CORBA::string_dup(m_application->get_system_name()),
                processId.in(),
                m_record.issuer.in(),
                m_record.message.in() );
        #else
        CdmwPlatformMngt::ProcessMessage_var event
            = dynamic_cast< ProcessMessageFactory* >(
                eventFactory.in() )->create(
                    m_record.header,
                    CORBA::string_dup(m_application->get_system_name()),
                    processId.in(),
                    m_record.issuer.in(),
                    m_record.message.in() );
        #endif

        m_application->get_event_notifier()->addEvent (event);
        event._retn();
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch (...)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}

// ----------------------------------------------------------------------
// ProcessErrorEvent class.
// ----------------------------------------------------------------------
ProcessErrorEvent::ProcessErrorEvent(
    Process_impl* process,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp,
    const char* issuer,
    const char* error_info)
:Event()
{
    m_process = process;
    
    m_record.event_type = CdmwPlatformMngt::PROCESS_ERROR_EVENT;
    m_record.header = EventHeaderFactory::createHeader(time_stamp, 
                                                       CdmwPlatformMngtBase::FTL);
    m_record.issuer = CORBA::string_dup (issuer);
    m_record.message = CORBA::string_dup (error_info); 
    m_record.application_name = process->get_application()->name();
    m_record.process_name = process->name();
    m_record.host_name = CORBA::string_dup (process->get_process_host_name());   
}

ProcessErrorEvent::ProcessErrorEvent(    
        Process_impl* process,
        const CdmwPlatformMngt::EventReportRecord& event_record)
:Event()
{
    m_process = process;
    
    m_record.event_type = CdmwPlatformMngt::PROCESS_ERROR_EVENT;
    m_record.header = event_record.header;
    
    m_record.issuer = event_record.issuer;
    m_record.message = event_record.message;  
    m_record.application_name = event_record.application_name;
    m_record.process_name = event_record.process_name;
    m_record.host_name = event_record.host_name;  
}


void ProcessErrorEvent::execute()
{
    m_process->error_event(
        m_record.header,
        m_record.issuer.in(),
        m_record.message.in());
}

// ----------------------------------------------------------------------
// ProcessEndingEvent class.
// ----------------------------------------------------------------------
ProcessEndingEvent::ProcessEndingEvent(
    Process_impl* process,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp )
:Event()
{
	m_process = process;
	
	m_record.event_type = CdmwPlatformMngt::PROCESS_ENDING_EVENT;
	
    m_record.header = EventHeaderFactory::createHeader(time_stamp, 
                                                       CdmwPlatformMngtBase::ERR); 
    m_record.application_name = process->get_application()->name();
    m_record.process_name = process->name();
    m_record.host_name = CORBA::string_dup (process->get_process_host_name());   
}

ProcessEndingEvent::ProcessEndingEvent(    
        Process_impl* process,
        const CdmwPlatformMngt::EventReportRecord& event_record)
:Event()
{
    m_process = process;
    
    m_record.event_type = CdmwPlatformMngt::PROCESS_ENDING_EVENT;
	
    m_record.header = event_record.header; 
    m_record.application_name = event_record.application_name;
    m_record.process_name = event_record.process_name;
    m_record.host_name = event_record.host_name;
}


void ProcessEndingEvent::execute()
{
    m_process->ending_event(m_record.header);
}

// ----------------------------------------------------------------------
// ProcessMonitoringFailure class.
// ----------------------------------------------------------------------
ProcessMonitoringFailure::ProcessMonitoringFailure(
    Process_impl* managedProcess,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp)
:Event()
{
    m_managedProcess = managedProcess;
    
    m_record.event_type = CdmwPlatformMngt::PROCESS_MONITORING_FAILURE;
    
    m_record.header = EventHeaderFactory::createHeader(time_stamp, 
                                                       CdmwPlatformMngtBase::ERR);
    m_record.application_name = managedProcess->get_application()->name();
    m_record.process_name = managedProcess->name();
    m_record.host_name = CORBA::string_dup (managedProcess->get_process_host_name());
}

ProcessMonitoringFailure::ProcessMonitoringFailure(    
     Process_impl* managedProcess,
     const CdmwPlatformMngt::EventReportRecord& event_record)
:Event()
{
    m_managedProcess = managedProcess;
    
    m_record.event_type = CdmwPlatformMngt::PROCESS_MONITORING_FAILURE;
    
    m_record.header = event_record.header;
    m_record.application_name = event_record.application_name;
    m_record.process_name = event_record.process_name;
    m_record.host_name = event_record.host_name;
}


void ProcessMonitoringFailure::execute()
{
    m_managedProcess->monitoringFailure_event(m_record.header);
}

// ----------------------------------------------------------------------
// ProcessUserEvent class.
// ----------------------------------------------------------------------
ProcessUserEvent::ProcessUserEvent(
    Process_impl* process,
    const char* issuer,
    const CORBA::Any& data)
:Event()
{
    m_process = process;
    
    m_record.event_type = CdmwPlatformMngt::PROCESS_USER_EVENT;
    m_record.header = EventHeaderFactory::createHeader(CdmwPlatformMngtBase::INF);
    
    m_record.system_name =
            CORBA::string_dup(m_process->get_application()->get_system_name());
    m_record.application_name = 
            CORBA::string_dup(m_process->get_application()->get_element_name());
    m_record.process_name = CORBA::string_dup(m_process->get_element_name());
    m_record.host_name = CORBA::string_dup(m_process->get_process_host_name());
    
    m_record.issuer = CORBA::string_dup (issuer);
    m_record.data = data;

#if CDMW_ORB_VDR == tao && CDMW_ORB_VER < 14
    CORBA_ValueFactoryBase_var factory;
#else
    CORBA::ValueFactoryBase_var factory;
#endif

    factory = new UserEventFactory();

    m_event = dynamic_cast <UserEventFactory*> (factory.in())->create(
        EventHeaderFactory::createHeader (CdmwPlatformMngtBase::INF),
        m_record.system_name.in(),
        m_record.application_name.in(),
        m_record.process_name.in(),
        m_record.host_name.in(),
        m_record.issuer.in(),
        m_record.data);   
}

ProcessUserEvent::ProcessUserEvent(    
        Process_impl* process,
        const CdmwPlatformMngt::EventReportRecord& event_record)
:Event()
{
    m_process = process;
    
    m_record.event_type = CdmwPlatformMngt::PROCESS_USER_EVENT;
    m_record.header = event_record.header;
    
    m_record.issuer = event_record.issuer;
    m_record.data = event_record.data;

#if CDMW_ORB_VDR == tao && CDMW_ORB_VER < 14
    CORBA_ValueFactoryBase_var factory;
#else
    CORBA::ValueFactoryBase_var factory;
#endif

    factory = new UserEventFactory();

    m_event = dynamic_cast <UserEventFactory*> (factory.in())->create(
        EventHeaderFactory::createHeader (CdmwPlatformMngtBase::INF),
        event_record.system_name.in(),
        event_record.application_name.in(),
        event_record.process_name.in(),
        event_record.host_name.in(),
        m_record.issuer.in(),
        m_record.data); 
}


void ProcessUserEvent::execute()
{
    try
    {
        m_process->get_application()->get_event_notifier()->addEvent (m_event);
        m_event._retn();
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch (...)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}

} // End namespace PlatformMngt
} // End namespace Cdmw

