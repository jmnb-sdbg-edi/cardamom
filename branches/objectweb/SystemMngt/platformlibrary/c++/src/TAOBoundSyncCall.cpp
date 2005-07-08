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


#if CDMW_ORB_VDR == tao

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "platformlibrary/TAOBoundSyncCall.hpp"

#include "SystemMngt/platformvaluetypes/Event_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationModeChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/EntityStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/Event_impl.hpp"
#include "SystemMngt/platformvaluetypes/GraphElement_impl.hpp"
#include "SystemMngt/platformvaluetypes/HostStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ProcessDef_impl.hpp"
#include "SystemMngt/platformvaluetypes/ProcessStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/SystemModeChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/SystemStatusChange_impl.hpp"

#include <tao/Messaging/Messaging.h>
#include <string>

namespace Cdmw
{
namespace PlatformMngt
{

CORBA::Object_ptr SetTimeoutPolicy(
    CORBA::ORB_ptr orb,
    CORBA::Object_ptr obj,
    unsigned long timeDuration )
{
    // Timeout success
    TimeBase::TimeT _timeout = ((unsigned long) (timeDuration * 1.0e4));
    CORBA::Any timeout;
    CORBA::PolicyList policy_list;

    timeout <<= _timeout;

    policy_list.length( 1 );
    policy_list[0] = orb->create_policy(
        Messaging::RELATIVE_RT_TIMEOUT_POLICY_TYPE,
        timeout );

    // Timed call to server
    CORBA::Object_var capsule
        = obj->_set_policy_overrides(
            policy_list,
            CORBA::SET_OVERRIDE );

    return capsule._retn();
}

// ----------------------------------------------------------------------
// TAOBoundSyncCallFactory class.
// ----------------------------------------------------------------------
TAOBoundSyncCallFactory::TAOBoundSyncCallFactory(
    CORBA::ORB_ptr orb )
    : BoundSyncCallFactory()
{
    m_orb = CORBA::ORB::_duplicate( orb );
}

TAOBoundSyncCallFactory::~TAOBoundSyncCallFactory()
{
}

ProcessInitCall*
TAOBoundSyncCallFactory::createProcessInitCall(
    CdmwPlatformMngt::Process_ptr process,
    const CdmwPlatformMngtBase::StartupKind& startupKind,
    unsigned long timeout )
throw( OutOfMemoryException )
{
    ProcessInitCall* processInitCall = NULL;

    try
    {
        processInitCall
            = new TAOProcessInitCall( m_orb.in(), process, startupKind, timeout );
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }

    return processInitCall;
}

ProcessStepCall*
TAOBoundSyncCallFactory::createProcessStepCall(
    CdmwPlatformMngt::Process_ptr process,
    unsigned long timeout )
throw( OutOfMemoryException )
{
    ProcessStepCall* processStepCall = NULL;

    try
    {
        processStepCall = new TAOProcessStepCall( m_orb.in(), process, timeout );
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }

    return processStepCall;
}

ProcessRunCall*
TAOBoundSyncCallFactory::createProcessRunCall(
    CdmwPlatformMngt::Process_ptr process,
    unsigned long timeout )
throw( OutOfMemoryException )
{
    ProcessRunCall* processRunCall = NULL;

    try
    {
        processRunCall = new TAOProcessRunCall( m_orb.in(), process, timeout );
    }    
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }

