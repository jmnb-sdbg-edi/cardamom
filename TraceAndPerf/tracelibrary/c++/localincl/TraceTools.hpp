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


#ifndef INCL_TRACE_TOOLS_HPP 
#define INCL_TRACE_TOOLS_HPP 

#include "Foundation/orbsupport/CORBA.hpp"

#include <iostream>

/**
* Root namespace for CDMW runtime.
*/
namespace Cdmw
{
/**
* Namespace for CDMW Trace types and data.
*/
namespace Trace
{

/**
* Purpose:
* <p> Check if a string allocated by CORBA function is valid 
*     or not. If CORBA has not been able to allocate memory
*     the OutOfMemoryException is thrown.
* 
*@param ptr the CORBA pointer to be checked
*
*@exception OutOfMemoryException
*
*/ 
inline
void 
check_corba_str( const char* ptr )
        throw (OutOfMemoryException)
{
    if ( ptr == NULL ) {

        CDMW_THROW( OutOfMemoryException );
    }

}


/*
#define CDMW_TRACE_DEBUG(mes) \
do { \
    std::cout << mes << std::endl; \
} while(0)
*/

#define CDMW_TRACE_DEBUG(mes)


} // End namespace Trace

} // End namespace Cdmw


#endif // INCL_TRACE_TOOLS_HPP

