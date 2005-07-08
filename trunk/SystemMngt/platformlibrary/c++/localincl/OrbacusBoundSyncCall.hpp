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


#ifndef INCL_PLATFORMMNGT_ORBACUS_BOUND_SYNC_CALL_HPP
#define INCL_PLATFORMMNGT_ORBACUS_BOUND_SYNC_CALL_HPP

#include "SystemMngt/platformlibrary/BoundSyncCall.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

/**
 *Purpose:
 *<p> Synchronous method invocation with timeout factory.
 *
 *Features:
 *<p> 
 */        
class ORBACUSBoundSyncCallFactory : virtual public BoundSyncCallFactory
{
private:
    CORBA::ORB_var m_orb;

public:
    /**
     *Purpose:
     *<p> Constructor
     *
     *@param orb An ORB ptr. The ORB to be used.
     *
     *@return An ORBACUS BoundSyncCall factory.
     */
    ORBACUSBoundSyncCallFactory( CORBA::ORB_ptr orb );

    /**
     *Purpose:
     *<p> Default destructor.
     */
     virtual
     ~ORBACUSBoundSyncCallFactory();
     
public:
    /**
     *Purpose:
     *<p> Concrete method for obtaining a bound synchronous method call. The returned
     * object provides method for the specified process to perform its initialisation 
     * procedure with the given timeout.
     *
     *@param process The recipient process of the synchronous call with timeout.
     *@param startupKind The startup kind.
     *@param timeout The time expiration.
     *
     *@return a bound synchronous call with timeout object.
     *
     *@exception OutOfMemoryException Lack of memory.
     */
    ProcessInitCall* createProcessInitCall(
        CdmwPlatformMngt::Process_ptr process,
        const CdmwPlatformMngtBase::StartupKind& startupKind,
        unsigned long timeout )
    throw( OutOfMemoryException );

    /**
     *Purpose:
     *<p> Concrete method for obtaining a bound synchronous method call. The returned
     * object provides method for the specified process to perform its next initialisation
     * step with the given timeout.
     *
     *@param process The recipient process of the synchronous call with timeout.
     *@param timeout The time expiration.
     *
     *@return a bound synchronous call with timeout object.
     *
     *@exception OutOfMemoryException Lack of memory.
     */
    ProcessStepCall* createProcessStepCall(
        CdmwPlatformMngt::Process_ptr process,
        unsigned long timeout )
    throw( OutOfMemoryException );

    /**
     *Purpose:
     *<p> Concrete method for obtaining a bound synchronous method call. The returned
     * object provides method for the specified process to perform its run method
     * with the given timeout.
     *
     *@param process The recipient process of the synchronous call with timeout.
     *@param timeout The time expiration.
     *
     *@return a bound synchronous call with timeout object.
     *
     *@exception OutOfMemoryException Lack of memory.
     */
    ProcessRunCall* createProcessRunCall(
        CdmwPlatformMngt::Process_ptr process,
        unsigned long timeout )
    throw( OutOfMemoryException );

    /**
     *Purpose:
     *<p> Concrete method for obtaining a bound synchronous method call. The returned
     * object provides method for the specified proces to perform its stop procedure
     * with the given timeout.
     *
     *@param process The recipient process of the synchronous call with timeout.
     *@param timeout The time expiration.
     *
     *@return a bound synchronous call with timeout object.
     *
     *@exception OutOfMemoryException Lack of memory.
     */
    ProcessStopCall* createProcessStopCall(
        CdmwPlatformMngt::Process_ptr process,
        unsigned long timeout )
    throw( OutOfMemoryException );

    /**
     *Purpose:
     *<p> Concrete method for obtaining a bound synchronous method call. The returned
     * object provides method for notifying (with the given timeout) the specified
     * observer that a specified event happened.
     *
     *@param observer The recipient observer of the notification.
     *@param event The event to be notified.
     *@param timeout The time expiration.
     *
     *@return a bound synchronous call with timeout object.
     *
     *@exception OutOfMemoryException Lack of memory.
     */
    EventNotificationCall* createEventNotificationCall(
        CdmwPlatformMngt::SupervisionObserver_ptr observer,
        CdmwPlatformMngt::Event* event,
        unsigned long timeout )
    throw( OutOfMemoryException );

