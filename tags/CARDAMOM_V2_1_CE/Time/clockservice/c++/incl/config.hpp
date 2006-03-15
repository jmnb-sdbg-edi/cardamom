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

#ifndef CARDAMOM_CLOCK_CONFIG_HPP
#define CARDAMOM_CLOCK_CONFIG_HPP

/**
 * @author Angelo Corsaro <acorsaro@selex-si.com>
 * @author Fabrizio Morciano <fmorciano@selex-si.com>
 */

#if defined(WIN32) && defined(USE_DLL)
# ifdef LIBCLOCK_EXPORTS
#   define CLOCK_API __declspec(dllexport)
# else
#   define CLOCK_API __declspec(dllimport)
# endif
#else
# define CLOCK_API
#endif


// -- Clock Service Includes --
#include <TimeBaseC.h>

namespace Cdmw {  namespace clock {

    extern CLOCK_API const unsigned int DECIMAL_PRECISION;

    extern CLOCK_API const unsigned long RESOLUTION;

    /**
     *  @brief  Base of time for clock computation.
     *
     * Portable constant to use like base time.
     */
    extern  CLOCK_API const TimeBase::TimeT TIME_BASE;

} /* clock */} /*Cdmw*/


#ifdef  WIN32

# pragma warning (disable: 4786 4231)
# include <Time/clockservice/CORBAExceptionsOstream.hpp>

# define FIX_ULL(a) a

#else 

# define FIX_ULL(a) a ## ULL

#endif

#endif /* CARDAMOM_CLOCK_CONFIG_HPP */
