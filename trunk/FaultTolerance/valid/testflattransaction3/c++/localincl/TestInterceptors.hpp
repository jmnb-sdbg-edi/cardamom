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
#include <Foundation/common/Assert.hpp>

namespace Cdmw
{




// ---------------------------------------------------------------------------
//

    class TestServerInterceptor : public PortableInterceptor::ServerRequestInterceptor,
                                  public Cdmw::OrbSupport::RefCountLocalObject
    {

    public:
        

        TestServerInterceptor (const char* host);

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

        void interception(int expected_event_count, const char* method
                          , const char* where,const char * what);
        
    private:

        enum {
          kIntercept_SEND_REPLY, kIntercept_RECEIVE_REQUEST
        };
        int m_expected_intercept_count;
        int m_actual_intercept_count;
        std::string m_host_name;
        std::string m_method_to_intercept;
        int m_event_to_intercept;
        std::string m_what;

        TestServerInterceptor(const TestServerInterceptor&);
        void operator=(const TestServerInterceptor&);

    }; // End class OnInit_impl


// ---------------------------------------------------------------------------
//

    class TestClientInterceptor : public PortableInterceptor::ClientRequestInterceptor,
                                  public Cdmw::OrbSupport::RefCountLocalObject
    {

    public:
        

        //TestClientInterceptor ( PortableInterceptor::ORBInitInfo_ptr info
        TestClientInterceptor (const char* host);

        //
        // IDL to C++ Mapping
        //
        virtual char*
        name() throw (CORBA::SystemException);

        virtual void
        destroy() throw (CORBA::SystemException);

        virtual void
        send_request (PortableInterceptor::ClientRequestInfo_ptr ri)
            throw (CORBA::SystemException, PortableInterceptor::ForwardRequest);

        virtual void
        send_poll (PortableInterceptor::ClientRequestInfo_ptr ri)
            throw (CORBA::SystemException);

        virtual void
        receive_reply (PortableInterceptor::ClientRequestInfo_ptr ri)
            throw (CORBA::SystemException);

        virtual void
        receive_exception(PortableInterceptor::ClientRequestInfo_ptr ri)
            throw (CORBA::SystemException, PortableInterceptor::ForwardRequest);

        virtual void
        receive_other(PortableInterceptor::ClientRequestInfo_ptr ri)
            throw (CORBA::SystemException, PortableInterceptor::ForwardRequest);

        void interception(int expected_event_count, const char* method
                          , const char* where,const char * what);
    private:

        enum {
          kIntercept_SEND_REQUEST, kIntercept_RECEIVE_REPLY
        };
        int         m_expected_intercept_count;
        int         m_actual_intercept_count;
        std::string m_host_name;
        std::string m_method_to_intercept;
        int         m_event_to_intercept;
        std::string m_what;

        TestClientInterceptor(const TestClientInterceptor&);
        void operator=(const TestClientInterceptor&);

    }; // End class OnInit_impl


// ---------------------------------------------------------------------------
//

class TestORBInitializer : public PortableInterceptor::ORBInitializer,
                                                       public Cdmw::OrbSupport::RefCountLocalObject
{
public:

    // constructor
    TestORBInitializer(  const char* process, bool is_server_interceptor = false)
        : m_process(process)
        , m_is_server_interceptor(is_server_interceptor) { }
    
    //destructor
    ~TestORBInitializer() { std::cerr<<"**************** destructor ***********"<<std::endl;}

    //
    // IDL to C++ Mapping
    //
    virtual void
    pre_init(PortableInterceptor::ORBInitInfo_ptr info) 
        throw (CORBA::SystemException) { }

    
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
        if (!m_is_server_interceptor)
        {            
            //
            // Create and register the interceptors
            //
            m_client_interceptor = new Cdmw::TestClientInterceptor(m_process.in());
            PortableInterceptor::ClientRequestInterceptor_var client_interceptor = m_client_interceptor;
            
            try {
                info->add_client_request_interceptor(client_interceptor.in());
            }
            catch(const PortableInterceptor::ORBInitInfo::DuplicateName&) {
                // can't happened while interceptor is not named!
                CDMW_ASSERT(false);
            }
        } else {
            //
            // Create and register the interceptors
            //
            m_server_interceptor = new Cdmw::TestServerInterceptor(m_process.in());
            PortableInterceptor::ServerRequestInterceptor_var server_interceptor = m_server_interceptor;
            
            try {
                info->add_server_request_interceptor(server_interceptor.in());
            }
            catch(const PortableInterceptor::ORBInitInfo::DuplicateName&) {
                // can't happened while interceptor is not named!
                CDMW_ASSERT(false);
            }
        
        }
    }

    void setup_interception(CORBA::Long expected_event_count,
                            const char * method,
                            const char * where,
                            const char * what)
        
    {
        if (!m_is_server_interceptor)
        {            
            std::cout<<"expected_event_count "<<expected_event_count<<" method "<<method<<" where " <<where<<" what "<<what<<std::endl;
            m_client_interceptor->interception(expected_event_count, method, where, what);
        } else {
            std::cout<<"expected_event_count "<<expected_event_count<<" method "<<method<<" where " <<where<<" what "<<what<<std::endl;           m_server_interceptor->interception(expected_event_count, method, where, what);
        }
    }
    
private:

    CORBA::String_var m_process;
    bool m_is_server_interceptor;
    Cdmw::TestClientInterceptor* m_client_interceptor;
    Cdmw::TestServerInterceptor* m_server_interceptor;
    

    };

}; // End namespace Cdmw

#endif // INCL_ONINIT_INTERFACE_IMPL_HPP 

