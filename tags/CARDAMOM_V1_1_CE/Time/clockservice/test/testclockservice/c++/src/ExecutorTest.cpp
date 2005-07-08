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

/**
  * @brief _implementation for ExecutorTest with cppUnit library.
  *
  *
  * @author Lello Mele <lellomele@yahoo.com>, 
  * @author Fabrizio Morciano <fmorciano@amsjv.it>
  */

#include "Time/testclockservice/ExecutorTest.hpp"
#include "Time/clockservice/Clock_impl.hpp"
#include "Time/clockservice/config.hpp"
#include "Time/clockservice/Util.hpp"
#include "Time/clockservice/CosClockService.stub.hpp"
#include "Time/testclockservice/CORBAManager.hpp"
#include "Time/testclockservice/MyPeriodicExecution.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include "ace/streams.h"
#include "ace/Read_Buffer.h"

using namespace CosClockService;
using namespace CosPropertyService;
using namespace Cdmw::clock;
using namespace CosClockService::PeriodicExecution;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION( ExecutorTest );
CPPUNIT_REGISTRY_ADD( "ExecutorTest", "ControlledClockTest" );

///////////////////////////////////////////////////////////////////////////////

class LocalPeriodic 
: public MyPeriodicExecution
{
public:
    LocalPeriodic(IExecutorTest* ref)
        :MyPeriodicExecution(ref)
    {
    }
    
    bool    my_work() 
    {
        return true;
    }
};

///////////////////////////////////////////////////////////////////////////////

ExecutorTest::ExecutorTest()
{
    my_ob_ = 0;
}

///////////////////////////////////////////////////////////////////////////////

void
ExecutorTest::setUp()
{
    my_ob_ = new LocalPeriodic(this);

    alias_name_ = "ClockService";
    CORBAManager::instance ()->add (alias_name_);
    
    try
    {
        obj_ = CORBAManager::instance ()->get_object (alias_name_);
    }
    catch (CORBAManager::InvalidObject &)
    {
        CPPUNIT_FAIL ("CORBAManager::InvalidObject");
    }
    catch ( ... )
    {
        CPPUNIT_FAIL ("Unknown exception!");
    }
    
    ClockCatalog_var clock_catalog = 
        CosClockService::ClockCatalog::_narrow(obj_);
    CPPUNIT_ASSERT(!CORBA::is_nil( clock_catalog.in()));
    
    ClockCatalog::ClockEntry_var 
        entry = clock_catalog->get_entry("LocalExecutor");
    clock_ = entry->subject;
    CPPUNIT_ASSERT(!CORBA::is_nil( clock_.in()));    

    stopExecution_ = false;
}

///////////////////////////////////////////////////////////////////////////////

void
ExecutorTest::tearDown()
{
    delete my_ob_;
}


///////////////////////////////////////////////////////////////////////////////

void
ExecutorTest::test_enable_periodic_execution()
{
    try
    {
        Executor_var executor;
        executor = Executor::_narrow(clock_.in());
        
        // recover LocalClock properties
        PropertySet_var
            clock_pr = executor->properties();
        
        CPPUNIT_ASSERT(!CORBA::is_nil(executor.in()));
       
        Periodic_var periodic = my_ob_->_this();

        controller_ = executor->enable_periodic_execution( periodic.in() );
        
        CORBA::Any aPar;
        controller_->start(0,
                           0,
                           my_ob_->exec_limit(),
                           aPar);
         sleep(5);
          
        CPPUNIT_ASSERT(controller_->executions() ==
                       my_ob_->max_count());
        controller_->terminate();
    }
    catch(CORBA::SystemException& e)
    {
        std::ostringstream oss;
        oss<<"CORBA::SystemException: "<<e;
        CPPUNIT_FAIL(oss.str());
    }
}

///////////////////////////////////////////////////////////////////////////////

void  ExecutorTest::test_start_at()
{
    try
    {
        Executor_var executor;
        executor = Executor::_narrow(clock_.in());
        
        // recover LocalClock properties
        PropertySet_var
            clock_pr = executor->properties();
        
        CPPUNIT_ASSERT(!CORBA::is_nil(executor.in()));
       
        Periodic_var periodic = my_ob_->_this();

        controller_ = executor->enable_periodic_execution( periodic.in() );
      
        TimeBase::TimeT at = Clock_impl::compute_current_time();
	at += 20000000;
	 
        CORBA::Any aPar;
        controller_->start_at(0,
                              at,
                              my_ob_->exec_limit(),
                              aPar);
        sleep(5);
        CPPUNIT_ASSERT(controller_->executions() ==
                       my_ob_->max_count());
        controller_->terminate();

    }
    catch(CORBA::SystemException& e)
    {
        std::ostringstream oss;
        oss<<"CORBA::SystemException: "<<e;
        CPPUNIT_FAIL(oss.str());
    }
}

///////////////////////////////////////////////////////////////////////////////

void  ExecutorTest::stop() 
{
  //   controller_->terminate();
    stopExecution_ = true;
}


///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
