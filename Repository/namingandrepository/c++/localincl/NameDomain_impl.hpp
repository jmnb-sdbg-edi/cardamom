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


#ifndef INCL_NAMINGANDREPOSITORY_NAMEDOMAIN_IMPL_HPP
#define INCL_NAMINGANDREPOSITORY_NAMEDOMAIN_IMPL_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.skel.hpp"
#include "namingandrepository/PersistentNameDomain.hpp"
#include "namingandrepository/Exceptions.hpp"


namespace Cdmw
{
namespace NamingAndRepository
{


class PersistentNameDomain;


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
class NameDomain_impl : virtual public POA_CdmwNamingAndRepository::NameDomain,
        virtual public PortableServer::RefCountServantBase
{

    friend class NameDomainActivator_impl;
    friend class NameDomainTemplates;
    friend class FactoryFinder_impl;
    friend class SystemFactoryFinder;

public:

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
    * IDL:thalesgroup.com/CdmwNamingAndRepository/NameDomain/resolve_name_domain:1.0
    * operation
    */
    virtual CdmwNamingAndRepository::NameDomain_ptr resolve_name_domain(const char* domain_name)
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
    * Redefines the PortableServer::ServantBase method
    * to return the correct POA.
    *
    * @return The M_POA member
    */
    virtual PortableServer::POA_ptr _default_POA();


    /**
    * The destructor.
    */
    virtual ~NameDomain_impl();


public:

    /**
    * Purpose:
    * <p>
    * Initializes the M_POA member.
    *
    * @param poa The POA
    * @exception AssertionFailedException if initialization has already been performed
    */
    static void initialize(PortableServer::POA_ptr poa)
            throw (AssertionFailedException);


    /**
    * Purpose:
    * <p>
    * Returns the name domain CORBA object specified by its name.
    *
    * @param rootCtx The root context where the name domain has been bound
    * @param domainName The name of the name domain to search for
    * @return   The CORBA object
    * @exception CdmwNamingAndRepository::NoNameDomain if domainName doesn't
    * denote a root name domain.
    * @exception AssertionFailedException if initialization has not been performed
    */
    static CdmwNamingAndRepository::NameDomain_ptr findRootNameDomain(
        CosNaming::NamingContextExt_ptr rootCtx,
	    const std::string& domainName)
        throw(CdmwNamingAndRepository::NoNameDomain, 
        CdmwNamingAndRepository::InvalidName, 
        OutOfResourcesException,
        AssertionFailedException, InternalErrorException);
	    

    /**
    * Purpose:
    * <p>
    * Creates a servant with a generated id, activates this servant,
    * and returns the corresponding CORBA object.
    * The servant's persistent peer is also created.
    *
    * This method is intended to be used only during the initialization of the
    * repository. It is not exception safe.
    *
    * @param generated_id The full identifier that has been generated.
    * @return             The reference to the CORBA object
    * @exception AlreadyExistsException if the domain name already exists
    * @exception BadParameterException  if the domain name is invalid
    */
    static NameDomain_impl* createNameDomain(
        CosNaming::NamingContext_ptr ctx,
	    const std::string& domainName,
	    CdmwNamingAndRepository::NameDomain_out nameDomain)
        throw (AlreadyExistsException, BadParameterException,
        OutOfResourcesException,
        AssertionFailedException, InternalErrorException);

    /**
    * Purpose:
    * <p>
    * Creates a servant with a generated id, activates this servant,
    * and returns the corresponding CORBA object.
    * The servant's persistent peer is also created.
    *
    * This method is intended to be used only during the initialization of the
    * repository. It is not exception safe.
    *
    * @param generated_id The full identifier that has been generated.
    * @return             The reference to the CORBA object
    * @exception AlreadyExistsException if the domain name already exists
    * @exception BadParameterException  if the domain name is invalid
    */
    NameDomain_impl* createSubNameDomain(
        const std::string& domainName)
        throw (AlreadyExistsException, BadParameterException,
        OutOfResourcesException,
        AssertionFailedException, InternalErrorException);


    /**
    * Purpose:
    * <p>
    * Returns a RegistrationId from a stringified identifier.
    *
    * @param id The stringified identifier
    * @return The RegistrationId
    */
    static CdmwNamingAndRepository::NameDomain::RegistrationId* 
    string_to_registrationId(const std::string& id)
        throw (OutOfResourcesException);

        
    /**
    * Purpose:
    * <p>
    * Returns a stringified identifier from a RegistrationId.
    *
    * @param regId The RegistrationId
    * @return The stringified identifier
    */
    static std::string registrationId_to_string(
        const CdmwNamingAndRepository::NameDomain::RegistrationId& regId)
        throw (OutOfResourcesException);
        
    /**
    * Purpose:
    * <p>
    * Returns a stringified identifier from a RegistrationId.
    * This method is similar as the registrationId_to_string method
    * except that the exception thrown is std::bad_alloc.
    * The purpose of this method is to be used by the tracing macros
    * that catch only std::bad_alloc exception.
    *
    * @param regId The RegistrationId
    * @return The stringified identifier
    */
    static std::string to_string(
        const CdmwNamingAndRepository::NameDomain::RegistrationId& regId)
        throw (std::bad_alloc);
		        
    /**
    * Purpose:
    * <p>
    * Returns a NameDomainInfo from a NameDomainDescription.
    *
    * @param nameDomainDesc The name domain description
    * @return The name domain information
    */
    static NameDomainInfo description_to_info(
        const CdmwNamingAndRepository::NameDomain::NameDomainDescription& nameDomainDesc)
        throw (OutOfResourcesException, InternalErrorException);
    
  

protected:

    /**
    * The name domain's read/write context
    */
    CosNaming::NamingContextExt_var m_context;

    /**
    * The name domain's read/write context for the factories
    */
    CosNaming::NamingContextExt_var m_factoriesContext;


protected:

    /**
    * The constructor.
    */
    NameDomain_impl(PersistentNameDomain *persistent_peer,
        CosNaming::NamingContextExt_ptr context,
	    CosNaming::NamingContextExt_ptr factoriesContext);

    /**
    * The constructor.
    */
    NameDomain_impl(const std::string& id);


private:

    /**
    * The identifier of this NameDomain.
    */
    std::string m_id;

    /**
    * The persistent peer.
    */
    PersistentNameDomain *m_persistentPeer;


private:

    /**
    * The POA in charge of NameDomain_impl and its derived class.
    *
    */
    static PortableServer::POA_ptr M_POA;


private:

    /**
    * Purpose:
    * <p>
    * Destroys completely this name domain.
    *
    * This method is intended to be used only when errors occured during
    * initialization.
    *    
    * @return true if the destruction has been correctly performed
    */
    bool destroyNameDomain();

private:

    /**
    * Purpose:
    * <p>
    * Checks whether the name is a simple name.
    *
    * @param name The name to check
    * @exception CdmwNamingAndRepository::InvalidName if the name is invalid
    * or is not a simple name
    */
    static void check_simple_name(const char* name)
    throw (CdmwNamingAndRepository::InvalidName, OutOfResourcesException);

    /**
    * Purpose:
    * <p>
    * Returns the NameDomain_impl servant instance corresponding
    * to the specified identifier. This instance is initialised with
    * the associated persistent data.
    *
    * @param id The full identifier of the name domain
    * @return   The servant
    * @exception NotFound if the servant cannot be found 
    * (no persistent data corresponding to the id)
    */
    static NameDomain_impl* findServantById(
            const std::string& id)
            throw (NotFoundException, OutOfResourcesException, InternalErrorException);

      
    /**
    * Purpose:
    * <p>
    * Returns the stringified id of the specified name domain.
    *
    * @param rootCtx The root context where the name domain has been bound
    * @param domainName The name of the name domain to search for
    * @return   The id
    * @exception CdmwNamingAndRepository::NoNameDomain if domainName doesn't
    * denote a root name domain.
    */
    static std::string findRootNameDomainId(
        CosNaming::NamingContextExt_ptr rootCtx,
	    const std::string& domainName)
        throw(CdmwNamingAndRepository::NoNameDomain, 
        CdmwNamingAndRepository::InvalidName, 
        OutOfResourcesException,
        AssertionFailedException, InternalErrorException);


    /**
    * Purpose:
    * <p>
    * Returns the stringified id of the specified name domain.
    *
    * @param nameDomain The name domain
    * @return The id of the specified name domain or an empty string
    * if nameDomain is not actually a system name domain.
    */
    static std::string getId(CORBA::Object_ptr nameDomain)
        throw (AssertionFailedException, OutOfMemoryException, InternalErrorException);


}; //End of NamingContext_impl

} // End of namespace NamingAndRepository
} // End of namespace Cdmw

#endif

