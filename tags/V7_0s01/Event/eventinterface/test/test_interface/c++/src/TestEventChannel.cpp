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


#include <iostream>
#include <string>
#include <sstream>
#include "test_interface/TestEventChannel.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/testutils/Testable.hpp"

#include "Event/idllib/CosEventComm.skel.hpp"
#include "Event/eventinterface/EventChannel.hpp"

namespace
{
    const unsigned short NB_MSG = 2;
    const unsigned short NB_RCVMSG = 10;
    const char* MSG_ARRAY[2] = {"Hello world 1", "Hello world 2"};
    
    
    class PushConsumer_impl : virtual public POA_CosEventComm::PushConsumer,
        virtual public PortableServer::RefCountServantBase
    {
        CORBA::ORB_var orb_; // The ORB
        PortableServer::POA_var poa_; // My POA.
        static int nb_consumer;  // nuber of consumer
        int id_;  // consumer id
        
    public:
        std::string m_rcvMsg_array[10];
        unsigned short m_nbRcvdMsg;
        
        PushConsumer_impl(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa)
            : orb_(CORBA::ORB::_duplicate(orb)),
            poa_(PortableServer::POA::_duplicate(poa))
        {
            m_nbRcvdMsg = 0;
            nb_consumer++;
            id_ = nb_consumer;
        }
        
        virtual
            ~PushConsumer_impl()
        {
        }
        
        void
            push(const CORBA::Any& data)
            throw(CORBA::SystemException)
        {
            const char* str;
            
            if (data >>= str)
            {
                std::cout << "Consumer" << id_ << " has received: " << str << std::endl;
                m_rcvMsg_array[m_nbRcvdMsg] = str;
            }
            else
            {
                std::cout << "Consumer" << id_ << " has an unknown event" << std::endl;
                m_rcvMsg_array[m_nbRcvdMsg] = "Unknown event";
            }
            m_nbRcvdMsg++;
        }
        
        void
            disconnect_push_consumer()
            throw(CORBA::SystemException)
        {
            std::cout << "Disconnect" << std::endl;
            PortableServer::ObjectId_var oid = poa_ -> servant_to_id(this);
            poa_ -> deactivate_object(oid.in());
            
            orb_ -> shutdown(false);
        }
        
        PortableServer::POA_ptr
            _default_POA()
        {
            return PortableServer::POA::_duplicate(poa_.in());
        }    
    }; // End PushConsumer_impl class
    
    int PushConsumer_impl::nb_consumer = 1; // (consumer1 is in the main (different code))
    
    
}; // End anonymous namespace

namespace Cdmw
{
    namespace Event
    {
        const char* CHANNEL_1_NAME = "TestEventChannel1";
        const char* CHANNEL_2_NAME = "TestEventChannel2";
        const char* CHANNEL_3_NAME = "TestEventChannel3";
        const char* CHANNEL_4_NAME = "TestEventChannel4";
        const char* COMPLETE_CHANNEL_1_NAME = "CDMW/SERVICES/EVENT/EVENT_CHANNELS/TestEventChannel1";
        const char* COMPLETE_CHANNEL_2_NAME = "CDMW/SERVICES/EVENT/EVENT_CHANNELS/TestEventChannel2";
        const char* COMPLETE_CHANNEL_3_NAME = "CDMW/SERVICES/EVENT/EVENT_CHANNELS/TestEventChannel3";
        const char* COMPLETE_CHANNEL_4_NAME = "CDMW/SERVICES/EVENT/EVENT_CHANNELS/TestEventChannel4";
        const char* UNCREATED_CHANNEL_NAME = "CDMW/SERVICES/EVENT/EVENT_CHANNELS/TestEventChannel6";
        const char* INV_CHANNEL_NAME = "Test/EventChannel4";
        
        
        /**
        *Implementation notes: [if no pertinent write none ]
        *<p>
        *Portability issues: [if no pertinent write none ]
        *<p>
        */
        
        
        TestEventChannel::TestEventChannel(CORBA::ORB_ptr orb)
            throw()
            :Testable("Cdmw::Event::EventChannel"),
            m_orb(CORBA::ORB::_duplicate(orb))
        {
            // does nothing
        }
        
        
        TestEventChannel::~TestEventChannel()
            throw()
        {
            // does nothing
        }
        
        
        TestEventChannel::TestEventChannel(const TestEventChannel& rhs)
            throw()
            :Testable("Cdmw::Event::EventChannel"),
            m_orb(CORBA::ORB::_duplicate(rhs.m_orb.in()))
        {
            // does nothing
        }
        
