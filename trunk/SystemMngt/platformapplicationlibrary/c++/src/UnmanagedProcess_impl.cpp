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


#include "platformapplicationlibrary/UnmanagedProcess_impl.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"
#include "SystemMngt/platformvaluetypes/ProcessStatusChange_impl.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/ApplicationLogMessageId.hpp"

namespace Cdmw
{
    namespace PlatformMngt
    {

	UnmanagedProcessInitial* UnmanagedProcess_impl::M_initial = NULL;
	UnmanagedProcessInitialising* UnmanagedProcess_impl::M_initialising = NULL;
	UnmanagedProcessRunning* UnmanagedProcess_impl::M_running = NULL;
	UnmanagedProcessStopped* UnmanagedProcess_impl::M_stopped = NULL;
	UnmanagedProcessEnded* UnmanagedProcess_impl::M_ended = NULL;
	UnmanagedProcessFailedDeath* UnmanagedProcess_impl::M_failedDeath = NULL;
	UnmanagedProcessFailedInvalid* UnmanagedProcess_impl::M_failedInvalid = NULL;

// ----------------------------------------------------------------------
// UnmanagedProcess_impl class.
// ----------------------------------------------------------------------
	UnmanagedProcess_impl::UnmanagedProcess_impl(
	    PortableServer::POA_ptr poa,
	    CdmwPlatformMngt::ProcessDef* process_def,
	    const char* process_key, const char* process_path,
	    Application_impl* application,
	    CdmwPlatformMngt::RWElementRepository_ptr element_repository)
	    throw(CORBA::SystemException)
	    : Process_impl(poa, process_def, process_key, process_path,
			   application, element_repository)
	{
	    try
	    {
		std::auto_ptr< UnmanagedProcessStateMachine > stateMachine(
		    new UnmanagedProcessStateMachine(
			this, "UnmanagedProcess_impl" ) );

		m_stateMachine = stateMachine.get();

		// Add the states to the state machine
		m_stateMachine->addState ("Initial", M_initial);
		m_stateMachine->addState ("Initialising", M_initialising);
		m_stateMachine->addState ("Running", M_running);
		m_stateMachine->addState ("Stopped", M_stopped);
		m_stateMachine->addState ("Ended", M_ended);
		m_stateMachine->addState ("FailedDeath", M_failedDeath);
		m_stateMachine->addState ("FailedInvalid", M_failedInvalid);

		// Set the initial state
		m_stateMachine->setState ("Initial");

		// Release the ownership of the temporary objects
		stateMachine.release();
	    }
	    catch (const std::bad_alloc&)
	    {
		throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
	    }
	}

	UnmanagedProcess_impl::~UnmanagedProcess_impl()
	{
	    if (m_stateMachine != NULL)
		delete m_stateMachine;
	}

