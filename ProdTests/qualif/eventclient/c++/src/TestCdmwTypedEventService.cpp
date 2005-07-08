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

#include <iostream>
#include <string>
#include <sstream>
#include "eventclient/TestCdmwTypedEventService.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/testutils/Testable.hpp"

#include "Event/eventinterface/TypedEventChannel.hpp"

#include "eventclient/Echo.skel.hpp"

namespace
{
    const unsigned short NB_RCVMSG = 4;
    const char* MSG = "Hello world";
    const char* INTERFACE_REF="IDL:TypedEcho:1.0";

    const char* CHANNEL_1_NAME = "TypedEventChannel1";
    const char* CHANNEL_2_NAME = "TypedEventChannel2";
    const char* CHANNEL_3_NAME = "TypedEventChannel3";
    const char* CHANNEL_4_NAME = "TypedEventChannel4";
    const char* COMPLETE_CHANNEL_1_NAME = "EVENT_CHANNELS/TypedEventChannel1";
    const char* COMPLETE_CHANNEL_2_NAME = "EVENT_CHANNELS/TypedEventChannel2";
    const char* COMPLETE_CHANNEL_3_NAME = "EVENT_CHANNELS/TypedEventChannel3";
    const char* COMPLETE_CHANNEL_4_NAME = "EVENT_CHANNELS/TypedEventChannel4";


    static bool end_of_test = false;


// ----------------------------------------------------------------------
// Echo_impl implementation
// ----------------------------------------------------------------------
class Echo_impl : virtual public POA_test::TypedEcho,
                  virtual public PortableServer::RefCountServantBase
{
    CORBA::ORB_var           m_orb; // The ORB
    PortableServer::POA_var  m_poa; // My POA.
    test::TypedEcho_var            m_pushConsumer; // ref of the consumer to
                                             // which msg received is pushed.
    int                      m_id;  // consumer id

public:
    std::string m_rcvMsg_array[10];
    unsigned short m_nbRcvdMsg;
    
    Echo_impl(CORBA::ORB_ptr orb, 
              PortableServer::POA_ptr poa,
	      test::TypedEcho_ptr pushConsumer, 
              int id)
        : m_orb(CORBA::ORB::_duplicate(orb)),
	       m_poa(PortableServer::POA::_duplicate(poa)),
          m_pushConsumer(test::TypedEcho::_duplicate(pushConsumer)),
          m_id(id)
    {
        m_nbRcvdMsg = 0;
    }
    
    //
    // IDL to C++ mapping
    //
    void
    echo_string(const char* str)
        throw(CORBA::SystemException)
    {
        std::cout << "Consumer" << m_id << " has received: " << str << std::endl;
        m_rcvMsg_array[m_nbRcvdMsg] = str;
        m_nbRcvdMsg++;

        if (!CORBA::is_nil(m_pushConsumer.in()))
        {
            m_pushConsumer->echo_string(str);
        }
    }

    void
    disconnect_push_consumer()
	     throw(CORBA::SystemException)
    {
        std::cout << "Disconnect" << std::endl;
        PortableServer::ObjectId_var oid = m_poa-> servant_to_id(this);
        m_poa-> deactivate_object(oid.in());

        end_of_test = true;
    }

    void
    push(const CORBA::Any& data)
	     throw(CORBA::SystemException)
    {
		 std::cout << "TypedPushEventClient.cpp : push any not implemented" << std::endl;
    }
    
    PortableServer::POA_ptr
    _default_POA()
    {
        return PortableServer::POA::_duplicate(m_poa.in());
    }    
}; // End class Echo_impl

static std::string int_to_string (int nb)
{
    char str[10];

    sprintf(str, "%d", nb);

    std::string std_str = str;
    return std_str;
}

}; // End anonymous namespace

