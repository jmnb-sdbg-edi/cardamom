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

// FIXME: move tests of Codec to OrbSupport !
 
 
// Std include files
#include <iostream>
#include <sstream>
#include <string.h>

#include "testtransengine/TestTransEngine.hpp"
#include <FaultTolerance/ftstatemanager/DataStoreContext.hpp>
#include <FaultTolerance/ftstatemanager/TXDataStore.hpp>
#include "Foundation/commonsvcs/datastore/StorageHome.hpp"
#include "Foundation/commonsvcs/datastore/StorageObject.hpp"
#include "ftstatemanager/Timer.hpp"
#include "ftstatemanager/DataStore.hpp"

#include "idllib/CdmwFTManager.skel.hpp"
    
DEFINE_VARIABLE_LENGTH_DATA_TRAITS(::FT::Properties);
DEFINE_VARIABLE_LENGTH_DATA_TRAITS(::CdmwFT::FTManagerObjects);
    
    
static int count = 0;

class Time_Handler : public  Cdmw::FT::Timer_Event_Handler<CdmwFT::Transaction::TransactionIdentification>
{ 
public:

    //Meth²d which is called back by the Reactor when timeout occurs.
    int handle_timeout (const CdmwFT::Transaction::TransactionIdentification* tid)
    {
        id = tid->id;
        std::cout<<"Timer "<<id<<std::endl;
        count ++;
        return 0; 
    }
    int get_current_id()
    {
        return id;
    }
    
protected:
    int id;
    
}; 

TestTransEngine::TestTransEngine (CORBA::ORB_ptr orb, PortableServer::POA_ptr rootPOA)
	:Cdmw::TestUtils::Testable("Test Trans Engine")
{
    m_orb = CORBA::ORB::_duplicate(orb);
    m_rootPOA = PortableServer::POA::_duplicate(rootPOA);
}

TestTransEngine::~TestTransEngine ()
{
}


