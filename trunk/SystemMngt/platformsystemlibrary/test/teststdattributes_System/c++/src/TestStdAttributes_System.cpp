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

#include "teststdattributes_System/TestStdAttributes_System.hpp"

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

CPPUNIT_TEST_SUITE_REGISTRATION (TestStdAttributes_System);

namespace Cdmw
{
  namespace PlatformMngt
  {



    TestStdAttributes_System::TestStdAttributes_System () throw ()
    {

    }


    TestStdAttributes_System::~TestStdAttributes_System () throw ()
    {

    }

/*
@brief   do_tests method for new methods (for std attributes) of IBB2
@brief   developments  .
*/

    void TestStdAttributes_System::do_tests ()
    {
// //  set_nbOfRequestedTestOK (7);


      char flag_set_mission = 0;
      char flag_get_mission = 0;
      char flag_get_sys_name = 0;
      char flag_set_sys_name = 0;
      char flag_get_master_host = 0;

      CdmwPlatformMngt::System_var mysystem;

      try
      {

	m_orb =
	  CORBA::ORB::_duplicate (Cdmw::TestUtils::CORBATestManager::
				  instance ()->get_ORB ());
	/*
	   ===================================================
	   Get the HelloInterface object
	   ===================================================

	   import the object reference from the ior file
	 */

	CORBA::Object_var objref;

	/* define the corbaloc */
	const char *corbaloc_str =
	  "corbaloc::localhost:21871/CdmwPlatformMngtSupervision";

	objref = m_orb->string_to_object (corbaloc_str);

	/* resolve the System */
	mysystem = CdmwPlatformMngt::System::_narrow (objref.in ());

	if (CORBA::is_nil (mysystem.in ()))
	  {
	    TEST_INFO ("Error in narrowing CdmwPlatformMngt::System");
	    exit (-1);
	  }

	/* name of system to test */
	std::string sys_name = "MySystem";

	TEST_INFO ("Test to get system name");
	CORBA::String_var name = mysystem->name ();

	if (!strcmp (sys_name.c_str (), name.in ()))
	  {
	    CPPUNIT_ASSERT (1);	/* test ok */
	  }
	else
	  {
	    CPPUNIT_ASSERT (0);	/* test not ok */
	  }

	try
	{
	  TEST_INFO ("Test on System::set_system_mission: ");
	  mysystem->set_system_mission ("CIAO");
	}
	catch (const CORBA::NO_IMPLEMENT & ex)
	{
	  flag_set_mission = 1;
	  CPPUNIT_ASSERT (1);
	}

	if (flag_set_mission == 0)
	  CPPUNIT_ASSERT (0);

	TEST_INFO ("Test on System::get_system_mission: ");
	try
	{
	  CORBA::String_var mission = mysystem->get_system_mission ();
	}
	catch (const CORBA::NO_IMPLEMENT & ex)
	{
	  flag_get_mission = 1;
	  CPPUNIT_ASSERT (1);
	}

	if (flag_get_mission == 0)
	  CPPUNIT_ASSERT (0);


	TEST_INFO ("Test on System::get_system_name: ");
	try
	{
	  CORBA::String_var system_name = mysystem->get_system_name ();
	}
	catch (const CORBA::NO_IMPLEMENT & ex)
	{
	  flag_get_sys_name = 1;
	  CPPUNIT_ASSERT (1);
	}

	if (flag_get_sys_name == 0)
	  CPPUNIT_ASSERT (0);

	TEST_INFO ("Test on System::set_system_name: ");
	try
	{
	  mysystem->set_system_name ("CIAO");
	}
	catch (const CORBA::NO_IMPLEMENT & ex)
	{
	  flag_set_sys_name = 1;
	  CPPUNIT_ASSERT (1);
	}

	if (flag_set_sys_name == 0)
	  CPPUNIT_ASSERT (0);


	TEST_INFO ("Test on System::get_master_host: ");
	try
	{
	  CORBA::String_var master_host_name = mysystem->get_master_host ();
	}
	catch (const CORBA::NO_IMPLEMENT & ex)
	{
	  flag_get_master_host = 1;
	  CPPUNIT_ASSERT (1);
	}

	if (flag_get_master_host == 0)
	  CPPUNIT_ASSERT (0);


	TEST_INFO ("Test on System::get_mode: ");
	CdmwPlatformMngt::FunctioningMode sys_mode;
	CORBA::String_var mode_info;

	sys_mode = mysystem->get_mode (mode_info.out ());

	if (sys_mode == CdmwPlatformMngt::NORMAL_MODE)
	  {
	    CPPUNIT_ASSERT (1);
	  }
	else
	  {
	    CPPUNIT_ASSERT (0);
	  }

	TEST_INFO ("Test on System::get_status: ");
	CdmwPlatformMngt::SystemStatus sys_status;
	CORBA::String_var status_info;
	sys_status = mysystem->get_status (status_info.out ());


	if ((sys_status == CdmwPlatformMngt::SYSTEM_INITIAL) ||
	    (sys_status == CdmwPlatformMngt::SYSTEM_STARTING) ||
	    (sys_status == CdmwPlatformMngt::SYSTEM_STARTED))
	  {
	    CPPUNIT_ASSERT (1);
	  }
	else
	  {
	    CPPUNIT_ASSERT (0);
	  }
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
