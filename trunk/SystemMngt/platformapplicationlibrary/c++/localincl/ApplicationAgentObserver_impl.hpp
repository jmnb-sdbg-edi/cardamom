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


#ifndef INCL_PLATFORMMNGT_APPLICATION_CONTROL_IMPL_HPP
#define INCL_PLATFORMMNGT_APPLICATION_CONTROL_IMPL_HPP

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtMonitoringObserver.skel.hpp"
#include "SystemMngt/platformlibrary/DeactivableServant_impl.hpp"
#include "SystemMngt/platformlibrary/EventHandler.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class Application_impl;

/**
 *Purpose:
 *<p> Notifies an application to invalidate its processes.
 */
class ApplicationAgentObserver_impl : public virtual DeactivableServant_impl,
                                      public virtual POA_CdmwPlatformMngt::MonitorableProcessObserver,
                                      public virtual PortableServer::RefCountServantBase
{
private:
    /**
     * The application reponsible for the processes.
     */
    Application_impl* m_application;

private:

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    ApplicationAgentObserver_impl(
        ApplicationAgentObserver_impl& rhs );

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    ApplicationAgentObserver_impl& operator=(
        const ApplicationAgentObserver_impl& rhs );

public:
    /**
     *Purpose:
     *<p> Constructor.
     */
    ApplicationAgentObserver_impl(
        PortableServer::POA_ptr poa,
        Application_impl* application );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgentObserver/notify_ending:1.0
     * operator
     */
    void notify_ending(
        const char* feature_name,
        const CdmwPlatformMngtBase::TimeStamp& time_stamp )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgentObserver/notify_monitoring_failure:1.0
     * operator
     */
    void notify_monitoring_failure(
        const char* feature_name,
        const CdmwPlatformMngtBase::TimeStamp& time_stamp )
            throw(CORBA::SystemException);
};

/**
 *Purpose:
 *<p> Provides the agent ending event to an application.
 */
class AgentEndingEvent : public virtual Event
{
private:
    /**
     * The application.
     */
    Application_impl* m_application;

    /**
     * The name of the unavailable host name.
     */
    std::string m_host_name;

    /**
     * The time and the date of the error issuance.
     */
    CdmwPlatformMngtBase::EventHeader m_header;

private:
    /**
     * Purpose:
     * <p> Copy constructor is not allowed.
     */ 
    AgentEndingEvent( const AgentEndingEvent& rhs );

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    AgentEndingEvent& operator =( const AgentEndingEvent& rhs );

public:
    /**
     *Purpose:
     *<p> Constructor.
     */
    AgentEndingEvent(
        Application_impl* application,
        const char* feature_name,
        const CdmwPlatformMngtBase::TimeStamp& time_stamp );

    /**
     *Purpose:
     *<p> Process event's action.
     */
    void execute();
};

/**
 *Purpose:
 *<p> Provides the agent monitoring failure event to an application.
 */
class AgentMonitoringFailureEvent : public virtual Event
{
private:
    /**
     * The application.
     */
    Application_impl* m_application;

    /**
     * The name of the unavailable host name.
     */
    std::string m_host_name;

    /**
     * The time and the date of the error issuance.
     */
    CdmwPlatformMngtBase::EventHeader m_header;

private:
    /**
     * Purpose:
     * <p> Copy constructor is not allowed.
     */ 
    AgentMonitoringFailureEvent( const AgentMonitoringFailureEvent& rhs );

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    AgentMonitoringFailureEvent& operator =( const AgentMonitoringFailureEvent& rhs );

public:
    /**
     *Purpose:
     *<p> Constructor.
     */
    AgentMonitoringFailureEvent(
        Application_impl* application,
        const char* feature_name,
        const CdmwPlatformMngtBase::TimeStamp& time_stamp );

    /**
     *Purpose:
     *<p> Process event's action.
     */
    void execute();
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_APPLICATION_CONTROL_IMPL_HPP

