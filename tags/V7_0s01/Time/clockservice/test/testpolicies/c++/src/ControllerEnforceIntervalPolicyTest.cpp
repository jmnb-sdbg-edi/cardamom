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
 * @brief Implementation for ControllerEnforceIntervalPolicyTest with cppUnit library.
 *
 * @author Francesca Annunziata<fannunziata@selex-si.com>, 
 * @author Fabrizio Morciano <fmorciano@selex-si.com>
 * @author Marco Macellari <mmacellari@progesi.it>
 */

#include "testpolicies/ControllerEnforceIntervalPolicyTest.hpp"
#include "clockservice/ControllerCancelAllPolicy.hpp"
#include "clockservice/ControllerUpdateAdmin_impl.hpp"

#include <string>
#include <iostream>
#include <sstream>

#include "Time/clockservice/test/testcommon/TimeTestManager.hpp"
#include "Time/clockservice/test/testcommon/MyPeriodicExecution.hpp"
#include "Time/clockservice/test/testcommon/anExecution.hpp"
#include "Time/clockservice/Util.hpp"

#include "testpolicies/EnforceIntervalPolicyAsserter.hpp"
#include "testpolicies/Engine.hpp"

using namespace CosClockService;
using namespace CosPropertyService;
using namespace Cdmw::clock;
using namespace Cdmw::clock::util;
using namespace CosClockService::PeriodicExecution;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION(ControllerEnforceIntervalPolicyTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ControllerEnforceIntervalPolicyTest, "testenforceintervalpolicy");

///////////////////////////////////////////////////////////////////////////////

ControllerEnforceIntervalPolicyTest::ControllerEnforceIntervalPolicyTest()
{  
}

///////////////////////////////////////////////////////////////////////////////

void  
ControllerEnforceIntervalPolicyTest::setUp()
{
    try
    {
        Cdmw::TestUtils::CORBATestManager::instance()->add("ClockService");
        m_obj = Cdmw::TestUtils::CORBATestManager::instance()->get_object("ClockService");
        ClockCatalog_var m_clockcatalog = CosClockService::ClockCatalog::_narrow(m_obj.in());
        CPPUNIT_ASSERT(!CORBA::is_nil(m_clockcatalog.in()));

        ClockCatalog::ClockEntry_var entry = m_clockcatalog->get_entry("ControlledExecutor");
        m_controlledClock = ControlledClock::_narrow(entry->subject.in());
        m_controlledClock->set(Cdmw::clock::compute_current_time());
        m_controlledClock->set_rate(1.0);
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
        m_admin = Cdmw::clock::policy::ControllerUpdateAdmin::_narrow(m_obj.in());
        CPPUNIT_ASSERT(!CORBA::is_nil(m_admin.in()));
    }
    catch(Cdmw::TestUtils::CORBATestManager::InvalidObject&)
    {
        CPPUNIT_FAIL("Cdmw::TestUtils::CORBATestManager::InvalidObject");
    }
}

///////////////////////////////////////////////////////////////////////////////

void  
ControllerEnforceIntervalPolicyTest::tearDown()
{
}

///////////////////////////////////////////////////////////////////////////////

void  
ControllerEnforceIntervalPolicyTest::check_policy()
{
    // check if the policy is not registered by PolicyAdmin
    m_admin->get_policy(Cdmw::clock::policy::ENFORCE_INTERVAL);
}

///////////////////////////////////////////////////////////////////////////////

