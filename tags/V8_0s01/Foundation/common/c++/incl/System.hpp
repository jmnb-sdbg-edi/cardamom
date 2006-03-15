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


#ifndef CDMW_SYSTEM_H
#define CDMW_SYSTEM_H

#ifdef _MSC_VER

# pragma warning( disable : 4786 )
#   define for if (1) for

#endif

#ifdef _MSC_VER

// define size_t type in std namespace

namespace std
{
    typedef ::size_t size_t;
}

#endif

#ifdef _MSC_VER

// The auto_ptr implementation shipped with visual is not compliant
//  and doesn't provide reset(), but it is broken as it used an assignement
//  operator with const auto_ptr&, we use this deviation
#   define CDMW_RESET_AUTO_PTR(autoPtr, type, ptr) \
    autoPtr = std::auto_ptr<type>( (ptr) );

// CDMW_RESET_AUTO_PTR(m_spImpl, ThreadImpl, new  ThreadImpl( this ) )
//
// is expanded as:
//
//    m_spImpl= std::auto_ptr<ThreadImpl>( (new  ThreadImpl( this )) )

#else
#   define CDMW_RESET_AUTO_PTR(autoPtr, type, ptr) \
    autoPtr.reset( (ptr) );


// CDMW_RESET_AUTO_PTR(m_spImpl, ThreadImpl, new  ThreadImpl( this ) )
//
// is expanded as:
//
//    m_spImpl.reset( (new  ThreadImpl( this )) )


#   endif

/****
// Note : "compare"implementation between stl and gnu is not identical
// See string_compare function in String.hpp
 
//Added for stl compatibility
#if defined __GNUC__ && !defined __SGI_STL_PORT
//GNUC with its own stdc++ lib
#define CDMW_COMPARE(pos1,n1,s) compare(s,pos1,n1)
#else
//__SGI_STL_PORT or others (default)
#define CDMW_COMPARE(pos1,n1,s) compare(pos1,n1,s)
#endif
****/



#endif // CDMW_SYSTEM_H

