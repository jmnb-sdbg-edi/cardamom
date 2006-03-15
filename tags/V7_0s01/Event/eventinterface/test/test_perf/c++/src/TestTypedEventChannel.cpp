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
#include "test_perf/TestTypedEventChannel.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "TraceAndPerf/performancelibrary/Probe.hpp"
#include "Foundation/testutils/Testable.hpp"

#include "Event/idllib/CosTypedEventComm.skel.hpp"
#include "Event/eventinterface/TypedEventChannel.hpp"

#include "test_perf/Echo.skel.hpp"

namespace
{
    const char TEST_MSG[20] = "Hello world, hello!";


// ----------------------------------------------------------------------
// Echo_impl implementation
// ----------------------------------------------------------------------
class Echo_impl : virtual public POA_TypedEcho,
                  virtual public PortableServer::RefCountServantBase
{
    CORBA::ORB_var orb_; // The ORB
    PortableServer::POA_var poa_; // My POA.
    static int nb_consumer;  // number of consumer
    int id_;  // consumer id

public:
    bool is_msg_received;
    
    Echo_impl(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa)
        : orb_(CORBA::ORB::_duplicate(orb)),
           poa_(PortableServer::POA::_duplicate(poa))
    {
        id_ = nb_consumer;
          is_msg_received = false;
    }
    
    //
    // IDL to C++ mapping
    //
    void
    echo_string(const char* str)
    throw(CORBA::SystemException)
    {
          is_msg_received = true;
    }

    void
    disconnect_push_consumer()
    throw(CORBA::SystemException)
    {
        std::cout << "Disconnect";
        PortableServer::ObjectId_var oid = poa_ -> servant_to_id(this);
        poa_ -> deactivate_object(oid.in());

         orb_ -> shutdown(false);
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
        return PortableServer::POA::_duplicate(poa_.in());
    }    
}; // End class Echo_impl

int Echo_impl::nb_consumer = 1; // (consumer1 is in the main (different code))


}; // End anonymous namespace

namespace Cdmw
{
namespace Event
{


    const char* TYPED_CHANNEL_1_NAME = "TestTypedEventChannel1";
    const char* TYPED_CHANNEL_2_NAME = "TestTypedEventChannel2";
    const char* TYPED_CHANNEL_3_NAME = "TestTypedEventChannel3";
    const char* TYPED_CHANNEL_4_NAME = "TestTypedEventChannel4";
    const char* COMPLETE_TYPED_CHANNEL_1_NAME = "CDMW/SERVICES/EVENT/EVENT_CHANNELS/TestTypedEventChannel1";
    const char* COMPLETE_TYPED_CHANNEL_2_NAME = "CDMW/SERVICES/EVENT/EVENT_CHANNELS/TestTypedEventChannel2";
    const char* COMPLETE_TYPED_CHANNEL_3_NAME = "CDMW/SERVICES/EVENT/EVENT_CHANNELS/TestTypedEventChannel3";
    const char* COMPLETE_TYPED_CHANNEL_4_NAME = "CDMW/SERVICES/EVENT/EVENT_CHANNELS/TestTypedEventChannel4";
     const char* UNCREATED_TYPED_CHANNEL_NAME = "CDMW/SERVICES/EVENT/EVENT_CHANNELS/TestTypedEventChannel6";
     const char* INV_TYPED_CHANNEL_NAME = "Test/TypedEventChannel4";

    const CORBA::String_var INTERFACE_REF=CORBA::string_dup("IDL:TypedEcho:1.0");
    const CORBA::String_var INVALID_INTERFACE_REF=CORBA::string_dup("IDL:InvalidInterface:1.0");

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/


TestTypedEventChannel::TestTypedEventChannel(CORBA::ORB_ptr orb)
    throw()
    :Testable("Cdmw::Event::TypedEventChannel"),
     m_orb(CORBA::ORB::_duplicate(orb))
{
    // does nothing
}


TestTypedEventChannel::~TestTypedEventChannel()
    throw()
{
    // does nothing
}


TestTypedEventChannel::TestTypedEventChannel(const TestTypedEventChannel& rhs)
    throw()
    :Testable("Cdmw::Event::TypedEventChannel"),
     m_orb(CORBA::ORB::_duplicate(rhs.m_orb.in()))
{
    // does nothing
}
        
TestTypedEventChannel&
TestTypedEventChannel::operator=(const TestTypedEventChannel& rhs)
    throw()
{
    if (this != &rhs) {
        m_orb = CORBA::ORB::_duplicate(rhs.m_orb.in());
    }
    return *this;
}

void TestTypedEventChannel::do_tests()        
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (7);

