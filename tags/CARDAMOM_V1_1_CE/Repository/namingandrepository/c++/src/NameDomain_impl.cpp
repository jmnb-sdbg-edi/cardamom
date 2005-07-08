/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your 
 * option) any later version. 
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public 
 * License for more details. 
 * 
 * You should have received a copy of the GNU Library General 
 * Public License along with CARDAMOM; see the file COPYING. If not, write to 
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "namingandrepository/NameDomain_impl.hpp"
#include "namingandrepository/NamingContext_impl.hpp"
#include "namingandrepository/NameDomainContext_impl.hpp"
#include "namingandrepository/ORBFacility.hpp"
#include "namingandrepository/NamingUtilities.hpp"
#include "Repository/naminginterface/NamingInterface.hpp"

#include "TraceAndPerf/tracelibrary/CdmwTrace.hpp"

namespace Cdmw
{
namespace NamingAndRepository
{


/**
*Implementation notes : none
*<p>
*Portability issues :   none
*/

// Initialisation

PortableServer::POA_ptr NameDomain_impl::M_POA = NULL;


/*
* The template class used for code factorization
*/
class NameDomainTemplates
{

public:

    /*
    * 
    */
    template <class T> static void add_registration(
        const char*name,
        const T& reg, NameDomain_impl* thisNameDomain)
    {

        NameDomain_impl::check_simple_name(name);

        std::string str_name(name);

        if (!thisNameDomain->m_persistentPeer->addRegistration(str_name, reg))
        {
            throw CdmwNamingAndRepository::NameDomain::AlreadyExists();
        }

    }


    /*
    * 
    */
    template <class T> static void activate_registration(
        const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
        const T& reg, NameDomain_impl* thisNameDomain)
    {

        std::string str_regId = NameDomain_impl::registrationId_to_string(reg_id);
        
        if (str_regId.empty())
            throw CdmwNamingAndRepository::NameDomain::InvalidRegistration();

        // activate the registration
        bool activated;

        if (!thisNameDomain->m_persistentPeer->activateRegistration(
            str_regId, reg, activated))
        {
            if (activated)
                throw CdmwNamingAndRepository::NameDomain::AlreadyExists();
            else
                throw CdmwNamingAndRepository::NameDomain::InvalidRegistration();
        }

    }


    /*
    * 
    */
    static bool bind(const CosNaming::Name& n,
        CORBA::Object_ptr the_object, NameDomain_impl* thisNameDomain)
    {

        bool result = false;

        try
        {
            thisNameDomain->m_context->bind(n, the_object);
            result = true;
        }
        catch(const CORBA::Exception &)
        {
        }

        return result;

    }


    /*
    * 
    */
    static bool bind(const CosNaming::Name& n,
        CdmwLifeCycle::FactoryBase_ptr the_factory, NameDomain_impl* thisNameDomain)
    {

        bool result = false;

        try
        {
            thisNameDomain->m_context->bind(n, the_factory);

            try
            {
                thisNameDomain->m_factoriesContext->bind(n, the_factory);
                result = true;
            }
            catch(const CORBA::Exception &)
            {
                thisNameDomain->m_context->unbind(n);
            }
            
        }
        catch(const CORBA::Exception &)
        {

        }

        return result;

    }


    /*
    * 
    */
    static bool bind(const CosNaming::Name& n,
        CosNaming::NamingContext_ptr the_context, NameDomain_impl* thisNameDomain)
    {

        bool result = false;

        try
        {
            thisNameDomain->m_context->bind_context(n, the_context);
            result = true;
        }
        catch(const CORBA::Exception &)
        {
        }

        return result;


    }


    /*
    * 
    */
    template <class T> static void actual_register(
        const std::string& str_name,
        T object, NameDomain_impl* thisNameDomain)
    {

        CosNaming::Name_var name = Cdmw::NamingAndRepository::NamingInterface::to_name(str_name);

        bool ret = bind(name.in(), object, thisNameDomain);

        if (!ret)
        {
            // clean up
            bool activated;
            RegistrationType type;
            thisNameDomain->m_persistentPeer->removeRegistration(str_name, activated, type);

            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);

        }

    }