void
ControllerEnforceIntervalPolicyTest::PCR_0450_on_set_rate()
{
    Asserter* asserter = 0;

    try
    {
        // Define the piecewise linear curve in the real-time vs virtual-time coordinated plane
        // used for the test through the definition of the ordered set of all its vertices.
        const int num_points = 5;
        Point_t points[num_points] = { {  0.00,  0.00 },
                                       {  6.00,  6.00 },
                                       { 10.00, 10.00 },
                                       { 18.00, 14.00 },
                                       { 22.00, 22.00 } };

        // Define the asserter and engine parameters.
        const double        time_origin     = 0.0;
        const double        period          = 4.0;
        const unsigned long execution_limit = 0;
        const double        real_epsilon    = 0.1;
        const double        virt_epsilon    = 0.1;

        // Get a controlled executor reference.
        ControlledExecutor_var executor = ControlledExecutor::_narrow(m_controlledClock.in());

        // Set the enforce interval policy.
        executor->set_controller_update_policy(Cdmw::clock::policy::ENFORCE_INTERVAL);

        // Create the extended asserter.
        asserter = new EnforceIntervalPolicyAsserter(time_origin,
                                                     period,
                                                     execution_limit,
                                                     real_epsilon,
                                                     virt_epsilon,
                                                     num_points,
                                                     points);

        // Get a periodic reference.
        anExecution<Cdmw::clock::ControlledExecutor>* servant =
            new anExecution<Cdmw::clock::ControlledExecutor>(executor.in(), (anExecutionCallback*) asserter);
        CosClockService::PeriodicExecution::Periodic_var periodic = servant->_this();

        // Get a controller reference.
        CosClockService::PeriodicExecution::Controller_var controller =
            executor->enable_periodic_execution(periodic.in());

        // Create the extended engine.
        Engine engine(executor.in(),
                      period,
                      execution_limit,
                      num_points,
                      points);

        CORBA::Any param;

        // Reset executor
        executor->set(0);
        executor->set_rate(1.0);

        // Initialize executor for the test
        executor->set(Cdmw::clock::util::seconds2TimeT(points[0].virt));

        // Start the controller
        controller->start(Cdmw::clock::util::seconds2TimeT(period), 0, execution_limit, param);

        // Run the test.
        engine.run();

        // Terminate the controller.
        controller->terminate();
    }
    catch (...)
    {
        // No exception should be thrown.
        CPPUNIT_FAIL("unexpected exception");
    }

    // Final check: the following two conditions must be verified:
    // 1. check for do_work() invocations that failed.
    // 2. check for do_work() invocations that erroneously were not done.
    CPPUNIT_ASSERT(asserter &&
                   asserter->get_num_failed_executions() == 0 &&
                   asserter->get_num_remaining_executions() == 0);
}

///////////////////////////////////////////////////////////////////////////////

void
ControllerEnforceIntervalPolicyTest::PCR_0450_on_set()
{
    Asserter* asserter = 0;

    try
    {
        // Define the piecewise linear curve in the real-time vs virtual-time coordinated plane
        // used for the test through the definition of the ordered set of all its vertices.
        const int num_points = 8;
        Point_t points[num_points] = { {  0.00,  0.00 },
                                       {  6.00,  6.00 },
                                       {  6.00, 14.00 },
                                       { 10.00, 18.00 },
                                       { 10.00, 26.00 },
                                       { 14.00, 30.00 },
                                       { 14.00, 10.00 },
                                       { 22.00, 18.00 } };

        // Define the asserter and engine parameters.
        const double        time_origin     = 0.0;
        const double        period          = 4.0;
        const unsigned long execution_limit = 0;
        const double        real_epsilon    = 0.1;
        const double        virt_epsilon    = 0.1;

        // Get a controlled executor reference.
        ControlledExecutor_var executor = ControlledExecutor::_narrow(m_controlledClock.in());

        // Set the enforce interval policy.
        executor->set_controller_update_policy(Cdmw::clock::policy::ENFORCE_INTERVAL);

        // Create the extended asserter.
        asserter = new EnforceIntervalPolicyAsserter(time_origin,
                                                     period,
                                                     execution_limit,
                                                     real_epsilon,
                                                     virt_epsilon,
                                                     num_points,
                                                     points);

        // Get a periodic reference.
        anExecution<Cdmw::clock::ControlledExecutor>* servant =
            new anExecution<Cdmw::clock::ControlledExecutor>(executor.in(), (anExecutionCallback*) asserter);
        CosClockService::PeriodicExecution::Periodic_var periodic = servant->_this();

        // Get a controller reference.
        CosClockService::PeriodicExecution::Controller_var controller =
            executor->enable_periodic_execution(periodic.in());

        // Create the extended engine.
        Engine engine(executor.in(),
                      period,
                      execution_limit,
                      num_points,
                      points);

        CORBA::Any param;

        // Reset executor
        executor->set(0);
        executor->set_rate(1.0);

        // Initialize executor for the test
        executor->set(Cdmw::clock::util::seconds2TimeT(points[0].virt));

        // Start the controller
        controller->start(Cdmw::clock::util::seconds2TimeT(period), 0, execution_limit, param);

        // Run the test.
        engine.run();

        // Terminate the controller.
        controller->terminate();
    }
    catch (...)
    {
        // No exception should be thrown.
        CPPUNIT_FAIL("unexpected exception");
    }

    // Final check: the following two conditions must be verified:
    // 1. check for do_work() invocations that failed.
    // 2. check for do_work() invocations that erroneously were not done.
    CPPUNIT_ASSERT(asserter &&
                   asserter->get_num_failed_executions() == 0 &&
                   asserter->get_num_remaining_executions() == 0);
}

