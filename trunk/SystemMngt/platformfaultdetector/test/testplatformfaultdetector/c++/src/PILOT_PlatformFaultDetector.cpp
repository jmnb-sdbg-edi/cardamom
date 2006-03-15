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


#include <iostream>

#include "testplatformfaultdetector/PILOT_PlatformFaultDetector.hpp"
#include <sstream>

#include <list>

#include "Foundation/common/Options.hpp"
#include "Foundation/testutils/TestManager.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "testplatformfaultdetector/SIM_OrbThread.hpp"

#include "platformfaultdetector/LocalFaultDetector_impl.hpp"

#include "testplatformfaultdetector/PullMonitorable_impl.hpp"
#include "SystemMngt/platformlibrary/ProcessKillMngr.hpp"
CPPUNIT_TEST_SUITE_REGISTRATION (Cdmw::PlatformMngt::
				 PILOT_PlatformFaultDetector);

namespace Cdmw
{
  namespace PlatformMngt
  {

/**
 *Implementation notes: none 
 *<p>
 *Portability issues: none 
 *<p>
 */

    PILOT_PlatformFaultDetector::PILOT_PlatformFaultDetector () throw ()
    {

    }


    PILOT_PlatformFaultDetector::~PILOT_PlatformFaultDetector () throw ()
    {

    }


