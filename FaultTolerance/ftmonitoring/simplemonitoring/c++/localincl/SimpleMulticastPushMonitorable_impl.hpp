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

#ifndef INCL_CDMWFT_MONITORINGLIBRARY_SIMPLEMONITORING_SIMPLEMULTICASTPUSHMONITORABLE_HPP
#define INCL_CDMWFT_MONITORINGLIBRARY_SIMPLEMONITORING_SIMPLEMULTICASTPUSHMONITORABLE_HPP

// private include files
#include "common/LivelinessRepository.hpp"

// include files
#include <string>
#include <Foundation/orbsupport/RefCountServantBase.hpp>

#if defined(EXT_RELIABLE_MULTICAST) || defined(EXT_UNRELIABLE_MULTICAST) 
#include "MulticastPushMonitorableBehavior.skel.hpp"
#else
#include "common/CdmwFTMulticastMonitorable.skel.hpp"
#endif

// class definition
/**
*Purpose:
*<p>    This class implements operations for the push monitorable object
*       of the simple monitoring.
* It receive (daemon) acknowledgments via i_am_alive and update livelinesses
*
*/

namespace Cdmw {
namespace FT {
namespace Supervision {

class InterrogatingMode;

};
}  // End namespace FT
}  // End namespace Cdmw

namespace CdmwFT {
namespace MonitoringLibrary {

#if defined(EXT_RELIABLE_MULTICAST) || defined(EXT_UNRELIABLE_MULTICAST) 
class SimpleMulticastPushMonitorable_impl :  public virtual MulticastPushMonitorableBehavior
#else
class SimpleMulticastPushMonitorable_impl : 
    public virtual POA_CdmwFT::MonitoringLibrary::MulticastPushMonitorable,
    public virtual Cdmw::OrbSupport::RefCountServantBase
#endif
{

public:

  // Constructor
  SimpleMulticastPushMonitorable_impl(CORBA::ORB_ptr orb, 
				      std::string pushCorbaloc,
				      Cdmw::FT::Supervision::InterrogatingMode* interrogatingMode);
/* Used when no on_start needed
  SimpleMulticastPushMonitorable_impl(CORBA::ORB_ptr orb, 
				      std::string pushCorbaloc,
				      Cdmw::FT::Supervision::LivelinessRepository* repository);
*/
  // Destructor
  virtual ~SimpleMulticastPushMonitorable_impl();

  /**
   * Purpose: receives the responses of the daemon
   * <p> 
   *
   *@param hostname name of the daemon
   *@param counterVal is used for internal identification of messages
   */
  virtual void i_am_alive (const char * hostname,
                           RequestType request_type,
			   CORBA::ULong counterVal) throw (CORBA::SystemException);

  // Start the push monitorable (activate object)
  void start();

  // Stop the push monitorable (deactivate object)
  void stop();
  
protected:

private:

#if !defined(EXT_RELIABLE_MULTICAST) && !defined(EXT_UNRELIABLE_MULTICAST)
  PortableServer::POA_var m_poa;
  PortableServer::ObjectId_var m_pushObjectId;
#endif
  // Cdmw::FT::Supervision::LivelinessRepository* m_pRepository;
  Cdmw::FT::Supervision::InterrogatingMode* m_pInterrogatingMode;
#if defined(EXT_RELIABLE_MULTICAST) || defined(EXT_UNRELIABLE_MULTICAST)
  MulticastPushMonitorable_var m_mPushMntrObject;
#endif
};

};

};
#endif

