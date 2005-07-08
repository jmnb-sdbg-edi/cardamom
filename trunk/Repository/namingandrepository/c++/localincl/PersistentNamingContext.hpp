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


#ifndef INCL_NAMINGANDREPOSITORY_PERSISTENTNAMINGCONTEXT_HPP
#define INCL_NAMINGANDREPOSITORY_PERSISTENTNAMINGCONTEXT_HPP

#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/ReaderWriterLock.hpp"
#include "namingandrepository/Exceptions.hpp"
#include <string>
#include <map>
#include <set>
#include <list>


namespace Cdmw
{
namespace NamingAndRepository
{


enum BindingType { ObjectBinding, NamingContextBinding };


/**
*Purpose:
*<p>
*Transient Binding.
*<p>
*Features:
*<p>
*
*@see PersistentNamingContext::addBinding
*@see PersistentNamingContext
*/
class Binding
{

public:

    /**
    * The stringified object reference
    */
    std::string m_objectRefString;

    /**
    * Indicates whether this binding corresponds to a naming context
    */
    BindingType m_type;

public:

    /**
    * Purpose:
    * <p>
    * Constructor.
    * 
    */ 
    Binding();

    /**
    * Purpose:
    * <p>
    * Constructor.
    * 
    */ 
    Binding(const std::string& object, BindingType type);

    /**
    * Purpose:
    * <p>
    * Copy constructor.
    *
    */ 
    Binding(const Binding& rhs);

    /**
    * Purpose:
    * <p>
    * Destructor.
    * 
    */ 
    virtual ~Binding();

    /**
    * Purpose:
    * <p>
    * Assignment operator.
    * 
    */ 
    Binding& operator=(const Binding& rhs);

};


/**
*Purpose:
*<p>
*Transient information about a binding.
*<p>
*Features:
*<p>
*
*@see PersistentNamingContext::BindingInfoList
*@see PersistentNamingContext::list
*/
class BindingInfo
{

public:

    /**
    * The name associated with this binding
    */
    std::string m_bindingName;

    /**
    * Indicates whether this binding corresponds to a naming context
    */
    BindingType m_bindingType;

public:

    /**
    * Purpose:
    * <p>
    * Constructor.
    * 
    */ 
    BindingInfo();

    /**
    * Purpose:
    * <p>
    * Constructor.
    * 
    */ 
    BindingInfo(const std::string& name, const Binding& binding);

    /**
    * Purpose:
    * <p>
    * Copy constructor.
    *
    */ 
    BindingInfo(const BindingInfo& rhs);

    /**
    * Purpose:
    * <p>
    * Destructor.
    * 
    */ 
    virtual ~BindingInfo();

    /**
    * Purpose:
    * <p>
    * Assignment operator.
    * 
    */ 
    BindingInfo& operator=(const BindingInfo& rhs);

};



/**
*Purpose:
*<p>
*Transient detailed information about a binding.
*<p>
*Features:
*<p>
*
*@see BindingInfo
*@see PersistentNamingContext::BindingInfoList
*@see PersistentNamingContext::list
*/
class DetailedBindingInfo : public BindingInfo
{

public:

    /**
    * The stringified object reference
    */
    std::string m_objectRefString;

public:

    /**
    * Purpose:
    * <p>
    * Constructor.
    * 
    */ 
    DetailedBindingInfo();

    /**
    * Purpose:
    * <p>
    * Constructor.
    * 
    */ 
    DetailedBindingInfo(const std::string& name, const Binding& binding);

    /**
    * Purpose:
    * <p>
    * Copy constructor.
    *
    */ 
    DetailedBindingInfo(const DetailedBindingInfo& rhs);

    /**
    * Purpose:
    * <p>
    * Destructor.
    * 
    */ 
    virtual ~DetailedBindingInfo();

    /**
    * Purpose:
    * <p>
    * Assignment operator.
    * 
    */ 
    DetailedBindingInfo& operator=(const DetailedBindingInfo& rhs);

