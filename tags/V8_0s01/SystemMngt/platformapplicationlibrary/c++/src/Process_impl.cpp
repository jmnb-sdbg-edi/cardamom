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


#include "platformapplicationlibrary/Process_impl.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtDaemon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"
#include "SystemMngt/platformvaluetypes/EntityStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ProcessStatusChange_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/ApplicationLogMessageId.hpp"
#include <set>

namespace Cdmw
{
  namespace PlatformMngt
  {

    Process_impl::Process_impl (PortableServer::POA_ptr poa,
				CdmwPlatformMngt::ProcessDef * process_def,
				const char *process_key,
				const char *process_path,
				Application_impl * application,
				CdmwPlatformMngt::
				RWElementRepository_ptr
				element_repository):DeactivableServant_impl
      (poa), ManagedElement_impl (element_repository)
    {
      if (application == NULL)
	CDMW_THROW2 (BadParameterException, "application", "NULL");

      m_procEntityStatusChangeFactory = NULL;
      m_procStatusChangeFactory = NULL;

      try
      {
	// Sets the process's internal data
	std::auto_ptr < ProcessEntityStatusChangeFactory >
	  procEntityStatusChangeFactory (new
					 ProcessEntityStatusChangeFactory ());
	std::auto_ptr < ProcessStatusChangeFactory >
	  procStatusChangeFactory (new ProcessStatusChangeFactory ());

	m_procEntityStatusChangeFactory =
	  procEntityStatusChangeFactory.get ();
	m_procStatusChangeFactory = procStatusChangeFactory.get ();

	m_application = application;
	m_type = process_def->type ();
	m_autoending = process_def->auto_ending ();

	m_process_info = new CdmwPlatformMngt::ProcessInfo ();
	m_process_info->process_exe = CORBA::string_dup ("");
	m_process_info->process_port = 0;
	m_process_info->working_directory = CORBA::string_dup ("");
	m_process_info->process_args = CORBA::string_dup ("");

	m_lifeCycle = new CdmwPlatformMngt::LifeCycleDef ();
	m_lifeCycle->init_steps.length (0);
	m_lifeCycle->run_timeout = 0;
	m_lifeCycle->stop_timeout = 0;

	m_monitoring_info = new CdmwPlatformMngt::CompleteMonitoringInfo ();
	m_monitoring_info->monitoring_model = CdmwPlatformMngt::NO_MONITORING;
	m_monitoring_info->pull_monitoring_interval = 0;
	m_monitoring_info->pull_monitoring_timeout = 0;
	m_monitoring_info->push_monitoring_interval = 0;
	m_monitoring_info->push_monitoring_timeout = 0;

	m_status_info = CORBA::string_dup ("");
	m_stepCounter = 0;

	m_host = CdmwPlatformMngt::Host::_nil ();


	// store element path in managed element
	set_element_path (process_path, process_def->name ());

	// memo the different names : process, host and process key
	m_proc_syst_name =
	  CORBA::string_dup (m_application->get_system_name ());
	m_proc_appli_name =
	  CORBA::string_dup (m_application->get_element_name ());
	m_proc_host_name = CORBA::string_dup (process_def->host_name ());
	m_proc_key = CORBA::string_dup (process_key);

	// Release the ownership of the temporary objects
	procEntityStatusChangeFactory.release ();
	procStatusChangeFactory.release ();
      }
      catch (const std::bad_alloc &)
      {
	CDMW_THROW (OutOfMemoryException);
      }

      try
      {
	Cdmw::CommonSvcs::DataStore::DataStoreBase * ds =
	  Cdmw::CommonSvcs::DataStore::DataStoreRegistry::
	  Get_data_store (Configuration::Get_process_datastore_id ());
	m_process_ds =
	  dynamic_cast < Cdmw::PlatformMngt::ProcessDataStore * >(ds);
      }
      catch (const NotExistException & e)
      {
	CDMW_ASSERT (false);
      }
    }


    Process_impl::~Process_impl ()
    {
      if (m_procEntityStatusChangeFactory != NULL)
	{
	  m_procEntityStatusChangeFactory->_remove_ref ();
	}

      if (m_procStatusChangeFactory != NULL)
	{
	  m_procStatusChangeFactory->_remove_ref ();
	}

      // remove record from datastore
      remove_record_in_datastore ();

    }




