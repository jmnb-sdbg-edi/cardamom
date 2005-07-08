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

#ifndef CONSTS_HPP
#define CONSTS_HPP

// 1 second in units of 10e-7 sec
const TimeBase::TimeT SEC_IN_TIME_UNITS = 10000000; 
// tolerance of 100 microsec
const TimeBase::TimeT CDMW_TOL = (TimeBase::TimeT)1000000;
// max number of loop execution
const int MAX_EXEC = 10;
// seconds to wait in some tests
const int WAIT_SEC = 1;
// rate of second virtual clock
const int RATE_2 = 3;

#endif

