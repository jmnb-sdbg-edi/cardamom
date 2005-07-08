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

#include "Time/testclockservice/MyPeriodicExecution.hpp"
#include "Time/testclockservice/PerformanceManager.hpp"
#include "Time/clockservice/Clock_impl.hpp"
#include "Time/clockservice/Util.hpp"
#include "Time/testclockservice/Consts.hpp"
#include <Foundation/common/Assert.hpp>
#include <Foundation/osthreads/Mutex.hpp>
#include <iostream>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

MyPeriodicExecution::MyPeriodicExecution(IExecutorTest* ref)
:ref_(ref)
{
    CPPUNIT_ASSERT(ref != 0);
    after_ =0;
    counter_ = 1;
    delta_ = Cdmw::clock::Clock_impl::compute_current_time();

    count_down_ = exec_limit_ = max_count_ = 15;
    period_    = 200000 /*20 msec*/;
    offset_    = 100000 /*10 msec*/;
    at_time_   = 0;
    m_barrier = 0;
}

///////////////////////////////////////////////////////////////////////////////

MyPeriodicExecution::~MyPeriodicExecution()
{
}

///////////////////////////////////////////////////////////////////////////////

CORBA::Boolean MyPeriodicExecution::do_work (const CORBA::Any&)
throw (CORBA::SystemException)
{

    delta_ =  Cdmw::clock::Clock_impl::compute_current_time() - delta_;
    PERFORMANCE_ASSERT( delta_ <= period_ + 
                                  offset_ +
                                  CDMW_TOL);

    this->my_work();

    ++counter_;
    if( !--count_down_ )
    {
        //Cdmw::OsSupport::Mutex mutex;
        //mutex.lock(); 
        //ref_->stop();
        //   std::cout<<"Barrier is down!"<<std::endl;
        if(m_barrier)
           m_barrier->wait ();
        //mutex.unlock();
        return false;
    }        
    delta_ = Cdmw::clock::Clock_impl::compute_current_time();
    
    return true;
}

///////////////////////////////////////////////////////////////////////////////

void
MyPeriodicExecution::set_barrier (Cdmw::OsSupport::Barrier * barrier)
{
  CDMW_ASSERT (barrier && "Barrier in null!");
  m_barrier = barrier;
}


///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
