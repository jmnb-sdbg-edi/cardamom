/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your 
 * option) any later version. 
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public 
 * License for more details. 
 * 
 * You should have received a copy of the GNU Library General 
 * Public License along with CARDAMOM; see the file COPYING. If not, write to 
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


#include "Foundation/common/Assert.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/Exception.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"


#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platforminterface/ServiceNames.hpp"
#include "SystemMngt/platforminterface/ProcessBehaviour.hpp"

#include "testplatforminterface/UserProcessBehaviour.hpp"
#include "testplatforminterface/ProcessCallback_impl.hpp"

#include "testplatforminterface/SimEntityObserver_impl.hpp"
#include "testplatforminterface/SimProcessMessageBroker_impl.hpp"
#include "testplatforminterface/SimServiceBroker_impl.hpp"
#include "testplatforminterface/PlatformInterfaceTest.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtEntity.stub.hpp"
	  


PlatformInterfaceTest::PlatformInterfaceTest()
  throw()
  : Testable("Cdmw::PlatformInterface")
{
}

PlatformInterfaceTest::~PlatformInterfaceTest()
  throw()
{
}


void PlatformInterfaceTest::do_tests()
  throw()
{
  // set number of requested successfull tests
  set_nbOfRequestedTestOK (32);

  const char* SERVICE_BROKER_USER_OBJECT_ID          = "service-broker.simulated";
  const char* ENTITY_OBSERVER_USER_OBJECT_ID         = "entity-observer.simulated";
  const char* PROCESS_MESSAGE_BROKER_USER_OBJECT_ID  = "process-message-broker.simulated";

  
  
  try 
    {
    	// ===================================================
        // Initialize the ORB
        // ===================================================
	TEST_INFO("Initialize the ORB");

        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(5);

        std::string str_opt( "" );
        char** argv_tmp = NULL;
        int argc_tmp(1);
        str_opt +=  "./cdmw_test_platform_interface ";
        argv_tmp = Cdmw::Common::String::to_char_array(
            Cdmw::Common::String::to_strings( str_opt ) );

        CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc_tmp, argv_tmp, orb_strategies);
	
        // ===================================================
        // Get the root POA 
        // ===================================================
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
      
        // ===================================================
        // Activate the root POA manager
        // ===================================================
        PortableServer::POAManager_var poaManager = rootPOA->the_POAManager();
        poaManager->activate();
 

	CORBA::PolicyList policies_1;
        policies_1.length(1);
        policies_1[0] =
	  rootPOA->create_id_assignment_policy(PortableServer::USER_ID);
	
        PortableServer::POA_var multithread_poa =
	  rootPOA->create_POA("POA-1", poaManager.in(), policies_1);
	
	// create callback
	TEST_INFO("create callback");
	CdmwPlatformMngt::ProcessCallback::ProcessAck processAck;


	CdmwPlatformMngt::ProcessMessageBroker_var  message_broker_ref =
	  CdmwPlatformMngt::ProcessMessageBroker::_nil();
	CdmwPlatformMngtEntity::EntityObserver_var  entity_observer_ref =
	  CdmwPlatformMngtEntity::EntityObserver::_nil();
	CdmwPlatformMngtService::ServiceBroker_var  service_broker_ref =
	  CdmwPlatformMngtService::ServiceBroker::_nil();
	
	// Create a ProcessMessageBroker object
        SimProcessMessageBroker_impl* message_broker =
	  new SimProcessMessageBroker_impl(); 
	
        PortableServer::ServantBase_var message_brokerServant = message_broker;
	
        {
	  PortableServer::ObjectId_var oid =
	    PortableServer::string_to_ObjectId(PROCESS_MESSAGE_BROKER_USER_OBJECT_ID);
	  multithread_poa->activate_object_with_id(oid.in(), message_brokerServant.in());
 	  message_broker_ref = message_broker->_this();
        }
	
	
        // Create an EntityObserver object
        SimEntityObserver_impl* entity_observer =
	  new SimEntityObserver_impl();

        PortableServer::ServantBase_var entity_observerServant = entity_observer;
	
        {
	  PortableServer::ObjectId_var oid =
	    PortableServer::string_to_ObjectId(ENTITY_OBSERVER_USER_OBJECT_ID);
	  multithread_poa->activate_object_with_id(oid.in(), entity_observerServant.in());
	  entity_observer_ref = entity_observer->_this();
        }
	
	
        // Create a ServiceBroker object
        SimServiceBroker_impl * service_broker =
	  new SimServiceBroker_impl();
	
        PortableServer::ServantBase_var service_brokerServant = service_broker;
	
        {
	  PortableServer::ObjectId_var oid =
	    PortableServer::string_to_ObjectId(SERVICE_BROKER_USER_OBJECT_ID);
	  multithread_poa -> activate_object_with_id(oid.in(), service_brokerServant.in());
	  service_broker_ref = service_broker -> _this();
        }
	
	
	processAck.application_name = "APPL1";
	processAck.process_name = "PROC1";
	
	processAck.entity_observer = CdmwPlatformMngtEntity::EntityObserver::_duplicate(entity_observer_ref.in());
	processAck.service_broker =  CdmwPlatformMngtService::ServiceBroker::_duplicate(service_broker_ref.in());
	processAck.process_message_broker = CdmwPlatformMngt::ProcessMessageBroker::_duplicate(message_broker_ref.in());
  
	
	Cdmw::PlatformInterface::ProcessCallback_impl* callbackServant_impl = 
	  new  Cdmw::PlatformInterface::ProcessCallback_impl (processAck);
	CdmwPlatformMngt::ProcessCallback_var process_callback = callbackServant_impl->_this();


	// Create the IOR callback
	
	
	// Initialise the platform management interface
	TEST_INFO("Initialise the platform management interface");
	
	
        // creates the hello process behaviour
	UserProcessBehaviour* pHelloProcess = new UserProcessBehaviour() ;

	TEST_INFO("Calling acknowledgeCreation before setup...");
	try 
	  {
	    Cdmw::PlatformMngt::PlatformInterface::acknowledgeCreation(pHelloProcess);
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadParameterException& ex)
	  {
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadOrderException& ex)
	  {
	    TEST_SUCCEED();
	  }



	TEST_INFO("Calling setup with no callback IOR argument...");

	// Call platform interface setup
	try 
	  {
	    Cdmw::PlatformMngt::PlatformInterface::setup(orb.in(), argc_tmp, argv_tmp);
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadParameterException& ex)
	  {
	    TEST_SUCCEED();
	  }
	catch(const Cdmw::BadOrderException& ex)
	  {
	    TEST_FAILED();
	  }


        str_opt +=  Cdmw::Common::Options::CALLBACK_IOR_OPTION;
        str_opt +=   "=";
	std::string callbackIOR = orb->object_to_string (process_callback.in());
        str_opt += callbackIOR;  	
        ++argc_tmp;
        
        argv_tmp = Cdmw::Common::String::to_char_array(
            Cdmw::Common::String::to_strings( str_opt ) );

	TEST_INFO("Calling setup with callback IOR argument...");

        // Call platform interface setup
	try 
	  {
	    Cdmw::PlatformMngt::PlatformInterface::setup(orb.in(), argc_tmp, argv_tmp);
	    TEST_SUCCEED();
	  }
	catch(const Cdmw::BadParameterException& ex)
	  {
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadOrderException& ex)
	  {
	    TEST_FAILED();
	  }
    

	TEST_INFO("Calling setup with callback IOR argument (second time)...");

        // Call platform interface setup
	try 
	  {
	    Cdmw::PlatformMngt::PlatformInterface::setup(orb.in(), argc_tmp, argv_tmp);
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadParameterException& ex)
	  {
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadOrderException& ex)
	  {
	    TEST_SUCCEED();
 	  }

        // creates the hello process behaviour with NULL
        std::auto_ptr<UserProcessBehaviour> pHelloProcess2(NULL);

    
	TEST_INFO("Calling acknowledgeCreation with a NULL parameter...");
	try 
	  {
	    Cdmw::PlatformMngt::PlatformInterface::acknowledgeCreation(pHelloProcess2.get());
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadParameterException& ex)
	  {
	    TEST_SUCCEED();
	  }
	catch(const Cdmw::BadOrderException& ex)
	  {
	    TEST_FAILED();
	  }


	TEST_INFO("check the notifyMessage before the acknowledgeCreation...");
	try
	  {
	    PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
					     "process",
					     "INF test");
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadParameterException& ex)
	  {
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadOrderException& ex)
	  {
	    TEST_SUCCEED();
	  }

	TEST_INFO("check the getApplicationName before the acknowledgeCreation...");
	try
	  {

	    Cdmw::PlatformMngt::PlatformInterface::getApplicationName();
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadParameterException& ex)
	  {
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadOrderException& ex)
	  {
	    TEST_SUCCEED();
	  }

	TEST_INFO("check the getProcessName before the acknowledgeCreation...");
	try
	  {

	    Cdmw::PlatformMngt::PlatformInterface::getProcessName();
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadParameterException& ex)
	  {
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadOrderException& ex)
	  {
	    TEST_SUCCEED();
	  }

	TEST_INFO("check the notifyFatalError FTL before the acknowledgeCreation...");
	try
	  {
	    PlatformInterface::notifyFatalError("process",
						"FTL test");
	    TEST_SUCCEED();
	  }
	catch(const Cdmw::BadParameterException& ex)
	  {
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadOrderException& ex)
	  {
	    TEST_FAILED();
	  }

	TEST_INFO("Check the setSystemEntityStatus before the acknowledgeCreation...");

	try
	  {
	    PlatformInterface::setSystemEntityStatus ( "entity", 
						       CdmwPlatformMngtEntity::ENTITY_FUNCTIONING,
						       "functioning");
	    TEST_FAILED();
	  }
	catch(const CdmwPlatformMngtEntity::EntityNotFound& ex)
	  {
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadParameterException& ex)
	  {
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadOrderException& ex)
	  {
	    TEST_SUCCEED();
	  }
	
	TEST_INFO("");




	TEST_INFO("Calling acknowledgeCreation ...");
	try 
	  {
	    Cdmw::PlatformMngt::PlatformInterface::acknowledgeCreation(pHelloProcess);
	    TEST_SUCCEED();
	  }
	catch(const Cdmw::BadParameterException& ex)
	  {
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadOrderException& ex)
	  {
	    TEST_FAILED();
	  }


	
	TEST_INFO("getProcess");
	CdmwPlatformMngt::Process_var process_proxy =  callbackServant_impl->getProcess();
	
	TEST_INFO("check the application name");
	std::string applicationName = Cdmw::PlatformMngt::PlatformInterface::getApplicationName();
	TEST_CHECK( applicationName == "APPL1");

	TEST_INFO("check the process name");
	std::string processName = Cdmw::PlatformMngt::PlatformInterface::getProcessName();
	TEST_CHECK( processName == "PROC1");

	TEST_INFO("check the notifyMessage INF");
	try
	  {
	    PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
					     Cdmw::PlatformMngt::PlatformInterface::getProcessName(),
					     "INF test");
	    TEST_SUCCEED();
	  }
	catch( ... )
	  {
	    TEST_FAILED();
	  }
	
	TEST_INFO("check the notifyMessage WRN");
	try
	  {
	    PlatformInterface::notifyMessage(CdmwPlatformMngtBase::WRN,
					     Cdmw::PlatformMngt::PlatformInterface::getProcessName(),
					     "WRN test");
	    TEST_SUCCEED();
	  }
	catch( ... )
	  {
	    TEST_FAILED();
	  }

	TEST_INFO("check the notifyMessage ERR");
	try
	  {
	    PlatformInterface::notifyMessage(CdmwPlatformMngtBase::ERR,
					     Cdmw::PlatformMngt::PlatformInterface::getProcessName(),
					     "ERR test");
	    TEST_SUCCEED();
	  }
	catch( ... )
	  {
	    TEST_FAILED();
	  }

	TEST_INFO("check the notifyMessage FTL");
	try
	  {
	    PlatformInterface::notifyMessage(CdmwPlatformMngtBase::FTL,
					     Cdmw::PlatformMngt::PlatformInterface::getProcessName(),
					     "FTL test");
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadParameterException& ex)
	  {
	    TEST_SUCCEED();
	  }
	catch(const Cdmw::BadOrderException& ex)
	  {
	    TEST_FAILED();
	  }

	TEST_INFO("check the notifyMessage with a NULL parameter");
	try
	  {
	    PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
					     NULL,
					     "INF test");
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadParameterException& ex)
	  {
	    TEST_SUCCEED();
	  }
	catch(const Cdmw::BadOrderException& ex)
	  {
	    TEST_FAILED();
	  }

	TEST_INFO("check the notifyMessage with PFMMNG issuer");
	try
	  {
	    PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
					     CdmwPlatformMngtBase::PFMMNG,
					     "INF test");
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadParameterException& ex)
	  {
	    TEST_SUCCEED();
	  }
	catch(const Cdmw::BadOrderException& ex)
	  {
	    TEST_FAILED();
	  }

	TEST_INFO("check the notifyFatalError FTL");
	try
	  {
	    PlatformInterface::notifyFatalError(Cdmw::PlatformMngt::PlatformInterface::getProcessName(),
						"FTL test");
	    TEST_SUCCEED();
	  }
	catch(const Cdmw::BadParameterException& ex)
	  {
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadOrderException& ex)
	  {
	    TEST_FAILED();
	  }

	TEST_INFO("check the notifyFatalError FTL with a NULL parameter");
	try
	  {
	    PlatformInterface::notifyFatalError(NULL,
						"FTL test");
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadParameterException& ex)
	  {
	    TEST_SUCCEED();
	  }
	catch(const Cdmw::BadOrderException& ex)
	  {
	    TEST_FAILED();
	  }

	TEST_INFO("Check the setSystemEntityStatus with a NULL parameter");

	try
	  {
	    PlatformInterface::setSystemEntityStatus ( NULL, 
						       CdmwPlatformMngtEntity::ENTITY_FUNCTIONING,
						       "functioning");
	    TEST_FAILED();
	  }
	catch(const CdmwPlatformMngtEntity::EntityNotFound& ex)
	  {
	    TEST_FAILED();
	  }
	catch(const Cdmw::BadParameterException& ex)
	  {
	    TEST_SUCCEED();
	  }
	catch(const Cdmw::BadOrderException& ex)
	  {
	    TEST_FAILED();
	  }
	
	

	TEST_INFO("getProcess");
	process_proxy =  callbackServant_impl->getProcess();

	CdmwPlatformMngtBase::StartupKind startup_kind;
	startup_kind.state_retrieval = CdmwPlatformMngtBase::LAST_STATE;
	startup_kind.startup_mode = 0;
	startup_kind.specified_state_id = CORBA::string_dup("");
	
	TEST_INFO("Check the current step");
	TEST_CHECK(pHelloProcess->getCurrentStep() == 1);
	
	TEST_INFO("Check the number of step");
	TEST_CHECK(process_proxy->nb_steps() == 2);

	
	TEST_INFO("Initilise the process");
	process_proxy->initialise(startup_kind);
	TEST_SUCCEED();

	TEST_INFO("Check the current step");
	TEST_CHECK(pHelloProcess->getCurrentStep() == 1);

	
	TEST_INFO("Next step the process");
	process_proxy->next_step();
	TEST_SUCCEED();
	try
	  { 
	    TEST_INFO("Next step the process");
	    process_proxy->next_step();
	    TEST_FAILED();
	  }
	catch (const CdmwPlatformMngt::Process::InvalidStep& e)
	  {
	    TEST_SUCCEED();
	  }

	TEST_INFO("Check the current step");
	TEST_CHECK(pHelloProcess->getCurrentStep() == 2);	

	TEST_INFO("Run the process");
	process_proxy->run();
	TEST_SUCCEED();

	TEST_INFO("Stop the process");
	process_proxy->stop();
	TEST_SUCCEED();

	TEST_INFO("Calling get_service");
	TEST_CHECK(process_proxy->get_service() == NULL);


	
	TEST_INFO("Destroy the ORB");
	
	orb->shutdown(false);
	Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
	orb->destroy();

	TEST_INFO("End of test");

   }
  
  catch (...)
    {
    }


}

