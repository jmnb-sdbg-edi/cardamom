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


#ifndef INCL_NAMINGANDREPOSITORY_REPOSITORY_IMPL_HPP
#define INCL_NAMINGANDREPOSITORY_REPOSITORY_IMPL_HPP


#include "Foundation/orbsupport/CORBA.hpp"
//#include "orbsupport/OrbSupport.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.skel.hpp"


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
class Repository_impl : virtual public POA_CdmwNamingAndRepository::Repository,
        virtual public PortableServer::RefCountServantBase
{

public:

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

    /**
    * Purpose:
    * <p>
    * This method must be redefined by sub-classes to initialize all needed POAs.
    */
    virtual bool initialize(CORBA::ORB_ptr orb) = 0;

    /**
    * Purpose:
    * <p>
    * Starts the repository by :
    * - activating the root POA's manager
    * - retrieving the default root context, or creating it if necessary.
    * - rebinding the factory finder within the default root context.
    */
    virtual bool start(CORBA::ORB_ptr orb);

    CdmwLifeCycle::FactoryFinder_ptr get_factory_finder();

protected:

    /**
    * The default root context
    */
    CosNaming::NamingContextExt_var m_defaultRootContext;

    CdmwLifeCycle::FactoryFinder_var m_factoryFinder;


}; //End of Repository_impl

} // End of namespace NamingAndRepository
} // End of namespace Cdmw

#endif

