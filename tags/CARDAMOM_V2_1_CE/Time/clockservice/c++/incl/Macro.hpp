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

#ifndef CARDAMOM_CLOCK_MACRO_HPP
#define CARDAMOM_CLOCK_MACRO_HPP

#include "Time/clockservice/config.hpp"
#include "Time/clockservice/CosClockService.stub.hpp"

/**
 * @file 
 *
 * @author Raffaele Mele <rmele@progesi.it>
 * @author Fabrizio Morciano <fmorciano@yahoo.com>
 */

/**
 * @namespace Cdmw::clock::macro
 * @brief This namespace contains some macro required 
 * to manages TimeT structures.
 */
namespace Cdmw { namespace clock { namespace macro {

  extern CLOCK_API const TimeBase::TimeT Mask48;
  extern CLOCK_API const TimeBase::TimeT Mask32;
  extern CLOCK_API const TimeBase::TimeT Mask16;
  extern CLOCK_API const TimeBase::TimeT MAX_TIMET;

} /* macro */ } /* clock */ } /* Cdmw */


///////////////////////////////////////////////////////////////////////////////

#if defined(WIN32) && (_MSC_VER < 1300)

namespace   std {

    template <class T>  
    inline  T 
    min(T a , T b)
    {
    return a < b ? a : b; 
    }

    ///////////////////////////////////////////////////////////////////////////

    template <class T>
    inline  T 
    max(T a , T b)
    {
    return a > b ? a : b; 
    }
}
#endif

#endif /* CARDAMOM_CLOCK_MACRO_HPP */
