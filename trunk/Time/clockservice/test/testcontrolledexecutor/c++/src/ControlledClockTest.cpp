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
 *
 * @author Raffaele Mele <rmele@progesi.it>
 * @author Fabrizio Morciano <fmorciano@selex-si.com>
 */

#include <string>
#include <sstream>
#include <iostream>
#include <ctime>
#include <limits>

#include "testcontrolledexecutor/ControlledClockTest.hpp"
#include "Time/clockservice/test/testcommon/TimeTestManager.hpp"

#include "clockservice/ControlledClock_impl.hpp"
#include "Time/clockservice/config.hpp"
#include "Time/clockservice/Macro.hpp"
#include "Time/clockservice/Util.hpp"

using namespace CosClockService;
using namespace CosPropertyService;
using namespace Cdmw::clock;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION( ControlledClockTest );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ControlledClockTest, "testcontrolledexecutor");

///////////////////////////////////////////////////////////////////////////////

namespace 
{
    const TimeBase::TimeT epsilon = util::seconds2TimeT(0.1);
    const TimeBase::TimeT one_sec_in_TimeT = util::seconds2TimeT(1);    
    
    class MyObserver 
    :   public virtual POA_Cdmw::clock::ClockObserver,
        public virtual Cdmw::clock::ClockObserver
    {
        private:
        Cdmw::clock::ControlledClockEvent event_;
        
        public:
        MyObserver()
        {
            event_ = Cdmw::clock::NONE;
        }
        
        void 
        handle_set (const ::CORBA::Any & params)
            throw (CORBA::SystemException)
        {
            event_ |= Cdmw::clock::SET_TIME;
        }
  
        void 
        handle_set_rate (const ::CORBA::Any & params)
            throw (CORBA::SystemException)
        {
            event_ |= Cdmw::clock::SET_RATE;
        }
  
        void 
        handle_pause (const ::CORBA::Any & params)
            throw (CORBA::SystemException)
        {
            event_ |= Cdmw::clock::PAUSE;            
        }
  
        void 
        handle_resume (const ::CORBA::Any & params)
            throw (CORBA::SystemException)
        {
            event_ |= Cdmw::clock::RESUME;            
        }
  
        void 
        handle_terminate (const ::CORBA::Any & params)
            throw (CORBA::SystemException)
        {
            event_ |= Cdmw::clock::TERMINATE;            
        }
        
        Cdmw::clock::ControlledClockEvent getProcessedEvents() const
        {
            return event_;
        }
    };
}

///////////////////////////////////////////////////////////////////////////////

void
ControlledClockTest::setUp()
{
    m_clockType =  "ControlledClock";
    m_clockType1 =  "ControlledClock1" ;
    m_clockType5 =  "ControlledClock5" ;

    m_alias_name = "ClockService";
    m_clock = 0;  
    Cdmw::TestUtils::CORBATestManager::instance()->add(m_alias_name);
    try
    {
        m_obj = Cdmw::TestUtils::CORBATestManager::instance()->get_object(m_alias_name);
    }
    catch(Cdmw::TestUtils::CORBATestManager::InvalidObject&)
    {
        CPPUNIT_FAIL("Cdmw::TestUtils::CORBATestManager::InvalidObject");
    }

    m_clockCatalog = ClockCatalog::_narrow(m_obj);

    if (CORBA::is_nil(m_clockCatalog.in())) {
        CPPUNIT_FAIL("testControlledClock:>> Unable to resolve ClockCatalog");
    }

    ClockCatalog::ClockEntry_var clockEntry =
        m_clockCatalog->get_entry("ControlledClock");

    m_controlledClock =
        ControlledClock::_narrow(clockEntry->subject.in());

    CPPUNIT_ASSERT(!CORBA::is_nil(m_controlledClock.in() ) );

    m_alias_name = "LocalClock";  
    Cdmw::TestUtils::CORBATestManager::instance()->add(m_alias_name);
    try
    {
        m_obj = Cdmw::TestUtils::CORBATestManager::instance()->get_object(m_alias_name);
    }
    catch(Cdmw::TestUtils::CORBATestManager::InvalidObject&)
    {
        CPPUNIT_FAIL("Cdmw::TestUtils::CORBATestManager::InvalidObject");
    }

    m_clock = Clock::_narrow(m_obj);
    CPPUNIT_ASSERT(!CORBA::is_nil(m_clock.in() ) );
}

///////////////////////////////////////////////////////////////////////////////

void
ControlledClockTest::tearDown()
{
    try
    {
        m_controlledClock->set_rate(1.0);
    }
    catch(...)
    {
    }
	
    try
    {
        m_controlledClock->set(m_clock->current_time());
    }
    catch(...)
    {
    }
}

///////////////////////////////////////////////////////////////////////////////

ControlledClockTest::ControlledClockTest()
{  
}

///////////////////////////////////////////////////////////////////////////////

