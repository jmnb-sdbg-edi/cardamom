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

#include "namingandrepositoryproxy/RepositoryProxy.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "Foundation/common/Assert.hpp"


namespace Cdmw
{
namespace NamingAndRepository
{

RepositoryProxy::RepositoryProxy
(CORBA::ORB_ptr orb,
 PortableServer::POA_ptr poa,
 const std::string& application_name,
 const std::string& process_name,
 CdmwNamingAndRepository::RepositoryExt_ptr repos)
  throw (CdmwNamingAndRepository::Repository::NoRootContext,
	 CosNaming::NamingContext::NotFound,
	 CosNaming::NamingContext::AlreadyBound,
	 CosNaming::NamingContext::CannotProceed,
	 CosNaming::NamingContext::InvalidName,
	 CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa)),
      m_repository(CdmwNamingAndRepository::RepositoryExt::_duplicate(repos)),
      m_namedomain_proxy_servant(orb, poa, repos),
      m_namingcontext_proxy_servant(orb, poa, repos)
{
    // Create context to hold new contexts
    std::string context_holder_name;
    CosNaming::NamingContext_var admin_root;
    
    // Build the name of the holder context
    // Pattern is :
    //    hostname/appli_name/process_name/CONTEXTS
    CosNaming::Name_var name = new CosNaming::Name(4);
    name->length(4);
    name[0L].id = Cdmw::OsSupport::OS::get_hostname().c_str();
    name[0L].kind = "";
    name[1L].id = application_name.c_str();
    name[1L].kind = "";
    name[2L].id = process_name.c_str();
    name[2L].kind = "";
    name[3L].id = "CONTEXTS";
    name[3L].kind = "";
    context_holder_name = 
       Cdmw::CommonSvcs::Naming::NamingInterface::to_string(name);

    admin_root =
      repos->resolve_root_context(Cdmw::Common::Locations::ADMIN_ROOT_CONTEXT_ID);
    CDMW_ASSERT(!CORBA::is_nil(admin_root.in()));
    
    Cdmw::CommonSvcs::Naming::NamingInterface ni (admin_root.in());
        
    ni.bind_new_context(context_holder_name, true);

    m_namingcontext_proxy_servant.set_base_name(name.in());
}

/**
 * This operation retrieves the name domain specified by the name.
 * Name domains are defined within the default root context only.
 * 
 * @param domain_name The full name of the name domain.
 * 
 * @return The name domain.
 * 
 * @exception NoNameDomain if the domain_name does not refer to a
 * Name Domain.
 * @exception InvalidName if the name has an illegal form.
 */
CdmwNamingAndRepository::NameDomain_ptr
RepositoryProxy::resolve_name_domain(const char* domain_name)
    throw (CdmwNamingAndRepository::NoNameDomain,
           CdmwNamingAndRepository::InvalidName,
           CORBA::SystemException)
{
    if (m_namedomain_map.find(domain_name) == m_namedomain_map.end())
    {
        m_namedomain_map[domain_name] =
            m_namedomain_proxy_servant.create(domain_name);
    }

    return CdmwNamingAndRepository::NameDomain::_duplicate
        (m_namedomain_map[domain_name].in());
}

/**
 * This operation retrieves the root naming context specified by the
 * identifier. The Default Root Context has <B>DEFAULT_ROOT_CONTEXT</B>
 * as identifier.
 * <p>
 * <B><U>NOTA</U>:</B>
 * <OL TYPE=0 START=1>
 * <LI>Only the Default Root Context can hold name domain contexts.
 * <LI>The destroy operation of the RootContext should raise a
 * CORBA::NO_PERMISSION system exception if accessed through
 * the public CosNaming::NamingContext interface.
 * This should only be allowed via an administrative interface.
 * <LI>Unbinding Name Domain contexts should be prohibited through
 * the Default Root Context public CosNaming::NamingContext
 * interface (raise a CORBA::NO_PERMISSION system exception).
 * This should only be allowed via an administrative interface.
 * </OL>
 * 
 * @param root_id The identifier of the root naming context. Default
 * value is DEFAULT_ROOT_CONTEXT.
 * 
 * @return The root naming context of the CDMW Naming and Repository
 * 
 * @exception NoRootContext if the root_id does not refer to a
 * Root Naming Context.
 */
CosNaming::NamingContext_ptr
RepositoryProxy::resolve_root_context(const char* root_id)
throw (CdmwNamingAndRepository::Repository::NoRootContext,
       CORBA::SystemException)
{
    std::string id(root_id);

    if (m_namingcontext_map.find(id) == m_namingcontext_map.end())
    {
        CosNaming::Name null_name;

        m_namingcontext_map[id] =
            m_namingcontext_proxy_servant.create
            (
                m_repository->resolve_root_context(root_id),
                m_repository.in(),
                root_id,
                null_name
            );
    }

    return CosNaming::NamingContext::_duplicate(m_namingcontext_map[id].in());
}


} // End of namespace NamingAndRepository
} // End of namespace Cdmw
