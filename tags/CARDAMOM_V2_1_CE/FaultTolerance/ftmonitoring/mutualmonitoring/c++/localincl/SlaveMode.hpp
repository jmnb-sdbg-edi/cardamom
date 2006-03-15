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

#ifndef INCL_CDMW_FT_SUPERVISION_MUTUALMONITORING_SLAVEMODE_HPP
#define INCL_CDMW_FT_SUPERVISION_MUTUALMONITORING_SLAVEMODE_HPP

// include files
#include <string>

#include "MutualMonitoringType.hpp"
#include "common/LivelinessRepository.hpp"
#include "Mode.hpp"

#include <Foundation/orbsupport/CORBA.hpp>
#include "idllib/CdmwFTMonitoring.stub.hpp"
#include "common/CdmwFTMulticastMonitorable.stub.hpp"

namespace Cdmw {
namespace FT {
namespace Supervision
{

// class definition
/**
*Purpose:
*<p>    This class implements operations for the slave mode
*       of the mutual monitoring
*
*/

class MutualMonitoring;

class SlaveMode : public Mode {

public:

  // Constructor
  SlaveMode(MutualMonitoring* monitoring, 
#if !defined(EXT_RELIABLE_MULTICAST) && !defined(EXT_UNRELIABLE_MULTICAST)
	    CORBA::ORB_ptr orb,
	    std::string pushCorbaloc,
#endif
	    LivelinessRepository* repository,
	    std::string hostname);
  
  // Destructor
  virtual ~SlaveMode();

  /**
   * Purpose: records host responses to the master
   * <p> 
   *
   *@param hostname name of the responding host
   *@param request_type can be "Acknowledgment" or "Candidature" (in election phase)
   *@param counterVal is used for internal identification of messages
   */
  void i_am_alive(CdmwFT::MonitoringLibrary::Location hostname, 
		  CdmwFT::MonitoringLibrary::RequestType request_type,
		  unsigned int counterVal);

  /**
   * Purpose: ask the slave host if it is alive
   * <p> 
   *
   *@param hostname name of the interrogating host
   *@param counterVal is used for internal identification of messages
   */
  MutualMonitoringTimeoutType is_alive(CdmwFT::MonitoringLibrary::Location hostname,
					 unsigned int counterVal);

  // when timeout occurs the update is done
  MutualMonitoringTimeoutType wake_up();
  
protected:

private:
  std::string m_candidate;
  std::string m_masterHostname;
  
};

} // End namespace Supervision
}  // End namespace FT
}  // End namespace Cdmw
#endif

