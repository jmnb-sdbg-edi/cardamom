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
#include "common/SimpleLivelinessRepository.hpp"

#ifdef EXT_RELIABLE_MULTICAST
#include "common/ReliableMulticastParameters.hpp"
#endif

// include files
#include "Foundation/osthreads/WriterLockGuard.hpp"
#include "ftmonitoring/common/MacroDefs.hpp"
using namespace CdmwFT;

namespace Cdmw {
namespace FT {
namespace Supervision
{
	
  // constructor
  SimpleLivelinessRepository::SimpleLivelinessRepository()
  {
      CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
  }

  // destructor
  SimpleLivelinessRepository::~SimpleLivelinessRepository()
  {
      CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
  }

  ::MonitoringLibrary::Locations * SimpleLivelinessRepository::get_started_locations ()
     throw (CORBA::SystemException)
  {
      CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
      // Implementation done in LivelinessRepository conflict with the current
      // simple monitoring set_locations (liveliness repository forced "as started")
      CORBA::NO_IMPLEMENT ex;
      CDMW_LOG_FUNCTION_EXCEPTION(ex);
      throw ex;
  }

  void SimpleLivelinessRepository::add_location(const MonitoringLibrary::Location location)
    throw (MonitoringLibrary::AlreadyExists)
  {
      CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"location="<<location);
      CDMW_WRITER_LOCK_GUARD(m_livelinessMutex);
#ifdef EXT_RELIABLE_MULTICAST
      if (m_livelinessMap.size() < NUMBER_OF_SENDERS)
          {
#endif
              LivelinessMap::iterator itLv = m_livelinessMap.find(location);
              Response response = {ACKNOWLEDGEMENT, 0};
              if (itLv != m_livelinessMap.end())
                  {
                      MonitoringLibrary::AlreadyExists ex;
                      CDMW_LOG_FUNCTION_EXCEPTION(ex);
                      throw ex;
                  }
              else    
                  {
                      m_livelinessMap[location]= response;
                  }
#ifdef EXT_RELIABLE_MULTICAST
          }
      else
          { // TODO : raise exception instead
              CDMW_WARN(FTLogger::GetLogger(), EXT_REL_MCAST_LIB_NAME << "limit exceeded: add_location ignored\n"
                        << "recompile with new ReliableMulticastParameters.hpp" );
          }
#endif

  }

  void SimpleLivelinessRepository::remove_location(const MonitoringLibrary::Location location)
    throw (CORBA::SystemException)
  {
      CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"location="<<location);
      remove_liveliness(location);
  }

  void SimpleLivelinessRepository::set_locations(const MonitoringLibrary::Locations& locations)
    throw (CdmwFT::MonitoringLibrary::NotEmpty, CORBA::SystemException)
  {
      CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CDMW_WRITER_LOCK_GUARD(m_livelinessMutex);
    if (m_livelinessMap.size() != 0)
    {
        MonitoringLibrary::NotEmpty ex;
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
#ifdef EXT_RELIABLE_MULTICAST
    // TODO : add exception
    if (m_livelinessMap.size() <= NUMBER_OF_SENDERS)
    {
#endif
      Response response = {ACKNOWLEDGEMENT, 0};
      for (CORBA::ULong i=0 ; i < locations.length() ; i++)
      {
	// store input location name
	m_livelinessMap[locations[i].in()]= response;      
      }
#ifdef EXT_RELIABLE_MULTICAST
    }
    else
    { // TODO : raise exception instead
        CDMW_WARN(FTLogger::GetLogger(), EXT_REL_MCAST_LIB_NAME << "limit exceeded: set_locations ignored\n"
                  << "recompile with new ReliableMulticastParameters.hpp");
    }
#endif    
  }

  void SimpleLivelinessRepository::reset_locations()
    throw (CORBA::SystemException)
  {
      CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
      CDMW_WRITER_LOCK_GUARD(m_livelinessMutex);
      m_livelinessMap.clear();
  }

  const MonitoringLibrary::Location SimpleLivelinessRepository::get_masterCandidate() throw(Cdmw::InternalErrorException)
  {
      CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
      Cdmw::InternalErrorException ex;
      CDMW_LOG_FUNCTION_EXCEPTION(ex);
      throw ex;
  }

} // End namespace Supervision
}  // End namespace FT
}  // End namespace Cdmw
