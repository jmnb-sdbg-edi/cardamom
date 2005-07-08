/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * ========================================================================= */

/**
* @brief Implementation for ControlledClockTest with cppUnit library.
*
* @author Lello Mele <lellomele@yahoo.com>
*/

#include <string>
#include <strstream>
#include <iostream>
#include <ctime>
#include <limits>

#include "ace/streams.h"
#include "ace/Read_Buffer.h"

#include "Time/testclockservice/ControlledClockTest.hpp"
#include "Time/clockservice/ControlledClock_impl.hpp"
#include "Time/clockservice/config.hpp"
#include "Time/clockservice/Macro.hpp"

#include "Time/testclockservice/CORBAManager.hpp"

using namespace CosClockService;
using namespace CosPropertyService;
using namespace Cdmw::clock;
using namespace std;


///////////////////////////////////////////////////////////////////////////////
// -- local utility functions
///////////////////////////////////////////////////////////////////////////////
#ifdef  WIN32            
#define sleep(a)  Sleep( a * 1000 )
#endif

TimeBase::TimeT
delta(const TimeBase::TimeT num1,
      const TimeBase::TimeT num2);

int
is_equal_w_tol ( const TimeBase::TimeT num1,
                const TimeBase::TimeT num2,
                const TimeBase::TimeT tol=CDMW_TOL);

///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION( ControlledClockTest );
CPPUNIT_REGISTRY_ADD( "ControlledClockTest", "ClockTest" );

///////////////////////////////////////////////////////////////////////////////

void
ControlledClockTest::setUp()
{
    alias_name_ = "ClockService";
    localClock_ = 0;  
    CORBAManager::instance()->add(alias_name_);
    try
    {
        obj_ = CORBAManager::instance()->get_object(alias_name_);
    }
    catch(CORBAManager::InvalidObject&)
    {
        CPPUNIT_FAIL("CORBAManager::InvalidObject");
    }
    
    clockCatalog_ = ClockCatalog::_narrow(obj_);
    
    if (CORBA::is_nil(clockCatalog_.in())) {
        CPPUNIT_FAIL("testControlledClock:>> Unable to resolve ClockCatalog");
    }
    
    ClockCatalog::ClockEntry_var clockEntry =
        clockCatalog_->get_entry("ControlledClock");
    
    controlledClock_ =
        ControlledClock::_narrow(clockEntry->subject.in());
    
    CPPUNIT_ASSERT(!CORBA::is_nil(controlledClock_.in() ) );
    
    alias_name_ = "LocalClock";  
    CORBAManager::instance()->add(alias_name_);
    try
    {
        obj_ = CORBAManager::instance()->get_object(alias_name_);
    }
    catch(CORBAManager::InvalidObject&)
    {
        CPPUNIT_FAIL("CORBAManager::InvalidObject");
    }
    
    localClock_ = Clock::_narrow(obj_);
    CPPUNIT_ASSERT(!CORBA::is_nil(localClock_.in() ) );
}

///////////////////////////////////////////////////////////////////////////////

void
ControlledClockTest::tearDown()
{
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
        first_time = controlledClock_->current_time();
        int tmp = 100 * (n > 0 ? n : -n);
        new_time = tmp*SEC_IN_TIME_UNITS;
        
        if (n < 0) // remove n*100 seconds
            controlledClock_->set(first_time - new_time); 
        else // add n*100 seconds
            controlledClock_->set(first_time + new_time); 
        
        new_time = controlledClock_->current_time();
        
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
        controlledClock_->set(controlledClock_->current_time() + 
            100 * n * SEC_IN_TIME_UNITS);
    }
    controlledClock_->set_rate(1);
    controlledClock_->set(localClock_->current_time()); // reset modify
}

///////////////////////////////////////////////////////////////////////////////

