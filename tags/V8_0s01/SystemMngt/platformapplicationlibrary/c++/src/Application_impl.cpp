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


#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/osthreads/ReaderLockGuard.hpp"
#include "Foundation/osthreads/WriterLockGuard.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtDaemon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp"

#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"
#include "SystemMngt/platformlibrary/GraphChecker.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"

#include "SystemMngt/platformlibrary/RWServiceDefContainer_impl.hpp"
#include "SystemMngt/platformlibrary/ServiceDefContainer_impl.hpp"
#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"
#include "SystemMngt/platformlibrary/SynchronisableTaskDef.hpp"
#include "SystemMngt/platformlibrary/TaskFactory.hpp"
#include "SystemMngt/platformlibrary/TaskSequencer.hpp"
#include "SystemMngt/platformlibrary/TaskStarter.hpp"

#include "SystemMngt/platformelementlibrary/EntityContainer_impl.hpp"
#include "SystemMngt/platformelementlibrary/RWEntityContainer_impl.hpp"
#include "SystemMngt/platformelementlibrary/EntityContainer.hpp"
#include "SystemMngt/platformlibrary/ManagedElementKey.hpp"

#include "SystemMngt/platformvaluetypes/EntityStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationModeChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/GraphElement_impl.hpp"

#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/ApplicationLogMessageId.hpp"

#include "platformapplicationlibrary/Process_impl.hpp"
#include "platformapplicationlibrary/ManagedProcess_impl.hpp"
#include "platformapplicationlibrary/UnmanagedProcess_impl.hpp"
#include "platformapplicationlibrary/ProcessObserver_impl.hpp"
#include "platformapplicationlibrary/ServiceBroker_impl.hpp"
#include "platformapplicationlibrary/ProcessesIterator_impl.hpp"
#include "platformapplicationlibrary/ProcessInit.hpp"
#include "platformapplicationlibrary/ProcessRun.hpp"
#include "platformapplicationlibrary/ProcessStop.hpp"



namespace Cdmw
{
  namespace PlatformMngt
  {

    ApplicationInitial *Application_impl::M_initial = NULL;
    ApplicationInitialising *Application_impl::M_initialising = NULL;
    ApplicationInitialisationSuspended *Application_impl::
      M_initialisationSuspended = NULL;
    ApplicationInitialised *Application_impl::M_initialised = NULL;
    ApplicationRunRequest *Application_impl::M_runRequest = NULL;
    ApplicationRunning *Application_impl::M_running = NULL;
    ApplicationStopping *Application_impl::M_stopping = NULL;
    ApplicationStopped *Application_impl::M_stopped = NULL;
    ApplicationInitialisationIncomplete *Application_impl::
      M_initialisationIncomplete = NULL;

// ----------------------------------------------------------------------
// Application_impl class.
// ----------------------------------------------------------------------
#ifdef CDMW_USE_FAULTTOLERANCE
      Application_impl::Application_impl (PortableServer::POA_ptr poa,
					  SupervisionEventNotifier *
					  eventNotifier,
					  CdmwPlatformMngt::
					  RWElementRepository_ptr
					  element_repository,
					  const std::
					  string & replicationManagerIOGR,
					  CdmwPlatformMngt::
					  FTConverter_ptr ftConverter)
      throw (BadParameterException, AlreadyDoneException,
	     UnknownStateException,
	     OutOfMemoryException):DeactivableServant_impl (poa),
      ManagedElement_impl (element_repository), m_applicationDefined (false),
      m_initialisation_condition (m_mutex), m_stop_condition (m_mutex),
      m_replicationManagerIOGR (replicationManagerIOGR)
#else
      Application_impl::Application_impl (PortableServer::POA_ptr poa,
					  SupervisionEventNotifier *
					  eventNotifier,
					  CdmwPlatformMngt::
					  RWElementRepository_ptr
					  element_repository)
      throw (BadParameterException, AlreadyDoneException,
	     UnknownStateException,
	     OutOfMemoryException):DeactivableServant_impl (poa),
      ManagedElement_impl (element_repository), m_applicationDefined (false),
      m_initialisation_condition (m_mutex), m_stop_condition (m_mutex)
#endif
    {
      if (eventNotifier == NULL)
	CDMW_THROW2 (BadParameterException, "eventNotifier", "NULL");

      m_graphElementFactory = NULL;
      m_appEntityStatusChangeFactory = NULL;
      m_appStatusChangeFactory = NULL;
      m_appModeChangeFactory = NULL;
      m_stateMachine = NULL;
      m_actualEntityContainer = NULL;
      m_actualServiceDefContainer = NULL;
      m_serviceBroker = NULL;
      m_processObserver = NULL;
      m_serviceDefContainer = NULL;
      m_default_entitydefs = NULL;

      try
      {
	std::auto_ptr < GraphElementFactory >
	  graphElementFactory (new GraphElementFactory ());

	std::auto_ptr < ApplicationEntityStatusChangeFactory >
	  appEntityStatusChangeFactory (new
					ApplicationEntityStatusChangeFactory
					());

	std::auto_ptr < ApplicationStatusChangeFactory >
	  appStatusChangeFactory (new ApplicationStatusChangeFactory ());

	std::auto_ptr < ApplicationModeChangeFactory >
	  appModeChangeFactory (new ApplicationModeChangeFactory ());

	std::auto_ptr < ApplicationStateMachine >
	  stateMachine (new ApplicationStateMachine (this, "Application"));

	std::auto_ptr < CdmwPlatformMngt::GraphElements >
	  processInitGraph (new CdmwPlatformMngt::GraphElements);

	std::auto_ptr < CdmwPlatformMngt::GraphElements >
	  processStopGraph (new CdmwPlatformMngt::GraphElements);


	// create the process observer       
	m_processObserverServant = new ProcessObserver_impl (poa, this);
	m_processObserver
	  =
	  dynamic_cast <
	  ProcessObserver_impl * >(m_processObserverServant.in ());


	// Set the internal data

	m_poa = PortableServer::POA::_duplicate (poa);
	m_graphElementFactory = graphElementFactory.get ();
	m_appEntityStatusChangeFactory = appEntityStatusChangeFactory.get ();
	m_appStatusChangeFactory = appStatusChangeFactory.get ();
	m_appModeChangeFactory = appModeChangeFactory.get ();
	m_stateMachine = stateMachine.get ();
	m_processInitGraph = processInitGraph.get ();
	m_processStopGraph = processStopGraph.get ();

	m_statusInfo = CORBA::string_dup ("");
	m_mode = CdmwPlatformMngt::NORMAL_MODE;
	m_modeInfo = CORBA::string_dup ("");
	m_emergency = false;
	m_newInitGraph = true;
	m_startup_kind = new CdmwPlatformMngtBase::StartupKind ();
	m_eventNotifier = eventNotifier;
	m_taskFactory = NULL;
	m_starter = NULL;
	m_initSyncTaskFactory = NULL;
	m_stopSyncTaskFactory = NULL;
	m_initSequencer = NULL;
	m_stopSequencer = NULL;

	m_init_condition_flag = false;
	m_stop_condition_flag = false;

	m_process_servants.clear ();

	// Start the event handler
	m_eventHandler.start ();

	// Add the states to the state machine
	m_stateMachine->addState ("Initial", M_initial);
	m_stateMachine->addState ("Initialising", M_initialising);
	m_stateMachine->addState ("InitialisationSuspended",
				  M_initialisationSuspended);
	m_stateMachine->addState ("Initialised", M_initialised);
	m_stateMachine->addState ("RunRequest", M_runRequest);
	m_stateMachine->addState ("Running", M_running);
	m_stateMachine->addState ("Stopping", M_stopping);
	m_stateMachine->addState ("Stopped", M_stopped);
	m_stateMachine->addState ("InitialisationIncomplete",
				  M_initialisationIncomplete);

	// Set the initial state
	m_stateMachine->setState ("Initial");



#ifdef CDMW_USE_FAULTTOLERANCE
	m_ftConverter =
	  CdmwPlatformMngt::FTConverter::_duplicate (ftConverter);
#endif


	// Release the ownership of the temporary objects
	graphElementFactory.release ();
	appEntityStatusChangeFactory.release ();
	appStatusChangeFactory.release ();
	appModeChangeFactory.release ();
	stateMachine.release ();
	processInitGraph.release ();
	processStopGraph.release ();
      }
      catch (const std::bad_alloc &)
      {
	CDMW_THROW (OutOfMemoryException);
      }

      try
      {
	Cdmw::CommonSvcs::DataStore::DataStoreBase * ds =
	  Cdmw::CommonSvcs::DataStore::DataStoreRegistry::
	  Get_data_store (Configuration::Get_application_datastore_id ());
	m_application_ds =
	  dynamic_cast < Cdmw::PlatformMngt::ApplicationDataStore * >(ds);
      }
      catch (const NotExistException & e)
      {
	CDMW_ASSERT (false);
      }
    }

    Application_impl::~Application_impl ()
    {
      destroyStarter ();
      destroyInitSequencer ();
      destroyStopSequencer ();

      m_eventHandler.stop ();

      if (m_graphElementFactory != NULL)
	{
	  m_graphElementFactory->_remove_ref ();
	}

      if (m_appEntityStatusChangeFactory != NULL)
	{
	  m_appEntityStatusChangeFactory->_remove_ref ();
	}

      if (m_appStatusChangeFactory != NULL)
	{
	  m_appStatusChangeFactory->_remove_ref ();
	}

      if (m_appModeChangeFactory != NULL)
	{
	  m_appModeChangeFactory->_remove_ref ();
	}

      if (m_stateMachine != NULL)
	{
	  delete m_stateMachine;
	}

      if (m_actualEntityContainer != NULL)
	{
	  delete m_actualEntityContainer;
	}

      if (m_actualServiceDefContainer != NULL)
	{
	  ServiceDefContainer_impl::ServiceDefContainer::
	    destroy (m_actualServiceDefContainer);
	}


#ifdef CDMW_USE_FAULTTOLERANCE

      // unregister the process observer from the ftconverter
      m_ftConverter->unregister_process_observer (m_element_name.in ());

#endif


      // note : servant are deleted by the corresponding _var attributes
      //        destruction

      if (m_processObserver != NULL)
	{
	  try
	  {
	    m_processObserver->deactivate ();
	  }
	  catch (...)
	  {
	  }
	}

      if (m_serviceDefContainer != NULL)
	{
	  try
	  {
	    m_serviceDefContainer->deactivate ();
	  }
	  catch (...)
	  {
	  }
	}

      if (m_serviceBroker != NULL)
	{
	  try
	  {
	    m_serviceBroker->deactivate ();
	  }
	  catch (...)
	  {
	  }
	}

      // remove record from datastore
      remove_record_in_datastore ();

    }





