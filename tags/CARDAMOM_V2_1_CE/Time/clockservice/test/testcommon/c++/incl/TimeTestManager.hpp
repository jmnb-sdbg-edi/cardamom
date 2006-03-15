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

#ifndef TIME_TEST_MANAGER_HPP
#define TIME_TEST_MANAGER_HPP
/**
 * @author Fabrizio Morciano <fmorciano@amjsv.it>
 */

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/testutils/CORBATestManager.hpp"
#include "Time/clockservice/CdmwCosClock.hpp"

#include <stdexcept>
#include <map>
#include <string>
#include <sstream>
#include <vector>


// 1 second in units of 10e-7 sec
extern const TimeBase::TimeT SEC_IN_TIME_UNITS;
// tolerance of 100 microsec
extern const TimeBase::TimeT CDMW_TOL;
// max number of loop execution
extern const int MAX_EXEC;
// seconds to wait in some tests
extern const int WAIT_SEC;
// rate of second virtual clock
extern const int RATE_2;

TimeBase::TimeT
delta(const TimeBase::TimeT num1,
      const TimeBase::TimeT num2);

int
delta_sign(const TimeBase::TimeT num1,
           const TimeBase::TimeT num2);

int
is_equal_w_tol (const TimeBase::TimeT num1,
                const TimeBase::TimeT num2,
                const TimeBase::TimeT tol=CDMW_TOL);

template <class T>
int 
sign(T value)
{
    if(value>=0)
        return  1;
    return -1;
}

#endif /* TIME_TEST_MANAGER_HPP */

