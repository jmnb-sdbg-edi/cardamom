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


#include "Foundation/common/Assert.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/Exception.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"


#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/idllib/FTPullMonitorable.skel.hpp"

#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platforminterface/ServiceNames.hpp"
#include "SystemMngt/platforminterface/ProcessBehaviour.hpp"

#include "testplatforminterface/UserProcessBehaviour.hpp"
#include "testplatforminterface/ProcessCallback_impl.hpp"

#include "testplatforminterface/SimProcessMessageBroker_impl.hpp"
#include "testplatforminterface/PlatformInterfaceTest.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtEntity.stub.hpp"
	  

CPPUNIT_TEST_SUITE_REGISTRATION( PlatformInterfaceTest ) ;

PlatformInterfaceTest::PlatformInterfaceTest()
  throw()
{
}

PlatformInterfaceTest::~PlatformInterfaceTest()
  throw()
{
}


void PlatformInterfaceTest::do_tests()
{
    // set number of requested successfull tests
// //     set_nbOfRequestedTestOK (30);

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
	    CdmwPlatformMngt::ProcessCallback::ProcessStartingData process_start_data;


	    CdmwPlatformMngt::ProcessMessageBroker_var  message_broker_ref =
	    CdmwPlatformMngt::ProcessMessageBroker::_nil();

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
	
	
	    process_start_data.application_name = "APPL1";
	    process_start_data.process_name = "PROC1";
	
	    process_start_data.process_message_broker = 
	          CdmwPlatformMngt::ProcessMessageBroker::_duplicate(message_broker_ref.in());
  
	
	    Cdmw::PlatformInterface::ProcessCallback_impl* callbackServant_impl = 
	        new  Cdmw::PlatformInterface::ProcessCallback_impl (process_start_data);
	    CdmwPlatformMngt::ProcessCallback_var process_callback = 
	           callbackServant_impl->_this();


	    // Create the IOR callback
	
	
	    // Initialise the platform management interface
	    TEST_INFO("Initialise the platform management interface");
	
	
        // creates the hello process behaviour
	    UserProcessBehaviour* pHelloProcess = new UserProcessBehaviour() ;

	    TEST_INFO("Calling acknowledgeCreation before setup...");
	    try 
	    {
                // TODO?: maybe we have to modify process behavior
	        Cdmw::PlatformMngt::PlatformInterface::Acknowledge_creation(pHelloProcess);
	        CPPUNIT_ASSERT(false);
	    }
	    catch(const Cdmw::BadParameterException& ex)
	    {
	        CPPUNIT_ASSERT(false);
	    }
	    catch(const Cdmw::BadOrderException& ex)
	    {
	        CPPUNIT_ASSERT(true);
	    }



	    TEST_INFO("Calling setup with no callback IOR argument...");

	    // Call platform interface setup
	    try 
	    {
	        Cdmw::PlatformMngt::PlatformInterface::Setup(orb.in(), argc_tmp, argv_tmp);
	        CPPUNIT_ASSERT(false);
	    }
	    catch(const Cdmw::BadParameterException& ex)
	    {
	        CPPUNIT_ASSERT(true);
	    }
	    catch(const Cdmw::BadOrderException& ex)
	    {
	        CPPUNIT_ASSERT(false);
	    }


        str_opt +=  Cdmw::Common::Options::CALLBACK_IOR_OPTION;
        str_opt +=   "=";
	    std::string callbackIOR = orb->object_to_string (process_callback.in());
        str_opt += callbackIOR;  	
        ++argc_tmp;
        
        argv_tmp = Cdmw::Common::String::to_char_array(
            Cdmw::Common::String::to_strings (str_opt));

	    TEST_INFO("Calling setup with callback IOR argument...");

        // Call platform interface setup
	    try 
	    {
	        Cdmw::PlatformMngt::PlatformInterface::Setup(orb.in(), argc_tmp, argv_tmp);
	        CPPUNIT_ASSERT(true);
	    }
	    catch(const Cdmw::BadParameterException& ex)
	    {
	        CPPUNIT_ASSERT(false);
	    }
	    catch(const Cdmw::BadOrderException& ex)
	    {
	        CPPUNIT_ASSERT(false);
	    }
    

	    TEST_INFO("Calling setup with callback IOR argument (second time)...");

        // Call platform interface setup
	    try 
	    {
	        Cdmw::PlatformMngt::PlatformInterface::Setup(orb.in(), argc_tmp, argv_tmp);
	        CPPUNIT_ASSERT(false);
	    }
	    catch(const Cdmw::BadParameterException& ex)
	    {
	        CPPUNIT_ASSERT(false);
	    }
	    catch(const Cdmw::BadOrderException& ex)
	    {
	        CPPUNIT_ASSERT(true);
 	    }

        // creates the hello process behaviour with NULL
        std::auto_ptr<UserProcessBehaviour> pHelloProcess2(NULL);

    
	    TEST_INFO("Calling acknowledgeCreation with a NULL parameter...");
	    try 
	    {
	        Cdmw::PlatformMngt::PlatformInterface::Acknowledge_creation(pHelloProcess2.get());
	        CPPUNIT_ASSERT(false);
	    }
	    catch(const Cdmw::BadParameterException& ex)
	    {
	        CPPUNIT_ASSERT(true);
	    }
	    catch(const Cdmw::BadOrderException& ex)
	    {
	        CPPUNIT_ASSERT(false);
	    }


	    TEST_INFO("check the notifyMessage before the acknowledgeCreation...");
	    try
	    {
	        PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
					     "process",
					     "INF test");
	        CPPUNIT_ASSERT(false);
	    }
	    catch(const Cdmw::BadParameterException& ex)
	    {
	        CPPUNIT_ASSERT(false);
	    }
	    catch(const Cdmw::BadOrderException& ex)
	    {
	        CPPUNIT_ASSERT(true);
	    }

	    TEST_INFO("check the getApplicationName before the acknowledgeCreation...");
	    try
	    {
	        Cdmw::PlatformMngt::PlatformInterface::Get_application_name();
	        CPPUNIT_ASSERT(false);
	    }
	    catch(const Cdmw::BadParameterException& ex)
	    {
	        CPPUNIT_ASSERT(false);
	    }
	    catch(const Cdmw::BadOrderException& ex)
	    {
	        CPPUNIT_ASSERT(true);
	    }

	    TEST_INFO("check the Get_process_name before the acknowledgeCreation...");
	    try
	    {
	        Cdmw::PlatformMngt::PlatformInterface::Get_process_name();
	        CPPUNIT_ASSERT(false);
	    }
	    catch(const Cdmw::BadParameterException& ex)
	    {
	        CPPUNIT_ASSERT(false);
	    }
	    catch(const Cdmw::BadOrderException& ex)
	    {
	        CPPUNIT_ASSERT(true);
	    }

	    TEST_INFO("check the notifyFatalError FTL before the acknowledgeCreation...");
	    try
	    {
	        PlatformInterface::Notify_fatal_error("process",
						"FTL test");
	        CPPUNIT_ASSERT(true);
	    }
	    catch(const Cdmw::BadParameterException& ex)
	    {
	        CPPUNIT_ASSERT(false);
	    }
	    catch(const Cdmw::BadOrderException& ex)
	    {
	        CPPUNIT_ASSERT(false);
	    }



	    TEST_INFO("Calling acknowledgeCreation ...");
	    try 
	    {
	        Cdmw::PlatformMngt::PlatformInterface::Acknowledge_creation(pHelloProcess);
	        CPPUNIT_ASSERT(true);
	    }
	    catch(const Cdmw::BadParameterException& ex)
	    {
	        CPPUNIT_ASSERT(false);
	    }
	    catch(const Cdmw::BadOrderException& ex)
	    {
	        CPPUNIT_ASSERT(false);
	    }


	
	    TEST_INFO("getProcess");
	    CdmwPlatformMngt::ProcessDelegate_var process_delegate =  callbackServant_impl->getProcess();
	
	    TEST_INFO("check the application name");
	    std::string applicationName = Cdmw::PlatformMngt::PlatformInterface::Get_application_name();
	    CPPUNIT_ASSERT( applicationName == "APPL1");

	    TEST_INFO("check the process name");
	    std::string processName = Cdmw::PlatformMngt::PlatformInterface::Get_process_name();
	    CPPUNIT_ASSERT( processName == "PROC1");

	    TEST_INFO("check the notifyMessage INF");
	    try
	    {
	        PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
					     Cdmw::PlatformMngt::PlatformInterface::Get_process_name(),
					     "INF test");
	        CPPUNIT_ASSERT(true);
	    }
	    catch( ... )
	    {
	        CPPUNIT_ASSERT(false);
	    }
	
	    TEST_INFO("check the notifyMessage WRN");
	    try
	    {
	        PlatformInterface::Notify_message(CdmwPlatformMngtBase::WRN,
					     Cdmw::PlatformMngt::PlatformInterface::Get_process_name(),
					     "WRN test");
	        CPPUNIT_ASSERT(true);
	    }
	    catch( ... )
	    {
	        CPPUNIT_ASSERT(false);
	    }

	    TEST_INFO("check the notifyMessage ERR");
	    try
	    {
	        PlatformInterface::Notify_message(CdmwPlatformMngtBase::ERR,
					     Cdmw::PlatformMngt::PlatformInterface::Get_process_name(),
					     "ERR test");
	        CPPUNIT_ASSERT(true);
	    }
	    catch( ... )
	    {
	        CPPUNIT_ASSERT(false);
	    }

	    TEST_INFO("check the notifyMessage FTL");
	    try
	    {
	        PlatformInterface::Notify_message(CdmwPlatformMngtBase::FTL,
					     Cdmw::PlatformMngt::PlatformInterface::Get_process_name(),
					     "FTL test");
	        CPPUNIT_ASSERT(false);
	    }
	    catch(const Cdmw::BadParameterException& ex)
	    {
	        CPPUNIT_ASSERT(true);
	    }
	    catch(const Cdmw::BadOrderException& ex)
	    {
	        CPPUNIT_ASSERT(false);
	    }

	    TEST_INFO("check the notifyMessage with a NULL parameter");
	    try
	    {
	        PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
					     NULL,
					     "INF test");
	        CPPUNIT_ASSERT(false);
	    }
	    catch(const Cdmw::BadParameterException& ex)
	    {
	        CPPUNIT_ASSERT(true);
	    }
	    catch(const Cdmw::BadOrderException& ex)
	    {
	        CPPUNIT_ASSERT(false);
	    }

	    TEST_INFO("check the notifyMessage with PFMMNG issuer");
	    try
	    {
	        PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
					     CdmwPlatformMngtBase::PFMMNG,
					     "INF test");
	        CPPUNIT_ASSERT(false);
	    }
	    catch(const Cdmw::BadParameterException& ex)
	    {
	        CPPUNIT_ASSERT(true);
	    }
	    catch(const Cdmw::BadOrderException& ex)
	    {
	        CPPUNIT_ASSERT(false);
	    }

	    TEST_INFO("check the notifyFatalError FTL");
	    try
	    {
	        PlatformInterface::Notify_fatal_error(Cdmw::PlatformMngt::PlatformInterface::Get_process_name(),
						"FTL test");
	        CPPUNIT_ASSERT(true);
	    }
	    catch(const Cdmw::BadParameterException& ex)
	    {
	        CPPUNIT_ASSERT(false);
	    }
	    catch(const Cdmw::BadOrderException& ex)
	    {
	        CPPUNIT_ASSERT(false);
	    }

	    TEST_INFO("check the notifyFatalError FTL with a NULL parameter");
	    try
	    {
	        PlatformInterface::Notify_fatal_error(NULL,
						"FTL test");
	        CPPUNIT_ASSERT(false);
	    }
	    catch(const Cdmw::BadParameterException& ex)
	    {
	        CPPUNIT_ASSERT(true);
	    }
	    catch(const Cdmw::BadOrderException& ex)
	    {
	        CPPUNIT_ASSERT(false);
	    }


	    TEST_INFO("getProcess");
	    process_delegate =  callbackServant_impl->getProcess();

	    CdmwPlatformMngtBase::StartupKind startup_kind;
	    startup_kind.state_retrieval = CdmwPlatformMngtBase::LAST_STATE;
	    startup_kind.startup_mode = 0;
	    startup_kind.specified_state_id = CORBA::string_dup("");
	
	    TEST_INFO("Check the current step");
	    CPPUNIT_ASSERT(pHelloProcess->getCurrentStep() == 1);
	
	    TEST_INFO("Check the number of step");
	    CPPUNIT_ASSERT(process_delegate->nb_steps() == 2);

	
	    TEST_INFO("Initilise the process");
	    process_delegate->initialise(startup_kind);
	    CPPUNIT_ASSERT(true);

	    TEST_INFO("Check the current step");
	    CPPUNIT_ASSERT(pHelloProcess->getCurrentStep() == 1);

	
	    TEST_INFO("Next step the process");
	    process_delegate->next_step();
	    CPPUNIT_ASSERT(true);
	    try
	    { 
	        TEST_INFO("Next step the process");
	        process_delegate->next_step();
	        CPPUNIT_ASSERT(false);
	    }
	    catch (const CdmwPlatformMngt::ProcessDelegate::InvalidStep& e)
	    {
	        CPPUNIT_ASSERT(true);
	    }

	    TEST_INFO("Check the current step");
	    CPPUNIT_ASSERT(pHelloProcess->getCurrentStep() == 2);	

	    TEST_INFO("Run the process");
	    process_delegate->run();
	    CPPUNIT_ASSERT(true);

	    TEST_INFO("Stop the process");
	    process_delegate->stop();
	    CPPUNIT_ASSERT(true);

	    TEST_INFO("Calling get_service");
	    CPPUNIT_ASSERT(process_delegate->get_service() == NULL);


        // cleanup the platform interface
        PlatformInterface::Cleanup();
	
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

