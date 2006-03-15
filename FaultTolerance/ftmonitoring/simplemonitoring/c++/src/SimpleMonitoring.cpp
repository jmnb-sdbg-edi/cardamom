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
#include "simplemonitoring/SimpleMonitoring.hpp"
#include "simplemonitoring/InterrogatingMode.hpp"
#include "simplemonitoring/AcknowledgingMode.hpp"

// include files
#include "ftmonitoring/common/MacroDefs.hpp"
#include "common/LivelinessRepository.hpp"
#include "common/SimpleLivelinessRepository.hpp"

#include <Foundation/ossupport/OS.hpp>
#include "ftmonitoring/common/MacroDefs.hpp"

namespace Cdmw {
namespace FT {
namespace Supervision
{

SimpleMonitoring* SimpleMonitoring::M_pMonitoring = NULL;

bool ActivePerfTrace = false;
    
void active_perf_trace(int sig)
{
    if (ActivePerfTrace == true)
        ActivePerfTrace = false;
    else
        ActivePerfTrace = true;
};



// constructor
SimpleMonitoring::SimpleMonitoring() :
    m_pMode(NULL)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
    // signal configuration to activate trace
    static struct sigaction action;
    action.sa_handler=active_perf_trace;
    sigemptyset(&action.sa_mask);
    sigaction(SIGUSR1, &action, NULL);
}

// destructor
SimpleMonitoring::~SimpleMonitoring()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
	if (m_pRepository != NULL) {
        delete m_pRepository;
        m_pRepository = NULL;
    }
    if (m_pTimer != NULL) {
        delete m_pTimer;
        m_pTimer = NULL;
    }
    if (m_pMode != NULL) {
        delete m_pMode;
        m_pMode = NULL;
    }
}

//uncomment here if you want a user copy constructor
//  SimpleMonitoring::SimpleMonitoring(const SimpleMonitoring& t)
//  {
//    //attribute=t.attribute
//  }

//  SimpleMonitoring& 
//  SimpleMonitoring::operator=(const SimpleMonitoring& t)
//  {
//    if (this != &t) {
//      //clean all attributes (delete[] attribute...)
//      //attribute=t.attribute
//   }
//   return *this;
//  }    

//That internal method is used to calculate a delay between given time and now
int get_delay(Cdmw::OsSupport::OS::Timeval time)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"time={"<<time.seconds<<","<<time.microseconds<<"}");
    Cdmw::OsSupport::OS::Timeval timenow;
	
	timenow = Cdmw::OsSupport::OS::get_time();

	//Return a delay in ms
	int delay =  ((timenow.seconds - time.seconds)*(int)1E6 + timenow.microseconds - time.microseconds)/1000;
    CDMW_LOG_FUNCTION_RETURN(delay);
    return delay;
	
}

  SimpleMonitoring* SimpleMonitoring::setup_interrogatingMode(CORBA::ORB_ptr orb, 
                                                              std::string pullCorbaloc, 
                                                              std::string pushCorbaloc,
                                                              int daemonAcknowledgement)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"pullCorbaloc="<<pullCorbaloc<<",pushCorbaloc="<<pushCorbaloc
                        << ",daemonAcknowledgement="<<daemonAcknowledgement);
      if (M_pMonitoring ==  NULL)
          {
              CDMW_DEBUG(::Cdmw::FT::FTLogger::GetLogger(),"InterrogatingMode: call new SimpleMonitoring..");
              M_pMonitoring = new SimpleMonitoring();
          }
      
      CDMW_DEBUG(::Cdmw::FT::FTLogger::GetLogger(),"InterrogatingMode: call new SimpleLivelinessRepository..");
      M_pMonitoring->m_started = false;
      
      M_pMonitoring->m_pRepository = new SimpleLivelinessRepository();
      
      M_pMonitoring->m_timeoutMap[DAEMON_ACKNOWLEDGEMENT] = daemonAcknowledgement;
     
      //Create a single shot timer
      M_pMonitoring->m_pTimer = 
          new SingleShotTimer(M_pMonitoring, 
                              M_pMonitoring->m_timeoutMap[DAEMON_ACKNOWLEDGEMENT]);
      
      //Initialize the first timeout to DAEMON_ACKNOWLEDGEMENT
      M_pMonitoring->m_currentTimeout = DAEMON_ACKNOWLEDGEMENT;
      
      CDMW_DEBUG(::Cdmw::FT::FTLogger::GetLogger(),"InterrogatingMode: call InterrogatingMode.." );
    
      M_pMonitoring->m_pMode = new InterrogatingMode(orb, 
                                                     pullCorbaloc,
                                                     pushCorbaloc, 
                                                     M_pMonitoring->m_pRepository);
      
      
      return M_pMonitoring;
  } 
  
  SimpleMonitoring* SimpleMonitoring::setup_acknowledgingMode(CORBA::ORB_ptr orb, 
		                                             std::string pullCorbaloc, 
				                             std::string pushCorbaloc)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"pullCorbaloc="<<pullCorbaloc<<",pushCorbaloc="<<pushCorbaloc);
      if (M_pMonitoring ==  NULL)
          {
              M_pMonitoring = new SimpleMonitoring();
          }
      
      M_pMonitoring->m_pTimer = NULL;
      M_pMonitoring->m_started = false;
      CDMW_DEBUG(::Cdmw::FT::FTLogger::GetLogger(),"SimpleMonitoring::setupAcknowledgingMode new AcknowledgingMode");
      M_pMonitoring->m_pMode = new AcknowledgingMode(orb, 
                                                     pullCorbaloc,
                                                     pushCorbaloc);
      
      return M_pMonitoring;
  }

  
  void SimpleMonitoring::cleanup()
  {
    if (M_pMonitoring != NULL) {
        delete M_pMonitoring;
        M_pMonitoring = NULL;
    }
  }
  
  void SimpleMonitoring::add_location(const CdmwFT::MonitoringLibrary::Location location)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"location="<<location);
      m_pRepository->add_location(location);
  }

  void SimpleMonitoring::remove_location(const CdmwFT::MonitoringLibrary::Location location)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"location="<<location);
      m_pRepository->remove_location(location);
  }

  void SimpleMonitoring::set_locations(const CdmwFT::MonitoringLibrary::Locations & locations)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      m_pRepository->set_locations(locations);
  }

  void SimpleMonitoring::reset_locations()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      m_pRepository->reset_locations();
  }

  void SimpleMonitoring::attach_observer(CdmwFT::MonitoringLibrary::MonitoringObserver_ptr observer) throw (CdmwFT::MonitoringLibrary::AlreadyExists)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      m_pMode->attach_observer(observer);	
  }

  void SimpleMonitoring::detach_observer(CdmwFT::MonitoringLibrary::MonitoringObserver_ptr observer) throw (CdmwFT::MonitoringLibrary::NotFound)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      m_pMode->detach_observer(observer);
  }

  void SimpleMonitoring::start() throw (CdmwFT::MonitoringLibrary::AlreadyStarted)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      if (m_started == false)
          {
              m_started = true;
              m_pMode->start();
              if (m_pTimer != NULL)
                  {
                      CDMW_DEBUG(::Cdmw::FT::FTLogger::GetLogger(),"SimpleMonitoring::start:pTimer");
                      m_pTimer->start(); 
                      CDMW_DEBUG(::Cdmw::FT::FTLogger::GetLogger(),"SimpleMonitoring::setup:pTimer");
                      m_pTimer->setup(m_timeoutMap[m_currentTimeout]);
                      CDMW_DEBUG(::Cdmw::FT::FTLogger::GetLogger(),"SimpleMonitoring::start_timer:pTimer"); 
                      m_pTimer->start_timer();	
                  }
          }
      else {
          CdmwFT::MonitoringLibrary::AlreadyStarted ex;
          CDMW_LOG_FUNCTION_EXCEPTION(ex);
          throw ex;
      }
  }

  void SimpleMonitoring::stop() throw (CdmwFT::MonitoringLibrary::NotStarted)
  {
     if (m_started)
      {
        m_started = false;
        if (m_pTimer != NULL)
        {
            CDMW_DEBUG(::Cdmw::FT::FTLogger::GetLogger(),"SimpleMonitoring::stop::stop_timer");
            m_pTimer->stop();
        }
        CDMW_DEBUG(::Cdmw::FT::FTLogger::GetLogger(), "SimpleMonitoring::stop::m_pMode->stop");
	    m_pMode->stop();
        
      }
    else throw CdmwFT::MonitoringLibrary::NotStarted();
  }

  void SimpleMonitoring::wake_up() throw () 
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      Cdmw::OsSupport::OS::Timeval timenow;
	
      //Get the time before the processing. 
      //That time will be used to calculate the delay between now and the moment where
      //the timer will be set again. 
      //The delay will be taken away from the next timeout to avoid any drift
      timenow = Cdmw::OsSupport::OS::get_time();
      
      SimpleMonitoringTimeoutType oldTimeout;
      oldTimeout = m_currentTimeout;

      CDMW_DEBUG_3(::Cdmw::FT::FTLogger::GetLogger(),"SimpleMonitoring::wake_up() : oldTimeout = "
                 << m_timeoutMap[oldTimeout]);
      
      //Delegate the wake_up processing to the mode (InterrogatingMode)
      m_currentTimeout = m_pMode->wake_up();
      
      CDMW_DEBUG_3(::Cdmw::FT::FTLogger::GetLogger(),"SimpleMonitoring::wake_up() : currentTimeout = "
                 << m_timeoutMap[m_currentTimeout]);
      
      //Set and restart the timer with the new timeout minus the calculated delay
      m_pTimer->setup(m_timeoutMap[m_currentTimeout]-get_delay(timenow));
      m_pTimer->start_timer();

      if (ActivePerfTrace)
          {
              Cdmw::OsSupport::OS::Timeval timeend;
              timeend = Cdmw::OsSupport::OS::get_time();
              CDMW_INFO(::Cdmw::FT::FTLogger::GetLogger(),"Elapsed time in function LocalFaultDetector_impl::wakeUp(): "
                        <<((timeend.seconds*1000000)+ timeend.microseconds) - ((timenow.seconds*1000000) + timenow.microseconds)
                        <<" microseconds");
          }
  }

} // End namespace Supervision
}  // End namespace FT
}  // End namespace Cdmw
