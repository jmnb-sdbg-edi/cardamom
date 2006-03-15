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
#include <fstream>

// CORBA include files
#include <Foundation/ossupport/OS.hpp>
#include "Foundation/orbsupport/StrategyList.hpp"
#include <Foundation/orbsupport/OrbSupport.hpp>

#include "testinterrogatingsimple/TestInterrogatingSimpleMonitoring.hpp"
#include "testinterrogatingsimple/OrbThread.hpp"
#include "ftmonitoring/MonitoringLibraryInterface_impl.hpp"

using namespace std;

void TestMonitoringObserver_impl::on_start (
  const char * the_location )
  throw(CORBA::SystemException)
{
  cout << "MonitoringObserver ON_START called, the location:"
       << the_location << std::endl;
}

void TestMonitoringObserver_impl::on_failure (
  const char * the_location )
  throw(CORBA::SystemException)
{
  cout << "MonitoringObserver ON_FAILURE called, the location:"
       << the_location << std::endl;
}

TestInterrogatingSimpleMonitoring::TestInterrogatingSimpleMonitoring ()
    : Cdmw::TestUtils::Testable("Test Interrogating Simple Monitoring")
{
}

TestInterrogatingSimpleMonitoring::TestInterrogatingSimpleMonitoring (int argc, char* argv[], int timeOn)
  : Cdmw::TestUtils::Testable("Test Interrogating Simple Monitoring"),
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
      "dynamic PortableGroup_Loader Service_Object * TAO_PortableGroup:_make_TAO_PortableGroup_Loader() \"\"",
      ""
    );
    m_orb = CORBA::ORB_init (argc, argv);
     */
    TEST_SUCCEED();    
}

TestInterrogatingSimpleMonitoring::~TestInterrogatingSimpleMonitoring ()
{
    m_argList.clear();
}
	
void TestInterrogatingSimpleMonitoring::do_tests()
{
    				    
    set_nbOfRequestedTestOK(14);

    try {
      
        TEST_INFO("Start Orb thread");
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
	
	// TEST_INFO("Build the locations sequence");	
	MonitoringLibrary::Locations_var locationSeq = new MonitoringLibrary::Locations;
        
	const char * filename = "location_list.txt";
	std::ifstream confStream(filename);
	std::string line;

	while(std::getline(confStream,line))
	{
		locationSeq->length(locationSeq->length() + 1);
		locationSeq[locationSeq->length() - 1] = line.c_str();
	}
	TEST_SUCCEED();	
	
	TEST_INFO("Create the interrogating simple monitoring");
	MonitoringLibrary::SimpleMonitoring::InterrogatingSimpleMonitoring_var interroSimpleLib =
	  lib.in()->create_interrogating_simple_monitoring(locationSeq.in());
	TEST_SUCCEED();
	
	TEST_INFO("Instanciate a monitoring observer");
	MonitoringLibrary::MonitoringObserver_var monitObs =
          new TestMonitoringObserver_impl();
	
	TEST_INFO("Attach this monitoring observer");
	interroSimpleLib->attach_observer(monitObs.in());
	TEST_SUCCEED();

        TEST_INFO("Try to attach same monitoring observer");
        try
	{
	  interroSimpleLib->attach_observer(monitObs.in());
          TEST_FAILED();
        }
        catch(const MonitoringLibrary::AlreadyExists&)
	{
          TEST_SUCCEED();
	}
        
	TEST_INFO("Start the simple monitoring");
	interroSimpleLib->start_monitoring();
	TEST_SUCCEED();
        cout << "I Simple Monitoring started: if no acknowledging launched" << std::endl;
        cout << "I (for ex. in auto test) you should soon see ON_FAILURE" << std::endl;
        cout << "I for all the hosts present in " << filename << std::endl;

	if (m_timeOn == 0)
	{
          char dummy;
          cin >> dummy;
        }
        else Cdmw::OsSupport::OS::sleep(m_timeOn);

	TEST_INFO("Stop the simple monitoring");
	interroSimpleLib->stop_monitoring();
	TEST_SUCCEED();
	
	TEST_INFO("Instanciate another monitoring observer");
	MonitoringLibrary::MonitoringObserver_var monitObs2 =
          new TestMonitoringObserver_impl();

	TEST_INFO("Try to detach this monitoring observer");
        try
        {
	  interroSimpleLib->detach_observer(monitObs2.in());
          TEST_FAILED();
        }
        catch (const MonitoringLibrary::NotFound&)
	{
	  TEST_SUCCEED();
        }

	TEST_INFO("Detach our monitoring observer");
	interroSimpleLib->detach_observer(monitObs.in());
	TEST_SUCCEED();
    	
        TEST_INFO("Stop Orb Thread");
        orbThread.destroy();
        orbThread.join();

	if (m_timeOn == 0)
	{
          cout << "When you have finish to read log press key + enter" << std::endl;
          char dummy;
          cin >> dummy;
        }

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

