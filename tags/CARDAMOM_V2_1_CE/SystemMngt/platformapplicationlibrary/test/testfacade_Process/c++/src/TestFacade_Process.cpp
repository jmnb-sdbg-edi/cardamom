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

#include "testfacade_Process/TestFacade_Process.hpp"

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

CPPUNIT_TEST_SUITE_REGISTRATION(TestFacade_Process);

namespace Cdmw
{
namespace PlatformMngt
{



TestFacade_Process::TestFacade_Process() 

{

}


TestFacade_Process::~TestFacade_Process()
{

}

/*
@brief   do_tests method for new facade methods to get/set attributes
@brief   developments  .
*/

void
TestFacade_Process::do_tests()                
{
// //  set_nbOfRequestedTestOK (5);


 CdmwPlatformMngt::System_ptr mysystem;

 try
    {

	/*get the orb*/
	m_orb = CORBA::ORB::_duplicate( Cdmw::TestUtils::CORBATestManager::instance()->get_ORB() );
	/*
         ===================================================
         Get the HelloInterface object
         ===================================================

	*/
        CORBA::Object_var objref;

        /*define the corbaloc*/
        const char *corbaloc_str = "corbaloc::localhost:21871/CdmwPlatformMngtSupervision";

        objref = m_orb->string_to_object(corbaloc_str);

        /*resolve the System_ptr*/
        mysystem = CdmwPlatformMngt::System::_narrow(objref.in() );

        if (CORBA::is_nil(mysystem) )
            {
                TEST_INFO("Error in narrowing CdmwPlatformMngt::System_ptr");
                exit(-1);
            }

        /*from System, use a method to retrieve an application*/
        CdmwPlatformMngt::Application_ptr app= mysystem->get_application("HELLO_WORLD");       

        /*from the application, use a method to retrieve a Process*/

        /*retrieve the current hostname*/
        char * hostname  = getenv("HOSTNAME");


        CdmwPlatformMngt::Process_ptr myprocess = app->get_process("HelloProcess1",hostname) ;



	try
	{
        TEST_INFO("Test in get_attribute of 'status' :");
	CORBA::Any_var outany; 
        myprocess->get_attribute("status", outany, 0);
        /*extract outany value;*/
	CdmwPlatformMngt::ProcessStatus stat = CdmwPlatformMngt::PROCESS_FAILED_NO_RESPONSE;
        (*outany) >>= stat;

        if ( stat == CdmwPlatformMngt::PROCESS_INITIAL)
	  {
	  CPPUNIT_ASSERT(1); /*test ok*/
	  }
        else
	  {
	  CPPUNIT_ASSERT(0); /*test not ok*/
	  }

	}
	catch(...)
	  {
          cout <<"Error, exception in get_attribute with 'status' "<<endl;
	  CPPUNIT_ASSERT(0);
	  }


	try
	{
        TEST_INFO("Test in get_attribute of 'status_info' :");
	CORBA::Any_var outany; 
        myprocess->get_attribute("status_info", outany, 0);
        /*extract outany value;*/
	char * status_info2 = "HELLO"; 
        (*outany) >>= CORBA::Any::to_string(status_info2, 0);


	string tmp1(status_info2);
	if (tmp1 == "")
	  {
          CPPUNIT_ASSERT(1); /*test ok*/
	  }
        else
	  {
          CPPUNIT_ASSERT(0); /*test not ok*/
	  }


	}
	catch(...)
	  {
          cout <<"Error, exception in get_attribute with 'status_info' "<<endl;
	  CPPUNIT_ASSERT(0);
	  }


    try
        {
        CORBA::Any_var outany;

        TEST_INFO("Test in get_attribute to throw AttributeNotFound :");

        myprocess->get_attribute("dummy", outany,0);

        CPPUNIT_ASSERT(0);

        }
        catch(AttributeNotFound)
          {
          CPPUNIT_ASSERT(1); /*test ok*/
          }

     try
        {
        CORBA::Any inany;
        CORBA::Short proshort1 = 99 ;

        TEST_INFO("Test in set_attribute of user defined attribute :");

        inany <<= proshort1;

        myprocess->set_attribute("CIAO", inany ,1);

        CORBA::Any_var outany;
        myprocess->get_attribute("CIAO", outany,1);
        CORBA::Short proshort2 = 0 ;

        (*outany) >>= proshort2;
        if (proshort2 == 99 )
          {
          CPPUNIT_ASSERT(1);
          }
        else
          {
          CPPUNIT_ASSERT(0);
          }
        }
        catch(...)
          {
          CPPUNIT_ASSERT(0); /*test not ok*/
          }


      try
        {
        CORBA::Any_var outany;
        TEST_INFO("Test in get_attribute of user defined attribute :");

        myprocess->get_attribute("CIAO", outany,1);
        CORBA::Short proshort = 0 ;

        (*outany) >>= proshort;
        if (proshort == 99 )
          {
          CPPUNIT_ASSERT(1);
          }
        else
          {
          CPPUNIT_ASSERT(0);
          }
        }
        catch(...)
          {
          CPPUNIT_ASSERT(0); /*test not ok*/
          }



  }
  catch (const Cdmw::Exception & ex)
    {
        std::cerr << ex.what() << std::endl;
        exit(-1);
    }
    catch(const CORBA::Exception& ex)
    {          
        std::cerr << ex << std::endl;
        exit(-1);
    }
    catch(...)
    {
        std::cerr << "unexpected exception" << std::endl;
        exit(-1);
    }


    /*
     ========================================================
     program stopping
     ========================================================

     ===================================================
     Call ORB cleanup
     ===================================================
    */
    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(m_orb.in());

    /*
     =====================================================
     Destroy orb
     =====================================================
    */

    if (!CORBA::is_nil(m_orb.in()))
    {
        try
        {     
            m_orb->destroy();
        }
        catch(const CORBA::Exception& ex)
        {
            std::cerr << ex << std::endl;
            exit(-1);
        }
    }

    
    
}    



} /* End namespace PlatformMngt*/
} /* End namespace Cdmw*/


