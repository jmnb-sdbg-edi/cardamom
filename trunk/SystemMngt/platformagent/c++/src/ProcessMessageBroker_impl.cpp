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


#include "Foundation/common/Assert.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "platformagent/ProcessMessageBroker_impl.hpp"


#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "platformdaemonlibrary/DaemonLogMessageId.hpp"

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
ProcessMessageBroker_impl::ProcessMessageBroker_impl(CORBA::ORB_ptr orb, const char* poaName,
                                                     CdmwPlatformMngt::Daemon_ptr daemon)
    : MultipleIdServant_impl(orb, poaName)
{
    m_daemon = CdmwPlatformMngt::Daemon::_duplicate(daemon);
    
    // get host name
    m_hostName = OsSupport::OS::get_hostname();
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

void ProcessMessageBroker_impl::setServiceBroker(CdmwPlatformMngtService::ServiceBroker_ptr serviceBroker)
    throw (BadParameterException)
{
    if (CORBA::is_nil(serviceBroker))
    {
        LogMngr::logMessage(FTL, MSG_ID_AGENT_PROCESS_OBSERVER_NIL);
        CDMW_THROW2(BadParameterException, "serviceBroker", "nil");
    }

    m_serviceBroker = CdmwPlatformMngtService::ServiceBroker::_duplicate(serviceBroker);
}


void ProcessMessageBroker_impl::setApplicationName(const char* applicationName)
        throw (BadParameterException)
{
    if (applicationName == NULL || *applicationName == '\0')
    {
        CDMW_THROW2(BadParameterException, "application name", "null");
    }
    
    m_applicationName = applicationName;
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

        m_processObserver->notify_message(m_applicationName.c_str(),
                                          processName.c_str(),
                                          m_hostName.c_str(),
                                          header, issuer, message);
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
        m_processObserver->notify_error(m_applicationName.c_str(),
                                        processName.c_str(),
                                        m_hostName.c_str(),
                                        time_stamp, issuer, error_info);
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

void ProcessMessageBroker_impl::notify_process_error (
        const char * failed_process_name,
        const CdmwPlatformMngtBase::TimeStamp & time_stamp,
        const char * issuer,
        const char * error_info)
    throw(CdmwPlatformMngt::ProcessNotFound,
          CORBA::SystemException)
{
    // TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
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

        m_processObserver->notify_user_event(m_applicationName.c_str(),
                                             processName.c_str(),
                                             m_hostName.c_str(),
                                             issuer, data);
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

CORBA::Object_ptr ProcessMessageBroker_impl::get_service(const char* service_name)
throw (CdmwPlatformMngtService::ServiceNotFound,
       CdmwPlatformMngtService::ServiceUnavailable,
       CORBA::SystemException)
{
    CDMW_ASSERT(!CORBA::is_nil(m_serviceBroker.in()));

    CORBA::Object_var service;
    
    try
    {
        // first search in local daemon
        service = m_daemon->resolve_local_service (service_name);
    
        // if service has not been found in local daemon
        if (CORBA::is_nil(service.in()))
        {
            std::string processName = getCurrentName();

            // request service to the supervision
            service = m_serviceBroker->get_service(m_applicationName.c_str(),
                                                   processName.c_str(),
                                                   m_hostName.c_str(),
                                                   service_name);
        }
    }
    catch(const CORBA::SystemException &e)
    {
        std::ostringstream exceptionInfo;
        exceptionInfo << e;
        LogMngr::logMessage(ERR, MSG_ID_AGENT_EXCEPTION_FROM_APPLICATION,
                            exceptionInfo.str().c_str());
        throw;
    }

    return service._retn();
}

CdmwPlatformMngt::ManagedElement_ptr ProcessMessageBroker_impl::get_managed_element (
     const char * element_path)
throw (CdmwPlatformMngt::ElementPathInvalid,
       CdmwPlatformMngt::ManagedElementNotFound,
       CORBA::SystemException)
{
    CDMW_ASSERT(!CORBA::is_nil(m_processObserver.in()));
    
    try
    {
        return m_processObserver->get_managed_element (element_path);
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

char* ProcessMessageBroker_impl::get_system_entity_status (
        const char* entity_name,
        CORBA::String_out entity_info)
throw (CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    CDMW_ASSERT(!CORBA::is_nil(m_processObserver.in()));
    
    try
    {
        return m_processObserver->get_system_entity_status(entity_name, entity_info);
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

char* ProcessMessageBroker_impl::get_host_entity_status (
        const char* host_name,
        const char* entity_name,
        CORBA::String_out entity_info)
throw (CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    CDMW_ASSERT(!CORBA::is_nil(m_processObserver.in()));
    
    try
    {
        std::string host_name_buf;
        
        if (host_name == NULL)
        {
            host_name_buf = m_hostName;
        }
        else if (!strcmp(host_name, ""))
        {
            host_name_buf = m_hostName;
        }
        else
        {
            host_name_buf = host_name;
        }
        
        return m_processObserver->get_host_entity_status(host_name_buf.c_str(),
                                                         entity_name, entity_info);
    }
    catch(const CdmwPlatformMngt::HostNotFound &)
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

               
char* ProcessMessageBroker_impl::get_application_entity_status (
        const char* application_name,
        const char* entity_name,
        CORBA::String_out entity_info)
throw (CdmwPlatformMngt::ApplicationNotFound,
       CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    CDMW_ASSERT(!CORBA::is_nil(m_processObserver.in()));
    
    try
    {
        std::string application_name_buf;
        
        if (application_name == NULL)
        {
            application_name_buf = m_applicationName;
        }
        else if (!strcmp(application_name, ""))
        {
            application_name_buf = m_applicationName;
        }
        else
        {
            application_name_buf = application_name;
        }
        
        return m_processObserver->get_application_entity_status(
                                                      application_name_buf.c_str(),
                                                      entity_name, entity_info);
    }
    catch(const CdmwPlatformMngt::ApplicationNotFound &)
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

               

char* ProcessMessageBroker_impl::get_process_entity_status (
        const char* application_name,
        const char* process_name,
        const char* host_name,
        const char* entity_name,
        CORBA::String_out entity_info)
throw (CdmwPlatformMngt::ProcessNotFound,
       CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    CDMW_ASSERT(!CORBA::is_nil(m_processObserver.in()));
    
    try
    {
        std::string application_name_buf = m_applicationName;
        std::string process_name_buf = getCurrentName();
        std::string host_name_buf = m_hostName;
        
        if (application_name != NULL)
        {
            if (strcmp(application_name, ""))
            {
                application_name_buf = application_name;
                process_name_buf = process_name;
                host_name_buf = host_name;
            }
        }
        
        return m_processObserver->get_process_entity_status(
                                                      application_name_buf.c_str(),
                                                      process_name_buf.c_str(),
                                                      host_name_buf.c_str(),
                                                      entity_name, entity_info);
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


void ProcessMessageBroker_impl::set_system_entity_status (
        const char* entity_name,
        const char* entity_status,
        const char* entity_info)
throw (CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    CDMW_ASSERT(!CORBA::is_nil(m_processObserver.in()));

    try
    {
        m_processObserver->set_system_entity_status(entity_name, entity_status, entity_info);
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
               
void ProcessMessageBroker_impl::set_host_entity_status (
        const char* host_name,
        const char* entity_name,
        const char* entity_status,
        const char* entity_info)
throw (CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    CDMW_ASSERT(!CORBA::is_nil(m_processObserver.in()));

    try
    {
        std::string host_name_buf;
        
        if (host_name == NULL)
        {
            host_name_buf = m_hostName;
        }
        else if (!strcmp(host_name, ""))
        {
            host_name_buf = m_hostName;
        }
        else
        {
            host_name_buf = host_name;
        }
     
        m_processObserver->set_host_entity_status(host_name_buf.c_str(),
                                                  entity_name, entity_status, entity_info);
    }
    catch(const CdmwPlatformMngt::HostNotFound &)
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


void ProcessMessageBroker_impl::set_application_entity_status (
        const char* application_name,
        const char* entity_name,
        const char* entity_status,
        const char* entity_info)
throw (CdmwPlatformMngt::ApplicationNotFound,
       CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    CDMW_ASSERT(!CORBA::is_nil(m_processObserver.in()));

    try
    {
        std::string application_name_buf;
        
        if (application_name == NULL)
        {
            application_name_buf = m_applicationName;
        }
        else if (!strcmp(application_name, ""))
        {
            application_name_buf = m_applicationName;
        }
        else
        {
            application_name_buf = application_name;
        }
        
        m_processObserver->set_application_entity_status(
                                                     application_name_buf.c_str(),
                                                     entity_name, entity_status, entity_info);
    }
    catch(const CdmwPlatformMngt::ApplicationNotFound &)
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

       

void ProcessMessageBroker_impl::set_process_entity_status (
        const char* application_name,
        const char* process_name,
        const char* host_name,
        const char* entity_name,
        const char* entity_status,
        const char* entity_info)
throw (CdmwPlatformMngt::ProcessNotFound,
       CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    CDMW_ASSERT(!CORBA::is_nil(m_processObserver.in()));

    try
    {
        std::string application_name_buf = m_applicationName;
        std::string process_name_buf = getCurrentName();
        std::string host_name_buf = m_hostName;
        
        if (application_name != NULL)
        {
            if (strcmp(application_name, ""))
            {
                application_name_buf = application_name;
                process_name_buf = process_name;
                host_name_buf = host_name;
            }
        }
        
        m_processObserver->set_process_entity_status(application_name_buf.c_str(),
                                                     process_name_buf.c_str(),
                                                     host_name_buf.c_str(),
                                                     entity_name, entity_status, entity_info);
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


