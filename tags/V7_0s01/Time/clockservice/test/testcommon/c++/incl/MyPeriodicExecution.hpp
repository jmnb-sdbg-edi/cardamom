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

#ifndef MYPERIODICEXECUTION_HPP
#define MYPERIODICEXECUTION_HPP

#include "Time/clockservice/CosClockService.skel.hpp"

class   MyPeriodicExecution
: public POA_CosClockService::PeriodicExecution::Periodic
{

public:

    MyPeriodicExecution();

    ~MyPeriodicExecution();

    virtual bool    my_work() = 0;

    int get_counter();

    TimeBase::TimeT& period();

    TimeBase::TimeT& offset();

    TimeBase::TimeT& exec_limit();

    TimeBase::TimeT& at_time();

    size_t &max_count();

private:

    CORBA::Boolean do_work (const CORBA::Any&)
        throw (CORBA::SystemException);

    MyPeriodicExecution(const MyPeriodicExecution&);
    MyPeriodicExecution& operator=(const MyPeriodicExecution&);

    TimeBase::TimeT m_period;
    TimeBase::TimeT m_offset;
    TimeBase::TimeT m_exec_limit;
    TimeBase::TimeT m_at_time;
    size_t    m_max_count;

    TimeBase::TimeT m_delta;
    int m_counter;
    int m_count_down;
    TimeBase::TimeT m_after;
};

///////////////////////////////////////////////////////////////////////////////

class LocalPeriodic
: public MyPeriodicExecution
{
public:
  LocalPeriodic()
  {
  }

  bool    my_work()
  {
      return true;
  }
};


///////////////////////////////////////////////////////////////////////////////

class   DummyExecution 
: public POA_CosClockService::PeriodicExecution::Periodic
{
public:

    CORBA::Boolean do_work (const CORBA::Any&)
      throw (CORBA::SystemException)
    {
      return 0;
    }
};

    
#endif /*MYPERIODICEXECUTION_HPP */