    /*
    * 
    */
    template <class T> static void actual_register(
        const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
        T object, NameDomain_impl* thisNameDomain)
    {

        std::string str_regId = NameDomain_impl::registrationId_to_string(reg_id);
        std::string str_name = thisNameDomain->m_persistentPeer->findName(str_regId);

        CosNaming::Name_var name = Cdmw::NamingAndRepository::NamingInterface::to_name(str_name);

        bool ret = bind(name.in(), object, thisNameDomain);

        if (!ret)
        {
            // clean up
            bool activated;
            RegistrationType type;
            thisNameDomain->m_persistentPeer->removeRegistration(str_name, activated, type);

            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);

        }
    
    }


};


/*
* The implementation of NameDomain_impl
*/

void NameDomain_impl::check_simple_name(const char* name)
    throw (CdmwNamingAndRepository::InvalidName, OutOfResourcesException)
{

    try
    {

        std::string reason;

        if (!NamingUtilities::isSimpleName(name, reason))
        {
            CdmwNamingAndRepository::InvalidName e;
            e.reason = CORBA::string_dup(reason.c_str());

            throw e;
        }

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}


CdmwNamingAndRepository::NameDomain::RegistrationId* NameDomain_impl::new_name(const char* name)
    throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, m_id, "NameDomain_impl", "new_name", "in", "name", name);

    try
    {

        try
        {    
            check_simple_name(name);

            std::string str_regId;
            std::string str_name(name);

            if (!m_persistentPeer->addRegistration(str_name, str_regId))
            {
                throw CdmwNamingAndRepository::NameDomain::AlreadyExists();
            }

            CdmwNamingAndRepository::NameDomain::RegistrationId_var regId =
                string_to_registrationId(str_regId);

            CDMW_TRACE_EXIT0(CDMW_NREP, m_id, "NameDomain_impl", "new_name", to_string(regId.in()));

            return regId._retn();

        }
        catch(const std::bad_alloc &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const OutOfResourcesException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NameDomain_impl", "new_name", WRN, e._name());

        throw;

    }

}


void NameDomain_impl::register_object(const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
                             CORBA::Object_ptr the_object)
    throw(CdmwNamingAndRepository::NameDomain::InvalidRegistration,
          CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, m_id, "NameDomain_impl", "register_object", "in", "reg_id", to_string(reg_id));

    try
    {

        if (CORBA::is_nil(the_object))
            throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);

        try
        {
            NameDomainTemplates::activate_registration<RegistrationType>(reg_id, ObjectRegistration, this);

            NameDomainTemplates::actual_register<CORBA::Object_ptr>(reg_id, the_object, this);
        }
        catch(const std::bad_alloc &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const OutOfResourcesException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NameDomain_impl", "register_object", WRN, e._name());

        throw;

    }
   
    CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NameDomain_impl", "register_object");
 
}



void NameDomain_impl::register_new_object(const char* object_name,
                                 CORBA::Object_ptr the_object)
    throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, m_id, "NameDomain_impl", "register_new_object", "in", "object_name", object_name);

    try
    {

        if (CORBA::is_nil(the_object))
            throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);

        try
        {
            NameDomainTemplates::add_registration<RegistrationType>(object_name, ObjectRegistration, this);

            NameDomainTemplates::actual_register<CORBA::Object_ptr>(object_name, the_object, this);
        }
        catch(const std::bad_alloc &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const OutOfResourcesException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NameDomain_impl", "register_new_object", WRN, e._name());

        throw;

    }

    CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NameDomain_impl", "register_new_object");

}



