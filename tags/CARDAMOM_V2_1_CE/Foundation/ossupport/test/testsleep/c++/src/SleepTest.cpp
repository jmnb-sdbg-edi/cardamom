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
 
#include "testsleep/SleepTest.hpp"

#include "Foundation/common/Assert.hpp"
#include "Foundation/ossupport/OS.hpp"

///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION( SleepTest );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(SleepTest, "test_sleep");

typedef unsigned long long ulonglong;

///////////////////////////////////////////////////////////////////////////////

ulonglong
get_my_time()
{
    Cdmw::OsSupport::OS::Timeval 
       tmp = Cdmw::OsSupport::OS::get_time();
    ulonglong out = tmp.seconds;
    out*=1000000;
    out+=tmp.microseconds;
    out*=10;
    return  out;
}

///////////////////////////////////////////////////////////////////////////////

SleepTest::SleepTest()
{

}

///////////////////////////////////////////////////////////////////////////////

SleepTest::~SleepTest()
{

}

///////////////////////////////////////////////////////////////////////////////


void
SleepTest::usleep()
{
	ulonglong prev = get_my_time();
	Cdmw::OsSupport::OS::sleep(0, 999999000); // 1 sec - 1 usec
	ulonglong after = get_my_time();

	ulonglong delta = after - prev;
	
	/*The size is TimeT*/
	CPPUNIT_ASSERT_DOUBLES_EQUAL(delta,
				 10000000,
				 500000);
}

///////////////////////////////////////////////////////////////////////////////

void
SleepTest::nsleep()
{
	ulonglong prev = get_my_time();
	Cdmw::OsSupport::OS::sleep(0, 999999999); // 1 sec - 1 nsec
	ulonglong after = get_my_time();
	
	ulonglong delta = after - prev;
	
	/*The size is TimeT*/
	CPPUNIT_ASSERT_DOUBLES_EQUAL(delta,
				10000000,
				 500000);								 	
}

///////////////////////////////////////////////////////////////////////////////

void 
SleepTest::usleep_exception()
{
	Cdmw::OsSupport::OS::sleep(0, 1000000000);
}

///////////////////////////////////////////////////////////////////////////////

void 
SleepTest::nsleep_exception()
{
	Cdmw::OsSupport::OS::sleep(0, 1000000000);
}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////