void
ControlledClockTest::set()
{    
    TimeBase::TimeT first_time,
        new_time;

    for (int n=-MAX_EXEC; n<= MAX_EXEC; n++)
    {
        first_time = m_controlledClock->current_time();
        int tmp = 100 * (n > 0 ? n : -n);
        new_time = tmp*SEC_IN_TIME_UNITS;

        if (n < 0) // remove n*100 seconds
            m_controlledClock->set(first_time - new_time); 
        else // add n*100 seconds
            m_controlledClock->set(first_time + new_time); 

        new_time = m_controlledClock->current_time();

        if (n < 0) // virtual time is LOWER than real time
            CPPUNIT_ASSERT(new_time < first_time);
        else if (n == 0) // virtual time follow the real time
            CPPUNIT_ASSERT(is_equal_w_tol(new_time,first_time));        
        else // virtual time is GREATHER than real time
            CPPUNIT_ASSERT(new_time > first_time);

        // test if virtual time is in correct interval
        if (n < 0)
            CPPUNIT_ASSERT(is_equal_w_tol(new_time, first_time ,
            (100 * (-n) * SEC_IN_TIME_UNITS)) + CDMW_TOL);
        else if (n==0)
            CPPUNIT_ASSERT(is_equal_w_tol(new_time, first_time));
        else
            CPPUNIT_ASSERT(is_equal_w_tol(new_time, first_time ,
            (100 * n * SEC_IN_TIME_UNITS)) + CDMW_TOL);
        // reset virtual time modify
        m_controlledClock->set(m_controlledClock->current_time() + 
            100 * n * SEC_IN_TIME_UNITS);
    }
}

///////////////////////////////////////////////////////////////////////////////

void  
ControlledClockTest::set_and_get_rate()
{
    ClockCatalog::ClockEntry_var clockEntry =
        m_clockCatalog->get_entry(m_clockType1.c_str());

    ControlledClockEx_var  controlledClock_1 =
        ControlledClockEx::_narrow(clockEntry->subject.in());

    if (CORBA::is_nil(controlledClock_1.in()))
        CPPUNIT_FAIL("testControlledClock:>> Unable to resolve ClockCatalog");
#if 0          
    // There is a min rate
    //  this value is a function of current_time
    //  time equation is 
    //  virtual_time = virtual_time_0 + (real_time-real_time_0) * rate
    //  
    //  max value for virtual_time is equal to 
    //  0xFFFFFFFFFFFFFFFF = MAX_TIMET ~ 18E18
    //  and min is 
    //  0x0 = MIN_TIME_VAL
    //
    // Then if rate is > o < than 0 
    //  an absolute rate = 18E18 can raise an exception

    float rate;

    controlledClock_1->set(macro::MAX_TIMET);
    for(rate=-100.0;
        rate<=0.0;
        rate+=10.0)
    {   
        controlledClock_1->set_rate(rate);
        double tmp = controlledClock_1->get_rate();
        CPPUNIT_ASSERT( tmp == rate );
    }

    controlledClock_1->set(0);
    for(rate=1.0;
        rate<=100.0;
        rate+=10.0)
    {   
        controlledClock_1->set_rate(rate);
        double tmp = controlledClock_1->get_rate();
        CPPUNIT_ASSERT( tmp == rate );
    }
#endif

	float  rate;
	TimeBase::TimeT	aTime = macro::MAX_TIMET>>10;
	
    controlledClock_1->set(aTime);
    for(rate=-10.0;
        rate<=10.0;
        rate+=1.0)
    {   
        controlledClock_1->set_rate(rate);
        double tmp = controlledClock_1->get_rate();
        CPPUNIT_ASSERT( tmp == rate );
    }	    	
}

///////////////////////////////////////////////////////////////////////////////

void  
ControlledClockTest::get_time_origin()
{
    ClockCatalog::ClockEntry_var clockEntry =
        m_clockCatalog->get_entry(m_clockType.c_str());

    ControlledClockEx_var controlledClockEx =
        ControlledClockEx::_narrow(clockEntry->subject.in());  

    TimeBase::TimeT value = m_clock->current_time();

    // just to avoid wrong exception
    controlledClockEx->set(value);
    controlledClockEx->set_rate(1.0);
	
    TimeBase::TimeT prev;
    prev = controlledClockEx->current_time();
    TimeBase::TimeT t0 = 0;
    controlledClockEx->set(t0);
    
    TimeBase::TimeT x0;
    TimeBase::TimeT y0;
    
    controlledClockEx->get_time_origins(x0, y0);

    const TimeBase::TimeT tolerance = 35000; // tolerance of 3.5 ms
    bool assertion1 = ((value <= x0)  && ((x0 - value) < tolerance));
    bool assertion2 = (y0 == t0);

    /* ----- DEBUG CODE [BEGIN] ----- *
    if ((assertion1 == false) || (assertion2 == false))
    {
        std::cerr << std::endl;
        std::cerr << "DEBUG: (x0 - value) (< " << tolerance << " ?): " << (x0 - value) << std::endl;
        std::cerr << "DEBUG: y0 (== 0 ?): " << y0 << std::endl;
    }
     * ----- DEBUG CODE [ END ] ----- */

    CPPUNIT_ASSERT(assertion1 && assertion2);
	
    /*
    ClockCatalog::ClockEntry_var clockEntry =
        m_clockCatalog->get_entry(m_clockType.c_str() );
        //m_clockCatalog->get_entry("ControlledClock");

    ControlledClockEx_var  controlledClockEx =
        ControlledClockEx::_narrow(clockEntry->subject.in());  

	// just to avoid wrong exception
    controlledClockEx->set(m_clock->current_time());
	controlledClockEx->set_rate(1.0);
	
    TimeBase::TimeT prev;
    prev = controlledClockEx->current_time();
    controlledClockEx->set(0);

    TimeBase::TimeT value;
    value = controlledClockEx->get_time_origin();

    CPPUNIT_ASSERT( prev <= value );
    */
}

