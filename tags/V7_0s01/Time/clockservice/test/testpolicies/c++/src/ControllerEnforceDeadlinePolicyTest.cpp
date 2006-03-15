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
 * @brief _implementation for ControllerEnforceDeadlinePolicyTest with cppUnit library.
 *
 * @author Francesca Annunziata<fannunziata@selex-si.com>, 
 * @author Fabrizio Morciano <fmorciano@selex-si.com>
 */

#include "testpolicies/ControllerEnforceDeadlinePolicyTest.hpp"
#include "clockservice/ControllerCancelAllPolicy.hpp"
#include "clockservice/ControllerUpdateAdmin_impl.hpp"
#include "clockservice/ControlledExecutor_impl.hpp"

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

CPPUNIT_TEST_SUITE_REGISTRATION(ControllerEnforceDeadlinePolicyTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ControllerEnforceDeadlinePolicyTest, "testenforcedeadlinepolicy");

///////////////////////////////////////////////////////////////////////////////

namespace 
{    
    TimeBase::TimeT 
        S_appointment=0;
    
    struct  do_set 
    : public controller_functor
    {
        do_set(ControlledExecutor_ptr ref)
            :   controller_functor(ref)
        {            
        }
        
        void 
        operator()()
        {
            TimeBase::TimeT
                newtime = compute_current_time()- seconds2TimeT(10.0) ;
            // std::cerr<<"Appointment retard: 10 seconds!!!"<<std::endl;
            m_controller->set(newtime);
        }                    
    };

    struct  do_set_rate 
    : public controller_functor
    {
        do_set_rate(ControlledExecutor_ptr ref)
            :   controller_functor(ref)
        {            
        }
        
        void 
        operator()()
        {
            TimeBase::TimeT
                delta = compute_current_time();
            assert( S_appointment > delta );
            delta = S_appointment-delta;
            //std::cerr<<"Appointment retard:"<<delta*4<<"seconds!!!"<<std::endl;            
            m_controller->set_rate(0.25);
        }                       
    };

    struct  do_terminate 
    : public controller_functor
    {
        do_terminate(ControlledExecutor_ptr ref)
            :   controller_functor(ref)
        {            
        }
        
        void 
        operator()()
        {
            m_controller->terminate();
        }                       
    };

    struct  do_pause 
    : public controller_functor
    {
        do_pause(ControlledExecutor_ptr ref)
            :   controller_functor(ref)
        {            
        }
        
        void 
        operator()()
        {
            CPPUNIT_ASSERT(m_controller && "Controller is null");
            m_controller->pause();
        }                       
    }; 
    
    //////////////////////////////////////////////////////////////////////////////
    
    class   SimpleExecution 
    : public POA_CosClockService::PeriodicExecution::Periodic
    {
        int m_counter;
        int m_id;
        Clock_ptr ref_;

    public:    
        SimpleExecution(int id,
                        Clock_ptr ref)
        :ref_(ref)
        {
            // app_ = appointment;
            m_counter = 0;
            m_id = id;
        }
        
        CORBA::Boolean do_work (const CORBA::Any&)
          throw (CORBA::SystemException)
        {
            /*
            std::cerr<<std::endl<<m_id<< ") Count is: "<<m_counter++<<std::endl;
            TimeBase::TimeT
                curr = compute_current_time();
            std::cerr<<"Time :"<< TimeT2string(curr);
            std::cerr<<"App  :"<< TimeT2string(S_appointment);
            std::cerr<<"Delta:";
            if (curr >  S_appointment )
                std::cerr<< '+';
            else
                std::cerr<< '-';
                
            TimeBase::TimeT
                delta =  curr >  S_appointment  ? curr - S_appointment : S_appointment  - curr;                  
            std::cerr<< TimeT2seconds(delta) <<std::endl;
            */
            return 0;
        }
    };    
}

///////////////////////////////////////////////////////////////////////////////

ControllerEnforceDeadlinePolicyTest::ControllerEnforceDeadlinePolicyTest()
{  
}

///////////////////////////////////////////////////////////////////////////////

