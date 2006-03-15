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

#ifndef INCL_CDMW_FT_SUPERVISION_SIMPLEMONITORING_INTERROGATINGMODE_HPP
#define INCL_CDMW_FT_SUPERVISION_SIMPLEMONITORING_INTERROGATINGMODE_HPP

// private include files
#include "simplemonitoring/SimpleMulticastPushMonitorable_impl.hpp"
#include "simplemonitoring/SimpleMonitoringMode.hpp"

// include files
#include <string>
#include <vector>

#include "common/LivelinessRepository.hpp"
#include "common/CdmwFTMulticastMonitorable.stub.hpp"

namespace Cdmw {
namespace FT {
namespace Supervision
{

// class definition
/**
*Purpose:
*<p>    This class implements operations for the interrogating mode
*       of the simple monitoring
*
*/

class InterrogatingMode : public SimpleMonitoringMode {

public:
  
  typedef std::vector<CdmwFT::MonitoringLibrary::MonitoringObserver_var> observerVector;
  
  // Constructor
  InterrogatingMode(CORBA::ORB_ptr orb, 
		    std::string pullCorbaloc, 
		    std::string pushCorbaloc,
		    LivelinessRepository* repository);
  
  // Destructor
  virtual ~InterrogatingMode();

  // At interrogating side when timeout occurs the interrogation&update is done
  SimpleMonitoringTimeoutType wake_up();

  // Start the interrogating mode
  void start();

  // Stop the interrogating mode
  void stop();

  /**
   * Purpose: records daemon responses and warn observers if one is on_start
   * <p> 
   *
   *@param hostname name of the responding daemon
   *@param request_type is "Acknowledgment" for i_am_alive
   *@param counterVal is used for internal identification of messages
   */
  void i_am_alive(CdmwFT::MonitoringLibrary::Location hostname,
                  CdmwFT::MonitoringLibrary::RequestType request_type,
                  unsigned int counterVal);
  
  /**
   * Purpose: Attach an observer that want to be warned of faulty daemons
   * <p> 
   *
   *@exception CdmwFT::MonitoringLibrary::AlreadyExists if observer was already attached
   */
  void attach_observer(CdmwFT::MonitoringLibrary::MonitoringObserver_ptr observer)
    throw (CdmwFT::MonitoringLibrary::AlreadyExists);

  /**
   * Purpose: Detach observer
   * <p> 
   *
   *@exception CdmwFT::MonitoringLibrary::NotFound if observer was not attached
   */
  void detach_observer(CdmwFT::MonitoringLibrary::MonitoringObserver_ptr observer)
    throw (CdmwFT::MonitoringLibrary::NotFound);

protected:

private:
  std::string m_localHostname;
  LivelinessRepository* m_pRepository;
  CdmwFT::MonitoringLibrary::MulticastPullMonitorable_var m_pullRef; 
  CdmwFT::MonitoringLibrary::SimpleMulticastPushMonitorable_impl* m_pPushMonitorableImpl;

  // map of observer, key is observer Id : an Id counter is used
  observerVector m_observers;
  unsigned int m_counterVal;
};

} // End namespace Supervision
}  // End namespace FT
}  // End namespace Cdmw
#endif

