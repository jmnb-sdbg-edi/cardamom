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


#include "Foundation/common/Assert.hpp"
#include "platformagent/ProcessMessageBroker_impl.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "platformagent/ApplicationAgentLogMessageId.hpp"

#ifdef CDMW_USE_FAULTTOLERANCE
#include "platformagent/ApplicationAgent_impl.hpp"
#endif

#include <sstream>

namespace Cdmw
{
namespace PlatformMngt
{


//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker:1.0
//
#ifdef CDMW_USE_FAULTTOLERANCE
ProcessMessageBroker_impl::ProcessMessageBroker_impl(ApplicationAgent_impl* agent, CORBA::ORB_ptr orb)
    : MultipleIdServant_impl(orb, "ProcessMessageBrokerPOA"), m_agent(agent)
#else
ProcessMessageBroker_impl::ProcessMessageBroker_impl(CORBA::ORB_ptr orb)
    : MultipleIdServant_impl(orb, "ProcessMessageBrokerPOA")
#endif
{
}


void ProcessMessageBroker_impl::setProcessObserver(CdmwPlatformMngt::ProcessObserver_ptr processObserver)
    throw (BadParameterException)
{
    if (CORBA::is_nil(processObserver))
    {
        LogMngr::logMessage(FTL, MSG_ID_AGENT_PROCESS_OBSERVER_NIL);
        CDMW_THROW2(BadParameterException, "processObserver", "nil");
    }

    m_processObserver = CdmwPlatformMngt::ProcessObserver::_duplicate(processObserver);

}

void ProcessMessageBroker_impl::notify_message(
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* issuer, const char* message)
    throw(CORBA::SystemException)
{
    CDMW_ASSERT(!CORBA::is_nil(m_processObserver.in()));

    try
    {
        std::string processName = getCurrentName();

        m_processObserver->notify_message(processName.c_str(), header, issuer, message);
    }
    catch(const OutOfMemoryException &)
    {
        LogMngr::logMessage(ERR, MSG_ID_AGENT_NO_MEMORY);
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch(const InternalErrorException &)
    {
        LogMngr::logMessage(ERR, MSG_ID_AGENT_INTERNAL);
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
    catch(const CdmwPlatformMngt::ProcessNotFound &)
    {
        LogMngr::logMessage(ERR, MSG_ID_AGENT_INTERNAL);
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
    catch(const CORBA::SystemException &e)
    {
        std::ostringstream exceptionInfo;
        exceptionInfo << e;
        
        LogMngr::logMessage(ERR, MSG_ID_AGENT_EXCEPTION_FROM_APPLICATION,
                            exceptionInfo.str().c_str());
        throw;
    }

}

void ProcessMessageBroker_impl::notify_error(
        const CdmwPlatformMngtBase::TimeStamp& time_stamp,
        const char* issuer, const char* error_info)
    throw(CORBA::SystemException)
{
    CDMW_ASSERT(!CORBA::is_nil(m_processObserver.in()));

    try
    {
        std::string processName = getCurrentName();
        m_processObserver->notify_error(processName.c_str(), time_stamp, issuer, error_info);
        //m_agent->kill_process(processName.c_str());
    }
    catch(const OutOfMemoryException &)
    {
        LogMngr::logMessage(ERR, MSG_ID_AGENT_NO_MEMORY);
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch(const InternalErrorException &)
    {
        LogMngr::logMessage(ERR, MSG_ID_AGENT_INTERNAL);
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
    catch(const CdmwPlatformMngt::ProcessNotFound &)
    {
        LogMngr::logMessage(ERR, MSG_ID_AGENT_INTERNAL);
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
    catch(const CORBA::SystemException &e)
    {
        std::ostringstream exceptionInfo;
        exceptionInfo << e;
        LogMngr::logMessage(ERR, MSG_ID_AGENT_EXCEPTION_FROM_APPLICATION,
                            exceptionInfo.str().c_str());
        throw;
    }
}

void ProcessMessageBroker_impl::notify_user_event(
        const char* issuer,
        const CORBA::Any& data )
    throw(CORBA::SystemException)
{
    CDMW_ASSERT(!CORBA::is_nil(m_processObserver.in()));

    try
    {
        std::string processName = getCurrentName();

        m_processObserver->notify_user_event(
            processName.c_str(), issuer, data);
    }
    catch(const OutOfMemoryException &)
    {
        LogMngr::logMessage(ERR, MSG_ID_AGENT_NO_MEMORY);
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch(const InternalErrorException &)
    {
        LogMngr::logMessage(ERR, MSG_ID_AGENT_INTERNAL);
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
    catch(const CdmwPlatformMngt::ProcessNotFound &)
    {
        LogMngr::logMessage(ERR, MSG_ID_AGENT_INTERNAL);
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
    catch(const CORBA::SystemException &e)
    {
        std::ostringstream exceptionInfo;
        exceptionInfo << e;
        LogMngr::logMessage(ERR, MSG_ID_AGENT_EXCEPTION_FROM_APPLICATION,
                            exceptionInfo.str().c_str());
        throw;
    }

}

} // End namespace PlatformMngt
} // End namespace Cdmw


