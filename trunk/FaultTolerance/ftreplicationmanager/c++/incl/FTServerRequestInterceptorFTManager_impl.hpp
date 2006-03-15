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

#ifndef INCL_CDMW_FT_FTINIT_SERVER_REQUEST_INTERCEPTOR_FTMANAGER_IMPL_HPP
#define INCL_CDMW_FT_FTINIT_SERVER_REQUEST_INTERCEPTOR_FTMANAGER_IMPL_HPP

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/orbsupport/PortableInterceptor.hpp>

#include <idllib/CdmwFTGroupRepository.stub.hpp>
#include <idllib/CdmwFTReplicationManager.stub.hpp>
#include <ftreplicationmanager/CdmwFTReplicationManagerRepository.stub.hpp>



namespace Cdmw
{
    
namespace FT
{

namespace ReplicationManager
{

class ServerRequestInterceptorFTManager_impl : public PortableInterceptor::ServerRequestInterceptor,
                                               public Cdmw::OrbSupport::RefCountLocalObject
{
public:

    ServerRequestInterceptorFTManager_impl(PortableInterceptor::ORBInitInfo_ptr info,
                 const PortableInterceptor::SlotId  ft_domain_id_slot_id,
                 const PortableInterceptor::SlotId  object_group_id_slot_id,
                 const PortableInterceptor::SlotId  object_group_ref_version_slot_id,
                 const PortableInterceptor::SlotId  client_id_slot_id,
                 const PortableInterceptor::SlotId  retention_id_slot_id,
                 const PortableInterceptor::SlotId  expiration_time_slot_id,
                 const PortableInterceptor::SlotId  is_a_ft_request_slot_id);

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

private:
    ServerRequestInterceptorFTManager_impl(const ServerRequestInterceptorFTManager_impl&);
    void operator=(const ServerRequestInterceptorFTManager_impl&);

    //
    // Used to encode/decode the call stack information so that
    // it can be used as the service context
    //
    IOP::Codec_var  m_cdrCodec;

    CdmwFT::Location::PrimaryBackupGroupRepository_var m_group_repository;

    CdmwFT::ReplicationManagerRepository_var m_repository;

    // current object ft domain id slot id
    PortableInterceptor::SlotId  m_ft_domain_id_slot_id;

    // current object ft group id slot id
    PortableInterceptor::SlotId  m_object_group_id_slot_id;

    // current object ft group ref version slot id
    PortableInterceptor::SlotId  m_object_group_ref_version_slot_id;

    // current object request client id slot id
    PortableInterceptor::SlotId  m_client_id_slot_id;

    // current object request retention id slot id
    PortableInterceptor::SlotId  m_retention_id_slot_id;

    // current object request expiration time slot id
    PortableInterceptor::SlotId  m_expiration_time_slot_id;

    // current object request slot id where is stored whether the incoming request 
    // is a ft request or not.
    PortableInterceptor::SlotId  m_is_a_ft_request_slot_id;

};

} // End of namespace ReplicationManager

} // End of namespace FT

} // End of namespace Cdmw

#endif // INCL_CDMW_FT_FTINIT_SERVER_REQUEST_INTERCEPTOR_FTMANAGER_IMPL_HPP

