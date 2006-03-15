/* =========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI.
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003.
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General
 * Public License along with CARDAMOM; see the file COPYING. If not, write to
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * =========================================================================== */



#include <ccm_ft_request_logging/CCM_Client_impl.hpp>

#include <Foundation/common/Assert.hpp>
#include <Foundation/ossupport/OS.hpp>


//
// IDL:thalesgroup.com/CcmFtTest/CCM_Client:1.0
//
CcmFtTest::CCM_Client_impl::CCM_Client_impl()
	: Testable("Test CCM with Fault Tolerance request logging - Client side"),
      m_expectedPrimaryHostname(""),
      m_expectedBackup1Hostname(""),
      m_session_context(CcmFtTest::CCM_Client_Context::_nil()),
      m_testThread(this)
{
}

CcmFtTest::CCM_Client_impl::~CCM_Client_impl()
{
    std::cout << "   ... CCM_Client_impl::~CCM_Client_impl called!" << std::endl;
}




//
// attribute expectedPrimaryHostname
//
char*
CcmFtTest::CCM_Client_impl::expectedPrimaryHostname()
    throw(CORBA::SystemException)
{
    return CORBA::string_dup(m_expectedPrimaryHostname);
}

void
CcmFtTest::CCM_Client_impl::expectedPrimaryHostname(const char* a)
    throw(CORBA::SystemException)
{
    m_expectedPrimaryHostname =  CORBA::string_dup(a);
}


//
// attribute expectedBackup1Hostname
//
char*
CcmFtTest::CCM_Client_impl::expectedBackup1Hostname()
    throw(CORBA::SystemException)
{
    return CORBA::string_dup(m_expectedBackup1Hostname);
}

void
CcmFtTest::CCM_Client_impl::expectedBackup1Hostname(const char* a)
    throw(CORBA::SystemException)
{
    m_expectedBackup1Hostname =  CORBA::string_dup(a);
}


//
// attribute expectedBackup2Hostname
//
char*
CcmFtTest::CCM_Client_impl::expectedBackup2Hostname()
    throw(CORBA::SystemException)
{
    return CORBA::string_dup(m_expectedBackup2Hostname);
}

void
CcmFtTest::CCM_Client_impl::expectedBackup2Hostname(const char* a)
    throw(CORBA::SystemException)
{
    m_expectedBackup2Hostname =  CORBA::string_dup(a);
}


    
//
// set_session_context function : it fixes the session context of the component (call by the container)
//
void
CcmFtTest::CCM_Client_impl::set_session_context(Components::SessionContext_ptr ctx)
    throw(CORBA::SystemException)
{
    std::cout << "   ... CCM_Client_impl::Set_session_context called!" << std::endl;
   
    m_session_context = ::CcmFtTest::CCM_Client_Context::_narrow(ctx); 
}


//
// ccm_activate function : it actives the component (call by the container)
//
void
CcmFtTest::CCM_Client_impl::ccm_activate()
   throw(CORBA::SystemException)
{
   std::cout << "   ... CCM_Client_impl::Ccm_activate called!" << std::endl;
   m_testThread.start();
}


//
// ccm_passivate function : it makes stop the component (call by the container)
//
void
CcmFtTest::CCM_Client_impl::ccm_passivate()
   throw(CORBA::SystemException)
{
   std::cout << "   ... CCM_Client_impl::Ccm_passivate called!" << std::endl;
} 



//
// ccm_remove function : it destroys the component (call by the container)
//
void
CcmFtTest::CCM_Client_impl::ccm_remove()
   throw(CORBA::SystemException)
{
   std::cout << "   ... CCM_Client_impl::Ccm_remove called!" << std::endl;
}



//
// Testable do_tests() operation
//
void
CcmFtTest::CCM_Client_impl::do_tests()
{
    Cdmw::OsSupport::OS::sleep(2000);
    
    set_nbOfRequestedTestOK(0);
    
    // test connections to group (primary hostname is expected)
    test_connections(m_expectedPrimaryHostname.in());
    
    // test connections to group (backup1 hostname is expected)
    test_connections(m_expectedBackup1Hostname.in());

}


void
CcmFtTest::CCM_Client_impl::test_connections(const char* expectedHostname)
{
    add_nbOfRequestedTestOK(1);
    
    try {
        TEST_INFO("Test connections to ServerWithFacet group (expected primary host: " 
                  << expectedHostname << ")");    
    
        CcmFtTest::HostInfo_var hostInfo = m_session_context->get_connection_serverHostInfo();

        if (CORBA::is_nil(hostInfo.in())) {
            TEST_INFO("hostInfo is NIL !!!! ");
            TEST_FAILED();
            return;
        }
        
        CORBA::String_var hostname = hostInfo->get_hostname();
        
        if (strcmp(expectedHostname, hostname.in()) == 0) {
            TEST_SUCCEED();
        } else {
            TEST_INFO("Server's hostname is not the one expected: " << hostname.in());
            TEST_INFO("expected hostname: " << hostname.in());
            TEST_FAILED();            
        }

    } catch (const CORBA::SystemException& e) {
        TEST_INFO("Unexpected CORBA::SystemException: " << e);
        TEST_FAILED();
    } catch (const CORBA::Exception& e) {
        TEST_INFO("Unexpected CORBA::Exception: " << e);
        TEST_FAILED();
    } catch (...) {
        TEST_INFO("Unknown exception !! ");
        TEST_FAILED();
    }
}




