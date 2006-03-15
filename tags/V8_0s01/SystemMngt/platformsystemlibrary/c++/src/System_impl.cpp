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


#include "platformsystemlibrary/System_impl.hpp"

#include "platformsystemlibrary/ServiceBinding_impl.hpp"
#include "platformsystemlibrary/HostContainer_impl.hpp"
#include "platformsystemlibrary/HostContainer.hpp"
#include "platformsystemlibrary/ApplicationObserver_impl.hpp"
#include "platformsystemlibrary/ApplicationIterator_impl.hpp"

#include "platformsystemlibrary/RunSetManager.hpp"
#include "platformsystemlibrary/SystemLogMessageId.hpp"

#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "SystemMngt/platformlibrary/RWServiceDefContainer_impl.hpp"
#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"
#include "SystemMngt/platformlibrary/GraphChecker.hpp"
#include "SystemMngt/platformlibrary/GraphUtility.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformlibrary/TaskFactory.hpp"
#include "SystemMngt/platformlibrary/TaskSequencer.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"
#include "SystemMngt/platformlibrary/ManagedElementKey.hpp"

#include "SystemMngt/platformelementlibrary/RWEntityContainer_impl.hpp"


#include "SystemMngt/platformvaluetypes/GraphElement_impl.hpp"
#include "SystemMngt/platformvaluetypes/SystemStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/SystemModeChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/EntityStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/HostStatusChange_impl.hpp"

#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/osthreads/ReaderLockGuard.hpp"
#include "Foundation/osthreads/WriterLockGuard.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp"

#include "platformsystemlibrary/ApplicationInit.hpp"
#include "platformsystemlibrary/ApplicationRun.hpp"
#include "platformsystemlibrary/ApplicationStop.hpp"
#include <string>


namespace
{

  const int MAX_RETRY_ON_FTMGR = 4;
  const int SLEEP_RETRY_ON_FTMGR_MS = 1000;
}




namespace Cdmw
{
  namespace PlatformMngt
  {

// set static members

    SystemInitial *System_impl::M_initial = 0;
    SystemStarting *System_impl::M_starting = 0;
    SystemStartupSuspended *System_impl::M_startupSuspended = 0;
    SystemStarted *System_impl::M_started = 0;
    SystemStopping *System_impl::M_stopping = 0;
    SystemStopped *System_impl::M_stopped = 0;

    const std::string System_impl::M_initialStateName = "initial";
    const std::string System_impl::M_startingStateName = "starting";
    const std::string System_impl::M_startupSuspendStateName =
      "startupSuspend";
    const std::string System_impl::M_startedStateName = "started";
    const std::string System_impl::M_stoppingStateName = "stopping";
    const std::string System_impl::M_stoppedStateName = "stopped";

//    
// Dumper of supervision observer from datastore
// operator ()
//    
    void ObserverDumper::operator () (const PairType & elt)
    {
      // for the extracted record from datastore
      // register the specified supervision observer
      try
      {
	if (!strcmp ((elt.second)->host_name, ""))
	  {
	    m_system->register_observer (elt.first.c_str (),
					 (elt.second)->observer.in ());
	  }
	else
	  {
	    m_system->register_proxy_observer (elt.first.c_str (),
					       (elt.second)->host_name,
					       (elt.second)->observer.in ());
	  }
      }
      catch (...)
      {
	// nothing to do
      }
    }

// ----------------------------------------------------------------------
// System_impl class.
// ----------------------------------------------------------------------

  System_impl::System_impl (SupervisionEventNotifier * eventNotifier, CORBA::ORB_ptr orb, PortableServer::POA_ptr poa, CdmwPlatformMngt::RWElementRepository_ptr element_repository) throw (BadParameterException, AlreadyDoneException, UnknownStateException, AlreadyExistsException, OutOfResourcesException, OutOfMemoryException):ManagedElement_impl
      (element_repository)
    {
      if (eventNotifier == 0)
	{
	  CDMW_THROW2 (BadParameterException, "eventNotifier", "0");
	}

      if (CORBA::is_nil (poa))
	{
	  CDMW_THROW2 (BadParameterException, "poa", "nil");
	}

      try
      {
	Cdmw::CommonSvcs::DataStore::DataStoreBase * ds1 =
	  Cdmw::CommonSvcs::DataStore::DataStoreRegistry::
	  Get_data_store (Configuration::Get_system_datastore_id ());
	m_system_ds =
	  dynamic_cast < Cdmw::PlatformMngt::SystemDataStore * >(ds1);

	Cdmw::CommonSvcs::DataStore::DataStoreBase * ds2 =
	  Cdmw::CommonSvcs::DataStore::DataStoreRegistry::
	  Get_data_store (Configuration::
			  Get_system_observer_registration_datastore_id ());
	m_system_observer_ds =
	  dynamic_cast <
	  Cdmw::PlatformMngt::SystemObserverRegistrationDataStore * >(ds2);
      }
      catch (const NotExistException & e)
      {
	CDMW_ASSERT (false);
      }



      // Set the internal data
      m_eventNotifier = eventNotifier;
      m_orb = CORBA::ORB::_duplicate (orb);
      m_poa = PortableServer::POA::_duplicate (poa);


      m_systemDefined = false;

      m_statusInfo = CORBA::string_dup ("");
      m_mode = CdmwPlatformMngt::NORMAL_MODE;
      m_modeInfo = CORBA::string_dup ("");
      m_emergency = false;
      m_newGraph = true;
      m_startup_kind = new CdmwPlatformMngtBase::StartupKind ();

      m_actualServiceDefContainer = 0;
      m_actualEntityContainer = 0;

      m_initSyncTaskFactory = 0;
      m_initSequencer = 0;

      m_stopSyncTaskFactory = 0;
      m_stopSequencer = 0;

      m_stateMachine = 0;
      m_rwServiceDefContainer = 0;
      m_rwEntityContainer = 0;
      m_actualHostContainer = 0;
      m_runSetManager = 0;

      m_sysStatusChangeFactory = 0;
      m_sysModeChangeFactory = 0;
      m_sysEntityStatusChangeFactory = 0;
      m_hostStatusChangeFactory = 0;

      m_default_entitydefs = 0;

      try
      {
	// create an empty init application graph
	std::auto_ptr < CdmwPlatformMngt::GraphElements >
	  applicationInitGraph (new CdmwPlatformMngt::GraphElements);

	// create an empty stop application graph
	std::auto_ptr < CdmwPlatformMngt::GraphElements >
	  applicationStopGraph (new CdmwPlatformMngt::GraphElements);

	// create an empty stop emergency graph
	std::auto_ptr < CdmwPlatformMngt::GraphElements >
	  stopGraph (new CdmwPlatformMngt::GraphElements);



	// create local objects        
	std::auto_ptr < SystemStateMachine >
	  stateMachine (new SystemStateMachine (this, "System"));

	std::auto_ptr < PlatformMngt::HostContainer >
	  actualHostContainer (new PlatformMngt::
			       HostContainer (this, poa, element_repository));


	std::auto_ptr < RunSetManager >
	  runSetManager (new RunSetManager (this));


	std::auto_ptr < SystemStatusChangeFactory >
	  sysStatusChangeFactory (new SystemStatusChangeFactory ());
	std::auto_ptr < SystemModeChangeFactory >
	  sysModeChangeFactory (new SystemModeChangeFactory ());
	std::auto_ptr < SystemEntityStatusChangeFactory >
	  sysEntityStatusChangeFactory (new
					SystemEntityStatusChangeFactory ());
	std::auto_ptr < HostStatusChangeFactory >
	  hostStatusChangeFactory (new HostStatusChangeFactory ());

	// create the actual system entity container with empty parent name         
	m_actualEntityContainer =
	  new PlatformMngt::EntityContainer (m_poa.in (),
					     m_element_repository.in (), "");

	// create the rw system entity container                                      
	m_rwEntityContainer =
	  new RWEntityContainer_impl (m_poa.in (), m_actualEntityContainer,
				      m_element_repository.in ());
	// create an object var to take pointer ownership
	PortableServer::ServantBase_var rwEntityContainer =
	  m_rwEntityContainer;


	// create the actual system service def container
	m_actualServiceDefContainer =
	  ServiceDefContainer_impl::ServiceDefContainer::
	  createSystemContainer ();

	// create the rw system service def container
	m_rwServiceDefContainer =
	  new RWServiceDefContainer_impl (m_poa.in (),
					  m_actualServiceDefContainer);
	// create an object var to take pointer ownership
	PortableServer::ServantBase_var rwServiceDefContainer =
	  m_rwServiceDefContainer;


	// Create Object servants

	// Create the ServiceDefContainer servant
	m_serviceDefContainer =
	  new ServiceDefContainer_impl (m_poa.in (),
					m_actualServiceDefContainer);
	// create an object var to take pointer ownership
	PortableServer::ServantBase_var serviceDefContainerServant =
	  m_serviceDefContainer;
	// activate object
	m_serviceDefContainer->activate ();
	m_serviceDefContainerObject = m_serviceDefContainer->_this ();


	// Create the ServiceBinding servant
	m_serviceBinding = new ServiceBinding_impl (this);
	// create an object var to take pointer ownership
	PortableServer::ServantBase_var serviceBindingServant =
	  m_serviceBinding;
	// activate object
	m_serviceBindingObject = m_serviceBinding->_this ();


	// Create the HostContainer servant
	m_hostContainer =
	  new HostContainer_impl (m_poa.in (), actualHostContainer.get (),
				  element_repository);
	// create an object var to take pointer ownership
	PortableServer::ServantBase_var hostContainerServant =
	  m_hostContainer;
	// activate object
	m_hostContainerObject = m_hostContainer->_this ();


	// Create the ApplicationObserver servant
	m_applicationObserver = new ApplicationObserver_impl (this);
	// create an object var to take pointer ownership
	PortableServer::ServantBase_var applicationObserverServant =
	  m_applicationObserver;
	// activate object
	m_applicationObserverObject = m_applicationObserver->_this ();


	// Set the internal data   
	m_applicationInitGraph = applicationInitGraph.get ();
	m_applicationStopGraph = applicationStopGraph.get ();
	m_stopGraph = stopGraph.get ();
	m_stateMachine = stateMachine.get ();
	m_actualHostContainer = actualHostContainer.get ();
	m_runSetManager = runSetManager.get ();
	m_sysStatusChangeFactory = sysStatusChangeFactory.get ();
	m_sysModeChangeFactory = sysModeChangeFactory.get ();
	m_sysEntityStatusChangeFactory = sysEntityStatusChangeFactory.get ();
	m_hostStatusChangeFactory = hostStatusChangeFactory.get ();


	// Add the states to the state machine
	m_stateMachine->addState (M_initialStateName, M_initial);
	m_stateMachine->addState (M_startingStateName, M_starting);
	m_stateMachine->addState (M_startupSuspendStateName,
				  M_startupSuspended);
	m_stateMachine->addState (M_startedStateName, M_started);
	m_stateMachine->addState (M_stoppingStateName, M_stopping);
	m_stateMachine->addState (M_stoppedStateName, M_stopped);

	// Set the initial state
	m_stateMachine->setState (M_initialStateName);


	// start event handler
	m_eventHandler.start ();


	// start RunSetManager thread
	m_runSetManager->start ();



	// Release the temporary objects
	applicationInitGraph.release ();
	applicationStopGraph.release ();
	stopGraph.release ();
	stateMachine.release ();
	actualHostContainer.release ();
	runSetManager.release ();
	sysStatusChangeFactory.release ();
	sysModeChangeFactory.release ();
	sysEntityStatusChangeFactory.release ();
	hostStatusChangeFactory.release ();

	// release servant var for local object (to avoid automatic desallocation)
	rwEntityContainer._retn ();
	rwServiceDefContainer._retn ();
      }
      catch (const std::bad_alloc &)
      {
	// destroy containers
	try
	{
	  ServiceDefContainer_impl::ServiceDefContainer::
	    destroy (m_actualServiceDefContainer);
	}
	catch (...)
	{
	}

	CDMW_THROW (OutOfMemoryException);
      }
    }



