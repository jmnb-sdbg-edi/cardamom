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

#include "namingandrepository/PersistentNamingContext.hpp"
#include "namingandrepository/FTDatastoreHelper.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
// include "Foundation/osthreads/ReaderLockGuard.hpp"
// include "Foundation/osthreads/WriterLockGuard.hpp"

#include <memory>
#include <sstream>
#include <iostream>

namespace Cdmw
{
namespace NamingAndRepository
{

typedef std::pair<std::string, CdmwNamingAndRepository::StorageData_var> value_type;

struct Matcher: public std::unary_function<value_type, bool>
{
    Matcher(std::string prefix) : m_prefix(prefix) {}

    bool operator()(const value_type& val)
    {
        // Test if string starts with supplied prefix
        return val.first.find(m_prefix) == 0;
    }

    std::string m_prefix;
};

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

	FTDataStore *storage = FTDataStoreHelper::getStorage(NAMING_CONTEXT_DATASTORE_ID);
	
	CdmwNamingAndRepository::ContextInfo info;
	info.the_name = id.c_str(); // OPTME useless strdup, block lifetime

	CdmwNamingAndRepository::StorageData_var data (new CdmwNamingAndRepository::StorageData());
	data->context_info (info);

	try {
	    storage->insert (id, data.in());
	    CDMW_ASSERT (storage->contains(id));
	    
	    // transfers ownership
	    result = pNC.release();
	} 
	catch (const Cdmw::CommonSvcs::DataStore::AlreadyExistException &ex) {
	    // ALERT, concurrent creation of entries, FIXME 
	    result = NULL;
	} catch (Cdmw::CommonSvcs::DataStore::StorageErrorException & ex) {
	    // ALERT, concurrently removed, but same result, FIXME,CHECKME
	    result = NULL;
	} 

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
    try
    {

        std::string gen_id;
	PersistentNamingContext *result = NULL;
        do
        {
            // FIXME : Overflow not currently managed
            ++M_current_gen_id;

            std::ostringstream temp;
            temp << M_current_gen_id; 

            gen_id = prefix + temp.str();
        }
        while (existsId(gen_id) ||      // OPTME use uuids
	       NULL == (result = create(gen_id))); // concurrent conflict

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

 
    PersistentNamingContext* result = NULL;

    if (existsId(id) ||
	NULL == (result = create(id))) // concurrent conflict
    {
    	CDMW_THROW(AlreadyExistsException);
    }

    return result;

}

bool PersistentNamingContext::destroy(PersistentNamingContext* namingContext)
{

    bool result = false;
    
    if (namingContext != NULL)
    {
	try {
	    FTDataStore *storage =  FTDataStoreHelper::getStorage(NAMING_CONTEXT_DATASTORE_ID);
	    
	    storage->remove(namingContext->m_id);
	    result = true;
	} catch (const Cdmw::CommonSvcs::DataStore::NotFoundException & ex) {
	    // ALERT, concurrently removed, but same result, FIXME,CHECKME
	    result = false;
	} catch (const Cdmw::CommonSvcs::DataStore::StorageErrorException & ex) {
	    // ALERT, concurrently removed, but same result, FIXME,CHECKME
	    result = false;
	} 

        delete namingContext; // CHECKME

        result = true;
    }
	
    return result;
}



bool PersistentNamingContext::exists(const std::string& id)
        throw (OutOfResourcesException)
{

    return existsId(id);

}


PersistentNamingContext* PersistentNamingContext::findById(const std::string& id)
        throw (OutOfResourcesException)
{

    try
    {
        PersistentNamingContext* result = NULL;

	FTDataStore *storage = 
	    FTDataStoreHelper::getStorage(NAMING_CONTEXT_DATASTORE_ID);

        if (storage->contains(id))
        {
	    // FIXME - insert here (new PersistentNamingContext(id))
	    // think about object creation/destruction
	    
	    // verify data is of type NameDomain
	    CdmwNamingAndRepository::StorageData_var data =
		storage->select (id);

	    // paranoid
	    if (data->_d() == CdmwNamingAndRepository::NAR_CONTEXT); {
		result = new PersistentNamingContext (id); // FIXME check lifetime
	    }
	}

        return result;
    }
    catch (Cdmw::CommonSvcs::DataStore::NotFoundException & ex) {
	// ALERT, select failed, element removed concurrently
	return NULL;
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

	return (FTDataStoreHelper::getStorage(NAMING_CONTEXT_DATASTORE_ID)->contains(id));

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}

Strings PersistentNamingContext::findWithPrefix(const std::string& prefix)
    throw (OutOfResourcesException, InternalErrorException)
{
    Strings result;

    FTDataStoreHelper::getStorage(NAMING_CONTEXT_DATASTORE_ID)->
        find_if(Matcher(prefix), std::back_inserter(result));

    return result;
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
    // same m_id, no need to change storage
    m_id = rhs.m_id;
}


PersistentNamingContext::~PersistentNamingContext()
{

}



bool PersistentNamingContext::isEmpty(void)
{
       
  CDMW_READER_LOCK_GUARD(m_rwLock);

    // OPTME - inefficient to gather all siblings simply to count the amount
   const std::string contextPrefix=createStorageKey(m_id, "");
   
   StorageKeyDataPairList siblings;
	
   FTDataStore *storage = FTDataStoreHelper::getStorage(NAMING_CONTEXT_DATASTORE_ID);
   storage->for_each(GatherSiblings(contextPrefix, siblings));
   
   return (siblings.size() == 0);
}




PersistentNamingContext& PersistentNamingContext::operator=(const PersistentNamingContext& rhs)
{

    PersistentNamingContext temp(rhs);
    swap(temp);

    return *this;

}

// FIXME - where is swap used, what is it used for? 
void PersistentNamingContext::swap(PersistentNamingContext& rhs)
{

    m_id.swap(rhs.m_id);
}



bool PersistentNamingContext::existsBinding(const std::string& name)
        throw (OutOfResourcesException)
{
  
    CDMW_READER_LOCK_GUARD(m_rwLock); 
  
    try
    {
        bool result = false;

	std::string key = createStorageKey (m_id, name);
        result = FTDataStoreHelper::getStorage(NAMING_CONTEXT_DATASTORE_ID)->contains (key);

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
	std::string key = createStorageKey (m_id, name);
 
        bool result = false;

	FTDataStore *storage = FTDataStoreHelper::getStorage(NAMING_CONTEXT_DATASTORE_ID);
	if (storage->contains(key)) {

	    CdmwNamingAndRepository::StorageData_var data = 
		storage->select (key);

	    switch (data->_d()) {
	    case CdmwNamingAndRepository::NAR_OBJECT_BINDING:
		{
		    result = true;
		    type   = ObjectBinding;
		    break;
		}
	    case CdmwNamingAndRepository::NAR_CONTEXT_BINDING:
		{ 
		    result = true;
		    type   = NamingContextBinding;
		    break;
		}
	    default:
		{ 
		    CDMW_NEVER_HERE();
		    break;
		}
	    }
	}

        return result;

    }
    catch (Cdmw::CommonSvcs::DataStore::NotFoundException & ex) {
	// ALERT, select failed, element removed concurrently
	return false;
    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}


std::string PersistentNamingContext::findObject(const std::string& name, bool &isContext)
        throw (OutOfResourcesException)
{
 
    CDMW_READER_LOCK_GUARD(m_rwLock);
 
    try
    {
	const std::string key = createStorageKey (m_id, name);
        std::string result = "";
	
	FTDataStore *storage = FTDataStoreHelper::getStorage(NAMING_CONTEXT_DATASTORE_ID);

        if (storage->contains(key))
        {
	    CdmwNamingAndRepository::StorageData_var data = 
		storage->select (key);

	    switch (data->_d()) {
	    case CdmwNamingAndRepository::NAR_OBJECT_BINDING:
		{
		    result = std::string (data->object_binding_info().the_object);
		    isContext = false;
		    break;
		}
	    case CdmwNamingAndRepository::NAR_CONTEXT_BINDING:
		{ 
		    result = std::string (data->context_binding_info().the_object);
		    isContext = true;
		    break;
		}
	    default:
		{ 
		    CDMW_NEVER_HERE();
		    break;
		}
	    }

        }
        return result;

    }  
    catch (Cdmw::CommonSvcs::DataStore::NotFoundException & ex) {
	// ALERT, select failed, element removed concurrently
	return std::string("");
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

    context = false; // init, AFAICS not done by caller :/

    try
    {
 
	std::string result = ""; // init

	const std::string key = createStorageKey (m_id, name);

	FTDataStore *storage = FTDataStoreHelper::getStorage(NAMING_CONTEXT_DATASTORE_ID);

	if (storage->contains(key)) {
	    CdmwNamingAndRepository::StorageData_var data = 
		storage->select (key);
	    
	    switch (data->_d()) {
	    case CdmwNamingAndRepository::NAR_OBJECT_BINDING:
		{ 
		    result = std::string (data->object_binding_info().the_object.in());
		    context = false;
		    break;
		}
	    case CdmwNamingAndRepository::NAR_CONTEXT_BINDING:
		{ 
		    result = std::string (data->context_binding_info().the_object.in());
		    context = true;
		    break;
		}
	    default:
		{ 
		    // FIXME - maybe useless here
		    CDMW_NEVER_HERE();
		    break;
		}
	    }
	}
	
        return result;

    }
    catch (Cdmw::CommonSvcs::DataStore::NotFoundException & ex) {
	// ALERT, select failed, element removed concurrently
	return std::string("");
    }
    catch(const std::bad_alloc &)
    {
	// FIXME - where is it raised
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

 	const std::string key = createStorageKey (m_id, name);

	FTDataStore *storage = FTDataStoreHelper::getStorage(NAMING_CONTEXT_DATASTORE_ID);

	if ( ! storage->contains(key)) {
	    CdmwNamingAndRepository::StorageData_var data 
		(new CdmwNamingAndRepository::StorageData());

	    if (ObjectBinding == binding.m_type) {
		// ObjectBinding == binding.m_type
		CdmwNamingAndRepository::ObjectBindingInfo info;
		info.the_name   = name.c_str(); 
		info.the_object = binding.m_objectRefString.c_str();

		data->object_binding_info (info);
	    } 
	    else {
		// NamingContextBinding == binding.m_type
		CDMW_ASSERT (NamingContextBinding == binding.m_type);
			
		// FIXME handle as ContextPeerInfo
		CdmwNamingAndRepository::ContextBindingInfo info;
		info.the_name   = name.c_str(); 
		info.the_object = binding.m_objectRefString.c_str();

		data->context_binding_info (info);
	    }

	    try {
                storage->insert (key, data.in());
                result = true;
		CDMW_ASSERT (storage->contains (key));
	    } 
	    catch (const Cdmw::CommonSvcs::DataStore::AlreadyExistException &ex) {
		// ALERT, insert railed, same object-key defined concurrently 
		result = false;
	    }
            catch (Cdmw::CommonSvcs::DataStore::StorageErrorException & ex) {
                // ALERT, concurrently removed, but same result, FIXME,CHECKME
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

// FIXME - check semantic, returns true only if  update occures, otherwise false
bool PersistentNamingContext::updateBinding(const std::string& name,
    const std::string& object, BindingType type)
    throw (OutOfResourcesException)
{

    CDMW_WRITER_LOCK_GUARD(m_rwLock);

    try
    {

	bool result = false;
	
	const std::string key = createStorageKey (m_id, name);
 
	FTDataStore *storage = FTDataStoreHelper::getStorage(NAMING_CONTEXT_DATASTORE_ID);

	CdmwNamingAndRepository::StorageData_var data 
	    (new CdmwNamingAndRepository::StorageData());
	
	if (ObjectBinding == type) {
	    CdmwNamingAndRepository::ObjectBindingInfo info;
	    info.the_name   = name.c_str(); 
	    info.the_object = object.c_str();
	    
	    data->object_binding_info (info);
	} 
	else {
	    CDMW_ASSERT (NamingContextBinding == type);
	    
	    // FIXME handle as ContextPeerInfo
	    CdmwNamingAndRepository::ContextBindingInfo info;
	    info.the_name   = name.c_str(); 
	    info.the_object = object.c_str();

	    data->context_binding_info (info);
	}
	
	if (storage->contains(key)) {
	    try {
		// 2phase commit might fail
		storage->update (key, data.in()); // to be expected true
                result = true;
		CDMW_ASSERT (storage->contains (key));
	    } catch (const Cdmw::CommonSvcs::DataStore::NotFoundException &ex) {
		// ALERT, someone changed database concurrently
		result = false;
	    } catch (const Cdmw::CommonSvcs::DataStore::StorageErrorException & ex) {
                // ALERT, concurrently removed, but same result, FIXME,CHECKME
                result = false;
            } 
	} 
	else {
	    try {
                storage->insert (key, data.in()); // to be expected true
                result = true;
		CDMW_ASSERT (storage->contains (key));
	    } catch (const Cdmw::CommonSvcs::DataStore::AlreadyExistException &ex) {
		// ALERT, someone changed database concurrently
		result = false;;
	    } catch (const Cdmw::CommonSvcs::DataStore::StorageErrorException & ex) {
                // ALERT, concurrently removed, but same result, FIXME,CHECKME
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


bool PersistentNamingContext::removeBinding(const std::string& name)
{
    bool result = false;

    CDMW_WRITER_LOCK_GUARD(m_rwLock);

    const std::string key = createStorageKey (m_id, name);
   
    FTDataStore *storage = FTDataStoreHelper::getStorage(NAMING_CONTEXT_DATASTORE_ID);
    
    CdmwNamingAndRepository::StorageData_var data 
	(new CdmwNamingAndRepository::StorageData());

    if (storage->contains(key)) {
	try {
	    // FIXME - verify it is Object Binding we erase, verify
	    // datastore keep consistent if erasing entry, check with
	    // erasing NamingContext, remove siblings, raise exception
            storage->remove(key);
            result = true;

	    CDMW_ASSERT (!storage->contains(key)); 	    
	} 
	catch (Cdmw::CommonSvcs::DataStore::NotFoundException &ex) {
	    // Alert, datastore changed concurrently
	    result = false;
	} catch (Cdmw::CommonSvcs::DataStore::StorageErrorException & ex) {
	    // ALERT, concurrently removed, but same result, FIXME,CHECKME
	    result = false;
	} 
    }

    return result;

}

// FIXME move into anonymous namespace
template<class BindingInfo_T>
class GatherSiblingsBindingInfo:  
    public std::unary_function<std::pair< 
                                   const std::string ,  
                                   CdmwNamingAndRepository::StorageData_var> , 
                               void>
{
public:
    
    GatherSiblingsBindingInfo (const std::string &contextPrefix, 
			       std::list<BindingInfo_T> &bindings) 
	: m_prefix(contextPrefix), m_bindings(bindings)
    { 
    } 
        
    void operator ()(const std::pair< const std::string, CdmwNamingAndRepository::StorageData_var > & it)
    {
	const std::string prefix = it.first.substr(0, m_prefix.length());
	// OPTME - do not create substr for prefix-comparison
	if (m_prefix == prefix) {
	    const CdmwNamingAndRepository::StorageData_var &data = it.second;

	    // FIXME - lots of duplicated code 
	    switch (data->_d()) {
	    case CdmwNamingAndRepository::NAR_OBJECT_BINDING:
		{
		    const std::string name =
			data->object_binding_info().the_name.in();
		    const std::string objref = 
			data->object_binding_info().the_object.in();
		    const Binding binding (objref, ObjectBinding);
		    const BindingInfo_T bi(name, binding);
		    m_bindings.push_back(bi);
		    break;
		}
	    case CdmwNamingAndRepository::NAR_CONTEXT_BINDING:
		{
		    const std::string name = 
			data->context_binding_info().the_name.in();
		    const std::string objref = 
			data->context_binding_info().the_object.in();
		    const Binding binding(objref, NamingContextBinding);
		    const BindingInfo_T bi(name, binding);
		    m_bindings.push_back(bi);
		    break;
		}
	    default:
		{ 
		    // domain should not be member of any context
		    CDMW_NEVER_HERE();
		}
	    }
	}
    }

    const std::string          &m_prefix; 
    std::list<BindingInfo_T>   &m_bindings;
};

PersistentNamingContext::BindingInfoList PersistentNamingContext::list(void)
        throw (OutOfResourcesException)
{

    CDMW_READER_LOCK_GUARD(m_rwLock);

    try
    {

        BindingInfoList bil; // OPTME - causes copy on return

	FTDataStore *storage = FTDataStoreHelper::getStorage(NAMING_CONTEXT_DATASTORE_ID);
	const std::string contextPrefix=createStorageKey(m_id, "");

 	storage->for_each(GatherSiblingsBindingInfo<BindingInfo>
			  (contextPrefix, bil));
	
        return bil; // OPTME - causes copy on return

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

	FTDataStore *storage = FTDataStoreHelper::getStorage(NAMING_CONTEXT_DATASTORE_ID);
	const std::string contextPrefix = createStorageKey (m_id, "");
 	storage->for_each(GatherSiblingsBindingInfo<DetailedBindingInfo>
			  (contextPrefix, bil));

        return bil; // OPTME - causes copy of list

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}


} // End of namespace NamingAndRepository
} // End of namespace Cdmw

