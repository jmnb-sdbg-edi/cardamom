/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI.
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003.
 * All rights reserved.
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
 
#ifndef INCL_PERIODIC_HPP_
#define INCL_PERIODIC_HPP_

// Std C++ include
#include <iostream>
#include <valarray>

// OMG EVoT include
#include "Time/clockservice/CosClockService.skel.hpp"

// Ctools include
#include "ctools/time/HighResTimer.h"
#include "ctools/time/HighResTime.h"
#include "ctools/time/HighResClock.h"
#include "ctools/time/LocalClock.h"


namespace perfPeriodic {

class myPeriodic:
public POA_CosClockService::PeriodicExecution::Periodic
{
public:
    CORBA::Boolean do_work (const CORBA::Any&)
        throw (CORBA::SystemException);
    
    myPeriodic::myPeriodic(const int);
    void writeRes(long double period);
private:
    std::valarray<long double> m_res;
    unsigned int m_count;
    unsigned int m_size;
};


//const size_t    maxCount  = 15;
//const TimeBase::TimeT period    = 350000; // 20 msec
//const TimeBase::TimeT offset    = 100000; // 10 msec
//const TimeBase::TimeT execLimit = 0;
//const TimeBase::TimeT atTime    = 0;


} // perfPeriodic

#endif
