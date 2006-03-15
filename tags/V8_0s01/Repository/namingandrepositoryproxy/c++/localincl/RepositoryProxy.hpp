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

#ifndef INCL_NAMINGANDREPOSITORY_REPOSITORY_PROXY_HPP
#define INCL_NAMINGANDREPOSITORY_REPOSITORY_PROXY_HPP


#include "NameDomainProxy.hpp"
#include "NamingContextProxy.hpp"

#include "Foundation/orbsupport/CORBA.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.skel.hpp"
#include <map>
#include <string>

namespace Cdmw
{
namespace NamingAndRepository
{

class RepositoryProxy: virtual public POA_CdmwNamingAndRepository::Repository,
                       virtual public PortableServer::RefCountServantBase
{
public:
    RepositoryProxy(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa,
                    const std::string& application_name,
                    const std::string& process_name,
                    CdmwNamingAndRepository::RepositoryExt_ptr repos)
        throw (CdmwNamingAndRepository::Repository::NoRootContext,
	       CosNaming::NamingContext::NotFound,
	       CosNaming::NamingContext::AlreadyBound,
	       CosNaming::NamingContext::CannotProceed,
	       CosNaming::NamingContext::InvalidName,
	       CORBA::SystemException); 

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
    CdmwNamingAndRepository::NameDomain_ptr resolve_name_domain(const char* domain_name)
        throw (CdmwNamingAndRepository::NoNameDomain,
               CdmwNamingAndRepository::InvalidName,
               CORBA::SystemException);
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
    CosNaming::NamingContext_ptr resolve_root_context(const char* root_id)
        throw (CdmwNamingAndRepository::Repository::NoRootContext,
               CORBA::SystemException);

private:
    typedef std::map<std::string, CdmwNamingAndRepository::NameDomain_var>
        NameDomainProxyMap;

    typedef std::map<std::string, CosNaming::NamingContext_var>
        NamingContextProxyMap;

    CORBA::ORB_var m_orb;

    PortableServer::POA_var m_poa;

    CdmwNamingAndRepository::RepositoryExt_var m_repository;

    NameDomainProxy m_namedomain_proxy_servant;

    NamingContextProxy m_namingcontext_proxy_servant;

    NameDomainProxyMap m_namedomain_map;

    NamingContextProxyMap m_namingcontext_map;
};

} // End of namespace NamingAndRepository
} // End of namespace Cdmw

#endif
