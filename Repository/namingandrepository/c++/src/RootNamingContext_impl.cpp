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
#include "namingandrepository/RootNamingContext_impl.hpp"
#include "namingandrepository/PersistentNamingContext.hpp"
#include "namingandrepository/NameDomainContext_impl.hpp"

#include "namingandrepository/ORBFacility.hpp"


namespace Cdmw
{
namespace NamingAndRepository
{

/**
*Implementation notes : none
*<p>
*Portability issues :   none
*/


void RootNamingContext_impl::unbind(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    CORBA::ULong len = n.length();
    
    if (len == 1)
    {
        CORBA::Object_var object = resolve(n);

        try
        {
            if (NameDomainContext_impl::isNameDomain(object.in()))
            {
                throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);
            }
        }
        catch(const AssertionFailedException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
        catch(const OutOfMemoryException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }

    }
    
    NamingContext_impl::unbind(n);

}



void RootNamingContext_impl::rebind_context(const CosNaming::Name& n,
    CosNaming::NamingContext_ptr nc)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    CORBA::ULong len = n.length();
    
    if (len == 1)
    {
        try
        {
            CORBA::Object_var object = resolve(n);

            if (NameDomainContext_impl::isNameDomain(object.in()))
            {
                throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);
            }
        }
        catch(const CosNaming::NamingContext::NotFound &)
        {
            // the name doesn't exist
            // rebind_context is used to create a new binding
        }
        catch(const AssertionFailedException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
        catch(const OutOfMemoryException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }

    }
    
    NamingContext_impl::rebind_context(n, nc);

}



CosNaming::NamingContextExt_ptr
RootNamingContext_impl::createContext(const std::string& id)
    throw (AlreadyExistsException, OutOfResourcesException,
    AssertionFailedException, InternalErrorException)

{

    PersistentNamingContext *persistentPeer = NULL;

    try
    {
        std::string rootContextId = NamingContext_impl::ROOT_CONTEXT_PREFIX + id;
       
        persistentPeer = PersistentNamingContext::createWithId(rootContextId);
    }
    catch(const std::bad_alloc &)
    {
        // due to rootContextId allocation
        CDMW_THROW(OutOfResourcesException);
    }


    try
    {

        PortableServer::ServantBase_var servant = new RootNamingContext_impl(persistentPeer);

        RootNamingContext_impl *rootContext_i = dynamic_cast<RootNamingContext_impl *>(servant.in());

        CosNaming::NamingContextExt_var result =
            ORBFacility::createActivatedReference<CosNaming::NamingContextExt_ptr, RootNamingContext_impl>
            (persistentPeer->id(), rootContext_i);

        // transfers ownership 
        return result._retn();

    }
    catch(const std::bad_alloc &)
    {
        // cleanup persistent data
        PersistentNamingContext::destroy(persistentPeer);
        
        CDMW_THROW(OutOfResourcesException);
    }
    catch(const AssertionFailedException &)
    {
        // cleanup persistent data
        PersistentNamingContext::destroy(persistentPeer);
        
        // rethrow
        throw;
    }
    catch(const InternalErrorException &)
    {
        // cleanup persistent data
        PersistentNamingContext::destroy(persistentPeer);
        
        // rethrow
        throw;
    }

}



RootNamingContext_impl* RootNamingContext_impl::findServantById(const std::string& id)
            throw (NotFoundException, OutOfResourcesException, InternalErrorException)
{

    PersistentNamingContext* persistentPeer = PersistentNamingContext::findById(id);

    if (persistentPeer == NULL)
    {
        CDMW_THROW(NotFoundException);
    }

    try
    {
        RootNamingContext_impl *rootContext_i = new RootNamingContext_impl(persistentPeer);

        return rootContext_i;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}


RootNamingContext_impl::RootNamingContext_impl(
    PersistentNamingContext *persistent_peer)
    : NamingContext_impl(persistent_peer)
{

}



RootNamingContext_impl::~RootNamingContext_impl()
{


}


} // End of namespace NamingAndRepository
} // End of namespace Cdmw