void NameDomain_impl::register_factory(const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
                              CdmwLifeCycle::FactoryBase_ptr the_factory)
    throw(CdmwNamingAndRepository::NameDomain::InvalidRegistration,
          CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, m_id, "NameDomain_impl", "register_factory", "in", "reg_id", to_string(reg_id));

    try
    {

        if (CORBA::is_nil(the_factory))
            throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);

        try
        {
            NameDomainTemplates::activate_registration<RegistrationType>(reg_id, FactoryRegistration, this);

            NameDomainTemplates::actual_register<CdmwLifeCycle::FactoryBase_ptr>(reg_id, the_factory, this);
        }
        catch(const std::bad_alloc &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const OutOfResourcesException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NameDomain_impl", "register_factory", WRN, e._name());

        throw;

    }

    CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NameDomain_impl", "register_factory");

}



void NameDomain_impl::register_new_factory(const char* factory_name,
                                  CdmwLifeCycle::FactoryBase_ptr the_factory)
    throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, m_id, "NameDomain_impl", "register_new_factory", "in", "factory_name", factory_name);

    try
    {

        if (CORBA::is_nil(the_factory))
            throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);

        try
        {
            NameDomainTemplates::add_registration<RegistrationType>(factory_name, FactoryRegistration, this);

            NameDomainTemplates::actual_register<CdmwLifeCycle::FactoryBase_ptr>(factory_name, the_factory, this);
        }
        catch(const std::bad_alloc &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const OutOfResourcesException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NameDomain_impl", "register_new_factory", WRN, e._name());

        throw;

    }

    CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NameDomain_impl", "register_new_factory");

}



