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

#include "Periodic.hpp"

using namespace std;
using namespace perfPeriodic;

myPeriodic::myPeriodic(std::valarray<long double> &va, 
					   size_t &count, 
					   IExecutor *ref)  
	: m_t1(va),
	  m_count(count),
	  m_ProcCtrlRef(ref)
{
}

CORBA::Boolean myPeriodic::do_work(const CORBA::Any& par)
    throw (CORBA::SystemException)
{
    m_t1[m_count++] = ctools::time::HighResClock::getTime().toMicrosec();
   if (m_count == m_t1.size()) { // no other sample to get
        // write file for script control
        std::ofstream feof("perf_timer.eof"); 
        feof.close();
        return false;
    }
	m_ProcCtrlRef->startAt();
    return true;
}
