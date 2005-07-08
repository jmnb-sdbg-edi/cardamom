/* =========================================================================== *
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
 * =========================================================================== */


#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/common/Exception.hpp>

#include "HelloUserFactory_impl.hpp"


namespace Cdmw
{
namespace Tutorial
{
    

HelloInterface_impl::HelloInterface_impl (CORBA::ORB_ptr orb,
                                          PortableServer::POA_ptr parent) 
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(parent))
{
}


HelloInterface_impl::~HelloInterface_impl()
    throw()
{
}


//
// IDL:thalesgroup.com/Hello/HelloInterface/display_hello:1.0
//
void HelloInterface_impl::display_hello ()
     throw(CORBA::SystemException)                  
{
    std::cout << "<####### HELLO ######>" << std::endl;
}

    

HelloUserFactory_impl::HelloUserFactory_impl(
                      CORBA::ORB_ptr  orb,
                      PortableServer::POA_ptr parent,
                      const std::string& factory_name,
                      Cdmw::OrbSupport::POAThreadingPolicy& threading_policy)
    throw(Cdmw::OutOfMemoryException,
          Cdmw::BadParameterException,
          Cdmw::InternalErrorException,
          CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(parent)),
      m_objects_poa(PortableServer::POA::_nil()),
      m_factory_name(factory_name)
{
    //
    // Create POA for use by the factory
    //
    // Service Life cycle framework use the following POA policies
    //
    // Lifespan policy            = TRANSIENT
    // Id Assignment policy       = SYSTEM_ID
    // Id Uniqueness policy       = UNIQUE_ID
    // Implicit Activation policy = NO_IMPLICIT_ACTIVATION
    // Request Processing policy  = USE_ACTIVE_OBJECT_MAP_ONLY;
    // Servant Retention policy   = RETAIN;
    //
    PortableServer::POAManager_var poaManager = parent->the_POAManager();
    
    CORBA::PolicyList policies;
    policies.length(6);
    policies[0] =
        parent -> create_id_assignment_policy(PortableServer::SYSTEM_ID);
    policies[1] =
        parent -> create_lifespan_policy(PortableServer::TRANSIENT);
    policies[2] =
        parent -> create_servant_retention_policy(PortableServer::RETAIN);
    policies[3] =
        parent -> create_id_uniqueness_policy(PortableServer::UNIQUE_ID);
    policies[4] = parent ->
        create_request_processing_policy(PortableServer::USE_ACTIVE_OBJECT_MAP_ONLY);
    policies[5] = parent ->
     create_implicit_activation_policy(PortableServer::NO_IMPLICIT_ACTIVATION);

    Cdmw::OrbSupport::StrategyList poaStrategies;
    // Strategies with MULTITHREAD policy will accept Single Thread and
    // Multi Thread strategies. However, SERIALIZE policy will only allow
    // Single Thread policy.
    threading_policy.append_to_StrategyList(poaStrategies);

    try 
    {
        m_objects_poa = Cdmw::OrbSupport::OrbSupport::create_POA(m_poa.in(),
                                                                  m_factory_name.c_str(),
                                                                  poaManager.in(), 
                                                                  policies,
                                                                  poaStrategies);    
    
    }
    catch (const std::bad_alloc & )
    {
        // This is to avoid Assertion failure in debug mode as internal ref count
        // equals 1 (CORBA C++ mapping oblige!)
        _set_ref_count(0);
        CDMW_THROW(Cdmw::OutOfMemoryException);
    } 
    catch (const BadParameterException & ) 
    {
        // This is to avoid Assertion failure in debug mode as internal ref count
        // equals 1 (CORBA C++ mapping oblige!)
        _set_ref_count(0);
        throw;
    } 
    catch (const Cdmw::InternalErrorException & ) 
    {
        // This is to avoid Assertion failure in debug mode as internal ref count
        // equals 1 (CORBA C++ mapping oblige!)
        _set_ref_count(0);
        throw;
    } 
    catch (const PortableServer::POA::AdapterAlreadyExists &)
    {
        // This is to avoid Assertion failure in debug mode as internal ref count
        // equals 1 (CORBA C++ mapping oblige!)
        _set_ref_count(0);
        throw CORBA::INTERNAL(0,CORBA::COMPLETED_YES);
    } 
    catch (const PortableServer::POA::InvalidPolicy &) 
    {
        // This is to avoid Assertion failure in debug mode as internal ref count
        // equals 1 (CORBA C++ mapping oblige!)
        _set_ref_count(0);
        throw CORBA::INTERNAL(0,CORBA::COMPLETED_YES);
    } 
    catch (const CORBA::SystemException &) 
    {
        // This is to avoid Assertion failure in debug mode as internal ref count
        // equals 1 (CORBA C++ mapping oblige!)
        _set_ref_count(0);
        throw; // rethrow
    }
}

HelloUserFactory_impl::~HelloUserFactory_impl()
    throw()
{
}



Hello::HelloInterface_ptr HelloUserFactory_impl::create_helloInterface ()
    throw(CORBA::SystemException)
{
    try 
    { 
        HelloInterface_impl *proc = new HelloInterface_impl(m_orb.in(),m_objects_poa.in());
        
        PortableServer::ServantBase_var servant = proc;
        PortableServer::ObjectId_var oid =
                   m_objects_poa->activate_object(servant.in());
                   
        std::cout << "<<HelloUserFactory>> HelloInterface servant created" << std::endl;

        return proc->_this();
        
        
    } 
    catch( const std::bad_alloc &) 
    {
        throw CORBA::NO_MEMORY(0,CORBA::COMPLETED_NO);
    } 
    catch (const CORBA::SystemException & ) 
    {
        throw;
    } 
    catch (...) 
    {
        throw CORBA::INTERNAL(0,CORBA::COMPLETED_NO);
    }
}


//
// IDL:thalesgroup.com/CdmwLifeCycle/lifecycle/FactoryBase/supports:1.0
// 
CORBA::Boolean HelloUserFactory_impl::supports(const char* repository_id)
    throw(CORBA::SystemException)
{
    return 
        (strcmp(repository_id,"IDL:thalesgroup.com/Hello/HelloInterface:1.0") == 0);
}

//
// IDL:thalesgroup.com/CdmwLifeCycle/lifecycle/FactoryBase/get_supported_objects:1.0
//
CdmwLifeCycle::RepositoryIdSeq* 
HelloUserFactory_impl::get_supported_objects()
    throw(CORBA::SystemException)
{
    CdmwLifeCycle::RepositoryIdSeq_var rep_ids =
                           new CdmwLifeCycle::RepositoryIdSeq(1);
    rep_ids->length (1);
    (*rep_ids)[0] = CORBA::string_dup("IDL:thalesgroup.com/Hello/HelloInterface:1.0");

    return rep_ids._retn();
}


}; // End namespace Tutorial
}; // End namespace Cdmw
