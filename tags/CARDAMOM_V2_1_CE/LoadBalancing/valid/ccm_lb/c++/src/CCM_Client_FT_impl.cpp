/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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


#include <Foundation/common/Assert.hpp>
#include <Foundation/ossupport/OS.hpp>

#include <ccm_lb/CCM_Client_FT_impl.hpp>


//
// IDL:thalesgroup.com/CcmLbTest/CCM_Client:1.0
//
CcmLbTest::CCM_Client_FT_impl::CCM_Client_FT_impl()
    : Testable("Test CCM with LB[client side]"),
      m_session_context(CcmLbTest::CCM_Client_Context::_nil()),
      m_testThread(this)
{
}


CcmLbTest::CCM_Client_FT_impl::~CCM_Client_FT_impl()
{
    std::cout << "   ... CCM_Client_FT_impl destructor called" << std::endl;
}



//
// set_session_context() sets the session context of the component
// (called by the container)
//
void
CcmLbTest::CCM_Client_FT_impl::set_session_context(
        Components::SessionContext_ptr ctx)
    throw(CORBA::SystemException)
{
    std::cout << "   ... CCM_Client_FT_impl::set_session_context() called"
              << std::endl;

    m_session_context = ::CcmLbTest::CCM_Client_Context::_narrow(ctx);
}


//
// ccm_activate() activates the component (called by the container)
//
void
CcmLbTest::CCM_Client_FT_impl::ccm_activate()
   throw(CORBA::SystemException)
{
    std::cout << "   ... CCM_Client_FT_impl::ccm_activate() called" << std::endl;
    m_testThread.start();
}


//
// ccm_passivate() stops the component (called by the container)
//
void
CcmLbTest::CCM_Client_FT_impl::ccm_passivate()
   throw(CORBA::SystemException)
{
    std::cout << "   ... CCM_Client_FT_impl::ccm_passivate() called" << std::endl;
}


//
// ccm_remove() destroys the component (called by the container)
//
void
CcmLbTest::CCM_Client_FT_impl::ccm_remove()
   throw(CORBA::SystemException)
{
    std::cout << "   ... CCM_Client_FT_impl::ccm_remove() called" << std::endl;
}


