/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your 
 * option) any later version. 
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public 
 * License for more details. 
 * 
 * You should have received a copy of the GNU Library General 
 * Public License along with CARDAMOM; see the file COPYING. If not, write to 
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


#include "platformdaemon/Callback_impl.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "platformdaemon/DaemonLogMessageId.hpp"

namespace Cdmw
{
namespace PlatformMngt
{


// //
// // IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationCallback:1.0
// //
// 
// //
// // IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationCallback/set_ready:1.0
// //
// CdmwPlatformMngt::ApplicationAck* ApplicationCallback_impl::set_ready(CdmwPlatformMngt::Application_ptr the_application,
//      CdmwPlatformMngt::ApplicationControl_ptr the_application_control)
//     throw(CORBA::SystemException)
// {
//
// }


//
// IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgentCallback:1.0
//

ApplicationAgentCallback_impl::ApplicationAgentCallback_impl(
            PortableServer::POA_ptr poa,
            const CdmwPlatformMngt::ApplicationAgentAck& acknowledgementData)
      : DeactivableServant_impl (poa)
{
    m_acknowledgementData = new CdmwPlatformMngt::ApplicationAgentAck(acknowledgementData);
}


ApplicationAgentCallback_impl::~ApplicationAgentCallback_impl()
  throw()
{

}


CdmwPlatformMngt::ApplicationAgent_ptr ApplicationAgentCallback_impl::getApplicationAgent()
{
    return CdmwPlatformMngt::ApplicationAgent::_duplicate(m_applicationAgent.in());
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgentCallback/set_ready:1.0
//
CdmwPlatformMngt::ApplicationAgentAck* ApplicationAgentCallback_impl::set_ready(CdmwPlatformMngt::ApplicationAgent_ptr the_agent)
    throw(CORBA::SystemException)
{
    // updates the application agent reference
    m_applicationAgent = CdmwPlatformMngt::ApplicationAgent::_duplicate(the_agent);

    // signals the acknowledgement
    if (!signal())
    {
        // The acknowledgement has been performed too late
        throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);
    }

    return m_acknowledgementData._retn();

}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgentCallback/notify_error:1.0
//
void ApplicationAgentCallback_impl::notify_error(const char* error_info)
    throw(CORBA::SystemException)
{

    try
    {
        m_errorInfo = error_info; 
    }
    catch(const std::bad_alloc&)
    {
        LogMngr::logMessage(ERR, MSG_ID_DAEMON_NO_MEMORY);
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }

    // signals the error
    if (!signal(true))
    {
        // The acknowledgement has been performed too late
        throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);
    }

}

// //
// // IDL:thalesgroup.com/CdmwPlatformMngt/HostProbeCallback:1.0
// //
// 
// //
// // IDL:thalesgroup.com/CdmwPlatformMngt/HostProbeCallback/set_ready:1.0
// //
// void HostProbeCallback_impl::set_ready(CdmwPlatformMngt::HostProbe_ptr the_probe)
//     throw(CORBA::SystemException)
// {
// 
// }



} // End namespace PlatformMngt
} // End namespace Cdmw


