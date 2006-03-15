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

#ifndef INCL_CDMW_FT_SUPERVISION_COMMON_SWITCHSTRATEGY_HPP
#define INCL_CDMW_FT_SUPERVISION_COMMON_SWITCHSTRATEGY_HPP

// include files
#include "ftmonitoring/common/CommonTypeDefinitions.hpp"

#include "idllib/CdmwFTMonitoring.stub.hpp"

#include <string>
#include <list>

namespace Cdmw {
namespace FT {
namespace Supervision {

// class definition
class SwitchStrategy {

public:
    
    // Constructor
    SwitchStrategy() {};
    // Destructor 
    virtual ~SwitchStrategy() throw() {}; 


    /* Add a location to the locations defining the strategy */
    virtual void add_location (const CdmwFT::MonitoringLibrary::Location location)
      throw(CdmwFT::MonitoringLibrary::AlreadyExists) = 0; 
    /* Remove a location from the locations defining the strategy */
    virtual void remove_location (const CdmwFT::MonitoringLibrary::Location location)
      throw(CdmwFT::MonitoringLibrary::NotFound) = 0;
    /* Set locations defining the strategy */
    virtual void set_locations (const CdmwFT::MonitoringLibrary::Locations& locations) = 0;
    /* Reset the locations defining the strategy */
    virtual void reset_locations() = 0;
    /* Get the ordered list of locations */
    virtual const LocationList& get_locations() = 0;
    

protected:

private:
    // user copy constructor
    SwitchStrategy(const SwitchStrategy&);
    // assignement operator
    SwitchStrategy& operator=(const SwitchStrategy&);    

};

}; // end namespace
}  // End namespace FT
}  // End namespace Cdmw
#endif

