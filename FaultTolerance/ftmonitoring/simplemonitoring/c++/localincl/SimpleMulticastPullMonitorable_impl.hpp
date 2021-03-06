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

#ifndef INCL_CDMWFT_MONITORINGLIBRARY_SIMPLEMONITORING_SIMPLEMULTICASTPULLMONITORABLE_HPP
#define INCL_CDMWFT_MONITORINGLIBRARY_SIMPLEMONITORING_SIMPLEMULTICASTPULLMONITORABLE_HPP

// include files
#include <string>
#include <Foundation/orbsupport/RefCountServantBase.hpp>

#if defined(EXT_RELIABLE_MULTICAST) || defined(EXT_UNRELIABLE_MULTICAST) 
#include "MulticastPullMonitorableBehavior.skel.hpp"
#else
#include "common/CdmwFTMulticastMonitorable.skel.hpp"
#endif

// class definition
/**
*Purpose:
*<p>    This class implements operations for the pull monitorable object
*       of the simple monitoring. It is interrogated via is_alive and answer
*
*/
namespace CdmwFT {
namespace MonitoringLibrary {

#if defined(EXT_RELIABLE_MULTICAST) || defined(EXT_UNRELIABLE_MULTICAST) 
class SimpleMulticastPullMonitorable_impl :  public virtual MulticastPullMonitorableBehavior
#else
class SimpleMulticastPullMonitorable_impl : 
    public virtual POA_CdmwFT::MonitoringLibrary::MulticastPullMonitorable,
    public virtual Cdmw::OrbSupport::RefCountServantBase
#endif
{

public:

  // Constructor
  SimpleMulticastPullMonitorable_impl(CORBA::ORB_ptr orb, 
				      std::string pullCorbaloc,
				      std::string pushCorbaloc);
 
  // Destructor
  virtual ~SimpleMulticastPullMonitorable_impl();

  /**
   * Purpose: ask the daemon if it is alive
   * <p> 
   *
   *@param hostname name of the interrogating host
   *@param counterVal is used for internal identification of messages
   */
  virtual void is_alive (const char * hostname,
			 CORBA::ULong counterVal) throw(CORBA::SystemException);
 
  // Start the pull monitorable (activate object)
  void start();

  // Stop the pull monitorable (deactivate object)
  void stop();

protected:

private:

#if !defined(EXT_RELIABLE_MULTICAST) && !defined(EXT_UNRELIABLE_MULTICAST)
  PortableServer::POA_var m_poa;
  PortableServer::ObjectId_var m_pullObjectId;
#endif
  MulticastPushMonitorable_var m_pushRef;
  std::string m_localHostname;
#if defined(EXT_RELIABLE_MULTICAST) || defined(EXT_UNRELIABLE_MULTICAST)
  MulticastPullMonitorable_var m_mPullMntrObject;
#endif
};

};

};

#endif

