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

#ifndef CLOCK_TEST_HPP
#define CLOCK_TEST_HPP
/**
 * @file
 * @brief 
 * 
 * @author Fabrizio Morciano <fmorciano@amjsv.it>
 * @author Lello Mele <lellomele@yahoo.com>
 */

#include <cppunit/extensions/HelperMacros.h>
#include "Time/clockservice/CosClockService.stub.hpp"

/**
 * @brief Class to test ClockCatalog method.
 */
class ClockTest :
public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( ClockTest );
        CPPUNIT_TEST( current_time );
        CPPUNIT_TEST( properties );
    CPPUNIT_TEST_SUITE_END( );

protected:
    CORBA::Object_ptr obj_;
    CosClockService::Clock_var clock_;
    std::string alias_name_;

public:
    ClockTest();
    void  setUp();
  
  
    /**
     * @brief test current reading of clock.
     *
     */
    void  current_time();

    /**
     * @brief test properties of clock.
     *
     */
    void  properties();

    void  tearDown() ;
};

#endif // CLOCK_TEST_HPP

