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


#include <cstring>
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"

#include "SystemMngt/platformelementlibrary/Entity_impl.hpp"

#include "platforminterface/ProcessDelegate_impl.hpp"

namespace Cdmw
{
  namespace PlatformMngt
  {

    PlatformInterface::InterfaceStatus PlatformInterface::M_status =
      PlatformInterface::INITIAL;

    OsSupport::Mutex PlatformInterface::M_status_mtx;

    CdmwPlatformMngt::ProcessCallback_var PlatformInterface::
      M_process_callback;

    std::string PlatformInterface::M_application_name;
    std::string PlatformInterface::M_process_name;
    CdmwPlatformMngt::ProcessConfigurationData PlatformInterface::
      M_configuration_data;

    CdmwPlatformMngt::ProcessMessageBroker_var PlatformInterface::
      M_process_message_broker;

    CORBA::ORB_var PlatformInterface::M_orb;

    void PlatformInterface::Check_status (InterfaceStatus requiredStatus)
      throw (BadOrderException)
    {
      CDMW_MUTEX_GUARD (M_status_mtx);

      if (M_status != requiredStatus)
	{
	  std::
	    cout << "M_status " << M_status << " requiredStatus " <<
	    requiredStatus << std::endl;
	  CDMW_THROW (BadOrderException);
	}

    }


    bool PlatformInterface::Is_launched_by_PlatformManagement (int argc,
							       char *argv[])
    {
      bool ret = true;

      std::string callbackIor_option =
	OsSupport::OS::get_option_value (argc, argv,
					 Common::Options::
					 CALLBACK_IOR_OPTION);

      if (callbackIor_option == "no" || callbackIor_option == "yes")
	{
	  ret = false;
	}

      return ret;
    }


    void PlatformInterface::Setup (CORBA::ORB_ptr orb, int argc, char *argv[])
      throw (BadOrderException, BadParameterException)
    {

      Check_status (INITIAL);

      if (CORBA::is_nil (orb))
	CDMW_THROW2 (BadParameterException, "orb", "nil");

      M_orb = CORBA::ORB::_duplicate (orb);

      std::string callbackIor_option =
	OsSupport::OS::get_option_value (argc, argv,
					 Common::Options::
					 CALLBACK_IOR_OPTION);

      if (callbackIor_option == "no" || callbackIor_option == "yes")
	{
	  CDMW_THROW2 (BadParameterException, "argv",
		       "doesn't contain the callback IOR");
	}
      else
	{
	  try
	  {
	    // gets the acknowledgement callback
	    CORBA::Object_var callback_obj =
	      orb->string_to_object (callbackIor_option.c_str ());

	    CdmwPlatformMngt::ProcessCallback_var callback =
	      CdmwPlatformMngt::ProcessCallback::_narrow (callback_obj.in ());

	    if (CORBA::is_nil (callback.in ()))
	      {
		CDMW_THROW2 (BadParameterException, "argv",
			     "doesn't contain a valid callback IOR");
	      }

	    M_process_callback = callback;

	  }
	  catch (const CORBA::SystemException &)
	  {
	    CDMW_THROW2 (BadParameterException, "argv",
			 "contains an invalid IOR");
	  }

	}

      CDMW_MUTEX_GUARD (M_status_mtx);

      M_status = SETUP_PERFORMED;

    }

    void PlatformInterface::Cleanup ()
    {
      CDMW_MUTEX_GUARD (M_status_mtx);
      M_status = PlatformInterface::INITIAL;

      M_process_callback = CdmwPlatformMngt::ProcessCallback::_nil ();
      M_process_message_broker =
	CdmwPlatformMngt::ProcessMessageBroker::_nil ();
      M_orb = CORBA::ORB::_nil ();
    }