void 
TestTransEngine::do_tests()
{
    set_nbOfRequestedTestOK(32);
    
    
    {
        TEST_INFO("Create a timer");
        Cdmw::FT::Timer<CdmwFT::Transaction::TransactionIdentification> timer;
        //timer.open();
        timer.start();
        TEST_INFO("Create a handler");
        Time_Handler *th1=new Time_Handler; 
        Time_Handler *th2=new Time_Handler; 
        Time_Handler *th3=new Time_Handler; 
        Time_Handler *th4=new Time_Handler; 
        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        Cdmw::OsSupport::OS::sleep( timescale * 2000 );
        TEST_INFO("start_timer");
        Cdmw::OsSupport::OS::Timeval time;
        time.seconds=1;
        time.microseconds=0;
        CdmwFT::Transaction::TransactionIdentification* id1 = new CdmwFT::Transaction::TransactionIdentification();
        id1->id=1;
        id1->initiator="initiator";
        timer.start_timer (th1, id1, time);


        Cdmw::OsSupport::OS::sleep( timescale * 2000 );
        TEST_CHECK(th1->get_current_id() == 1);

        time.seconds=6;
        CdmwFT::Transaction::TransactionIdentification* id2 = new CdmwFT::Transaction::TransactionIdentification();
        id2->initiator="initiator";
        id2->id=2;
        timer.start_timer (th2, id2, time);

        time.seconds=10;
        CdmwFT::Transaction::TransactionIdentification* id3 = new CdmwFT::Transaction::TransactionIdentification();
        id3->initiator="initiator";
        id3->id=3;
        int timer_id = timer.start_timer (th3, id3, time);
        
        time.seconds=14;
        CdmwFT::Transaction::TransactionIdentification* id4 = new CdmwFT::Transaction::TransactionIdentification();
        id4->initiator="initiator";
        id4->id=4;
        timer.start_timer (th4, id4, time);

        Cdmw::OsSupport::OS::sleep( 7000 );
        TEST_CHECK(th2->get_current_id() == 2);

        std::cout<<"cancel:"<<timer.cancel_timer (timer_id)<<std::endl;
        
        Cdmw::OsSupport::OS::sleep( 1000 );
        std::cout<<count<<std::endl;
        TEST_CHECK(count == 2);

        Cdmw::OsSupport::OS::sleep( 10000 );
        TEST_CHECK(th4->get_current_id() == 4);
        TEST_INFO("Stop the timer");
        timer.stop();
        Cdmw::OsSupport::OS::sleep( 1000 );
    }
    
    try 
    {         
        {
            TEST_INFO("Create a DataStore");
            typedef Cdmw::FT::DataStore<std::string, CORBA::Boolean> TestDataStore;
            Cdmw::OsSupport::OS::Timeval cohort_timeout;
            cohort_timeout.seconds=2;
            cohort_timeout.microseconds=0;
            Cdmw::OsSupport::OS::Timeval coordinator_timeout;
            coordinator_timeout.seconds=2;
            coordinator_timeout.microseconds=0;
            std::ostringstream multicast_domain;
            multicast_domain << Cdmw::OsSupport::OS::get_logname(); 
            multicast_domain << "ftdomain" << std::ends;
            Cdmw::FT::DataStoreContext * context = new Cdmw::FT::DataStoreContext(m_orb.in(),
                                                                                  m_rootPOA.in(),
                                                                                  "5555",
                                                                                  multicast_domain.str().c_str(),
                                                                                  "225.1.1.8",
                                                                                  100, 100,
                                                                                  cohort_timeout,
                                                                                  coordinator_timeout,
                                                                                  1);
            
            TestDataStore* myDataStore = new TestDataStore(10,
                                                           m_orb.in(),
                                                           m_rootPOA.in(),
                                                           *context);


            Cdmw::OsSupport::OS::sleep( 2000 );
            TEST_SUCCEED();



            myDataStore->activate();
            TEST_SUCCEED();

            myDataStore->on_failure("hello");
            TEST_SUCCEED();

            myDataStore->deactivate();
            TEST_SUCCEED();
            
            myDataStore->on_failure("hello");
            TEST_SUCCEED();

            myDataStore->activate();
            TEST_SUCCEED();

            myDataStore->on_failure("hello");
            TEST_SUCCEED();

            myDataStore->insert_local("hello", true);
            
            myDataStore->update_local("hello", true);

            TEST_INFO("Create a Codec");
            typedef Cdmw::OrbSupport::Codec<std::string> TestCodec;

            TestCodec* codec = new TestCodec(m_orb.in());
            std::string text("hello");

            CORBA::OctetSeq_var seq = codec->encode(text);
            TEST_SUCCEED();
            
            std::string text_res;
            codec->decode(seq.in(), text_res);
            
            TEST_SUCCEED();
            TEST_CHECK((strcmp(text.c_str(), text_res.c_str()) == 0));
        }
        {
            TEST_INFO("Create a Codec");
            typedef Cdmw::OrbSupport::Codec< ::FT::Properties > TestCodec;
            TestCodec* codec = new TestCodec(m_orb.in());
            ::FT::Properties prop;
            
            prop.length(3);
            prop[0].nam.length(1);
            prop[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
            prop[0].val <<= (CORBA::UShort)2;
            
            prop[1].nam.length(1);
            prop[1].nam[0].id="org.omg.ft.ReplicationStyle";
            prop[1].val <<= (CORBA::UShort)2;
            
            prop[2].nam.length(1);
            prop[2].nam[0].id="org.omg.ft.MembershipStyle";
            prop[2].val <<= (CORBA::UShort)0;
            
            
            CORBA::OctetSeq_var seq = codec->encode(prop);
            TEST_SUCCEED();
            
            ::FT::Properties prop_res;
            codec->decode(seq.in(), prop_res);
            
            TEST_SUCCEED();
            TEST_CHECK((strcmp(prop_res[0].nam[0].id, prop[0].nam[0].id) == 0));
            TEST_CHECK((strcmp(prop_res[1].nam[0].id, prop[1].nam[0].id) == 0));
            TEST_CHECK((strcmp(prop_res[2].nam[0].id, prop[2].nam[0].id) == 0));

            CORBA::UShort val1, val2, val3;
            prop_res[0].val >>= val1;
            prop_res[1].val >>= val2;
            prop_res[2].val >>= val3;
            
            TEST_CHECK(val1==2);
            TEST_CHECK(val2==2);
            TEST_CHECK(val3==0);
        } 
        {
            TEST_INFO("Create a Codec");
            typedef Cdmw::OrbSupport::Codec< ::FT::ObjectGroupId > TestCodec;
            TestCodec* codec = new TestCodec(m_orb.in());
            ::FT::ObjectGroupId id = 5;
            
            CORBA::OctetSeq_var seq = codec->encode(id);
            TEST_SUCCEED();
            
            ::FT::ObjectGroupId id_res;
            codec->decode(seq.in(), id_res);
            
            TEST_SUCCEED();;
            TEST_CHECK(id == id_res);
        } 

        {
            TEST_INFO("Create a Codec");
            typedef Cdmw::OrbSupport::Codec< CORBA::Boolean > TestCodec;
            TestCodec* codec = new TestCodec(m_orb.in());
            CORBA::Boolean b = true;
            
            CORBA::OctetSeq_var seq = codec->encode(b);
            TEST_SUCCEED();
            
            CORBA::Boolean b_res;
            codec->decode(seq.in(), b_res);
            
            TEST_SUCCEED();
            TEST_CHECK(b_res);
        } 

        {
            TEST_INFO("Create a Codec");
            typedef Cdmw::OrbSupport::Codec< CosNotifyComm::StructuredPushConsumer > TestCodec;
            TestCodec* codec = new TestCodec(m_orb.in());
            CosNotifyComm::StructuredPushConsumer_var consumer;
            
            CORBA::OctetSeq_var seq = codec->encode(consumer.in());
            TEST_SUCCEED();
            
            CosNotifyComm::StructuredPushConsumer_var consumer2;
            codec->decode(seq.in(), consumer2.inout());
            
            TEST_SUCCEED();
        } 

        {
            TEST_INFO("Create a Codec");
            typedef Cdmw::OrbSupport::Codec< ::CdmwFT::FTManagerObjects > TestCodec;
            TestCodec* codec = new TestCodec(m_orb.in());
            ::CdmwFT::FTManagerObjects objects;
            ::CdmwFT::FTManagerObjects objects2;
            
            CORBA::OctetSeq_var seq = codec->encode(objects);
            TEST_SUCCEED();
            
            codec->decode(seq.in(), objects2);
            
            TEST_SUCCEED();
        } 
    } 
    catch (...)
    {
        std::ostringstream info_stream1;
        info_stream1 << "Test State Manager Exception caught : " << std::endl;
        TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }
    
}

