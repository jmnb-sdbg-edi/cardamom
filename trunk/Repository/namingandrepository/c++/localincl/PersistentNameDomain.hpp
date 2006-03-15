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


#ifndef INCL_NAMINGANDREPOSITORY_PERSISTENTNAMEDOMAIN_HPP
#define INCL_NAMINGANDREPOSITORY_PERSISTENTNAMEDOMAIN_HPP




#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/ReaderWriterLock.hpp"
#include "namingandrepository/Exceptions.hpp"

#include "namingandrepository/FTDatastoreHelper.hpp"

#include <string>
#include <map>
#include <set>
#include <list>


namespace Cdmw
{
namespace NamingAndRepository
{


/**
* The different types of registration
*/
enum RegistrationType { ObjectRegistration, 
			FactoryRegistration, 
			NameDomainRegistration };



/**
*Purpose:
*<p>
*Transient information about a name domain.
*<p>
*Features:
*<p>
*
*@see Registration
*@see PersistentNameDomain
*/
class NameDomainInfo
{

public:

    /**
    * The stringified name domain reference
    */
    std::string m_nameDomainRefString;

    /**
    * The flag indicating whether the name domain is user-supplied or not.
    */
    bool m_userSupplied;
    
    /**
    * The stringified user-supplied factory finder reference
    */
    std::string m_userFactoryFinderRefString;

public:

    /**
    * Purpose:
    * <p>
    * Default constructor.
    */ 
    NameDomainInfo();

    /**
    * Purpose:
    * <p>
    * Constructs the information for a system name domain.
    */ 
    NameDomainInfo(const std::string& nameDomainRef);

    /**
    * Purpose:
    * <p>
    * Constructs the information for a user-supplied name domain.
    */ 
    NameDomainInfo(const std::string& nameDomainRef,
       const std::string& userFactoryFinderRef);
    
    /**
    * Purpose:
    * <p>
    * Copy constructor.
    */ 
    NameDomainInfo(const NameDomainInfo& rhs);

    /**
    * Purpose:
    * <p>
    * Destructor.
    */ 
    virtual ~NameDomainInfo();

    /**
    * Purpose:
    * <p>
    * Assignment operator.
    * 
    */ 
    NameDomainInfo& operator=(const NameDomainInfo& rhs);

    /**
    * Purpose:
    * <p>
    * atomic swap.
    * 
    */ 
    void swap(NameDomainInfo& rhs);


};




/**
*Purpose:
*<p>
*Persistent name registration.
*<p>
*Features:
*<p>
*Persistent in a next release.
*
*@see PersistentNameDomain
*/
class Registration
{

public:

    /**
    * The name reserved by this registration
    */
    std::string m_name;

    /**
    * The flag indicating whether this registration is activated or not.
    */
    bool m_activated;

    /**
    * The type of this registration
    */
    RegistrationType m_type;

    /**
    * The additional information for a name domain
    * Currently, this member corresponds to the transient NameDomainInfo as
    * the actual persistency is not yet achieved.
    */
    NameDomainInfo* m_nameDomainInfo;

public:

    /**
    * Purpose:
    * <p>
    * Constructor.
    * 
    */ 
    Registration();

    /**
    * Purpose:
    * <p>
    * Constructs a registration that is not yet activated.
    */ 
    Registration(const std::string& name);

    /**
    * Purpose:
    * <p>
    * Constructs a registration that is directly activated.
    */ 
    Registration(const std::string& name, RegistrationType type);

    /**
    * Purpose:
    * <p>
    * Constructs a registration for a name domain that is directly activated.
    */ 
    Registration(const std::string& name, const NameDomainInfo& nameDomainInfo);

    /**
    * Purpose:
    * <p>
    * Copy constructor.
    */ 
    Registration(const Registration& rhs);

    /**
    * Purpose:
    * <p>
    * Destructor.
    * 
    */ 
    virtual ~Registration();

    /**
    * Purpose:
    * <p>
    * Assignment operator.
    * 
    */ 
    Registration& operator=(const Registration& rhs);


    /**
    * Purpose:
    * <p>
    * atomic swap.
    * 
    */ 
    void swap(Registration& rhs);


};





/**
*Purpose:
*<p>
*The persistent peer of a NameDomain.
*<p>
*Features:
*<p>
*Thread safe.
*Persistent in a next release.
*
*@see NameDomain_impl
*/
class PersistentNameDomain
{

public:
	
/**
*Purpose:
*<p>
* Defines the time between registration and activation; assumes host
* in network are synchronising system-clock, eg with NTP.
*<p>
*/
    const TimeBase::TimeT REGISTRATION_TIMEOUT; // 1000*1000*10;
/**
*Purpose:
*<p>
*The registration identifier type.
*<p>
*/
typedef std::string RegistrationId;


    
public:

