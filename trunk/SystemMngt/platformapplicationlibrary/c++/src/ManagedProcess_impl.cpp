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


#include "platformapplicationlibrary/ManagedProcess_impl.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"
#include "SystemMngt/platformlibrary/RWServiceDefContainer_impl.hpp"
#include "SystemMngt/platformelementlibrary/RWEntityContainer_impl.hpp"
#include "SystemMngt/platformvaluetypes/ProcessStatusChange_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/ApplicationLogMessageId.hpp"
#include "platformapplicationlibrary/ServiceBroker_impl.hpp"

#include <set>

namespace Cdmw
{
    namespace PlatformMngt
    {

	ManagedProcessInitial*
	ManagedProcess_impl::M_initial = NULL;
	ManagedProcessInitialising*
	ManagedProcess_impl::M_initialising = NULL;
	ManagedProcessStepPerformed*
	ManagedProcess_impl::M_stepPerformed = NULL;
	ManagedProcessInitialised*
	ManagedProcess_impl::M_initialised = NULL;
	ManagedProcessRunRequest*
	ManagedProcess_impl::M_runRequest = NULL;
	ManagedProcessRunning*
	ManagedProcess_impl::M_running = NULL;
	ManagedProcessStopping*
	ManagedProcess_impl::M_stopping = NULL;
	ManagedProcessStopped*
	ManagedProcess_impl::M_stopped = NULL;
	ManagedProcessEnded*
	ManagedProcess_impl::M_ended = NULL;
	ManagedProcessFailedError*
	ManagedProcess_impl::M_failedError = NULL;
	ManagedProcessFailedMonitoringFailure*
	ManagedProcess_impl::M_failedMonitoringFailure = NULL;
	ManagedProcessFailedDeath*
	ManagedProcess_impl::M_failedDeath = NULL;
	ManagedProcessFailedDiscrepancy*
	ManagedProcess_impl::M_failedDiscrepancy = NULL;
	ManagedProcessFailedTimeout*
	ManagedProcess_impl::M_failedTimeout = NULL;
	ManagedProcessFailedInvalid*
	ManagedProcess_impl::M_failedInvalid = NULL;

// ----------------------------------------------------------------------
// ManagedProcess_impl class.
// ----------------------------------------------------------------------
	ManagedProcess_impl::ManagedProcess_impl(
	    PortableServer::POA_ptr poa,
	    CdmwPlatformMngt::ManagedProcessDef* process_def,
	    const char* process_key, const char* process_path,
	    Application_impl* application,
	    CdmwPlatformMngt::RWElementRepository_ptr element_repository)
	    throw(CORBA::SystemException)
	    : Process_impl(poa, process_def, process_key, process_path,
			   application, element_repository)
	{
	    m_stateMachine = NULL;
	    m_actualEntityContainer = NULL;
	    m_actualServiceDefContainer = NULL;
	    m_processDelegateWrap = CdmwPlatformMngt::ProcessDelegateWrapper::_nil();
	    m_default_entitydefs = NULL;       
	    
	    try
	    {
		m_actualServiceDefContainer
		    = ServiceDefContainer_impl::ServiceDefContainer::createProcessContainer(
			application->get_element_name(),
			process_def->name(),
			process_def->host_name());
                
		CdmwPlatformMngtService::ServiceDefContainer_var appliServiceDefContainer =
		    application->get_rwservice_defcontainer()->_this();
        
		m_rwServiceDefContainerServant = new RWServiceDefContainer_impl (
		    poa,
		    m_actualServiceDefContainer,
		    appliServiceDefContainer.in());
                
		std::auto_ptr <PlatformMngt::EntityContainer> actualEntityContainer (
		    new PlatformMngt::EntityContainer(poa, m_element_repository.in(),
						      m_element_path.in()));
    
		m_rwEntityContainerServant = 
		    new RWEntityContainer_impl ( 
                        poa, actualEntityContainer.get(), m_element_repository.in());

        

		std::auto_ptr <ManagedProcessStateMachine> stateMachine (
		    new ManagedProcessStateMachine (
			this, "ManagedProcess_impl"));
                                                
		std::auto_ptr <CommandCallback> process_start_cbk (new CommandCallback());
                                       
		std::auto_ptr <CommandCallback> process_stop_cbk (new CommandCallback());
    
    
		// Sets the process's internal data
		m_rwServiceDefContainer
		    = dynamic_cast <RWServiceDefContainer_impl*> (m_rwServiceDefContainerServant.in());
            
		m_rwEntityContainer
		    = dynamic_cast <RWEntityContainer_impl*> (m_rwEntityContainerServant.in());


		m_actualEntityContainer = actualEntityContainer.get();
                
		m_process_start_cbk = process_start_cbk.get();
		m_process_stop_cbk = process_stop_cbk.get();
        
        

		m_stateMachine = stateMachine.get();

		// Add the states to the state machine
		m_stateMachine->addState ("Initial", M_initial);
		m_stateMachine->addState ("Initialising", M_initialising);
		m_stateMachine->addState ("StepPerformed", M_stepPerformed);
		m_stateMachine->addState ("Initialised", M_initialised);
		m_stateMachine->addState ("RunRequest", M_runRequest);
		m_stateMachine->addState ("Running", M_running);
		m_stateMachine->addState ("Stopping", M_stopping);
		m_stateMachine->addState ("Stopped", M_stopped);
		m_stateMachine->addState ("Ended", M_ended);
		m_stateMachine->addState ("FailedError", M_failedError);
		m_stateMachine->addState ("FailedMonitoringFailure",
					  M_failedMonitoringFailure);
		m_stateMachine->addState ("FailedDeath", M_failedDeath);
		m_stateMachine->addState ("FailedDiscrepancy", M_failedDiscrepancy);
		m_stateMachine->addState ("FailedTimeout", M_failedTimeout);
		m_stateMachine->addState ("FailedInvalid", M_failedInvalid);

 
		// Release the ownership of the temporary objects
		actualEntityContainer.release();
		process_start_cbk.release();
		process_stop_cbk.release();
		stateMachine.release();

		// Set the initial state
		m_stateMachine->setState ("Initial");
	    }
	    catch (const std::bad_alloc&)
	    {
		if (m_actualEntityContainer != NULL)
		{
		    delete m_actualEntityContainer;
		}

		if (m_actualServiceDefContainer != NULL)
		{
		    ServiceDefContainer_impl::ServiceDefContainer::destroy(
			m_actualServiceDefContainer);
		}

		throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
	    }
	}

