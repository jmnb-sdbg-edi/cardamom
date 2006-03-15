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

#ifndef INCL_CDMW_FT_SUPERVISION_COMMON_SIMPLELIVELINESSREPOSITORY_HPP
#define INCL_CDMW_FT_SUPERVISION_COMMON_SIMPLELIVELINESSREPOSITORY_HPP

// private include files
#include "common/LivelinessRepository.hpp"

namespace Cdmw {
namespace FT {
namespace Supervision
{

// class definition
/**
*Purpose:
*<p>    This class implements operations to handle the liveliness repository
*       used to record answers of the monitored hosts for the simple monitoring
*
*Features:
*<p> Thread safe, exception safe
*
*/

class SimpleLivelinessRepository : public LivelinessRepository {

public:

  // Constructor
  SimpleLivelinessRepository();

  // Destructor
  virtual ~SimpleLivelinessRepository();

  // In the simple liveliness these methods are not implemented

  virtual ::CdmwFT::MonitoringLibrary::Locations * get_started_locations ()
    throw (CORBA::SystemException);

  // In simple monitoring the locations operations force the liveliness
  // repository with entries with the given locations
  // It allow to verify that a set of locations are present after system start

  /**
   * Purpose:
   * <p> Add a location to the liveliness repository of simple monitoring
   *
   *@param     location the location to add
   *@exception CdmwFT::MonitoringLibrary::AlreadyExists if location
   *  is already in the liveliness repository
   */
  void add_location(const CdmwFT::MonitoringLibrary::Location location)
    throw (CdmwFT::MonitoringLibrary::AlreadyExists);

  // Not implemented as the monitoring mechanism will add again
  // the location in liveliness repository if host respond
  void remove_location(const CdmwFT::MonitoringLibrary::Location location)
    throw (CORBA::SystemException);

  /**
   * Purpose:
   * <p> Set locations of the liveliness repository of simple monitoring
   *
   *@param     locations the locations to set
   *@exception CdmwFT::MonitoringLibrary::NotEmpty if there is
   *  locations in the liveliness repository
   */
  void set_locations(const CdmwFT::MonitoringLibrary::Locations& locations)
    throw (CdmwFT::MonitoringLibrary::NotEmpty, CORBA::SystemException);

  /**
   * Purpose:
   * <p> Reset the locations defining the strategy
   */
  void reset_locations()
    throw (CORBA::SystemException);

  // No use of these methods in the simple liveliness case

  const CdmwFT::MonitoringLibrary::Location get_masterCandidate()
    throw(Cdmw::InternalErrorException);

protected:

private:

  // default copy constructor
  SimpleLivelinessRepository(const SimpleLivelinessRepository&);
  // assignement operator
  SimpleLivelinessRepository& operator=(const SimpleLivelinessRepository&);    

};

} // End namespace Supervision
}  // End namespace FT
}  // End namespace Cdmw
#endif