    void PlatformInterface::Acknowledge_creation (CdmwPlatformMngt::
						  ProcessDelegate_ptr process)
      throw (CORBA::SystemException, BadOrderException, BadParameterException)
    {

      Check_status (SETUP_PERFORMED);

      if (CORBA::is_nil (process))
	CDMW_THROW2 (BadParameterException, "process", "nil");


      CDMW_MUTEX_GUARD (M_status_mtx);

      CdmwPlatformMngt::ProcessCallback::ProcessStartingData_var start_data =
	M_process_callback->set_ready (process);

      M_application_name = start_data->application_name;

      M_process_name = start_data->process_name;

      M_configuration_data = start_data->process_configuration_data;

      CDMW_ASSERT (!CORBA::
		   is_nil ((start_data->process_message_broker).in ()));
      M_process_message_broker =
	CdmwPlatformMngt::ProcessMessageBroker::
	_duplicate ((start_data->process_message_broker).in ());

      M_status = CREATION_DONE_PERFORMED;

    }


    void PlatformInterface::Acknowledge_creation (ProcessBehaviour *
						  behaviour) throw (CORBA::
								    SystemException,
								    BadOrderException,
								    BadParameterException)
    {

      Check_status (SETUP_PERFORMED);

      if (behaviour == NULL)
	CDMW_THROW2 (BadParameterException, "behaviour", "NULL");

      // creates the default managed process servant
      PortableServer::ServantBase_var process_servant
	= new ProcessDelegate_impl (M_orb.in (), behaviour);

      ProcessDelegate_impl *processDelegate_impl =
	dynamic_cast < ProcessDelegate_impl * >(process_servant.in ());

      // activates the servant and creates the CORBA object
      CdmwPlatformMngt::ProcessDelegate_var processDelegate =
	processDelegate_impl->_this ();

      Acknowledge_creation (processDelegate.in ());

    }


    void PlatformInterface::Notify_creation_starting (CdmwPlatformMngt::
						      ProcessDelegate_ptr
						      process) throw (CORBA::
								      SystemException,
								      BadOrderException,
								      BadParameterException)
    {

      Check_status (SETUP_PERFORMED);

      if (CORBA::is_nil (process))
	CDMW_THROW2 (BadParameterException, "process", "nil");


      CDMW_MUTEX_GUARD (M_status_mtx);

      CdmwPlatformMngt::ProcessCallback::ProcessStartingData_var start_data =
	M_process_callback->get_starting_info (process);

      M_application_name = start_data->application_name;

      M_process_name = start_data->process_name;

      M_configuration_data = start_data->process_configuration_data;

      CDMW_ASSERT (!CORBA::
		   is_nil ((start_data->process_message_broker).in ()));
      M_process_message_broker =
	CdmwPlatformMngt::ProcessMessageBroker::
	_duplicate ((start_data->process_message_broker).in ());

      M_status = CREATION_STARTING_PERFORMED;
    }


    void PlatformInterface::Notify_creation_starting (ProcessBehaviour *
						      behaviour)
      throw (CORBA::SystemException, BadOrderException, BadParameterException)
    {

      Check_status (SETUP_PERFORMED);

      if (behaviour == NULL)
	CDMW_THROW2 (BadParameterException, "behaviour", "NULL");

      // creates the default managed process servant
      PortableServer::ServantBase_var process_servant
	= new ProcessDelegate_impl (M_orb.in (), behaviour);

      ProcessDelegate_impl *processDelegate_impl =
	dynamic_cast < ProcessDelegate_impl * >(process_servant.in ());

      // activates the servant and creates the CORBA object
      CdmwPlatformMngt::ProcessDelegate_var processDelegate =
	processDelegate_impl->_this ();

      Notify_creation_starting (processDelegate.in ());
    }


    void PlatformInterface::Notify_creation_done ()
      throw (CORBA::SystemException, BadOrderException)
    {

      Check_status (CREATION_STARTING_PERFORMED);

      CDMW_MUTEX_GUARD (M_status_mtx);

      M_process_callback->set_creation_done ();

      M_status = CREATION_DONE_PERFORMED;
    }



    void PlatformInterface::
      Check_issuer (const char *issuer) throw (BadParameterException)
    {

      if (issuer == NULL)
	CDMW_THROW2 (BadParameterException, "issuer", "NULL");

      size_t len =::strlen (CdmwPlatformMngtBase::PFMMNG);

      if (::strncmp (CdmwPlatformMngtBase::PFMMNG, issuer, len) == 0)
	CDMW_THROW2 (BadParameterException, "issuer", "PFMMNG");

    }