	ManagedProcess_impl::~ManagedProcess_impl()
	{
	    if (m_stateMachine != NULL) 
		delete m_stateMachine;

	    if (m_actualEntityContainer != NULL)
	    {
		delete m_actualEntityContainer;
	    }

	    if (m_actualServiceDefContainer != NULL)
	    {
		ServiceDefContainer_impl::ServiceDefContainer::destroy(
		    m_actualServiceDefContainer);
	    }
	}
	
	    
	void ManagedProcess_impl::end_creation(
	    CdmwPlatformMngt::ProcessDef* process_def)
	    throw (CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateStep,
                   OutOfMemoryException,
		   CosPropertyService::MultipleExceptions,
                   CORBA::SystemException)
	{


	    CdmwPlatformMngt::ManagedProcessDef* managed_process_def
		= CdmwPlatformMngt::ManagedProcessDef::_downcast(
		    process_def);
        
                        
	    // set element path in rwservicedef container
	    m_rwServiceDefContainer->set_container_path (m_element_path.in());



                        
	    // the following initialisation call the methods of the object
	    // setting the attribute changes in datastore
	    // in case of exception the object will be deleted by the application
    
	    // set the host for this process
	    // if not found throw HostNotFound exception
	    setHost(managed_process_def->host_name());
	
	    // set the process info
	    setProcessInfo (managed_process_def->process_info());
	
	    // set the init steps of the process
	    // if duplicated throw DuplicateStep exception
	    setLifeCycle (managed_process_def->life_cycle());
    
	    // set the monitoring info
	    setMonitoringInfo (managed_process_def->monitoring_info());
    
	    // initialise data store of the object
	    set_record_in_datastore();
    

	
	    // Add the entities of the process
	    // if duplicated throw DuplicateEntity exception
	    const CdmwPlatformMngt::EntityDefs& entities = managed_process_def->entities();
	    
	                        
	    unsigned int entityCount =  managed_process_def->entities().length();
	    // Store entity initial values into default_entities
	    m_default_entitydefs =  new CdmwPlatformMngt::EntityDefs();
	    m_default_entitydefs->length(entityCount);
    


	    for (unsigned int entityIndex=0;
		 entityIndex < entities.length();
		 entityIndex++)
	    {  
		// get entity definition (EntityDef is a structure, a reference on the
		// structure is returned by the [] operator)
		const CdmwPlatformMngt::EntityDef& entity_def = entities[entityIndex];
		
		// store values of the entityDef
		(*m_default_entitydefs)[entityIndex] = entity_def;

		try
		{
		    add_entity (entity_def);
		}
		catch(const CdmwPlatformMngt::EntityAlreadyExists&)
		{
		    // in this case the servant will be deleted by application
		    throw CdmwPlatformMngt::DuplicateEntity (entity_def.entity_name.in());
		}
	    }

	    // Add the services of the process
	    // if duplicated throw DuplicateService exception
	    const CdmwPlatformMngtService::ServiceDefs& services =
		managed_process_def->service_definitions();

	    for (unsigned int serviceIndex = 0;
		 serviceIndex < services.length();
		 serviceIndex++)
	    {
		// get service definition (ServiceDef is a structure, a reference on the
		// structure is returned by the [] operator)
		const CdmwPlatformMngtService::ServiceDef& service_def = services[serviceIndex];
                               
		try
		{                          
		    add_service_def (service_def);
		}
		catch (const CdmwPlatformMngtService::ServiceAlreadyExists&)
		{
		    // in this case the servant will be deleted by application
		    throw CdmwPlatformMngt::DuplicateService(
			service_def.service_name.in());
		}
	    }
	
	    //Add all Properties in ManagedProcess
                                                                                                                             
	    unsigned int propCount = managed_process_def->properties().length();
	    for (unsigned int propIndex=0 ; propIndex < propCount ; propIndex++)
	    {
		const CdmwPlatformMngt::PropertyDef& prop_def =
		    managed_process_def->properties()[propIndex];

		try
		{
		    std::string propertyName(managed_process_def->properties()[propIndex].property_name);
		    CosPropertyService::PropertyModeType propertyMode = CosPropertyService::PropertyModeType(0);
		    CdmwPlatformMngt::PropertyModeType propertyModeCdmw = managed_process_def->properties()[propIndex].mode_type;
		    if (propertyModeCdmw == 1) propertyMode = CosPropertyService::PropertyModeType(1);
		    define_property_with_mode(propertyName.c_str() , prop_def.initial_value, propertyMode);
		}
		catch (const CosPropertyService::InvalidPropertyName&)
                {
		    throw   CosPropertyService::MultipleExceptions(); /* TO-DO */
                }
		catch (const CosPropertyService::ConflictingProperty&)
                {
		    throw   CosPropertyService::MultipleExceptions(); /* TO-DO */
                }
		catch (const CosPropertyService::UnsupportedTypeCode&)
                {
		    throw   CosPropertyService::MultipleExceptions(); /* TO-DO */
                }
		catch (const CosPropertyService::UnsupportedProperty&)
                {
		    throw   CosPropertyService::MultipleExceptions(); /* TO-DO */
                }
		catch (const CosPropertyService::UnsupportedMode&)
                {
		    throw   CosPropertyService::MultipleExceptions(); /* TO-DO */
                }
		catch (const CosPropertyService::ReadOnlyProperty&)
                {
		    throw   CosPropertyService::MultipleExceptions(); /* TO-DO */
                }

	    }
	    // recover the default user defined attributes values 
	    store_default_properties();
	
	}

