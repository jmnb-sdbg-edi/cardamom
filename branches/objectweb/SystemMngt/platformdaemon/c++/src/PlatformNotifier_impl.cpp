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


#include "platformdaemon/PlatformNotifier_impl.hpp"
#include "platformdaemon/Daemon_impl.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/common/Assert.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


//
// IDL:thalesgroup.com/CdmwPlatformMngt/PlatformNotifier:1.0
//

PlatformNotifier_impl::PlatformNotifier_impl(
        Daemon_impl *daemon, unsigned long callTimeout)
    throw (InternalErrorException) : m_daemon(daemon), m_actualNotifier(callTimeout)
{
    CDMW_ASSERT(m_daemon != NULL);
    m_actualNotifier.start();
}


void PlatformNotifier_impl::notify(CdmwPlatformMngt::PlatformInfo* event)
    throw (OutOfMemoryException, InternalErrorException)
{

    try
    {
        m_actualNotifier.addEvent(event);
    }
    catch(const OutOfMemoryException&)
    {
        throw;
    }
    catch(...)
    {
        CDMW_THROW(InternalErrorException);
    }

}


void PlatformNotifier_impl::stop()
    throw ()
{

    try
    {
        m_actualNotifier.stop();
        m_actualNotifier.join();
    }
    catch(...)
    {
    }

}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/PlatformNotifier/register:1.0
//
CdmwPlatformMngt::PlatformObserver_ptr PlatformNotifier_impl::_cxx_register(const char* observer_name,
        CdmwPlatformMngt::PlatformObserver_ptr platform_observer,
        CdmwPlatformMngt::PlatformSnapshot_out platform_snapshot)
    throw(CORBA::SystemException)
{

    try
    {

        CdmwPlatformMngt::PlatformSnapshot_var snapshot =
            m_daemon->createSnapshot();

        CdmwPlatformMngt::PlatformObserver_var prevObserver =
            m_actualNotifier.registerObserver(observer_name, platform_observer);

        platform_snapshot = snapshot._retn();
        return prevObserver._retn();

    }
    catch(const BadParameterException&)
    {
        throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAM, CORBA::COMPLETED_NO);
    }
    catch(const OutOfMemoryException&)
    {
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch(const EventNotifierStoppedException&)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }

}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/PlatformNotifier/unregister:1.0
//
CdmwPlatformMngt::PlatformObserver_ptr PlatformNotifier_impl::unregister(const char* observer_name)
    throw(CdmwPlatformMngt::ObserverNotFound, CORBA::SystemException)
{
    try
    {
        CdmwPlatformMngt::PlatformObserver_var observer =
            m_actualNotifier.unregisterObserver(observer_name);

        if (CORBA::is_nil(observer.in()))
            throw CdmwPlatformMngt::ObserverNotFound();

        return observer._retn();

    }
    catch(const EventNotifierStoppedException&)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}


} // End namespace PlatformMngt
} // End namespace Cdmw