	void UnmanagedProcess_impl::end_creation(
	    CdmwPlatformMngt::ProcessDef* process_def)
	    throw (CdmwPlatformMngt::HostNotFound,
		   CdmwPlatformMngt::DuplicateEntity,
		   CdmwPlatformMngt::DuplicateService,
		   CdmwPlatformMngt::DuplicateStep,
		   OutOfMemoryException,
		   CosPropertyService::MultipleExceptions,
		   CORBA::SystemException)
	{
	    // the following initialisation call the methods of the object
	    // setting the attribute changes in datastore
	    // in case of exception the object will be deleted by the application
    
	    // set the host for this process
	    // if not found throw HostNotFound exception
	    setHost(process_def->host_name());
	
	    // set the process info
	    setProcessInfo (process_def->process_info());
	
	                                                                                                                             
	    //Add all Properties in Un-ManagedProcess
                                                                                                                             
	    unsigned int propCount = process_def->properties().length();
        
	    for (unsigned int propIndex=0 ; propIndex < propCount ; propIndex++)
	    {
                // get service definition (ServiceDef is a structure, a reference on the
		// structure is returned by the [] operator)
		const CdmwPlatformMngt::PropertyDef& prop_def =
		    process_def->properties()[propIndex];
                                                                                                                             
		try
		{
		    std::string propertyName(process_def->properties()[propIndex].property_name);
		    CosPropertyService::PropertyModeType propertyMode = CosPropertyService::PropertyModeType(0);                
		    CdmwPlatformMngt::PropertyModeType propertyModeCdmw = process_def->properties()[propIndex].mode_type;        
		    if (propertyModeCdmw == 1) propertyMode = CosPropertyService::PropertyModeType(1);
		    define_property_with_mode(propertyName.c_str() , prop_def.initial_value, propertyMode);
		}
		catch (const CosPropertyService::InvalidPropertyName&)
                {
		    throw  CosPropertyService::MultipleExceptions(); /* TO-DO */
                }
		catch (const CosPropertyService::ConflictingProperty&)
                {
		    throw  CosPropertyService::MultipleExceptions(); /* TO-DO */
		}
		catch (const CosPropertyService::UnsupportedTypeCode&)
                {
		    throw  CosPropertyService::MultipleExceptions(); /* TO-DO */
		}
		catch (const CosPropertyService::UnsupportedProperty&)
                {
		    throw  CosPropertyService::MultipleExceptions(); /* TO-DO */
		}
		catch (const CosPropertyService::UnsupportedMode&)
                {
		    throw  CosPropertyService::MultipleExceptions(); /* TO-DO */

		}
		catch (const CosPropertyService::ReadOnlyProperty&)
                {
		    throw  CosPropertyService::MultipleExceptions(); /* TO-DO */
		}

	    }	
	
	    // store default properties values
	    store_default_properties();     
	  
	    // initialise data store of the object
	    set_record_in_datastore();    
	}


	void UnmanagedProcess_impl::synchro_processing_to_initial()
	{
	    m_stateMachine->setStateName("Initial");
	}
    

	void UnmanagedProcess_impl::synchro_processing_to_initialised()
	{
	    // "initialised" state does not exist for unmanaged process
	    // after "initialising" it is set to "running" immediateley
	    m_stateMachine->setStateName("Running");
	}
    

	void UnmanagedProcess_impl::synchro_processing_to_running()
	{
	    m_stateMachine->setStateName("Running");
	}


	const char* UnmanagedProcess_impl::get_state_machine_state_name()
	{
	    return m_stateMachine->getStateName();
	}

	void UnmanagedProcess_impl::set_state_machine_state_name(const char* state_name)
	{
	    m_stateMachine->setStateName(state_name);
	}

	CdmwPlatformMngt::ProcessStatus UnmanagedProcess_impl::get_internal_status()
	{
	    CDMW_SM_READER_LOCK_GUARD(*m_stateMachine);
    
	    // get status
	    CdmwPlatformMngt::ProcessStatus processStatus = 
		m_stateMachine->getState()->status();
                            
	    return processStatus;
	}


	CdmwPlatformMngt::ProcessStatus UnmanagedProcess_impl::get_status(
	    CORBA::String_out status_info)
	    throw (CORBA::SystemException)
	{
	    try
	    {
		CDMW_SM_READER_LOCK_GUARD(*m_stateMachine);
        
		// get record                
		CdmwPlatformMngt::ProcessRecord_var process_record = get_record_from_datastore();
    
		// get status
		CdmwPlatformMngt::ProcessStatus process_status = process_record->status;
    
		// get info associated to status
		status_info = CORBA::string_dup ((process_record->status_info).in());

		return process_status;
	    }
	    catch (const std::bad_alloc&)
	    {
		throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
	    }
	}

	void UnmanagedProcess_impl::set_auto_ending(CORBA::Boolean auto_ending)
	    throw( CdmwPlatformMngt::IncompatibleStatus,
		   CORBA::SystemException )
	{
	    CDMW_SM_READER_LOCK_GUARD( *m_stateMachine );

	    m_stateMachine->getState()->set_autoending (m_stateMachine, auto_ending);
	}

