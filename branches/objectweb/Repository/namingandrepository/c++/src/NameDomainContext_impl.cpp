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
#include "namingandrepository/NameDomainContext_impl.hpp"
#include "namingandrepository/NamingContext_impl.hpp"
#include "namingandrepository/RONamingContext_impl.hpp"
#include "Repository/naminginterface/NamingInterface.hpp"

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


PortableServer::POA_ptr NameDomainContext_impl::M_POA = NULL;

const int NameDomainContext_impl::PREFIX_LENGTH = 1;

const char* NameDomainContext_impl::LOCAL_PREFIX = "L";
const char* NameDomainContext_impl::DISTRIBUTED_PREFIX = "D";

const char* NameDomainContext_impl::FACTORIES_SUFFIX = "_FAC";
const char* NameDomainContext_impl::FACTORIES_NAME = "Factories";


void NameDomainContext_impl::bind(const CosNaming::Name& n,
                  CORBA::Object_ptr obj)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CosNaming::NamingContext::AlreadyBound,
          CORBA::SystemException)
{

    throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);

}


void NameDomainContext_impl::rebind(const CosNaming::Name& n,
                    CORBA::Object_ptr obj)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);

}


void NameDomainContext_impl::bind_context(const CosNaming::Name& n,
                          CosNaming::NamingContext_ptr nc)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CosNaming::NamingContext::AlreadyBound,
          CORBA::SystemException)
{

    throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);

}


void NameDomainContext_impl::rebind_context(const CosNaming::Name& n,
                            CosNaming::NamingContext_ptr nc)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);

}


CORBA::Object_ptr NameDomainContext_impl::resolve(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    return m_actualContext->resolve(n);

}


void NameDomainContext_impl::unbind(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);

}


CosNaming::NamingContext_ptr NameDomainContext_impl::new_context()
    throw(CORBA::SystemException)
{

    throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);

}


CosNaming::NamingContext_ptr NameDomainContext_impl::bind_new_context(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::AlreadyBound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);

}


void NameDomainContext_impl::destroy()
    throw(CosNaming::NamingContext::NotEmpty,
          CORBA::SystemException)
{

    throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);

}


void NameDomainContext_impl::list(CORBA::ULong how_many,
                  CosNaming::BindingList_out bl,
                  CosNaming::BindingIterator_out bi)
    throw(CORBA::SystemException)
{

    m_actualContext->list(how_many, bl, bi);

}


CosNaming::NamingContextExt::StringName NameDomainContext_impl::to_string(
    const CosNaming::Name& n)
    throw(CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    return m_actualContext->to_string(n);

}


CosNaming::Name* NameDomainContext_impl::to_name(const char* sn)
    throw(CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    return m_actualContext->to_name(sn);

}


CosNaming::NamingContextExt::URLString NameDomainContext_impl::to_url(
        const char* addr, const char* sn)
    throw(CosNaming::NamingContextExt::InvalidAddress,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    return m_actualContext->to_url(addr, sn);

}


CORBA::Object_ptr NameDomainContext_impl::resolve_str(const char* sn)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    return m_actualContext->resolve_str(sn);

}




void NameDomainContext_impl::initialize(PortableServer::POA_ptr poa)
    throw (AssertionFailedException)
{

    if (!CORBA::is_nil(M_POA))
    {
        CDMW_THROW(AssertionFailedException);
    }

    M_POA = PortableServer::POA::_duplicate(poa);

}


PortableServer::POA_ptr NameDomainContext_impl::_default_POA()
{

    return PortableServer::POA::_duplicate(M_POA);

}


bool NameDomainContext_impl::isNameDomain(CORBA::Object_ptr o)
    throw (AssertionFailedException, OutOfMemoryException)
{

    if (CORBA::is_nil(M_POA))
    {
        CDMW_THROW(AssertionFailedException);
    }

    bool res = false;

    try
    {
        PortableServer::ObjectId_var temp = M_POA->reference_to_id(o);
        res = true;
    }
    catch(const PortableServer::POA::WrongAdapter &)
    {
        // Not a name domain managed by the repository
    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfMemoryException);
    }

    return res;

}



