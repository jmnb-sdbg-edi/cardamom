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
 * @file UTCTimeServiceTest.cpp
 * @brief unit tests for UTCTimeService
 * 
 * @author Fabrizio Morciano
 * @author Raffaele Mele
 * @author Copyright (C) 2004-2005 SELEX-SI
 */

#include "testclockservice/UTCTimeServiceTest.hpp"
#include "Time/clockservice/test/testcommon/PerformanceManager.hpp"
#include "Time/clockservice/test/testcommon/TimeTestManager.hpp"

#include "clockservice/UTCTimeService_impl.hpp"
#include "clockservice/UTC_impl.hpp"
#include "Time/clockservice/Macro.hpp"

#include "Foundation/common/Assert.hpp"

#include "tao/ORB_Core.h"
	
#include <iostream>
#include <iterator>
#include <algorithm>
#include <limits>
#include <bitset>

using namespace std;
using namespace CosClockService;
using namespace Cdmw::clock;

///////////////////////////////////////////////////////////////////////////////

bool  UTCTimeServiceTest::is_initialized_ = false;
CosClockService::UTC_init *UTCTimeServiceTest::utc_factory_ = 0;
CosClockService::Clock_var UTCTimeServiceTest::clock_=0;
CORBA::Object_ptr UTCTimeServiceTest::obj_ = 0;

///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION( UTCTimeServiceTest );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(UTCTimeServiceTest, "testclockservice");

///////////////////////////////////////////////////////////////////////////////


UTCTimeServiceTest::UTCTimeServiceTest()
{
    if( !is_initialized_ )
    {
        is_initialized_ = true;
        alias_name_ = "ClockService";
        Cdmw::TestUtils::CORBATestManager::instance ()->add (alias_name_);
        
        bool exce = false;
        try
        {
            obj_ = Cdmw::TestUtils::CORBATestManager::instance ()->get_object (alias_name_);
        }
        catch (Cdmw::TestUtils::CORBATestManager::InvalidObject &)
        {
            exce = true;
        }
        catch ( ... )
        {
            exce = true;
        }
        
        if( exce )
            CPPUNIT_FAIL ("Cdmw::TestUtils::CORBATestManager::InvalidObject");
        
        try 
        {
            UTCTimeServiceTest::utc_factory_ = new UTC_impl_init;
        }
        catch (...) 
        {
            CPPUNIT_FAIL("not allocate utc factory");
        }  
        CPPUNIT_ASSERT ( utc_factory_ != 0);
        TAO_OBV_REGISTER_FACTORY (UTC_impl_init, UTC );

        /*
        Cdmw::TestUtils::CORBATestManager::instance ()->get_ORB ()->
            register_value_factory (utc_factory_->tao_repository_id (),
            utc_factory_);
        */

        ClockCatalog_var clock_catalog = 
            CosClockService::ClockCatalog::_narrow(obj_);
        CPPUNIT_ASSERT( !CORBA::is_nil( clock_catalog.in() ) );
        
        ClockCatalog::ClockEntry_var 
            entry = clock_catalog->get_entry("UTCTimeService");
        clock_ = entry->subject;
        CPPUNIT_ASSERT( !CORBA::is_nil( clock_.in() ) );
    }
}

///////////////////////////////////////////////////////////////////////////////

UTCTimeServiceTest::~UTCTimeServiceTest()
{
    // No-Op
}

///////////////////////////////////////////////////////////////////////////////

void
UTCTimeServiceTest::setUp()
{
    // No-Op
}

///////////////////////////////////////////////////////////////////////////////

/**
 * Check if time is not decreasing.
 */
void 
UTCTimeServiceTest::univ_time_test()
{
    CPPUNIT_ASSERT(!CORBA::is_nil( clock_.in()));
    UtcTimeService_var 
        utc_time = UtcTimeService::_narrow(clock_.in());
    
    CosClockService::UTC_var
        timer =utc_time->universal_time();
    
    TimeBase::TimeT 
        tmp = timer->time();

    int i = 100;    

    //  CosClockService::UTC_var    timer2;    
    //  UtcTimeService_var utc_time2 = UtcTimeService::_narrow(clock_.in());

    TimeBase::TimeT 
        tmp2, delta;

    while (--i)
    {
        timer=utc_time->universal_time();
        tmp2 = timer->time();
        delta = tmp2 - tmp; 
        CPPUNIT_ASSERT( tmp2 > tmp );
        PERFORMANCE_ASSERT( tmp2 - tmp <= 100000 /*10 usec*/ );
        tmp = tmp2;
    }
}

///////////////////////////////////////////////////////////////////////////////

void 
UTCTimeServiceTest::abs_time_test_except()
{
    TimeBase::TimeT time = Cdmw::clock::macro::MAX_TIMET;
    CORBA::ULong inacclo = 1;
    CORBA::UShort inacchi = 2;
    TimeBase::TdfT tdf = 3;
    
    // CPPUNIT_ASSERT( utc_factory_ != 0 );
    //create an UTC with bigger value of time data member
    UTC_var utc1;
    utc1 = utc_factory_->compose (time, inacclo, inacchi, tdf);
    
    //obtain address of servant from the ORB
    CPPUNIT_ASSERT( !CORBA::is_nil( clock_.in()));
    UtcTimeService_var utc_time = UtcTimeService::_narrow(clock_.in());
    
    //The following line should throw a CORBA::DATA_CONVERSION exception
    //for overflow of UTC::time data member
    UTC_var timer;
    timer = utc_time->absolute_time(utc1);
    CDMW_ASSERT(timer);
    CPPUNIT_FAIL("Error not exception caught"); //to prevent unused variable warning
}
///////////////////////////////////////////////////////////////////////////////

void 
UTCTimeServiceTest::abs_time_test()
{
    TimeBase::TimeT time = Cdmw::clock::macro::MAX_TIMET>>1;
    CORBA::ULong inacclo = 1;
    CORBA::UShort inacchi= 2;
    TimeBase::TdfT tdf = 3;
    
    //obtain address of servant from the ORB
    CPPUNIT_ASSERT( !CORBA::is_nil( clock_.in()));
    UtcTimeService_var utc_time;
    utc_time = UtcTimeService::_narrow(clock_.in());

    try
    {
        int i = 50;
        while (--i) 
        {
            //create an UTC with bigger value of time data member
            UTC_var utc1;
            utc1 = utc_factory_->compose (time, inacclo, inacchi, tdf);
            
            UTC_var timer;
            timer = utc_time->absolute_time(utc1);
            CPPUNIT_ASSERT(timer->time());
        }
    }
    catch( CORBA::SystemException& e )
    {
        ostringstream oss;
        oss<<"CORBA::SystemException: "<<e._name();
        CPPUNIT_FAIL(oss.str());
    }
    catch(...)
    {
    }
}

///////////////////////////////////////////////////////////////////////////////

void  
UTCTimeServiceTest::tearDown()
{
    // No-Op
}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////

