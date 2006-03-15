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
  * @author Raffaele Mele <rmele@progesi.it>, 
  * @author Fabrizio Morciano <fmorciano@selex-si.com>
  */

#include "Time/clockservice/test/testcommon/ExecutorTest.hpp"
#include "clockservice/Clock_impl.hpp"
#include "Time/clockservice/config.hpp"
#include "Time/clockservice/Util.hpp"
#include "Time/clockservice/CosClockService.stub.hpp"
#include "Time/clockservice/test/testcommon/TimeTestManager.hpp"
#include "Time/clockservice/test/testcommon/MyPeriodicExecution.hpp"

#include <string>
#include <sstream>
#include <iostream>

using namespace CosClockService;
using namespace CosPropertyService;
using namespace Cdmw::clock;
using namespace CosClockService::PeriodicExecution;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION( ExecutorTest );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ExecutorTest, "testclockservice");

///////////////////////////////////////////////////////////////////////////////

ExecutorTest::ExecutorTest()
{
    m_my_ob = 0;
}

///////////////////////////////////////////////////////////////////////////////

void
ExecutorTest::setUp()
{
    m_my_ob = new LocalPeriodic();

    m_alias_name = "ClockService";
    Cdmw::TestUtils::CORBATestManager::instance()->add (m_alias_name);
    
    try
    {
        m_obj = Cdmw::TestUtils::CORBATestManager::instance ()->get_object (m_alias_name);
    }
    catch (Cdmw::TestUtils::CORBATestManager::InvalidObject &)
    {
        CPPUNIT_FAIL ("Cdmw::TestUtils::CORBATestManager::InvalidObject");
    }
    catch ( ... )
    {
        CPPUNIT_FAIL ("Unknown exception!");
    }
    
    ClockCatalog_var clock_catalog = 
        CosClockService::ClockCatalog::_narrow(m_obj);
    CPPUNIT_ASSERT(!CORBA::is_nil( clock_catalog.in()));
    
    ClockCatalog::ClockEntry_var 
        entry = clock_catalog->get_entry("LocalExecutor");
    m_clock = entry->subject;
    CPPUNIT_ASSERT(!CORBA::is_nil( m_clock.in()));
}

///////////////////////////////////////////////////////////////////////////////

void
ExecutorTest::tearDown()
{
    //delete m_my_ob; // Removed to avoid signal SIGSEGV during ORB shutdown.
}


///////////////////////////////////////////////////////////////////////////////

void
ExecutorTest::enable_periodic_execution()
{
    CORBA::Boolean assertion = 0;

    try
    {
        Executor_var executor;
        executor = Executor::_narrow(m_clock.in());
        CPPUNIT_ASSERT(!CORBA::is_nil(executor.in()));
        
        Periodic_var periodic = m_my_ob->_this();
        m_controller = executor->enable_periodic_execution(periodic.in());
        assertion = (!CORBA::is_nil(m_controller.in()));

        m_controller->terminate();
        CPPUNIT_ASSERT(assertion);
    }
    catch(CORBA::SystemException& e)
    {
        std::ostringstream oss;
        oss<<"CORBA::SystemException: "<<e;
        CPPUNIT_FAIL(oss.str());
    }
}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