    void Application_impl::end_creation (const CdmwPlatformMngt::
					 ApplicationStartingData &
					 applicationStartData,
					 const char *application_path)
      throw (OutOfMemoryException, CORBA::SystemException)
    {
      try
      {
	// store element path in managed element
	set_element_path (application_path,
			  applicationStartData.application_name.in ());

	// save starting data
	m_applicationStartData =
	  new CdmwPlatformMngt::
	  ApplicationStartingData (applicationStartData);

	// save system name
	m_system_name =
	  CORBA::string_dup (m_applicationStartData->system_name.in ());


	// create containers                
	std::auto_ptr < PlatformMngt::EntityContainer >
	  actualEntityContainer (new PlatformMngt::
				 EntityContainer (m_poa.in (),
						  m_element_repository.in (),
						  m_element_path.in ()));

	m_actualServiceDefContainer =
	  ServiceDefContainer_impl::ServiceDefContainer::
	  createApplicationContainer (m_element_name.in ());

	m_rwEntityContainerServant =
	  new RWEntityContainer_impl (m_poa.in (),
				      actualEntityContainer.get (),
				      m_element_repository.in ());

	m_serviceDefContainerServant =
	  new ServiceDefContainer_impl (m_poa.in (),
					m_actualServiceDefContainer,
					applicationStartData.
					system_service_def_container.in ());

	m_rwServiceDefContainerServant =
	  new RWServiceDefContainer_impl (m_poa.in (),
					  m_actualServiceDefContainer,
					  applicationStartData.
					  system_service_def_container.in ());



	// create service broker
	m_serviceBrokerServant = new ServiceBroker_impl (m_poa.in (), this,
							 applicationStartData.
							 service_binding.
							 in ());
	m_serviceBroker =
	  dynamic_cast < ServiceBroker_impl * >(m_serviceBrokerServant.in ());

	// get system service broker object reference
	m_syst_serviceBroker =
	  CdmwPlatformMngtService::ServiceBroker::
	  _duplicate (applicationStartData.service_broker.in ());

	// get system process observer object reference
	m_syst_processObserver =
	  CdmwPlatformMngt::ProcessObserver::_duplicate (applicationStartData.
							 process_observer.
							 in ());

	// Set the additional internal data with the acknowledgement data
	m_rwEntityContainer
	  =
	  dynamic_cast <
	  RWEntityContainer_impl * >(m_rwEntityContainerServant.in ());

	m_serviceDefContainer
	  =
	  dynamic_cast <
	  ServiceDefContainer_impl * >(m_serviceDefContainerServant.in ());

	m_rwServiceDefContainer
	  =
	  dynamic_cast <
	  RWServiceDefContainer_impl *
	  >(m_rwServiceDefContainerServant.in ());


	m_hostContainer =
	  CdmwPlatformMngt::HostContainer::_duplicate (applicationStartData.
						       host_container.in ());

	m_eventNotifier->registerObserver (m_element_name.in (),
					   m_applicationStartData->
					   supervision_observer.in ());

	// set element path in rwservicedef container
	m_rwServiceDefContainer->set_container_path (application_path);


	// Activate objects
	m_processObserver->activate ();
	m_serviceBroker->activate ();
	m_serviceDefContainer->activate ();

	// set internal with temporary auto pointer
	m_actualEntityContainer = actualEntityContainer.get ();


#ifdef CDMW_USE_FAULTTOLERANCE
	// register the process observer for the application
	m_ftConverter->register_process_observer (m_element_name.in (),
						  m_processObserver->
						  _this ());
#endif


	// initialise data store of the object
	set_record_in_datastore ();

	// Release the ownership of the temporary objects
	actualEntityContainer.release ();
      }
      catch (const std::bad_alloc &)
      {
	if (m_actualEntityContainer != NULL)
	  delete m_actualEntityContainer;

	if (m_actualServiceDefContainer != NULL)
	  ServiceDefContainer_impl::ServiceDefContainer::
	    destroy (m_actualServiceDefContainer);

	CDMW_THROW (OutOfMemoryException);
      }
    }


    void Application_impl::set_record_in_datastore ()
    {
      // test if the supervision is not fault tolerant or if it is, is it the primary?
      if (!Configuration::Is_fault_tolerant ()
	  || Configuration::Is_primary ())
	{
	  if (Configuration::Is_synchronising ())
	    {
	      std::
		cout <<
		"<<Application_impl::set_record_in_datastore() start >> " <<
		m_element_name.in () << std::endl;
	    }

	  CdmwPlatformMngt::ApplicationRecord record;
	  record.status = m_stateMachine->getState ()->status ();
	  record.status_info = m_statusInfo;
	  record.mode = m_mode;
	  record.mode_info = m_modeInfo;
	  record.state_machine =
	    CORBA::string_dup (m_stateMachine->getStateName ());
	  record.startup_kind = m_startup_kind.in ();
	  record.stop_emergency = m_emergency;
	  record.new_graph = m_newInitGraph;
	  record.process_init_graph = m_processInitGraph;
	  record.process_stop_graph = m_processStopGraph;

	  if (m_application_ds->contains (m_element_path.in ()))
	    {
	      m_application_ds->update (m_element_path.in (), record);
	    }
	  else
	    {
	      set_element_in_datastore ();

	      m_application_ds->insert (m_element_path.in (), record);
	    }

	  if (Configuration::Is_synchronising ())
	    {
	      std::
		cout <<
		"<<Application_impl::set_record_in_datastore() done >> " <<
		m_element_name.in () << std::endl;
	    }
	}
    }


    void Application_impl::remove_record_in_datastore ()
    {
      // inhibition if backup
      if (!Configuration::Is_fault_tolerant ()
	  || Configuration::Is_primary ())
	{
	  try
	  {
	    m_application_ds->remove (m_element_path.in ());
	  }
	  catch (Common::NotFoundException &)
	  {
	  }
	}
    }

    CdmwPlatformMngt::ApplicationRecord *
      Application_impl::get_record_from_datastore ()
    {
      CdmwPlatformMngt::ApplicationRecord * application_record = NULL;

      try
      {
	application_record = m_application_ds->select (m_element_path.in ());
      }
      catch (Common::NotFoundException &)
      {
	std::cout << "application record cannot be found in datastore : "
	  << m_element_path.in () << std::endl;
	CDMW_ASSERT (false);
      }

      return application_record;
    }


    void Application_impl::synchro_with_datastore ()
    {
      CdmwPlatformMngt::ApplicationRecord_var application_record =
	get_record_from_datastore ();

      m_statusInfo = application_record->status_info;
      m_mode = application_record->mode;
      m_modeInfo = application_record->mode_info;

      m_stateMachine->setStateName (application_record->state_machine.in ());

      m_startup_kind =
	new CdmwPlatformMngtBase::StartupKind (application_record->
					       startup_kind);

      m_emergency = application_record->stop_emergency;
      m_newInitGraph = true;

      m_processInitGraph =
	new CdmwPlatformMngt::GraphElements (application_record->
					     process_init_graph);

      m_processStopGraph =
	new CdmwPlatformMngt::GraphElements (application_record->
					     process_stop_graph);
    }

    void Application_impl::synchro_processes_with_datastore ()
    {
      Cdmw::PlatformMngt::ManagedElement_impl::synchro_with_datastore ();
      ProcessServants::iterator processIt;

      for (processIt = m_process_servants.begin ();
	   processIt != m_process_servants.end (); processIt++)
	{
	  Process_impl *process = processIt->second;

	  // synchro the process with its datastore
	  process->synchro_with_datastore ();
	}
    }

    void Application_impl::synchro_processing ()
    {
      // reset condition wait flags
      m_init_condition_flag = false;
      m_stop_condition_flag = false;

      // if status Initialising, RunRequest, Stopping restart the activity
      CdmwPlatformMngt::ApplicationStatus appStatus =
	m_stateMachine->getState ()->status ();

      // reset the process status according to the application status restarting
      // if application is in "initialising", processes must be set to "initial"
      // to be able to go to "initialised"
      // if application is in "run_request", processes must be set to "initialised"
      // to be able to go to "running"
      // if application is in "stopping", processes must be set to "running"
      // to be able to go to "stopping"

      // for each process of application
      ProcessServants::iterator processIt;

      for (processIt = m_process_servants.begin ();
	   processIt != m_process_servants.end (); processIt++)
	{
	  Process_impl *process = processIt->second;

	  if (appStatus == CdmwPlatformMngt::APPLICATION_INITIALISING)
	    {
	      // synchro the process status
	      process->synchro_processing_to_initial ();
	    }
	  else if (appStatus == CdmwPlatformMngt::APPLICATION_RUN_REQUEST)
	    {
	      // synchro the process status
	      process->synchro_processing_to_initialised ();
	    }
	  else if (appStatus == CdmwPlatformMngt::APPLICATION_STOPPING)
	    {
	      // synchro the process status
	      process->synchro_processing_to_running ();
	    }
	}

      // if application is in run request force it to initialised
      // to restart the runset by using initialisation notification
      // (all processes are in initialised status)
      if (appStatus == CdmwPlatformMngt::APPLICATION_RUN_REQUEST)
	{
	  std::
	    cout <<
	    "<<Application_impl::synchro_processing() appli run_request >> "
	    << m_element_name.in () << std::endl;

	  appStatus = CdmwPlatformMngt::APPLICATION_INITIALISED;
	  m_stateMachine->setStateName ("Initialised");
	}

      try
      {
	if (appStatus == CdmwPlatformMngt::APPLICATION_INITIALISING ||
	    appStatus == CdmwPlatformMngt::APPLICATION_STOPPING)
	  {
	    // first lock mutex for condition
	    CDMW_MUTEX_GUARD (m_mutex);

	    // restart the activity of the application
	    std::
	      cout <<
	      "<<Application_impl::synchro_processing() restart activity >> "
	      << m_element_name.in () << std::endl;

	    m_stateMachine->restartStateActivity ();


	    // if initialisation is in progress
	    if (appStatus == CdmwPlatformMngt::APPLICATION_INITIALISING)
	      {
		std::
		  cout <<
		  "<<Application_impl::synchro_processing() appli initialising >> "
		  << m_element_name.in () << std::endl;

		// so wait the condition flag is set by broadcasting
		while (!m_init_condition_flag)
		  {
		    m_initialisation_condition.wait ();
		  }

		// reset condition wait flag
		m_init_condition_flag = false;
	      }

	    // if stopping is in progress
	    else
	      {
		std::
		  cout <<
		  "<<Application_impl::synchro_processing() appli stopping >> "
		  << m_element_name.in () << std::endl;

		// so wait the condition flag is set by broadcasting
		while (!m_stop_condition_flag)
		  {
		    m_stop_condition.wait ();
		  }

		// reset condition wait flag
		m_stop_condition_flag = false;
	      }

	    std::
	      cout <<
	      "<<Application_impl::synchro_processing() update datastore >> "
	      << m_element_name.in () << std::endl;


	    // update data store of the object
	    set_record_in_datastore ();

	    std::
	      cout <<
	      "<<Application_impl::synchro_processing() restart activity done >> "
	      << m_element_name.in () << std::endl;

	  }
	else if (appStatus == CdmwPlatformMngt::APPLICATION_INITIALISED ||
		 appStatus == CdmwPlatformMngt::APPLICATION_RUNNING)
	  {
	    std::
	      cout <<
	      "<<Application_impl::synchro_processing() notify initialisation >> "
	      << m_element_name.in () << std::endl;


	    // Notify the initialisation event to the application observer
	    // to restart the runset
	    // If appli is running the runset has no effect on the application itself
	    (m_applicationStartData->application_observer)->
	      notify_initialisation (m_element_name.in ());

	    std::
	      cout <<
	      "<<Application_impl::synchro_processing() update datastore >> "
	      << m_element_name.in () << std::endl;

	    // update data store of the object
	    set_record_in_datastore ();

	    std::
	      cout <<
	      "<<Application_impl::synchro_processing() notify initialisation done >> "
	      << m_element_name.in () << std::endl;
	  }
      }
      catch (...)
      {
	std::
	  cout <<
	  "<<Application_impl::synchro_processing() restart activity exception >> "
	  << m_element_name.in () << std::endl;
	// do nothing
      }
    }


    void Application_impl::synchro_processing_of_processes ()
    {
      ProcessServants::iterator processIt;

      for (processIt = m_process_servants.begin ();
	   processIt != m_process_servants.end (); processIt++)
	{
	  Process_impl *process = processIt->second;

	  // synchro the process with its datastore
	  process->synchro_processing ();
	}
    }


