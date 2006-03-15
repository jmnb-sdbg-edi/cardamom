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

