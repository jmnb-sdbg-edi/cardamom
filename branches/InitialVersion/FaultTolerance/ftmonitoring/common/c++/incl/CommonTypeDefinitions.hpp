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

#ifndef INCL_CDMW_FT_SUPERVISION_COMMON_COMMONTYPEDEFINITIONS_HPP
#define INCL_CDMW_FT_SUPERVISION_COMMON_COMMONTYPEDEFINITIONS_HPP

#include <list>
#include <string>

namespace Cdmw {
namespace FT {
namespace Supervision {

    /* Type defining the strategy type passed to mutual liveliness repository
       to create its corresponding switch strategy */
    enum ElectionStrategyType
    {
        HOST_PRIORITY,
        CPU_LOAD
    };      

    /**
    *Purpose:
    *<p> The type used to return internal list of locations
    */
    typedef std::list<std::string> LocationList;
   
    /**
    *Purpose:
    *<p> The type used to pass command line arguments
    */
    typedef std::list<std::string> ArgvList;    
};
}  // End namespace FT
}  // End namespace Cdmw

#endif

