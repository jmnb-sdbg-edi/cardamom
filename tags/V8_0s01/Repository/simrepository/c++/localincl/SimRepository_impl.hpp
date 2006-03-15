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


#ifndef INCL_TOOLS_SIM_REPOSITORY_IMPL_HPP
#define INCL_TOOLS_SIM_REPOSITORY_IMPL_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/CosNaming.stub.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"

#include "Repository/idllib/CdmwNamingAndRepository.skel.hpp"

#include <set>
#include <map>
#include <string>

namespace Cdmw
{
namespace Tools
{

class SimNameDomain_impl;

class SimRepository_impl : 
    public virtual POA_CdmwNamingAndRepository::Repository,
    public virtual PortableServer::RefCountServantBase
{
public:
    SimRepository_impl (CORBA::ORB_ptr orb, 
                     PortableServer::POA_ptr parent,
                     CosNaming::NamingContext_ptr nc,
					 int& argc, char** argv) throw (CORBA::SystemException);
    virtual ~SimRepository_impl() throw();
    
    virtual PortableServer::POA_ptr _default_POA();


    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/Repository/resolve_root_context:1.0
    //
    virtual CosNaming::NamingContext_ptr 
    resolve_root_context(const char* root_id)
        throw(CdmwNamingAndRepository::Repository::NoRootContext,
              CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/Repository/resolve_name_domain:1.0
    //
    virtual CdmwNamingAndRepository::NameDomain_ptr 
    resolve_name_domain(const char* domain_name)
        throw(CdmwNamingAndRepository::NoNameDomain,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException);

private:
    // Hide copy constructor/assignment operator
    SimRepository_impl(const SimRepository_impl&);
    void operator=(const SimRepository_impl&);
    

    typedef std::map<std::string, SimNameDomain_impl*> NameDomainMap;
    typedef std::map<std::string, CosNaming::NamingContext_var> 
        RootContextMap;

    CORBA::ORB_var          m_orb;
    PortableServer::POA_var m_poa;
    RootContextMap          m_root_contexts;
    NameDomainMap           m_domains;

    bool                    m_initialised;

    void init(CosNaming::NamingContext_ptr nc, int& argc, char** argv)
       throw (int);
    CosNaming::NamingContext_ptr 
    bind_new_context(CosNaming::NamingContext_ptr nc,
                     const std::string &          name) throw (int);
};

class SimNameDomain_impl : 
    public virtual POA_CdmwNamingAndRepository::NameDomain,
    public virtual PortableServer::RefCountServantBase
{
public:
    SimNameDomain_impl (CORBA::ORB_ptr               orb, 
	                 PortableServer::POA_ptr      parent,
                     CosNaming::NamingContext_ptr root,
                     std::string                  name,
                     SimRepository_impl*             rep)
		     throw (CORBA::SystemException);

    virtual ~SimNameDomain_impl() throw ();
        
    virtual PortableServer::POA_ptr _default_POA();


    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/new_name:1.0
    //
    virtual CdmwNamingAndRepository::NameDomain::RegistrationId* 
    new_name(const char* name)
        throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_object:1.0
    //
    virtual void 
    register_object(const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
                    CORBA::Object_ptr the_object)
        throw(CdmwNamingAndRepository::NameDomain::InvalidRegistration,
              CdmwNamingAndRepository::NameDomain::AlreadyExists,
              CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_new_object:1.0
    //
    virtual void 
    register_new_object(const char* object_name,
                        CORBA::Object_ptr the_object)
        throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_factory:1.0
    //
    virtual void 
    register_factory(const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
                     CdmwLifeCycle::FactoryBase_ptr the_factory)
        throw(CdmwNamingAndRepository::NameDomain::InvalidRegistration,
              CdmwNamingAndRepository::NameDomain::AlreadyExists,
              CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_new_factory:1.0
    //
    virtual void 
    register_new_factory(const char* factory_name,
                         CdmwLifeCycle::FactoryBase_ptr the_factory)
        throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/resolve_sub_domain:1.0
    //
    virtual CdmwNamingAndRepository::NameDomain_ptr 
    resolve_sub_domain(const char* domain_name)
        throw(CdmwNamingAndRepository::NoNameDomain,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_name_domain:1.0
    //
    virtual void 
    register_name_domain(const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
                         const CdmwNamingAndRepository::NameDomain::NameDomainDescription& the_name_domain)
        throw(CdmwNamingAndRepository::NameDomain::InvalidRegistration,
              CdmwNamingAndRepository::NameDomain::AlreadyExists,
              CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_new_name_domain:1.0
    //
    virtual void 
    register_new_name_domain(const char* domain_name,
                             const CdmwNamingAndRepository::NameDomain::NameDomainDescription& the_name_domain)
        throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/release_name:1.0
    //
    virtual void release_name(const char* name)
        throw(CdmwNamingAndRepository::NameDomain::NotRegistered,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException);
private:
    // Hide copy constructor/assignment operator
    SimNameDomain_impl(const SimNameDomain_impl&);
    void operator=(const SimNameDomain_impl&);

    typedef std::set<std::string> RegistrationSet;

    CORBA::ORB_var                          m_orb;
    PortableServer::POA_var                 m_poa;
    Cdmw::CommonSvcs::Naming::NamingInterface m_root;
    const std::string                       m_name;

    SimRepository_impl *                    m_repository;

    RegistrationSet                         m_registry;

    bool is_valid(const std::string& s) const throw();
    bool is_in_registry(const std::string & s) const throw();
    void insert_in_registry(const std::string & s) throw();
    void remove_from_registry(const std::string & s) throw();
};

} // namespace Tools
} // namespace Cdmw

#endif // INCL_TOOLS_SIM_REPOSITORY_IMPL_HPP

