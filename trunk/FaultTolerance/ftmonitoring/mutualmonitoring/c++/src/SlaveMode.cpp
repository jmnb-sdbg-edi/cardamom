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
#include "mutualmonitoring/SlaveMode.hpp"
#include "mutualmonitoring/MutualMonitoring.hpp"

// include files
#include <Foundation/ossupport/OS.hpp>

using namespace CdmwFT;
using namespace std;

namespace Cdmw {
namespace FT {
namespace Supervision
{

// constructor
SlaveMode::SlaveMode(MutualMonitoring* monitoring, 
#if !defined(EXT_RELIABLE_MULTICAST) && !defined(EXT_UNRELIABLE_MULTICAST)
                     CORBA::ORB_ptr orb,
                     std::string pushCorbaloc,
#endif
                     LivelinessRepository* repository,
                     std::string hostname)
  : Mode(monitoring, repository, hostname),
    m_candidate(""),
    m_masterHostname("")
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"hostname="<<hostname);

#ifdef EXT_RELIABLE_MULTICAST       
    m_pushRef = MonitoringLibrary::MutualMulticastPushClientFactory::get_unique_object();
#else
#ifdef EXT_UNRELIABLE_MULTICAST
    m_pushRef = MonitoringLibrary::MutualMulticastPushClientFactoryUnreliable::get_unique_object();
#else
	//Construct the miopref associated with MulticastPushMonitorable corba objects
	CORBA::String_var corbaStr = CORBA::string_dup(pushCorbaloc.c_str());
	CORBA::Object_var group = orb->string_to_object(corbaStr.in());
	m_pushRef = MonitoringLibrary::MulticastPushMonitorable::_unchecked_narrow(group.in());	
#endif
#endif
}

// destructor
SlaveMode::~SlaveMode()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

//uncomment here if you want a user copy constructor
//  SlaveMode::SlaveMode(const SlaveMode& t)
//  {
//    //attribute=t.attribute
//  }

//  SlaveMode& 
//  SlaveMode::operator=(const SlaveMode& t)
//  {
//    if (this != &t) {
//      //clean all attributes (delete[] attribute...)
//      //attribute=t.attribute
//   }
//   return *this;
//  }    

  void SlaveMode::i_am_alive(MonitoringLibrary::Location hostname, 
                             MonitoringLibrary::RequestType request_type,
                             unsigned int counterVal)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"hostname="<<hostname<<",request_type="<<request_type
                        <<",counterVal="<<counterVal);
      //Update liveliness repository
      m_pRepository->update_liveliness(hostname,
                                       request_type,
                                       counterVal);
  }

  MutualMonitoringTimeoutType SlaveMode::is_alive(MonitoringLibrary::Location hostname,
                                                  unsigned int counterVal)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"hostname="<<hostname<<",counterVal="<<counterVal);
      //Acknowledge the master interrogation
      m_pushRef->i_am_alive(m_localHostname.c_str(), 
                            MonitoringLibrary::ACKNOWLEDGEMENT,
                            counterVal);  

      if (m_masterHostname == "")
          {
              CDMW_INFO(::Cdmw::FT::FTLogger::GetLogger(),"The local supervision remains in the slave mode");
          }
      else if ((m_masterHostname != "") &&
               (m_masterHostname != std::string(hostname)))
          {
              CDMW_INFO(::Cdmw::FT::FTLogger::GetLogger(),"The " << hostname << " supervision has taken the mastership");
              /* Useless with the counter processing

              // Remove the former master from the liveliness repository.
              // It only makes sense if the former master had been seen as a
              // slave by the current slave.
              m_pRepository->removeLiveliness((MonitoringLibrary::Location)m_masterHostname.c_str());
              */
          }
      else if (m_masterHostname == std::string(hostname))
          {
              // Refresh list of faulty slaves : this avoid to declare again a failure
              // already declared by the previous master when we become new master
              LocationList dummyList;
              m_pRepository->get_faultySlaves(dummyList, counterVal);
          }
	
      m_masterHostname = std::string(hostname);
	
      return MASTER_INTERROGATION;
  }	

  MutualMonitoringTimeoutType SlaveMode::wake_up()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      MutualMonitoringTimeoutType currentTimeout, timeout = MASTER_INTERROGATION;
      currentTimeout = m_pMonitoring->get_current_timeout();
      
      if (currentTimeout == MASTER_INTERROGATION)
          {
              Cdmw::OsSupport::OS::Timeval timenow;
              
              timenow = Cdmw::OsSupport::OS::get_time();
              
              if (m_masterHostname != "")
                  {
                      CDMW_INFO(::Cdmw::FT::FTLogger::GetLogger(),
                                "The " << m_masterHostname << " master supervision has not interrogated\n" 
                                << "Failure detection time sec  : " << timenow.seconds << "\n"
                                << "Failure detection time usec : " << timenow.microseconds );
                      
                  }
		
              //Update liveliness repository in precising its own candidature to the mastership
              m_pRepository->update_liveliness(m_localHostname.c_str(),
                                               MonitoringLibrary::CANDIDATURE,
                                               0);
              
              //Declare its own mastership candidature to the other slave of the system 
              m_pushRef->i_am_alive(m_localHostname.c_str(), 
                                    MonitoringLibrary::CANDIDATURE,
                                    0);
              
              timeout = MASTER_ELECTION;
          }
      else if (currentTimeout == MASTER_ELECTION)
          {
              //Get the designated candidate to the mastership
              try
                  {
                      m_candidate = std::string(m_pRepository->get_masterCandidate());
                  }
              catch(const Cdmw::InternalErrorException& ex)
                  {
                      CDMW_ERROR(::Cdmw::FT::FTLogger::GetLogger(),"No Candidate: location file may not contain the current standalone host");
                      CDMW_THROW1(Cdmw::InternalErrorException,
                                  "There should be a candidate when get_masterCandidate() is called");
                  }
              //Check if the designated candidate is the local monitoring
              if (m_candidate == m_localHostname) timeout = SLAVE_ACKNOWLEDGEMENT;
              else timeout = MASTER_CONFIRMATION;
              
          }
      else if (currentTimeout == MASTER_CONFIRMATION)
          {
              //Reset the liveliness of the monitoring that was supposed to take the mastership
              //It did not do it in time
              m_pRepository->reset_liveliness(const_cast<char*>(m_candidate.c_str()));
              
              //Get another designated candidate to the mastership
              m_candidate = std::string(m_pRepository->get_masterCandidate());
              
              //Check if the designated candidate is the local monitoring
              if (m_candidate == m_localHostname) timeout = SLAVE_ACKNOWLEDGEMENT;
              else timeout = MASTER_CONFIRMATION;		
          }
      
      return timeout;
  }

} // End namespace Supervision
}  // End namespace FT
}  // End namespace Cdmw
