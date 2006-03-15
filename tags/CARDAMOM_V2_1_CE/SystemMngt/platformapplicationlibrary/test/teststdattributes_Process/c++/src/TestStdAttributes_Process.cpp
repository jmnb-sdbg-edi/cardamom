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

/* @author Fabrizio Lorenna <card03@selex-si.com> */

#include <iostream>
#include <limits>
#include <stdlib.h>
#include <string.h>

#include "teststdattributes_Process/TestStdAttributes_Process.hpp"

#include "Foundation/common/Options.hpp"
#include "Foundation/testutils/CORBATestManager.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "Foundation/common/Exception.hpp"
#include "Foundation/common/System.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

#include "idllib/CdmwPlatformMngtSupervisionObserver.skel.hpp"
#include "idllib/CdmwPlatformMngtSystem.stub.hpp"
#include "idllib/CdmwPlatformMngtApplication.stub.hpp"

using namespace std;

using namespace Cdmw;

using namespace Common;

using namespace PlatformMngt;

using namespace CdmwPlatformMngt;

CPPUNIT_TEST_SUITE_REGISTRATION (TestStdAttributes_Process);

namespace Cdmw
{
  namespace PlatformMngt
  {



    TestStdAttributes_Process::TestStdAttributes_Process () throw ()
    {

    }


    TestStdAttributes_Process::~TestStdAttributes_Process () throw ()
    {

    }

/*
@brief   do_tests method for new navigability methods of IBB2
@brief   developments  .
*/

