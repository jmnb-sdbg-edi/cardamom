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

#include "Time/testclockservice/UtilTest.hpp"

#include "Time/clockservice/Util.hpp"

using namespace Cdmw::clock::util;
///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION( UtilTest );

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
UtilTest::test_mult()
{
    TimeBase::TimeT val1 = 10;
    double val2 = 0.5;

    CPPUNIT_ASSERT( 5.0 == mult(val1,
                                val2));
}

///////////////////////////////////////////////////////////////////////////////

void    
UtilTest::test_divide()
{
    TimeBase::TimeT val1 = 10;
    TimeBase::TimeT val2 = 100;

                                                                                                           
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.1,
								 divide(val1,
                                        val2),
                                 1e-8);
}



///////////////////////////////////////////////////////////////////////////////

void    
UtilTest::test_add()
{
    TimeBase::TimeT val1 = 101;
    double  val2 = 10E3;

    CPPUNIT_ASSERT( 10101 == add(val1,
                                 val2));
}

///////////////////////////////////////////////////////////////////////////////

void    
UtilTest::test_seconds2TimeT()
{
    CPPUNIT_ASSERT_THROW( seconds2TimeT(-1.0), std::range_error);

    CPPUNIT_ASSERT(100000  == seconds2TimeT(10*0.001 /*10 msec*/));
}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////

