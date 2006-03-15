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

#ifndef CONTROLLED_CLOCK_TEST_H
#define CONTROLLED_CLOCK_TEST_H

/**
  * @author Fabrizio Morciano <fmorciano@amjsv.it>
  * @author Raffaele Mele <rmele@progesi.it>
  */

#include <cppunit/extensions/HelperMacros.h>
#include "Time/clockservice/CdmwTime.stub.hpp"
#include "Time/clockservice/test/testcommon/ClockTest.hpp"

/**
 * @brief Class to test ControlledClock method.
 */
class ControlledClockTest 
:  public ClockTest
{
    CPPUNIT_TEST_SUB_SUITE( ControlledClockTest, ClockTest );
        CPPUNIT_TEST( set );
        CPPUNIT_TEST( set_and_get_rate );
        CPPUNIT_TEST( current_time_with_double_rate );
        CPPUNIT_TEST( real2controlled );
        CPPUNIT_TEST_EXCEPTION( real2controlled_wrong_real,\
                                CosClockService::TimeUnavailable );
        CPPUNIT_TEST( controlled2real );
        CPPUNIT_TEST_EXCEPTION( controlled2real_wrong_virtual,\
                                CosClockService::TimeUnavailable );
        CPPUNIT_TEST_EXCEPTION( controlled2real_wrong_rate,\
                                CosClockService::TimeUnavailable );        
        CPPUNIT_TEST( negative_rate );
        CPPUNIT_TEST( limit_condition );        
        CPPUNIT_TEST( multiple_virtual_clock );
        CPPUNIT_TEST( pause_resume );
        CPPUNIT_TEST_EXCEPTION( pause_except,\
                                CORBA::BAD_INV_ORDER );
        CPPUNIT_TEST_EXCEPTION( resume_except,\
                                CORBA::BAD_INV_ORDER );
        CPPUNIT_TEST_EXCEPTION( underflow_test_exc_slow,\
                                CORBA::BAD_PARAM );
        CPPUNIT_TEST_EXCEPTION( underflow_test_exc_fast,\
                                CORBA::BAD_PARAM );
        CPPUNIT_TEST_EXCEPTION( overflow_test_exc_slow,\
                                CORBA::BAD_PARAM );
        CPPUNIT_TEST_EXCEPTION( overflow_test_exc_fast,\
                                CORBA::BAD_PARAM );
        CPPUNIT_TEST( get_time_origin );
              
        // verify that observer works
        CPPUNIT_TEST_EXCEPTION( wrong_attach_with_NONE,\
                                Cdmw::clock::ControlledClockEx::AlreadyPresentEntry );
        CPPUNIT_TEST_EXCEPTION( wrong_attach_with_repetition,\
                                Cdmw::clock::ControlledClockEx::AlreadyPresentEntry );
        CPPUNIT_TEST_EXCEPTION( wrong_attach_with_no_inclusion,\
                                CosClockService::ControlledClock::NotSupported );
        CPPUNIT_TEST_EXCEPTION( wrong_detach_with_different_observers,\
                                CosClockService::ControlledClock::NotSupported );
        CPPUNIT_TEST_EXCEPTION( wrong_detach_with_NONE,\
                                CosClockService::ControlledClock::NotSupported );
        CPPUNIT_TEST( right_attach_and_detach );
        CPPUNIT_TEST( on_set_notify );
        CPPUNIT_TEST( on_set_rate_notify );
        CPPUNIT_TEST( on_pause_notify );
        CPPUNIT_TEST( on_resume_notify );
        CPPUNIT_TEST( on_pause_and_resume_notify );
        CPPUNIT_TEST( terminate_and_notify );
    CPPUNIT_TEST_SUITE_END( );

protected:

    CosClockService::ControlledClock_var 
        m_controlledClock;
        
    CosClockService::ClockCatalog_var 
        m_clockCatalog;
        
    std::string 
        m_alias_name;
        
    //type of clock "ControlledClock" or "Federated..."
    std::string  
        m_clockType;

    //type of clock "ControlledClock1" or "Federated..."
    std::string 
        m_clockType1 ;

    //type of clock "ControlledClock5" or "Federated..."
    std::string 
        m_clockType5; 

public:
    ControlledClockTest();
    
    void setUp();
    void tearDown();
	
    void set();
    void set_and_get_rate();
    void current_time_with_double_rate();
    void real2controlled();
    void real2controlled_wrong_real();
    void controlled2real();    
    void controlled2real_wrong_virtual();
    void controlled2real_wrong_rate();
    void negative_rate();
    void limit_condition();
    virtual void multiple_virtual_clock();
    void pause_resume();
    void pause_except();
    void resume_except();
    void underflow_test_exc_slow();
    void underflow_test_exc_fast();
    void overflow_test_exc_slow();
    void overflow_test_exc_fast();
    void get_time_origin();

    // test cases for clock observers
    void wrong_attach_with_NONE();
    void wrong_attach_with_repetition();
    void wrong_attach_with_no_inclusion();
    void wrong_detach_with_different_observers();
    void wrong_detach_with_NONE();
    void right_attach_and_detach();
    void on_set_notify();
    void on_set_rate_notify();
    void on_pause_notify();
    void on_resume_notify();
    void on_pause_and_resume_notify();
    void terminate_and_notify();
};

#endif // CONTROLLED_CLOCK_TEST_H