	void UnmanagedProcess_impl::set_host(
	    const char* host_name )
	    throw( CdmwPlatformMngt::HostNotFound,
		   CdmwPlatformMngt::IncompatibleStatus,
		   CORBA::SystemException )
	{
	    CDMW_SM_READER_LOCK_GUARD( *m_stateMachine );

	    m_stateMachine->getState()->set_host( m_stateMachine, host_name );
	}

	void UnmanagedProcess_impl::set_process_info(
	    const CdmwPlatformMngt::ProcessInfo& process_info )
	    throw( CdmwPlatformMngt::IncompatibleStatus,
		   CORBA::SystemException )
	{
	    CDMW_SM_READER_LOCK_GUARD( *m_stateMachine );

	    m_stateMachine->getState()->set_process_info(
		m_stateMachine,
		process_info );
	}

	CdmwPlatformMngt::RequestStatus UnmanagedProcess_impl::initialise(
	    const CdmwPlatformMngtBase::StartupKind& startup_kind)
	    throw (CdmwPlatformMngt::IncompatibleStatus,
		   CORBA::SystemException)
	{
	    LogMngr::logMessage(
		INF,
		MSG_ID_PROCESS_INITALISE,
		get_process_key(),
		m_application->get_element_name());

	    CDMW_SM_WRITER_LOCK_GUARD(*m_stateMachine);

	    CdmwPlatformMngt::RequestStatus req_status;
    
	    try
	    {

		// recover the default user defined attributes values 
		recover_default_properties();
		
		req_status =
		    m_stateMachine->getState()->initialise(
			m_stateMachine,
			startup_kind);
	    }
	    catch (...)
	    {
		// even in case of exception update record in datastore
		set_record_in_datastore();
            
		// rethrow exception
		throw;
	    }
        
	    // update record in datastore
	    set_record_in_datastore();
    
	    return req_status;
	}

	CdmwPlatformMngt::RequestStatus UnmanagedProcess_impl::stop(
	    CORBA::Boolean emergency )
	    throw (CORBA::SystemException)
	{
	    LogMngr::logMessage(
		INF,
		MSG_ID_PROCESS_STOP,
		get_process_key(),
		m_application->get_element_name());

	    CDMW_SM_WRITER_LOCK_GUARD(*m_stateMachine);

	    CdmwPlatformMngt::RequestStatus req_status;
    
	    try
	    {
		req_status =
		    m_stateMachine->getState()->stop(m_stateMachine);
	    }
	    catch (...)
	    {
		// even in case of exception update record in datastore
		set_record_in_datastore();
            
		// rethrow exception
		throw;
	    }
        
	    // update record in datastore
	    set_record_in_datastore();
    
	    return req_status;
	}

	void UnmanagedProcess_impl::error_event(
	    const CdmwPlatformMngtBase::EventHeader& header,
	    const char* issuer,
	    const char* error_info )
	{
	}

	void UnmanagedProcess_impl::ending_event(
	    const CdmwPlatformMngtBase::EventHeader& header )
	{
	    CDMW_SM_WRITER_LOCK_GUARD( *m_stateMachine );

	    m_stateMachine->getState()->ending_event(
		m_stateMachine,
		header );
	}

	void UnmanagedProcess_impl::invalidate_event(
	    const CdmwPlatformMngtBase::EventHeader& header,
	    const char* reason )
	{
	    CDMW_SM_WRITER_LOCK_GUARD( *m_stateMachine );

	    m_stateMachine->getState()->invalidate_event(
		m_stateMachine,
		header,
		reason );
	}

