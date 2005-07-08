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


#ifndef INCL_NAMINGANDREPOSITORY_NAMEDOMAINCONTEXT_IMPL_HPP
#define INCL_NAMINGANDREPOSITORY_NAMEDOMAINCONTEXT_IMPL_HPP


#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/CosNaming.skel.hpp"
#include "namingandrepository/Exceptions.hpp"

#include <string>


namespace Cdmw
{
namespace NamingAndRepository
{


/**
*Purpose:
*<p>
*The implementation of the CosNaming::NamingContextExt IDL interface corresponding
*to a local name domain.
*Write access operations from this implementation raises the CORBA::NO_PERMISSION
*system exception. Read access operations are delegated to the actual read/write 
*naming context as this implementation corresponds to a local name domain.
*<p>
*Features:
*<p>
*Thread safe.
*
*@see NamingContext_impl
*/
class NameDomainContext_impl : virtual public POA_CosNaming::NamingContextExt,
        virtual public PortableServer::RefCountServantBase
{

    friend class NameDomainContextActivator_impl;
    friend class NameDomain_impl;
    friend class SystemFactoryFinder;

public:

    /**
    * The prefix length constant.
    */
    static const int PREFIX_LENGTH;

    /**
    * The prefix constant for local name domain.
    */
    static const char* LOCAL_PREFIX;
    
    /**
    * The prefix constant for distributed name domain.
    */
    static const char* DISTRIBUTED_PREFIX;

    /**
    * The name for the context containing name domain's factories
    */
    static const char* FACTORIES_NAME;
    
    /**
    * The suffix constant for the context containing name domain's factories
    */
    static const char* FACTORIES_SUFFIX;

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
    * Redefines the PortableServer::ServantBase method
    * to return the correct POA.
    *
    * @return The M_POA member
    */
    virtual PortableServer::POA_ptr _default_POA();


    /**
    * The destructor.
    */
    virtual ~NameDomainContext_impl();


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
    * Returns the ObjectId of the specified name domain context.
    *
    * @param nameDomainContext The name domain context
    * @return The ObjectId of the specified name domain context or an empty ObjectId
    * if nameDomainContext is not actually a system name domain context.
    * @exception AssertionFailedException If initialization has not been performed
    */
    static PortableServer::ObjectId* getObjectId(CORBA::Object_ptr nameDomainContext)
            throw (AssertionFailedException, OutOfMemoryException);


    /**
    * Purpose:
    * <p>
    * Returns the stringified id of the specified name domain context.
    *
    * @param nameDomainContext The name domain context
    * @return The id of the specified name domain context or an empty string
    * if nameDomainContext is not actually a system name domain context.
    */
    static std::string getId(CORBA::Object_ptr nameDomainContext)
        throw (AssertionFailedException, OutOfMemoryException, InternalErrorException);


    /**
    * Purpose:
    * <p>
    * Indicated whether the specified object is a name domain context
    * managed by the repository.
    *
    * @param nameDomainContext The name domain context
    * @return true if the specified object is a name domain context
    * managed by the repository.
    * @exception AssertionFailedException If initialization has not been performed
    */
    static bool isNameDomain(CORBA::Object_ptr obj)
            throw (AssertionFailedException, OutOfMemoryException);


    /**
    * Purpose:
    * <p>
    * Creates a servant, activates this servant, and returns the corresponding
    * CORBA object.
    * The actual read/write naming context and the contexts containing the
    * factories (read only and associated read/write) are also created.
    *
    * @param nameDomainId The full identifier of the created name domain context
    * @param actualContext The actual context of the created name domain context
    * @param actualFactoriesContext The actual context of the factories context
    * @return   The reference to the CORBA object
    * @exception OutOfResourcesException if the system is running out of resources
    * @exception AssertionFailedException if initialization has not been performed
    */
    static CosNaming::NamingContextExt_ptr createContext(
            std::string& nameDomainId, 
            CosNaming::NamingContextExt_out actualContext,
            CosNaming::NamingContextExt_out actualFactoriesContext)
            throw (OutOfResourcesException,
            AssertionFailedException, InternalErrorException);

    /**
    * Purpose:
    * <p>
    * Destroys completely a name domain context.
    *
    * This method is intended to be used only when errors occured during
    * initialization.
    *    
    * @param nameDomainId The full identifier of the created name domain context
    * @return true if the destruction has been correctly performed
    */
    static bool destroyContext(const std::string& nameDomainId);
    

protected:

    /**
    * The constructor.
    */
    NameDomainContext_impl(CosNaming::NamingContextExt_ptr actualContext);
    

protected:

    /**
    * Purpose:
    * <p>
    * Destroys the actual naming contexts.
    *
    * This method is intended to be used only when errors occured during
    * initialization.
    *
    * @param actualContext The actual context of the name domain context
    * @param actualFactoriesContext The actual context of the factories context
    * @return true if the cleanup has been performed completely
    */
    static bool cleanUp(
            CosNaming::NamingContextExt_ptr actualContext,
            CosNaming::NamingContextExt_ptr actualFactoriesContext)
            throw ();


private:

    /**
    * The actual read/write naming context
    */
    CosNaming::NamingContextExt_var m_actualContext;


    /**
    * The POA in charge of NameDomainContext_impl and its derived class.
    */
    static PortableServer::POA_ptr M_POA;


private:

    /**
    * Purpose:
    * <p>
    * Returns the id of the read/write context associated with the name domain.
    *
    * @param id The full identifier of the name domain context.
    */
    static std::string getActualContextId(const std::string& id);


    /**
    * Purpose:
    * <p>
    * Returns the id of the read/write factories' context associated
    * with the name domain.
    *
    * @param id The full identifier of the name domain context.
    */
    static std::string getActualFactoriesContextId(const std::string& id);


    /**
    * Purpose:
    * <p>
    * Returns the NameDomainContext_impl servant instance corresponding
    * to the specified identifier. This instance is initialised with
    * the associated read/write naming context.
    *
    * @param id The full identifier of the name domain context.
    * @return   The servant.
    * @exception NotFoundException if the servant cannot be found 
    */
    static NameDomainContext_impl* findServantById(
            const std::string& id)
            throw (NotFoundException, OutOfResourcesException, InternalErrorException);


      
    /**
    * Purpose:
    * <p>
    * Returns the read/write context associated with the name domain
    * specified by the id.
    *
    * @param id The full identifier of the name domain.
    * @return   The context
    * @exception NotFoundException if id doesn't denote an existing name domain
    */
    static CosNaming::NamingContextExt_ptr findActualContext(const std::string& id)
        throw (NotFoundException, OutOfResourcesException, InternalErrorException);

      
    /**
    * Purpose:
    * <p>
    * Returns the read/write factories' context associated with the name domain
    * specified by the id.
    *
    * @param id The full identifier of the name domain.
    * @return   The context
    * @exception NotFoundException if id doesn't denote an existing name domain
    */
    static CosNaming::NamingContextExt_ptr findActualFactoriesContext(const std::string& id)
        throw (NotFoundException, OutOfResourcesException, InternalErrorException);


}; //End of NameDomainContext_impl

} // End of namespace NamingAndRepository
} // End of namespace Cdmw

#endif

