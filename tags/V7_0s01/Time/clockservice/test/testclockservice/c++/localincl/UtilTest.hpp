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

#ifndef UTILTEST_HPP
#define UTILTEST_HPP

/**
 * @author Fabrizio Morciano <fmorciano@amjsv.it>
 */

#include <cppunit/extensions/HelperMacros.h>
#include "Time/clockservice/CosClockService.stub.hpp"
#include <stdexcept>

class UtilTest :
public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(UtilTest);
        CPPUNIT_TEST(mult);
        CPPUNIT_TEST(divide);
        CPPUNIT_TEST(add);
        CPPUNIT_TEST(seconds2TimeT);
        CPPUNIT_TEST(TimeT2seconds);
        CPPUNIT_TEST(compute_current_time);
    CPPUNIT_TEST_SUITE_END();
public:
	UtilTest();
	virtual ~UtilTest();
    void mult();
    void divide();
    void add();
    void seconds2TimeT();
    void TimeT2seconds();
    void clock2controlled();
    void controlled2clock();    
    void compute_current_time();

    void daylight();
    void displacement();
};

#endif // !defined(UTILTEST_HPP)
