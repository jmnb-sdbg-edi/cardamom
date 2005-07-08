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

#include <Time/clockservice/Macro.hpp>
#include <limits>

using namespace Cdmw::clock;

extern  CLOCK_API const TimeBase::TimeT macro::Mask48 = FIX_ULL(0x00FFFFFFFFFFFFFF);
extern  CLOCK_API const TimeBase::TimeT macro::Mask32 = FIX_ULL(0x00000000FFFFFFFF);
extern  CLOCK_API const TimeBase::TimeT macro::Mask16 = FIX_ULL(0x000000000000FFFF);

extern  CLOCK_API const TimeBase::TimeT macro::MAX_TIME_VAL =
#if defined(WIN32) && (_MSC_VER < 1300)
    // ops can not solve this value in win32 with old compiler
    18446744073709551615;
#else
    std::numeric_limits<TimeBase::TimeT>::max();
#endif
