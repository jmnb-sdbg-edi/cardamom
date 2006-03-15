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

#ifndef INCL_CDMW_FT_SUPERVISION_MUTUALMONITORING_MODE_HPP
#define INCL_CDMW_FT_SUPERVISION_MUTUALMONITORING_MODE_HPP

// private include files
#include "mutualmonitoring/MutualMonitoringType.hpp"
//#include "mutualmonitoring/MutualMonitoring.hpp"

// include files
#include <string>
#include <vector>

#include "common/LivelinessRepository.hpp"

#include "idllib/CdmwFTMonitoring.stub.hpp"
#include "idllib/CdmwFTMutualMonitoring.stub.hpp"
#include "common/CdmwFTMulticastMonitorable.stub.hpp"

namespace Cdmw {
namespace FT {
namespace Supervision
{

// class definition
/**
*Purpose:
*<p>    This pure virtual class is mother of Master and Slave modes
*
*/

class MutualMonitoring;

class Mode {

public:

  Mode( MutualMonitoring* monitoring,
        LivelinessRepository* repository,
        std::string hostname );

  virtual ~Mode();
  
  virtual void i_am_alive(CdmwFT::MonitoringLibrary::Location hostname, 
		          CdmwFT::MonitoringLibrary::RequestType request_type,
			  unsigned int counterVal) = 0;

  virtual MutualMonitoringTimeoutType is_alive(CdmwFT::MonitoringLibrary::Location hostname,
		  				 unsigned int counterVal) = 0;

  virtual MutualMonitoringTimeoutType wake_up() = 0;

protected:
  Mode();

  MutualMonitoring* m_pMonitoring;
  CdmwFT::MonitoringLibrary::MulticastPullMonitorable_var m_pullRef;
  CdmwFT::MonitoringLibrary::MulticastPushMonitorable_var m_pushRef;
  LivelinessRepository* m_pRepository;
  // CORBA::ORB_var m_orb;
  std::string m_localHostname;
  
private:
  
};

} // End namespace Supervision
}  // End namespace FT
}  // End namespace Cdmw
#endif

