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

#include "lbinit/LBGroupManagerInterceptor.hpp"
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <iostream>
#include <LoadBalancing/lbstrategy/StrategyFactory_impl.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include "LoadBalancing/lbcommon/StateTransferConfig.hpp"

#if CDMW_ORB_VDR == tao
#include <LoadBalancing/lbcommon/TAO_LB_IOGRFactory.hpp>

#else
#error "Unsupported ORB"
#endif



#define ECHO_HEADER() \
    "[CDMW LBGroupManagerInterceptor] "


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

LBGroupManagerInterceptor_impl::LBGroupManagerInterceptor_impl (CORBA::ORB_ptr orb,
                                                              const char *name, 
                                                              Cdmw::LB::IOGRFactory* iogr_factory)
    : m_orb (CORBA::ORB::_duplicate(orb)),
      m_name(name),
      m_iogr_factory(iogr_factory)
    
{
}
    
void
LBGroupManagerInterceptor_impl::destroy ()
    throw (CORBA::SystemException)
{
  delete m_iogr_factory;
}

char *
LBGroupManagerInterceptor_impl::name ()
    throw (CORBA::SystemException)
{
    return CORBA::string_dup (this->m_name.in ());
}

void
LBGroupManagerInterceptor_impl::receive_request_service_contexts(
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
	
	
	if(!LBConfiguration::M_useRepository)
	{

	    obj = m_orb->resolve_initial_references("LBGroupManager");
	    // get NamingInterface to object_groups (for reading)
	}
	else
	{
	    Cdmw::CommonSvcs::Naming::NamingInterface objGroupsNamingInterface =
		Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface(REPOSITORY_GROUP_MANAGER_GROUP_NAME.c_str());
	 
	    // get reference to Database ObjectGroup from NamingInterface
	    try
	    {
		obj =
		    objGroupsNamingInterface.resolve (GROUP_MANAGER_OBJECT_GROUP_NAME);
	    }
	    catch(const CosNaming::NamingContext::NotFound& e)
	    {
		DEBUG_DUMP(__FILE__ << "  " << __LINE__ );
		DEBUG_DUMP("Resolve " << GROUP_MANAGER_OBJECT_GROUP_NAME << " => NamingContext::NotFound raised!");
	    }
	    catch(const CosNaming::NamingContext::CannotProceed& e)
	    {
		DEBUG_DUMP(__FILE__ << "  " << __LINE__);
		DEBUG_DUMP("Resolve " << GROUP_MANAGER_OBJECT_GROUP_NAME << " => NamingContext::CannotProceed raised!");
	    }
	    catch(const CosNaming::NamingContext::InvalidName& e)
	    {
		DEBUG_DUMP(__FILE__ << "  " << __LINE__);
		DEBUG_DUMP("Resolve " << GROUP_MANAGER_OBJECT_GROUP_NAME << " => NamingContext::InvalidName raised!");
	    }
	    catch (const CORBA::SystemException& e)
	    {
		DEBUG_DUMP(__FILE__ << "  " << __LINE__ );
		DEBUG_DUMP( "Resolve " << GROUP_MANAGER_OBJECT_GROUP_NAME << " => CORBA::SystemException raised!");
	    }
	}	 
	if(!CORBA::is_nil(obj.in()))
	{
	    CdmwLB::LBGroupManager_var group_mgr = CdmwLB::LBGroupManager::_narrow(obj.in());
	     if (sc->context_id == CdmwLB::LB_GROUP_VERSION)
	    {
		lb_service_ctx = m_iogr_factory->decode_group_version_service_context(sc.in());
		DEBUG_DUMP("  LBGroupVersionServiceContext: ");
		DEBUG_ECHO("     Object group ref version: " << lb_service_ctx.object_group_ref_version);
		DEBUG_ECHO("     Object group Id: " << lb_service_ctx.object_group_id);
		PortableGroup::ObjectGroupRefVersion new_version = group_mgr->get_object_group_version_from_gid(lb_service_ctx.object_group_id);
		PortableGroup::ObjectGroup_var object_group = group_mgr->get_object_group_ref_from_id(lb_service_ctx.object_group_id);
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
	
	else
	{
	    DEBUG_DUMP("No Check on the object group version...LB Group Manager is not available");
	}
	
    }
    catch(const CORBA::BAD_PARAM&)
    {
	DEBUG_DUMP("No service context LB_GROUP_VERSION for that request");
	
	// This is not an error while a request can come to a single reference.
	// with no LB service context.
    }
    catch(const ::PortableGroup::ObjectGroupNotFound &)
    {
	DEBUG_DUMP("LBGroupManager Exception: ObjectGroupNotFound raised");
    }
    catch(const CORBA::SystemException &)
    {
	DEBUG_DUMP("No Check on the object group version...LB Group Manager is not available");  
    }
    
}	   

void
LBGroupManagerInterceptor_impl::receive_request(PortableInterceptor::ServerRequestInfo_ptr ri)
    throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
        DEBUG_DUMP( "    receive_request()   " );
}

void
LBGroupManagerInterceptor_impl::send_reply(PortableInterceptor::ServerRequestInfo_ptr ri)
    throw (CORBA::SystemException)
{
    DEBUG_DUMP( "    receive_reply()   " );
}

void
LBGroupManagerInterceptor_impl::send_other(PortableInterceptor::ServerRequestInfo_ptr ri)
    throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    DEBUG_DUMP( "    send_other()   " );

    // nothing to do!
}

void
LBGroupManagerInterceptor_impl::send_exception(PortableInterceptor::ServerRequestInfo_ptr ri)
    throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    DEBUG_DUMP( "    send_exception  " );
    // nothing to do!
}

}//End of namespace LBInit

}//End of namespace LB

}//End of namespace Cdmw
