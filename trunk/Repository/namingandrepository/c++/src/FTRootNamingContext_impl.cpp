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


#include "namingandrepository/FTRootNamingContext_impl.hpp"

namespace Cdmw
{
namespace NamingAndRepository
{


FTRootNamingContext_impl::FTRootNamingContext_impl
(CdmwNamingAndRepository::ProxyFeatureNamingContextExt_ptr context,
 ::CdmwFT::Location::GroupRepository_ptr group_repository,
 ::FT::ObjectGroupId  factory_finder_gid)
    :  m_context(CdmwNamingAndRepository::ProxyFeatureNamingContextExt::_duplicate(context)),
       m_group_repository(::CdmwFT::Location::GroupRepository::_duplicate(group_repository)),
       m_factory_finder_gid(factory_finder_gid)
{
}	    

FTRootNamingContext_impl::~FTRootNamingContext_impl() 
{
}


/**
 * Purpose:
 * <p>
 * Implements the
 * IDL:thales.com/CdmwNamingAndRepository/ProxyFeatureNamingContextExt/get_id:1.0
 * operation
 */
char* FTRootNamingContext_impl::get_id() throw() 
{
    return m_context->get_id();
}
    
/**
 * Purpose:
 * <p>
 * Implements the
 * IDL:omg.org/CosNaming/NamingContext/bind:1.0
 * operation
 */
void FTRootNamingContext_impl::bind(const CosNaming::Name& n,
				    CORBA::Object_ptr obj)
    throw(CosNaming::NamingContext::NotFound,
	  CosNaming::NamingContext::CannotProceed,
	  CosNaming::NamingContext::InvalidName,
	  CosNaming::NamingContext::AlreadyBound,
	  CORBA::SystemException) 
{
    m_context->bind(n, obj);
}

/**
 * Purpose:
 * <p>
 * Implements the
 * IDL:omg.org/CosNaming/NamingContext/rebind:1.0
 * operation
 */
void FTRootNamingContext_impl::rebind(const CosNaming::Name& n,
				      CORBA::Object_ptr obj)
    throw(CosNaming::NamingContext::NotFound,
	  CosNaming::NamingContext::CannotProceed,
	  CosNaming::NamingContext::InvalidName,
	  CORBA::SystemException) {
    m_context->rebind(n, obj);
}


/**
 * Purpose:
 * <p>
 * Implements the
 * IDL:omg.org/CosNaming/NamingContext/bind_context:1.0
 * operation
 */
void FTRootNamingContext_impl::bind_context(const CosNaming::Name& n,
					    CosNaming::NamingContext_ptr nc)
    throw(CosNaming::NamingContext::NotFound,
	  CosNaming::NamingContext::CannotProceed,
	  CosNaming::NamingContext::InvalidName,
	  CosNaming::NamingContext::AlreadyBound,
	  CORBA::SystemException) 
{
    m_context->bind_context(n,nc);
}

/**
 * Purpose:
 * <p>
 * Implements the
 * IDL:omg.org/CosNaming/NamingContext/rebind_context:1.0
 * operation
 */
void FTRootNamingContext_impl::rebind_context(const CosNaming::Name& n,
					      CosNaming::NamingContext_ptr nc)
    throw(CosNaming::NamingContext::NotFound,
	  CosNaming::NamingContext::CannotProceed,
	  CosNaming::NamingContext::InvalidName,
	  CORBA::SystemException) 
{
    m_context->rebind_context(n, nc);
}

/**
 * Purpose:
 * <p>
 * Implements the
 * IDL:omg.org/CosNaming/NamingContext/resolve:1.0
 * operation
 */
CORBA::Object_ptr FTRootNamingContext_impl::resolve(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::NotFound,
	  CosNaming::NamingContext::CannotProceed,
	  CosNaming::NamingContext::InvalidName,
	  CORBA::SystemException) 
{
    CORBA::ULong len = n.length();
    const std::string str_name =
	NamingUtilities::simple_name_to_string(n);

    if (len == 1 && 
	CdmwNamingAndRepository::FACTORY_FINDER == str_name) {
	try {
	    // get the string form of the name
		
			
	    return m_group_repository->get_object_group_from_gid
		(m_factory_finder_gid);
	    // FIXME full exception list
	}
	catch(...) {
	    CDMW_THROW(InternalErrorException); // FIXME proper 
	}
    }
    else {
	return m_context->resolve(n);
    }
}
	    
/**
 * Purpose:
 * <p>
 * Implements the
 * IDL:omg.org/CosNaming/NamingContext/unbind:1.0
 * operation
 */
void FTRootNamingContext_impl::unbind(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::NotFound,
	  CosNaming::NamingContext::CannotProceed,
	  CosNaming::NamingContext::InvalidName,
	  CORBA::SystemException) 
{
    m_context->unbind(n);
}

/**
 * Purpose:
 * <p>
 * Implements the
 * IDL:omg.org/CosNaming/NamingContext/new_context:1.0
 * operation
 */
CosNaming::NamingContext_ptr FTRootNamingContext_impl::new_context()
    throw(CORBA::SystemException) 
{
    return m_context->new_context();
}

/**
 * Purpose:
 * <p>
 * Implements the
 * IDL:omg.org/CosNaming/NamingContext/bind_new_context:1.0
 * operation
 */
CosNaming::NamingContext_ptr FTRootNamingContext_impl::bind_new_context(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::NotFound,
	  CosNaming::NamingContext::AlreadyBound,
	  CosNaming::NamingContext::CannotProceed,
	  CosNaming::NamingContext::InvalidName,
	  CORBA::SystemException) {
    return m_context->bind_new_context(n);
}

/**
 * Purpose:
 * <p>
 * Implements the
 * IDL:omg.org/CosNaming/NamingContext/destroy:1.0
 * operation
 */
void FTRootNamingContext_impl::destroy()
    throw(CosNaming::NamingContext::NotEmpty,
	  CORBA::SystemException) 
{
    m_context->destroy();
    m_context = CdmwNamingAndRepository::ProxyFeatureNamingContextExt::_nil();
}

/**
 * Purpose:
 * <p>
 * Implements the
 * IDL:omg.org/CosNaming/NamingContext/list:1.0
 * operation
 */
void FTRootNamingContext_impl::list(CORBA::ULong how_many,
				    CosNaming::BindingList_out bl,
				    CosNaming::BindingIterator_out bi)
    throw(CORBA::SystemException) 
{
    return m_context->list(how_many, bl, bi);
}
	
	
/**
 * Purpose:
 * <p>
 * Implements the
 * IDL:omg.org/CosNaming/NamingContextExt/to_string:1.0
 * operation
 */
CosNaming::NamingContextExt::StringName FTRootNamingContext_impl::to_string(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::InvalidName,
	  CORBA::SystemException)
{
    return m_context->to_string(n);
}

/**
 * Purpose:
 * <p>
 * Implements the
 * IDL:omg.org/CosNaming/NamingContextExt/to_name:1.0
 * operation
 */
CosNaming::Name* FTRootNamingContext_impl::to_name(const char* sn)
    throw(CosNaming::NamingContext::InvalidName,
	  CORBA::SystemException) {
    return  m_context->to_name(sn);
}

/**
 * Purpose:
 * <p>
 * Implements the
 * IDL:omg.org/CosNaming/NamingContextExt/to_url:1.0
 * operation
 *
 * Not yet implemented.
 */
CosNaming::NamingContextExt::URLString FTRootNamingContext_impl::to_url(const char* addr,
									const char* sn)
    throw(CosNaming::NamingContextExt::InvalidAddress,
	  CosNaming::NamingContext::InvalidName,
	  CORBA::SystemException) 
{
    return m_context->to_url(addr, sn);
}

/**
 * Purpose:
 * <p>
 * Implements the
 * IDL:omg.org/CosNaming/NamingContextExt/resolve_str:1.0
 * operation
 */
CORBA::Object_ptr FTRootNamingContext_impl::resolve_str(const char* sn)
    throw(CosNaming::NamingContext::NotFound,
	  CosNaming::NamingContext::CannotProceed,
	  CosNaming::NamingContext::InvalidName,
	  CORBA::SystemException)
{
    return m_context->resolve_str(sn);
}


/**
 * Purpose:
 * <p>
 * Redefines the PortableServer::ServantBase method
 * to return the correct POA.
 *
 * @return The M_POA member
 */
// 	 PortableServer::POA_ptr _default_POA() { // FIXME default impl
// 		return 
 
};
};
