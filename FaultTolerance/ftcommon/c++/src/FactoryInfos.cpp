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

#include "FaultTolerance/ftcommon/FactoryInfos.hpp"
#include "FaultTolerance/ftcommon/FTLocation.hpp"
//#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#define ECHO_HEADER() \
   "[FactoryInfos] (file: " << __FILE__ << ", line: " << __LINE__ << ")\n -->"

namespace Cdmw {
namespace FT {

FactoryInfos::FactoryInfos()
    throw()
{
  m_factoryInfos.length(0);
}
        
FactoryInfos::~FactoryInfos()
    throw()
{
}

/**
 * add the FT location
 *
 * @param hostname        Host name of the location     
 * @param applicationName Application name of the location
 * @param processName     Process name of the location
 */
void
FactoryInfos::add_ftLocation(const std::string hostName,
		             const std::string applicationName,
		             const std::string processName)
{
  int infosSize = m_factoryInfos.length();
  ::FT::FactoryInfo factoryInfo;
  // No factory passed at the moment
  factoryInfo.the_factory = ::FT::GenericFactory::_nil();
  // No criteria passed at the moment
  ::FT::Properties criteriaProp;
  criteriaProp.length( 0 );
  factoryInfo.the_criteria =  criteriaProp;
  // Create Location where object will be created
  FTLocation ftLoc(hostName, applicationName, processName);
  factoryInfo.the_location = ftLoc.get_FT_Location();

  m_factoryInfos.length(infosSize+1);
  m_factoryInfos[infosSize] = factoryInfo;
}

}; // namespace FT
}; // namespace Cdmw

