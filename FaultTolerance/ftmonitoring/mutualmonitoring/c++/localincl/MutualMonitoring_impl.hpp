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

#ifndef INCL_CDMWFT_MONITORINGLIBRARY_MUTUALMONITORING_IMPL_HPP
#define INCL_CDMWFT_MONITORINGLIBRARY_MUTUALMONITORING_IMPL_HPP

// private include files
#include "MutualMonitoring.hpp"

// include files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>

#include "idllib/CdmwFTMutualMonitoring.skel.hpp"
#include "ftmonitoring/common/CommonTypeDefinitions.hpp"

// class definition
// 
namespace CdmwFT
{
namespace MonitoringLibrary
{
namespace PrimaryBackup
{
	
/**
*Purpose:
*<p>    This class is responsible of the implementations of
*       interfaces of the mutual monitoring
*/

class MutualMonitoring_impl : 
  public virtual PrimaryBackup::MutualMonitoring ,
  public virtual Cdmw::OrbSupport::RefCountLocalObject {

public:
 
  // Constructor
  MutualMonitoring_impl(CORBA::ORB_ptr orb,
		 	Cdmw::FT::Supervision::ArgvList argv, 
			const Locations & locations);

  // Destructor
  virtual ~MutualMonitoring_impl();

// For add_/remove_/set_/reset_/get_started location see MutualLivelinessRepository
virtual void add_location (
    const char * the_location
  )
  throw(CORBA::SystemException, AlreadyExists);

virtual void remove_location (
    const char * the_location
  )
  throw(CORBA::SystemException, NotFound);

virtual void set_locations (
    const Locations & the_locations
  )
  throw(CORBA::SystemException, NotEmpty);

virtual void reset_locations (
  )
  throw(CORBA::SystemException);

virtual Locations * get_started_locations (
  )
  throw(CORBA::SystemException);

  // Operations to set observer : see MutualMonitoring
virtual void attach_observer (
    PrimaryBackup::MutualMonitoringObserver_ptr the_observer
  )
  throw(CORBA::SystemException, AlreadyExists);

virtual void detach_observer (
    PrimaryBackup::MutualMonitoringObserver_ptr the_observer
  )
  throw(CORBA::SystemException, NotFound);

  // Operations to start and stop monitoring : see MutualMonitoring
virtual void start_monitoring ( )
  throw(CORBA::SystemException, AlreadyStarted);

virtual void stop_monitoring ( )
  throw(CORBA::SystemException, NotStarted);

protected:

private:

  Cdmw::FT::Supervision::MutualMonitoring* m_pMonitoring;
  
};
}
}
}
#endif