    void Application_impl::createStates () throw (OutOfMemoryException)
    {
      try
      {
	std::auto_ptr < ApplicationInitial >
	  initial (new ApplicationInitial ());
	std::auto_ptr < ApplicationInitialising >
	  initialising (new ApplicationInitialising ());
	std::auto_ptr < ApplicationInitialisationSuspended >
	  initialisationSuspended (new ApplicationInitialisationSuspended ());
	std::auto_ptr < ApplicationInitialised >
	  initialised (new ApplicationInitialised ());
	std::auto_ptr < ApplicationRunRequest >
	  runRequest (new ApplicationRunRequest ());
	std::auto_ptr < ApplicationRunning >
	  running (new ApplicationRunning ());
	std::auto_ptr < ApplicationStopping >
	  stopping (new ApplicationStopping ());
	std::auto_ptr < ApplicationStopped >
	  stopped (new ApplicationStopped ());
	std::auto_ptr < ApplicationInitialisationIncomplete >
	  initialisationIncomplete (new
				    ApplicationInitialisationIncomplete ());

	if (M_initial == NULL)
	  M_initial = initial.get ();
	if (M_initialising == NULL)
	  M_initialising = initialising.get ();
	if (M_initialisationSuspended == NULL)
	  M_initialisationSuspended = initialisationSuspended.get ();
	if (M_initialised == NULL)
	  M_initialised = initialised.get ();
	if (M_runRequest == NULL)
	  M_runRequest = runRequest.get ();
	if (M_running == NULL)
	  M_running = running.get ();
	if (M_stopping == NULL)
	  M_stopping = stopping.get ();
	if (M_stopped == NULL)
	  M_stopped = stopped.get ();
	if (M_initialisationIncomplete == NULL)
	  M_initialisationIncomplete = initialisationIncomplete.get ();

	// Create the processes states
	ManagedProcess_impl::createStates ();
	UnmanagedProcess_impl::createStates ();

	// Release the ownership of the temporary objects
	initial.release ();
	initialising.release ();
	initialisationSuspended.release ();
	initialised.release ();
	runRequest.release ();
	running.release ();
	stopping.release ();
	stopped.release ();
	initialisationIncomplete.release ();

      }
      catch (const std::bad_alloc &)
      {
	CDMW_THROW (OutOfMemoryException);
      }
    }

    void Application_impl::destroyStates ()
    {
      if (M_initial != NULL)
	{
	  delete M_initial;
	  M_initial = NULL;
	}

      if (M_initialising != NULL)
	{
	  delete M_initialising;
	  M_initialising = NULL;
	}

      if (M_initialisationSuspended != NULL)
	{
	  delete M_initialisationSuspended;
	  M_initialisationSuspended = NULL;
	}

      if (M_initialised != NULL)
	{
	  delete M_initialised;
	  M_initialised = NULL;
	}

      if (M_runRequest != NULL)
	{
	  delete M_runRequest;
	  M_runRequest = NULL;
	}

      if (M_running != NULL)
	{
	  delete M_running;
	  M_running = NULL;
	}

      if (M_stopping != NULL)
	{
	  delete M_stopping;
	  M_stopping = NULL;
	}

      if (M_stopped != NULL)
	{
	  delete M_stopped;
	  M_stopped = NULL;
	}

      if (M_initialisationIncomplete != NULL)
	{
	  delete M_initialisationIncomplete;
	  M_initialisationIncomplete = NULL;
	}

      // Destroy the processes states
      ManagedProcess_impl::destroyStates ();
      UnmanagedProcess_impl::destroyStates ();
    }

// TODO: - Activer une seule fois ProcessObserver

    CdmwPlatformMngt::ApplicationAgent_ptr Application_impl::
      getAgent (const char *hostName) throw (CdmwPlatformMngt::HostNotFound,
					     CdmwPlatformMngt::
					     HostNotReachable,
					     CdmwPlatformMngt::CreationError,
					     CdmwPlatformMngt::
					     ApplicationAgentAlreadyExists,
					     CdmwPlatformMngt::InvalidInfo,
					     CdmwPlatformMngt::
					     CreationTimeout,
					     AgentNotFoundException,
					     CORBA::SystemException)
    {
      CdmwPlatformMngt::ApplicationAgent_var applicationAgent =
	CdmwPlatformMngt::ApplicationAgent::_nil ();

      try
      {
	CdmwPlatformMngt::Daemon_var daemon;

	// try to find host name in host set for the application
	bool agentFound = false;
	std::string hostName_str = hostName;

	{
	  CDMW_READER_LOCK_GUARD (m_rwAgentHosts_lock);

	  AgentHosts::iterator hostSetIt = m_agentHosts.find (hostName_str);

	  if (hostSetIt != m_agentHosts.end ())
	    {
	      // agent has been already created in map
	      agentFound = true;

	      // get the daemon
	      daemon = (hostSetIt->second)->daemonObj;
	    }
	}

	// If application agent not found in map, search or create a new one
	if (!agentFound)
	  {
	    // protect with the WRITER lock
	    CDMW_WRITER_LOCK_GUARD (m_rwAgentHosts_lock);

	    // check again if agent has not been created by another thread
	    // in the meantime
	    AgentHosts::iterator hostSetIt = m_agentHosts.find (hostName_str);

	    if (hostSetIt != m_agentHosts.end ())
	      {
		// the agent has been already created by another thread in map
		agentFound = true;

		// get the daemon
		daemon = (hostSetIt->second)->daemonObj;
	      }
	    else
	      {
		// Get the daemon running on the specified host
		daemon =
		  Configuration::Get_daemon (hostName,
					     m_applicationStartData->
					     system_port);

		// first get the agent from daemon if already exist
		applicationAgent =
		  daemon->search_application_agent (m_element_name.in ());

		// if agent does not exist for the host
		if (CORBA::is_nil (applicationAgent.in ()))
		  {
		    // create a new one
		    CdmwPlatformMngt::ApplicationAgentInfo_var agentInfo
		      = new CdmwPlatformMngt::ApplicationAgentInfo ();
		    agentInfo->application_name = m_element_name.in ();

#ifdef CDMW_USE_FAULTTOLERANCE
		    agentInfo->replication_manager_iogr =
		      CORBA::string_dup (m_replicationManagerIOGR.c_str ());

		    agentInfo->process_observer =
		      CdmwPlatformMngt::ProcessObserver::
		      _duplicate (m_syst_processObserver.in ());
		    agentInfo->service_broker =
		      CdmwPlatformMngtService::ServiceBroker::
		      _duplicate (m_syst_serviceBroker.in ());
#else
		    agentInfo->process_observer = m_processObserver->_this ();
		    agentInfo->service_broker = m_serviceBroker->_this ();
#endif

		    // request to daemon to create the agent
		    applicationAgent =
		      daemon->create_application_agent (agentInfo.in ());
		  }

		// store host name with daemon in map
		std::auto_ptr < HostData > hostData (new HostData);

		(hostData.get ())->daemonObj = daemon;

		m_agentHosts[hostName_str] = hostData.get ();
		hostData.release ();
	      }
	  }

	// if host name has been found in map, application agent should exist        
	if (agentFound)
	  {
	    // Get the application agent from the daemon
	    applicationAgent =
	      daemon->search_application_agent (m_element_name.in ());

	    if (CORBA::is_nil (applicationAgent.in ()))
	      {
		// If application agent not found by daemon, throw exception
		CDMW_THROW (AgentNotFoundException);
	      }
	  }
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }

      return applicationAgent._retn ();
    }

    CdmwPlatformMngt::ApplicationAgent_ptr Application_impl::
      searchAgentFromDaemon (const char *hostName) throw (CdmwPlatformMngt::
							  HostNotFound,
							  CdmwPlatformMngt::
							  HostNotReachable,
							  CORBA::
							  SystemException)
    {
      CdmwPlatformMngt::ApplicationAgent_var applicationAgent =
	CdmwPlatformMngt::ApplicationAgent::_nil ();

      try
      {
	// Get the daemon running on the specified host
	CdmwPlatformMngt::Daemon_var daemon =
	  Configuration::Get_daemon (hostName,
				     m_applicationStartData->system_port);

	// Get the application agent from the daemon
	applicationAgent =
	  daemon->search_application_agent (m_element_name.in ());
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }

      return applicationAgent._retn ();
    }


    void Application_impl::killAllAgents ()
    {
      // Kill all agents create by daemons for application
      {
	CDMW_WRITER_LOCK_GUARD (m_rwAgentHosts_lock);

	bool errorSet = true;


	AgentHosts::iterator hostSetIt;

	for (hostSetIt = m_agentHosts.begin ();
	     hostSetIt != m_agentHosts.end (); hostSetIt++)
	  {
	    try
	    {
	      LogMngr::logMessage (INF,
				   MSG_ID_APP_START_KILL_AGENT_HOST,
				   m_element_name.in (),
				   (hostSetIt->first).c_str ());

	      // Get the daemon running on the specified host                
	      CdmwPlatformMngt::Daemon_var daemon =
		(hostSetIt->second)->daemonObj;

	      // request to daemon to kill agent
	      daemon->kill_application_agent (m_element_name.in ());

	      // kill is ok
	      errorSet = false;
	    }
	    catch (const CdmwPlatformMngt::HostNotFound & e)
	    {
	      // nothing to do
	    }
	    catch (const CdmwPlatformMngt::HostNotReachable & e)
	    {
	      // nothing to do
	    }
	    catch (const CdmwPlatformMngt::ApplicationAgentNotFound & e)
	    {
	      // nothing to do
	    }
	    catch (...)
	    {
	      // nothing to do
	    }

	    long messageId;

	    if (errorSet)
	      {
		messageId = MSG_ID_APP_KILL_AGENT_FAIL;
	      }
	    else
	      {
		messageId = MSG_ID_APP_KILL_AGENT_OK;
	      }

	    LogMngr::logMessage (INF,
				 messageId,
				 m_element_name.in (),
				 (hostSetIt->first).c_str ());
	  }

	// clear the map
	for (hostSetIt = m_agentHosts.begin ();
	     hostSetIt != m_agentHosts.end (); hostSetIt++)
	  {
	    HostData *p_hostData = hostSetIt->second;
	    delete p_hostData;
	  }

	m_agentHosts.clear ();
      }
    }

    void Application_impl::invalidateProcesses (CdmwPlatformMngtBase::
						EventHeader header,
						const char *hostName,
						InvalidationType reason)
    {
      std::string faultyHostName = hostName;

      // remove invalidated host from application hosts set
      {
	CDMW_WRITER_LOCK_GUARD (m_rwAgentHosts_lock);

	AgentHosts::iterator hostSetIt = m_agentHosts.find (faultyHostName);

	if (hostSetIt != m_agentHosts.end ())
	  {
	    HostData *p_hostData = hostSetIt->second;
	    delete p_hostData;
	    m_agentHosts.erase (hostSetIt);
	  }
      }

      CDMW_READER_LOCK_GUARD (m_rwApplicationLock);

      // Invalidate all the activ processes
      ProcessServants::iterator processIt;

      for (processIt = m_process_servants.begin ();
	   processIt != m_process_servants.end (); processIt++)
	{
	  Process_impl *process = processIt->second;

	  const char *processHostName = process->get_process_host_name ();

	/***
        CORBA::String_var statusInfo;
        CdmwPlatformMngt::ProcessStatus status =
              process->get_status (statusInfo.out());
        ***/

	  CdmwPlatformMngt::ProcessStatus status
	    = process->get_internal_status ();

	  if ((faultyHostName == processHostName) &&
	      ((status == CdmwPlatformMngt::PROCESS_INITIALISING) ||
	       (status == CdmwPlatformMngt::PROCESS_STEP_PERFORMED) ||
	       (status == CdmwPlatformMngt::PROCESS_INITIALISED) ||
	       (status == CdmwPlatformMngt::PROCESS_RUN_REQUEST) ||
	       (status == CdmwPlatformMngt::PROCESS_RUNNING) ||
	       (status == CdmwPlatformMngt::PROCESS_STOPPING)))
	    {
	      switch (reason)
		{
		case HOST_UNAVAILABLE:
		  process->invalidate_event (header,
					     LogMngr::
					     getFormattedMessage
					     (MSG_ID_HOST_CRASHED,
					      hostName).c_str ());
		  break;
		}

	      // Enter the degraded mode
	      degradation_event ();
	    }
	}
    }