	void ManagedProcess_impl::synchro_with_datastore()
	{
	    // synchro the mother class element
	    Process_impl::synchro_with_datastore();
    
	    // synchro the ProcessDelegateWrapper reference    
	    try
	    {
		// get application agent from daemon
		CdmwPlatformMngt::ApplicationAgent_var agent = searchApplicationAgent();
        
		if( CORBA::is_nil (agent.in()))
		{
		    std::cout << "<<ManagedProcess_impl::synchro_with_datastore()>> agent not exist "
			      << m_element_name.in() << std::endl;
		}
		else
		{        
		    // get the ProcessDelegateWrapper
		    m_processDelegateWrap = agent->search_process_delegate_wrapper(m_element_name.in());
        
		    if (CORBA::is_nil(m_processDelegateWrap.in()))
		    {
			std::cout << "<<ManagedProcess_impl::synchro_with_datastore()>> ProcessDelegateWrapper not exist "
				  << m_element_name.in() << std::endl;
		    }  
		}      
	    }
	    catch(const CdmwPlatformMngt::HostNotFound& e)
	    {
		std::string error_info =
		    LogMngr::getFormattedMessage (MSG_ID_HOST_NOT_FOUND, 
						  m_element_name.in());
        
		std::cerr << "Process synchronisation : unexpected exception to get agent" << std::endl;
		std::cerr << error_info << std::endl;
	    }
	    catch(const CdmwPlatformMngt::HostNotReachable& e)
	    {
		std::string error_info =
		    LogMngr::getFormattedMessage (MSG_ID_HOST_UNREACHABLE, 
						  m_element_name.in());
        
		std::cerr << "Process synchronisation : unexpected exception to get agent" << std::endl;
		std::cerr << error_info << std::endl;

	    }
	    catch(const CORBA::SystemException& e)
	    {
		std::cerr << "Process synchronisation : unexpected exception to get agent for the host "
			  << m_element_name.in() << std::endl
			  << e << std::endl;
	    }
	    catch(const CORBA::Exception& e)
	    {
		std::cerr << "Process synchronisation : unexpected exception to get agent for the host "
			  << m_element_name.in() << std::endl
			  << e << std::endl;
	    }
	}




