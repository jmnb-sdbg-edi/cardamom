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

// Std include files
#include <iostream>
#include <sstream>
#include <string.h>

// CORBA include files
#include <Foundation/ossupport/OS.hpp>
#include "Foundation/orbsupport/StrategyList.hpp"
#include <Foundation/orbsupport/OrbSupport.hpp>

#include "testacknowledgingsimple/OrbThread.hpp"

#include "testacknowledgingsimple/TestAcknowledgingSimpleMonitoring.hpp"
#include "ftmonitoring/MonitoringLibraryInterface_impl.hpp"

using namespace std;

TestAcknowledgingSimpleMonitoring::TestAcknowledgingSimpleMonitoring ()
	:Cdmw::TestUtils::Testable("Test Acknowledging Simple Monitoring")
{
}

TestAcknowledgingSimpleMonitoring::TestAcknowledgingSimpleMonitoring (int argc, char* argv[], int timeOn)
  : Cdmw::TestUtils::Testable("Test Acknowledging Simple Monitoring"),
    m_timeOn(timeOn)
{
    for (int i=0; i<argc; i++) m_argList.push_back(argv[i]);

    Cdmw::OrbSupport::StrategyList strategyList;
    strategyList.add_OrbThreaded(); // Mandatory in OrbSupport::ORB_init
    strategyList.add_multicast();
    TEST_INFO("Initialisation of the ORB");
    m_orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);
     /*
    TAO_Internal::default_svc_conf_entries
    (
      "dynamic UIPMC_Factory Service_Object * TAO_PortableGroup:_make_TAO_UIPMC_Protocol_Factory() \"\"\nstatic Resource_Factory \"-ORBProtocolFactory IIOP_Factory -ORBProtocolFactory UIPMC_Factory\"",
      "",
      ""
    );
    m_orb = CORBA::ORB_init (argc, argv);
    */
    TEST_SUCCEED();
}

TestAcknowledgingSimpleMonitoring::~TestAcknowledgingSimpleMonitoring ()
{
    m_argList.clear();
}
	
void 
TestAcknowledgingSimpleMonitoring::do_tests()
{
    				    
    set_nbOfRequestedTestOK(9);
    using namespace CdmwFT;

    try {

    cout << "Start Orb thread" << std::endl;
    OrbThread orbThread(m_orb.in());
    orbThread.start();
    Cdmw::OsSupport::OS::sleep(50);
    
	TEST_INFO("Get RootPoa");
	CORBA::Object_var object = m_orb->resolve_initial_references("RootPOA");
	PortableServer::POA_var poa = PortableServer::POA::_narrow(object.in());
	TEST_SUCCEED();

	TEST_INFO("Get Poa Manager");
	PortableServer::POAManager_var poaManager =
	poa->the_POAManager();
	TEST_SUCCEED();

	TEST_INFO("Activate Poa Manager");
	poaManager->activate();
	TEST_SUCCEED();

	TEST_INFO("Init MonitoringLibraryInterface");
        MonitoringLibrary::MonitoringLibraryInterface_impl::Init(m_orb.in(), m_argList);
        TEST_SUCCEED();

	TEST_INFO("Resolve initial reference \"MonitoringLibrary\"");
	CORBA::Object_var monitLibObj = m_orb->resolve_initial_references("MonitoringLibrary");
	TEST_SUCCEED();
        
	TEST_INFO("Get the reference of the monitoring library");
	MonitoringLibrary::MonitoringLibraryInterface_var lib = 
	MonitoringLibrary::MonitoringLibraryInterface::_narrow(monitLibObj.in());
	TEST_SUCCEED();
	
	TEST_INFO("Create the acknowledging simple monitoring");
	MonitoringLibrary::SimpleMonitoring::AcknowledgingSimpleMonitoring_var ackSimpleLib =
	  lib.in()->create_acknowledging_simple_monitoring();
	TEST_SUCCEED();
	        
	
	TEST_INFO("Start the acknowledging simple monitoring");
	ackSimpleLib->start_monitoring();
	TEST_SUCCEED();
		
	if (m_timeOn == 0)
	{
          char dummy;
          cin >> dummy;
        }
        else Cdmw::OsSupport::OS::sleep(m_timeOn);

	TEST_INFO("Stop the acknowledging simple monitoring");
	ackSimpleLib->stop_monitoring();
	TEST_SUCCEED();
         
        Cdmw::OsSupport::OS::sleep(50);

    cout << "Stop Orb Thread" << std::endl;
    orbThread.destroy();
    orbThread.join();

    } catch (CORBA::Exception &e) {
	std::ostringstream info_stream1;
	info_stream1 << "CORBA Exception caught : " << e << endl;
	TEST_INFO(info_stream1.str());
        TEST_FAILED();	
    } catch (...) {
	std::ostringstream info_stream2;
	info_stream2 << "Supervision Exception caught : " << endl;
	TEST_INFO(info_stream2.str());	
        TEST_FAILED();
    }
    
}

