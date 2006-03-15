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

#include "ftmonitoring/common/MacroDefs.hpp"
#include "mutualmonitoring/MasterMode.hpp"
#include "mutualmonitoring/MutualMonitoring.hpp"

using namespace CdmwFT;

namespace Cdmw {
namespace FT {
namespace Supervision
{
	
// constructor
MasterMode::MasterMode(MutualMonitoring* monitoring,
#if !defined(EXT_RELIABLE_MULTICAST) && !defined(EXT_UNRELIABLE_MULTICAST)
                       CORBA::ORB_ptr orb,
                       std::string pullCorbaloc,
#endif
	               LivelinessRepository* repository,
		       std::string localHostname,
		       std::string masterHostname)
  : Mode(monitoring, repository, localHostname),
    m_switchToMasterNotified(false),
    m_masterHostname(masterHostname),
    m_counterVal(0)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"localHostname="<<localHostname<<",masterHostname="<<masterHostname);

	// Remove the former master from the liveliness repository.
	// It only makes sense if the former master had been seen as a slave
	// by the current master.
	m_pRepository->remove_liveliness(const_cast<char*>(masterHostname.c_str()));

	//Reset the liveliness repository in order to detected slaves failures that might occur during the changeover phase
	m_pRepository->reset_livelinesses();

	//Construct the miopref associated with MulticastPullMonitorable corba objects
#ifdef EXT_RELIABLE_MULTICAST   
    m_pullRef = MonitoringLibrary::MutualMulticastPullClientFactory::get_unique_object();
#else
#ifdef EXT_UNRELIABLE_MULTICAST
    m_pullRef = MonitoringLibrary::MutualMulticastPullClientFactoryUnreliable::get_unique_object();
#else
	CORBA::String_var corbaStr = CORBA::string_dup(pullCorbaloc.c_str());
	CORBA::Object_var group = orb->string_to_object(corbaStr.in());
	m_pullRef = MonitoringLibrary::MulticastPullMonitorable::_unchecked_narrow(group.in());
#endif
#endif
	//First time the master interrogates slaves
	m_pullRef->is_alive(m_localHostname.c_str(), m_counterVal);
}

// destructor
MasterMode::~MasterMode()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

//uncomment here if you want a user copy constructor
//  MasterMode::MasterMode(const MasterMode& t)
//  {
//    //attribute=t.attribute
//  }

//  MasterMode& 
//  MasterMode::operator=(const MasterMode& t)
//  {
//    if (this != &t) {
//      //clean all attributes (delete[] attribute...)
//      //attribute=t.attribute
//   }
//   return *this;
//  }    

