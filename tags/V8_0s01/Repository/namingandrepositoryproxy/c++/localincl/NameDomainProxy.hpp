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


#ifndef INCL_NAMINGANDREPOSITORY_NAMEDOMAIN_PROXY_HPP
#define INCL_NAMINGANDREPOSITORY_NAMEDOMAIN_PROXY_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Repository/idllib/CdmwRepositoryExt.skel.hpp"
#include "namingandrepositoryproxy/ProxyUtil.hpp"
#include <map>

namespace Cdmw
{
namespace NamingAndRepository
{


/**
*Purpose:
*<p>
*Implementation of the CdmwNamingAndRepository::NameDomain IDL interface.
*<p>
*Features:
*<p>
*Thread safe.
*
*@see PersistentNameDomain
*/
class NameDomainProxy: virtual public POA_CdmwNamingAndRepository::NameDomain,
                       virtual public PortableServer::RefCountServantBase,
                       public ProxyUtil
{
public:
    NameDomainProxy(CORBA::ORB_ptr orb,
                    PortableServer::POA_ptr parent_poa,
                    CdmwNamingAndRepository::RepositoryExt_ptr repos);

    /**
    * The destructor.
    */
    virtual ~NameDomainProxy();

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/new_name:1.0
    * operation
    */
    virtual CdmwNamingAndRepository::NameDomain::RegistrationId* new_name(const char* name)
        throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_object:1.0
    * operation
    */
    virtual void register_object(const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
                                 CORBA::Object_ptr the_object)
        throw(CdmwNamingAndRepository::NameDomain::InvalidRegistration,
              CdmwNamingAndRepository::NameDomain::AlreadyExists,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_new_object:1.0
    * operation
    */
    virtual void register_new_object(const char* object_name,
                                     CORBA::Object_ptr the_object)
        throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_factory:1.0
    * operation
    */
    virtual void register_factory(const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
                                  CdmwLifeCycle::FactoryBase_ptr the_factory)
        throw(CdmwNamingAndRepository::NameDomain::InvalidRegistration,
              CdmwNamingAndRepository::NameDomain::AlreadyExists,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_new_factory:1.0
    * operation
    */
    virtual void register_new_factory(const char* factory_name,
                                      CdmwLifeCycle::FactoryBase_ptr the_factory)
        throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/resolve_sub_domain:1.0
    * operation
    */
    virtual CdmwNamingAndRepository::NameDomain_ptr resolve_sub_domain(const char* domain_name)
        throw(CdmwNamingAndRepository::NoNameDomain,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_name_domain:1.0
    * operation
    */
    virtual void register_name_domain(const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
                                      const CdmwNamingAndRepository::NameDomain::NameDomainDescription& the_name_domain)
        throw(CdmwNamingAndRepository::NameDomain::InvalidRegistration,
              CdmwNamingAndRepository::NameDomain::AlreadyExists,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_new_name_domain:1.0
    * operation
    */
    virtual void register_new_name_domain(const char* domain_name,
                                          const CdmwNamingAndRepository::NameDomain::NameDomainDescription& the_name_domain)
        throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/release_name:1.0
    * operation
    */
    virtual void release_name(const char* name)
        throw(CdmwNamingAndRepository::NameDomain::NotRegistered,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException);

    CdmwNamingAndRepository::NameDomain_ptr create(const char* path);

private:
    char* full_path(const char* name)
        throw (CdmwNamingAndRepository::InvalidName);

    std::string to_string(const CdmwNamingAndRepository::NameDomain::RegistrationId& regId)
        throw (std::bad_alloc);

    typedef std::map<CORBA::ULong, std::string> NameDomainMap;

    CdmwNamingAndRepository::RepositoryExt_var m_repository;
    NameDomainMap m_map;
};

} // End of namespace NamingAndRepository
} // End of namespace Cdmw

#endif

