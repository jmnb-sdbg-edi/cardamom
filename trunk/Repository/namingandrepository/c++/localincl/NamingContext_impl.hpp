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


#ifndef INCL_NAMINGANDREPOSITORY_NAMINGCONTEXT_IMPL_HPP
#define INCL_NAMINGANDREPOSITORY_NAMINGCONTEXT_IMPL_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/CosNaming.skel.hpp"
#include "namingandrepository/Exceptions.hpp"


namespace Cdmw
{
namespace NamingAndRepository
{

class PersistentNamingContext;

/**
*Purpose:
*<p>
*Actual read/write implementation of the CosNaming::NamingContextExt
*IDL interface.
*All instances of NamingContext_impl and its derived class are managed by
*a unique POA.
*<p>
*Features:
*<p>
*Thread safe.
*
*@see RootNamingContext_impl
*@see RONamingContext_impl
*@see NameDomainContext_impl
*/
class NamingContext_impl : virtual public POA_CosNaming::NamingContextExt,
        virtual public PortableServer::RefCountServantBase
{

    friend class NamingContextActivator_impl;
    friend class NamingTemplates;

public:

    /**
    * The prefix constant for root naming context.
    */
    static const char* ROOT_CONTEXT_PREFIX;
    
    /**
    * The prefix constant for read/write naming context.
    */
    static const char* READ_WRITE_CONTEXT_PREFIX;
    
    /**
    * The prefix constant for read only naming context.
    */
    static const char* READ_ONLY_CONTEXT_PREFIX;

    /**
    * The prefix length constant.
    */
    static const int PREFIX_LENGTH;
    

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContext/bind:1.0
    * operation
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
    * Implements the
    * IDL:omg.org/CosNaming/NamingContext/rebind:1.0
    * operation
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
    * Implements the
    * IDL:omg.org/CosNaming/NamingContext/bind_context:1.0
    * operation
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
    * Implements the
    * IDL:omg.org/CosNaming/NamingContext/rebind_context:1.0
    * operation
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
    * Implements the
    * IDL:omg.org/CosNaming/NamingContext/resolve:1.0
    * operation
    */
    virtual CORBA::Object_ptr resolve(const CosNaming::Name& n)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContext/unbind:1.0
    * operation
    */
    virtual void unbind(const CosNaming::Name& n)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContext/new_context:1.0
    * operation
    */
    virtual CosNaming::NamingContext_ptr new_context()
        throw(CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContext/bind_new_context:1.0
    * operation
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
    * Implements the
    * IDL:omg.org/CosNaming/NamingContext/destroy:1.0
    * operation
    */
    virtual void destroy()
        throw(CosNaming::NamingContext::NotEmpty,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContext/list:1.0
    * operation
    */
    virtual void list(CORBA::ULong how_many,
                      CosNaming::BindingList_out bl,
                      CosNaming::BindingIterator_out bi)
        throw(CORBA::SystemException);
	
	
    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContextExt/to_string:1.0
    * operation
    */
    virtual CosNaming::NamingContextExt::StringName to_string(const CosNaming::Name& n)
        throw(CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContextExt/to_name:1.0
    * operation
    */
    virtual CosNaming::Name* to_name(const char* sn)
        throw(CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContextExt/to_url:1.0
    * operation
    *
    * Not yet implemented.
    */
    virtual CosNaming::NamingContextExt::URLString to_url(const char* addr,
                                                          const char* sn)
        throw(CosNaming::NamingContextExt::InvalidAddress,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContextExt/resolve_str:1.0
    * operation
    */
    virtual CORBA::Object_ptr resolve_str(const char* sn)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);


    /**
    * Purpose:
    * <p>
    * Redefines the PortableServer::ServantBase method
    * to return the correct POA.
    *
    * @return The M_POA member
    */
    virtual PortableServer::POA_ptr _default_POA();


    /**
    * Purpose:
    * <p>
    * Destroys this instance. If the flag m_destroyed is true, the persistent
    * data is actually destroyed from the persistent storage.
    */
    virtual ~NamingContext_impl();
  

public:

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
    * Creates a servant with the specified id, activates this servant,
    * and returns the corresponding CORBA object.
    * The servant's persistent peer is also created.
    *
    * @param id The full identifier of the servant to create.
    * @return   The reference to the CORBA object.
    * @exception AlreadyExistsException if the id already exists
    * @exception OutOfResourcesException if the system is running out of resources
    * @exception AssertionFailedException if initialization has not been performed
    */
    static CosNaming::NamingContextExt_ptr createWithUserId(
            const std::string& id)
            throw (AlreadyExistsException, OutOfResourcesException,
            AssertionFailedException, InternalErrorException);

    /**
    * Purpose:
    * <p>
    * Creates a servant with a generated id, activates this servant,
    * and returns the corresponding CORBA object.
    * The servant's persistent peer is also created.
    *
    * @param generated_id The full identifier that has been generated.
    * @return             The reference to the CORBA object
    * @exception OutOfResourcesException if the system is running out of resources
    * @exception AssertionFailedException if initialization has not been performed
    */
    static CosNaming::NamingContextExt_ptr createWithSystemId(
            std::string& generated_id)
            throw (OutOfResourcesException,
            AssertionFailedException, InternalErrorException);

    /**
    * Purpose:
    * <p>
    * Returns the CORBA object corresponding to the specified
    * identifier. 
    *
    * @param id The full identifier of the naming context to search for.
    * @return   The CORBA object or NIL reference if the identifier
    *           doesn't denote an existing naming context.
    * @exception AssertionFailedException if initialization has not been performed
    */
    static CosNaming::NamingContextExt_ptr findById(
            const std::string& id)
            throw (AssertionFailedException, InternalErrorException);


protected:

    /**
    * Purpose:
    * <p>
    * Constructs a new NamingContext_impl instance associated with the
    * persistent peer. The identifier of this instance is retrieved from the
    * persistent peer.
    */
    NamingContext_impl(PersistentNamingContext *persistentPeer);

    /**
    * Purpose:
    * <p>
    * Constructs a new NamingContext_impl instance with the specified id but
    * without persistent peer. This constructor is used by derived class that
    * has no actual persistent data.
    */
    NamingContext_impl(const std::string& id);


protected:

    /**
    * Purpose:
    * <p>
    * Returns true if a naming context with the specified id exists.
    *
    * @param id The full identifier of the naming context to search for
    * @return   The result of the search
    */
    static bool exists(const std::string& id);


private:

    /**
    * The identifier of this naming context.
    */
    std::string m_id;

    /**
    * The persistent peer.
    */
    PersistentNamingContext *m_persistentPeer;

    /**
    * The flag indicating whether this naming context has been destroyed.
    */
    bool m_destroyed;


private:

    /**
    * The POA in charge of NamingContext_impl and its derived class.
    */
    static PortableServer::POA_ptr M_POA;


private:

    /**
    * Purpose:
    * <p>
    * Resolves the simple name within this naming context.
    *
    * @param n   The name
    * @return    The resolved object
    * @exception InvalidName if n is not a simple name or
    * if n is invalid
    * @exception NotFound if the object cannot be found
    */
    CORBA::Object_ptr resolveObject( const CosNaming::Name& n)
        throw(CosNaming::NamingContext::InvalidName,
        CosNaming::NamingContext::NotFound,
        OutOfResourcesException, InternalErrorException);

    /**
    * Purpose:
    * <p>
    * Resolves the first name component within this naming context.
    * The resolved object is expected to be a naming context.
    *
    * @param n   The name
    * @return    The resolved naming context
    * @exception InvalidName if the first name component is invalid
    * @exception NotFound if the object cannot be found
    * or is not a naming context
    */
    CosNaming::NamingContext_ptr resolveNamingContext(
        const CosNaming::Name& n)
        throw(CosNaming::NamingContext::InvalidName,
	CosNaming::NamingContext::NotFound,
        CosNaming::NamingContext::CannotProceed,
        OutOfResourcesException, InternalErrorException);


private:

    /**
    * Purpose:
    * <p>
    * Creates a servant with a generated id, activates this servant,
    * and returns the corresponding CORBA object.
    *
    * If the creation cannot be performed, the persistent peer is destroyed.
    *
    * @param persistent_peer The persistent peer
    * @return                The reference to the CORBA object
    * @exception OutOfResourcesException if the system is running out of resources
    * @exception AssertionFailedException if initialization has not been performed
    */
    static CosNaming::NamingContextExt_ptr create(
            PersistentNamingContext *persistentPeer)
            throw (OutOfResourcesException,
            AssertionFailedException, InternalErrorException);

    /**
    * Purpose:
    * <p>
    * Returns the NamingContext_impl servant instance corresponding
    * to the specified identifier. This instance is initialised with
    * the associated persistent data.
    *
    * @param id The full identifier of the naming context
    * @return   The servant
    * @exception NotFoundException if the servant cannot be found 
    * (no persistent data corresponding to the id)
    */
    static NamingContext_impl* findServantById(
            const std::string& id)
            throw (NotFoundException, OutOfResourcesException, InternalErrorException);

  
}; //End of NamingContext_impl

} // End of namespace NamingAndRepository
} // End of namespace Cdmw

#endif

