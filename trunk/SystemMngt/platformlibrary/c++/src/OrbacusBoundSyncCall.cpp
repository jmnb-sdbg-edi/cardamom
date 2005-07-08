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


#if CDMW_ORB_VDR == orbacus

#include <OB/CORBA.h>
#include <OB/OBPolicies.h>
#include <string>
#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "OrbacusBoundSyncCall.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

CORBA::Object_ptr SetTimeoutPolicy(
    CORBA::ORB_ptr orb,
    CORBA::Object_ptr obj,
    unsigned long timeDuration )
{
    // Create the timeout policy
    CORBA::Any timeout;
    CORBA::PolicyList policies(1);

    timeout <<= (CORBA::ULong) timeDuration;
    policies.length( 1 );
    policies[0] = orb->create_policy( OB::TIMEOUT_POLICY_ID, timeout );

    // Set the policy
    CORBA::Object_var capsule = obj->_set_policy_overrides( policies, CORBA::ADD_OVERRIDE );
    if( CORBA::is_nil( capsule ) )
        throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );
    
    return capsule._retn();
}

// ----------------------------------------------------------------------
// ORBACUSBoundSyncCallFactory class.
// ----------------------------------------------------------------------
ORBACUSBoundSyncCallFactory::ORBACUSBoundSyncCallFactory(
    CORBA::ORB_ptr orb )
    : BoundSyncCallFactory()
{
    m_orb = CORBA::ORB::_duplicate( orb );
}

ORBACUSBoundSyncCallFactory::~ORBACUSBoundSyncCallFactory()
{
}

ProcessInitCall*
ORBACUSBoundSyncCallFactory::createProcessInitCall(
    CdmwPlatformMngt::Process_ptr process,
    const CdmwPlatformMngtBase::StartupKind& startupKind,
    unsigned long timeout )
throw( OutOfMemoryException )
{
    ProcessInitCall* processInitCall = NULL;

    try
    {
        processInitCall = new ORBACUSProcessInitCall( m_orb, process, startupKind, timeout );
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }

    return processInitCall;
}

ProcessStepCall*
ORBACUSBoundSyncCallFactory::createProcessStepCall(
    CdmwPlatformMngt::Process_ptr process,
    unsigned long timeout )
throw( OutOfMemoryException )
{
    ProcessStepCall* processStepCall = NULL;

    try
    {
        processStepCall = new ORBACUSProcessStepCall( m_orb, process, timeout );
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }

    return processStepCall;
}

ProcessRunCall*
ORBACUSBoundSyncCallFactory::createProcessRunCall(
    CdmwPlatformMngt::Process_ptr process,
    unsigned long timeout )
throw( OutOfMemoryException )
{
    ProcessRunCall* processRunCall = NULL;

    try
    {
        processRunCall = new ORBACUSProcessRunCall( m_orb, process, timeout );
    }    
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }

    return processRunCall;
}

ProcessStopCall*
ORBACUSBoundSyncCallFactory::createProcessStopCall(
    CdmwPlatformMngt::Process_ptr process,
    unsigned long timeout )
throw( OutOfMemoryException )
{
    ProcessStopCall* processStopCall = NULL;

    try
    {
        processStopCall = new ORBACUSProcessStopCall( m_orb, process, timeout );
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }

    return processStopCall;
}

EventNotificationCall*
ORBACUSBoundSyncCallFactory::createEventNotificationCall(
    CdmwPlatformMngt::SupervisionObserver_ptr observer,
    CdmwPlatformMngt::Event* event,
    unsigned long timeout )
throw( OutOfMemoryException )
{
    EventNotificationCall* eventNotificationCall = NULL;

    try
    {
        eventNotificationCall = new ORBACUSEventNotificationCall( m_orb, observer, event, timeout );
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }

    return eventNotificationCall;
}

PullMonitoringCall*
ORBACUSBoundSyncCallFactory::createPullMonitoringCall(
    CdmwPlatformMngt::PullMonitorable_ptr monitorable,
    unsigned long timeout )
throw( OutOfMemoryException )
{
    PullMonitoringCall* pullMonitorableCall = NULL;

    try
    {
        pullMonitorableCall = new ORBACUSPullMonitoringCall( m_orb, monitorable, timeout );
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }

    return pullMonitorableCall;
}