    return processRunCall;
}

ProcessStopCall*
TAOBoundSyncCallFactory::createProcessStopCall(
    CdmwPlatformMngt::Process_ptr process,
    unsigned long timeout )
throw( OutOfMemoryException )
{
    ProcessStopCall* processStopCall = NULL;

    try
    {
        processStopCall = new TAOProcessStopCall( m_orb.in(), process, timeout );
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }

    return processStopCall;
}

EventNotificationCall*
TAOBoundSyncCallFactory::createEventNotificationCall(
    CdmwPlatformMngt::SupervisionObserver_ptr observer,
    CdmwPlatformMngt::Event* event,
    unsigned long timeout )
throw( OutOfMemoryException )
{
    EventNotificationCall* eventNotificationCall = NULL;

    try
    {
        eventNotificationCall
            = new TAOEventNotificationCall( m_orb.in(), observer, event, timeout );
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }

    return eventNotificationCall;
}

PullMonitoringCall*
TAOBoundSyncCallFactory::createPullMonitoringCall(
    CdmwPlatformMngt::PullMonitorable_ptr monitorable,
    unsigned long timeout )
throw( OutOfMemoryException )
{
    PullMonitoringCall* pullMonitorableCall = NULL;

    try
    {
        pullMonitorableCall
            = new TAOPullMonitoringCall( m_orb.in(), monitorable, timeout );
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }

    return pullMonitorableCall;
}

PlatformInfoNotificationCall*
TAOBoundSyncCallFactory::createPlatformInfoNotificationCall(
    CdmwPlatformMngt::PlatformObserver_ptr observer,
    const CdmwPlatformMngt::PlatformInfo& platformInfo,
    unsigned long timeout )
throw( OutOfMemoryException )
{
    PlatformInfoNotificationCall* platformInfoNotificationCall = NULL;

    try
    {
        platformInfoNotificationCall
            = new TAOPlatformInfoNotificationCall(
                m_orb.in(), observer, platformInfo, timeout );
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }

    return platformInfoNotificationCall;
}

// ----------------------------------------------------------------------
// TAOProcessInitCall class.
// ----------------------------------------------------------------------
TAOProcessInitCall::TAOProcessInitCall(
    CORBA::ORB_ptr orb,
    CdmwPlatformMngt::Process_ptr process,
    const CdmwPlatformMngtBase::StartupKind& startupKind,
    unsigned long timeout )
throw( OutOfMemoryException )
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_process = CdmwPlatformMngt::Process::_duplicate( process );

    try
    {
        m_startupKind = new CdmwPlatformMngtBase::StartupKind( startupKind );
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }

    m_timeout = timeout;
}

TAOProcessInitCall::~TAOProcessInitCall()
{
}

BoundSyncCallReturn
TAOProcessInitCall::execute()
throw()
{
    BoundSyncCallReturn result = SUCCEED;
    
    try
    {
        // Set the timeout policy
        CORBA::Object_var capsule = SetTimeoutPolicy(
            m_orb.in(), m_process.in(), m_timeout );

        // Perform the process's action
        CdmwPlatformMngt::Process_var process
            = CdmwPlatformMngt::Process::_narrow( capsule.in() );
        if( CORBA::is_nil( process.in() ) )
            throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );
        process->initialise( m_startupKind.in() );
    }
    catch( const CORBA::TIMEOUT& )
    {
        result = TIMEOUT;
    }
    catch( const CORBA::Exception& e )
    {
        result = EXCEPTION;
        setExceptionInfo( e );
    }
    
    return result;
}

// ----------------------------------------------------------------------
// TAOProcessStepCall class.
// ----------------------------------------------------------------------
TAOProcessStepCall::TAOProcessStepCall(
    CORBA::ORB_ptr orb,
    CdmwPlatformMngt::Process_ptr process,
    unsigned long timeout )
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_process = CdmwPlatformMngt::Process::_duplicate( process );
    m_timeout = timeout;
}

TAOProcessStepCall::~TAOProcessStepCall()
{
}

