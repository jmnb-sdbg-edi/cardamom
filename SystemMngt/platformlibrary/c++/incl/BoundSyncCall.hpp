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


#ifndef INCL_PLATFORMMNGT_BOUND_SYNC_CALL_HPP 
#define INCL_PLATFORMMNGT_BOUND_SYNC_CALL_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtMonitoring.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessDelegate.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtSupervisionObserver.stub.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class BoundSyncCall;
class ProcessInitCall;
class ProcessStepCall;
class ProcessRunCall;
class ProcessStopCall;
class EventNotificationCall;
class PullMonitoringCall;
class PlatformInfoNotificationCall;


/**
 *
 */
enum BoundSyncCallReturn
{
    SUCCEED,
    TIMEOUT,
    EXCEPTION
};

/**
 *Purpose:
 *<p> Wrapper layer factory for synchronous method invocation with timeout.
 *
 *Features:
 *<p> Singleton.
 */        
class BoundSyncCallFactory
{
protected:
    static CORBA::ORB_ptr M_orb;
    static BoundSyncCallFactory* M_factory;

protected:
    /**
     *Purpose:
     *<p> Default constructor.
     */
     BoundSyncCallFactory();

    /**
     *Purpose:
     *<p> Default destructor.
     */
     virtual
     ~BoundSyncCallFactory();
     
public:
    /**
     *Purpose:
     *<p> .
     *
     *@param orb An ORB ptr. The ORB to be used.
     */
    static
    void initialise( CORBA::ORB_ptr orb )
    throw( BadParameterException );

    /**
     *Purpose:
     *<p> .
     */
    static
    void destroy();
     
    /**
     *Purpose:
     *<p> This static method returns an instance of a bound synchronous method call factory.
     *
     *@return A concrete BoundSyncCall factory.
     *
     *@exception OutOfMemoryException Lack of memory.
     */
    static
    BoundSyncCallFactory* getFactory()
    throw(OutOfMemoryException);

    /**
     *Purpose:
     *<p> Abstract method for obtaining a bound synchronous method call. The returned
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
    virtual
    ProcessInitCall* createProcessInitCall(
        CdmwPlatformMngt::ProcessDelegate_ptr process,
        const CdmwPlatformMngtBase::StartupKind& startupKind,
        unsigned long timeout )
    throw( OutOfMemoryException ) = 0;

    /**
     *Purpose:
     *<p> Abstract method for obtaining a bound synchronous method call. The returned
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
    virtual
    ProcessStepCall* createProcessStepCall(
        CdmwPlatformMngt::ProcessDelegate_ptr process,
        unsigned long timeout )
    throw( OutOfMemoryException ) = 0;

    /**
     *Purpose:
     *<p> Abstract method for obtaining a bound synchronous method call. The returned
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
    virtual
    ProcessRunCall* createProcessRunCall(
        CdmwPlatformMngt::ProcessDelegate_ptr process,
        unsigned long timeout )
    throw( OutOfMemoryException ) = 0;

    /**
     *Purpose:
     *<p> Abstract method for obtaining a bound synchronous method call. The returned
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
    virtual
    ProcessStopCall* createProcessStopCall(
        CdmwPlatformMngt::ProcessDelegate_ptr process,
        unsigned long timeout )
    throw( OutOfMemoryException ) = 0;

    /**
     *Purpose:
     *<p> Abstract method for obtaining a bound synchronous method call. The returned
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
    virtual
    EventNotificationCall* createEventNotificationCall(
        CdmwPlatformMngt::SupervisionObserver_ptr observer,
        CdmwPlatformMngt::Event* event,
        unsigned long timeout )
    throw( OutOfMemoryException ) = 0;

    /**
     *Purpose:
     *<p> Abstract method for obtaining a bound synchronous method call. The returned
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
    virtual
    PullMonitoringCall* createPullMonitoringCall(
        CdmwPlatformMngt::PullMonitorable_ptr monitorable,
        unsigned long timeout )
    throw( OutOfMemoryException ) = 0;
};

/**
 *Purpose:
 * <p> Abstract layer for synchronous method invocation with timeout.
 *
 *Features:
 *<p> 
 */        
class BoundSyncCall
{
protected:
    CdmwPlatformMngt::Timeout m_timeout;

    std::string m_exceptionRepId;
    std::string m_exceptionName;

protected:
    /**
     *Purpose:
     *<p> This method is called when an exception is raised by the execution
     * of the bound synchronous call.
     * It stores the necessary information about the exception.
     */
    void setExceptionInfo(
        const CORBA::Exception& exception ) throw();

public:
    /**
     *Purpose:
     *<p> Destructor
     */
    virtual
    ~BoundSyncCall();
    
    /**
     *Purpose:
     *<p> Abstract method for invocating the embedded object's method with
     * timeout.
     *
     *@return this method returns the status of the invocation.
     */
    virtual
    BoundSyncCallReturn execute()
    throw() = 0;

    /**
     *Purpose:
     *<p> Returns the repository Id of the exception raised by the
     * call execution.
     */
    const char* getExceptionRepId() throw();

    /**
     *Purpose:
     *<p> Returns the name of the exception raised by the call execution.
     */
    const char* getExceptionName() throw();
};

/**
 *Purpose:
 *<p> Abstract synchronous process initialisation with timeout.
 *
 *Features:
 *<p> 
 */        
class ProcessInitCall : virtual public BoundSyncCall
{
public:
    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual
    ~ProcessInitCall()
    {
    };
    
protected:
    CdmwPlatformMngt::ProcessDelegate_var m_process;
    CdmwPlatformMngtBase::StartupKind_var m_startupKind;
};

/**
 *Purpose:
 *<p> Abstract synchronous process initialisation step with timeout.
 *
 *Features:
 *<p> 
 */        
class ProcessStepCall : virtual public BoundSyncCall
{
public:
    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual
    ~ProcessStepCall()
    {
    };
    
protected:
    CdmwPlatformMngt::ProcessDelegate_var m_process;
};

/**
 *Purpose:
 *<p> Abstract synchronous process running with timeout.
 *
 *Features:
 *<p> 
 */        
class ProcessRunCall : virtual public BoundSyncCall
{
public:
    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual
    ~ProcessRunCall()
    {
    };
    
protected:
    CdmwPlatformMngt::ProcessDelegate_var m_process;
};

/**
 *Purpose:
 *<p> Abstract synchronous process stop with timeout.
 *
 *Features:
 *<p> 
 */        
class ProcessStopCall : virtual public BoundSyncCall
{
public:
    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual
    ~ProcessStopCall()
    {
    };
    
protected:
    CdmwPlatformMngt::ProcessDelegate_var m_process;
};


/**
 *Purpose:
 *<p> Abstract synchronous event notification with timeout.
 *
 *Features:
 *<p> 
 */        
class EventNotificationCall : virtual public BoundSyncCall
{
public:
    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual
    ~EventNotificationCall()
    {
    };
    
protected:
    CdmwPlatformMngt::SupervisionObserver_var m_observer;
    CdmwPlatformMngt::Event_var m_event;
};

/**
 *Purpose:
 *<p> Abstract synchronous monitoring with timeout.
 *
 *Features:
 *<p> 
 */        
class PullMonitoringCall : virtual public BoundSyncCall
{
public:
    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual
    ~PullMonitoringCall()
    {
    };
    
protected:
    CdmwPlatformMngt::PullMonitorable_var m_monitorable;
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_BOUND_SYNC_CALL_HPP