    bool test_ok=false;
    TypedEcho_var pushConsumer;
    PortableServer::POA_var rootPOA;


    // This tests TypedEventChannel class
    //-------------------------------------------------
    TEST_INFO("Testing TypedEventChannel");

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



    Echo_impl* pcImpl = new Echo_impl(m_orb.in(), rootPOA.in());
    Echo_impl* pcImpl2 = new Echo_impl(m_orb.in(), rootPOA.in());


    try 
    {        
        TypedEventChannel* eventchannel1 = NULL;
        TypedEventChannel* eventchannel2 = NULL;

        CORBA::Object_var obj;
        TEST_INFO("Initialise event channel");
        test_ok=true;
        try
        {
            eventchannel1 = new TypedEventChannel(COMPLETE_TYPED_CHANNEL_1_NAME, rootCtx.in());
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
            eventchannel2 = new TypedEventChannel(COMPLETE_TYPED_CHANNEL_2_NAME, rootCtx.in());
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Get a TypedPushConsumer on eventchannel1");
        test_ok=false;
        try
        {
            pushConsumer = 
                eventchannel1->get_typedPushConsumer<TypedEcho>(INTERFACE_REF,
                                                    CosEventComm::PushSupplier::_nil());

            if (CORBA::is_nil(pushConsumer.in()))
            {
                TEST_INFO("Typed push consumer is nil!");
            }
            else
                test_ok=true;
        }
        catch(const CORBA::Exception& e)
        {
            std::ostringstream os;
            os << "Unexpected  exception raised: " << e << std::ends;
            TEST_INFO(os.str());
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Connect local consumer to eventchannel2");
        test_ok=false;
        try
        {
            eventchannel2->connect_typedPushConsumer(INTERFACE_REF, pcImpl->_this());
            test_ok=true;
        }
        catch(const CORBA::Exception& e)
        {
            std::ostringstream os;
            os << "Unexpected  exception raised: " << e << std::ends;
            TEST_INFO(os.str());
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

        std::ostringstream os1;
          os1 << "Send message of " << nb_bytes <<  " to eventchannel1" << std::ends;
        TEST_INFO(os1.str());
        for (short i=0; i<nb_msg; i++)
        {

                Performance::Probe probe("TYPED EVENT");
                
            try
            {
               pushConsumer->echo_string(CORBA::string_dup(test_msg.c_str()));
            }
            catch (...)
            {
                test_ok = false;
            }

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
            eventchannel1->connect_typedPushConsumer(INTERFACE_REF, pcImpl2->_this());
            test_ok=true;
        }
        catch(const CORBA::Exception& e)
        {
            std::ostringstream os;
            os << "Unexpected  exception raised: " << e << std::ends;
            TEST_INFO(os.str());
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Connect an other local consumer to eventchannel2");
        test_ok=false;
        try
        {
            eventchannel2->connect_typedPushConsumer(INTERFACE_REF, pcImpl2->_this());
            test_ok=true;
        }
        catch(const CORBA::Exception& e)
        {
            std::ostringstream os;
            os << "Unexpected  exception raised: " << e << std::ends;
            TEST_INFO(os.str());
        }
        TEST_CHECK(test_ok);

          TEST_INFO("Send messages to eventchannel1 with 2 consumers");
        test_ok=true;
        std::ostringstream os2;
          os2 << "Send message of " << nb_bytes <<  " to eventchannel1" << std::ends;
        TEST_INFO(os2.str());
        for (short i=0; i<nb_msg; i++)
        {
                Performance::Probe probe("TYPED EVENT with 2 consumers");
                
            try
            {
               pushConsumer->echo_string(CORBA::string_dup(test_msg.c_str()));
            }
            catch (...)
            {
                test_ok = false;
            }

            // wait for received msg
            int loop_index = 0;
            while (!pcImpl->is_msg_received)
            {
                //OsSupport::OS::sleep(1);
                //if (loop_index == 500)
               // {
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