    System_impl::~System_impl ()
    {
      // stop event handler
      m_eventHandler.stop ();

      // destroy init and stop sequencers
      destroyInitSequencer ();
      destroyStopSequencer ();

      // destroy RunSet manager
      destroyRunSetManager ();

      // destroy containers
      try
      {
	ServiceDefContainer_impl::ServiceDefContainer::
	  destroy (m_actualServiceDefContainer);
      }
      catch (...)
      {
      }


      if (m_stateMachine != 0)
	{
	  delete m_stateMachine;
	}

      if (m_rwServiceDefContainer != 0)
	{
	  // remove reference before deletion
	  m_rwServiceDefContainer->_remove_ref ();
	}

      if (m_actualEntityContainer != 0)
	{
	  delete m_actualEntityContainer;
	}

      if (m_rwEntityContainer != 0)
	{
	  // remove reference before deletion
	  m_rwEntityContainer->_remove_ref ();
	}

      if (m_actualHostContainer != 0)
	{
	  delete m_actualHostContainer;
	}

      if (m_sysStatusChangeFactory != 0)
	{
	  m_sysStatusChangeFactory->_remove_ref ();
	}

      if (m_sysModeChangeFactory != 0)
	{
	  m_sysModeChangeFactory->_remove_ref ();
	}

      if (m_sysEntityStatusChangeFactory != 0)
	{
	  m_sysEntityStatusChangeFactory->_remove_ref ();
	}

      if (m_hostStatusChangeFactory != 0)
	{
	  m_hostStatusChangeFactory->_remove_ref ();
	}

      // remove record from datastore
      remove_system_record_in_datastore ();
    }


    void System_impl::destroyInitSequencer ()
    {
      // Stop and destroy the init sequencer 
      if (m_initSequencer != 0)
	{
	  m_initSequencer->stop ();

	  delete m_initSequencer;
	  m_initSequencer = 0;
	}

      if (m_initSyncTaskFactory != 0)
	{
	  delete m_initSyncTaskFactory;
	  m_initSyncTaskFactory = 0;
	}
    }

    void System_impl::destroyStopSequencer ()
    {
      // Stop and destroy the stop sequencer 
      if (m_stopSequencer != 0)
	{
	  m_stopSequencer->stop ();

	  delete m_stopSequencer;
	  m_stopSequencer = 0;
	}

      if (m_stopSyncTaskFactory != 0)
	{
	  delete m_stopSyncTaskFactory;
	  m_stopSyncTaskFactory = 0;
	}
    }


    void System_impl::destroyRunSetManager ()
    {
      if (m_runSetManager != 0)
	{
	  // stop RunSetManager starters
	  m_runSetManager->stopAllRunSets ();

	  // stop thread
	  m_runSetManager->stop ();

	  delete m_runSetManager;
	  m_runSetManager = 0;
	}
    }


    void System_impl::createStates () throw (OutOfMemoryException)
    {
      try
      {
	std::auto_ptr < SystemInitial > initial (new SystemInitial ());
	std::auto_ptr < SystemStarting > starting (new SystemStarting ());
	std::auto_ptr < SystemStartupSuspended >
	  startupSuspended (new SystemStartupSuspended ());
	std::auto_ptr < SystemStarted > started (new SystemStarted ());
	std::auto_ptr < SystemStopping > stopping (new SystemStopping ());
	std::auto_ptr < SystemStopped > stopped (new SystemStopped ());



	if (M_initial == 0)
	  {
	    M_initial = initial.get ();
	  }
	if (M_starting == 0)
	  {
	    M_starting = starting.get ();
	  }
	if (M_startupSuspended == 0)
	  {
	    M_startupSuspended = startupSuspended.get ();
	  }
	if (M_started == 0)
	  {
	    M_started = started.get ();
	  }
	if (M_stopping == 0)
	  {
	    M_stopping = stopping.get ();
	  }
	if (M_stopped == 0)
	  {
	    M_stopped = stopped.get ();
	  }

	initial.release ();
	starting.release ();
	startupSuspended.release ();
	started.release ();
	stopping.release ();
	stopped.release ();

      }
      catch (const std::bad_alloc &)
      {
	CDMW_THROW (OutOfMemoryException);
      }
    }

    void System_impl::destroyStates ()
    {
      if (M_initial != 0)
	{
	  delete M_initial;
	  M_initial = 0;
	}

      if (M_starting != 0)
	{
	  delete M_starting;
	  M_starting = 0;
	}

      if (M_startupSuspended != 0)
	{
	  delete M_startupSuspended;
	  M_startupSuspended = 0;
	}

      if (M_started != 0)
	{
	  delete M_started;
	  M_started = 0;
	}

      if (M_stopping != 0)
	{
	  delete M_stopping;
	  M_stopping = 0;
	}

      if (M_stopped != 0)
	{
	  delete M_stopped;
	  M_stopped = 0;
	}

    }


    HostContainer_impl *System_impl::get_host_container_impl ()
    {
      return m_hostContainer;
    }

    PlatformMngt::HostContainer * System_impl::get_actual_host_container ()
    {
      return m_actualHostContainer;
    }

    const CdmwPlatformMngtService::ServiceBinding_ptr System_impl::
      get_service_binding ()
    {
      return m_serviceBindingObject.in ();
    }



#ifdef CDMW_USE_FAULTTOLERANCE

    void System_impl::set_replication_mngr_iogr (const std::
						 string &
						 replication_mngr_iogr)
    {
      m_replicationManagerIOGR = replication_mngr_iogr;
    }

    void System_impl::set_replication_mngr (CdmwFT::
					    ReplicationManager_ptr
					    replication_mngr)
    {
      m_replicationMngt =
	CdmwFT::ReplicationManager::_duplicate (replication_mngr);
    }

    void System_impl::set_ft_converter_object (CdmwPlatformMngt::
					       FTConverter_ptr
					       ft_converter_obj)
    {
      m_FTConverterObject =
	CdmwPlatformMngt::FTConverter::_duplicate (ft_converter_obj);
    }

    void System_impl::set_ft_process_observer_group (CdmwPlatformMngt::
						     ProcessObserver_ptr
						     ft_process_observer_group)
    {
      m_FTProcessObserverGroup =
	CdmwPlatformMngt::ProcessObserver::
	_duplicate (ft_process_observer_group);
    }