    void PILOT_PlatformFaultDetector::do_tests ()
    {
      // set number of requested successfull tests
// // //     set_nbOfRequestedTestOK (10);

      TEST_INFO ("Creating the ORB");
      OrbThread orbThread;

      try
      {
	orbThread.start ();
      }
      catch (...)
      {
	return;
      }


      int timescale = Cdmw::TestUtils::get_timescale ();

      OsSupport::OS::sleep (timescale * 4000);

      CORBA::ORB_var orb = orbThread.orb ();
      PortableServer::POA_var poa = orbThread.poa ();

      // Defines the service ports for each PullMonitorable CORBA object
      // Those services ports will be used to construct each corbaloc 
      // associated with a PullMonitorable CORBA object
      const std::string defServicePort1 = "55234";
      const std::string defServicePort2 = "55235";
      const std::string defServicePort3 = "55236";
      const std::string defServicePort4 = "55237";
      const std::string defServicePort5 = "55238";

      // Defines the arguments that will be passed to OS process executing
      // the first PullMonitorable CORBA object implementation

      const std::string pullMonitorableName1 = "pull_1";

      std::ostringstream pullArgs1;
      pullArgs1 << "--server" << " ";
      pullArgs1 << Cdmw::Common::Options::
	LOCALISATION_SERVICE_OPTION << "=" << defServicePort1 << " ";
      pullArgs1 << "--PullMonitorableName" << "=" << pullMonitorableName1 <<
	" ";
      pullArgs1 << "--Sleep" << "=" << "200" << " ";
      pullArgs1 << "--Request" << "=" << "true" << " ";
      pullArgs1 << "--Exception" << "=" << "false";

      std::string pullArgsStr1 = pullArgs1.str ();

      // Defines the arguments that will be passed to OS process executing
      // the second PullMonitorable CORBA object implementation

      const std::string pullMonitorableName2 = "pull_2";

      std::ostringstream pullArgs2;
      pullArgs2 << "--server" << " ";
      pullArgs2 << Cdmw::Common::Options::
	LOCALISATION_SERVICE_OPTION << "=" << defServicePort2 << " ";
      pullArgs2 << "--PullMonitorableName" << "=" << pullMonitorableName2 <<
	" ";
      pullArgs2 << "--Sleep" << "=" << "200" << " ";
      pullArgs2 << "--Request" << "=" << "true" << " ";
      pullArgs2 << "--Exception" << "=" << "false";

      std::string pullArgsStr2 = pullArgs2.str ();

      // Defines the arguments that will be passed to OS process executing
      // the third PullMonitorable CORBA object implementation

      const std::string pullMonitorableName3 = "pull_3";

      std::ostringstream pullArgs3;
      pullArgs3 << "--server" << " ";
      pullArgs3 << Cdmw::Common::Options::
	LOCALISATION_SERVICE_OPTION << "=" << defServicePort3 << " ";
      pullArgs3 << "--PullMonitorableName" << "=" << pullMonitorableName3 <<
	" ";
      pullArgs3 << "--Sleep" << "=" << "200" << " ";
      pullArgs3 << "--Request" << "=" << "false" << " ";
      pullArgs3 << "--Exception" << "=" << "false";

      std::string pullArgsStr3 = pullArgs3.str ();

      // Defines the arguments that will be passed to OS process executing
      // the fourth PullMonitorable CORBA object implementation

      const std::string pullMonitorableName4 = "pull_4";

      std::ostringstream pullArgs4;
      pullArgs4 << "--server" << " ";
      pullArgs4 << Cdmw::Common::Options::
	LOCALISATION_SERVICE_OPTION << "=" << defServicePort4 << " ";
      pullArgs4 << "--PullMonitorableName" << "=" << pullMonitorableName4 <<
	" ";
      pullArgs4 << "--Sleep" << "=" << "200" << " ";
      pullArgs4 << "--Request" << "=" << "true" << " ";
      pullArgs4 << "--Exception" << "=" << "true";

      std::string pullArgsStr4 = pullArgs4.str ();

      // Defines the arguments that will be passed to OS process executing
      // the fifth PullMonitorable CORBA object implementation

      const std::string pullMonitorableName5 = "pull_5";

      std::ostringstream pullArgs5;
      pullArgs5 << "--server" << " ";
      pullArgs5 << Cdmw::Common::Options::
	LOCALISATION_SERVICE_OPTION << "=" << defServicePort5 << " ";
      pullArgs5 << "--PullMonitorableName" << "=" << pullMonitorableName5 <<
	" ";
      pullArgs5 << "--Sleep" << "=" << "800" << " ";
      pullArgs5 << "--Request" << "=" << "true" << " ";
      pullArgs5 << "--Exception" << "=" << "false";

      std::string pullArgsStr5 = pullArgs5.str ();

      OsSupport::OS::ProcessId pullOSId1 = 0;
      OsSupport::OS::ProcessId pullOSId2 = 0;
      OsSupport::OS::ProcessId pullOSId3 = 0;
      OsSupport::OS::ProcessId pullOSId4 = 0;
      OsSupport::OS::ProcessId pullOSId5 = 0;

      try
      {

	TEST_INFO
	  ("Creates a first OS process to execute a pullMonitorable corba object server");

	pullOSId1 =
	  OsSupport::OS::create_process ("cdmw_test_platformfaultdetector",
					 pullArgsStr1);

	OsSupport::OS::sleep (timescale * 5000);

	std::string corbalocId1 ("PullMonitorable_");
	corbalocId1 += pullMonitorableName1;

	std::ostringstream corbaloc_ref1;
	corbaloc_ref1 << "corbaloc::localhost:" << defServicePort1;
	corbaloc_ref1 << "/" << corbalocId1;

	std::string corbaloc1 = corbaloc_ref1.str ();

	CORBA::Object_ptr obj1 = orb->string_to_object (corbaloc1.c_str ());

	::FT::PullMonitorable_var pullMonitorable1 =::FT::PullMonitorable::
	  _narrow (obj1);

	TEST_INFO
	  ("Creates a second OS process to execute a pullMonitorable corba object server");

	pullOSId2 =
	  OsSupport::OS::create_process ("cdmw_test_platformfaultdetector",
					 pullArgsStr2);

	OsSupport::OS::sleep (timescale * 5000);

	std::string corbalocId2 ("PullMonitorable_");
	corbalocId2 += pullMonitorableName2;

	std::ostringstream corbaloc_ref2;
	corbaloc_ref2 << "corbaloc::localhost:" << defServicePort2;
	corbaloc_ref2 << "/" << corbalocId2;

	std::string corbaloc2 = corbaloc_ref2.str ();

	CORBA::Object_ptr obj2 = orb->string_to_object (corbaloc2.c_str ());

	::FT::PullMonitorable_var pullMonitorable2 =::FT::PullMonitorable::
	  _narrow (obj2);

	TEST_INFO
	  ("Creates a third OS process to execute a pullMonitorable corba object server");

	pullOSId3 =
	  OsSupport::OS::create_process ("cdmw_test_platformfaultdetector",
					 pullArgsStr3);

	OsSupport::OS::sleep (timescale * 5000);

	std::string corbalocId3 ("PullMonitorable_");
	corbalocId3 += pullMonitorableName3;

	std::ostringstream corbaloc_ref3;
	corbaloc_ref3 << "corbaloc::localhost:" << defServicePort3;
	corbaloc_ref3 << "/" << corbalocId3;

	std::string corbaloc3 = corbaloc_ref3.str ();

	CORBA::Object_ptr obj3 = orb->string_to_object (corbaloc3.c_str ());

	::FT::PullMonitorable_var pullMonitorable3 =::FT::PullMonitorable::
	  _narrow (obj3);

	TEST_INFO
	  ("Creates a fourth OS process to execute a pullMonitorable corba object server");

	pullOSId4 =
	  OsSupport::OS::create_process ("cdmw_test_platformfaultdetector",
					 pullArgsStr4);

	OsSupport::OS::sleep (timescale * 5000);

	std::string corbalocId4 ("PullMonitorable_");
	corbalocId4 += pullMonitorableName4;

	std::ostringstream corbaloc_ref4;
	corbaloc_ref4 << "corbaloc::localhost:" << defServicePort4;
	corbaloc_ref4 << "/" << corbalocId4;

	std::string corbaloc4 = corbaloc_ref4.str ();

	CORBA::Object_ptr obj4 = orb->string_to_object (corbaloc4.c_str ());

	::FT::PullMonitorable_var pullMonitorable4 =::FT::PullMonitorable::
	  _narrow (obj4);

	TEST_INFO
	  ("Creates a fifth OS process to execute a pullMonitorable corba object server");

	pullOSId5 =
	  OsSupport::OS::create_process ("cdmw_test_platformfaultdetector",
					 pullArgsStr5);

	OsSupport::OS::sleep (timescale * 5000);

	std::string corbalocId5 ("PullMonitorable_");
	corbalocId5 += pullMonitorableName5;

	std::ostringstream corbaloc_ref5;
	corbaloc_ref5 << "corbaloc::localhost:" << defServicePort5;
	corbaloc_ref5 << "/" << corbalocId5;

	std::string corbaloc5 = corbaloc_ref5.str ();

	CORBA::Object_ptr obj5 = orb->string_to_object (corbaloc5.c_str ());

	::FT::PullMonitorable_var pullMonitorable5 =::FT::PullMonitorable::
	  _narrow (obj5);

	LocalFaultDetector_impl *localFaultDetector_impl =
	  new LocalFaultDetector_impl (orb.in (), poa.in ());

	CdmwPlatformMngt::LocalFaultDetector_var localFaultDetector =
	  localFaultDetector_impl->_this ();

	::CdmwPlatformMngt::MonitoringId id1 = 0;
	::CdmwPlatformMngt::MonitoringId id2 = 0;
	::CdmwPlatformMngt::MonitoringId id3 = 0;
	::CdmwPlatformMngt::MonitoringId id4 = 0;
	::CdmwPlatformMngt::MonitoringId id5 = 0;

	std::string localHostName = OsSupport::OS::get_hostname ();

	::FT::Location location1;
	location1.length (3);
	location1[0].kind = CORBA::string_dup ("hostname");
	location1[0].id = CORBA::string_dup ("toto");
	location1[1].kind = CORBA::string_dup ("applicationname");
	location1[1].id = CORBA::string_dup ("app1");
	location1[2].kind = CORBA::string_dup ("processname");
	location1[2].id = CORBA::string_dup ("process1");

	::FT::Location location2;
	location2.length (3);
	location2[0].kind = CORBA::string_dup ("hostname");
	location2[0].id = CORBA::string_dup (localHostName.c_str ());
	location2[1].kind = CORBA::string_dup ("applicationname");
	location2[1].id = CORBA::string_dup ("app1");
	location2[2].kind = CORBA::string_dup ("processname");
	location2[2].id = CORBA::string_dup ("process2");

	::FT::Location location3;
	location3.length (3);
	location3[0].kind = CORBA::string_dup ("hostname");
	location3[0].id = CORBA::string_dup (localHostName.c_str ());
	location3[1].kind = CORBA::string_dup ("applicationname");
	location3[1].id = CORBA::string_dup ("app1");
	location3[2].kind = CORBA::string_dup ("processname");
	location3[2].id = CORBA::string_dup ("process3");

	::FT::Location location4;
	location4.length (3);
	location4[0].kind = CORBA::string_dup ("hostname");
	location4[0].id = CORBA::string_dup (localHostName.c_str ());
	location4[1].kind = CORBA::string_dup ("applicationname");
	location4[1].id = CORBA::string_dup ("app1");
	location4[2].kind = CORBA::string_dup ("processname");
	location4[2].id = CORBA::string_dup ("process4");

	::FT::Location location5;
	location5.length (3);
	location5[0].kind = CORBA::string_dup ("hostname");
	location5[0].id = CORBA::string_dup (localHostName.c_str ());
	location5[1].kind = CORBA::string_dup ("applicationname");
	location5[1].id = CORBA::string_dup ("app1");
	location5[2].kind = CORBA::string_dup ("processname");
	location5[2].id = CORBA::string_dup ("process5");

	FT::FaultMonitoringIntervalAndTimeoutValue timeout1;
	timeout1.monitoring_interval = 2000;
	timeout1.timeout = 6000;

	FT::FaultMonitoringIntervalAndTimeoutValue timeout2;
	timeout2.monitoring_interval = 4000;
	timeout2.timeout = 1000;

	FT::FaultMonitoringIntervalAndTimeoutValue timeout3;
	timeout3.monitoring_interval = 8000;
	timeout3.timeout = 1000;

	FT::FaultMonitoringIntervalAndTimeoutValue timeout4;
	timeout4.monitoring_interval = 8000;
	timeout4.timeout = 1000;

	FT::FaultMonitoringIntervalAndTimeoutValue timeout5;
	timeout5.monitoring_interval = 8000;
	timeout5.timeout = 500;

	TEST_INFO ("Stops a monitoring that does not exist");
	try
	{
	  localFaultDetector->stop_monitoring (1);
	  CPPUNIT_ASSERT (false);
	}
	catch (const CdmwPlatformMngt::InvalidMonitoringId &)
	{
	  CPPUNIT_ASSERT (true);
	}
	TEST_INFO ("Sleeping a while");
	OsSupport::OS::sleep (timescale * 5000);



	TEST_INFO ("Starts a monitoring with a bad location");
	try
	{
	  localFaultDetector->start_monitoring (pullMonitorable1.in (),
						location1, timeout1);
	  CPPUNIT_ASSERT (false);
	}
	catch (const CdmwPlatformMngt::BadLocation &)
	{
	  CPPUNIT_ASSERT (true);
	}

	TEST_INFO ("Sleeping a while");
	OsSupport::OS::sleep (timescale * 5000);

	TEST_INFO
	  ("Starts a monitoring with an incompatible monitoring timeout");
	try
	{
	  location1[0].id = CORBA::string_dup (localHostName.c_str ());
	  id1 =
	    localFaultDetector->start_monitoring (pullMonitorable1.in (),
						  location1, timeout1);
	  CPPUNIT_ASSERT (false);
	}
	catch (const CORBA::BAD_PARAM & ex)
	{
	  std::cout << ex.minor () << std::endl;
	  if (ex.minor () == (Cdmw::OrbSupport::CDMW_VMCID | 43))
	    CPPUNIT_ASSERT(true);
	  else
	    CPPUNIT_ASSERT(false);
	}

	TEST_INFO ("Sleeping a while");
	OsSupport::OS::sleep (timescale * 5000);

	TEST_INFO ("Starts a monitoring");
	try
	{
	  timeout1.timeout = 1000;
	  Cdmw::PlatformMngt::ProcessKillMngr::Instance ().
	    register_process (location1[1].id, location1[2].id, pullOSId1);
	  id1 =
	    localFaultDetector->start_monitoring (pullMonitorable1.in (),
						  location1, timeout1);
	  CPPUNIT_ASSERT(true);
	}
	catch (const CORBA::Exception & ex)
	{
	  std::cout << ex << std::endl;
	  CPPUNIT_ASSERT(false);
	}
	catch (...)
	{
	  CPPUNIT_ASSERT (false);
	}

	TEST_INFO ("Sleeping a while");
	OsSupport::OS::sleep (timescale * 20000);


	TEST_INFO ("Starts a monitoring that is already started");
	try
	{
	  id1 =
	    localFaultDetector->start_monitoring (pullMonitorable1.in (),
						  location1, timeout1);
	  CPPUNIT_ASSERT (false);
	}
	catch (...)
	{
	  CPPUNIT_ASSERT (true);
	}

	TEST_INFO ("Sleeping a while");
	OsSupport::OS::sleep (timescale * 5000);

	TEST_INFO ("Starts another monitoring");
	try
	{
	  Cdmw::PlatformMngt::ProcessKillMngr::Instance ().
	    register_process (location2[1].id, location2[2].id, pullOSId2);
	  id2 =
	    localFaultDetector->start_monitoring (pullMonitorable2.in (),
						  location2, timeout2);
	  CPPUNIT_ASSERT (true);
	}
	catch (...)
	{
	  CPPUNIT_ASSERT (false);
	}

	TEST_INFO ("Sleeping a while");
	OsSupport::OS::sleep (timescale * 20000);

	TEST_INFO ("Receives a pullMonitorable bad processing");
	try
	{
	  Cdmw::PlatformMngt::ProcessKillMngr::Instance ().
	    register_process (location3[1].id, location3[2].id, pullOSId3);
	  id3 =
	    localFaultDetector->start_monitoring (pullMonitorable3.in (),
						  location3, timeout3);
	  CPPUNIT_ASSERT (true);
	}
	catch (...)
	{
	  CPPUNIT_ASSERT (false);
	}

	TEST_INFO ("Sleeping a while");
	OsSupport::OS::sleep (timescale * 20000);

	TEST_INFO ("Detects a pullMonitorable failure");
	try
	{
	  Cdmw::PlatformMngt::ProcessKillMngr::Instance ().
	    register_process (location4[1].id, location4[2].id, pullOSId4);
	  id4 =
	    localFaultDetector->start_monitoring (pullMonitorable4.in (),
						  location4, timeout4);
	  CPPUNIT_ASSERT (true);
	}
	catch (...)
	{
	  CPPUNIT_ASSERT (false);
	}

	TEST_INFO ("Sleeping a while");
	OsSupport::OS::sleep (timescale * 20000);

	TEST_INFO ("Detects a pullMonitorable timeout expiring");
	try
	{
	  Cdmw::PlatformMngt::ProcessKillMngr::Instance ().
	    register_process (location5[1].id, location5[2].id, pullOSId5);
	  id5 =
	    localFaultDetector->start_monitoring (pullMonitorable5.in (),
						  location5, timeout5);
	  CPPUNIT_ASSERT (true);
	}
	catch (...)
	{
	  CPPUNIT_ASSERT (false);
	}

	TEST_INFO ("Sleeping a while");
	OsSupport::OS::sleep (timescale * 20000);

	TEST_INFO ("Stops the remaining monitorings");
	try
	{
	  localFaultDetector->stop_monitoring (id1);
	  localFaultDetector->stop_monitoring (id2);
	  CPPUNIT_ASSERT(true);
	}
	catch (const CdmwPlatformMngt::InvalidMonitoringId &)
	{
	  CPPUNIT_ASSERT(false);
	}

	TEST_INFO
	  ("Check if the monitoring for 3,4,5 are stopped and the process are killed()");
	try
	{
	  localFaultDetector->stop_monitoring (id3);
	  CPPUNIT_ASSERT(false);
	}
	catch (const CdmwPlatformMngt::InvalidMonitoringId &)
	{
	  CPPUNIT_ASSERT(true);
	}
	try
	{
	  localFaultDetector->stop_monitoring (id4);
	  CPPUNIT_ASSERT(false);
	}
	catch (const CdmwPlatformMngt::InvalidMonitoringId &)
	{
	  CPPUNIT_ASSERT(true);
	}
	try
	{
	  localFaultDetector->stop_monitoring (id5);
	  CPPUNIT_ASSERT(false);
	}
	catch (const CdmwPlatformMngt::InvalidMonitoringId &)
	{
	  CPPUNIT_ASSERT (true);
	}


	TEST_INFO ("Sleeping a while");

	OsSupport::OS::sleep (timescale * 5000);

	TEST_INFO ("Clean the test environment");

	try
	{
	  OsSupport::OS::kill_process (pullOSId1);
	  CPPUNIT_ASSERT(true);
	}
	catch (...)
	{
	  CPPUNIT_ASSERT(false);
	}

	try
	{
	  OsSupport::OS::kill_process (pullOSId2);
	  CPPUNIT_ASSERT(true);
	}
	catch (...)
	{
	  CPPUNIT_ASSERT(false);
	}

	try
	{
	  OsSupport::OS::kill_process (pullOSId3);
	  CPPUNIT_ASSERT(false);
	}
	catch (const Cdmw::BadParameterException &)
	{
	  CPPUNIT_ASSERT(true);
	}
	catch (...)
	{
	  CPPUNIT_ASSERT(false);
	}

	try
	{
	  //The process associated with the pullOSId4 is already killed (by an exception raised on purpose) 
	  OsSupport::OS::kill_process (pullOSId4);
	  CPPUNIT_ASSERT(false);
	}
	catch (const Cdmw::BadParameterException &)
	{
	  CPPUNIT_ASSERT(true);
	}
	catch (...)
	{
	  CPPUNIT_ASSERT(false);
	}

	try
	{
	  OsSupport::OS::kill_process (pullOSId5);
	  CPPUNIT_ASSERT(false);
	}
	catch (const Cdmw::BadParameterException &)
	{
	  CPPUNIT_ASSERT(true);
	}
	catch (...)
	{
	  CPPUNIT_ASSERT(false);
	}
      }
      catch (...)
      {
	std::cerr << "unexpected exception raised" << std::endl;

	try
	{
	  OsSupport::OS::kill_process (pullOSId1);
	}
	catch (...)
	{
	}

	try
	{
	  OsSupport::OS::kill_process (pullOSId2);
	}
	catch (...)
	{
	}

	try
	{
	  OsSupport::OS::kill_process (pullOSId3);
	}
	catch (...)
	{
	}

	try
	{
	  OsSupport::OS::kill_process (pullOSId4);
	}
	catch (...)
	{
	}

	try
	{
	  OsSupport::OS::kill_process (pullOSId5);
	}
	catch (...)
	{
	}

	CPPUNIT_ASSERT(false);
      }

      OsSupport::OS::sleep (timescale * 2000);

      TEST_INFO ("Stopping the orb");
      orbThread.shutdown ();
    }



  }				// End namespace PlatformMngt
}				// End namespace Cdmw
