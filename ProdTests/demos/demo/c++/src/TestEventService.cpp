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

#include "TestEventService.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/testutils/Testable.hpp"

#include "Event/eventinterface/TypedEventChannel.hpp"
#include "Event/eventinterface/EventChannelInterface.hpp"
#include "Event/eventinterface/EventChannel.hpp"
#include "Event/idllib/CdmwEvent.stub.hpp"

#include "Echo.skel.hpp"

namespace
{
    const unsigned short NB_RCVMSG = 2;
    const char* MSG = "Hello world";
    const char* INTERFACE_REF="IDL:TypedEcho:1.0";

    const char* CHANNEL_1_NAME = "EventChannel1";
    const char* TYPED_CHANNEL_1_NAME = "TypedEventChannel1";
    const char* TYPED_CHANNEL_2_NAME = "TypedEventChannel2";
    const char* COMPLETE_CHANNEL_1_NAME = "EVENT/EVENT_CHANNELS/EventChannel1";
    const char* COMPLETE_TYPED_CHANNEL_1_NAME = "EVENT/EVENT_CHANNELS/TypedEventChannel1";
    const char* COMPLETE_TYPED_CHANNEL_2_NAME = "EVENT/EVENT_CHANNELS/TypedEventChannel2";


    const char* CHANNEL_2_NAME = "EventChannel2";
    const char* CHANNEL_3_NAME = "EventChannel3";

    const char* COMPLETE_CHANNEL_2_NAME = "EVENT/EVENT_CHANNELS/EventChannel2";
    const char* COMPLETE_CHANNEL_3_NAME = "EVENT/EVENT_CHANNELS/EventChannel3";


// ----------------------------------------------------------------------
// PushConsumer implementation
// ----------------------------------------------------------------------
class PushConsumer_impl : virtual public POA_CosEventComm::PushConsumer,
                          virtual public PortableServer::RefCountServantBase
{
    CORBA::ORB_var                 m_orb; // The ORB
    PortableServer::POA_var        m_poa; // My POA.

public:
    std::string m_rcvMsg_array[10];
    unsigned short m_nbRcvdMsg;
    
    PushConsumer_impl(CORBA::ORB_ptr orb, 
                      PortableServer::POA_ptr poa)
        : m_orb(CORBA::ORB::_duplicate(orb)),
	       m_poa(PortableServer::POA::_duplicate(poa))
    {
        m_nbRcvdMsg = 0;
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
           std::cout << "Consumer in client has received: " << str << std::endl;
           m_rcvMsg_array[m_nbRcvdMsg] = str;

        }
		  else
        {
			  std::cout << "Consumer in client has an unknown event" << std::endl;
           m_rcvMsg_array[m_nbRcvdMsg] = "Unknown event";
        }
        m_nbRcvdMsg++;
    }

    void
    disconnect_push_consumer()
	    throw(CORBA::SystemException)
    {
        std::cout << "Disconnect" << std::endl;
        PortableServer::ObjectId_var oid = m_poa-> servant_to_id(this);
        m_poa-> deactivate_object(oid.in());
    }

    PortableServer::POA_ptr
    _default_POA()
    {
        return PortableServer::POA::_duplicate(m_poa.in());
    }    
}; // End PushConsumer_impl class

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


TestEventService::TestEventService(CORBA::ORB_ptr orb,
                                   CosNaming::NamingContext_ptr nc)
    throw()
    :Testable("TestEventService"),
     m_orb(CORBA::ORB::_duplicate(orb)),
     m_namingcontext(CosNaming::NamingContext::_duplicate(nc))
{
    // does nothing
}


TestEventService::~TestEventService()
    throw()
{
    // does nothing
}


TestEventService::TestEventService(const TestEventService& rhs)
    throw()
    :Testable("CdmwEventService Test"),
     m_orb(CORBA::ORB::_duplicate(rhs.m_orb)),
     m_namingcontext(CosNaming::NamingContext::_duplicate(rhs.m_namingcontext))

{
    // does nothing
}
		
TestEventService&
TestEventService::operator=(const TestEventService& rhs)
    throw()
{
    if (this != &rhs) 
    {
        m_orb = CORBA::ORB::_duplicate(rhs.m_orb.in());
        m_namingcontext = CosNaming::NamingContext::_duplicate(rhs.m_namingcontext);
    }
    return *this;
}

