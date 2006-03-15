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


#ifndef INCL_CDMW_TEST_ORB_INITIALIZER_IMPL_HPP
#define INCL_CDMW_TEST_ORB_INITIALIZER_IMPL_HPP

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/orbsupport/PortableInterceptor.hpp>

#include "testftrequestlogging3/TestServerRequestInterceptor_impl.hpp"

namespace Cdmw
{


class TestORBInitializer : public PortableInterceptor::ORBInitializer,
                       public Cdmw::OrbSupport::RefCountLocalObject
{
public:

    // constructor
    TestORBInitializer(const std::string& host1,
                       const std::string& host2,
                       const std::string& host3)
    :  m_host1(host1),
       m_host2(host2),
       m_host3(host3)
    {
    }

    //destructor
    ~TestORBInitializer(){}

    //
    // IDL to C++ Mapping
    //
    virtual void
    pre_init(PortableInterceptor::ORBInitInfo_ptr info) 
       throw (CORBA::SystemException){}

    
    //
    // Note: the interceptors are registered in post_init()
    //       if their constructors require
    //       ORBInitInfo::resolve_initial_reference(), which XXX
    //       cannot be called in pre_init().
    //
    virtual void
    post_init(PortableInterceptor::ORBInitInfo_ptr info) 
       throw (CORBA::SystemException)
    {
       M_interceptor = new TestServerRequestInterceptor_impl(info, 
                                                             m_host1,
                                                             m_host2,
                                                             m_host3);
       
       try
       {
           info->add_server_request_interceptor(M_interceptor);
       }
       catch(const PortableInterceptor::ORBInitInfo::DuplicateName&)
       {
           // can not happened while interceptor is not named!
           CDMW_ASSERT(false);
       }
    }
    
private:
    static TestServerRequestInterceptor_impl*  M_interceptor;
    const std::string& m_host1;
    const std::string& m_host2;
    const std::string& m_host3;
};

TestServerRequestInterceptor_impl* TestORBInitializer::M_interceptor = 0;
   
}; // end of namespace Cdmw

#endif
