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

#ifndef	UTC_TIME_SERVICE_TEST_HPP
#define UTC_TIME_SERVICE_TEST_HPP
/**
 * @file
 * @brief 
 * 
 * @author Fabrizio Morciano <fmorciano@amjsv.it>
 * @author Lello Mele <lellomele@yahoo.com>
 */

#include <cppunit/extensions/HelperMacros.h>
#include "Time/clockservice/CosClockService.stub.hpp"

class UTCTimeServiceTest :
public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(UTCTimeServiceTest);
        CPPUNIT_TEST(univ_time_test);
        CPPUNIT_TEST(abs_time_test);
        CPPUNIT_TEST_EXCEPTION(abs_time_test_except, CORBA::DATA_CONVERSION);
    CPPUNIT_TEST_SUITE_END();
    
    std::string alias_name_ ;
    static CORBA::Object_ptr obj_;
    static CosClockService::UTC_init *utc_factory_;
    static CosClockService::Clock_var clock_;
    
    static bool is_initialized_;
    
public:
    UTCTimeServiceTest();
    ~UTCTimeServiceTest();
public:
    void  setUp();
    
    /**
     * @brief test UTCTimeService universal_time() method. Make some reading
     * of the Clock, and compare the reading.
     *
     */
    void  univ_time_test();
    
    /**
     * @brief test UTCTimeService absolute_time() method. Make reading
     * of the Clock, offset with a UTC parameter.
     *
     */
    void  abs_time_test();

    /**
     * @brief test UTCTimeService absolute_time() exception. Make reading
     * of the Clock, offset with a too big UTC parameter, throw a 
     * CORBA::DATA_CONVERSION exception.
     *
     */
    void  abs_time_test_except();
    void  tearDown() ;
};

#endif // UTC_TIME_SERVICE_TEST_HPP

