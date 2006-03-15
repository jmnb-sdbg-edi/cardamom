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
* @file TimeSpanTest.cpp
* @brief unit test of EVoT TimeSpan valuetype implementation.
* 
* @author Fabrizio Morciano <fmorciano@selex-si.com>
* @author Raffaele Meloe <rmele@progesi.it>
* @author Copyright (C) 2004-2005 SELEX-SI
*/
#include "testclockservice/TimeSpanTest.hpp"
#include "clockservice/TimeSpan_impl.hpp"
#include "clockservice/UTC_impl.hpp"
#include "clockservice/test/testcommon/TimeTestManager.hpp"

#include <iostream>
#include "tao/ORB_Core.h"

using namespace Cdmw::clock;
using namespace CosClockService;
using namespace std;

bool  TimeSpanCppTest::is_initialized_ = false;
CosClockService::TimeSpan_init *TimeSpanCppTest::tspan_factory_ = 0;
CosClockService::UTC_init *TimeSpanCppTest::utc_factory_ = 0;

CORBA::Object_ptr TimeSpanCppTest::obj_;

///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION (TimeSpanCppTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TimeSpanCppTest, "testclockservice");

///////////////////////////////////////////////////////////////////////////////

TimeSpanCppTest::TimeSpanCppTest()
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

TimeSpanCppTest::~TimeSpanCppTest()
{
}

///////////////////////////////////////////////////////////////////////////////

void
TimeSpanCppTest::tearDown ()
{
}

///////////////////////////////////////////////////////////////////////////////

void
TimeSpanCppTest::setUp ()
{
}

///////////////////////////////////////////////////////////////////////////////

void 
TimeSpanCppTest::init()
{
    CPPUNIT_ASSERT( tspan_factory_ != 0 );
    // prepare an intervalT
    TimeBase::IntervalT from;
    from.lower_bound = 2000;
    from.upper_bound = 4000;
    // construct the TimeSpan
    TimeSpan_var tspan = tspan_factory_->init(from);
    // test valuetype construction
    CPPUNIT_ASSERT( tspan->lower_bound() == 2000);
    CPPUNIT_ASSERT( tspan->upper_bound() == 4000);

    // prepare another intervalT
    from.lower_bound = 1234;
    from.upper_bound = 4321;
    // construct the TimeSpan
    TimeSpan_var tspan1 = tspan_factory_->init(from);
    // test valuetype construction
    CPPUNIT_ASSERT( tspan1->lower_bound() == 1234);
    CPPUNIT_ASSERT( tspan1->upper_bound() == 4321);
}

///////////////////////////////////////////////////////////////////////////////

void 
TimeSpanCppTest::compose()
{  
    TimeSpan_var tspan = tspan_factory_->compose(3000,9000);
    CPPUNIT_ASSERT( tspan->lower_bound() == 3000);
    CPPUNIT_ASSERT( tspan->upper_bound() == 9000);
    TimeSpan_var tspan1 = tspan_factory_->compose(5678,9876);
    CPPUNIT_ASSERT( tspan1->lower_bound() == 5678);
    CPPUNIT_ASSERT( tspan1->upper_bound() == 9876);
}

///////////////////////////////////////////////////////////////////////////////

void  
TimeSpanCppTest::time_interval ()
{
    CPPUNIT_ASSERT( tspan_factory_ != 0 );
    // prepare an intervalT
    TimeBase::IntervalT from;
    from.lower_bound = 2000;
    from.upper_bound = 4000;
    // construct the TimeSpan
    TimeSpan_var tspan = tspan_factory_->init(from);
    // get an IntervalT from valuetype  
    TimeBase::IntervalT to;
    to = tspan->time_interval();
    // test valuetype construction
    CPPUNIT_ASSERT( to.lower_bound == from.lower_bound);
    CPPUNIT_ASSERT( to.upper_bound == from.upper_bound);
}

///////////////////////////////////////////////////////////////////////////////