    /**
     *Purpose:
     *<p> Concrete method for obtaining a bound synchronous method call. The returned
     * object provides method for notifying (with the given timeout) the supervision
     * that the specified monitorable object is still alive.
     *
     *@param monitorable The recipient monitorable of the synchronous call with timeout.
     *@param timeout The time expiration.
     *
     *@return a bound synchronous call with timeout object.
     *
     *@exception OutOfMemoryException Lack of memory.
     */
    PullMonitoringCall* createPullMonitoringCall(
        CdmwPlatformMngt::PullMonitorable_ptr monitorable,
        unsigned long timeout )
    throw( OutOfMemoryException );

    /**
     *Purpose:
     *<p> Concrete method for obtaining a bound synchronous method call. The returned
     * object provides method for notifying (with the given timeout) the specified
     * observer of the specified platform information.
     *
     *@param observer The recipient observer of the synchronous call with timeout.
     *@param platformInfo The platform information to be notified.
     *@param timeout The time expiration.
     *
     *@return a bound synchronous call with timeout object.
     *
     *@exception OutOfMemoryException Lack of memory.
     */
    PlatformInfoNotificationCall* createPlatformInfoNotificationCall(
        CdmwPlatformMngt::PlatformObserver_ptr observer,
        const CdmwPlatformMngt::PlatformInfo& platformInfo,
        unsigned long timeout )
    throw( OutOfMemoryException );
};

/**
 *Purpose:
 *<p> Synchronous process initialisation with timeout.
 *
 *Features:
 *<p> 
 */        
class ORBACUSProcessInitCall : virtual public ProcessInitCall
{
private:
    CORBA::ORB_var m_orb;

public:
    /**
     *Purpose:
     *<p> Constructor
     *
     *@param orb An ORB ptr. The ORB to be used.
     *@param process The recipient process of the synchronous call with timeout.
     *@param startupKind The startup kind.
     *@param timeout The time expiration.
     *
     *@exception OutOfMemoryException Lack of memory.
     */
    ORBACUSProcessInitCall(
        CORBA::ORB_ptr orb,
        CdmwPlatformMngt::Process_ptr process,
        const CdmwPlatformMngtBase::StartupKind& startupKind,
        unsigned long timeout )
    throw( OutOfMemoryException );

    /**
     *Purpose:
     *<p> Default destructor.
     */
     virtual
     ~ORBACUSProcessInitCall();
     
    /**
     *Purpose:
     *<p> Performs the process's initialisation with the given timeout.
     *
     *@return This method returns the status of the invocation.
     */
    BoundSyncCallReturn execute()
    throw();
};

/**
 *Purpose:
 *<p> Synchronous process step initialisation with timeout.
 *
 *Features:
 *<p> 
 */        
class ORBACUSProcessStepCall : virtual public ProcessStepCall
{
private:
    CORBA::ORB_var m_orb;

public:
    /**
     *Purpose:
     *<p> Constructor
     *
     *@param orb An ORB ptr. The ORB to be used.
     *@param process The recipient process of the synchronous call with timeout.
     *@param timeout The time expiration.
     */
    ORBACUSProcessStepCall(
        CORBA::ORB_ptr orb,
        CdmwPlatformMngt::Process_ptr process,
        unsigned long timeout );

    /**
     *Purpose:
     *<p> Default destructor.
     */
     virtual
     ~ORBACUSProcessStepCall();
     
    /**
     *Purpose:
     *<p> Performs the process's next initialisation step with the given timeout.
     *
     *@return This method returns the status of the invocation.
     */
    BoundSyncCallReturn execute()
    throw();
};

/**
 *Purpose:
 *<p> Synchronous process running with timeout.
 *
 *Features:
 *<p> 
 */        
class ORBACUSProcessRunCall : virtual public ProcessRunCall
{
private:
    CORBA::ORB_var m_orb;

public:
    /**
     *Purpose:
     *<p> Constructor
     *
     *@param orb An ORB ptr. The ORB to be used.
     *@param process The recipient process of the synchronous call with timeout.
     *@param timeout The time expiration.
     */
    ORBACUSProcessRunCall(
        CORBA::ORB_ptr orb,
        CdmwPlatformMngt::Process_ptr process,
        unsigned long timeout );

    /**
     *Purpose:
     *<p> Default destructor.
     */
     virtual
     ~ORBACUSProcessRunCall();
     
    /**
     *Purpose:
     *<p> Performs the process's run method with the given timeout.
     *
     *@return This method returns the status of the invocation.
     */
    BoundSyncCallReturn execute()
    throw();
};

