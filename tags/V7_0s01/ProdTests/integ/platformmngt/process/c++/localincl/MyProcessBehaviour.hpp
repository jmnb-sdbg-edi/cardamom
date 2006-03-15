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

#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platforminterface/ProcessBehaviour.hpp"
#include "Foundation/orbsupport/CORBA.hpp"

#include "process/Thread.hpp"

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
    MyProcessBehaviour()
    {
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
    void initialise( const CdmwPlatformMngtBase::StartupKind& startup_kind )
    throw( CdmwPlatformMngt::ProcessDelegate::BadOrder,
           CORBA::SystemException )
    {
        PlatformInterface::Notify_message(
            CdmwPlatformMngtBase::INF, "USER PROCESS", "Initialise requested" );
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
        PlatformInterface::Notify_message(
            CdmwPlatformMngtBase::INF, "USER PROCESS", "Run requested" );

        // Launch the test in a thread
        m_test_thread = new TestThread();
        m_test_thread->start();
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
        PlatformInterface::Notify_message(
            CdmwPlatformMngtBase::INF, "USER PROCESS", "Stop requested" );

        m_test_thread->shutdown();
        m_test_thread->join();
        delete m_test_thread;
    }

private:

    TestThread* m_test_thread;
};


#endif // __MYPROCESSBEHAVIOUR_HPP__

