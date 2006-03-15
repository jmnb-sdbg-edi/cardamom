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


#if CDMW_ORB_VDR == orbacus        

#include <iostream>
#include <string>
#include <sstream>
#include "test_interface/TestTypedEventChannel.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/testutils/Testable.hpp"

#include "Event/idllib/CosTypedEventComm.skel.hpp"
#include "Event/eventinterface/TypedEventChannel.hpp"

#include <Event/idllib/Echo_skel.h>

namespace
{
    const unsigned short NB_MSG = 2;
    const unsigned short NB_RCVMSG = 10;
    const char* MSG_ARRAY[2] = {"Hello world 1", "Hello world 2"};


    using namespace test;
	    
// ----------------------------------------------------------------------
// Echo_impl implementation
// ----------------------------------------------------------------------
class InvalidConsumer_impl : virtual public POA_Echo,
                             virtual public PortableServer::RefCountServantBase
{
    CORBA::ORB_var orb_;          // The ORB
    PortableServer::POA_var poa_; // My POA.
public:
    std::string m_rcvMsg_array[10];
    unsigned short m_nbRcvdMsg;
    
    InvalidConsumer_impl(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa)
        : orb_(CORBA::ORB::_duplicate(orb)),
           poa_(PortableServer::POA::_duplicate(poa))
    {
    }
    
    //
    // IDL to C++ mapping
    //
    void
    echo_string(const char* str)
    throw(CORBA::SystemException)
    {
        std::cout << "Invalid Consumer has received: " << str << std::endl;
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
        return PortableServer::POA::_duplicate(poa_);
    }    
}; // End class Echo_impl

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
    std::string m_rcvMsg_array[10];
    unsigned short m_nbRcvdMsg;
    
    Echo_impl(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa)
        : orb_(CORBA::ORB::_duplicate(orb)),
           poa_(PortableServer::POA::_duplicate(poa))
    {
        m_nbRcvdMsg = 0;
        nb_consumer++;
        id_ = nb_consumer;
    }
    
