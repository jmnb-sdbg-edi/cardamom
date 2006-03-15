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

#include "test/TestSingleShotTimer.hpp"
#include "common/SingleShotTimer.hpp"
#include "Foundation/ossupport/OS.hpp"

using namespace Cdmw::OsSupport;
using namespace Cdmw::FT::Supervision;

//
// Observer implementation
//
TestTimeoutObserver::TestTimeoutObserver(const std::string& message)
     : m_message(message),
       m_executed(false)
{
}

void TestTimeoutObserver::wake_up() throw()
{
    m_executed = true;
    std::cout << m_message << std::endl;
}

bool TestTimeoutObserver::get_executed ()
{
    return m_executed;
}


//  
// Test implementation
//
TestSingleShotTimer::TestSingleShotTimer(const std::string& name)
    : Testable(name)
{
}


TestSingleShotTimer::~TestSingleShotTimer()
{
}


void TestSingleShotTimer::do_tests()
{    

    set_nbOfRequestedTestOK(21);

  // The observer is first initialised to NULL (to test with no observer)
    TestTimeoutObserver  *p_TimeoutObserver = NULL;
    int timescale = get_timescale();

    // This part test basic operations and exceptions without observer
    try
    {
    	TEST_INFO("Creates a SingleShotTimer with a NULL observer and timeout of 1s");
    	SingleShotTimer ssTimer (p_TimeoutObserver, timescale*1000);
        TEST_SUCCEED();

        TEST_INFO("Change the timeout value (timer thread not yet started) to 2s");
        ssTimer.setup(timescale*2000);
        TEST_SUCCEED();
	        
        TEST_INFO("Verify timer thread is in CREATED state");
        TEST_CHECK (ssTimer.get_status() == THREAD_CREATED);
	
        TEST_INFO("Start the timer thread (not the timer itself)");
        // This start is done only once for a particular timer
        ssTimer.start();
        OS::sleep(timescale*100);
        TEST_SUCCEED();
	
        TEST_INFO("Verify timer thread is in RUNNING state");
        TEST_CHECK (ssTimer.get_status() == THREAD_RUNNING);
	
        /* 29/09/03 : This test reveals a probable problem in Cdmw
        TEST_INFO("Try to start again the timer thread");
        try
	{
          ssTimer.start();
          TEST_FAILED();
        }
        // The exception is raised by the underlying (CDMW) thread mechanism
        catch(...)
	{
          TEST_SUCCEED();
	}
	*/
        TEST_INFO("Change the timeout value (thread started but not the timer) to 1s");
        ssTimer.setup(timescale*1000);
        TEST_SUCCEED();

        TEST_INFO("Start the timer");
        ssTimer.start_timer();
        OS::sleep(timescale*100);
        TEST_SUCCEED();
	
        TEST_INFO("try to change the timeout value with timer started");
        try
	{
          ssTimer.setup(timescale*2000);
          TEST_FAILED();
        }
        catch(Cdmw::BadOrderException)
	{
          TEST_SUCCEED();
        }
        
        TEST_INFO("try to start again the timer");
        try
	{
          ssTimer.start_timer();
          TEST_FAILED();
        }
        catch(Cdmw::AlreadyDoneException)
	{
          TEST_SUCCEED();
        }
        
        TEST_INFO("stop the timer");
        OS::sleep(timescale*100);
        ssTimer.cancel_timer();
        TEST_SUCCEED();
        
        /* In fact we should keep the possibility to stop the timer even
           if already stopped (then do nothing) because we can't control
           when monitoring stop (which can cancel_timer) occurs 
        TEST_INFO("try to stop again the timer");
        try
	{
          ssTimer.cancel_timer();
          TEST_FAILED();
        }
        catch(Cdmw::AlreadyDoneException)
	{
          TEST_SUCCEED();
        }
        */
        
        TEST_INFO("Change the timeout value (thread started but not the timer) to 0.5s");
        ssTimer.setup(timescale*500);
        TEST_SUCCEED();

        TEST_INFO("Start again the timer");
        ssTimer.start_timer();
        OS::sleep(timescale*100);
        TEST_SUCCEED();
        
        TEST_INFO("Wait until timeout..");
        OS::sleep(timescale*500);

        TEST_INFO("Verify that timer stopped (with a timeout change)");
        ssTimer.setup(timescale*1000);
        TEST_SUCCEED();

        TEST_INFO("Stop the timer thread");
        // This stop is done only once for a particular timer
        // It is then not possible to use this timer again
        ssTimer.stop();
        TEST_SUCCEED();
        OS::sleep(timescale*100);
                
        TEST_INFO("Verify timer thread is in ENDED state");
        TEST_CHECK (ssTimer.get_status() == THREAD_ENDED);
    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
        return;
    }
     
    // This part test the Timeout observer
    try
    {   
        TEST_INFO("Creates the TimeoutObserver");
        p_TimeoutObserver = new TestTimeoutObserver("The Observer has awaken");

    	TEST_INFO("Creates a SingleShotTimer with an observer and timeout of 0.5s");
    	SingleShotTimer ssTimer (p_TimeoutObserver, timescale*500);
        TEST_SUCCEED();

        TEST_INFO("Start the timer thread (not the timer itself)");
        // This start is done only once for a particular timer
        ssTimer.start();
        OS::sleep(timescale*100);
        TEST_SUCCEED();

        TEST_INFO("Wait timeout..");
        OS::sleep(timescale*500);

        TEST_INFO("verify Observer has not awaken (timer itself not started)");
        TEST_CHECK (p_TimeoutObserver->get_executed()==false);
 
        TEST_INFO("Start the timer");
        ssTimer.start_timer();
        OS::sleep(timescale*100);
        TEST_SUCCEED();

        TEST_INFO("Wait timeout..");
        OS::sleep(timescale*500);

        TEST_INFO("verify Observer has awaken");
        TEST_CHECK (p_TimeoutObserver->get_executed()==true);

        TEST_INFO("Stop the timer thread");
        // This stop is done only once for a particular timer
        // It is then not possible to use this timer again
        ssTimer.stop();
        TEST_SUCCEED();

    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
        return;
    }
}

