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

#ifndef INCL_CDMW_FT_MIOP_MUTEX_HPP
#define INCL_CDMW_FT_MIOP_MUTEX_HPP
 
#include <Foundation/osthreads/Mutex.hpp>
#include <Foundation/osthreads/MutexGuard.hpp>


namespace Cdmw {
namespace FT {

    extern Cdmw::OsSupport::Mutex miop_cohort_mutex;
};
};


#define CDMW_FT_GUARD_MIOP_CALLS
#ifdef CDMW_FT_GUARD_MIOP_CALLS

// macros to protect each MIOP call with a mutex
#define CDMW_FT_BEGIN_MIOP_MUTEX { CDMW_MUTEX_GUARD(::Cdmw::FT::miop_cohort_mutex);
#define CDMW_FT_END_MIOP_MUTEX  } 

#else

#define CDMW_FT_BEGIN_MIOP_MUTEX
#define CDMW_FT_END_MIOP_MUTEX

#endif // CDMW_FT_GUARD_MIOP_CALLS


#endif  // INCL_CDMW_FT_MIOP_MUTEX_HPP

