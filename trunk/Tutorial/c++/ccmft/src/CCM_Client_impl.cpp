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



#include <CCM_Client_impl.hpp>

#include <Command_impl.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/ossupport/OS.hpp>


//
// IDL:thalesgroup.com/CcmFtTest/CCM_Client:1.0
//
CcmFtTest::CCM_Client_impl::CCM_Client_impl()
	: Testable("Test CCM with Fault Tolerance - Client side"),
      m_expectedPrimaryHostname(""),
      m_expectedBackup1Hostname(""),
      m_hostnameCommandResult(""),
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
// facet clientHostInfo
//
CcmFtTest::CCM_HostInfo_ptr 
CcmFtTest::CCM_Client_impl::get_clientHostInfo ()
    throw (CORBA::SystemException)
{
    return CcmFtTest::CCM_HostInfo::_duplicate(this);
}

char*
CcmFtTest::CCM_Client_impl::get_hostname()
    throw (CORBA::SystemException)
{
    std::string hostname = Cdmw::OsSupport::OS::get_hostname();
    return CORBA::string_dup(hostname.c_str());
}    


    
//
// consumer hostnameResults
//
void 
CcmFtTest::CCM_Client_impl::push_hostnameResults (CcmFtTest::HostnameResult * event)
    throw (CORBA::SystemException)
{
    std::cout << "   ... CCM_Client_impl::CCM_Client_impl::push_hostnameResults called with event " << event->hostname() << std::endl;
    m_hostnameCommandResult = event->hostname();
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
// get result of assynchronous getHostname command
//
char* 
CcmFtTest::CCM_Client_impl::getHostnameCommandResult()
{
    char* result = CORBA::string_dup(m_hostnameCommandResult.in());
    m_hostnameCommandResult = "";
    return result;
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
    
    try {
        TEST_INFO("Send kill command to group (only primary will stop)");    
    
        CcmFtTest::Command_var command = new CcmFtTest::Command_impl();
        command->commandName(CcmFtTest::COMMAND_KILL);
    
        TEST_INFO("    emits command " <<  command->commandName());
        m_session_context->push_commandEmits(command.in());
        
        TEST_INFO("    wait new primary election");
        Cdmw::OsSupport::OS::sleep(6000);

    } catch (const CORBA::SystemException& e) {
        TEST_INFO("Unexpected CORBA::SystemException: " << e);
        TEST_FAILED();
    }
    
    // test connections to group (backup1 hostname is expected)
    test_connections(m_expectedBackup1Hostname.in());


    try {
        TEST_INFO("Send kill command to group (only backup1 will stop)");    
    
        CcmFtTest::Command_var command = new CcmFtTest::Command_impl();
        command->commandName(CcmFtTest::COMMAND_KILL);
    
        TEST_INFO("    emits command " <<  command->commandName());
        m_session_context->push_commandEmits(command.in());
        
        TEST_INFO("    wait new primary election");
        Cdmw::OsSupport::OS::sleep(6000);

    } catch (const CORBA::SystemException& e) {
        TEST_INFO("Unexpected CORBA::SystemException: " << e);
        TEST_FAILED();
    }
    
    // test connections to group (backup2 hostname is expected)
    test_connections(m_expectedBackup2Hostname.in());

    try {
        TEST_INFO("Send kill command to group (last primary will stop)");    
    
        CcmFtTest::Command_var command = new CcmFtTest::Command_impl();
        command->commandName(CcmFtTest::COMMAND_KILL);
    
        TEST_INFO("    emits command " <<  command->commandName());
        m_session_context->push_commandEmits(command.in());
        
        TEST_INFO("    wait new primary election");
        Cdmw::OsSupport::OS::sleep(6000);

    } catch (const CORBA::SystemException& e) {
        TEST_INFO("Unexpected CORBA::SystemException: " << e);
        TEST_FAILED();
    }
    
    try {
        TEST_INFO("Test that group is empty");
        add_nbOfRequestedTestOK(1);
        
        CcmFtTest::HostInfo_var hostInfo = m_session_context->get_connection_serverHostInfo();
    
        CORBA::String_var hostname = hostInfo->get_hostname();
        TEST_INFO("Request on empty grou succeed !! hostname: " << hostname.in());
        TEST_FAILED();
         
    } catch (const CORBA::TRANSIENT& e) {
        TEST_SUCCEED();
    } catch (const CORBA::SystemException& e) {
        TEST_INFO("Unexpected CORBA::SystemException: " << e);
        TEST_FAILED();
    }
    
}


void
CcmFtTest::CCM_Client_impl::test_connections(const char* expectedHostname)
{
    add_nbOfRequestedTestOK(3);
    
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
    

    
    try {
        TEST_INFO("Test connections to ServerWithEvents group (expected primary host: " 
                  << expectedHostname << ")");    
    
        CcmFtTest::Command_var command = new CcmFtTest::Command_impl();
        command->commandName(CcmFtTest::COMMAND_GET_HOSTNAME);
    
        TEST_INFO("    emits command " <<  command->commandName());
        m_session_context->push_commandEmits(command.in());
        
        TEST_INFO("    wait result");
        Cdmw::OsSupport::OS::sleep(2000);
        
        CORBA::String_var hostname1 = getHostnameCommandResult();
        
        if (strcmp(expectedHostname, hostname1.in()) == 0) {
            TEST_SUCCEED();
        } else {
            TEST_INFO("Server's hostname is not the one expected: " << hostname1.in());
            TEST_FAILED();            
        }
        

        TEST_INFO("    publishes command " <<  command->commandName());
        m_session_context->push_commandPublishes(command.in());
        
        TEST_INFO("    wait result");
        Cdmw::OsSupport::OS::sleep(2000);
        
        CORBA::String_var hostname2 = getHostnameCommandResult();
        
        if (strcmp(expectedHostname, hostname2.in()) == 0) {
            TEST_SUCCEED();
        } else {
            TEST_INFO("Server's hostname is not the one expected: " << hostname2.in());
            TEST_FAILED();            
        }

    } catch (const CORBA::SystemException& e) {
        TEST_INFO("Unexpected CORBA::SystemException: " << e);
        TEST_FAILED();
    }
}




