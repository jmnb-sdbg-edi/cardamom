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

#ifndef CONTROLLED_CLOCK_TEST_H
#define CONTROLLED_CLOCK_TEST_H
/**
 * @file
 * @brief 
 * 
 * @author Fabrizio Morciano <fmorciano@amjsv.it>
 * @author Lello Mele <lellomele@yahoo.com>
 */

#include <cppunit/extensions/HelperMacros.h>
#include "Time/clockservice/Time.hpp"
#include "Time/clockservice/CdmwTime.stub.hpp"


/**
 * @brief Class to test ControlledClock method.
 */
class ControlledClockTest :
public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( ControlledClockTest );
        CPPUNIT_TEST( set );
        CPPUNIT_TEST( set_get_rate );
        CPPUNIT_TEST( multiple_virtual_clock );
        CPPUNIT_TEST( pause_resume );
        CPPUNIT_TEST_EXCEPTION( pause_except, CORBA::BAD_INV_ORDER );
        CPPUNIT_TEST_EXCEPTION( resume_except, CORBA::BAD_INV_ORDER );
        CPPUNIT_TEST( underflow_test_exc );
        CPPUNIT_TEST( overflow_test_exc );
        CPPUNIT_TEST( terminate );
    CPPUNIT_TEST_SUITE_END( );

    CORBA::Object_ptr obj_;
    CosClockService::ControlledClock_var controlledClock_;
    CosClockService::ClockCatalog_var clockCatalog_;
    CosClockService::Clock_var localClock_;

    std::string alias_name_;

public:
    ControlledClockTest();
    void  setUp();
 
    /**
     * @brief test virtual clock set.
     *
     */
    void  set();

    /**
     * @brief test rate setting for virtual clock.
     *
     */
    void  set_get_rate();

    /**
     * @brief test virtual clock pausing and resuming.
     *
     */
    void  pause_resume();

    /**
     * @brief test underflow of virtual clock.
     *
     */
    void overflow_test_exc();
    /**
     * @brief test underflow of virtual clock.
     *
     */
    void underflow_test_exc();

    /**
     * @brief test exception if multiple pause call.
     *
     */
    void pause_except();

    /**
     * @brief test exception if multiple resume call.
     *
     */
    void resume_except();
    /**
     * @brief test virtual clock stopping.
     *
     */
    void  terminate();

    /**
     * @brief test multiple virtual clock.
     *
     */
    void multiple_virtual_clock();

    void  tearDown();
};

// 1 second in units of 10e-7 sec
const TimeBase::TimeT SEC_IN_TIME_UNITS = 10000000; 
// tolerance of 100 microsec
const TimeBase::TimeT CDMW_TOL = (TimeBase::TimeT)1000000;
// max number of loop execution
const int MAX_EXEC = 10;
// seconds to wait in some tests
const int WAIT_SEC = 1;
// rate of second virtual clock
const int RATE_2 = 3;

#endif // CONTROLLED_CLOCK_TEST_H

