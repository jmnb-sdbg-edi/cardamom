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

#include <testfaultdetector/TestFaultDetector.hpp>
#include <testfaultdetector/SIM_consumer.hpp>
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "FaultTolerance/idllib/CdmwFTFaultDetector.stub.hpp"
#include "FaultTolerance/idllib/CdmwFTManager.stub.hpp"
#include "FaultTolerance/idllib/FT.stub.hpp"

namespace Cdmw {

namespace FT {
        

    TestFaultDetector::TestFaultDetector(CORBA::ORB_ptr                  orb,
                                         PortableServer::POA_ptr         rootPOA,
                                         const std::string& name)
        : Testable( name ),
          m_orb(CORBA::ORB::_duplicate(orb)),
          m_poa(PortableServer::POA::_duplicate(rootPOA))
    {
    }
    
    
    TestFaultDetector::~TestFaultDetector()
    {
    }
    
    void TestFaultDetector::do_tests()
    {
        // set number of requested successfull tests
        set_nbOfRequestedTestOK (29);
        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        try{

            TEST_INFO("Start the fault manager");
            std::string managerArgs(" --CdmwXMLFile=CdmwFaultToleranceManager_conf.xml  --groupConf=CdmwFTSystemMngtGroupCreator_conf.xml");
            OsSupport::OS::ProcessId ft_manager_id = OsSupport::OS::create_process("cdmw_ft_manager", managerArgs);
            OsSupport::OS::sleep(timescale*15000);

            TEST_INFO("Create a consumer for the fault notifier");
            SIM_consumer* consumer_obj = new SIM_consumer();
        
            CosNotifyComm::StructuredPushConsumer_var consumer = consumer_obj->_this();
        
            TEST_INFO("Get the fault manager reference");
            std::string fault_manager_corbaloc(
                  "corbaloc::localhost:4555/fault_manager");
            CORBA::Object_var fm_obj
            = m_orb->string_to_object(fault_manager_corbaloc.c_str());

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

            fault_notifier->connect_structured_fault_consumer(consumer.in());
        

            TEST_INFO("Get the fault detector reference");
            CdmwFT::FaultManagement::FaultDetector_var fault_detector 
            = fault_manager->get_fault_detector();        
        
            if ( CORBA::is_nil( fault_detector.in() ) )
                CDMW_THROW2(Exception,
                            Exception::FATAL,
                            "Invalid fault_notifier reference" );


            // start the test
            TEST_INFO("");            
            TEST_INFO("FISRT PART");
            TEST_INFO("");
            //create the location
            ::FT::Location location;
            location.length(1);
            location[0].kind="hostname";
            location[0].id=CORBA::string_dup(OsSupport::OS::get_hostname().c_str());

            // add the location on the consumer
            consumer_obj->add_location(OsSupport::OS::get_hostname().c_str());        
        
            // create the time monitoring value 
            ::FT::FaultMonitoringIntervalAndTimeoutValue time;
            time.monitoring_interval=0;
            time.timeout=0; 


            ::FT::PullMonitorable_var monitorable_object = ::FT::PullMonitorable::_nil();

            // call start monitoring 
            TEST_INFO("start monitoring on a daemon not already started");        
            CdmwFT::FaultManagement::MonitoringId id = 
            fault_detector->start_monitoring (monitorable_object.in(),
                                              location,
                                              time );
            // get the status of the host just before the initialisation
            // if the status is 1 , the host is reponding 
            // else if the status is 0, the host is UNREACHABLE 
            TEST_CHECK(consumer_obj->get_host_status(OsSupport::OS::get_hostname().c_str()));

            OsSupport::OS::sleep(timescale*6000);

            // the host must be change
            TEST_CHECK(consumer_obj->a_new_event_is_arrived());        
            TEST_CHECK(!consumer_obj->get_host_status(OsSupport::OS::get_hostname().c_str()));
        
            // restart monitoring on the same host 
            CdmwFT::FaultManagement::MonitoringId id2 = 0;  

            TEST_INFO("Dump the FaultDetector datastore");
            fault_manager->dump_datastore(::CdmwFT::FAULT_DETECTOR);
            TEST_SUCCEED();            

      
            TEST_INFO("restart monitoring on a daemon not already started");        
            try{            
                id2 = fault_detector->start_monitoring (monitorable_object.in(),
                                                        location,
                                                        time );
                TEST_SUCCEED();            
            }
            catch (...)
            {
                TEST_FAILED();            
            }
        
            // the ids must be equal
            TEST_CHECK(id = id2);        
            OsSupport::OS::sleep(timescale*6000);
            TEST_CHECK(!consumer_obj->a_new_event_is_arrived()); 
            TEST_CHECK(!consumer_obj->get_host_status(OsSupport::OS::get_hostname().c_str()));

            TEST_INFO("stop monitoring on a daemon not already started");   
            try
            {            
                fault_detector->stop_monitoring (id);
                TEST_SUCCEED();            
            }        
            catch (...)
            {
                TEST_FAILED();            
            }
        
            TEST_INFO("restop monitoring on a daemon not already started");   
            try
            {    
                fault_detector->stop_monitoring (id);
                TEST_FAILED();            
            }
            catch ( const CdmwFT::FaultManagement::InvalidMonitoringId& )
            {
                TEST_SUCCEED();            
            }   
            catch (const CORBA::SystemException& ex)
            {
                std::cout<<ex<<std::endl;            
            }
        
            catch (...)
            {
                TEST_FAILED();            
            }

            TEST_INFO("");
            TEST_INFO("SECOND PART");            
            TEST_INFO("");

            // add the location on the consumer
            consumer_obj->add_location(OsSupport::OS::get_hostname().c_str()); 
            consumer_obj->add_location("HOST1");
            
            std::string daemonArgs(" --CdmwXMLFile=CdmwPlatformMngtDaemon_conf.xml --FaultManagerRegistration="); 
            daemonArgs += fault_manager_corbaloc;
            TEST_INFO("Starting the PlatformDaemon");
            OsSupport::OS::ProcessId  daemon_id = OsSupport::OS::create_process("cdmw_platform_daemon.sh", daemonArgs);   
            OsSupport::OS::sleep(timescale*8000);

            // no event because the monitoring is not starting
            TEST_CHECK(!consumer_obj->a_new_event_is_arrived()); 


            // call start monitoring 
            TEST_INFO("start monitoring on a daemon already started");        
            CdmwFT::FaultManagement::MonitoringId id4 = 
            fault_detector->start_monitoring (monitorable_object.in(),
                                              location,
                                              time );
        
            OsSupport::OS::sleep(timescale*8000);
            // no new event because the status of the host is responding in the supervision and 
            // the status in the moap is responding
            TEST_CHECK(!consumer_obj->a_new_event_is_arrived()); 
            TEST_CHECK(consumer_obj->get_host_status(OsSupport::OS::get_hostname().c_str()));

            location[0].id=CORBA::string_dup("HOST1");
            // call start monitoring 
            TEST_INFO("start monitoring on a daemon unknown");        
            CdmwFT::FaultManagement::MonitoringId id5 = 
            fault_detector->start_monitoring (monitorable_object.in(),
                                              location,
                                              time );

            OsSupport::OS::sleep(timescale*8000);
            // no new event because the status of the host is responding in the supervision and 
            // the status in the moap is responding
            TEST_CHECK(consumer_obj->a_new_event_is_arrived()); 
            TEST_CHECK(!consumer_obj->get_host_status("HOST1"));

            TEST_INFO("Dump the FaultDetector datastore");
            fault_manager->dump_datastore(::CdmwFT::FAULT_DETECTOR);
            TEST_SUCCEED();            


            TEST_INFO("kill the daemon");            
            OsSupport::OS::kill_process(daemon_id);

            OsSupport::OS::sleep(timescale*8000);
            TEST_CHECK(consumer_obj->a_new_event_is_arrived()); 
            TEST_CHECK(!consumer_obj->get_host_status(OsSupport::OS::get_hostname().c_str()));

            TEST_INFO("Restarting the PlatformDaemon");
            daemon_id = OsSupport::OS::create_process("cdmw_platform_daemon.sh", daemonArgs);   
            OsSupport::OS::sleep(timescale*8000);
            TEST_CHECK(consumer_obj->a_new_event_is_arrived()); 
            TEST_CHECK(consumer_obj->get_host_status(OsSupport::OS::get_hostname().c_str()));

            TEST_INFO("stop monitoring on the daemon already started");   
            try
            {            
                fault_detector->stop_monitoring (id4);
                TEST_SUCCEED();            
            }        
            catch (...)
            {
                TEST_FAILED();            
            }

            TEST_INFO("stop monitoring on the unkown daemon");   
            try
            {            
                fault_detector->stop_monitoring (id5);
                TEST_SUCCEED();            
            }        
            catch (...)
            {
                TEST_FAILED();            
            }

            TEST_INFO("kill the daemon");            
            OsSupport::OS::kill_process(daemon_id);
            OsSupport::OS::sleep(timescale*8000);

            
            TEST_INFO("");
            TEST_INFO("THIRD PART");
            TEST_INFO("");
            
            // add the location on the consumer
            consumer_obj->add_location(OsSupport::OS::get_hostname().c_str());  
            TEST_INFO("Starting the PlatformDaemon");
            daemon_id = OsSupport::OS::create_process("cdmw_platform_daemon.sh", daemonArgs);   
            OsSupport::OS::sleep(timescale*8000);

            // no event because the monitoring is not starting
            TEST_CHECK(!consumer_obj->a_new_event_is_arrived()); 

            location[0].id=CORBA::string_dup(OsSupport::OS::get_hostname().c_str());
            // call start monitoring 
            TEST_INFO("start monitoring on a daemon already started");        
            CdmwFT::FaultManagement::MonitoringId id3 = 
            fault_detector->start_monitoring (monitorable_object.in(),
                                              location,
                                              time );
        
            OsSupport::OS::sleep(timescale*5000);
            // no new event because the status of the host is responding in the supervision and 
            // the status in the moap is responding
            TEST_CHECK(!consumer_obj->a_new_event_is_arrived()); 

            TEST_INFO("kill the daemon");            
            OsSupport::OS::kill_process(daemon_id);

            OsSupport::OS::sleep(timescale*4000);
            TEST_CHECK(consumer_obj->a_new_event_is_arrived()); 
            TEST_CHECK(!consumer_obj->get_host_status(OsSupport::OS::get_hostname().c_str()));

            TEST_INFO("Restarting the PlatformDaemon");
            daemon_id = OsSupport::OS::create_process("cdmw_platform_daemon.sh", daemonArgs);   
            OsSupport::OS::sleep(timescale*10000);
            TEST_CHECK(consumer_obj->a_new_event_is_arrived()); 
            TEST_CHECK(consumer_obj->get_host_status(OsSupport::OS::get_hostname().c_str()));

            TEST_INFO("stop monitoring on a daemon already started");   
            try
            {            
                fault_detector->stop_monitoring (id3);
                TEST_SUCCEED();            
            }        
            catch (...)
            {
                TEST_FAILED();            
            }

            TEST_INFO("kill the daemon");            
            OsSupport::OS::kill_process(daemon_id);
            OsSupport::OS::sleep(timescale*4000);
            
            TEST_INFO("kill the replication manager");            
            OsSupport::OS::kill_process(ft_manager_id);
            OsSupport::OS::sleep(timescale*5000);

        }
        catch(const CORBA::SystemException& ex)
        {
            std::cout<<"<<<TEST_FAILED>>> : "<<ex<<std::endl;            
        }    
        catch(...)
        {
            std::cout<<"<<<TEST_FAILED>>>"<<std::endl;            
        }    
    }
    
    
}; // namespace FT
}; // namespace Cdmw
