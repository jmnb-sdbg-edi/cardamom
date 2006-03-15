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


#include <iostream>

#include "simrepository/SimRepository_impl.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"

namespace
{
    const char* FACTORIES_CONTEXT = "Factories";
}

namespace Cdmw
{
namespace Tools
{


SimRepository_impl::SimRepository_impl (CORBA::ORB_ptr orb, 
                     PortableServer::POA_ptr parent,
                     CosNaming::NamingContext_ptr nc,
					 int& argc, char** argv) throw (CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(parent)),
      m_initialised(false)
{

    try {
        init(nc,argc,argv);
        m_initialised = true;
    } catch (int & i) {
        std::cerr << "REPOSITORY: Initialisation failed (" << i <<")."
            << std::endl;
    }
}


CosNaming::NamingContext_ptr 
SimRepository_impl::bind_new_context(CosNaming::NamingContext_ptr nc, 
                                  const std::string &          name)
    throw (int)
{
    Cdmw::CommonSvcs::Naming::NamingInterface ni(nc);

    try {
        (void)ni.bind_new_context(name,false);
    } catch (const CosNaming::NamingContext::AlreadyBound &) {
        // Good...
    } catch (...) {
        std::cerr << "Cannot bind <" << name << "> context"
            << std::endl;
        throw -1;
    }
    CORBA::Object_var o = ni.resolve(name);
    
    CosNaming::NamingContext_var nc__ =
            CosNaming::NamingContext::_narrow(o.in());
    return nc__._retn();
}





void SimRepository_impl::init(CosNaming::NamingContext_ptr nc, 
                           int& argc, char** argv)
                           throw (int)
{
    std::string name(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);

    // get default root context
    CosNaming::NamingContext_var default_root_nc =
        bind_new_context(nc,name);
    m_root_contexts[name] = default_root_nc;   

    Cdmw::CommonSvcs::Naming::NamingInterface default_ni(default_root_nc.in());

    //
    // Create persistent POA
    //
    PortableServer::POAManager_var poaManager = m_poa -> the_POAManager();
    CORBA::PolicyList policies;
    policies.length(1);
    policies[0] =
        m_poa -> create_lifespan_policy(PortableServer::PERSISTENT);
    PortableServer::POA_var dom_poa =
        m_poa -> create_POA("NameDomains", poaManager.in(), policies);

   // This installs initial bindings for NameDomains
   int i = 0;
   while(i < argc)
   {
      const char* option = argv[i];
      if(strcmp(option, "-DOM") == 0)
      {
         if(i + 1 >= argc)
         {
            std::cerr << "Argument expected for -DOM" << std::endl;
            throw i;
         }
         
         std::string dom(argv[i+1]);
         
         NameDomainMap::iterator domain_pos = m_domains.find(dom);
         if (domain_pos  == m_domains.end()) {
             // add a name domain
             SimNameDomain_impl * nd 
                 = new SimNameDomain_impl (m_orb.in(),
                                        dom_poa.in(),
                                        default_root_nc.in(),
                                        dom,
                                        this);
             m_domains[dom]=nd;
	         // activate object
		     try {
				 dom_poa->activate_object(nd);
		     } catch (const PortableServer::POA::ObjectAlreadyActive&) {
		         std::cerr << __FILE__ << "(" << __LINE__ << ") ObjectAlreadyActive" << std::endl;
                 throw i;
		     } catch (const PortableServer::POA::ServantAlreadyActive&) {
		         std::cerr << __FILE__ << "(" << __LINE__ << ") ServantAlreadyActive" << std::endl;
                 throw i;
		     } catch (const PortableServer::POA::WrongPolicy&) {
		         std::cerr << __FILE__ << "(" << __LINE__ << ") WrongPolicy" << std::endl;
                 throw i;
		     }catch (...) {
		         std::cerr << __FILE__ << "(" << __LINE__ << ") ..." << std::endl;
                 throw i;
		     }
             // create associated naming context
             try {
                 (void)default_ni.bind_new_context(dom,true);
             } catch (Cdmw::CommonSvcs::Naming::InvalidNameException & ex) {
                 std::cerr << "Unvalid name " << ex.what() << std::endl;
                 throw i;
             } catch (...) {
                 std::cerr << "Unable to bind <" << dom << "> naming context."
                     << std::endl;
                 throw i;
             }
         }
         else {
            std::cerr << "Name Domain <" << dom << "> already present." 
                << std::endl;
            throw i;
         }
         for(int j = i ; j + 2 < argc ; j++)
             argv[j] = argv[j + 2];
         
         argc -= 2;
      }
      else if(strcmp(option, "-ROOT") == 0)
      {
         if(i + 1 >= argc)
         {
            std::cerr << "Argument expected for -ROOT" << std::endl;
            throw i;
         }
         
         std::string root(argv[i+1]);

         RootContextMap::iterator context_pos = m_root_contexts.find(root);
         if (context_pos  == m_root_contexts.end()) {
             // add a root context
             CosNaming::NamingContext_var root_nc =
                 bind_new_context(nc,root);
             m_root_contexts[root] = root_nc;
         }
         else {             
            std::cerr << "Root Naming Context <" << root << "> already present." 
                << std::endl;
            throw i;
         }
         for(int j = i ; j + 2 < argc ; j++)
             argv[j] = argv[j + 2];
         argc -= 2;
      }
      else
         i++;
   }
}

SimRepository_impl::~SimRepository_impl() throw()
{
    // Remove all bindings
    NameDomainMap::iterator domain_pos = m_domains.begin();

    while (domain_pos != m_domains.end())
    {        
        SimNameDomain_impl * nd= domain_pos->second;
//        domain_pos = m_domains.erase(domain_pos);
        //m_domains.erase(domain_pos++);
        nd->_remove_ref();
        domain_pos++;
    }
    m_domains.clear();
}
    
PortableServer::POA_ptr SimRepository_impl::_default_POA()
{
	return PortableServer::POA::_duplicate(m_poa.in());
}


//
// IDL:thalesgroup.com/CdmwNamingAndRepository/Repository/resolve_root_context:1.0
//
CosNaming::NamingContext_ptr 
SimRepository_impl::resolve_root_context(const char* root_id)
    throw(CdmwNamingAndRepository::Repository::NoRootContext,
          CORBA::SystemException)
{    
    if (!m_initialised)
        throw CORBA::INTERNAL();
    
    RootContextMap::iterator context_pos = m_root_contexts.find(root_id);
    if (context_pos  == m_root_contexts.end())
        throw CdmwNamingAndRepository::Repository::NoRootContext();
    else {
        CosNaming::NamingContext_var nc = context_pos->second;
        return nc._retn();
    }
}

//
// IDL:thalesgroup.com/CdmwNamingAndRepository/Repository/resolve_name_domain:1.0
//
CdmwNamingAndRepository::NameDomain_ptr 
SimRepository_impl::resolve_name_domain(const char* domain_name)
        throw(CdmwNamingAndRepository::NoNameDomain,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException)
{  
    if (!m_initialised)
        throw CORBA::INTERNAL();

    // Is the name valid
    try {
        CosNaming::Name_var name 
	        = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(domain_name);
    } catch (const Cdmw::CommonSvcs::Naming::InvalidNameException& ) {
        throw CdmwNamingAndRepository::InvalidName();
    }
    NameDomainMap::iterator domain_pos = m_domains.find(domain_name);
    if (domain_pos  == m_domains.end())
        throw CdmwNamingAndRepository::NoNameDomain();
    else {
        SimNameDomain_impl * nd = domain_pos->second;
        return nd->_this();
    }
}

//**************************************************************//
//**                      SimNameDomain_impl                     **//
//**************************************************************//
SimNameDomain_impl::SimNameDomain_impl (CORBA::ORB_ptr               orb, 
                                        PortableServer::POA_ptr      parent,
                                        CosNaming::NamingContext_ptr root,
                                        std::string                  name,
                                        SimRepository_impl*             rep)
  throw (CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(parent)),
      m_root(root),
      m_name(name),
      m_repository(rep)
{
}

SimNameDomain_impl::~SimNameDomain_impl() throw()
{
}
        
PortableServer::POA_ptr SimNameDomain_impl::_default_POA()
{
	return PortableServer::POA::_duplicate(m_poa.in());
}


// Checks that the name contains a single name component
bool SimNameDomain_impl::is_valid(const std::string& s) const throw()
{
    bool valid = false;
    try
    {
        CosNaming::Name_var name = m_root.to_name(s);
        if (name->length() == 1)
            valid = true;
    } catch (const Cdmw::CommonSvcs::Naming::InvalidNameException & ) {
    }
    return valid;
}

bool SimNameDomain_impl::is_in_registry(const std::string & s) const throw()
{
    return m_registry.find(s) != m_registry.end();
}

void SimNameDomain_impl::insert_in_registry(const std::string & s) throw()
{
    m_registry.insert(s);
}
void SimNameDomain_impl::remove_from_registry(const std::string & s) throw()
{
    m_registry.erase(s);
}
    
//
// IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/new_name:1.0
//
CdmwNamingAndRepository::NameDomain::RegistrationId* 
SimNameDomain_impl::new_name(const char* name)
    throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{
    if (!is_valid(name))
        throw CdmwNamingAndRepository::InvalidName();

    if (is_in_registry(name))
        throw CdmwNamingAndRepository::NameDomain::AlreadyExists();
    try
    {
        std::string sname(m_name);
        sname += "/";
        sname += name;
        CORBA::Object_var obj = m_root.resolve(sname);
        throw CdmwNamingAndRepository::NameDomain::AlreadyExists();
    } catch (const CosNaming::NamingContext::NotFound &) {
        insert_in_registry(name);
        CORBA::Any_var id = new CORBA::Any;
        *id <<= name;
        return id._retn();
    } catch (const Cdmw::CommonSvcs::Naming::InvalidNameException &) {
        throw CORBA::BAD_PARAM();
    } catch (const CORBA::Exception &) {
        throw;
    }
    return 0; // just to silence the compiler
}

//
// IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_object:1.0
//
void 
SimNameDomain_impl::register_object(const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
                                    CORBA::Object_ptr the_object)
    throw(CdmwNamingAndRepository::NameDomain::InvalidRegistration,
          CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CORBA::SystemException)
{
    const char* n;
    reg_id >>= n;
    if (!is_in_registry(n))
        throw CdmwNamingAndRepository::NameDomain::InvalidRegistration();

    try {
        std::string name(m_name);
        name += "/";
        name += n;
        m_root.bind(name,the_object,false);
        // Invalidate Registration ID
        remove_from_registry(n);
    } catch (const CosNaming::NamingContext::AlreadyBound& ) {
        throw CdmwNamingAndRepository::NameDomain::AlreadyExists();
    } catch (const Cdmw::CommonSvcs::Naming::InvalidNameException &) {
        throw CdmwNamingAndRepository::NameDomain::InvalidRegistration();
    } catch (const CORBA::SystemException &) {
        throw;
    } catch (const CORBA::Exception&) {
        throw CORBA::INTERNAL();
    }
}

//
// IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_new_object:1.0
//
void 
SimNameDomain_impl::register_new_object(const char* object_name,
                                        CORBA::Object_ptr the_object)
        throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException)
{
    // Check validity of the name
    if (!is_valid(object_name))
        throw CdmwNamingAndRepository::InvalidName();

    if (is_in_registry(object_name))
        throw CdmwNamingAndRepository::NameDomain::AlreadyExists();

    try {
        std::string name(m_name);
        name += "/";
        name += object_name;
        m_root.bind(name,the_object,false);
    } catch (const CosNaming::NamingContext::AlreadyBound& ) {
        throw CdmwNamingAndRepository::NameDomain::AlreadyExists();
    } catch (const Cdmw::CommonSvcs::Naming::InvalidNameException &) {
        throw CdmwNamingAndRepository::NameDomain::InvalidRegistration();
    } catch (const CORBA::SystemException &) {
        throw;
    } catch (const CORBA::Exception&) {
        throw CORBA::INTERNAL();
    }
}

//
// IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_factory:1.0
//
void 
SimNameDomain_impl::register_factory(const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
                                     CdmwLifeCycle::FactoryBase_ptr the_factory)
    throw(CdmwNamingAndRepository::NameDomain::InvalidRegistration,
              CdmwNamingAndRepository::NameDomain::AlreadyExists,
              CORBA::SystemException)
{
    register_object(reg_id,the_factory);
    // Bind factory in "Factories"
    
    std::string ctx(m_name);
    const char* s;
    reg_id >>= s;
    try {
        ctx += "/";
        ctx += FACTORIES_CONTEXT;
        std::string name (ctx);
        name += "/";
        name += s;
        m_root.bind(name,the_factory,false);
    } catch(const CosNaming::NamingContext::NotFound &) {
        try {
            Cdmw::CommonSvcs::Naming::NamingInterface ni = 
                m_root.bind_new_context(ctx,false);
            ni.bind(s,the_factory,false);
        } catch (...) {
            // bad news!
        }
    }
}

//
// IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_new_factory:1.0
//
void 
SimNameDomain_impl::register_new_factory(const char* factory_name,
                                         CdmwLifeCycle::FactoryBase_ptr the_factory)
    throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{
    register_new_object(factory_name,the_factory);
    
    std::string ctx(m_name);
    // Bind factory in "Factories"
    try {
        ctx += "/";
        ctx += FACTORIES_CONTEXT;
        std::string name (ctx);
        name += "/";
        name += factory_name;
        m_root.bind(name,the_factory,false);
    } catch(const CosNaming::NamingContext::NotFound &) {
        try {
            Cdmw::CommonSvcs::Naming::NamingInterface ni = 
                m_root.bind_new_context(ctx,false);
            ni.bind(factory_name,the_factory,false);
        } catch (...) {
            // bad news!
        }
    }
}

//
// IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/resolve_sub_domain:1.0
//
CdmwNamingAndRepository::NameDomain_ptr 
SimNameDomain_impl::resolve_sub_domain(const char* domain_name)
    throw(CdmwNamingAndRepository::NoNameDomain,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{
    // Check validity of domain_name
    try {
        CosNaming::Name_var name = m_root.to_name(domain_name);
    } catch (const Cdmw::CommonSvcs::Naming::InvalidNameException & ) {
        throw CdmwNamingAndRepository::InvalidName();
    }
    
    std::string name (m_name);
    name += "/";
    name += domain_name;

    return m_repository->resolve_name_domain(name.c_str());
}

//
// IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_name_domain:1.0
//
void 
SimNameDomain_impl::register_name_domain(const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
                                         const CdmwNamingAndRepository::NameDomain::NameDomainDescription& the_name_domain)
        throw(CdmwNamingAndRepository::NameDomain::InvalidRegistration,
              CdmwNamingAndRepository::NameDomain::AlreadyExists,
              CORBA::SystemException)
{
   throw CORBA::NO_IMPLEMENT();
}

//
// IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_new_name_domain:1.0
//
void 
SimNameDomain_impl::register_new_name_domain(const char* domain_name,
                                             const CdmwNamingAndRepository::NameDomain::NameDomainDescription& the_name_domain)
    throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{
   throw CORBA::NO_IMPLEMENT();
}

//
// IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/release_name:1.0
//
void SimNameDomain_impl::release_name(const char* name)
        throw(CdmwNamingAndRepository::NameDomain::NotRegistered,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException)
{
    // Check validity of the name
    if (!is_valid(name))
        throw CdmwNamingAndRepository::InvalidName();

    if (is_in_registry(name))
       remove_from_registry(name);
    else {
       try {
          std::string n(m_name);
          n += "/";
          n += name;
          m_root.unbind(n);
          // If its a factory, then remove it from "Factories"
          try {
              std::string f(m_name);
              f += "/";
              f += FACTORIES_CONTEXT;
              f += "/";
              f += name;
              m_root.unbind(f);
          } catch (...) {
              // continue anyway ...
          }
       } catch (const CosNaming::NamingContext::NotFound& ) {
          throw CdmwNamingAndRepository::NameDomain::NotRegistered();
       } catch (const Cdmw::CommonSvcs::Naming::InvalidNameException &) {
          throw CORBA::INTERNAL();
       } catch (const CORBA::SystemException &) {
          throw;
       } catch (const CORBA::Exception&) {
          throw CORBA::INTERNAL();
       }
    }

}

} // namespace Tools
} // namespace Cdmw