	void UnmanagedProcess_impl::createStates()
	    throw( OutOfMemoryException )
	{
	    try
	    {
		std::auto_ptr< UnmanagedProcessInitial > initial(
		    new UnmanagedProcessInitial() );
		std::auto_ptr< UnmanagedProcessInitialising > initialising(
		    new UnmanagedProcessInitialising() );
		std::auto_ptr< UnmanagedProcessRunning > running(
		    new UnmanagedProcessRunning() );
		std::auto_ptr< UnmanagedProcessStopped > stopped(
		    new UnmanagedProcessStopped() );
		std::auto_ptr< UnmanagedProcessEnded > ended(
		    new UnmanagedProcessEnded() );
		std::auto_ptr< UnmanagedProcessFailedDeath > failedDeath(
		    new UnmanagedProcessFailedDeath() );
		std::auto_ptr< UnmanagedProcessFailedInvalid > failedInvalid(
		    new UnmanagedProcessFailedInvalid() );

		if( M_initial == NULL )
		    M_initial = initial.release();
		if( M_initialising == NULL )
		    M_initialising = initialising.release();
		if( M_running == NULL )
		    M_running = running.release();
		if( M_stopped == NULL )
		    M_stopped = stopped.release();
		if( M_ended == NULL )
		    M_ended = ended.release();
		if( M_failedDeath == NULL )
		    M_failedDeath = failedDeath.release();
		if( M_failedInvalid == NULL )
		    M_failedInvalid = failedInvalid.release();
	    }
	    catch( const std::bad_alloc& )
	    {
		CDMW_THROW( OutOfMemoryException );
	    }
	}

	void UnmanagedProcess_impl::destroyStates()
	{
	    if( M_initial != NULL )
	    {
		delete M_initial;
		M_initial = NULL;
	    }

	    if( M_initialising != NULL )
	    {
		delete M_initialising;
		M_initialising = NULL;
	    }

	    if( M_running != NULL )
	    {
		delete M_running;
		M_running = NULL;
	    }

	    if( M_stopped != NULL )
	    {
		delete M_stopped;
		M_stopped = NULL;
	    }

	    if( M_ended != NULL )
	    {
		delete M_ended;
		M_ended = NULL;
	    }

	    if( M_failedDeath != NULL )
	    {
		delete M_failedDeath;
		M_failedDeath = NULL;
	    }

	    if( M_failedInvalid != NULL )
	    {
		delete M_failedInvalid;
		M_failedInvalid = NULL;
	    }
	}

	void UnmanagedProcess_impl::setState( const char* stateName )
	{
	    m_stateMachine->setState( stateName );
	}

	CdmwPlatformMngt::ProcessStatus UnmanagedProcess_impl::getStatus()
	    throw( CORBA::SystemException )    
	{
	    return m_stateMachine->getState()->status();
	}

	void UnmanagedProcess_impl::notifyStatusChangeEvent(
	    const CdmwPlatformMngtBase::EventHeader& header,
	    const char* issuer )
	{
	    try
	    {
		// Get the status
		CdmwPlatformMngt::ProcessStatus status
		    = m_stateMachine->getState()->status();
            
		// Create the event
		CdmwPlatformMngt::ProcessID_var processId
		    = new CdmwPlatformMngt::ProcessID;
		processId->application_name = m_application->name();
		processId->process_name = name();
		processId->host_name = m_host->name();

		CORBA::String_var processHostName = m_host->name();
        
		const char* p_stepLabel = 
		    (m_lifeCycle->init_steps.length() == 0) ?
		    "" :
		    m_lifeCycle->init_steps[m_stepCounter].step_label.in(); 
        
		CdmwPlatformMngt::ProcessStatusChange_var event
		    = m_procStatusChangeFactory->create(
			header,
			m_application->get_system_name(),
			processId.in(),
			issuer,
			status,
			m_stepCounter,
			p_stepLabel,
			m_status_info.in() );

		CORBA::String_var eventStr = event->to_string();
		LogMngr::logMessage(eventStr.in());

		m_application->get_event_notifier()->addEvent(event);
		event._retn();
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

    } // End namespace PlatformMngt
} // End namespace Cdmw

 
