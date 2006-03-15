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


#ifndef INCL_NAMINGANDREPOSITORY_NAMEDOMAINACTIVATOR_IMPL_HPP
#define INCL_NAMINGANDREPOSITORY_NAMEDOMAINACTIVATOR_IMPL_HPP


#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/RefCountLocalObject.hpp"


namespace Cdmw
{
namespace NamingAndRepository
{

/**
*Purpose:
*<p>
*The servant activator for NameDomain_impl and its derived class
*servant instances.
*<p>
*Features:
*<p>
*Thread safe.
*
*@see NameDomain_impl
*/
class NameDomainActivator_impl : virtual public PortableServer::ServantActivator,
          public virtual Cdmw::OrbSupport::RefCountLocalObject
{

public:

    /**
    * Purpose:
    * <p>
    * Incarnates the correct servant according to the prefix embedded in the ObjectId.
    */
    virtual PortableServer::Servant incarnate(const PortableServer::ObjectId& oid,
                                              PortableServer::POA_ptr adapter)
        throw(PortableServer::ForwardRequest,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Etherealizes the servant.
    */
    virtual void etherealize(const PortableServer::ObjectId& oid,
                             PortableServer::POA_ptr adapter,
                             PortableServer::Servant serv,
                             CORBA::Boolean cleanup_in_progress,
                             CORBA::Boolean remaining_activations)
        throw(CORBA::SystemException);


}; //End of NameDomainActivator_impl

} // End of namespace NamingAndRepository
} // End of namespace Cdmw

#endif

