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

#include "Time/clockservice/test/testcommon/MyPeriodicExecution.hpp"
#include "Time/clockservice/test/testcommon/PerformanceManager.hpp"
#include "clockservice/Clock_impl.hpp"
#include "Time/clockservice/Util.hpp"
#include "Time/clockservice/test/testcommon/TimeTestManager.hpp"
#include <Foundation/common/Assert.hpp>

///////////////////////////////////////////////////////////////////////////////

MyPeriodicExecution::MyPeriodicExecution ()
{
  m_after = 0;
  m_counter = 0;
  m_delta = Cdmw::clock::compute_current_time ();

  m_count_down = m_exec_limit = m_max_count = 15;
  m_period = 200000 /*20 msec */ ;
  m_offset = 100000 /*10 msec */ ;
  m_at_time = 0;
}

///////////////////////////////////////////////////////////////////////////////

MyPeriodicExecution::~MyPeriodicExecution ()
{
}

///////////////////////////////////////////////////////////////////////////////

CORBA::Boolean MyPeriodicExecution::do_work (const CORBA::Any &)
throw (CORBA::SystemException)
{
  //S_TRACE(MyPeriodicExecution::do_work);    
  m_delta = Cdmw::clock::compute_current_time () - m_delta;

  this->my_work ();

  ++m_counter;
  if (!--m_count_down)
  { 
    return false;
  }

  m_delta = Cdmw::clock::compute_current_time ();
  //std::cerr << "Delta is:" << m_delta << std::endl;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
int
MyPeriodicExecution::get_counter ()
{
  return m_counter;
}

///////////////////////////////////////////////////////////////////////////////
TimeBase::TimeT & MyPeriodicExecution::period ()
{
  return m_period;
}

///////////////////////////////////////////////////////////////////////////////
TimeBase::TimeT & MyPeriodicExecution::offset ()
{
  return m_offset;
}

///////////////////////////////////////////////////////////////////////////////
TimeBase::TimeT & MyPeriodicExecution::exec_limit ()
{
  return m_exec_limit;
}

///////////////////////////////////////////////////////////////////////////////
TimeBase::TimeT & MyPeriodicExecution::at_time ()
{
  return m_at_time;
}

///////////////////////////////////////////////////////////////////////////////
size_t & MyPeriodicExecution::max_count ()
{
  return m_max_count;
}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
