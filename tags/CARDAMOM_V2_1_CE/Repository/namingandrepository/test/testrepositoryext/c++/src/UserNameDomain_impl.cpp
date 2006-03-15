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


#include "testrepositoryext/UserNameDomain_impl.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include <string>

/*
* The implementation of UserNameDomain_impl
*/

namespace
{
    // TODO: [i18n]put strings in a message catalog
    const char* COMPOUND_NAME = "Compound name not allowed !";
};


bool isSimpleName(const char* sn, std::string& reason)
{

    bool result = false;

    try
    {
        CosNaming::Name_var name = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(sn);
    
        if (name->length() == 1)
        {
            result = true;
        }
        else
        {
            reason = COMPOUND_NAME;
        }
    
    }
    catch(const Cdmw::CommonSvcs::Naming::InvalidNameException &e)
    {
        // invalid name
        reason = e.what();
    }

    return result;

}


void check_simple_name(const char* name)
    throw (CdmwNamingAndRepository::InvalidName)
{

    std::string reason;

    if (!isSimpleName(name, reason))
    {
        CdmwNamingAndRepository::InvalidName e;
        e.reason = CORBA::string_dup(reason.c_str());

        throw e;
    }

}


CdmwNamingAndRepository::NameDomain::RegistrationId* UserNameDomain_impl::new_name(const char* name)
    throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{

    throw CORBA::NO_IMPLEMENT();

}


void UserNameDomain_impl::register_object(const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
                             CORBA::Object_ptr the_object)
    throw(CdmwNamingAndRepository::NameDomain::InvalidRegistration,
          CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CORBA::SystemException)
{

    throw CORBA::NO_IMPLEMENT();

}



void UserNameDomain_impl::register_new_object(const char* object_name,
                                 CORBA::Object_ptr the_object)
    throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{

    check_simple_name(object_name);

    if (CORBA::is_nil(the_object))
        throw CORBA::BAD_PARAM();

    try
    {

        CosNaming::Name_var name = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(object_name);
        m_context->bind(name.in(), the_object);

    }
    catch(const CosNaming::NamingContext::AlreadyBound &)
    {
        throw CdmwNamingAndRepository::NameDomain::AlreadyExists();
    }

}



void UserNameDomain_impl::register_factory(const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
                              CdmwLifeCycle::FactoryBase_ptr the_factory)
    throw(CdmwNamingAndRepository::NameDomain::InvalidRegistration,
          CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CORBA::SystemException)
{

    throw CORBA::NO_IMPLEMENT();
    
}



void UserNameDomain_impl::register_new_factory(const char* factory_name,
                                  CdmwLifeCycle::FactoryBase_ptr the_factory)
    throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{

    check_simple_name(factory_name);

    if (CORBA::is_nil(the_factory))
        throw CORBA::BAD_PARAM();

    try
    {

        CosNaming::Name_var name = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(factory_name);
        m_context->bind(name.in(), the_factory);

    }
    catch(const CosNaming::NamingContext::AlreadyBound &)
    {
        throw CdmwNamingAndRepository::NameDomain::AlreadyExists();
    }

}



CdmwNamingAndRepository::NameDomain_ptr UserNameDomain_impl::resolve_sub_domain(const char* domain_name)
    throw(CdmwNamingAndRepository::NoNameDomain,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{

    throw CORBA::NO_IMPLEMENT();

}


void UserNameDomain_impl::register_name_domain(const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
                                  const CdmwNamingAndRepository::NameDomain::NameDomainDescription& the_name_domain)
    throw(CdmwNamingAndRepository::NameDomain::InvalidRegistration,
          CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CORBA::SystemException)
{

    throw CORBA::NO_IMPLEMENT();

}


void UserNameDomain_impl::register_new_name_domain(const char* domain_name,
                                      const CdmwNamingAndRepository::NameDomain::NameDomainDescription& the_name_domain)
    throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{

    throw CORBA::NO_IMPLEMENT();

}


void UserNameDomain_impl::release_name(const char* name)
    throw(CdmwNamingAndRepository::NameDomain::NotRegistered,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{

    check_simple_name(name);

    try
    {

        CosNaming::Name_var name2 = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(name);
        m_context->unbind(name2.in());

    }
    catch(const CosNaming::NamingContext::NotFound &)
    {
        throw CdmwNamingAndRepository::NameDomain::NotRegistered();
    }
    
}


UserNameDomain_impl::UserNameDomain_impl(CosNaming::NamingContextExt_ptr context)
{

    m_context = CosNaming::NamingContextExt::_duplicate(context);

}



UserNameDomain_impl::~UserNameDomain_impl()
{

}


