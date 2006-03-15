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
#include "simplemonitoring/SimpleMulticastFactories.hpp"
#endif

// include files
#include "ftmonitoring/common/MacroDefs.hpp"
#include "simplemonitoring/InterrogatingMode.hpp"

#include <Foundation/ossupport/OS.hpp>
#include "ftmonitoring/common/MacroDefs.hpp"

using namespace CdmwFT;

namespace Cdmw {
namespace FT {
namespace Supervision
{

// constructor
InterrogatingMode::InterrogatingMode(CORBA::ORB_ptr orb, 
		    		     std::string pullCorbaloc, 
		    		     std::string pushCorbaloc,
		    		     LivelinessRepository* repository)
  : m_pRepository(repository),
    m_pPushMonitorableImpl(NULL),
    m_counterVal(0)
{  
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"pullCorbaloc="<<pullCorbaloc<<",pushCorbaloc="<<pushCorbaloc);
        m_observers.clear();
	m_localHostname = Cdmw::OsSupport::OS::get_hostname();

#ifdef EXT_RELIABLE_MULTICAST
        MonitoringLibrary::SimpleMulticastPullClientFactory::setup(orb, pullCorbaloc);
        m_pullRef = MonitoringLibrary::SimpleMulticastPullClientFactory::get_unique_object();
#else
#ifdef EXT_UNRELIABLE_MULTICAST        
        MonitoringLibrary::SimpleMulticastPullClientFactoryUnreliable::setup(orb, pullCorbaloc);
        m_pullRef = MonitoringLibrary::SimpleMulticastPullClientFactoryUnreliable::get_unique_object();
#else
        // Create a pull group reference
	CORBA::String_var corbaStr = CORBA::string_dup(pullCorbaloc.c_str());
	CORBA::Object_var group = orb->string_to_object(corbaStr.in());
	m_pullRef = 
	MonitoringLibrary::MulticastPullMonitorable::_unchecked_narrow(group.in());
#endif
#endif

	m_pPushMonitorableImpl = 
	
        new MonitoringLibrary::SimpleMulticastPushMonitorable_impl(orb,
                                                                   pushCorbaloc,
                                                                   this);
	/* used if no on_start implemented
	new MonitoringLibrary::SimpleMulticastPushMonitorable_impl(orb,
							           pushCorbaloc,
								   repository);
	*/
}

// destructor
InterrogatingMode::~InterrogatingMode()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
    CDMW_DEBUG(::Cdmw::FT::FTLogger::GetLogger(),"~InterrogatingMode");
    if (m_pPushMonitorableImpl != NULL) {
        m_pPushMonitorableImpl->_remove_ref();
        m_pPushMonitorableImpl = NULL;
    }
}

//uncomment here if you want a user copy constructor
//  InterrogatingMode::InterrogatingMode(const InterrogatingMode& t)
//  {
//    //attribute=t.attribute
//  }

//  InterrogatingMode& 
//  InterrogatingMode::operator=(const InterrogatingMode& t)
//  {
//    if (this != &t) {
//      //clean all attributes (delete[] attribute...)
//      //attribute=t.attribute
//   }
//   return *this;
//  }    

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

        void operator()(const MonitoringLibrary::MonitoringObserver_var & observer)
        {
           observer.in()->on_failure(m_host.c_str()); 
        }

    private:
        std::string m_host;
    };

  SimpleMonitoringTimeoutType InterrogatingMode::wake_up()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      CDMW_DEBUG_3(::Cdmw::FT::FTLogger::GetLogger(),"InterrogatingMode::wake_up: Interrogate daemons, hostname:"
                << m_localHostname );
	
      LocationList faultyDaemonsList;
      LocationList::iterator itList;
		
      //Check daemons liveliness
      m_pRepository->get_faultySlaves(faultyDaemonsList, m_counterVal);
      
      // It is not supposed to have several threads executing monitoring code
      // so this counter don't need to have mutex
      if (m_counterVal < (unsigned int)2E32 -1) m_counterVal += 1;
      else m_counterVal = 0;
      
      //Interrogate daemons
      m_pullRef.in()->is_alive(m_localHostname.c_str(), m_counterVal);
    
      //Notify all observers with faulty daemons
      for (itList=faultyDaemonsList.begin(); itList!=faultyDaemonsList.end();itList++)
          {
              for_each(m_observers.begin(), m_observers.end(), WarnObserverOfHostFailure(*itList));
          }
     
      CDMW_LOG_FUNCTION_RETURN(DAEMON_ACKNOWLEDGEMENT);
      return DAEMON_ACKNOWLEDGEMENT;
  }

  void InterrogatingMode::start()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      m_pPushMonitorableImpl->start();
  }

  void InterrogatingMode::stop()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      m_pPushMonitorableImpl->stop();
  }

  void InterrogatingMode::i_am_alive(MonitoringLibrary::Location hostname,
                                     MonitoringLibrary::RequestType request_type,
                                     unsigned int counterVal)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"hostname="<<hostname<<",request_type="<<request_type
                        << ",counterVal="<<counterVal);
      // DEBUG_ECHO << "InterrogatingMode::i_am_alive" << std::endl;
      //Update liveliness repository with the current daemon acknowledgement
      if (m_pRepository->update_liveliness(hostname,
                                           request_type,
                                           counterVal))
	  {
          CDMW_DEBUG_3(::Cdmw::FT::FTLogger::GetLogger(), " daemon says i_am_alive and is new" );
          //Notify all observers with the new slave
          for (size_t i=0;i<m_observers.size();i++)
              {
                 m_observers[i]->on_start(hostname);	
              }
      }
  }

  void InterrogatingMode::attach_observer(MonitoringLibrary::MonitoringObserver_ptr observer) throw (MonitoringLibrary::AlreadyExists)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      observerVector::iterator itObs; 
      if ( CORBA::is_nil(observer) )
      {
          CDMW_WARN(::Cdmw::FT::FTLogger::GetLogger(), "InterrogatingMode::attach_observer an observer is nil!" );
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

        MonitoringLibrary::MonitoringObserver_var eltObserver;
        eltObserver = MonitoringLibrary::MonitoringObserver::_duplicate(observer);
        m_observers.push_back(eltObserver);
      }
  }

  void InterrogatingMode::detach_observer(MonitoringLibrary::MonitoringObserver_ptr observer) throw (MonitoringLibrary::NotFound)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      observerVector::iterator itObs;
      if ( CORBA::is_nil(observer) )
          {
              CDMW_WARN(::Cdmw::FT::FTLogger::GetLogger(),"InterrogatingMode::detach_observer observer is nil!");
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
    
} // End namespace Supervision
}  // End namespace FT
}  // End namespace Cdmw
