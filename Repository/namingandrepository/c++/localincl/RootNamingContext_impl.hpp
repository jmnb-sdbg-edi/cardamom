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


#ifndef INCL_NAMINGANDREPOSITORY_ROOTNAMINGCONTEXT_IMPL_HPP
#define INCL_NAMINGANDREPOSITORY_ROOTNAMINGCONTEXT_IMPL_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "namingandrepository/NamingContext_impl.hpp"


namespace Cdmw
{
namespace NamingAndRepository
{


/**
*Purpose:
*<p>
*Implementation of the CosNaming::NamingContextExt IDL interface
*corresponding to a root naming context.
*A root naming context protects name domains for being unbound through
*the unbind and rebind_context operations.
*
*<p>
*Features:
*<p>
*Thread safe.
*
*@see NamingContext_impl
*/
class RootNamingContext_impl : virtual public NamingContext_impl
{

    friend class NamingContextActivator_impl;


public:

    /**
    * Purpose:
    * <p>
    * Throws CORBA::NO_PERMISSION system exception if the object directly bound
    * within this root naming context corresponds to a name domain.
    */
    virtual void unbind(const CosNaming::Name& n)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Throws CORBA::NO_PERMISSION system exception if the object directly bound
    * within this root naming context corresponds to a name domain.
    */
    virtual void rebind_context(const CosNaming::Name& n,
                                CosNaming::NamingContext_ptr nc)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);


    /**
    * The destructor.
    */
    virtual ~RootNamingContext_impl();


public:

    /**
    * Creates a servant with the specified id, activates this servant,
    * and returns the corresponding CORBA object.
    *
    * @param id The identifier of the servant to create
    * @return   The reference to the CORBA object
    * @exception AlreadyExistsException  if the id already exists
    * @exception OutOfResourcesException if the system is running out of resources
    * @exception AssertionFailedException if initialization has not been performed
    */
    static CosNaming::NamingContextExt_ptr createContext(const std::string& id)
            throw (AlreadyExistsException, OutOfResourcesException,
            AssertionFailedException, InternalErrorException);


protected:

    /**
    * The constructor.
    */
    RootNamingContext_impl(PersistentNamingContext *persistent_peer);


private:

    /**
    * Returns the RootNamingContext_impl servant instance corresponding
    * to the specified identifier. This instance is initialised with
    * the associated persistent data.
    *
    * @param id The full identifier of the root naming context
    * @return   The servant
    * @exception NotFoundException if the servant cannot be found 
    * (no persistent data corresponding to the id)
    */
    static RootNamingContext_impl* findServantById(
            const std::string& id)
            throw (NotFoundException, OutOfResourcesException, InternalErrorException);


}; //End of RootNamingContext_impl

} // End of namespace NamingAndRepository
} // End of namespace Cdmw

#endif

