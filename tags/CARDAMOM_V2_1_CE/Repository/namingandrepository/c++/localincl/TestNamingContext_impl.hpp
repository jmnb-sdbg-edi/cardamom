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


#ifndef INCL_NAMINGANDREPOSITORY_TESTNAMINGCONTEXT_IMPL_HPP
#define INCL_NAMINGANDREPOSITORY_TESTNAMINGCONTEXT_IMPL_HPP


#include "namingandrepository/NamingContext_impl.hpp"
#include "Foundation/osthreads/Mutex.hpp"



namespace Cdmw
{
namespace NamingAndRepository
{


/**
*Purpose:
*<p>
*Test class for NamingContext_impl.
*Test the thread safe behaviour.
*<p>
*Features:
*<p>
*Thread safe.
*
*@see NamingContext_impl
*/
class TestNamingContext_impl : virtual public NamingContext_impl
{

public:

    /**
    * Purpose:
    * <p>
    * Traces the number of threads accessing this naming context
    * and performs the operation.
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
    * Traces the number of threads accessing this naming context
    * and performs the operation.
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
    * Traces the number of threads accessing this naming context
    * and performs the operation.
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
    * Traces the number of threads accessing this naming context
    * and performs the operation.
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
    * Traces the number of threads accessing this naming context
    * and performs the operation.
    */
    virtual CORBA::Object_ptr resolve(const CosNaming::Name& n)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Traces the number of threads accessing this naming context
    * and performs the operation.
    */
    virtual void unbind(const CosNaming::Name& n)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);
	      
    /**
    * Purpose:
    * <p>
    * Traces the number of threads accessing this naming context
    * and performs the operation.
    */
    virtual CosNaming::NamingContext_ptr new_context()
        throw(CORBA::SystemException);


    /**
    * Purpose:
    * <p>
    * Traces the number of threads accessing this naming context
    * and performs the operation.
    */
    virtual void destroy()
        throw(CosNaming::NamingContext::NotEmpty,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Traces the number of threads accessing this naming context
    * and performs the operation.
    */
    virtual void list(CORBA::ULong how_many,
                      CosNaming::BindingList_out bl,
                      CosNaming::BindingIterator_out bi)
        throw(CORBA::SystemException);



private:

    /**
    * The mutex used to manage concurrent thread safe access to
    * M_current_gen_id and M_existingNamingContexts.
    */
    Cdmw::OsSupport::Mutex m_nbThreads_mutex;

    /**
    * The number of threads accessing the naming context
    */
    unsigned short m_nbThreads;


public:

    /**
    * Purpose:
    * <p>
    * Creates a servant with the specified id, activates this servant,
    * and returns the corresponding CORBA object.
    * The servant's persistent peer is also created.
    *
    * @return   The reference to the CORBA object.
    * @exception Cdmw::Exception if initialization has not been performed
    */
    static CosNaming::NamingContextExt_ptr create(const std::string& id)
            throw (AlreadyExistsException, OutOfResourcesException,
            AssertionFailedException, InternalErrorException);


protected:

    /**
    * Purpose:
    * <p>
    * Increments the number of threads and displays the latter.
    */
    void enterMethod(const std::string& method);

    /**
    * Purpose:
    * <p>
    * Decrements the number of threads and displays the latter.
    */
    void exitMethod(const std::string& method);
    
    /**
    * Purpose:
    * <p>
    * The constructor.
    */
    TestNamingContext_impl(PersistentNamingContext *persistent_peer);

    /**
    * Purpose:
    * <p>
    * The destructor.
    */
    virtual ~TestNamingContext_impl();


}; //End of TestNamingContext_impl

} // End of namespace NamingAndRepository
} // End of namespace Cdmw

#endif

