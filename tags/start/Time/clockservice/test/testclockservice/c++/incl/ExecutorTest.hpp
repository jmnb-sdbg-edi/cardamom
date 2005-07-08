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

#ifndef EXECUTOR_TEST_HPP
#define EXECUTOR_TEST_HPP
/**
 * @file
 * @brief
 * @author Fabrizio Morciano
 */

#include <cppunit/extensions/HelperMacros.h>
#include "Time/clockservice/CosClockService.stub.hpp"
#include "Time/testclockservice/IExecutorTest.hpp"

class   MyPeriodicExecution;

/**
 * @brief Class to test ClockCatalog method.
 */
class ExecutorTest 
:   public CppUnit::TestFixture
,   public IExecutorTest
{
    CPPUNIT_TEST_SUITE(ExecutorTest);
        CPPUNIT_TEST(test_enable_periodic_execution);
        CPPUNIT_TEST(test_start_at);
    CPPUNIT_TEST_SUITE_END( );
    
    CORBA::Object_ptr obj_;
    CosClockService::Clock_var clock_;
    
    std::string alias_name_;
    CosClockService::PeriodicExecution::Controller_var controller_ ;
    bool    stopExecution_;
    MyPeriodicExecution *my_ob_;

public:
    ExecutorTest();

    void  setUp();
    
    void  test_enable_periodic_execution();
    void  test_start_at();
    
    void  stop();
    
    void  tearDown() ;
};

#endif // EXECUTOR_TEST_HPP

///////////////////////////////////////////////////////////////////////////////
// End Of File
///////////////////////////////////////////////////////////////////////////////

