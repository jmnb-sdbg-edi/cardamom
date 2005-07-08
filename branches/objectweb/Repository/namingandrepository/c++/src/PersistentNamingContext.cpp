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


#include "namingandrepository/PersistentNamingContext.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/osthreads/ReaderLockGuard.hpp"
#include "Foundation/osthreads/WriterLockGuard.hpp"

#include <memory>
#include <sstream>
#include <iostream>

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


// Binding implementation

Binding::Binding()
{

}


Binding::Binding(const std::string& object, BindingType type)
: m_objectRefString(object), m_type(type)

{

}


Binding::Binding(const Binding& rhs)
{

    m_objectRefString = rhs.m_objectRefString;
    m_type = rhs.m_type;

}


Binding::~Binding()
{

}



Binding& Binding::operator=(const Binding& rhs)
{

    m_objectRefString = rhs.m_objectRefString;
    m_type = rhs.m_type;

    return *this;

}



// BindingInfo implementation

BindingInfo::BindingInfo()
{

}


BindingInfo::BindingInfo(const std::string& name, const Binding& binding)
: m_bindingName(name), m_bindingType(binding.m_type)

{

}


BindingInfo::BindingInfo(const BindingInfo& rhs)
{

    m_bindingName = rhs.m_bindingName;
    m_bindingType = rhs.m_bindingType;

}


BindingInfo::~BindingInfo()
{

}



BindingInfo& BindingInfo::operator=(const BindingInfo& rhs)
{

    m_bindingName = rhs.m_bindingName;
    m_bindingType = rhs.m_bindingType;

    return *this;

}



// DetailedBindingInfo implementation

DetailedBindingInfo::DetailedBindingInfo()
{

}


DetailedBindingInfo::DetailedBindingInfo(const std::string& name, const Binding& binding)
: BindingInfo(name, binding), m_objectRefString(binding.m_objectRefString)
{

}


DetailedBindingInfo::DetailedBindingInfo(const DetailedBindingInfo& rhs)
{

    m_bindingName = rhs.m_bindingName;
    m_bindingType = rhs.m_bindingType;
    m_objectRefString = rhs.m_objectRefString;

}


DetailedBindingInfo::~DetailedBindingInfo()
{

}



DetailedBindingInfo& DetailedBindingInfo::operator=(const DetailedBindingInfo& rhs)
{

    DetailedBindingInfo temp(rhs);
    swap(temp);

    return *this;
}


void DetailedBindingInfo::swap(DetailedBindingInfo& rhs)
{

    std::swap<BindingType>(m_bindingType, rhs.m_bindingType);
    m_bindingName.swap(rhs.m_bindingName);
    m_objectRefString.swap(rhs.m_objectRefString);


}



// PersistentNamingContext implementation

Cdmw::OsSupport::Mutex PersistentNamingContext::M_existingNamingContexts_mutex;

unsigned long PersistentNamingContext::M_current_gen_id = 0;

PersistentNamingContext::NamingContexts PersistentNamingContext::M_existingNamingContexts;



