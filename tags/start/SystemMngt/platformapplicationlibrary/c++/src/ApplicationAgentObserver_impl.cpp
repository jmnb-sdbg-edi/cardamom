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


#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "platformapplicationlibrary/ApplicationAgentObserver_impl.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

// ----------------------------------------------------------------------
// ApplicationAgentObserver_impl class.
// ----------------------------------------------------------------------
ApplicationAgentObserver_impl::ApplicationAgentObserver_impl(
    PortableServer::POA_ptr poa,
    Application_impl* application )
: DeactivableServant_impl( poa ), POA_CdmwPlatformMngt::MonitorableProcessObserver()
{
    m_application = application;
}

void ApplicationAgentObserver_impl::notify_ending(
    const char* feature_name,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp )
throw( CORBA::SystemException )
{
    try
    {
        std::auto_ptr< AgentEndingEvent > event(
            new AgentEndingEvent(
                m_application,
                feature_name,
                time_stamp ) );

        m_application->m_eventHandler.addEvent( event.get() );
        event.release();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

void ApplicationAgentObserver_impl::notify_monitoring_failure(
    const char* feature_name,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp )
throw( CORBA::SystemException )
{
    try
    {
        std::auto_ptr< AgentMonitoringFailureEvent > event(
            new AgentMonitoringFailureEvent(
                m_application,
                feature_name,
                time_stamp ) );

        m_application->m_eventHandler.addEvent( event.get() );
        event.release();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

// ----------------------------------------------------------------------
// AgentEndingEvent class.
// ----------------------------------------------------------------------
AgentEndingEvent::AgentEndingEvent(
    Application_impl* application,
    const char* feature_name,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp )
:Event()
{
    m_application = application;
    m_header.level = CdmwPlatformMngtBase::ERR;
    m_header.time_stamp = time_stamp;
    m_host_name = feature_name;
}

void AgentEndingEvent::execute()
{
    // Log and notify the agent death when application is still active
    CORBA::String_var statusInfo;
    CdmwPlatformMngt::ApplicationStatus status
        = m_application->get_status( statusInfo.out() );

    if( (status == CdmwPlatformMngt::APPLICATION_INITIALISING) ||
        (status == CdmwPlatformMngt::APPLICATION_INITIALISATION_INCOMPLETE) ||
        (status == CdmwPlatformMngt::APPLICATION_INITIALISED) ||
        (status == CdmwPlatformMngt::APPLICATION_RUN_REQUEST) ||
        (status == CdmwPlatformMngt::APPLICATION_RUNNING) ||
        (status == CdmwPlatformMngt::APPLICATION_STOPPING) )
    {
        m_application->notifyAgentDeathEvent(
            m_header,
            m_host_name.c_str() );
    }

    // Invalidate all dependency
    m_application->invalidateProcesses(
        m_header,
        m_host_name.c_str(),
        Application_impl::AGENT_DEATH );
}

// ----------------------------------------------------------------------
// AgentMonitoringFailureEvent class.
// ----------------------------------------------------------------------
AgentMonitoringFailureEvent::AgentMonitoringFailureEvent(
    Application_impl* application,
    const char* feature_name,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp )
:Event()
{
    m_application = application;
    m_header.level = CdmwPlatformMngtBase::ERR;
    m_header.time_stamp = time_stamp;
    m_host_name = feature_name;
}

void AgentMonitoringFailureEvent::execute()
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );

    /*
    m_application->invalidateProcesses(
        m_header,
        m_host_name.c_str(),
        Application_impl::HOST_UNAVAILABLE );
    */
}

} // End namespace PlatformMngt
} // End namespace Cdmw