PlatformInfoNotificationCall*
ORBACUSBoundSyncCallFactory::createPlatformInfoNotificationCall(
    CdmwPlatformMngt::PlatformObserver_ptr observer,
    const CdmwPlatformMngt::PlatformInfo& platformInfo,
    unsigned long timeout )
throw( OutOfMemoryException )
{
    PlatformInfoNotificationCall* platformInfoNotificationCall = NULL;

    try
    {
        platformInfoNotificationCall = new ORBACUSPlatformInfoNotificationCall( m_orb, observer, platformInfo, timeout );
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }

    return platformInfoNotificationCall;
}

// ----------------------------------------------------------------------
// ORBACUSProcessInitCall class.
// ----------------------------------------------------------------------
ORBACUSProcessInitCall::ORBACUSProcessInitCall(
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

ORBACUSProcessInitCall::~ORBACUSProcessInitCall()
{
}

BoundSyncCallReturn
ORBACUSProcessInitCall::execute()
throw()
{
    BoundSyncCallReturn result = SUCCEED;
    
    try
    {
        // Set the timeout policy
        CORBA::Object_var capsule = SetTimeoutPolicy( m_orb, m_process, m_timeout );

        // Perform the process's action
        CdmwPlatformMngt::Process_var process = CdmwPlatformMngt::Process::_narrow( capsule );
        if( CORBA::is_nil( process ) )
            throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );
        process->initialise( m_startupKind );
    }
    catch( const CORBA::NO_RESPONSE& )
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
// ORBACUSProcessStepCall class.
// ----------------------------------------------------------------------
ORBACUSProcessStepCall::ORBACUSProcessStepCall(
    CORBA::ORB_ptr orb,
    CdmwPlatformMngt::Process_ptr process,
    unsigned long timeout )
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_process = CdmwPlatformMngt::Process::_duplicate( process );
    m_timeout = timeout;
}

ORBACUSProcessStepCall::~ORBACUSProcessStepCall()
{
}

BoundSyncCallReturn
ORBACUSProcessStepCall::execute()
throw()
{
    BoundSyncCallReturn result = SUCCEED;
    
    try
    {
        // Set the timeout policy
        CORBA::Object_var capsule = SetTimeoutPolicy( m_orb, m_process, m_timeout );

        // Perform the process's action
        CdmwPlatformMngt::Process_var process = CdmwPlatformMngt::Process::_narrow( capsule );
        if( CORBA::is_nil( process ) )
            throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );
        process->next_step();
    }
    catch( const CORBA::NO_RESPONSE& )
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
// ORBACUSProcessRunCall class.
// ----------------------------------------------------------------------
ORBACUSProcessRunCall::ORBACUSProcessRunCall(
    CORBA::ORB_ptr orb,
    CdmwPlatformMngt::Process_ptr process,
    unsigned long timeout )
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_process = CdmwPlatformMngt::Process::_duplicate( process );
    m_timeout = timeout;
}

ORBACUSProcessRunCall::~ORBACUSProcessRunCall()
{
}

BoundSyncCallReturn
ORBACUSProcessRunCall::execute()
throw()
{
    BoundSyncCallReturn result = SUCCEED;
    
    try
    {
        // Set the timeout policy
        CORBA::Object_var capsule = SetTimeoutPolicy( m_orb, m_process, m_timeout );

        // Perform the process's action
        CdmwPlatformMngt::Process_var process = CdmwPlatformMngt::Process::_narrow( capsule );
        if( CORBA::is_nil( process ) )
            throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );
        process->run();
    }
    catch( const CORBA::NO_RESPONSE& )
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
// ORBACUSProcessStopCall class.
// ----------------------------------------------------------------------
ORBACUSProcessStopCall::ORBACUSProcessStopCall(
    CORBA::ORB_ptr orb,
    CdmwPlatformMngt::Process_ptr process,
    unsigned long timeout )
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_process = CdmwPlatformMngt::Process::_duplicate( process );
    m_timeout = timeout;
}

