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

#ifndef INCL_CDMW_FT_SUPERVISION_SIMPLEMONITORING_ACKNOWLEDGINGMODE_HPP
#define INCL_CDMW_FT_SUPERVISION_SIMPLEMONITORING_ACKNOWLEDGINGMODE_HPP

// private include files
#include "simplemonitoring/SimpleMonitoringMode.hpp"
#include "simplemonitoring/SimpleMulticastPullMonitorable_impl.hpp"

// include files
#include <string>

#include <Foundation/orbsupport/CORBA.hpp>

namespace Cdmw {
namespace FT {
namespace Supervision
{
	
// class definition
/**
*Purpose:
*<p>    This class implements operations for the acknowledging mode
*       of the simple monitoring
*
*/

class AcknowledgingMode : public SimpleMonitoringMode {

public:

  // Constructor
  AcknowledgingMode(CORBA::ORB_ptr orb, 
		    std::string pullCorbaloc, 
		    std::string pushCorbaloc);
  
  // Destructor
  virtual ~AcknowledgingMode();

  // At acknowledging side no processing is done when timeout occurs
  SimpleMonitoringTimeoutType wake_up();

  // Start the acknowledging mode
  void start();

  // Stop the acknowledging mode
  void stop();

  // At acknowledging side this is not implemented
  void attach_observer(CdmwFT::MonitoringLibrary::MonitoringObserver_ptr observer)
    throw (CdmwFT::MonitoringLibrary::AlreadyExists);

  // At acknowledging side this is not implemented
  void detach_observer(CdmwFT::MonitoringLibrary::MonitoringObserver_ptr observer)
    throw (CdmwFT::MonitoringLibrary::NotFound);
  
protected:

private:
  CdmwFT::MonitoringLibrary::SimpleMulticastPullMonitorable_impl* m_pPullMonitorableImpl;
};

} // End namespace Supervision
}  // End namespace FT
}  // End namespace Cdmw
#endif

