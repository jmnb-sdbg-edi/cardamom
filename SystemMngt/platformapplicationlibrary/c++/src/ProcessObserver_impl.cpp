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
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "platformapplicationlibrary/ProcessObserver_impl.hpp"
#include "platformapplicationlibrary/ProcessProxy_impl.hpp"
#include "platformapplicationlibrary/ManagedProcessProxy_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "SystemMngt/platformvaluetypes/ProcessStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/UserEvent_impl.hpp"
#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

// ----------------------------------------------------------------------
// ProcessObserver_impl class.
// ----------------------------------------------------------------------
ProcessObserver_impl::ProcessObserver_impl(
    PortableServer::POA_ptr poa,
    Application_impl* application )
: DeactivableServant_impl( poa ), POA_CdmwPlatformMngt::ProcessObserver()
{
    m_application = application;
}

void ProcessObserver_impl::notify_message(
    const char* process_name,
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* issuer,
    const char* message )
throw( CdmwPlatformMngt::ProcessNotFound,
       CORBA::SystemException )
{
    try
    {
        std::auto_ptr< ProcessMessageEvent > event(
            new ProcessMessageEvent(
                m_application->getProcessProxy( process_name ),
                header,
                issuer,
                message ) );

        m_application->m_eventHandler.addEvent( event.get() );
        event.release();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

void ProcessObserver_impl::notify_error(
    const char* process_name,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp,
    const char* issuer,
    const char* error_info )
throw( CdmwPlatformMngt::ProcessNotFound,
       CORBA::SystemException )
{
    try
    {
        std::auto_ptr< ProcessErrorEvent > event(
            new ProcessErrorEvent(
                m_application->getProcessProxy( process_name ),
                time_stamp,
                issuer,
                error_info ) );

        m_application->m_eventHandler.addEvent( event.get() );
        event.release();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

void ProcessObserver_impl::notify_ending(
    const char* process_name,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp )
throw( CdmwPlatformMngt::ProcessNotFound,
       CORBA::SystemException )
{
    try
    {
        std::auto_ptr< ProcessEndingEvent > event(
            new ProcessEndingEvent(
                m_application->getProcessProxy( process_name ),
                time_stamp ) );

        m_application->m_eventHandler.addEvent( event.get() );
        event.release();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

void ProcessObserver_impl::notify_monitoring_failure(
    const char* process_name,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp )
throw( CdmwPlatformMngt::ProcessNotFound,
       CORBA::SystemException )
{
    try
    {
        ManagedProcessProxy_impl* managedProcessProxy
            = dynamic_cast< ManagedProcessProxy_impl* >(
                m_application->getProcessProxy( process_name ) );
        if( managedProcessProxy == NULL )
            throw CORBA::BAD_OPERATION(
                OrbSupport::BAD_OPERATION,
                CORBA::COMPLETED_NO );

        std::auto_ptr< ProcessMonitoringFailure > event(
            new ProcessMonitoringFailure(
                managedProcessProxy,
                time_stamp ) ) ;

        m_application->m_eventHandler.addEvent( event.get() );
        event.release();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

void ProcessObserver_impl::notify_point_monitoring_failure(
    const char* process_name,
    CORBA::ULong point_index,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp )
throw( CdmwPlatformMngt::ProcessNotFound,
       CdmwPlatformMngt::ActivityPointNotFound,
       CORBA::SystemException )
{
    ManagedProcessProxy_impl* managedProcessProxy = NULL;

    managedProcessProxy = dynamic_cast< ManagedProcessProxy_impl* >(
        m_application->getProcessProxy( process_name ) );
    if( managedProcessProxy == NULL )
        throw CORBA::BAD_OPERATION(
            OrbSupport::BAD_OPERATION,
            CORBA::COMPLETED_NO );

    try
    {
        std::auto_ptr< PointMonitoringFailure > event(
            new PointMonitoringFailure(
                managedProcessProxy,
                point_index,
                time_stamp ) ) ;

        m_application->m_eventHandler.addEvent( event.get() );
        event.release();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

void ProcessObserver_impl::notify_user_event(
    const char* process_name,
    const char* issuer,
    const CORBA::Any& data )
throw( CdmwPlatformMngt::ProcessNotFound,
       CORBA::SystemException )
{
    try
    {
        std::auto_ptr< ProcessUserEvent > event(
            new ProcessUserEvent(
                m_application->getProcessProxy( process_name ),
                issuer,
                data ) );

        m_application->m_eventHandler.addEvent( event.get() );
        event.release();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

// ----------------------------------------------------------------------
// ProcessMessageEvent class.
// ----------------------------------------------------------------------
ProcessMessageEvent::ProcessMessageEvent(
    ProcessProxy_impl* processProxy,
    const CdmwPlatformMngtBase::EventHeader& header,
    const char* issuer,
    const char* message )
:Event()
{
    m_processProxy = processProxy;
    m_header = header;
    m_issuer = issuer;
    m_message = message;
}

void ProcessMessageEvent::execute()
{
    try
    {
        #if CDMW_ORB_VDR == tao
        ProcessMessageFactory eventFactory;
        #else
        CORBA::ValueFactoryBase_var eventFactory
            = new ProcessMessageFactory();
        #endif

        CdmwPlatformMngt::ProcessID_var processId
            = new CdmwPlatformMngt::ProcessID;
        processId->application_name = m_processProxy->m_application->name();
        processId->process_name = m_processProxy->name();
        
        CdmwPlatformMngt::HostProxy_var hostProxy = m_processProxy->get_host();
        CORBA::String_var processHostName = hostProxy->name();
            
        #if CDMW_ORB_VDR == tao
        CdmwPlatformMngt::ProcessMessage_var event
            = eventFactory.create(
                m_header,
                m_processProxy->m_application->m_applicationAck->system_name.in(),
                processId.in(),
                processHostName.in(),
                m_issuer.c_str(),
                m_message.c_str() );
        #else
        CdmwPlatformMngt::ProcessMessage_var event
            = dynamic_cast< ProcessMessageFactory* >(
                eventFactory.in() )->create(
                    m_header,
                    m_processProxy->m_application->m_applicationAck->system_name.in(),
                    processId.in(),
                    processHostName.in(),
                    m_issuer.c_str(),
                    m_message.c_str() );
        #endif

        m_processProxy->m_application->m_eventNotifier->addEvent( event );
        event._retn();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

// ----------------------------------------------------------------------
// ProcessErrorEvent class.
// ----------------------------------------------------------------------
ProcessErrorEvent::ProcessErrorEvent(
    ProcessProxy_impl* processProxy,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp,
    const char* issuer,
    const char* error_info )
:Event()
{
    m_processProxy = processProxy;
    m_header.time_stamp = time_stamp;
    m_header.level = CdmwPlatformMngtBase::FTL;
    m_issuer = issuer;
    m_errorInfo = error_info;
}

void ProcessErrorEvent::execute()
{
    m_processProxy->error_event(
        m_header,
        m_issuer.c_str(),
        m_errorInfo.c_str() );
}

// ----------------------------------------------------------------------
// ProcessEndingEvent class.
// ----------------------------------------------------------------------
ProcessEndingEvent::ProcessEndingEvent(
    ProcessProxy_impl* processProxy,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp )
:Event()
{
    m_processProxy = processProxy;
    m_header.time_stamp = time_stamp;
    m_header.level = CdmwPlatformMngtBase::ERR;
}

void ProcessEndingEvent::execute()
{
    m_processProxy->ending_event(
        m_header );
}

// ----------------------------------------------------------------------
// ProcessMonitoringFailure class.
// ----------------------------------------------------------------------
ProcessMonitoringFailure::ProcessMonitoringFailure(
    ManagedProcessProxy_impl* managedProcessProxy,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp )
:Event()
{
    m_managedProcessProxy = managedProcessProxy;
    m_header.time_stamp = time_stamp;
    m_header.level = CdmwPlatformMngtBase::ERR;
}

void ProcessMonitoringFailure::execute()
{
    m_managedProcessProxy->monitoringFailure_event(
        m_header );
}

// ----------------------------------------------------------------------
// PointMonitoringFailure class.
// ----------------------------------------------------------------------
PointMonitoringFailure::PointMonitoringFailure(
    ManagedProcessProxy_impl* managedProcessProxy,
    CORBA::ULong point_index,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp )
:Event()
{
    m_managedProcessProxy = managedProcessProxy;
    m_point_index = point_index;
    m_header.time_stamp = time_stamp;
    m_header.level = CdmwPlatformMngtBase::ERR;
}

void PointMonitoringFailure::execute()
{
    // TODO
    throw CORBA::NO_IMPLEMENT(
        OrbSupport::NO_IMPLEMENT,
        CORBA::COMPLETED_NO );
}

// ----------------------------------------------------------------------
// ProcessUserEvent class.
// ----------------------------------------------------------------------
ProcessUserEvent::ProcessUserEvent(
    ProcessProxy_impl* processProxy,
    const char* issuer,
    const CORBA::Any& data )
:Event()
{
    m_processProxy = processProxy;
    CORBA::String_var application_name
        = m_processProxy->m_application->name();
    CORBA::String_var process_name
        = m_processProxy->name();

#if CDMW_ORB_VDR == tao && CDMW_ORB_VER < 14
    CORBA_ValueFactoryBase_var factory;
#else
    CORBA::ValueFactoryBase_var factory;
#endif
    factory = new UserEventFactory();

    m_event = dynamic_cast< UserEventFactory* >( factory.in() )->create(
        EventHeaderFactory::createHeader( CdmwPlatformMngtBase::INF ),
        m_processProxy->m_application->m_applicationAck->system_name.in(),
        application_name.in(),
        process_name.in(),
        issuer,
        data );
}

void ProcessUserEvent::execute()
{
    try
    {
        m_processProxy->m_application->m_eventNotifier->addEvent( m_event );
        m_event._retn();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

} // End namespace PlatformMngt
} // End namespace Cdmw

