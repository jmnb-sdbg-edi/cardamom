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

#ifndef CONTROLLED_EXECUTOR_TEST_HPP
#define CONTROLLED_EXECUTOR_TEST_HPP
/**
 * @author Fabrizio Morciano
 */

#include <cppunit/extensions/HelperMacros.h>
#include "Time/clockservice/CosClockService.stub.hpp"
#include "testcontrolledexecutor/ControlledClockTest.hpp"


class   MyPeriodicExecution;
/**
 * @brief Class to test ClockCatalog method.
 */
class ControlledExecutorTest_ControlledClockTest 
:  public ControlledClockTest 
{
    CPPUNIT_TEST_SUB_SUITE( ControlledExecutorTest_ControlledClockTest, ControlledClockTest);
        CPPUNIT_TEST(backward);
    CPPUNIT_TEST_SUITE_END();

    CosClockService::PeriodicExecution::Controller_var
        m_controller;

    MyPeriodicExecution*
       m_my_ob;
 
public:
    ControlledExecutorTest_ControlledClockTest();
    
    void  setUp();  
    void  tearDown() ;
    void  multiple_virtual_clock();
    void  backward();    
};

#endif // CONTROLLED_EXECUTOR_TEST_HPP

///////////////////////////////////////////////////////////////////////////////
// End Of File
///////////////////////////////////////////////////////////////////////////////

