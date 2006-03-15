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
#include "Foundation/common/String.hpp"

#include "namingandrepository/NameDomainActivator_impl.hpp"

#include <string>

#include "namingandrepository/NameDomainContext_impl.hpp"
#include "namingandrepository/NameDomain_impl.hpp"


namespace Cdmw
{
namespace NamingAndRepository
{


/**
*Implementation notes :
*Distributed name domains will be added in a future release.
*<p>
*Portability issues :
*Warning : std::string::compare definition in C++ Standard Library
*delivered with GNU 2.95 is deprecated.
*OK with Visual C++ and STLPORT.
*/



PortableServer::Servant NameDomainActivator_impl::incarnate(
        const PortableServer::ObjectId& oid,
        PortableServer::POA_ptr adapter)
        throw(PortableServer::ForwardRequest, CORBA::SystemException)
{

    PortableServer::Servant servant = NULL;
    std::string id;

    try
    {
        CORBA::String_var id_temp = PortableServer::ObjectId_to_string(oid);
        id = id_temp.in();
    }
    catch(const CORBA::BAD_PARAM &)
    {
        // Malformed OID
        throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, CORBA::COMPLETED_NO);
    }


    try
    {
        int result = Common::string_compare (id, 0,
                                             NameDomainContext_impl::PREFIX_LENGTH,
                                             NameDomainContext_impl::LOCAL_PREFIX);
                                             
        if (result == 0)
        {
            // Local name domain
            servant = NameDomain_impl::findServantById(id);
        }
        else 
        {
            result = Common::string_compare (id, 0,
                                             NameDomainContext_impl::PREFIX_LENGTH,
                                             NameDomainContext_impl::DISTRIBUTED_PREFIX);
            if (result == 0)
            {
                // Distributed name domain
                // TODO
            }
            else
            {
                // Not found
                throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, CORBA::COMPLETED_NO);
            }
        }

    }
    catch(const NotFoundException &)
    {
        throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, CORBA::COMPLETED_NO);
    }
    catch(const OutOfResourcesException &)
    {
        throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
    }
    catch(const InternalErrorException &)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }

    return servant;

}


void NameDomainActivator_impl::etherealize(
        const PortableServer::ObjectId& oid,
        PortableServer::POA_ptr adapter,
        PortableServer::Servant serv,
        CORBA::Boolean cleanup_in_progress,
        CORBA::Boolean remaining_activations)
        throw(CORBA::SystemException)
{

    if (!remaining_activations)
        serv->_remove_ref();

}


} // End of namespace NamingAndRepository
} // End of namespace Cdmw

