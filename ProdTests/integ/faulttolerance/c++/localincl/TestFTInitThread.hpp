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

#ifndef INCL_TESTFTINITT_THREAD_HPP
#define INCL_TESTFTINITT_THREAD_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include "FaultTolerance/idllib/FT.stub.hpp"

#include "faulttolerance/TestFTInit.hpp"
#include "faulttolerance/TestUtils.hpp"

#include <string>

class TestFTInitThread : public Cdmw::OsSupport::Thread
{
public:
        
    TestFTInitThread(
        CORBA::ORB_ptr orb,
        Cdmw::Test::HelloInterface_ptr group,
        CdmwFT::ReplicationManager_ptr replication_mgr)
    : m_orb( CORBA::ORB::_duplicate( orb ) ),
      m_replication_mgr( CdmwFT::ReplicationManager::_duplicate( replication_mgr ) ),
      m_group( Cdmw::Test::HelloInterface::_duplicate( group ) )
    {
    }
     
    virtual ~TestFTInitThread()
        throw()
    {
    }

    void shutdown()
    {
       // nothing to do : shutdown done in ProcessControl
    }

protected:

    void run() throw()
    {
        // Do the actual test
        Cdmw::FT::TestFTInit test(
            m_orb.in(), m_replication_mgr.in(), m_group.in(), "TestFaultTolerance");

        test.start();
    }

private:

    CORBA::ORB_var m_orb;
    CdmwFT::ReplicationManager_var m_replication_mgr;
    Cdmw::Test::HelloInterface_var m_group;

};

#endif //INCL_TESTFTINITT_THREAD_HPP