	void ManagedProcess_impl::synchro_processing_to_initial()
	{
	    m_stateMachine->setStateName("Initial");
	}
    

	void ManagedProcess_impl::synchro_processing_to_initialised()
	{
	    m_stateMachine->setStateName("Initialised");
	}
    

	void ManagedProcess_impl::synchro_processing_to_running()
	{
	    m_stateMachine->setStateName("Running");
	}



	const char* ManagedProcess_impl::get_state_machine_state_name()
	{
	    return m_stateMachine->getStateName();
	}

	void ManagedProcess_impl::set_state_machine_state_name(const char* state_name)
	{
	    m_stateMachine->setStateName(state_name);
	}


	void ManagedProcess_impl::createStates()
	    throw( OutOfMemoryException )
	{
	    try
	    {
		std::auto_ptr< ManagedProcessInitial > initial(
		    new ManagedProcessInitial() );
		std::auto_ptr< ManagedProcessInitialising > initialising(
		    new ManagedProcessInitialising() );
		std::auto_ptr< ManagedProcessStepPerformed > stepPerformed(
		    new ManagedProcessStepPerformed() );
		std::auto_ptr< ManagedProcessInitialised > initialised(
		    new ManagedProcessInitialised() );
		std::auto_ptr< ManagedProcessRunRequest > runRequest(
		    new ManagedProcessRunRequest() );
		std::auto_ptr< ManagedProcessRunning > running(
		    new ManagedProcessRunning() );
		std::auto_ptr< ManagedProcessStopping > stopping(
		    new ManagedProcessStopping() );
		std::auto_ptr< ManagedProcessStopped > stopped(
		    new ManagedProcessStopped() );
		std::auto_ptr< ManagedProcessEnded > ended(
		    new ManagedProcessEnded() );
		std::auto_ptr< ManagedProcessFailedError > failedError(
		    new ManagedProcessFailedError() );
		std::auto_ptr< ManagedProcessFailedMonitoringFailure > failedMonitoringFailure(
		    new ManagedProcessFailedMonitoringFailure() );
		std::auto_ptr< ManagedProcessFailedDeath > failedDeath(
		    new ManagedProcessFailedDeath() );
		std::auto_ptr< ManagedProcessFailedDiscrepancy > failedDiscrepancy(
		    new ManagedProcessFailedDiscrepancy() );
		std::auto_ptr< ManagedProcessFailedTimeout > failedTimeout(
		    new ManagedProcessFailedTimeout() );
		std::auto_ptr< ManagedProcessFailedInvalid > failedInvalid(
		    new ManagedProcessFailedInvalid() );

		if( M_initial == NULL )
		    M_initial = initial.release();
		if( M_initialising == NULL )
		    M_initialising = initialising.release();
		if( M_stepPerformed == NULL )
		    M_stepPerformed = stepPerformed.release();
		if( M_initialised == NULL )
		    M_initialised = initialised.release();
		if( M_runRequest == NULL )
		    M_runRequest = runRequest.release();
		if( M_running == NULL )
		    M_running = running.release();
		if( M_stopping == NULL )
		    M_stopping = stopping.release();
		if( M_stopped == NULL )
		    M_stopped = stopped.release();
		if( M_ended == NULL )
		    M_ended = ended.release();
		if( M_failedError == NULL )
		    M_failedError = failedError.release();
		if( M_failedMonitoringFailure == NULL )
		    M_failedMonitoringFailure = failedMonitoringFailure.release();
		if( M_failedDeath == NULL )
		    M_failedDeath = failedDeath.release();
		if( M_failedDiscrepancy == NULL )
		    M_failedDiscrepancy = failedDiscrepancy.release();
		if( M_failedTimeout == NULL )
		    M_failedTimeout = failedTimeout.release();
		if( M_failedInvalid == NULL )
		    M_failedInvalid = failedInvalid.release();
	    }
	    catch( const std::bad_alloc& )
	    {
		CDMW_THROW( OutOfMemoryException );
	    }
	}