BoundSyncCallReturn
TAOProcessStepCall::execute()
throw()
{
    BoundSyncCallReturn result = SUCCEED;
    
    try
    {
        // Set the timeout policy
        CORBA::Object_var capsule = SetTimeoutPolicy(
            m_orb.in(), m_process.in(), m_timeout );

        // Perform the process's action
        CdmwPlatformMngt::Process_var process
            = CdmwPlatformMngt::Process::_narrow( capsule.in() );
        if( CORBA::is_nil( process.in() ) )
            throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );
        process->next_step();
    }
    catch( const CORBA::TIMEOUT& )
    {
        result = TIMEOUT;
    }
    catch( const CORBA::Exception& e )
    {
        result = EXCEPTION;
        setExceptionInfo( e );
    }
    
    return result;
}

// ----------------------------------------------------------------------
// TAOProcessRunCall class.
// ----------------------------------------------------------------------
TAOProcessRunCall::TAOProcessRunCall(
    CORBA::ORB_ptr orb,
    CdmwPlatformMngt::Process_ptr process,
    unsigned long timeout )
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_process = CdmwPlatformMngt::Process::_duplicate( process );
    m_timeout = timeout;
}

TAOProcessRunCall::~TAOProcessRunCall()
{
}

BoundSyncCallReturn
TAOProcessRunCall::execute()
throw()
{
    BoundSyncCallReturn result = SUCCEED;
    
    try
    {
        // Set the timeout policy
        CORBA::Object_var capsule = SetTimeoutPolicy(
            m_orb.in(), m_process.in(), m_timeout );

        // Perform the process's action
        CdmwPlatformMngt::Process_var process
            = CdmwPlatformMngt::Process::_narrow( capsule.in() );
        if( CORBA::is_nil( process.in() ) )
            throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );
        process->run();
    }
    catch( const CORBA::TIMEOUT& )
    {
        result = TIMEOUT;
    }
    catch( const CORBA::Exception& e )
    {
        result = EXCEPTION;
        setExceptionInfo( e );
    }
    
    return result;
}

// ----------------------------------------------------------------------
// TAOProcessStopCall class.
// ----------------------------------------------------------------------
TAOProcessStopCall::TAOProcessStopCall(
    CORBA::ORB_ptr orb,
    CdmwPlatformMngt::Process_ptr process,
    unsigned long timeout )
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_process = CdmwPlatformMngt::Process::_duplicate( process );
    m_timeout = timeout;
}

TAOProcessStopCall::~TAOProcessStopCall()
{
}

BoundSyncCallReturn
TAOProcessStopCall::execute()
throw()
{
    BoundSyncCallReturn result = SUCCEED;
    
    try
    {
        // Set the timeout policy
        CORBA::Object_var capsule = SetTimeoutPolicy(
            m_orb.in(), m_process.in(), m_timeout );

        // Perform the process's action
        CdmwPlatformMngt::Process_var process
            = CdmwPlatformMngt::Process::_narrow( capsule.in() );
        if( CORBA::is_nil( process.in() ) )
            throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );
        process->stop();
    }
    catch( const CORBA::TIMEOUT& )
    {
        result = TIMEOUT;
    }
    catch( const CORBA::Exception& e )
    {
        result = EXCEPTION;
        setExceptionInfo( e );
    }
    
    return result;
}

// ----------------------------------------------------------------------
// TAOEventNotificationCall class.
// ----------------------------------------------------------------------
TAOEventNotificationCall::TAOEventNotificationCall(
    CORBA::ORB_ptr orb,
    CdmwPlatformMngt::SupervisionObserver_ptr observer,
    CdmwPlatformMngt::Event* event,
    unsigned long timeout )
{
    Event_impl* event_impl = NULL;
    event_impl = dynamic_cast<Event_impl*>( event );

    m_orb = CORBA::ORB::_duplicate( orb );
    m_observer = CdmwPlatformMngt::SupervisionObserver::_duplicate( observer );
    m_event = CdmwPlatformMngt::Event::_downcast( event_impl->_copy_value() );
    m_timeout = timeout;
}

TAOEventNotificationCall::~TAOEventNotificationCall()
{
}

