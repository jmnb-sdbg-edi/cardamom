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

#include "Foundation/orbsupport/CORBA.hpp"

#include "Supervisioncollaborationtest/Test.hpp"
#include "Supervisioncollaborationtest/SIM_consumer.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "FaultTolerance/idllib/CdmwFTManager.stub.hpp"
#include "Foundation/ossupport/OS.hpp"
// constructor
Test::Test(CORBA::ORB_ptr orb, std::string host1, std::string host2, std::string host3, std::string port)
    : Cdmw::TestUtils::Testable( "Supervision collaboration test" ),
      m_orb(CORBA::ORB::_duplicate(orb)),
      m_host1(host1),
      m_host2(host2),
      m_host3(host3),
      m_port(port)
{
}

// destructor
Test::~Test()
{
}

// do_tests
void Test::do_tests()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK( 18 );
    int timescale = Cdmw::TestUtils::Testable::get_timescale();

    SIM_consumer* consumer_obj = new SIM_consumer();
    CosNotifyComm::StructuredPushConsumer_var consumer = consumer_obj->_this();

    using namespace Cdmw;

    TEST_INFO("Get the fault manager reference");
    CORBA::Object_var fm_obj
    = m_orb->string_to_object("corbaloc::localhost:4555/fault_manager");

    ::CdmwFT::FTManager_var fault_manager = ::CdmwFT::FTManager::_narrow( fm_obj.in() );
    
    ::FT::FaultNotifier_var fault_notifier = fault_manager->get_fault_notifier();        
    
    ::FT::FaultNotifier::ConsumerId id = 0;            
    TEST_INFO("register the Fault Notifier consumer");
    id =  fault_notifier->connect_structured_fault_consumer(consumer.in());


    TEST_INFO("Start the monitoring on a host not already started");
    std::string adminArgs(" --sys-add-host V-FLT-004-2.xml");
    
    OsSupport::OS::ProcessId admin_id = OsSupport::OS::create_process("platform_admin.sh", adminArgs);
    OsSupport::OS::sleep(timescale*5000);
    

    TEST_INFO("Check if the "<< m_host2 <<"host status is HOST_UNREACHABLE");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::string dummy;
    std::cin >> dummy;
    OsSupport::OS::sleep(timescale*1000);
    TEST_CHECK(consumer_obj->new_event_not_responding());
    TEST_CHECK(!consumer_obj->new_event_responding());

    TEST_INFO("Launch the daemon on the host "<< m_host2 );
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;

    
    TEST_INFO("Check if the "<< m_host2 <<"host status is HOST_RESPONDING");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;
    OsSupport::OS::sleep(timescale*1000);
    TEST_CHECK(!consumer_obj->new_event_not_responding());
    TEST_CHECK(consumer_obj->new_event_responding());


    TEST_INFO("Kill the daemon on the host "<< m_host2 );
    TEST_INFO("Check if the "<< m_host2 <<"host status is HOST_UNREACHABLE");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;
    OsSupport::OS::sleep(timescale*1000);
    TEST_CHECK(consumer_obj->new_event_not_responding());
    TEST_CHECK(!consumer_obj->new_event_responding());


    TEST_INFO("Restart the daemon on the host "<< m_host2 );
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;

    TEST_INFO("Check if the "<< m_host2 <<"host status is HOST_RESPONDING");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;
    OsSupport::OS::sleep(timescale*1000);
    TEST_CHECK(!consumer_obj->new_event_not_responding());
    TEST_CHECK(consumer_obj->new_event_responding());

    TEST_INFO("Stop the monitoring on a host " <<m_host2 );
    adminArgs = " --sys-remove-host ";
    adminArgs += m_host2;
    
    admin_id = OsSupport::OS::create_process("platform_admin.sh", adminArgs);
    OsSupport::OS::sleep(timescale*5000);

    TEST_INFO("Kill the daemon on the host "<< m_host2 );
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;
    OsSupport::OS::sleep(timescale*1000);
    TEST_CHECK(!consumer_obj->new_event_not_responding());
    TEST_CHECK(!consumer_obj->new_event_responding());


    TEST_INFO("Launch the daemon on the host "<< m_host3 );
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;
    OsSupport::OS::sleep(timescale*1000);
    TEST_CHECK(!consumer_obj->new_event_not_responding());
    TEST_CHECK(!consumer_obj->new_event_responding());

    TEST_INFO("Kill the daemon on the host "<< m_host3 );
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;
    OsSupport::OS::sleep(timescale*1000);
    TEST_CHECK(!consumer_obj->new_event_not_responding());
    TEST_CHECK(!consumer_obj->new_event_responding());

    TEST_INFO("Launch the daemon on the host "<< m_host3 );
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;
    OsSupport::OS::sleep(timescale*1000);
    TEST_CHECK(!consumer_obj->new_event_not_responding());
    TEST_CHECK(!consumer_obj->new_event_responding());


    TEST_INFO("Start the monitoring on a previous host");
    adminArgs=" --sys-add-host V-FLT-004-3.xml";
    
    admin_id = OsSupport::OS::create_process("platform_admin.sh", adminArgs);
    OsSupport::OS::sleep(timescale*5000);


    TEST_INFO("Check if the "<< m_host3 <<"host status is HOST_RESPONDING");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;
    OsSupport::OS::sleep(timescale*1000);
    TEST_CHECK(!consumer_obj->new_event_not_responding());
    TEST_CHECK(!consumer_obj->new_event_responding());


    TEST_INFO("End of the test");
}