    //
    // IDL to C++ mapping
    //
    void
    echo_string(const char* str)
    throw(CORBA::SystemException)
    {
        std::cout << "Consumer" << id_ << " has received: " << str << std::endl;
        m_rcvMsg_array[m_nbRcvdMsg] = str;
        m_nbRcvdMsg++;
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
        return PortableServer::POA::_duplicate(poa_);
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
     m_orb(CORBA::ORB::_duplicate(rhs.m_orb))
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
    set_nbOfRequestedTestOK (12);
    
    bool test_ok=false;
    TypedEcho_var pushConsumer;
    PortableServer::POA_var rootPOA;


    // This tests TypedEventChannel class
    //-------------------------------------------------
    TEST_INFO("Testing TypedEventChannel");

    // Create the implementation object.
    // Resolve Root POA.
    rootPOA = PortableServer::POA::_narrow(CORBA::Object_var(
        m_orb -> resolve_initial_references("RootPOA")));
    
    if (CORBA::is_nil(rootPOA))
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



    Echo_impl* pcImpl = new Echo_impl(m_orb, rootPOA);
    Echo_impl* pcImpl2 = new Echo_impl(m_orb, rootPOA);


    try 
    {        
        TypedEventChannel* eventchannel1;
        TypedEventChannel* eventchannel2;
        TypedEventChannel* eventchannel3;
        TypedEventChannel* eventchannel4;
        TypedEventChannel* eventchannel4_2;

        CORBA::Object_var obj;
        TEST_INFO("Initialise an event channel with an invalid channel name");
        test_ok=false;
        try
        {
            eventchannel1 = new TypedEventChannel(INV_TYPED_CHANNEL_NAME, rootCtx);
        }
        catch (const CORBA::BAD_PARAM& e)
        {
            TEST_INFO(e);
            test_ok=true;
        }
        catch(...)
        {
            TEST_INFO("Unexpected exception raised!");
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Initialise an event channel with an uncreated channel name");
        test_ok=false;
        try
        {
            eventchannel1= new TypedEventChannel(UNCREATED_TYPED_CHANNEL_NAME, rootCtx);
        }
        catch (const CORBA::BAD_PARAM& e)
        {
            TEST_INFO(e);
            test_ok=true;
        }
        catch(...)
        {
            TEST_INFO("Unexpected exception raised!");
        }
        TEST_CHECK(test_ok);
        
        TEST_INFO("Initialise an event channel with a valid channel name");
        test_ok=true;
        try
        {
            eventchannel1 = new TypedEventChannel(COMPLETE_TYPED_CHANNEL_1_NAME, rootCtx);
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
            eventchannel2 = new TypedEventChannel(COMPLETE_TYPED_CHANNEL_2_NAME, rootCtx);
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
            eventchannel3 = new TypedEventChannel(COMPLETE_TYPED_CHANNEL_3_NAME, rootCtx);
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
            eventchannel4 = new TypedEventChannel(COMPLETE_TYPED_CHANNEL_4_NAME, rootCtx);
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
            eventchannel4->destroy();
            test_ok=true;
        }
        catch(...)
        {
            TEST_INFO("Unexpected exception raise!");
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Try to get interface on eventchannel4 again");
        test_ok=false;
        try
        {
            eventchannel4_2 = new TypedEventChannel(COMPLETE_TYPED_CHANNEL_4_NAME, rootCtx);
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

        TEST_INFO("Get a TypedPushConsumer on eventchannel1 with an invalid interface ref");
        test_ok=false;
        try
        {
            obj = 
                eventchannel1->get_typedPushConsumer<Echo>(INVALID_INTERFACE_REF,
                                                    CosEventComm::PushSupplier::_nil());
        }
        catch(const CosTypedEventChannelAdmin::InterfaceNotSupported& e)
        {
            std::ostringstream os;
            os << "Expected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=true;
        }
        catch(...)
        {
            TEST_INFO("Unexpected exception raised!");
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Get a TypedPushConsumer on eventchannel1");
        test_ok=false;
        try
        {
            pushConsumer = 
                eventchannel1->get_typedPushConsumer<TypedEcho>(INTERFACE_REF,
                                                    CosEventComm::PushSupplier::_nil());

            if (CORBA::is_nil(pushConsumer))
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

        TEST_INFO("Connect local consumer to eventchannel2 with an invalid interface");
        test_ok=false;
        try
        {
            eventchannel2->connect_typedPushConsumer(INVALID_INTERFACE_REF, 
                                                     pcImpl->_this());
        }
        catch(CosTypedEventChannelAdmin::NoSuchImplementation& e)
        {
            std::ostringstream os;
            os << "Expected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
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

        TEST_INFO("Connect an other local consumer to eventchannel3");
        test_ok=false;
        try
        {
            eventchannel3->connect_typedPushConsumer(INTERFACE_REF, pcImpl2->_this());
            test_ok=true;
        }
        catch(const CORBA::Exception& e)
        {
            std::ostringstream os;
            os << "Unexpected  exception raised: " << e << std::ends;
            TEST_INFO(os.str());
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Connect eventchannel3 to eventchannel1");
        test_ok=false;
        try
        {
            eventchannel3->connect_to_channel(INTERFACE_REF, COMPLETE_TYPED_CHANNEL_1_NAME);
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
        for (short i=0; i<NB_MSG; i++)
        {
            std::ostringstream os;

            os << "Send <" << MSG_ARRAY[i] << "> to eventchannel1" << std::ends;
            TEST_INFO(os.str());

            try
            {
               pushConsumer->echo_string(CORBA::string_dup(MSG_ARRAY[i]));
            }
            catch (...)
            {
                test_ok = false;
            }
        }

        // wait for received msg
        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        OsSupport::OS::sleep(timescale*2000);
        int loop_index = 0;
        while (pcImpl->m_nbRcvdMsg != NB_MSG)
        {
            OsSupport::OS::sleep(timescale*200);
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

#endif // orbaccus