CdmwNamingAndRepository::NameDomain_ptr NameDomain_impl::resolve_name_domain(const char* domain_name)
    throw(CdmwNamingAndRepository::NoNameDomain,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, m_id, "NameDomain_impl", "resolve_name_domain", "in", "domain_name", domain_name);

    try
    {

        try
        {
            CdmwNamingAndRepository::NameDomain_var nameDomain;

            CosNaming::Name_var name;

            try
            {
                name = Cdmw::NamingAndRepository::NamingInterface::to_name(domain_name);
            }
            catch(const Cdmw::NamingAndRepository::InvalidNameException &e)
            {
                // invalid name
                CdmwNamingAndRepository::InvalidName excep;
                excep.reason = CORBA::string_dup(e.what());

                throw excep;
            }

            // get the name domain corresponding to the first name component
            std::string firstPartStr = NamingUtilities::stringifiedNameFirstPart(name.in());

            std::string firstNameDomainRef = m_persistentPeer->findNameDomain(firstPartStr);

            if (firstNameDomainRef.empty())
            {
                // invalid name
                throw CdmwNamingAndRepository::NoNameDomain();
            }

            CORBA::Object_var obj = ORBFacility::string_to_object(firstNameDomainRef);

            CdmwNamingAndRepository::NameDomain_var firstNameDomain = CdmwNamingAndRepository::NameDomain::_narrow(obj.in());

            if (name->length() > 1)
            {
                // propagates the resolution
                std::string endPartStr = NamingUtilities::stringifiedNameEndPart(name.in());

                nameDomain = firstNameDomain->resolve_name_domain(endPartStr.c_str());
            }
            else
            {
                nameDomain = firstNameDomain;
            }

            CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NameDomain_impl", "resolve_name_domain");

            return nameDomain._retn();

        }
        catch(const std::bad_alloc &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const OutOfResourcesException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const AssertionFailedException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
        catch(const InternalErrorException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NameDomain_impl", "resolve_name_domain", WRN, e._name());

        throw;

    }


}



void NameDomain_impl::register_name_domain(const CdmwNamingAndRepository::NameDomain::RegistrationId& reg_id,
                                  const CdmwNamingAndRepository::NameDomain::NameDomainDescription& the_name_domain)
    throw(CdmwNamingAndRepository::NameDomain::InvalidRegistration,
          CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, m_id, "NameDomain_impl", "register_name_domain", "in", "reg_id", to_string(reg_id));

    try
    {

        if (CORBA::is_nil(the_name_domain.the_name_domain)
         || CORBA::is_nil(the_name_domain.the_factory_finder)
         || CORBA::is_nil(the_name_domain.the_context) )
            throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);

        try
        {
            NameDomainInfo nameDomainInfo = description_to_info(the_name_domain);
            NameDomainTemplates::activate_registration<NameDomainInfo>(reg_id, nameDomainInfo, this);

            NameDomainTemplates::actual_register<CosNaming::NamingContext_ptr>(reg_id, the_name_domain.the_context, this);
        }
        catch(const std::bad_alloc &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const OutOfResourcesException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const AssertionFailedException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
        catch(const InternalErrorException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NameDomain_impl", "register_name_domain", WRN, e._name());

        throw;

    }

    CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NameDomain_impl", "register_name_domain");

}



void NameDomain_impl::register_new_name_domain(const char* domain_name,
                                      const CdmwNamingAndRepository::NameDomain::NameDomainDescription& the_name_domain)
    throw(CdmwNamingAndRepository::NameDomain::AlreadyExists,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, m_id, "NameDomain_impl", "register_new_name_domain", "in", "domain_name", domain_name);

    try
    {

        if (CORBA::is_nil(the_name_domain.the_name_domain)
         || CORBA::is_nil(the_name_domain.the_factory_finder)
         || CORBA::is_nil(the_name_domain.the_context) )
            throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);

        try
        {
            NameDomainInfo nameDomainInfo = description_to_info(the_name_domain);
            NameDomainTemplates::add_registration<NameDomainInfo>(domain_name, nameDomainInfo, this);

            NameDomainTemplates::actual_register<CosNaming::NamingContext_ptr>(domain_name, the_name_domain.the_context, this);
        }
        catch(const std::bad_alloc &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const OutOfResourcesException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const AssertionFailedException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
        catch(const InternalErrorException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NameDomain_impl", "register_new_name_domain", WRN, e._name());

        throw;

    }

    CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NameDomain_impl", "register_new_name_domain");

}



void NameDomain_impl::release_name(const char* name)
    throw(CdmwNamingAndRepository::NameDomain::NotRegistered,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, m_id, "NameDomain_impl", "release_name", "in", "name", name);

    try
    {

        try
        {
            check_simple_name(name);

            bool isSystem;
            bool nameDomainFound = m_persistentPeer->existsNameDomain(name, isSystem);

            if (nameDomainFound && isSystem)
                throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);

            // remove the registration    
            bool activated;
            RegistrationType type;
            std::string str_name(name);

            if (!m_persistentPeer->removeRegistration(str_name, activated, type))
                throw CdmwNamingAndRepository::NameDomain::NotRegistered();

            if (activated)
            {
                // remove the binding(s)
                CosNaming::Name_var temp_name = Cdmw::NamingAndRepository::NamingInterface::to_name(str_name);

                try
                {
                    m_context->unbind(temp_name.in());

                    if (type == FactoryRegistration)
                    {
                        m_factoriesContext->unbind(temp_name.in());
                    }

                }
                catch(const CORBA::Exception &)
                {
                    throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
                }
            }
        }
        catch(const std::bad_alloc &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const OutOfResourcesException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NameDomain_impl", "release_name", WRN, e._name());

        throw;

    }

    CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NameDomain_impl", "release_name");
    
}



void NameDomain_impl::initialize(PortableServer::POA_ptr poa)
    throw (AssertionFailedException)
{

    if (!CORBA::is_nil(M_POA))
    {
        CDMW_THROW(AssertionFailedException);
    }

    M_POA = PortableServer::POA::_duplicate(poa);

}


PortableServer::POA_ptr NameDomain_impl::_default_POA()
{

    return PortableServer::POA::_duplicate(M_POA);

}