void  
TimeSpanCppTest::spans ()
{
    CPPUNIT_ASSERT( tspan_factory_ != 0 );
    CPPUNIT_ASSERT( utc_factory_ != 0 );
    // create an useful UTC 
    UTC_var
        utc1 = utc_factory_->compose (2000,100,0,0);
    // TimeSpan out parameter
    TimeSpan_var tspan_out;

    // test for OTContainer
    //   +-----+  :utc
    //  +-------+ :tspan
    TimeSpan_var tspan1 = 
        tspan_factory_->compose(utc1->time() - utc1->inaccuracy() - 1,
        utc1->time() + utc1->inaccuracy() + 1);
    CPPUNIT_ASSERT(tspan1->spans(utc1,tspan_out)==OTContainer);
    // test TimeSpan out parameter
    CPPUNIT_ASSERT( (tspan_out->lower_bound()==utc1->interval()->lower_bound()) &&
                    (tspan_out->upper_bound()==utc1->interval()->upper_bound()) );

    // test for OTContained
    //  +-----+ :utc
    //   +---+  :tspan
    TimeSpan_var tspan2 = 
        tspan_factory_->compose(utc1->time() - utc1->inaccuracy() + 1,
        utc1->time() + utc1->inaccuracy() - 1);
    CPPUNIT_ASSERT(tspan2->spans(utc1,tspan_out)==OTContained);
    // test TimeSpan out parameter
    CPPUNIT_ASSERT( (tspan_out->lower_bound()==tspan2->lower_bound()) &&
                    (tspan_out->upper_bound()==tspan2->upper_bound()) );

    // test for OTOverlap
    //  +-----+   :utc
    //       +--+ :tspan
    TimeSpan_var tspan3 = 
        tspan_factory_->compose(utc1->time() + utc1->inaccuracy() - 10,
        utc1->time() + utc1->inaccuracy() + 100);
    CPPUNIT_ASSERT(tspan3->spans(utc1,tspan_out)==OTOverlap);
    // test TimeSpan out parameter
    CPPUNIT_ASSERT( (tspan_out->lower_bound()==tspan3->lower_bound()) &&
                    (tspan_out->upper_bound()==utc1->interval()->upper_bound()) );

    // test for OTNoOverlap
    //  +-----+      :utc
    //          +--+ :tspan
    TimeSpan_var tspan4 = 
        tspan_factory_->compose(utc1->time() + utc1->inaccuracy() + 10,
        utc1->time() + utc1->inaccuracy() + 100);
    CPPUNIT_ASSERT(tspan4->spans(utc1,tspan_out)==OTNoOverlap);
    // test TimeSpan out parameter
    CPPUNIT_ASSERT( (tspan_out->lower_bound()==utc1->interval()->upper_bound()) &&
                    (tspan_out->upper_bound()==tspan4->lower_bound()) );
}

///////////////////////////////////////////////////////////////////////////////

