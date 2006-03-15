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


#ifndef INCL_NAMINGANDREPOSITORY_REPOSITORY_EXT_IMPL_HPP
#define INCL_NAMINGANDREPOSITORY_REPOSITORY_EXT_IMPL_HPP

#include "Foundation/orbsupport/CORBA.hpp"
// include "idllib/CdmwFTReplicationManager.stub.hpp"
#include "Repository/idllib/CdmwRepositoryExt.skel.hpp"
#include "namingandrepository/Repository_impl.hpp"
#include "FaultTolerance/idllib/CdmwFTGroupRepository.stub.hpp"

#include <map>

namespace Cdmw
{
namespace NamingAndRepository
{

/**
*Purpose:
*<p>
*Implementation of the CdmwNamingAndRepository::Repository IDL interface.
*<p>
*Features:
*<p>
*Thread safe.
*
*@see NameDomain_impl
*@see RootNamingContext_impl
*/
class RepositoryExt_impl:
            virtual public POA_CdmwNamingAndRepository::RepositoryExt,
            virtual public PortableServer::RefCountServantBase
{

public:

    RepositoryExt_impl(CdmwNamingAndRepository::Repository_ptr repository,
		       CdmwFT::ReplicationManager_ptr replication_mgr,
		       CdmwFT::Location::GroupRepository_ptr group_repository);

    virtual ~RepositoryExt_impl();

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

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:thalesgroup.com/CdmwNamingAndRepository/Repository/resolve_root_context:1.0
    * operation
    */
    virtual CosNaming::NamingContext_ptr resolve_root_context(const char* root_id)
        throw(CdmwNamingAndRepository::Repository::NoRootContext,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:thalesgroup.com/CdmwNamingAndRepository/Repository/resolve_name_domain:1.0
    * operation
    */
    virtual CdmwNamingAndRepository::NameDomain_ptr resolve_name_domain(const char* domain_name)
        throw(CdmwNamingAndRepository::NoNameDomain,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException);

    virtual ::CdmwNamingAndRepository::RootContextDecriptionSeq * get_root_context ()
      throw (CORBA::SystemException);

    virtual ::CdmwNamingAndRepository::NameDomain_ptr 
    define_name_domain (const char * domain_name)
        throw (CORBA::SystemException,
               CdmwNamingAndRepository::NameDomain::AlreadyExists,
               CdmwNamingAndRepository::InvalidName);
    
    virtual void remove_name_domain (const char * domain_name)
        throw (CORBA::SystemException,
               CdmwNamingAndRepository::NoNameDomain,
               CdmwNamingAndRepository::InvalidName,
               CdmwNamingAndRepository::RepositoryExt::NotEmpty);

private:
    ::CdmwNamingAndRepository::NameDomain_ptr lookup_parent (const char *id)
        throw (CORBA::SystemException, CdmwNamingAndRepository::InvalidName);

    std::string last(const char *path)
        throw (CORBA::SystemException, CdmwNamingAndRepository::InvalidName);

    std::string registrationId_to_string
    (const CdmwNamingAndRepository::NameDomain::RegistrationId& regId)
        throw (OutOfResourcesException);

    typedef std::map<std::string, ::CdmwNamingAndRepository::NameDomain_var>
        RegistrationMap;

    CdmwNamingAndRepository::Repository_var m_repository;

    RegistrationMap m_map;

   /**
     * The replication manager.
     */
   ::CdmwFT::ReplicationManager_var m_replication_mgr;

   /**
     * The replication manager.
     */
   ::CdmwFT::Location::GroupRepository_var m_group_repository;

    CosNaming::NamingContext_var m_ftrootCtx;

    /**
    * The group ids of FT-CORBA objects;
    */
    ::FT::ObjectGroupId  m_root_context_gid;
    ::FT::ObjectGroupId  m_admin_context_gid;
    ::FT::ObjectGroupId  m_factory_finder_gid;

}; //End of Repository_impl

} // End of namespace NamingAndRepository
} // End of namespace Cdmw

#endif