/**
 *Purpose:
 *<p> Synchronous process stop with timeout.
 *
 *Features:
 *<p> 
 */        
class ORBACUSProcessStopCall : virtual public ProcessStopCall
{
private:
    CORBA::ORB_var m_orb;

public:
    /**
     *Purpose:
     *<p> Constructor
     *
     *@param orb An ORB ptr. The ORB to be used.
     *@param process The recipient process of the synchronous call with timeout.
     *@param timeout The time expiration.
     */
    ORBACUSProcessStopCall(
        CORBA::ORB_ptr orb,
        CdmwPlatformMngt::Process_ptr process,
        unsigned long timeout );

    /**
     *Purpose:
     *<p> Default destructor.
     */
     virtual
     ~ORBACUSProcessStopCall();
     
    /**
     *Purpose:
     *<p> Performs the process's stop procedure with the given timeout.
     *
     *@return This method returns the status of the invocation.
     */
    BoundSyncCallReturn execute()
    throw();
};

/**
 *Purpose:
 *<p> Synchronous event notification with timeout.
 *
 *Features:
 *<p> 
 */        
class ORBACUSEventNotificationCall : virtual public EventNotificationCall
{
private:
    CORBA::ORB_var m_orb;

public:
    /**
     *Purpose:
     *<p> Constructor
     *
     *@param orb An ORB ptr. The ORB to be used.
     *@param observer The recipient observer of the notification.
     *@param event The event to be notified.
     *@param timeout The time expiration.
     */
    ORBACUSEventNotificationCall(
        CORBA::ORB_ptr orb,
        CdmwPlatformMngt::SupervisionObserver_ptr observer,
        CdmwPlatformMngt::Event* event,
        unsigned long timeout );
        
    /**
     *Purpose:
     *<p> Default destructor.
     */
     virtual
     ~ORBACUSEventNotificationCall();
     
    /**
     *Purpose:
     *<p> Notifies (with the given timout) the observer that an event happened.
     *
     *@return This method returns the status of the invocation.
     */
    BoundSyncCallReturn execute()
    throw();
};

/**
 *Purpose:
 *<p> Synchronous pull monitoring with timeout.
 *
 *Features:
 *<p> 
 */        
class ORBACUSPullMonitoringCall : virtual public PullMonitoringCall
{
private:
    CORBA::ORB_var m_orb;

public:
    /**
     *Purpose:
     *<p> Constructor
     *
     *@param orb An ORB ptr. The ORB to be used.
     *@param monitorable The recipient monitorable of the synchronous call with timeout.
     *@param timeout The time expiration.
     */
    ORBACUSPullMonitoringCall(
        CORBA::ORB_ptr orb,
        CdmwPlatformMngt::PullMonitorable_ptr monitorable,
        unsigned long timeout );

    /**
     *Purpose:
     *<p> Default destructor.
     */
     virtual
     ~ORBACUSPullMonitoringCall();
     
    /**
     *Purpose:
     *<p> Notifies the supervision that the monitorable is still alive.
     *
     *@return This method returns the status of the invocation.
     */
    BoundSyncCallReturn execute()
    throw();
};

/**
 *Purpose:
 *<p> Synchronous notification of platform information with timeout.
 *
 *Features:
 *<p> 
 */        
class ORBACUSPlatformInfoNotificationCall : virtual public PlatformInfoNotificationCall
{
private:
    CORBA::ORB_var m_orb;

public:
    /**
     *Purpose:
     *<p> Constructor
     *
     *@param orb An ORB ptr. The ORB to be used.
     *@param observer The recipient observer of the synchronous call with timeout.
     *@param platformInfo The platform information to be notified.
     *@param timeout The time expiration.
     *
     *@exception OutOfMemoryException Lack of memory.
     */
    ORBACUSPlatformInfoNotificationCall(
        CORBA::ORB_ptr orb,
        CdmwPlatformMngt::PlatformObserver_ptr observer,
        const CdmwPlatformMngt::PlatformInfo& platformInfo,
        unsigned long timeout )
    throw( OutOfMemoryException );

    /**
     *Purpose:
     *<p> Default destructor.
     */
     virtual
     ~ORBACUSPlatformInfoNotificationCall();
     
    /**
     *Purpose:
     *<p> Notifies the platform observer of the platform information
     * in the given timeout.
     *
     *@return This method returns the status of the invocation.
     */
    BoundSyncCallReturn execute()
    throw();
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_ORBACUS_BOUND_SYNC_CALL_HPP

