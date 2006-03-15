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

#include "ctools/time/HighResTime.h"

ctools::time::HighResTime::HighResTime(time_t clockTicks) 
{
    this->set(clockTicks);
}

ctools::time::HighResTime::HighResTime(time_t msec, time_t nsec)
{ 
    this->set(msec, nsec);
}

ctools::time::HighResTime::HighResTime(const HighResTime& time) 
{
    *this = time;
}

ctools::time::HighResTime::~HighResTime() { }

ctools::time::HighResTime::time_t 
ctools::time::HighResTime::millisec() const
{ 
    return msec_; 
}

void   
ctools::time::HighResTime::millisec(time_t msec) 
{ 
    msec_ = msec; 
}

ctools::time::HighResTime::time_t
ctools::time::HighResTime::nanosec() const 
{ 
    return nsec_; 
}

void   
ctools::time::HighResTime::nanosec(time_t nsec) 
{ 
    nsec_ = nsec; 
}

void 
ctools::time::HighResTime::set(time_t clockTicks) 
{
    long double timeNS = static_cast<long double>(clockTicks) * CLOCK_PERIOD_NS;

    long cMilliSec = static_cast<long>(timeNS/POW_6_TEN);
    long cNanoSec = static_cast<long>(timeNS - (cMilliSec *POW_6_TEN));

   this->set(cMilliSec, cNanoSec);
}

void 
ctools::time::HighResTime::set(time_t msec, time_t nsec) 
{
   msec_ = msec;
   nsec_ = nsec;
   
   time_t msecInNsec = nsec_ / POW_6_TEN;
   if (msecInNsec) {
      msec_ += msecInNsec;
      nsec_ -= msecInNsec * POW_6_TEN;
   }
}

void 
ctools::time::HighResTime::reset() 
{ 
   this->set(0, 0); 
}

long double 
ctools::time::HighResTime::toMillisec() const 
{
   return msec_ + static_cast<long double>(nsec_) / POW_6_TEN; 
}

long double 
ctools::time::HighResTime::toMicrosec() const 
{
   return msec_* POW_3_TEN + static_cast<long double>(nsec_) / POW_3_TEN; 
}

long double 
ctools::time::HighResTime::toNanosec() const 
{
   return msec_* POW_6_TEN + nsec_;
}


ctools::time::HighResTime
ctools::time::HighResTime::operator+(const HighResTime& rhs) 
{
    return HighResTime(this->millisec() + rhs.millisec(),
                       this->nanosec() + rhs.nanosec());  
}
 
ctools::time::HighResTime
ctools::time::HighResTime::operator-(const HighResTime& rhs) 
{
    return HighResTime(this->millisec() - rhs.millisec(),
                       this->nanosec() - rhs.nanosec());  
}

const ctools::time::HighResTime&
ctools::time::HighResTime::operator=(const HighResTime& rhs) 
{
   this->set(rhs.millisec(), rhs.nanosec());
   return *this;
}

std::ostream& 
operator<<(std::ostream& os, const  ctools::time::HighResTime& time)
{
     os << time.millisec() << " msec " << time.nanosec() << " nsec";
     return os;
}
