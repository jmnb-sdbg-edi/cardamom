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

#include <testfaultnotifier/TestFaultNotifier.hpp>
#include <testfaultnotifier/SIM_consumer.hpp>
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "FaultTolerance/idllib/CdmwFTManager.stub.hpp"
#include "FaultTolerance/idllib/FT.stub.hpp"

namespace Cdmw {

namespace FT {
        

    TestFaultNotifier::TestFaultNotifier(CORBA::ORB_ptr                  orb,
                                         PortableServer::POA_ptr         rootPOA,
                                         const std::string& name)
        : Testable( name ),
          m_orb(CORBA::ORB::_duplicate(orb)),
          m_poa(PortableServer::POA::_duplicate(rootPOA))
    {
    }
    
    
    TestFaultNotifier::~TestFaultNotifier()
    {
    }
    
    void TestFaultNotifier::do_tests()
    {
        // set number of requested successfull tests
        set_nbOfRequestedTestOK (19);
        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        try{
            
            TEST_INFO("Start the fault manager");
            std::string managerArgs(" --CdmwXMLFile=CdmwFaultToleranceManager_conf.xml");
            OsSupport::OS::ProcessId ft_manager_id = OsSupport::OS::create_process("cdmw_ft_manager", managerArgs);
            OsSupport::OS::sleep(timescale*15000);

            TEST_INFO("Create three consumers for the fault notifier");
            SIM_consumer* consumer_obj1 = new SIM_consumer(1);
            CosNotifyComm::StructuredPushConsumer_var consumer1 = consumer_obj1->_this();
        
            SIM_consumer* consumer_obj2 = new SIM_consumer(2);
            CosNotifyComm::StructuredPushConsumer_var consumer2 = consumer_obj2->_this();

            SIM_consumer* consumer_obj3 = new SIM_consumer(3);
            CosNotifyComm::StructuredPushConsumer_var consumer3 = consumer_obj3->_this();



            TEST_INFO("Get the fault manager reference");
            CORBA::Object_var fm_obj
            = m_orb->string_to_object("corbaloc::localhost:4555/fault_manager");

            if ( CORBA::is_nil( fm_obj.in() ) )
                CDMW_THROW2(Exception,
                            Exception::FATAL,
                            "Invalid corbaloc" );
        
            ::CdmwFT::FTManager_var fault_manager = ::CdmwFT::FTManager::_narrow( fm_obj.in() );

            ::FT::FaultNotifier_var fault_notifier = fault_manager->get_fault_notifier();        

            if ( CORBA::is_nil( fault_notifier.in() ) )
                CDMW_THROW2(Exception,
                            Exception::FATAL,
                            "Invalid fault_notifier reference" );


            ::FT::FaultNotifier::ConsumerId current_id = 0;            
            ::FT::FaultNotifier::ConsumerId id1 = 0;            
            ::FT::FaultNotifier::ConsumerId id2 = 0;            
            ::FT::FaultNotifier::ConsumerId id3 = 0;            
            ::FT::FaultNotifier::ConsumerId id4 = 0;            
            TEST_INFO("register the first consumer");
            id1 =  fault_notifier->connect_structured_fault_consumer(consumer1.in());
            current_id = id1;
            
            TEST_INFO("register the second consumer");
            id2 =  fault_notifier->connect_structured_fault_consumer(consumer2.in());
            TEST_CHECK((current_id + 1) == id2);
            current_id = id2;
            
            TEST_INFO("register the third consumer");
            id3 =  fault_notifier->connect_structured_fault_consumer(consumer3.in());
            TEST_CHECK((current_id + 1) == id3);
            current_id = id3;
            
            TEST_INFO("register the third consumer again");
            id4 =  fault_notifier->connect_structured_fault_consumer(consumer3.in());
            TEST_CHECK((current_id + 1) == id4);
            current_id = id4;
            
            TEST_INFO("create a fault report and sent it");
            ::FT::Location location;
            location.length(1);
            location[0].kind = CORBA::string_dup( "hostname" );
            location[0].id = CORBA::string_dup("localhost");
            
            // Prepare the event for the fault notification
            CosNotification::StructuredEvent fault_event;
            fault_event.header.fixed_header.event_type.domain_name = CORBA::string_dup( "FT_CORBA" );
            fault_event.header.fixed_header.event_type.type_name = CORBA::string_dup( "ObjectCrashFault" );
            fault_event.filterable_data.length( 2 );
            fault_event.filterable_data[0].name = CORBA::string_dup( "FTDomainId" );
            fault_event.filterable_data[0].value <<= "thalesgroup.com";
            fault_event.filterable_data[1].name = CORBA::string_dup( "Location" );
            fault_event.filterable_data[1].value <<= location;
            fault_notifier->push_structured_fault( fault_event );
            OsSupport::OS::sleep(timescale*2000);

            
            TEST_CHECK(consumer_obj1->new_event());
            TEST_CHECK(consumer_obj2->new_event());
            TEST_CHECK(consumer_obj3->new_event());
            
            TEST_INFO("Dump the FaultNotifier Datastore");
            fault_manager->dump_datastore(::CdmwFT::FAULT_NOTIFIER);
            TEST_SUCCEED();

            TEST_INFO("remove the consumer3 with th id4");
            fault_notifier->disconnect_consumer(id4);
            
            TEST_INFO("Send the fault report");            
            fault_notifier->push_structured_fault( fault_event );
            OsSupport::OS::sleep(timescale*2000);
            
            TEST_CHECK(consumer_obj1->new_event());
            TEST_CHECK(consumer_obj2->new_event());
            TEST_CHECK(consumer_obj3->new_event());

            TEST_INFO("remove the consumer3 with th id3");
            fault_notifier->disconnect_consumer(id3);
            
            TEST_INFO("Send the fault report");            
            fault_notifier->push_structured_fault( fault_event );
            OsSupport::OS::sleep(timescale*2000);
            
            TEST_CHECK(consumer_obj1->new_event());
            TEST_CHECK(consumer_obj2->new_event());
            TEST_CHECK(!consumer_obj3->new_event());
            
            TEST_INFO("Disconnect a consumer already disconnected");            
            try
            {
                fault_notifier->disconnect_consumer(id3);
                TEST_FAILED();                
            }
            catch (const CosEventComm::Disconnected&)
            {
                TEST_SUCCEED();
            }
            catch (CORBA::SystemException&)
            {
                TEST_FAILED();                
            }
            
            TEST_INFO("try to connect a nil consumer");            
            try
            {
                fault_notifier->connect_structured_fault_consumer(CosNotifyComm::StructuredPushConsumer::_nil());
                TEST_FAILED();                
            }
            catch(const CORBA::BAD_PARAM&)
            {
                TEST_SUCCEED();
            }
            catch(CORBA::SystemException&)
            {
                TEST_FAILED();                
            }
            
            TEST_INFO("try to disconnect a consummer with a bad id ");            
            try
            {
                fault_notifier->disconnect_consumer(123456);
                TEST_FAILED();                
            }
            catch (const CosEventComm::Disconnected&)
            {
                TEST_SUCCEED();
            }
            catch (CORBA::SystemException&)
            {
                TEST_FAILED();                
            }

            TEST_INFO("remove the consumer2 with th id2");
            fault_notifier->disconnect_consumer(id2);
            TEST_INFO("remove the consumer1 with th id1");
            fault_notifier->disconnect_consumer(id1);
            
            TEST_INFO("Send the fault report");            
            fault_notifier->push_structured_fault( fault_event );
            OsSupport::OS::sleep(timescale*2000);
            
            TEST_CHECK(!consumer_obj1->new_event());
            TEST_CHECK(!consumer_obj2->new_event());
            TEST_CHECK(!consumer_obj3->new_event());

            TEST_INFO("kill the replication manager");            
            OsSupport::OS::kill_process(ft_manager_id);
            OsSupport::OS::sleep(timescale*5000);
        }
        catch(CORBA::SystemException& ex)
        {
            TEST_INFO ("Unexpected CORBA Systenm exception: " << ex);
            TEST_FAILED();
        }
        catch (...)
        {
            TEST_FAILED();            
        }
        

    }
    
    
}; // namespace FT
}; // namespace Cdmw
