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
            = EventHeaderFactory::createHeader( CdmwPlatformMngtBase::ERR );

        std::auto_ptr< InvalidateProcessEvent > event(
            new InvalidateProcessEvent(
                m_application,
                unavailable_host_name,
                header.time_stamp ) );

        m_application->m_eventHandler.addEvent( event.get() );
        event.release();
    }
    catch( const std::bad_alloc& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
}

void ApplicationControl_impl::destroy_application()
throw( CORBA::SystemException )
{
    m_application->removeProcesses();
    m_application->deactivate();    
    deactivate();
}

// ----------------------------------------------------------------------
// InvalidateProcessEvent class.
// ----------------------------------------------------------------------
InvalidateProcessEvent::InvalidateProcessEvent(
    Application_impl* application,
    const char* unavailable_host_name,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp )
:Event()
{
    m_application = application;
    m_header.level = CdmwPlatformMngtBase::ERR;
    m_header.time_stamp = time_stamp;
    m_unavailable_host_name = unavailable_host_name;
}

void InvalidateProcessEvent::execute()
{
    m_application->invalidateProcesses(
        m_header,
        m_unavailable_host_name.c_str(),
        Application_impl::HOST_UNAVAILABLE );
}

} // End namespace PlatformMngt
} // End namespace Cdmw

