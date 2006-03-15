/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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


#ifndef ___CCM_LB_TEST_CCM_CLIENT_FT_IMPL_HPP__
#define ___CCM_LB_TEST_CCM_CLIENT_FT_IMPL_HPP__


#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/testutils/Testable.hpp>
#include <Foundation/osthreads/ThreadHandle.hpp>

#include <ccm_lb/User_ccm_lb.skel.hpp>


namespace CcmLbTest
{

class TestThread : public Cdmw::OsSupport::Thread
{
    public:
        TestThread(Cdmw::TestUtils::Testable* testable)
           : m_testable(testable)
        {}

    protected:
        void run() throw()
        {
            m_testable->start();
        }

    private:
        TestThread();
        TestThread(const TestThread&);
        TestThread& operator = (const TestThread&);

        Cdmw::TestUtils::Testable* m_testable;
};


//
// IDL:thalesgroup.com/UserCcmLbTest/CCM_Client:1.0
//
class CCM_Client_FT_impl : virtual public ::UserCcmLbTest::User_CCM_Client,
                        virtual public Cdmw::OrbSupport::RefCountLocalObject,
                        virtual Cdmw::TestUtils::Testable
{
    private:
        CCM_Client_FT_impl(const CCM_Client_FT_impl&);
        void operator=(const CCM_Client_FT_impl&);


        ::CcmLbTest::CCM_Client_Context_var m_session_context;

    public:
        CCM_Client_FT_impl();
        ~CCM_Client_FT_impl();


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

        ::CcmLbTest::CCM_Client_Context*
        get_context()
        {
            return ::CcmLbTest::CCM_Client_Context::_duplicate(m_session_context.in());
        }

    private:
        //
        // Testable operations
        //
        void do_tests();

        char* test_connections(void);

        TestThread m_testThread;
};

}; // End of namespace CcmLbTest


#endif // ___CCM_LB_TEST_CCM_CLIENT_FT_IMPL_HPP__
