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

/**
* @brief Implementation for FederatedControlledClockTest with cppUnit library.
*
* @author Fabrizio Lorenna <f.lorenna@vitrociset.it>
* @author Raffaele Mele <rmele@progesi.it>
*/

#include <string>
#include <sstream>
#include <iostream>
#include <ctime>
#include <limits>

#include "testcontrolledexecutor/FederatedControlledClockTest.hpp"
#include "Time/clockservice/test/testcommon/TimeTestManager.hpp"

#include "clockservice/FederatedControlledClock_impl.hpp"
#include "Time/clockservice/config.hpp"
#include "Time/clockservice/Macro.hpp"

using namespace CosClockService;
using namespace CosPropertyService;
using namespace Cdmw::clock;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION( FederatedControlledClockTest );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FederatedControlledClockTest, "testcontrolledexecutor");

///////////////////////////////////////////////////////////////////////////////

void
FederatedControlledClockTest::setUp()
{
    m_clockType =  "FederatedControlledClock" ;
    m_clockType1 = "FederatedControlledClock" ;
    m_clockType5 =  "FederatedControlledClock" ;

    m_alias_name = "ClockService";
    m_clock = 0;  
    Cdmw::TestUtils::CORBATestManager::instance()->add(m_alias_name);
    try
    {
        m_obj = Cdmw::TestUtils::CORBATestManager::instance()->get_object(m_alias_name);
    }
    catch(Cdmw::TestUtils::CORBATestManager::InvalidObject&)
    {
        CPPUNIT_FAIL("Cdmw::TestUtils::CORBATestManager::InvalidObject");
    }
    
    m_clockCatalog = ClockCatalog::_narrow(m_obj);
    
    if (CORBA::is_nil(m_clockCatalog.in())) {
        CPPUNIT_FAIL("testFederatedControlledClock:>> Unable to resolve ClockCatalog");
    }
    
    ClockCatalog::ClockEntry_var clockEntry =
        m_clockCatalog->get_entry("FederatedControlledClock");
    
    //in this case must enter in action the polimorphism
    m_controlledClock = ControlledClockEx::_narrow(clockEntry->subject.in());
    
    CPPUNIT_ASSERT(!CORBA::is_nil(m_controlledClock.in() ) );
    
    m_alias_name = "LocalClock";  
    Cdmw::TestUtils::CORBATestManager::instance()->add(m_alias_name);
    try
    {
        m_obj = Cdmw::TestUtils::CORBATestManager::instance()->get_object(m_alias_name);
    }
    catch(Cdmw::TestUtils::CORBATestManager::InvalidObject&)
    {
        CPPUNIT_FAIL("Cdmw::TestUtils::CORBATestManager::InvalidObject");
    }
    
    m_clock = Clock::_narrow(m_obj);
    CPPUNIT_ASSERT(!CORBA::is_nil(m_clock.in() ) );
}

///////////////////////////////////////////////////////////////////////////////

void
FederatedControlledClockTest::tearDown()
{
    try
    {
    m_controlledClock->set_rate(1.0); // reset virtual time rate
    }
   catch(...)
        {
        }


    try
    {
    m_controlledClock->set(m_clock->current_time()); // reset modify
    }
   catch(...)
        {
        }
}


///////////////////////////////////////////////////////////////////////////////
//it is necessary this empty method to override
//the father method that doe not interest
void FederatedControlledClockTest::multiple_virtual_clock()
{
}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
