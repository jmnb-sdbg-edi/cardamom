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


#ifndef INCL_NAMINGANDREPOSITORY_REPOSITORYINTERFACE_HPP
#define INCL_NAMINGANDREPOSITORY_REPOSITORYINTERFACE_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/CosNaming.stub.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"

#include <Foundation/common/Exception.hpp>
#include <string>

/**
* Root namespace for CDMW runtime.
*/
namespace Cdmw
{
/**
* Namespace for CDMW RepositoryInterface types and data.
*/
namespace NamingAndRepository
{

/**
*Purpose:
*<p> This class provides access to the naming context associated to a name domain.
* If no name domain is specified, it will use the default name domain.
*
*Features:
*<p> 
*
*@see  [if necessary] 
*@link [if necessary] 
*
*/


class RepositoryInterface 
{
public:		
		
    /**
     * Purpose:
     * <p> Initialise the RepositoryInterface for use by get_domain_context 
     * and get_domain_naming_interface operations.
     *
     *@param default_domain Name of the default name domain.
     *@param repository Reference of the Cdmw NamingAndRepository.
     *
     *@exception CdmwNamingAndRepository::NoNameDomain  <I>default_domain</I> does
     * not refer to a Name Domain.
     *@exception CdmwNamingAndRepository::InvalidName <I>default_domain</I> is ill-formed.
     *@exception CORBA::SystemException Any CORBA system exception that may be raised
     * by calls to repository object.
     */ 
    static void 
    init(const std::string&                       default_domain,
         CdmwNamingAndRepository::Repository_ptr repository)
        throw(CdmwNamingAndRepository::NoNameDomain,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException);
    
    /**
     * Purpose:
     * <p> This operation releases all static object references initialised
     * by previous call to <I>init()</I>. This should be the last operation on 
     * this class.
     */ 
    static void 
    finish()
        throw();
    
    /**
     * Purpose:
     * <p> Returns the object reference of the CDMW naming and repository service.
     * RepositoryInterface should be initialed before any call to get_repository
     *
     *@return Object reference of the CDMW naming and repository.
     * 
     *@exception CORBA::BAD_INV_ORDER RepositoryInterface is not initialed.
     *@exception CORBA::SystemException Any CORBA system exception that may be raised
     * by calls to CDMW repository.
     */ 
    static CdmwNamingAndRepository::Repository_ptr
    get_repository()
        throw(CORBA::SystemException);
    
    /**
     * Purpose:
     * <p> Returns a NamingInterface object embedding a naming context
     * associated to a name domain. If no name is specified for the domain,
     * the default name domain is used.
     * RepositoryInterface should be initialed before any calls to get_domain_context, get_domain,
     * and get_domain_naming_interface operations.
     *
     *@param domain_name Name of the name domain.
     *
     *@return NamingInterface object to access the naming context associated to 
     * the name domain.
     *
     *@exception CdmwNamingAndRepository::NoNameDomain  <I>domain_name</I> does
     * not refer to a Name Domain.
     *@exception CdmwNamingAndRepository::InvalidName <I>domain_name</I> is ill-formed.
     *@exception CORBA::BAD_INV_ORDER RepositoryInterface is not initialed.
     *@exception CORBA::SystemException Any CORBA system exception that may be raised
     * by calls to CDMW repository.
     */ 
    static Cdmw::CommonSvcs::Naming::NamingInterface
    get_domain_naming_interface(const std::string& domain_name = std::string(""))
        throw(CdmwNamingAndRepository::NoNameDomain,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException);
    
    /**
     * Purpose:
     * <p> Returns the naming context associated to a name domain. If no name
     * is specified for the domain, the default name domain is used.
     * RepositoryInterface should be initialed before any calls to get_domain_context, get_domain,
     * and get_domain_naming_interface operations.
     *
     *@param domain_name Name of the name domain.
     *
     *@return Naming context associated to the name domain.
     *
     *@exception CdmwNamingAndRepository::NoNameDomain  <I>domain_name</I> does
     * not refer to a Name Domain.
     *@exception CdmwNamingAndRepository::InvalidName <I>domain_name</I> is ill-formed.
     *@exception CORBA::BAD_INV_ORDER RepositoryInterface is not initialed.
     *@exception CORBA::SystemException Any CORBA system exception that may be raised
     * by calls to CDMW repository.
     */ 
    static CosNaming::NamingContext_ptr
    get_domain_context(const std::string& domain_name = std::string(""))
        throw(CdmwNamingAndRepository::NoNameDomain,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException);
    
    /**
     * Purpose:
     * <p> Returns the object reference to a name domain. If no name is specified for the domain,
     * the default name domain is used.
     * RepositoryInterface should be initialed before any calls to get_domain_context, get_domain,
     * and get_domain_naming_interface operations.
     *
     *@param domain_name Name of the name domain.
     *
     *@return A reference to the Name Domain object.
     *
     *@exception CdmwNamingAndRepository::NoNameDomain  <I>domain_name</I> does
     * not refer to a Name Domain.
     *@exception CdmwNamingAndRepository::InvalidName <I>domain_name</I> is ill-formed.
     *@exception CORBA::BAD_INV_ORDER RepositoryInterface is not initialed.
     *@exception CORBA::SystemException Any CORBA system exception that may be raised
     * by calls to CDMW repository.
     */ 
    static CdmwNamingAndRepository::NameDomain_ptr
    get_domain(const std::string& domain_name = std::string(""))
        throw(CdmwNamingAndRepository::NoNameDomain,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException);
    
    /**
     *[Attribute description]
     */ 
    
    
protected:
    
private:        
    
    RepositoryInterface() throw(CORBA::SystemException);
    RepositoryInterface(const RepositoryInterface & rhs)
        throw(CORBA::SystemException);
    
    
    virtual 
    ~RepositoryInterface()
        throw();
    
    RepositoryInterface&
    operator=(const RepositoryInterface& rhs)
        throw(CORBA::SystemException);
    /**
     * Purpose:
     * <p> Checks if internal object references are non-nil.
     *
     *@return <B>true</B> if Repository interface is initiased, <B>false</B>
     * otherwise.
     */ 
    static bool
    is_initialised();
    
    /**
     * Purpose:
     * <p> Asserts that internal object references are non-nil.
     *
     *@exception CORBA::BAD_INV_ORDER If one of these internal references is nil
     * - init() is not called.
     */ 
    static void
    assert_initialised() throw(CORBA::BAD_INV_ORDER);		
    
    /**
     *[Attribute description]
     */
    static std::string                              M_default_domain;
    static CdmwNamingAndRepository::Repository_var M_repository;
    static CdmwNamingAndRepository::NameDomain_var M_default_name_domain;
    static CosNaming::NamingContext_var             M_default_root_context;
    static CosNaming::NamingContext_var             M_default_domain_context;
    
}; // End class RepositoryInterface 

}; // End namespace NamingAndRepository
}; // End namespace Cdmw
#endif // INCL_NAMINGANDREPOSITORY_REPOSITORYINTERFACE_HPP






