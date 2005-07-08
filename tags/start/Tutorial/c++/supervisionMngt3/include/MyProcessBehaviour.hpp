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
#include <Foundation/osthreads/Thread.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <SystemMngt/platforminterface/ProcessBehaviour.hpp>

#include "Data_impl.hpp"

using namespace Cdmw::PlatformMngt;
//using namespace Cdmw::OsSupport;

#define TEST_INFO( info ) { std::cout << info << "... "; }
#define TEST_SUCCEED()    { std::cout << "SUCCEED" << std::endl; }
#define TEST_FAILED()     { std::cout << "*** FAILED ***" << std::endl; }

/**
 * Example of thread allowing long or time unlimited activity during the
 * running state of the process.
 */
class RunThread
: virtual public Cdmw::OsSupport::Thread
{

public:
        
    RunThread()
    {
    }
     
    virtual
    ~RunThread()
    throw()
    {
    }

    void shutdown()
    {
    }

protected:

    /**
     * The thread's main activity.
     */
    void run()
    throw()
    {
        CORBA::Any data;

        try
        {
            // Example:
            // Notify a user event with data of type DataA to observers
            TEST_INFO( "Notifying a user event with data of type 'TYPE_A'" );
            data <<= m_factory_type_a.create( "First notification" );
            Cdmw::PlatformMngt::PlatformInterface::notifyUserEvent(
                "USER PROCESS", data );
            TEST_SUCCEED();  
        }
        catch (const CORBA::Exception& e)
        {
            TEST_FAILED();
            TEST_INFO( "Unexpected CORBA Exception raised: " << e );
        }

        try
        {
            // Example:
            // Notify a user event with data of type DataB to observers
            TEST_INFO( "Notifying a user event with data of type 'TYPE_B'" );
            data <<= m_factory_type_b.create( "Second notification", 12345L );
            Cdmw::PlatformMngt::PlatformInterface::notifyUserEvent(
                "USER PROCESS", data );
            TEST_SUCCEED();  
        }
        catch (const CORBA::Exception& e)
        {
            TEST_FAILED();
            TEST_INFO( "Unexpected CORBA Exception raised: " << e );
        }
    }

private:

    /**
     * The factories.
     */
    DataAFactory m_factory_type_a;
    DataBFactory m_factory_type_b;
};

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

        // Start long treatment in a thread
        // so as to be time unlimited
        m_run_thread = new RunThread();
        m_run_thread->start();
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

        // Stop the run thread's activity
        m_run_thread->shutdown();
        m_run_thread->join();
        delete m_run_thread;
    }

private:

    /**
     * The thread for long treatment in the running state of the process.
     */
    RunThread* m_run_thread;
};


#endif // __MYPROCESSBEHAVIOUR_HPP__
