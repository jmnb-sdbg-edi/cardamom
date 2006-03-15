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

// private include files
#include "mutualmonitoring/Mode.hpp"

// include files

namespace Cdmw {
namespace FT {
namespace Supervision
{
	
// constructor
Mode::Mode(MutualMonitoring* monitoring,
           LivelinessRepository* repository,
           std::string hostname)
  : m_pMonitoring(monitoring),
    m_pRepository(repository),
    m_localHostname(hostname)    
{
}

// protected constructor
Mode::Mode()
{
}

// destructor
Mode::~Mode()
{
}

//uncomment here if you want a user copy constructor
//  Mode::Mode(const Mode& t)
//  {
//    //attribute=t.attribute
//  }

//  Mode& 
//  Mode::operator=(const Mode& t)
//  {
//    if (this != &t) {
//      //clean all attributes (delete[] attribute...)
//      //attribute=t.attribute
//   }
//   return *this;
//  }    
  
} // End namespace Supervision
}  // End namespace FT
}  // End namespace Cdmw
