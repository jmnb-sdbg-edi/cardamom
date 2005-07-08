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
                                                                                                 
/* -*- C++ -*-
 * $Id: HighResClock.cc,v 1.1.1.1 2005-07-08 14:31:04 moghrabi Exp $
 */ 
#include "ctools/time/HighResClock.h"
#include "ctools/time/HighResTime.h"
#include <sys/time.h>
#include "SysInfo.h"

ctools::time::HighResTime
ctools::time::HighResClock::getTime() 
{
    ctools::time::HighResTime time;
    HighResClock::getTime(time);
    return time;
}

void
ctools::time::HighResClock::getTime(::ctools::time::HighResTime& hrtime)
{
    // Here I use the RDTSC instruction provided by the Pentium
    // Processor family to access the clock counter. To avoid problem
    // with out of order execution, and to make sure that the clock
    // cycle number is accessed really where the call appears in the
    // source code a cpuid instruction is added. 
    //    
    //   unsigned long cpuid;
    unsigned long long rdtsc;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (rdtsc));

    // Convert clock number into nano seconds
    unsigned long long timeNS = 
      static_cast<unsigned long long>(rdtsc * CLOCK_PERIOD_NS);
    
    long cMilliSec = static_cast<long>(timeNS/POW_6_TEN);
    
    long cNanoSec = static_cast<long>(timeNS - (cMilliSec*POW_6_TEN));
    
    hrtime.set(cMilliSec, cNanoSec);
}



double
ctools::time::HighResClock::getClockFrequency ()
{
    double clockFreq = CLOCK_FREQUENCY;
    double freq = static_cast<double>(clockFreq);
    
    return freq;
}

double
ctools::time::HighResClock::getClockPeriod ()
{
  double period = CLOCK_PERIOD_NS;
  double jp = static_cast<double>(period);
  return jp;
}

unsigned long long
ctools::time::HighResClock::getClockTickCount ()
{
    unsigned long long int rdtsc;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (rdtsc));
    return rdtsc;
}
