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

#include "FaultTolerance/ftcommon/FTLocation.hpp"

namespace Cdmw {
namespace FT {

FTLocation::FTLocation(std::string hostName,
		       std::string applicationName,
		       std::string processName)
    throw()
  : m_hostName(hostName),
    m_applicationName(applicationName),
    m_processName(processName)
{
}
        
FTLocation::~FTLocation()
    throw()
{
}

/**
 * get the FT location
 * @return the location in FT::Location type
 *
 */
::FT::Location
FTLocation::get_FT_Location()
{
  ::FT::Location loc;
  loc.length( 3 );
  loc[0].id = m_hostName.c_str();
  loc[0].kind = "hostname";
  loc[1].id = m_applicationName.c_str();
  loc[1].kind = "applicationname";
  loc[2].id = m_processName.c_str();
  loc[2].kind = "processname";  

  return loc;
}

}; // namespace FT
}; // namespace Cdmw

