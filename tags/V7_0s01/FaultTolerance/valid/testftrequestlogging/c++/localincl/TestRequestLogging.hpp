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


#ifndef INCL_TEST_BACKUP_INSERTION_HPP
#define INCL_TEST_BACKUP_INSERTION_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/testutils/Testable.hpp>
#include <Foundation/osthreads/Mutex.hpp>
#include <Foundation/osthreads/Condition.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/commonsvcs/datastore/DataStoreBase.hpp>
#include <FaultTolerance/ftrequestlogging/FTReplyRecording.hpp>
#include "testftrequestlogging/TestHello_impl.hpp"


namespace Cdmw {
namespace FT {

class TestRequestLogging : virtual public Cdmw::TestUtils::Testable
{
    typedef Cdmw::CommonSvcs::DataStore::DsIdentifier DsIdentifier;

public:
    // constructor
    TestRequestLogging(
        CORBA::ORB_ptr orb,
        PortableServer::POA_ptr rootPOA,
        const std::string& name,
        const size_t duration_time,
        const std::string& host1,
        const std::string& host2,
        const std::string& host3);

    // destructor
    virtual ~TestRequestLogging();

protected:
    // do_tests
    virtual void do_tests();

private:
    size_t m_duration_time;
    std::string m_host1;
    std::string m_host2;
    std::string m_host3;
    CORBA::ORB_var m_orb;
    PortableServer::POA_var m_rootPOA;
    CdmwRequestLogging::HelloInterface_var m_hello;
};

}; // namespace FT
}; // namespace Cdmw

#endif