///////////////////////////////////////////////////////////////////////////////

void
ControlledClockTest::multiple_virtual_clock()
{
    if (CORBA::is_nil(m_clockCatalog.in()))
        CPPUNIT_FAIL("testControlledClock:>> Unable to resolve ClockCatalog");

    ClockCatalog::ClockEntry_var clockEntry =
        m_clockCatalog->get_entry(m_clockType.c_str() );

    ControlledClockEx_var  controlledClock_1 =
        ControlledClockEx::_narrow(clockEntry->subject.in());

    if (CORBA::is_nil(controlledClock_1.in()))
        CPPUNIT_FAIL("testControlledClock:>> Unable to resolve ClockCatalog");

    ControlledClockEx_var   controlledClock_2 = 
        ControlledClockEx::_narrow(clockEntry->subject.in());

    if (CORBA::is_nil(controlledClock_2.in()))
        CPPUNIT_FAIL("testControlledClock:>> Unable to resolve ClockCatalog");

    controlledClock_1->set(0);  // reset first virtual clock
    controlledClock_2->set(0); // reset second virtual clock
    const float rate1 = 1.0;
    controlledClock_1->set_rate(rate1);
    const float rate2 = 2.0;
    controlledClock_2->set_rate(rate2);

    // ops the same controller is used than rate are the same and equal to 
    // last
    CPPUNIT_ASSERT(controlledClock_1->get_rate() ==      
        controlledClock_2->get_rate());

    const int MIN_VIRTUAL_CLOCK = 10;             
    std::string name;
    for (int n = 1; n < MIN_VIRTUAL_CLOCK; ++n)
    {
        name = m_clockType ;        
        ostringstream oss;
        oss<<n;
        name += oss.str();
        clockEntry = 
            m_clockCatalog->get_entry(name.c_str());

        ControlledClockEx_var tmp;
        tmp = ControlledClockEx::_narrow(clockEntry->subject.in());
        CPPUNIT_ASSERT(!CORBA::is_nil(tmp.in()));                
    }                     
}

///////////////////////////////////////////////////////////////////////////////