void  
ControlledClockTest::set_get_rate()
{
    /*
    TimeBase::TimeT currentTime = 0;
    TimeBase::TimeT firstTime = 0;
    
    for (int rate = -MAX_EXEC; rate <= MAX_EXEC; ++rate) 
    {
        controlledClock_->set_rate(rate);
        firstTime = controlledClock_->current_time();
        sleep(WAIT_SEC);
        currentTime = controlledClock_->current_time();
        if (rate < 0)
        {
            CPPUNIT_ASSERT(is_equal_w_tol (firstTime,currentTime,(-rate) * WAIT_SEC * 
                SEC_IN_TIME_UNITS +(CDMW_TOL * (-rate))));
        }
        else if (rate == 0) // the virtual time is frozen
        {
            CPPUNIT_ASSERT(delta(firstTime,currentTime) == 0);
        }
        else
        {
            CPPUNIT_ASSERT(is_equal_w_tol (firstTime,currentTime,rate * WAIT_SEC * 
                SEC_IN_TIME_UNITS +(CDMW_TOL * rate)));
        }
        
    }
    */

    ClockCatalog::ClockEntry_var clockEntry =
        clockCatalog_->get_entry("ControlledClock1");

    ControlledClockEx_var  controlledClock_1 =
        ControlledClockEx::_narrow(clockEntry->subject.in());

    if (CORBA::is_nil(controlledClock_1.in()))
        CPPUNIT_FAIL("testControlledClock:>> Unable to resolve ClockCatalog");

    // some error with negative values
    for(int rate=1;
        rate<=100;
        ++rate)
    {   
        controlledClock_1->set(0); // reset virtual time rate
        controlledClock_1->set_rate(rate); // reset virtual time rate
        controlledClock_1->get_rate();
        CPPUNIT_ASSERT( controlledClock_1->get_rate() == rate );
    }

    controlledClock_->set_rate(1); // reset virtual time rate
    controlledClock_->set(localClock_->current_time()); // reset modify
}


///////////////////////////////////////////////////////////////////////////////

