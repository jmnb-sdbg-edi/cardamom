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
#include "Foundation/common/Locations.hpp"
#include "namingandrepositoryproxy/NamingContextProxy.hpp"

#include "TraceAndPerf/tracelibrary/CdmwTrace.hpp"

namespace Cdmw
{
namespace NamingAndRepository
{

/*
* The implementation of NamingContextProxy
*/

NamingContextProxy::NamingContextProxy
(CORBA::ORB_ptr orb,
 PortableServer::POA_ptr parent_poa,
 CdmwNamingAndRepository::RepositoryExt_ptr repos)
    : ProxyUtil(orb, parent_poa, "NamingContextProxyPOA"),
      m_repository(CdmwNamingAndRepository::RepositoryExt::_duplicate(repos))
{
    m_poa->set_servant(this);
}

NamingContextProxy::~NamingContextProxy()
{
}

void NamingContextProxy::bind(const CosNaming::Name& n,
                              CORBA::Object_ptr obj)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CosNaming::NamingContext::AlreadyBound,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, get_id(), "NamingContextProxy", "bind", "in", "n", to_string(n));

    root_context()->bind(adjust_path(n), obj);

    CDMW_TRACE_EXIT(CDMW_NREP, get_id(), "NamingContextProxy", "bind");

}


void NamingContextProxy::rebind(const CosNaming::Name& n,
                                CORBA::Object_ptr obj)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, get_id(), "NamingContextProxy", "rebind", "in", "n", to_string(n));

    root_context()->rebind(adjust_path(n), obj);

    CDMW_TRACE_EXIT(CDMW_NREP, get_id(), "NamingContextProxy", "rebind");

}


void NamingContextProxy::bind_context(const CosNaming::Name& n,
                                      CosNaming::NamingContext_ptr nc)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CosNaming::NamingContext::AlreadyBound,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, get_id(), "NamingContextProxy", "bind_context", "in", "n", to_string(n));

    root_context()->bind_context(adjust_path(n), nc);

    CDMW_TRACE_EXIT(CDMW_NREP, get_id(), "NamingContextProxy", "bind_context");

}


void NamingContextProxy::rebind_context(const CosNaming::Name& n,
                            CosNaming::NamingContext_ptr nc)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, get_id(), "NamingContextProxy", "rebind_context", "in", "n", to_string(n));

    root_context()->rebind_context(adjust_path(n), nc);

    CDMW_TRACE_EXIT(CDMW_NREP, get_id(), "NamingContextProxy", "rebind_context");

}


CORBA::Object_ptr NamingContextProxy::resolve(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{
    CDMW_TRACE_ENTRY1(CDMW_NREP, get_id(), "NamingContextProxy", "resolve", "in", "n", to_string(n));

    CosNaming::Name name(adjust_path(n));

    return check_for_context(root_context()->resolve(name), name);
}


void NamingContextProxy::unbind(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, get_id(), "NamingContextProxy", "unbind", "in", "n", to_string(n));

    root_context()->unbind(adjust_path(n));

    CDMW_TRACE_EXIT(CDMW_NREP, get_id(), "NamingContextProxy", "unbind");

}


CosNaming::NamingContext_ptr NamingContextProxy::new_context()
    throw(CORBA::SystemException)
{
    CDMW_TRACE_ENTRY(CDMW_NREP, get_id(), "NamingContextProxy", "new_context");

    try
    {
    CosNaming::NamingContext_var nc = root_context()->new_context();
    
    // Obtain id from context
    CdmwNamingAndRepository::ProxyFeatureNamingContextExt_var cdmwCtx 
    = CdmwNamingAndRepository::ProxyFeatureNamingContextExt::_narrow(nc.in());
    
    if (!CORBA::is_nil(cdmwCtx.in()))
    {
        MapEntry& entry = m_map[get_id()];
        CosNaming::Name name;

        name.length(m_base_name.length() + 1);

        for (CORBA::ULong i = 0; i < m_base_name.length(); i++)
        {
            name[i] = m_base_name[i];
        }

        name[m_base_name.length()].id = cdmwCtx->get_id();

        entry.parent->resolve_root_context
        (Common::Locations::ADMIN_ROOT_CONTEXT_ID)->bind_context(name, nc.in());

        nc = create(nc.in(), entry.parent.in(), Common::Locations::ADMIN_ROOT_CONTEXT_ID, name);
    }

    return nc._retn();
    }
    catch (const CORBA::Exception& ex)
    {
        std::cerr << "NamingContextProxy::new_context caught " << ex << std::endl;
        throw;
    }
    catch (...)
    {
        std::cerr << "NamingContextProxy::new_context caught something" << std::endl;
        throw;
    }

}


