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

#include "lbinit/LBServerRequestInterceptor.hpp"
#include <iostream>
#include <LoadBalancing/lbstrategy/StrategyFactory_impl.hpp>
#include "LoadBalancing/lbcommon/StateTransferConfig.hpp"



#if CDMW_ORB_VDR == tao
#include <LoadBalancing/lbcommon/TAO_LB_IOGRFactory.hpp>

#else
#error "Unsupported ORB"
#endif



#define ECHO_HEADER() \
    "[CDMW LBServerRequestInterceptor] "


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
    std::cerr << comment << std::endl;\
} while(0)

#endif

static size_t send_repetition = 2;
static const int TOPIC_ID = 1003;


namespace Cdmw
{
    
namespace LB
{
    
namespace LBInit
{

ServerRequestInterceptor_impl::ServerRequestInterceptor_impl (CORBA::ORB_ptr orb,
							      PortableServer::POA_ptr poa,
                                                              const char *name, 
                                                              Cdmw::LB::IOGRFactory* iogr_factory)
    : m_miop_corbaloc(Cdmw::LB::LBConfiguration::miop_corbaloc),
      m_orb (CORBA::ORB::_duplicate(orb)),
      m_name(name),
      m_iogr_factory(iogr_factory),
      m_group_cache(orb, m_codec),
      m_protocol_handler(orb,
			 poa,
			 m_miop_corbaloc,
			 send_repetition),
      m_set_command(m_group_cache),
      m_remove_command(m_group_cache),
      m_topic_update_handler(m_codec) 
    
{
    ::Cdmw::CommonSvcs::LocalTopicUpdateManager::init(&m_protocol_handler);
    
    m_topic_update_handler.register_command(::CdmwLB::SET,
                                            &m_set_command);
    
    m_topic_update_handler.register_command(::CdmwLB::REMOVE,
					    &m_remove_command);
    
    
    ::Cdmw::CommonSvcs::LocalTopicUpdateManager::instance().set_topic_handler(TOPIC_ID,
                                                                              &m_topic_update_handler);


}
    
void
ServerRequestInterceptor_impl::destroy ()
    throw (CORBA::SystemException)
{
    delete m_iogr_factory;
}

char *
ServerRequestInterceptor_impl::name ()
    throw (CORBA::SystemException)
{
    return CORBA::string_dup (this->m_name.in ());
}

void
ServerRequestInterceptor_impl::receive_request_service_contexts(
    PortableInterceptor::ServerRequestInfo_ptr ri)
    throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    DEBUG_DUMP( "    receive_request_service_contexts()   " );
    CORBA::Object_var obj;
    try
    {
        //
        // Get LB_GROUP_VERSION request service context
        //
        IOP::ServiceContext_var sc
            = ri->get_request_service_context(CdmwLB::LB_GROUP_VERSION);
        CdmwLB::LBGroupVersionServiceContext lb_service_ctx ;
        

        if (sc->context_id == CdmwLB::LB_GROUP_VERSION)
        {
            lb_service_ctx = m_iogr_factory->decode_group_version_service_context(sc.in());
            DEBUG_DUMP("  LBGroupVersionServiceContext: ");
            DEBUG_ECHO("     Object group ref version: " << lb_service_ctx.object_group_ref_version);
            DEBUG_ECHO("     Object group Id: " << lb_service_ctx.object_group_id);
            CdmwLB::CacheData* cache_data = m_group_cache.get(lb_service_ctx.object_group_id);
            PortableGroup::ObjectGroupRefVersion new_version = cache_data->object_group_ref_version;
            PortableGroup::ObjectGroup_var object_group = cache_data->iogr;
            CORBA::Long n_profile = m_iogr_factory->profile_count(object_group.in());
                    
            if ((lb_service_ctx.object_group_ref_version < new_version) || (n_profile == 1))
            {
                DEBUG_ECHO("    New Object group ref version: " << new_version);
                // Get the most recent object group reference.
                // Trigger LOCATION_FORWARD_PERM
                throw PortableInterceptor::ForwardRequest(object_group.in(),0); 
            }       
        }
        else
        {
            DEBUG_DUMP("ERROR: Bad context id for IOP::LB_GROUP_VERSION");
            throw CORBA::INTERNAL(OrbSupport::INTERNALLoadBalancingError, CORBA::COMPLETED_NO);
        }
        
    }
    catch(const CORBA::BAD_PARAM&)
    {
        DEBUG_DUMP("No service context LB_GROUP_VERSION for that request");
        
        // This is not an error while a request can come to a single reference.
        // with no LB service context.
    }
    catch(const ::CdmwLB::GroupRefCache::UnknownGroupReference &)
    {
        DEBUG_DUMP("GroupRefCache Exception: UnknownGroupReference raised");
    }
    catch(const CORBA::SystemException &)
    {
        DEBUG_DUMP("No Check on the object group version...");  
    }
    
}          

void
ServerRequestInterceptor_impl::receive_request(PortableInterceptor::ServerRequestInfo_ptr ri)
    throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
        DEBUG_DUMP( "    receive_request()   " );
}

void
ServerRequestInterceptor_impl::send_reply(PortableInterceptor::ServerRequestInfo_ptr ri)
    throw (CORBA::SystemException)
{
    DEBUG_DUMP( "    receive_reply()   " );
}

void
ServerRequestInterceptor_impl::send_other(PortableInterceptor::ServerRequestInfo_ptr ri)
    throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    DEBUG_DUMP( "    send_other()   " );

    // nothing to do!
}

void
ServerRequestInterceptor_impl::send_exception(PortableInterceptor::ServerRequestInfo_ptr ri)
    throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    DEBUG_DUMP( "    send_exception  " );
    // nothing to do!
}

}//End of namespace LBInit

}//End of namespace LB

}//End of namespace Cdmw