    void PlatformInterface::Notify_message (CdmwPlatformMngtBase::
					    EventLevel level,
					    const char *issuer,
					    const char *message)
      throw (CORBA::SystemException, BadOrderException, BadParameterException)
    {
      Check_status (CREATION_DONE_PERFORMED);

      if (level == CdmwPlatformMngtBase::FTL)
	CDMW_THROW2 (BadParameterException, "level", "FTL");

      Check_issuer (issuer);

      if (message == NULL)
	CDMW_THROW2 (BadParameterException, "message", "NULL");

      CdmwPlatformMngtBase::EventHeader header =
	Cdmw::PlatformMngt::EventHeaderFactory::createHeader (level);

      M_process_message_broker->notify_message (header, issuer, message);

    }



    void PlatformInterface::Notify_fatal_error (const char *issuer,
						const char *errorInfo)
      throw (CORBA::SystemException, BadOrderException, BadParameterException)
    {
      CDMW_MUTEX_GUARD (M_status_mtx);

      if (M_status == INITIAL)
	CDMW_THROW (BadOrderException);

      Check_issuer (issuer);

      if (errorInfo == NULL)
	CDMW_THROW2 (BadParameterException, "errorInfo", "NULL");

      if (M_status == SETUP_PERFORMED)
	{
	  // the acknowledgement has not been performed yet
	  M_process_callback->notify_error (issuer, errorInfo);
	}
      else
	{
	  // the acknowledgement has already been performed

	  CdmwPlatformMngtBase::TimeStamp timeStamp =
	    Cdmw::PlatformMngt::EventHeaderFactory::createTimeStamp ();

	  M_process_message_broker->notify_error (timeStamp, issuer,
						  errorInfo);

	}

    }