    void Process_impl::set_record_in_datastore ()
    {
      // test if the supervision is not fault tolerant or if it is, is it the primary?
      if (!Configuration::Is_fault_tolerant ()
	  || Configuration::Is_primary ())
	{
	  if (Configuration::Is_synchronising ())
	    {
	      std::
		cout << "<<Process_impl::set_record_in_datastore() start >> "
		<< m_element_name.in () << std::endl;
	    }

	  CdmwPlatformMngt::ProcessRecord record;
	  record.host_name = m_proc_host_name;
	  record.type = m_type;
	  record.auto_ending = m_autoending;
	  record.process_info = m_process_info.in ();
	  record.life_cycle = m_lifeCycle.in ();
	  record.monitoring_info = m_monitoring_info.in ();
	  record.status = this->getStatus ();
	  record.status_info = m_status_info;
	  record.state_machine =
	    CORBA::string_dup (this->get_state_machine_state_name ());
	  record.step_counter = m_stepCounter;
	  record.last_request_status = m_lastRequestStatus;
	  record.last_error_header = m_lastErrorHeader;
	  record.last_error_issuer =
	    CORBA::string_dup (m_lastErrorIssuer.c_str ());

	  if (m_process_ds->contains (m_element_path.in ()))
	    {
	      m_process_ds->update (m_element_path.in (), record);
	    }
	  else
	    {
	      set_element_in_datastore ();

	      m_process_ds->insert (m_element_path.in (), record);
	    }

	  if (Configuration::Is_synchronising ())
	    {
	      std::
		cout << "<<Process_impl::set_record_in_datastore() done >> "
		<< m_element_name.in () << std::endl;
	    }
	}

      m_status_in_ds = this->getStatus ();
    }


    void Process_impl::remove_record_in_datastore ()
    {
      // inhibition if backup
      if (!Configuration::Is_fault_tolerant ()
	  || Configuration::Is_primary ())
	{
	  try
	  {
	    m_process_ds->remove (m_element_path.in ());
	  }
	  catch (Common::NotFoundException &)
	  {
	  }
	}
    }

    CdmwPlatformMngt::ProcessRecord *
      Process_impl::get_record_from_datastore ()
    {
      CdmwPlatformMngt::ProcessRecord * process_record = NULL;

      try
      {
	process_record = m_process_ds->select (m_element_path.in ());
      }
      catch (Common::NotFoundException &)
      {
	std::cout << "process record cannot be found in datastore : "
	  << m_element_path.in () << std::endl;
	CDMW_ASSERT (false);
      }

      return process_record;
    }


    void Process_impl::synchro_with_datastore ()
    {
      Cdmw::PlatformMngt::ManagedElement_impl::synchro_with_datastore ();
      CdmwPlatformMngt::ProcessRecord_var process_record =
	get_record_from_datastore ();

      m_autoending = process_record->auto_ending;
      m_process_info =
	new CdmwPlatformMngt::ProcessInfo (process_record->process_info);
      m_lifeCycle =
	new CdmwPlatformMngt::LifeCycleDef (process_record->life_cycle);
      m_monitoring_info =
	new CdmwPlatformMngt::CompleteMonitoringInfo (process_record->
						      monitoring_info);
      m_status_info = process_record->status_info;
      m_stepCounter = process_record->step_counter;
      m_lastRequestStatus = process_record->last_request_status;
      m_lastErrorHeader = process_record->last_error_header;
      m_lastErrorIssuer = (process_record->last_error_issuer).in ();

      m_status_in_ds = process_record->status;

      this->set_state_machine_state_name (process_record->state_machine.
					  in ());
    }

    void Process_impl::synchro_processing ()
    {
    }


    void Process_impl::monitoringFailure_event (const CdmwPlatformMngtBase::
						EventHeader & header)
      throw (CdmwPlatformMngt::IncompatibleType, CORBA::SystemException)
    {
      throw CdmwPlatformMngt::IncompatibleType ();
    }


    CdmwPlatformMngt::ProcessStatus Process_impl::get_status_in_ds ()
    {
      // return status stored in datastore         
      return m_status_in_ds;
    }



