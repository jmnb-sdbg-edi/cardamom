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
 * @brief _implementation for FederatedControllerEnforceDeadlinePolicyTest with cppUnit library.
 *
 * @author Francesca Annunziata<fannunziata@selex-si.com>, 
 * @author Fabrizio Morciano <fmorciano@selex-si.com>
 * @author Marco Macellari <mmacellari@progesi.it>
 */

#include "testpolicies/FederatedControllerEnforceDeadlinePolicyTest.hpp"
#include "clockservice/ControllerUpdateAdmin_impl.hpp"

#include <string>
#include <iostream>
#include <sstream>

#include "Time/clockservice/test/testcommon/TimeTestManager.hpp"
#include "Time/clockservice/Util.hpp"
#include "Time/clockservice/Time.hpp"

using namespace CosClockService;
using namespace CosPropertyService;
using namespace Cdmw::clock;
using namespace Cdmw::clock::util;
using namespace CosClockService::PeriodicExecution;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION(FederatedControllerEnforceDeadlinePolicyTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FederatedControllerEnforceDeadlinePolicyTest, "testenforcedeadlinepolicy");

///////////////////////////////////////////////////////////////////////////////

FederatedControllerEnforceDeadlinePolicyTest::FederatedControllerEnforceDeadlinePolicyTest()
{  
}

///////////////////////////////////////////////////////////////////////////////

void  
FederatedControllerEnforceDeadlinePolicyTest::setUp()
{
    try
    {
        Cdmw::TestUtils::CORBATestManager::instance ()->add ("ClockService");    
        m_obj = Cdmw::TestUtils::CORBATestManager::instance ()->get_object ("ClockService");
       
        ClockCatalog_var clock_catalog = 
            CosClockService::ClockCatalog::_narrow(m_obj);
        CPPUNIT_ASSERT(!CORBA::is_nil( clock_catalog.in()));
    
        ClockCatalog::ClockEntry_var
            entry = clock_catalog->get_entry("FederatedControlledExecutor");
        m_controlledClock = ControlledClock::_narrow(entry->subject.in());
           
        CPPUNIT_ASSERT(!CORBA::is_nil(m_controlledClock.in() ) );
    }
    catch (Cdmw::TestUtils::CORBATestManager::InvalidObject &)
    {
        CPPUNIT_FAIL ("Cdmw::TestUtils::CORBATestManager::InvalidObject");
    }
    catch ( ... )
    {
        CPPUNIT_FAIL ("Unknown exception!");
    }
        
    try
    {
        m_alias_name = "PolicyControllerUpdateAdmin";  
        Cdmw::TestUtils::CORBATestManager::instance()->add(m_alias_name);
        m_obj = Cdmw::TestUtils::CORBATestManager::instance()->get_object(m_alias_name);
        m_admin = Cdmw::clock::policy::ControllerUpdateAdmin::_narrow(m_obj);    
        CPPUNIT_ASSERT(!CORBA::is_nil(m_admin.in() ) );
    }
    catch(Cdmw::TestUtils::CORBATestManager::InvalidObject&)
    {
        CPPUNIT_FAIL("Cdmw::TestUtils::CORBATestManager::InvalidObject");
    }
}

///////////////////////////////////////////////////////////////////////////////

void  
FederatedControllerEnforceDeadlinePolicyTest::tearDown()
{
}

///////////////////////////////////////////////////////////////////////////////
// End Of File
///////////////////////////////////////////////////////////////////////////////