void
ControlledClockTest::multiple_virtual_clock()
{
    if (CORBA::is_nil(clockCatalog_.in()))
        CPPUNIT_FAIL("testControlledClock:>> Unable to resolve ClockCatalog");
    
    ClockCatalog::ClockEntry_var clockEntry =
        clockCatalog_->get_entry("ControlledClock");

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
        name = "ControlledClock";        
        ostringstream oss;
        oss<<n;
        name += oss.str();
        clockEntry = 
            clockCatalog_->get_entry(name.c_str());
        
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
    
    firstTime = controlledClock_->current_time();
    controlledClock_->pause();
    sleep(WAIT_SEC);
    currentTime = controlledClock_->current_time();
    CPPUNIT_ASSERT(is_equal_w_tol(firstTime,currentTime));
    controlledClock_->resume();
    sleep(WAIT_SEC);
    currentTime = controlledClock_->current_time();
    CPPUNIT_ASSERT(currentTime > firstTime);   
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::underflow_test_exc()
{
    controlledClock_->set_rate(1.0); // reset virtual time rate
    controlledClock_->set(1000*SEC_IN_TIME_UNITS);// 1000 seconds
    controlledClock_->set_rate(-100.0); // set bigger negative rate
    int i = 1;
    bool exc = false;
    try
    {
        do // try to get an underflow
        {   
            sleep(WAIT_SEC);
            i++;
        } while (controlledClock_->current_time() <= 
            1000 * SEC_IN_TIME_UNITS + CDMW_TOL);
    }
    catch (CORBA::BAD_PARAM&)
    {
        exc = true;
    }
    
    if (!exc) 
        CPPUNIT_FAIL("Underflow Exception not catched!");
    
    controlledClock_->set(1000*SEC_IN_TIME_UNITS);// reset underflow situation
    
    /////////////////////////////////////
    // test Underflow with slower rate //
    /////////////////////////////////////
    
    controlledClock_->set_rate(-0.1f); // set slower virtual time rate
    controlledClock_->set(SEC_IN_TIME_UNITS); // 1 seconds
    exc = false;
    try
    {
        do // try to get an underflow
        {   
            sleep(WAIT_SEC);
            i++;
        } while (controlledClock_->current_time() <= 
            SEC_IN_TIME_UNITS + CDMW_TOL);
    }
    catch (CORBA::BAD_PARAM&)
    {
        exc = true;
    }
    
    if (!exc) 
        CPPUNIT_FAIL("Underflow Exception not catched!");
    // reset virtual time for later test
    controlledClock_->set(1000*SEC_IN_TIME_UNITS);
    controlledClock_->set_rate(1); 
    
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::overflow_test_exc()
{    
    const TimeBase::TimeT biggerTime = Cdmw::clock::macro::MAX_TIME_VAL;

    controlledClock_->set(biggerTime - 1000 * SEC_IN_TIME_UNITS);
    controlledClock_->set_rate(100);
    int i = 1;
    bool exc = false;
    try
    {
        do // try to get an overflow
        {   
            sleep(WAIT_SEC);
            i++;
        } while (controlledClock_->current_time() >= 
            1000 * SEC_IN_TIME_UNITS + CDMW_TOL);
    }
    catch (CORBA::BAD_PARAM&)
    {
        exc = true;
    }
    
    if (!exc) 
        CPPUNIT_FAIL("Overflow Exception not catched!");
    
    controlledClock_->set(0);// reset overflow situation
    controlledClock_->set_rate(1);
    
    
    ////////////////////////////////////
    // test Overflow with slower rate //
    ////////////////////////////////////
    
    controlledClock_->set_rate(0.1f); // set slower virtual time rate
    // set virtual time near to the max numeric limit (1 sec left)
    controlledClock_->set(biggerTime - SEC_IN_TIME_UNITS);
    exc = false;
    try
    {
        do // try to get an overflow
        {   
            sleep(WAIT_SEC);
            i++;
        } while (controlledClock_->current_time() >= 
            biggerTime - SEC_IN_TIME_UNITS + CDMW_TOL);
    }
    catch (CORBA::BAD_PARAM&)
    {
        exc = true;
    }
    
    if (!exc) 
        CPPUNIT_FAIL("Overflow Exception not catched!");
    // reset virtual time for later test
    controlledClock_->set(0);
    controlledClock_->set_rate(1);
    
}

//////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::pause_except()
{
    controlledClock_->pause();
    //The following line should throw a CORBA::BAD_INV_ORDER exception
    controlledClock_->pause();
}

///////////////////////////////////////////////////////////////////////////////

void 
ControlledClockTest::resume_except()
{
    controlledClock_->resume();
    //The following line should throw a CORBA::BAD_INV_ORDER exception
    controlledClock_->resume();
}

///////////////////////////////////////////////////////////////////////////////

void  
ControlledClockTest::terminate()
{
    TimeBase::TimeT previousTime = 0;
    controlledClock_->set(localClock_->current_time()); // reset modify
    controlledClock_->set_rate(1); // reset virtual time rate
    sleep(1);
    controlledClock_->terminate();
    previousTime = controlledClock_->current_time();
    for (int n=0; n < MAX_EXEC; ++n)
    {
        sleep(WAIT_SEC);
        TimeBase::TimeT actualTime = controlledClock_->current_time();
        CPPUNIT_ASSERT( (actualTime - previousTime) == 0);
    }
}

///////////////////////////////////////////////////////////////////////////////

// compare two numbers with tolerance (default CARDAMOM tolerance)
int
is_equal_w_tol ( const TimeBase::TimeT num1,
                const TimeBase::TimeT num2,
                const TimeBase::TimeT tol)
{
    TimeBase::TimeT diff;
    diff = delta(num1,num2);
    if (diff <= tol)
        return 1; // Ok
    else
        return 0; // Too difference
}

///////////////////////////////////////////////////////////////////////////////

TimeBase::TimeT 
delta(const TimeBase::TimeT num1,const TimeBase::TimeT num2)
{
    if (num1 > num2)
        return num1 -num2;
    else
        return num2-num1;
}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