void  
ControlledClockTest::pause_resume()
{
    TimeBase::TimeT currentTime = 0;
    TimeBase::TimeT firstTime = 0;
    m_controlledClock->set_rate(1.0);
    firstTime = m_controlledClock->current_time();
    m_controlledClock->pause();
    Cdmw::OsSupport::OS::sleep(0, 10000);
    currentTime = m_controlledClock->current_time();
    CPPUNIT_ASSERT(is_equal_w_tol(firstTime,currentTime));
    m_controlledClock->resume();
    Cdmw::OsSupport::OS::sleep(0, 10000);
    currentTime = m_controlledClock->current_time();
    CPPUNIT_ASSERT(currentTime > firstTime);   
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::underflow_test_exc_fast()
{
    m_controlledClock->set_rate(1.0); // reset virtual time rate
    TimeBase::TimeT 
    	time2wait = util::seconds2TimeT(1000); // 1000 seconds
    m_controlledClock->set(time2wait);
    m_controlledClock->set_rate(-500.0); // set bigger negative rate

	int i = 10; // just to avoid infinite loop
    do // try to get an underflow
    {   
        Cdmw::OsSupport::OS::sleep(WAIT_SEC,0);
        m_controlledClock->current_time();
    } while (--i > 0 );
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::underflow_test_exc_slow()
{
    // test Underflow with slower rate

    m_controlledClock->set_rate(-0.5f); // set slower virtual time rate
    TimeBase::TimeT 
    	time2wait = util::seconds2TimeT(1); // 1 seconds
    m_controlledClock->set(time2wait); 

	int i = 10; // just to avoid infinite loop
        TimeBase::TimeT curtime;


    do // try to get an underflow
    {  
        Cdmw::OsSupport::OS::sleep(WAIT_SEC,0);
        m_controlledClock->current_time();
    } while (--i > 0 );
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::overflow_test_exc_fast()
{ 
    TimeBase::TimeT 
    	time2wait = macro::MAX_TIMET- util::seconds2TimeT(1000); // 1000 seconds
    m_controlledClock->set( time2wait );
    m_controlledClock->set_rate(500.0);

	int i = 10; // just to avoid infinite loop
    do // try to get an underflow
    {   
        Cdmw::OsSupport::OS::sleep(WAIT_SEC,0);
        m_controlledClock->current_time();
    } while (--i > 0 );
	
/*	
    int i = 1;
    do // try to get an overflow
    {   
        Cdmw::OsSupport::OS::sleep(WAIT_SEC,0);
        i++;
    } while (controlledClock_->current_time() >= 
        1000 * SEC_IN_TIME_UNITS + CDMW_TOL);
*/
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::overflow_test_exc_slow()
{ 
    // test overflow with slower rate
    TimeBase::TimeT 
    	time2wait = macro::MAX_TIMET- util::seconds2TimeT(100); // 1000 seconds
    m_controlledClock->set( time2wait );
    m_controlledClock->set_rate(50.0);    

	int i = 10; // just to avoid infinite loop
    do // try to get an underflow
    {   
        Cdmw::OsSupport::OS::sleep(WAIT_SEC,0);
        m_controlledClock->current_time();
    } while (--i > 0 );
	
#if 0
    m_controlledClock->set(0);// reset overflow situation
    m_controlledClock->set_rate(1);


    ////////////////////////////////////
    // test Overflow with slower rate //
    ////////////////////////////////////

    m_controlledClock->set_rate(0.1f); // set slower virtual time rate
    // set virtual time near to the max numeric limit (1 sec left)
    m_controlledClock->set(macro::MAX_TIMET - SEC_IN_TIME_UNITS);

    do // try to get an overflow
    {   
        Cdmw::OsSupport::OS::sleep(WAIT_SEC);
    } while (m_controlledClock->current_time() >= 
        macro::MAX_TIMET - SEC_IN_TIME_UNITS + CDMW_TOL);

    // reset virtual time for later test
    m_controlledClock->set(0);
    m_controlledClock->set_rate(1);
#endif
}

//////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::pause_except()
{
    m_controlledClock->pause();
    //The following line should throw a CORBA::BAD_INV_ORDER exception
    m_controlledClock->pause();
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::resume_except()
{
    m_controlledClock->resume();
    //The following line should throw a CORBA::BAD_INV_ORDER exception
    m_controlledClock->resume();
}

///////////////////////////////////////////////////////////////////////////////

void  
ControlledClockTest::negative_rate()
{
    TimeBase::TimeT previousTime = 0;
    m_controlledClock->set(m_clock->current_time()); // reset modify

    for(float rate=4.0; rate > -7.0; rate-=2.0)
    {
        m_controlledClock->set_rate(rate); // reset virtual time rate
        previousTime = m_controlledClock->current_time();
        // std::cout<<"Rate now is: "<<rate<<std::endl;
        for (int n=0; n < MAX_EXEC; ++n)
        {
            Cdmw::OsSupport::OS::sleep(0, 1000000);
            TimeBase::TimeT actualTime = m_controlledClock->current_time();
            TimeBase::TimeT delta_diff = delta(actualTime,previousTime);
            double delta_in_sec = util::TimeT2seconds( delta_diff );
            int sign_1 = delta_sign(actualTime,previousTime);
            int sign_2 = sign(rate);

            //std::cout<<"Delta (TimeT/s): "<< delta_diff<<"/"<<delta_in_sec<<std::endl;
            //std::cout<<"     Rate: "<<fabs(1.0e-3/rate)<<std::endl;
            double rate_to_use = rate != 0.0 ? fabs(1.0e-3/rate) : 0.0;
            CPPUNIT_ASSERT_DOUBLES_EQUAL(rate_to_use,
                delta_in_sec,
                0.2);

            //std::cout<<"delta_sign: "<< sign_1 <<std::endl;
            //std::cout<<"      sign: "<< sign_2 <<std::endl;
            CPPUNIT_ASSERT(  sign_1 == sign_2 );

            previousTime = actualTime;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::limit_condition()
{
   //   ClockCatalog::ClockEntry_var
   //      clockEntry = m_clockCatalog->get_entry(m_clockType5.c_str() );
		
	// retrieve a "clean" controlledclock
   // ControlledClockEx_var  m_controlledClock =
   //     ControlledClockEx::_narrow(clockEntry->subject.in());

   // CPPUNIT_ASSERT(!CORBA::is_nil(m_controlledClock.in()));                

    m_controlledClock->set(0);
    m_controlledClock->set_rate(1.0e-20f);
	
    m_controlledClock->set(Cdmw::clock::macro::MAX_TIMET-1);	
    CPPUNIT_ASSERT_MESSAGE("Not exception, regression Test",true);

	
    m_controlledClock->set(m_clock->current_time());
    m_controlledClock->set_rate(-1.0e-20f);
    m_controlledClock->set(1);
    m_controlledClock->set_rate(-1.0e-20f);
    CPPUNIT_ASSERT_MESSAGE("Not exception, regression Test",true);
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::real2controlled()
{    
    TimeBase::TimeT 
        virt_time,
        realTime0,
        virtualTime0;

    Cdmw::clock::ControlledClockEx_ptr 
        extControlledClock = 
            Cdmw::clock::ControlledClockEx::_narrow(m_controlledClock.in());
    CPPUNIT_ASSERT(!CORBA::is_nil(extControlledClock));
    const int rate = 10;
    m_controlledClock->set_rate(rate);
    m_controlledClock->set(m_clock->current_time());
           
    // wait one second to retrieve a new time
    Cdmw::OsSupport::OS::sleep(1,0);
    
    // retrieve actual virtual time: V(t0+1s)
    virt_time = m_controlledClock->current_time();
        
    // retrieve to,V(t0) informations
    extControlledClock->get_time_origins(realTime0,
                                         virtualTime0);
    // compute reference time 
    TimeBase::TimeT
        time_val = realTime0 + one_sec_in_TimeT;
               
    // retrieve one second in the future
    TimeBase::TimeT
        computed_time = util::real2controlled(m_controlledClock.in(), 
                                              time_val);
        
    TimeBase::TimeT 
        delta_diff = delta(virtualTime0,
                           computed_time);

    /* ----- DEBUG CODE [BEGIN] ----- *
    std::cerr<<"\nT0       Time: "<<realTime0
             <<"\nV(T0)    Time: "<<virtualTime0
             <<"\nT        Time: "<<time_val <<" diff: "<<time_val  - realTime0
             <<"\nV        Time: "<<virt_time<<" diff: "<<(virt_time - virtualTime0)/rate
             <<"\nComputed Time: "<<computed_time
             <<"\nDelta    Time: "<<delta_diff<<std::endl;
     * ----- DEBUG CODE [ END ] ----- */

    CPPUNIT_ASSERT_DOUBLES_EQUAL(delta_diff/rate,
                                 one_sec_in_TimeT,
                                 epsilon);
                                 
    // the same test in the past
    m_controlledClock->set_rate(-rate);
    m_controlledClock->set(m_clock->current_time());
    Cdmw::OsSupport::OS::sleep(1,0);    
    virt_time = m_controlledClock->current_time();
        
    extControlledClock->get_time_origins(realTime0,
                                         virtualTime0);
    time_val = realTime0 + one_sec_in_TimeT;
    
    // retrieve one second in the past                       
    computed_time = util::real2controlled(m_controlledClock.in(), 
                                          time_val);
        
    delta_diff = delta(virtualTime0,
                       computed_time);

    /* ----- DEBUG CODE [BEGIN] ----- *
    std::cerr<<"\nT0       Time: "<<realTime0
             <<"\nV(T0)    Time: "<<virtualTime0
             <<"\nT        Time: "<<time_val <<" diff: "<<time_val  - realTime0
             <<"\nV        Time: "<<virt_time<<" diff: "<<(virtualTime0 - virt_time)/rate
             <<"\nComputed Time: "<<computed_time
             <<"\nDelta    Time: "<<delta_diff<<std::endl;
     * ----- DEBUG CODE [ END ] ----- */

    CPPUNIT_ASSERT_DOUBLES_EQUAL(delta_diff/rate,
                                 one_sec_in_TimeT,
                                 epsilon);
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::real2controlled_wrong_real()
{
    TimeBase::TimeT
        time_val = m_clock->current_time();
        
    m_controlledClock->set_rate(-20.0);
    m_controlledClock->set(time_val);
    
    TimeBase::TimeT
        virt_time = m_controlledClock->current_time();
        
    util::real2controlled(m_controlledClock.in(), 
                          time_val-1);
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::controlled2real()
{   
    TimeBase::TimeT 
        virt_time,
        realTime0,
        virtualTime0;

    Cdmw::clock::ControlledClockEx_ptr 
        extControlledClock = 
            Cdmw::clock::ControlledClockEx::_narrow(m_controlledClock.in());
    CPPUNIT_ASSERT(!CORBA::is_nil(extControlledClock));
    const int rate = 10;    
    extControlledClock->set_rate(rate);
    extControlledClock->set(m_clock->current_time());
           
    // wait one second to retrieve a new time
    Cdmw::OsSupport::OS::sleep(1,0);
    
    // retrieve actual virtual time: V(t0+1s)
    virt_time = extControlledClock->current_time();
        
    // retrieve to,V(t0) informations
    extControlledClock->get_time_origins (realTime0,
                                         virtualTime0);
    // compute current time 
    TimeBase::TimeT
        time_val = realTime0 + one_sec_in_TimeT;
               
    // retrieve one second in the future
    TimeBase::TimeT
        computed_time = util::controlled2real(extControlledClock, 
                                              virt_time);
    TimeBase::TimeT 
        delta_diff = delta(realTime0,
                           computed_time);

    /* ----- DEBUG CODE [BEGIN] ----- *
    std::cerr<<"\nT0       Time: "<<realTime0
             <<"\nV(T0)    Time: "<<virtualTime0
             <<"\nT        Time: "<<time_val <<" diff: "<<time_val  - realTime0
             <<"\nV        Time: "<<virt_time<<" diff: "<<delta(virt_time,virtualTime0)/rate 
             <<"\nComputed Time: "<<computed_time
             <<"\nDelta    Time: "<<delta_diff<<std::endl;
     * ----- DEBUG CODE [ END ] ----- */

    // here time is real
    CPPUNIT_ASSERT_DOUBLES_EQUAL(delta_diff,
                                 one_sec_in_TimeT,
                                 epsilon);
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::controlled2real_wrong_virtual()
{
    TimeBase::TimeT
        time_val = m_clock->current_time();
        
    m_controlledClock->set_rate(1.0);
    m_controlledClock->set(time_val);
    
    TimeBase::TimeT
        virt_time = m_controlledClock->current_time();        
           
    util::controlled2real(m_controlledClock.in(), 
                          virt_time - util::seconds2TimeT(1));
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::controlled2real_wrong_rate()
{
    TimeBase::TimeT
        time_val = m_clock->current_time();
        
    m_controlledClock->set_rate(0.0);
    m_controlledClock->set(time_val);
    
    TimeBase::TimeT
        virt_time = m_controlledClock->current_time();        
           
    util::controlled2real(m_controlledClock.in(), 
                          virt_time);
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::current_time_with_double_rate()
{
    TimeBase::TimeT
        before,
        after; 
    const float rate = 2.0;
    m_controlledClock->set_rate(rate);
    before = m_controlledClock->current_time();
    Cdmw::OsSupport::OS::sleep(1,0);
    after = m_controlledClock->current_time();    

    /* ----- DEBUG CODE [BEGIN] ----- *
    std::cerr<<"\n   before      : "<<before
             <<"\n before + 10*1s: "<<(before + (int)(rate)*one_sec_in_TimeT)
             <<"\n   after       : "<<after
             <<"\n   epsilon     : "<<epsilon
             <<std::endl;
     * ----- DEBUG CODE [ END ] ----- */

    CPPUNIT_ASSERT_DOUBLES_EQUAL(before + (int)(rate)*one_sec_in_TimeT,
                                 after,
                                 epsilon);
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::wrong_attach_with_NONE()
{
    MyObserver* my_observer = new MyObserver();
    Cdmw::clock::ClockObserver_var observer = my_observer->_this();
 
    CORBA::Any aparam;

    ControlledClockEx_var controlledClockEx = ControlledClockEx::_narrow(m_controlledClock.in());
        
    // this assert
    controlledClockEx->attach(Cdmw::clock::NONE, observer.in(), aparam);    
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::wrong_attach_with_repetition()
{
    MyObserver* my_observer = new MyObserver();
    Cdmw::clock::ClockObserver_var observer = my_observer->_this();

    CORBA::Any aparam;

    ControlledClockEx_var controlledClockEx = ControlledClockEx::_narrow(m_controlledClock.in());
        
    controlledClockEx->attach(Cdmw::clock::SET_TIME, observer.in(), aparam);

    try
    {
        // this assert
        controlledClockEx->attach(Cdmw::clock::SET_TIME, observer.in(), aparam);
    }
    catch (const Cdmw::clock::ControlledClockEx::AlreadyPresentEntry&)
    {
        controlledClockEx->detach(Cdmw::clock::ALL, observer.in());
        throw;
    }
}

///////////////////////////////////////////////////////////////////////////////

void
ControlledClockTest::wrong_attach_with_no_inclusion()
{
    MyObserver* my_observer = new MyObserver();
    Cdmw::clock::ClockObserver_var observer = my_observer->_this();

    CORBA::Any aparam;

    ControlledClockEx_var controlledClockEx = ControlledClockEx::_narrow(m_controlledClock.in());

    controlledClockEx->attach(Cdmw::clock::SET_TIME, observer.in(), aparam);

    try
    {
        // this assert
        controlledClockEx->attach(Cdmw::clock::SET_RATE, observer.in(), aparam);
    }
    catch (const CosClockService::ControlledClock::NotSupported&)
    {
        controlledClockEx->detach(Cdmw::clock::ALL, observer.in());
        throw;
    }
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::wrong_detach_with_different_observers()
{
    MyObserver* my_observer1 = new MyObserver();
    MyObserver* my_observer2 = new MyObserver();
    Cdmw::clock::ClockObserver_var observer1 = my_observer1->_this();
    Cdmw::clock::ClockObserver_var observer2 = my_observer2->_this();

    CORBA::Any aparam;

    ControlledClockEx_var controlledClockEx = ControlledClockEx::_narrow(m_controlledClock.in());

    controlledClockEx->attach(Cdmw::clock::SET_TIME, observer1.in(), aparam);

    try
    {
        // this assert
        controlledClockEx->detach(Cdmw::clock::SET_TIME, observer2.in());
    }
    catch (const CosClockService::ControlledClock::NotSupported&)
    {
        controlledClockEx->detach(Cdmw::clock::ALL, observer1.in());
        throw;
    }
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::wrong_detach_with_NONE()
{
    MyObserver* my_observer = new MyObserver();
    Cdmw::clock::ClockObserver_var observer = my_observer->_this();

    CORBA::Any aparam;

    ControlledClockEx_var controlledClockEx = ControlledClockEx::_narrow(m_controlledClock.in());

    controlledClockEx->attach(Cdmw::clock::SET_TIME, observer.in(), aparam);

    try
    {
        // this assert
        controlledClockEx->detach(Cdmw::clock::NONE, observer.in());
    }
    catch (const CosClockService::ControlledClock::NotSupported&)
    {
        controlledClockEx->detach(Cdmw::clock::ALL, observer.in());
        throw;
    }
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::right_attach_and_detach ()
{
    MyObserver* my_observer = new MyObserver();
    Cdmw::clock::ClockObserver_var observer = my_observer->_this();

    CORBA::Any aparam1, aparam2;

    ControlledClockEx_var controlledClockEx = ControlledClockEx::_narrow(m_controlledClock.in());

    controlledClockEx->attach(Cdmw::clock::SET_TIME, observer.in(), aparam1);
    controlledClockEx->attach(Cdmw::clock::SET_TIME | Cdmw::clock::SET_RATE, observer.in(), aparam2);

    controlledClockEx->detach(Cdmw::clock::SET_TIME, observer.in());
    controlledClockEx->detach(Cdmw::clock::ALL, observer.in());
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::on_set_notify ()
{
    MyObserver* my_observer1 = new MyObserver();
    MyObserver* my_observer2 = new MyObserver();
    Cdmw::clock::ClockObserver_var observer1 = my_observer1->_this();
    Cdmw::clock::ClockObserver_var observer2 = my_observer2->_this();

    CORBA::Any aparam1, aparam2;

    ControlledClockEx_var controlledClockEx = ControlledClockEx::_narrow(m_controlledClock.in());

    controlledClockEx->attach(Cdmw::clock::SET_TIME, observer1.in(), aparam1);
    controlledClockEx->attach(Cdmw::clock::SET_TIME | Cdmw::clock::SET_RATE, observer2.in(), aparam2);

    controlledClockEx->set(m_clock->current_time());
    
    CPPUNIT_ASSERT(my_observer1->getProcessedEvents() == Cdmw::clock::SET_TIME);
    CPPUNIT_ASSERT(my_observer2->getProcessedEvents() == Cdmw::clock::SET_TIME);

    controlledClockEx->detach(Cdmw::clock::ALL, observer1.in());    
    controlledClockEx->detach(Cdmw::clock::ALL, observer2.in());    
}
 
///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::on_set_rate_notify ()
{
    MyObserver* my_observer1 = new MyObserver();
    MyObserver* my_observer2 = new MyObserver();
    Cdmw::clock::ClockObserver_var observer1 = my_observer1->_this();
    Cdmw::clock::ClockObserver_var observer2 = my_observer2->_this();

    CORBA::Any aparam1, aparam2;

    ControlledClockEx_var controlledClockEx = ControlledClockEx::_narrow(m_controlledClock.in());
        
    controlledClockEx->attach(Cdmw::clock::SET_RATE, observer1.in(), aparam1);
    controlledClockEx->attach(Cdmw::clock::SET_TIME | Cdmw::clock::SET_RATE, observer2.in(), aparam2);

    controlledClockEx->set_rate(2.0);
    
    CPPUNIT_ASSERT(my_observer1->getProcessedEvents() == Cdmw::clock::SET_RATE);
    CPPUNIT_ASSERT(my_observer2->getProcessedEvents() == Cdmw::clock::SET_RATE);

    controlledClockEx->detach(Cdmw::clock::ALL, observer1.in());    
    controlledClockEx->detach(Cdmw::clock::ALL, observer2.in());    
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::on_pause_notify ()
{
    MyObserver* my_observer1 = new MyObserver();
    MyObserver* my_observer2 = new MyObserver();
    Cdmw::clock::ClockObserver_var observer1 = my_observer1->_this();
    Cdmw::clock::ClockObserver_var observer2 = my_observer2->_this();

    CORBA::Any aparam1, aparam2;

    ControlledClockEx_var controlledClockEx = ControlledClockEx::_narrow(m_controlledClock.in());
        
    controlledClockEx->attach(Cdmw::clock::PAUSE, observer1.in(), aparam1);
    controlledClockEx->attach(Cdmw::clock::PAUSE | Cdmw::clock::RESUME, observer2.in(), aparam2);

    controlledClockEx->pause();
    
    CPPUNIT_ASSERT(my_observer1->getProcessedEvents() == Cdmw::clock::PAUSE);
    CPPUNIT_ASSERT(my_observer2->getProcessedEvents() == Cdmw::clock::PAUSE);

    controlledClockEx->detach(Cdmw::clock::ALL, observer1.in());    
    controlledClockEx->detach(Cdmw::clock::ALL, observer2.in());    
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::on_resume_notify ()
{
    MyObserver* my_observer1 = new MyObserver();
    MyObserver* my_observer2 = new MyObserver();
    Cdmw::clock::ClockObserver_var observer1 = my_observer1->_this();
    Cdmw::clock::ClockObserver_var observer2 = my_observer2->_this();

    CORBA::Any aparam1, aparam2;

    ControlledClockEx_var controlledClockEx = ControlledClockEx::_narrow(m_controlledClock.in());

    controlledClockEx->attach(Cdmw::clock::RESUME, observer1.in(), aparam1);
    controlledClockEx->attach(Cdmw::clock::PAUSE | Cdmw::clock::RESUME, observer2.in(), aparam2);

    controlledClockEx->resume();

    CPPUNIT_ASSERT(my_observer1->getProcessedEvents() == Cdmw::clock::RESUME);
    CPPUNIT_ASSERT(my_observer2->getProcessedEvents() == Cdmw::clock::RESUME);

    controlledClockEx->detach(Cdmw::clock::ALL, observer1.in());
    controlledClockEx->detach(Cdmw::clock::ALL, observer2.in());
}

///////////////////////////////////////////////////////////////////////////////

void
ControlledClockTest::on_pause_and_resume_notify ()
{
    MyObserver* my_observer1 = new MyObserver();
    MyObserver* my_observer2 = new MyObserver();
    Cdmw::clock::ClockObserver_var observer1 = my_observer1->_this();
    Cdmw::clock::ClockObserver_var observer2 = my_observer2->_this();

    CORBA::Any aparam1, aparam2;

    ControlledClockEx_var controlledClockEx = ControlledClockEx::_narrow(m_controlledClock.in());

    controlledClockEx->attach(Cdmw::clock::RESUME, observer1.in(), aparam1);
    controlledClockEx->attach(Cdmw::clock::PAUSE | Cdmw::clock::RESUME, observer2.in(), aparam2);

    controlledClockEx->pause();

    CPPUNIT_ASSERT(my_observer1->getProcessedEvents() == Cdmw::clock::NONE);
    CPPUNIT_ASSERT(my_observer2->getProcessedEvents() == Cdmw::clock::PAUSE);

    controlledClockEx->resume();

    CPPUNIT_ASSERT(my_observer1->getProcessedEvents() == Cdmw::clock::RESUME);
    CPPUNIT_ASSERT(my_observer2->getProcessedEvents() == Cdmw::clock::PAUSE | Cdmw::clock::RESUME);

    controlledClockEx->detach(Cdmw::clock::ALL, observer1.in());
    controlledClockEx->detach(Cdmw::clock::ALL, observer2.in());
}

///////////////////////////////////////////////////////////////////////////////

void ControlledClockTest::terminate_and_notify()
{
    MyObserver* my_observer1 = new MyObserver();
    MyObserver* my_observer2 = new MyObserver();
    Cdmw::clock::ClockObserver_var observer1 = my_observer1->_this();
    Cdmw::clock::ClockObserver_var observer2 = my_observer2->_this();

    CORBA::Any aparam1, aparam2;

    ControlledClockEx_var controlledClockEx = ControlledClockEx::_narrow(m_controlledClock.in());

    controlledClockEx->attach(Cdmw::clock::TERMINATE, observer1.in(), aparam1);
    controlledClockEx->attach(Cdmw::clock::PAUSE | Cdmw::clock::RESUME | Cdmw::clock::TERMINATE, observer2.in(), aparam2);

    TimeBase::TimeT previousTime = 0;
    m_controlledClock->set(m_clock->current_time()); // reset modify
    m_controlledClock->set_rate(1); // reset virtual time rate
    Cdmw::OsSupport::OS::sleep(1,0);
    m_controlledClock->terminate();
    previousTime = m_controlledClock->current_time();
    for (int n=0; n < MAX_EXEC; ++n)
    {
        Cdmw::OsSupport::OS::sleep(WAIT_SEC,0);
        TimeBase::TimeT actualTime = m_controlledClock->current_time();
        CPPUNIT_ASSERT( (actualTime - previousTime) == 0);
    }

    CPPUNIT_ASSERT(my_observer1->getProcessedEvents() == Cdmw::clock::TERMINATE);
    CPPUNIT_ASSERT(my_observer2->getProcessedEvents() == Cdmw::clock::TERMINATE);

    controlledClockEx->detach(Cdmw::clock::ALL, observer1.in());
    controlledClockEx->detach(Cdmw::clock::ALL, observer2.in());
}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
