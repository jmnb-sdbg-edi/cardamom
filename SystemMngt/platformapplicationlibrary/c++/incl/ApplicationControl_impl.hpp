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


#ifndef INCL_PLATFORMMNGT_APPLICATION_CONTROL_IMPL_HPP
#define INCL_PLATFORMMNGT_APPLICATION_CONTROL_IMPL_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtApplication.skel.hpp"
#include "SystemMngt/platformlibrary/DeactivableServant_impl.hpp"
#include "SystemMngt/platformlibrary/EventHandler.hpp"
#include "SystemMngt/platformlibrary/DataStoreBaseDefinition.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class Application_impl;
class ProcessObserver_impl;

/**
 *Purpose:
 *<p> Notifies an application to invalidate its processes.
 */
class ApplicationControl_impl : public virtual DeactivableServant_impl,
                                public virtual POA_CdmwPlatformMngt::ApplicationControl,
                                public virtual PortableServer::RefCountServantBase
{
private:

    /**
     * The application reponsible for the processes.
     */
    Application_impl* m_application;
    
    /**
     * The application object var to take pointer ownership
     */
    PortableServer::ServantBase_var m_applicationServant;

private:

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    ApplicationControl_impl(
        ApplicationControl_impl& rhs );

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    ApplicationControl_impl& operator=(
        const ApplicationControl_impl& rhs );

public:
    /**
     *Purpose:
     *<p> Constructor.
     */
    ApplicationControl_impl(
        PortableServer::POA_ptr poa,
        Application_impl* application );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationControl/update_application:1.0
     * operator
     */
    void update_application(
        const char* unavailable_host_name )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationControl/destroy_application:1.0
     * operator
     */
    void destroy_application()
        throw(CORBA::SystemException);
        
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/get_process_observer:1.0
     * operation
     */
    CdmwPlatformMngt::ProcessObserver_ptr get_process_observer()
            throw(CORBA::SystemException);
            
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Application/get_internal_status:1.0
     * operation
     */
    CdmwPlatformMngt::ApplicationStatus get_internal_status()
            throw(CORBA::SystemException);
            
            
    /**
     *Purpose:
     *<p> Get the process observer servant.
     *
     *@return the process observer servant.
     *
     */        
    ProcessObserver_impl* get_process_observer_servant();
            
    /**
     *Purpose:
     *<p> Synchronise the event from the datastore.
     *
     *@param event_record the event extracted from datastore
     *
     */
    void synchro_event (const CdmwPlatformMngt::EventReportRecord& event_record);
            
};

/**
 *Purpose:
 *<p> Provides the host unavailibility error event to an application.
 */
class InvalidateProcessEvent : public virtual Event
{
private:
    /**
     * The application.
     */
    Application_impl* m_application;

private:
    /**
     * Purpose:
     * <p> Copy constructor is not allowed.
     */ 
    InvalidateProcessEvent( const InvalidateProcessEvent& rhs );

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    InvalidateProcessEvent& operator =( const InvalidateProcessEvent& rhs );

public:

    /**
     *Purpose:
     *<p> Constructor.
     */
    InvalidateProcessEvent(
        Application_impl* application,
        const char* unavailable_host_name,
        const CdmwPlatformMngtBase::TimeStamp& time_stamp);
        
    /**
     *Purpose:
     *<p> Constructor.
     */
    InvalidateProcessEvent(
        Application_impl* application,
        const CdmwPlatformMngt::EventReportRecord& event_record);

    /**
     *Purpose:
     *<p> Process event's action.
     */
    void execute();
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_APPLICATION_CONTROL_IMPL_HPP