PersistentNamingContext* PersistentNamingContext::create(const std::string& id)
    throw (OutOfResourcesException)
{

    try
    {
        PersistentNamingContext* result = NULL;

        std::auto_ptr<PersistentNamingContext> pNC(new PersistentNamingContext(id));
    
        // memorize in the existing naming contexts set
        M_existingNamingContexts[id] = pNC.get();
        
        // transfers ownership
        result = pNC.release();

        return result;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}



PersistentNamingContext* PersistentNamingContext::createWithPrefix(const std::string& prefix)
    throw (OutOfResourcesException)
{

    CDMW_MUTEX_GUARD(M_existingNamingContexts_mutex);

    try
    {

        std::string gen_id;

        do
        {
            // FIXME : Overflow not currently managed
            ++M_current_gen_id;

            std::ostringstream temp;
            temp << M_current_gen_id; 

            gen_id = prefix + temp.str();

	   
        }
        while (existsId(gen_id));

        PersistentNamingContext *result = create(gen_id);  

        return result;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }


}
    
PersistentNamingContext* PersistentNamingContext::createWithId(const std::string& id)
            throw (AlreadyExistsException, OutOfResourcesException)
{

    CDMW_MUTEX_GUARD(M_existingNamingContexts_mutex);

    if (existsId(id))
    {
    	CDMW_THROW(AlreadyExistsException);
    }

    PersistentNamingContext* result = create(id);

    return result;

}



bool PersistentNamingContext::destroy(PersistentNamingContext* namingContext)
{

    bool result = false;
    
    if (namingContext != NULL)
    {

        CDMW_MUTEX_GUARD(M_existingNamingContexts_mutex);
        M_existingNamingContexts.erase(namingContext->m_id);
	
        delete namingContext;
        result = true;
    }
	
	return result;

}



bool PersistentNamingContext::exists(const std::string& id)
        throw (OutOfResourcesException)
{

    CDMW_MUTEX_GUARD(M_existingNamingContexts_mutex);

    return existsId(id);

}


PersistentNamingContext* PersistentNamingContext::findById(const std::string& id)
        throw (OutOfResourcesException)
{

    CDMW_MUTEX_GUARD(M_existingNamingContexts_mutex);

    try
    {

        PersistentNamingContext* result = NULL;

        NamingContexts::iterator it = M_existingNamingContexts.find(id);

        if (it != M_existingNamingContexts.end())
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



bool PersistentNamingContext::existsId(const std::string& id)
        throw (OutOfResourcesException)
{
    // NOT THREAD SAFE

    try
    {

        bool result = false;

        NamingContexts::iterator it = M_existingNamingContexts.find(id);

        if (it != M_existingNamingContexts.end())
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



PersistentNamingContext::PersistentNamingContext()
{

}


PersistentNamingContext::PersistentNamingContext(const std::string& id)
: m_id(id)
{

}



PersistentNamingContext::PersistentNamingContext(const PersistentNamingContext& rhs)
{

    m_id = rhs.m_id;
    m_bindings = rhs.m_bindings;

}


PersistentNamingContext::~PersistentNamingContext()
{

}



bool PersistentNamingContext::isEmpty(void)
{

    bool result = false;

    CDMW_READER_LOCK_GUARD(m_rwLock);

    if (m_bindings.size() == 0)
    {
        result = true;
    }

    return result;

}




PersistentNamingContext& PersistentNamingContext::operator=(const PersistentNamingContext& rhs)
{

    PersistentNamingContext temp(rhs);
    swap(temp);

    return *this;

}


void PersistentNamingContext::swap(PersistentNamingContext& rhs)
{

    m_id.swap(rhs.m_id);
    m_bindings.swap(rhs.m_bindings);

}



bool PersistentNamingContext::existsBinding(const std::string& name)
        throw (OutOfResourcesException)
{

    CDMW_READER_LOCK_GUARD(m_rwLock);

    try
    {
 
        bool result = false;

        Bindings::iterator it = m_bindings.find(name);

        if (it != m_bindings.end())
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


bool PersistentNamingContext::existsBinding(const std::string& name, BindingType& type)
        throw (OutOfResourcesException)
{

    CDMW_READER_LOCK_GUARD(m_rwLock);

    try
    {
 
        bool result = false;

        Bindings::iterator it = m_bindings.find(name);

        if (it != m_bindings.end())
        {
            result = true;

            type = it->second.m_type;
        }

        return result;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}


std::string PersistentNamingContext::findObject(const std::string& name)
        throw (OutOfResourcesException)
{

    CDMW_READER_LOCK_GUARD(m_rwLock);

    try
    {
        std::string result;
 
        Bindings::iterator it = m_bindings.find(name);

        if (it != m_bindings.end())
        {
            result = it->second.m_objectRefString;
        }

        return result;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}


std::string PersistentNamingContext::findNamingContext(const std::string& name, 
    bool& context)
        throw (OutOfResourcesException)
{

    CDMW_READER_LOCK_GUARD(m_rwLock);

    try
    {
 
        std::string result;

        Bindings::iterator it = m_bindings.find(name);

        if (it != m_bindings.end())
        {
            result = it->second.m_objectRefString;
            
            if (it->second.m_type == NamingContextBinding)
            {
                context = true;
            }
            else
            {
                context = false;
            }
        }

        return result;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}


bool PersistentNamingContext::addBinding(const std::string& name, const Binding& binding)
        throw (OutOfResourcesException)
{

    CDMW_WRITER_LOCK_GUARD(m_rwLock);

    try
    {
 
        bool result = false;

        Bindings::iterator it = m_bindings.find(name);

        if (it == m_bindings.end())
        {
            // add the binding
            m_bindings[name] = binding;
            result = true;
        }

        return result;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}


bool PersistentNamingContext::updateBinding(const std::string& name,
    const std::string& object, BindingType type)
    throw (OutOfResourcesException)
{

    CDMW_WRITER_LOCK_GUARD(m_rwLock);

    try
    {

        bool result = false;
        Bindings::iterator it = m_bindings.find(name);

        if (it != m_bindings.end())
        {
            // a new binding is added
            result = true;
        }
        
        m_bindings[name].m_objectRefString = object;
        m_bindings[name].m_type = type;

        return result;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}


bool PersistentNamingContext::removeBinding(const std::string& name)
{

    bool result = false;

    CDMW_WRITER_LOCK_GUARD(m_rwLock);

    int count = m_bindings.erase(name);

    if (count != 0)
    {
        result = true;
    }

    return result;

}


PersistentNamingContext::BindingInfoList PersistentNamingContext::list(void)
        throw (OutOfResourcesException)
{

    CDMW_READER_LOCK_GUARD(m_rwLock);

    try
    {

        BindingInfoList bil;
        Bindings::iterator it;

        for (it = m_bindings.begin(); it != m_bindings.end(); ++it)
        {

            BindingInfo bi(it->first.c_str(), it->second);
            bil.push_back(bi);

        }
        
        return bil;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}


PersistentNamingContext::DetailedBindingInfoList PersistentNamingContext::detailedList(void)
        throw (OutOfResourcesException)
{

    CDMW_READER_LOCK_GUARD(m_rwLock);

    try
    {

        DetailedBindingInfoList bil;
        Bindings::iterator it;

        for (it = m_bindings.begin(); it != m_bindings.end(); ++it)
        {

            DetailedBindingInfo bi(it->first.c_str(), it->second);
            bil.push_back(bi);

        }
        
        return bil;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}


} // End of namespace NamingAndRepository
} // End of namespace Cdmw

