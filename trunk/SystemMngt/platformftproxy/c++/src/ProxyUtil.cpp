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

#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include "platformftproxy/ProxyUtil.hpp"

namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{

ProxyUtil::ProxyUtil()
    throw (Cdmw::BadParameterException,
	   Cdmw::OutOfMemoryException)
{
    CDMW_NEVER_HERE();
}

ProxyUtil::ProxyUtil(CORBA::ORB_ptr orb)
    throw (Cdmw::BadParameterException,
	   Cdmw::OutOfMemoryException)
{
    CDMW_ASSERT( ! CORBA::is_nil(orb));

    if (CORBA::is_nil(orb)) {
	CDMW_THROW2(Cdmw::BadParameterException, "orb", "nil");
    }

    try {
	m_orb = CORBA::ORB::_duplicate(orb);

	CORBA::Object_var obj = orb->resolve_initial_references("POACurrent");
	m_current = PortableServer::Current::_narrow(obj.in());
    }
    catch (const std::bad_alloc &) {
	CDMW_THROW(Cdmw::OutOfMemoryException);
    } 
    catch (...) { // FIXME handle exceptions correctly
	CDMW_THROW2(Cdmw::BadParameterException, "orb", "");
    }
}


ProxyUtil::~ProxyUtil() throw()
{
}

std::string ProxyUtil::getObjectId() throw (CORBA::SystemException)
{
    PortableServer::ObjectId_var oid = m_current->get_object_id();
    try {
	CORBA::String_var oid_string = 
	    PortableServer::ObjectId_to_string(oid);
	
	return std::string(oid_string.in());
    }
    catch(const CORBA::BAD_PARAM&) {
	throw CORBA::OBJECT_NOT_EXIST();
    }
}


PortableServer::POA_ptr
ProxyUtil::createDefaultServantPOA(CORBA::ORB_ptr orb, 
				   PortableServer::POA_ptr parent_poa,
				   const std::string &name)
 
    throw (Cdmw::BadParameterException, 
	   Cdmw::PlatformMngt::AlreadyExistsException,
	   Cdmw::OutOfMemoryException)
{
    try {
	// Get the POA manager
	PortableServer::POAManager_var POAMgr = parent_poa->the_POAManager();
	
	// Create the policies and the strategies
	CORBA::PolicyList policies(6);
	policies.length(6);
	policies[0] =
	    parent_poa->create_thread_policy(PortableServer::ORB_CTRL_MODEL);
	policies[1] =
	    parent_poa->create_lifespan_policy(PortableServer::PERSISTENT);
	policies[2] =
	    parent_poa->create_id_assignment_policy(PortableServer::USER_ID);
	policies[3] =
	    parent_poa->create_servant_retention_policy(PortableServer::NON_RETAIN);
	policies[4] =
	    parent_poa->create_request_processing_policy(PortableServer::USE_DEFAULT_SERVANT);
	policies[5] =
	    parent_poa->create_id_uniqueness_policy(PortableServer::MULTIPLE_ID);
	
	Cdmw::OrbSupport::StrategyList poaStrategies;
	
	// the PERSISTENT policy is incompatible with the THREADPOOL policy for TAO
#if CDMW_ORB_VDR == tao
	poaStrategies.add_PoaThreadPerConnection();
#else
	poaStrategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);
#endif
	PortableServer::POA_var new_poa = 
	    Cdmw::OrbSupport::OrbSupport::create_POA(parent_poa, name.c_str(),
						     POAMgr.in(), policies,
						     poaStrategies);

	return new_poa._retn();
    }
    catch(const PortableServer::POA::AdapterAlreadyExists&) {
	std::cerr << "ProxyUtil::ProxyUtil catch PortableServer::POA::AdapterAlreadyExists" << std::endl;
	CDMW_THROW(Cdmw::PlatformMngt::AlreadyExistsException);
    }
    catch(const PortableServer::POA::InvalidPolicy&) {
	std::cerr << "ProxyUtil::ProxyUtil catch PortableServer::POA::InvalidPolicy" << std::endl;
	CDMW_THROW2(Cdmw::BadParameterException, "parent_poa","");
    }
    catch(const CORBA::SystemException&) {
	std::cerr << "ProxyUtil::ProxyUtil catch CORBA::SystemException" << std::endl;
	CDMW_THROW2(Cdmw::BadParameterException, "parent_poa","");
    }   
    catch(const std::bad_alloc &) {
	CDMW_THROW(Cdmw::OutOfMemoryException);
    }
    catch(...) {
        std::cerr << "ProxyUtil::ProxyUtil catch" << std::endl;
	CDMW_THROW2(Cdmw::BadParameterException, "parent_poa","");
    }
}
    

} // End of namespace Proxy
} // End of namespace NamingAndRepository
} // End of namespace Cdmw
