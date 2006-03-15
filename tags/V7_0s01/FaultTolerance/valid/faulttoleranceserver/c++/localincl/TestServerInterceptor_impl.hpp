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

#ifndef _TEST_SERVER_INTERCEPTOR_HPP_
#define _TEST_SERVER_INTERCEPTOR_HPP_

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/orbsupport/PortableInterceptor.hpp>
#include <FaultTolerance/idllib/CdmwFTMembersAdmin.stub.hpp>

#include "faulttoleranceserver/TestHello.skel.hpp"
#include <FaultTolerance/idllib/FT.stub.hpp>

class TestServerInterceptor_impl : public Cdmw::Test::ServerRequestInterceptor,
                                   public Cdmw::OrbSupport::RefCountLocalObject
{
public:

    TestServerInterceptor_impl(PortableInterceptor::ORBInitInfo_ptr info,
                               PortableInterceptor::SlotId slotId,
                               std::string i);

    //
    // IDL to C++ Mapping
    //
    virtual char*
    name() throw (CORBA::SystemException);

    virtual void
    destroy() throw (CORBA::SystemException);

    /// Set the references to which requests will be forwarded.
    virtual void forward_references (CORBA::Object_ptr obj1)
        throw (CORBA::SystemException);

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

private:
    //
    // Used to encode/decode the call stack information so that
    // it can be used as the service context
    //
    IOP::Codec_var  m_cdrCodec;

    //
    // My slot id
    //
    PortableInterceptor::SlotId  m_slotId;

    std::string i;
    
    bool m_is_forwared;

    //::CdmwFT::Location::MembersAdmin_var m_testMembersAdmin;
    ::CdmwFT::Location::PrimaryBackupAdmin_var m_testPrimaryBackupAdmin;
    
};
#endif // _TEST_SERVER_INTERCEPTOR_HPP_

