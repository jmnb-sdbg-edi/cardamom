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


#ifndef ___CCM_LB_TEST_CCM_CLIENT_IMPL_HPP__
#define ___CCM_LB_TEST_CCM_CLIENT_IMPL_HPP__


#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/osthreads/ThreadHandle.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <TestThread.hpp>

#include <User_ccm_lb.skel.hpp>

#define ECHO_HEADER() \
    "[Debug] "

#define ECHO_ERROR(comment) \
do {\
    std::cerr << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)
#define ENABLE_LB_DEBUG_DUMP 

#ifndef ENABLE_LB_DEBUG_DUMP

#   define DEBUG_DUMP(comment)
#   define DEBUG_ECHO(comment)

#else

#   define DEBUG_DUMP(comment) \
do {\
    std::cerr << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)

#   define DEBUG_ECHO(comment) \
do {\
    std::cerr << comment ;\
} while(0)

#endif


namespace CcmLbTest
{

//
// IDL:thalesgroup.com/UserCcmLbTest/CCM_Client:1.0
//
class CCM_Client_impl : virtual public ::UserCcmLbTest::User_CCM_Client,
                        virtual public Cdmw::OrbSupport::RefCountLocalObject
{
    private:
        CCM_Client_impl(const CCM_Client_impl&);
        void operator=(const CCM_Client_impl&);


        ::CcmLbTest::CCM_Client_Context_var m_session_context;

    public:
        CCM_Client_impl();
        ~CCM_Client_impl();


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
        ::CcmLbTest::TestThread m_testThread;
};

}; // End of namespace CcmLbTest


#endif // ___CCM_LB_TEST_CCM_CLIENT_IMPL_HPP__
