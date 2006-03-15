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

#ifndef INCL_CDMW_FT_SUPERVISION_MUTUALMONITORING_HPP
#define INCL_CDMW_FT_SUPERVISION_MUTUALMONITORING_HPP

// private include files
#include "mutualmonitoring/MutualMonitoringType.hpp"
#include "mutualmonitoring/Mode.hpp"
#include "mutualmonitoring/MutualMulticastPullMonitorable_impl.hpp"
#include "mutualmonitoring/MutualMulticastPushMonitorable_impl.hpp"

// include files
#include <map>

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/osthreads/Mutex.hpp"

#include "common/MulticastMonitoring.hpp"
#include "ftmonitoring/common/CommonTypeDefinitions.hpp"

#include "idllib/CdmwFTMonitoring.stub.hpp"
#include "idllib/CdmwFTMutualMonitoring.stub.hpp"
#include "common/CdmwFTMulticastMonitorable.stub.hpp"

namespace Cdmw {
namespace FT {
namespace Supervision
{
	
// class definition
// This class is not supposed to be used by several threads
// So no thread safe mecanism are present (for example in setup, start/stop..)

class MutualMonitoring : virtual public Supervision::MulticastMonitoring {

public:

  typedef std::vector<CdmwFT::MonitoringLibrary::PrimaryBackup::MutualMonitoringObserver_var> MutualObserverVector;

  typedef std::map<MutualMonitoringTimeoutType,int> TimeoutMap;

  virtual ~MutualMonitoring();

  /**
   * Purpose: setup the mutual monitoring
   * <p> 
   *
   *@param the orb
   *@param pullCorbaloc: multicast Corbaloc used to create ref to interrogate hosts
   *@param pushCorbaloc: multicast Corbaloc used to create ref to receive answer
   *@param strategy: strategy for the elections, for the moment only HOST_PRIORITY 
   *@param locations: hosts to be monitored
   *@param timeoutMap: map of timeouts for the differents mutual monitoring phases
   */
  static MutualMonitoring* setup(CORBA::ORB_ptr orb, 
		                 std::string pullCorbaloc, 
				 std::string pushCorbaloc,
				 ElectionStrategyType strategy,
				 const CdmwFT::MonitoringLibrary::Locations & locations,
				 TimeoutMap timeoutMap);
  
  /**
   * Puropse: cleanup the mutual monitoring
   */
  static void cleanup();
  
  // For add_/remove_/set_/reset_location see MutualLivelinessRepository
  void add_location(const CdmwFT::MonitoringLibrary::Location location);

  void remove_location(const CdmwFT::MonitoringLibrary::Location location);

  void set_locations(const CdmwFT::MonitoringLibrary::Locations & locations);

  void reset_locations();

  CdmwFT::MonitoringLibrary::Locations * get_started_locations ();

  /**
   * Purpose: 
   * <p> Attach an observer that want to be warned of start, switch to primary,
   *     and failure of an host of the mutual monitoring
   *
   *@exception CdmwFT::MonitoringLibrary::AlreadyExists if observer was already attached
   */
  void attach_observer(const CdmwFT::MonitoringLibrary::PrimaryBackup::MutualMonitoringObserver_ptr observer) throw(CdmwFT::MonitoringLibrary::AlreadyExists);

  /**
   * Purpose: Detach observer
   * <p> 
   *
   *@exception CdmwFT::MonitoringLibrary::NotFound if observer was not attached
   */
  void detach_observer(const CdmwFT::MonitoringLibrary::PrimaryBackup::MutualMonitoringObserver_ptr observer) throw(CdmwFT::MonitoringLibrary::NotFound);

  /**
   * Purpose:
   * <p> Start the mutual monitoring (start the push&pull monitorables and timer)
   *
   *@exception CdmwFT::MonitoringLibrary::AlreadyStarted if it is already started
   */
  void start() throw(CdmwFT::MonitoringLibrary::AlreadyStarted);

  /**
   * Purpose:
   * <p> Stop the simple monitoring (stop the push&pull and timer and go to slave mode)
   *
   *@exception CdmwFT::MonitoringLibrary::NotStarted if it was not started
   */
  void stop() throw(CdmwFT::MonitoringLibrary::NotStarted);

  // delegate to corresponding mode (Slave or Master)
  void i_am_alive(CdmwFT::MonitoringLibrary::Location hostname, 
		  CdmwFT::MonitoringLibrary::RequestType request_type,
		  unsigned int counterVal);

  /**
   * Purpose:
   * <p> At each call of is_alive the timer is canceled, the is_alive
   * of the corresponding mode is called, the mode possibly switch to Slave,
   * a new timeout is calculated and timer restarted
   *
   *@param hostname name of the interrogating host
   *@param counterVal is used for internal identification of messages
   */
  void is_alive(CdmwFT::MonitoringLibrary::Location hostname,
		unsigned int counterVal);

  /**
   * Purpose:
   * <p> get the current timeout type (SLAVE_ACKNOWLEDGEMENT, MASTER_INTERROGATION,
   *     MASTER_CONFIRMATION, MASTER_ELECTION)
   */
  MutualMonitoringTimeoutType get_current_timeout();

  /**
   * Purpose:
   * <p> get the current list of observers
   */
  std::vector<CdmwFT::MonitoringLibrary::PrimaryBackup::MutualMonitoringObserver_var> get_observers();

  /**
   * Purpose:
   * <p> called at timeout of SLAVE_ACKNOWLEDGEMENT, MASTER_ELECTION, MASTER_CONFIRMATION
   * periods : call wake_up of the mode, possibly change of mode, calculate new timeout,
   * and restart timer
   *
   */
  void wake_up() throw();
  
protected:
  
    MutualObserverVector m_observers;
 
private:
  MutualMonitoring();

  static MutualMonitoring* M_pMonitoring;
  Mode* m_pMode;
  MutualMonitoringTimeoutType m_currentTimeout;
  bool m_started;
  std::string m_localHostname;
  std::string m_masterHostname;
  std::string m_pullCorbaloc;
  std::string m_pushCorbaloc;
  CORBA::ORB_var m_orb;
  CdmwFT::MonitoringLibrary::MutualMulticastPullMonitorable_impl* m_pPullMonitorableImpl;
  CdmwFT::MonitoringLibrary::MutualMulticastPushMonitorable_impl* m_pPushMonitorableImpl;
   
  TimeoutMap m_timeoutMap; 
  // Mutex protecting timer use (double start or stop) and liveliness use
  // (full map coherency) at global level
  Cdmw::OsSupport::Mutex m_mutex;

};

} // End namespace Supervision
}  // End namespace FT
}  // End namespace Cdmw
#endif

