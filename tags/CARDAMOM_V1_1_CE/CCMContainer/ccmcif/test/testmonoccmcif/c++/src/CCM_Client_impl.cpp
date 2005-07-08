/* ===================================================================== */
/*
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
*/
/* ===================================================================== */

#include "testmonoccmcif/CCM_Client_impl.hpp"
#include "testmonoccmcif/Event_impl.hpp"

#include "Foundation/testutils/Testable.hpp"
#include "Foundation/common/Assert.hpp"


namespace 
{

/**
*  This class provides an object to test the ccm_activate.
*
**/

class test_ccm_client_activate : public Cdmw::TestUtils::Testable
{
  BasicDemo::CCM_Client_impl* m_client;
  
 public:
  test_ccm_client_activate(BasicDemo::CCM_Client_impl* client)
      : Testable("Test ccm_activate on Client"),
        m_client(client)
  {
  };

  ~test_ccm_client_activate() {};

  void do_tests()
  {
     // set number of requested successfull tests
     set_nbOfRequestedTestOK (5);
    
     CDMW_ASSERT(m_client);

      ::BasicDemo::Registration_var register_server;
     ::BasicDemo::CCM_Client_Context_var client_context = m_client->get_context();
      
     TEST_INFO("Get_connection_to_register...");
      
      bool test_ok = false;
      try
      {
          register_server = client_context->get_connection_to_register();

            if (!CORBA::is_nil(register_server.in()))
            {
                test_ok = true;
            }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      // register client
      TEST_INFO("Register client...");
     test_ok = false;
      try
      {
             if (!CORBA::is_nil(register_server.in()))
            {
              CORBA::String_var client_name = register_server->_cxx_register();
              m_client->set_name(client_name.in());
                test_ok = true;
         }
      }
      catch (...)
      {
            TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);
     
     //get the connection to the server 
      TEST_INFO("Connect to the display facet...");
      ::BasicDemo::Display_var display;
     test_ok = false;
      try
      {
         display = client_context->get_connection_to_server();

            if (!CORBA::is_nil(display.in()))
            {
                test_ok = true;
            }
      }
      catch (...)
      {
            TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

     //print "hello" via the server
      TEST_INFO("Print hello...");
     test_ok = false;
      try
      {
             if (!CORBA::is_nil(register_server.in()))
            {
            display->print(m_client->get_text());
                test_ok = true;
            }
      }
      catch (...)
      {
            TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

     //print_event "hello" via the server
      TEST_INFO("Print ValueType hello...");
     test_ok = false;
      try
      {
             if (!CORBA::is_nil(register_server.in()))
            {
             BasicDemo::Event_var event = new BasicDemo::Event_impl();
             event->text("Hello ValueType!");
             
             display->print_event(event.in());
             test_ok = true;
            }
      }
      catch (...)
      {
            TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);
  };
}; // End of class test_ccm_client_activate
    
} // End of anonymous namespace



//
// IDL:thalesgroup.com/BasicDemo/CCM_Client:1.0
//
BasicDemo::CCM_Client_impl::CCM_Client_impl()
    : m_name(""),
      m_session_context(BasicDemo::CCM_Client_Context::_nil())
{
}

BasicDemo::CCM_Client_impl::~CCM_Client_impl()
{
    std::cout << "CCM_Client_impl::~CCM_Client_impl called!" << std::endl;
}


//
// set_session_context function : it fixes the session context of the component (call by the container)
//
void
BasicDemo::CCM_Client_impl::set_session_context(Components::SessionContext_ptr ctx)
    throw(CORBA::SystemException)
{
    std::cout << "Set_session_context called!" << std::endl;
   
    m_session_context = ::BasicDemo::CCM_Client_Context::_narrow(ctx); 
}


//
// ccm_activate function : it actives the component (call by the container)
//
void
BasicDemo::CCM_Client_impl::ccm_activate()
   throw(CORBA::SystemException)
{

    test_ccm_client_activate test(this);

    test.start();
}


//
// ccm_passivate function : it makes stop the component (call by the container)
//

void
BasicDemo::CCM_Client_impl::ccm_passivate()
   throw(CORBA::SystemException)
{
   std::cout << "Ccm_passivate called!" << std::endl;
} 



//
// ccm_remove function : it destroys the component (call by the container)
void
BasicDemo::CCM_Client_impl::ccm_remove()
   throw(CORBA::SystemException)
{
   std::cout << "Ccm_remove called!" << std::endl;
}


//
// IDL:thalesgroup.com/BasicDemo/CCM_Client_Executor/name:1.0
//
char*
BasicDemo::CCM_Client_impl::name()
    throw(CORBA::SystemException)
{
    char* name = CORBA::string_dup(m_name.in());
    return name;
}

//
// IDL:thalesgroup.com/BasicDemo/CCM_Client_Executor/event:1.0
//
BasicDemo::Event* 
BasicDemo::CCM_Client_impl::event()
    throw(CORBA::SystemException)
{
    BasicDemo::Event* event = new BasicDemo::Event_impl();
    event->text(m_text.in());

    return event;
}

void 
BasicDemo::CCM_Client_impl::event(BasicDemo::Event* event)
    throw(CORBA::SystemException)
{
    m_text = CORBA::string_dup(event->text());
}

