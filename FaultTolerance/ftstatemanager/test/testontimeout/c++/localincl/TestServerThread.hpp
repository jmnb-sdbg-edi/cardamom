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


#ifndef INCL_TEST_CLIENT_THREAD_HPP 
#define INCL_TEST_CLIENT_THREAD_HPP 

#include <string>

#include <Foundation/common/Options.hpp>
#include <Foundation/testutils/Testable.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/osthreads/Mutex.hpp>
#include <Foundation/osthreads/Condition.hpp>

#include "idllib/CdmwFTReplicationManager.stub.hpp"

#include "FaultTolerance/ftlocationmanager/PrimaryBackupGroupRepository_impl.hpp"
#include "FaultTolerance/ftlocationmanager/PrimaryBackupAdmin_impl.hpp"

using namespace Cdmw;


class TestServerThread
{

public:
        
    TestServerThread(CORBA::ORB_ptr orb,
                     PortableServer::POA_ptr rootPOA,
                     std::string& application_name);
        
    ~TestServerThread() throw() {} ;


    void start(int& argc, char** argv) throw();

private:

    CORBA::ORB_var  m_orb;
    PortableServer::POA_var m_rootPOA;
    std::string m_name;

public:
    ::FT::ObjectGroupId m_objectGroupId1;
    CORBA::Object_var m_objectGroup1;
    ::FT::ObjectGroupId m_objectGroupId2;
    CORBA::Object_var m_objectGroup2;
    ::FT::Location loc;

    ::CdmwFT::Location::PrimaryBackupAdmin_var m_primaryBackupAdmin;

public:
    bool flag;
    

};

#endif // INCL_TEST_CLIENT_THREAD_HPP 