///////////////////////////////////////////////////////////////////////////////

void
ControllerEnforceIntervalPolicyTest::PCR_0450_on_pause_resume()
{
    Asserter* asserter = 0;

    try
    {
        // Define the piecewise linear curve in the real-time vs virtual-time coordinated plane
        // used for the test through the definition of the ordered set of all its vertices.
        const int num_points = 6;
        Point_t points[num_points] = { {  0.00,  0.00 },
                                       {  6.00,  6.00 },
                                       { 14.00,  6.00 },
                                       { 18.00, 10.00 },
                                       { 22.00, 10.00 },
                                       { 30.00, 18.00 } };

        // Define the asserter and engine parameters.
        const double        time_origin     = 0.0;
        const double        period          = 4.0;
        const unsigned long execution_limit = 0;
        const double        real_epsilon    = 0.1;
        const double        virt_epsilon    = 0.1;

        // Get a controlled executor reference.
        ControlledExecutor_var executor = ControlledExecutor::_narrow(m_controlledClock.in());

        // Set the enforce interval policy.
        executor->set_controller_update_policy(Cdmw::clock::policy::ENFORCE_INTERVAL);

        // Create the extended asserter.
        asserter = new EnforceIntervalPolicyAsserter(time_origin,
                                                     period,
                                                     execution_limit,
                                                     real_epsilon,
                                                     virt_epsilon,
                                                     num_points,
                                                     points);

        // Get a periodic reference.
        anExecution<Cdmw::clock::ControlledExecutor>* servant =
            new anExecution<Cdmw::clock::ControlledExecutor>(executor.in(), (anExecutionCallback*) asserter);
        CosClockService::PeriodicExecution::Periodic_var periodic = servant->_this();

        // Get a controller reference.
        CosClockService::PeriodicExecution::Controller_var controller =
            executor->enable_periodic_execution(periodic.in());

        // Create the extended engine.
        Engine engine(executor.in(),
                      period,
                      execution_limit,
                      num_points,
                      points);

        CORBA::Any param;

        // Reset executor
        executor->set(0);
        executor->set_rate(1.0);

        // Initialize executor for the test
        executor->set(Cdmw::clock::util::seconds2TimeT(points[0].virt));

        // Start the controller
        controller->start(Cdmw::clock::util::seconds2TimeT(period), 0, execution_limit, param);

        // Run the test.
        engine.run();

        // Terminate the controller.
        controller->terminate();
    }
    catch (...)
    {
        // No exception should be thrown.
        CPPUNIT_FAIL("unexpected exception");
    }

    // Final check: the following two conditions must be verified:
    // 1. check for do_work() invocations that failed.
    // 2. check for do_work() invocations that erroneously were not done.
    CPPUNIT_ASSERT(asserter &&
                   asserter->get_num_failed_executions() == 0 &&
                   asserter->get_num_remaining_executions() == 0);
}

///////////////////////////////////////////////////////////////////////////////
/*
void
ControllerEnforceIntervalPolicyTest::PCR_0450_on_mixed_controls()
{
    ControlledExecutor_var executor = ControlledExecutor::_narrow(m_controlledClock.in());
    executor->set_controller_update_policy(Cdmw::clock::policy::ENFORCE_INTERVAL);
    executor->set(0);
    executor->set_rate(1.0);

    const int ref_period = 4, delta = ref_period / 2;
    myExtendedAsserter::Point_t points_array[9] = { {  0.00, 0.00 },
                                                    {  1.00, 3.00 },
                                                    {  2.00, 3.00 },
                                                    {  3.00, 2.00 },
                                                    {  6.00, 1.00 },
                                                    {  7.00, 4.00 },
                                                    {  8.00, 4.00 },
                                                    {  9.00, 5.00 },
                                                    { 10.00, 6.00 } };
    myExtendedAsserter anAsserter(ref_period, 9, points_array);
    anExecution<Cdmw::clock::ControlledExecutor>* periodic =
        new anExecution<Cdmw::clock::ControlledExecutor>(executor.in(), &anAsserter);
    Periodic_var periodic_var = periodic->_this();

    CosClockService::PeriodicExecution::Controller_var controller_var =
        executor->enable_periodic_execution(periodic_var.in());

    TimeBase::TimeT period = Cdmw::clock::util::seconds2TimeT(ref_period);
    CORBA::Any params;

    controller_var->start(period, 0, 0, params);
    TimeBase::TimeT time_set;

    Cdmw::OsSupport::OS::sleep(delta);
    time_set = Cdmw::clock::util::seconds2TimeT(2.5 * ref_period);
    executor->set(time_set);
    std::cerr << "\nSet time to " << Cdmw::clock::util::TimeT2seconds(time_set) << std::endl;

    Cdmw::OsSupport::OS::sleep(ref_period);
    executor->set_rate(-1.0);
    std::cerr << "\nSet rate to -1.0" << std::endl;

    Cdmw::OsSupport::OS::sleep(2 * ref_period);
    executor->pause();
    std::cerr << "\nPaused executor" << std::endl;

    Cdmw::OsSupport::OS::sleep(2 * ref_period);
    executor->resume();
    std::cerr << "\nResumed executor" << std::endl;

    Cdmw::OsSupport::OS::sleep(ref_period);
    time_set = Cdmw::clock::util::seconds2TimeT(4.5 * ref_period);
    executor->set(time_set);
    std::cerr << "\nSet time to " << Cdmw::clock::util::TimeT2seconds(time_set) << std::endl;

    Cdmw::OsSupport::OS::sleep(ref_period);
    executor->set_rate(1.0);
    std::cerr << "\nSet rate to 1.0" << std::endl;

    Cdmw::OsSupport::OS::sleep(3 * ref_period);
    controller_var->terminate();

    CPPUNIT_ASSERT(anAsserter.get_num_execs() == anAsserter.get_num_max_execs());
}
*/
///////////////////////////////////////////////////////////////////////////////

