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


#include "SystemMngt/platformapplicationlibrary/ApplicationControl_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

// ----------------------------------------------------------------------
// ApplicationControl_impl class.
// ----------------------------------------------------------------------
ApplicationControl_impl::ApplicationControl_impl(
    PortableServer::POA_ptr poa,
    Application_impl* application )
: DeactivableServant_impl( poa )
{
    m_application = application;
    
    // take owner ship of servant (increment reference counter)
    // ServantBase_var will remove reference autoamtically
    // in ApplicationControl_impl destructor
    application->_add_ref();
    m_applicationServant = application;
}

void ApplicationControl_impl::update_application(
    const char* unavailable_host_name )
throw( CORBA::SystemException )
{
    try
    {
        CdmwPlatformMngtBase::EventHeader header
            = EventHeaderFactory::createHeader (CdmwPlatformMngtBase::ERR);

        std::auto_ptr <InvalidateProcessEvent> event(
            new InvalidateProcessEvent(
                m_application,
                unavailable_host_name,
                header.time_stamp));

        m_application->get_event_handler()->addEvent (event.get());
        event.release();
    }
    catch(const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch(...)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}

void ApplicationControl_impl::destroy_application()
throw( CORBA::SystemException )
{
    m_application->removeProcesses();
    m_application->deactivate();    
    deactivate();
}

CdmwPlatformMngt::ProcessObserver_ptr ApplicationControl_impl::get_process_observer()
            throw(CORBA::SystemException)
{
	return m_application->get_process_observer();
}

CdmwPlatformMngt::ApplicationStatus ApplicationControl_impl::get_internal_status()
throw( CORBA::SystemException )
{
    return m_application->get_internal_status();
}
        
ProcessObserver_impl* ApplicationControl_impl::get_process_observer_servant()
{
    return m_application->get_process_observer_servant();
}


void ApplicationControl_impl::synchro_event (
    const CdmwPlatformMngt::EventReportRecord& event_record)
{
    try
    {
        if (event_record.event_type == CdmwPlatformMngt::INVALIDATE_PROCESS_EVENT)
        {
            std::auto_ptr <InvalidateProcessEvent> event(
            new InvalidateProcessEvent(
                m_application, event_record));

            // execute the event now
            event->execute();
        }
    }
    catch(const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch(...)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}


// ----------------------------------------------------------------------
// InvalidateProcessEvent class.
// ----------------------------------------------------------------------
InvalidateProcessEvent::InvalidateProcessEvent(
    Application_impl* application,
    const char* unavailable_host_name,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp)
:Event()
{
    m_application = application;
    
    m_record.event_type = CdmwPlatformMngt::INVALIDATE_PROCESS_EVENT;
    m_record.header = EventHeaderFactory::createHeader(time_stamp, 
                                                       CdmwPlatformMngtBase::ERR);
    m_record.application_name = m_application->name();
    m_record.host_name = CORBA::string_dup (unavailable_host_name);
}

InvalidateProcessEvent::InvalidateProcessEvent(
    Application_impl* application,
    const CdmwPlatformMngt::EventReportRecord& event_record)
:Event()
{
    m_application = application;
    
    m_record.event_type = CdmwPlatformMngt::INVALIDATE_PROCESS_EVENT;
    m_record.header = event_record.header;
    m_record.application_name = event_record.application_name;
    m_record.host_name = event_record.host_name;
}


void InvalidateProcessEvent::execute()
{
    m_application->invalidateProcesses(
        m_record.header,
        m_record.host_name.in(),
        Application_impl::HOST_UNAVAILABLE);
}

} // End namespace PlatformMngt
} // End namespace Cdmw

