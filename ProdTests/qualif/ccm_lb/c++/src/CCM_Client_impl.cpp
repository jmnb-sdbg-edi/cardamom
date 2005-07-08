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


#include <Foundation/common/Assert.hpp>
#include <Foundation/ossupport/OS.hpp>

#include <ccm_lb/CCM_Client_impl.hpp>


//
// IDL:thalesgroup.com/CcmLbTest/CCM_Client:1.0
//
CcmLbTest::CCM_Client_impl::CCM_Client_impl()
    : Testable("Test CCM with Load Balancing [client side]"),
      m_expectedReplica1Hostname(""),
      m_expectedReplica2Hostname(""),
      m_session_context(CcmLbTest::CCM_Client_Context::_nil()),
      m_testThread(this)
{
}


CcmLbTest::CCM_Client_impl::~CCM_Client_impl()
{
    std::cout << "   ... CCM_Client_impl destructor called" << std::endl;
}


//
// attribute expectedReplica1Hostname
//
char*
CcmLbTest::CCM_Client_impl::expectedReplica1Hostname()
    throw(CORBA::SystemException)
{
    return CORBA::string_dup(m_expectedReplica1Hostname);
}


void
CcmLbTest::CCM_Client_impl::expectedReplica1Hostname(const char* a)
    throw(CORBA::SystemException)
{
    m_expectedReplica1Hostname = CORBA::string_dup(a);
}


//
// attribute expectedReplica2Hostname
//
char*
CcmLbTest::CCM_Client_impl::expectedReplica2Hostname()
    throw(CORBA::SystemException)
{
    return CORBA::string_dup(m_expectedReplica2Hostname);
}


void
CcmLbTest::CCM_Client_impl::expectedReplica2Hostname(const char* a)
    throw(CORBA::SystemException)
{
    m_expectedReplica2Hostname = CORBA::string_dup(a);
}


//
// set_session_context() sets the session context of the component
// (called by the container)
//
void
CcmLbTest::CCM_Client_impl::set_session_context(
        Components::SessionContext_ptr ctx)
    throw(CORBA::SystemException)
{
    std::cout << "   ... CCM_Client_impl::set_session_context() called"
              << std::endl;

    m_session_context = ::CcmLbTest::CCM_Client_Context::_narrow(ctx);
}


//
// ccm_activate() activates the component (called by the container)
//
void
CcmLbTest::CCM_Client_impl::ccm_activate()
   throw(CORBA::SystemException)
{
    std::cout << "   ... CCM_Client_impl::ccm_activate() called" << std::endl;
    m_testThread.start();
}


//
// ccm_passivate() stops the component (called by the container)
//
void
CcmLbTest::CCM_Client_impl::ccm_passivate()
   throw(CORBA::SystemException)
{
    std::cout << "   ... CCM_Client_impl::ccm_passivate() called" << std::endl;
}


//
// ccm_remove() destroys the component (called by the container)
//
void
CcmLbTest::CCM_Client_impl::ccm_remove()
   throw(CORBA::SystemException)
{
    std::cout << "   ... CCM_Client_impl::ccm_remove() called" << std::endl;
}


//
// Testable do_tests() operation
//
void
CcmLbTest::CCM_Client_impl::do_tests()
{
    Cdmw::OsSupport::OS::sleep(2000);
    set_nbOfRequestedTestOK(1);

    // the first replica to answer is selected randomly
    // by the LB Group Manager.
    CORBA::String_var first_host_name =
        CORBA::string_dup(get_first_hostname());

    // Round-robin
    if (strcmp(first_host_name.in(), m_expectedReplica2Hostname.in()) == 0) {
        test_connections(m_expectedReplica1Hostname.in());
        test_connections(m_expectedReplica2Hostname.in());
        test_connections(m_expectedReplica1Hostname.in());
        test_connections(m_expectedReplica2Hostname.in());
        TEST_SUCCEED();
    } else if (strcmp(first_host_name.in(), m_expectedReplica1Hostname.in()) == 0) {
        test_connections(m_expectedReplica2Hostname.in());
        test_connections(m_expectedReplica1Hostname.in());
        test_connections(m_expectedReplica2Hostname.in());
        test_connections(m_expectedReplica1Hostname.in());
        TEST_SUCCEED();
    } else {
        TEST_INFO("error: unexpected hostname: " << first_host_name.in());
        TEST_FAILED();
    }
}


char*
CcmLbTest::CCM_Client_impl::get_first_hostname(void)
{
    add_nbOfRequestedTestOK(1);

    CORBA::String_var hostname = CORBA::string_dup("");

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
        hostname = hostInfo->get_hostname();

        if (strcmp(hostname.in(), "") == 0) {
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

    return CORBA::string_dup(hostname);
}


void
CcmLbTest::CCM_Client_impl::test_connections(const char* expectedHostname)
{
    add_nbOfRequestedTestOK(1);

    try {
        TEST_INFO("Testing connections to ServerWithFacet group (expected host: "
                  << expectedHostname << ")");

        // get the server HostInfo
        CcmLbTest::HostInfo_var hostInfo =
            m_session_context->get_connection_serverHostInfo();

        if (CORBA::is_nil(hostInfo.in())) {
            TEST_INFO("error: hostInfo is NIL!");
            TEST_FAILED();
            return;
        }

        // read the hostname from the server HostInfo
        CORBA::String_var hostname = hostInfo->get_hostname();

        if (strcmp(expectedHostname, hostname.in()) == 0) {
            TEST_SUCCEED();
        } else {
            TEST_INFO("Unexpected server hostname: " << hostname.in());
            TEST_FAILED();
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
}
