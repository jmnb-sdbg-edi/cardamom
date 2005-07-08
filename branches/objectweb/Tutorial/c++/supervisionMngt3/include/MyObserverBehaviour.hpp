/* =========================================================================== *
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
 * =========================================================================== */

#ifndef __MYOBSERVERBEHAVIOUR_HPP__
#define __MYOBSERVERBEHAVIOUR_HPP__

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/common/Exception.hpp>

#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <SystemMngt/platforminterface/ProcessBehaviour.hpp>

#include <SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp>

#include "MyObserver_impl.hpp"

using namespace Cdmw::PlatformMngt;
using namespace Cdmw::OsSupport;


/**
 * Example of observer behaviour.
 */
class MyObserverBehaviour
: virtual public Cdmw::PlatformMngt::ProcessBehaviour
{

public:

    /**
     * The constructor.
     */ 
    MyObserverBehaviour(
        CdmwPlatformMngt::System_ptr system,
        const char* observer_name )
    {
        m_system = CdmwPlatformMngt::System::_duplicate( system );
        m_observer_name = CORBA::string_dup( observer_name );

        // Create a user event observer object
        MyObserver_impl *observer_i = new MyObserver_impl();
        m_observer_servant = observer_i;
        m_observer = observer_i->_this();

        // Subscribe the observer to the CARDAMOM supervision
        m_system->register_observer( m_observer_name.in(), m_observer.in() );
    }


    /**
     * The destructor.
     */ 
    virtual
    ~MyObserverBehaviour()
    throw()
    {
    }


    /**
     * Never invoked if the observer is an unmanaged process.
     *
     * IDL:thalesgroup.com/CdmwSystemMngt/platformlibrary/Process/initialise:1.0
     */
    virtual
    void initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
    throw( CdmwPlatformMngt::Process::BadOrder,
           CORBA::SystemException )
    {
        PlatformInterface::notifyMessage(
            CdmwPlatformMngtBase::INF,
            "USER OBSERVER", "Initialise requested by supervision" );
    }


    /**
     * Never invoked if the observer is an unmanaged process.
     *
     * IDL:thalesgroup.com/CdmwSystemMngt/platformlibrary/Process/run:1.0
     */
    virtual
    void run()
    throw( CdmwPlatformMngt::Process::NotReadyToRun,
           CdmwPlatformMngt::Process::AlreadyDone,
           CORBA::SystemException )
    {
        PlatformInterface::notifyMessage(
            CdmwPlatformMngtBase::INF,
            "USER OBSERVER", "Run requested by supervision" );
    }


    /**
     * Never invoked if the observer is an unmanaged process.
     *
     * IDL:thalesgroup.com/CdmwSystemMngt/platformlibrary/Process/stop:1.0
     */
    virtual
    void stop()
    throw( CORBA::SystemException )
    {
        PlatformInterface::notifyMessage(
            CdmwPlatformMngtBase::INF,
            "USER OBSERVER", "Stop requested by supervision" );

        // Unsubscribe the observer from the CARDAMOM Supervision
        m_system->unregister_observer( m_observer_name.in() );
    }


private:

    /**
     * The supervision.
     */
    CdmwPlatformMngt::System_var m_system;

    /**
     * The observer name.
     */
    CORBA::String_var m_observer_name;

    /**
     * The observer object.
     */
    PortableServer::ServantBase_var m_observer_servant;
    CdmwPlatformMngt::SupervisionObserver_var m_observer;
};


#endif // __MYOBSERVERBEHAVIOUR_HPP__
