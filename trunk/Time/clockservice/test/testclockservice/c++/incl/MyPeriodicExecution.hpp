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

#ifndef MYPERIODICEXECUTION_HPP
#define MYPERIODICEXECUTION_HPP

#include "Time/clockservice/CosClockService.skel.hpp"
#include "Time/testclockservice/IExecutorTest.hpp"
#include <Foundation/osthreads/Barrier.hpp>

class   MyPeriodicExecution : 
public POA_CosClockService::PeriodicExecution::Periodic
{

public:

    MyPeriodicExecution(IExecutorTest* ref);
    
    ~MyPeriodicExecution();

    virtual bool    my_work() = 0;
 
    void set_barrier( Cdmw::OsSupport::Barrier* barrier);
 
    int get_counter()   { 
        return counter_;
    }

    TimeBase::TimeT& period()   { 
        return period_;
    }
    
    TimeBase::TimeT& offset()    { 
        return offset_;
    }
    
    TimeBase::TimeT& exec_limit()   { 
        return exec_limit_;
    }
    
    TimeBase::TimeT& at_time()  { 
        return at_time_;
    }

    size_t   & max_count()  { 
        return max_count_;
    }

private:

    CORBA::Boolean do_work (const CORBA::Any&)
        throw (CORBA::SystemException);

    MyPeriodicExecution(const MyPeriodicExecution&);
    MyPeriodicExecution& operator=(const MyPeriodicExecution&);

    TimeBase::TimeT period_;
    TimeBase::TimeT offset_;
    TimeBase::TimeT exec_limit_;
    TimeBase::TimeT at_time_;
    size_t    max_count_;

    TimeBase::TimeT delta_;
    int counter_;
    int count_down_;
    TimeBase::TimeT after_;
    IExecutorTest* ref_;

    Cdmw::OsSupport::Barrier* m_barrier;

};

#endif /*MYPERIODICEXECUTION_HPP */
