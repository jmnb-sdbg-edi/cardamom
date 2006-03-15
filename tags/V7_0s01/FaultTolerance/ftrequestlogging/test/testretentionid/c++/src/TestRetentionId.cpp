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

#include <testretentionid/TestRetentionId.hpp>
#include <Foundation/common/Assert.hpp>
#include <FaultTolerance/idllib/CdmwFTManager.stub.hpp>


namespace 
{
 
    const char* REP_ID_FAULT_DETECTOR = "IDL:thalesgroup.com/CdmwFT/FaultManagement/FaultDetector:1.0";

} // end anonymous namespace
namespace CdmwFT 
{

namespace Test 
{


class TestThread : public Cdmw::OsSupport::Thread
{
    public:
        TestThread(CdmwFT::Test::RetentionIdTester_ptr tester)
            : m_tester(CdmwFT::Test::RetentionIdTester::_duplicate(tester)),
              m_sleep_time(0),
              m_send_exception(false)
        {
            m_request_infos.reception_time  = 0;
            m_request_infos.retention_id    = 0;
            m_request_infos.expiration_time = 0;
        };

        virtual ~TestThread() throw()
        {};

        void set_sleep_time(CORBA::UShort sleep_time)
        {
            m_sleep_time = sleep_time;
        };
        
        void set_send_exception(CORBA::Boolean send_exception)
        {
            m_send_exception = send_exception;
        };
            
        RequestInfos get_request_infos()
        {
            return m_request_infos;
        };
            
