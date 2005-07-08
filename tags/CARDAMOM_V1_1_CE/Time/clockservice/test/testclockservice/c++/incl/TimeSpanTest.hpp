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

#ifndef TIMESPANTEST_HPP
#define TIMESPANTEST_HPP
/**
 * @brief TimeSpan unit test case
 * 
 * @author Fabrizio Morciano <fmorciano@amjsv.it>
 * @author Lello Mele <lellomele@yahoo.com>
 */

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include "Time/clockservice/CdmwTime.stub.hpp"

class TimeSpanCppTest :
    public CppUnit::TestFixture
{

    CPPUNIT_TEST_SUITE( TimeSpanCppTest );
        CPPUNIT_TEST( init ) ;
        CPPUNIT_TEST( compose ) ;
        CPPUNIT_TEST( lower_bound ) ;
        CPPUNIT_TEST( upper_bound ) ;
        CPPUNIT_TEST( time_interval ) ;
        CPPUNIT_TEST( spans ) ;
        CPPUNIT_TEST( overlaps );
        CPPUNIT_TEST( time );
    CPPUNIT_TEST_SUITE_END( );

    static  bool  is_initialized_;
    std::string alias_name_ ;
    static  CORBA::Object_ptr obj_;
    static  CosClockService::TimeSpan_init *tspan_factory_;
    static  CosClockService::UTC_init *utc_factory_;

public:
    TimeSpanCppTest();
    ~TimeSpanCppTest();
public:
    void  setUp();
    void  tearDown() ;

    /**
     * @brief test TimeSpan upper_bound var member.
     *
     */
    void  upper_bound () ;


    /**
     * @brief test TimeSpan lower_bound var member.
     *
     */
    void  lower_bound () ;

    /**
     * @brief test TimeSpan init() factory.
     *
     */
    void  init();

    /**
     * @brief test TimeSpan compose() factory.
     *
     */
    void  compose();

    /**
     * @brief test TimeSpan IntervalT return type.
     *
     */
    void  time_interval () ;

    /**
     * @brief test all overlap type of a TimeSpan and a UTC.
     *
     */
    void  spans () ;


    /**
     * @brief test all overlap type of two TimeSpan.
     *
     */
    void  overlaps ();

    /**
     * @brief test UTC creation from a TimeSpan.
     *
     */
    void  time ();

};


#endif // TIMESPANTEST_HPP