    /**
    * Purpose:
    * <p>
    * atomic swap.
    * 
    */ 
    void swap(DetailedBindingInfo& rhs);


};



/**
*Purpose:
*<p>
*The persistent peer of a NamingContext.
*<p>
*Features:
*<p>
*Thread safe.
*Persistent in a next release.
*
*@see
*@link
*/
class PersistentNamingContext
{

public:

/**
*Purpose:
*<p>
*The types used for retrieving the list of bindings of a given
*PersistentNamingContext in a thread safe manner.
*/
typedef std::list<BindingInfo> BindingInfoList;

typedef std::list<DetailedBindingInfo> DetailedBindingInfoList;


private:

/**
*Purpose:
*<p>
*The type used to manage the bindings of a given PersistentNamingContext.
*Currently, this type uses the transient binding class as the persistency
*of the PersistentNamingContext class is not yet achieved.
*<p>
*/
typedef std::map<std::string, Binding> Bindings;

//class PersistentNamingContext;

/**
*Purpose:
*<p>
*The type used to manage the existing PersistentNamingContext instances.
*/
typedef std::map<std::string, PersistentNamingContext*> NamingContexts;


public:

    /**
    * Purpose:
    * <p>
    * Default constructor.
    */ 
    PersistentNamingContext();


    /**
    * Purpose:
    * <p>
    * Copy constructor.
    *
    */ 
    PersistentNamingContext(const PersistentNamingContext& rhs);


    /**
    * Purpose:
    * <p>
    * Destructor.
    * 
    */ 
    virtual ~PersistentNamingContext();


    /**
    * Purpose:
    * <p>
    * Assignment operator.
    * 
    */ 
    PersistentNamingContext& operator=(const PersistentNamingContext& rhs);


    /**
    * Purpose:
    * <p>
    * atomic swap.
    * 
    */ 
    void swap(PersistentNamingContext& rhs);


public:
     
    /**
    * Purpose:
    * <p>
    * Returns the full identifier of this naming context.
    */ 
    const std::string& id(void) { return m_id; }


    /**
    * Purpose:
    * <p>
    * Indicates whether this naming context is empty or not.
    */ 
    bool isEmpty(void);
    
    
    /**
    * Purpose:
    * <p>
    * Indicates whether a binding exists with the specified name.
    * 
    * @param  name The name specifying the binding
    * @return true if the binding exists
    */ 
    bool existsBinding(const std::string& name)
        throw (OutOfResourcesException);

    /**
    * Purpose:
    * <p>
    * Indicates whether a binding exists with the specified name.
    * 
    * @param  name The name specifying the binding
    * @param  type set to the type of the found binding
    * @return true if the binding exists
    */ 
    bool existsBinding(const std::string& name, BindingType& type)
        throw (OutOfResourcesException);


    /**
    * Purpose:
    * <p>
    * Finds the stringified object associated with the specified name.
    * 
    * @param  name The name specifying the binding
    * @return The stringified object or the empty string if not found
    */ 
    std::string findObject(const std::string& name)
        throw (OutOfResourcesException);

    /**
    * Purpose:
    * <p>
    * Finds the stringified naming context associated with the specified name.
    * 
    * @param  name    The name specifying the binding
    * @param  context set to true if the binding denotes a naming context
    * @return The stringified naming context or the empty string if not found
    */ 
    std::string findNamingContext(const std::string& name, bool& context)
        throw (OutOfResourcesException);

    /**
    * Purpose:
    * <p>
    * Adds a new binding
    * 
    * @param name    The name specifying the binding
    * @param binding The binding
    * @return        True if the binding has been added.
    */ 
    bool addBinding(const std::string& name, const Binding& binding)
        throw (OutOfResourcesException);


    /**
    * Purpose:
    * <p>
    * Adds a new binding or modifies an existing binding.
    * 
    * @param name    The name specifying the binding
    * @param object  The stringified object reference
    * @param type    The type of the binding
    * @return        True if a new binding has been added.
    */ 
    bool updateBinding(const std::string& name, const std::string& object,
        BindingType type) throw (OutOfResourcesException);


