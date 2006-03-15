/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

/**
  * @author Raffaele Mele <rmele@progesi.it>,
  * @author Fabrizio Morciano <fmorciano@selex-si.com>
  */

#include "Time/clockservice/test/testcommon/ControllerTest.hpp"
#include "Time/clockservice/test/testcommon/anExecution.hpp"
#include "clockservice/Controller_impl.hpp"
#include "clockservice/IClock.hpp"
#include "ace/streams.h"
#include "ace/Read_Buffer.h"
#include "Time/clockservice/config.hpp"
#include "Time/clockservice/Util.hpp"
#include "Time/clockservice/CosClockService.stub.hpp"
#include "Foundation/ossupport/OS.hpp"

#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>
#include <memory>
#include <fstream>
#include <string.h>

#include "Time/clockservice/test/testcommon/TimeTestManager.hpp"
#include "Time/clockservice/test/testcommon/MyPeriodicExecution.hpp"
#include "Time/clockservice/Util.hpp"

using namespace CosClockService;
using namespace CosPropertyService;
using namespace Cdmw::clock;
using namespace Cdmw::clock::util;
using namespace CosClockService::PeriodicExecution;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION( ControllerTest );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ControllerTest, "testclockservice");

namespace 
{
    struct myAsserter : anExecutionCallback
    {
        TimeBase::TimeT real_start_, virt_start_;
        TimeBase::TimeT virt_prev_;

        double period_;
        double virt_epsilon_;
        int    failed_execs_;
        int    execs_;

        myAsserter(double period, double virt_epsilon)
        {
            period_       = period;
            virt_epsilon_ = virt_epsilon;
            failed_execs_ = 0;
            execs_        = 0;
        }

        int get_num_failed_executions()
        {
            return failed_execs_;
        }

        bool operator()(TimeBase::TimeT virt_current_time)
        {
            bool result = true;

            try
            {
                execs_++;

                TimeBase::TimeT real_curr = Cdmw::clock::compute_current_time();
                TimeBase::TimeT virt_curr = virt_current_time;

                if (execs_ == 1)
                {
                    real_start_ = real_curr;
                    virt_start_ = virt_curr;
                }

                double real_sec = Cdmw::clock::util::TimeT2seconds(real_curr - real_start_);
                double virt_sec = Cdmw::clock::util::TimeT2seconds(virt_curr - virt_start_);

                //std::cerr << '.';
                //std::cerr << std::endl;
                //std::cerr << "asserter execution: " << execs_ << ": real sampled (secs): " << real_sec << std::endl;
                //std::cerr << "asserter execution: " << execs_ << ": virt sampled (secs): " << virt_sec << std::endl;

                if (execs_ > 1)
                {
                    double virt_dlt_sec = Cdmw::clock::util::TimeT2seconds(virt_curr - virt_prev_);
                    //std::cerr << "asserter execution: " << execs_ << ": virt delta   (secs): " << virt_dlt_sec << std::endl;
                    if (std::fabs(period_ - virt_dlt_sec) > virt_epsilon_)
                    {
                        std::cerr << "error: asserter execution: " << execs_ << ": virt delta out of range" << std::endl;
                        failed_execs_++;
                        result = false;
                    }
                }

                virt_prev_ = virt_curr;
            }
            catch (const CosClockService::TimeUnavailable& e)
            {
                std::cerr << "error: asserter execution: " << execs_ << ": could not get current time" << std::endl;
                failed_execs_++;
                result = false;
            }
            catch (...)
            {
                std::cerr << "error: asserter execution: " << execs_ << ": unexpected exception" << std::endl;
                failed_execs_++;
                result = false;
            }
            return result;
        }
    };

    struct myAsserter4AutoController : anExecutionCallback
    {
        PeriodicExecution::Controller_ptr ptr_;
        int                               execs_;

        myAsserter4AutoController()
        {
            execs_ = 0;
        }

        void
        set_controller(PeriodicExecution::Controller_ptr ptr)
        {
            ptr_ = ptr;
        }

        bool operator()(TimeBase::TimeT /*aCurrent_time*/)
        {
            CORBA::Any aPar;
            ptr_->start(0, 0, 0, aPar);
            if (++execs_ > 10) return false;
            return true;
        }
    };

}

///////////////////////////////////////////////////////////////////////////////

ControllerTest::ControllerTest()
{
    m_my_ob = 0;
}

///////////////////////////////////////////////////////////////////////////////

