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

#ifndef ___CCM_FT_TEST_CCM_CLIENT_IMPL_HPP__
#define ___CCM_FT_TEST_CCM_CLIENT_IMPL_HPP__

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/testutils/Testable.hpp>
#include <Foundation/osthreads/ThreadHandle.hpp>

#include <ccm_ft_request_logging/User_ccm_ft_request_logging.skel.hpp>

namespace CcmFtTest
{

    
    
class TestThread : public Cdmw::OsSupport::Thread 
{
    public:
        TestThread(Cdmw::TestUtils::Testable* testable)
           : m_testable(testable)
        {};
        
    protected:
        void run() throw()
	    {
	      m_testable->start();
	    };
        
    private:
        TestThread();
        TestThread(const TestThread&);
        TestThread& operator = (const TestThread&);
   
        Cdmw::TestUtils::Testable* m_testable;
};

    
//
// IDL:thalesgroup.com/UserCcmFtTest/CCM_Client:1.0
//
class CCM_Client_impl : virtual public ::UserCcmFtTest::User_CCM_Client,
                        virtual public Cdmw::OrbSupport::RefCountLocalObject,
                        virtual Cdmw::TestUtils::Testable
{
    CCM_Client_impl(const CCM_Client_impl&);
    void operator=(const CCM_Client_impl&);

    CORBA::String_var m_expectedPrimaryHostname;
    CORBA::String_var m_expectedBackup1Hostname;
    CORBA::String_var m_expectedBackup2Hostname;

    ::CcmFtTest::CCM_Client_Context_var m_session_context;
	 
public:

    CCM_Client_impl();
    ~CCM_Client_impl();
	 

    //
    // attribute expectedPrimaryHostname
    //
    virtual char * expectedPrimaryHostname ()
        throw (CORBA::SystemException);
    virtual void expectedPrimaryHostname (const char * a)
        throw (CORBA::SystemException);

        
    //
    // attribute expectedBackup1Hostname
    //
    virtual char * expectedBackup1Hostname ()
        throw (CORBA::SystemException);
    virtual void expectedBackup1Hostname (const char * a)
        throw (CORBA::SystemException);

    //
    // attribute expectedBackup2Hostname
    //
    virtual char * expectedBackup2Hostname ()
        throw (CORBA::SystemException);
    virtual void expectedBackup2Hostname (const char * a)
        throw (CORBA::SystemException);

        
    //
    // Session component specific operations
    //
    void set_session_context(Components::SessionContext_ptr ctx)
        throw(CORBA::SystemException);
    void ccm_activate()
        throw(CORBA::SystemException);
    void ccm_passivate()
        throw(CORBA::SystemException);
    void ccm_remove()
        throw(CORBA::SystemException);


    ::CcmFtTest::CCM_Client_Context* get_context()
    {
        return ::CcmFtTest::CCM_Client_Context::_duplicate(m_session_context.in());
    }


private:

    //
    // Testable operations
    //
    void do_tests();
    
    void test_connections(const char* expectedHostname);
    
    TestThread m_testThread;
    
};

}; // End of namespace CcmFtTest


#endif // ___CCM_FT_TEST_CCM_CLIENT_IMPL_HPP__