BoundSyncCallReturn
TAOEventNotificationCall::execute()
throw()
{
    BoundSyncCallReturn result = SUCCEED;
    
    try
    {
        // Set the timeout policy
        CORBA::Object_var capsule = SetTimeoutPolicy(
            m_orb.in(), m_observer.in(), m_timeout );

        // Perform the process's action
        CdmwPlatformMngt::SupervisionObserver_var observer
            = CdmwPlatformMngt::SupervisionObserver::_narrow( capsule.in() );
        if( CORBA::is_nil( observer.in() ) )
            throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );
        observer->notify( m_event );
    }
    catch( const CORBA::TIMEOUT& )
    {
        result = TIMEOUT;
    }
    catch( const CORBA::Exception& e )
    {
        result = EXCEPTION;
        setExceptionInfo( e );
    }
    
    return result;
}

// ----------------------------------------------------------------------
// TAOPullMonitoringCall class.
// ----------------------------------------------------------------------
TAOPullMonitoringCall::TAOPullMonitoringCall(
    CORBA::ORB_ptr orb,
    CdmwPlatformMngt::PullMonitorable_ptr monitorable,
    unsigned long timeout )
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_monitorable = CdmwPlatformMngt::PullMonitorable::_duplicate( monitorable );
    m_timeout = timeout;
}

TAOPullMonitoringCall::~TAOPullMonitoringCall()
{
}

BoundSyncCallReturn
TAOPullMonitoringCall::execute()
throw()
{
    BoundSyncCallReturn result = SUCCEED;
    
    try
    {
        // Set the timeout policy
        CORBA::Object_var capsule = SetTimeoutPolicy(
            m_orb.in(), m_monitorable.in(), m_timeout );

        // Perform the monitorable's action
        CdmwPlatformMngt::PullMonitorable_var monitorable
            = CdmwPlatformMngt::PullMonitorable::_narrow( capsule.in() );
        if( CORBA::is_nil( monitorable.in() ) )
            throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );
        monitorable->is_alive();
    }
    catch( const CORBA::TIMEOUT& )
    {
        result = TIMEOUT;
    }
    catch( const CORBA::Exception& e )
    {
        result = EXCEPTION;
        setExceptionInfo( e );
    }
    
    return result;
}

// ----------------------------------------------------------------------
// TAOPlatformInfoNotificationCall class.
// ----------------------------------------------------------------------
TAOPlatformInfoNotificationCall::TAOPlatformInfoNotificationCall(
    CORBA::ORB_ptr orb,
    CdmwPlatformMngt::PlatformObserver_ptr observer,
    const CdmwPlatformMngt::PlatformInfo& platformInfo,
    unsigned long timeout )
throw( OutOfMemoryException )
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_observer = CdmwPlatformMngt::PlatformObserver::_duplicate( observer );

    try
    {
        m_platformInfo = new CdmwPlatformMngt::PlatformInfo( platformInfo );
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }

    m_timeout = timeout;
}

TAOPlatformInfoNotificationCall::~TAOPlatformInfoNotificationCall()
{
}

BoundSyncCallReturn
TAOPlatformInfoNotificationCall::execute()
throw()
{
    BoundSyncCallReturn result = SUCCEED;
    
    try
    {
        // Set the timeout policy
        CORBA::Object_var capsule = SetTimeoutPolicy(
            m_orb.in(), m_observer.in(), m_timeout );

        // Perform the platform observer's action
        CdmwPlatformMngt::PlatformObserver_var observer
            = CdmwPlatformMngt::PlatformObserver::_narrow( capsule.in() );
        if( CORBA::is_nil( observer.in() ) )
            throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );
        observer->notify( m_platformInfo.in() );
    }
    catch( const CORBA::TIMEOUT& )
    {
        result = TIMEOUT;
    }
    catch( const CORBA::Exception& e )
    {
        result = EXCEPTION;
        setExceptionInfo( e );
    }
    
    return result;
}

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //CDMW_ORB_VDR == tao

