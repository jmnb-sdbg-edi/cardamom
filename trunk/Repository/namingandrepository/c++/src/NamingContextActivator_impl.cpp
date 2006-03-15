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
#include "namingandrepository/NamingContextActivator_impl.hpp"

#include <string>

#include "namingandrepository/NamingContext_impl.hpp"
#include "namingandrepository/RONamingContext_impl.hpp"
#include "namingandrepository/RootNamingContext_impl.hpp"
#include "namingandrepository/NameDomainContext_impl.hpp"


namespace Cdmw
{
namespace NamingAndRepository
{


/**
*Implementation notes : None
*<p>
*Portability issues :
*Warning : std::string::compare definition in C++ Standard Library
*delivered with GNU 2.95 is deprecated.
*OK with Visual C++ and STLPORT.
*/


PortableServer::Servant NamingContextActivator_impl::incarnate(
        const PortableServer::ObjectId& oid,
        PortableServer::POA_ptr adapter)
        throw(PortableServer::ForwardRequest, CORBA::SystemException)
{

    PortableServer::Servant servant;
    std::string id;

    // FIXME - remove
//     this->m_datastore->foo();

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
                                             NamingContext_impl::PREFIX_LENGTH,
                                             NamingContext_impl::READ_WRITE_CONTEXT_PREFIX);
                                             
        if (result == 0)
        {
            // Read/Write Naming Context
            servant = NamingContext_impl::findServantById(id);
        }
        
        
        if (result != 0)
        {
            result = Common::string_compare (id, 0,
                                             NamingContext_impl::PREFIX_LENGTH,
                                             NamingContext_impl::READ_ONLY_CONTEXT_PREFIX);
            if (result == 0)
            {
                // Read Only Naming Context
                servant = RONamingContext_impl::findServantById(id);
            }
        }
        
        if (result != 0)
        {
            result = Common::string_compare (id, 0,
                                             NamingContext_impl::PREFIX_LENGTH,
                                             NamingContext_impl::ROOT_CONTEXT_PREFIX);
            if (result == 0)
            {
               // Root Context
               servant = RootNamingContext_impl::findServantById(id);
            }
        }

        if (result != 0)
        {
            result = Common::string_compare (id, 0,
                                             NameDomainContext_impl::PREFIX_LENGTH,
                                             NameDomainContext_impl::LOCAL_PREFIX);
            if (result == 0)
            {
                // Local name domain
                servant = NameDomainContext_impl::findServantById(id);
            }
        }
                    
        if (result != 0)
        {
            // Not found
            throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, CORBA::COMPLETED_NO);
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


void NamingContextActivator_impl::etherealize(
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