namespace Cdmw
{
namespace Event
{


/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/


TestCdmwTypedEventService::TestCdmwTypedEventService(CORBA::ORB_ptr orb,
                                             CosNaming::NamingContext_ptr nc,
                                             int test_nb)
    throw()
    :Testable(std::string("CdmwTypedEventService TestProcess") + int_to_string(test_nb)),
     m_orb(CORBA::ORB::_duplicate(orb)),
     m_namingcontext(CosNaming::NamingContext::_duplicate(nc)),
     m_test_nb(test_nb)
{
    // does nothing
}


TestCdmwTypedEventService::~TestCdmwTypedEventService()
    throw()
{
    // does nothing
}


TestCdmwTypedEventService::TestCdmwTypedEventService(const TestCdmwTypedEventService& rhs)
    throw()
    :Testable(std::string("CdmwTypedEventService TestProcess") + int_to_string(rhs.m_test_nb)),
     m_orb(CORBA::ORB::_duplicate(rhs.m_orb)),
     m_namingcontext(CosNaming::NamingContext::_duplicate(rhs.m_namingcontext)),
     m_test_nb(rhs.m_test_nb)

{
    // does nothing
}
		
TestCdmwTypedEventService&
TestCdmwTypedEventService::operator=(const TestCdmwTypedEventService& rhs)
    throw()
{
    if (this != &rhs) 
    {
        m_orb = CORBA::ORB::_duplicate(rhs.m_orb.in());
        m_namingcontext = CosNaming::NamingContext::_duplicate(rhs.m_namingcontext);
        m_test_nb = m_test_nb;
    }
    return *this;
}

void TestCdmwTypedEventService::do_tests()		
{
    bool test_ok=false;
    test::TypedEcho_var pushConsumer;
    PortableServer::POA_var rootPOA;
    Echo_impl* pcImpl;
    CORBA::Object_var obj;


    // This tests EventChannel class
    //-------------------------------------------------
    TEST_INFO("Testing Cdmw Typed Event Service");
    
    // Create the implementation object.
    // Resolve Root POA.
    rootPOA = PortableServer::POA::_narrow(CORBA::Object_var(
        m_orb -> resolve_initial_references("RootPOA")).in());
    
    if (CORBA::is_nil(rootPOA.in()))
    {
        std::cerr << "ERROR: No RootPOA" << std::endl;
        TEST_CHECK(false);
        return;
    }
    
	int timescale = Cdmw::TestUtils::Testable::get_timescale();

    try 
    {        
        
        Cdmw::Event::TypedEventChannel* p_eventchannel1 = NULL;
        Cdmw::Event::TypedEventChannel* p_eventchannel2 = NULL;
        Cdmw::Event::TypedEventChannel* p_eventchannel3 = NULL;
        Cdmw::Event::TypedEventChannel* p_eventchannel4 = NULL;

        TEST_INFO("Get Event Channel references");
        test_ok=true;
        for (int i=0; i<200; i++)
        {
            try
            {
                p_eventchannel4 = new Cdmw::Event::TypedEventChannel(COMPLETE_CHANNEL_4_NAME, m_namingcontext.in());
                break;
            }
            catch (const CORBA::BAD_PARAM& e)
            {
                // try again, maybe event channel is still not created!
            }
            OsSupport::OS::sleep(timescale*20);
        }

        if (p_eventchannel4 == NULL)
        {
            TEST_INFO("EventChannel4 not found!");
            test_ok=false;
        }
        try
        {
            p_eventchannel1 = new Cdmw::Event::TypedEventChannel(COMPLETE_CHANNEL_1_NAME, m_namingcontext.in());
            p_eventchannel2 = new Cdmw::Event::TypedEventChannel(COMPLETE_CHANNEL_2_NAME, m_namingcontext.in());
            p_eventchannel3 = new Cdmw::Event::TypedEventChannel(COMPLETE_CHANNEL_3_NAME, m_namingcontext.in());
        }
        catch (const CORBA::BAD_PARAM& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        switch (m_test_nb)
        {
        case 1:
            {
                int loop_index = 0;
                
                TEST_INFO("Get a PushConsumer on EventChannel1");
                test_ok=false;
                try
                {
                    pushConsumer = p_eventchannel1->get_typedPushConsumer<test::TypedEcho>(INTERFACE_REF, CosEventComm::PushSupplier::_nil());
                    test_ok=true;
                }
                catch(...)
                {
                    TEST_INFO("Unexpected exception raise!");
                }
                TEST_CHECK(test_ok);
                
                TEST_INFO("Connect local consumer to EventChannel2");
                test_ok=false;
                pcImpl = new Echo_impl(m_orb.in(), 
                    rootPOA.in(), 
		    test::TypedEcho::_nil(), 
                    m_test_nb);
                try
                {
                    p_eventchannel2->connect_typedPushConsumer(INTERFACE_REF, pcImpl->_this());
                    test_ok=true;
                }
                catch(...)
                {
                    TEST_INFO("Unexpected exception raised!");
                }
                TEST_CHECK(test_ok);
                
                OsSupport::OS::sleep(timescale*2000);

                TEST_INFO("Send message to EventChannel1");
                test_ok=true;
                {
                    std::ostringstream os;
                    
                    os << "Send <" << MSG << "> to EventChannel1" << std::ends;
                    TEST_INFO(os.str());
                    
                    pushConsumer->echo_string(MSG);
                }
                
                // wait for received msg
                while (pcImpl->m_nbRcvdMsg != NB_RCVMSG)
                {
                    OsSupport::OS::sleep(timescale*10);
                    if (loop_index == 200)
                    {
                        test_ok = false;
                        break;
                    }
                    
                    loop_index++;
                }
                
                if (test_ok)
                {
                    // messages received! Compare them!
                    for (int i=0; i<NB_RCVMSG ; i++)
                    {
                        if (strcmp(MSG, pcImpl->m_rcvMsg_array[i].c_str()) != 0)
                            test_ok = false;
                    }
                }
                TEST_CHECK(test_ok);

                OsSupport::OS::sleep(timescale*2000);
                TEST_INFO("Destroy EventChannels...");
                test_ok = false;
                try
                {
                    p_eventchannel1->destroy();
                    p_eventchannel2->destroy();
                    p_eventchannel3->destroy();
                    p_eventchannel4->destroy();
                    delete p_eventchannel1;
                    delete p_eventchannel2;
                    delete p_eventchannel3;
                    delete p_eventchannel4;
                    test_ok = true;
                }
                catch (...)
                {
                    TEST_INFO("Unexpected exception raised!");
                }
                TEST_CHECK(test_ok);

                TEST_INFO("Try to find EventChannel1!");
                test_ok = false;
                try
                {
                    p_eventchannel1 = new Cdmw::Event::TypedEventChannel(COMPLETE_CHANNEL_1_NAME, m_namingcontext.in());
                }
                catch (const CORBA::BAD_PARAM& e)
                {
                    std::ostringstream os;
                    os << e << std::endl;
                    TEST_INFO(os.str());
                    test_ok = true;
                }
                TEST_CHECK(test_ok);

                while (!end_of_test)
                {
                    OsSupport::OS::sleep(timescale*100);
                }
                
                TEST_INFO("TestProcess1 ending...");
                
                m_orb->shutdown(false);
                break;
            }

        case 2:
            TEST_INFO("Get a PushConsumer on EventChannel3");
            test_ok=false;
            try
            {
                pushConsumer = p_eventchannel3->get_typedPushConsumer<test::TypedEcho>(INTERFACE_REF, CosEventComm::PushSupplier::_nil());
                test_ok=true;
            }
            catch(...)
            {
                TEST_INFO("Unexpected exception raised!");
            }
            TEST_CHECK(test_ok);

            TEST_INFO("Connect local consumer to EventChannel1");
            test_ok=false;
            pcImpl = new Echo_impl(m_orb.in(), rootPOA.in(), pushConsumer.in(), m_test_nb);
            try
            {
                p_eventchannel1->connect_typedPushConsumer(INTERFACE_REF,pcImpl->_this());
                test_ok=true;
            }
            catch(...)
            {
                TEST_INFO("Unexpected exception raised!");
            }
            TEST_CHECK(test_ok);

            // wait for disconnect on consumer
            while (!end_of_test)
            {
                OsSupport::OS::sleep(timescale*100);
            }

            TEST_INFO("TestProcess2 ending...");

            delete p_eventchannel1;
            delete p_eventchannel2;
            delete p_eventchannel3;
            delete p_eventchannel4;

            m_orb->shutdown(false);
            break;
            
        case 3:
            TEST_INFO("Get a PushConsumer on EventChannel2");
            test_ok=false;
            try
            {
                pushConsumer = p_eventchannel2->get_typedPushConsumer<test::TypedEcho>(INTERFACE_REF,
                                                             CosEventComm::PushSupplier::_nil());
                test_ok=true;
            }
            catch(...)
            {
                TEST_INFO("Unexpected exception raised!");
            }
            TEST_CHECK(test_ok);

            TEST_INFO("Connect local consumer to EventChannel3");
            test_ok=false;
            pcImpl = new Echo_impl(m_orb.in(), rootPOA.in(), pushConsumer.in(), m_test_nb);
            try
            {
                p_eventchannel3->connect_typedPushConsumer(INTERFACE_REF, pcImpl->_this());
                test_ok=true;
            }
            catch(...)
            {
                TEST_INFO("Unexpected exception raised!");
            }
            TEST_CHECK(test_ok);
         
            // wait for disconnect on consumer
            while (!end_of_test)
            {
                OsSupport::OS::sleep(timescale*100);
            }

            TEST_INFO("TestProcess3 ending...");

            delete p_eventchannel1;
            delete p_eventchannel2;
            delete p_eventchannel3;
            delete p_eventchannel4;

            m_orb->shutdown(false);
            break;

        case 4:
            TEST_INFO("Get a PushConsumer on EventChannel4");
            test_ok=false;
            try
            {
                pushConsumer = p_eventchannel4->get_typedPushConsumer<test::TypedEcho>(INTERFACE_REF, CosEventComm::PushSupplier::_nil());
                test_ok=true;
            }
            catch(...)
            {
                TEST_INFO("Unexpected exception raised!");
            }
            TEST_CHECK(test_ok);

            TEST_INFO("Connect local consumer to EventChannel3");
            test_ok=false;
            pcImpl = new Echo_impl(m_orb.in(), rootPOA.in(), pushConsumer.in(), m_test_nb);
            try
            {
                p_eventchannel3->connect_typedPushConsumer(INTERFACE_REF, pcImpl->_this());
                test_ok=true;
            }
            catch(...)
            {
                TEST_INFO("Unexpected exception raised!");
            }
            TEST_CHECK(test_ok);
           
            // wait for disconnect on consumer
            while (!end_of_test)
            {
                OsSupport::OS::sleep(timescale*100);
            }

            TEST_INFO("TestProcess4 ending...");

            delete p_eventchannel1;
            delete p_eventchannel2;
            delete p_eventchannel3;
            delete p_eventchannel4;

            m_orb->shutdown(false);
            break;

        default:
            TEST_INFO("Invalid test number!");
            break;
        }
    } 
    catch (const CORBA::Exception & e) 
    {
        std::cerr << "ERROR: " << e << std::endl;
    }
}
		
}; // End namespace Event
}; // End namespace Cdmw

