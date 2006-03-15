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

#ifndef INCL_MY_PROCESS_BEHAVIOUR_HPP 
#define INCL_MY_PROCESS_BEHAVIOUR_HPP 

// Standard Files 
#include <string>

// Cdmw Files
#include "Foundation/common/Options.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>
#include "testftrepository/TestFTRepositoryThread.hpp"


/**
* Purpose:
* <p>
* The abstract process behaviour.
*/
class MyProcessBehaviour : public Cdmw::CdmwInit::ProcessControl
{

public:

    /**
     * Purpose:
     * <p> The constructor.
     */
    MyProcessBehaviour(
        CORBA::ORB_ptr orb,
        PortableServer::POA_ptr rootPOA,
        const std::string&  name)
        : m_test_name(name)
    {
        m_orb = CORBA::ORB::_duplicate( orb );
        m_rootPOA = PortableServer::POA::_duplicate( rootPOA );
        m_test_thread = NULL;
    }

    /**
     * Purpose:
     * <p> The destructor.
     */
    virtual ~MyProcessBehaviour() throw ()
    {
    }

    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
     * operation
     */
    virtual void on_initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
    throw( CORBA::SystemException )
    {

    }

    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
     * operation
     */
    virtual void on_run()
    throw( CORBA::SystemException,
           CdmwPlatformMngt::ProcessDelegate::NotReadyToRun )
    {
        // Launch the test in a thread
        m_test_thread = new TestFTRepositoryThread(m_orb.in(),
                                                   m_rootPOA.in(),
                                                   m_test_name);

        m_test_thread->start();
    }

    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
     * operation
     */
    virtual void on_stop()
    throw( CORBA::SystemException )
    {
        m_test_thread->join();
    }

private:

    /**
     * The ORB.
     */
    CORBA::ORB_var m_orb;

    /**
     * The root POA.
     */
    PortableServer::POA_var m_rootPOA;

    /**
     * The test thread.
     */
    TestFTRepositoryThread* m_test_thread;

    /**
     * test name
     */
    std::string m_test_name;

};


#endif // INCL_MY_PROCESS_BEHAVIOUR_HPP

