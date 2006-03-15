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

#ifndef INCL_COMMONSVCS_DATASTORE_STDMAPDATASTORE_HPP
#define INCL_COMMONSVCS_DATASTORE_STDMAPDATASTORE_HPP

#include "Foundation/osthreads/ReaderLockGuard.hpp"

#include <Foundation/commonsvcs/datastore/DataStoreInterface.hpp>
#include <Foundation/commonsvcs/datastore/DataStoreBase.hpp>

#include <Foundation/common/Exception.hpp>
#include <Foundation/osthreads/ReaderLockGuard.hpp>
#include <Foundation/osthreads/WriterLockGuard.hpp>
#include <Foundation/osthreads/MutexGuard.hpp>
#include <Foundation/osthreads/ReaderWriterLock.hpp>
#include <sstream>
#include <list>
#include <functional>
#include <map>

namespace Cdmw {
namespace CommonSvcs {
namespace DataStore {

/**
*This template class defines a DataStore which is  backed by a standard map.
*
*This is a template class where the Oid and Data should be IDL types.
*
*Features:
*<p> Thread safe. Exception safe.
*/
template <class Oid, class Data, class OidCompare=std::less<typename Cdmw::OrbSupport::type_traits<Oid>::_var_type> > 
class StdMapDataStore : public DataStoreInterface<Oid, Data, OidCompare>,
                        public DataStoreBase
{

public:   

    typedef DataStoreInterface<Oid, Data, OidCompare> DataStoreInterface;
    typedef typename DataStoreInterface::_Oid_var_type  _Oid_var_type;
    typedef typename DataStoreInterface::_Oid_in_type   _Oid_in_type;
    typedef typename DataStoreInterface::_Oid_retn_type _Oid_retn_type;

    typedef typename DataStoreInterface::_Data_var_type  _Data_var_type;
    typedef typename DataStoreInterface::_Data_in_type   _Data_in_type;
    typedef typename DataStoreInterface::_Data_retn_type _Data_retn_type;
    
    typedef typename DataStoreInterface::DataStoreObserver DataStoreObserver;
    typedef std::list<DataStoreObserver*> DataStoreObserverList;
    typedef typename DataStoreObserverList::iterator DataStoreObserverListIterator;

    typedef std::map<_Oid_var_type, _Data_var_type, OidCompare> DataMap;
    typedef typename DataMap::iterator DataMapIterator;
    typedef typename DataMap::const_iterator DataMapConstIterator;

    /**
     * Constructor of a StdMapDataStore.
     *
     *@param dsid the id of the data store. this id must be unique in the system
     */ 
    StdMapDataStore(DsIdentifier dsId);
    
    /**
     * Destructor.
     */
    virtual ~StdMapDataStore();

    /**
     * Registers an observer to be notified whenever there is an insertion, update, or removal
     * of a record.
     *
     *@param observer a const pointer to a DataStoreObserver object
     *@return true if the Data Store observer is registered, false if failed to register it
     * (it has already been registered, or exceeded maximum allowed number of observers ...)
     */
    virtual bool register_observer(DataStoreObserver * observer) throw();

    /**
     * Un-registers an already registered observer. The observer will no longer be notified by the datastore.
     *
     *@param observer a const pointer to the DataStoreObserver object
     *@return true if the Data Store observer is unregistered, false if the observer was not previously registered
     */
    virtual bool unregister_observer(DataStoreObserver * observer) throw();

    /**
     * Return true if the DataStore contains a data with Object Id key
     *
     *@param oid is the Object Id key
     *@return true if the Data Store contains this data
     */
    bool contains(_Oid_in_type oid) throw();

    /**
     * Return the data with Object Id key
     *
     *@param oid is the Object Id key
     *@exception NotFound if no object found with this oid
     */
    _Data_retn_type 
    select(const _Oid_in_type oid) 
        throw (NotFoundException);

    /**
     * Remove from the DataStore the entry with the given oid key
     *
     *@param oid is the Object Id key
     */
    void remove(const _Oid_in_type oid) throw (NotFoundException,StorageErrorException);
    
    /**
     * insert in the DataStore the entry with the given oid key
     *
     *@param oid is the Object Id key
     *@param data is the data associated with Oid key
     */
    void insert(const _Oid_in_type oid, const _Data_in_type data)
        throw (AlreadyExistException,StorageErrorException);

    /**
     * update the DataStore entry with the given oid key
     *
     *@param oid is the Object Id key
     *@param data is the data associated with Oid key
     */
    void update(const _Oid_in_type oid, const _Data_in_type data)
        throw (NotFoundException,StorageErrorException);

    /**
     * Give the size of the DataStore
     *
     *@return int the size
     */
    int size();

    /**
     * Initialized the datastore in cold/hot restart.
     */
    void init(bool cold_restart) throw (Cdmw::InitErrorException);

protected:



    /**
     * execute function for each element of DataStore
     * NOTE: The implementation does not autorise the given function to modify the data
     *       Only Function with const parameter will compile
     *       (typical example: const std::pair<Oid, Data>& is OK)
     *
     *@param Functor f to execute
     */
    void do_for_each(Cdmw::CommonSvcs::DataStore::Functor<Oid,Data>& f);

private:

    // The data in local memory
    DataMap m_data;

    // Current observers
    DataStoreObserverList m_observers;

    // Mutex protection to R/W data
    // For simplicity we choose a global one for all data instances
    // (if we choosed to handle each Oid independently the lock should
    //  be global anyway when insert and remove)
    OsSupport::ReaderWriterLock m_dataMapMutex;    
    
    /**
     * Default constructor.
     */
    StdMapDataStore();
    
    /**
     * Assignement operator is not allowed.
     */
    StdMapDataStore& operator=(const StdMapDataStore& rhs);

}; // End class StdMapDataStore

//
// IMPLEMENTATION PART OF THE TEMPLATE
//

template <class Oid, class Data, class OidCompare>
StdMapDataStore<Oid, Data, OidCompare>::StdMapDataStore(DsIdentifier dsId)
    : DataStoreBase(dsId)
{
}

template <class Oid, class Data, class OidCompare>
StdMapDataStore<Oid, Data, OidCompare>::~StdMapDataStore()
{
}


template <class Oid, class Data, class OidCompare>
bool
StdMapDataStore<Oid, Data, OidCompare>::register_observer(DataStoreObserver * observer) throw()
{
    DataStoreObserverListIterator pos;

    pos = std::find(m_observers.begin(), m_observers.end(), observer);

    if (pos == m_observers.end())
    {
        m_observers.push_back(observer);
        return true;
    }
    else
    {
        return false;
    }
}

template <class Oid, class Data, class OidCompare>
bool
StdMapDataStore<Oid, Data, OidCompare>::unregister_observer(DataStoreObserver * observer) throw()
{
    DataStoreObserverListIterator pos;

    pos = std::find(m_observers.begin(), m_observers.end(), observer);

    if (pos != m_observers.end())
    {
        m_observers.erase(pos);
        return true;
    }
    else
    {
        return false;
    }
}

template <class Oid, class Data, class OidCompare>
int
StdMapDataStore<Oid, Data, OidCompare>::size()
{
    CDMW_READER_LOCK_GUARD(m_dataMapMutex);
    return m_data.size();
}

template <class Oid, class Data, class OidCompare>
bool 
StdMapDataStore<Oid, Data, OidCompare>::contains(_Oid_in_type oid) throw ()
{
    CDMW_READER_LOCK_GUARD(m_dataMapMutex);
    return (m_data.find(oid) != m_data.end());
}



template <class Oid, class Data, class OidCompare>
typename StdMapDataStore<Oid, Data, OidCompare>::_Data_retn_type
StdMapDataStore<Oid, Data, OidCompare>::select(const _Oid_in_type oid) 
    throw (NotFoundException)
{
    CDMW_READER_LOCK_GUARD(m_dataMapMutex);
    DataMapConstIterator itData = m_data.find(oid);
    if (itData != m_data.end())
    {
        // make deep copy
        _Data_var_type result = itData->second;
        // return result
        return Cdmw::OrbSupport::type_traits<Data>::_retn(result);
    }
    else
    {
        throw NotFoundException("The Oid does not exist");
    }
}

template <class Oid, class Data, class OidCompare>
void 
StdMapDataStore<Oid, Data, OidCompare>::remove(const _Oid_in_type oid)
    throw (NotFoundException,StorageErrorException)
{
    {
        CDMW_WRITER_LOCK_GUARD(m_dataMapMutex);

        DataMapConstIterator itData = m_data.find(oid);
        if (itData != m_data.end())
        {
            m_data.erase(oid);
        }
        else
        {
            throw NotFoundException("DataStore: The Oid does not exist");
        }
    }

    DataStoreObserverListIterator pos;

    for (pos = m_observers.begin(); pos != m_observers.end(); ++pos)
    {
        (*pos)->on_remove(oid);
    }
}

template <class Oid, class Data, class OidCompare>
void 
StdMapDataStore<Oid, Data, OidCompare>::insert(const _Oid_in_type oid,
                                               const _Data_in_type data) 
    throw (AlreadyExistException,StorageErrorException)
{
    {
        CDMW_WRITER_LOCK_GUARD(m_dataMapMutex);

        DataMapConstIterator itData = m_data.find(oid);
        if (itData == m_data.end())
        {
            // store copy of data
           m_data[oid] = Cdmw::OrbSupport::type_traits<Data>::_copy(data);
        }
        else
        {
           throw AlreadyExistException("DataStore: The oid already exist");
        }
    }

    DataStoreObserverListIterator pos;
    for (pos = m_observers.begin(); pos != m_observers.end(); ++pos)
    {
        (*pos)->on_insert(oid, data);
    }
}

template <class Oid, class Data, class OidCompare>
void 
StdMapDataStore<Oid, Data, OidCompare>::update(const _Oid_in_type oid,
                                               const _Data_in_type data) 
    throw (NotFoundException,StorageErrorException)
{
    {
        CDMW_WRITER_LOCK_GUARD(m_dataMapMutex);

        DataMapIterator itData = m_data.find(oid);
        if (itData != m_data.end())
        {
            // store copy of data
            itData->second = Cdmw::OrbSupport::type_traits<Data>::_copy(data);
        }
        else
        {
            throw NotFoundException("DataStore: The Oid does not exist");
        }
    }

    DataStoreObserverListIterator pos;

    for (pos = m_observers.begin(); pos != m_observers.end(); ++pos)
    {
        (*pos)->on_update(oid, data);
    }
}

template <class Oid, class Data, class OidCompare>
void StdMapDataStore<Oid, Data, OidCompare>::do_for_each(Cdmw::CommonSvcs::DataStore::Functor<Oid,Data>& f)
{
    CDMW_READER_LOCK_GUARD(m_dataMapMutex);

    DataMapConstIterator itBegin = m_data.begin();
    DataMapConstIterator itEnd = m_data.end();
    // Security: we force call of std::for_each with const iterators
    Cdmw::CommonSvcs::DataStore::FunctorWrapper<Oid,Data> f__(f);
    std::for_each(itBegin, itEnd, f__);
}

}  // End namespace DataStore
}  // End namespace CommonSvcs
}  // End namespace Cdmw

#endif /* INCL_COMMONSVCS_DATASTORE_STDMAPDATASTORE_HPP */

