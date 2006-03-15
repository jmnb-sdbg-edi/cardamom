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
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "namingandrepositoryproxy/NameDomainProxy.hpp"
#include "namingandrepositoryproxy/ProxyUtil.hpp"
#include "TraceAndPerf/tracelibrary/CdmwTrace.hpp"

namespace Cdmw
{
namespace NamingAndRepository
{

NameDomainProxy::NameDomainProxy
(CORBA::ORB_ptr orb,
 PortableServer::POA_ptr parent_poa,
 CdmwNamingAndRepository::RepositoryExt_ptr repos)
    : ProxyUtil(orb, parent_poa, "NameDomainProxyPOA"),
      m_repository(CdmwNamingAndRepository::RepositoryExt::_duplicate(repos))
{
    m_poa->set_servant(this);
}

NameDomainProxy::~NameDomainProxy()
{
}

CdmwNamingAndRepository::NameDomain::RegistrationId* NameDomainProxy::new_name(const char* name)
    throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, get_id(), "NameDomainProxy", "new_name", "in", "name", name);
try
{

    CORBA::String_var path = full_path(name);

    return m_repository->new_name(path.in());
}
catch (const CORBA::Exception& ex)
{
    std::cerr << "NameDomainProxy::new_name caught " << ex << std::endl;
    throw;
}
catch (...)
{
    std::cerr << "NameDomainProxy::new_name caught something" << std::endl;
    throw;
}
    // CDMW_TRACE_EXIT(CDMW_NREP, get_id(), "NameDomainProxy", "new_name");
}


void NameDomainProxy::register_object(const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
                             CORBA::Object_ptr the_object)
    throw(CdmwNamingAndRepository::NameDomain::InvalidRegistration,
          CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, get_id(), "NameDomainProxy", "register_object", "in", "reg_id", to_string(reg_id));

    m_repository->register_object(reg_id, the_object);

    CDMW_TRACE_EXIT(CDMW_NREP, get_id(), "NameDomainProxy", "register_object"); 
}



void NameDomainProxy::register_new_object(const char* object_name,
                                 CORBA::Object_ptr the_object)
    throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, get_id(), "NameDomainProxy", "register_new_object", "in", "object_name", object_name);

    CORBA::String_var path = full_path(object_name);

    m_repository->register_new_object(path.in(), the_object);

    CDMW_TRACE_EXIT(CDMW_NREP, get_id(), "NameDomainProxy", "register_new_object");
}



void NameDomainProxy::register_factory(const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
                              CdmwLifeCycle::FactoryBase_ptr the_factory)
    throw(CdmwNamingAndRepository::NameDomain::InvalidRegistration,
          CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, get_id(), "NameDomainProxy", "register_factory", "in", "reg_id", to_string(reg_id));

    m_repository->register_factory(reg_id, the_factory);

    CDMW_TRACE_EXIT(CDMW_NREP, get_id(), "NameDomainProxy", "register_factory");
}



void NameDomainProxy::register_new_factory(const char* factory_name,
                                  CdmwLifeCycle::FactoryBase_ptr the_factory)
    throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, get_id(), "NameDomainProxy", "register_new_factory", "in", "factory_name", factory_name);

    CORBA::String_var path = full_path(factory_name);

    m_repository->register_new_factory(path.in(), the_factory);

    CDMW_TRACE_EXIT(CDMW_NREP, get_id(), "NameDomainProxy", "register_new_factory");
}

CdmwNamingAndRepository::NameDomain_ptr NameDomainProxy::resolve_sub_domain(const char* domain_name)
    throw(CdmwNamingAndRepository::NoNameDomain,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{
    CDMW_TRACE_ENTRY1(CDMW_NREP, get_id(), "NameDomainProxy", "resolve_sub_domain", "in", "domain_name", domain_name);

    CORBA::String_var path = full_path(domain_name);

    return m_repository->resolve_sub_domain(path.in());
}

void NameDomainProxy::register_name_domain(const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
                                  const CdmwNamingAndRepository::NameDomain::NameDomainDescription& the_name_domain)
    throw(CdmwNamingAndRepository::NameDomain::InvalidRegistration,
          CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, get_id(), "NameDomainProxy", "register_name_domain", "in", "reg_id", to_string(reg_id));

    m_repository->register_name_domain(reg_id, the_name_domain);

    CDMW_TRACE_EXIT(CDMW_NREP, get_id(), "NameDomainProxy", "register_name_domain");
}



void NameDomainProxy::register_new_name_domain(const char* domain_name,
                                      const CdmwNamingAndRepository::NameDomain::NameDomainDescription& the_name_domain)
    throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, get_id(), "NameDomainProxy", "register_new_name_domain", "in", "domain_name", domain_name);

    CORBA::String_var path = full_path(domain_name);

    m_repository->register_new_name_domain(path.in(), the_name_domain);

    CDMW_TRACE_EXIT(CDMW_NREP, get_id(), "NameDomainProxy", "register_new_name_domain");

}



void NameDomainProxy::release_name(const char* name)
    throw(CdmwNamingAndRepository::NameDomain::NotRegistered,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, get_id(), "NameDomainProxy", "release_name", "in", "name", name);

    CORBA::String_var path = full_path(name);

    m_repository->release_name(path.in());

    CDMW_TRACE_EXIT(CDMW_NREP, get_id(), "NameDomainProxy", "release_name");
}

char* NameDomainProxy::full_path(const char* name)
    throw (CdmwNamingAndRepository::InvalidName)
{
    const char* COMPOUND_NAME = "Compound name not allowed !";

    try
    {
        CosNaming::Name_var name_seq =
            Cdmw::CommonSvcs::Naming::NamingInterface::to_name(name);

        if (name_seq->length() != 1)
        {
            throw CdmwNamingAndRepository::InvalidName(COMPOUND_NAME);
        }
    }
    catch (const Cdmw::CommonSvcs::Naming::InvalidNameException& ex)
    {
        throw CdmwNamingAndRepository::InvalidName(ex.what());
    }

    std::string path = m_map[get_id()];

    char *result = CORBA::string_alloc(strlen(path.c_str()) + strlen(name) + 1);
    strcpy(result, path.c_str());
    strcat(result, "/");
    strcat(result, name);

    return result;
}


CdmwNamingAndRepository::NameDomain_ptr
NameDomainProxy::create(const char* path)
{
    CORBA::ULong id = next_id();
    PortableServer::ObjectId oid;
    CORBA::Object_var obj;

    m_map[id] = path;

    oid.length(sizeof(CORBA::ULong));

    for (CORBA::ULong i = 0; i < sizeof(CORBA::ULong); i++)
    {
        oid[i] = id & 0xFF;
        id >>= 8;
    }

    obj = m_poa->create_reference_with_id
        (oid, "IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain:1.0");

    return CdmwNamingAndRepository::NameDomain::_narrow(obj.in());
}

std::string NameDomainProxy::to_string(const CdmwNamingAndRepository::NameDomain::RegistrationId& regId)
    throw (std::bad_alloc)
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

} // End of namespace NamingAndRepository
} // End of namespace Cdmw
