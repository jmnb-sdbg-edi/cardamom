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


#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>

#include <testmiop/TestMIOP.hpp>
#include <testmiop/MIOPTester_impl.hpp>

#include <string>
#include <sstream>

#define NB_TEST_THREADS 10
#define NB_CALLS 10

namespace CdmwFT 
{

namespace Test 
{


class TestThread : public Cdmw::OsSupport::Thread
{
    public:
        TestThread(int thread_id,
                   CdmwFT::Test::MIOPTester_ptr tester,
                   int nb_calls)
            : m_thread_id(thread_id),
              m_tester(CdmwFT::Test::MIOPTester::_duplicate(tester)),
              m_nb_calls(nb_calls)
        {};

        virtual ~TestThread() throw()
        {};

    protected:
        virtual void run() 
        throw ()
        {
            for (int i=0; i<m_nb_calls; ++i) { 
                try {
                    m_tester->test_miop_request(m_thread_id, i+1);
                    Cdmw::OsSupport::OS::sleep(1000);
                } catch (const CORBA::SystemException& e) {
                    std::cerr << "... TestThread received SystemException: " << e << std::endl;
                }
            }
        };
        
            
    private:
        int m_thread_id;
        CdmwFT::Test::MIOPTester_var m_tester;
        int m_nb_calls;
};
    




    
    
// constructor
TestMIOP::TestMIOP(CORBA::ORB_ptr orb)
    : Testable("TestMIOP"),
      m_orb(CORBA::ORB::_duplicate(orb))
{ 
}

// destructor
TestMIOP::~TestMIOP()
{
}

// do_tests
void TestMIOP::do_tests()
{
    set_nbOfRequestedTestOK(3);

    // Get the root POA
    CORBA::Object_var obj = m_orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

    TEST_INFO("Create local MIOPTester object");
    CdmwFT::Test::MIOPTester_impl * miop_tester_impl; 
    MIOPTester_var tester, wrong_tester;
    try {
        // create servant
        miop_tester_impl = new CdmwFT::Test::MIOPTester_impl(m_orb.in());
        // activate Hello Object
        PortableServer::ObjectId_var obj_id = 
            rootPOA->activate_object(miop_tester_impl);

        // create MIOP reference with UMAX_LONG+10 as group id
        unsigned long long groupid = ULONG_MAX ;
        groupid += 10;
        std::ostringstream ostr;
        ostr << "corbaloc:miop:1.0@1.0-cdmwftdomain-" << groupid << "/225.1.1.8:5555";
        std::string multicast_addr = ostr.str();
        CORBA::Object_var miop_ref = m_orb->string_to_object(multicast_addr.c_str());

        // associate MIOP reference with MIOPTester object
        TEST_INFO("Associate MIOP reference " << multicast_addr << " to MIOPTester object");
        rootPOA->associate_reference_with_id (miop_ref.in(), obj_id);

        // create MIOPTester reference
        tester = MIOPTester::_unchecked_narrow (miop_ref.in());

        // create wrong MIOPTester reference with UMAX_LONG as group_id
        std::ostringstream ostr2;
        ostr2 << "corbaloc:miop:1.0@1.0-cdmwftdomain-" << ULONG_MAX << "/225.1.1.8:5555";
        TEST_INFO("Create wrong MIOP reference " << ostr2.str());
        CORBA::Object_var wrong_miop_ref = m_orb->string_to_object(ostr2.str().c_str());
        wrong_tester = MIOPTester::_unchecked_narrow (wrong_miop_ref.in());

        TEST_SUCCEED();
    } catch (const CORBA::SystemException& e) {
        TEST_FAILED();
        TEST_INFO("CORBA::SystemException received: " << e);
        return;
    }
    
    TEST_INFO("Test MIOP with wrong MIOP reference");
    TestThread wrongTestThread(666, wrong_tester.in(), 1);
    wrongTestThread.start();
    wrongTestThread.join();
    if (miop_tester_impl->m_received_requests[666].find(1) == 
        miop_tester_impl->m_received_requests[666].end())
    {
        TEST_SUCCEED();
    } 
    else 
    {
        TEST_INFO("Error: Request received with a wrong MIOP reference. PCR-0379 not corrected");
        TEST_FAILED();
    }
    
    
    TEST_INFO("Starting " << NB_TEST_THREADS << " client threads sending "
              << NB_CALLS << " MIOP requests each.");

    // create client threads
    TestThread** test_thread_pool = new (TestThread*)[NB_TEST_THREADS];
    for (int i=0; i<NB_TEST_THREADS; ++i) {
        test_thread_pool[i] = new TestThread(i+1, tester.in(), NB_CALLS);
    }

    // start client threads
    for (int i=0; i<NB_TEST_THREADS; ++i) {
        test_thread_pool[i]->start();
    }
        
    // wait client threads
    for (int i=0; i<NB_TEST_THREADS; ++i) {
        test_thread_pool[i]->join();
    }
    
    
    TEST_INFO("Check if servant received all MIOP requests.");
    bool test_failed=false;
    for (int i=1; i<=NB_TEST_THREADS; ++i) {
        for (int j=1; j<=NB_CALLS; ++j) {
            std::map< CORBA::UShort, bool >::iterator it =
                miop_tester_impl->m_received_requests[i].find(j);
            if (it == miop_tester_impl->m_received_requests[i].end()) {
                TEST_INFO("Request numero " << j << " from thread " << i 
                          << " has never been received.");
                test_failed=true;
            }
        }
    }
    if (test_failed)
        TEST_FAILED();
    else
        TEST_SUCCEED();
    
    
    for (int i=0; i<NB_TEST_THREADS; ++i) {
        delete test_thread_pool[i];
    }
    
}


} // end namespace Test

} // end namespace CdmwFT

