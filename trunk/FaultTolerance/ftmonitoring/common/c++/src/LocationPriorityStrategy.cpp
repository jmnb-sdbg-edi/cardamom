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
#include "ftmonitoring/common/MacroDefs.hpp"
#include "common/LocationPriorityStrategy.hpp"

#ifdef EXT_RELIABLE_MULTICAST
#include "common/ReliableMulticastParameters.hpp"
#endif

// include files
#include "Foundation/osthreads/ReaderLockGuard.hpp"
#include "Foundation/osthreads/WriterLockGuard.hpp"

using namespace CdmwFT;

namespace Cdmw {
namespace FT {
namespace Supervision
{

// constructor with the given locations
LocationPriorityStrategy::LocationPriorityStrategy(const MonitoringLibrary::Locations& locations)
{
    for (CORBA::ULong i=0 ; i < locations.length() ; i++)
    {
        // store location name in strategy
        m_strategy.push_back((locations[i]).in());
    }
}

// destructor
LocationPriorityStrategy::~LocationPriorityStrategy()
  throw()
{
    m_strategy.clear();
}

// Add a location to the locations
void LocationPriorityStrategy::add_location (const MonitoringLibrary::Location location)
   throw(MonitoringLibrary::AlreadyExists)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"location="<<location);
    CDMW_WRITER_LOCK_GUARD(m_strategyMutex);
#ifdef EXT_RELIABLE_MULTICAST
    if (m_strategy.size() < NUMBER_OF_SENDERS)
    {
#endif
    if (this->contains(location))
    {
        MonitoringLibrary::AlreadyExists ex;
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
    else
    {
      m_strategy.push_back(location);
    }
#ifdef EXT_RELIABLE_MULTICAST
    }
    else
    { // TODO : raise exception instead
        CDMW_WARN(FTLogger::GetLogger(), EXT_REL_MCAST_LIB_NAME << "limit exceeded: add_location ignored\n"
                  << "recompile with new ReliableMulticastParameters.hpp");
    }
#endif
}

// Remove a location from the locations
void LocationPriorityStrategy::remove_location (const MonitoringLibrary::Location location)
   throw(MonitoringLibrary::NotFound)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"location="<<location);
    CDMW_WRITER_LOCK_GUARD(m_strategyMutex);
    if (this->contains(location))
    {
      m_strategy.remove(location);
    }
    else
    {
        MonitoringLibrary::NotFound ex;
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
}

// Get the ordered list of locations
const LocationList& LocationPriorityStrategy::get_locations()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CDMW_READER_LOCK_GUARD(m_strategyMutex);
    return m_strategy;
}

void LocationPriorityStrategy::set_locations (const MonitoringLibrary::Locations& locations)
   throw(MonitoringLibrary::NotEmpty)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CDMW_WRITER_LOCK_GUARD(m_strategyMutex);
    if (m_strategy.size() != 0)
    {
        MonitoringLibrary::NotEmpty ex;
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }

#ifdef EXT_RELIABLE_MULTICAST
    // TODO : add exception
    if (locations.length() <= NUMBER_OF_SENDERS)
    {
#endif
    for (CORBA::ULong i=0 ; i < locations.length() ; i++)
    {
      // store input location name
      m_strategy.push_back((locations[i]).in());
    }
#ifdef EXT_RELIABLE_MULTICAST
    }
    else
    { // TODO : raise exception instead
        CDMW_WARN(FTLogger::GetLogger(), EXT_REL_MCAST_LIB_NAME << "limit exceeded: set_locations ignored\n" 
                  << "recompile with new ReliableMulticastParameters.hpp" );
    }
#endif
}

void LocationPriorityStrategy::reset_locations()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CDMW_WRITER_LOCK_GUARD(m_strategyMutex);
    // erase all locations
    m_strategy.clear();
}

bool LocationPriorityStrategy::contains(const MonitoringLibrary::Location location)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"location="<<location);
    bool result = false;

    LocationList::iterator it =
      find(m_strategy.begin(), m_strategy.end(), location);

    if (it != m_strategy.end())
      {
          result = true;
      }

    CDMW_LOG_FUNCTION_RETURN(result);
    return result;
}

}; // end namespace
}  // End namespace FT
}  // End namespace Cdmw