CosNaming::NamingContext_ptr NamingContextProxy::bind_new_context
(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::AlreadyBound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY(CDMW_NREP, get_id(), "NamingContextProxy", "bind_new_context");
    CosNaming::NamingContext_var nc;
    CosNaming::Name name;

    nc = root_context()->bind_new_context(adjust_path(n));

    MapEntry& entry = m_map[get_id()];
    CORBA::ULong len = entry.name.length();

    name.length(len + n.length());

    for (CORBA::ULong i = 0; i < len; i++)
    {
        name[i] = entry.name[i];
    }

    for (CORBA::ULong j = 0; j < n.length(); j++)
    {
        name[len + j] = n[j];
    }

    return create(nc.in(), entry.parent.in(), entry.id.c_str(), name);
}


void NamingContextProxy::destroy()
    throw(CosNaming::NamingContext::NotEmpty,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY(CDMW_NREP, get_id(), "NamingContextProxy", "destroy");

    RETRY(destroy());

    m_map.erase(get_id());

    CDMW_TRACE_EXIT(CDMW_NREP, get_id(), "NamingContextProxy", "destroy");

}



void NamingContextProxy::list(CORBA::ULong how_many,
                              CosNaming::BindingList_out bl,
                              CosNaming::BindingIterator_out bi)
    throw(CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, get_id(), "NamingContextProxy", "list", "in", "how_many", how_many);

    RETRY(list(how_many, bl, bi))

    CDMW_TRACE_EXIT(CDMW_NREP, get_id(), "NamingContextProxy", "list");

}


CosNaming::NamingContextExt::StringName NamingContextProxy::to_string(
    const CosNaming::Name& n)
    throw(CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, get_id(), "NamingContextProxy", "to_string", "in", "n", to_string(n));

    // Do locally as this used by the debug trace

    try
    {

        try
        {
            std::string str;

            try
            {
                str = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(n);
            }
            catch(const Cdmw::CommonSvcs::Naming::InvalidNameException &)
            {
                throw CosNaming::NamingContext::InvalidName();
            }

            char *result = CORBA::string_dup(str.c_str());

            CDMW_TRACE_EXIT(CDMW_NREP, get_id(), "NamingContextProxy", "to_string");

            return result;

        }
        catch(const std::bad_alloc &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, get_id(), "NamingContextProxy", "to_string", WRN, e._name());

        throw;

    }
}


CosNaming::Name* NamingContextProxy::to_name(const char* sn)
    throw(CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, get_id(), "NamingContextProxy", "to_name", "in", "sn", sn);

    return CosNaming::NamingContextExt::_narrow(root_context())->to_name(sn);
}


CosNaming::NamingContextExt::URLString NamingContextProxy::to_url(
        const char* addr, const char* sn)
    throw(CosNaming::NamingContextExt::InvalidAddress,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY2(CDMW_NREP, get_id(), "NamingContextProxy", "to_url", "in", "addr", addr, "in", "sn", sn);

    return CosNaming::NamingContextExt::_narrow(root_context())->to_url(addr, sn);
}


