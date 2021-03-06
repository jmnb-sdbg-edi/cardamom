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

#ifndef INCL_TESTFTINITT_HREAD_HPP
#define INCL_TESTFTINITT_HREAD_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/osthreads/Thread.hpp"
#include "FaultTolerance/idllib/FT.stub.hpp"

#include "faulttoleranceclient2/TestFTInit.hpp"
#include "faulttoleranceclient2/TestUtils.hpp"

class TestFTInitThread : public Cdmw::OsSupport::Thread
{

public:
        
    TestFTInitThread(
        CORBA::ORB_ptr orb,
        PortableServer::POA_ptr rootPOA,
        CdmwFT::ReplicationManager_ptr replication_mgr,
        std::string host_name1,
        std::string host_name2,
        std::string host_name3 )
        :  m_orb( CORBA::ORB::_duplicate( orb ) ),
           m_rootPOA( PortableServer::POA::_duplicate( rootPOA ) ),
           m_replication_mgr( CdmwFT::ReplicationManager::_duplicate(replication_mgr ) ),
           m_host_name1(host_name1),
           m_host_name2(host_name2),
           m_host_name3(host_name3)
           
    {
    }
    
    virtual ~TestFTInitThread()
        throw()
    {
    }

protected:

    void run() throw()
    {
        // Do test
        Cdmw::FT::TestFTInit test(
            m_orb.in(), m_rootPOA.in(), m_replication_mgr.in(),
            "TestFTInit", m_host_name1, m_host_name2, m_host_name3);

        test.start();
    }
        
private:

    CORBA::ORB_var m_orb;
    PortableServer::POA_var m_rootPOA;
    CdmwFT::ReplicationManager_var m_replication_mgr;
    std::string m_host_name1;
    std::string m_host_name2;
    std::string m_host_name3;

};

#endif //INCL_TESTFTINITT_HREAD_HPP

