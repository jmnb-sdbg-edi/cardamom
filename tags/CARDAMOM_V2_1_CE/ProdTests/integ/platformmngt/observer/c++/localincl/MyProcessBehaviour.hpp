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

#ifndef __MYPROCESSBEHAVIOUR_HPP__
#define __MYPROCESSBEHAVIOUR_HPP__

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/common/Exception.hpp"

#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platforminterface/ProcessBehaviour.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp"
#include "observer/Observer_impl.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace Cdmw::PlatformMngt;
using namespace Cdmw::OsSupport;


/**
 * Purpose:
 * <p>
 * The concrete process behaviour.
 */
class MyProcessBehaviour : public Cdmw::PlatformMngt::ProcessBehaviour
{

public:

    /**
     * Purpose:
     * <p> The constructor.
     */ 
    MyProcessBehaviour(
        CORBA::ORB_ptr orb,
        CdmwPlatformMngt::System_ptr system,
        const char* observer_name )
    {
        m_orb = CORBA::ORB::_duplicate( orb );
        m_system = CdmwPlatformMngt::System::_duplicate( system );
        m_observer_name = CORBA::string_dup( observer_name );

        // Create an observer
        Observer_impl *observer_i = new Observer_impl();
        m_observer_servant = observer_i;
        m_observer = observer_i->_this();

        // Register the observer to the system
        m_system->register_observer( m_observer_name.in(), m_observer.in() );
    }


    /**
     * Purpose:
     * <p> The destructor.
     */ 
    virtual
    ~MyProcessBehaviour()
    throw()
    {
    }


    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/initialise:1.0
     * operation
     */
    virtual
    void initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
    throw( CdmwPlatformMngt::ProcessDelegate::BadOrder,
           CORBA::SystemException )
    {
    }


    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
     * operation
     */
    virtual
    void run()
    throw( CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
           CdmwPlatformMngt::ProcessDelegate::AlreadyDone,
           CORBA::SystemException )
    {
    }

    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
     * operation
     */
    virtual
    void stop()
    throw( CORBA::SystemException )
    {
        // Unregister the status change event observer from the system
        m_system->unregister_observer( m_observer_name.in() );
    }


private:

    /**
     * The ORB.
     */
    CORBA::ORB_var m_orb;

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


#endif // __MYPROCESSBEHAVIOUR_HPP__