        TestEventChannel&
            TestEventChannel::operator=(const TestEventChannel& rhs)
            throw()
        {
            if (this != &rhs) {
                m_orb = CORBA::ORB::_duplicate(rhs.m_orb.in());
            }
            return *this;
        }
        
        void TestEventChannel::do_tests()        
        {
            // set number of requested successfull tests
            set_nbOfRequestedTestOK (10);
            
            bool test_ok=false;
            CosEventComm::PushConsumer_var pushConsumer;
            PortableServer::POA_var rootPOA;
            
            // This tests EventChannel class
            //-------------------------------------------------
            TEST_INFO("Testing EventChannel");
            
            // Create the implementation object.
            // Resolve Root POA.
            CORBA::Object_var obj = m_orb -> resolve_initial_references("RootPOA");
            rootPOA = PortableServer::POA::_narrow(obj.in());
            
            if (CORBA::is_nil(rootPOA.in()))
            {
                std::cerr << "ERROR: No RootPOA" << std::endl;
                TEST_CHECK(false);
                return;
            }
            
            // Get a reference to the CDMW Naming And Repository from RepositoryInterface
            CdmwNamingAndRepository::Repository_var repository = 
                NamingAndRepository::RepositoryInterface::get_repository();
            
            // Get the default root context from the repository
            CosNaming::NamingContext_var rootCtx =
                repository->resolve_root_context(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
            
            
            PushConsumer_impl* pcImpl = new PushConsumer_impl(m_orb.in(), rootPOA.in());
            PushConsumer_impl* pcImpl2 = new PushConsumer_impl(m_orb.in(), rootPOA.in());
            
            try 
            {        
                
                EventChannel* p_eventchannel1 = NULL;
                EventChannel* p_eventchannel2 = NULL;
                EventChannel* p_eventchannel3 = NULL;
                EventChannel* p_eventchannel4 = NULL;
                EventChannel* p_eventchannel4_2 = NULL;
                
                TEST_INFO("Initialise an event channel with an invalid channel name");
                test_ok=false;
                try
                {
                    p_eventchannel1 = new EventChannel(INV_CHANNEL_NAME, rootCtx.in());
                }
                catch (const CORBA::BAD_PARAM& e)
                {
                    TEST_INFO(e);
                    test_ok=true;
                }
                TEST_CHECK(test_ok);
                
                TEST_INFO("Initialise an event channel with an uncreated channel name");
                test_ok=false;
                try
                {
                    p_eventchannel1 = new EventChannel(UNCREATED_CHANNEL_NAME, rootCtx.in());
                }
                catch (const CORBA::BAD_PARAM& e)
                {
                    TEST_INFO(e);
                    test_ok=true;
                }
                catch (const CORBA::SystemException& e)
                {
                    std::ostringstream os;
                    os << "Unexpected exception raised: " << e << std::ends;
                    TEST_INFO(os.str());
                    test_ok=false;
                }
                TEST_CHECK(test_ok);
                
                TEST_INFO("Initialise an event channel with a valid channel name");
                test_ok=true;
                try
                {
                    p_eventchannel1 = new EventChannel(COMPLETE_CHANNEL_1_NAME, rootCtx.in());
                }
                catch (const CORBA::SystemException& e)
                {
                    std::ostringstream os;
                    os << "Unexpected exception raised: " << e << std::ends;
                    TEST_INFO(os.str());
                    test_ok=false;
                }
                
                try
                {
                    p_eventchannel2 = new EventChannel(COMPLETE_CHANNEL_2_NAME, rootCtx.in());
                }
                catch (const CORBA::SystemException& e)
                {
                    std::ostringstream os;
                    os << "Unexpected exception raised: " << e << std::ends;
                    TEST_INFO(os.str());
                    test_ok=false;
                }
                try
                {
                    p_eventchannel3 = new EventChannel(COMPLETE_CHANNEL_3_NAME, rootCtx.in());
                }
                catch (const CORBA::SystemException& e)
                {
                    std::ostringstream os;
                    os << "Unexpected exception raised: " << e << std::ends;
                    TEST_INFO(os.str());
                    test_ok=false;
                }
                
                try
                {
                    p_eventchannel4 = new EventChannel(COMPLETE_CHANNEL_4_NAME, rootCtx.in());
                }
                catch (const CORBA::SystemException& e)
                {
                    std::ostringstream os;
                    os << "Unexpected exception raised: " << e << std::ends;
                    TEST_INFO(os.str());
                    test_ok=false;
                }
                TEST_CHECK(test_ok);
                
                TEST_INFO("Destroy eventchannel4");
                test_ok=false;
                try
                {
                    p_eventchannel4->destroy();
                    test_ok=true;
                }
                catch(...)
                {
                    TEST_INFO("Unexpected exception raised!");
                }
                TEST_CHECK(test_ok);
                
                TEST_INFO("Try to get interface on eventchannel4 again");
                test_ok=false;
                try
                {
                    p_eventchannel4_2 = new EventChannel(COMPLETE_CHANNEL_4_NAME, rootCtx.in());
                }
                catch (const CORBA::BAD_PARAM& e)
                {
                    TEST_INFO(e);
                    test_ok = true;
                }
                catch(...)
                {
                    std::ostringstream os;
                    os << "Unexpected exception raised" << std::ends;
                    TEST_INFO(os.str());
                    test_ok=false;
                }
                TEST_CHECK(test_ok);
                
                
                TEST_INFO("Get a PushConsumer on eventchannel1");
                test_ok=false;
                try
                {
                    pushConsumer = p_eventchannel1->get_pushConsumer( CosEventComm::PushSupplier::_nil());
                    test_ok=true;
                }
                catch(...)
                {
                    TEST_INFO("Unexpected exception raised!");
                }
                TEST_CHECK(test_ok);
                
                TEST_INFO("Connect local consumer to eventchannel2");
                test_ok=false;
                try
                {
                    p_eventchannel2->connect_pushConsumer(pcImpl->_this());
                    test_ok=true;
                }
                catch(...)
                {
                    TEST_INFO("Unexpected exception raised!");
                }
                TEST_CHECK(test_ok);
                
                TEST_INFO("Connect an other local consumer to eventchannel3");
                test_ok=false;
                try
                {
                    p_eventchannel3->connect_pushConsumer(pcImpl2->_this());
                    test_ok=true;
                }
                catch(...)
                {
                    TEST_INFO("Unexpected exception raised!");
                }
                TEST_CHECK(test_ok);
                
                TEST_INFO("Connect eventchannel3 to eventchannel1");
                test_ok=false;
                try
                {
                    p_eventchannel3->connect_to_channel(COMPLETE_CHANNEL_1_NAME);
                    test_ok=true;
                }
                catch(...)
                {
                    TEST_INFO("Unexpected exception raised!");
                }
                TEST_CHECK(test_ok);
                
                // Get a reference to the POA manager.
                PortableServer::POAManager_var manager = rootPOA->the_POAManager();
                
                manager->activate();
                
                TEST_INFO("Send messages to eventchannel1");
                test_ok=true;
                for (short i=0; i<NB_MSG; i++)
                {
                    CORBA::Any data;
                    std::ostringstream os;
                    
                    os << "Send <" << MSG_ARRAY[i] << "> to eventchannel1" << std::ends;
                    TEST_INFO(os.str());
                    
                    data <<= MSG_ARRAY[i];
                    pushConsumer->push(data);
                }
                
                // wait for received msg
                int timescale = Cdmw::TestUtils::Testable::get_timescale();
                OsSupport::OS::sleep(timescale*2000);
                int loop_index = 0;
                while (pcImpl->m_nbRcvdMsg != NB_MSG)
                {
                    OsSupport::OS::sleep(timescale*2);
                    if (loop_index == 200)
                    {
                        test_ok = false;
                        break;
                    }
                    
                    loop_index++;
                }
                
                OsSupport::OS::sleep(timescale*2000);
                if (test_ok)
                {
                    // message received! Compare them!
                    for (int i=0; i<NB_MSG ; i++)
                    {
                        if (strcmp(MSG_ARRAY[i], pcImpl->m_rcvMsg_array[i].c_str()) != 0)
                            test_ok = false;
                    }
                    
                    for (int i=0; i<NB_MSG ; i++)
                    {
                        if (strcmp(MSG_ARRAY[i], pcImpl2->m_rcvMsg_array[i].c_str()) != 0)
                            test_ok = false;
                    }
                    
                }
                TEST_CHECK(test_ok);
                
    } 
    catch (const CORBA::Exception & e) 
    {
        std::cerr << "ERROR: " << e << std::endl;
    }
}

}; // End namespace Event
}; // End namespace Cdmw


