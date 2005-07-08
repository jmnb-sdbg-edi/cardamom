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

#include <Foundation/orbsupport/CORBA.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <SystemMngt/platforminterface/ProcessBehaviour.hpp>

using namespace Cdmw::PlatformMngt;

/**
 * Example of a concrete process behaviour.
 */
class MyProcessBehaviour
: virtual public Cdmw::PlatformMngt::ProcessBehaviour
{

public:

    /**
     * The constructor.
     */ 
    MyProcessBehaviour()
    {
    }

    /**
     * The destructor.
     */ 
    virtual
    ~MyProcessBehaviour()
    throw()
    {
    }

    /**
     * IDL:thalesgroup.com/CdmwSystemMngt/platformlibrary/Process/initialise:1.0
     */
    virtual
    void initialise( const CdmwPlatformMngtBase::StartupKind& startup_kind )
    throw( CdmwPlatformMngt::Process::BadOrder,
           CORBA::SystemException )
    {
        PlatformInterface::notifyMessage(
            CdmwPlatformMngtBase::INF,
            "USER PROCESS", "Initialise requested by supervision" );
    }

    /**
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
            "USER PROCESS", "Run requested by supervision" );
    }

    /**
     * IDL:thalesgroup.com/CdmwSystemMngt/platformlibrary/Process/stop:1.0
     */
    virtual
    void stop()
    throw( CORBA::SystemException )
    {
        PlatformInterface::notifyMessage(
            CdmwPlatformMngtBase::INF,
            "USER PROCESS", "Stop requested by supervision" );
    }

};


#endif // __MYPROCESSBEHAVIOUR_HPP__