  void MasterMode::i_am_alive(MonitoringLibrary::Location hostname, 
		              MonitoringLibrary::RequestType request_type,
			      unsigned int counterVal)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"hostname="<<hostname<<",request_type="<<request_type
                        <<",counterVal="<<counterVal);
      CDMW_DEBUG_3(::Cdmw::FT::FTLogger::GetLogger(),"MasterMode::i_am_alive");
      MutualMonitoringTimeoutType currentTimeout;
      currentTimeout = m_pMonitoring->get_current_timeout();
        
      if ((currentTimeout == SLAVE_ACKNOWLEDGEMENT) &&
          (request_type == MonitoringLibrary::ACKNOWLEDGEMENT))
          {
              //Update liveliness repository with the current slave acknowledgement
              if (m_pRepository->update_liveliness(hostname,
                                                   request_type,
                                                   counterVal))	
                  {
                      CDMW_DEBUG_3(::Cdmw::FT::FTLogger::GetLogger(),hostname << " says i_am_alive to this master and is new" );
                      //Notify all observers with the new slave
                      for (size_t i=0;i<m_pMonitoring->get_observers().size();i++)
                          {
                              m_pMonitoring->get_observers()[i].in()->on_start(hostname);	
                          }
                  }
          }
      
  }

  MutualMonitoringTimeoutType MasterMode::is_alive(MonitoringLibrary::Location hostname,
		  				     unsigned int counterVal)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"hostname="<<hostname<<",counterVal="<<counterVal);
      CDMW_DEBUG_3(::Cdmw::FT::FTLogger::GetLogger(),"MasterMode::is_alive" );
      MutualMonitoringTimeoutType timeout;
      
      //Prepare the master election
      
      //The local monitoring is a candidate to the mastership
      m_pRepository->update_liveliness(m_localHostname.c_str(),
                                       MonitoringLibrary::CANDIDATURE,
                                       0);
      
      //The interrogating master is also a candidate to the mastership
      m_pRepository->update_liveliness(hostname,
                                       MonitoringLibrary::CANDIDATURE,
                                       0);
      MonitoringLibrary::Location candidate;
      
      //Get the candidate that will hold the mastership
      candidate = m_pRepository->get_masterCandidate();
      
      if (strcmp(candidate, m_localHostname.c_str()) == 0) timeout = SLAVE_ACKNOWLEDGEMENT;
      else timeout = MASTER_INTERROGATION;
      
      return timeout;
  }	

    struct WarnObserverOfSwitchToPrimary {
        WarnObserverOfSwitchToPrimary() { }
        void operator()(const MonitoringLibrary::PrimaryBackup::MutualMonitoringObserver_var & mutualObserver)
        {
           mutualObserver.in()->on_switch_to_primary();
        }
    };

    /**
     * A functor for use to warn of failure of the specified host
     */
    class WarnObserverOfHostFailure
    {
    public:
        /**
         *Purpose:
         * <p>Constructor.
         *
         *@param failedHost the host to be declared on_failure.
         *
         */
        WarnObserverOfHostFailure(std::string failedHost)
            : m_host(failedHost)
        {}

        void operator()(const MonitoringLibrary::PrimaryBackup::MutualMonitoringObserver_var & mutualObserver)
        {
           mutualObserver.in()->on_failure(m_host.c_str()); 
        }

    private:
        std::string m_host;
    };

  MutualMonitoringTimeoutType MasterMode::wake_up()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
	
      MutualMonitoringTimeoutType currentTimeout;
      currentTimeout = m_pMonitoring->get_current_timeout();
	
      if (currentTimeout == SLAVE_ACKNOWLEDGEMENT)
        {
            LocationList faultySlavesList;
            LocationList::iterator itList;
            
            //Get faulty slave
            m_pRepository->get_faultySlaves(faultySlavesList, m_counterVal);
            
            // It is not supposed to have several threads executing monitoring code
            // so this counter don't need to have mutex
            if (m_counterVal < (unsigned int)2E32 -1) m_counterVal += 1;
            else m_counterVal = 0;
            
            //Interrogate slaves
            m_pullRef->is_alive(m_localHostname.c_str(), m_counterVal);
            
            //The on_switch_to_primary and on_failure method has not been
            //sent in the wake of the mode change so as to guarantee the 
            //switchover.
            //The notifications are only sent after two interrogations of 
            //the slaves (including the master confirmation) 
            std::vector<MonitoringLibrary::PrimaryBackup::MutualMonitoringObserver_var> mutualObservers = m_pMonitoring->get_observers();
            if (m_switchToMasterNotified == false)
                {
                    
                    m_switchToMasterNotified = true;
                    
                    //Notify all observers with the master mode switch
                    for_each(mutualObservers.begin(), mutualObservers.end(),
                             WarnObserverOfSwitchToPrimary());
                    
                    std::string voidStr = "";
                    
                    if (voidStr != m_masterHostname)
                        {
                            //Notify all observers with the former master failure
                            for_each(mutualObservers.begin(), mutualObservers.end(),
                                     WarnObserverOfHostFailure(m_masterHostname));
                        }			
                }
            
            //Notify all observers with faulty slaves
            for (itList=faultySlavesList.begin(); itList!=faultySlavesList.end();itList++)
                {
                    if (*itList != m_localHostname)
                        {
                            CDMW_INFO(::Cdmw::FT::FTLogger::GetLogger(),"The " << *itList << " slave supervision has failed");
                            for_each(mutualObservers.begin(), mutualObservers.end(),
                                     WarnObserverOfHostFailure(*itList));
                        }			
                }
            
        }
      
      CDMW_LOG_FUNCTION_RETURN (SLAVE_ACKNOWLEDGEMENT);
      return SLAVE_ACKNOWLEDGEMENT;
  }	


} // End namespace Supervision
}  // End namespace FT
}  // End namespace Cdmw
