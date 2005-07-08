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

#include "testccmcif/CCM_Client_impl.hpp"
#include "testccmcif/Event_impl.hpp"

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
     set_nbOfRequestedTestOK (29);
    
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


      TEST_INFO("test_short");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              CORBA::Short in_ = 1;
              CORBA::Short inout_;              
              CORBA::Short out_;
              CORBA::Short ret_;

              ret_ = display->test_short(in_, inout_, out_);
              if ((inout_ == (in_*2))
                  && (out_ == (in_*3))
                  && (ret_ == (in_*4)))
                  test_ok = true;
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok); 

      TEST_INFO("test_long");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              CORBA::Long in_ = 1;
              CORBA::Long inout_;              
              CORBA::Long out_;
              CORBA::Long ret_;

              ret_ = display->test_long(in_, inout_, out_);
              if ((inout_ == (in_*2))
                  && (out_ == (in_*3))
                  && (ret_ == (in_*4)))
                  test_ok = true;              
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_long_long");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              CORBA::LongLong in_ = 1;
              CORBA::LongLong inout_;              
              CORBA::LongLong out_;
              CORBA::LongLong ret_;

              ret_ = display->test_long_long(in_, inout_, out_);
              if ((inout_ == (in_*2))
                  && (out_ == (in_*3))
                  && (ret_ == (in_*4)))
                  test_ok = true;              
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_unsigned_short");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              CORBA::UShort in_ = 1;
              CORBA::UShort inout_;              
              CORBA::UShort out_;
              CORBA::UShort ret_;

              ret_ = display->test_unsigned_short(in_, inout_, out_);
              if ((inout_ == (in_*2))
                  && (out_ == (in_*3))
                  && (ret_ == (in_*4)))
                  test_ok = true;              
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_unsigned_long");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              CORBA::ULong in_ = 1;
              CORBA::ULong inout_;              
              CORBA::ULong out_;
              CORBA::ULong ret_;

              ret_ = display->test_unsigned_long(in_, inout_, out_);
              if ((inout_ == (in_*2))
                  && (out_ == (in_*3))
                  && (ret_ == (in_*4)))
                  test_ok = true;              
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_unsigned_long_long");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              CORBA::ULongLong in_ = 1;
              CORBA::ULongLong inout_;              
              CORBA::ULongLong out_;
              CORBA::ULongLong ret_;

              ret_ = display->test_unsigned_long_long(in_, inout_, out_);
              if ((inout_ == (in_*2))
                  && (out_ == (in_*3))
                  && (ret_ == (in_*4)))
                  test_ok = true;              
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_float");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              CORBA::Float in_ = 1;
              CORBA::Float inout_;              
              CORBA::Float out_;
              CORBA::Float ret_;

              ret_ = display->test_float(in_, inout_, out_);
              if ((inout_ == (in_*2))
                  && (out_ == (in_*3))
                  && (ret_ == (in_*4)))
                  test_ok = true;              
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_double");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              CORBA::Double in_ = 1;
              CORBA::Double inout_;              
              CORBA::Double out_;
              CORBA::Double ret_;

              ret_ = display->test_double(in_, inout_, out_);
              if ((inout_ == (in_*2))
                  && (out_ == (in_*3))
                  && (ret_ == (in_*4)))
                  test_ok = true;              
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_boolean");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              CORBA::Boolean in_ = 1;
              CORBA::Boolean inout_;              
              CORBA::Boolean out_;
              CORBA::Boolean ret_;

              ret_ = display->test_boolean(in_, inout_, out_);
              if ((inout_ == in_)
                  && (out_ == in_)
                  && (ret_ == in_))
                  test_ok = true;              
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_char");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              CORBA::Char in_ ('1');
              CORBA::Char inout_;              
              CORBA::Char out_;
              CORBA::Char ret_;

              ret_ = display->test_char(in_, inout_, out_);
              if ((inout_ == in_)
                  && (out_ == in_)
                  && (ret_ == in_))
                  test_ok = true;              
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_wchar");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              CORBA::WChar in_ ('1');
              CORBA::WChar inout_;              
              CORBA::WChar out_;
              CORBA::WChar ret_;

              ret_ = display->test_wchar(in_, inout_, out_);
              if ((inout_ == in_)
                  && (out_ == in_)
                  && (ret_ == in_))
                  test_ok = true;              
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_octet");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              CORBA::Octet in_ = 1;
              CORBA::Octet inout_;              
              CORBA::Octet out_;
              CORBA::Octet ret_;

              ret_ = display->test_octet(in_, inout_, out_);
              if ((inout_ == in_)
                  && (out_ == in_)
                  && (ret_ == in_))
                  test_ok = true;              
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_function");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              display->set(1);
              BasicDemo::Display_var inout_ = ::BasicDemo::Display::_duplicate (display.in());
              BasicDemo::Display_var out_;
              BasicDemo::Display_var ret_;

              ret_ = display->test_function(display.in(), inout_.inout(), out_.out());
              
              if ((inout_->get() == display->get())
                  && (out_->get() == display->get())
                  && (ret_->get() == display->get()))
                  test_ok = true;              
          }
      }
      catch (const CORBA::SystemException& ex)
      {
          std::cout<<ex<<std::endl;
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_fixed_struct");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              BasicDemo::s1 in_;
              in_.l=1;
              BasicDemo::s1 inout_;              
              BasicDemo::s1 out_;
              BasicDemo::s1 ret_;

              ret_ = display->test_fixed_struct(in_, inout_, out_);
              if ((inout_.l == in_.l)
                  && (out_.l == in_.l)
                  && (ret_.l == in_.l))
                  test_ok = true;              
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_var_struct");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              BasicDemo::s2 in_;
              in_.s = CORBA::string_dup("1");
              BasicDemo::s2_var inout_ = new BasicDemo::s2();              
              BasicDemo::s2_var out_;
              BasicDemo::s2_var ret_;

             ret_ = display->test_var_struct(in_, inout_.inout(), out_.out());
             if (strcmp(inout_->s, in_.s) == 0 
                 && strcmp(out_->s, in_.s) == 0
                 && strcmp(ret_->s, in_.s) == 0)
                  test_ok = true;              
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_fixed_union");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              ::BasicDemo::Display::union1 in_;
              in_.s(1);
              ::BasicDemo::Display::union1 inout_;
              ::BasicDemo::Display::union1 out_;
              ::BasicDemo::Display::union1 ret_;
              
              ret_ = display->test_fixed_union(in_, inout_, out_);
              if ((inout_.s() == in_.s())  
                  && (out_.s() == in_.s())
                  && (ret_.s() == in_.s()))
                  test_ok = true;              
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_var_union");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              ::BasicDemo::Display::union2 in_;
              in_.s1("1");
              ::BasicDemo::Display::union2 inout_;              
              ::BasicDemo::Display::union2_var out_;
              ::BasicDemo::Display::union2_var ret_;
              
              ret_ = display->test_var_union(in_, inout_, out_.out());
              if (strcmp(inout_.s1(), in_.s1()) == 0  
                  && strcmp(out_->s1(), in_.s1()) == 0
                  && strcmp(ret_->s1(), in_.s1()) == 0)
                  test_ok = true;              
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_seq_struct");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              ::BasicDemo::s1s in_;
              in_.length(1);
              in_[0].l = 1;
              ::BasicDemo::s1s inout_;              
              ::BasicDemo::s1s_var out_;
              ::BasicDemo::s1s_var ret_;
              
              ret_ = display->test_seq_struct(in_, inout_, out_.out());
              if (inout_[0].l == in_[0].l  
                  && out_[0].l == in_[0].l
                  && ret_[0].l == in_[0].l)
                  test_ok = true;              
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_string");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              char * in_ = CORBA::string_dup("hello");
              char * inout_ = CORBA::string_dup("hella");;
              char * out_;
              char * ret_;
              CORBA::String_out str_out (out_);
              ret_ = display->test_string(in_, inout_, str_out);
              if (strcmp(inout_, in_) == 0  
                  && strcmp(out_, in_) == 0
                  && strcmp(ret_, in_) == 0)
                  test_ok = true;              
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_wstring");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              CORBA::WChar * in_ = CORBA::wstring_dup((CORBA::WChar *)"hello");
              CORBA::WChar * inout_ = CORBA::wstring_dup((CORBA::WChar *)"hella");              
              CORBA::WChar * out_;
              CORBA::WChar * ret_;
              
              ret_ = display->test_wstring(in_, inout_, out_);
              if (strcmp((char *)inout_, (char *)in_) == 0  
                  && strcmp((char *)out_, (char *)in_) == 0
                  && strcmp((char *)ret_, (char *)in_) == 0)
                  test_ok = true;              
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_fixed_array");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              ::BasicDemo::Display::array1 in_;
              for (int i =0 ; i<5; i++)
                  in_[i] = i;
              ::BasicDemo::Display::array1 inout_;              
              for (int i =0 ; i<5; i++)
                  inout_[i] = i + 5;
              ::BasicDemo::Display::array1 out_;
              ::BasicDemo::Display::array1_slice * ret_;
              
              ret_ = display->test_fixed_array(in_, inout_, out_);
              if (inout_[0] == in_[0]  
                  && out_[0] == in_[0]
                  && ret_[0] == in_[0])
                  test_ok = true;              
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_var_array");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              ::BasicDemo::Display::array2 in_;
              for (int i=0 ; i<5; i++)
                  in_[i] = CORBA::string_dup("hello");
              ::BasicDemo::Display::array2 inout_;              
              for (int i=0 ; i<5; i++)
                  inout_[i] = CORBA::string_dup("hella");
              ::BasicDemo::Display::array2_slice * out_;
              ::BasicDemo::Display::array2_slice * ret_;
              ret_ = display->test_var_array(in_, inout_, out_);
              if (strcmp(inout_[0], in_[0]) == 0  
                  && strcmp(out_[0], in_[0]) == 0
                  && strcmp(ret_[0], in_[0])==0 )
                  test_ok = true;              
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_any");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              CORBA::Any in_;
              CORBA::Short s1 = 2;
              in_ <<= s1;

              CORBA::Any inout_;
              CORBA::Short s2 = 3;
              inout_ <<= s2;

              CORBA::Any_var out_;
              CORBA::Any_var ret_;
              
              ret_ = display->test_any(in_, inout_, out_);
              CORBA::Short short_in, short_inout, short_out, short_ret;
              if ((in_ >>= short_in) &&
                  (inout_ >>= short_inout) &&
                  (out_.in () >>= short_out) &&
                  (ret_.in () >>= short_ret))
              {
                  if (short_ret == short_in
                      && short_inout == short_in
                      && short_out == short_in)
                      test_ok = true;              
              }
          }
      }
      catch (...)
      {
          TEST_INFO("Unexpected exception raised!");
      }
      TEST_CHECK(test_ok);

      TEST_INFO("test_value_type");
      test_ok = false;
      try
      {
          if (!CORBA::is_nil(register_server.in()))
          {
              BasicDemo::Event_var in_ = new BasicDemo::Event_impl();
              BasicDemo::Event_var inout_ = new BasicDemo::Event_impl();
              BasicDemo::Event_var out_;
              BasicDemo::Event_var ret_;

              in_->text("Hello");
              inout_->text("Hella");

              ret_ = display->test_event(in_.in(), inout_.inout(), out_.out());

              if (strcmp(in_->text(), inout_->text()) == 0
                  && strcmp(in_->text(), out_->text()) == 0 
                  && strcmp(in_->text(), ret_->text()) == 0)
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