void
ControllerTest::setUp()
{
    m_my_ob = new  LocalPeriodic();

    m_alias_name = "ClockService";
    Cdmw::TestUtils::CORBATestManager::instance ()->add (m_alias_name);
    
    try
    {
        m_obj = Cdmw::TestUtils::CORBATestManager::instance ()->get_object (m_alias_name);
    }
    catch (Cdmw::TestUtils::CORBATestManager::InvalidObject &)
    {
        CPPUNIT_FAIL ("Cdmw::TestUtils::CORBATestManager::InvalidObject");
    }
    catch ( ... )
    {
        CPPUNIT_FAIL ("Unknown exception!");
    }
    
    ClockCatalog_var clock_catalog = 
        CosClockService::ClockCatalog::_narrow(m_obj);
    CPPUNIT_ASSERT(!CORBA::is_nil( clock_catalog.in()));

    ClockCatalog::ClockEntry_var
        entry = clock_catalog->get_entry("ControlledExecutor");
    m_clock = entry->subject;
    CPPUNIT_ASSERT(!CORBA::is_nil( m_clock.in()));    

    m_executor = ControlledExecutor::_narrow(m_clock.in());
    CPPUNIT_ASSERT(!CORBA::is_nil(m_executor.in()));	
}

///////////////////////////////////////////////////////////////////////////////

void
ControllerTest::tearDown()
{
    //delete m_my_ob; // Removed to avoid signal SIGSEGV during ORB shutdown.
}


///////////////////////////////////////////////////////////////////////////////

void
ControllerTest::start_zero()
{
    try
    {
        // Get periodic.
        Periodic_var periodic = m_my_ob->_this();

        // Get controller.
        m_controller = m_executor->enable_periodic_execution( periodic.in() );

        // Start controller.
        CORBA::Any aPar;
        m_controller->start(0, 0, m_my_ob->exec_limit(), aPar);

        // Wait a few seconds.
        Cdmw::OsSupport::OS::sleep(2,0);
        CORBA::ULong execs = m_controller->executions();
        size_t       m_cnt = m_my_ob->max_count();
        bool         cond  = (execs == m_cnt);

        // Terminate controller.
        m_controller->terminate();

        /* ----- DEBUG CODE [BEGIN] ----- *
        if (!cond)
        {
            std::cerr << std::endl;
            std::cerr << "DEBUG: ControllerTest::start_zero: executions: " << execs << std::endl;
            std::cerr << "DEBUG: ControllerTest::start_zero: max_count : " << m_cnt << std::endl;
        }
         * ----- DEBUG CODE [ END ] ----- */

        // Check assertion.
        CPPUNIT_ASSERT(cond);
    }
    catch(CORBA::SystemException& e)
    {
        std::ostringstream oss;
        oss<<"CORBA::SystemException: "<<e;
        CPPUNIT_FAIL(oss.str());
    }
}

///////////////////////////////////////////////////////////////////////////////

void
ControllerTest::start_at_zero_period()
{
    try
    {
        // Get periodic.
        Periodic_var periodic = m_my_ob->_this();

        // Get controller.
        m_controller = m_executor->enable_periodic_execution( periodic.in() );

        // Start controller.
        CORBA::Any aPar;
        TimeBase::TimeT now = m_executor->current_time();
        TimeBase::TimeT delay = Cdmw::clock::util::seconds2TimeT(1.0);
        delay+=now;
        m_controller->start_at(0, delay, m_my_ob->exec_limit(), aPar);

        // Wait for a few seconds.
        Cdmw::OsSupport::OS::sleep(3,0);
        CORBA::ULong execs = m_controller->executions();
        size_t       m_cnt = m_my_ob->max_count();
        bool         cond  = (execs == m_cnt);

        // Terminate controller.
        m_controller->terminate();

        /* ----- DEBUG CODE [BEGIN] ----- *
        if (!cond)
        {
            std::cerr << std::endl;
            std::cerr << "DEBUG: ControllerTest::start_at_zero_period: executions: " << execs << std::endl;
            std::cerr << "DEBUG: ControllerTest::start_at_zero_period: max_count : " << m_cnt << std::endl;
        }
         * ----- DEBUG CODE [ END ] ----- */

        // Check assertion.
        CPPUNIT_ASSERT(cond);
    }
    catch(CORBA::SystemException& e)
    {
        std::ostringstream oss;
        oss<<"CORBA::SystemException: "<<e;
        CPPUNIT_FAIL(oss.str());
    }
}

///////////////////////////////////////////////////////////////////////////////

void
ControllerTest::start_at_zero()
{
    Periodic_var periodic = m_my_ob->_this();
    m_controller = m_executor->enable_periodic_execution( periodic.in() );

    CORBA::Any aPar;

    try
    {
        m_controller->start_at(0, 0, 1, aPar);
    }
    catch (const CosClockService::PeriodicExecution::Controller::TimePast& e)
    {
        m_controller->terminate();
        throw;
    }

    m_controller->terminate();
}

///////////////////////////////////////////////////////////////////////////////

/*
 * This test verify the solution of PCR_0450.
 * A Controller is obtained, after some non integer period
 * is executed a pause/resume sequence. 
 * Periodic execution is done after a reference period,
 * and this period is constant.
 */