    void System_impl::set_ft_service_broker_group (CdmwPlatformMngtService::
						   ServiceBroker_ptr
						   ft_service_broker_group)
    {
      m_FTServiceBrokerGroup =
	CdmwPlatformMngtService::ServiceBroker::
	_duplicate (ft_service_broker_group);
    }


#endif

    void System_impl::notifyStatusChangeEvent ()
    {
      try
      {
	// Note : lock must be set by state machine caller

	// create header
	CdmwPlatformMngtBase::EventHeader header =
	  EventHeaderFactory::createHeader (CdmwPlatformMngtBase::INF);

	// get status
	CdmwPlatformMngt::SystemStatus systemStatus =
	  m_stateMachine->getState ()->status ();

	// create event value type using its factory        
	CdmwPlatformMngt::SystemStatusChange_var event =
	  m_sysStatusChangeFactory->create (header, m_element_name.in (),
					    systemStatus, m_statusInfo.in ());

	// Trace to the log manager (must be done before addEvent)
	CORBA::String_var eventStr = event->to_string ();
	LogMngr::logMessage (eventStr.in ());

	// Perform the status change notification
	m_eventNotifier->addEvent (event);

	// release owner ship if no error
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

    void System_impl::notifyModeChangeEvent (CdmwPlatformMngt::
					     FunctioningMode functioning_mode,
					     const char *mode_info)
    {
      try
      {
	// Note : lock on mode must be set by caller

	// create header
	CdmwPlatformMngtBase::EventHeader header =
	  EventHeaderFactory::createHeader (CdmwPlatformMngtBase::INF);

	// create event value type using its factory
	CdmwPlatformMngt::SystemModeChange_var event =
	  m_sysModeChangeFactory->create (header, m_element_name.in (),
					  functioning_mode, mode_info);

	// Trace to the log manager (must be done before addEvent)
	CORBA::String_var eventStr = event->to_string ();
	LogMngr::logMessage (eventStr.in ());

	// Perform the status change notification
	m_eventNotifier->addEvent (event);

	// release owner ship if no error
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



    void System_impl::notifyEntityStatusChangeEvent (const char *entity_name,
						     const char
						     *entity_status,
						     const char *entity_info)
    {
      try
      {
	// create header
	CdmwPlatformMngtBase::EventHeader header =
	  EventHeaderFactory::createHeader (CdmwPlatformMngtBase::INF);

	// create event value type using its factory
	CdmwPlatformMngt::SystemEntityStatusChange_var event =
	  m_sysEntityStatusChangeFactory->create (header,
						  m_element_name.in (),
						  entity_name, entity_status,
						  entity_info);

	// Trace to the log manager (must be done before addEvent)
	CORBA::String_var eventStr = event->to_string ();
	LogMngr::logMessage (eventStr.in ());

	// Perform the status change notification
	m_eventNotifier->addEvent (event);

	// release owner ship if no error
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


    void System_impl::notifyHostStatusChangeEvent (const char *host_name,
						   CdmwPlatformMngt::
						   HostStatus host_status,
						   CdmwPlatformMngtBase::
						   TimeStamp time_stamp)
    {
      try
      {
	// create header
	CdmwPlatformMngtBase::EventHeader header =
	  EventHeaderFactory::createHeader (time_stamp,
					    CdmwPlatformMngtBase::WRN);

	// create event value type using its factory
	CdmwPlatformMngt::HostStatusChange_var event =
	  m_hostStatusChangeFactory->create (header, m_element_name.in (),
					     host_name, host_status);

	// Trace to the log manager (must be done before addEvent)
	CORBA::String_var eventStr = event->to_string ();
	LogMngr::logMessage (eventStr.in ());

	// Perform the status change notification
	m_eventNotifier->addEvent (event);

	// release owner ship if no error
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

    void System_impl::defineSystem (const CdmwPlatformMngt::
				    SystemDef & system_def)
      throw (CdmwPlatformMngt::DuplicateEntity,
	     CosPropertyService::MultipleExceptions,
	     CdmwPlatformMngt::DuplicateService,
	     CdmwPlatformMngt::DuplicateHost,
	     CdmwPlatformMngt::DuplicateHostEntity,
	     CdmwPlatformMngt::HostPropertyExceptions,
	     CdmwPlatformMngt::AlreadyDone, CORBA::SystemException)
    {
      CDMW_WRITER_LOCK_GUARD (m_rwSystemLock);

      if (!m_systemDefined)
	{
	  // build the system path        
	  std::string system_path =
	    ManagedElementKey::Get_system_path (system_def.system_name.in ());

	  // store element path in managed element
	  set_element_path (system_path.c_str (),
			    system_def.system_name.in ());

	  try
	  {
	    // add in repository the system object
	    m_element_repository->add_element (system_path.c_str (),
					       CdmwPlatformMngt::SYSTEM_TYPE,
					       this->_this ());
	  }
	  catch (...)
	  {
	    throw CORBA::INTERNAL (OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
	  }

	  // set element path in rwservicedef container
	  m_rwServiceDefContainer->set_container_path (system_path.c_str ());

	  // get system port
	  m_systemPort = system_def.system_port;


	  // Store entity initial values into default_entities
	  m_default_entitydefs = new CdmwPlatformMngt::EntityDefs ();
	  unsigned int entityCount = system_def.entities.length ();
	  m_default_entitydefs->length (entityCount);

	  // initialise data store of the object
	  set_system_record_in_datastore ();

	  // register the parent path in entity container            
	  m_actualEntityContainer->setParentPath (m_element_path.in ());

	  // Add all the entities
	  entityCount = system_def.entities.length ();

	  for (unsigned int entityIndex = 0;
	       entityIndex < entityCount; entityIndex++)
	    {
	      // get entity definition (EntityDef is a structure, a reference on the
	      // structure is returned by the [] operator)
	      const CdmwPlatformMngt::EntityDef & entity_def =
		system_def.entities[entityIndex];

	      // store values of the entityDef
	      (*m_default_entitydefs)[entityIndex] = entity_def;

	      try
	      {
		m_rwEntityContainer->add_entity (entity_def);
	      }
	      catch (const CdmwPlatformMngt::EntityAlreadyExists &)
	      {
		throw CdmwPlatformMngt::DuplicateEntity (entity_def.
							 entity_name.in ());
	      }
	    }

	  // Add all the services
	  unsigned int serviceCount =
	    system_def.service_definitions.length ();

	  for (unsigned int serviceIndex = 0;
	       serviceIndex < serviceCount; serviceIndex++)
	    {
	      // get service definition (ServiceDef is a structure, a reference on the
	      // structure is returned by the [] operator)
	      const CdmwPlatformMngtService::ServiceDef & service_def =
		system_def.service_definitions[serviceIndex];

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

	  // add the hosts
	  unsigned int hostCount = system_def.hosts.length ();

	  // Check the validity of all the host definitions
	  for (unsigned int hostIndex = 0; hostIndex < hostCount; hostIndex++)
	    {
	      // get host definition (HostDef is a structure, a reference on the
	      // structure is returned by the [] operator)
	      const CdmwPlatformMngt::HostDef & host_def =
		system_def.hosts[hostIndex];

	      // Check there is no duplicate entity            
	      for (unsigned int entityIndex = 0;
		   entityIndex < host_def.entities.length (); entityIndex++)
		{
		  std::string entityName =
		    host_def.entities[entityIndex].entity_name.in ();

		  for (unsigned int entityIndex2 = entityIndex + 1;
		       entityIndex2 < host_def.entities.length ();
		       entityIndex2++)
		    {
		      std::string entityName2 =
			host_def.entities[entityIndex2].entity_name.in ();

		      if (entityName == entityName2)
			throw CdmwPlatformMngt::DuplicateHostEntity (host_def.
								     host_name.
								     in (),
								     entityName.
								     c_str
								     ());
		    }
		}
	    }

	  // All has been checked - add hosts
	  for (unsigned int hostIndex = 0; hostIndex < hostCount; hostIndex++)
	    {
	      // get host definition (HostDef is a structure, a reference on the
	      // structure is returned by the [] operator)
	      const CdmwPlatformMngt::HostDef & host_def =
		system_def.hosts[hostIndex];

	      try
	      {
		// add host in container
		CdmwPlatformMngt::Host_var host = addHost (host_def);
	      }
	      catch (const CdmwPlatformMngt::HostAlreadyExists &)
	      {
		throw CdmwPlatformMngt::DuplicateHost (host_def.host_name.
						       in ());
	      }
	    }


	  //add the properties to System

	  unsigned int propCount = system_def.properties.length ();

	  for (unsigned int propIndex = 0; propIndex < propCount; propIndex++)
	    {
	      const CdmwPlatformMngt::PropertyDef & prop_def =
		system_def.properties[propIndex];

	      try
	      {
		std::string propertyName (system_def.properties[propIndex].
					  property_name);
		CosPropertyService::PropertyModeType propertyMode =
		  CosPropertyService::PropertyModeType (0);
		CdmwPlatformMngt::PropertyModeType propertyModeCdmw =
		  system_def.properties[propIndex].mode_type;
		if (propertyModeCdmw == 1)
		  propertyMode = CosPropertyService::PropertyModeType (1);
		CORBA::Any any_val;
		define_property_with_mode (propertyName.c_str (),
					   prop_def.initial_value,
					   propertyMode);
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

	  // set system is defined
	  m_systemDefined = true;
	}
      else
	{
	  throw CdmwPlatformMngt::AlreadyDone ();
	}
    }




    void System_impl::set_system_record_in_datastore ()
    {
      // test if the supervision is not fault tolerant or if it is, is it the primary?
      if (!Configuration::Is_fault_tolerant ()
	  || Configuration::Is_primary ())
	{
	  CdmwPlatformMngt::SystemRecord record;
	  record.system_port = m_systemPort;
	  record.status = m_stateMachine->getState ()->status ();
	  record.status_info = m_statusInfo;
	  record.mode = m_mode;
	  record.mode_info = m_modeInfo;
	  record.state_machine =
	    CORBA::string_dup (m_stateMachine->getStateName ());
	  record.startup_kind = m_startup_kind.in ();
	  record.stop_emergency = m_emergency;
	  record.new_graph = m_newGraph;
	  record.application_init_graph = m_applicationInitGraph;
	  record.application_stop_graph = m_applicationStopGraph;
	  record.stop_graph = m_stopGraph;

	  if (m_system_ds->contains (m_element_path.in ()))
	    {
	      m_system_ds->update (m_element_path.in (), record);
	    }
	  else
	    {
	      set_element_in_datastore ();

	      m_system_ds->insert (m_element_path.in (), record);
	    }
	}
    }


    void System_impl::remove_system_record_in_datastore ()
    {
      // inhibition if backup
      if (!Configuration::Is_fault_tolerant ()
	  || Configuration::Is_primary ())
	{
	  try
	  {
	    m_system_ds->remove (m_element_path.in ());
	  }
	  catch (Common::NotFoundException &)
	  {
	  }
	}
    }


    CdmwPlatformMngt::SystemRecord *
      System_impl::get_system_record_from_datastore ()throw (Common::
							     NotFoundException)
    {
      CdmwPlatformMngt::SystemRecord * system_record = 0;

      try
      {
	system_record = m_system_ds->select (m_element_path.in ());
      }
      catch (Common::NotFoundException &)
      {
	std::cerr << "system record cannot be found in datastore: "
	  << m_element_path.in () << std::endl;
	throw;
      }
      catch (...)
      {
	std::
	  cerr <<
	  "Unexpected Exception raised: system record cannot be found in datastore : "
	  << m_element_path.in () << std::endl;
	CDMW_ASSERT (false);
      }

      return system_record;
    }


    void System_impl::
      set_observer_record_in_datastore (const char *observer_name,
					const char *host_name,
					CdmwPlatformMngt::
					SupervisionObserver_ptr observer)
    {
      if (Configuration::Is_primary ())
	{
	  CdmwPlatformMngt::SystemObserverRegistrationRecord record;
	  record.host_name = CORBA::string_dup (host_name);
	  record.observer =
	    CdmwPlatformMngt::SupervisionObserver::_duplicate (observer);

	  if (m_system_observer_ds->contains (observer_name))
	    {
	      m_system_observer_ds->update (observer_name, record);
	    }
	  else
	    {
	      m_system_observer_ds->insert (observer_name, record);
	    }
	}
    }


    void System_impl::
      remove_observer_record_in_datastore (const char *observer_name)
    {
      // inhibition if backup
      if (Configuration::Is_primary ())
	{
	  try
	  {
	    m_system_observer_ds->remove (observer_name);
	  }
	  catch (Common::NotFoundException &)
	  {
	  }
	}
    }


    void System_impl::synchro_with_datastore ()
    {
      // synchronise with system record
      try
      {

	Cdmw::PlatformMngt::ManagedElement_impl::synchro_with_datastore ();

	CdmwPlatformMngt::SystemRecord_var system_record =
	  get_system_record_from_datastore ();

	m_statusInfo = system_record->status_info;
	m_mode = system_record->mode;
	m_modeInfo = system_record->mode_info;

	m_stateMachine->setStateName (system_record->state_machine.in ());

	m_startup_kind =
	  new CdmwPlatformMngtBase::StartupKind (system_record->startup_kind);

	m_emergency = system_record->stop_emergency;
	m_newGraph = true;

	m_applicationInitGraph =
	  new CdmwPlatformMngt::GraphElements (system_record->
					       application_init_graph);

	m_applicationStopGraph =
	  new CdmwPlatformMngt::GraphElements (system_record->
					       application_stop_graph);

	m_stopGraph =
	  new CdmwPlatformMngt::GraphElements (system_record->stop_graph);

	// synchronise with the system observer records
	// to register the reference of observers
	m_system_observer_ds->for_each (ObserverDumper (this));
      }
      catch (const Common::NotFoundException &)
      {
	// Element not found, This append when the SystemManagement is the first primary
      }
    }

    void System_impl::synchro_processing ()
    {
      // if status Starting, Stopping restart the activity
      CdmwPlatformMngt::SystemStatus systemStatus =
	m_stateMachine->getState ()->status ();

      if (systemStatus == CdmwPlatformMngt::SYSTEM_STARTING ||
	  systemStatus == CdmwPlatformMngt::SYSTEM_STOPPING)
	{
	  std::
	    cout << "<<System_impl::synchro_processing() restart activity>> "
	    << std::endl;

	  m_stateMachine->restartStateActivity ();

	  std::
	    cout << "<<System_impl::synchro_processing() update datastore>> "
	    << std::endl;

	  // update data store of the object
	  set_system_record_in_datastore ();

	  std::
	    cout <<
	    "<<System_impl::synchro_processing() restart activity done>> " <<
	    std::endl;
	}
    }


    void System_impl::setInitGraph (const CdmwPlatformMngt::
				    GraphElements & application_graph)
      throw (OutOfMemoryException, CdmwPlatformMngt::ReferenceNotFound,
	     CdmwPlatformMngt::CircularReference, CORBA::SystemException)
    {
      try
      {
	CDMW_WRITER_LOCK_GUARD (m_rwSystemLock);

	// Check the validity of the init graph of applications

	// fill existing element map with application name (step is not used)
	ExistingElementMap existingElementMap;
	Applications::iterator applicationIt;

	{
	  for (applicationIt = m_applications.begin ();
	       applicationIt != m_applications.end (); applicationIt++)
	    {
	      // insert in map the application name
	      existingElementMap.
		insert (PlatformMngt::ExistingElementMap::
			value_type (applicationIt->first, 0));

	    }
	}

	GraphChecker applicationGraph (application_graph, &existingElementMap,
				       true);

	// Actually sets the graph of applications to this system
	// (the previous init graph is deleted by var mechanism)
	m_applicationInitGraph =
	  new CdmwPlatformMngt::GraphElements (application_graph);

	// set flag new graph
	m_newGraph = true;

	// TODO: Notify the configuration change

	// TODO: Trace to the log manager

	// update record in datastore
	set_system_record_in_datastore ();
      }
      catch (const CdmwPlatformMngt::StepOutOfBound &)
      {
	// nothing to do
      }
      catch (const CdmwPlatformMngt::StepJump &)
      {
	// nothing to do
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
    }


    void System_impl::setStopGraph (const CdmwPlatformMngt::
				    GraphElements & application_graph)
      throw (OutOfMemoryException, CdmwPlatformMngt::ReferenceNotFound,
	     CdmwPlatformMngt::CircularReference, CORBA::SystemException)
    {
      try
      {
	CDMW_WRITER_LOCK_GUARD (m_rwSystemLock);

	// Check the validity of the stop graph of applications

	// fill existing element map with application name (step is not used)
	ExistingElementMap existingElementMap;
	Applications::iterator applicationIt;

	{
	  for (applicationIt = m_applications.begin ();
	       applicationIt != m_applications.end (); applicationIt++)
	    {
	      // insert in map the application name
	      existingElementMap.
		insert (PlatformMngt::ExistingElementMap::
			value_type (applicationIt->first, 0));

	    }
	}

	GraphChecker applicationGraph (application_graph, &existingElementMap,
				       true);

	// Actually sets the graph of applications to this system
	// (the previous stop graph is deleted by var mechanism)
	m_applicationStopGraph =
	  new CdmwPlatformMngt::GraphElements (application_graph);

	// TODO: Notify the configuration change

	// TODO: Trace to the log manager

	// update record in datastore
	set_system_record_in_datastore ();
      }
      catch (const CdmwPlatformMngt::StepOutOfBound &)
      {
	// nothing to do
      }
      catch (const CdmwPlatformMngt::StepJump &)
      {
	// nothing to do
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
    }


    void System_impl::setRunSet (const CdmwPlatformMngt::
				 RunSetDef & run_set_def)
      throw (CdmwPlatformMngt::DuplicateRunSetElement, CORBA::SystemException)
    {
      try
      {
	m_runSetManager->setRunSet (run_set_def);
      }
      catch (Cdmw::OutOfResourcesException &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
    }


    void System_impl::removeRunSet (const char *run_set_name)
      throw (CdmwPlatformMngt::RunSetNotFound, CORBA::SystemException)
    {
      m_runSetManager->removeRunSet (run_set_name);
    }


    void System_impl::removeAllRunSets ()
    {
      m_runSetManager->removeAllRunSets ();
    }


    CdmwPlatformMngt::Host_ptr
      System_impl::addHost (const CdmwPlatformMngt::HostDef & host_def)
      throw (CdmwPlatformMngt::HostAlreadyExists,
	     CdmwPlatformMngt::DuplicateEntity,
	     CosPropertyService::MultipleExceptions, CORBA::SystemException)
    {
      // add host in container
      return m_actualHostContainer->addHost (host_def);
    }


    void System_impl::removeHost (const char *host_name)
      throw (CdmwPlatformMngt::HostNotFound, CORBA::SystemException)
    {
      // remove host from container
      m_actualHostContainer->removeHost (host_name);
    }

    Host_impl *System_impl::getHost (const char *host_name)
      throw (CdmwPlatformMngt::HostNotFound, CORBA::SystemException)
    {
      return m_actualHostContainer->getHost (host_name);
    }



    CdmwPlatformMngt::Application_ptr
      System_impl::addApplication (const CdmwPlatformMngt::
				   PlatformApplicationDef &
				   platform_application_def,
				   const CdmwPlatformMngt::
				   ApplicationDef & application_def)
      throw (CdmwPlatformMngt::ApplicationAlreadyExists,
	     CdmwPlatformMngt::HostNotFound,
	     CdmwPlatformMngt::DuplicateEntity,
	     CosPropertyService::MultipleExceptions,
	     CdmwPlatformMngt::DuplicateService,
	     CdmwPlatformMngt::DuplicateProcess,
	     CdmwPlatformMngt::ProcessHostNotFound,
	     CdmwPlatformMngt::DuplicateProcessEntity,
	     CdmwPlatformMngt::ProcessPropertyExceptions,
	     CdmwPlatformMngt::DuplicateProcessService,
	     CdmwPlatformMngt::DuplicateProcessStep,
	     CdmwPlatformMngt::DuplicateProcessCommand,
	     CdmwPlatformMngt::DuplicateProcessCommandArg,
	     CdmwPlatformMngt::DuplicateEntityCommand,
	     CdmwPlatformMngt::DuplicateEntityCommandArg,
	     CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      // add application according to centralised/distributed supervision
      return doAddApplication (platform_application_def, application_def);
    }


    void System_impl::removeApplication (const char *application_name)
      throw (CdmwPlatformMngt::ApplicationNotFound,
	     CdmwPlatformMngt::ApplicationStillActive, CORBA::SystemException)
    {
      // remove application according to centralised/distributed supervision
      doRemoveApplication (application_name);
    }




    void System_impl::updateApplications (const char *host_name) throw ()
    {
      Applications::iterator applicationIt;

      CDMW_READER_LOCK_GUARD (m_rwSystemLock);

      // update each application with faulty host name
      for (applicationIt = m_applications.begin ();
	   applicationIt != m_applications.end (); applicationIt++)
	{
	  try
	  {
	    ((applicationIt->second).m_applicationControl)->
	      update_application (host_name);
	  }
	  catch (...)
	  {
	  }
	}
    }



    CdmwPlatformMngt::ApplicationControl_ptr System_impl::
      get_application_control (const char *application_name)
      throw (CdmwPlatformMngt::ApplicationNotFound)
    {
      CDMW_READER_LOCK_GUARD (m_rwSystemLock);

      // search application in map
      Applications::iterator applicationIt =
	m_applications.find (application_name);

      // if application not found
      if (applicationIt == m_applications.end ())
	{
	  throw CdmwPlatformMngt::ApplicationNotFound ();
	}

      // return a duplicated object reference
      // (duplicate is implicit in var copy)
      CdmwPlatformMngt::ApplicationControl_var application =
	(applicationIt->second).m_applicationControl;

      return application._retn ();
    }

    CdmwPlatformMngt::ProcessObserver_ptr System_impl::
      get_process_observer (const char *application_name)
      throw (CdmwPlatformMngt::ApplicationNotFound)
    {
      CDMW_READER_LOCK_GUARD (m_rwSystemLock);

      // search application in map
      Applications::iterator applicationIt =
	m_applications.find (application_name);

      // if application not found
      if (applicationIt == m_applications.end ())
	{
	  throw CdmwPlatformMngt::ApplicationNotFound ();
	}

      // return a duplicated object reference
      // (duplicate is implicit in var copy)
      CdmwPlatformMngt::ProcessObserver_var process_observer =
	(applicationIt->second).m_processObserver;

      return process_observer._retn ();
    }


    void System_impl::degradation_event ()
    {
      CDMW_MUTEX_GUARD (m_mutexMode);

      CdmwPlatformMngt::FunctioningMode mode =
	CdmwPlatformMngt::DEGRADED_MODE;
      CORBA::String_var mode_info = CORBA::string_dup ("");

      set_mode (mode, mode_info.in ());

      // Notify the change of mode
      notifyModeChangeEvent (mode, mode_info.in ());
    }


    void System_impl::suspension_event ()
    {
      CDMW_SM_WRITER_LOCK_GUARD (*m_stateMachine);

      // send event to state machine for transition
      m_stateMachine->getState ()->suspension_event (m_stateMachine);

      // update record in datastore
      set_system_record_in_datastore ();
    }

    void System_impl::completion_event ()
    {
      CDMW_SM_WRITER_LOCK_GUARD (*m_stateMachine);

      // send event to state machine for transition
      m_stateMachine->getState ()->completion_event (m_stateMachine);

      // update record in datastore
      set_system_record_in_datastore ();
    }



    CdmwPlatformMngt::Port System_impl::systemPort ()
    {
      return m_systemPort;
    }

    CdmwPlatformMngt::SystemStatus System_impl::get_internal_status ()
    {
      CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);

      // get status
      CdmwPlatformMngt::SystemStatus systemStatus =
	m_stateMachine->getState ()->status ();

      return systemStatus;
    }

    CdmwPlatformMngt::SystemStatus System_impl::get_status (CORBA::
							    String_out
							    status_info)
      throw (CORBA::SystemException)
    {
      try
      {
	CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);

	CdmwPlatformMngt::SystemStatus system_status;

	if (!m_systemDefined)
	  {
	    // get status from state machine
	    system_status = m_stateMachine->getState ()->status ();

	    status_info = CORBA::string_dup ("");
	  }

	else
	  {
	    // get record                
	    CdmwPlatformMngt::SystemRecord_var system_record =
	      get_system_record_from_datastore ();

	    // get status
	    system_status = system_record->status;

	    // get info associated to status
	    status_info =
	      CORBA::string_dup ((system_record->status_info).in ());
	  }

	return system_status;
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

    void System_impl::get_attribute (const char *attr_name,
				     CORBA::Any_out ret_attr,
				     CORBA::Char flag) throw (CORBA::
							      SystemException,
							      CdmwPlatformMngt::
							      AttributeNotFound)
    {

      char flag_found = 0;
      std::string tmp1 (attr_name);
      /*active the right methods */

      if (flag == 0)
	{			/*case of stardard attribute */
	  if (tmp1 == "mode")
	    {

	      flag_found = 1;
	      /*CdmwPlatformMngt::FunctioningMode * System_impl::get_mode(CORBA::String_out mode_info) */

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

	  /*status */
	  if (tmp1 == "status")
	    {
	      flag_found = 1;
	      CdmwPlatformMngt::SystemStatus out_status;
	      CORBA::String_var status_info;

	      CORBA::Any_var tempany = new CORBA::Any;

	      out_status = get_status (status_info);
	      tempany.inout () <<= out_status;
	      ret_attr = tempany._retn ();
	    }


	  /*status_info */
	  if (tmp1 == "status_info")
	    {
	      flag_found = 1;
	      CdmwPlatformMngt::SystemStatus out_status;
	      CORBA::String_var status_info;

	      CORBA::Any_var tempany = new CORBA::Any;

	      out_status = get_status (status_info);
	      tempany.inout () <<= status_info;
	      ret_attr = tempany._retn ();
	    }

	  if (flag_found == 0)
	    throw CdmwPlatformMngt::AttributeNotFound ();

	}
      else
	{			/*case of user defined attribute */
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

    void System_impl::set_attribute (const char *attr_name,
				     const CORBA::Any & attr,
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
	  if (tmp1 == "mode")
	    {
	      flag_found = 1;
	      /*set actual mode */
	      CdmwPlatformMngt::FunctioningMode func_mode1;
	      CORBA::String_var act_info;

	      func_mode1 = get_mode (act_info);
	      CdmwPlatformMngt::FunctioningMode modeset;
	      attr >>= modeset;

	      set_mode (modeset, act_info);
	    }

	  if (tmp1 == "mode_info")
	    {
	      flag_found = 1;
	      /*set actual mode */
	      CdmwPlatformMngt::FunctioningMode func_mode1;
	      CORBA::String_var act_info;

	      func_mode1 = get_mode (act_info);
	      char *mode_info;

	      attr >>= CORBA::Any::to_string (mode_info, 0);

	      set_mode (func_mode1, mode_info);
	    }

	  if (flag_found == 0)
	    throw CdmwPlatformMngt::AttributeNotFound ();
	}
      else
	{
	  /* case of user defined attributes */
	  define_property (attr_name, attr);
	}

    }

	/**
	 *Purpose:
	 *<p> Returns the system_mission
	 *
	 */

    char *System_impl::get_system_mission () throw (CORBA::SystemException)
    {
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    };

	/**
	 *Purpose:
	 *<p> to set the system_mission attribute
	 *
	 */

    void System_impl::
      set_system_mission (const char *system_mission) throw (CORBA::
							     SystemException)
    {
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    };

	/**
	 *Purpose:
	 *<p> to get system_name
	 *
	 */

    char *System_impl::get_system_name () throw (CORBA::SystemException)
    {
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    };


	/**
	 *Purpose:
	 *<p> to set system_name
	 *
	 */

    void System_impl::set_system_name (const char *system_name) throw (CORBA::
								       SystemException)
    {
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    };

	/**
	 *Purpose:
	 *<p> to get master_host
	 *
	 */

    char *System_impl::get_master_host () throw (CORBA::SystemException)
    {
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    };


    CdmwPlatformMngt::FunctioningMode System_impl::get_mode (CORBA::
							     String_out
							     mode_info)
      throw (CORBA::SystemException)
    {
      try
      {
	CdmwPlatformMngt::FunctioningMode functioning_mode;

	if (!m_systemDefined)
	  {
	    mode_info = CORBA::string_dup ("");
	    functioning_mode = CdmwPlatformMngt::NORMAL_MODE;
	  }

	else
	  {
	    // get record
	    CdmwPlatformMngt::SystemRecord_var system_record =
	      get_system_record_from_datastore ();

	    // get mode
	    functioning_mode = system_record->mode;

	    // get info associated to mode
	    mode_info = CORBA::string_dup ((system_record->mode_info).in ());
	  }

	return functioning_mode;
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
    }


    void System_impl::set_mode (CdmwPlatformMngt::
				FunctioningMode functioning_mode,
				const char *mode_info)
    {
      m_mode = functioning_mode;
      m_modeInfo = CORBA::string_dup (mode_info);

      set_system_record_in_datastore ();
    }


    CdmwPlatformMngt::SystemSnapshot *
      System_impl::get_snapshot ()throw (CORBA::SystemException)
    {
      // Create a system snapshot
      try
      {
	CDMW_READER_LOCK_GUARD (m_rwSystemLock);

	// Get the snapshot of all hosts
	CdmwPlatformMngt::HostsSnapshot_var hostsSnapshot =
	  m_actualHostContainer->getSnapshot ();

	// Get the snapshot of all applications
	CdmwPlatformMngt::ApplicationsSnapshot_var applicationsSnapshot
	  = new CdmwPlatformMngt::ApplicationsSnapshot ();

	applicationsSnapshot->length (m_applications.size ());
	Applications::iterator applicationIt;
	unsigned int i;

	for (applicationIt = m_applications.begin (), i = 0;
	     applicationIt != m_applications.end (); applicationIt++, i++)
	  {
	    applicationsSnapshot[i] =
	      *(((applicationIt->second).m_application)->get_snapshot ());
	  }

	// Get the snapshot of all entities of system
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
	// Create a system snapshot
	CdmwPlatformMngt::SystemSnapshot_var snapshot =
	  new CdmwPlatformMngt::SystemSnapshot;

	// note : the copy of a snapshot sequence (= operator) 
	//        make a deeply copy with a new memory allocation
	//        for the sequence 

	snapshot->system_status = get_status ((snapshot->status_info).out ());
	snapshot->system_mode = get_mode ((snapshot->mode_info).out ());
	snapshot->system_hosts = hostsSnapshot.in ();
	snapshot->system_applications = applicationsSnapshot.in ();
	snapshot->system_entities = entitiesSnapshot.in ();
	snapshot->system_properties = propertiesSnapshot.in ();
	return snapshot._retn ();
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
      catch (const Cdmw::OutOfResourcesException &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
    }


    CORBA::ULong System_impl::get_number_of_applications ()throw (CORBA::
								  SystemException)
    {

      CORBA::ULong num_apps = m_applications.size ();

      return num_apps;
    }

    CdmwPlatformMngt::Applications *
      System_impl::get_applications (CORBA::ULong how_many,
				     CdmwPlatformMngt::
				     ApplicationsIterator_out
				     applications_iterator) throw (CORBA::
								   SystemException)
    {
      try
      {
	CDMW_READER_LOCK_GUARD (m_rwSystemLock);

	Applications::iterator applicationIt;

	// create sequence
	CdmwPlatformMngt::Applications_var applications =
	  new CdmwPlatformMngt::Applications ();

	// Get the how_many application references   
	size_t len = m_applications.size ();

	// set length of requested sequence and rest of sequence
	size_t requested_len = len;
	size_t rest_len = 0;

	// adjust to how_many requested       
	if (len > how_many)
	  {
	    requested_len = how_many;
	    rest_len = len - how_many;
	  }

	// set sequence length 
	applications->length (requested_len);

	// store requested application reference objects         
	unsigned int count;
	for (applicationIt = m_applications.begin (), count = 0;
	     count < requested_len; applicationIt++, count++)
	  {
	    // Store application reference in sequence (duplicate is processed 
	    // during var copy)
	    applications[count] = (applicationIt->second).m_application;
	  }

	// Get an iterator on the rest of the applications
	std::list < CdmwPlatformMngt::Application_var > applicationsRemaining;
	for (; applicationIt != m_applications.end (); applicationIt++)
	  {
	    // Store remaining application reference in list (duplicate is processed 
	    // during var copy)

	    applicationsRemaining.push_back ((applicationIt->second).
					     m_application);
	  }

	CdmwPlatformMngt::ApplicationsIterator_var applicationsRemainingIt =
	  ApplicationsIterator_impl::create (applicationsRemaining);

	// Assign the iterator to the processes_iterator
	applications_iterator = applicationsRemainingIt._retn ();

	return applications._retn ();
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
    }





    CdmwPlatformMngt::Application_ptr System_impl::
      get_application (const char *application_name) throw (CdmwPlatformMngt::
							    ApplicationNotFound,
							    CORBA::
							    SystemException)
    {
      CDMW_READER_LOCK_GUARD (m_rwSystemLock);

      // search application in map
      Applications::iterator applicationIt =
	m_applications.find (application_name);

      // if application not found
      if (applicationIt == m_applications.end ())
	{
	  throw CdmwPlatformMngt::ApplicationNotFound ();
	}

      // return a duplicated object reference
      // (duplicate is implicit in var copy)
      CdmwPlatformMngt::Application_var application =
	(applicationIt->second).m_application;
      return application._retn ();
    }


    CdmwPlatformMngt::SupervisionObserver_ptr
      System_impl::register_observer (const char *observer_name,
				      CdmwPlatformMngt::
				      SupervisionObserver_ptr observer)
      throw (CORBA::SystemException)
    {
      try
      {
	LogMngr::logMessage (INF, MSG_ID_SYS_REG_OBSERVER,
			     observer_name, m_element_name.in ());

	CdmwPlatformMngt::SupervisionObserver_var previousObserver =
	  m_eventNotifier->registerObserver (observer_name, observer);

	// store observer in datastore
	set_observer_record_in_datastore (observer_name, "", observer);

	return previousObserver._retn ();
      }
      catch (const BadParameterException &)
      {
	throw CORBA::BAD_PARAM (OrbSupport::BAD_PARAM, CORBA::COMPLETED_NO);
      }
      catch (const EventNotifierStoppedException &)
      {
	throw CORBA::INTERNAL (OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
      }
      catch (const OutOfMemoryException &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
    }

    CdmwPlatformMngt::SupervisionObserver_ptr
      System_impl::register_proxy_observer (const char *observer_name,
					    const char *host_name,
					    CdmwPlatformMngt::
					    SupervisionObserver_ptr observer)
      throw (CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }

    CdmwPlatformMngt::SupervisionObserver_ptr
      System_impl::unregister_observer (const char *observer_name)
      throw (CdmwPlatformMngt::ObserverNotFound, CORBA::SystemException)
    {
      try
      {
	LogMngr::logMessage (INF, MSG_ID_SYS_UNREG_OBSERVER,
			     observer_name, m_element_name.in ());

	CdmwPlatformMngt::SupervisionObserver_var observer =
	  m_eventNotifier->unregisterObserver (observer_name);

	// remove observer in datastore
	remove_observer_record_in_datastore (observer_name);

	return observer._retn ();
      }
      catch (const EventNotifierStoppedException &)
      {
	throw CORBA::INTERNAL (OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
      }
    }


    void System_impl::define (const CdmwPlatformMngt::SystemDef & system_def)
      throw (CdmwPlatformMngt::DuplicateEntity,
	     CosPropertyService::MultipleExceptions,
	     CdmwPlatformMngt::DuplicateService,
	     CdmwPlatformMngt::DuplicateHost,
	     CdmwPlatformMngt::DuplicateHostEntity,
	     CdmwPlatformMngt::HostPropertyExceptions,
	     CdmwPlatformMngt::AlreadyDone,
	     CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      LogMngr::logMessage (INF, MSG_ID_DEFINE_SYSTEM,
			   system_def.system_name.in ());

      CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);

      m_stateMachine->getState ()->define (m_stateMachine, system_def);
    }


    void System_impl::set_init_graph (const CdmwPlatformMngt::
				      GraphElements & application_graph)
      throw (CdmwPlatformMngt::ReferenceNotFound,
	     CdmwPlatformMngt::CircularReference,
	     CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      LogMngr::logMessage (INF, MSG_ID_SET_INITGRAPH, m_element_name.in ());

      CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);

      m_stateMachine->getState ()->set_init_graph (m_stateMachine,
						   application_graph);
    }


    void System_impl::set_stop_graph (const CdmwPlatformMngt::
				      GraphElements & application_graph)
      throw (CdmwPlatformMngt::ReferenceNotFound,
	     CdmwPlatformMngt::CircularReference,
	     CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      LogMngr::logMessage (INF, MSG_ID_SET_STOPGRAPH, m_element_name.in ());

      CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);

      m_stateMachine->getState ()->set_stop_graph (m_stateMachine,
						   application_graph);
    }



    void System_impl::set_run_set (const CdmwPlatformMngt::
				   RunSetDef & run_set_def)
      throw (CdmwPlatformMngt::DuplicateRunSetElement,
	     CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      LogMngr::logMessage (INF, MSG_ID_SET_RUNSET, m_element_name.in ());

      CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);

      m_stateMachine->getState ()->set_run_set (m_stateMachine, run_set_def);
    }

    void System_impl::remove_run_set (const char *run_set_name)
      throw (CdmwPlatformMngt::RunSetNotFound,
	     CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      LogMngr::logMessage (INF, MSG_ID_REMOVE_RUNSET, m_element_name.in ());

      CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);

      m_stateMachine->getState ()->remove_run_set (m_stateMachine,
						   run_set_name);
    }

    void System_impl::remove_all_run_sets ()
      throw (CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      LogMngr::logMessage (INF, MSG_ID_REMOVE_ALL_RUNSET,
			   m_element_name.in ());

      CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);

      m_stateMachine->getState ()->remove_all_run_sets (m_stateMachine);
    }

    CdmwPlatformMngt::Host_ptr
      System_impl::add_host (const CdmwPlatformMngt::HostDef & host_def)
      throw (CdmwPlatformMngt::HostAlreadyExists,
	     CdmwPlatformMngt::DuplicateEntity,
	     CosPropertyService::MultipleExceptions,
	     CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      std::string hostName = host_def.host_name.in ();

      LogMngr::logMessage (INF, MSG_ID_ADD_HOST, hostName.c_str (),
			   m_element_name.in ());

      CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);

      return m_stateMachine->getState ()->add_host (m_stateMachine, host_def);
    }

    void System_impl::remove_host (const char *host_name)
      throw (CdmwPlatformMngt::HostNotFound,
	     CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      LogMngr::logMessage (INF, MSG_ID_REMOVE_HOST, host_name,
			   m_element_name.in ());

      CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);

      m_stateMachine->getState ()->remove_host (m_stateMachine, host_name);
    }


    CdmwPlatformMngt::Application_ptr
      System_impl::add_application (const CdmwPlatformMngt::
				    PlatformApplicationDef &
				    platform_application_def,
				    const CdmwPlatformMngt::
				    ApplicationDef & application_def)
      throw (CdmwPlatformMngt::ApplicationAlreadyExists,
	     CdmwPlatformMngt::HostNotFound,
	     CdmwPlatformMngt::DuplicateEntity,
	     CosPropertyService::MultipleExceptions,
	     CdmwPlatformMngt::DuplicateService,
	     CdmwPlatformMngt::DuplicateProcess,
	     CdmwPlatformMngt::ProcessHostNotFound,
	     CdmwPlatformMngt::DuplicateProcessEntity,
	     CdmwPlatformMngt::ProcessPropertyExceptions,
	     CdmwPlatformMngt::DuplicateProcessService,
	     CdmwPlatformMngt::DuplicateProcessStep,
	     CdmwPlatformMngt::DuplicateProcessCommand,
	     CdmwPlatformMngt::DuplicateProcessCommandArg,
	     CdmwPlatformMngt::DuplicateEntityCommand,
	     CdmwPlatformMngt::DuplicateEntityCommandArg,
	     CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      std::string applicationName =
	platform_application_def.application_name.in ();
      LogMngr::logMessage (INF, MSG_ID_ADD_APPLICATION,
			   applicationName.c_str (), m_element_name.in ());

      bool systemDefined = false;
      {
	CDMW_READER_LOCK_GUARD (m_rwSystemLock);
	systemDefined = m_systemDefined;
      }

      // check if system is defined
      if (systemDefined)
	{
	  CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);

	  return m_stateMachine->getState ()->add_application (m_stateMachine,
							       platform_application_def,
							       application_def);
	}
      else
	{
	  throw CdmwPlatformMngt::IncompatibleStatus ();
	}
    }

    void System_impl::remove_application (const char *application_name)
      throw (CdmwPlatformMngt::ApplicationNotFound,
	     CdmwPlatformMngt::ApplicationStillActive,
	     CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      LogMngr::logMessage (INF, MSG_ID_REMOVE_APPLICATION,
			   application_name, m_element_name.in ());

      CDMW_SM_READER_LOCK_GUARD (*m_stateMachine);

      return m_stateMachine->getState ()->remove_application (m_stateMachine,
							      application_name);
    }

    void System_impl::start (const CdmwPlatformMngtBase::
			     StartupKind & startup_kind)
      throw (CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      LogMngr::logMessage (INF, MSG_ID_SYS_START, m_element_name.in ());

      // check if system is defined
      bool systemDefined = false;
      {
	CDMW_READER_LOCK_GUARD (m_rwSystemLock);
	systemDefined = m_systemDefined;
      }

      if (systemDefined)
	{
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
		m_rwEntityContainer->
		  add_entity ((*m_default_entitydefs)[count]);
	      }

	    m_stateMachine->getState ()->start (m_stateMachine, startup_kind);
	  }
	  catch (...)
	  {
	    // even in case of exception update record in datastore
	    set_system_record_in_datastore ();

	    // rethrow exception
	    throw;
	  }

	  // update record in datastore
	  set_system_record_in_datastore ();
	}
      else
	{
	  throw CdmwPlatformMngt::IncompatibleStatus ();
	}
    }


    void System_impl::resume ()
      throw (CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      LogMngr::logMessage (INF, MSG_ID_SYS_RESUME, m_element_name.in ());

      CDMW_SM_WRITER_LOCK_GUARD (*m_stateMachine);

      try
      {
	m_stateMachine->getState ()->resume (m_stateMachine);
      }
      catch (...)
      {
	// even in case of exception update record in datastore
	set_system_record_in_datastore ();

	// rethrow exception
	throw;
      }

      // update record in datastore
      set_system_record_in_datastore ();
    }


    void System_impl::acknowledge ()
      throw (CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      LogMngr::logMessage (INF, MSG_ID_SYS_ACK, m_element_name.in ());

      CDMW_MUTEX_GUARD (m_mutexMode);

      CdmwPlatformMngt::FunctioningMode mode = CdmwPlatformMngt::NORMAL_MODE;
      CORBA::String_var mode_info = CORBA::string_dup ("");

      set_mode (mode, mode_info.in ());

      // Notify the change of mode
      notifyModeChangeEvent (mode, mode_info.in ());
    }


    void System_impl::stop (CORBA::Boolean emergency)
      throw (CORBA::SystemException)
    {
      try
      {
	LogMngr::logMessage (INF, MSG_ID_SYS_STOP, m_element_name.in ());

	// create graph element factory
#if CDMW_ORB_VDR == tao
	GraphElementFactory factory;
#else
	CORBA::ValueFactoryBase_var factory = new GraphElementFactory ();
#endif

	CDMW_SM_WRITER_LOCK_GUARD (*m_stateMachine);

	// emergency stop
	if (emergency)
	  {
	    // empty the actual emergency stop graph
	    m_stopGraph->length (0);

	    // set reader lock for application protection
	    CDMW_READER_LOCK_GUARD (m_rwSystemLock);

	    // Add all the active applications as roots of the stop graph          
	    Applications::iterator applicationIt;

	    for (applicationIt = m_applications.begin ();
		 applicationIt != m_applications.end (); applicationIt++)
	      {
		CdmwPlatformMngt::ApplicationControl_var application_control =
		  (applicationIt->second).m_applicationControl;

		CdmwPlatformMngt::Application_var application =
		  (applicationIt->second).m_application;

		// get application status

		/***
                CORBA::String_var info;
                CdmwPlatformMngt::ApplicationStatus appStatus = 
                                 application->get_status(info.out());
                ***/

		CdmwPlatformMngt::ApplicationStatus appStatus =
		  application_control->get_internal_status ();

		if ((appStatus == CdmwPlatformMngt::APPLICATION_INITIALISING)
		    || (appStatus ==
			CdmwPlatformMngt::APPLICATION_INITIALISED)
		    || (appStatus ==
			CdmwPlatformMngt::
			APPLICATION_INITIALISATION_INCOMPLETE)
		    || (appStatus ==
			CdmwPlatformMngt::
			APPLICATION_INITIALISATION_SUSPENDED)
		    || (appStatus ==
			CdmwPlatformMngt::APPLICATION_RUN_REQUEST)
		    || (appStatus == CdmwPlatformMngt::APPLICATION_RUNNING))
		  {
		    // create graph element using factory
		    CORBA::String_var appliName = application->name ();
#if CDMW_ORB_VDR == tao
		    CdmwPlatformMngt::GraphElement_var element =
		      factory.create (appliName.in (), 0);
#else
		    CdmwPlatformMngt::GraphElement_var element =
		      dynamic_cast < GraphElementFactory * >(factory.in ())->
		      create (appliName.in (), 0);
#endif

		    // insert new element in graph
		    int graphlen = m_stopGraph->length ();
		    m_stopGraph->length (graphlen + 1);
		    m_stopGraph->operator[](graphlen) = element;
		  }
	      }
	  }

	else
	  {
	    // Get the list of all element of the stop graph
	    GraphElementNames elementNames =
	      GraphUtility::getElementNames (m_applicationStopGraph.in ());


	    // create a new stop graph copy of the original
	    m_stopGraph = m_applicationStopGraph;

	    // set reader lock for application protection
	    CDMW_READER_LOCK_GUARD (m_rwSystemLock);

	    // Add all the active applications as roots of the stop graph
	    // if not present in the stop graph.
	    Applications::iterator applicationIt;

	    for (applicationIt = m_applications.begin ();
		 applicationIt != m_applications.end (); applicationIt++)
	      {
		CdmwPlatformMngt::ApplicationControl_var application_control =
		  (applicationIt->second).m_applicationControl;

		CdmwPlatformMngt::Application_var application =
		  (applicationIt->second).m_application;

		// if application not found in the stop graph  
		if (elementNames.find (applicationIt->first) ==
		    elementNames.end ())
		  {
		    // get application status

		    /***
                    CORBA::String_var info;
                    CdmwPlatformMngt::ApplicationStatus appStatus = 
                                 application->get_status(info.out());
                    ***/

		    CdmwPlatformMngt::ApplicationStatus appStatus =
		      application_control->get_internal_status ();

		    // if application candidate to be stopping        
		    if ((appStatus ==
			 CdmwPlatformMngt::APPLICATION_INITIALISING)
			|| (appStatus ==
			    CdmwPlatformMngt::APPLICATION_INITIALISED)
			|| (appStatus ==
			    CdmwPlatformMngt::
			    APPLICATION_INITIALISATION_INCOMPLETE)
			|| (appStatus ==
			    CdmwPlatformMngt::
			    APPLICATION_INITIALISATION_SUSPENDED)
			|| (appStatus ==
			    CdmwPlatformMngt::APPLICATION_RUN_REQUEST)
			|| (appStatus ==
			    CdmwPlatformMngt::APPLICATION_RUNNING))
		      {
			// create graph element from application name using factory
			CORBA::String_var appliName = application->name ();
#if CDMW_ORB_VDR == tao
			CdmwPlatformMngt::GraphElement_var element =
			  factory.create (appliName.in (), 0);
#else
			CdmwPlatformMngt::GraphElement_var element =
			  dynamic_cast <
			  GraphElementFactory *
			  >(factory.in ())->create (appliName.in (), 0);
#endif

			// insert new application element in graph
			int graphlen = m_stopGraph->length ();
			m_stopGraph->length (graphlen + 1);
			m_stopGraph->operator[](graphlen) = element;
		      }
		  }
	      }
	  }

	// request stop      
	try
	{
	  m_emergency = emergency;
	  m_stateMachine->getState ()->stop (m_stateMachine, emergency);
	}
	catch (...)
	{
	  // even in case of exception update record in datastore
	  set_system_record_in_datastore ();

	  // rethrow exception
	  throw;
	}

	// update record in datastore
	set_system_record_in_datastore ();
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
    }


    void System_impl::cold_restart (const CdmwPlatformMngtBase::
				    StartupKind & startup_kind)
      throw (CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }


    void System_impl::hot_restart (const CdmwPlatformMngtBase::
				   StartupKind & startup_kind)
      throw (CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }


    CORBA::ULong System_impl::get_number_of_entities ()throw (CORBA::
							      SystemException)
    {
      return m_rwEntityContainer->get_number_of_entities ();
    }

    void System_impl::get_all_entity_names (CORBA::ULong how_many,
					    CdmwPlatformMngt::
					    ElementNames_out entity_names,
					    CdmwPlatformMngt::
					    ElementNamesIterator_out rest)
      throw (CORBA::SystemException)
    {

      m_rwEntityContainer->get_all_entity_names (how_many, entity_names,
						 rest);

    }


    CdmwPlatformMngt::Entity_ptr System_impl::
      get_entity (const char *entity_name) throw (CdmwPlatformMngt::
						  EntityNotFound,
						  CORBA::SystemException)
    {
      return m_rwEntityContainer->get_entity (entity_name);
    }

    CORBA::Boolean System_impl::get_entities (const CdmwPlatformMngt::
					      ElementNames & entity_names,
					      CdmwPlatformMngt::
					      Entities_out nentities)
      throw (CORBA::SystemException)
    {

      return m_rwEntityContainer->get_entities (entity_names, nentities);

    }

    void System_impl::get_all_entities (CORBA::ULong how_many,
					CdmwPlatformMngt::
					Entities_out nentities,
					CdmwPlatformMngt::
					EntitiesIterator_out rest)
      throw (CORBA::SystemException)
    {

      m_rwEntityContainer->get_all_entities (how_many, nentities, rest);

    }

    CdmwPlatformMngt::Entity_ptr System_impl::
      add_entity (const CdmwPlatformMngt::
		  EntityDef & entity_def) throw (CdmwPlatformMngt::
						 EntityAlreadyExists,
						 CORBA::SystemException)
    {
      std::string entityName = entity_def.entity_name.in ();

      LogMngr::logMessage (INF, MSG_ID_ADD_SYSTEM_ENTITY,
			   entityName.c_str (), m_element_name.in ());

      return m_rwEntityContainer->add_entity (entity_def);

      // TODO: Notify the configuration change

    }


    void System_impl::remove_entity (const char *entity_name)
      throw (CdmwPlatformMngt::EntityNotFound, CORBA::SystemException)
    {
      LogMngr::logMessage (INF, MSG_ID_REMOVE_SYSTEM_ENTITY,
			   entity_name, m_element_name.in ());

      m_rwEntityContainer->remove_entity (entity_name);

      // TODO: Notify the configuration change

    }



    CdmwPlatformMngtService::ServiceProviderID *
      System_impl::
      get_service_def (const char *service_name)
      throw (CdmwPlatformMngtService::ServiceNotFound, CORBA::SystemException)
    {
      return m_rwServiceDefContainer->get_service_def (service_name);
    }

    void System_impl::add_service_def (const CdmwPlatformMngtService::
				       ServiceDef & service_def)
      throw (CdmwPlatformMngtService::ServiceAlreadyExists,
	     CORBA::SystemException)
    {
      m_rwServiceDefContainer->add_service_def (service_def);

      // TODO: Notify the configuration change

      // TODO: Trace to the log manager
    }

    void System_impl::remove_service_def (const char *service_name)
      throw (CdmwPlatformMngtService::ServiceNotFound, CORBA::SystemException)
    {
      m_rwServiceDefContainer->remove_service_def (service_name);

      // TODO: Notify the configuration change

      // TODO: Trace to the log manager
    }

    void System_impl::remove_all_service_defs ()
      throw (CORBA::SystemException)
    {
      m_rwServiceDefContainer->remove_all_service_defs ();

      // TODO: Notify the configuration change

      // TODO: Trace to the log manager
    }


    CORBA::ULong System_impl::get_number_of_hosts ()throw (CORBA::
							   SystemException)
    {
      return m_hostContainer->get_number_of_hosts ();
    }



    void System_impl::get_all_host_names (CORBA::ULong how_many,
					  CdmwPlatformMngt::
					  ElementNames_out host_names,
					  CdmwPlatformMngt::
					  ElementNamesIterator_out rest)
      throw (CORBA::SystemException)
    {
      m_hostContainer->get_all_host_names (how_many, host_names, rest);
    }



    CdmwPlatformMngt::Host_ptr System_impl::get_host (const char *host_name)
      throw (CdmwPlatformMngt::HostNotFound, CORBA::SystemException)
    {
      return m_hostContainer->get_host (host_name);
    }



    CORBA::Boolean System_impl::get_hosts (const CdmwPlatformMngt::
					   ElementNames & host_names,
					   CdmwPlatformMngt::
					   Hosts_out hosts) throw (CORBA::
								   SystemException)
    {
      return m_hostContainer->get_hosts (host_names, hosts);
    }



    void System_impl::get_all_hosts (CORBA::ULong how_many,
				     CdmwPlatformMngt::Hosts_out hosts,
				     CdmwPlatformMngt::HostsIterator_out rest)
      throw (CORBA::SystemException)
    {
      m_hostContainer->get_all_hosts (how_many, hosts, rest);
    }



    RunSetManager *System_impl::get_runset_manager ()
    {
      return m_runSetManager;
    }


    EventHandler *System_impl::get_event_handler ()
    {
      return &m_eventHandler;
    }

    SupervisionEventNotifier *System_impl::get_event_notifier ()
    {
      return m_eventNotifier;
    }




  }				// End namespace PlatformMngt
}				// End namespace Cdmw
