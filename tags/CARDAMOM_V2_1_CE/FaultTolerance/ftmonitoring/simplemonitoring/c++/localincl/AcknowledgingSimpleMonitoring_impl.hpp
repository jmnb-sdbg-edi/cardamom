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

#ifndef INCL_CDMWFT_MONITORINGLIBRARY_SIMPLEMONITORING_ACKNOWLEDGINGSIMPLEMONITORING_HPP
#define INCL_CDMWFT_MONITORINGLIBRARY_SIMPLEMONITORING_ACKNOWLEDGINGSIMPLEMONITORING_HPP

// private include files
#include "simplemonitoring/SimpleMonitoring.hpp"

// include files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>

#include "idllib/CdmwFTSimpleMonitoring.skel.hpp"

#include "ftmonitoring/common/CommonTypeDefinitions.hpp"

// class definition

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
*
*/
class AcknowledgingSimpleMonitoring_impl : 
  public virtual SimpleMonitoring::AcknowledgingSimpleMonitoring ,
  public virtual Cdmw::OrbSupport::RefCountLocalObject
{

public:

  // Constructor
  AcknowledgingSimpleMonitoring_impl(CORBA::ORB_ptr orb, 
		        	     Cdmw::FT::Supervision::ArgvList argv);
  
  // destructor
  virtual ~AcknowledgingSimpleMonitoring_impl();

  /**
   * Purpose:
   * <p> Start the acknowledging simple monitoring
   *
   *@exception AlreadyStarted if acknowledging simple monitoring was started
   */
  virtual void start_monitoring ( )
  throw(CORBA::SystemException, AlreadyStarted);

  /**
   * Purpose: Stop the acknowledging simple monitoring
   * <p> 
   *
   *@exception NotStarted if acknowledging simple monitoring was stopped
   */
  virtual void stop_monitoring ( )
  throw(CORBA::SystemException, NotStarted);

protected:

private:

  Cdmw::FT::Supervision::SimpleMonitoring* m_pMonitoring;
  
};
}
}
}
#endif