void 
ControllerTest::PCR_0450()
{
    myAsserter* asserter = 0;

    try
    {
        const int    period_sec   = 4;
        const double virt_epsilon = 0.1; // NB: previous value was 0.01.
        const int    delta_sec    = period_sec / 2;

        asserter = new myAsserter(period_sec, virt_epsilon);

        anExecution<CosClockService::PeriodicExecution::Executor>* periodic =
            new anExecution<CosClockService::PeriodicExecution::Executor>(m_executor.in(), asserter);

        Periodic_var periodic_var = periodic->_this();

        CosClockService::PeriodicExecution::Controller_var controller_var =
            m_executor->enable_periodic_execution(periodic_var.in());

        // Start the current controller.
        CORBA::Any      params;
        TimeBase::TimeT period_tmt = seconds2TimeT(period_sec);
        controller_var->start(period_tmt, 0, 0, params);

        Cdmw::OsSupport::OS::sleep(period_sec + delta_sec,0);
        controller_var->pause();

        controller_var->resume();
        Cdmw::OsSupport::OS::sleep(period_sec + delta_sec,0);
        controller_var->pause();

        controller_var->resume();
        Cdmw::OsSupport::OS::sleep(period_sec + delta_sec,0);
        controller_var->pause();

        controller_var->resume();
        Cdmw::OsSupport::OS::sleep(3 * (period_sec + delta_sec),0);
        controller_var->pause();

        controller_var->resume();

        // Terminate the current controller.
        controller_var->terminate();
    }
    catch (...)
    {
        CPPUNIT_FAIL("unexpected exception");
    }

    CPPUNIT_ASSERT(asserter && (asserter->get_num_failed_executions() == 0));
}

///////////////////////////////////////////////////////////////////////////////

void 
ControllerTest::auto_controlled()
{
    myAsserter4AutoController
        myAsserter;
        
    anExecution<CosClockService::PeriodicExecution::Executor>*
        periodic = new anExecution<CosClockService::PeriodicExecution::Executor>(m_executor.in(),
                                                                                 &myAsserter);
    Periodic_var   
        periodic_var = periodic->_this();

    CosClockService::PeriodicExecution::Controller_var
        m_controller = m_executor->enable_periodic_execution(periodic_var.in());

    myAsserter.set_controller(m_controller.in());

    CORBA::Any aPar;
    m_controller->start_at(0,
                           0,
                           0,
                           aPar);
}

void 
ControllerTest::compute_time2end()
{
    class MyClock : public IClock
    {
        public:
            MyClock(const TimeBase::TimeT& ct = 0) : m_ct(ct) {}
            void set_current_time(const TimeBase::TimeT& ct)  { m_ct = ct; }
            TimeBase::TimeT current_time()                    { return m_ct; }
        private:
            TimeBase::TimeT m_ct;
    } m_clock;

    TimeBase::TimeT time2end;
    m_clock.set_current_time(3);
    time2end = Cdmw::clock::compute_delta2end(m_clock, 4, 2);
    //std::cerr << "Time2end: " << time2end << std::endl;
    CPPUNIT_ASSERT_EQUAL((TimeBase::TimeT) 3, time2end);

    m_clock.set_rate(0.5);
    time2end = Cdmw::clock::compute_delta2end(m_clock, 4, 2);
    //std::cerr << "Time2end: " << time2end << std::endl;
    CPPUNIT_ASSERT_EQUAL((TimeBase::TimeT) 6, time2end);
}

///////////////////////////////////////////////////////////////////////////////


static char* hello_world = "Hello World!!!";

void 
ControllerTest::check_any()
{
    try 
    {
	class   CheckAny
	: public POA_CosClockService::PeriodicExecution::Periodic
	{
		bool m_check;
	public:
	    CORBA::Boolean 
	    	do_work (const CORBA::Any& aPar)
        throw (CORBA::SystemException)
        {
	        m_check = false;

                // String Extraction
	        const char* tmp_str;
                CORBA::Boolean ok = (aPar >>= tmp_str);
                CPPUNIT_ASSERT(ok);
                if ( strcmp(tmp_str,::hello_world) == 0  )  
		  m_check = true;

        	return false;
        }
            bool 
                 get_m_check () 
                 {
                    return m_check;
                 }	    
	
	};
	
        CheckAny*
           periodic = new CheckAny();

        Periodic_var
           periodic_var = periodic->_this();

        CosClockService::PeriodicExecution::Controller_var
            m_controller = m_executor->enable_periodic_execution(periodic_var.in());
 
        TimeBase::TimeT now = m_executor->current_time();
        TimeBase::TimeT delay = Cdmw::clock::util::seconds2TimeT(1.0);
        delay+=now;
        // Insert string into a CORBA::Any
        CORBA::Any 
	     aPar;
        aPar <<= ::hello_world ;

        m_controller->start_at(0, delay, m_my_ob->exec_limit(), aPar);
        Cdmw::OsSupport::OS::sleep(3,0);

        // Terminate controller.
        m_controller->terminate();
        CPPUNIT_ASSERT(periodic->get_m_check());

     } 
   catch(CORBA::SystemException& e)
    {
        std::ostringstream oss;
        oss<<"CORBA::SystemException: "<<e;
        CPPUNIT_FAIL(oss.str());
    }
    catch (...)
    {
        CPPUNIT_FAIL("unexpected exception");
    }
}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
