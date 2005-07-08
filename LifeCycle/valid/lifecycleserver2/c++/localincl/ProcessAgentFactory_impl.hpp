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

#ifndef INCL_VALIDATION_PROCESSAGENTFACTORY_IMPL_HPP 
#define INCL_VALIDATION_PROCESSAGENTFACTORY_IMPL_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "lifecycleserver2/ProcessAgent.skel.hpp"
#include "Foundation/orbsupport/POAThreadingPolicy.hpp"
#include "LifeCycle/lifecycle/FactoryAllocator.hpp"

#include "Foundation/orbsupport/RefCountServantBase.hpp"

#include <string>

namespace Cdmw
{
namespace Validation
{

/**
*Purpose:
*<p>    [class description]
*
*Features:
*<p> [Thread safety, exception safety, performance]
*    [if no pertinent write none ]
*
*@see  [if necessary] 
*@link [if necessary] 
*
*/
class ProcessAgent_impl :
        public virtual POA_Validation::ProcessAgent,
        public virtual Cdmw::OrbSupport::RefCountServantBase
{
public:
    ProcessAgent_impl(CORBA::ORB_ptr          orb,
                      PortableServer::POA_ptr parent) 
        throw(CORBA::SystemException);   
    
    virtual PortableServer::POA_ptr _default_POA() {
        return PortableServer::POA::_duplicate(m_poa.in());
    }
    //
    // IDL:thalesgroup.com/Validation/ProcessAgent/stop_process:1.0
    //
    virtual void stop_process()
        throw(CORBA::SystemException);
private:
    /**
     *[Attribute description]
     */ 
    CORBA::ORB_var                            m_orb;
    PortableServer::POA_var                   m_poa;
}; // End class ProcessAgent_impl
/**
*Purpose:
*<p>    [class description]
*
*Features:
*<p> [Thread safety, exception safety, performance]
*    [if no pertinent write none ]
*
*@see  [if necessary] 
*@link [if necessary] 
*
*/
class ProcessAgentFactory_impl :
        public virtual POA_Validation::ProcessAgentFactory,
        public virtual Cdmw::OrbSupport::RefCountServantBase
{

public:
    
    /**
     * Purpose:
     * <p> [Constructor description if necessary]
     * 
     *@param
     *@exception
     */ 
    ProcessAgentFactory_impl (CORBA::ORB_ptr                           orb, 
                              PortableServer::POA_ptr                  parent,
                              const std::string &                      factory_name,
                              Cdmw::OrbSupport::POAThreadingPolicy &   threading_policy)
        throw(Cdmw::OutOfMemoryException,
              Cdmw::BadParameterException,
              Cdmw::InternalErrorException,
              CORBA::SystemException);
    
    /**
     * Purpose:
     * <p> [ Destructor description if necessary]
     * 
     */ 
    virtual 
    ~ProcessAgentFactory_impl()
        throw();
    
    virtual PortableServer::POA_ptr _default_POA() {
        return PortableServer::POA::_duplicate(m_poa.in());
    }
    
    /**
     * Purpose:
     * <p>
     * 
     *@param
     *@return
     *@exception
     */ 
    //
    // IDL:thalesgroup.com/Validation/ProcessAgentFactory/create_process_agent:1.0
    //
    virtual ::Validation::ProcessAgent_ptr create_process_agent()
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
    //
    /**
     * Supports the following objects:
     *     IDL:thalesgroup.com/CF/Resource:1.0
     *     IDL:thalesgroup.com/CF/Device:1.0
     *     IDL:thalesgroup.com/CF/Application:1.0
     */
    virtual CORBA::Boolean supports(const char* repository_id)
        throw(CORBA::SystemException);
    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/get_supported_objects:1.0
    //
    virtual CdmwLifeCycle::RepositoryIdSeq* get_supported_objects()
        throw(CORBA::SystemException);
    /**
     *[Attribute description]
     */ 
    
    
private:
    /**
     * Purpose:
     * <p> [Constructor description if necessary]
     * 
     *@param
     *@exception
     */ 
    ProcessAgentFactory_impl()
        throw(CORBA::SystemException);
    
    // Hide copy constructor/assignment operator
    ProcessAgentFactory_impl(const ProcessAgentFactory_impl& rhs)
        throw();
    
    ProcessAgentFactory_impl&
    operator=(const ProcessAgentFactory_impl& rhs)
        throw();
    /**
     *[Attribute description]
     */ 
    CORBA::ORB_var                            m_orb;
    PortableServer::POA_var                   m_poa;
    PortableServer::POA_var                   m_objects_poa;
    std::string                               m_factory_name;
    
}; // End class ProcessAgentFactory_impl 




}; // End namespace Validation

// 
namespace LifeCycle
{
// // Named Objects Factory Allocator
// template <>
// Validation::ProcessAgentFactory_impl*
// NamedObjectsFactoryAllocator<Validation::ProcessAgentFactory_impl>::allocate
// (CORBA::ORB_ptr                           orb, 
//  PortableServer::POA_ptr                  parent,
//  CdmwNamingAndRepository::NameDomain_ptr name_domain,
//  const std::string &                      name_domain_name,
//  const std::string &                      factory_name,
//  POAThreadingPolicy &                     threading_policy,
//  int & argc, char** argv)
//     throw(Cdmw::OutOfMemoryException,
//           Cdmw::BadParameterException,
//           Cdmw::InternalErrorException,
//           CORBA::SystemException)
// {
//     try {
//         return new Validation::ProcessAgentFactory_impl(orb,
//                                                         parent,
//                                                         factory_name,
//                                                         threading_policy);
//     } catch (const std::bad_alloc &) {
//         CDMW_THROW(Cdmw::OutOfMemoryException);
//     } catch (...) {
//         throw;
//     }
// }


// Anonymous Objects Factory Allocator
template <>
PortableServer::Servant
AnonymousObjectsFactoryAllocator<Validation::ProcessAgentFactory_impl>::allocate
(CORBA::ORB_ptr                           orb, 
 PortableServer::POA_ptr                  parent,
 const std::string &                      factory_name,
 Cdmw::OrbSupport::POAThreadingPolicy &                     threading_policy,
 int & argc, char** argv)
    throw(Cdmw::OutOfMemoryException,
          Cdmw::BadParameterException,
          Cdmw::InternalErrorException,
          CORBA::SystemException)
{
    try {
        return new Validation::ProcessAgentFactory_impl(orb,
                                                        parent,
                                                        factory_name,
                                                        threading_policy);
    } catch (const std::bad_alloc &) {
        CDMW_THROW(Cdmw::OutOfMemoryException);
    } catch (...) {
        throw;
    }
}

}; // End namespace LifeCycle


namespace Validation
{

typedef Cdmw::LifeCycle::AnonymousObjectsFactoryAllocator<ProcessAgentFactory_impl> ProcessAgentFactory_impl_Allocator;

}; // End namespace Validation

}; // End namespace Cdmw


#endif // INCL_VALIDATION_PROCESSAGENTFACTORY_IMPL_HPP 

