/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General
 * Public License along with CARDAMOM; see the file COPYING. If not, write to
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * ========================================================================= */

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

#else
#error "Unsupported ORB"
#endif



#define ECHO_HEADER() \
    "[CDMW LBClientRequestInterceptor] "

#define ECHO_ERROR(comment) \
do {\
    std::cerr << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)


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
                                                              const PortableInterceptor::SlotId recurseId,
                                                              const PortableInterceptor::Current_ptr pi_current,
                                                              Cdmw::LB::IOGRFactory* iogr_factory)
    : m_orb (CORBA::ORB::_duplicate(orb)),
      m_name(name),
      m_recurseId(recurseId),
      m_pi_current (PortableInterceptor::Current::_duplicate (pi_current)),
      m_iogr_factory(iogr_factory)
    
{
    m_lb_factory = new Cdmw::LB::StrategyFactory_impl(m_iogr_factory);
    i = 0;
}
    
void
ClientRequestInterceptor_impl::destroy ()
    throw (CORBA::SystemException)
{}

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
    if (strcmp(ri->operation(),"_is_a"))
    {
	DEBUG_DUMP( "    send_request()   " ); 
	CdmwLB::TagLBGroupTaggedComponent* lb_group_tag=0;
	CORBA::Boolean is_LB_request = true;
	PortableGroup::ObjectGroupRefVersion lb_iogr_version=0;
	
	PortableGroup::ObjectGroup_var object_group = CORBA::Object::_duplicate(ri->target());

	try
	{
	    lb_group_tag = m_iogr_factory->decode_profile_with_group_tag(ri->target(), 0);
	}catch(const CdmwLB::TagNotFound&)
	{
	    is_LB_request = false;
	    
	}catch(const CdmwLB::ProfileNotFound& )
	{
	    ECHO_ERROR("Error occured when reading TagLBGroupTaggedComponent");
	    throw CORBA::INV_OBJREF(OrbSupport::INV_OBJREF, CORBA::COMPLETED_NO);
	}
	if(is_LB_request)
	{ 
	    lb_iogr_version = lb_group_tag->object_group_ref_version;
	    CORBA::Long n_profile = m_iogr_factory->profile_count(ri->target());
	   
	    DEBUG_DUMP( "  TagLBGroupTaggedComponent: ");
	    DEBUG_ECHO("        version = "
		       << (int)lb_group_tag->version.major << "."
		       << (int)lb_group_tag->version.minor);
	    DEBUG_ECHO("        lb_domain_id = " << lb_group_tag->lb_domain_id.in());
	    DEBUG_ECHO("        object_group_id = " << lb_group_tag->object_group_id);
	    DEBUG_ECHO("        object_group_ref_version = " << lb_iogr_version);
	    DEBUG_ECHO("        lb_policy = " << lb_group_tag->lb_policy);
	    
	    try
	    {
		m_version_pos =  m_version_map.find(lb_group_tag->object_group_id);
		if(m_version_pos !=  m_version_map.end())
		{
		    lb_iogr_version = m_version_pos->second;
		}
		else
		{
		    m_version_map.insert(std::make_pair(lb_group_tag->object_group_id, lb_iogr_version));
		    
		}
	    }catch (const CORBA::SystemException&)
	    {
		ECHO_ERROR("ERROR: Error occured when creating a strategy policy");
		throw CORBA::INTERNAL(OrbSupport::INTERNALLoadBalancingError, CORBA::COMPLETED_NO);;
	    }  
	    
            // A ServiceContext with the Object Group Version
            // will be embedded into the request.
            IOP::ServiceContext service_context =
                (*(m_iogr_factory->create_group_version_service_context(lb_iogr_version, lb_group_tag->object_group_id)));
	    // Add this context to the service context list.
            ri->add_request_service_context (service_context, 1);
            

	    if(i == 0)  
	    {
		if (n_profile == 1)
		   throw CORBA::TRANSIENT(OrbSupport::TRANSIENT, CORBA::COMPLETED_NO);
		CdmwLB::Strategy_var lb_strategy;
		try
		{
		    m_strategy_pos = m_strategy_map.find(lb_group_tag->object_group_id);
		    if(m_strategy_pos !=  m_strategy_map.end())
		    {
			lb_strategy = m_strategy_pos->second;
		    }
		    else
		    {
			lb_strategy = m_lb_factory->create_strategy(object_group.in());
			DEBUG_DUMP("   LoadBalancing Strategy Selected   ==>   " << CORBA::string_dup(lb_strategy->name()));
			m_strategy_map.insert(std::make_pair(lb_group_tag->object_group_id, lb_strategy.in()));
			
		    }                
		}catch (const CORBA::SystemException&)
		{
			ECHO_ERROR("ERROR: Error occured when creating a strategy policy");
			throw CORBA::INTERNAL(OrbSupport::INTERNALLoadBalancingError, CORBA::COMPLETED_NO);;
		}  
		CORBA::Object_var obj = lb_strategy->next_member(ri);
	        i = 1;
		delete lb_group_tag;
		throw PortableInterceptor::ForwardRequest(obj.in(), 0);   
	    }
	    else
	      delete lb_group_tag;
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
    if (strcmp(ri->operation(),"_is_a"))
    {
	CdmwLB::TagLBGroupTaggedComponent* lb_group_tag=0;
    CORBA::Boolean is_LB_request = true;
    try
    {
        lb_group_tag = m_iogr_factory->decode_profile_with_group_tag(ri->target(), 0);
    }catch(CdmwLB::TagNotFound&)
    {
        is_LB_request = false;
    }
    catch(const CdmwLB::ProfileNotFound& )
    {
        ECHO_ERROR("Error occured when reading TagLBGroupTaggedComponent");
        throw CORBA::INV_OBJREF(OrbSupport::INV_OBJREF, CORBA::COMPLETED_NO);
    }
    
    if(is_LB_request)
    {
        DEBUG_DUMP( "    receive_reply()   " );
	delete lb_group_tag;
        i = 0;	        
    }
    
    }
}

void
ClientRequestInterceptor_impl::receive_exception (PortableInterceptor::ClientRequestInfo_ptr ri)
    throw (CORBA::SystemException,
           PortableInterceptor::ForwardRequest)
{
    if (strcmp(ri->operation(),"_is_a"))
    {
	DEBUG_DUMP( "    receive_exception()   " );
    
    CdmwLB::TagLBGroupTaggedComponent* lb_group_tag=0;
    CORBA::Boolean is_LB_request = true;
    try
    {
        lb_group_tag = m_iogr_factory->decode_profile_with_group_tag(ri->target(), 0);
    }catch(CdmwLB::TagNotFound&)
    {
        is_LB_request = false;
    }
    catch(const CdmwLB::ProfileNotFound& )
    {
        ECHO_ERROR("Error occured when reading TagLBGroupTaggedComponent");
        throw CORBA::INV_OBJREF(OrbSupport::INV_OBJREF, CORBA::COMPLETED_NO);
    }
        
    if(is_LB_request)
    {
        CdmwLB::Strategy_var lb_strategy;
        PortableGroup::ObjectGroup_var object_group = CORBA::Object::_duplicate(ri->target());
     
       
            
        try
        {
            m_strategy_pos = m_strategy_map.find(lb_group_tag->object_group_id);
            if(m_strategy_pos !=  m_strategy_map.end())
            {
                lb_strategy = m_strategy_pos->second;
            }
            else
            {
                lb_strategy = m_lb_factory->create_strategy(object_group.in());
                m_strategy_map.insert(std::make_pair(lb_group_tag->object_group_id, lb_strategy.in()));
                m_version_map.insert(std::make_pair(lb_group_tag->object_group_id, lb_group_tag->object_group_ref_version));
            }
        }catch (const CORBA::SystemException& )
        {
            ECHO_ERROR("ERROR: Error occured when creating a strategy policy");
            throw CORBA::INTERNAL(OrbSupport::INTERNALLoadBalancingError, CORBA::COMPLETED_NO);;
        }  
        CORBA::Object_var obj = lb_strategy->next_member(ri);
	delete lb_group_tag;
        throw PortableInterceptor::ForwardRequest(obj.in(), 0);   
    }
    }
}
    
void
ClientRequestInterceptor_impl::receive_other (PortableInterceptor::ClientRequestInfo_ptr ri)
    throw (CORBA::SystemException,
           PortableInterceptor::ForwardRequest)
{
    if (strcmp(ri->operation(),"_is_a"))
    {
	DEBUG_DUMP( "    receive_other()   " );
    CdmwLB::TagLBGroupTaggedComponent* lb_group_tag=0;
    CORBA::Boolean is_LB_request = true;
    try
    {
        lb_group_tag = m_iogr_factory->decode_profile_with_group_tag(ri->forward_reference(), 0);
    }catch(CdmwLB::TagNotFound&)
    {
        is_LB_request = false;
    }
    catch(const CdmwLB::ProfileNotFound& )
    {
        ECHO_ERROR("Error occured when reading TagLBGroupTaggedComponent");
        throw CORBA::INV_OBJREF(OrbSupport::INV_OBJREF, CORBA::COMPLETED_NO);
    }
        
    if(is_LB_request)
    {
        DEBUG_DUMP( "    receive_other()   " );
        CdmwLB::Strategy_var lb_strategy;
        PortableGroup::ObjectGroup_var object_group = CORBA::Object::_duplicate(ri->forward_reference());
        try
        {
            m_strategy_pos = m_strategy_map.find(lb_group_tag->object_group_id);
            m_version_pos = m_version_map.find(lb_group_tag->object_group_id);
            if(m_strategy_pos !=  m_strategy_map.end())
            {
                m_strategy_map.erase(m_strategy_pos);
            }
            lb_strategy = m_lb_factory->create_strategy(object_group.in());
            m_strategy_map.insert(std::make_pair(lb_group_tag->object_group_id, lb_strategy.in()));
            if(m_version_pos !=  m_version_map.end())
            {
                m_version_map.erase(m_version_pos);
            }
            m_version_map.insert(std::make_pair(lb_group_tag->object_group_id, lb_group_tag->object_group_ref_version));
            
        } catch (const CORBA::SystemException&)
        {
                ECHO_ERROR("ERROR: Error occured when creating a strategy policy");
                throw CORBA::INTERNAL(OrbSupport::INTERNALLoadBalancingError, CORBA::COMPLETED_NO);;
        }
        CORBA::Object_var obj = lb_strategy->next_member(ri);
	delete lb_group_tag;
        throw PortableInterceptor::ForwardRequest(obj.in(), 0);
    }
    }
}
}//End of namespace LBInit

}//End of namespace LB
    
}//End of namespace Cdmw