PortableServer::ObjectId* NameDomainContext_impl::getObjectId(CORBA::Object_ptr nameDomainContext)
    throw (AssertionFailedException, OutOfMemoryException)
{
    
    if (CORBA::is_nil(M_POA))
    {
        CDMW_THROW(AssertionFailedException);
    }


    PortableServer::ObjectId_var oid;

    try
    {
        try
        {
            oid = M_POA->reference_to_id(nameDomainContext);
        }
        catch(const PortableServer::POA::WrongAdapter &)
        {
            // Not a name domain managed by the repository
            // => empty sequence
            oid = new PortableServer::ObjectId;
        }
    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfMemoryException);
    }

    return oid._retn();

}


std::string NameDomainContext_impl::getId(CORBA::Object_ptr nameDomainContext)
    throw (AssertionFailedException, OutOfMemoryException, InternalErrorException)
{
    
    if (CORBA::is_nil(M_POA))
    {
        CDMW_THROW(AssertionFailedException);
    }

    try
    {
        std::string id;

        try
        {
            PortableServer::ObjectId_var oid = M_POA->reference_to_id(nameDomainContext);

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


CosNaming::NamingContextExt_ptr
NameDomainContext_impl::createContext(
    std::string& nameDomainId, 
    CosNaming::NamingContextExt_out actualContext,
    CosNaming::NamingContextExt_out actualFactoriesContext)
    throw (OutOfResourcesException, AssertionFailedException, InternalErrorException)
{

    CosNaming::NamingContextExt_var temp_actualContext;
    CosNaming::NamingContextExt_var temp_actualFactoriesContext;

    try
    {
        std::string genId;

        // create the actual naming context
        temp_actualContext = NamingContext_impl::createWithSystemId(genId);

        // remove the prefix to keep only the id part
        genId.erase(0, NamingContext_impl::PREFIX_LENGTH);

        // create the factories' context
        std::string factoriesContextId = genId + NameDomainContext_impl::FACTORIES_SUFFIX;
        CosNaming::NamingContextExt_var roFactoriesContext =
            RONamingContext_impl::createContext(factoriesContextId, temp_actualFactoriesContext);

        CosNaming::Name_var name = Cdmw::NamingAndRepository::
            NamingInterface::to_name(NameDomainContext_impl::FACTORIES_NAME);

        // bind the factories' context within the actual context
        temp_actualContext->bind_context(name.in(), roFactoriesContext.in());

        // create the name domain context
        nameDomainId = NameDomainContext_impl::LOCAL_PREFIX + genId;

        PortableServer::ServantBase_var servant = new NameDomainContext_impl(temp_actualContext.in());
        
        NameDomainContext_impl *nameDomainContext_i = dynamic_cast<NameDomainContext_impl *>(servant.in());

        CosNaming::NamingContextExt_var result =
            ORBFacility::createActivatedReference<CosNaming::NamingContextExt_ptr, NameDomainContext_impl>
            (nameDomainId, nameDomainContext_i);
        
        // transfers ownership
        actualContext = temp_actualContext._retn();
        actualFactoriesContext = temp_actualFactoriesContext._retn();
        
        return result._retn();

    }
    catch(const std::bad_alloc &)
    {
        cleanUp(temp_actualContext.in(), temp_actualFactoriesContext.in());
        CDMW_THROW(OutOfResourcesException);
    }
    catch(const Cdmw::NamingAndRepository::InvalidNameException &)
    {
        // may be thrown by the NamingInterface::to_name() method
        // (should never happened)
        cleanUp(temp_actualContext.in(), temp_actualFactoriesContext.in());
        CDMW_THROW(InternalErrorException);
    }
    catch(const CORBA::Exception &)
    {
        // may be thrown by the bind_context() operation
        cleanUp(temp_actualContext.in(), temp_actualFactoriesContext.in());
        CDMW_THROW(InternalErrorException);
    }
    catch(const OutOfResourcesException &)
    {
        cleanUp(temp_actualContext.in(), temp_actualFactoriesContext.in());
        throw;
    }
    catch(const AssertionFailedException &)
    {
        cleanUp(temp_actualContext.in(), temp_actualFactoriesContext.in());
        throw;
    }
    catch(const InternalErrorException &)
    {
        cleanUp(temp_actualContext.in(), temp_actualFactoriesContext.in());
        throw;
    }

}


bool NameDomainContext_impl::cleanUp(
    CosNaming::NamingContextExt_ptr actualContext,
    CosNaming::NamingContextExt_ptr actualFactoriesContext)
    throw ()
{

    bool result = true;

    if (!CORBA::is_nil(actualContext))
    {
        // try to unbind the factories context
        // (if the binding has been done)
        try
        {
            CosNaming::Name_var name = 
                Cdmw::NamingAndRepository::NamingInterface::to_name(NameDomainContext_impl::FACTORIES_NAME);

            // unbind to allow the destruction
            actualContext->unbind(name.in());
        }
        catch(...)
        {
            result = false;
        }

        // try to destroy the actual context
        try
        {
            actualContext->destroy();
        }
        catch(...)
        {
            result = false;
        }

    }        
        
    if (!CORBA::is_nil(actualFactoriesContext))
    {

        // try to destroy the actual factories' context
        try
        {
            actualFactoriesContext->destroy();
        }
        catch(...)
        {
            result = false;
        }
        
    }

    return result;

}


bool NameDomainContext_impl::destroyContext(const std::string& nameDomainId)
{

    CosNaming::NamingContextExt_var actualContext;
    CosNaming::NamingContextExt_var actualFactoriesContext;

    try
    {
        actualContext = findActualContext(nameDomainId);
    }
    catch(...)
    {
    }

    try
    {
        actualFactoriesContext = findActualFactoriesContext(nameDomainId);
    }
    catch(...)
    {
    }

    return cleanUp(actualContext.in(), actualFactoriesContext.in());

}


std::string NameDomainContext_impl::getActualContextId(const std::string& id)
{

    std::string temp_contextId(id);
    temp_contextId.erase(0, NameDomainContext_impl::PREFIX_LENGTH);
    std::string contextId = NamingContext_impl::READ_WRITE_CONTEXT_PREFIX + temp_contextId;

    return contextId;

}

std::string NameDomainContext_impl::getActualFactoriesContextId(const std::string& id)
{

    std::string contextId = getActualContextId(id);

    std::string factoriesContextId = contextId + NameDomainContext_impl::FACTORIES_SUFFIX;

    return factoriesContextId;

}


NameDomainContext_impl* NameDomainContext_impl::findServantById(const std::string& id)
    throw (NotFoundException, OutOfResourcesException, InternalErrorException)
{

    try
    {
        CosNaming::NamingContextExt_var actualContext = findActualContext(id);

        NameDomainContext_impl *nameDomainContext_i = new NameDomainContext_impl(actualContext.in());

        return nameDomainContext_i;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}



CosNaming::NamingContextExt_ptr NameDomainContext_impl::findActualContext(const std::string& id)
    throw (NotFoundException, OutOfResourcesException, InternalErrorException)
{

    try
    {

        std::string actualContextId = getActualContextId(id);
        CosNaming::NamingContextExt_ptr actualContext =
            NamingContext_impl::findById(actualContextId);

        if (CORBA::is_nil(actualContext))
        {
            CDMW_THROW(NotFoundException);
        }

        return actualContext;

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


CosNaming::NamingContextExt_ptr NameDomainContext_impl::findActualFactoriesContext(const std::string& id)
    throw (NotFoundException, OutOfResourcesException, InternalErrorException)
{

    try
    {

        std::string actualFactoriesContextId = getActualFactoriesContextId(id);
        CosNaming::NamingContextExt_ptr actualFactoriesContext =
            NamingContext_impl::findById(actualFactoriesContextId);

        if (CORBA::is_nil(actualFactoriesContext))
        {
            CDMW_THROW(NotFoundException);
        }
        
        return actualFactoriesContext;

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

NameDomainContext_impl::NameDomainContext_impl(
    CosNaming::NamingContextExt_ptr actualContext)
{

    m_actualContext = CosNaming::NamingContextExt::_duplicate(actualContext);

}



NameDomainContext_impl::~NameDomainContext_impl()
{


}


} // End of namespace NamingAndRepository
} // End of namespace Cdmw