CdmwNamingAndRepository::NameDomain_ptr NameDomain_impl::findRootNameDomain(
    CosNaming::NamingContextExt_ptr rootCtx,
    const std::string& domainName)
    throw(CdmwNamingAndRepository::NoNameDomain,
    CdmwNamingAndRepository::InvalidName,
    OutOfResourcesException, AssertionFailedException, InternalErrorException)
{

    if (CORBA::is_nil(M_POA))
    {
        CDMW_THROW(AssertionFailedException);
    }

    try
    {

        PortableServer::ObjectId_var oid;

        try
        {
            CORBA::Object_var obj = rootCtx->resolve_str(domainName.c_str());
            oid = NameDomainContext_impl::getObjectId(obj.in());
        }
        catch(const CosNaming::NamingContext::NotFound &)
        {
            throw CdmwNamingAndRepository::NoNameDomain();
        }
        catch(const CosNaming::NamingContext::InvalidName &)
        {
            throw CdmwNamingAndRepository::InvalidName();
        }
        catch(const CosNaming::NamingContext::CannotProceed &)
        {
            CDMW_THROW(InternalErrorException);
        }

        if (oid->length() == 0)
        {
            // The resolved  object is not a name domain
            throw CdmwNamingAndRepository::NoNameDomain();
        }
        
        CORBA::RepositoryId_var repId = CORBA::string_dup(
            "IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain:1.0");
        
        CORBA::Object_var nd_temp = M_POA->create_reference_with_id(oid.in(), repId.in());

        CdmwNamingAndRepository::NameDomain_var result =
            CdmwNamingAndRepository::NameDomain::_narrow(nd_temp.in());

        return result._retn();

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
    catch(const OutOfMemoryException &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
    catch(const CORBA::SystemException &)
    {
        CDMW_THROW(InternalErrorException);
    }

}



std::string NameDomain_impl::findRootNameDomainId(
    CosNaming::NamingContextExt_ptr rootCtx,
    const std::string& domainName)
    throw(CdmwNamingAndRepository::NoNameDomain,
    CdmwNamingAndRepository::InvalidName,
    OutOfResourcesException, AssertionFailedException, InternalErrorException)
{

    try
    {

        std::string id;

        try
        {
            CORBA::Object_var obj = rootCtx->resolve_str(domainName.c_str());
            id = NameDomainContext_impl::getId(obj.in());
        }
        catch(const CosNaming::NamingContext::NotFound &)
        {
            throw CdmwNamingAndRepository::NoNameDomain();
        }
        catch(const CosNaming::NamingContext::InvalidName &)
        {
            throw CdmwNamingAndRepository::InvalidName();
        }
        catch(const CosNaming::NamingContext::CannotProceed &)
        {
            CDMW_THROW(InternalErrorException);
        }

        if (id.empty())
        {
            // The resolved  object is not a name domain
            throw CdmwNamingAndRepository::NoNameDomain();
        }
        
        return id;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
    catch(const OutOfMemoryException &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
    catch(const CORBA::SystemException &)
    {
        CDMW_THROW(InternalErrorException);
    }

}


std::string NameDomain_impl::getId(CORBA::Object_ptr nameDomain)
    throw (AssertionFailedException, OutOfMemoryException, InternalErrorException)
{
    
    if (CORBA::is_nil(M_POA))
    {
        CDMW_THROW(AssertionFailedException);
    }

    if (CORBA::is_nil(nameDomain))
    {
        CDMW_THROW(AssertionFailedException);
    }

    try
    {
        std::string id;

        try
        {
            PortableServer::ObjectId_var oid = M_POA->reference_to_id(nameDomain);

            CORBA::String_var str_id = PortableServer::ObjectId_to_string(oid.in());
            
            id = str_id.in();

        }
        catch(const PortableServer::POA::WrongAdapter &)
        {
            // Not a name domain managed by the repository
        }
        catch(...)
        {
            // Error
            CDMW_THROW(InternalErrorException);
        }

        return id;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfMemoryException);
    }

}



NameDomain_impl* NameDomain_impl::createNameDomain(
    CosNaming::NamingContext_ptr ctx,
    const std::string& domainName,
    CdmwNamingAndRepository::NameDomain_out nameDomain)
    throw (AlreadyExistsException, BadParameterException, 
    OutOfResourcesException,
    AssertionFailedException, InternalErrorException)
{

    try
    {

        bool nameDomainContext_created = false;
        bool nameDomainContext_bound = false;
        bool persistentPeer_created = false;

        std::string nameDomainId;
        CosNaming::Name_var name;
        PersistentNameDomain *persistentPeer = NULL;

        try
        {
            name = Cdmw::NamingAndRepository::
                NamingInterface::to_name(domainName);
        }
        catch(const Cdmw::NamingAndRepository::InvalidNameException &)
        {
            CDMW_THROW2(BadParameterException, "domainName", "invalid");
        }

        try
        {
            // Create the contexts associated with the name domain
            CosNaming::NamingContextExt_var nameDomainContext;
            CosNaming::NamingContextExt_var nameDomainActualContext;
            CosNaming::NamingContextExt_var nameDomainFactoriesContext;

            nameDomainContext = NameDomainContext_impl::createContext(
                        nameDomainId, nameDomainActualContext, nameDomainFactoriesContext);
            nameDomainContext_created = true;

            // bind the created name domain context within the context
            ctx->bind_context(name.in(), nameDomainContext.in());
            nameDomainContext_bound = true;
            
            // create the persistent peer
            persistentPeer = PersistentNameDomain::createWithId(nameDomainId);
            persistentPeer_created = true;
            
            // create the servant
            PortableServer::ServantBase_var servant = new NameDomain_impl(persistentPeer,
                nameDomainActualContext.in(), nameDomainFactoriesContext.in());

            NameDomain_impl *nameDomain_i = dynamic_cast<NameDomain_impl *>(servant.in());

            CdmwNamingAndRepository::NameDomain_var nameDomain_temp =
                ORBFacility::createActivatedReference<CdmwNamingAndRepository::NameDomain_ptr, NameDomain_impl>
                (nameDomainId, nameDomain_i);

            // transfers ownership
            nameDomain = nameDomain_temp._retn();

            return nameDomain_i;

        }
        catch(...)
        {
            // cleanup
            if (persistentPeer_created)
            {
                PersistentNameDomain::destroy(persistentPeer);
            }

            if (nameDomainContext_bound)
            {
                ctx->unbind(name.in());
            }

            if (nameDomainContext_created)
            {
                NameDomainContext_impl::destroyContext(nameDomainId);
            }

            // rethrow 
            throw;
  
        }

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
    catch(const OutOfResourcesException &)
    {
        // rethrow
        throw;
    }
    catch(const CosNaming::NamingContext::AlreadyBound &)
    {
        // rethrow
        CDMW_THROW(AlreadyExistsException);
    }
    catch(const AssertionFailedException &)
    {
        // rethrow
        throw;
    }
    catch(const InternalErrorException &)
    {
        // rethrow
        throw;
    }
    catch(const CORBA::Exception &)
    {
        CDMW_THROW(InternalErrorException);
    }

}


NameDomain_impl* NameDomain_impl::createSubNameDomain(
	const std::string& domainName)
    throw (AlreadyExistsException, BadParameterException,
    OutOfResourcesException,
    AssertionFailedException, InternalErrorException)
{

    try
    {

        bool subNameDomain_created = false;

        CdmwNamingAndRepository::NameDomain_var subNameDomain;
        NameDomain_impl* subNameDomain_i =  NULL;

        try
        {
            subNameDomain_i =  createNameDomain(m_context.in(), domainName, subNameDomain.out());
            subNameDomain_created = true;
            
            std::string str_obj = ORBFacility::object_to_string(subNameDomain.in());
        
            // create the registration
            NameDomainInfo nameDomainInfo(str_obj);
            
            if (!m_persistentPeer->addRegistration(domainName, nameDomainInfo))
            {
                CDMW_THROW(AlreadyExistsException);
            }

            return subNameDomain_i;

        }
        catch(...)
        {
            // cleanup
            if (subNameDomain_created)
            {
                // destroy the created sub_name domain
                subNameDomain_i->destroyNameDomain();
            }

            // rethrow
            throw;
        }
        

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}



bool NameDomain_impl::destroyNameDomain()
{

    bool result = true;

    try
    {
        // destroy the persistent peer
	    PersistentNameDomain::destroy(m_persistentPeer);
        // destroy the associated contexts
        NameDomainContext_impl::destroyContext(m_id);

	    // destroy the CORBA object	
        PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(m_id.c_str());
        M_POA->deactivate_object(oid.in());
	
	}
	catch(...)
	{
	    result = false;
	}
	
    return result;

}
    


NameDomain_impl* NameDomain_impl::findServantById(const std::string& id)
            throw (NotFoundException, OutOfResourcesException, InternalErrorException)
{

    try
    {

        CosNaming::NamingContextExt_var context =
            NameDomainContext_impl::findActualContext(id);

        // Get the factories context reference
        CosNaming::NamingContextExt_var factoriesContext =
            NameDomainContext_impl::findActualFactoriesContext(id);

        if (CORBA::is_nil(context.in()) || CORBA::is_nil(factoriesContext.in()))
        {
            CDMW_THROW(NotFoundException);
        }
        
        PersistentNameDomain* persistentPeer = PersistentNameDomain::findById(id);

        if (persistentPeer == NULL)
        {
            CDMW_THROW(NotFoundException);
        }

        NameDomain_impl *nameDomain_i =
            new NameDomain_impl(persistentPeer, context.in(), factoriesContext.in()); 

        return nameDomain_i;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}



NameDomain_impl::NameDomain_impl(
    PersistentNameDomain *persistent_peer, CosNaming::NamingContextExt_ptr context,
    CosNaming::NamingContextExt_ptr factoriesContext)
    : m_persistentPeer(persistent_peer)
{

    m_id = persistent_peer->id();
    m_context = CosNaming::NamingContextExt::_duplicate(context);
    m_factoriesContext = CosNaming::NamingContextExt::_duplicate(factoriesContext);

}


NameDomain_impl::NameDomain_impl(const std::string& id)
    : m_id(id)
{

    m_persistentPeer = NULL;

}


NameDomain_impl::~NameDomain_impl()
{


}


CdmwNamingAndRepository::NameDomain::RegistrationId* NameDomain_impl::string_to_registrationId(const std::string& id)
    throw (OutOfResourcesException)
{

    try
    {
        CdmwNamingAndRepository::NameDomain::RegistrationId *regId = new CdmwNamingAndRepository::NameDomain::RegistrationId;
    
        (*regId) <<= id.c_str();
    
        return regId;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}

        
std::string NameDomain_impl::registrationId_to_string(const CdmwNamingAndRepository::NameDomain::RegistrationId& regId)
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


std::string NameDomain_impl::to_string(const CdmwNamingAndRepository::NameDomain::RegistrationId& regId)
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


NameDomainInfo NameDomain_impl::description_to_info(const CdmwNamingAndRepository::NameDomain::NameDomainDescription& nameDomainDesc)
    throw (OutOfResourcesException, InternalErrorException)
{

    try
    {

        // get the stringified reference of the object
        std::string nameDomainRef = ORBFacility::object_to_string(nameDomainDesc.the_name_domain);
        
        std::string factoryFinderRef = ORBFacility::object_to_string(nameDomainDesc.the_factory_finder);

        NameDomainInfo nameDomainInfo(nameDomainRef, factoryFinderRef);
        
        return nameDomainInfo;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
    catch(const AssertionFailedException &)
    {
        CDMW_THROW(InternalErrorException);
    }

}



} // End of namespace NamingAndRepository
} // End of namespace Cdmw

