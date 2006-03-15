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

/**
 * @author Fabrizio Morciano <fmorciano@selex-si.com>
 */
#include "Time/clockservice/test/testcommon/TimeTestManager.hpp"
#include <iostream>
#include <string>
#include "Foundation/common/Assert.hpp"

using namespace std;

const TimeBase::TimeT SEC_IN_TIME_UNITS = 10000000; 
const TimeBase::TimeT CDMW_TOL = (TimeBase::TimeT)1000000;
const int MAX_EXEC = 10;
const int WAIT_SEC = 1;
const int RATE_2 = 3;

///////////////////////////////////////////////////////////////////////////////

// compare two numbers with tolerance (default CARDAMOM tolerance)
int
is_equal_w_tol (const TimeBase::TimeT num1,
                const TimeBase::TimeT num2,
                const TimeBase::TimeT tol)
{
    TimeBase::TimeT diff;
    diff = delta(num1,num2);
    if (diff <= tol)
        return 1; // Ok
    return 0; // Too difference
}

///////////////////////////////////////////////////////////////////////////////

TimeBase::TimeT 
delta(const TimeBase::TimeT num1,
      const TimeBase::TimeT num2)
{
    if (num1 > num2)
        return num1 -num2;
    return num2-num1;
}

///////////////////////////////////////////////////////////////////////////////

int
delta_sign(const TimeBase::TimeT num1,
           const TimeBase::TimeT num2)
{
    if (num1 >= num2)
        return 1;
    return -1;
}

///////////////////////////////////////////////////////////////////////////////
// End Of File
///////////////////////////////////////////////////////////////////////////////
