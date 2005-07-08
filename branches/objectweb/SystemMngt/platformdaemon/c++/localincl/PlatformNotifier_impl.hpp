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


#ifndef INCL_PLATFORMMNGT_PLATFORMNOTIFIER_IMPL_HPP 
#define INCL_PLATFORMMNGT_PLATFORMNOTIFIER_IMPL_HPP 

#include "Foundation/orbsupport/CORBA.hpp"

#include "idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "idllib/CdmwPlatformMngtPlatformObserver.stub.hpp"
#include "idllib/CdmwPlatformMngtDaemon.skel.hpp"

#include "platformdaemon/PlatformNotifier.hpp"

namespace Cdmw
{
namespace PlatformMngt
{


class Daemon_impl;


/**
* Purpose:
* <p>
* implements the
* IDL:thalesgroup.com/CdmwPlatformMngt/PlatformNotifier:1.0
* interface
*/
class PlatformNotifier_impl : virtual public POA_CdmwPlatformMngt::PlatformNotifier,
    virtual public PortableServer::RefCountServantBase
{

public:

    /**
    * Purpose:
    * <p> Constructor.
    */ 
    PlatformNotifier_impl(Daemon_impl *daemon, unsigned long callTimeout)
        throw (InternalErrorException);

    /**
    * Purpose:
    * <p> Adds the event for notification.
    */ 
    void notify(CdmwPlatformMngt::PlatformInfo* event)
        throw (OutOfMemoryException, InternalErrorException);

    /**
    * Purpose:
    * <p> Stops the notifier.
    */ 
    void stop() throw ();

    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/PlatformNotifier/register:1.0
    * operation
    */
    virtual CdmwPlatformMngt::PlatformObserver_ptr _cxx_register(const char* observer_name,
            CdmwPlatformMngt::PlatformObserver_ptr platform_observer,
            CdmwPlatformMngt::PlatformSnapshot_out platform_snapshot)
        throw(CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/PlatformNotifier/unregister:1.0
    * operation
    */
    virtual CdmwPlatformMngt::PlatformObserver_ptr unregister(const char* observer_name)
        throw(CdmwPlatformMngt::ObserverNotFound, CORBA::SystemException);

private:

    /**
     *Purpose:
     *<p> The daemon owning this platform notifier.
     */
    Daemon_impl *m_daemon;

    /**
     *Purpose:
     *<p> The actual platform notifier.
     */
    Cdmw::PlatformMngt::PlatformNotifier m_actualNotifier;

};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_PLATFORMNOTIFIER_IMPL_HPP

