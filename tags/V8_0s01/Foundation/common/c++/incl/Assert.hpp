/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
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


#ifndef CDMW_COMMON_ASSERT_HPP
#define CDMW_COMMON_ASSERT_HPP

#include <cassert>
#include <iostream>

#include "Foundation/common/MacroDefs.h"

namespace Cdmw
{

    /**
    * Function pointer type for assertion failed.
    */
    typedef void (*assertion_failed_handler)();


    /**
    * Sets the assertion failed handler.
    */
    assertion_failed_handler set_assertion_failed(assertion_failed_handler handler);


    /**
    * Calls the assertion failed handler set by set_assertion_failed and aborts
    * or aborts directly if no assertion failed handler has been set.
    */
    void assertion_failed();

}

//
// If CDMW_NO_ASSERT is defined all assertion are removed from the code
//
#ifdef CDMW_NO_ASSERT

#   define CDMW_ASSERT( condition ) ((void)0)

//
// Else assertion are active, but their behaviour depends on other flags
//
#else

//
// If CDMW_ASSERT_NO_THROW, the process abort when an assertion failed
//
#   ifdef CDMW_ASSERT_NO_THROW

//#       define CDMW_ASSERT( condition ) assert(condition)

#ifndef _WIN32
extern "C"
{
    extern void abort(void);
}

#endif

// Redefined to allow better control with codewizard
#   define CDMW_ASSERT( condition ) if ( !( condition ) ) { \
        std::cerr << "Cdmw: failed assertion in " \
        __FILE__ << ":" <<  __LINE__ << std::endl; \
        Cdmw::assertion_failed(); \
    }

#   define CDMW_NEVER_HERE() { \
        std::cerr << "Cdmw: executing unreachable code in " \
        __FILE__ << ":" <<  __LINE__ << std::endl; \
        ::abort(); \
    }

#   else
#       error CDMW_NOT_IMPLEMENTED_FEATURE
#   endif

#endif

#endif // CDMW_COMMON_ASSERT_HPP

