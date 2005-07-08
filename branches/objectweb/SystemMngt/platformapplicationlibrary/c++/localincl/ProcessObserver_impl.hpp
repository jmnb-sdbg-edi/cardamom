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


#ifndef INCL_PLATFORMMNGT_PROCESS_OBSERVER_IMPL_HPP
#define INCL_PLATFORMMNGT_PROCESS_OBSERVER_IMPL_HPP

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtApplication.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp"
#include "SystemMngt/platformlibrary/DeactivableServant_impl.hpp"
#include "SystemMngt/platformlibrary/EventHandler.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class Application_impl;
class ProcessProxy_impl;
class ManagedProcessProxy_impl;

/**
 *Purpose:
 *<p> Provides the event notifications to the processes of an application.
 */
class ProcessObserver_impl : public virtual DeactivableServant_impl,
                             public virtual POA_CdmwPlatformMngt::ProcessObserver,
                             public virtual PortableServer::RefCountServantBase
{
private:
    /**
     * The application reponsible for the process.
     */
    Application_impl* m_application;

private:

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    ProcessObserver_impl(
        ProcessObserver_impl& rhs );

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    ProcessObserver_impl& operator=(
        const ProcessObserver_impl& rhs );

public:
    /**
     *Purpose:
     *<p> Constructor.
     */
    ProcessObserver_impl(
        PortableServer::POA_ptr poa,
        Application_impl* application );

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/notify_message:1.0
     * operator
     */
    virtual
    void notify_message(
        const char* process_name,
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* issuer,
        const char* message )
            throw( CdmwPlatformMngt::ProcessNotFound ,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/notify_error:1.0
     * operator
     */
    void notify_error(
        const char* process_name,
        const CdmwPlatformMngtBase::TimeStamp& time_stamp,
        const char* issuer,
        const char* error_info )
            throw( CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/notify_ending:1.0
     * operator
     */
    void notify_ending(
        const char* process_name,
        const CdmwPlatformMngtBase::TimeStamp& time_stamp )
            throw( CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/notify_monitoring_failure:1.0
     * operator
     */
    void notify_monitoring_failure(
        const char* process_name,
        const CdmwPlatformMngtBase::TimeStamp& time_stamp )
            throw( CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/notify_point_monitoring_failure:1.0
     * operator
     */
    void notify_point_monitoring_failure(
        const char* process_name,
        CORBA::ULong point_index,
        const CdmwPlatformMngtBase::TimeStamp& time_stamp )
            throw( CdmwPlatformMngt::ProcessNotFound,
                   CdmwPlatformMngt::ActivityPointNotFound,
                   CORBA::SystemException );

    void notify_user_event(
        const char* process_name,
        const char* issuer,
        const CORBA::Any& data )
            throw( CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException );
};

/**
 *Purpose:
 *<p> Provides the notification of a message event to a process. 
 */
class ProcessMessageEvent : public virtual Event
{
private:
    /**
     * The process proxy responsible for the process.
     */
    ProcessProxy_impl* m_processProxy;

    /**
     * The time and the date of the message issuance.
     */
    CdmwPlatformMngtBase::EventHeader m_header;

    /**
     * The issuer of the message.
     */
    std::string m_issuer;

    /**
     * The message to be notified.
     */
    std::string m_message;

private:
    /**
     * Purpose:
     * <p> Copy constructor is not allowed.
     */ 
    ProcessMessageEvent( const ProcessMessageEvent& rhs );

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    ProcessMessageEvent& operator =( const ProcessMessageEvent& rhs );

public:
    /**
     *Purpose:
     *<p> Constructor.
     */
    ProcessMessageEvent(
        ProcessProxy_impl* processProxy,
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* issuer,
        const char* message );

    /**
     *Purpose:
     *<p> Process mesage event's action.
     */
    void execute();
};

/**
 *Purpose:
 *<p> Provides the notification of an error event to a process. 
 */
class ProcessErrorEvent : public virtual Event
{
private:
    /**
     * The process proxy responsible for the process.
     */
    ProcessProxy_impl* m_processProxy;

    /**
     * The time and the date of the error issuance.
     */
    CdmwPlatformMngtBase::EventHeader m_header;

    /**
     * The issuer of the message.
     */
    std::string m_issuer;

    /**
     * The description of the error to be notified.
     */
    std::string m_errorInfo;

private:
    /**
     * Purpose:
     * <p> Copy constructor is not allowed.
     */ 
    ProcessErrorEvent( const ProcessErrorEvent& rhs );

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    ProcessErrorEvent& operator =( const ProcessErrorEvent& rhs );

public:
    /**
     *Purpose:
     *<p> Constructor.
     */
    ProcessErrorEvent(
        ProcessProxy_impl* processProxy,
        const CdmwPlatformMngtBase::TimeStamp& time_stamp,
        const char* issuer,
        const char* error_info );

    /**
     *Purpose:
     *<p> Process error event's action.
     */
    void execute();
};

/**
 *Purpose:
 *<p> Provides the notification of an ending event to a process. 
 */
class ProcessEndingEvent : public virtual Event
{
private:
    /**
     * The process proxy responsible for the process.
     */
    ProcessProxy_impl* m_processProxy;

    /**
     * The time and the date of the ending event issuance.
     */
    CdmwPlatformMngtBase::EventHeader m_header;

private:
    /**
     * Purpose:
     * <p> Copy constructor is not allowed.
     */ 
    ProcessEndingEvent( const ProcessEndingEvent& rhs );

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    ProcessEndingEvent& operator =( const ProcessEndingEvent& rhs );

public:
    /**
     *Purpose:
     *<p> Constructor.
     */
    ProcessEndingEvent(
        ProcessProxy_impl* processProxy,
        const CdmwPlatformMngtBase::TimeStamp& time_stamp );

    /**
     *Purpose:
     *<p> Process ending event's action.
     */
    void execute();
};

/**
 *Purpose:
 *<p> Provides the notification of a monitoring failure event to a process.
 */
class ProcessMonitoringFailure : public virtual Event
{
private:
    /**
     * The process proxy responsible for the process.
     */
    ManagedProcessProxy_impl* m_managedProcessProxy;

    /**
     * The time and the date of the monitoring failure event issuance.
     */
    CdmwPlatformMngtBase::EventHeader m_header;

private:
    /**
     * Purpose:
     * <p> Copy constructor is not allowed.
     */ 
    ProcessMonitoringFailure( const ProcessMonitoringFailure& rhs );

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    ProcessMonitoringFailure& operator =( const ProcessMonitoringFailure& rhs );

public:
    /**
     *Purpose:
     *<p> Constructor.
     */
    ProcessMonitoringFailure(
        ManagedProcessProxy_impl* managedProcessProxy,
        const CdmwPlatformMngtBase::TimeStamp& time_stamp );

    /**
     *Purpose:
     *<p> Process monitoring failure's action.
     */
    void execute();
};

/**
 *Purpose:
 *<p> Provides the notification of a monitoring point failure event to a process.
 */
class PointMonitoringFailure : public virtual Event
{
private:
    /**
     * The process proxy responsible for the process.
     */
    ManagedProcessProxy_impl* m_managedProcessProxy;

    /**
     * The time and the date of the point monitoring failure event issuance.
     */
    CdmwPlatformMngtBase::EventHeader m_header;

    /**
     * The index of the point monitoring.
     */
    CORBA::ULong m_point_index;

private:
    /**
     * Purpose:
     * <p> Copy constructor is not allowed.
     */ 
    PointMonitoringFailure( const PointMonitoringFailure& rhs );

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    PointMonitoringFailure& operator =( const PointMonitoringFailure& rhs );

public:
    /**
     *Purpose:
     *<p> Constructor.
     */
    PointMonitoringFailure(
        ManagedProcessProxy_impl* managedProcessProxy,
        CORBA::ULong point_index,
        const CdmwPlatformMngtBase::TimeStamp& time_stamp );

    /**
     *Purpose:
     *<p> Process monitoring failure's action.
     */
    void execute();
};

/**
 *Purpose:
 *<p> Provides the notification of a user event to a process. 
 */
class ProcessUserEvent : public virtual Event
{
public:
    /**
     *Purpose:
     *<p> Constructor.
     */
    ProcessUserEvent(
        ProcessProxy_impl* processProxy,
        const char* issuer,
        const CORBA::Any& data );

    /**
     *Purpose:
     *<p> Process error event's action.
     */
    void execute();

private:
    /**
     * Purpose:
     * <p> Copy constructor is not allowed.
     */ 
    ProcessUserEvent( const ProcessUserEvent& rhs );

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    ProcessUserEvent& operator =( const ProcessUserEvent& rhs );

    /**
     * The process proxy responsible for the process.
     */
    ProcessProxy_impl* m_processProxy;

    /**
     * The data of the user event.
     */
    CdmwPlatformMngt::UserEvent_var m_event;
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_PROCESS_OBSERVER_IMPL_HPP

