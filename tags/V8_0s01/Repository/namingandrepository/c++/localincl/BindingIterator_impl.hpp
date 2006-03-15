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


#ifndef INCL_NAMINGANDREPOSITORY_BINDINGITERATOR_IMPL_HPP
#define INCL_NAMINGANDREPOSITORY_BINDINGITERATOR_IMPL_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/CosNaming.skel.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "namingandrepository/Exceptions.hpp"


namespace Cdmw
{
namespace NamingAndRepository
{


/**
*Purpose:
*<p>
*Implementation of the CosNaming::BindingIterator IDL interface.
*<p>
*Features:
*<p>
*Thread safe.
*
*@see NamingContext_impl
*/
class BindingIterator_impl : virtual public POA_CosNaming::BindingIterator,
        virtual public PortableServer::RefCountServantBase
{

public:


    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/BindingIterator/next_one:1.0
    * operation
    */
    virtual CORBA::Boolean next_one(CosNaming::Binding_out b)
        throw(CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/BindingIterator/next_n:1.0
    * operation
    */
    virtual CORBA::Boolean next_n(CORBA::ULong how_many,
                                  CosNaming::BindingList_out bl)
        throw(CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/BindingIterator/destroy:1.0
    * operation
    */
    virtual void destroy()
        throw(CORBA::SystemException);


    /**
    * The destructor.
    */
    virtual ~BindingIterator_impl();


public:

    /**
    * Purpose:
    * <p>
    * Extracts one binding from the specified list.
    *
    * @param bl The supplied binding list
    * @param rest_of_binding_list A new binding list corresponding to the rest
    * of bl after the extraction
    * @return The extracted binding
    * @exception BadParameterException If bl is an empty list
    */
    static CosNaming::Binding* extract_binding(const CosNaming::BindingList& bl,
                        CosNaming::BindingList_out rest_of_binding_list)
            throw (BadParameterException, OutOfMemoryException);


    /**
    * Purpose:
    * <p>
    * Creates two binding list by splitting the initial list.
    *
    * @param bl The supplied binding list
    * @param how_many The desired length for the first part of the split
    * @param rest_of_binding_list The second part of the split
    * @return The binding list corresponding to the first part of the split
    */
    static CosNaming::BindingList* split_binding_list(const CosNaming::BindingList& bl,
        CORBA::ULong how_many, CosNaming::BindingList_out rest_of_binding_list)
        throw (OutOfMemoryException);


    /**
    * Purpose:
    * <p>
    * Initializes the M_POA member.
    *
    * @param poa The POA
    * @exception AssertionFailedException if initialization has already been performed
    */
    static void initialize(PortableServer::POA_ptr poa)
            throw (AssertionFailedException);


    /**
    * Purpose:
    * <p>
    * Creates a servant, activates this servant, and returns the corresponding
    * CORBA object.
    *
    * @param bl The supplied binding list
    * @return   The reference to the CORBA object
    * @exception AssertionFailedException if initialization has not been performed
    */
    static CosNaming::BindingIterator_ptr create(const CosNaming::BindingList& bl)
            throw (OutOfMemoryException,
            AssertionFailedException, InternalErrorException);


private:

    /**
    * The mutex used to manage concurrent thread safe access to
    * m_bindingList.
    */
    Cdmw::OsSupport::Mutex m_bindingList_mutex;
    
    /**
    * The binding list of this binding iterator.
    */
    CosNaming::BindingList_var m_bindingList;

    /**
    * The ObjectId of this iterator.
    */
    PortableServer::ObjectId_var m_oid;

private:

    /**
    * The class member that contains the POA in charge
    * of BindingIterators.
    * must have SYSTEM_ID and RETAIN policies.
    *
    * The rootPOA may be used as Binding Iterators will not
    * have persistent references
    */
    static PortableServer::POA_ptr M_POA;


private:

    /**
    * Purpose:
    * <p>
    * Constructs a binding iterator with the supplied binding list.
    */
    BindingIterator_impl(const CosNaming::BindingList& bl);


}; //End of BindingIterator_impl

} // End of namespace NamingAndRepository
} // End of namespace Cdmw

#endif

