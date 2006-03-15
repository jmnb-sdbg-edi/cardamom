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
                                                                                                                            
#ifndef CDMW_TIME_TEST_HPP
#define CDMW_TIME_TEST_HPP

/**
 * @file TimeTest.hpp
 * @brief some tests on the functions get_tm, asctime_r and get_time_t of Cdmw::clock::util namespace
 * 
 * @author Fabrizio Lorenna <f.lorenna@vitrociset.it>
 * @author Marco Macellari <mmacellari@progesi.it>
 */

#include <cppunit/extensions/HelperMacros.h>
#include "Time/clockservice/CosClockService.stub.hpp"

/**
 * @brief Class to test get_tm, asctime_r and get_time_t of Cdmw::clock::util namespace
 */
class TimeTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TimeTest );
        CPPUNIT_TEST( cdmw_tm_time_base );
        CPPUNIT_TEST( timeval_time_base );
        CPPUNIT_TEST( cdmw_tm_time_dates );
        CPPUNIT_TEST( timeval_time_dates );
        CPPUNIT_TEST( ret_values );
        CPPUNIT_TEST( daylight_saving );
        CPPUNIT_TEST( cdmw_year_of_time_base );
        CPPUNIT_TEST( cdmw_year_vs_linux_year );
    CPPUNIT_TEST_SUITE_END();


public:

    /**
     * @brief test if the functions TimeT2cdmw_tm, cdmw_tm2TimeT and asctime_r are ok compared with TIME_BASE of CARDAMOM.
     *        TIME_BASE is a TimeT time since Fri 15-OCT-1582 00:00:00,000,000 to 1-JAN-1970 00:00:00.
     */
    void cdmw_tm_time_base();

    /**
     * @brief test if the functions TimeT2timeval, timeval2TimeT and asctime_r are ok compared with TIME_BASE of CARDAMOM.
     *        TIME_BASE is a TimeT time since Fri 15-OCT-1582 00:00:00,000,000 to 1-JAN-1970 00:00:00.
     */
    void timeval_time_base();

    /**
     * @brief test if the functions TimeT2cdmw_tm, cdmw_tm2TimeT and asctime_r are ok
     *        comparing 15 dates taken from reference by boost library
     */
    void cdmw_tm_time_dates();

    /**
     * @brief test if the functions TimeT2timeval, timeval2TimeT and asctime_r are ok
     *        comparing 15 dates taken from reference by boost library
     */
    void timeval_time_dates();

    /**
     * @brief test the function cdmw_tm2TimeT for return values
     */
    void ret_values();

    /**
     * @brief test the returned flag daylight saving time
     */
    void daylight_saving();

    /**
     * @brief test if the cardamom year of the Epoch is 70
     */
    void cdmw_year_of_time_base();

    /**
     * @brief test if, for the current time, the cardamom and linux years are equal
     */
    void cdmw_year_vs_linux_year();
  
    void tearDown() ;
    void setUp() ;
};

#endif  // CDMW_UTIL_TIME_TEST_HPP