    CdmwPlatformMngt::ProcessType Process_impl::type ()throw (CORBA::
							      SystemException)
    {
      return m_type;
    }


    CORBA::Object_ptr Process_impl::embedded_service ()throw (CORBA::
							      SystemException)
    {
      return CORBA::Object::_nil ();
    }


    CdmwPlatformMngt::ProcessPID Process_impl::pid ()throw (CORBA::
							    SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }


    CORBA::Boolean Process_impl::created_dynamically ()throw (CORBA::
							      SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }

    CdmwPlatformMngt::RestartType Process_impl::restart_type ()throw (CORBA::
								      SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }

    CORBA::Long Process_impl::restart_attempt ()throw (CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }

    bool Process_impl::get_run_as_root () throw (CORBA::SystemException)
    {
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }

    CdmwPlatformMngtBase::TimeStamp Process_impl::start_time ()throw (CORBA::
								      SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }

    CdmwPlatformMngtBase::TimeStamp Process_impl::stop_time ()throw (CORBA::
								     SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }

    CORBA::Boolean Process_impl::is_manual_starting ()throw (CORBA::
							     SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }

    void Process_impl::set_manual_starting (CORBA::Boolean manual_starting)
      throw (CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }

	/**
	 *Purpose:
	 *<p> facade to get an attribute
	 *
	 */

    void Process_impl::get_attribute (const char *attr_name,
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
	      CdmwPlatformMngt::ProcessStatus stat;
	      CORBA::String_var status_info;
	      CORBA::Any_var tempany = new CORBA::Any;

	      stat = get_status (status_info);
	      tempany.inout () <<= stat;
	      ret_attr = tempany._retn ();
	    }