//
// Testable do_tests() operation
//
void
CcmLbTest::CCM_Client_FT_impl::do_tests()
{
    Cdmw::OsSupport::OS::sleep(2000);
    set_nbOfRequestedTestOK(70);
    CORBA::String_var temp = CORBA::string_dup("");
	 
    // perform 10 request with the RoundRobin policy
    for (int i = 0; i<10; i++)
    {
	CORBA::String_var process_name =
	    CORBA::string_dup(test_connections());
	TEST_INFO("REQUEST " << i << "...check the RoundRobin policy...") ;
	TEST_INFO("the process called is ==> " << process_name.in());
	if((strcmp(temp, process_name) == 0) && (!(i==5)))
	    TEST_FAILED();
	else
	{
	    temp = process_name;
	    TEST_SUCCEED();
	}
    }
    std::string dummy;
    TEST_INFO("Test the crash of FTGroupManager Primary...");
    TEST_INFO("kill process FTGroupManager1 ");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;
     // perform 10 request with the RoundRobin policy
    for (int i = 0; i<10; i++)
    {
	CORBA::String_var process_name =
	    CORBA::string_dup(test_connections());
	TEST_INFO("REQUEST " << i << "...check the RoundRobin policy...") ;
	TEST_INFO("the process called is ==> " << process_name.in());
	if((strcmp(temp, process_name) == 0) && (!(i==5)))
	    TEST_FAILED();
	else
	{
	    temp = process_name;
	    TEST_SUCCEED();
	}
    }
    TEST_INFO("Test the crash of a replica...");
    TEST_INFO("kill process ServerProcess11");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;
    
    temp = CORBA::string_dup("");
    // perform 10 request with the RoundRobin policy
    for (int i = 0; i<10; i++)
    {
	CORBA::String_var process_name =
	    CORBA::string_dup(test_connections());
	TEST_INFO("REQUEST " << i << "...check the RoundRobin policy...") ;
	TEST_INFO("the process called is ==> " << process_name.in());
	if((strcmp(temp, process_name) == 0) && (!(i==5)) || (strcmp(process_name, "ServerProcess11") == 0))
	    TEST_FAILED();
	else
	{
	    temp = process_name;
	    TEST_SUCCEED();
	}	   
    }

    TEST_INFO("Test the re-start of FTGroupManager killed...");
    TEST_INFO("re-start the FTGroupManager1");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;

    temp = CORBA::string_dup("");
    // perform 10 request with the RoundRobin policy
    for (int i = 0; i<10; i++)
    {
	CORBA::String_var process_name =
            CORBA::string_dup(test_connections());
	TEST_INFO("REQUEST " << i << "...check the RoundRobin policy...") ;
	TEST_INFO("the process called is ==> " << process_name.in());
        if((strcmp(temp, process_name) == 0) && (!(i==5)))
            TEST_FAILED();
        else
        {
            temp = process_name;
            TEST_SUCCEED();
        }
    }

    TEST_INFO("Test the crash of a replica...");
    TEST_INFO("kill process ServerProces21");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;
    
    temp = CORBA::string_dup("");
    // perform 10 request with the RoundRobin policy
    for (int i = 0; i<10; i++)
    {
	CORBA::String_var process_name =
	    CORBA::string_dup(test_connections());
	TEST_INFO("REQUEST " << i << "...check the RoundRobin policy...") ;
	TEST_INFO("the process called is ==> " << process_name.in());
	if((strcmp(temp, process_name) == 0) && (!(i==5)) || (strcmp(process_name, "ServerProces21") == 0))
	    TEST_FAILED();
	else
	{
	    temp = process_name;
	    TEST_SUCCEED();
	}	   
    }
    TEST_INFO("Test the LoadBalancing service with only one FTGroupManager replica alive...");
    TEST_INFO("kill the FTGroupManager2 and FTGroupManager3");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;

    temp = CORBA::string_dup("");
    // perform 10 request with the RoundRobin policy
     for (int i = 0; i<10; i++)
    {
	CORBA::String_var process_name =
            CORBA::string_dup(test_connections());
	TEST_INFO("REQUEST " << i << "...check the RoundRobin policy...") ;
	TEST_INFO("the process called is ==> " << process_name.in());
        if((strcmp(temp, process_name) == 0) && (!(i==5)))
            TEST_FAILED();
        else
        {
            temp = process_name;
            TEST_SUCCEED();
        }
    }

    TEST_INFO("Test the crash of a replica...");
    TEST_INFO("kill process ServerProcess31");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;
    
    temp = CORBA::string_dup("");
    // perform 10 request with the RoundRobin policy
    for (int i = 0; i<10; i++)
    {
	CORBA::String_var process_name =
	    CORBA::string_dup(test_connections());
	TEST_INFO("REQUEST " << i << "...check the RoundRobin policy...") ;
	TEST_INFO("the process called is ==> " << process_name.in());
	if((strcmp(temp, process_name) == 0) && (!(i==5)) || (strcmp(process_name, "ServerProcess31") == 0))
	    TEST_FAILED();
	else
	{
	    temp = process_name;
	    TEST_SUCCEED();
	}	   
    }
 

}

char*
CcmLbTest::CCM_Client_FT_impl::test_connections(void)
{
    add_nbOfRequestedTestOK(1);

    CORBA::String_var process_name = CORBA::string_dup("");

    try {
        TEST_INFO("Testing connections to ServerWithFacet group");

        // get the server HostInfo
        CcmLbTest::HostInfo_var hostInfo =
            m_session_context->get_connection_serverHostInfo();

        if (CORBA::is_nil(hostInfo.in())) {
            TEST_INFO("error: hostInfo is NIL!");
            TEST_FAILED();
            return 0;
        }

        // read the hostname from the server HostInfo
        process_name = hostInfo->get_process_info();

        if (strcmp(process_name.in(), "") == 0) {
            TEST_INFO("error: hostname is empty string");
            TEST_FAILED();
        } else {
            TEST_SUCCEED();
        }
    } catch (const CORBA::SystemException& e) {
        TEST_INFO("Unexpected CORBA::SystemException: " << e);
        TEST_FAILED();
    } catch (const CORBA::Exception& e) {
        TEST_INFO("Unexpected CORBA::Exception: " << e);
        TEST_FAILED();
    } catch (...) {
        TEST_INFO("Unknown exception!");
        TEST_FAILED();
    }

    return CORBA::string_dup(process_name);
}


