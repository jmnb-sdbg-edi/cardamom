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
* @file UTCTest.cpp
* @brief unit tests for EVoT UTC valuetype.
* 
* @author Fabrizio Morciano <fmorciano@selex-si.com>
* @author Raffaele Mele <rmele@progesi.it>
* @author Copyright (C) 2004-2005 SELEX-SI
*/
#include "testclockservice/UTCTest.hpp"
#include "clockservice/UTC_impl.hpp"
#include "clockservice/TimeSpan_impl.hpp"
#include "Time/clockservice/test/testcommon/TimeTestManager.hpp"
#include "Time/clockservice/Macro.hpp"

#include "tao/ORB_Core.h"

#include <iostream>

using namespace Cdmw::clock;
using namespace CosClockService;
using namespace std;

using Cdmw::clock::macro::Mask16;
using Cdmw::clock::macro::Mask32;
using Cdmw::clock::macro::Mask48;

///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION (UTCTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(UTCTest, "testclockservice");

///////////////////////////////////////////////////////////////////////////////

bool UTCTest::is_initialised_ = false;
UTC_init *UTCTest::utc_factory_ = 0;
TimeSpan_init *UTCTest::tspan_factory_ = 0;
CORBA::Object_ptr UTCTest::obj_;

///////////////////////////////////////////////////////////////////////////////


void  
UTCTest::setUp ()
{
}

///////////////////////////////////////////////////////////////////////////////


void
UTCTest::tearDown ()
{
}

///////////////////////////////////////////////////////////////////////////////


void
UTCTest::inaccuracy()
{
    TimeBase::TimeT 
        time = 6000;
    
    CORBA::ULong 
        inacclo = 4563,
        inacclo_;
        
    CORBA::UShort 
        inacchi= 2671,
        inacchi_;
        
    TimeBase::TdfT 
        tdf = 0;

    UTC_var
        utc = utc_factory_->compose (time, inacclo, inacchi, tdf);

    TimeBase::InaccuracyT 
        inaccuracy = utc->inaccuracy();
    TimeBase::TimeT 
        res = inaccuracy & Mask48;
        
    res&=Mask32;

    inacclo_ = static_cast<unsigned long>(res);
    res = inaccuracy >> 32;
    res &= Mask16;

    inacchi_ = static_cast<unsigned short>(res);

    CPPUNIT_ASSERT( inacclo == inacclo_ );
    CPPUNIT_ASSERT( inacchi == inacchi_ );
}

///////////////////////////////////////////////////////////////////////////////

void
UTCTest::utc_time()
{
    TimeBase::TimeT time = 1234;
    CORBA::ULong inacclo = 4321;
    CORBA::UShort inacchi= 9876;
    TimeBase::TdfT tdf = 3;

    UTC_var
        utc = utc_factory_->compose (time, inacclo, inacchi, tdf);

    TimeBase::UtcT utc_time = utc->utc_time();

    CPPUNIT_ASSERT( utc->time() == utc_time.time );
    CPPUNIT_ASSERT( utc->inacclo() == utc_time.inacclo );
    CPPUNIT_ASSERT( utc->inacchi() == utc_time.inacchi );
    CPPUNIT_ASSERT( utc->tdf() == utc_time.tdf );
}

///////////////////////////////////////////////////////////////////////////////


