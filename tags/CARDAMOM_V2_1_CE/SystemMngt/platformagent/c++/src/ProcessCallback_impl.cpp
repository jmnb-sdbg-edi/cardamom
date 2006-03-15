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


#include "platformagent/ProcessCallback_impl.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "platformdaemonlibrary/DaemonLogMessageId.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


ProcessCallback_impl::ProcessCallback_impl(
    PortableServer::POA_ptr poa,
    const CdmwPlatformMngt::ProcessCallback::ProcessStartingData& startingData)
    : DeactivableServant_impl (poa)
{
    m_startingData = new CdmwPlatformMngt::ProcessCallback::ProcessStartingData(startingData);
}


ProcessCallback_impl::~ProcessCallback_impl()
  throw()
{

}


CdmwPlatformMngt::ProcessDelegate_ptr ProcessCallback_impl::getProcess()
{
    return CdmwPlatformMngt::ProcessDelegate::_duplicate(m_process.in());
}


const char* ProcessCallback_impl::getErrorIssuer()
{
    return m_errorIssuer.c_str();
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessCallback/set_ready:1.0
//
CdmwPlatformMngt::ProcessCallback::ProcessStartingData* ProcessCallback_impl::set_ready(
        CdmwPlatformMngt::ProcessDelegate_ptr the_process)
    throw(CORBA::SystemException)
{
    // updates the application agent reference
    m_process = CdmwPlatformMngt::ProcessDelegate::_duplicate(the_process);

    // signals the acknowledgement
    if (!signal())
    {
        // The acknowledgement has been performed too late
        throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);
    }

    CdmwPlatformMngt::ProcessCallback::ProcessStartingData_var result(m_startingData);

    return result._retn();
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessCallback/get_starting_info:1.0
//
CdmwPlatformMngt::ProcessCallback::ProcessStartingData * ProcessCallback_impl::get_starting_info (
             CdmwPlatformMngt::ProcessDelegate_ptr the_process)
      throw(CORBA::SystemException)
{
    // updates the application agent reference
    m_process = CdmwPlatformMngt::ProcessDelegate::_duplicate(the_process);

    CdmwPlatformMngt::ProcessCallback::ProcessStartingData_var result(m_startingData);

    return result._retn();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessCallback/set_creation_done:1.0
//     
void ProcessCallback_impl::set_creation_done ()
        throw(CORBA::SystemException)
{
    // signals the acknowledgement
    if (!signal())
    {
        // The acknowledgement has been performed too late
        throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);
    }
}



//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessCallback/notify_error:1.0
//
void ProcessCallback_impl::notify_error(const char* issuer, const char* error_info)
    throw(CORBA::SystemException)
{

    try
    {
        m_errorIssuer = issuer; 
        m_errorInfo = error_info; 
    }
    catch(const std::bad_alloc&)
    {
        LogMngr::logMessage(ERR, MSG_ID_AGENT_NO_MEMORY);
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }

    // signals the error
    if (!signal(true))
    {
        // The acknowledgement has been performed too late
        throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);
    }

}


} // End namespace PlatformMngt
} // End namespace Cdmw