void TestEventService::do_tests()		
{
    bool test_ok=false;
#if CDMW_ORB_VDR == orbacus

    TypedEcho_var pushConsumer1;
    TypedEcho_var pushConsumer2;
#else

    CosEventComm::PushConsumer_var pushConsumer1;
    CosEventComm::PushConsumer_var pushConsumer2;
#endif

    PortableServer::POA_var rootPOA;
    PushConsumer_impl* pcImpl;
    CORBA::Object_var obj;


    // This tests EventChannel class
    //-------------------------------------------------
    TEST_INFO("Testing Event Service");
    
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
        CosEventChannelAdmin::EventChannel_var channel;

        // Get NamingInterface object from RepositoryInterface
        Cdmw::NamingAndRepository::NamingInterface ni
            = Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface(Cdmw::Common::Locations::CDMW_SERVICES_NAME_DOMAIN);

        OsSupport::OS::sleep(timescale*10000);
        //Get a reference on the EventChannelFactory
        TEST_INFO("Find a factory to create a generic channel");
        test_ok=true;
        obj = ni.resolve("EVENT/FACTORIES/ApplEventChannelFactory");
        CdmwEvent::EventChannelFactory_var factory = 
            CdmwEvent::EventChannelFactory::_narrow(obj.in());
        
        if (CORBA::is_nil(factory.in())) 
        {
            std::cerr << "ApplEventChannelFactory is a nil object reference"
                << std::endl;
            test_ok=false;
            TEST_CHECK(test_ok);
            return;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Creating a new generic channel EventChannel1");
        test_ok=true;
        try
        {
            channel = factory->create_event_channel(CHANNEL_1_NAME);
        }
        catch (CdmwEvent::CreationError& e)
        {
            std::ostringstream os;
            os << "Unexpected CORBA exception raised: " << e << " "
               << e.reason.in() << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        catch (CdmwLifeCycle::AlreadyExistant& e)
        {
            std::ostringstream os;
            os << "Unexpected CORBA exception raised: " << e << " "
               << e.name.in() << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        catch (CdmwLifeCycle::InvalidName& e)
        {
            std::ostringstream os;
            os << "Unexpected CORBA exception raised: " << e << " "
               << e.reason.in() << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        catch (CORBA::Exception& e)
        {
            std::ostringstream os;
            os << "Unexpected CORBA exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        catch (...)
        {
            // test failed!
            TEST_INFO("Unexpected exception raised!");
            test_ok=false;
        }   
        TEST_CHECK(test_ok);
        
        Cdmw::Event::EventChannelInterface* p_eventchannel1;

#if CDMW_ORB_VDR == orbacus

        Cdmw::Event::TypedEventChannel* p_typedeventchannel1;
        Cdmw::Event::TypedEventChannel* p_typedeventchannel2;

#else

        Cdmw::Event::EventChannel* p_eventchannel2;
        Cdmw::Event::EventChannel* p_eventchannel3;

#endif

        TEST_INFO("Get Event Channel references");
        test_ok=true;
        p_eventchannel1 = new Cdmw::Event::EventChannelInterface(channel.in());
        for (int i=0; i<200; i++)
        {
            try
            {

#if CDMW_ORB_VDR == orbacus
              p_typedeventchannel2 = new Cdmw::Event::TypedEventChannel(COMPLETE_TYPED_CHANNEL_2_NAME, m_namingcontext.in());
#else
              p_eventchannel3 = new Cdmw::Event::EventChannel(COMPLETE_CHANNEL_3_NAME, m_namingcontext.in());
#endif
               break;
            }
            catch (const CORBA::BAD_PARAM& e)
            {
                // try again, maybe event channel is still not created!
            }
            OsSupport::OS::sleep(timescale*20);
        }

        if (p_eventchannel1 == NULL)
        {
            TEST_INFO("EventChannel4 not found!");
            test_ok=false;
        }
        try
        {

#if CDMW_ORB_VDR == orbacus
           p_typedeventchannel1 = new Cdmw::Event::TypedEventChannel(COMPLETE_TYPED_CHANNEL_1_NAME, m_namingcontext.in());
#else
           p_eventchannel2 = new Cdmw::Event::EventChannel(COMPLETE_CHANNEL_2_NAME, m_namingcontext.in());
#endif
        }
        catch (const CORBA::BAD_PARAM& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

#if CDMW_ORB_VDR == orbacus    
        TEST_INFO("Get a PushConsumer on TypedEventChannel1");
#else
        TEST_INFO("Get a PushConsumer on EventChannel2");
#endif

        test_ok=false;
        try
        {
#if CDMW_ORB_VDR == orbacus
            pushConsumer1 = p_typedeventchannel1->get_typedPushConsumer<TypedEcho>(INTERFACE_REF, CosEventComm::PushSupplier::_nil());
#else
            pushConsumer1 = p_eventchannel2->get_pushConsumer( CosEventComm::PushSupplier::_nil());
#endif
            test_ok=true;
        }
        catch(...)
        {
            TEST_INFO("Unexpected exception raised!");
        }
        TEST_CHECK(test_ok);
 

#if CDMW_ORB_VDR == orbacus    
        TEST_INFO("Get a PushConsumer on TypedEventChannel2");
#else
        TEST_INFO("Get a PushConsumer on EventChannel3");
#endif

        test_ok=false;
        try
        {
#if CDMW_ORB_VDR == orbacus
           pushConsumer2 = p_typedeventchannel2->get_typedPushConsumer<TypedEcho>(INTERFACE_REF, CosEventComm::PushSupplier::_nil());
#else
           pushConsumer2 = p_eventchannel3->get_pushConsumer( CosEventComm::PushSupplier::_nil());
#endif
            test_ok=true;
        }
        catch(...)
        {
            TEST_INFO("Unexpected exception raised!");
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Connect local consumer to EventChannel1");
        test_ok=false;
        pcImpl = new PushConsumer_impl(m_orb.in(), rootPOA.in());
        try
        {
            p_eventchannel1->connect_pushConsumer(pcImpl->_this());
            test_ok=true;
        }
        catch(...)
        {
            TEST_INFO("Unexpected exception raised!");
        }
        TEST_CHECK(test_ok);
               
        TEST_INFO("Send message to EventChannel1");
        test_ok=true;
        {
            std::ostringstream os;
            
            os << "Send <" << MSG << "> to EventChannel1" << std::ends;
            TEST_INFO(os.str());
#if CDMW_ORB_VDR == orbacus
            
            pushConsumer1->echo_string(MSG);
            pushConsumer2->echo_string(MSG);
#else

	    CORBA::Any data1;
	    data1 <<= MSG;
	    pushConsumer1->push(data1);
	    CORBA::Any data2;
	    data2 <<= MSG;
	    pushConsumer2->push(data2);
#endif

        }
        
        // wait for received msg
        int loop_index = 0;
        while (pcImpl->m_nbRcvdMsg != NB_RCVMSG)
        {
            OsSupport::OS::sleep(timescale*100);
            if (loop_index == 100)
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
#if CDMW_ORB_VDR == orbacus
            p_typedeventchannel1->destroy();
            p_typedeventchannel2->destroy();
#else
	    p_eventchannel2->destroy();
	    p_eventchannel3->destroy();

#endif
            delete p_eventchannel1;
#if CDMW_ORB_VDR == orbacus
            delete p_typedeventchannel1;
            delete p_typedeventchannel2;
#else
            delete p_eventchannel2;
            delete p_eventchannel3;
#endif

            test_ok = true;
        }
        catch (...)
        {
            TEST_INFO("Unexpected exception raised!");
        }
        TEST_CHECK(test_ok);
        
        TEST_INFO("Try to find destroyed EventChannel1!");
        test_ok = false;
        try
        {
#if CDMW_ORB_VDR == orbacus
            p_typedeventchannel1 = new Cdmw::Event::TypedEventChannel(COMPLETE_TYPED_CHANNEL_1_NAME, m_namingcontext.in());
#else
            p_eventchannel2 = new Cdmw::Event::EventChannel(COMPLETE_CHANNEL_2_NAME, m_namingcontext.in());
#endif
        }
        catch (const CORBA::BAD_PARAM& e)
        {
            std::ostringstream os;
            os << e << std::endl;
            TEST_INFO(os.str());
            test_ok = true;
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