	void ManagedProcess_impl::destroyStates()
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

	    if( M_stepPerformed != NULL )
	    {
		delete M_stepPerformed;
		M_stepPerformed = NULL;
	    }

	    if( M_initialised != NULL )
	    {
		delete M_initialised;
		M_initialised = NULL;
	    }

	    if( M_runRequest != NULL )
	    {
		delete M_runRequest;
		M_runRequest = NULL;
	    }

	    if( M_running != NULL )
	    {
		delete M_running;
		M_running = NULL;
	    }

	    if( M_stopping != NULL )
	    {
		delete M_stopping;
		M_stopping = NULL;
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

	    if( M_failedError != NULL )
	    {
		delete M_failedError;
		M_failedError = NULL;
	    }

	    if( M_failedMonitoringFailure != NULL )
	    {
		delete M_failedMonitoringFailure;
		M_failedMonitoringFailure = NULL;
	    }

	    if( M_failedDeath != NULL )
	    {
		delete M_failedDeath;
		M_failedDeath = NULL;
	    }

	    if( M_failedDiscrepancy != NULL )
	    {
		delete M_failedDiscrepancy;
		M_failedDiscrepancy = NULL;
	    }

	    if( M_failedTimeout != NULL )
	    {
		delete M_failedTimeout;
		M_failedTimeout = NULL;
	    }

	    if( M_failedInvalid != NULL )
	    {
		delete M_failedInvalid;
		M_failedInvalid = NULL;
	    }
	}


	void ManagedProcess_impl::error_event(
	    const CdmwPlatformMngtBase::EventHeader& header,
	    const char* issuer,
	    const char* error_info)
	{
	    CDMW_SM_WRITER_LOCK_GUARD(*m_stateMachine);

	    m_stateMachine->getState()->error_event(
		m_stateMachine,
		header,
		issuer,
		error_info);
	}

	void ManagedProcess_impl::ending_event(
	    const CdmwPlatformMngtBase::EventHeader& header)
	{
	    CDMW_SM_WRITER_LOCK_GUARD(*m_stateMachine);

	    m_stateMachine->getState()->ending_event(
		m_stateMachine,
		header);
	}

	void ManagedProcess_impl::monitoringFailure_event(
	    const CdmwPlatformMngtBase::EventHeader& header)
	    throw( CdmwPlatformMngt::IncompatibleType,
		   CORBA::SystemException )
	{
	    CDMW_SM_WRITER_LOCK_GUARD(*m_stateMachine);

	    m_stateMachine->getState()->monitoringFailure_event(
		m_stateMachine,
		header);
        
	    // update record in datastore
	    set_record_in_datastore();
	}

	void ManagedProcess_impl::invalidate_event(
	    const CdmwPlatformMngtBase::EventHeader& header,
	    const char* reason)
	{
	    CDMW_SM_WRITER_LOCK_GUARD(*m_stateMachine);

	    m_stateMachine->getState()->invalidate_event(
		m_stateMachine,
		header,
		reason);
	}

	CdmwPlatformMngt::ProcessStatus ManagedProcess_impl::get_internal_status()
	{
	    CDMW_SM_READER_LOCK_GUARD(*m_stateMachine);
    
	    // get status
	    CdmwPlatformMngt::ProcessStatus processStatus = 
		m_stateMachine->getState()->status();
                            
	    return processStatus;
	}


	CORBA::Object_ptr ManagedProcess_impl::embedded_service()
	    throw( CORBA::SystemException ) 
	{
	    if( type() != CdmwPlatformMngt::SERVICE_PROVIDER )
		return CORBA::Object::_nil();

	    if (CORBA::is_nil (m_processDelegateWrap.in()))
		return CORBA::Object::_nil();
        
	    return m_processDelegateWrap->get_service();
	}


	CdmwPlatformMngt::ProcessStatus ManagedProcess_impl::get_status(
	    CORBA::String_out status_info )
	    throw( CORBA::SystemException )    
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

	void ManagedProcess_impl::set_auto_ending(CORBA::Boolean auto_ending)
	    throw (CdmwPlatformMngt::IncompatibleStatus,
		   CORBA::SystemException)
	{
	    CDMW_SM_READER_LOCK_GUARD(*m_stateMachine);

	    m_stateMachine->getState()->set_autoending (m_stateMachine, auto_ending);
	}

	void ManagedProcess_impl::set_host(
	    const char* host_name)
	    throw (CdmwPlatformMngt::HostNotFound,
		   CdmwPlatformMngt::IncompatibleStatus,
		   CORBA::SystemException)
	{
	    CDMW_SM_READER_LOCK_GUARD(*m_stateMachine);

	    m_stateMachine->getState()->set_host(m_stateMachine, host_name);
	}

