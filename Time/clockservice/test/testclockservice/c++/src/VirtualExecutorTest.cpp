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
  * @brief _implementation for VirtualExecutorTest with cppUnit library.
  *
  *
  * @author Lello Mele <lellomele@yahoo.com>, 
  * @author Fabrizio Morciano <fmorciano@amsjv.it>
  */

#include "Time/testclockservice/VirtualExecutorTest.hpp"
#include "Time/clockservice/Clock_impl.hpp"
#include "ace/streams.h"
#include "ace/Read_Buffer.h"
#include "Time/clockservice/config.hpp"
#include "Time/clockservice/Util.hpp"
#include "Time/clockservice/CosClockService.stub.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <memory>

#include "Time/testclockservice/CORBAManager.hpp"
#include "Time/testclockservice/MyPeriodicExecution.hpp"

using namespace CosClockService;
using namespace CosPropertyService;
using namespace Cdmw::clock;
using namespace CosClockService::PeriodicExecution;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION( VirtualExecutorTest );
CPPUNIT_REGISTRY_ADD( "VirtualExecutorTest", "ExecutorTest" );

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

class   DummyExecution : 
public POA_CosClockService::PeriodicExecution::Periodic
{
public:

    CORBA::Boolean do_work (const CORBA::Any&)
      throw (CORBA::SystemException)
    {
      return 0;
    }
};

///////////////////////////////////////////////////////////////////////////////

VirtualExecutorTest::VirtualExecutorTest()
{
    my_ob_ = 0;
}

///////////////////////////////////////////////////////////////////////////////

void
VirtualExecutorTest::setUp()
{
    my_ob_ = new  LocalPeriodic(this);

    stopExecution_ = false;
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
        entry = clock_catalog->get_entry("ControlledExecutor");
    clock_ = entry->subject;
    CPPUNIT_ASSERT(!CORBA::is_nil( clock_.in()));    
}

///////////////////////////////////////////////////////////////////////////////

void
VirtualExecutorTest::tearDown()
{
    delete  my_ob_ ;
}

///////////////////////////////////////////////////////////////////////////////

void
VirtualExecutorTest::enable_periodic_execution()
{      
    try
    {
        ControlledExecutor_var executor;
        executor = ControlledExecutor::_narrow(clock_.in());
        
        executor->set_rate(0.5);

        // recover LocalClock properties
        PropertySet_var
            clock_pr = executor->properties();
        
        CPPUNIT_ASSERT(!CORBA::is_nil(executor.in()));
       
        Periodic_var periodic = my_ob_->_this();

        controller_ = executor->enable_periodic_execution( periodic.in() );
        
        CORBA::Any aPar;
        controller_->start(my_ob_->period(),
                           0,
                           my_ob_->exec_limit(),
                           aPar);
        
        sleep(10);
        CPPUNIT_ASSERT(controller_->executions() == 
                       my_ob_->max_count());
    }
    catch(CORBA::SystemException& e)
    {
        std::ostringstream oss;
        oss<<"CORBA::SystemException: "<<e;
        CPPUNIT_FAIL(oss.str());
    }
}


///////////////////////////////////////////////////////////////////////////////

void
VirtualExecutorTest::delete_on_set()
{
  try
  {
    ControlledExecutor_var executor;
    executor = ControlledExecutor::_narrow(clock_.in());
    
    CPPUNIT_ASSERT(!CORBA::is_nil(executor.in()));
    
    POA_CosClockService::PeriodicExecution::Periodic
      *p1=new DummyExecution,
      *p2=new DummyExecution,
      *p3=new DummyExecution;

    Periodic_var periodic_1 = p1->_this();
    Periodic_var periodic_2 = p2->_this();
    Periodic_var periodic_3 = p3->_this(); 
    
    CosClockService::PeriodicExecution::Controller_var 
      controller_1,
      controller_2,
      controller_3;
        
    controller_1 = executor->enable_periodic_execution( periodic_1.in() );
    controller_2 = executor->enable_periodic_execution( periodic_2.in() );
    controller_3 = executor->enable_periodic_execution( periodic_3.in() );

    executor->set(0);
    
    delete p1;
    delete p2;
    delete p3;

    CPPUNIT_ASSERT(true); // if not problem all clock have been deleted
  }
  catch(CORBA::SystemException& e)
  {
    std::ostringstream oss;
    oss<<"CORBA::SystemException: "<<e;
    CPPUNIT_FAIL(oss.str());
  }
}

///////////////////////////////////////////////////////////////////////////////

void  VirtualExecutorTest::stop() 
{
    stopExecution_ = true;
    controller_->terminate();
}


///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
