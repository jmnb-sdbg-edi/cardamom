/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#include "testplatformlibrary/TestPullMonitor.hpp"

#include "SystemMngt/platformlibrary/PullMonitor.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtMonitoring.stub.hpp"


#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/System.hpp"
#include "Foundation/testutils/Testable.hpp"
#include "Foundation/orbsupport/CORBA.hpp"

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>


using namespace CdmwPlatformMngt;
using namespace Cdmw::PlatformMngt;
using namespace Cdmw::OsSupport;




MonitorCallback::MonitorCallback(const std::string& message)
     : m_message(message),
       m_executed(false)
{
}

void MonitorCallback::execute() throw()
{
    m_executed = true;

    std::cout << m_message << std::endl;
}


bool MonitorCallback::get_executed ()
{
    return m_executed;
}
     


TestPullMonitor::TestPullMonitor(const std::string& name, 
                                 CORBA::ORB_ptr orb,
                                 PortableServer::POA_ptr poa)
    : Testable(name)
{
	m_orb = CORBA::ORB::_duplicate(orb);
    m_POA = PortableServer::POA::_duplicate(poa);
}


TestPullMonitor::~TestPullMonitor()
{
}


void TestPullMonitor::do_tests()
{   
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (17);
    
     
	MonitorCallback  *p_ResponseCallback = NULL;
	MonitorCallback  *p_NoResponseCallback = NULL;
	
  
    std::string monitorableServicePort = "51234";
    std::string monitorableServiceName = "MonitorableService";
    std::string corbaloc;

    int timescale = Cdmw::TestUtils::Testable::get_timescale();

    try
    {
    	TEST_INFO("Initialize Pull Monitoring");
    	
    	// initialize Bound Synchronous Call Factory
        Cdmw::PlatformMngt::BoundSyncCallFactory::initialise(m_orb.in());
        
        // initialize corbaloc reference
		std::ostringstream corbaloc_ref;
		corbaloc_ref << "corbaloc::localhost:" << monitorableServicePort.c_str();    
		corbaloc_ref << "/" << monitorableServiceName.c_str();
		
        corbaloc = corbaloc_ref.str();
        
        TEST_SUCCEED();
    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
        return;
    }
     
    try
    {   
		int sleep_time=timescale*200;
	    TEST_INFO("Starting the monitorable process 1 - sleep time 200*timescale  msec");
		std::ostringstream Monitorable_Args;
		Monitorable_Args << "--no-unit-test --sleep-time=" << sleep_time <<" --CdmwLocalisationService="<<monitorableServicePort;

		OS::ProcessId pidMonitorable = 
		       OS::create_process ("cdmw_test_monitoring", Monitorable_Args.str());
							               
	    TEST_INFO("Sleeping a while to wait for the process creation");
	    OS::sleep(timescale*5000);
				
	    TEST_INFO("Trying to resolve the corbaloc reference");				    									    
	    CORBA::Object_ptr obj = m_orb->string_to_object(corbaloc.c_str());
		TEST_CHECK(! CORBA::is_nil(obj));
												    
	    TEST_INFO("Trying to narrowing the reference");
	    PullMonitorable_var pullMonitorable = PullMonitorable::_narrow(obj);
	    TEST_CHECK(! CORBA::is_nil(pullMonitorable.in()));
																			

        TEST_INFO("Creates a PullMonitor Object: Polling timescale*1s, Timeout timescale*500ms");
        PullMonitor pullMonitor (pullMonitorable.in(), timescale*1000,timescale*500);
        
        TEST_INFO("Creates callbacks and set them in Monitor");
        p_ResponseCallback = new MonitorCallback ("Monitorable object is alive");
        p_NoResponseCallback = new MonitorCallback ("no response from Monitorable object");
        
        pullMonitor.setResponseCallback (p_ResponseCallback);
        pullMonitor.setNoResponseCallback (p_NoResponseCallback);
           
        TEST_SUCCEED();
    
    
        TEST_INFO("Start monitor");
        pullMonitor.start();
        OS::sleep(timescale*100);
        
        TEST_INFO("Check  callback has not yet been called");
        TEST_CHECK(p_ResponseCallback->get_executed() == false);       
        TEST_CHECK(p_NoResponseCallback->get_executed() == false);

        OS::sleep(timescale*1500);
        TEST_INFO("Check response callback has been called");
        TEST_CHECK(p_ResponseCallback->get_executed() == true);
        
        TEST_INFO("Check no response callback has not been called");
        TEST_CHECK(p_NoResponseCallback->get_executed() == false);
        
        
        TEST_INFO("Stop monitoring");
        
        pullMonitor.stop();
        
        // then kill the Monitorable process
        OS::kill_process (pidMonitorable);
        OS::sleep(timescale*3000);
                      
        TEST_SUCCEED();
                
    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
        return;
    }



    try
    {   
	    TEST_INFO("Starting the monitorable process 2 - sleep time timescale*1000 msec");
		std::ostringstream Monitorable_Args;
		Monitorable_Args << "--no-unit-test --sleep-time=" << timescale*1000 << " --CdmwLocalisationService=" << monitorableServicePort;

		OS::ProcessId pidMonitorable = 
		       OS::create_process ("cdmw_test_monitoring", Monitorable_Args.str());
							               
	    TEST_INFO("Sleeping a while to wait for the process creation");
	    OS::sleep(timescale*5000);
				
	    TEST_INFO("Trying to resolve the corbaloc reference");				    									    
	    CORBA::Object_ptr obj = m_orb->string_to_object(corbaloc.c_str());
		TEST_CHECK(! CORBA::is_nil(obj));
												    
	    TEST_INFO("Trying to narrowing the reference");
	    PullMonitorable_var pullMonitorable = PullMonitorable::_narrow(obj);
	    TEST_CHECK(! CORBA::is_nil(pullMonitorable.in()));
																			
																			
        TEST_INFO("Creates a PullMonitor Object: Polling timescale*1s, Timeout timescale*500ms");
        PullMonitor pullMonitor (pullMonitorable.in(), 1000*timescale, 500*timescale);
        
        TEST_INFO("Creates callbacks and set them in Monitor");
        p_ResponseCallback = new MonitorCallback ("Monitorable object is alive");
        p_NoResponseCallback = new MonitorCallback ("no response from Monitorable object");
        
        pullMonitor.setResponseCallback (p_ResponseCallback);
        pullMonitor.setNoResponseCallback (p_NoResponseCallback);
           
        TEST_SUCCEED();
    
    
        TEST_INFO("Start monitor");
        pullMonitor.start();
        OS::sleep(timescale*100);
        
        TEST_INFO("Check  callback has not yet been called");
        TEST_CHECK(p_ResponseCallback->get_executed() == false);       
        TEST_CHECK(p_NoResponseCallback->get_executed() == false);
        
        
        OS::sleep(timescale*1500);
        TEST_INFO("Check response callback has not been called");
        TEST_CHECK(p_ResponseCallback->get_executed() == false);
        
        TEST_INFO("Check no response callback has been called");
        TEST_CHECK(p_NoResponseCallback->get_executed() == true);
        
        
        TEST_INFO("Stop monitoring");
       
        
        pullMonitor.stop();
        
        
        // then kill the Monitorable process
        OS::kill_process (pidMonitorable);
        OS::sleep(timescale*3000);
                      
        TEST_SUCCEED();
                
    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
        return;
    }
    
    
    Cdmw::PlatformMngt::BoundSyncCallFactory::destroy();
}


