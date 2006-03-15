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


#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "Foundation/common/Locations.hpp"
#include "namingandrepository/RepositoryExt_impl.hpp"
#include "namingandrepository/RootNamingContext_impl.hpp"
#include "namingandrepository/FTRootNamingContext_impl.hpp"
#include "namingandrepository/NameDomain_impl.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"

namespace Cdmw
{
namespace NamingAndRepository
{

RepositoryExt_impl::RepositoryExt_impl(CdmwNamingAndRepository::Repository_ptr repository,
                        ::CdmwFT::ReplicationManager_ptr replication_mgr,
                        ::CdmwFT::Location::GroupRepository_ptr group_repository)
    : m_repository(CdmwNamingAndRepository::Repository::_duplicate(repository))
{
    ::FT::Location_var location;
    
    try {
        m_replication_mgr = ::CdmwFT::ReplicationManager::_duplicate(replication_mgr);
        m_group_repository = ::CdmwFT::Location::GroupRepository::_duplicate(group_repository);
	
        location = m_group_repository->the_location();
    }	   
    catch (...) {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
    
//     /** init FactoryFinder GID */ 
//     try {
//         ::FT::Name objGroupName;
//         objGroupName.length(1);
//         objGroupName[0].id =
// 	    CdmwNamingAndRepository::FACTORY_FINDER_GROUP_ID;
	
//         CosNaming::NamingContext_var rootCtx = 
// 	    m_repository->resolve_root_context
// 	    (CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
	
//         CosNaming::NamingContextExt_var rootCtxExt = 
// 	    CosNaming::NamingContextExt::_narrow(rootCtx.in());
	
//         CORBA::Object_var facFinderObj = 
// 	    rootCtxExt->resolve_str
// 	    (CdmwNamingAndRepository::FACTORY_FINDER);
	
//         CdmwLifeCycle::FactoryFinder_var facFinder = 
// 	    CdmwLifeCycle::FactoryFinder::_narrow(facFinderObj.in());
	
//         ::FT::ObjectGroup_var objGroup = ::FT::ObjectGroup::_nil();
	
//         objGroup = m_replication_mgr->get_object_group_ref_from_name(objGroupName);
//         objGroup = m_replication_mgr->add_member
// 	    (objGroup.in(),
// 	     location.in(),
// 	     facFinder.in());
	
//         m_factory_finder_gid =
// 	    m_replication_mgr->get_object_group_id(objGroup.in());
//     }
//     catch (...) {
//         throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
//     }
    
    /** init DefaultRootContext GID */ 
    try {
        ::FT::Name objGroupName;
        objGroupName.length(1);
        objGroupName[0].id=
	    CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT_GROUP_ID;
	
        CosNaming::NamingContext_var rootCtx = 
	    m_repository->resolve_root_context
	    (CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
	
        CdmwNamingAndRepository::ProxyFeatureNamingContextExt_var rootCtxExt = 
	    CdmwNamingAndRepository::ProxyFeatureNamingContextExt::_narrow(rootCtx.in());

        FTRootNamingContext_impl * ftrootCtx_i = 
	    new FTRootNamingContext_impl
	    (rootCtxExt.in(),
	     m_group_repository.in(),
	     m_factory_finder_gid);
	
        m_ftrootCtx = ftrootCtx_i->_this();
	
        ::FT::ObjectGroup_var objGroup = ::FT::ObjectGroup::_nil();
	
        objGroup = m_replication_mgr->get_object_group_ref_from_name (objGroupName);
        objGroup = m_replication_mgr->add_member
	    (objGroup.in(),
	     location.in(),
	     rootCtx.in());
	
        m_root_context_gid =	
	    m_replication_mgr->get_object_group_id(objGroup.in());
    }
    catch (...) {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
    
    /** init AdminNameDomain GID */ 
    try {
        ::FT::Name objGroupName;
        objGroupName.length(1);
        objGroupName[0].id =
	    CdmwNamingAndRepository::ADMIN_ROOT_CONTEXT_GROUP_ID ;
	
        CosNaming::NamingContext_var adminRootContext =
	    m_repository->resolve_root_context
	    (Cdmw::Common::Locations::ADMIN_ROOT_CONTEXT_ID);
	
        ::FT::ObjectGroup_var objGroup = ::FT::ObjectGroup::_nil();
	
        objGroup = m_replication_mgr->get_object_group_ref_from_name (objGroupName);
        objGroup = m_replication_mgr->add_member
	    (objGroup.in(),
	     location.in(),
	     adminRootContext.in());
	
        m_admin_context_gid =
	    m_replication_mgr->get_object_group_id(objGroup.in());
    }
    catch (...) {
       	throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }

}

    RepositoryExt_impl::~RepositoryExt_impl() 
    {
    }

/**
* Purpose:
* <p>
* Implements the
* IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/new_name:1.0
* operation
*/
CdmwNamingAndRepository::NameDomain::RegistrationId*
RepositoryExt_impl::new_name(const char* name)
    throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{
    ::CdmwNamingAndRepository::NameDomain_ptr nd = lookup_parent(name);
    CdmwNamingAndRepository::NameDomain::RegistrationId* reg_id =
        nd->new_name(last(name).c_str());

    // FIXME erase from list if 
    m_map[registrationId_to_string(*reg_id)] =
        ::CdmwNamingAndRepository::NameDomain::_duplicate(nd);
    return reg_id;
}

/**
* Purpose:
* <p>
* Implements the
* IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_object:1.0
* operation
*/
void RepositoryExt_impl::register_object
(const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
 CORBA::Object_ptr the_object)
    throw(CdmwNamingAndRepository::NameDomain::InvalidRegistration,
          CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CORBA::SystemException)
{
    RegistrationMap::iterator iter =
        m_map.find(registrationId_to_string(reg_id));

    if (iter != m_map.end())
    {
        iter->second->register_object(reg_id, the_object);
    }
    else
    {
        throw CdmwNamingAndRepository::NameDomain::InvalidRegistration();
    }
}

/**
* Purpose:
* <p>
* Implements the
* IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_new_object:1.0
* operation
*/
void RepositoryExt_impl::register_new_object(const char* object_name,
                                 CORBA::Object_ptr the_object)
    throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{
    lookup_parent(object_name)->register_new_object(last(object_name).c_str(),
                                                    the_object);
}

/**
* Purpose:
* <p>
* Implements the
* IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_factory:1.0
* operation
*/
void RepositoryExt_impl::register_factory
(const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
 CdmwLifeCycle::FactoryBase_ptr the_factory)
    throw(CdmwNamingAndRepository::NameDomain::InvalidRegistration,
          CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CORBA::SystemException)
{
    RegistrationMap::iterator iter =
        m_map.find(registrationId_to_string(reg_id));

    if (iter != m_map.end())
    {
        iter->second->register_factory(reg_id, the_factory);
    }
    else
    {
        throw CdmwNamingAndRepository::NameDomain::InvalidRegistration();
    }
}

/**
* Purpose:
* <p>
* Implements the
* IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_new_factory:1.0
* operation
*/
void RepositoryExt_impl::register_new_factory(const char* factory_name,
                                  CdmwLifeCycle::FactoryBase_ptr the_factory)
    throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{
    lookup_parent(factory_name)->register_new_factory
        (last(factory_name).c_str(),
         the_factory);
}

/**
* Purpose:
* <p>
* Implements the
* IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/resolve_sub_domain:1.0
* operation
*/
CdmwNamingAndRepository::NameDomain_ptr RepositoryExt_impl::resolve_sub_domain(const char* domain_name)
    throw(CdmwNamingAndRepository::NoNameDomain,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{
    return lookup_parent(domain_name)->resolve_sub_domain(last(domain_name).c_str());
}

/**
* Purpose:
* <p>
* Implements the
* IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_name_domain:1.0
* operation
*/
void RepositoryExt_impl::register_name_domain
(const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
 const CdmwNamingAndRepository::NameDomain::NameDomainDescription& the_name_domain)
    throw(CdmwNamingAndRepository::NameDomain::InvalidRegistration,
          CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CORBA::SystemException)
{
    RegistrationMap::iterator iter =
        m_map.find(registrationId_to_string(reg_id));

    if (iter != m_map.end())
    {
        iter->second->register_name_domain(reg_id, the_name_domain);
    }
    else
    {
        throw CdmwNamingAndRepository::NameDomain::InvalidRegistration();
    }
}

/**
* Purpose:
* <p>
* Implements the
* IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/register_new_name_domain:1.0
* operation
*/
void RepositoryExt_impl::register_new_name_domain(const char* domain_name,
                                      const CdmwNamingAndRepository::NameDomain::NameDomainDescription& the_name_domain)
    throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{
    lookup_parent(domain_name)->register_new_name_domain
        (last(domain_name).c_str(), the_name_domain);
}

/**
* Purpose:
* <p>
* Implements the
* IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/release_name:1.0
* operation
*/
void RepositoryExt_impl::release_name(const char* name)
    throw(CdmwNamingAndRepository::NameDomain::NotRegistered,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{
    lookup_parent(name)->release_name(last(name).c_str());
}

/**
* Purpose:
* <p>
* Implements the
* IDL:thalesgroup.com/CdmwNamingAndRepository/Repository/resolve_root_context:1.0
* operation
*/
CosNaming::NamingContext_ptr RepositoryExt_impl::resolve_root_context
(const char* root_id)
    throw(CdmwNamingAndRepository::Repository::NoRootContext,
          CORBA::SystemException)
{
	if (root_id == CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT) {
		try {
		    ::FT::ObjectGroup_var group = 
            m_group_repository->get_object_group_from_gid 
            (m_root_context_gid);
		    return CosNaming::NamingContext::_narrow(group.in());
        }
        catch (...) {
			throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
	} 
	else if (root_id == CdmwNamingAndRepository::ADMIN_ROOT_CONTEXT_GROUP_ID) {
		try {
		    ::FT::ObjectGroup_var group = 
            m_group_repository->get_object_group_from_gid
            (m_admin_context_gid);
		    return CosNaming::NamingContext::_narrow(group.in());
        }
        catch (...) {
			throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
	} else {
		return m_repository->resolve_root_context(root_id);
	}
}

/**
* Purpose:
* <p>
* Implements the
* IDL:thalesgroup.com/CdmwNamingAndRepository/Repository/resolve_name_domain:1.0
* operation
*/
CdmwNamingAndRepository::NameDomain_ptr RepositoryExt_impl::resolve_name_domain(const char* domain_name)
    throw(CdmwNamingAndRepository::NoNameDomain,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{
    return m_repository->resolve_name_domain(domain_name);
}

::CdmwNamingAndRepository::RootContextDecriptionSeq*
RepositoryExt_impl::get_root_context ()
  throw (CORBA::SystemException)
{
    ::CdmwNamingAndRepository::RootContextDecriptionSeq_var
          result(new ::CdmwNamingAndRepository::RootContextDecriptionSeq());
    Strings ids = NamingContext_impl::findWithPrefix
        (NamingContext_impl::ROOT_CONTEXT_PREFIX);
    CORBA::ULong i = 0;

    const size_t len = strlen(NamingContext_impl::ROOT_CONTEXT_PREFIX);

    result->length(ids.size());

    for (Strings::const_iterator iter = ids.begin(); iter != ids.end(); ++iter)
    {
        result[i].id = CORBA::string_dup(iter->substr(len).c_str());
        result[i].root = RootNamingContext_impl::findById(*iter);
        ++i;
    }

    return result._retn();
}

::CdmwNamingAndRepository::NameDomain_ptr 
RepositoryExt_impl::define_name_domain (const char * domain_name)
    throw (CORBA::SystemException,
           CdmwNamingAndRepository::NameDomain::AlreadyExists,
           CdmwNamingAndRepository::InvalidName)
{
    CdmwNamingAndRepository::NameDomain_var nameDomain;

    try
    {
        Cdmw::CommonSvcs::Naming::NamingInterface::to_name(domain_name);
    }
    catch(const Cdmw::CommonSvcs::Naming::InvalidNameException &)
    {
        throw CdmwNamingAndRepository::InvalidName();
    }

    try
    {
        NameDomain_impl::createNameDomain
            (resolve_root_context(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT),
             domain_name,
             nameDomain.out());
    }
    catch (const AlreadyExistsException&)
    {
        throw CdmwNamingAndRepository::NameDomain::AlreadyExists();
    }
    catch (...)
    {
        // TODO: correct exception
        throw CORBA::INTERNAL();
    }

    return nameDomain._retn();
}

void RepositoryExt_impl::remove_name_domain (const char * domain_name)
    throw (CORBA::SystemException,
           CdmwNamingAndRepository::NoNameDomain,
           CdmwNamingAndRepository::InvalidName,
           CdmwNamingAndRepository::RepositoryExt::NotEmpty)
{
    CosNaming::NamingContext_var root_context;
    CosNaming::NamingContext_var domain_naming_context;
    CosNaming::Name_var name;

    root_context =
        resolve_root_context(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);

    try
    {
        name = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(domain_name);
    }
    catch(const Cdmw::CommonSvcs::Naming::InvalidNameException &)
    {
        throw CdmwNamingAndRepository::InvalidName();
    }

    try
    {
        domain_naming_context =
            CosNaming::NamingContext::_narrow(root_context->resolve(name));

        if (CORBA::is_nil(domain_naming_context.in()))
        {
            throw CdmwNamingAndRepository::NoNameDomain();
        }

        domain_naming_context->destroy();
    }
    catch (const CosNaming::NamingContext::NotFound&)
    {
        throw CdmwNamingAndRepository::NoNameDomain();
    }
    catch (const CosNaming::NamingContext::CannotProceed&)
    {
        // TODO: correct exception
        throw CORBA::INTERNAL();
    }
    catch (const CosNaming::NamingContext::InvalidName&)
    {
        throw CdmwNamingAndRepository::InvalidName();
    }
    catch (const CosNaming::NamingContext::NotEmpty&)
    {
        throw CdmwNamingAndRepository::RepositoryExt::NotEmpty();
    }

    // TODO remove PersistentNameDomain
}

::CdmwNamingAndRepository::NameDomain_ptr
RepositoryExt_impl::lookup_parent (const char *id)
    throw (CORBA::SystemException,
           CdmwNamingAndRepository::InvalidName)
{
    try
    {
        return m_repository->resolve_name_domain
            (Cdmw::CommonSvcs::Naming::NamingInterface::dirname(id).c_str());
    }
    catch (const Cdmw::CommonSvcs::Naming::InvalidNameException&)
    {
        throw CdmwNamingAndRepository::InvalidName();
    }
}

std::string RepositoryExt_impl::last(const char *path)
    throw (CORBA::SystemException,
           CdmwNamingAndRepository::InvalidName)
{
    try
    {
        return Cdmw::CommonSvcs::Naming::NamingInterface::basename(path);
    }
    catch (const Cdmw::CommonSvcs::Naming::InvalidNameException&)
    {
        throw CdmwNamingAndRepository::InvalidName();
    }
}


// Stolen from NameDomain_impl
std::string RepositoryExt_impl::registrationId_to_string(const CdmwNamingAndRepository::NameDomain::RegistrationId& regId)
    throw (OutOfResourcesException)
{

    try
    {

        std::string id;
        
        const char * tempId;

        if (regId >>= tempId)
        {
            // OK, regId contains a string
            id = tempId;
        }		
        
        return id;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}

} // End of namespace NamingAndRepository
} // End of namespace Cdmw

