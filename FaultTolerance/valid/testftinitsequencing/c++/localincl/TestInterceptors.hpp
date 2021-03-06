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


#ifndef INCL_TEST_INTERCEPTORS_HPP 
#define INCL_TEST_INTERCEPTORS_HPP 

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/orbsupport/PortableInterceptor.hpp>
#include "testftinitsequencing/TestHello_impl.hpp"

namespace Cdmw
{


    class TestServerInterceptor : public PortableInterceptor::ServerRequestInterceptor,
                                  public Cdmw::OrbSupport::RefCountLocalObject
    {

    public:
        

        TestServerInterceptor (const char* where,
                               const char* host);

        //
        // IDL to C++ Mapping
        //
        virtual char*
        name() throw (CORBA::SystemException);

        virtual void
        destroy() throw (CORBA::SystemException);

        virtual void
        receive_request_service_contexts(
            PortableInterceptor::ServerRequestInfo_ptr ri)
            throw (CORBA::SystemException, PortableInterceptor::ForwardRequest);

        virtual void
        receive_request(PortableInterceptor::ServerRequestInfo_ptr ri)
            throw (CORBA::SystemException, PortableInterceptor::ForwardRequest);

        virtual void
        send_reply(PortableInterceptor::ServerRequestInfo_ptr ri)
            throw (CORBA::SystemException);

        virtual void
        send_other(PortableInterceptor::ServerRequestInfo_ptr ri)
            throw (CORBA::SystemException, PortableInterceptor::ForwardRequest);

        virtual void
        send_exception(PortableInterceptor::ServerRequestInfo_ptr ri)
            throw (CORBA::SystemException, PortableInterceptor::ForwardRequest);

        void init(CdmwReplicationManager::time_collector_ptr time_collector)
        {
            m_init = true;
            m_time_collector = CdmwReplicationManager::time_collector::_duplicate(time_collector);
        }
        

    private:

        enum {
          kIntercept_SEND_REPLY, kIntercept_RECEIVE_REQUEST
        };
        std::string m_host_name;
        int m_event_to_intercept;

        TestServerInterceptor(const TestServerInterceptor&);
        void operator=(const TestServerInterceptor&);

        CdmwReplicationManager::time_collector_var m_time_collector;
        bool m_init ;
        

    }; // End class OnInit_impl

}; // End namespace Cdmw

#endif // INCL_ONINIT_INTERFACE_IMPL_HPP 

