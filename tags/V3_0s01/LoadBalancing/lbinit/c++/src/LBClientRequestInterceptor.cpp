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
 * @brief Implementation for Client Request Interceptor.
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 * @author Antonello Cedrone <a.cedrone@vitrociset.it>
 */

#include "lbinit/LBClientRequestInterceptor.hpp"
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <iostream>
#include <LoadBalancing/lbstrategy/StrategyFactory_impl.hpp>

#if CDMW_ORB_VDR == tao
#include <LoadBalancing/lbcommon/TAO_LB_IOGRFactory.hpp>
#include "tao/Any_Impl.h"
#else
#error "Unsupported ORB"
#endif

#define ECHO_HEADER() \
    "[CDMW LBClientRequestInterceptor] "

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

namespace Cdmw
{
    
namespace LB
{
    
namespace LBInit
{

ClientRequestInterceptor_impl::ClientRequestInterceptor_impl (CORBA::ORB_ptr orb,
                                                              const char *name, 
                                                              Cdmw::LB::IOGRFactory* iogr_factory)
    : m_orb (CORBA::ORB::_duplicate(orb)),
      m_name(name),
      m_iogr_factory(iogr_factory)
    
{
    m_isIn = 0;
    m_lb_factory = new Cdmw::LB::StrategyFactory_impl(m_iogr_factory);

}
    
void
ClientRequestInterceptor_impl::destroy ()
    throw (CORBA::SystemException)
{
    delete m_iogr_factory;
    for(m_lb_cache_pos=m_lb_cache.begin(); m_lb_cache_pos!=m_lb_cache.end();++m_lb_cache_pos)
    {
	delete (m_lb_cache_pos->second);
    }
}

char *
ClientRequestInterceptor_impl::name ()
    throw (CORBA::SystemException)
{
    return CORBA::string_dup (this->m_name.in ());
}

void
ClientRequestInterceptor_impl::send_request (PortableInterceptor::ClientRequestInfo_ptr ri)
    throw(CORBA::SystemException,
          PortableInterceptor::ForwardRequest)
{
    DEBUG_DUMP( "    send_request()   " ); 
    CORBA::Boolean is_LB_request = true;
    PortableGroup::ObjectGroupRefVersion lb_iogr_version=0;
    PortableGroup::ObjectGroup_var object_group = CORBA::Object::_duplicate(ri->target());
    
    CdmwLB::TagLBGroupTaggedComponent* lb_group_tag = 0;
    LBDataStructure* lb_data = 0;
    IOP::TaggedComponent_var  eff_component = 0; 
    CdmwLB::Strategy_var lb_strategy = CdmwLB::Strategy::_nil();
    
    
    /* Take the pointer of the target iogr and put it into a cache (map) */ 
    
    size_t hash_lb_key = object_group->_hash(100000L);
   
    try                    
    {                    
	
	m_lb_cache_pos = m_lb_cache.find(hash_lb_key);
	
	if (m_lb_cache_pos == m_lb_cache.end()) 
	{
	    eff_component = ri-> get_effective_component(CdmwLB::TAG_LB_GROUP);
	    lb_data = new LBDataStructure();
	    IOP::ServiceContext service_context;
	    lb_group_tag = m_iogr_factory->get_lb_tagged_components(eff_component.in());
	    lb_iogr_version = lb_group_tag->object_group_ref_version;
	    
	    DEBUG_DUMP( "  TagLBGroupTaggedComponent: ");
	    DEBUG_ECHO("        version = "
		       << (int)lb_group_tag->version.major << "."
		       << (int)lb_group_tag->version.minor);
	    DEBUG_ECHO("        lb_domain_id = " << lb_group_tag->lb_domain_id.in());
	    DEBUG_ECHO("        object_group_id = " << lb_group_tag->object_group_id);
	    DEBUG_ECHO("        object_group_ref_version = " << lb_iogr_version);
	    DEBUG_ECHO("        lb_policy = " << lb_group_tag->lb_policy);
	    
	    service_context = (*(m_iogr_factory->create_group_version_service_context(lb_iogr_version, lb_group_tag->object_group_id)));
	    lb_data -> obj_group_version = lb_group_tag->object_group_ref_version;
	    lb_data -> svc_cxt = service_context;
	    lb_data -> isLB = 1;
	}
	else 
	    if (m_lb_cache_pos -> second -> isLB == 0) 
		is_LB_request = false;
	    else
		lb_data =  m_lb_cache_pos->second;
	
	
    }
    catch(const CORBA::BAD_PARAM& )
    {
	LBDataStructure* lb_data = new LBDataStructure();
	lb_data -> strategy_ptr = CdmwLB::Strategy::_nil();
	lb_data -> isLB = 0 ;
	m_lb_cache.insert(std::make_pair(hash_lb_key,lb_data));
	is_LB_request = false;
    }	
    catch (const CORBA::SystemException&)
    {
	DEBUG_DUMP("ERROR: Error occured when creating a service context");
	throw CORBA::INTERNAL(OrbSupport::INTERNALLoadBalancingError, CORBA::COMPLETED_NO);;
    }  
    
    if(is_LB_request)
    {   	
	delete lb_group_tag;
	    
	if (strcmp(ri->operation(),"_is_a")) 
	{
	    IOP::ServiceContext service_context = lb_data->svc_cxt; 
	
	    ri->add_request_service_context (service_context, 1);
	
	    if(m_isIn == 0)  
	    {   
		try
		{
		    if(m_lb_cache_pos == m_lb_cache.end())
		    {
			lb_strategy = m_lb_factory->create_strategy(object_group.in());
			std::cerr << "   LoadBalancing Strategy Selected   ==>   " << CORBA::string_dup(lb_strategy->name()) << std::endl;
			lb_data->strategy_ptr = CdmwLB::Strategy::_duplicate(lb_strategy.in());
			m_lb_cache.insert(std::make_pair(hash_lb_key , lb_data));
		    }
		    else
			lb_strategy = lb_data->strategy_ptr;
		    
		}catch (const CORBA::SystemException&)
		{
		    DEBUG_DUMP("ERROR: Error occured when creating a strategy policy");
		    throw CORBA::INTERNAL(OrbSupport::INTERNALLoadBalancingError, CORBA::COMPLETED_NO);;
		}  
		CORBA::Object_var obj = lb_strategy->next_member(ri);
		m_isIn = 1;
		throw PortableInterceptor::ForwardRequest(obj.in(), 0);	
	    }
	    else
		m_isIn = 0;
	}
    
    }
}


void
ClientRequestInterceptor_impl::send_poll (PortableInterceptor::ClientRequestInfo_ptr ri)
    throw (CORBA::SystemException)
{
    DEBUG_DUMP( "    send_poll()   " );
}

void
ClientRequestInterceptor_impl::receive_reply (PortableInterceptor::ClientRequestInfo_ptr ri)
    throw (CORBA::SystemException)
{
    DEBUG_DUMP( "    receive_reply()   " );
}

void
ClientRequestInterceptor_impl::receive_exception (PortableInterceptor::ClientRequestInfo_ptr ri)
    throw (CORBA::SystemException,
           PortableInterceptor::ForwardRequest)
{
	DEBUG_DUMP( "    receive_exception()   " );
	IOP::TaggedComponent_var eff_component = 0;
	CORBA::Boolean is_LB_request = true;
	
	try
	{
	    eff_component = ri->get_effective_component(CdmwLB::TAG_LB_GROUP);
	}
	catch(const CORBA::BAD_PARAM&)
	{
	    is_LB_request = false;
	}
	if(is_LB_request)
	{
	    PortableInterceptor::ReplyStatus rs = ri->reply_status ();
	    if (rs != PortableInterceptor::SYSTEM_EXCEPTION)
	    {
		return;
	    }
	    CORBA::Any_var ex = ri->received_exception ();
	    
	    TAO_OutputCDR cdr;
	   
	    if (!(ex->impl ()->marshal_value (cdr)))
		return;
	    TAO_InputCDR icdr (cdr);
	   
	    CORBA::String_var rep_id;
	    if (!(icdr.read_string (rep_id.out ())))
		throw CORBA::MARSHAL(OrbSupport::INTERNALLoadBalancingError, CORBA::COMPLETED_NO);
	    std::cerr << "System Exception raised: " << rep_id.in() << std::endl;
	    if ((strcmp (rep_id.in (),
			 "IDL:omg.org/CORBA/TRANSIENT:1.0") == 0)
		||
		(strcmp (rep_id.in (),
			 "IDL:omg.org/CORBA/COMM_FAILURE:1.0") == 0)
		||
		(strcmp (rep_id.in (),
			 "IDL:omg.org/CORBA/NO_REPONSE:1.0") == 0)
		||
		(strcmp (rep_id.in (),
			 "IDL:omg.org/CORBA/OBJ_ADAPTER:1.0") == 0))
	    {
		throw PortableInterceptor::ForwardRequest(ri->target(), 0);
	    }
	    else
	    {
		return;
	    }
	}
	else 
	{
	    return;
	}
}
  
    
void
ClientRequestInterceptor_impl::receive_other (PortableInterceptor::ClientRequestInfo_ptr ri)
    throw (CORBA::SystemException,
           PortableInterceptor::ForwardRequest)
{
    DEBUG_DUMP( "    receive_other()   " );
    IOP::TaggedComponent_var eff_component = 0;
    CdmwLB::TagLBGroupTaggedComponent* lb_group_tag=0;
    CORBA::Boolean is_LB_request = true;
    PortableGroup::ObjectGroupRefVersion lb_iogr_version=0;
    LBDataStructure* lb_data = 0;
    PortableGroup::ObjectGroup_var obj_grp = CORBA::Object::_duplicate(ri->target());
    size_t hash_lb_key2 = obj_grp->_hash(100000L);

    try
    {
	eff_component = ri->get_effective_component(CdmwLB::TAG_LB_GROUP);
    }
    catch (CORBA::BAD_PARAM &)
    {
	is_LB_request = false;
    }
    if(is_LB_request)
    {
	PortableGroup::ObjectGroup_var object_group = CORBA::Object::_duplicate(ri->forward_reference());

	CORBA::Long n_profile = m_iogr_factory->profile_count(object_group.in());
	if (n_profile == 1) 
	throw CORBA::TRANSIENT(OrbSupport::TRANSIENT, CORBA::COMPLETED_NO);     

	
	try
	{
	    lb_data = new LBDataStructure();
	    m_lb_cache_pos = m_lb_cache.find(hash_lb_key2);
	    IOP::ServiceContext service_context;
	    lb_group_tag = m_iogr_factory->decode_profile_with_group_tag(object_group.in(), 0);
	    lb_iogr_version = lb_group_tag->object_group_ref_version;
	    lb_data->obj_group_version = lb_iogr_version;
	    lb_data->svc_cxt = (*(m_iogr_factory->create_group_version_service_context(lb_iogr_version, lb_group_tag->object_group_id)));
	    lb_data->strategy_ptr =  m_lb_factory -> create_strategy (object_group.in());
	    lb_data -> isLB = 1;
	    if (m_lb_cache_pos!=m_lb_cache.end())
	      m_lb_cache.erase(m_lb_cache_pos);
	    
	    m_lb_cache.insert(std::make_pair(hash_lb_key2 , lb_data));
	} catch (const CORBA::SystemException&)
	{
	    delete lb_group_tag;
	    DEBUG_DUMP("ERROR: Error occured when creating a strategy policy");
	    throw CORBA::INTERNAL(OrbSupport::INTERNALLoadBalancingError, CORBA::COMPLETED_NO);;
	}
	delete lb_group_tag;
    }
}

}//End of namespace LBInit

}//End of namespace LB
    
}//End of namespace Cdmw
