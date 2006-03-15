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

#include "teststdattributes_Host/TestStdAttributes_Host.hpp"

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

CPPUNIT_TEST_SUITE_REGISTRATION (TestStdAttributes_Host);

namespace Cdmw
{
  namespace PlatformMngt
  {



    TestStdAttributes_Host::TestStdAttributes_Host () throw ()
    {

    }


    TestStdAttributes_Host::~TestStdAttributes_Host () throw ()
    {

    }


    void TestStdAttributes_Host::do_tests ()
    {
// //  set_nbOfRequestedTestOK (7);


      char flag_get_host_name = 0;
      char flag_host_status_info = 0;
      char flag_get_mon_par = 0;
      char flag_set_mon_par = 0;
      char flag_get_sup_ind = 0;
      char flag_set_sup_ind = 0;

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

	/* retrieve the current hostname */
	char *hostname = getenv ("HOSTNAME");

	CdmwPlatformMngt::Host_var myhost = mysystem->get_host (hostname);

	TEST_INFO ("Test to get host name");
	CORBA::String_var name = myhost->name ();

	if (!strcmp (hostname, name.in ()))
	  {
	    CPPUNIT_ASSERT (1);	/* test ok */
	  }
	else
	  {
	    CPPUNIT_ASSERT (0);	/* test not ok */
	  }


	try
	{
	  TEST_INFO ("Test on Host::get_host_name: ");
	  CORBA::String_var name = myhost->get_host_name ();
	}
	catch (const CORBA::NO_IMPLEMENT & ex)
	{
	  flag_get_host_name = 1;
	  CPPUNIT_ASSERT (1);	/* test ok */
	}

	if (flag_get_host_name == 0)
	  CPPUNIT_ASSERT (0);	/* test not ok */

	TEST_INFO ("Test on Host::status: ");
	CdmwPlatformMngt::HostStatus host_status = myhost->status ();

	if (host_status == CdmwPlatformMngt::HOST_RESPONDING)
	  {
	    CPPUNIT_ASSERT (1);
	  }
	else
	  {
	    CPPUNIT_ASSERT (0);
	  }


	try
	{
	  TEST_INFO ("Test on Host::host_status_info: ");
	  CORBA::String_var status_info = myhost->host_status_info ();
	}
	catch (const CORBA::NO_IMPLEMENT & ex)
	{
	  flag_host_status_info = 1;
	  CPPUNIT_ASSERT (1);	/* test ok */
	}

	if (flag_host_status_info == 0)
	  CPPUNIT_ASSERT (0);	/* test not ok */


	TEST_INFO ("Test on Host::get_monitoring_parameters: ");
	CdmwPlatformMngt::CompleteMonitoringInfo monitor_info;

	try
	{
	  monitor_info = myhost->get_monitoring_parameters ();
	}
	catch (const CORBA::NO_IMPLEMENT & ex)
	{
	  flag_get_mon_par = 1;
	  CPPUNIT_ASSERT (1);	/* test ok */
	}

	if (flag_get_mon_par == 0)
	  CPPUNIT_ASSERT (0);	/* test not ok */


	TEST_INFO ("Test on Host::set_monitoring_parameters: ");
	try
	{
	  myhost->set_monitoring_parameters (monitor_info);
	}
	catch (const CORBA::NO_IMPLEMENT & ex)
	{
	  flag_set_mon_par = 1;
	  CPPUNIT_ASSERT (1);	/* test ok */
	}

	if (flag_set_mon_par == 0)
	  CPPUNIT_ASSERT (0);	/* test not ok */


	TEST_INFO ("Test on Host::get_global_supervision_indicator: ");
	try
	{
	  bool mybool;
	  mybool = myhost->get_global_supervision_indicator ();
	}
	catch (const CORBA::NO_IMPLEMENT & ex)
	{
	  flag_get_sup_ind = 1;
	  CPPUNIT_ASSERT (1);	/* test ok */
	}

	if (flag_get_sup_ind == 0)
	  CPPUNIT_ASSERT (0);	/* test not ok */

	TEST_INFO ("Test on Host::set_global_supervision_indicator: ");
	try
	{
	  bool mybool = true;
	  myhost->set_global_supervision_indicator (mybool);
	}
	catch (const CORBA::NO_IMPLEMENT & ex)
	{
	  flag_set_sup_ind = 1;
	  CPPUNIT_ASSERT (1);	/* test ok */
	}

	if (flag_set_sup_ind == 0)
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
         ===================================================
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