	  if (tmp1 == "status_info")
	    {
	      flag_found = 1;
	      CdmwPlatformMngt::ProcessStatus stat;
	      CORBA::String_var status_info;
	      CORBA::Any_var tempany = new CORBA::Any;

	      stat = get_status (status_info);
	      tempany.inout () <<= status_info;

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

    void Process_impl::set_attribute (const char *attr_name,
				      const CORBA::Any & attr,
				      CORBA::Char flag) throw (CORBA::
							       SystemException,
							       CdmwPlatformMngt::
							       AttributeNotFound)
    {
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


    CdmwPlatformMngt::ProcessMode Process_impl::
      get_process_mode ()throw (CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }

    bool Process_impl::get_memory_resident () throw (CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }

    void Process_impl::set_memory_resident (bool memory_resident)
      throw (CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);

    }

    CORBA::Long Process_impl::get_core_dump_size ()throw (CORBA::
							  SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }




    CORBA::Boolean Process_impl::is_auto_ending ()throw (CORBA::
							 SystemException)
    {
      return m_autoending;
    }

    CdmwPlatformMngt::ManagedElement_ptr Process_impl::
      get_parent ()throw (CORBA::SystemException)
    {
      return m_application->_this ();
    }

    CdmwPlatformMngt::Host_ptr Process_impl::get_host ()throw (CORBA::
							       SystemException)
    {
      CdmwPlatformMngt::Host_var host = m_host;

      return host._retn ();
    }

    CdmwPlatformMngt::ProcessInfo *
      Process_impl::get_process_info ()throw (CORBA::SystemException)
    {
      try
      {
	CdmwPlatformMngt::ProcessInfo_var processInfo
	  = new CdmwPlatformMngt::ProcessInfo (m_process_info.in ());

	return processInfo._retn ();
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
    }

    CdmwPlatformMngt::LifeCycleDef *
      Process_impl::get_life_cycle ()throw (CdmwPlatformMngt::
					    IncompatibleType,
					    CORBA::SystemException)
    {
      throw CdmwPlatformMngt::IncompatibleType ();
    }

    void Process_impl::set_life_cycle (const CdmwPlatformMngt::
				       LifeCycleDef & life_cycle)
      throw (CdmwPlatformMngt::DuplicateStep,
	     CdmwPlatformMngt::IncompatibleType,
	     CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      throw CdmwPlatformMngt::IncompatibleType ();
    }

    CdmwPlatformMngt::CompleteMonitoringInfo Process_impl::
      get_monitoring_info ()throw (CdmwPlatformMngt::IncompatibleType,
				   CORBA::SystemException)
    {
      throw CdmwPlatformMngt::IncompatibleType ();
    }

    void Process_impl::set_monitoring_info (const CdmwPlatformMngt::
					    CompleteMonitoringInfo &
					    monitoring_info)
      throw (CdmwPlatformMngt::IncompatibleType,
	     CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      throw CdmwPlatformMngt::IncompatibleType ();
    }

    CdmwPlatformMngt::ExitAutoRestartInfo *
      Process_impl::get_exit_autorestart_info ()throw (CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }



    void Process_impl::set_exit_autorestart_info (const CdmwPlatformMngt::
						  ExitAutoRestartInfo &
						  exit_autorestart_info)
      throw (CdmwPlatformMngt::IncompatibleStatus, CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }



    CdmwPlatformMngt::ProcessSnapshot *
      Process_impl::get_snapshot ()throw (CORBA::SystemException)
    {
      try
      {
	unsigned int i;

	// Get the snapshot of all entities of application
	CdmwPlatformMngt::EntitiesSnapshot_var entitiesSnapshot
	  = new CdmwPlatformMngt::EntitiesSnapshot ();

	// If the process is managed
	if (type () != CdmwPlatformMngt::UNMANAGED_PROCESS)
	  {
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
	  }
	else
	  {
	    entitiesSnapshot->length (0);
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


	// Create a process snapshot
	CORBA::String_var statusInfo;
	CdmwPlatformMngt::ProcessSnapshot_var snapshot
	  = new CdmwPlatformMngt::ProcessSnapshot;

	// note : the copy of a snapshot sequence (= operator) 
	//        make a deeply copy with a new memory allocation
	//        for the sequence 

	snapshot->process_name = CORBA::string_dup (m_element_name.in ());
	snapshot->host_name = CORBA::string_dup (m_proc_host_name.in ());
	snapshot->process_status = get_status (statusInfo.out ());
	snapshot->process_status_info = statusInfo;
	snapshot->process_entities = entitiesSnapshot.in ();
	snapshot->process_properties = propertiesSnapshot.in ();

	return snapshot._retn ();
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
    }

    CdmwPlatformMngt::RequestStatus Process_impl::
      next_step ()throw (CdmwPlatformMngt::InvalidStep,
			 CdmwPlatformMngt::IncompatibleType,
			 CdmwPlatformMngt::IncompatibleStatus,
			 CORBA::SystemException)
    {
      throw CdmwPlatformMngt::IncompatibleType ();
    }

    CdmwPlatformMngt::RequestStatus Process_impl::
      run ()throw (CdmwPlatformMngt::NotReadyToRun,
		   CdmwPlatformMngt::IncompatibleType,
		   CdmwPlatformMngt::IncompatibleStatus,
		   CORBA::SystemException)
    {
      throw CdmwPlatformMngt::IncompatibleType ();
    }

    CdmwPlatformMngt::RequestStatus Process_impl::
      cold_restart (const CdmwPlatformMngtBase::
		    StartupKind & startup_kind) throw (CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }


    CdmwPlatformMngt::RequestStatus Process_impl::
      hot_restart (const CdmwPlatformMngtBase::
		   StartupKind & startup_kind) throw (CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }



    CORBA::ULong Process_impl::get_number_of_entities ()throw (CORBA::
							       SystemException)
    {
      throw CORBA::BAD_OPERATION (OrbSupport::BAD_OPERATION,
				  CORBA::COMPLETED_NO);
    }

    void Process_impl::get_all_entity_names (CORBA::ULong how_many,
					     CdmwPlatformMngt::
					     ElementNames_out entity_names,
					     CdmwPlatformMngt::
					     ElementNamesIterator_out rest)
      throw (CORBA::SystemException)
    {
      throw CORBA::BAD_OPERATION (OrbSupport::BAD_OPERATION,
				  CORBA::COMPLETED_NO);
    }

    CdmwPlatformMngt::Entity_ptr Process_impl::
      get_entity (const char *entity_name) throw (CdmwPlatformMngt::
						  EntityNotFound,
						  CORBA::SystemException)
    {
      throw CORBA::BAD_OPERATION (OrbSupport::BAD_OPERATION,
				  CORBA::COMPLETED_NO);
    }

    CORBA::Boolean Process_impl::get_entities (const CdmwPlatformMngt::
					       ElementNames & entity_names,
					       CdmwPlatformMngt::
					       Entities_out nentities)
      throw (CORBA::SystemException)
    {
      throw CORBA::BAD_OPERATION (OrbSupport::BAD_OPERATION,
				  CORBA::COMPLETED_NO);
    }

    void Process_impl::get_all_entities (CORBA::ULong how_many,
					 CdmwPlatformMngt::
					 Entities_out nentities,
					 CdmwPlatformMngt::
					 EntitiesIterator_out rest)
      throw (CORBA::SystemException)
    {
      throw CORBA::BAD_OPERATION (OrbSupport::BAD_OPERATION,
				  CORBA::COMPLETED_NO);
    }

    CdmwPlatformMngt::Entity_ptr Process_impl::
      add_entity (const CdmwPlatformMngt::
		  EntityDef & entity_def) throw (CdmwPlatformMngt::
						 EntityAlreadyExists,
						 CORBA::SystemException)
    {
      throw CORBA::BAD_OPERATION (OrbSupport::BAD_OPERATION,
				  CORBA::COMPLETED_NO);
    }

    void Process_impl::remove_entity (const char *entity_name)
      throw (CdmwPlatformMngt::EntityNotFound, CORBA::SystemException)
    {
      throw CORBA::BAD_OPERATION (OrbSupport::BAD_OPERATION,
				  CORBA::COMPLETED_NO);
    }

    CdmwPlatformMngtService::ServiceProviderID *
      Process_impl::get_service_def (const char *service_name)
      throw (CdmwPlatformMngtService::ServiceNotFound, CORBA::SystemException)
    {
      throw CORBA::BAD_OPERATION (OrbSupport::BAD_OPERATION,
				  CORBA::COMPLETED_NO);
    }

    void Process_impl::add_service_def (const CdmwPlatformMngtService::
					ServiceDef & service_def)
      throw (CdmwPlatformMngtService::ServiceAlreadyExists,
	     CORBA::SystemException)
    {
      throw CORBA::BAD_OPERATION (OrbSupport::BAD_OPERATION,
				  CORBA::COMPLETED_NO);
    }

    void Process_impl::remove_service_def (const char *service_name)
      throw (CdmwPlatformMngtService::ServiceNotFound, CORBA::SystemException)
    {
      throw CORBA::BAD_OPERATION (OrbSupport::BAD_OPERATION,
				  CORBA::COMPLETED_NO);
    }

    void Process_impl::remove_all_service_defs ()
      throw (CORBA::SystemException)
    {
      throw CORBA::BAD_OPERATION (OrbSupport::BAD_OPERATION,
				  CORBA::COMPLETED_NO);
    }

    void Process_impl::is_alive ()
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }

    CdmwPlatformMngt::ApplicationAgent_ptr
      Process_impl::getApplicationAgent ()throw (CdmwPlatformMngt::
						 HostNotFound,
						 CdmwPlatformMngt::
						 HostNotReachable,
						 CdmwPlatformMngt::
						 CreationError,
						 CdmwPlatformMngt::
						 ApplicationAgentAlreadyExists,
						 CdmwPlatformMngt::
						 InvalidInfo,
						 CdmwPlatformMngt::
						 CreationTimeout,
						 AgentNotFoundException,
						 CORBA::SystemException)
    {
      CdmwPlatformMngt::ApplicationAgent_var agent;

      // Get the application agent on the host of this process
      // if does not exist start a new one
      agent = m_application->getAgent (m_proc_host_name.in ());

      return agent._retn ();
    }

    CdmwPlatformMngt::ApplicationAgent_ptr
      Process_impl::searchApplicationAgent ()throw (CdmwPlatformMngt::
						    HostNotFound,
						    CdmwPlatformMngt::
						    HostNotReachable,
						    CORBA::SystemException)
    {
      CdmwPlatformMngt::ApplicationAgent_var agent;

      // Get the application agent on the host of this process
      // if does not exist nil is returned
      agent = m_application->searchAgentFromDaemon (m_proc_host_name.in ());

      return agent._retn ();
    }




    void Process_impl::setAutoending (CORBA::Boolean auto_ending)
    {
      m_autoending = auto_ending;

      // update record in datastore
      set_record_in_datastore ();
    }

    void Process_impl::notifyEntityStatusChangeEvent (const char *entity_name,
						      const char
						      *entity_status,
						      const char *entity_info)
    {
      try
      {
	CdmwPlatformMngtBase::EventHeader header
	  = EventHeaderFactory::createHeader (CdmwPlatformMngtBase::INF);

	CdmwPlatformMngt::ProcessEntityStatusChange_var event
	  = m_procEntityStatusChangeFactory->create (header,
						     m_application->
						     get_system_name (),
						     m_application->
						     get_element_name (),
						     m_element_name.in (),
						     m_proc_host_name.in (),
						     entity_name,
						     entity_status,
						     entity_info);

	CORBA::String_var eventStr = event->to_string ();
	LogMngr::logMessage (eventStr.in ());

	m_application->get_event_notifier ()->addEvent (event);
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

    void Process_impl::setHost (const char *host_name)
      throw (CdmwPlatformMngt::HostNotFound, CORBA::SystemException)
    {
      CdmwPlatformMngt::HostContainer_var host_container =
	m_application->get_host_container ();
      m_host = host_container->get_host (host_name);

      m_proc_host_name = CORBA::string_dup (host_name);

      // update record in datastore
      set_record_in_datastore ();
    }

    void Process_impl::setProcessInfo (const CdmwPlatformMngt::
				       ProcessInfo & process_info)
    {
      try
      {
	m_process_info = new CdmwPlatformMngt::ProcessInfo (process_info);

	// update record in datastore
	set_record_in_datastore ();
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
    }

    void Process_impl::setMonitoringInfo (const CdmwPlatformMngt::
					  CompleteMonitoringInfo &
					  monitoring_info)
    {
      try
      {
	m_monitoring_info =
	  new CdmwPlatformMngt::CompleteMonitoringInfo (monitoring_info);

	// update record in datastore    
	set_record_in_datastore ();
      }
      catch (const std::bad_alloc &)
      {
	throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
      }
    }

    void Process_impl::errorEvent (const CdmwPlatformMngtBase::
				   EventHeader & header, const char *issuer,
				   const char *error_info)
    {
      // Do nothing if in a failed status
      CdmwPlatformMngt::ProcessStatus status = getStatus ();

      if ((status == CdmwPlatformMngt::PROCESS_FAILED_ERROR) ||
	  (status == CdmwPlatformMngt::PROCESS_FAILED_TIMEOUT) ||
	  (status == CdmwPlatformMngt::PROCESS_FAILED_DISCREPANCY) ||
	  (status == CdmwPlatformMngt::PROCESS_FAILED_INVALID) ||
	  (status == CdmwPlatformMngt::PROCESS_FAILED_NO_RESPONSE) ||
	  (status == CdmwPlatformMngt::PROCESS_FAILED_DEATH))
	return;

      // Set the status info's message
      m_status_info = CORBA::string_dup (error_info);

      // Store the info about the issuance of this error event.
      m_lastErrorHeader = header;
      m_lastErrorIssuer = issuer;

      // Change to the state FailedError
      setState ("FailedError");

      // update record in datastore
      set_record_in_datastore ();
    }

    void Process_impl::endingEvent (const CdmwPlatformMngtBase::
				    EventHeader & header)
    {
      // Do nothing if in a failed status
      CdmwPlatformMngt::ProcessStatus status = getStatus ();

      if ((status == CdmwPlatformMngt::PROCESS_FAILED_ERROR) ||
	  (status == CdmwPlatformMngt::PROCESS_FAILED_TIMEOUT) ||
	  (status == CdmwPlatformMngt::PROCESS_FAILED_DISCREPANCY) ||
	  (status == CdmwPlatformMngt::PROCESS_FAILED_INVALID) ||
	  (status == CdmwPlatformMngt::PROCESS_FAILED_NO_RESPONSE) ||
	  (status == CdmwPlatformMngt::PROCESS_FAILED_DEATH))
	return;

      // if process is authorised to end itself    
      if (is_auto_ending ())
	{
	  // change to the status Ended
	  setState ("Ended");
	}

      else
	{
	  // Set the status info's message
	  m_status_info =
	    CORBA::string_dup (LogMngr::
			       getFormattedMessage
			       (MSG_ID_PROCESS_ENDING_ERROR).c_str ());

	  // Store the info about the issuance of this event.
	  m_lastErrorHeader = header;

	  // Change to the state FailedDeath
	  setState ("FailedDeath");
	}

      // update record in datastore
      set_record_in_datastore ();
    }

    void Process_impl::invalidateEvent (const CdmwPlatformMngtBase::
					EventHeader & header,
					const char *reason)
    {
      // Do nothing if in a failed status
      CdmwPlatformMngt::ProcessStatus status = getStatus ();

      if ((status == CdmwPlatformMngt::PROCESS_FAILED_ERROR) ||
	  (status == CdmwPlatformMngt::PROCESS_FAILED_TIMEOUT) ||
	  (status == CdmwPlatformMngt::PROCESS_FAILED_DISCREPANCY) ||
	  (status == CdmwPlatformMngt::PROCESS_FAILED_INVALID) ||
	  (status == CdmwPlatformMngt::PROCESS_FAILED_NO_RESPONSE) ||
	  (status == CdmwPlatformMngt::PROCESS_FAILED_DEATH))
	return;

      // Set the status info's message
      m_status_info = CORBA::string_dup (reason);

      // Store the info about the issuance of this event.
      m_lastErrorHeader = header;

      // Change to the state FailedInvalid
      setState ("FailedInvalid");

      // update record in datastore
      set_record_in_datastore ();
    }


    Application_impl *Process_impl::get_application ()
    {
      return m_application;
    }

    const char *Process_impl::get_process_host_name ()
    {
      return m_proc_host_name.in ();
    }


    const char *Process_impl::get_process_key ()
    {
      return m_proc_key.in ();
    }

    void Process_impl::set_create_process_result (CdmwPlatformMngt::
						  ProcessCommandResult result,
						  const char *error_info,
						  const char *error_issuer,
						  CdmwPlatformMngt::
						  ProcessDelegateWrapper_ptr
						  process_delegate_wrap)
      throw (CORBA::SystemException)
    {
      // the process delegate wrapper object is not used in this case

      m_starting_result = result;

      m_creation_error_info = CORBA::string_dup (error_info);
      m_creation_error_issuer = CORBA::string_dup (error_issuer);

      // signals the acknowledgement
      if (!m_process_start_cbk->signal ())
	{
	  // The acknowledgement has been performed too late
	  throw CORBA::NO_PERMISSION (OrbSupport::NO_PERMISSION,
				      CORBA::COMPLETED_NO);
	}
    }



    void Process_impl::set_initialisation_result (CdmwPlatformMngt::
						  ProcessCommandResult result)
      throw (CORBA::SystemException)
    {
      throw CORBA::BAD_OPERATION (OrbSupport::BAD_OPERATION,
				  CORBA::COMPLETED_NO);
    }


    void Process_impl::set_next_step_result (CdmwPlatformMngt::
					     ProcessCommandResult result,
					     CORBA::
					     ULong step_nbr) throw (CORBA::
								    SystemException)
    {
      throw CORBA::BAD_OPERATION (OrbSupport::BAD_OPERATION,
				  CORBA::COMPLETED_NO);
    }


    void Process_impl::set_run_result (CdmwPlatformMngt::
				       ProcessCommandResult result)
      throw (CORBA::SystemException)
    {
      throw CORBA::BAD_OPERATION (OrbSupport::BAD_OPERATION,
				  CORBA::COMPLETED_NO);
    }


    void Process_impl::set_stop_result (CdmwPlatformMngt::
					ProcessCommandResult result)
      throw (CORBA::SystemException)
    {
      throw CORBA::BAD_OPERATION (OrbSupport::BAD_OPERATION,
				  CORBA::COMPLETED_NO);
    }


  }				// End namespace PlatformMngt
}				// End namespace Cdmw
