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
#include "test_perf/TestEventChannel.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/testutils/Testable.hpp"
#include "TraceAndPerf/performancelibrary/Probe.hpp"

#include "Event/idllib/CosEventComm.skel.hpp"
#include "Event/eventinterface/EventChannel.hpp"

namespace
{
    const char TEST_MSG[20] = "Hello world, hello!";


class PushConsumer_impl : virtual public POA_CosEventComm::PushConsumer,
                          virtual public PortableServer::RefCountServantBase
{
    CORBA::ORB_var orb_; // The ORB
    PortableServer::POA_var poa_; // My POA.
    static int nb_consumer;  // nuber of consumer
    int id_;  // consumer id

public:
     bool is_msg_received;
    
    PushConsumer_impl(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa)
        : orb_(CORBA::ORB::_duplicate(orb)),
           poa_(PortableServer::POA::_duplicate(poa))
    {
        id_ = nb_consumer;
         is_msg_received = false;
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
            is_msg_received = true;
        }
          else
        {
              std::cout << "Consumer" << id_ << " has an unknown event" << std::endl;
        }
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
     m_orb(CORBA::ORB::_duplicate(rhs.m_orb))
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
    set_nbOfRequestedTestOK (6);

    bool test_ok=false;
    CosEventComm::PushConsumer_var pushConsumer;
    PortableServer::POA_var rootPOA;

    // This tests EventChannel class
    //-------------------------------------------------
    TEST_INFO("Testing EventChannel");
    
    // Create the implementation object.
    // Resolve Root POA.
     CORBA::Object_var poa_obj = m_orb -> resolve_initial_references("RootPOA");
    rootPOA = PortableServer::POA::_narrow(poa_obj.in());
    
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

        TEST_INFO("Get a PushConsumer on eventchannel1");
        test_ok=false;
        try
        {
            pushConsumer = p_eventchannel1->get_pushConsumer( CosEventComm::PushSupplier::_nil());
            test_ok=true;
        }
        catch(...)
        {
            TEST_INFO("Unexpected exception raise!");
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

        // Get a reference to the POA manager.
          PortableServer::POAManager_var manager = rootPOA->the_POAManager();

        manager->activate();

        TEST_INFO("Send messages to eventchannel1");
        test_ok=true;
          int nb_msg=200;
          int nb_bytes = 200;
          std::string test_msg(nb_bytes, 'A');

        std::ostringstream os;
        //os << "Send <" << TEST_MSG << "> to eventchannel1" << std::ends;
        os << "Send message of " << nb_bytes <<  " to eventchannel1" << std::ends;
        TEST_INFO(os.str());
        for (short i=0; i<nb_msg; i++)
        {
                Performance::Probe probe("GENERIC EVENT");

            CORBA::Any data;
            data <<= test_msg.c_str();
            pushConsumer->push(data);

            // wait for received msg
            int loop_index = 0;
             while (!pcImpl->is_msg_received)
            {
                //OsSupport::OS::sleep(1);
                //if (loop_index == 500)
                //{
                //    test_ok = false;
                //    break;
                //}
    
                loop_index++;
            }
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Connect an other local consumer to eventchannel1");
        test_ok=false;
        try
        {
            p_eventchannel1->connect_pushConsumer(pcImpl2->_this());
            test_ok=true;
        }
        catch(...)
        {
            TEST_INFO("Unexpected exception raised!");
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Connect an other local consumer to eventchannel2");
        test_ok=false;
        try
        {
            p_eventchannel2->connect_pushConsumer(pcImpl2->_this());
            test_ok=true;
        }
        catch(...)
        {
            TEST_INFO("Unexpected exception raised!");
        }
        TEST_CHECK(test_ok);

          TEST_INFO("Send messages to eventchannel1");
        test_ok=true;
        std::ostringstream os1;
        os1 << "Send message of " << nb_bytes <<  " to eventchannel1" << std::ends;
        TEST_INFO(os1.str());
        for (short i=0; i<nb_msg; i++)
        {
               Performance::Probe probe("GENERIC EVENT with 2 consumers");

            CORBA::Any data;
            data <<= test_msg.c_str();
            pushConsumer->push(data);

            // wait for received msg
            int loop_index = 0;
             while (!pcImpl->is_msg_received)
            {
                //OsSupport::OS::sleep(1);
                //if (loop_index == 500)
                //{
                //    test_ok = false;
                //    break;
                //}
    
                loop_index++;
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


