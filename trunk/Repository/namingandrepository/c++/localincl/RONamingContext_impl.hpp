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


#ifndef INCL_NAMINGANDREPOSITORY_RONAMINGCONTEXT_IMPL_HPP
#define INCL_NAMINGANDREPOSITORY_RONAMINGCONTEXT_IMPL_HPP


#include "Foundation/orbsupport/CORBA.hpp"
#include "namingandrepository/NamingContext_impl.hpp"


namespace Cdmw
{
namespace NamingAndRepository
{


/**
*Purpose:
*<p>
*The read only implementation of the CosNaming::NamingContextExt
*IDL interface.
*Write access operations from this implementation raises the CORBA::NO_PERMISSION
*system exception. Read access operations are delegated to the actual read/write 
*naming context.
*<p>
*Features:
*<p>
*Thread safe.
*
*@see NamingContext_impl
*/
class RONamingContext_impl : virtual public NamingContext_impl
{

    friend class NamingContextActivator_impl;


public:

    /**
    * Purpose:
    * <p>
    * Throws CORBA::NO_PERMISSION system exception.
    */
    virtual void bind(const CosNaming::Name& n,
                      CORBA::Object_ptr obj)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CosNaming::NamingContext::AlreadyBound,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Throws CORBA::NO_PERMISSION system exception.
    */
    virtual void rebind(const CosNaming::Name& n,
                        CORBA::Object_ptr obj)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Throws CORBA::NO_PERMISSION system exception.
    */
    virtual void bind_context(const CosNaming::Name& n,
                              CosNaming::NamingContext_ptr nc)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CosNaming::NamingContext::AlreadyBound,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Throws CORBA::NO_PERMISSION system exception.
    */
    virtual void rebind_context(const CosNaming::Name& n,
                                CosNaming::NamingContext_ptr nc)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Delegates to the actual read/write naming context.
    */
    virtual CORBA::Object_ptr resolve(const CosNaming::Name& n)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Throws CORBA::NO_PERMISSION system exception.
    */
    virtual void unbind(const CosNaming::Name& n)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Throws CORBA::NO_PERMISSION system exception.
    */
    virtual CosNaming::NamingContext_ptr new_context()
        throw(CORBA::SystemException);


    /**
    * Purpose:
    * <p>
    * Throws CORBA::NO_PERMISSION system exception.
    */
    virtual CosNaming::NamingContext_ptr bind_new_context(const CosNaming::Name& n)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::AlreadyBound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);


    /**
    * Purpose:
    * <p>
    * Throws CORBA::NO_PERMISSION system exception.
    */
    virtual void destroy()
        throw(CosNaming::NamingContext::NotEmpty,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Delegates to the actual read/write naming context.
    */
    virtual void list(CORBA::ULong how_many,
                      CosNaming::BindingList_out bl,
                      CosNaming::BindingIterator_out bi)
        throw(CORBA::SystemException);


    /**
    * Purpose:
    * <p>
    * Delegates to the actual read/write naming context.
    */
    virtual CosNaming::NamingContextExt::StringName to_string(const CosNaming::Name& n)
        throw(CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Delegates to the actual read/write naming context.
    */
    virtual CosNaming::Name* to_name(const char* sn)
        throw(CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Delegates to the actual read/write naming context.
    */
    virtual CosNaming::NamingContextExt::URLString to_url(const char* addr,
                                                          const char* sn)
        throw(CosNaming::NamingContextExt::InvalidAddress,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Delegates to the actual read/write naming context.
    */
    virtual CORBA::Object_ptr resolve_str(const char* sn)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);


    /**
    * Purpose:
    * <p>
    * The destructor.
    */
    virtual ~RONamingContext_impl();


public:

    /**
    * Purpose:
    * <p>
    * Creates a servant with the specified id, activates this servant,
    * and returns the corresponding CORBA object.
    * The actual read/write naming context is also created.
    *
    * @param id            The identifier of the servant to create
    * @param actualContext The associated actual read/write naming context
    * that has been created
    * @return   The reference to the CORBA object
    * @exception AlreadyExistsException  if the id already exists
    * @exception OutOfResourcesException if the system is running out of resources
    * @exception AssertionFailedException if initialization has not been performed
    */
    static CosNaming::NamingContextExt_ptr createContext(
            const std::string& id, CosNaming::NamingContextExt_out actualContext)
            throw (AlreadyExistsException, OutOfResourcesException,
            AssertionFailedException, InternalErrorException);


protected:

    /**
    * Purpose:
    * <p>
    * The constructor.
    */
    RONamingContext_impl(const std::string& id,
        CosNaming::NamingContextExt_ptr actualContext);


private:

    /**
    * The actual read/write naming context
    */
    CosNaming::NamingContextExt_var m_actualContext;


private:

    /**
    * Purpose:
    * <p>
    * Returns the RONamingContext_impl servant instance corresponding
    * to the specified identifier. This instance is initialised with
    * the associated read/write naming context.
    *
    * @param id The full identifier of the read only naming context.
    * @return   The servant.
    * @exception NotFoundException if the servant cannot be found 
    */
    static RONamingContext_impl* findServantById(
            const std::string& id)
            throw (NotFoundException, OutOfResourcesException, InternalErrorException);


}; //End of RONamingContext_impl

} // End of namespace NamingAndRepository
} // End of namespace Cdmw

#endif

