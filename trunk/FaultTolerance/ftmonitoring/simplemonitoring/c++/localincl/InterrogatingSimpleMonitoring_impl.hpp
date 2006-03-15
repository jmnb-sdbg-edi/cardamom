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

#ifndef INCL_CDMWFT_MONITORINGLIBRARY_SIMPLEMONITORING_INTERROGATINGSIMPLEMONITORING_HPP
#define INCL_CDMWFT_MONITORINGLIBRARY_SIMPLEMONITORING_INTERROGATINGSIMPLEMONITORING_HPP

// private include files
#include "simplemonitoring/SimpleMonitoring.hpp"

// include files
#include "idllib/CdmwFTSimpleMonitoring.skel.hpp"

#include "ftmonitoring/common/CommonTypeDefinitions.hpp"

#include <Foundation/orbsupport/RefCountLocalObject.hpp>

namespace CdmwFT
{
namespace MonitoringLibrary
{
namespace SimpleMonitoring
{

/**
*Purpose:
*<p>    This class is responsible of the acknowledging mode
*       of the simple monitoring
*/
  class  InterrogatingSimpleMonitoring_impl :
    public virtual SimpleMonitoring::InterrogatingSimpleMonitoring,
    public virtual Cdmw::OrbSupport::RefCountLocalObject
{
public:

      InterrogatingSimpleMonitoring_impl (
          CORBA::ORB_ptr orb,
          Cdmw::FT::Supervision::ArgvList argv,
	  const Locations & locations);

  // Operations on locations with simple monitoring :
  // They will directly modify the liveliness repository
  // add and set allow to have notifications of absence of necessary Cdmw Daemons
  // the use of remove and reset is delicate and very limited :
  // A remove of a location does not mean this one won't be monitored because
  // at the next answer the entry will be added again in the repository.
  // So we could choose to throw NO_IMPLEMENT instead, to warn of misuse..

  // For add_/remove_/set_/reset_location see SimpleMonitoring->SimpleLivelinessRepository
      virtual void add_location (
          const char * the_location
	  ) throw (CORBA::SystemException, AlreadyExists);
      
      virtual void remove_location (
          const char * the_location
        ) throw (CORBA::SystemException, NotFound);
      
      virtual void set_locations (
          const Locations & the_locations
        ) throw (CORBA::SystemException);
      
      virtual void reset_locations( )
          throw (CORBA::SystemException);
      
  // Operations to set observer : see SimpleMonitoring->InterrogatingMode
      virtual void attach_observer (
          MonitoringObserver_ptr the_observer
        ) throw (CORBA::SystemException, AlreadyExists);
      
      virtual void detach_observer (
          MonitoringObserver_ptr the_observer
        ) throw (CORBA::SystemException, NotFound);

  // Operation to control monitoring launch : see InterrogatingMode
      void start_monitoring ()
	  throw(CORBA::SystemException, AlreadyStarted);

      void stop_monitoring ()
	  throw(CORBA::SystemException, NotStarted);

protected:
  //Destructor 
  virtual ~InterrogatingSimpleMonitoring_impl (void);

private:

  //Constructor 
  InterrogatingSimpleMonitoring_impl (void);

  Cdmw::FT::Supervision::SimpleMonitoring* m_pMonitoring;

}; // end class

}; // end namespace Simple Monitoring

}; // end namespace MonitoringLibrary

}; // end namespace CdmwFT

#endif // INCL_CDMWFT_MONITORINGLIBRARY_SIMPLEMONITORING_INTERROGATINGSIMPLEMONITORING_HPP
