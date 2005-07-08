/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
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
*/
/* ===================================================================== */


#include "SystemMngt/platformlibrary/MultipleIdServant_impl.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


MultipleIdServant_impl::MultipleIdServant_impl(CORBA::ORB_ptr orb, const std::string& poaName)
{

    if (CORBA::is_nil(orb))
        CDMW_THROW2(BadParameterException, "orb", "nil");

    // creates the specific POA
    CORBA::Object_var object = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var rootPoa = PortableServer::POA::_narrow(object.in());

    PortableServer::POAManager_var manager = rootPoa->the_POAManager();
	    
    CORBA::PolicyList policies;
    OrbSupport::StrategyList poaStrategyList;

    policies.length(5);
    policies[0] = rootPoa->create_id_uniqueness_policy(PortableServer::MULTIPLE_ID);
    policies[1] = rootPoa->create_lifespan_policy(PortableServer::TRANSIENT);
    policies[2] = rootPoa->create_servant_retention_policy(PortableServer::RETAIN);
    policies[3] = rootPoa->create_id_assignment_policy(PortableServer::USER_ID);
    policies[4] = rootPoa->create_request_processing_policy(
        PortableServer::USE_ACTIVE_OBJECT_MAP_ONLY);

    poaStrategyList.add_PoaThreadPerRequest();

    m_poa = OrbSupport::OrbSupport::create_POA(rootPoa.in(), poaName.c_str(), 
                                               manager.in(), policies, poaStrategyList);

    // gets the POA Current reference
    CORBA::Object_var current = orb->resolve_initial_references("POACurrent");
    m_current = PortableServer::Current::_narrow(current.in());

}


MultipleIdServant_impl::~MultipleIdServant_impl()
{

}


CORBA::Object_ptr MultipleIdServant_impl::activate(const std::string& name)
    throw (BadParameterException, InternalErrorException)
{

    try
    {

        PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(name.c_str());

        m_poa->activate_object_with_id(oid.in(), this);
        
        
        return m_poa->id_to_reference(oid.in());

    }
    catch(const PortableServer::POA::ObjectAlreadyActive&)
    {
        CDMW_THROW2(BadParameterException, "name", "already exists");
    }
    catch(...)
    {
        CDMW_THROW(InternalErrorException);
    }


}


void MultipleIdServant_impl::deactivate(const std::string& name)
    throw (BadParameterException, InternalErrorException)
{

    try
    {

        PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(name.c_str());

        m_poa->deactivate_object(oid.in());
        
    }
    catch(const PortableServer::POA::ObjectNotActive&)
    {
        CDMW_THROW2(BadParameterException, "name", "doesn't exist");
    }
    catch(...)
    {
        CDMW_THROW(InternalErrorException);
    }


}


void MultipleIdServant_impl::destroy()
    throw (InternalErrorException)
{

    try
    {

        // delete poa : the reference counter of activated objects
        //              is decremented 
        // (release the poa ownership to avoid
        //  destruction of not existing object by the var)
        PortableServer::POA* p_poa = m_poa._retn();
        p_poa->destroy(false, false);
        
    }
    catch(...)
    {
        CDMW_THROW(InternalErrorException);
    }

}

std::string MultipleIdServant_impl::getCurrentName()
    throw (OutOfMemoryException, InternalErrorException)
{
    try
    {

        PortableServer::ObjectId_var oid = m_current->get_object_id();
        CORBA::String_var oid_str = PortableServer::ObjectId_to_string(oid.in());

        std::string processName(oid_str.in());

        return processName;

    }
    catch(const PortableServer::Current::NoContext&)
    {
        CDMW_THROW(InternalErrorException);
    }
    catch(const std::bad_alloc&)
    {
        CDMW_THROW(OutOfMemoryException);
    }

}


} // End namespace PlatformMngt
} // End namespace Cdmw


