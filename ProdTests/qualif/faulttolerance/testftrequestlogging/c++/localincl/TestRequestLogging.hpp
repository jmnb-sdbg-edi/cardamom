/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * This file and the information it contains are confidential and proprietary. 
 * They shall not be reproduced nor disclosed to any person except to those 
 * having a need to know them without prior written consent of the owner.
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