    void PlatformInterface::
      Notify_process_fatal_error (const char *process_name,
				  const char *issuer,
				  const char *errorInfo) throw (CORBA::
								SystemException,
								BadOrderException,
								BadParameterException,
								CdmwPlatformMngt::
								ProcessNotFound)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);
    }


    void PlatformInterface::Notify_user_event (const char *issuer,
					       const CORBA::Any & data)
      throw (CORBA::SystemException, BadOrderException, BadParameterException)
    {
      Check_status (CREATION_DONE_PERFORMED);

      M_process_message_broker->notify_user_event (issuer, data);

    }



    const char *PlatformInterface::Get_application_name ()
      throw (BadOrderException)
    {
      Check_status (CREATION_DONE_PERFORMED);

      return M_application_name.c_str ();
    }


    const char *PlatformInterface::Get_process_name ()
      throw (BadOrderException)
    {
      Check_status (CREATION_DONE_PERFORMED);

      return M_process_name.c_str ();
    }

    CdmwPlatformMngt::ProcessConfigurationData PlatformInterface::
      Get_process_config_data ()throw (BadOrderException)
    {
      return M_configuration_data;
    }



    CORBA::Object_ptr PlatformInterface::Get_service (const char *serviceName)
      throw (CdmwPlatformMngtService::ServiceNotFound,
	     CdmwPlatformMngtService::ServiceUnavailable,
	     CORBA::SystemException, BadOrderException, BadParameterException)
    {
      Check_status (CREATION_DONE_PERFORMED);

      if (serviceName == NULL)
	CDMW_THROW2 (BadParameterException, "serviceName", "NULL");

      return M_process_message_broker->get_service (serviceName);

    }


    CdmwPlatformMngt::ManagedElement_ptr PlatformInterface::
      Get_managed_element (const char *element_path) throw (CdmwPlatformMngt::
							    ManagedElementNotFound,
							    CORBA::
							    SystemException,
							    BadOrderException,
							    BadParameterException)
    {
      Check_status (CREATION_DONE_PERFORMED);

      if (element_path == NULL)
	CDMW_THROW2 (BadParameterException, "element_path", "NULL");

      CdmwPlatformMngt::ManagedElement_var element;

      try
      {
	element =
	  M_process_message_broker->get_managed_element (element_path);
      }
      catch (CdmwPlatformMngt::ElementPathInvalid)
      {
	CDMW_THROW2 (BadParameterException, "element_path", "invalid");
      }


      return element._retn ();
    }




    void PlatformInterface::Check_entity_name (const char *entity_name)
      throw (BadOrderException, BadParameterException)
    {

      Check_status (CREATION_DONE_PERFORMED);

      if (entity_name == NULL)
	CDMW_THROW2 (BadParameterException, "entity_name", "NULL");

      if (!strcmp (entity_name, ""))
	CDMW_THROW2 (BadParameterException, "entity_name", "NULL");
    }

    void PlatformInterface::
      Check_entity_status_parameters (const char *entity_name,
				      const char *entity_status,
				      const char *entity_info)
      throw (BadOrderException, BadParameterException)
    {

      Check_status (CREATION_DONE_PERFORMED);

      if (entity_name == NULL)
	CDMW_THROW2 (BadParameterException, "entity_name", "NULL");

      if (!strcmp (entity_name, ""))
	CDMW_THROW2 (BadParameterException, "entity_name", "NULL");

      if (entity_status == NULL)
	CDMW_THROW2 (BadParameterException, "entity_status", "NULL");

      if (entity_info == NULL)
	CDMW_THROW2 (BadParameterException, "entity_info", "NULL");

    }


    char *PlatformInterface::
      Get_system_entity_status (const char *entity_name,
				CORBA::
				String_out entity_info)
      throw (CdmwPlatformMngt::EntityNotFound, CORBA::SystemException,
	     BadOrderException, BadParameterException)
    {
      Check_entity_name (entity_name);

      return M_process_message_broker->get_system_entity_status (entity_name,
								 entity_info);
    }

    char *PlatformInterface::Get_host_entity_status (const char *entity_name,
						     CORBA::
						     String_out entity_info)
      throw (CdmwPlatformMngt::EntityNotFound, CORBA::SystemException,
	     BadOrderException, BadParameterException)
    {
      Check_entity_name (entity_name);

      try
      {
	return M_process_message_broker->get_host_entity_status ("",
								 entity_name,
								 entity_info);
      }
      catch (CdmwPlatformMngt::HostNotFound &)
      {
	// must never occurred
	std::
	  cout << "Get_host_entity_status : HostNotFound Exception" << std::
	  endl;

	throw CdmwPlatformMngt::EntityNotFound ();
      }
    }


    char *PlatformInterface::
      Get_application_entity_status (const char *entity_name,
				     CORBA::
				     String_out entity_info)
      throw (CdmwPlatformMngt::EntityNotFound, CORBA::SystemException,
	     BadOrderException, BadParameterException)
    {
      Check_entity_name (entity_name);

      try
      {
	return M_process_message_broker->get_application_entity_status ("",
									entity_name,
									entity_info);
      }
      catch (CdmwPlatformMngt::ApplicationNotFound &)
      {
	// must never occurred
	std::
	  cout <<
	  "Get_application_entity_status : ApplicationNotFound Exception" <<
	  std::endl;

	throw CdmwPlatformMngt::EntityNotFound ();
      }
    }


    char *PlatformInterface::
      Get_process_entity_status (const char *entity_name,
				 CORBA::
				 String_out entity_info)
      throw (CdmwPlatformMngt::EntityNotFound, CORBA::SystemException,
	     BadOrderException, BadParameterException)
    {
      Check_entity_name (entity_name);

      try
      {
	return M_process_message_broker->get_process_entity_status ("", "",
								    "",
								    entity_name,
								    entity_info);
      }
      catch (CdmwPlatformMngt::ProcessNotFound &)
      {
	// must never occurred
	std::
	  cout << "Get_process_entity_status : ProcessNotFound Exception" <<
	  std::endl;

	throw CdmwPlatformMngt::EntityNotFound ();
      }
    }

    void PlatformInterface::Set_system_entity_status (const char *entity_name,
						      const char
						      *entity_status,
						      const char *entity_info)
      throw (CdmwPlatformMngt::EntityNotFound, CORBA::SystemException,
	     BadOrderException, BadParameterException)
    {
      Check_entity_status_parameters (entity_name, entity_status,
				      entity_info);

      M_process_message_broker->set_system_entity_status (entity_name,
							  entity_status,
							  entity_info);
    }


    void PlatformInterface::Set_host_entity_status (const char *entity_name,
						    const char *entity_status,
						    const char *entity_info)
      throw (CdmwPlatformMngt::EntityNotFound,
	     CORBA::SystemException, BadOrderException, BadParameterException)
    {
      Check_entity_status_parameters (entity_name, entity_status,
				      entity_info);

      try
      {
	M_process_message_broker->set_host_entity_status ("", entity_name,
							  entity_status,
							  entity_info);
      }
      catch (CdmwPlatformMngt::HostNotFound &)
      {
	// must never occurred
	std::
	  cout << "Set_host_entity_status : HostNotFound Exception" << std::
	  endl;

	throw CdmwPlatformMngt::EntityNotFound ();
      }

    }

    void PlatformInterface::
      Set_application_entity_status (const char *entity_name,
				     const char *entity_status,
				     const char *entity_info)
      throw (CdmwPlatformMngt::EntityNotFound, CORBA::SystemException,
	     BadOrderException, BadParameterException)
    {
      Check_entity_status_parameters (entity_name, entity_status,
				      entity_info);

      try
      {
	M_process_message_broker->set_application_entity_status ("",
								 entity_name,
								 entity_status,
								 entity_info);
      }
      catch (CdmwPlatformMngt::ApplicationNotFound &)
      {
	// must never occurred
	std::
	  cout <<
	  "Set_application_entity_status : ApplicationNotFound Exception" <<
	  std::endl;

	throw CdmwPlatformMngt::EntityNotFound ();
      }
    }


    void PlatformInterface::
      Set_process_entity_status (const char *entity_name,
				 const char *entity_status,
				 const char *entity_info)
      throw (CdmwPlatformMngt::EntityNotFound, CORBA::SystemException,
	     BadOrderException, BadParameterException)
    {
      Check_entity_status_parameters (entity_name, entity_status,
				      entity_info);

      try
      {
	M_process_message_broker->set_process_entity_status ("", "", "",
							     entity_name,
							     entity_status,
							     entity_info);
      }
      catch (CdmwPlatformMngt::ProcessNotFound &)
      {
	// must never occurred
	std::
	  cout << "Set_process_entity_status : ProcessNotFound Exception" <<
	  std::endl;

	throw CdmwPlatformMngt::EntityNotFound ();
      }
    }



    char *PlatformInterface::Get_host_entity_status (const char *host_name,
						     const char *entity_name,
						     CORBA::
						     String_out entity_info)
      throw (CdmwPlatformMngt::HostNotFound, CdmwPlatformMngt::EntityNotFound,
	     CORBA::SystemException, BadOrderException, BadParameterException)
    {
      Check_entity_name (entity_name);

      return M_process_message_broker->get_host_entity_status (host_name,
							       entity_name,
							       entity_info);

    }

    char *PlatformInterface::
      Get_application_entity_status (const char *application_name,
				     const char *entity_name,
				     CORBA::
				     String_out entity_info)
      throw (CdmwPlatformMngt::ApplicationNotFound,
	     CdmwPlatformMngt::EntityNotFound, CORBA::SystemException,
	     BadOrderException, BadParameterException)
    {
      Check_entity_name (entity_name);

      return M_process_message_broker->
	get_application_entity_status (application_name, entity_name,
				       entity_info);
    }


    char *PlatformInterface::
      Get_process_entity_status (const char *application_name,
				 const char *process_name,
				 const char *host_name,
				 const char *entity_name,
				 CORBA::
				 String_out entity_info)
      throw (CdmwPlatformMngt::ProcessNotFound,
	     CdmwPlatformMngt::EntityNotFound, CORBA::SystemException,
	     BadOrderException, BadParameterException)
    {
      Check_entity_name (entity_name);

      return M_process_message_broker->
	get_process_entity_status (application_name, process_name, host_name,
				   entity_name, entity_info);
    }


    void PlatformInterface::Set_host_entity_status (const char *host_name,
						    const char *entity_name,
						    const char *entity_status,
						    const char *entity_info)
      throw (CdmwPlatformMngt::HostNotFound,
	     CdmwPlatformMngt::EntityNotFound,
	     CORBA::SystemException, BadOrderException, BadParameterException)
    {
      Check_entity_status_parameters (entity_name, entity_status,
				      entity_info);

      M_process_message_broker->set_host_entity_status (host_name,
							entity_name,
							entity_status,
							entity_info);
    }


    void PlatformInterface::
      Set_application_entity_status (const char *application_name,
				     const char *entity_name,
				     const char *entity_status,
				     const char *entity_info)
      throw (CdmwPlatformMngt::ApplicationNotFound,
	     CdmwPlatformMngt::EntityNotFound, CORBA::SystemException,
	     BadOrderException, BadParameterException)
    {
      Check_entity_status_parameters (entity_name, entity_status,
				      entity_info);

      M_process_message_broker->
	set_application_entity_status (application_name, entity_name,
				       entity_status, entity_info);
    }


    void PlatformInterface::
      Set_process_entity_status (const char *application_name,
				 const char *process_name,
				 const char *host_name,
				 const char *entity_name,
				 const char *entity_status,
				 const char *entity_info)
      throw (CdmwPlatformMngt::ProcessNotFound,
	     CdmwPlatformMngt::EntityNotFound, CORBA::SystemException,
	     BadOrderException, BadParameterException)
    {
      Check_entity_status_parameters (entity_name, entity_status,
				      entity_info);

      M_process_message_broker->set_process_entity_status (application_name,
							   process_name,
							   host_name,
							   entity_name,
							   entity_status,
							   entity_info);
    }



    void PlatformInterface::Get_property_value (const char *element_path,
						const char *property_name,
						CORBA::Any * property_value)
      throw (CosPropertyService::PropertyNotFound,
	     CORBA::SystemException, BadOrderException)
    {

      CdmwPlatformMngt::ManagedElement_var managed_element =
	PlatformInterface::Get_managed_element (element_path);
      CORBA::Any * return_value =
	managed_element->get_property_value (property_name);
      *property_value = *return_value;
    }


    void PlatformInterface::Set_property_value (const char *element_path,
						const char *property_name,
						const CORBA::
						Any & property_value)
      throw (CosPropertyService::PropertyNotFound,
	     CosPropertyService::ReadOnlyProperty, CORBA::SystemException,
	     BadOrderException)
    {
      CdmwPlatformMngt::ManagedElement_var managed_element =
	PlatformInterface::Get_managed_element (element_path);
      managed_element->define_property (property_name, property_value);
    }


    void PlatformInterface::Call_command (const char *process_path,
					  const char *command_name,
					  const CdmwPlatformMngt::
					  CmdParameterValues & command_args,
					  CdmwPlatformMngt::
					  CommandResponseCallback_ptr
					  response_callback)
      throw (CdmwPlatformMngt::ProcessNotFound,
	     CdmwPlatformMngt::CommandNotFound,
	     CdmwPlatformMngt::CommandArgumentInvalid,
	     CdmwPlatformMngt::CommandRejected, CORBA::SystemException,
	     BadOrderException)
    {
    }


    void PlatformInterface::Call_entity_command (const char *entity_path,
						 const char *command_name,
						 const CdmwPlatformMngt::
						 CmdParameterValues &
						 command_args,
						 CdmwPlatformMngt::
						 CommandResponseCallback_ptr
						 response_callback)
      throw (CdmwPlatformMngt::EntityNotFound,
	     CdmwPlatformMngt::CommandNotFound,
	     CdmwPlatformMngt::CommandArgumentInvalid,
	     CdmwPlatformMngt::CommandRejected, CORBA::SystemException,
	     BadOrderException)
    {
    }

    void PlatformInterface::Set_command_callback (const char *command_name,
						  UserCommandCallbackBase *
						  command_callback)
      throw (BadParameterException)
    {
    }




    void PlatformInterface::
      Set_entity_command_callback (const char *entity_name,
				   const char *command_name,
				   UserCommandCallbackBase *
				   command_callback)
      throw (BadParameterException)
    {
    }


  }				// End namespace PlatformMngt
}				// End namespace Cdmw
