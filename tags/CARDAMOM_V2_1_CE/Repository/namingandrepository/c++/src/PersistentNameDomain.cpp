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

#include <Foundation/common/Assert.hpp> 
#include <Foundation/ossupport/OS.hpp>
#include "namingandrepository/PersistentNameDomain.hpp"
#include "namingandrepository/FTDatastoreHelper.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/osthreads/ReaderLockGuard.hpp"
#include "Foundation/osthreads/WriterLockGuard.hpp"
#include <sstream>

#include <memory>

namespace Cdmw
{
namespace NamingAndRepository
{


/**
*Implementation notes :
*Actual persistency mechanism will result in code changes.
*<p>
*Portability issues :   none
*/


// NameDomainInfo implementation

NameDomainInfo::NameDomainInfo()
{

}


NameDomainInfo::NameDomainInfo(const std::string& nameDomainRef)
: m_nameDomainRefString(nameDomainRef), m_userSupplied(false)
{

}


NameDomainInfo::NameDomainInfo(const std::string& nameDomainRef,
       const std::string& userFactoryFinderRef)
: m_nameDomainRefString(nameDomainRef), m_userSupplied(true),
m_userFactoryFinderRefString(userFactoryFinderRef)
{

}


NameDomainInfo::NameDomainInfo(const NameDomainInfo& rhs)
{

    m_nameDomainRefString = rhs.m_nameDomainRefString;
    m_userSupplied = rhs.m_userSupplied;
    m_userFactoryFinderRefString = rhs.m_userFactoryFinderRefString;

}


NameDomainInfo::~NameDomainInfo()
{

}



NameDomainInfo& NameDomainInfo::operator=(const NameDomainInfo& rhs)
{

    NameDomainInfo temp(rhs);
    swap(temp);

    return *this;

}


void NameDomainInfo::swap(NameDomainInfo& rhs)
{

    m_nameDomainRefString.swap(rhs.m_nameDomainRefString);
    m_userFactoryFinderRefString.swap(rhs.m_userFactoryFinderRefString);
    std::swap<bool>(m_userSupplied, rhs.m_userSupplied);

}



// Registration implementation

Registration::Registration()
:  m_activated(false), m_nameDomainInfo(NULL)
{
    
}


Registration::Registration(const std::string& name)
    : m_name(name), m_activated(false), m_nameDomainInfo(NULL) // FIXME what about init of "m_type" 

{

}


Registration::Registration(const std::string& name, RegistrationType type)
: m_name(name), m_activated(true), m_type(type), m_nameDomainInfo(NULL)

{

}


Registration::Registration(const std::string& name, const NameDomainInfo& nameDomainInfo)
: m_name(name), m_activated(true), m_type(NameDomainRegistration)

{
    m_nameDomainInfo = new NameDomainInfo(nameDomainInfo);
}




Registration::Registration(const Registration& rhs)
{

    m_name = rhs.m_name;
    m_activated = rhs.m_activated;
    m_type = rhs.m_type;
    
    if (rhs.m_nameDomainInfo != NULL)
    {
        m_nameDomainInfo = new  NameDomainInfo(*(rhs.m_nameDomainInfo));
    }
    else
    {
        m_nameDomainInfo = NULL;
    }
}


Registration::~Registration()
{

    if (m_nameDomainInfo != NULL)
    {
        delete m_nameDomainInfo;
        m_nameDomainInfo = NULL;
    }

}

Registration& Registration::operator=(const Registration& rhs)
{

    Registration temp(rhs);
    swap(temp);

    return *this;

}


void Registration::swap(Registration& rhs)
{

    m_name.swap(rhs.m_name);

    std::swap<NameDomainInfo*>(m_nameDomainInfo, rhs.m_nameDomainInfo);
    std::swap<bool>(m_activated, rhs.m_activated);
    std::swap<RegistrationType>(m_type, rhs.m_type);

}




// PersistentNameDomain implementation

/** 
 * Class Attributes
 **/
Cdmw::OsSupport::Mutex PersistentNameDomain::M_current_gen_id_mutex;

unsigned long long PersistentNameDomain::M_current_gen_id = 0;

/** 
 * Methods
 **/
    
PersistentNameDomain* PersistentNameDomain::createWithId(const std::string& id)
            throw (AlreadyExistsException, OutOfResourcesException)
{

    if (existsId(id))
    {
        CDMW_THROW(AlreadyExistsException);
    }


    try
    {
        PersistentNameDomain* result = NULL;

        std::auto_ptr<PersistentNameDomain> pND(new PersistentNameDomain(id));

        FTDataStore *storage = FTDataStoreHelper::getStorage(NAME_DOMAIN_DATASTORE_ID);
        
        CdmwNamingAndRepository::DomainInfo info;
        info.the_name   = id.c_str(); // OPTME useless strdup, block lifetime
        CdmwNamingAndRepository::StorageData_var data (new CdmwNamingAndRepository::StorageData());
        data->domain_info (info);

	try {
	    storage->insert (id, data); // FIXME what about multiple occurence of id
	} 
	catch (const Cdmw::CommonSvcs::DataStore::AlreadyExistException &ex) {
	    // never here
	    CDMW_THROW(AlreadyExistsException);
	}
	catch (const Cdmw::CommonSvcs::DataStore::StorageErrorException &ex) {
	    // never here
	    CDMW_THROW(OutOfResourcesException);
	}

        // transfers ownership
        result = pND.release();

        return result;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
    
}



bool PersistentNameDomain::destroy(PersistentNameDomain* nameDomain)
{
    bool result = false;
    
    if (nameDomain != NULL)
    {
	try {
	    FTDataStore *storage =  FTDataStoreHelper::getStorage(NAME_DOMAIN_DATASTORE_ID);
	    
	    try {
		// FIXME,  make sure no siblings exist 
		storage->remove(nameDomain->m_id);
	    } 
	    catch (const Cdmw::CommonSvcs::DataStore::NotFoundException & ex) {
		CDMW_NEVER_HERE(); // FIXME - remove
		// ignore error
		result = false;
	    }
	    catch (const Cdmw::CommonSvcs::DataStore::StorageErrorException & ex) {
		CDMW_NEVER_HERE(); // FIXME - remove
		// ignore error
		result = false;
	    }

	    CDMW_ASSERT (! existsId(nameDomain->m_id));

	} catch (const Cdmw::CommonSvcs::DataStore::NotFoundException & ex) {
	    std::cerr << "Exception raised " /* << ex */ << std::endl; // FIXME error handling
	} 

        result = true;
    }

    return result;

}


bool PersistentNameDomain::exists(const std::string& id)
    throw (OutOfResourcesException)
{
    return existsId(id);
}


PersistentNameDomain* PersistentNameDomain::findById(const std::string& id)
    throw (OutOfResourcesException)
{
    try
    {
        PersistentNameDomain* result = NULL;

	FTDataStore * storage = FTDataStoreHelper::getStorage(NAME_DOMAIN_DATASTORE_ID);
	if (storage->contains (id)) {

#if !defined(NDEBUG)	
 	    CdmwNamingAndRepository::StorageData_var data =
 		FTDataStoreHelper::getStorage(NAME_DOMAIN_DATASTORE_ID)->select (id);
	    	    
	    // paranoid - verify data is of type NameDomain
	    CDMW_ASSERT (data->_d() == CdmwNamingAndRepository::NAR_DOMAIN);
#endif

	    result = new PersistentNameDomain(id);  // FIXME lifetime, otherwise memleak
	} 

        return result;
    }
    catch(const Cdmw::CommonSvcs::DataStore::NotFoundException & ex) {
	// ALERT - concurrently removed by other process
	return NULL;
    }
    catch(const std::bad_alloc &)
	{
        CDMW_THROW(OutOfResourcesException);
    }
}



bool PersistentNameDomain::existsId(const std::string& id)
    throw (OutOfResourcesException)
{
    try
    {
	bool result = 
	(FTDataStoreHelper::getStorage(NAME_DOMAIN_DATASTORE_ID)->contains(id));

        return result;
    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
}




PersistentNameDomain::PersistentNameDomain()
{
}


PersistentNameDomain::PersistentNameDomain(const std::string& id)
: m_id(id)
{
}


PersistentNameDomain::PersistentNameDomain(const PersistentNameDomain& rhs)
{

    m_id = rhs.m_id;
}


PersistentNameDomain::~PersistentNameDomain()
{
}




PersistentNameDomain& PersistentNameDomain::operator=(const PersistentNameDomain& rhs)
{

    PersistentNameDomain temp(rhs);
    swap(temp);

    return *this;

}


void PersistentNameDomain::swap(PersistentNameDomain& rhs)
{

    m_id.swap(rhs.m_id);
}


static const std::string REGID_SEPERATOR = "::"; 

PersistentNameDomain::RegistrationId PersistentNameDomain::generateRegistrationId(const std::string& id)
    throw (OutOfResourcesException)
{

    try {
	RegistrationId gen_id;
	unsigned long long current_gen_id = 0; // holding copy

	// FIXME - assertion fails for demo_ft, incoking start_demo.sh 
	//	CDMW_ASSERT (NULL==strchr(name.c_str(), '/'));
	
        // FIXME : Overflow not currently managed
	std::ostringstream temp;
	temp << id;
	temp << REGID_SEPERATOR;
	{
	    // OPTME atomic increment
	    CDMW_MUTEX_GUARD(M_current_gen_id_mutex);
	    current_gen_id = ++M_current_gen_id; 
	}
	temp << current_gen_id;
        gen_id =  temp.str(); // FIXME - assigning to any-type
	
        return gen_id;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}

static std::string
extractFromRegistrationId(const PersistentNameDomain::RegistrationId &regId) {

    const std::string regIdStr = regId; // FIXME assigning any

    CDMW_ASSERT(regIdStr.rfind(REGID_SEPERATOR)<regIdStr.size());
    
    std::string result = regIdStr.substr(0, regIdStr.rfind(REGID_SEPERATOR));

    CDMW_ASSERT(result.size()>0);
    
    return result;
}


bool PersistentNameDomain::existsNameDomain(const std::string& name, bool& isSystem)
    throw (OutOfResourcesException)
{
    CDMW_READER_LOCK_GUARD(m_rwLock);
 
    //FIXME - handle compound names
    try
    {

        bool result = false;

	const std::string key=createStorageKey(m_id, name);
	FTDataStore *storage=FTDataStoreHelper::getStorage(NAME_DOMAIN_DATASTORE_ID);

	if (storage->contains(key)) {
	    try {
		const CdmwNamingAndRepository::StorageData_var data=
		    storage->select(key);
		
		// paranoid 
		if (CdmwNamingAndRepository::NAR_DOMAIN_REGISTRATION == data->_d()) {
		    result=true;
		    isSystem=!(data->domain_registration_info().is_user_supplied);
		}
	    }
	    catch(const Cdmw::CommonSvcs::DataStore::NotFoundException & ex) {
		// ALERT, concurrently removed from storage
		result = false;
	    }
	}

        return result;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}



std::string PersistentNameDomain::findNameDomain(const std::string& name)
    throw (OutOfResourcesException)
{
 
    CDMW_READER_LOCK_GUARD(m_rwLock);

    try
    {
        std::string result="";

        const std::string key = createStorageKey(m_id, name);
        
        FTDataStore *storage=FTDataStoreHelper::getStorage(NAME_DOMAIN_DATASTORE_ID);
        try { 
            if (storage->contains(key)) {
                const CdmwNamingAndRepository::StorageData_var data=
                storage->select(key);
                
                if (data->_d()==CdmwNamingAndRepository::NAR_DOMAIN_REGISTRATION) {
                    result=data->domain_registration_info().the_domain;
                } else {
                    // paranoid
                    std::cerr << "findNameDomain error: " << name << std::endl;
                    CDMW_NEVER_HERE();
                } 
            }
        } 
        catch (const Cdmw::CommonSvcs::DataStore::NotFoundException & ex) {
            // ALERT - concurrently removed 
            result="";
        }
	
        return result;
        
    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}


std::string PersistentNameDomain::findFactoryFinder(const std::string& name)
    throw (OutOfResourcesException)
{

    CDMW_READER_LOCK_GUARD(m_rwLock);
 
    try
    {

        std::string result;

	const std::string key = createStorageKey(m_id, name);

	FTDataStore *storage=FTDataStoreHelper::getStorage(NAME_DOMAIN_DATASTORE_ID);
	try { 
	    if (storage->contains(key)) {
		const CdmwNamingAndRepository::StorageData_var data=
		    storage->select(key);
		if (data->_d()==CdmwNamingAndRepository::NAR_DOMAIN_REGISTRATION) {
		    result = data->domain_registration_info().the_factory_finder;
		} else {
		    // paranoid
		    CDMW_NEVER_HERE();
		} 
	    }
	} 
	catch (const Cdmw::CommonSvcs::DataStore::NotFoundException & ex) {
	    // ALERT, concurrently removed, two primary Repositories in environment
	    result = "";
	}
	
        return result;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}

static TimeBase::TimeT 
toTimeT (const OsSupport::OS::Timeval tv) {
    // Time in TimeT is expressed in units of 100 nano seconds
    // typedef unsigned long long TimeT;
    const TimeBase::TimeT result = 
       (tv.seconds * (1000*1000*10)) + (tv.microseconds * (10));

   return result;
}

static bool
passiveRegistrationExpired(const CdmwNamingAndRepository::StorageData_var data,
			   const TimeBase::TimeT &current)
{
     switch (data->_d()) {
     case CdmwNamingAndRepository::NAR_REGISTRATION:
     {
	  // Time in TimeT is expressed in units of 100 nano seconds
          // typedef unsigned long long TimeT;
	  const TimeBase::TimeT expiration = 
		  data->registration_info().the_expiration_time;
	  return (expiration != 0 && 
		  current > expiration);
	  break;
     }
     default:
     {
	     // it is active registration
	     return false;
	     break;
     }
     }
}

bool PersistentNameDomain::addRegistration(const std::string& name, RegistrationId& regId)
    throw (OutOfResourcesException)
{
 
    CDMW_WRITER_LOCK_GUARD(m_rwLock);
 
    try
    {
        bool result = false;
        
	FTDataStore *storage = FTDataStoreHelper::getStorage(NAME_DOMAIN_DATASTORE_ID);
	const std::string key = createStorageKey(m_id, name);
	const RegistrationId regIdTmp = generateRegistrationId(key); 
	const std::string regIdTmpStr = regIdTmp; // FIXME any assignment
	TimeBase::TimeT current = 0L;
	TimeBase::TimeT expiration = 0L;

        OsSupport::OS::Timeval tv = OsSupport::OS::get_time();
	current = toTimeT(tv);
        expiration = current + REGISTRATION_TIMEOUT;

	// expiration==0 or expiration>0

	CDMW_ASSERT(extractFromRegistrationId(regIdTmp) == key);


	if (! storage->contains (key)) {
	    CdmwNamingAndRepository::RegistrationInfo info;
	    info.the_name = name.c_str(); 
	    info.the_registration_id = regIdTmpStr.c_str();
 	    info.the_expiration_time = expiration;

	    CdmwNamingAndRepository::StorageData_var data (new CdmwNamingAndRepository::StorageData());
	    data->registration_info (info);

	    storage->insert (key, data.in());
	    CDMW_ASSERT (storage->contains(key));

	    regId  = regIdTmp; // finally define out-value
	    result = true;
	} 
	else {
	    // In case of failover client might fail to activate
	    // registration; Repository should not block the id for
	    // restarted app.
	    const CdmwNamingAndRepository::StorageData_var currentData=
		 storage->select(key); // raises Cdmw::CommonSvcs::DataStore::NotFoundException

	    if (passiveRegistrationExpired(currentData, current)) {
		CdmwNamingAndRepository::StorageData_var data =
		    new CdmwNamingAndRepository::StorageData();
		    
		CdmwNamingAndRepository::RegistrationInfo info;
		info.the_name = name.c_str(); 
		info.the_registration_id = regIdTmpStr.c_str();
		info.the_expiration_time = expiration;
		
		data->registration_info (info);
		
		storage->update (key, data.in());
		CDMW_ASSERT (storage->contains(key));
		
		regId  = regIdTmp; // finally define out-value
		result = true;
	    }    
	}

        return result;

    }
    catch (const Cdmw::CommonSvcs::DataStore::AlreadyExistException &ex) {
	// ALERT, insert/select failed, concurrent access to datastore
	return false;
    }
    catch (const Cdmw::CommonSvcs::DataStore::StorageErrorException &ex) {
	// ALERT, insert/select failed, concurrent access to datastore
	return false;
    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
}


bool PersistentNameDomain::addRegistration(const std::string& name, RegistrationType type)
    throw (OutOfResourcesException)
{

    CDMW_WRITER_LOCK_GUARD(m_rwLock);

    try
    {
    
        bool result = false;

	const std::string key = createStorageKey(m_id, name);
	const RegistrationId regIdTmp = generateRegistrationId(key); 
	const std::string regIdTmpStr = regIdTmp; // FIXME any assignment

 	CDMW_ASSERT(extractFromRegistrationId(regIdTmp) == key);

	FTDataStore *storage = FTDataStoreHelper::getStorage(NAME_DOMAIN_DATASTORE_ID);
	
	if ( ! storage->contains (key)) {
	    
	    CdmwNamingAndRepository::StorageData_var 
		data (new CdmwNamingAndRepository::StorageData());
	    
	    switch (type) {
	    case ObjectRegistration:
		{ 
		    // activated=true;
		    CdmwNamingAndRepository::ObjectRegistrationInfo info;
		    info.the_name = name.c_str();
		    info.the_registration_id = regIdTmpStr.c_str();
		    data->object_registration_info(info);
		    break;
		}
	    case FactoryRegistration:
		{ 
		    // activated=true;
		    CdmwNamingAndRepository::FactoryRegistrationInfo info;
		    info.the_name = name.c_str();
		    info.the_registration_id = regIdTmpStr.c_str();
		    data->factory_registration_info(info);
		    break;
		}
	    case NameDomainRegistration: 
		/* falling through */ 
	    default:
		CDMW_NEVER_HERE();
		break;
	    }
	    
	    // use RegistrationId as key for hashtable
	    try {
		storage->insert (key, data); /* true */
		result = true;
		CDMW_ASSERT (storage->contains(key));
	    } 
	    catch (const Cdmw::CommonSvcs::DataStore::AlreadyExistException &) {
		// ALERT, insert failed, concurrent access to datastore
		result=false;
	    }
	    catch (const Cdmw::CommonSvcs::DataStore::StorageErrorException &) {
		// ALERT, insert failed, concurrent access to datastore
		result=false;
	    }
	}
	
	return  result;
    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}


bool PersistentNameDomain::addRegistration(const std::string& name,
    const NameDomainInfo& nameDomainInfo)
    throw (OutOfResourcesException)
{
 
    CDMW_WRITER_LOCK_GUARD(m_rwLock);
 
    try
    {
        bool result = false;

	FTDataStore *storage = FTDataStoreHelper::getStorage(NAME_DOMAIN_DATASTORE_ID);

	const std::string key = createStorageKey(m_id, name);
	const RegistrationId regIdTmp = generateRegistrationId(key); 
	const std::string regIdTmpStr = regIdTmp; // FIXME any assignment

	CDMW_ASSERT(extractFromRegistrationId(regIdTmp) == key);
	
	if ( ! storage->contains (key)) {
	    
	    CdmwNamingAndRepository::DomainRegistrationInfo info;
	    info.the_name    = name.c_str();
	    info.the_registration_id = regIdTmpStr.c_str(); 
	    info.the_domain  = nameDomainInfo.m_nameDomainRefString.c_str();
	    info.the_factory_finder = nameDomainInfo.m_userFactoryFinderRefString.c_str();
	    info.is_user_supplied =  nameDomainInfo.m_userSupplied;

	    CdmwNamingAndRepository::StorageData_var data (new CdmwNamingAndRepository::StorageData());
	    data->domain_registration_info (info);
	    
	    try {
		// OPTME, use RegistrationId as key for hashtable
                storage->insert (key, data); /* should be true  */
		result = true;
		CDMW_ASSERT (storage->contains(key));
	    } 
	    catch (const Cdmw::CommonSvcs::DataStore::AlreadyExistException &) {
		// ALERT, insert failed, concurrent write
		result=false;
	    }
	    catch (const Cdmw::CommonSvcs::DataStore::StorageErrorException &) {
		// ALERT, insert failed, concurrent access to datastore
		result=false;
	    }
	}

        return result;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}



std::string PersistentNameDomain::findName(const RegistrationId& regId)
    throw (OutOfResourcesException)
{
 
    CDMW_READER_LOCK_GUARD(m_rwLock);

    try
    {

        std::string result="";

	const std::string regIdStr = regId; // FIXME export any
	const std::string key = extractFromRegistrationId(regId);
	FTDataStore *storage=FTDataStoreHelper::getStorage(NAME_DOMAIN_DATASTORE_ID);

	try { 
	    if (storage->contains(key)) {
		const CdmwNamingAndRepository::StorageData_var data=
		    storage->select(key); // raises Cdmw::CommonSvcs::DataStore::NotFoundException
		//FIXME -  is it really NAR_DOMAIN
		switch (data->_d()) {
		case CdmwNamingAndRepository::NAR_REGISTRATION:
		    {
			result = data->registration_info().the_name;
			break;
		    }
		case CdmwNamingAndRepository::NAR_DOMAIN_REGISTRATION:
		    {
			result = data->domain_registration_info().the_name;
			break;
		    }
		case CdmwNamingAndRepository::NAR_FACTORY_REGISTRATION:
		    {
			result = data->factory_registration_info().the_name;
			break;
		    }	
		case CdmwNamingAndRepository::NAR_OBJECT_REGISTRATION:
		    {
			result = data->object_registration_info().the_name;
			break;
		    }	
		default:
		    CDMW_NEVER_HERE();
		    result = "";
		} 
	    }
	} 
	catch (const Cdmw::CommonSvcs::DataStore::NotFoundException & ex) {
	    // ALERT, select failed, object removed concurrently
	    result = "";
	}

        return result;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}



bool PersistentNameDomain::activateRegistration(const RegistrationId& regId, RegistrationType type,
    bool& alreadyActivated)
    throw (OutOfResourcesException)
{
 
    CDMW_WRITER_LOCK_GUARD(m_rwLock);

    try
    {

        bool result = false;

	if (type != NameDomainRegistration)
        {
	    std::string key = extractFromRegistrationId(regId); // export to string
		
	    FTDataStore *storage = FTDataStoreHelper::getStorage(NAME_DOMAIN_DATASTORE_ID);
	    try { 
		if (storage->contains(key)) {
		    CdmwNamingAndRepository::StorageData_var data=
			storage->select(key);
		    
		    switch (data->_d()) {
		    case CdmwNamingAndRepository::NAR_REGISTRATION:
			{
			    const std::string nameCopy =  
				data->registration_info().the_name.in();
			    const RegistrationId regIdCopy= 
				data->registration_info().the_registration_id.in(); 
			    if (regId == regIdCopy) {
				switch (type) {
				case ObjectRegistration:
				    { 
					// activated=true;
					CdmwNamingAndRepository::ObjectRegistrationInfo info;
					info.the_name = nameCopy.c_str();
					info.the_registration_id = regIdCopy.c_str();;
					data->object_registration_info(info);
					break;
				    }
				case FactoryRegistration:
				    { 
					// activated=true;
					CdmwNamingAndRepository::FactoryRegistrationInfo info;
					info.the_name = nameCopy.c_str();
					info.the_registration_id = regIdCopy.c_str();
					data->factory_registration_info(info);
					break;
				    }
				case NameDomainRegistration: 
				    /* falling through */ 
				default:
				    CDMW_NEVER_HERE();
				    result = false;
				}
				
				try {
				    storage->update(key, data);
				    alreadyActivated = false;
				    result = true;
				} 
				catch(const NotFoundException & ex) {
				    // update failed, someone removed the entry concurrently
				    result = false;
				}
				catch(const Cdmw::CommonSvcs::DataStore::StorageErrorException & ex) {
				    // update failed, someone removed the entry concurrently
				    result = false;
				}
			    } else {
				 result = false;
			    }
			    break;
			}
			
		    case CdmwNamingAndRepository::NAR_FACTORY_REGISTRATION:
			{
			    CDMW_ASSERT(type==FactoryRegistration);
			    alreadyActivated = true;
			    result = true;
			    break;
			}	
		    case CdmwNamingAndRepository::NAR_OBJECT_REGISTRATION:
		    {
			CDMW_ASSERT(type==ObjectRegistration);
			alreadyActivated = true;
			result = true;
			break;
		    }	
		    case CdmwNamingAndRepository::NAR_DOMAIN_REGISTRATION:
			/* fall through */ 
		    default:
			CDMW_NEVER_HERE();
			result = false;
		    } 
		}
	    } 
	    catch (const Cdmw::CommonSvcs::DataStore::NotFoundException & ex) {
		// select failed, element removed concurrently
		result = false;
	    }
	    
	}

        return result;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}

bool PersistentNameDomain::activateRegistration(const RegistrationId& regId, const NameDomainInfo& nameDomainInfo,
        bool& alreadyActivated)
    throw (OutOfResourcesException)
{
  
    CDMW_WRITER_LOCK_GUARD(m_rwLock);
 
   try
    {

        bool result = false;

	const std::string key = extractFromRegistrationId(regId); 
	FTDataStore *storage = FTDataStoreHelper::getStorage(NAME_DOMAIN_DATASTORE_ID);

	try { 
	    if (storage->contains(key)) {
		CdmwNamingAndRepository::StorageData_var data=
		    storage->select(key);
		
		switch (data->_d()) {
		case CdmwNamingAndRepository::NAR_REGISTRATION:
		    {
			const std::string nameCopy = 
			    data->registration_info().the_name.in();
			const RegistrationId regIdCopy = 
			    data->registration_info().the_registration_id.in();
				
			if (regId == regIdCopy) {
				CdmwNamingAndRepository::DomainRegistrationInfo info;
				info.the_name = nameCopy.c_str();
				info.the_registration_id = regIdCopy.c_str();
				info.the_domain  = nameDomainInfo.m_nameDomainRefString.c_str();
				info.the_factory_finder = nameDomainInfo.m_userFactoryFinderRefString.c_str();
				info.is_user_supplied = nameDomainInfo.m_userSupplied;
				
				data->domain_registration_info(info);
				
				try {
                                    storage->update(key, data);
                                    result = true;
                                    alreadyActivated = false;
				} 
				catch(const Cdmw::CommonSvcs::DataStore::NotFoundException & ex) {
                                    // update failed,  concurrent access
                                    result = false;
				}
				catch(const Cdmw::CommonSvcs::DataStore::StorageErrorException & ex) {
                                    // update failed,  concurrent access
                                    result = false;
				}
			}
			break;
		    }		    
		case CdmwNamingAndRepository::NAR_FACTORY_REGISTRATION:
		    {
			CDMW_NEVER_HERE();
			break;
		    }	
		case CdmwNamingAndRepository::NAR_OBJECT_REGISTRATION:
		    {
			CDMW_NEVER_HERE();
			break;
		    }	
		case CdmwNamingAndRepository::NAR_DOMAIN_REGISTRATION:
		    {
			const RegistrationId regIdCopy = 
			   data->domain_registration_info().the_registration_id.in();

			// allready activated
			result = (regId == regIdCopy);
			alreadyActivated = true;
		    }
		default:
		    CDMW_NEVER_HERE();
		    break;
		} 
	    }
	}
	catch (const Cdmw::CommonSvcs::DataStore::NotFoundException & ex) {
	    // select failed, element removed concurrently
	    result = false;
	}
	
        return result;
	
    }
    catch(const std::bad_alloc &) {
	CDMW_THROW(OutOfResourcesException);
    }

}


bool PersistentNameDomain::removeRegistration(const std::string& name, bool& activated, RegistrationType& type)
    throw (OutOfResourcesException)
{
 
    CDMW_WRITER_LOCK_GUARD(m_rwLock);
 
    try
    {

        bool result = false;
	
	const std::string key = createStorageKey(m_id, name);
	FTDataStore *storage = FTDataStoreHelper::getStorage(NAME_DOMAIN_DATASTORE_ID);
	try { 
	    // FIXME make sure siblings have been removed
	    if (storage->contains(key)) {
		storage->remove(key); // expected to be true
		result = true;
	    }
	}
	catch (const Cdmw::CommonSvcs::DataStore::NotFoundException & ex) {
	    // ALERT, remove failed, element removed concurrently
	    result = false;
	}
	catch (const Cdmw::CommonSvcs::DataStore::StorageErrorException & ex) {
	    // ALERT, remove failed, element removed concurrently
	    result = false;
	}
        return result;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}


void PersistentNameDomain::list(std::list<std::string>& systemNameDomains, std::list<std::string>& userFactoryFinders)
    throw (OutOfResourcesException)
{

    CDMW_READER_LOCK_GUARD(m_rwLock);
                                                                               
    try
    {
	StorageKeyDataPairList siblings;
	std::list<std::string> systemNameDomainsTmp; // exception safe: tmp objects
	std::list<std::string> userFactoryFindersTmp;// exception safe: tmp objects

	FTDataStore *storage=FTDataStoreHelper::getStorage(NAME_DOMAIN_DATASTORE_ID);
	const std::string prefix=createStorageKey(m_id, "");
	storage->for_each(GatherSiblings(prefix, siblings));
	
	for (StorageKeyDataPairList::iterator it = siblings.begin(); 
	     it != siblings.end(); 
	     ++it)
        {
	    const std::string key =  it->first;
	    CdmwNamingAndRepository::StorageData &data =
		it->second;

	    switch (data._d()) {
	    case CdmwNamingAndRepository::NAR_DOMAIN_REGISTRATION:
		// is always activated 
		{
		    const std::string name =
			data.domain_registration_info().the_name.in();
		    const std::string domainRef=
			data.domain_registration_info().the_domain.in();
		    const std::string factoryRef=
			data.domain_registration_info().the_factory_finder.in();
		    
		    if (data.domain_registration_info().is_user_supplied) {
			userFactoryFindersTmp.push_back(factoryRef);
		    } 
		    else {
			systemNameDomainsTmp.push_back(domainRef);
		    } 
		    break;
		}
	    default:
		// ignore element
		break;
	    }
        }

	userFactoryFinders = userFactoryFindersTmp; // exception safe: copy
	systemNameDomains  = systemNameDomainsTmp;  // exception safe: copy	
    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}

} // End of namespace NamingAndRepository
} // End of namespace Cdmw