void  
ControllerEnforceDeadlinePolicyTest::setUp()
{
    try
    {
        Cdmw::TestUtils::CORBATestManager::instance ()->add ("ClockService");    
        m_obj = Cdmw::TestUtils::CORBATestManager::instance ()->get_object ("ClockService");
       
        ClockCatalog_var clock_catalog = 
            CosClockService::ClockCatalog::_narrow(m_obj);
        CPPUNIT_ASSERT(!CORBA::is_nil( clock_catalog.in()));
    
        ClockCatalog::ClockEntry_var
            entry = clock_catalog->get_entry("ControlledExecutor");
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
ControllerEnforceDeadlinePolicyTest::tearDown()
{
}

///////////////////////////////////////////////////////////////////////////////

void  
ControllerEnforceDeadlinePolicyTest::check_policy()
{
    m_admin->get_policy(Cdmw::clock::policy::ENFORCE_DEADLINE);
}
///////////////////////////////////////////////////////////////////////////////

void
ControllerEnforceDeadlinePolicyTest::on_set()
{
    ControlledExecutor_var executor;
    executor = ControlledExecutor::_narrow(m_controlledClock.in());
    do_set my_op(executor.in());
    this->checkPolicyForMethod(my_op);
}

///////////////////////////////////////////////////////////////////////////////

void
ControllerEnforceDeadlinePolicyTest::on_set_rate()
{
    ControlledExecutor_var executor;
    executor = ControlledExecutor::_narrow(m_controlledClock.in());
    do_set_rate my_op(executor.in());
    this->checkPolicyForMethod(my_op);
}

///////////////////////////////////////////////////////////////////////////////

void
ControllerEnforceDeadlinePolicyTest::on_pause()
{
    ControlledExecutor_var executor;
    executor = ControlledExecutor::_narrow(m_controlledClock.in());
    do_pause my_op(executor.in());
    this->checkPolicyForMethod(my_op);
    executor->resume();
}

///////////////////////////////////////////////////////////////////////////////
void
ControllerEnforceDeadlinePolicyTest::on_terminate()
{
    ControlledExecutor_var executor;
    executor = ControlledExecutor::_narrow(m_controlledClock.in());
    do_terminate my_op(executor.in());
    this->checkPolicyForMethod(my_op);

}

///////////////////////////////////////////////////////////////////////////////

void
ControllerEnforceDeadlinePolicyTest::checkPolicyForMethod(controller_functor& my_op)
{
    ControlledExecutor_ptr executor = my_op.getExecutor();
    executor->set_controller_update_policy(Cdmw::clock::policy::ENFORCE_DEADLINE);

    const int controller_size = 1;

    vector<POA_CosClockService::PeriodicExecution::Periodic*>  periodics;
    vector<Periodic_var>                                       periodic_vars;
    vector<CosClockService::PeriodicExecution::Controller_var> controller_vars;
    vector<unsigned int>                                       executions;

    TimeBase::TimeT period = seconds2TimeT(1);

    CORBA::Any params;

    for (int i = 0; i < controller_size; ++i)
    {
        // retrieve appointement
        S_appointment = compute_current_time() + seconds2TimeT(5.0);

        periodics.push_back(new SimpleExecution(i, executor));
        periodic_vars.push_back(periodics.back()->_this());

        controller_vars.push_back(executor->enable_periodic_execution(periodic_vars.back().in()));
        controller_vars.back()->start_at(period, S_appointment, 0, params);

        //std::cerr << std::endl << "Time of appointment: " << TimeT2string(S_appointment) << std::endl;
    }

    Cdmw::OsSupport::OS::sleep(10,0);

    for (int i = 0; i < controller_size; ++i)
    {
        // retrieve appointement
        S_appointment = compute_current_time() + seconds2TimeT(5.0);

        periodics.push_back(new SimpleExecution(i, executor));
        periodic_vars.push_back(periodics.back()->_this());

        controller_vars.push_back(executor->enable_periodic_execution(periodic_vars.back().in()));
        controller_vars.back()->start_at(period, S_appointment, 0, params);

        //std::cerr << std::endl << "Time of appointment: " << TimeT2string(S_appointment) << std::endl;
    }

    my_op();

    for (unsigned int i = 0; i < controller_vars.size(); ++i)
        executions.push_back(controller_vars[i]->executions());

    bool assertion = true;

    for (unsigned int i = 0; (i < controller_vars.size()) && assertion; ++i)
    {
        //int diff = (int) executions[i] - (int) controller_vars[i]->executions();
        //std::cerr << "Diff is: "<< diff << std::endl;
        assertion = (controller_vars[i]->executions() >= executions[i]);
    }

    while (!controller_vars.empty())
    {
        controller_vars.back()->terminate();
        controller_vars.pop_back();
    }

    CPPUNIT_ASSERT(assertion);
}

///////////////////////////////////////////////////////////////////////////////
// End Of File
///////////////////////////////////////////////////////////////////////////////
