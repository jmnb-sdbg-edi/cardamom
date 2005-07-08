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

#ifndef VIRTUAL_EXECUTOR_TEST_HPP
#define VIRTUAL_EXECUTOR_TEST_HPP
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
class VirtualExecutorTest 
:   public  CppUnit::TestFixture
,   public  IExecutorTest
{
    CPPUNIT_TEST_SUITE(VirtualExecutorTest);
        CPPUNIT_TEST(enable_periodic_execution);
        CPPUNIT_TEST(delete_on_set);
    CPPUNIT_TEST_SUITE_END( );
    
    CORBA::Object_ptr obj_;
    CosClockService::Clock_var clock_;    
    std::string alias_name_;
    CosClockService::PeriodicExecution::Controller_var controller_ ;
    bool    stopExecution_;
    MyPeriodicExecution *my_ob_;

public:
    VirtualExecutorTest();

    void  setUp();
    
    void  enable_periodic_execution();

    void  delete_on_set();
    
    void  stop();
    
    void  tearDown() ;
};

#endif // _EXECUTOR_TEST_H

///////////////////////////////////////////////////////////////////////////////
// End Of File
///////////////////////////////////////////////////////////////////////////////

