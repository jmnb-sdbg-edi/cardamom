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
#include "common/MutualLivelinessRepository.hpp"
#include "common/LocationPriorityStrategy.hpp"

// include files
#include "Foundation/osthreads/WriterLockGuard.hpp"
#include "Foundation/osthreads/ReaderLockGuard.hpp"
#include "Foundation/common/Exception.hpp"

using namespace CdmwFT;

namespace Cdmw {
namespace FT {
namespace Supervision
{
	
  // constructor
  MutualLivelinessRepository::MutualLivelinessRepository(ElectionStrategyType strategy,
                                                         const MonitoringLibrary::Locations& locations)
  {
    // REM: at this time host priority is the only available strategy
    // CPU load strategy is not implemented yet
    
    switch(strategy)
    {
      case HOST_PRIORITY:
        m_pSwitchStrategy = new LocationPriorityStrategy(locations);
	break;

      case CPU_LOAD:
        throw CORBA::NO_IMPLEMENT();
    }
  }

  // destructor
  MutualLivelinessRepository::~MutualLivelinessRepository()
  {
    // Delete the internal data
    if (m_pSwitchStrategy != 0)
    {
        delete m_pSwitchStrategy;
        m_pSwitchStrategy = 0;
    }   
  }


  void MutualLivelinessRepository::add_location(const MonitoringLibrary::Location location)
    throw(MonitoringLibrary::AlreadyExists)
  {
    m_pSwitchStrategy->add_location(location);
  }

  void MutualLivelinessRepository::remove_location(const MonitoringLibrary::Location location)
    throw(MonitoringLibrary::NotFound)
  {
    m_pSwitchStrategy->remove_location(location);
  }

  void MutualLivelinessRepository::set_locations(const MonitoringLibrary::Locations& locations)
    throw (MonitoringLibrary::NotEmpty, CORBA::SystemException)
  {
    m_pSwitchStrategy->set_locations(locations);
  }

  void MutualLivelinessRepository::reset_locations()
     throw (CORBA::SystemException)
  {
    m_pSwitchStrategy->reset_locations();
  }

  const MonitoringLibrary::Location MutualLivelinessRepository::get_masterCandidate()
    throw(Cdmw::InternalErrorException)
  {
    CDMW_READER_LOCK_GUARD(m_livelinessMutex);

    LocationList locations = m_pSwitchStrategy->get_locations();

    LocationList::iterator itLst;
    LivelinessMap::iterator itLv;

    for (itLst = locations.begin(); itLst != locations.end(); itLst++)
    {
      itLv = m_livelinessMap.find(*itLst);
      if (itLv != m_livelinessMap.end())
      {
        if (itLv->second.responseType == CANDIDATURE) return strdup((*itLst).c_str());
      }
    }

    CDMW_THROW(Cdmw::InternalErrorException);
  }	  

} // End namespace Supervision
}  // End namespace FT
}  // End namespace Cdmw