    void TestStdAttributes_Process::do_tests ()
    {

// //  set_nbOfRequestedTestOK (10);


      char flag_get_proc_info = 0;
      char flag_set_proc_info = 0;
      char flag_set_test = 0;
      char flag_get_test = 0;
      char flag_restart_attempt = 0;
      char flag_get_process_mode = 0;
      char flag_get_memory_resident = 0;
      char flag_set_memory_resident = 0;
      char flag_get_core_dump_size = 0;
      char flag_get_run_as_root = 0;

      CdmwPlatformMngt::System_var mysystem;

      try
      {
	/*get the orb */
	m_orb =
	  CORBA::ORB::_duplicate (Cdmw::TestUtils::CORBATestManager::
				  instance ()->get_ORB ());

	/*
	   ===================================================
	   Get the HelloInterface object
	   ===================================================

	   import the object reference from the ior file */

	CORBA::Object_var objref;

	const char *corbaloc_str =
	  "corbaloc::localhost:21871/CdmwPlatformMngtSupervision";

	objref = m_orb->string_to_object (corbaloc_str);

	/*resolve the System_ptr */
	mysystem = CdmwPlatformMngt::System::_narrow (objref.in ());

	if (CORBA::is_nil (mysystem.in()))
	  {
	    TEST_INFO ("Error in narrowing CdmwPlatformMngt::System");
	    exit (-1);
	  }

	/* from System, use a method to retrieve an application */
	CdmwPlatformMngt::Application_var app =
	  mysystem->get_application ("HELLO_WORLD");

	/* from the application, use a method to retrieve a Process */

	/* retrieve the current hostname */
	char *hostname = getenv ("HOSTNAME");

	/* name of process to test */
	std::string proc_name = "HelloProcess1";


	CdmwPlatformMngt::Process_var proc =
	  app->get_process (proc_name.c_str (), hostname);


	TEST_INFO ("Test to get process name");
	CORBA::String_var name = proc->name ();

	if (!strcmp (proc_name.c_str (), name.in ()))
	  {
	    CPPUNIT_ASSERT (1);	/* test ok */
	  }
	else
	  {
	    CPPUNIT_ASSERT (0);	/* test not ok */
	  }

	TEST_INFO ("Test to get process status");

	CORBA::String_var status_info;

	CdmwPlatformMngt::ProcessStatus proc_status =
	  proc->get_status (status_info.out ());

	if (proc_status == CdmwPlatformMngt::PROCESS_INITIAL ||
	    proc_status == CdmwPlatformMngt::PROCESS_RUNNING)
	  {
	    CPPUNIT_ASSERT (1);
	  }
	else
	  {
	    CPPUNIT_ASSERT (0);
	  }

	TEST_INFO ("Test to get process info: exe, port, args, ...");

	CdmwPlatformMngt::ProcessInfo_var proc_info;

	try
	{
	  proc_info = proc->get_process_info ();
	  flag_get_proc_info = 1;
	}
	catch (...)
	{
	  flag_get_proc_info = 0;
	}

	if (flag_get_proc_info == 0)
	  {
	    CPPUNIT_ASSERT (0);	/* test not ok */
	  }
	else
	  {
	    CPPUNIT_ASSERT (1);	/* test ok */
	  }

	TEST_INFO ("Test to set process info: exe, port, args, ...");

	try
	{
	  proc->set_process_info (proc_info.in ());
	  flag_set_proc_info = 1;
	}
	catch (...)
	{
	  flag_set_proc_info = 0;
	}

	if (flag_set_proc_info == 0)
	  {
	    CPPUNIT_ASSERT (0);	/* test not ok */
	  }
	else
	  {
	    CPPUNIT_ASSERT (1);	/* test ok */
	  }


	/* test on set_exit_autorestart_info( .... ) */
	try
	{
	  CdmwPlatformMngt::ExitAutoRestartInfo exit_autorestart_info;
	  memset (&exit_autorestart_info, 0, sizeof (exit_autorestart_info));
	  TEST_INFO ("Test on Process::set_exit_autorestart_info: ");
	  proc->set_exit_autorestart_info (exit_autorestart_info);
	}
	catch (const CORBA::NO_IMPLEMENT & ex)
	{
	  flag_set_test = 1;
	  CPPUNIT_ASSERT (1);	/* test ok */
	}

	if (flag_set_test == 0)
	  CPPUNIT_ASSERT (0);	/* test not ok */

	/*test get_exit_autorestart_info( ... ) */

	try
	{
	  TEST_INFO ("Test on Process::get_exit_autorestart_info: ");
	  CdmwPlatformMngt::ExitAutoRestartInfo_var exit_info =
	    proc->get_exit_autorestart_info ();
	}
	catch (const CORBA::NO_IMPLEMENT & ex)
	{
	  flag_get_test = 1;
	  CPPUNIT_ASSERT (1);	/*test ok */
	}

	if (flag_get_test == 0)
	  CPPUNIT_ASSERT (0);	/*test not ok */



	try
	{
	  TEST_INFO ("Test on Process::restart_attempt: ");
	  CORBA::Long varlong = proc->restart_attempt ();
	  varlong = 0;
	}
	catch (const CORBA::NO_IMPLEMENT & ex)
	{
	  flag_restart_attempt = 1;
	  CPPUNIT_ASSERT (1);	/*test ok */
	}

	if (flag_restart_attempt == 0)
	  CPPUNIT_ASSERT (0);	/*test not ok */


	try
	{
	  TEST_INFO ("Test on Process::get_process_mode: ");
	  CdmwPlatformMngt::ProcessMode proc_mode = proc->get_process_mode ();
	  proc_mode = CdmwPlatformMngt::ProcessMode (BACKUP);
	}
	catch (const CORBA::NO_IMPLEMENT & ex)
	{
	  flag_get_process_mode = 1;
	  CPPUNIT_ASSERT (1);	/*test ok */
	}

	if (flag_restart_attempt == 0)
	  CPPUNIT_ASSERT (0);	/*test not ok */


	try
	{
	  TEST_INFO ("Test on Process::get_memory_resident: ");
	  bool memory_bool = proc->get_memory_resident ();
	  memory_bool = true;
	}
	catch (const CORBA::NO_IMPLEMENT & ex)
	{
	  flag_get_memory_resident = 1;
	  CPPUNIT_ASSERT (1);	/*test ok */
	}

	if (flag_get_memory_resident == 0)
	  CPPUNIT_ASSERT (0);	/*test not ok */


	try
	{
	  TEST_INFO ("Test on Process::set_memory_resident: ");
	  bool memory_bool = true;
	  proc->set_memory_resident (memory_bool);
	}
	catch (const CORBA::NO_IMPLEMENT & ex)
	{
	  flag_set_memory_resident = 1;
	  CPPUNIT_ASSERT (1);	/* test ok */
	}

	if (flag_set_memory_resident == 0)
	  CPPUNIT_ASSERT (0);	/* test not ok */

	try
	{
	  TEST_INFO ("Test on Process::get_core_dump_size: ");
	  CORBA::Long mylong;
	  mylong = proc->get_core_dump_size ();
	}
	catch (const CORBA::NO_IMPLEMENT & ex)
	{
	  flag_get_core_dump_size = 1;
	  CPPUNIT_ASSERT (1);	/* test ok */
	}

	if (flag_get_core_dump_size == 0)
	  CPPUNIT_ASSERT (0);	/* test not ok */




	TEST_INFO ("Test to get process_name by get_snapshot: ");
	CdmwPlatformMngt::ProcessSnapshot_var snap;
	snap = proc->get_snapshot ();

	string conf1 (snap->process_name);

	if (conf1 == proc_name)
	  {
	    CPPUNIT_ASSERT (1);	/* test ok */
	  }
	else
	  {
	    CPPUNIT_ASSERT (0);	/* test not ok */
	  }

	TEST_INFO ("Test to get status by get_snapshot: ");

	if ((snap->process_status == CdmwPlatformMngt::PROCESS_INITIAL) ||
	    (snap->process_status == CdmwPlatformMngt::PROCESS_RUNNING))
	  {
	    CPPUNIT_ASSERT (1);
	  }
	else
	  {
	    CPPUNIT_ASSERT (0);
	  }



	try
	{
	  TEST_INFO ("Test on Process::get_run_as_root: ");
	  bool mybool = proc->get_run_as_root ();
	  mybool = true;
	}
	catch (const CORBA::NO_IMPLEMENT & ex)
	{
	  flag_get_run_as_root = 1;
	  CPPUNIT_ASSERT (1);	/* test ok */
	}

	if (flag_get_run_as_root == 0)
	  CPPUNIT_ASSERT (0);	/* test not ok */

      }
      catch (const Cdmw::Exception & ex)
      {
	std::cerr << ex.what () << std::endl;
	exit (-1);
      }
      catch (const CORBA::Exception & ex)
      {
	std::cerr << ex << std::endl;
	exit (-1);
      }
      catch (...)
      {
	std::cerr << "unexpected exception" << std::endl;
	exit (-1);
      }

      /*
         ========================================================
         program stopping
         ========================================================
       */

      // Call ORB cleanup
      Cdmw::OrbSupport::OrbSupport::ORB_cleanup (m_orb.in ());

      // Destroy orb
      if (!CORBA::is_nil (m_orb.in ()))
	{
	  try
	  {
	    m_orb->destroy ();
	  }
	  catch (const CORBA::Exception & ex)
	  {
	    std::cerr << ex << std::endl;
	    exit (-1);
	  }
	}
    }



  }				/* End namespace PlatformMngt */
}				/* End namespace Cdmw */