void  
UTCTest::compare_time()
{
    // create ad-hoc TimeSpans for this test
    TimeSpan_var tspan1 = tspan_factory_->compose(3000,9000);
    TimeSpan_var tspan2 = tspan_factory_->compose(3000,9000);

    // get UTCs from the Timespans
    UTC_var utc1 = tspan1->time();
    UTC_var utc2 = tspan2->time();

    // test MidC Comparison for TCEqualTo utc
    TimeComparison res = utc1->compare_time(MidC,utc2);
    CPPUNIT_ASSERT(res == TCEqualTo);
    res = utc2->compare_time(MidC,utc1);
    CPPUNIT_ASSERT(res == TCEqualTo);

    // test MidC Comparison for TCGreatherThan utc
    tspan2->lower_bound(2000);
    tspan2->upper_bound(8000);
    utc2 = tspan2->time();
    res = utc1->compare_time(MidC,utc2);
    CPPUNIT_ASSERT(res == TCGreaterThan);

    // test UTC MidC Comparison for TCGreatherThan
    res = utc2->compare_time(MidC,utc1);
    CPPUNIT_ASSERT(res == TCLessThan);

    // test UTC IntervalC Comparison for TCGreatherThan
    tspan2->lower_bound(4000);
    tspan2->upper_bound(8000);
    utc2 = tspan2->time();
    res = utc1->compare_time(IntervalC,utc2);
    CPPUNIT_ASSERT(res == TCGreaterThan);

    // test UTC IntervalC Comparison for TCLessThan
    res = utc2->compare_time(IntervalC,utc1);
    CPPUNIT_ASSERT(res == TCLessThan);
   
    // test UTC IntervalC Comparison for TCIndeterminate
    tspan2->lower_bound(1000);
    tspan2->upper_bound(3500);
    utc2 = tspan2->time();
    res = utc1->compare_time(IntervalC,utc2);
    CPPUNIT_ASSERT(res == TCIndeterminate);
 
    // test UTC IntervalC Comparison for TCIndeterminate
    res = utc2->compare_time(IntervalC,utc1);
    CPPUNIT_ASSERT(res == TCIndeterminate);

    // test UTC IntervalC Comparison for TCIndeterminate
    tspan2->lower_bound(1000);
    tspan2->upper_bound(2000);
    utc2 = tspan2->time();
    res = utc1->compare_time(IntervalC,utc2);
    CPPUNIT_ASSERT(res == TCIndeterminate);
    res = utc2->compare_time(IntervalC,utc1);
    CPPUNIT_ASSERT(res == TCIndeterminate);
}

///////////////////////////////////////////////////////////////////////////////

void  UTCTest::interval()
{
    TimeBase::TimeT time = 1234;
    CORBA::ULong inacclo = 4321;
    CORBA::UShort inacchi= 9876;
    TimeBase::TdfT tdf = 3;

    UTC_var
        utc = utc_factory_->compose (time, inacclo, inacchi, tdf);

    TimeSpan_var tspan = utc->interval();
    CPPUNIT_ASSERT(tspan->lower_bound() == utc->time() - utc->inaccuracy());
    CPPUNIT_ASSERT(tspan->upper_bound() == utc->time() + utc->inaccuracy());
}

///////////////////////////////////////////////////////////////////////////////

void  UTCTest::factory_init ()
{
    TimeBase::TimeT time = 2000;
    CORBA::ULong inacclo = 500;
    CORBA::UShort inacchi= 20;
    TimeBase::TdfT tdf = 3;

    TimeBase::UtcT utc_time = 
    {time,
    inacclo,
    inacchi,
    tdf};

    UTC_var 
        utc = utc_factory_->init (utc_time);

    CPPUNIT_ASSERT( utc->time() == utc_time.time );
    CPPUNIT_ASSERT( utc->inacclo() == utc_time.inacclo );
    CPPUNIT_ASSERT( utc->inacchi() == utc_time.inacchi );
    CPPUNIT_ASSERT( utc->tdf() == utc_time.tdf );
}

///////////////////////////////////////////////////////////////////////////////

void  UTCTest::factory_compose ()
{
    TimeBase::TimeT time = 2222;
    CORBA::ULong inacclo = 555;
    CORBA::UShort inacchi= 22;
    TimeBase::TdfT tdf = 2;

    UTC_var 
        utc = utc_factory_->compose (time, inacclo,inacchi,tdf);

    CPPUNIT_ASSERT( utc->time() == time );
    CPPUNIT_ASSERT( utc->inacclo() == inacclo );
    CPPUNIT_ASSERT( utc->inacchi() == inacchi );
    CPPUNIT_ASSERT( utc->tdf() == tdf );
}

///////////////////////////////////////////////////////////////////////////////

UTCTest::UTCTest()
{
    if (!is_initialised_ )
    {
        is_initialised_  = true;
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
            tspan_factory_ = new TimeSpan_impl_init;
        }
        catch (...) 
        {
            CPPUNIT_FAIL("not allocate tspan factory");
        }  
        CPPUNIT_ASSERT ( tspan_factory_ != 0);

        try 
        {
            utc_factory_ = new UTC_impl_init;
        }
        catch (...) 
        {
            CPPUNIT_FAIL("not allocate utc factory");
        }  
        CPPUNIT_ASSERT ( utc_factory_ != 0);
        TAO_OBV_REGISTER_FACTORY (UTC_impl_init, UTC);
        TAO_OBV_REGISTER_FACTORY (TimeSpan_impl_init, TimeSpan);
    }
}

///////////////////////////////////////////////////////////////////////////////

UTCTest::~UTCTest()
{
}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////

