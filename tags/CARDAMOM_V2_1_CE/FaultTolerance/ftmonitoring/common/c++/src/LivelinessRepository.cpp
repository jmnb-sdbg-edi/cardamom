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
#include "common/LivelinessRepository.hpp"

// include files
#include "Foundation/osthreads/ReaderLockGuard.hpp"
#include "Foundation/osthreads/WriterLockGuard.hpp"


using namespace CdmwFT;

namespace Cdmw {
namespace FT {
namespace Supervision
{
	
  // constructor
  LivelinessRepository::LivelinessRepository()
  {
  }

  // destructor
  LivelinessRepository::~LivelinessRepository()
  {
  }

  // Retrieve (in the repository) the list of faulty slaves since last master interrogation
  // Suppress them from the liveliness map and reset the other entry responses in the map
  // counter is used to set to UNKNOWN only the entries from last previous
  // interrogation (for mutual monitoring to avoid double failure notif)
  void LivelinessRepository::get_faultySlaves(LocationList& faultySlaves,
		  			     unsigned int counterVal)
  {
    CDMW_WRITER_LOCK_GUARD(m_livelinessMutex);
    
    faultySlaves.clear();

    LivelinessMap::iterator itLv;
    for (itLv = m_livelinessMap.begin(); itLv != m_livelinessMap.end(); itLv++)
      {
        // cout << itLv->first << ":" << itLv->second << std::endl;
        if ((itLv->second.responseType) == UNKNOWN)
        {
          // add to the list of faulty slaves
          faultySlaves.push_back(itLv->first);
        }
        else if (itLv->second.counterVal != counterVal)
	{
          // reset the other entry responses in the map
          itLv->second.responseType = UNKNOWN;
        }
      }

    // suppress the faulty slaves from the map
    LocationList::iterator itSl;
    for (itSl = faultySlaves.begin(); itSl != faultySlaves.end(); itSl++)
      {
        m_livelinessMap.erase(*itSl);
      }
        
  }	    

  // Update the liveliness map with the response of the location
  // return true if the location is new in the map
  bool LivelinessRepository::update_liveliness(const char * location,
					      MonitoringLibrary::RequestType requestType,
					      unsigned int counterVal)
  {
    CDMW_WRITER_LOCK_GUARD(m_livelinessMutex);

    Response response = {UNKNOWN, counterVal};
    bool newLocation = false;

    // std::string location_str = location;
    if (requestType == MonitoringLibrary::ACKNOWLEDGEMENT)
    {
      response.responseType = ACKNOWLEDGEMENT;
    }
    else if (requestType == MonitoringLibrary::CANDIDATURE)
    {
      response.responseType = CANDIDATURE;
    }
    LivelinessMap::iterator itLv = m_livelinessMap.find(location);
    if (itLv != m_livelinessMap.end())
    {
      itLv->second = response;
    }
    else    
    {
      m_livelinessMap[location]= response;
      newLocation = true;
    }
    return newLocation;
  }

  // Set response to UNKNOWN for the given location
  void LivelinessRepository::reset_liveliness(const MonitoringLibrary::Location location)
    throw (MonitoringLibrary::NotFound, CORBA::SystemException)
  {
    CDMW_WRITER_LOCK_GUARD(m_livelinessMutex);   
    
    LivelinessMap::iterator itLv = m_livelinessMap.find(location);
    if (itLv != m_livelinessMap.end())
    {
      itLv->second.responseType = UNKNOWN;
      itLv->second.counterVal = 0;
    }
    else    
    {
      throw MonitoringLibrary::NotFound();
    }
    
  }	  

  // Erase the given location from the liveliness map
  void LivelinessRepository::remove_liveliness(const MonitoringLibrary::Location location) throw (CORBA::SystemException)
  {
    CDMW_WRITER_LOCK_GUARD(m_livelinessMutex);   

    m_livelinessMap.erase(location);
  }	  

  // Set response to UNKNOWN for all the locations in the map (i.e. all started locations)
  void LivelinessRepository::reset_livelinesses() throw (CORBA::SystemException)
  {
    CDMW_WRITER_LOCK_GUARD(m_livelinessMutex);

    LivelinessMap::iterator itLv;
    for (itLv = m_livelinessMap.begin(); itLv != m_livelinessMap.end(); itLv++)
    {
      itLv->second.responseType = UNKNOWN;
      itLv->second.counterVal = 0;
    }
  }	  

  // Clear the liveliness map
  void LivelinessRepository::remove_livelinesses() throw (CORBA::SystemException)
  {
    CDMW_WRITER_LOCK_GUARD(m_livelinessMutex);   

    m_livelinessMap.clear();
  }	  

  // Get the list of all the locations in the map (Even the one with response=UNKNOWN
  // because they have started and they are not yet declared FAILED
  MonitoringLibrary::Locations * LivelinessRepository::get_started_locations ()
     throw (CORBA::SystemException)
  {
    CDMW_READER_LOCK_GUARD(m_livelinessMutex);
    try
    {
      MonitoringLibrary::Locations_var locationSeq = new MonitoringLibrary::Locations;
      locationSeq->length(m_livelinessMap.size());
      // fill location sequence with all the locations in the liveliness map
      // (started_locations : not the locations in the switch strategy list)
      LivelinessMap::iterator itLv;
      // initialize sequence index
      unsigned int inx = 0;
      for (itLv = m_livelinessMap.begin(); itLv != m_livelinessMap.end(); itLv++)
      {
	locationSeq[inx]=CORBA::string_dup((itLv->first).c_str());
        inx++;
      }
      return locationSeq._retn();
    }
    catch (std::bad_alloc&)
    {
      CDMW_THROW (Cdmw::OutOfMemoryException);
    }
  }

} // End namespace Supervision
}  // End namespace FT
}  // End namespace Cdmw