void
ControllerEnforceIntervalPolicyTest::PCR_0450_on_terminate()
{
    Asserter* asserter = 0;

    try
    {
        // Define the piecewise linear curve in the real-time vs virtual-time coordinated plane
        // used for the test through the definition of the ordered set of all its vertices.
        const int num_points = 2;
        Point_t points[num_points] = { {  0.00,  0.00 },
                                       { 10.00, 10.00 } };

        // Define the asserter and engine parameters.
        const double        time_origin     = 0.0;
        const double        period          = 4.0;
        const unsigned long execution_limit = 0;
        const double        real_epsilon    = 0.1;
        const double        virt_epsilon    = 0.1;

        // Get a controlled executor reference.
        ControlledExecutor_var executor = ControlledExecutor::_narrow(m_controlledClock.in());

        // Set the enforce interval policy.
        executor->set_controller_update_policy(Cdmw::clock::policy::ENFORCE_INTERVAL);

        // Create the extended asserter.
        asserter = new EnforceIntervalPolicyAsserter(time_origin,
                                                     period,
                                                     execution_limit,
                                                     real_epsilon,
                                                     virt_epsilon,
                                                     num_points,
                                                     points);

        // Get a periodic reference.
        anExecution<Cdmw::clock::ControlledExecutor>* servant =
            new anExecution<Cdmw::clock::ControlledExecutor>(executor.in(), (anExecutionCallback*) asserter);
        CosClockService::PeriodicExecution::Periodic_var periodic = servant->_this();

        // Get a controller reference.
        CosClockService::PeriodicExecution::Controller_var controller =
            executor->enable_periodic_execution(periodic.in());

        CORBA::Any param;

        // Initialize the executor.
        executor->set(0);
        executor->set_rate(1.0);

        // Initialize executor for the test
        executor->set(Cdmw::clock::util::seconds2TimeT(points[0].virt));

        // Start the controller
        controller->start(Cdmw::clock::util::seconds2TimeT(period), 0, execution_limit, param);

        // Terminate the executor after 10 seconds from controller start.
        int int_period = (int) period;
        Cdmw::OsSupport::OS::sleep(2 * int_period + int_period / 2,0 /* 10 seconds sleep */);
        executor->terminate();

        // Wait for a while, just to make sure that no other invocations of do_work are made.
        Cdmw::OsSupport::OS::sleep(2 * int_period ,0 /* 8 seconds sleep */);
        //controller->terminate(); // NB: not needed: executor->terminate() already did the work...
    }
    catch (...)
    {
        // No exception should be thrown.
        CPPUNIT_FAIL("unexpected exception");
    }

    // Final check: the following two conditions must be verified:
    // 1. check for do_work() invocations that failed.
    // 2. check for do_work() invocations that erroneously were not done.
    CPPUNIT_ASSERT(asserter &&
                   asserter->get_num_failed_executions() == 0 &&
                   asserter->get_num_remaining_executions() == 0);
}

///////////////////////////////////////////////////////////////////////////////
// End Of File
///////////////////////////////////////////////////////////////////////////////
