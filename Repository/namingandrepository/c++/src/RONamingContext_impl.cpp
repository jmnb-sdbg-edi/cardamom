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
#include "namingandrepository/RONamingContext_impl.hpp"

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


void RONamingContext_impl::bind(const CosNaming::Name& n,
                  CORBA::Object_ptr obj)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CosNaming::NamingContext::AlreadyBound,
          CORBA::SystemException)
{

    throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);

}


void RONamingContext_impl::rebind(const CosNaming::Name& n,
                    CORBA::Object_ptr obj)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);

}


void RONamingContext_impl::bind_context(const CosNaming::Name& n,
                          CosNaming::NamingContext_ptr nc)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CosNaming::NamingContext::AlreadyBound,
          CORBA::SystemException)
{

    throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);

}


void RONamingContext_impl::rebind_context(const CosNaming::Name& n,
                            CosNaming::NamingContext_ptr nc)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);

}


CORBA::Object_ptr RONamingContext_impl::resolve(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    return m_actualContext->resolve(n);

}


void RONamingContext_impl::unbind(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);

}


CosNaming::NamingContext_ptr RONamingContext_impl::new_context()
    throw(CORBA::SystemException)
{

    throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);

}


CosNaming::NamingContext_ptr RONamingContext_impl::bind_new_context(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::AlreadyBound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);

}


void RONamingContext_impl::destroy()
    throw(CosNaming::NamingContext::NotEmpty,
          CORBA::SystemException)
{

    throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);

}


void RONamingContext_impl::list(CORBA::ULong how_many,
                  CosNaming::BindingList_out bl,
                  CosNaming::BindingIterator_out bi)
    throw(CORBA::SystemException)
{

    m_actualContext->list(how_many, bl, bi);

}


CosNaming::NamingContextExt::StringName RONamingContext_impl::to_string(
    const CosNaming::Name& n)
    throw(CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    return m_actualContext->to_string(n);

}


CosNaming::Name* RONamingContext_impl::to_name(const char* sn)
    throw(CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    return m_actualContext->to_name(sn);

}


CosNaming::NamingContextExt::URLString RONamingContext_impl::to_url(
        const char* addr, const char* sn)
    throw(CosNaming::NamingContextExt::InvalidAddress,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    return m_actualContext->to_url(addr, sn);

}


CORBA::Object_ptr RONamingContext_impl::resolve_str(const char* sn)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    return m_actualContext->resolve_str(sn);

}


CosNaming::NamingContextExt_ptr
RONamingContext_impl::createContext(
    const std::string& id, CosNaming::NamingContextExt_out actualContext)
    throw (AlreadyExistsException, OutOfResourcesException,
    AssertionFailedException, InternalErrorException)
{

    CosNaming::NamingContextExt_var temp_actualContext;

    try
    {

        std::string actualContextId = NamingContext_impl::READ_WRITE_CONTEXT_PREFIX + id;
       
        // create the actual naming context
        temp_actualContext = NamingContext_impl::createWithUserId(actualContextId);

    }
    catch(const std::bad_alloc &)
    {
        // due to actualContextId allocation
        CDMW_THROW(OutOfResourcesException);
    }


    try
    {

        // create the RO naming context
        std::string roContextId = NamingContext_impl::READ_ONLY_CONTEXT_PREFIX + id;

        PortableServer::ServantBase_var servant = new RONamingContext_impl(roContextId, temp_actualContext.in());

        RONamingContext_impl *ro_namingContext_i = dynamic_cast<RONamingContext_impl *>(servant.in());

        CosNaming::NamingContextExt_var result =
            ORBFacility::createActivatedReference<CosNaming::NamingContextExt_ptr, RONamingContext_impl>
            (roContextId, ro_namingContext_i);

        // transfers ownership
        actualContext = temp_actualContext._retn();
        return result._retn();

    }
    catch(const std::bad_alloc &)
    {
        temp_actualContext->destroy();

        CDMW_THROW(OutOfResourcesException);
    }
    catch(const AssertionFailedException &)
    {
        temp_actualContext->destroy();
        
        // rethrow
        throw;
    }
    catch(const InternalErrorException &)
    {
        temp_actualContext->destroy();
        
        // rethrow
        throw;
    }

}



RONamingContext_impl* RONamingContext_impl::findServantById(const std::string& id)
    throw (NotFoundException, OutOfResourcesException, InternalErrorException)
{

    try
    {

        // Get the actual context reference
        std::string actualContextId = id;
        actualContextId.replace(0, NamingContext_impl::PREFIX_LENGTH, NamingContext_impl::READ_WRITE_CONTEXT_PREFIX);
        
        CosNaming::NamingContextExt_var actualContext = NamingContext_impl::findById(actualContextId);

        if (CORBA::is_nil(actualContext.in()))
        {
            CDMW_THROW(NotFoundException);
        }

        RONamingContext_impl *ro_namingContext_i = new RONamingContext_impl(id, actualContext.in());

        return ro_namingContext_i;

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


RONamingContext_impl::RONamingContext_impl(const std::string& id,
    CosNaming::NamingContextExt_ptr actualContext)
    : NamingContext_impl(id)
{

    m_actualContext = CosNaming::NamingContextExt::_duplicate(actualContext);

}



RONamingContext_impl::~RONamingContext_impl()
{


}


} // End of namespace NamingAndRepository
} // End of namespace Cdmw

