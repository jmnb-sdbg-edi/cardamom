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
#if defined(EXT_RELIABLE_MULTICAST) || defined(EXT_UNRELIABLE_MULTICAST)
#include "mutualmonitoring/MutualMulticastFactories.hpp"
#endif

#include "mutualmonitoring/MutualMonitoring.hpp"
#include "mutualmonitoring/SlaveMode.hpp"
#include "mutualmonitoring/MasterMode.hpp"

// include files
#include "ftmonitoring/common/MacroDefs.hpp"
#include "common/LivelinessRepository.hpp"
#include "common/MutualLivelinessRepository.hpp"
#include "common/SingleShotTimer.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/common/Exception.hpp"

#include "ftmonitoring/common/MacroDefs.hpp" 

using namespace CdmwFT;
using namespace std;

namespace Cdmw {
namespace FT {
namespace Supervision
{
	
MutualMonitoring* MutualMonitoring::M_pMonitoring = NULL;

// constructor
MutualMonitoring::MutualMonitoring()
    : m_pPullMonitorableImpl(NULL),
      m_pPushMonitorableImpl(NULL)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

// destructor
MutualMonitoring::~MutualMonitoring()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
    if (m_pTimer != NULL) {
        delete m_pTimer;
        m_pTimer = NULL;
    }
    if (m_pMode != NULL) {
        delete m_pMode;
        m_pMode = NULL;
    }
    if (m_pRepository != NULL) {
        delete m_pRepository;
        m_pRepository = NULL;
    }
    if (m_pPullMonitorableImpl != NULL) {
        m_pPullMonitorableImpl->_remove_ref();
        m_pPullMonitorableImpl = NULL;
    }
    if (m_pPushMonitorableImpl != NULL) {
        m_pPushMonitorableImpl->_remove_ref();
        m_pPushMonitorableImpl = NULL;
    }
}

//uncomment here if you want a user copy constructor
//  MutualMonitoring::MutualMonitoring(const MutualMonitoring& t)
//  {
//    //attribute=t.attribute
//  }

//  MutualMonitoring& 
//  MutualMonitoring::operator=(const MutualMonitoring& t)
//  {
//    if (this != &t) {
//      //clean all attributes (delete[] attribute...)
//      //attribute=t.attribute
//   }
//   return *this;
//  }    

//That internal method is used to calculate a delay between given time and now
int getDelay(Cdmw::OsSupport::OS::Timeval time)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"time={"<<time.seconds<<","<<time.microseconds<<"}");
    Cdmw::OsSupport::OS::Timeval timenow;
	
	timenow = Cdmw::OsSupport::OS::get_time();

	//Return a delay in ms
    int delay =  ((timenow.seconds - time.seconds)*(int)1E6 + timenow.microseconds - time.microseconds)/1000;
    CDMW_LOG_FUNCTION_RETURN(delay);
    return delay;
}

MutualMonitoring* MutualMonitoring::setup(CORBA::ORB_ptr orb, 
		                                  std::string pullCorbaloc, 
				                          std::string pushCorbaloc,
				                          ElectionStrategyType strategy,
				                          const MonitoringLibrary::Locations & locations,
                                          TimeoutMap timeoutMap)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"pullCorbaloc="<<pullCorbaloc<<",pushCorbaloc="<<pushCorbaloc
                        << ",strategy="<<strategy);
      if (M_pMonitoring ==  NULL)
          {
              M_pMonitoring = new MutualMonitoring();
          }
      M_pMonitoring->m_started = false;
      M_pMonitoring->m_masterHostname = "";
      
      M_pMonitoring->m_orb = CORBA::ORB::_duplicate(orb);
      M_pMonitoring->m_pullCorbaloc = pullCorbaloc;
      M_pMonitoring->m_pushCorbaloc = pushCorbaloc;
      M_pMonitoring->m_localHostname = 
          Cdmw::OsSupport::OS::get_hostname();
      
      M_pMonitoring->m_pRepository = 
          new MutualLivelinessRepository(strategy, locations);
      
      //Timeout values in ms
      M_pMonitoring->m_timeoutMap = timeoutMap;
      
      //Create a single shot timer
      M_pMonitoring->m_pTimer= 
          new SingleShotTimer(M_pMonitoring, 
                              M_pMonitoring->m_timeoutMap[MASTER_INTERROGATION]);
      
      //Initialize the first timeout to MASTER_INTERROGATION
      M_pMonitoring->m_currentTimeout = MASTER_INTERROGATION;
      
      //Setup the reliable Multicast only once for SlaveMode and MasterMode
