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

#ifndef INCL_CDMW_FT_SUPERVISION_SIMPLEMONITORING_HPP
#define INCL_CDMW_FT_SUPERVISION_SIMPLEMONITORING_HPP

// private include files
#include "SimpleMonitoringMode.hpp"

// include files
#include <string>
#include <map>

#include <Foundation/orbsupport/CORBA.hpp>

#include "common/MulticastMonitoring.hpp"

#include "idllib/CdmwFTMonitoring.stub.hpp"

namespace Cdmw {
namespace FT {
namespace Supervision
{
	
// class definition
// This class is not supposed to be used by several threads
// So no thread safe mecanism are present (for example in setup, start/stop..)

class SimpleMonitoring : virtual public Supervision::MulticastMonitoring {

public:
  
  // Destructor
  virtual ~SimpleMonitoring();

  /**
   * Purpose: setup the interrogating mode
   * <p> 
   *
   *@param the orb
   *@param pullCorbaloc: multicast Corbaloc used to create ref to interrogate daemons
   *@param pushCorbaloc: multicast Corbaloc used to create ref to receive daemons answer
   *@param daemonAcknowledgement: This is the period of interrogation and timeout
   */
  static SimpleMonitoring* setup_interrogatingMode(CORBA::ORB_ptr orb, 
		                                  std::string pullCorbaloc, 
				                  std::string pushCorbaloc,
						  int daemonAcknowledgement); 
  
  /**
   * Purpose: setup the acknowledging mode
   * <p> 
   *
   *@param the orb
   *@param pullCorbaloc: multicast Corbaloc used to receive interrogation
   *@param pushCorbaloc: multicast Corbaloc used to answer
   */
  static SimpleMonitoring* setup_acknowledgingMode(CORBA::ORB_ptr orb, 
		                                  std::string pullCorbaloc, 
				                  std::string pushCorbaloc);

  /**
   * Purpose: cleanup the simple monitoring
   *
   */
  static void cleanup();                               
                                  
  // For add_/remove_/set_/reset_location see SimpleLivelinessRepository
  void add_location(const CdmwFT::MonitoringLibrary::Location location);

  void remove_location(const CdmwFT::MonitoringLibrary::Location location);

  void set_locations(const CdmwFT::MonitoringLibrary::Locations & locations);

  void reset_locations();

  // Operations to set observer : see InterrogatingMode
  void attach_observer(CdmwFT::MonitoringLibrary::MonitoringObserver_ptr observer)
    throw (CdmwFT::MonitoringLibrary::AlreadyExists);

  void detach_observer(CdmwFT::MonitoringLibrary::MonitoringObserver_ptr observer)
    throw (CdmwFT::MonitoringLibrary::NotFound);

  /**
   * Purpose:
   * <p> Start the simple monitoring (start the mode and timer)
   *
   *@exception CdmwFT::MonitoringLibrary::AlreadyStarted if it is already started
   */
  void start() throw(CdmwFT::MonitoringLibrary::AlreadyStarted);

  /**
   * Purpose:
   * <p> Stop the simple monitoring (stop the mode and timer)
   *
   *@exception CdmwFT::MonitoringLibrary::NotStarted if it was not started
   */
  void stop() throw(CdmwFT::MonitoringLibrary::NotStarted);
 
  /**
   * Purpose:
   * <p> called at timeout of the interrogation period :
   * call wake_up of the mode, calculate new timeout and restart timer
   *
   */
  void wake_up() throw();

protected:

private:

  SimpleMonitoring();

  static SimpleMonitoring* M_pMonitoring;
  bool m_started;
  SimpleMonitoringMode* m_pMode;
  SimpleMonitoringTimeoutType m_currentTimeout;
  std::map<SimpleMonitoringTimeoutType,int> m_timeoutMap;
  
};

}  // End namespace Supervision
}  // End namespace FT
}  // End namespace Cdmw
#endif