    /**
    * Purpose:
    * <p>
    * Default constructor.
    */ 
    PersistentNameDomain();

// FIXME - following constructor should be private
    /**
    * Purpose:
    * <p>
    * Copy constructor.
    *
    */ 
    PersistentNameDomain(const PersistentNameDomain& rhs);


    /**
    * Purpose:
    * <p>
    * Destructor.
    * 
    */ 
    virtual ~PersistentNameDomain();


    /**
    * Purpose:
    * <p>
    * Assignment operator.
    * 
    */ 
    PersistentNameDomain& operator=(const PersistentNameDomain& rhs);


    /**
    * Purpose:
    * <p>
    * atomic swap.
    * 
    */ 
    void swap(PersistentNameDomain& rhs);


public:
    
    /**
    * Purpose:
    * <p>
    * Returns the full identifier.
    */ 
    const std::string& id(void) { return m_id; }

    
    /**
    * Purpose:
    * <p>
    * Indicates whether the name is reserved for a name domain.
    * 
    * @param name The name to search for
    * @param isSystem Indicates whether the name domain is a system one
    * @return The result of the search
    */ 
    bool existsNameDomain(const std::string& name, bool& isSystem)
        throw (OutOfResourcesException);


    /**
    * Purpose:
    * <p>
    * Finds the stringified name domain reference associated
    * with the specified name domain.
    * 
    * @param name The name that has been reserved for the searched name domain
    * @return The stringified name domain reference
    * or the empty string if not found
    */ 
    std::string findNameDomain(const std::string& name)
        throw (OutOfResourcesException);

    /**
    * Purpose:
    * <p>
    * Finds the stringified factory finder associated
    * with the specified name domain.
    * 
    * @param name The name that has been reserved for the searched name domain
    * @return The stringified factory finder reference
    * or the empty string if not found or the name domain is not a user supplied one
    */ 
    std::string findFactoryFinder(const std::string& name)
        throw (OutOfResourcesException);

    /**
    * Purpose:
    * <p>
    * Adds a registration that is not yet activated.
    * 
    * @param name    The name reserved by the registration
    * @param regId   The identifier of the registration if successful
    * @return        True if successful (= the name was not reserved
    * by an existing registration)
    */ 
    bool addRegistration(const std::string& name,
        RegistrationId& regId)
        throw (OutOfResourcesException);

    /**
    * Purpose:
    * <p>
    * Adds a registration that is directly activated. This method should not
    * be used for a name domain.
    * 
    * @param name    The name reserved by the registration
    * @param type    The type of the registration
    * @return        True if successful (= the name was not reserved
    * by an existing registration and the registration is not for a name domain)
    */ 
    bool addRegistration(const std::string& name, RegistrationType type)
        throw (OutOfResourcesException);

    /**
    * Purpose:
    * <p>
    * Adds a registration for a name domain that is directly activated.
    * 
    * @param name          The name reserved by the registration
    * @param nameDomainInfo The name domain information
    * @return        True if successful (= the name was not reserved
    * by an existing registration)
    */ 
    bool addRegistration(const std::string& name,
        const NameDomainInfo& nameDomainInfo)
        throw (OutOfResourcesException);

    /**
    * Purpose:
    * <p>
    * Returns the name reserved by the specified registration.
    * 
    * @param regId   The identifier of the registration
    * @return        The name of the empty string if not found.
    */ 
    std::string findName(const RegistrationId& regId)
        throw (OutOfResourcesException);

    /**
    * Purpose:
    * <p>
    * Activates an existing registration. This method should not be
    * used for a name domain.
    * 
    * @param regId   The identifier of the registration
    * @param type    The type of the registration
    * @param alreadyActivated Set to true if the registration has already been activated.
    * @return        True if successful (= the registration exists, was not
    * activated and the registration is not for a name domain)
    */ 
    bool activateRegistration(const RegistrationId& regId, RegistrationType type,
        bool& alreadyActivated)
        throw (OutOfResourcesException);

    /**
    * Purpose:
    * <p>
    * Activates an existing registration for a name domain.
    * 
    * @param regId   The identifier of the registration
    * @param nameDomainInfo The name domain information
    * @param alreadyActivated Set to true if the registration has already been activated.
    * @return        True if successful (= the registration exists and was not
    * activated)
    */ 
    bool activateRegistration(const RegistrationId& regId, const NameDomainInfo& nameDomainInfo,
        bool& alreadyActivated)
        throw (OutOfResourcesException);

