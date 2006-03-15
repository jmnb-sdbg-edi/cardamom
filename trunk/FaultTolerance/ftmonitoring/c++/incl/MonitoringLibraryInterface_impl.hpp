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

#ifndef INCL_CDMWFT_MONITORINGLIBRARY_MONITORINGLIBRARYINTERFACE_IMPL_HPP
#define INCL_CDMWFT_MONITORINGLIBRARY_MONITORINGLIBRARYINTERFACE_IMPL_HPP

// include files
#include "idllib/CdmwFTMonitoringLibrary.skel.hpp"

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>

#include "ftmonitoring/common/CommonTypeDefinitions.hpp"

// class definition
/**
*Purpose:
*<p>    This class is the entry point to the ftmonitoring
*
*/

namespace CdmwFT
{
namespace MonitoringLibrary
{
	
class MonitoringLibraryInterface_impl : 
  public virtual MonitoringLibraryInterface,
  public virtual Cdmw::OrbSupport::RefCountLocalObject {

public:
  
  virtual ~MonitoringLibraryInterface_impl();
  
  /**
   * Purpose:
   * <p> Init the library : singleton creation, register MonitoringLibrary
   *
   *@param orb  the orb
   *@param argv the arguments used for simple & mutual monitoring
   *            (duration of each periods + multicast corbalocs)
   */
static void Init(CORBA::ORB_ptr orb, 
	         Cdmw::FT::Supervision::ArgvList argv);
	  
  /**
   * Purpose:
   * <p> create a mutual monitoring object
   *
   *@param the_locations ordered (for election priority) list of hosts
   *       participating in the mutual monitoring
   */
virtual PrimaryBackup::MutualMonitoring_ptr create_mutual_monitoring (
    const Locations & the_locations
  )
  throw(CORBA::SystemException);

  /**
   * Purpose:
   * <p> create an interrogating simple monitoring object
   *
   *@param the_locations list of host daemons that must be
   *       seen at start by the simple monitoring
   */
virtual SimpleMonitoring::InterrogatingSimpleMonitoring_ptr create_interrogating_simple_monitoring (
    const Locations & the_locations
  )
  throw(CORBA::SystemException);

  /**
   * Purpose:
   * <p> create an acknowledging simple monitoring object
   *
   */
virtual SimpleMonitoring::AcknowledgingSimpleMonitoring_ptr create_acknowledging_simple_monitoring (
    
  )
  throw(CORBA::SystemException);

protected:

  MonitoringLibraryInterface_impl(CORBA::ORB_ptr orb, 
		  			     Cdmw::FT::Supervision::ArgvList argv);

private:

  MonitoringLibraryInterface_impl();
  
  static MonitoringLibraryInterface_impl* M_pMonitoringImpl;
  CORBA::ORB_var m_orb;
  Cdmw::FT::Supervision::ArgvList m_argvList;
};
}
}
#endif