#ifdef EXT_RELIABLE_MULTICAST        
      MonitoringLibrary::MutualMulticastPushClientFactory::setup(orb, pushCorbaloc);
      MonitoringLibrary::MutualMulticastPullClientFactory::setup(orb, pullCorbaloc);
#endif
#ifdef EXT_UNRELIABLE_MULTICAST
      MonitoringLibrary::MutualMulticastPushClientFactoryUnreliable::setup(orb, pushCorbaloc);
      MonitoringLibrary::MutualMulticastPullClientFactoryUnreliable::setup(orb, pullCorbaloc);
#endif

      //At the beginning, every mutual monitoring starts in the slave mode
      
      CDMW_INFO(::Cdmw::FT::FTLogger::GetLogger(),"The local supervision is in the SLAVE mode");
      M_pMonitoring->m_pMode =
          new SlaveMode(M_pMonitoring,
#if !defined(EXT_RELIABLE_MULTICAST) && !defined(EXT_UNRELIABLE_MULTICAST)
                        orb,
                        pushCorbaloc, 
#endif
                        M_pMonitoring->m_pRepository,
                        M_pMonitoring->m_localHostname);

      CDMW_DEBUG(::Cdmw::FT::FTLogger::GetLogger(),"MutualMonitoring: new SlaveMode done");

      M_pMonitoring->m_pPullMonitorableImpl = 
          new MonitoringLibrary::MutualMulticastPullMonitorable_impl(orb,
                                                                     M_pMonitoring,
                                                                     pullCorbaloc,
                                                                     M_pMonitoring->m_localHostname);
      
      CDMW_DEBUG(::Cdmw::FT::FTLogger::GetLogger(),"MutualMonitoring: new PullMonitorable done");

      M_pMonitoring->m_pPushMonitorableImpl = 
          new MonitoringLibrary::MutualMulticastPushMonitorable_impl(orb,
                                                                     M_pMonitoring,
                                                                     pushCorbaloc,
                                                                     M_pMonitoring->m_localHostname);
      
      CDMW_DEBUG(::Cdmw::FT::FTLogger::GetLogger(),"MutualMonitoring: new PushMonitorable done");
      
      return M_pMonitoring;
  
  }
  
  void MutualMonitoring::cleanup()
  {
    if (M_pMonitoring != NULL) {
        delete M_pMonitoring;
        M_pMonitoring = NULL;
    }
  }
  
  void MutualMonitoring::add_location(const MonitoringLibrary::Location location)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"location="<<location);
      m_pRepository->add_location(location);
  }

  void MutualMonitoring::remove_location(const MonitoringLibrary::Location location)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"location="<<location);
      m_pRepository->remove_location(location);
  }

  void MutualMonitoring::set_locations(const MonitoringLibrary::Locations & locations)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      m_pRepository->set_locations(locations);
  }	  

  void MutualMonitoring::reset_locations()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      m_pRepository->reset_locations();
  }

  MonitoringLibrary::Locations * MutualMonitoring::get_started_locations ()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      // Eventually we could add m_localHostname if necessary 
      return m_pRepository->get_started_locations();
  }

  void MutualMonitoring::attach_observer(const MonitoringLibrary::PrimaryBackup::MutualMonitoringObserver_ptr observer) throw(MonitoringLibrary::AlreadyExists)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      MutualObserverVector::iterator itObs;
      if ( CORBA::is_nil(observer) )
          {
              CDMW_WARN(::Cdmw::FT::FTLogger::GetLogger(),"MutualMonitoring::attach_observer observer is nil!");
          }
      else
          {
              for (itObs = m_observers.begin(); itObs != m_observers.end(); itObs++)
                  {
                      if ((*itObs)->_is_equivalent(observer))
                          {
                              MonitoringLibrary::AlreadyExists ex;
                              CDMW_LOG_FUNCTION_EXCEPTION(ex);
                              throw ex;
                          }
                  }
              MonitoringLibrary::PrimaryBackup::MutualMonitoringObserver_var eltObserver;
              eltObserver = MonitoringLibrary::PrimaryBackup::MutualMonitoringObserver::_duplicate(observer);
              m_observers.push_back(eltObserver);
          }
  }

  void MutualMonitoring::detach_observer(const MonitoringLibrary::PrimaryBackup::MutualMonitoringObserver_ptr observer) throw(MonitoringLibrary::NotFound)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      MutualObserverVector::iterator itObs;
      if ( CORBA::is_nil(observer) )
          {
              CDMW_WARN(::Cdmw::FT::FTLogger::GetLogger(),"MutualMonitoring::detach_observer observer is nil!");
          }
      else
          {
              for (itObs = m_observers.begin(); itObs != m_observers.end(); itObs++)
                  {
                      if ((*itObs)->_is_equivalent(observer))
                          {
                              break;
                          }
                  }
              if (itObs != m_observers.end())
                  {
                      m_observers.erase(itObs);
                  }
              else
                  {
                      MonitoringLibrary::NotFound ex;
                      CDMW_LOG_FUNCTION_EXCEPTION(ex);
                      throw ex;
                  }        
          }
  }

  void MutualMonitoring::start() throw (MonitoringLibrary::AlreadyStarted)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      // We could use the m_mutex here but it is impossible to have a start_timer
      // before the start : is_alive or wake_up can't be received 
      if (m_started == false)
          {
              m_started = true;
              m_pPullMonitorableImpl->start();
              m_pPushMonitorableImpl->start();
              m_pTimer->start();
              m_pTimer->setup(m_timeoutMap[m_currentTimeout]);
              m_pTimer->start_timer();       	
          }
      else 
          {
              MonitoringLibrary::AlreadyStarted ex;
              CDMW_LOG_FUNCTION_EXCEPTION(ex);
              throw ex;
          }        
  }

  void MutualMonitoring::stop() throw (MonitoringLibrary::NotStarted)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      if (m_started)
      { 
          CDMW_MUTEX_GUARD(m_mutex);

          CDMW_DEBUG(::Cdmw::FT::FTLogger::GetLogger(),"MutualMonitoring::stop");
          m_started = false;
          m_pTimer->stop();
          m_pPullMonitorableImpl->stop();
          m_pPushMonitorableImpl->stop();
          // Set to clean slave mode
          m_pRepository->remove_livelinesses();
          if (m_currentTimeout != MASTER_INTERROGATION)
          {
              m_currentTimeout = MASTER_INTERROGATION;
              //Delete the master instance
              delete m_pMode;
              CDMW_INFO(::Cdmw::FT::FTLogger::GetLogger(),"The local supervision stop set in clean SLAVE mode");
              m_pMode = 
                  new SlaveMode(M_pMonitoring,
#if !defined(EXT_RELIABLE_MULTICAST) && !defined(EXT_UNRELIABLE_MULTICAST)
                                m_orb.in(),
                                m_pushCorbaloc, 
#endif
                                m_pRepository,
                                m_localHostname);
          }
      }
      else {
          MonitoringLibrary::NotStarted ex;
          CDMW_LOG_FUNCTION_EXCEPTION(ex);
          throw ex;
      }        
  }

  void MutualMonitoring::i_am_alive(MonitoringLibrary::Location hostname, 
		                    MonitoringLibrary::RequestType request_type,
				    unsigned int counterVal)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"hostname="<<hostname<<",request_type="<<request_type
                        <<",counterVal="<<counterVal);
      // DEBUG_ECHO << "MutualMonitoring::i_am_alive" << std::endl;
      //Delegate the i_am_alive processing to the mode (MasterMode or SlaveMode)
      CDMW_MUTEX_GUARD(m_mutex);
      
      m_pMode->i_am_alive(hostname, request_type, counterVal);
  }

  void MutualMonitoring::is_alive(MonitoringLibrary::Location hostname,
		  		  unsigned int counterVal)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"hostname="<<hostname<<",counterVal="<<counterVal);
      if (strcmp(hostname, m_localHostname.c_str()) != 0)
      {
          CDMW_MUTEX_GUARD(m_mutex);

          // DEBUG_ECHO << "MutualMonitoring::is_alive" << std::endl;
          MutualMonitoringTimeoutType oldTimeout;
          oldTimeout = m_currentTimeout;

          m_masterHostname = std::string(hostname);
	
          //Stop the timer
          m_pTimer->cancel_timer();

          //Delegate the is_alive processing to the mode (MasterMode or SlaveMode)
          m_currentTimeout = m_pMode->is_alive(hostname, counterVal);

          if ((m_currentTimeout == MASTER_INTERROGATION)&&
              (oldTimeout == SLAVE_ACKNOWLEDGEMENT))
          {
              //Delete the master instance
              delete m_pMode;
		
              //Switch to the slave mode (release the mastership)
              CDMW_INFO(::Cdmw::FT::FTLogger::GetLogger(),"The local supervision switches into the SLAVE mode");

              m_pMode = 
                  new SlaveMode(this->M_pMonitoring,
#if !defined(EXT_RELIABLE_MULTICAST) && !defined(EXT_UNRELIABLE_MULTICAST)
                                m_orb.in(),
                                m_pushCorbaloc, 
#endif
                                m_pRepository,
                                m_localHostname);
          }
	
          //Set and restart the timer with the new timeout
          m_pTimer->setup(m_timeoutMap[m_currentTimeout]);
          m_pTimer->start_timer();
      }
      else if (m_currentTimeout != SLAVE_ACKNOWLEDGEMENT)
      {
          stop();
          CDMW_FATAL(::Cdmw::FT::FTLogger::GetLogger(),"ANOTHER supervision is already started at the SAME host !\n"
                     << "Mutual Monitoring stopped and abort process" );
          // ANOTHER supervision is already started at the same host
          // The verification could fail if mutual monitorings start same time
          // and polling period is long
          assertion_failed();
      }
  }

  MutualMonitoringTimeoutType MutualMonitoring::get_current_timeout()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      // DEBUG_ECHO << "MutualMonitoring::get_current_timeout" << std::endl;
      CDMW_LOG_FUNCTION_RETURN(m_currentTimeout);
      return m_currentTimeout; 
  }

  std::vector<MonitoringLibrary::PrimaryBackup::MutualMonitoringObserver_var> MutualMonitoring::get_observers()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      return m_observers;
  }

  void MutualMonitoring::wake_up() throw () 
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      CDMW_MUTEX_GUARD(m_mutex);

      // DEBUG_ECHO << "MutualMonitoring::wake_up()" << std::endl;	
      Cdmw::OsSupport::OS::Timeval timenow;
	
      //Get the time before the processing. 
      //That time will be used to calculate the delay between now and the moment where
      //the timer will be set again. 
      //The delay will be taken away from the next timeout to avoid any drift
      timenow = Cdmw::OsSupport::OS::get_time();
      
      MutualMonitoringTimeoutType oldTimeout;
      oldTimeout = m_currentTimeout;
      
      CDMW_DEBUG_3(::Cdmw::FT::FTLogger::GetLogger(),"MutualMonitoring::wake_up() : oldTimeout = "<< m_timeoutMap[oldTimeout]);

      //Delegate the wake_up processing to the mode (MasterMode or SlaveMode)
      m_currentTimeout = m_pMode->wake_up();

      CDMW_DEBUG_3(::Cdmw::FT::FTLogger::GetLogger(),"MutualMonitoring::wake_up() : currentTimeout = "
                 << m_timeoutMap[m_currentTimeout] );

      if ((m_currentTimeout == SLAVE_ACKNOWLEDGEMENT)&&
          ((oldTimeout == MASTER_ELECTION) || (oldTimeout == MASTER_CONFIRMATION)))
          {
              //Delete the slave instance
              delete m_pMode;
              
              //Switch to the master mode
              CDMW_INFO(::Cdmw::FT::FTLogger::GetLogger(),"The local supervision switches into the MASTER mode");

              m_pMode =
                  new MasterMode(this->M_pMonitoring,
#if !defined(EXT_RELIABLE_MULTICAST) && !defined(EXT_UNRELIABLE_MULTICAST)
                                 m_orb.in(),
                                 m_pullCorbaloc,
#endif
                                 m_pRepository,
                                 m_localHostname,
                                 m_masterHostname);
		
              //Set the timer with the new timeout
              //The delay is not taken away from the timeout
              //It does not make sense to do it since a new phase is starting
              m_pTimer->setup(m_timeoutMap[m_currentTimeout]);
              
          }
      else 
          {
              
              //Set the timer with the new timeout minus the calculated delay
              try {
                  m_pTimer->setup(m_timeoutMap[m_currentTimeout]-getDelay(timenow));
              }
              catch (const Cdmw::Exception & ex)
                  {
                      CDMW_WARN(::Cdmw::FT::FTLogger::GetLogger(),"MutualMonitoring::wake_up : exception '"
                                << ex << "' in timer setup: timer should be stopped!" );
                  }
          }
      
      //Restart the timer with the new timeout
      m_pTimer->start_timer();
  }

} // End namespace Supervision
}  // End namespace FT
}  // End namespace Cdmw
