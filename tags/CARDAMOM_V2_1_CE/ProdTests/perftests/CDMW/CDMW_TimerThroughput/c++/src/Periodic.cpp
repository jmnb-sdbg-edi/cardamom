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

myPeriodic::myPeriodic(int reqIter, IExecutor *ref) 
	:	m_counter(0),
		m_reqIter(reqIter),
		m_ProcCtrlRef(ref)
{
}

CORBA::Boolean myPeriodic::do_work(const CORBA::Any& par)
    throw (CORBA::SystemException)
{
	if (m_counter < m_reqIter) 
	{
		++m_counter;
		//std::cerr << "** PERIODIC COUNTER: " << m_counter << std::endl;
		m_ProcCtrlRef->startAt();
		return true;
	}
	// write file for script control
	std::ofstream feof("perf_timer.eof"); 
	feof.close();
	return false;
}

void myPeriodic::writeRes(int IterRequested)
{
    std::ofstream fp("COUNTER.dat");
    fp	<< "Timer iteration throughput" << std::endl
		<< "--------------------------" << std::endl
		<< "Requested iteration: " << IterRequested << std::endl
		<< "Performed iteration: " << m_counter << std::endl;
    fp.close();
} 
