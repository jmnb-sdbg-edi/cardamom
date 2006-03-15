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
#include "Time/clockservice/CdmwTime.stub.hpp"

class   MyPeriodicExecution;
/**
 * @brief Class to test ClockCatalog method.
 */
class ControllerTest 
:   public  CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(ControllerTest);
        CPPUNIT_TEST(start_zero);
        CPPUNIT_TEST_EXCEPTION(start_at_zero,\
                               CosClockService::PeriodicExecution::Controller::TimePast);
        CPPUNIT_TEST(start_at_zero_period);
        CPPUNIT_TEST(PCR_0450);
        //CPPUNIT_TEST(auto_controlled);
        CPPUNIT_TEST(compute_time2end);
        CPPUNIT_TEST(check_any);        
    CPPUNIT_TEST_SUITE_END();
    
protected:
	
    CORBA::Object_ptr 
        m_obj;
		
    CosClockService::Clock_var 
        m_clock;    
		
    std::string 
        m_alias_name;
		
    CosClockService::PeriodicExecution::Controller_var 
        m_controller;
	
    Cdmw::clock::ControlledExecutor_var
        m_executor;
	
    MyPeriodicExecution*
        m_my_ob;
    
public:
    ControllerTest();
    
    void  setUp();
    
    void  start_zero();
    void  start_at_zero();   
    void  start_at_zero_period();
    void  PCR_0450();
    void  auto_controlled();
    void  compute_time2end();
    void  check_any();
    void  tearDown() ;
};

#endif // CONTROLLED_EXECUTOR_TEST_HPP

///////////////////////////////////////////////////////////////////////////////
// End Of File
///////////////////////////////////////////////////////////////////////////////

