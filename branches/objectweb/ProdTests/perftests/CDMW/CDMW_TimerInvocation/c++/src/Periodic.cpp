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
 
#include <fstream>
#include "Periodic.hpp"
#include "math.h"

using namespace std;
using namespace perfPeriodic;

CORBA::Boolean myPeriodic::do_work(const CORBA::Any& par)
    throw (CORBA::SystemException)
{
    
    m_res[m_count++] = ctools::time::HighResClock::getTime().toMicrosec();
    if (m_count == m_size) { // no other sample to get
        // write file for script control
        std::ofstream feof("perf_timer.eof"); 
        feof.close();
        return false;
    }
    return true;
}

myPeriodic::myPeriodic(const int vsize)
{
    m_res.resize(vsize);
    m_size = vsize;
    m_count = 0;
}

void myPeriodic::writeRes(long double period)
{
    std::ofstream fp("DELTA_C.dat");
    fp << ios_base::fixed;
    for (unsigned int n=1; n < m_count; n++) {
    // write absolute deltas between consecutive waking ups.
        // Pay attention: delta is expressed in *** usec *** units (10^-6 sec).
        fp << m_res[n] - m_res[n-1]<< std::endl;
    }
    fp.close();
}    
                   
