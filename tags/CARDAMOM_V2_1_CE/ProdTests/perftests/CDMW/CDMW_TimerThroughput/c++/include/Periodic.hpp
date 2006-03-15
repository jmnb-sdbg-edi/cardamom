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
#ifndef INCL_PERIODIC_HPP_
#define INCL_PERIODIC_HPP_

// std C++ include
#include <iostream>
#include <valarray>
#include <fstream>

// OMG EVoT include
#include "Time/clockservice/CosClockService.skel.hpp"

// cTools include
#include "ctools/time/HighResTimer.h"
#include "ctools/time/HighResTime.h"
#include "ctools/time/HighResClock.h"
#include "ctools/time/LocalClock.h"

// local include
#include "IExecutor.hpp"

namespace perfPeriodic {

class myPeriodic:
public POA_CosClockService::PeriodicExecution::Periodic
{
public:
    CORBA::Boolean do_work (const CORBA::Any&)
        throw (CORBA::SystemException);
    
    myPeriodic(int reqIter, IExecutor *ref);
	void writeRes(int IterRequested);
public:
	int m_counter;
	int m_reqIter;
private:
	IExecutor *m_ProcCtrlRef;

};

} // perfPeriodic

#endif // INCL_PERIODIC_HPP_