CORBA::Object_ptr NamingContextProxy::resolve_str(const char* sn)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, get_id(), "NamingContextProxy", "resolve_str", "in", "sn", sn);

    CosNaming::Name name = m_map[get_id()].name;
    CosNaming::NamingContextExt::StringName_var path;

    if (name.length() > 0)
    {
        CosNaming::NamingContextExt::StringName_var base = to_string(name);
        path = CORBA::string_alloc(strlen(base) + strlen(sn) + 1);

        strcpy(path, base);
        strcat(path, "/");
        strcat(path, sn);
    }
    else
    {
        path = sn;
    }

    return CosNaming::NamingContextExt::_narrow(root_context())->resolve_str(path);
}

CosNaming::NamingContext_ptr
NamingContextProxy::create(CosNaming::NamingContext_ptr proxied,
                           CdmwNamingAndRepository::RepositoryExt_ptr parent,
                           const char* id,
                           CosNaming::Name name)
{
    CORBA::ULong new_id = next_id();
    PortableServer::ObjectId oid;
    MapEntry& entry = m_map[new_id];
    CORBA::Object_var obj;

    entry.proxied = CosNaming::NamingContextExt::_narrow(proxied);
    entry.parent = CdmwNamingAndRepository::RepositoryExt::_duplicate(parent);
    entry.id = id;
    entry.name = name;

    oid.length(sizeof(CORBA::ULong));

    for (CORBA::ULong i = 0; i < sizeof(CORBA::ULong); i++)
    {
        oid[i] = new_id & 0xFF;
        new_id >>= 8;
    }

    obj = m_poa->create_reference_with_id
        (oid, "IDL:omg.org/CosNaming/NamingContextExt:1.0");

    return CosNaming::NamingContext::_narrow(obj.in());
}

void NamingContextProxy::set_base_name(const CosNaming::Name& base_name)
{
    m_base_name = base_name;
}

CosNaming::NamingContext_ptr NamingContextProxy::root_context()
    throw (CORBA::SystemException)
{
    MapEntry& entry = m_map[get_id()];
    return entry.parent->resolve_root_context(entry.id.c_str());
}

CosNaming::Name NamingContextProxy::adjust_path(const CosNaming::Name& n)
{
    CosNaming::Name result = m_map[get_id()].name;
    CORBA::ULong len = result.length();

    result.length(len + n.length());

    for (CORBA::ULong i = 0; i < n.length(); i++)
    {
        result[len + i] = n[i];
    }

    return result;
}

CosNaming::NamingContextExt_ptr NamingContextProxy::get_object()
    throw (CORBA::SystemException)
{
    NamingContextMap::iterator iter = m_map.find(get_id());

    if (iter != m_map.end())
    {
        return iter->second.proxied.in();
    }
    else
    {
        // TODO use Cardamom mechanism
        throw CORBA::OBJECT_NOT_EXIST();
    }
}

CORBA::Object_ptr NamingContextProxy::check_for_context
(CORBA::Object_ptr obj,
 const CosNaming::Name& n)
{
    // TODO check for Repository generated NCs
    CosNaming::NamingContextExt_var nc =
        CosNaming::NamingContextExt::_narrow(obj);

    if (CORBA::is_nil(nc.in()))
    {
        return obj;
    }
    else
    {
        MapEntry& entry = m_map[get_id()];

        return create(nc.in(), entry.parent.in(), entry.id.c_str(), n);
    }
}

void NamingContextProxy::reconstruct() throw (CORBA::SystemException)
{
    MapEntry& entry = m_map[get_id()];
    CosNaming::NamingContext_var root_context =
        entry.parent->resolve_root_context(entry.id.c_str());
    CORBA::Object_var obj;

    if (entry.name.length() > 0)
    {
        entry.proxied = CosNaming::NamingContextExt::_narrow
            (root_context->resolve(entry.name));
    }
    else
    {
        entry.proxied =
            CosNaming::NamingContextExt::_narrow(root_context.in());
    }
}

} // End of namespace NamingAndRepository
} // End of namespace Cdmw