	void ManagedProcess_impl::set_process_info(
	    const CdmwPlatformMngt::ProcessInfo& process_info)
	    throw (CdmwPlatformMngt::IncompatibleStatus,
		   CORBA::SystemException)
	{
	    CDMW_SM_READER_LOCK_GUARD(*m_stateMachine);

	    m_stateMachine->getState()->set_process_info(
		m_stateMachine,
		process_info);
	}

	CdmwPlatformMngt::LifeCycleDef* ManagedProcess_impl::get_life_cycle()
	    throw (CdmwPlatformMngt::IncompatibleType,
		   CORBA::SystemException)
	{
	    try
	    {
		CdmwPlatformMngt::LifeCycleDef_var lifeCycleDef
		    = new CdmwPlatformMngt::LifeCycleDef (m_lifeCycle.in());
            
		return lifeCycleDef._retn();
	    }
	    catch(const std::bad_alloc&)
	    {
		throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
	    }
	}

	void ManagedProcess_impl::set_life_cycle(
	    const CdmwPlatformMngt::LifeCycleDef& life_cycle)
	    throw (CdmwPlatformMngt::DuplicateStep,
		   CdmwPlatformMngt::IncompatibleType,
		   CdmwPlatformMngt::IncompatibleStatus,
		   CORBA::SystemException)
	{
	    CDMW_SM_READER_LOCK_GUARD(*m_stateMachine);

	    m_stateMachine->getState()->set_life_cycle(
		m_stateMachine,
		life_cycle);
	}

	CdmwPlatformMngt::CompleteMonitoringInfo ManagedProcess_impl::get_monitoring_info()
	    throw (CdmwPlatformMngt::IncompatibleType,
		   CORBA::SystemException)
	{
	    return m_monitoring_info.in();
	}

	void ManagedProcess_impl::set_monitoring_info(
	    const CdmwPlatformMngt::CompleteMonitoringInfo& monitoring_info)
	    throw (CdmwPlatformMngt::IncompatibleType,
		   CdmwPlatformMngt::IncompatibleStatus,
		   CORBA::SystemException)
	{
	    CDMW_SM_READER_LOCK_GUARD(*m_stateMachine);

	    m_stateMachine->getState()->set_monitoring_info(
		m_stateMachine,
		monitoring_info);
	}

