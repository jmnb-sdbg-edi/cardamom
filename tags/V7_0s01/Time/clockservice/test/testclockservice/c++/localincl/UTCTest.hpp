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

#ifndef UTCTEST_HPP
#define UTCTEST_HPP
/**
 * @brief UTC unit test case
 * 
 * @author Fabrizio Morciano <fmorciano@amjsv.it>
 * @author Raffaele Mele <rmele@progesi.it>
 */

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include "Time/clockservice/CosClockService.stub.hpp"

class UTCTest 
:   public CppUnit::TestFixture
{  
    CPPUNIT_TEST_SUITE(UTCTest);
        CPPUNIT_TEST(factory_init);
        CPPUNIT_TEST(factory_compose);
        CPPUNIT_TEST(inaccuracy);
        CPPUNIT_TEST(utc_time);
        CPPUNIT_TEST(compare_time);
        CPPUNIT_TEST(interval);
    CPPUNIT_TEST_SUITE_END();

    static  bool is_initialised_ ;
    std::string alias_name_ ;
    static CORBA::Object_ptr obj_;
    static CosClockService::UTC_init *utc_factory_;
    static  CosClockService::TimeSpan_init *tspan_factory_;

public:
    UTCTest();
    ~UTCTest();
public:
    void setUp();
    
    /**
     * @brief test UTC inaccuracy creation from inacclo and inacchi.
     *
     */
    void inaccuracy();
    
    /**
     * @brief test TimeBase::UtcT creation from a UTC object method. 
     *
     */
    void utc_time();
    
    /**
     * @brief test all UTC valuetype comparison type.
     *
     */
    void compare_time();
    
    /**
     * @brief test TimeSpan creation from a UTC valuetype.
     *
     */
    void interval();

    /**
     * @brief test UTC init() factory.
     *
     */
    void factory_init();
 
    /**
     * @brief test UTC compose() factory.
     *
     */
    void factory_compose();
    void tearDown() ;
};


#endif // UTCTEST_HPP
