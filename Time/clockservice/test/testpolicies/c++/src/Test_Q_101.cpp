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
 * @brief _implementation for Test_Q_101 with cppUnit library.
 *
 * @author Francesca Annunziata<fannunziata@selex-si.com>, 
 * @author Fabrizio Morciano <fmorciano@selex-si.com>
 */

#include "testpolicies/Test_Q_101.hpp"
#include "clockservice/ControllerCancelAllPolicy.hpp"
#include "clockservice/ControllerUpdateAdmin_impl.hpp"
#include "clockservice/ControlledExecutor_impl.hpp"


#include <string>
#include <iostream>
#include <sstream>

#include "Time/clockservice/test/testcommon/TimeTestManager.hpp"
#include "Time/clockservice/test/testcommon/MyPeriodicExecution.hpp"
#include "Time/clockservice/Util.hpp"
#include <Foundation/common/Assert.hpp>

using namespace CosClockService;
using namespace CosPropertyService;
using namespace Cdmw::clock;
using namespace Cdmw::clock::util;
using namespace CosClockService::PeriodicExecution;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION(Test_Q_101);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Test_Q_101, "test_Q_101");

///////////////////////////////////////////////////////////////////////////////

#include "ace/TP_Reactor.h"

namespace 
{    
    ///////////////////////////////////////////////////////////////////////////////
    
    class   EnforceDeadLineCallback 
    : public POA_CosClockService::PeriodicExecution::Periodic
    {
        ControlledExecutor_ptr ref_;
    public:    
        EnforceDeadLineCallback(ControlledExecutor_ptr executor)
        :ref_(executor)
        {
        }
        
        CORBA::Boolean do_work (const CORBA::Any&)
          throw (CORBA::SystemException)
        {
          return 1;
        }
    };    
    
    ///////////////////////////////////////////////////////////////////////////////
    
    class   CancelAllCallback
    : public POA_CosClockService::PeriodicExecution::Periodic
    {
    public:    
        CancelAllCallback()
        {
        }
        
        CORBA::Boolean do_work (const CORBA::Any&)
          throw (CORBA::SystemException)
        {
          return 1;
        }
    };    

    ///////////////////////////////////////////////////////////////////////////////
    
    class   EnforceIntervalCallback 
    : public POA_CosClockService::PeriodicExecution::Periodic
    {
        ControlledExecutor_ptr ref_;
    public:    
        EnforceIntervalCallback(ControlledExecutor_ptr executor)
        :ref_(executor)
        {
        }
                
        CORBA::Boolean do_work (const CORBA::Any&)
          throw (CORBA::SystemException)
        {
            return 1;
        }
    };    
}

///////////////////////////////////////////////////////////////////////////////

Test_Q_101::Test_Q_101()
{  
}

///////////////////////////////////////////////////////////////////////////////

void  
Test_Q_101::setUp()
{
    try
    {
        Cdmw::TestUtils::CORBATestManager::instance()->add("ClockService");    
        m_obj = Cdmw::TestUtils::CORBATestManager::instance()->get_object("ClockService");

        ClockCatalog_var clock_catalog = CosClockService::ClockCatalog::_narrow(m_obj);
        CPPUNIT_ASSERT(!CORBA::is_nil(clock_catalog.in()));
                                                                                                                            
        ClockCatalog::ClockEntry_var entry = clock_catalog->get_entry("ControlledExecutor");
        m_controlledClock = ControlledClock::_narrow(entry->subject.in());
        CPPUNIT_ASSERT(!CORBA::is_nil(m_controlledClock.in()));
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
        CPPUNIT_ASSERT(!CORBA::is_nil(m_admin.in()));
    }
    catch(Cdmw::TestUtils::CORBATestManager::InvalidObject&)
    {
        CPPUNIT_FAIL("Cdmw::TestUtils::CORBATestManager::InvalidObject");
    }
}

///////////////////////////////////////////////////////////////////////////////

void  
Test_Q_101::tearDown()
{
    //m_controlledClock->terminate();
    //CORBA::release(m_controlledClock);
}

///////////////////////////////////////////////////////////////////////////////

void
Test_Q_101::on_set()
{
    ControlledExecutor_var executor = ControlledExecutor::_narrow(m_controlledClock.in());
    
    vector<POA_CosClockService::PeriodicExecution::Periodic*>  periodics;
    vector<Periodic_var>                                       periodic_vars;
    vector<CosClockService::PeriodicExecution::Controller_var> controller_vars;

    TimeBase::TimeT period = seconds2TimeT(1.0);
    CORBA::Any params;

    // start a new controller with the cancel all policy.
    periodics.push_back(new CancelAllCallback());
    periodic_vars.push_back(periodics.back()->_this());
    controller_vars.push_back(executor->enable_periodic_execution(periodic_vars.back().in()));
    controller_vars.back()->start(period, 0, 0, params);       

    // start a new controller with the enforce interval policy.
    periodics.push_back(new EnforceIntervalCallback(executor.in()));
    periodic_vars.push_back(periodics.back()->_this());
    controller_vars.push_back(executor->enable_periodic_execution_with_policy(periodic_vars.back().in(), 
                                                                              Cdmw::clock::policy::ENFORCE_INTERVAL));
    controller_vars.back()->start(period, 0, 0, params);

    // start a new controller with the enforce deadline policy.
    periodics.push_back(new EnforceDeadLineCallback(executor.in()));
    periodic_vars.push_back(periodics.back()->_this());
    controller_vars.push_back(executor->enable_periodic_execution_with_policy(periodic_vars.back().in(), 
                                                                              Cdmw::clock::policy::ENFORCE_DEADLINE));
    controller_vars.back()->start(period, 0, 0, params);

    Cdmw::OsSupport::OS::sleep(4,0);
    
    executor->set_rate(0.5);
    
    CORBA::ULong executions_of_cancel_all       = controller_vars[0]->executions();
    CORBA::ULong executions_of_enforce_interval = controller_vars[1]->executions();
    CORBA::ULong executions_of_enforce_deadline = controller_vars[2]->executions();

    Cdmw::OsSupport::OS::sleep(0, 3000);

    // not more executions with this policy
    bool assertion = ((controller_vars[0]->executions() == executions_of_cancel_all)       &&
                      (controller_vars[1]->executions() == executions_of_enforce_interval) &&
                      (controller_vars[2]->executions() == executions_of_enforce_deadline));

    // terminate the controlled executor,
    // (so there's no need to explicitly
    //  terminate all created controllers)
    m_controlledClock->terminate();

    // not more executions with this policy
    CPPUNIT_ASSERT(assertion);
}

///////////////////////////////////////////////////////////////////////////////
// End Of File
///////////////////////////////////////////////////////////////////////////////