    /**
    * Purpose:
    * <p>
    * Removes an existing registration.
    * 
    * @param name      The name reserved by the registration to remove
    * @param activated Set to the status of the registration
    * @param type      Set to the type of the registration
    * @return     True if the removal has been performed.
    *             False if the name doesn't denote an existing registration.
    */ 
    bool removeRegistration(const std::string& name, bool& activated, RegistrationType& type)
        throw (OutOfResourcesException);


    /**
    * Purpose:
    * <p>
    * Returns the list of stringified reference of system name domains and
    * user-supplied name domains' factory finder that have been registered.
    */ 
    void list(std::list<std::string>& systemNameDomains, std::list<std::string>& userFactoryFinders)
        throw (OutOfResourcesException);


public:

    /**
    * Purpose:
    * <p>
    * Creates a new PersistentNameDomain instance with the supplied identifier.
    * 
    * @param id The full identifier of the instance to create
    * @return   The created instance
    * @exception AlreadyExistsException If id already exists
    */ 
    static PersistentNameDomain* createWithId(const std::string& id)
            throw (AlreadyExistsException, OutOfResourcesException);
	    
    /**
    * Purpose:
    * <p>
    * Destroy the PersistentNameDomain instance from the persistent storage.
    * 
    * @return true if the destruction has been correctly performed
    */ 
    static bool destroy(PersistentNameDomain* nameDomain);

    /**
    * Purpose:
    * <p>
    * Indicates whether a PersistentNameDomain having the specified id
    * exists.
    *
    */ 
    static bool exists(const std::string& id)
        throw (OutOfResourcesException);

    /**
    * Purpose:
    * <p>
    * Returns the persistent naming context instance corresponding
    * to the specified id.
    *
    */ 
    static PersistentNameDomain* findById(const std::string& id)
        throw (OutOfResourcesException);


protected:

    /**
    * Purpose:
    * <p>
    * Constructs a persistent name domain with the supplied id.
    */ 
    PersistentNameDomain(const std::string& id);


private:

/**
*Purpose:
*<p>
*The type used to manage the registrations of a given PersistentNameDomain.
*<p>
*/
typedef std::map<RegistrationId, Registration> Registrations;


/**
*Purpose:
*<p>
*The type used to manage the existing name domain instances.
*/
typedef std::map<std::string, PersistentNameDomain*> NameDomains;


private:

    /**
    * The identifier of this name domain.
    */
    std::string m_id;

    /**
    * The reader/writer lock used to manage concurrent thread safe access to
    * m_registrations and m_current_gen_id.
    */
   Cdmw::OsSupport::ReaderWriterLock m_rwLock;
    
    /**
    * The counter used for generated registration identifier.
    */
//     size_t m_current_gen_id;


private:

    /**
    * The mutex used to manage concurrent thread safe access to
    * M_existingNameDomains.
    */
//     static Cdmw::OsSupport::Mutex M_existingNameDomains_mutex;

    /**
    * The existing name domains
    */
//     static NameDomains M_existingNameDomains;

private:
    
    /**
    * Purpose:
    * <p>
    * Generates a registration identifier.
    * 
    * This method is NOT THREAD SAFE and must be used with
    * a lock on m_rwLock.
    *
    * @return The generated identifier.
    */ 
    RegistrationId generateRegistrationId(const std::string& name)
        throw (OutOfResourcesException);

 
    /**
    * Purpose:
    * <p>
    * Returns the iterator on the registration denoted by the name.
    * 
    * This method is NOT THREAD SAFE and must be used with
    * a lock on m_rwLock.
    *
    * @param name The name to look for
    * @return The iterator that is equal to m_registrations.end() if not found
    */ 
    Registrations::iterator findRegistration(const std::string& name)
        throw (OutOfResourcesException);

   
private:

    /**
    * Purpose:
    * <p>
    * Indicates whether a PersistentNameDomain having the specified id
    * exists.
    *
    * Unlike exists, this method is NOT THREAD SAFE and must be used with
    * a lock on M_existingNamingContexts_mutex.
    */ 
    static bool existsId(const std::string& id)
        throw (OutOfResourcesException);

    /**
    * Purpose:
    * <p>
    * The mutex used to manage concurrent thread safe access to
    * M_current_gen_id.
    *
    */ 
    static Cdmw::OsSupport::Mutex M_current_gen_id_mutex;
    
    /**
    * Purpose:
    * <p>
    * Used to create unique identifier for registration-id, counter 
    * being incremented, needs to be protected in some form;
    *
    */ 
    static unsigned long long M_current_gen_id; // OPTME thread safe datatype
};



} // End of namespace NamingAndRepository
} // End of namespace Cdmw

#endif