    void Application_impl::notifyModeChangeEvent (CdmwPlatformMngt::
						  FunctioningMode
						  functioning_mode,
						  const char *mode_info)
    {
      try
      {
	CdmwPlatformMngtBase::EventHeader header
	  = EventHeaderFactory::createHeader (CdmwPlatformMngtBase::INF);

	CdmwPlatformMngt::ApplicationModeChange_var event
	  = m_appModeChangeFactory->create (header,
					    m_system_name.in (),
					    m_element_name.in (),
					    functioning_mode, mode_info);

	CORBA::String_var eventStr = event->to_string ();
	LogMngr::logMessage (eventStr.in ());

	m_eventNotifier->addEvent (event);
	event._retn ();
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
      catch (...)
      {
	throw CORBA::INTERNAL (OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
      }
    }

    void Application_impl::notifyStatusChangeEvent ()
    {
      try
      {
	CdmwPlatformMngtBase::EventHeader header
	  = EventHeaderFactory::createHeader (CdmwPlatformMngtBase::INF);

	CdmwPlatformMngt::ApplicationStatusChange_var event
	  = m_appStatusChangeFactory->create (header,
					      m_system_name.in (),
					      m_element_name.in (),
					      m_stateMachine->getState ()->
					      status (), m_statusInfo.in ());

	CORBA::String_var eventStr = event->to_string ();
	LogMngr::logMessage (eventStr.in ());

	m_eventNotifier->addEvent (event);
	event._retn ();
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
      catch (...)
      {
	throw CORBA::INTERNAL (OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
      }
    }

    void Application_impl::
      notifyEntityStatusChangeEvent (const char *entity_name,
				     const char *entity_status,
				     const char *entity_info)
    {
      try
      {
	CdmwPlatformMngtBase::EventHeader header
	  = EventHeaderFactory::createHeader (CdmwPlatformMngtBase::INF);

	CdmwPlatformMngt::ApplicationEntityStatusChange_var event
	  = m_appEntityStatusChangeFactory->create (header,
						    m_system_name.in (),
						    m_element_name.in (),
						    entity_name,
						    entity_status,
						    entity_info);

	CORBA::String_var eventStr = event->to_string ();
	LogMngr::logMessage (eventStr.in ());

	m_eventNotifier->addEvent (event);
	event._retn ();
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
      catch (...)
      {
	throw CORBA::INTERNAL (OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
      }
    }

    void Application_impl::setInitialisationStatus (CdmwPlatformMngt::
						    LongRequestStatus status)
    {
      m_mutex.lock ();

      // set condition wait flag
      m_init_condition_flag = true;

      m_initialisation_status = status;
      m_initialisation_condition.broadcast ();
      m_mutex.unlock ();
    }


    void Application_impl::setStopStatus (CdmwPlatformMngt::
					  LongRequestStatus status)
    {
      m_mutex.lock ();

      // set condition wait flag
      m_stop_condition_flag = true;

      m_stop_status = status;
      m_stop_condition.broadcast ();
      m_mutex.unlock ();
    }

    void Application_impl::setInitGraph (const CdmwPlatformMngt::
					 GraphElements & process_graph)
      throw (OutOfMemoryException, CdmwPlatformMngt::ReferenceNotFound,
	     CdmwPlatformMngt::CircularReference,
	     CdmwPlatformMngt::StepOutOfBound, CdmwPlatformMngt::StepJump,
	     CORBA::SystemException)
    {
      try
      {
	CDMW_WRITER_LOCK_GUARD (m_rwApplicationLock);

	// Build the map of exisiting elements
	ExistingElementMap existingElements;
	ProcessServants::iterator processIt;

	for (processIt = m_process_servants.begin ();
	     processIt != m_process_servants.end (); processIt++)
	  {
	    Process_impl *process = processIt->second;
	    unsigned long initStepCount = 1;

	    if (process->type () != CdmwPlatformMngt::UNMANAGED_PROCESS)
	      {
		CdmwPlatformMngt::LifeCycleDef_var lifeCycleDef =
		  process->get_life_cycle ();

		initStepCount = lifeCycleDef->init_steps.length ();
	      }

	    existingElements.
	      insert (PlatformMngt::ExistingElementMap::
		      value_type (process->get_process_key (),
				  initStepCount));

	    // if process name is unique, insert it as existing element
	    // (host cannot be specified) 
	    if (checkNbrOfProcessNames (process->get_element_name ()) == 1)
	      {
		existingElements.
		  insert (PlatformMngt::ExistingElementMap::
			  value_type (process->get_element_name (),
				      initStepCount));
	      }
	  }

	// Check the validity of the graph of processes
	GraphChecker processGraph (process_graph, &existingElements, false);

	// Actually sets the graph of processes to this application
	m_processInitGraph =
	  new CdmwPlatformMngt::GraphElements (process_graph);

	// set flag new graph
	m_newInitGraph = true;

	// TODO: Notify the configuration change

	// TODO: Trace to the log manager

	// update record in datastore
	set_record_in_datastore ();
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
    }

    void Application_impl::setStopGraph (const CdmwPlatformMngt::
					 GraphElements & process_graph)
      throw (OutOfMemoryException, CdmwPlatformMngt::ReferenceNotFound,
	     CdmwPlatformMngt::CircularReference,
	     CdmwPlatformMngt::StepOutOfBound, CdmwPlatformMngt::StepJump,
	     CORBA::SystemException)
    {
      try
      {
	CDMW_WRITER_LOCK_GUARD (m_rwApplicationLock);

	// Build the map of existing elements
	ExistingElementMap existingElements;
	ProcessServants::iterator processIt;

	for (processIt = m_process_servants.begin ();
	     processIt != m_process_servants.end (); processIt++)
	  {
	    Process_impl *p_process = processIt->second;

	    existingElements.
	      insert (PlatformMngt::ExistingElementMap::
		      value_type (p_process->get_process_key (), 0));

	    // if process name is unique, insert it as existing element
	    // (host cannot be specified) 
	    if (checkNbrOfProcessNames (p_process->get_element_name ()) == 1)
	      {
		existingElements.
		  insert (PlatformMngt::ExistingElementMap::
			  value_type (p_process->get_element_name (), 0));
	      }
	  }

	// Check the validity of the graph of processes
	GraphChecker processGraph (process_graph, &existingElements, true);

	// Actually sets the graph of processes to this application
	m_processStopGraph =
	  new CdmwPlatformMngt::GraphElements (process_graph);

	// TODO: Notify the configuration change

	// TODO: Trace to the log manager

	// update record in datastore
	set_record_in_datastore ();
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
    }

    int Application_impl::checkNbrOfProcessNames (const char *process_name)
    {
      int nbr_of_process = 0;

      ProcessServants::iterator processIt;

      for (processIt = m_process_servants.begin ();
	   processIt != m_process_servants.end (); processIt++)
	{
	  Process_impl *process = processIt->second;

	  if (!strcmp (process_name, process->get_element_name ()))
	    {
	      nbr_of_process++;
	    }
	}

      return nbr_of_process;
    }

    Process_impl *Application_impl::getProcessHostName (const std::
							string & process_name,
							std::
							string & host_name)
      throw (ProcessNameNotUnique, CdmwPlatformMngt::ProcessNotFound)
    {
      int nbr_of_process = 0;
      Process_impl *process = NULL;

      const char *p_process_name = process_name.c_str ();

      host_name = "";

      CDMW_READER_LOCK_GUARD (m_rwApplicationLock);

      ProcessServants::iterator processIt;

      for (processIt = m_process_servants.begin ();
	   processIt != m_process_servants.end (); processIt++)
	{
	  if (!strcmp
	      (p_process_name, processIt->second->get_element_name ()))
	    {
	      // if first process found
	      if (nbr_of_process == 0)
		{
		  process = processIt->second;
		  nbr_of_process++;
		}
	      else
		{
		  // throw exception if more than one process is found
		  throw ProcessNameNotUnique ();
		}
	    }
	}

      // if process not found
      if (nbr_of_process == 0)
	{
	  throw CdmwPlatformMngt::ProcessNotFound ();
	}

      host_name = process->get_process_host_name ();

      return process;
    }


    void Application_impl::defineApplication (const CdmwPlatformMngt::
					      ApplicationDef &
					      application_def)
      throw (CdmwPlatformMngt::DuplicateEntity,
	     CdmwPlatformMngt::DuplicateService,
	     CdmwPlatformMngt::DuplicateProcess,
	     CdmwPlatformMngt::ProcessHostNotFound,
	     CdmwPlatformMngt::DuplicateProcessEntity,
	     CdmwPlatformMngt::DuplicateProcessService,
	     CdmwPlatformMngt::DuplicateProcessStep,
	     CdmwPlatformMngt::AlreadyDone,
	     CdmwPlatformMngt::IncompatibleStatus,
	     CosPropertyService::MultipleExceptions, CORBA::SystemException)
    {
      {
	CDMW_READER_LOCK_GUARD (m_rwApplicationLock);

	if (m_applicationDefined)
	  throw CdmwPlatformMngt::AlreadyDone ();
      }

      // Check the validity of all the process definitions
      // regarding the context of this application
      unsigned int processCount = application_def.processes.length ();

      for (unsigned int processIndex = 0;
	   processIndex < processCount; processIndex++)
	{
	  CdmwPlatformMngt::ProcessDef_var process_def
	    = application_def.processes[processIndex];

	  const char *p_host_name = process_def->host_name ();
	  const char *p_process_name = process_def->name ();

	  try
	  {
	    // Check the host is known from the system
	    CdmwPlatformMngt::Host_var host =
	      m_hostContainer->get_host (p_host_name);

	    // if process is managed
	    if (process_def->type () != CdmwPlatformMngt::UNMANAGED_PROCESS)
	      {
		CdmwPlatformMngt::ManagedProcessDef * managed_process_def
		  =
		  CdmwPlatformMngt::ManagedProcessDef::
		  _downcast (process_def);

		// Check there is no duplicate entity
		CdmwPlatformMngt::EntityDefs & entities =
		  managed_process_def->entities ();

		for (unsigned int entityIndex = 0;
		     entityIndex < entities.length (); entityIndex++)
		  {
		    std::string entityName =
		      entities[entityIndex].entity_name.in ();

		    for (unsigned int entityIndex2 = entityIndex + 1;
			 entityIndex2 < entities.length (); entityIndex2++)
		      {
			std::string entityName2 =
			  entities[entityIndex2].entity_name.in ();

			if (entityName == entityName2)
			  throw CdmwPlatformMngt::
			    DuplicateProcessEntity (p_process_name,
						    p_host_name,
						    entityName.c_str ());
		      }
		  }

		// Check there is no duplicate service
		CdmwPlatformMngtService::ServiceDefs & services
		  = managed_process_def->service_definitions ();

		for (unsigned int serviceIndex = 0;
		     serviceIndex < services.length (); serviceIndex++)
		  {
		    std::string serviceName
		      = services[serviceIndex].service_name.in ();

		    for (unsigned int serviceIndex2 = serviceIndex + 1;
			 serviceIndex2 < services.length (); serviceIndex2++)
		      {
			std::string serviceName2
			  = services[serviceIndex2].service_name.in ();

			if (serviceName == serviceName2)
			  throw CdmwPlatformMngt::
			    DuplicateProcessService (p_process_name,
						     p_host_name,
						     serviceName.c_str ());
		      }
		  }

		// Check there is no duplicate step
		CdmwPlatformMngt::Steps & initSteps
		  = managed_process_def->life_cycle ().init_steps;

		unsigned int initStepCount = initSteps.length ();
		for (unsigned int initStepIndex = 0;
		     initStepIndex < initStepCount; initStepIndex++)
		  {
		    std::string initStepName
		      = initSteps[initStepIndex].step_label.in ();

		    for (unsigned int initStepIndex2 = initStepIndex + 1;
			 initStepIndex2 < initStepCount; initStepIndex2++)
		      {
			std::string initStepName2
			  = initSteps[initStepIndex2].step_label.in ();

			if (initStepName == initStepName2)
			  throw CdmwPlatformMngt::
			    DuplicateProcessStep (p_process_name, p_host_name,
						  initStepName.c_str ());
		      }
		  }
	      }
	  }
	  catch (const CdmwPlatformMngt::HostNotFound &)
	  {
	    throw CdmwPlatformMngt::ProcessHostNotFound (p_process_name,
							 p_host_name);
	  }
	}


      // All has been checked

      // Add all the processes
      for (unsigned int processIndex = 0;
	   processIndex < processCount; processIndex++)
	{
	  // get process definition (ProcessDef is a value type, a var
	  // is returned by the [] operator, a copy is made in a new var)         
	  CdmwPlatformMngt::ProcessDef_var process_def =
	    application_def.processes[processIndex];

	  const char *p_host_name = process_def->host_name ();
	  const char *p_process_name = process_def->name ();

	  try
	  {
	    CdmwPlatformMngt::Process_var process
	      = addProcess (process_def.in ());
	  }
	  catch (const CdmwPlatformMngt::HostNotFound &)
	  {
	    throw CdmwPlatformMngt::ProcessHostNotFound (p_process_name,
							 p_host_name);
	  }
	  catch (const CdmwPlatformMngt::ProcessAlreadyExists &)
	  {
	    throw CdmwPlatformMngt::DuplicateProcess (p_process_name,
						      p_host_name);
	  }
	}

      // Add all the entities
      unsigned int entityCount = application_def.entities.length ();


      // Store entity initial values into default_entities
      m_default_entitydefs = new CdmwPlatformMngt::EntityDefs ();
      m_default_entitydefs->length (entityCount);

      for (unsigned int entityIndex = 0;
	   entityIndex < entityCount; entityIndex++)
	{
	  // get entity definition (EntityDef is a structure, a reference on the
	  // structure is returned by the [] operator)
	  const CdmwPlatformMngt::EntityDef & entity_def =
	    application_def.entities[entityIndex];

	  // store values of the entityDef
	  (*m_default_entitydefs)[entityIndex] = entity_def;

	  try
	  {
	    m_rwEntityContainer->add_entity (entity_def);
	  }
	  catch (const CdmwPlatformMngt::EntityAlreadyExists &)
	  {
	    throw CdmwPlatformMngt::DuplicateEntity (entity_def.entity_name.
						     in ());
	  }
	}

      // Add all the services

      unsigned int serviceCount =
	application_def.service_definitions.length ();

      for (unsigned int serviceIndex = 0;
	   serviceIndex < serviceCount; serviceIndex++)
	{
	  // get service definition (ServiceDef is a structure, a reference on the
	  // structure is returned by the [] operator)
	  const CdmwPlatformMngtService::ServiceDef & service_def =
	    application_def.service_definitions[serviceIndex];

	  try
	  {
	    m_rwServiceDefContainer->add_service_def (service_def);
	  }
	  catch (const CdmwPlatformMngtService::ServiceAlreadyExists &)
	  {
	    throw CdmwPlatformMngt::DuplicateService (service_def.
						      service_name.in ());
	  }
	}

      // Add Properties to Applicartion
      unsigned int propCount = application_def.properties.length ();

      for (unsigned int propIndex = 0; propIndex < propCount; propIndex++)
	{
	  const CdmwPlatformMngt::PropertyDef & prop_def =
	    application_def.properties[propIndex];

	  try
	  {
	    std::string propertyName (application_def.properties[propIndex].
				      property_name);
	    CosPropertyService::PropertyModeType propertyMode =
	      CosPropertyService::PropertyModeType (0);
	    CdmwPlatformMngt::PropertyModeType propertyModeCdmw =
	      application_def.properties[propIndex].mode_type;
	    if (propertyModeCdmw == 1)
	      propertyMode = CosPropertyService::PropertyModeType (1);
	    define_property_with_mode (propertyName.c_str (),
				       prop_def.initial_value, propertyMode);
	  }
	  catch (const CosPropertyService::InvalidPropertyName &)
	  {
	    throw CosPropertyService::MultipleExceptions ();	/* TO-DO */
	  }
	  catch (const CosPropertyService::ConflictingProperty &)
	  {
	    throw CosPropertyService::MultipleExceptions ();	/* TO-DO */
	  }
	  catch (const CosPropertyService::UnsupportedTypeCode &)
	  {
	    throw CosPropertyService::MultipleExceptions ();	/* TO-DO */
	  }
	  catch (const CosPropertyService::UnsupportedProperty &)
	  {
	    throw CosPropertyService::MultipleExceptions ();	/* TO-DO */
	  }
	  catch (const CosPropertyService::UnsupportedMode &)
	  {
	    throw CosPropertyService::MultipleExceptions ();	/* TO-DO */
	  }
	  catch (const CosPropertyService::ReadOnlyProperty &)
	  {
	    throw CosPropertyService::MultipleExceptions ();	/* TO-DO */
	  }

	}

      // store default properties values
      store_default_properties ();

      m_applicationDefined = true;
    }

    CdmwPlatformMngt::Process_ptr Application_impl::
      addProcess (CdmwPlatformMngt::ProcessDef *
		  process_def) throw (CdmwPlatformMngt::ProcessAlreadyExists,
				      CdmwPlatformMngt::HostNotFound,
				      CdmwPlatformMngt::DuplicateEntity,
				      CdmwPlatformMngt::DuplicateService,
				      CdmwPlatformMngt::DuplicateStep,
				      CosPropertyService::MultipleExceptions,
				      CORBA::SystemException)
    {
      // if host name is "localhost" replace by the real host name
      if (!strcmp (process_def->host_name (), "localhost"))
	{
	  // get host name and set in value type
	  std::string name = OsSupport::OS::get_hostname ();
	  process_def->host_name (name.c_str ());
	}

      const char *p_host_name = process_def->host_name ();
      const char *p_process_name = process_def->name ();

      // build the process relative key
      std::string process_key =
	ManagedElementKey::Get_process_rel_key (p_host_name, p_process_name);

      {
	CDMW_READER_LOCK_GUARD (m_rwApplicationLock);

	ProcessServants::iterator processIt =
	  m_process_servants.find (process_key.c_str ());

	if (processIt != m_process_servants.end ())
	  throw CdmwPlatformMngt::ProcessAlreadyExists ();
      }

      // build the process path
      std::string process_path =
	ManagedElementKey::Get_process_path (m_element_path.in (),
					     p_host_name, p_process_name);

      // Create a process 
      Process_impl *processImpl = NULL;

      // Servant var to take pointer ownership and to release
      // ref counter from 2 to 1 after object activation
      PortableServer::ServantBase_var processServant;

      try
      {
	if (process_def->type () == CdmwPlatformMngt::UNMANAGED_PROCESS)
	  {
	    UnmanagedProcess_impl *unmanagedProcess =
	      new UnmanagedProcess_impl (m_poa.in (), process_def,
					 process_key.c_str (),
					 process_path.c_str (),
					 this,
					 m_element_repository.in ());

	    // take pointer ownership
	    processServant = unmanagedProcess;

	    // set the end creation for process
	    // in case of exception the servant will be deleted automatically
	    unmanagedProcess->end_creation (process_def);

	    // Add it to the map of process 
	    CDMW_WRITER_LOCK_GUARD (m_rwApplicationLock);

	    m_process_servants.
	      insert (ProcessServants::
		      value_type (process_key.c_str (), unmanagedProcess));
	    processImpl = unmanagedProcess;
	  }
	else
	  {
	    CdmwPlatformMngt::ManagedProcessDef * managed_process_def
	      = CdmwPlatformMngt::ManagedProcessDef::_downcast (process_def);


	    ManagedProcess_impl *managedProcess =
	      new ManagedProcess_impl (m_poa.in (), managed_process_def,
				       process_key.c_str (),
				       process_path.c_str (),
				       this,
				       m_element_repository.in ());

	    // take pointer ownership
	    processServant = managedProcess;

	    // set the end creation for process
	    // in case of exception the servant will be deleted automatically
	    managedProcess->end_creation (managed_process_def);

	    // Add it to the map of process 
	    CDMW_WRITER_LOCK_GUARD (m_rwApplicationLock);

	    m_process_servants.
	      insert (ProcessServants::
		      value_type (process_key.c_str (), managedProcess));

	    processImpl = managedProcess;
	  }
      }
      catch (const OutOfMemoryException &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }

      // activate process servant
      processImpl->activate ();
      CdmwPlatformMngt::Process_var process = processImpl->_this ();

      try
      {
	// add in repository the process object
	m_element_repository->add_element (process_path.c_str (),
					   CdmwPlatformMngt::PROCESS_TYPE,
					   process.in ());
      }
      catch (const CdmwPlatformMngt::ElementPathInvalid)
      {
	throw CORBA::INTERNAL (OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
      }
      catch (const CdmwPlatformMngt::ElementAlreadyExists)
      {
	throw CdmwPlatformMngt::ProcessAlreadyExists ();
      }

      return process._retn ();
    }

    void Application_impl::removeProcess (const char *process_key)
      throw (CdmwPlatformMngt::ProcessNotFound,
	     CdmwPlatformMngt::ProcessStillActive, CORBA::SystemException)
    {
      try
      {
	CDMW_WRITER_LOCK_GUARD (m_rwApplicationLock);

	ProcessServants::iterator processIt =
	  m_process_servants.find (process_key);
	if (processIt == m_process_servants.end ())
	  throw CdmwPlatformMngt::ProcessNotFound ();

	Process_impl *p_process_impl = processIt->second;

	CORBA::String_var statusInfo;
	CdmwPlatformMngt::ProcessStatus status
	  = p_process_impl->get_status (statusInfo.out ());

	if ((status == CdmwPlatformMngt::PROCESS_INITIALISING) ||
	    (status == CdmwPlatformMngt::PROCESS_STEP_PERFORMED) ||
	    (status == CdmwPlatformMngt::PROCESS_INITIALISED) ||
	    (status == CdmwPlatformMngt::PROCESS_RUN_REQUEST) ||
	    (status == CdmwPlatformMngt::PROCESS_RUNNING) ||
	    (status == CdmwPlatformMngt::PROCESS_STOPPING))
	  throw CdmwPlatformMngt::ProcessStillActive ();

	// suppress process servant from map
	m_process_servants.erase (processIt);

	// remove from repository the process object
	m_element_repository->remove_element (p_process_impl->
					      get_element_path ());

	// deactivate process servant
	p_process_impl->deactivate ();
      }
      catch (const CdmwPlatformMngt::ElementPathInvalid)
      {
	throw CORBA::INTERNAL (OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
      }
      catch (const CdmwPlatformMngt::ManagedElementNotFound)
      {
	throw CdmwPlatformMngt::ProcessNotFound ();
      }
    }

    void Application_impl::removeProcesses ()
    {
      // deactivate process object referenced in map
      ProcessServants::iterator processIt;

      for (processIt = m_process_servants.begin ();
	   processIt != m_process_servants.end (); processIt++)
	{
	  Process_impl *process = processIt->second;

	  try
	  {
	    // remove from repository the process object
	    m_element_repository->remove_element (process->
						  get_element_path ());


	    // deactivate process servant
	    process->deactivate ();
	  }
	  catch (...)
	  {
	  }
	}

      m_process_servants.clear ();
    }

    Process_impl *Application_impl::getProcess (const char *process_key)
      throw (CdmwPlatformMngt::ProcessNotFound,
	     CdmwPlatformMngt::ElementPathInvalid, CORBA::SystemException)
    {
      Process_impl *process = 0;

      try
      {
	// check that process key is complete (host defined)

	std::string process_name;
	std::string host_name;

	// if host name undefined in the process key
	if (ManagedElementKey::
	    Check_process_rel_key (process_key, process_name))
	  {
	    try
	    {
	      // search the host name from the process name if it is unique
	      process = getProcessHostName (process_name, host_name);
	    }
	    catch (ProcessNameNotUnique &)
	    {
	      throw CdmwPlatformMngt::ElementPathInvalid ();
	    }
	  }

	// else the process key is ok, search in map
	else
	  {
	    CDMW_READER_LOCK_GUARD (m_rwApplicationLock);

	    ProcessServants::iterator processIt =
	      m_process_servants.find (process_key);
	    if (processIt == m_process_servants.end ())
	      {
		throw CdmwPlatformMngt::ProcessNotFound ();
	      }

	    process = processIt->second;
	  }
      }
      catch (...)
      {
	throw CdmwPlatformMngt::ProcessNotFound ();
      }

      return process;
    }


    Process_impl *Application_impl::getProcess (const char *process_name,
						const char *host_name)
      throw (CdmwPlatformMngt::ProcessNotFound, CORBA::SystemException)
    {
      Process_impl *process = 0;

      try
      {
	std::string process_key =
	  ManagedElementKey::Get_process_rel_key (host_name, process_name);

	CDMW_READER_LOCK_GUARD (m_rwApplicationLock);

	ProcessServants::iterator processIt =
	  m_process_servants.find (process_key);
	if (processIt == m_process_servants.end ())
	  {
	    throw CdmwPlatformMngt::ProcessNotFound ();
	  }

	process = processIt->second;
      }
      catch (...)
      {
	throw CdmwPlatformMngt::ProcessNotFound ();
      }

      return process;
    }




    CdmwPlatformMngt::ProcessObserver_ptr Application_impl::
      get_process_observer ()throw (CORBA::SystemException)
    {
      return m_processObserver->_this ();
    }

    ProcessObserver_impl *Application_impl::get_process_observer_servant ()
    {
      return m_processObserver;
    }

    CdmwPlatformMngtService::ServiceBroker_ptr Application_impl::
      get_service_broker ()throw (CORBA::SystemException)
    {
      return m_serviceBroker->_this ();
    }


    CdmwPlatformMngt::HostContainer_ptr Application_impl::
      get_host_container ()
    {
      CdmwPlatformMngt::HostContainer_var host_container = m_hostContainer;

      return host_container._retn ();
    }

    CdmwPlatformMngt::ApplicationObserver_ptr Application_impl::
      get_application_observer ()
    {
      CdmwPlatformMngt::ApplicationObserver_var app_observer =
	m_applicationStartData->application_observer;

      return app_observer._retn ();
    }



    RWServiceDefContainer_impl *Application_impl::
      get_rwservice_defcontainer ()
    {
      return m_rwServiceDefContainer;
    }

    SupervisionEventNotifier *Application_impl::get_event_notifier ()
    {
      return m_eventNotifier;
    }


    EventHandler *Application_impl::get_event_handler ()
    {
      return &m_eventHandler;
    }




    void Application_impl::destroyInitSequencer ()
    {
      // Stop and destroy the init sequencer 
      if (m_initSequencer != NULL)
	{
	  m_initSequencer->stop ();

	  delete m_initSequencer;
	  m_initSequencer = NULL;
	}

      if (m_initSyncTaskFactory != NULL)
	{
	  delete m_initSyncTaskFactory;
	  m_initSyncTaskFactory = NULL;
	}
    }

    void Application_impl::destroyStopSequencer ()
    {
      // Stop and destroy the stop sequencer 
      if (m_stopSequencer != NULL)
	{
	  m_stopSequencer->stop ();

	  delete m_stopSequencer;
	  m_stopSequencer = NULL;
	}

      if (m_stopSyncTaskFactory != NULL)
	{
	  delete m_stopSyncTaskFactory;
	  m_stopSyncTaskFactory = NULL;
	}
    }

    void Application_impl::destroyStarter ()
    {
      // Stop and destroy the run starter 
      if (m_starter != NULL)
	{
	  m_starter->stop ();

	  delete m_starter;
	  m_starter = NULL;
	}

      if (m_taskFactory != NULL)
	{
	  delete m_taskFactory;
	  m_taskFactory = NULL;
	}
    }

    void Application_impl::degradation_event ()
    {
      bool changed = false;

      CDMW_MUTEX_GUARD (m_mutexMode);

      if (m_mode == CdmwPlatformMngt::NORMAL_MODE)
	{
	  changed = true;
	}

      if (changed)
	{
	  // Log and notify the change of mode
	  CdmwPlatformMngt::FunctioningMode mode =
	    CdmwPlatformMngt::DEGRADED_MODE;
	  CORBA::String_var mode_info = CORBA::string_dup ("");

	  set_mode (mode, mode_info.in ());

	  notifyModeChangeEvent (mode, mode_info.in ());

	  try
	  {
	    // Notify the degradation event to the application observers
	    (m_applicationStartData->application_observer)->
	      notify_degradation (m_element_name.in ());
	  }
	  catch (const CORBA::Exception & e)
	  {
	    // TODO: What to do with the raised exception ?!!
	    std::string errorName = e._name ();
	    LogMngr::logMessage (INF,
				 (errorName +
				  " exception raised by app observer notification").
				 c_str ());
	  }
	}

    }

    void Application_impl::suspension_event ()
    {
      CDMW_SM_WRITER_LOCK_GUARD (*m_stateMachine);

      m_stateMachine->getState ()->suspension_event (m_stateMachine);

      // update record in datastore
      set_record_in_datastore ();
    }

    void Application_impl::completion_event ()
    {
      CDMW_SM_WRITER_LOCK_GUARD (*m_stateMachine);

      m_stateMachine->getState ()->completion_event (m_stateMachine);

      // update record in datastore
      set_record_in_datastore ();
    }

    void Application_impl::non_completion_event ()
    {
      CDMW_SM_WRITER_LOCK_GUARD (*m_stateMachine);

      m_stateMachine->getState ()->non_completion_event (m_stateMachine);

      // update record in datastore
      set_record_in_datastore ();
    }

    CdmwPlatformMngt::ApplicationStatus Application_impl::
      get_internal_status ()
    {
      CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);

      // get status
      CdmwPlatformMngt::ApplicationStatus applicationStatus =
	m_stateMachine->getState ()->status ();

      return applicationStatus;
    }



    CdmwPlatformMngt::RestartType Application_impl::
      restart_type ()throw (CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }


    CORBA::Long Application_impl::restart_attempt ()throw (CORBA::
							   SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }



    CORBA::Boolean Application_impl::is_manual_starting ()throw (CORBA::
								 SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }



    void Application_impl::set_manual_starting (CORBA::
						Boolean manual_starting)
      throw (CORBA::SystemException, CdmwPlatformMngt::IncompatibleStatus)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }



    CdmwPlatformMngt::ApplicationStatus Application_impl::get_status (CORBA::
								      String_out
								      status_info)
      throw (CORBA::SystemException)
    {
      try
      {
	CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);

	// get record                
	CdmwPlatformMngt::ApplicationRecord_var application_record =
	  get_record_from_datastore ();

	// get status
	CdmwPlatformMngt::ApplicationStatus application_status =
	  application_record->status;

	// get info associated to status
	status_info =
	  CORBA::string_dup ((application_record->status_info).in ());

	return application_status;
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
    }



    CdmwPlatformMngt::FunctioningMode Application_impl::get_mode (CORBA::
								  String_out
								  mode_info)
      throw (CORBA::SystemException)
    {
      try
      {
	// get record
	CdmwPlatformMngt::ApplicationRecord_var application_record =
	  get_record_from_datastore ();

	// get mode
	CdmwPlatformMngt::FunctioningMode functioning_mode =
	  application_record->mode;

	// get info associated to mode
	mode_info = CORBA::string_dup ((application_record->mode_info).in ());

	return functioning_mode;
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
    }


    void Application_impl::set_mode (CdmwPlatformMngt::
				     FunctioningMode functioning_mode,
				     const char *mode_info)
    {
      m_mode = functioning_mode;
      m_modeInfo = CORBA::string_dup (mode_info);

      // update record in datastore
      set_record_in_datastore ();
    }


    CdmwPlatformMngt::ApplicationSnapshot *
      Application_impl::get_snapshot ()throw (CORBA::SystemException)
    {
      try
      {
	CDMW_READER_LOCK_GUARD (m_rwApplicationLock);

	// Get the snapshot of all the processes
	CdmwPlatformMngt::ProcessesSnapshot_var processesSnapshot
	  = new CdmwPlatformMngt::ProcessesSnapshot ();

	ProcessServants::iterator processIt;
	unsigned int i;

	processesSnapshot->length (m_process_servants.size ());
	for (processIt = m_process_servants.begin (), i = 0;
	     processIt != m_process_servants.end (); processIt++, i++)
	  {
	    processesSnapshot[i] = *(processIt->second->get_snapshot ());
	  }

	// Get the snapshot of all entities of application
	CdmwPlatformMngt::EntitiesSnapshot_var entitiesSnapshot
	  = new CdmwPlatformMngt::EntitiesSnapshot ();

	// Gets all the entities
	CdmwPlatformMngt::Entities_var entities;
	CdmwPlatformMngt::EntitiesIterator_var entitiesIt;
	CORBA::ULong nbr_of_entities;

	nbr_of_entities = get_number_of_entities ();

	get_all_entities (nbr_of_entities,
			  entities.out (), entitiesIt.out ());

	entitiesSnapshot->length (nbr_of_entities);
	for (i = 0; i < nbr_of_entities; i++)
	  {
	    entitiesSnapshot[i] = *(entities[i]->get_snapshot ());
	  }

	//Gets all the properties
	CosPropertyService::PropertiesIterator_var propertiesIt;
	CORBA::ULong nbr_of_properties;
	CdmwPlatformMngt::PropertiesSnapshot_var propertiesSnapshot =
	  new CdmwPlatformMngt::PropertiesSnapshot;
	nbr_of_properties = get_number_of_properties ();
	CosPropertyService::PropertyNames_var prop_names;
	CosPropertyService::PropertyNamesIterator_var prop_namesIt;
	get_all_property_names (nbr_of_properties, prop_names.out (),
				prop_namesIt.out ());
	propertiesSnapshot->length (nbr_of_properties);
	for (i = 0; i < nbr_of_properties; i++)
	  {
	    CdmwPlatformMngt::PropertySnapshot * propertySnapshot =
	      new CdmwPlatformMngt::PropertySnapshot ();
	    propertySnapshot->property_name =
	      CORBA::string_dup (prop_names[i]);
	    CORBA::Any prop_value;
	    prop_value = *(get_property_value (prop_names[i]));
	    CORBA::TypeCode_var tc;
	    tc = prop_value.type ();
	    switch (tc->kind ())
	      {
	      default:
		break;
	      case CORBA::tk_string:
		propertySnapshot->property_type =
		  CORBA::string_dup ("String");
		break;
	      case CORBA::tk_short:
		propertySnapshot->property_type = CORBA::string_dup ("Short");
		break;
	      case CORBA::tk_float:
		propertySnapshot->property_type = CORBA::string_dup ("Float");
		break;
	      };
	    CosPropertyService::PropertyModeType mode =
	      get_property_mode (prop_names[i]);
	    propertySnapshot->property_value = prop_value;
	    if (mode == 0)
	      propertySnapshot->property_mode =
		CORBA::string_dup ("Read/Write");
	    else
	      propertySnapshot->property_mode =
		CORBA::string_dup ("Read/Only");
	    propertySnapshot->property_value = prop_value;
	    propertiesSnapshot[i] = *(propertySnapshot);
	  }



	// Create an application snapshot
	CORBA::String_var modeInfo;
	CORBA::String_var statusInfo;

	CdmwPlatformMngt::ApplicationSnapshot_var snapshot
	  = new CdmwPlatformMngt::ApplicationSnapshot;

	// note : the copy of a snapshot sequence (= operator) 
	//        make a deeply copy with a new memory allocation
	//        for the sequence        

	snapshot->application_name = m_element_name.in ();
	snapshot->application_status = get_status (statusInfo.out ());
	snapshot->status_info = statusInfo;
	snapshot->application_mode = get_mode (modeInfo.out ());
	snapshot->mode_info = modeInfo;
	snapshot->application_processes = processesSnapshot.in ();
	snapshot->application_entities = entitiesSnapshot.in ();
	snapshot->application_properties = propertiesSnapshot.in ();

	return snapshot._retn ();
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
    }


    /**
     *Purpose:
     *<p> facade to get an attribute
     *
     */

    void Application_impl::get_attribute (const char *attr_name,
					  CORBA::Any_out ret_attr,
					  CORBA::Char flag) throw (CORBA::
								   SystemException,
								   CdmwPlatformMngt::
								   AttributeNotFound)
    {
      char flag_found = 0;
      std::string tmp1 (attr_name);
      if (flag == 0)
	{			/* case od std attributes */
	  /*active the right methods */
	  if (tmp1 == "status")
	    {
	      flag_found = 1;
	      CdmwPlatformMngt::ApplicationStatus stat;
	      CORBA::String_var act_info;
	      CORBA::Any_var tempany = new CORBA::Any;

	      stat = get_status (act_info);
	      tempany.inout () <<= stat;

	      ret_attr = tempany._retn ();
	    }


	  if (tmp1 == "status_info")
	    {
	      flag_found = 1;
	      CdmwPlatformMngt::ApplicationStatus stat;
	      CORBA::String_var status_info;
	      CORBA::Any_var tempany = new CORBA::Any;

	      stat = get_status (status_info);
	      tempany.inout () <<= status_info;

	      ret_attr = tempany._retn ();
	    }


	  if (tmp1 == "mode")
	    {
	      flag_found = 1;
	      CdmwPlatformMngt::FunctioningMode func_mode;
	      CORBA::String_var mode_info;
	      CORBA::Any_var tempany = new CORBA::Any;

	      func_mode = get_mode (mode_info);
	      tempany.inout () <<= func_mode;

	      ret_attr = tempany._retn ();
	    }

	  if (tmp1 == "mode_info")
	    {
	      flag_found = 1;
	      CdmwPlatformMngt::FunctioningMode func_mode;
	      CORBA::String_var mode_info;
	      CORBA::Any_var tempany = new CORBA::Any;

	      func_mode = get_mode (mode_info);
	      tempany.inout () <<= mode_info;

	      ret_attr = tempany._retn ();
	    }

	  if (flag_found == 0)
	    throw CdmwPlatformMngt::AttributeNotFound ();

	}
      else
	{
	  /*case of user defined attribute */
	  try
	  {
	    CORBA::Any * outany = get_property_value (attr_name);

	    CORBA::Any_var tempany = new CORBA::Any;

	    ret_attr = outany;
	  }
	  catch (CosPropertyService::PropertyNotFound)
	  {
	    throw CdmwPlatformMngt::AttributeNotFound ();
	  }
	}

    }


    /**
     *Purpose:
     *<p> facade to set an attribute
     *
     */

    void Application_impl::set_attribute (const char *attr_name,
					  const CORBA::Any & attr,
					  CORBA::Char flag) throw (CORBA::
								   SystemException,
								   CdmwPlatformMngt::
								   AttributeNotFound)
    {
      /*char flag_found=0; */
      std::string tmp1 (attr_name);
      if (flag == 0)
	{			/* case od std attributes */
	  /*no attributes at the moment */
	  throw CdmwPlatformMngt::AttributeNotFound ();
	}
      else
	{
	  /* case of user defined attributes */
	  define_property (attr_name, attr);
	}
    }


    CORBA::ULong Application_impl::get_number_of_processes ()throw (CORBA::
								    SystemException)
    {
      CORBA::ULong len = m_process_servants.size ();
      return len;
    }

    CdmwPlatformMngt::Processes *
      Application_impl::get_processes (CORBA::ULong how_many,
				       CdmwPlatformMngt::
				       ProcessesIterator_out
				       processes_iterator) throw (CORBA::
								  SystemException)
    {
      try
      {
	CDMW_READER_LOCK_GUARD (m_rwApplicationLock);

	ProcessServants::iterator processesIt;

	// Get the how_many application references   
	size_t len = m_process_servants.size ();

	// set length of requested sequence and rest of sequence
	size_t requested_len = len;
	size_t rest_len = 0;

	// adjust to how_many requested       
	if (len > how_many)
	  {
	    requested_len = how_many;
	    rest_len = len - how_many;
	  }

	// Get the how_many process references
	unsigned int count;
	CdmwPlatformMngt::Processes_var processes
	  = new CdmwPlatformMngt::Processes ();

	processes->length (requested_len);
	for (processesIt = m_process_servants.begin (), count = 0;
	     count < requested_len; processesIt++, count++)
	  {
	    processes[count] = processesIt->second->_this ();
	  }

	// Get an iterator on the rest of the process 
	std::list < CdmwPlatformMngt::Process_var > processesRemaining;
	for (; processesIt != m_process_servants.end (); processesIt++)
	  {
	    CdmwPlatformMngt::Process_var process
	      = processesIt->second->_this ();

	    processesRemaining.push_back (process);
	  }

	CdmwPlatformMngt::ProcessesIterator_var processesRemainingIt
	  = ProcessesIterator_impl::create (processesRemaining);

	// Assign the iterator to the processes_iterator
	processes_iterator = processesRemainingIt._retn ();

	return processes._retn ();
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
    }

    CdmwPlatformMngt::Process_ptr Application_impl::
      get_process (const char *process_name,
		   const char *host_name) throw (CdmwPlatformMngt::
						 ProcessNotFound,
						 CORBA::SystemException)
    {
      Process_impl *process = 0;

      try
      {
	// check that host is defined
	std::string host_name_buf;

	// if host name undefined
	if (!strcmp (host_name, ""))
	  {
	    // search the host name from the process name if it is unique
	    process = getProcessHostName (process_name, host_name_buf);
	  }

	// else the process key is ok, search in map
	else
	  {
	    CDMW_READER_LOCK_GUARD (m_rwApplicationLock);

	    std::string process_key =
	      ManagedElementKey::Get_process_rel_key (host_name,
						      process_name);

	    ProcessServants::iterator processIt =
	      m_process_servants.find (process_key);
	    if (processIt == m_process_servants.end ())
	      {
		throw CdmwPlatformMngt::ProcessNotFound ();
	      }

	    process = processIt->second;
	  }

	return process->_this ();
      }
      catch (ProcessNameNotUnique &)
      {
	throw CdmwPlatformMngt::ProcessNotFound ();
      }
      catch (...)
      {
	throw CdmwPlatformMngt::ProcessNotFound ();
      }
    }



    void Application_impl::define (const CdmwPlatformMngt::
				   ApplicationDef & application_def)
      throw (CdmwPlatformMngt::DuplicateEntity,
	     CdmwPlatformMngt::DuplicateService,
	     CdmwPlatformMngt::DuplicateProcess,
	     CdmwPlatformMngt::ProcessHostNotFound,
	     CdmwPlatformMngt::DuplicateProcessEntity,
	     CdmwPlatformMngt::DuplicateProcessService,
	     CdmwPlatformMngt::DuplicateProcessStep,
	     CdmwPlatformMngt::AlreadyDone,
	     CdmwPlatformMngt::IncompatibleStatus,
	     CosPropertyService::MultipleExceptions, CORBA::SystemException)
    {
      LogMngr::logMessage (INF,
			   MSG_ID_APPLICATION_DEFINE, m_element_name.in ());

      CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);

      m_stateMachine->getState ()->define (m_stateMachine, application_def);
    }

    CdmwPlatformMngt::Process_ptr Application_impl::
      add_process (CdmwPlatformMngt::ProcessDef *
		   process_def) throw (CdmwPlatformMngt::ProcessAlreadyExists,
				       CdmwPlatformMngt::HostNotFound,
				       CdmwPlatformMngt::DuplicateEntity,
				       CdmwPlatformMngt::DuplicateService,
				       CdmwPlatformMngt::DuplicateStep,
				       CdmwPlatformMngt::IncompatibleStatus,
				       CosPropertyService::MultipleExceptions,
				       CORBA::SystemException)
    {
      LogMngr::logMessage (INF,
			   MSG_ID_PROCESS_ADD,
			   process_def->name (), m_element_name.in ());

      CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);

      return m_stateMachine->getState ()->add_process (m_stateMachine,
						       process_def);
    }

    void Application_impl::remove_process (const char *process_name,
					   const char *host_name)
      throw (CdmwPlatformMngt::ProcessNotFound,
	     CdmwPlatformMngt::ProcessStillActive,
	     CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      std::string process_key =
	ManagedElementKey::Get_process_rel_key (host_name, process_name);

      LogMngr::logMessage (INF,
			   MSG_ID_PROCESS_REMOVE,
			   process_key.c_str (), m_element_name.in ());

      CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);

      m_stateMachine->getState ()->remove_process (m_stateMachine,
						   process_key.c_str ());
    }

    void Application_impl::set_init_graph (const CdmwPlatformMngt::
					   GraphElements & process_graph)
      throw (CdmwPlatformMngt::ReferenceNotFound,
	     CdmwPlatformMngt::CircularReference,
	     CdmwPlatformMngt::StepOutOfBound, CdmwPlatformMngt::StepJump,
	     CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      LogMngr::logMessage (INF,
			   MSG_ID_APPLICATION_SET_INIT_GRAPH,
			   m_element_name.in ());

      CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);

      try
      {

	m_stateMachine->getState ()->set_init_graph (m_stateMachine,
						     process_graph);

      }
      catch (OutOfMemoryException)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }



    }

    void Application_impl::set_stop_graph (const CdmwPlatformMngt::
					   GraphElements & process_graph)
      throw (CdmwPlatformMngt::ReferenceNotFound,
	     CdmwPlatformMngt::CircularReference,
	     CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      LogMngr::logMessage (INF,
			   MSG_ID_APPLICATION_SET_STOP_GRAPH,
			   m_element_name.in ());

      CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);
      try
      {

	m_stateMachine->getState ()->set_stop_graph (m_stateMachine,
						     process_graph);

      }
      catch (OutOfMemoryException)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
    }


    CdmwPlatformMngt::ApplicationAutoRestartInfo Application_impl::
      get_autorestart_info ()throw (CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }



    void Application_impl::set_autorestart_info (const CdmwPlatformMngt::
						 ApplicationAutoRestartInfo &
						 autorestart_info)
      throw (CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }



    void Application_impl::initialise (const CdmwPlatformMngtBase::
				       StartupKind & startup_kind)
      throw (CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      LogMngr::logMessage (INF,
			   MSG_ID_APPLICATION_INITIALISE,
			   m_element_name.in ());

      CDMW_SM_WRITER_LOCK_GUARD (*m_stateMachine);

      try
      {
	m_stateMachine->getState ()->initialise (m_stateMachine,
						 startup_kind);
      }
      catch (...)
      {
	// even in case of exception update record in datastore
	set_record_in_datastore ();

	// rethrow exception
	throw;
      }

      // update record in datastore
      set_record_in_datastore ();
    }



    CdmwPlatformMngt::LongRequestStatus Application_impl::
      initialise_and_wait (const CdmwPlatformMngtBase::
			   StartupKind & startup_kind)
      throw (CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      CdmwPlatformMngt::ApplicationStatus appStatus;
      CdmwPlatformMngt::LongRequestStatus req_status =
	CdmwPlatformMngt::LONG_REQUEST_NOT_COMPLETED;
      bool init_req_done = false;

      {
	// lock mutex for reading status
	CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);

	// get the current status
	appStatus = m_stateMachine->getState ()->status ();

	if (appStatus == CdmwPlatformMngt::APPLICATION_INITIALISED ||
	    appStatus == CdmwPlatformMngt::APPLICATION_RUN_REQUEST ||
	    appStatus == CdmwPlatformMngt::APPLICATION_RUNNING)
	  {
	    req_status = CdmwPlatformMngt::LONG_REQUEST_COMPLETED;
	    init_req_done = true;
	  }

	else if (appStatus ==
		 CdmwPlatformMngt::APPLICATION_INITIALISATION_SUSPENDED)
	  {
	    req_status = CdmwPlatformMngt::LONG_REQUEST_SUSPENDED;
	    init_req_done = true;
	  }

	else if (appStatus ==
		 CdmwPlatformMngt::APPLICATION_INITIALISATION_INCOMPLETE)
	  {
	    req_status = CdmwPlatformMngt::LONG_REQUEST_NOT_COMPLETED;
	    init_req_done = true;
	  }
      }


      // if initialise request not done :
      // init is in progress : condition flag is not yet set
      // init is done now with error or not (previously in progress) : condition flag is set
      // or appli in Initial, Stopped state : condition flag not set                    
      if (!init_req_done)
	{
	  // first lock mutex for condition
	  CDMW_MUTEX_GUARD (m_mutex);

	  // if current status get previously in guarded sequence
	  // is not in transcient initialising (in this case initialised cannot be reached)
	  // it can be only in Initial, Stopped state
	  if (appStatus != CdmwPlatformMngt::APPLICATION_INITIALISING)
	    {
	      // start initialise
	      // exception is thrown in case of incompatible status
	      initialise (startup_kind);
	    }

	  // initialisation is in progress
	  // so wait the condition flag is set by broadcasting
	  while (!m_init_condition_flag)
	    {
	      m_initialisation_condition.wait ();
	    }

	  m_init_condition_flag = false;

	  // get the result
	  req_status = m_initialisation_status;
	}

      return req_status;
    }


    void Application_impl::resume ()
      throw (CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      LogMngr::logMessage (INF,
			   MSG_ID_APPLICATION_RESUME, m_element_name.in ());

      CDMW_SM_WRITER_LOCK_GUARD (*m_stateMachine);

      try
      {
	m_stateMachine->getState ()->resume (m_stateMachine);
      }
      catch (...)
      {
	// even in case of exception update record in datastore
	set_record_in_datastore ();

	// rethrow exception
	throw;
      }

      // update record in datastore
      set_record_in_datastore ();
    }


    CdmwPlatformMngt::LongRequestStatus Application_impl::
      resume_and_wait ()throw (CdmwPlatformMngt::IncompatibleStatus,
			       CORBA::SystemException)
    {
      CdmwPlatformMngt::ApplicationStatus appStatus;
      CdmwPlatformMngt::LongRequestStatus req_status =
	CdmwPlatformMngt::LONG_REQUEST_NOT_COMPLETED;
      bool init_req_done = false;

      {
	// lock mutex for reading status
	CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);

	// get the current status
	appStatus = m_stateMachine->getState ()->status ();

	if (appStatus == CdmwPlatformMngt::APPLICATION_INITIALISED ||
	    appStatus == CdmwPlatformMngt::APPLICATION_RUN_REQUEST ||
	    appStatus == CdmwPlatformMngt::APPLICATION_RUNNING)
	  {
	    req_status = CdmwPlatformMngt::LONG_REQUEST_COMPLETED;
	    init_req_done = true;
	  }

	else if (appStatus ==
		 CdmwPlatformMngt::APPLICATION_INITIALISATION_INCOMPLETE)
	  {
	    req_status = CdmwPlatformMngt::LONG_REQUEST_NOT_COMPLETED;
	    init_req_done = true;
	  }
      }


      // if initialise request not done :
      // initialisation is in progress : condition flag is not yet set
      // initialisation is done now with error or not (previously in progress) : flag is set
      // or application in Initial, Suspended, Stopped state 
      // (flag not set)                    
      if (!init_req_done)
	{
	  // first lock mutex for condition
	  CDMW_MUTEX_GUARD (m_mutex);

	  // if current status get previously in guarded sequence
	  // is not in transcient initialising (in this case initialised cannot be reached)
	  // it can be only in Initial, RunRequest, Stopping, Stopped state
	  if (appStatus != CdmwPlatformMngt::APPLICATION_INITIALISING)
	    {
	      // start resume
	      // exception is thrown in case of incompatible status
	      resume ();
	    }

	  // initialisation is in progress
	  // so wait the condition flag is set by broadcasting
	  while (!m_init_condition_flag)
	    {
	      m_initialisation_condition.wait ();
	    }

	  m_init_condition_flag = false;

	  // get the result
	  req_status = m_initialisation_status;
	}

      return req_status;
    }


    void Application_impl::acknowledge ()
      throw (CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      LogMngr::logMessage (INF,
			   MSG_ID_APPLICATION_ACKNOWLEDGE,
			   m_element_name.in ());

      CDMW_MUTEX_GUARD (m_mutexMode);

      CdmwPlatformMngt::FunctioningMode mode = CdmwPlatformMngt::NORMAL_MODE;
      CORBA::String_var mode_info = CORBA::string_dup ("");

      set_mode (mode, mode_info.in ());

      // Notify the change of mode
      notifyModeChangeEvent (mode, mode_info.in ());
    }

    void Application_impl::run ()
      throw (CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      LogMngr::logMessage (INF, MSG_ID_APPLICATION_RUN, m_element_name.in ());

      CDMW_SM_WRITER_LOCK_GUARD (*m_stateMachine);

      try
      {
	// recover the default user defined attributes values 
	recover_default_properties ();

	// recover default entityDef
	CORBA::ULong how_many =
	  m_rwEntityContainer->get_number_of_entities ();
	CdmwPlatformMngt::ElementNames_var element_names;
	CdmwPlatformMngt::ElementNamesIterator_var element_names_iterator;
	m_rwEntityContainer->get_all_entity_names (how_many,
						   element_names.out (),
						   element_names_iterator.
						   out ());
	//delete all entity
	CORBA::ULong size = element_names->length ();
	for (CORBA::ULong count = 0; count < size; count++)
	  {
	    m_rwEntityContainer->remove_entity (element_names[count]);
	  }
	//restore the size of default entities
	element_names->length (m_default_entitydefs->length ());
	for (CORBA::ULong count = 0; count < element_names->length ();
	     count++)
	  {
	    m_rwEntityContainer->add_entity ((*m_default_entitydefs)[count]);
	  }


	m_stateMachine->getState ()->run (m_stateMachine);
      }
      catch (...)
      {
	// even in case of exception update record in datastore
	set_record_in_datastore ();

	// rethrow exception
	throw;
      }

      // update record in datastore
      set_record_in_datastore ();
    }

    void Application_impl::stop (CORBA::Boolean emergency)
      throw (CORBA::SystemException)
    {
      LogMngr::logMessage (INF,
			   MSG_ID_APPLICATION_STOP, m_element_name.in ());

      CDMW_SM_WRITER_LOCK_GUARD (*m_stateMachine);

      try
      {
	m_emergency = emergency;
	m_stateMachine->getState ()->stop (m_stateMachine, emergency);
      }
      catch (...)
      {
	// even in case of exception update record in datastore
	set_record_in_datastore ();

	// rethrow exception
	throw;
      }

      // update record in datastore
      set_record_in_datastore ();
    }


    CdmwPlatformMngt::LongRequestStatus Application_impl::
      stop_and_wait (CORBA::Boolean emergency) throw (CORBA::SystemException)
    {
      CdmwPlatformMngt::ApplicationStatus appStatus;
      CdmwPlatformMngt::LongRequestStatus req_status =
	CdmwPlatformMngt::LONG_REQUEST_COMPLETED;
      bool stop_req_done = false;

      {
	// lock mutex for reading status
	CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);

	// get the current status
	appStatus = m_stateMachine->getState ()->status ();

	if (appStatus == CdmwPlatformMngt::APPLICATION_INITIAL ||
	    appStatus == CdmwPlatformMngt::APPLICATION_STOPPED)
	  {
	    req_status = CdmwPlatformMngt::LONG_REQUEST_COMPLETED;
	    stop_req_done = true;
	  }
      }


      // if stop request not done :
      // stopping is in progress : condition flag is not set
      // stop is done now with error or not (previously in progress) : flag is set
      // or application in Initialising, Initialised, Suspended, 
      // RunRequest, Running state 
      // (flag not set)                    
      if (!stop_req_done)
	{
	  // first lock mutex for condition
	  CDMW_MUTEX_GUARD (m_mutex);

	  // if current status get previously in guarded sequence
	  // is not in transcient stopping (in this case stopped cannot be reached)
	  // it can be only in Initialising, Initialised, Suspended, 
	  // RunRequest, Running state
	  if (appStatus != CdmwPlatformMngt::APPLICATION_STOPPING)
	    {
	      // stop application
	      // exception is thrown in case of incompatible status
	      stop (emergency);
	    }

	  // stopping is in progress
	  // so wait the condition flag is set by broadcasting
	  while (!m_stop_condition_flag)
	    {
	      m_stop_condition.wait ();
	    }

	  m_stop_condition_flag = false;

	  // get the result
	  req_status = m_stop_status;
	}

      return req_status;
    }


    void Application_impl::cold_restart (const CdmwPlatformMngtBase::
					 StartupKind & startup_kind)
      throw (CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }




    void Application_impl::hot_restart (const CdmwPlatformMngtBase::
					StartupKind & startup_kind)
      throw (CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }





    CORBA::ULong Application_impl::get_number_of_entities ()throw (CORBA::
								   SystemException)
    {
      return m_rwEntityContainer->get_number_of_entities ();
    }

    void Application_impl::get_all_entity_names (CORBA::ULong how_many,
						 CdmwPlatformMngt::
						 ElementNames_out
						 entity_names,
						 CdmwPlatformMngt::
						 ElementNamesIterator_out
						 rest) throw (CORBA::
							      SystemException)
    {
      m_rwEntityContainer->get_all_entity_names (how_many, entity_names,
						 rest);
    }

    CdmwPlatformMngt::Entity_ptr Application_impl::
      get_entity (const char *entity_name) throw (CdmwPlatformMngt::
						  EntityNotFound,
						  CORBA::SystemException)
    {
      return m_rwEntityContainer->get_entity (entity_name);
    }

    CORBA::Boolean Application_impl::get_entities (const CdmwPlatformMngt::
						   ElementNames &
						   entity_names,
						   CdmwPlatformMngt::
						   Entities_out nentities)
      throw (CORBA::SystemException)
    {
      return m_rwEntityContainer->get_entities (entity_names, nentities);
    }

    void Application_impl::get_all_entities (CORBA::ULong how_many,
					     CdmwPlatformMngt::
					     Entities_out nentities,
					     CdmwPlatformMngt::
					     EntitiesIterator_out rest)
      throw (CORBA::SystemException)
    {
      m_rwEntityContainer->get_all_entities (how_many, nentities, rest);
    }


    CdmwPlatformMngt::Entity_ptr Application_impl::
      add_entity (const CdmwPlatformMngt::
		  EntityDef & entity_def) throw (CdmwPlatformMngt::
						 EntityAlreadyExists,
						 CORBA::SystemException)
    {
      std::string entityName = entity_def.entity_name.in ();

      LogMngr::logMessage (INF, MSG_ID_ADD_APPLICATION_ENTITY,
			   entityName.c_str (), m_element_name.in ());

      return m_rwEntityContainer->add_entity (entity_def);

      // TODO: Notify the configuration change

    }

    void Application_impl::remove_entity (const char *entity_name)
      throw (CdmwPlatformMngt::EntityNotFound, CORBA::SystemException)
    {
      LogMngr::logMessage (INF, MSG_ID_REMOVE_APPLICATION_ENTITY,
			   entity_name, m_element_name.in ());

      m_rwEntityContainer->remove_entity (entity_name);

      // TODO: Notify the configuration change

    }

    CdmwPlatformMngtService::ServiceProviderID *
      Application_impl::
      get_service_def (const char *service_name)
      throw (CdmwPlatformMngtService::ServiceNotFound, CORBA::SystemException)
    {
      return m_rwServiceDefContainer->get_service_def (service_name);
    }

    void Application_impl::add_service_def (const CdmwPlatformMngtService::
					    ServiceDef & service_def)
      throw (CdmwPlatformMngtService::ServiceAlreadyExists,
	     CORBA::SystemException)
    {
      m_rwServiceDefContainer->add_service_def (service_def);

      // TODO: Notify the configuration change

      // TODO: Trace to the log manager
    }

    void Application_impl::remove_service_def (const char *service_name)
      throw (CdmwPlatformMngtService::ServiceNotFound, CORBA::SystemException)
    {
      m_rwServiceDefContainer->remove_service_def (service_name);

      // TODO: Notify the configuration change

      // TODO: Trace to the log manager
    }

    void Application_impl::remove_all_service_defs ()
      throw (CORBA::SystemException)
    {
      m_rwServiceDefContainer->remove_all_service_defs ();

      // TODO: Notify the configuration change

      // TODO: Trace to the log manager
    }


    void Application_impl::is_alive () throw (CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }

    CdmwPlatformMngt::SupervisionObserver_ptr Application_impl::
      register_observer (const char *observer_name,
			 CdmwPlatformMngt::
			 SupervisionObserver_ptr observer) throw (CORBA::
								  SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }

    CdmwPlatformMngt::SupervisionObserver_ptr Application_impl::
      register_proxy_observer (const char *observer_name,
			       const char *host_name,
			       CdmwPlatformMngt::
			       SupervisionObserver_ptr observer)
      throw (CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }

    CdmwPlatformMngt::SupervisionObserver_ptr Application_impl::
      unregister_observer (const char *observer_name)
      throw (CdmwPlatformMngt::ObserverNotFound, CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }



    const char *Application_impl::get_system_name ()
    {
      return m_system_name.in ();
    }




  }				// End namespace PlatformMngt
}				// End namespace Cdmw
