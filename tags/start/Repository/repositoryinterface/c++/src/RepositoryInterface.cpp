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


#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Repository/naminginterface/NamingUtil.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

namespace Cdmw
{
namespace NamingAndRepository
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/
    
std::string RepositoryInterface::M_default_domain("");

CdmwNamingAndRepository::Repository_var 
    RepositoryInterface::M_repository(CdmwNamingAndRepository::Repository::_nil());
    
CdmwNamingAndRepository::NameDomain_var
    RepositoryInterface::M_default_name_domain(CdmwNamingAndRepository::NameDomain::_nil());
    
CosNaming::NamingContext_var 
    RepositoryInterface::M_default_root_context(CosNaming::NamingContext::_nil());
    
CosNaming::NamingContext_var 
    RepositoryInterface::M_default_domain_context(CosNaming::NamingContext::_nil());


void
RepositoryInterface::init(const std::string&                       default_domain,
                          CdmwNamingAndRepository::Repository_ptr repository)
    throw(CdmwNamingAndRepository::NoNameDomain,
	      CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{
    if (CORBA::is_nil(repository)) 
        throw CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMNilObjectReference,
                               CORBA::COMPLETED_NO);
    
    M_default_domain =  default_domain;
    using namespace CdmwNamingAndRepository;
    M_repository = Repository::_duplicate(repository);
    // Get the default root context
    try {
        M_default_root_context 
            = repository->resolve_root_context(DEFAULT_ROOT_CONTEXT);
    } catch (const Repository::NoRootContext &) {
        M_default_domain =  "";
        M_repository = Repository::_nil();
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALCdmwRepositoryError,
                              CORBA::COMPLETED_NO);        
    } catch (const CORBA::SystemException &) {
        throw;
    }
    // Check that default domain is really a Name Domain
    CdmwNamingAndRepository::NameDomain_var dom 
        = repository->resolve_name_domain (default_domain.c_str());
    // default_domain is a name domain, so we should be able to get its naming 
    // context.
    Cdmw::NamingAndRepository::NamingInterface ni(M_default_root_context.in());
    
    try {
        typedef Cdmw::NamingAndRepository::NamingUtil<CosNaming::NamingContext> Util;
        M_default_domain_context = Util::resolve_name(ni,default_domain);
    } catch (const Cdmw::Common::TypeMismatchException & ){
        M_default_domain =  "";
        M_repository = CdmwNamingAndRepository::Repository::_nil();
        M_default_root_context   = CosNaming::NamingContext::_nil();
        M_default_domain_context = CosNaming::NamingContext::_nil();
        throw  CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALCdmwRepositoryError,
                               CORBA::COMPLETED_NO);
    } catch (const CORBA::UserException & ) {
        M_default_domain =  "";
        M_repository = CdmwNamingAndRepository::Repository::_nil();
        M_default_root_context   = CosNaming::NamingContext::_nil();
        M_default_domain_context = CosNaming::NamingContext::_nil();
        throw  CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALCdmwRepositoryError,
                               CORBA::COMPLETED_NO);
    } catch (const CORBA::SystemException &) {
        throw;
    }
    M_default_name_domain   = dom;
}

void
RepositoryInterface::finish()
    throw()
{
    // Release all static object references
    M_repository = CdmwNamingAndRepository::Repository::_nil();
    M_default_root_context   = CosNaming::NamingContext::_nil();
    M_default_domain_context = CosNaming::NamingContext::_nil();
}

Cdmw::NamingAndRepository::NamingInterface
RepositoryInterface::get_domain_naming_interface(const std::string& domain_name)
    throw(CdmwNamingAndRepository::NoNameDomain,
	      CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{
    CosNaming::NamingContext_var the_context = get_domain_context(domain_name);
    return Cdmw::NamingAndRepository::NamingInterface(the_context.in());
}
CdmwNamingAndRepository::Repository_ptr
RepositoryInterface::get_repository()
    throw(CORBA::SystemException)
{
    assert_initialised();
    return CdmwNamingAndRepository::Repository::_duplicate(M_repository.in());
}

CosNaming::NamingContext_ptr
RepositoryInterface::get_domain_context(const std::string& domain_name)
    throw(CdmwNamingAndRepository::NoNameDomain,
	      CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{
    assert_initialised();
    
    using namespace CosNaming;
    NamingContext_var the_context;
    if (domain_name.empty()) 
        the_context = M_default_domain_context;
	else {	
        // Check that default name domain is really a Name Domain
        CdmwNamingAndRepository::NameDomain_var dom
            = M_repository->resolve_name_domain (domain_name.c_str());
    	// default_domain is a name domain, so we should be able to get its naming 
	    // context.
    	Cdmw::NamingAndRepository::NamingInterface ni(M_default_root_context.in());
	    
    	try {
            typedef Cdmw::NamingAndRepository::NamingUtil<NamingContext> Util;
            the_context = Util::resolve_name(ni,domain_name);
        } catch (const Cdmw::Common::TypeMismatchException & ) {
            throw  CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALCdmwRepositoryError,
                                   CORBA::COMPLETED_NO);
    	} catch (const CORBA::UserException & ) {
            throw  CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALCdmwRepositoryError,
                                   CORBA::COMPLETED_NO);
    	} catch (const CORBA::SystemException &) {
	        throw;
    	}
	}
    return the_context._retn();
}

CdmwNamingAndRepository::NameDomain_ptr
RepositoryInterface::get_domain(const std::string& domain_name)
    throw(CdmwNamingAndRepository::NoNameDomain,
          CdmwNamingAndRepository::InvalidName,
          CORBA::SystemException)
{
    assert_initialised();

    CdmwNamingAndRepository::NameDomain_var the_name_domain;
    if (domain_name.empty())
        the_name_domain = M_default_name_domain;
    else {	
        the_name_domain = M_repository->resolve_name_domain (domain_name.c_str());
    }
    return the_name_domain._retn();
}


bool 
RepositoryInterface::is_initialised()
{
    // Check that init have been done properly
    bool nil_ref  = (CORBA::is_nil(M_repository.in()) ||
                     CORBA::is_nil(M_default_root_context.in()) ||
                     CORBA::is_nil(M_default_domain_context.in()));
    return !nil_ref;
}

void
RepositoryInterface::assert_initialised() 
    throw(CORBA::BAD_INV_ORDER)
{
    if (!is_initialised())
        throw CORBA::BAD_INV_ORDER(Cdmw::OrbSupport::BAD_INV_ORDERInitNotCalled,
                                   CORBA::COMPLETED_NO);
}

}; // End namespace NamingAndRepository
}; // End namespace Cdmw