ORBACUSProcessStopCall::~ORBACUSProcessStopCall()
{
}

BoundSyncCallReturn
ORBACUSProcessStopCall::execute()
throw()
{
    BoundSyncCallReturn result = SUCCEED;
    
    try
    {
        // Set the timeout policy
        CORBA::Object_var capsule = SetTimeoutPolicy( m_orb, m_process, m_timeout );

        // Perform the process's action
        CdmwPlatformMngt::Process_var process = CdmwPlatformMngt::Process::_narrow( capsule );
        if( CORBA::is_nil( process ) )
            throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );
        process->stop();
    }
    catch( const CORBA::NO_RESPONSE& )
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
// ORBACUSEventNotificationCall class.
// ----------------------------------------------------------------------
ORBACUSEventNotificationCall::ORBACUSEventNotificationCall(
    CORBA::ORB_ptr orb,
    CdmwPlatformMngt::SupervisionObserver_ptr observer,
    CdmwPlatformMngt::Event* event,
    unsigned long timeout )
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_observer = CdmwPlatformMngt::SupervisionObserver::_duplicate( observer );
    m_event = CdmwPlatformMngt::Event::_downcast( event->_copy_value() );
    m_timeout = timeout;
}

ORBACUSEventNotificationCall::~ORBACUSEventNotificationCall()
{
}

BoundSyncCallReturn
ORBACUSEventNotificationCall::execute()
throw()
{
    BoundSyncCallReturn result = SUCCEED;
    
    try
    {
        // Set the timeout policy
        CORBA::Object_var capsule = SetTimeoutPolicy( m_orb, m_observer, m_timeout );

        // Perform the process's action
        CdmwPlatformMngt::SupervisionObserver_var observer = CdmwPlatformMngt::SupervisionObserver::_narrow( capsule );
        if( CORBA::is_nil( observer ) )
            throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );
        observer->notify( m_event );
    }
    catch( const CORBA::NO_RESPONSE& )
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
// ORBACUSPullMonitoringCall class.
// ----------------------------------------------------------------------
ORBACUSPullMonitoringCall::ORBACUSPullMonitoringCall(
    CORBA::ORB_ptr orb,
    CdmwPlatformMngt::PullMonitorable_ptr monitorable,
    unsigned long timeout )
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_monitorable = CdmwPlatformMngt::PullMonitorable::_duplicate( monitorable );
    m_timeout = timeout;
}

ORBACUSPullMonitoringCall::~ORBACUSPullMonitoringCall()
{
}

BoundSyncCallReturn
ORBACUSPullMonitoringCall::execute()
throw()
{
    BoundSyncCallReturn result = SUCCEED;
    
    try
    {
        // Set the timeout policy
        CORBA::Object_var capsule = SetTimeoutPolicy( m_orb, m_monitorable, m_timeout );

        // Perform the monitorable's action
        CdmwPlatformMngt::PullMonitorable_var monitorable = CdmwPlatformMngt::PullMonitorable::_narrow( capsule );
        if( CORBA::is_nil( monitorable ) )
            throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );
        monitorable->is_alive();
    }
    catch( const CORBA::NO_RESPONSE& )
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
// ORBACUSPlatformInfoNotificationCall class.
// ----------------------------------------------------------------------
ORBACUSPlatformInfoNotificationCall::ORBACUSPlatformInfoNotificationCall(
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

ORBACUSPlatformInfoNotificationCall::~ORBACUSPlatformInfoNotificationCall()
{
}

BoundSyncCallReturn
ORBACUSPlatformInfoNotificationCall::execute()
throw()
{
    BoundSyncCallReturn result = SUCCEED;
    
    try
    {
        // Set the timeout policy
        CORBA::Object_var capsule = SetTimeoutPolicy( m_orb, m_observer, m_timeout );

        // Perform the platform observer's action
        CdmwPlatformMngt::PlatformObserver_var observer = CdmwPlatformMngt::PlatformObserver::_narrow( capsule );
        if( CORBA::is_nil( observer ) )
            throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );
        observer->notify( m_platformInfo );
    }
    catch( const CORBA::NO_RESPONSE& )
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

#endif //CDMW_ORB_VDR == orbacus

