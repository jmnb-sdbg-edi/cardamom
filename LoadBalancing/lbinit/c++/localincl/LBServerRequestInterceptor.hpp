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

/**
 * @brief Implementation for Server Request Interceptor.
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 * @author Antonello Cedrone <a.cedrone@vitrociset.it>
 */

#ifndef INCL_CDMW_LB_LBINIT_SERVER_REQUEST_INTERCEPTOR_H
#define INCL_CDMW_LB_LBINIT_SERVER_REQUEST_INTERCEPTOR_H

// Cdmw Files
#include <Foundation/orbsupport/PortableInterceptor.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <LoadBalancing/idllib/CdmwLBService.stub.hpp>
#include <LoadBalancing/lbcommon/LB_IOGRFactory.hpp>
#include <LoadBalancing/lbcommon/LBConfiguration.hpp>
#include <LoadBalancing/idllib/CdmwLBGroupManager.stub.hpp>
#include <map>
#include <LoadBalancing/lbcommon/GroupRefCache_impl.hpp>

#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>

#include "Foundation/commonsvcs/federation/SimpleMiopUpdateProtocolHandler.hpp"
#include "Foundation/commonsvcs/federation/LocalTopicUpdateManager.hpp"
#include "LoadBalancing/lbcommon/GroupRefCacheMessageCodec.hpp"
#include "LoadBalancing/lbcommon/GroupRefTopicUpdateHandler.hpp"
#include "LoadBalancing/lbcommon/GroupRefCache_impl.hpp"
#include "LoadBalancing/lbcommon/UpdateCommandHandler.hpp"


namespace Cdmw
{
    
namespace LB
{
    
namespace LBInit
{

class ServerRequestInterceptor_impl : public PortableInterceptor::ServerRequestInterceptor,
                                      public Cdmw::OrbSupport::RefCountLocalObject
{
public:

    /// Constructor.
    ServerRequestInterceptor_impl (CORBA::ORB_ptr orb,
				   PortableServer::POA_ptr poa,
                                   const char *name, 
                                   Cdmw::LB::IOGRFactory* iogr_factory);
    //
    // IDL to C++ Mapping
    //
    virtual void destroy ()
        throw(CORBA::SystemException);
    
    virtual char * name ()
        throw(CORBA::SystemException);
    
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
    
    std::string m_miop_corbaloc;

    // The orb reference
    CORBA::ORB_var m_orb;

    // The name of this interceptor.
    CORBA::String_var m_name;
    
    Cdmw::LB::IOGRFactory* m_iogr_factory;

private:
    ::Cdmw::LB::GroupRefUpdateMessageCodec_t              m_codec; 

    // The Group Cache Map
    Cdmw::LB::GroupRefCache_impl  m_group_cache;

    // Group Cache Federation Members
    ::Cdmw::CommonSvcs::SimpleMiopUpdateProtocolHandler   m_protocol_handler;
    ::Cdmw::LB::SetCommand                                m_set_command;
    ::Cdmw::LB::RemoveCommand                             m_remove_command;
    ::Cdmw::LB::GroupRefTopicUpdateHandler_t              m_topic_update_handler;
    
};
    
};//End of namespace LBInit

};//End of namespace LB

};//End of namespace Cdmw

#endif  /* INCL_CDMW_LB_LBINIT_SERVER_REQUEST_INTERCEPTOR_H */