    protected:
        virtual void run() 
        throw ()
        {
            try {
                std::cout << "... TestThread sending request (" 
                          << m_sleep_time << ", " << m_send_exception
                          << ")" << std::endl;
                m_request_infos = m_tester->test_request(m_sleep_time, 
                                                         m_send_exception);
                std::cout << "... TestThread received normal answer" << std::endl;
            } catch (const CORBA::SystemException& e) {
                std::cerr << "... TestThread received SystemException: " << e << std::endl;
            } catch (const MyException& e) {
                std::cout << "... TestThread received MyException" << std::endl;
                m_request_infos = e.infos;
            }
            std::cout << "... TestThread exiting" << std::endl;
        };
        
            
    private:
        CdmwFT::Test::RetentionIdTester_var m_tester;
        CORBA::UShort m_sleep_time;
        CORBA::Boolean m_send_exception;
        RequestInfos m_request_infos;
};
    



class TesterHandler_impl : 
    virtual public ::POA_CdmwFT::Test::AMI_RetentionIdTesterHandler,
    virtual public PortableServer::RefCountServantBase
{
    public:
  
        TesterHandler_impl()
        {
            m_request_infos.reception_time  = 0;
            m_request_infos.retention_id    = 0;
            m_request_infos.expiration_time = 0;
        };
         
        virtual ~TesterHandler_impl()
            throw()
        {};
        
        virtual void test_request(const RequestInfos& ami_return_val)
            throw (CORBA::SystemException)
        {
            std::cout << "... AMI Handler received normal answer" << std::endl;
            m_request_infos = ami_return_val;
        }

        virtual void test_request_excep(AMI_RetentionIdTesterExceptionHolder* excep_holder)
            throw (CORBA::SystemException)
        {
            try {
                excep_holder->raise_test_request();
            } catch (const CORBA::SystemException& e) {
                std::cerr << "... AMI Handler received SystemException: " << e << std::endl;
            } catch (const MyException& e) {
                std::cout << "... AMI Handler received MyException" << std::endl;
                m_request_infos = e.infos;
            }
        }

        RequestInfos get_request_infos()
        {
            return m_request_infos;
        };
            

    private:
        RequestInfos m_request_infos;
};

    
    
// constructor
TestRetentionId::TestRetentionId(
    CORBA::ORB_ptr                      orb,
    CdmwFT::Test::RetentionIdTester_ptr tester)
    : Testable("TestRetentionId"),
      m_orb(CORBA::ORB::_duplicate(orb)),
      m_tester(CdmwFT::Test::RetentionIdTester::_duplicate(tester))
      
{ 
}

// destructor
TestRetentionId::~TestRetentionId()
{
}

// do_tests
void TestRetentionId::do_tests()
{
    set_nbOfRequestedTestOK(31);
    
    TEST_INFO("------->Send request 1");
    RequestInfos infos1;
    try {
        infos1 = m_tester->test_request(0, false);
        TEST_SUCCEED();
    } catch (const CORBA::SystemException& e) {
        TEST_INFO("CORBA System Exception raised: " << e);
        TEST_FAILED();
    } catch (const MyException& e) {
        TEST_INFO("Unexpected MyException received.");
        TEST_FAILED();
    }
    
    Cdmw::OsSupport::OS::sleep(2000);
    TEST_INFO("------->Send request 2");
    RequestInfos infos2;
    try {
        infos2 = m_tester->test_request(0, false);
        TEST_SUCCEED();
    } catch (const CORBA::SystemException& e) {
        TEST_INFO("CORBA System Exception raised: " << e);
        TEST_FAILED();
    } catch (const MyException& e) {
        TEST_INFO("Unexpected MyException received.");
        TEST_FAILED();
    }
    TEST_CHECK(infos2.reception_time > infos1.reception_time);
    TEST_CHECK(infos2.retention_id > infos1.retention_id);
    TEST_CHECK(infos2.expiration_time > infos1.expiration_time);
    
    Cdmw::OsSupport::OS::sleep(2000);
    TEST_INFO("------->Send request 3 (sending exception)");
    RequestInfos infos3;
    try {
        infos3 = m_tester->test_request(0, true);
        TEST_FAILED();
    } catch (const CORBA::SystemException& e) {
        TEST_INFO("CORBA System Exception raised: " << e);
        TEST_FAILED();
    } catch (const MyException& e) {
        TEST_SUCCEED();
        infos3 = e.infos;
    }
    TEST_CHECK(infos3.reception_time > infos2.reception_time);
    TEST_CHECK(infos3.retention_id > infos2.retention_id);
    TEST_CHECK(infos3.expiration_time > infos2.expiration_time);
    
    Cdmw::OsSupport::OS::sleep(2000);
    TEST_INFO("------->Send request 4");
    RequestInfos infos4;
    try {
        infos4 = m_tester->test_request(0, false);
        TEST_SUCCEED();
    } catch (const CORBA::SystemException& e) {
        TEST_INFO("CORBA System Exception raised: " << e);
        TEST_FAILED();
    } catch (const MyException& e) {
        TEST_INFO("Unexpected MyException received.");
        TEST_FAILED();
    }
    TEST_CHECK(infos4.reception_time > infos3.reception_time);
    TEST_CHECK(infos4.retention_id > infos3.retention_id);
    TEST_CHECK(infos4.expiration_time > infos3.expiration_time);
 
    Cdmw::OsSupport::OS::sleep(1000);
    TEST_INFO("------->Send request 5 & 6 at the same time");
    TestThread test_thread(m_tester.in());
    test_thread.set_sleep_time(10000);
    RequestInfos infos5, infos6;
    try {
        TEST_INFO("    send req 5");
        test_thread.start();
        TEST_INFO("    wait 2 sec");
        Cdmw::OsSupport::OS::sleep(2000);
        TEST_INFO("    send req 6");
        infos6 = m_tester->test_request(2000, false);
        TEST_INFO("    returned req 6");
        TEST_SUCCEED();
    } catch (const CORBA::SystemException& e) {
        TEST_INFO("CORBA System Exception raised: " << e);
        TEST_FAILED();
    } catch (const MyException& e) {
        TEST_INFO("Unexpected MyException received.");
        TEST_FAILED();
    }

    TEST_INFO("    wait thread");
    test_thread.join();
    TEST_INFO("    thread ended");
    
    infos5 = test_thread.get_request_infos();
    TEST_CHECK(infos5.reception_time =! 0);
    TEST_CHECK(infos6.retention_id > infos5.retention_id);
    // no check of reception_time and expiration_time
    // as there is no guarantee about request sending order


    Cdmw::OsSupport::OS::sleep(2000);
    TEST_INFO("------->Test with AMI");
    
    TEST_INFO("------->Send request 7 as AMI");
    TesterHandler_impl* tester_handler_impl1 = new TesterHandler_impl();
    CdmwFT::Test::AMI_RetentionIdTesterHandler_var tester_handler1 =
        tester_handler_impl1->_this();
    RequestInfos infos7;
    try {
        m_tester->sendc_test_request(tester_handler1.in(),
                                     5000,
                                     false);
        TEST_SUCCEED();
    } catch (const CORBA::SystemException& e) {
        TEST_INFO("CORBA System Exception raised: " << e);
        TEST_FAILED();
    }
    
    Cdmw::OsSupport::OS::sleep(1000);
    TEST_INFO("------->Send request 8 as AMI");
    TesterHandler_impl* tester_handler_impl2 = new TesterHandler_impl();
    CdmwFT::Test::AMI_RetentionIdTesterHandler_var tester_handler2 =
        tester_handler_impl2->_this();
    RequestInfos infos8;
    try {
        m_tester->sendc_test_request(tester_handler2.in(),
                                     5000,
                                     true);
        TEST_SUCCEED();
    } catch (const CORBA::SystemException& e) {
        TEST_INFO("CORBA System Exception raised: " << e);
        TEST_FAILED();
    }

    Cdmw::OsSupport::OS::sleep(1000);
    TEST_INFO("------->Send request 9 as AMI");
    TesterHandler_impl* tester_handler_impl3 = new TesterHandler_impl();
    CdmwFT::Test::AMI_RetentionIdTesterHandler_var tester_handler3 =
        tester_handler_impl3->_this();
    RequestInfos infos9;
    try {
        m_tester->sendc_test_request(tester_handler3.in(),
                                     5000,
                                     false);
        TEST_SUCCEED();
    } catch (const CORBA::SystemException& e) {
        TEST_INFO("CORBA System Exception raised: " << e);
        TEST_FAILED();
    }
    
    
    Cdmw::OsSupport::OS::sleep(8000);
    infos7 = tester_handler_impl1->get_request_infos();
    infos8 = tester_handler_impl2->get_request_infos();
    infos9 = tester_handler_impl3->get_request_infos();

    TEST_CHECK(infos7.reception_time != 0);
    TEST_CHECK(infos7.reception_time > infos6.reception_time);
    TEST_CHECK(infos7.retention_id > infos6.retention_id);
    TEST_CHECK(infos7.expiration_time > infos6.expiration_time);
    
    TEST_CHECK(infos8.reception_time != 0);
    TEST_CHECK(infos8.reception_time > infos7.reception_time);
    TEST_CHECK(infos8.retention_id > infos7.retention_id);
    TEST_CHECK(infos8.expiration_time > infos7.expiration_time);
    
    TEST_CHECK(infos9.reception_time != 0);
    TEST_CHECK(infos9.reception_time > infos8.reception_time);
    TEST_CHECK(infos9.retention_id > infos8.retention_id);
    TEST_CHECK(infos9.expiration_time > infos8.expiration_time);
    
    
}


} // end namespace Test

} // end namespace CdmwFT