    /**
    * Purpose:
    * <p>
    * Removes an existing binding.
    * 
    * @param name The name specifying the binding
    * @return     True if the removal has been performed.
    *             False if the name doesn't exist.
    */ 
    bool removeBinding(const std::string& name);


    /**
    * Purpose:
    * <p>
    * Returns the list of bindings of this naming context.
    */ 
    BindingInfoList list(void)
        throw (OutOfResourcesException);


    /**
    * Purpose:
    * <p>
    * Returns the detailed list of bindings of this naming context.
    */ 
    DetailedBindingInfoList detailedList(void)
        throw (OutOfResourcesException);

public:

    /**
    * Purpose:
    * <p>
    * Creates a new PersistentNamingContext instance with the supplied prefix.
    * 
    * @param prefix The prefix that will be used for the generated full
    * identifier
    * @return The created instance
    */ 
    static PersistentNamingContext* createWithPrefix(const std::string& prefix)
        throw (OutOfResourcesException);
    
    /**
    * Purpose:
    * <p>
    * Creates a new PersistentNamingContext instance with the supplied identifier.
    * 
    * @param id The full identifier of the instance to create
    * @return   The created instance
    * @exception AlreadyExistsException If id already exists
    */ 
    static PersistentNamingContext* createWithId(const std::string& id)
            throw (AlreadyExistsException, OutOfResourcesException);

    /**
    * Purpose:
    * <p>
    * Destroy the PersistentNamingContext instance from the persistent storage.
    * 
    * @return true if the destruction has been correctly performed
    */ 
    static bool destroy(PersistentNamingContext* namingContext);


    /**
    * Purpose:
    * <p>
    * Indicates whether a PersistentNamingContext having the specified id
    * exists.
    *
    */ 
    static bool exists(const std::string& id)
        throw (OutOfResourcesException);


    /**
    * Purpose:
    * <p>
    * Returns the persistent naming context instance corresponding
    * to the specified id or NULL if not found.
    *
    */ 
    static PersistentNamingContext* findById(const std::string& id)
        throw (OutOfResourcesException);


protected:
    /**
    * Purpose:
    * <p>
    * Constructs a persistent naming context with the supplied id.
    */ 
    PersistentNamingContext(const std::string& id);


private:

    /**
    * The identifier of this naming context
    */
    std::string m_id;

    /**
    * The reader/writer lock used to manage concurrent thread safe access to
    * m_bindings.
    */
    Cdmw::OsSupport::ReaderWriterLock m_rwLock;
    
    /**
    * The bindings contained in this naming context
    */
    Bindings m_bindings;


private:

    /**
    * The mutex used to manage concurrent thread safe access to
    * M_current_gen_id and M_existingNamingContexts.
    */
    static Cdmw::OsSupport::Mutex M_existingNamingContexts_mutex;

    /**
    * The counter used for generated identifier.
    */
    static unsigned long M_current_gen_id;

    /**
    * The existing naming contexts.
    */
    static NamingContexts M_existingNamingContexts;


private:

    /**
    * Purpose:
    * <p>
    * Indicates whether a PersistentNamingContext having the specified id
    * exists.
    *
    * Unlike exists, this method is not thread safe and must be used with
    * a lock on M_existingNamingContexts_mutex.
    */ 
    static bool existsId(const std::string& id)
        throw (OutOfResourcesException);

    /**
    * Purpose:
    * <p>
    * Creates a new PersistentNamingContext instance
    * with the supplied identifier.
    * 
    * This method is not thread safe and must be used with
    * a lock on M_existingNamingContexts_mutex.
    *
    * @param id The identifier
    * @return The created instance
    */ 
    static PersistentNamingContext* create(const std::string& id)
        throw (OutOfResourcesException);



};



} // End of namespace NamingAndRepository
} // End of namespace Cdmw

#endif

