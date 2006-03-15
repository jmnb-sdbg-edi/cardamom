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


#ifndef INCL_CDMW_FT_FTINIT_SERVER_REQUEST_INTERCEPTOR_IMPL_HPP
#define INCL_CDMW_FT_FTINIT_SERVER_REQUEST_INTERCEPTOR_IMPL_HPP

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/orbsupport/PortableInterceptor.hpp>

#include <FaultTolerance/idllib/CdmwFTGroupRepository.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp>

#include <FaultTolerance/ftcommon/ReplyRecording.hpp>

namespace Cdmw
{
    
namespace FT
{

namespace FTInit
{

class ServerRequestInterceptor_impl : public PortableInterceptor::ServerRequestInterceptor,
                                      public Cdmw::OrbSupport::RefCountLocalObject
{
public:

    ServerRequestInterceptor_impl(PortableInterceptor::ORBInitInfo_ptr info,
                 const PortableInterceptor::SlotId  ft_domain_id_slot_id,
                 const PortableInterceptor::SlotId  object_group_id_slot_id,
                 const PortableInterceptor::SlotId  object_group_ref_version_slot_id,
                 const PortableInterceptor::SlotId  client_id_slot_id,
                 const PortableInterceptor::SlotId  retention_id_slot_id,
                 const PortableInterceptor::SlotId  expiration_time_slot_id,
                 const PortableInterceptor::SlotId  is_a_ft_request_slot_id,
                 const PortableInterceptor::SlotId  is_request_valid_slot_id);

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

#if TAO_HAS_EXTENDED_FT_INTERCEPTORS == 1
    /**
     * This is a TAO-specific operation to enable returning previous logged replies.
     * NOTA: Only the primary actually returns a previous reply!
     */  
    virtual void
    tao_ft_interception_point(PortableInterceptor::ServerRequestInfo_ptr ri,
                              CORBA::OctetSeq_out ocs)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest);
#endif /*TAO_HAS_EXTENDED_FT_INTERCEPTORS*/

    void set_reply_recorder(Cdmw::FT::ReplyRecording* recorder);

private:
    ServerRequestInterceptor_impl(const ServerRequestInterceptor_impl&);
    ServerRequestInterceptor_impl& operator=(const ServerRequestInterceptor_impl&);

    
    ::FT::FTRequestServiceContext*
    get_FTRequestServiceContext(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::INTERNAL);
    ::FT::FTRequestServiceContext*
    get_service_context(PortableInterceptor::ServerRequestInfo_ptr ri,
                        const char * location_str)
        throw (CORBA::SystemException);
    
    /**
     * This operation extracts the (FT-specific) service contexts that may 
     * be attached to the request. It then sets appropriate PI slots
     * (from now on, FTCurrent can be used to get these info)
     *
     * @return true if this an FT request, false otherwise
     *
     * @exception TODO: describe exceptions...
     */
    inline bool
    get_service_contexts(PortableInterceptor::ServerRequestInfo_ptr ri,
                         const char *                               location_str,
                         ::FT::FTRequestServiceContext_out          ft_request_context,
                         ::FT::TagFTGroupTaggedComponent_out        ft_group_version_context)
        throw (CORBA::SystemException);
    /**
     * This operation insures server-side FT CORBA failover semantics.
     * If it returns with no exception, the request is valid and is safe to be delivered
     * to the target servant.
     * NOTA: This operation does not check the expiration time!
     */
    inline void 
    validate_request(PortableInterceptor::ServerRequestInfo_ptr ri,
                     const char *                               location_str,
                     const ::FT::FTRequestServiceContext &      ft_request_context,
                     const ::FT::TagFTGroupTaggedComponent &    ft_group_version_context)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest);

    /*
     * Returns true if request is an FT request, false otherwise
     */
    inline bool 
    check_incoming_request(PortableInterceptor::ServerRequestInfo_ptr ri,
                           ::FT::FTRequestServiceContext_out   ft_request_context)
      throw (CORBA::SystemException, PortableInterceptor::ForwardRequest);

    inline const char* get_location() throw ();
  
    //
    // Used to encode/decode the call stack information so that
    // it can be used as the service context
    //
    IOP::Codec_var  m_cdrCodec;

    CdmwFT::Location::PrimaryBackupGroupRepository_var m_group_repository;

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

    // did the current request generate a CORBA exception or not?
  PortableInterceptor::SlotId  m_is_request_valid_slot_id;

    Cdmw::FT::ReplyRecording* m_recorder;
};

} // End of namespace FTInit

} // End of namespace FT

} // End of namespace Cdmw

#endif // INCL_CDMW_FT_FTINIT_SERVER_REQUEST_INTERCEPTOR_IMPL_HPP

