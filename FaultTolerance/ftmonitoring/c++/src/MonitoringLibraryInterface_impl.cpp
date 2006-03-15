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

// include files
#include "mutualmonitoring/MutualMonitoring_impl.hpp"
#include "simplemonitoring/AcknowledgingSimpleMonitoring_impl.hpp"
#include "simplemonitoring/InterrogatingSimpleMonitoring_impl.hpp"
#include "ftmonitoring/MonitoringLibraryInterface_impl.hpp"

namespace CdmwFT
{
namespace MonitoringLibrary
{

MonitoringLibraryInterface_impl* MonitoringLibraryInterface_impl::M_pMonitoringImpl = NULL;

// constructor
MonitoringLibraryInterface_impl::MonitoringLibraryInterface_impl
  ( CORBA::ORB_ptr orb, 
    Cdmw::FT::Supervision::ArgvList argv)
{
	m_orb = CORBA::ORB::_duplicate(orb);

	m_argvList = argv;
		
	m_orb->register_initial_reference("MonitoringLibrary", this);
}

// constructor
MonitoringLibraryInterface_impl::MonitoringLibraryInterface_impl()
{
}

// destructor
MonitoringLibraryInterface_impl::~MonitoringLibraryInterface_impl()
{
}

//uncomment here if you want a user copy constructor
//  MonitoringLibraryInterface_impl::MonitoringLibraryInterface_impl(const MonitoringLibraryInterface_impl& t)
//  {
//    //attribute=t.attribute
//  }

//  MonitoringLibraryInterface_impl& 
//  MonitoringLibraryInterface_impl::operator=(const MonitoringLibraryInterface_impl& t)
//  {
//    if (this != &t) {
//      //clean all attributes (delete[] attribute...)
//      //attribute=t.attribute
//   }
//   return *this;
//  }    

  void MonitoringLibraryInterface_impl::Init(CORBA::ORB_ptr orb, 
		  			     Cdmw::FT::Supervision::ArgvList argv)
  {
	if (M_pMonitoringImpl ==  NULL)
      {
	    M_pMonitoringImpl = new MonitoringLibraryInterface_impl(orb, argv);
      }
  }

  PrimaryBackup::MutualMonitoring_ptr 
  MonitoringLibraryInterface_impl::create_mutual_monitoring (
    const Locations & the_locations
  )
  throw(CORBA::SystemException)
  {
	PrimaryBackup::MutualMonitoring_var mutual = 
	new PrimaryBackup::MutualMonitoring_impl(m_orb.in(), m_argvList, the_locations);
	
	return mutual._retn();
	
  }

  SimpleMonitoring::InterrogatingSimpleMonitoring_ptr 
  MonitoringLibraryInterface_impl::create_interrogating_simple_monitoring (
    const Locations & the_locations
  )
  throw(CORBA::SystemException)
  {
	SimpleMonitoring::InterrogatingSimpleMonitoring_var interro =
	new SimpleMonitoring::InterrogatingSimpleMonitoring_impl(m_orb.in(), m_argvList, the_locations);
	
	return interro._retn();
  }

  SimpleMonitoring::AcknowledgingSimpleMonitoring_ptr 
  MonitoringLibraryInterface_impl::create_acknowledging_simple_monitoring (
    
  )
  throw(CORBA::SystemException)
  {
	SimpleMonitoring::AcknowledgingSimpleMonitoring_var acknow =
	new SimpleMonitoring::AcknowledgingSimpleMonitoring_impl(m_orb.in(), m_argvList);
	
	return acknow._retn();
											   
  }

}
}
