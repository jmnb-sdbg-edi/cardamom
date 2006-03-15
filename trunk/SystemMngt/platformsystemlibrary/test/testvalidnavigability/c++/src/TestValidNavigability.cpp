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

/*
@author Fabrizio Lorenna <card03@selex-si.com>
*/

#include <iostream>
#include <limits>

#include "testvalidnavigability/TestValidNavigability.hpp"

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

CPPUNIT_TEST_SUITE_REGISTRATION (TestValidNavigability);

namespace Cdmw
{
  namespace PlatformMngt
  {



    TestValidNavigability::TestValidNavigability () throw ()
    {

    }


    TestValidNavigability::~TestValidNavigability () throw ()
    {

    }

/*
@brief   do_tests method for new navigability methods of IBB2
@brief   developments  .
*/

    void TestValidNavigability::do_tests ()
    {
// //  set_nbOfRequestedTestOK (15);



      try
      {

	m_orb =
	  CORBA::ORB::_duplicate (Cdmw::TestUtils::CORBATestManager::
				  instance ()->get_ORB ());
	/*
	   import the object reference from the ior file
	 */

	CORBA::Object_var objref;

	/*define the corbaloc */
	const char *corbaloc_str =
	  "corbaloc::localhost:21871/CdmwPlatformMngtSupervision";

	objref = m_orb->string_to_object (corbaloc_str);

	/* resolve the System */
	CdmwPlatformMngt::System_var mysystem =
	  CdmwPlatformMngt::System::_narrow (objref.in ());

	if (CORBA::is_nil (mysystem.in ()))
	  {
	    TEST_INFO ("Error in narrowing CdmwPlatformMngt::System");
	    exit (-1);
	  }

	CORBA::ULong i;		/*generic counter */

	/* test: from system to application */

	CdmwPlatformMngt::Application_var myapplication1 =
	  CdmwPlatformMngt::Application::_nil ();
	CdmwPlatformMngt::Application_var myapplication2 =
	  CdmwPlatformMngt::Application::_nil ();
	CdmwPlatformMngt::Application_var myapplication3 =
	  CdmwPlatformMngt::Application::_nil ();

	CdmwPlatformMngt::System_var mysystem1;
	CdmwPlatformMngt::System_var mysystem2;
	CdmwPlatformMngt::System_var mysystem3;

	CdmwPlatformMngt::Processes_var myprocesses1;
	CdmwPlatformMngt::Processes_var myprocesses2;

	try
	{
	  TEST_INFO ("Test retrieving application HELLO_WORLD1 from system");
	  myapplication1 = mysystem->get_application ("HELLO_WORLD1");

	  if (CORBA::is_nil (myapplication1.in ()))
	    {
	      TEST_INFO ("Error: application1 is nil");
	      exit (-1);
	    }

	  /*cout <<"ok in get application"<<endl; */
	  CPPUNIT_ASSERT (1);
	}
	catch (CdmwPlatformMngt::ApplicationNotFound & ex)
	{
	  /* cout <<"application not found: "<<ex<<endl; */
	  CPPUNIT_ASSERT (0);
	}
	catch (...)
	{
	  CPPUNIT_ASSERT (0);
	}


	try
	{
	  TEST_INFO ("Test retrieving application HELLO_WORLD2 from system");
	  myapplication2 = mysystem->get_application ("HELLO_WORLD2");

	  if (CORBA::is_nil (myapplication2.in ()))
	    {
	      TEST_INFO ("Error: application2 is nil");
	      exit (-1);
	    }
	  /*cout <<"ok in get application"<<endl; */
	  CPPUNIT_ASSERT (1);
	}
	catch (CdmwPlatformMngt::ApplicationNotFound & ex)
	{
	  /* cout <<"application not found: "<<ex<<endl; */
	  CPPUNIT_ASSERT (0);
	}
	catch (...)
	{
	  CPPUNIT_ASSERT (0);
	}


	try
	{
	  TEST_INFO ("Test retrieving application HELLO_WORLD3 from system");
	  myapplication3 = mysystem->get_application ("HELLO_WORLD3");

	  if (CORBA::is_nil (myapplication3.in ()))
	    {
	      TEST_INFO ("Error: application3 ptr is nil");
	      exit (-1);
	    }

	  /*cout <<"ok in get application"<<endl; */
	  CPPUNIT_ASSERT (1);
	}
	catch (CdmwPlatformMngt::ApplicationNotFound & ex)
	{
	  /* cout <<"application not found: "<<ex<<endl; */
	  CPPUNIT_ASSERT (0);
	}
	catch (...)
	{
	  CPPUNIT_ASSERT (0);
	}

	/* tests: from application to system */

	try
	{

	  TEST_INFO ("Test retrieving system from HELLO_WORLD1 application");

	  mysystem1 =
	    CdmwPlatformMngt::System::_narrow (myapplication1->get_parent ());

	  if (mysystem->_is_equivalent (mysystem1.in ()))
	    {
	      CPPUNIT_ASSERT (1);
	    }
	  else
	    {
	      CPPUNIT_ASSERT (0);
	    }
	}
	catch (...)
	{
	  CPPUNIT_ASSERT (0);
	}


	try
	{
	  TEST_INFO ("Test retrieving system from HELLO_WORLD2 application");

	  mysystem2 =
	    CdmwPlatformMngt::System::_narrow (myapplication2->get_parent ());

	  if (mysystem->_is_equivalent (mysystem2.in ()))
	    {
	      CPPUNIT_ASSERT (1);
	    }
	  else
	    {
	      CPPUNIT_ASSERT (0);
	    }
	}
	catch (...)
	{
	  CPPUNIT_ASSERT (0);
	}


	try
	{

	  TEST_INFO ("Test retrieving system from HELLO_WORLD3 application");

	  mysystem3 =
	    CdmwPlatformMngt::System::_narrow (myapplication3->get_parent ());

	  if (mysystem->_is_equivalent (mysystem3.in ()))
	    {
	      CPPUNIT_ASSERT (1);
	    }
	  else
	    {
	      CPPUNIT_ASSERT (0);
	    }
	}
	catch (...)
	{
	  CPPUNIT_ASSERT (0);
	}

	/* test: from application to processes */
	CdmwPlatformMngt::Processes_var my_processes_from_app;
	CdmwPlatformMngt::Process_ptr tmp_process;
	CdmwPlatformMngt::ProcessesIterator_var process_iter;
	CORBA::ULong size_seq = 0;

	try
	{
	  TEST_INFO
	    ("Test retrieving processes from application HELLO_WORLD1");

	  my_processes_from_app =
	    myapplication1->get_processes (5, process_iter.out ());

	  size_seq = my_processes_from_app->length ();

	  if (size_seq == 2)
	    {
	      CPPUNIT_ASSERT (1);
	    }
	  else
	    {
	      CPPUNIT_ASSERT (0);
	    }
	}
	catch (...)
	{
	  CPPUNIT_ASSERT (0);
	}

	/* tests: from process to application */

	TEST_INFO
	  ("Test retrieving HELLO_WORLD1 application from its processes");

	for (i = 0; i < size_seq; i++)
	  {
	    tmp_process = my_processes_from_app[i];

	    if (CORBA::is_nil (tmp_process))
	      {
		std::cout << "Error tmp_process is nil" << std::endl;
		exit (-1);
	      }

	    CdmwPlatformMngt::Application_var myapplication1_new;

	    try
	    {
	      myapplication1_new =
		CdmwPlatformMngt::Application::_narrow (tmp_process->
							get_parent ());

	      if (myapplication1->_is_equivalent (myapplication1_new.in ()))
		{
		  CPPUNIT_ASSERT (1);
		}
	      else
		{
		  CPPUNIT_ASSERT (0);
		}
	    }
	    catch (...)
	    {
	      CPPUNIT_ASSERT (0);
	    }
	  }


	/*test on number of host and to read the host-names */
	/* get all host names */
	CORBA::ULong how_many = 2;
	CdmwPlatformMngt::ElementNames_var host_names;
	CdmwPlatformMngt::ElementNamesIterator_var rest;

	try
	{
	  TEST_INFO ("Test retrieving host names from system");
	  mysystem->get_all_host_names (how_many, host_names, rest);
	  CPPUNIT_ASSERT (1);
	}
	catch (...)
	{
	  /*cout <<"error in get_all_host_names"<<endl; */
	  CPPUNIT_ASSERT (0);
	}


	CdmwPlatformMngt::Host_var host[2];


	for (i = 0; i < 2; i++)
	  {
	    if (i == 0)
	      {
		TEST_INFO
		  ("Test retrieving host1 ptr from system by get_host");
	      }
	    else
	      {
		TEST_INFO
		  ("Test retrieving host2 ptr from system by get_host");
	      }

	    try
	    {
	      host[i] = mysystem->get_host ((host_names[i]).in ());

	      if (CORBA::is_nil (host[i].in ()))
		{
		  TEST_INFO ("Error: an host is nil");
		  exit (-1);
		}

	      CPPUNIT_ASSERT (1);
	    }
	    catch (...)
	    {
	      CPPUNIT_ASSERT (0);
	    }
	  }


	for (i = 0; i < 2; i++)
	  {
	    if (i == 0)
	      {
		TEST_INFO ("Test retrieving system from host1");
	      }
	    else
	      {
		TEST_INFO ("Test retrieving system from host2");
	      }


	    try
	    {
	      mysystem1 =
		CdmwPlatformMngt::System::_narrow (host[i]->get_parent ());

	      if (mysystem->_is_equivalent (mysystem1.in ()))
		{
		  CPPUNIT_ASSERT (1);
		}
	      else
		{
		  CPPUNIT_ASSERT (0);
		}
	    }
	    catch (...)
	    {
	      CPPUNIT_ASSERT (0);
	    }
	  }

	/*test: navigate from Host to Processes */

	TEST_INFO ("Test navigate from host1 to Processes");

	try
	{
	  CdmwPlatformMngt::ProcessesIterator_var proc_iter;
	  myprocesses1 = host[0]->get_processes (1000, proc_iter);
	  CPPUNIT_ASSERT (1);
	}
	catch (...)
	{
	  CPPUNIT_ASSERT (0);	/*test error */
	}



	TEST_INFO ("Test navigate from host2 to Processes:");

	try
	{
	  CdmwPlatformMngt::ProcessesIterator_var proc_iter;
	  myprocesses2 = host[1]->get_processes (1000, proc_iter);
	  CPPUNIT_ASSERT (1);
	}
	catch (...)
	{
	  CPPUNIT_ASSERT (0);	/*test error */
	}

	CORBA::ULong size_seq1, size_seq2;	/*to store the sequence size */

	CdmwPlatformMngt::Host_var host_new;


	char flag_fail = 0;

	TEST_INFO ("Test navigate from Processes (of host1) to host1");

	size_seq1 = myprocesses1->length ();

	try
	{
	  for (i = 0; i < size_seq1; i++)
	    {
	      host_new = (myprocesses1[i])->get_host ();
	      if (host[0]->_is_equivalent (host_new.in ()))
		{
		  /* ok */
		}
	      else
		{
		  flag_fail = 1;
		}
	    }
	}
	catch (...)
	{
	  flag_fail = 1;	/*test error */
	}


	if (flag_fail == 0)
	  {
	    CPPUNIT_ASSERT (1);
	  }
	else
	  {
	    CPPUNIT_ASSERT (0);	/*test error */
	  }


	flag_fail = 0;
	TEST_INFO ("Test navigate from Processes (of host2) to host2");

	size_seq2 = myprocesses2->length ();

	try
	{
	  for (i = 0; i < size_seq2; i++)
	    {
	      host_new = (myprocesses2[i])->get_host ();
	      if (host[1]->_is_equivalent (host_new.in ()))
		{
		  /*ok */
		}
	      else
		{
		  flag_fail = 1;
		}
	    }
	}
	catch (...)
	{
	  flag_fail = 1;	/*test error */
	}

	if (flag_fail == 0)
	  {
	    CPPUNIT_ASSERT (1);
	  }
	else
	  {
	    CPPUNIT_ASSERT (0);	/*test error */
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
