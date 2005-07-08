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


#include "namingandrepository/PersistentNameDomain.hpp"
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
: m_name(name), m_activated(false), m_nameDomainInfo(NULL)

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

Cdmw::OsSupport::Mutex PersistentNameDomain::M_existingNameDomains_mutex;

PersistentNameDomain::NameDomains PersistentNameDomain::M_existingNameDomains;


    
PersistentNameDomain* PersistentNameDomain::createWithId(const std::string& id)
            throw (AlreadyExistsException, OutOfResourcesException)
{

    CDMW_MUTEX_GUARD(M_existingNameDomains_mutex);

    if (existsId(id))
    {
        CDMW_THROW(AlreadyExistsException);
    }


    try
    {
        PersistentNameDomain* result = NULL;

        std::auto_ptr<PersistentNameDomain> pND(new PersistentNameDomain(id));
    
        // memorize in the existing name domains set
        M_existingNameDomains[id] = pND.get();
        
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

        CDMW_MUTEX_GUARD(M_existingNameDomains_mutex);
        M_existingNameDomains.erase(nameDomain->m_id);

        delete nameDomain;
        result = true;
    }

    return result;

}


bool PersistentNameDomain::exists(const std::string& id)
    throw (OutOfResourcesException)
{

    CDMW_MUTEX_GUARD(M_existingNameDomains_mutex);

    return existsId(id);

}


PersistentNameDomain* PersistentNameDomain::findById(const std::string& id)
    throw (OutOfResourcesException)
{

    CDMW_MUTEX_GUARD(M_existingNameDomains_mutex);

    try
    {

        PersistentNameDomain* result = NULL;

        NameDomains::iterator it = M_existingNameDomains.find(id);

        if (it != M_existingNameDomains.end())
        {
            result = it->second;
        }

        return result;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}



bool PersistentNameDomain::existsId(const std::string& id)
    throw (OutOfResourcesException)
{
    // NOT THREAD SAFE

    try
    {

        bool result = false;

        NameDomains::iterator it = M_existingNameDomains.find(id);

        if (it != M_existingNameDomains.end())
        {
            result = true;
        }

        return result;
    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}




PersistentNameDomain::PersistentNameDomain()
{
    m_current_gen_id = 0;
}


PersistentNameDomain::PersistentNameDomain(const std::string& id)
: m_id(id)
{
    m_current_gen_id = 0;
}


PersistentNameDomain::PersistentNameDomain(const PersistentNameDomain& rhs)
{

    m_id = rhs.m_id;
    m_registrations = rhs.m_registrations;
    m_current_gen_id = rhs.m_current_gen_id;

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
    m_registrations.swap(rhs.m_registrations);
    std::swap<size_t>(m_current_gen_id, rhs.m_current_gen_id);

}


PersistentNameDomain::Registrations::iterator PersistentNameDomain::findRegistration(const std::string& name)
    throw (OutOfResourcesException)
{
    // NOT THREAD SAFE

    try
    {

        Registrations::iterator result = m_registrations.end();
        
        for (Registrations::iterator it = m_registrations.begin(); it != m_registrations.end(); ++it)
        {
            if (name.compare(it->second.m_name) == 0)
                result = it;
        }

        return result;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
}


PersistentNameDomain::RegistrationId PersistentNameDomain::generateRegistrationId()
    throw (OutOfResourcesException)
{
    // NOT THREAD SAFE

    try
    {

        RegistrationId gen_id;

        // FIXME : Overflow not currently managed
        ++m_current_gen_id;

        std::ostringstream temp;
        temp << m_current_gen_id;

        gen_id = temp.str();

        return gen_id;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}



bool PersistentNameDomain::existsNameDomain(const std::string& name, bool& isSystem)
    throw (OutOfResourcesException)
{
    
    CDMW_READER_LOCK_GUARD(m_rwLock);

    try
    {

        bool result = false;

        Registrations::iterator it = findRegistration(name);

        if (it != m_registrations.end() && it->second.m_type == NameDomainRegistration)
        {
            result = true;
            isSystem = !((it->second.m_nameDomainInfo)->m_userSupplied);
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

        std::string result;

        Registrations::iterator it = findRegistration(name);

        if (it != m_registrations.end() && it->second.m_type == NameDomainRegistration)
        {
            result = (it->second.m_nameDomainInfo)->m_nameDomainRefString;
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

        Registrations::iterator it = findRegistration(name);

        if (it != m_registrations.end() && it->second.m_type == NameDomainRegistration)
        {
            result = (it->second.m_nameDomainInfo)->m_userFactoryFinderRefString;
        }

        return result;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}


bool PersistentNameDomain::addRegistration(const std::string& name, RegistrationId& regId)
    throw (OutOfResourcesException)
{

    CDMW_WRITER_LOCK_GUARD(m_rwLock);

    try
    {

        bool result = false;
        
        Registrations::iterator it = findRegistration(name);

        if (it == m_registrations.end())
        {
            regId = generateRegistrationId();
        
            Registration reg(name);
    
            // add the registration
            m_registrations[regId] = reg;
            result = true;
        
        }

        return result;

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

        if (type != NameDomainRegistration)
        {

            Registrations::iterator it = findRegistration(name);

            if (it == m_registrations.end())
            {

                RegistrationId regId = generateRegistrationId();
    
                Registration reg(name, type);
    
                // add the registration
                m_registrations[regId] = reg;
                result = true;
            
            }
        }

        return result;

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
        
        Registrations::iterator it = findRegistration(name);

        if (it == m_registrations.end())
        {

            RegistrationId regId = generateRegistrationId();
    
            Registration reg(name, nameDomainInfo);
    
            // add the registration
            m_registrations[regId] = reg;
            result = true;
        
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

        std::string result;

        Registrations::iterator it = m_registrations.find(regId);

        if (it != m_registrations.end())
        {
            result = it->second.m_name;
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

            Registrations::iterator it = m_registrations.find(regId);

            if (it != m_registrations.end())
            {
                alreadyActivated = it->second.m_activated;
        
                if (!alreadyActivated)
                {
                    it->second.m_activated = true;
                    it->second.m_type = type;
                    result = true;
                }
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

        Registrations::iterator it = m_registrations.find(regId);

        if (it != m_registrations.end())
        {
            alreadyActivated = it->second.m_activated;
        
            if (!alreadyActivated)
            {
                NameDomainInfo* temp_nameDomainInfo = new NameDomainInfo(nameDomainInfo);
                
                it->second.m_activated = true;
                it->second.m_type = NameDomainRegistration;
                it->second.m_nameDomainInfo = temp_nameDomainInfo;

                result = true;
            }
        }

        return result;

    }
    catch(const std::bad_alloc &)
    {
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

        Registrations::iterator it = findRegistration(name);

        if (it != m_registrations.end())
        {
            activated = it->second.m_activated;
            type = it->second.m_type;

            m_registrations.erase(it);
        
            result = true;
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

        for (Registrations::iterator it = m_registrations.begin(); it != m_registrations.end(); ++it)
        {
            if (it->second.m_activated && it->second.m_type == NameDomainRegistration)
            {
            
                NameDomainInfo* nameDomainInfo = it->second.m_nameDomainInfo;
            
                if (nameDomainInfo->m_userSupplied)
                {
                    userFactoryFinders.push_back(nameDomainInfo->m_userFactoryFinderRefString);
                }
                else
                {
                    systemNameDomains.push_back(nameDomainInfo->m_nameDomainRefString);
                }
            
            }

        }

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}



} // End of namespace NamingAndRepository
} // End of namespace Cdmw

