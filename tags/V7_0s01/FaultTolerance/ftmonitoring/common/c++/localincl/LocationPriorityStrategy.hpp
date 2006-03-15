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

#ifndef INCL_CDMW_FT_SUPERVISION_COMMON_LOCATIONPRIORITYSTRATEGY_HPP
#define INCL_CDMW_FT_SUPERVISION_COMMON_LOCATIONPRIORITYSTRATEGY_HPP

// include files
#include "ftmonitoring/common/CommonTypeDefinitions.hpp"

#include "Foundation/osthreads/ReaderWriterLock.hpp"

#include "SwitchStrategy.hpp"

namespace Cdmw {
namespace FT {
namespace Supervision {

// class definition
/**
*Purpose:
*<p> This class defines the LocationPriority Strategy
* The built list will be the priority order
*Features:
*<p> Thread safe, exception safe
*
*/
class LocationPriorityStrategy : public virtual SwitchStrategy {

public:
    
    // Constructor
    // LocationPriorityStrategy();

    /**
    * Purpose:
    * <p> Constructor with the given locations
    *
    *@param CdmwFT::MonitoringLibrary::Locations set the initial priority list
    */
    LocationPriorityStrategy(const CdmwFT::MonitoringLibrary::Locations& locations);

    // Destructor 
    virtual ~LocationPriorityStrategy() throw(); 

    /* Add a location to the locations defining the strategy */
    void add_location (const CdmwFT::MonitoringLibrary::Location location)
      throw(CdmwFT::MonitoringLibrary::AlreadyExists); 
    /* Remove a location from the locations defining the strategy */
    void remove_location (const CdmwFT::MonitoringLibrary::Location location)
      throw(CdmwFT::MonitoringLibrary::NotFound);
    /* Set locations defining the strategy */
    void set_locations (const CdmwFT::MonitoringLibrary::Locations& locations)
      throw(CdmwFT::MonitoringLibrary::NotEmpty);
    /* Reset the locations defining the strategy */
    void reset_locations();
    /* Get the ordered list of locations */
    const LocationList& get_locations();

protected:

private:

    // user copy constructor
    LocationPriorityStrategy(const LocationPriorityStrategy&);
    // assignement operator
    LocationPriorityStrategy& operator=(const LocationPriorityStrategy&);    

    // Check if element is contained in runset
    bool contains(const CdmwFT::MonitoringLibrary::Location location);

    // The mutex used to maintain strategy list coherency
    Cdmw::OsSupport::ReaderWriterLock m_strategyMutex;

    // The strategy (list of locations ordered by priority)
    LocationList m_strategy;

}; // end class

}; // end namespace
}  // End namespace FT
}  // End namespace Cdmw
#endif

