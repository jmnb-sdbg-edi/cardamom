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
#include "test_perf/TestEventChannelContinue.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/testutils/Testable.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "TraceAndPerf/performancelibrary/Probe.hpp"

#include "Event/idllib/CosEventComm.skel.hpp"
#include "test_perf/Echo.stub.hpp"
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
    unsigned short m_nbRcvdMsg;
    unsigned short m_nbRcvdStruct;
    
    PushConsumer_impl(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa)
        : orb_(CORBA::ORB::_duplicate(orb)),
           poa_(PortableServer::POA::_duplicate(poa))
    {
        id_ = nb_consumer;
          m_nbRcvdMsg = 0;
          m_nbRcvdStruct = 0;
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
          const TNNStruct* tnn_struct;
         
          if (data >>= str)
        {
              m_nbRcvdMsg++;
        }
          else if (data >>= tnn_struct)
          {
              m_nbRcvdStruct++;
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
    const char* CHANNEL_5_NAME = "TestEventChannel5";
    const char* COMPLETE_CHANNEL_5_NAME = "CDMW/SERVICES/EVENT/EVENT_CHANNELS/TestEventChannel5";


/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/


TestEventChannelContinue::TestEventChannelContinue(CORBA::ORB_ptr orb)
    throw()
    :Testable("Cdmw::Event::EventChannelContinue"),
     m_orb(CORBA::ORB::_duplicate(orb))
{
    // does nothing
}


TestEventChannelContinue::~TestEventChannelContinue()
    throw()
{
    // does nothing
}


TestEventChannelContinue::TestEventChannelContinue(const TestEventChannelContinue& rhs)
    throw()
    :Testable("Cdmw::Event::EventChannelContinue"),
     m_orb(CORBA::ORB::_duplicate(rhs.m_orb))
{
    // does nothing
}
        
TestEventChannelContinue&
TestEventChannelContinue::operator=(const TestEventChannelContinue& rhs)
    throw()
{
    if (this != &rhs) {
        m_orb = CORBA::ORB::_duplicate(rhs.m_orb.in());
    }
    return *this;
}

void TestEventChannelContinue::do_tests()        
{

    // set number of requested successfull tests
    set_nbOfRequestedTestOK (7);

    bool test_ok=false;
    CosEventComm::PushConsumer_var pushConsumer;
    PortableServer::POA_var rootPOA;

    // This tests EventChannel class
    //-------------------------------------------------
    TEST_INFO("Testing EventChannel Continue");
    
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
        
        EventChannel* p_eventchannel5 = NULL;

        
        TEST_INFO("Initialise an event channel with a valid channel name");
        test_ok=true;
        try
        {
            p_eventchannel5 = new EventChannel(COMPLETE_CHANNEL_5_NAME, rootCtx.in());
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Get a PushConsumer on eventchannel5");
        test_ok=false;
        try
        {
            pushConsumer = p_eventchannel5->get_pushConsumer( CosEventComm::PushSupplier::_nil());
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
            p_eventchannel5->connect_pushConsumer(pcImpl->_this());
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

        TEST_INFO("Send messages to eventchannel5");
        test_ok=true;
          int nb_msg=370;
          int nb_bytes = 20000;
          std::string test_msg(nb_bytes, 'A');

        std::ostringstream os;
        os << "Send " << nb_msg << " messages of " << nb_bytes <<  " to eventchannel5" << std::ends;
        TEST_INFO(os.str());

        for (short i=0; i<nb_msg; i++)
        {
                Performance::Probe probe("GENERIC EVENT sending in Continue");

            CORBA::Any data;
            data <<= test_msg.c_str();
            pushConsumer->push(data);
          }
          
        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        OsSupport::OS::sleep(timescale*1000);
        // wait for received msg
        int loop_index = 0;
         while (pcImpl->m_nbRcvdMsg < nb_msg)
        {
                OsSupport::OS::sleep(timescale*1);
                if (loop_index == 5000)
                {
                    test_ok = false;
                    break;
                }
    
                    Performance::Probe probe("GENERIC EVENT receiving in Continue");
                loop_index++;
        }
          std::ostringstream os2;
        os2 << "Received " << pcImpl->m_nbRcvdMsg <<  " messages" << std::ends;
        TEST_INFO(os2.str());
        TEST_CHECK(test_ok);

          std::ostringstream os5;
        os5 << "Send " << nb_msg << " struct of " << nb_bytes <<  " to eventchannel5" << std::ends;
        TEST_INFO(os5.str());
        test_ok=true;
          TNNStruct test_struct;
          test_struct.along = 10000;
          test_struct.ashort = 2000;
          for (int k=0; k < 6; k++)
          {
              test_struct.char_array[k] = 'B';
          }
          nb_msg=600;

        for (short i=0; i<nb_msg; i++)
        {
                Performance::Probe probe("GENERIC EVENT Struct sending in Continue");

            CORBA::Any data;
            data <<= test_struct;
            pushConsumer->push(data);
          }
          
        OsSupport::OS::sleep(timescale*1000);
        // wait for received msg
        loop_index = 0;
         while (pcImpl->m_nbRcvdStruct < nb_msg)
        {
                OsSupport::OS::sleep(timescale*1);
                if (loop_index == 5000)
                {
                    test_ok = false;
                    break;
                }
    
                    Performance::Probe probe("GENERIC EVENT Struct receiving in Continue");
                loop_index++;
        }
        std::ostringstream os6;
        os6 << "Received " << pcImpl->m_nbRcvdMsg <<  " messages" << std::ends;
        TEST_INFO(os6.str());
        TEST_CHECK(test_ok);

        TEST_INFO("Connect an other local consumer to eventchannel2");
        test_ok=false;
        try
        {
            p_eventchannel5->connect_pushConsumer(pcImpl2->_this());
            test_ok=true;
        }
        catch(...)
        {
            TEST_INFO("Unexpected exception raised!");
        }
        TEST_CHECK(test_ok);

          nb_msg=450;
        std::ostringstream os3;
        os3 << "Send " << nb_msg << " messages of " << nb_bytes <<  " to eventchannel5" << std::ends;
        TEST_INFO(os3.str());
        test_ok=true;
        for (short i=0; i<nb_msg; i++)
        {
               Performance::Probe probe("GENERIC EVENT sending in Continue with 2 consumers");

            CORBA::Any data;
            data <<= test_msg.c_str();
            pushConsumer->push(data);
          }
          
         OsSupport::OS::sleep(timescale*1000);
        // wait for received msg
        loop_index = 0;
         while (pcImpl->m_nbRcvdMsg < nb_msg)
        {
                OsSupport::OS::sleep(timescale*1);
                if (loop_index == 5000)
                {
                    test_ok = false;
                    break;
                }
    
                    Performance::Probe probe("GENERIC EVENT receiving in Continue with two consumers");
                loop_index++;
        }
        std::ostringstream os4;
        os4 << "Received " << pcImpl2->m_nbRcvdMsg <<  " messages" << std::ends;
        TEST_INFO(os4.str());
        TEST_CHECK(test_ok);
    } 
    catch (const CORBA::Exception & e) 
    {
        std::cerr << "ERROR: " << e << std::endl;
    }
}
        
}; // End namespace Event
}; // End namespace Cdmw