	CdmwPlatformMngt::RequestStatus ManagedProcess_impl::initialise(
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

	CdmwPlatformMngt::RequestStatus ManagedProcess_impl::next_step()
	    throw( CdmwPlatformMngt::InvalidStep,
		   CdmwPlatformMngt::IncompatibleType,
		   CdmwPlatformMngt::IncompatibleStatus,
		   CORBA::SystemException )
	{
	    CDMW_SM_WRITER_LOCK_GUARD(*m_stateMachine);

	    CdmwPlatformMngt::RequestStatus req_status;
    
	    try
	    {
		req_status =
		    m_stateMachine->getState()->next_step(m_stateMachine);
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

	CdmwPlatformMngt::RequestStatus ManagedProcess_impl::nextStep(
	    unsigned int targetStep)
	    throw (CdmwPlatformMngt::InvalidStep,
		   CdmwPlatformMngt::IncompatibleType,
		   CdmwPlatformMngt::IncompatibleStatus,
		   CORBA::SystemException)
	{
	    CDMW_SM_WRITER_LOCK_GUARD(*m_stateMachine);

	    if (targetStep != (m_stepCounter+1))
	    {
		LogMngr::logMessage(
		    ERR,
		    MSG_ID_PROCESS_UNEXPECTED_STEP,
		    targetStep,
		    get_process_key());

		return CdmwPlatformMngt::REQUEST_ERROR;
	    }

	    CdmwPlatformMngt::RequestStatus req_status;
    
	    try
	    {
		req_status =
		    m_stateMachine->getState()->next_step(m_stateMachine);
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

	CdmwPlatformMngt::RequestStatus ManagedProcess_impl::run()
	    throw( CdmwPlatformMngt::NotReadyToRun,
		   CdmwPlatformMngt::IncompatibleType,
		   CdmwPlatformMngt::IncompatibleStatus,
		   CORBA::SystemException )
	{
	    LogMngr::logMessage(
		INF,
		MSG_ID_PROCESS_RUN,
		get_process_key(),
		m_application->get_element_name());

	    CDMW_SM_WRITER_LOCK_GUARD(*m_stateMachine);

	    CdmwPlatformMngt::RequestStatus req_status;
	    
	    try
	      {
		// recover the default user defined attributes values 
		recover_default_properties();
		
		// recover default entityDef
		CORBA::ULong how_many = m_rwEntityContainer->get_number_of_entities();
		CdmwPlatformMngt::ElementNames_var element_names;
		CdmwPlatformMngt::ElementNamesIterator_var element_names_iterator;
		m_rwEntityContainer -> get_all_entity_names(how_many, element_names.out(), element_names_iterator.out());
		//delete all entity
		CORBA::ULong size = element_names->length();
		for (CORBA::ULong count = 0; count < size; count++ )
		  {
		    m_rwEntityContainer -> remove_entity (element_names[count]);
		  }
		//restore the size of default entities
		element_names->length(m_default_entitydefs->length());
		for (CORBA::ULong count=0; count < element_names->length();count++ )
		  {
		    m_rwEntityContainer -> add_entity ( (*m_default_entitydefs)[count]);
		  }
		
		req_status =
		    m_stateMachine->getState()->run(m_stateMachine);
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

	CdmwPlatformMngt::RequestStatus ManagedProcess_impl::stop(
	    CORBA::Boolean emergency)
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
		    m_stateMachine->getState()->stop(m_stateMachine, emergency);
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

	CORBA::ULong ManagedProcess_impl::get_number_of_entities()
	    throw( CORBA::SystemException )
	{
	    return m_rwEntityContainer->get_number_of_entities();
	}

	void ManagedProcess_impl::get_all_entity_names(
	    CORBA::ULong how_many,
	    CdmwPlatformMngt::ElementNames_out entity_names,
	    CdmwPlatformMngt::ElementNamesIterator_out rest )
	    throw( CORBA::SystemException )
	{
	    m_rwEntityContainer->get_all_entity_names(
		how_many, entity_names, rest );
	}

	CdmwPlatformMngt::Entity_ptr ManagedProcess_impl::get_entity (
	    const char* entity_name)
	    throw( CdmwPlatformMngt::EntityNotFound, CORBA::SystemException )
	{
	    return m_rwEntityContainer->get_entity (entity_name);
	}

	CORBA::Boolean ManagedProcess_impl::get_entities(
	    const CdmwPlatformMngt::ElementNames& entity_names,
	    CdmwPlatformMngt::Entities_out nentities )
	    throw( CORBA::SystemException )
	{
	    return m_rwEntityContainer->get_entities(
		entity_names, nentities );
	}

	void ManagedProcess_impl::get_all_entities(
	    CORBA::ULong how_many,
	    CdmwPlatformMngt::Entities_out nentities,
	    CdmwPlatformMngt::EntitiesIterator_out rest )
	    throw( CORBA::SystemException )
	{
	    m_rwEntityContainer->get_all_entities(
		how_many, nentities, rest );
	}

	CdmwPlatformMngt::Entity_ptr ManagedProcess_impl::add_entity(
	    const CdmwPlatformMngt::EntityDef & entity_def)
	    throw( CdmwPlatformMngt::EntityAlreadyExists, CORBA::SystemException )
	{
	    std::string entityName = entity_def.entity_name.in();
    
	    LogMngr::logMessage(INF, MSG_ID_ADD_PROCESS_ENTITY, 
				entityName.c_str(), m_proc_appli_name.in(), 
				m_element_name.in(), m_proc_host_name.in());
    
	    return m_rwEntityContainer->add_entity(entity_def);

	    // TODO: Notify the configuration change
	}

	void ManagedProcess_impl::remove_entity(
	    const char* entity_name )
	    throw( CdmwPlatformMngt::EntityNotFound, CORBA::SystemException )
	{
	    LogMngr::logMessage(INF, MSG_ID_REMOVE_PROCESS_ENTITY, 
				entity_name, m_proc_appli_name.in(), 
				m_element_name.in(), m_proc_host_name.in());
                        
	    m_rwEntityContainer->remove_entity(entity_name);

	    // TODO: Notify the configuration change
	}

	CdmwPlatformMngtService::ServiceProviderID*
	ManagedProcess_impl::get_service_def(
	    const char* service_name )
	    throw( CdmwPlatformMngtService::ServiceNotFound, CORBA::SystemException )
	{
	    return m_rwServiceDefContainer->get_service_def( service_name );
	}

	void ManagedProcess_impl::add_service_def(
	    const CdmwPlatformMngtService::ServiceDef& service_def )
	    throw( CdmwPlatformMngtService::ServiceAlreadyExists, CORBA::SystemException )
	{
	    m_rwServiceDefContainer->add_service_def( service_def );

	    // TODO: Notify the configuration change

	    // TODO: Trace to the log manager
	}

	void ManagedProcess_impl::remove_service_def(
	    const char* service_name )
	    throw( CdmwPlatformMngtService::ServiceNotFound, CORBA::SystemException )
	{
	    m_rwServiceDefContainer->remove_service_def( service_name );

	    // TODO: Notify the configuration change

	    // TODO: Trace to the log manager
	}

	void ManagedProcess_impl::remove_all_service_defs()
	    throw (CORBA::SystemException)
	{
	    m_rwServiceDefContainer->remove_all_service_defs();

	    // TODO: Notify the configuration change

	    // TODO: Trace to the log manager
	}


	void ManagedProcess_impl::setState( const char* stateName )
	{
	    m_stateMachine->setState (stateName);
	}

	CdmwPlatformMngt::ProcessStatus ManagedProcess_impl::getStatus()
	    throw( CORBA::SystemException )    
	{
	    return m_stateMachine->getState()->status();
	}

	void ManagedProcess_impl::setLifeCycle(
	    const CdmwPlatformMngt::LifeCycleDef& life_cycle)
	    throw (CdmwPlatformMngt::DuplicateStep,
		   CORBA::SystemException)
	{
	    try
	    {
		std::set< std::string > steps;
		for (unsigned int i=0; i<life_cycle.init_steps.length(); i++)
		{
		    std::string stepName = life_cycle.init_steps[i].step_label.in();
		    if (steps.find(stepName) != steps.end())
			throw CdmwPlatformMngt::DuplicateStep(stepName.c_str());
                
		    steps.insert(stepName);
		}
    
		m_lifeCycle = new CdmwPlatformMngt::LifeCycleDef(life_cycle);
        
		// update record in datastore
		set_record_in_datastore();
	    }
	    catch(const std::bad_alloc&)
	    {
		throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
	    }
	}

	void ManagedProcess_impl::notifyStatusChangeEvent(
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


	void ManagedProcess_impl::set_create_process_result (
	    CdmwPlatformMngt::ProcessCommandResult result,
	    const char* error_info,
	    const char* error_issuer,
	    CdmwPlatformMngt::ProcessDelegateWrapper_ptr process_delegate_wrap)
            throw (CORBA::SystemException)
	{
	    // store the process delegate wrapper object
	    m_processDelegateWrap =
		CdmwPlatformMngt::ProcessDelegateWrapper::_duplicate (process_delegate_wrap);
         
	    m_starting_result = result;
    
	    m_creation_error_info = CORBA::string_dup (error_info);
	    m_creation_error_issuer = CORBA::string_dup (error_issuer);
    
	    // signals the acknowledgement
	    if (!m_process_start_cbk->signal())
	    {
		// The acknowledgement has been performed too late
		throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);
	    }
	}


     

	void ManagedProcess_impl::set_initialisation_result (
	    CdmwPlatformMngt::ProcessCommandResult result)
            throw (CORBA::SystemException)
	{
	    m_starting_result = result;
    
	    // signals the acknowledgement
	    if (!m_process_start_cbk->signal())
	    {
		// The acknowledgement has been performed too late
		throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);
	    }
	}
     

	void ManagedProcess_impl::set_next_step_result (
	    CdmwPlatformMngt::ProcessCommandResult result,
	    CORBA::ULong step_nbr)
            throw (CORBA::SystemException)
	{
	    m_starting_result = result;
	    m_step_nbr_done = step_nbr;
    
	    // signals the acknowledgement
	    if (!m_process_start_cbk->signal())
	    {
		// The acknowledgement has been performed too late
		throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);
	    }
	}
     

	void ManagedProcess_impl::set_run_result (
	    CdmwPlatformMngt::ProcessCommandResult result)
            throw (CORBA::SystemException)
	{
	    m_starting_result = result;
    
	    // signals the acknowledgement
	    if (!m_process_start_cbk->signal())
	    {
		// The acknowledgement has been performed too late
		throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);
	    }
	}
     

	void ManagedProcess_impl::set_stop_result (
	    CdmwPlatformMngt::ProcessCommandResult result)
            throw (CORBA::SystemException)
	{
	    m_stopping_result = result;
    
	    // signals the acknowledgement
	    if (!m_process_stop_cbk->signal())
	    {
		// The acknowledgement has been performed too late
		throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);
	    }
	}

    } // End namespace PlatformMngt
} // End namespace Cdmw

