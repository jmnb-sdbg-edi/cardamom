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
 
#include "testclockservice/UtilTest.hpp"

#include "Time/clockservice/Util.hpp"
#include "Time/clockservice/Time.hpp"
#include "clockservice/Clock_impl.hpp"

#include <Foundation/common/Assert.hpp>

using namespace Cdmw::clock;
const double tolerance = 1.0e-8;

///////////////////////////////////////////////////////////////////////////////

static TimeBase::TimeT
local_compute_current_time()
{
    TimeBase::TimeT out;
    struct      timeval  tv;
    
    if (gettimeofday(&tv, 0) < 0)
        throw CosClockService::TimeUnavailable();
        
    if (Cdmw::clock::util::timeval2TimeT(tv, out) != 0)
        throw CosClockService::TimeUnavailable();    

    return out;
}

///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION( UtilTest );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(UtilTest, "testclockservice");

///////////////////////////////////////////////////////////////////////////////

UtilTest::UtilTest()
{

}

///////////////////////////////////////////////////////////////////////////////

UtilTest::~UtilTest()
{

}

///////////////////////////////////////////////////////////////////////////////

void    
UtilTest::mult()
{
    TimeBase::TimeT val1 = 10;
    double val2 = 0.5;

    CPPUNIT_ASSERT( 5.0 == util::mult(val1,
                                val2));
}

///////////////////////////////////////////////////////////////////////////////

void    
UtilTest::divide()
{
    TimeBase::TimeT val1 = 10;
    TimeBase::TimeT val2 = 100;

                                                                                                           
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.1,
                                 util::divide(val1,
                                 val2),
                                 1e-8);
}



///////////////////////////////////////////////////////////////////////////////

void    
UtilTest::add()
{
    TimeBase::TimeT val1 = 101;
    double  val2 = 10E3;

    CPPUNIT_ASSERT( 10101 == util::add(val1,
                                 val2));
}

///////////////////////////////////////////////////////////////////////////////

void    
UtilTest::seconds2TimeT()
{
    CPPUNIT_ASSERT_THROW( util::seconds2TimeT(-1.0), std::range_error);

    CPPUNIT_ASSERT(100000  == util::seconds2TimeT(10*0.001 /*10 msec*/));

    double
        in_val = 7654321e-7;
    TimeBase::TimeT
        ref = FIX_ULL(7654321);
    
    CPPUNIT_ASSERT( ref == util::seconds2TimeT(in_val));
    ref = FIX_ULL(3000000000);

    CPPUNIT_ASSERT( ref == util::seconds2TimeT( 300.0 ));

    in_val = 3007654321e-7;
    TimeBase::TimeT
        out = util::seconds2TimeT(in_val);
    CPPUNIT_ASSERT(FIX_ULL(3007654321) == out );
    
    TimeBase::TimeT max_range = FIX_ULL(170000000000);
    double max_value = max_range;
    TimeBase::TimeT rounded_max = (TimeBase::TimeT)(max_value);
    
    CPPUNIT_ASSERT( rounded_max<=max_range);
    out = util::seconds2TimeT( max_value );    
    CPPUNIT_ASSERT( rounded_max*FIX_ULL(10000000) == out);    
}

///////////////////////////////////////////////////////////////////////////////

void
UtilTest::TimeT2seconds()
{
    double awaited = 1.0e-7;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(util::TimeT2seconds(1), 
                                 awaited,
                                 awaited * tolerance  );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(util::TimeT2seconds(10000000),
                                 1.0,
                                 tolerance);
}

///////////////////////////////////////////////////////////////////////////////

void 
UtilTest::compute_current_time()
{
    TimeBase::TimeT val1 = local_compute_current_time();
    TimeBase::TimeT val2 = Cdmw::clock::compute_current_time();

//    std::cerr<<"Val1: "<<val1<<std::endl;
//    std::cerr<<"Val2: "<<val2<<std::endl;

    CPPUNIT_ASSERT( val2 >= val1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(val1,
                                 val2,
                                 100);    
}

///////////////////////////////////////////////////////////////////////////////

void 
UtilTest::daylight()
{
    CPPUNIT_ASSERT(false);
}

///////////////////////////////////////////////////////////////////////////////

void 
UtilTest::displacement()
{
    CPPUNIT_ASSERT(false);
}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////