void  
TimeSpanCppTest::overlaps ()
{
    //create TimeSpan for overlap test
    TimeSpan_var tspan1 = tspan_factory_->compose(3,9);
    TimeSpan_var tspan2 = tspan_factory_->compose(5,8);
    TimeSpan_var tspan_out;

    // test for OTContainer
    //  +-----+     :tspan1
    //    +--+      :tspan2
    OverlapType ret_type = 
        tspan1->overlaps(tspan2.in(),tspan_out );
    CPPUNIT_ASSERT( ret_type == OTContainer );
    CPPUNIT_ASSERT( tspan_out->lower_bound() == 5 && 
        tspan_out->upper_bound() == 8 );
    // test TimeSpan out parameter
    CPPUNIT_ASSERT( (tspan_out->lower_bound()==tspan2->lower_bound()) &&
                    (tspan_out->upper_bound()==tspan2->upper_bound()) );

    // test for OTContained
    //  +-----+     :tspan1
    //    +--+      :tspan2
    CPPUNIT_ASSERT(tspan2->overlaps(tspan1,tspan_out) == OTContained);
    // test TimeSpan out parameter
    CPPUNIT_ASSERT( (tspan_out->lower_bound()==tspan2->lower_bound()) &&
                    (tspan_out->upper_bound()==tspan2->upper_bound()) );

    // test for OTOverlap
    //  +-----+     :tspan1
    //      +---+   :tspan3
    TimeSpan_var tspan3 = tspan_factory_->compose(7,11);
    CPPUNIT_ASSERT(tspan1->overlaps(tspan3,tspan_out) == OTOverlap);
    CPPUNIT_ASSERT(tspan3->overlaps(tspan1,tspan_out) == OTOverlap);
    // test TimeSpan out parameter
    CPPUNIT_ASSERT( (tspan_out->lower_bound()==tspan3->lower_bound()) &&
                    (tspan_out->upper_bound()==tspan1->upper_bound()) );

    // test for OTNoOverlap
    //  +-----+      :tspan1
    //          +--+ :tspan4
    TimeSpan_var tspan4 = tspan_factory_->compose(13,16);
    CPPUNIT_ASSERT(tspan1->overlaps(tspan4,tspan_out) == OTNoOverlap);
    CPPUNIT_ASSERT(tspan4->overlaps(tspan1,tspan_out) == OTNoOverlap);
    // test TimeSpan out parameter
    CPPUNIT_ASSERT( (tspan_out->lower_bound()==tspan1->upper_bound()) &&
                    (tspan_out->upper_bound()==tspan4->lower_bound()) );

    // test for the same interval
    //  +-----+ :tspan1
    //  +-----+ :tspan5
    TimeSpan_var tspan5 = tspan_factory_->compose(3,9);
    CPPUNIT_ASSERT(tspan1->overlaps(tspan5,tspan_out) == OTContainer);
    // test TimeSpan out parameter
    CPPUNIT_ASSERT( (tspan_out->lower_bound()==tspan1->lower_bound()) &&
                    (tspan_out->upper_bound()==tspan1->upper_bound()) );
    CPPUNIT_ASSERT(tspan5->overlaps(tspan1,tspan_out) == OTContainer);
    // test TimeSpan out parameter
    CPPUNIT_ASSERT( (tspan_out->lower_bound()==tspan5->lower_bound()) &&
                    (tspan_out->upper_bound()==tspan5->upper_bound()) );
}

///////////////////////////////////////////////////////////////////////////////

void  
TimeSpanCppTest::time ()
{
    CPPUNIT_ASSERT( tspan_factory_ != 0 );
    TimeSpan_var tspan = tspan_factory_->compose(1000,9000);
    UTC_var utc = tspan->time();

    TimeBase::TimeT intervMidp = (tspan->lower_bound() + tspan->upper_bound())/2L; 

    TimeBase::TimeT intervInacc = intervMidp - tspan->lower_bound();

    CPPUNIT_ASSERT(utc->inaccuracy() == intervInacc);
    CPPUNIT_ASSERT(utc->time() == intervMidp);
}

///////////////////////////////////////////////////////////////////////////////

void  
TimeSpanCppTest::upper_bound ()
{
    TimeSpan_var tspan = tspan_factory_->compose(3000,9000);
    CPPUNIT_ASSERT( tspan->upper_bound() == 9000);
    TimeSpan_var tspan1 = tspan_factory_->compose(5678,9876);
    CPPUNIT_ASSERT( tspan1->upper_bound() == 9876);

}

///////////////////////////////////////////////////////////////////////////////

void  
TimeSpanCppTest::lower_bound ()
{
    TimeSpan_var tspan = tspan_factory_->compose(3000,9000);
    CPPUNIT_ASSERT( tspan->lower_bound() == 3000);
    TimeSpan_var tspan1 = tspan_factory_->compose(5678,9876);
    CPPUNIT_ASSERT( tspan1->lower_bound() == 5678);

}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
