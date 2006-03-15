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

#ifndef INCL_CDMW_FT_FTSTATEMANAGER_DATASTORE_HPP
#define INCL_CDMW_FT_FTSTATEMANAGER_DATASTORE_HPP

#include <Foundation/osthreads/ReaderLockGuard.hpp>

#include <FaultTolerance/ftstatemanager/DataStoreBase.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreRecordIteratorBase.hpp>
#include <FaultTolerance/ftstatemanager/TransactionEngine.hpp>
#include <FaultTolerance/ftstatemanager/TransactionEngineCohort.hpp>
#include <FaultTolerance/ftstatemanager/TransactionEngineCoordinator.hpp>
#include <FaultTolerance/idllib/CdmwFTStateTransfer.skel.hpp>

#include <Foundation/common/Exception.hpp>
#include "Foundation/commonsvcs/datastore/DataStoreInterface.hpp"
#include "Foundation/commonsvcs/datastore/DataStoreObserver.hpp"
#include "Foundation/commonsvcs/datastore/DataStoreRegistry.hpp"
#include <Foundation/osthreads/ReaderLockGuard.hpp>
#include <Foundation/osthreads/WriterLockGuard.hpp>
#include <Foundation/osthreads/MutexGuard.hpp>
#include <Foundation/orbsupport/TypeTraits.hpp>
#include <Foundation/orbsupport/Codec.hpp>
#include <Foundation/orbsupport/Sequence.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <sstream>
#include <list>
#include <functional>

#include <FaultTolerance/ftcommon/FTLogger.hpp>


namespace Cdmw {
namespace FT {

const CORBA::ULong DEFAULT_FREEZE_THRESHOLD = 20;
const CORBA::ULong DEFAULT_ITERATOR_INCREMENT = 100;
const OsSupport::OS::Timeval DEFAULT_FREEZE_TIMEOUT(5L, 0L);
    
    
// Forward declaration
template <class OID, class DATA, class OIDCOMPARE> 
class DataStoreInitialiser;

template <class OID, class DATA, class OIDCOMPARE> 
class DataStoreRecordIterator;

typedef Cdmw::CommonSvcs::DataStore::AlreadyExistException
    AlreadyExistException;
typedef Cdmw::CommonSvcs::DataStore::NotFoundException NotFoundException;

/**
*This template class defines a DataStore which is able to transfer
*each of its modified data to other DataStores in other backup process.
*The state transfer could use multicast or point-to-point protocol. 
*
*This is a template class where the Oid and Data should be IDL types.
*
*Features:
*<p> Thread safe. Exception safe.
*/
template <class OID, class DATA, class OIDCOMPARE=std::less<typename Cdmw::OrbSupport::type_traits<OID>::_var_type> > 
class DataStore : public DataStoreBase,
                  public Timer_Event_Handler<void>,
                  public Cdmw::CommonSvcs::DataStore::DataStoreInterface<OID, DATA, OIDCOMPARE>
{
    typedef typename CdmwFT::Transaction::StateChangeRecord StateChangeRecord;
    typedef typename CdmwFT::Transaction::StateChangeRecords StateChangeRecords;
    typedef DataStoreRecordIterator<OID, DATA, OIDCOMPARE>
        DataStoreRecordIterator;
    typedef std::map<std::string, DataStoreRecordIterator*,
                     std::less<std::string> >
        RecordIteratorMap;
    typedef typename DataStoreContext::LocationList LocationList;
    typedef Timer<void> Timer;
    typedef typename Timer::TimerId TimerId;
    typedef std::list<TimerId> TimerIdList;

public:   

    typedef typename Cdmw::OrbSupport::type_traits<OID>::_var_type     _Oid_var_type;
    typedef typename Cdmw::OrbSupport::type_traits<OID>::_in_type      _Oid_in_type;
    typedef typename Cdmw::OrbSupport::type_traits<OID>::_retn_type    _Oid_retn_type;

    typedef typename Cdmw::OrbSupport::type_traits<DATA>::_var_type     _Data_var_type;
    typedef typename Cdmw::OrbSupport::type_traits<DATA>::_in_type      _Data_in_type;
    typedef typename Cdmw::OrbSupport::type_traits<DATA>::_retn_type    _Data_retn_type;
    
    typedef Cdmw::CommonSvcs::DataStore::DataStoreObserver<OID, DATA> DataStoreObserver;
    typedef std::list<DataStoreObserver*> DataStoreObserverList;
    typedef typename DataStoreObserverList::iterator DataStoreObserverListIterator;
    typedef Cdmw::CommonSvcs::DataStore::Functor<OID, DATA> Functor;

    typedef std::map<_Oid_var_type, _Data_var_type, OIDCOMPARE> DataMap;
    typedef typename DataMap::iterator DataMapIterator;
    typedef typename DataMap::const_iterator DataMapConstIterator;
    typedef std::insert_iterator<DataMap> DataInsertIterator;
    typedef DataStoreInitialiser<OID, DATA, OIDCOMPARE> DataStoreInitialiser;


    /**
     * Constructor of a DataStore using multicast state transfer.
     *
     *@param dsid the id of the data store. this id must be unique in the system
     *@param orb the orb reference
     *@param poa the poa reference
     *@param port the port for the multicast address. the port and the port+1 will be used
     *            to the transfert of data between coordinator and cohort
     *@param domain the domain for the multicast address.
     *@param ip_address the ip address for the multicast address.
     *@param max_transaction_in_progress the max number of transaction in progress 
     *@param max_transaction_done the max number of transaction done
     *@param cohort_timeout the cohort timeout (timeout between a prepared_ operation and
     * the commit)
     *@param coordinator_timeout (timeout between the prepared_ operation and a yes/no, 
     *                           and a commit/roolback operation and a end
     *@param nbthreads the number of worker threads.
     *@param freeze_threshold threshold of the count of state change records
     * remaining to transfer that below which an iterator will freeze the
     * DataStore.
     *@param iterator_increment the size of sequence to request when copying
     * state from a primary. 
     *
     *@exception CORBA::SystemException Any CORBA system exception that
     * may be raised by the underlying ORB.
     */ 
    DataStore(DsIdentifier            dsId,
              CORBA::ORB_ptr          orb,
              PortableServer::POA_ptr poa,
              DataStoreContext&       context,
              CORBA::ULong            freeze_threshold = DEFAULT_FREEZE_THRESHOLD,
              CORBA::ULong            iterator_increment = DEFAULT_ITERATOR_INCREMENT,
              OsSupport::OS::Timeval  freeze_timeout = DEFAULT_FREEZE_TIMEOUT,
              bool                    register_data_store = true)
        throw(AlreadyExistException, CORBA::SystemException)
        : Cdmw::FT::DataStoreBase(dsId),
          m_orb(CORBA::ORB::_duplicate(orb)),
          m_poa(PortableServer::POA::_duplicate(poa)),
          m_context(context),
          m_oid_codec(m_orb.in()),
          m_data_codec(m_orb.in()),
          m_pInitialiser(0),
          m_iterator_increment(iterator_increment),
          m_freeze_threshold(freeze_threshold),
          m_freeze_timeout(freeze_timeout),
          m_condition(m_mutex),
          m_is_activated(false)
    {
        if (register_data_store)
        {
            Cdmw::CommonSvcs::DataStore::DataStoreRegistry::
                Register_data_store(this);
        }
        m_context.register_datastore(this);
        // FIXME: We may have to add a mutex here in case of applications do not
        // rely on generated code!
        M_ft_dsMap[dsId] = this;
    }

    
    /**
     * Destructor.
     */
    virtual ~DataStore()
    {
        try
        {
            Cdmw::CommonSvcs::DataStore::DataStoreRegistry::
                Unregister_data_store(this->get_dsid());
        }
        catch (const NotExistException&)
        {
            // Ignore
        }

        // FIXME: We may have to add a mutex here in case of applications do not
        // rely on generated code!
        M_ft_dsMap.erase(this->get_dsid());
        m_timer.stop();
    }

    inline DataStoreContext& get_context()
    {
        return m_context;
    }

    /**
     * Returns an iterator that provides state information necessary to copy
     * a DataStore.
     *
     * @param from_location is the name of the location asking the records.
     * @param coordinator the transaction cordinator.
     * @param cohort the cohort.
     *
     * @exception InvalidDataStoreState is raised if the data store is not the 
     * coordinator
     */
    virtual DataStoreRecordIteratorBase* get_record_iterator
    (const char * from_location)
        throw (CORBA::SystemException,
               CdmwFT::StateTransfer::InvalidDataStoreState);

    /**
     * Adds the given location as a new backup for the DataStore and removes
     * the RecordIterator associated with the location.
     *
     * @param location location of new backup.
     * @param coordinator the transaction cordinator.
     * @param cohort the cohort.
     */
    void iterator_finished(const std::string& location)
        throw (AlreadyExistException, NotFoundException);

    /**
     * Freeze timer callback method.
     */
    int handle_timeout(const void*);

    /**
     * Registers an observer to be notified whenever there is an insertion, update, or removal
     * of a record.
     *
     *@param observer a const pointer to a DataStoreObserver object
     *@return true if the Data Store observer is registered, false if failed to register it
     * (it has already been registered, or exceeded maximum allowed number of observers ...)
     */
    bool register_observer(DataStoreObserver * observer) throw()
    {
        CDMW_WRITER_LOCK_GUARD(m_observers_mutex);
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

    /**
     * Un-registers an already registered observer. The observer will no longer be notified by the datastore.
     *
     *@param observer a const pointer to the DataStoreObserver object
     *@return true if the Data Store observer is unregistered, false if the observer was not previously registered
     */
    bool unregister_observer(DataStoreObserver * observer) throw()
    {
        CDMW_WRITER_LOCK_GUARD(m_observers_mutex);
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

    /**
     * Registers an initialiser object to be notified when the coordinator datastore is initialising.
     * NOTA: Only one initialiser is allowed.
     *
     *@param initialiser a const pointer to a DataStoreInitialiser object
     *@return true if the Data Store initialiser is registered, false if failed to register it
     * (it has already been registered)
     */

    bool register_initialiser(DataStoreInitialiser * initialiser) throw()
    {
        if (!m_pInitialiser)
        {
            m_pInitialiser = initialiser;
            return true;
        }
        else
        {
            return false;
        }
    }
    
    /**
     * Un-registers an already registered initialiser. The initialiser will no longer be called by the datastore.
     *
     *@param initialiser a const pointer to the DataStoreInitialiser object
     *@return true if the Data Store initialiser is unregistered, false if the initialiser was not previously registered
     */
    bool unregister_initialiser(DataStoreInitialiser * initialiser) throw()
    {
        if (m_pInitialiser == initialiser)
        {
            m_pInitialiser = 0;
            return true;
        }
        else
        {
            return false;
        }
    }
    
    /**
     * Registers an backup observer to be notified whenever there is
     * an insertion, update, or removal of a record. In contrast to
     * the regular obsever a backup observer is called while the data
     * map mutex is held.
     *
     *@param observer a const pointer to a DataStoreObserver object
     *@return true if the Data Store observer is registered, false if
     *failed to register it (it has already been registered, or
     *exceeded maximum allowed number of observers ...)
     */
    bool register_backup_observer(DataStoreObserver * observer) throw()
    {
        CDMW_WRITER_LOCK_GUARD(m_backup_observers_mutex);
        DataStoreObserverListIterator pos;

        pos = std::find(m_backup_observers.begin(),
                        m_backup_observers.end(), observer);

        if (pos == m_backup_observers.end())
        {
            m_backup_observers.push_back(observer);
            return true;
        }
        else
        {
            return false;
        }
    }

    /**
     * Un-registers an already registered backup observer. The
     * observer will no longer be notified by the datastore.
     *
     *@param observer a const pointer to the DataStoreObserver object
     *@return true if the Data Store observer is unregistered, false
     *if the observer was not previously registered
     */
    bool unregister_backup_observer(DataStoreObserver * observer) throw()
    {
        CDMW_WRITER_LOCK_GUARD(m_backup_observers_mutex);
        DataStoreObserverListIterator pos;

        pos = std::find(m_backup_observers.begin(),
                        m_backup_observers.end(), observer);

        if (pos != m_backup_observers.end())
        {
            m_backup_observers.erase(pos);
            return true;
        }
        else
        {
            return false;
        }
    }

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
     * Do it in the cohorts then localy (if no problem with cohorts)
     *
     * This operation will potentially block until updating replicas
     *
     *@param oid is the Object Id key
     */
    void remove(const _Oid_in_type oid) throw (NotFoundException,
                                               Cdmw::CommonSvcs::DataStore::StorageErrorException);
    
    /**
     * Remove from the local DataStore the entry with the given oid key
     *     
     *@param oid is the Object Id key
     */
    void remove_local(const _Oid_in_type oid) throw (NotFoundException);
    
    /**
     * insert in the DataStore the entry with the given oid key
     * do it in the cohorts then localy (if no problem with cohorts)
     *
     * This operation will potentially block until updating replicas
     *
     *@param oid is the Object Id key
     *@param data is the data associated with Oid key
     */
    void insert(const _Oid_in_type oid, _Data_in_type data)
        throw (AlreadyExistException,Cdmw::CommonSvcs::DataStore::StorageErrorException);

    /**
     * insert in the local DataStore the entry with the given oid key
     *
     *@param oid is the Object Id key
     *@param data is the data associated with Oid key
     */
    void insert_local(const _Oid_in_type oid, const _Data_in_type data)
        throw (AlreadyExistException);

    /**
     * update the DataStore entry with the given oid key
     * do it in the cohorts then localy (if no problem with cohorts)
     *
     * This operation will potentially block until updating replicas
     *
     *@param oid is the Object Id key
     *@param data is the data associated with Oid key
     */
    void update(const _Oid_in_type oid, const _Data_in_type data)
        throw (NotFoundException,Cdmw::CommonSvcs::DataStore::StorageErrorException);

    /**
     * update the local DataStore entry with the given oid key
     *
     *@param oid is the Object Id key
     *@param data is the data associated with Oid key
     */
    void update_local(const _Oid_in_type oid, const _Data_in_type data)
        throw (NotFoundException);

    /**
     * Give the size of the DataStore
     *
     *@return int the size
     */
    int size()
    {
        CDMW_READER_LOCK_GUARD(m_dataMapMutex);
        return m_data.size();
    }

    /**
     * Initialized the datastore in cold/hot restart.
     */
    void init(bool cold_restart) throw (Cdmw::InitErrorException);

    //  Part for link with transaction engine

    /**
     * The data store becomes the one of the master
     */
    void activate();

    /**
     * The data store becomes the one of the slave
     */
    void deactivate();

    /**
     * process failure of a location
     *
     *@param location is the failed location
     */
    void on_failure(const std::string& location)
    {
        m_context.on_failure(location);
    }

    /**
     * Load state transferred from another DataStore
     *
     * @param recs sequence of change records
     */
    void load_state(const StateChangeRecords& recs)
        throw (NotFoundException, AlreadyExistException);

    /**
     * Validate a set of changes.
     *
     * @param recs sequence of state changes (inserts, updates and removes).
     */
    bool validate(const StateChangeRecords& recs);

    /**
     * Compare the oids for a set of changes.
     *
     * @result true if any oids match, false otherwise
     */
    bool oids_match(const CdmwFT::Transaction::StateChangeRecords& recs1,
                    const CdmwFT::Transaction::StateChangeRecords& recs2);

    /**
     * Requests the DataStore is frozen with respect to insert, update
     * and remove. Multiple locations may requests freezes concurrently.
     * When frozen insert, update and remove operations will block.
     *
     * @param location the location of the iterator requesting the freeze.
     */
    void freeze(const std::string& location);

    /**
     * Indicates a iterator is finished reading from the DataStore and
     * the freeze can be released. Unfreezing will actually occur when
     * the last iterator requesting a freeze is released,
     *
     * @param location the location of the iterator requesting the unfreeze.
     */
    void unfreeze(const std::string& location);

    /**
     * Wait for transactions in progress to complete.
     * Deprecated. Only for use by RecordIterator.
     */
    void wait_for_no_transactions_in_progress();

    inline bool is_activated()
    {
        return m_is_activated;
    }

protected:

    /**
     * execute function for each element of DataStore
     * NOTE: The implementation does not autorise the given function to modify the data
     *       Only Function with const parameter will compile
     *       (typical example: const std::pair<Oid, Data>& is OK)
     *
     *@param Functor f to execute
     */
    void do_for_each(Cdmw::CommonSvcs::DataStore::Functor<OID,DATA>& f);

private:

    /**
     * Applies a state changes (insert, update or remove) to the DataStore.
     *
     * @param rec a StateChangeRecord.
     */
    void apply_change(const StateChangeRecord& rec)
        throw (NotFoundException, AlreadyExistException);

    /**
     * Blocks while any freeze request are present (or until a timeout).
     */
    void freeze_check();

    /**
     * Invokes any TimeOut handlers supplying each location of an
     * iterator that has frozen the DataStore.
     */
    void invoke_timeouts();

    CORBA::ORB_var m_orb;
    PortableServer::POA_var m_poa;
    
    DataStoreContext& m_context;

    Cdmw::OrbSupport::Codec<OID> m_oid_codec;
    Cdmw::OrbSupport::Codec<DATA> m_data_codec; 

    PortableServer::ObjectId_var m_coordinator_id;
    PortableServer::ObjectId_var m_cohort_id;

    // The data in local memory
    DataMap m_data;

    // Current observers
    DataStoreObserverList m_observers;

    // Lock for observers list
    OsSupport::ReaderWriterLock m_observers_mutex;

    // Current backup observers
    DataStoreObserverList m_backup_observers;

    // Lock for backup observers list
    OsSupport::ReaderWriterLock m_backup_observers_mutex;

    // Initialiser
    DataStoreInitialiser* m_pInitialiser;

    // Iterator map
    RecordIteratorMap m_iter_map;

    // Size of sequence used by update_with_coordinator to transfer state
    CORBA::ULong m_iterator_increment;

    // Size of change set for freezing
    CORBA::ULong m_freeze_threshold;

    // Freeze timeout for cancelling freeze
    OsSupport::OS::Timeval m_freeze_timeout;

    // Freeze timeout timer
    Timer m_timer;

    // Mutex for freezing
    Cdmw::OsSupport::Mutex m_mutex;

    // Condition variable for freezing
    Cdmw::OsSupport::Condition m_condition;

    // List of frozen iterator locations
    LocationList m_freeze_locations;

    // Mutex protection to R/W data
    // For simplicity we choose a global one for all data instances
    // (if we choosed to handle each Oid independently the lock should
    //  be global anyway when insert and remove)
    OsSupport::ReaderWriterLock m_dataMapMutex;    

    bool m_is_activated;

    /**
     * Default constructor.
     */
    DataStore();
    
    /**
     * Assignement operator is not allowed.
     */
    DataStore& operator=(const DataStore& rhs);

}; // End class DataStore

}  // End namespace FT
}  // End namespace Cdmw

#include <FaultTolerance/ftstatemanager/DataStoreInitialiser.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreRecordIterator.hpp>

namespace Cdmw {
namespace FT {

//
// IMPLEMENTATION PART OF THE TEMPLATE
//


template <class OID, class DATA, class OIDCOMPARE>
DataStoreRecordIteratorBase*
DataStore<OID, DATA, OIDCOMPARE>::get_record_iterator
(const char * from_location)
   throw (CORBA::SystemException,
          CdmwFT::StateTransfer::InvalidDataStoreState)
{
    typename RecordIteratorMap::iterator pos;
    LocationList locations;

    if (!this->is_activated())
    {
        // Not the primary
        throw CdmwFT::StateTransfer::InvalidDataStoreState();
    }

    pos = m_iter_map.find(from_location);

    if (pos != m_iter_map.end ())
    {
        // Iterator already exists for this location
        throw CORBA::BAD_INV_ORDER(Cdmw::OrbSupport::BAD_INV_ORDERIteratorPresent,
                                   CORBA::COMPLETED_NO);
    }

    locations = m_context.get_locations(this->get_dsid());

    if (locations.find(from_location) != locations.end())
    {
        // DataStore already exists for this location
        throw CORBA::BAD_INV_ORDER(Cdmw::OrbSupport::BAD_INV_ORDERLocationAlreadyPresent,
                                   CORBA::COMPLETED_NO);
    }

    CDMW_READER_LOCK_GUARD(m_dataMapMutex);

    DataStoreRecordIterator* iter_impl =
       new DataStoreRecordIterator(*this, m_orb.in(), from_location);
    m_iter_map[from_location] = iter_impl;

    return iter_impl;
}

template <class OID, class DATA, class OIDCOMPARE>
void
DataStore<OID, DATA, OIDCOMPARE>::iterator_finished
(const std::string& location)
    throw (AlreadyExistException, NotFoundException)
{
    m_context.add_location(this->get_dsid(), location);
    m_iter_map.erase(location);
}

template <class OID, class DATA, class OIDCOMPARE>
int DataStore<OID, DATA, OIDCOMPARE>::handle_timeout(const void*)
{
    this->invoke_timeouts();
    return 0;
}

template <class OID, class DATA, class OIDCOMPARE>
bool DataStore<OID, DATA, OIDCOMPARE>::contains(_Oid_in_type oid) throw ()
{
    CDMW_READER_LOCK_GUARD(m_dataMapMutex);
    return (m_data.find(oid) != m_data.end());
}



template <class OID, class DATA, class OIDCOMPARE>
typename DataStore<OID, DATA, OIDCOMPARE>::_Data_retn_type
DataStore<OID, DATA, OIDCOMPARE>::select(const _Oid_in_type oid) 
    throw (NotFoundException)
{
    CDMW_READER_LOCK_GUARD(m_dataMapMutex);
    DataMapConstIterator itData = m_data.find(oid);
    if (itData != m_data.end())
    {
        // make deep copy
        _Data_var_type result = itData->second;
        // return result
        return Cdmw::OrbSupport::type_traits<DATA>::_retn(result);
    }
    else
    {
        throw NotFoundException("The Oid does not exist");
    }
}


template <class OID, class DATA, class OIDCOMPARE>
void DataStore<OID, DATA, OIDCOMPARE>::remove(const _Oid_in_type oid) 
    throw (NotFoundException,Cdmw::CommonSvcs::DataStore::StorageErrorException)
{
    freeze_check();

    if (m_context.get_locations(this->get_dsid()).empty())
    {
        remove_local(oid);
    }
    else
    {
        CDMW_ASSERT(m_is_activated);

        if (m_is_activated)
        {
            CdmwFT::Transaction::StateChanges changes;

            changes.length(1);
            changes[0].dsid = this->get_dsid();
            changes[0].records.length(1);

            // remove in cohorts
            CORBA::OctetSeq_var oid_seq_var = m_oid_codec.encode(oid);
            changes[0].records[0].oid_seq = oid_seq_var;
            changes[0].records[0].change_data._d
                (CdmwFT::Transaction::DS_REMOVAL);
            
            bool doLocal = true;

            try
            {
                doLocal = m_context.request(changes);
            }
            catch (const ValidationFailureException&)
            {
                throw NotFoundException();
            }

            if (doLocal)
            {
                remove_local(oid);
            } else {
                throw Cdmw::CommonSvcs::DataStore::StorageErrorException("TX Rollback by a Cohort!");
            }
        }
    }
}

template <class OID, class DATA, class OIDCOMPARE>
void DataStore<OID, DATA, OIDCOMPARE>::remove_local(const _Oid_in_type oid)
    throw (NotFoundException)
{
    DataStoreObserverList observers;
    DataStoreObserverListIterator pos;

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

        {
            CDMW_READER_LOCK_GUARD(m_backup_observers_mutex);
            observers = m_backup_observers;
        }

        for (pos = observers.begin(); pos != observers.end(); ++pos)
        {
            (*pos)->on_remove(oid);
        }
    }

    {
        CDMW_READER_LOCK_GUARD(m_observers_mutex);
        observers = m_observers;
    }

    for (pos = observers.begin(); pos != observers.end(); ++pos)
    {
        (*pos)->on_remove(oid);
    }
}

template <class OID, class DATA, class OIDCOMPARE>
void DataStore<OID, DATA, OIDCOMPARE>::insert(const _Oid_in_type oid,
                                              const _Data_in_type data) 
    throw (AlreadyExistException,Cdmw::CommonSvcs::DataStore::StorageErrorException)
{
    freeze_check();

    if (m_context.get_locations(this->get_dsid()).empty())
    {
        insert_local(oid, data);
    }
    else
    {
        CDMW_ASSERT(m_is_activated);

        if (m_is_activated)
        {
            CdmwFT::Transaction::StateChanges changes;

            changes.length(1);
            changes[0].dsid = this->get_dsid();
            changes[0].records.length(1);

            // insert in cohorts
            CORBA::OctetSeq_var oid_seq_var = m_oid_codec.encode(oid);
            changes[0].records[0].oid_seq = oid_seq_var;

            CORBA::OctetSeq_var data_seq_var = m_data_codec.encode(data);
            changes[0].records[0].change_data.data_seq(data_seq_var.in());
            changes[0].records[0].change_data._d
                (CdmwFT::Transaction::DS_INSERTION);

            bool doLocal = true;
    
            try
            {
                doLocal = m_context.request(changes);
            }
            catch (const ValidationFailureException&)
            {
                throw AlreadyExistException();
            }

            if (doLocal)
            {
                insert_local(oid, data);
            } else {
                throw Cdmw::CommonSvcs::DataStore::StorageErrorException("TX Rollback by a Cohort!");
            }
        }
    }
}

template <class OID, class DATA, class OIDCOMPARE>
void  DataStore<OID, DATA, OIDCOMPARE>::insert_local(const _Oid_in_type oid,
                                                     const _Data_in_type data) 
    throw (AlreadyExistException)
{
    DataStoreObserverList observers;
    DataStoreObserverListIterator pos;

    {
        CDMW_WRITER_LOCK_GUARD(m_dataMapMutex);

        DataMapConstIterator itData = m_data.find(oid);
        if (itData == m_data.end())
        {
            // store copy of data
            m_data[oid] = Cdmw::OrbSupport::type_traits<DATA>::_copy(data);
        }
        else
        {
            throw AlreadyExistException("DataStore: The oid already exist");
        }

        {
            CDMW_READER_LOCK_GUARD(m_backup_observers_mutex);
            observers = m_backup_observers;
        }

        for (pos = observers.begin(); pos != observers.end(); ++pos)
        {
            (*pos)->on_insert(oid, data);
        }
    }

    {
        CDMW_READER_LOCK_GUARD(m_observers_mutex);
        observers = m_observers;
    }

    for (pos = observers.begin(); pos != observers.end(); ++pos)
    {
        (*pos)->on_insert(oid, data);
    }
}

template <class OID, class DATA, class OIDCOMPARE>
void DataStore<OID, DATA, OIDCOMPARE>::update(const _Oid_in_type oid,
                                              const _Data_in_type data) 
    throw (NotFoundException,Cdmw::CommonSvcs::DataStore::StorageErrorException)
{
    freeze_check();

    if (m_context.get_locations(this->get_dsid()).empty())
    {
        update_local(oid, data);
    }
    else
    {
        CDMW_ASSERT(m_is_activated);

        if (m_is_activated)
        {
            CdmwFT::Transaction::StateChanges changes;

            changes.length(1);
            changes[0].dsid = this->get_dsid();
            changes[0].records.length(1);

            // update cohorts
            CORBA::OctetSeq_var oid_seq_var = m_oid_codec.encode(oid);
            changes[0].records[0].oid_seq = oid_seq_var;

            CORBA::OctetSeq_var data_seq_var = m_data_codec.encode(data);
            changes[0].records[0].change_data.data_seq(data_seq_var.in());
            changes[0].records[0].change_data._d
                (CdmwFT::Transaction::DS_UPDATE);

            bool doLocal = true;
            try
            {
                doLocal = m_context.request(changes);
            }
            catch (const ValidationFailureException&)
            {
                throw NotFoundException();
            }

            if (doLocal)
            {
                update_local(oid, data);
            } else {
                throw Cdmw::CommonSvcs::DataStore::StorageErrorException("TX Rollback by a Cohort!");
            }
        }
    }
}

template <class OID, class DATA, class OIDCOMPARE>
void DataStore<OID, DATA, OIDCOMPARE>::update_local(const _Oid_in_type oid,
                                                    const _Data_in_type data) 
    throw (NotFoundException)
{
    DataStoreObserverList observers;
    DataStoreObserverListIterator pos;

    {
        CDMW_WRITER_LOCK_GUARD(m_dataMapMutex);

        DataMapIterator itData = m_data.find(oid);
        if (itData != m_data.end())
        {
            // store copy of data
            itData->second = Cdmw::OrbSupport::type_traits<DATA>::_copy(data);
        }
        else
        {
            throw NotFoundException("DataStore: The Oid does not exist");
        }

        {
            CDMW_READER_LOCK_GUARD(m_backup_observers_mutex);
            observers = m_backup_observers;
        }

        for (pos = observers.begin(); pos != observers.end(); ++pos)
        {
            (*pos)->on_update(oid, data);
        }
    }

    {
        CDMW_READER_LOCK_GUARD(m_observers_mutex);
        observers = m_observers;
    }

    for (pos = observers.begin(); pos != observers.end(); ++pos)
    {
        (*pos)->on_update(oid, data);
    }
}

template <class OID, class DATA, class OIDCOMPARE>
void DataStore<OID, DATA, OIDCOMPARE>::do_for_each(Cdmw::CommonSvcs::DataStore::Functor<OID,DATA>& f)
{
    CDMW_READER_LOCK_GUARD(m_dataMapMutex);

    DataMapConstIterator itBegin = m_data.begin();
    DataMapConstIterator itEnd = m_data.end();
    // Security: we force call of std::for_each with const iterators
    Cdmw::CommonSvcs::DataStore::FunctorWrapper<OID,DATA> f__(f);
    std::for_each(itBegin, itEnd, f__);
}

template <class OID, class DATA, class OIDCOMPARE>
void DataStore<OID, DATA, OIDCOMPARE>::init(bool cold_restart) 
   throw (Cdmw::InitErrorException) 
{
    CDMW_WRITER_LOCK_GUARD(m_dataMapMutex);
    m_timer.start();

    if (m_pInitialiser)
    {
        DataInsertIterator iter(m_data, m_data.begin());

        m_pInitialiser->on_init(iter, cold_restart);
    }
}

template <class OID, class DATA, class OIDCOMPARE>
void DataStore<OID, DATA, OIDCOMPARE>::activate()
{
    // if the datastore is already activate, do nothing
    if (m_is_activated)
        return;
    m_is_activated = true;

    m_context.activate();
}

template <class OID, class DATA, class OIDCOMPARE>
void DataStore<OID, DATA, OIDCOMPARE>::deactivate()
{
    // if the datastore is already deactivate, do nothing
    if (!m_is_activated)
        return;

    m_is_activated = false;

    m_context.deactivate();
}

template <class OID, class DATA, class OIDCOMPARE>
void
DataStore<OID, DATA, OIDCOMPARE>::load_state(const StateChangeRecords& recs)
    throw (NotFoundException, AlreadyExistException)
{
    for (CORBA::ULong i = 0; i < recs.length(); i++)
    {
        apply_change(recs[i]);
    }
}


template <class OID, class DATA, class OIDCOMPARE>
bool
DataStore<OID, DATA, OIDCOMPARE>::validate(const StateChangeRecords& recs)
{
    CDMW_READER_LOCK_GUARD(m_dataMapMutex);
    for (CORBA::ULong i = 0; i < recs.length(); i++)
    {
        _Oid_var_type oid = Cdmw::OrbSupport::type_traits<OID>::_default_value();

        m_oid_codec.decode(recs[i].oid_seq,
                           Cdmw::OrbSupport::type_traits<OID>::_inout(oid));
        DataMapConstIterator itData = m_data.find(oid);
        bool oid_present = (itData != m_data.end());

        switch(recs[i].change_data._d())
        {
            case CdmwFT::Transaction::DS_INSERTION:
            {
                if (oid_present)
                {
                    return false;
                }
                break;
            }
            case CdmwFT::Transaction::DS_UPDATE:
            case CdmwFT::Transaction::DS_REMOVAL:
            {
                if (!oid_present)
                {
                    return false;
                }
                break;
            }
        }
    }

    return true;
}


template <class OID, class DATA, class OIDCOMPARE>
bool
DataStore<OID, DATA, OIDCOMPARE>::oids_match
(const CdmwFT::Transaction::StateChangeRecords& recs1,
 const CdmwFT::Transaction::StateChangeRecords& recs2)
{
    typedef std::set<_Oid_var_type, OIDCOMPARE> OidSet;
    OidSet set1;

    for (CORBA::ULong i = 0; i < recs1.length(); i++)
    {
        _Oid_var_type oid1 =
            Cdmw::OrbSupport::type_traits<OID>::_default_value();
        m_oid_codec.decode(recs1[i].oid_seq,
                           Cdmw::OrbSupport::type_traits<OID>::_inout(oid1));
        set1.insert(oid1);
    }

    for (CORBA::ULong j = 0; j < recs2.length(); j++)
    {
        _Oid_var_type oid2 =
            Cdmw::OrbSupport::type_traits<OID>::_default_value();
        m_oid_codec.decode(recs2[j].oid_seq,
                           Cdmw::OrbSupport::type_traits<OID>::_inout(oid2));

        if (set1.find(oid2) != set1.end())
        {
            return true;
        }
    }

    return false;
}


template <class OID, class DATA, class OIDCOMPARE>
void
DataStore<OID, DATA, OIDCOMPARE>::apply_change(const StateChangeRecord& rec)
    throw (NotFoundException, AlreadyExistException)
{
    _Oid_var_type oid = Cdmw::OrbSupport::type_traits<OID>::_default_value();

    m_oid_codec.decode(rec.oid_seq,
                       Cdmw::OrbSupport::type_traits<OID>::_inout(oid));

    switch(rec.change_data._d())
    {
        case CdmwFT::Transaction::DS_INSERTION:
        {
            _Data_var_type data =
                Cdmw::OrbSupport::type_traits<DATA>::_default_value();
            m_data_codec.decode(rec.change_data.data_seq(),
                                Cdmw::OrbSupport::type_traits<DATA>::_inout(data));
            insert_local(Cdmw::OrbSupport::type_traits<OID>::_in(oid), Cdmw::OrbSupport::type_traits<DATA>::_in(data));
            break;
        }
        case CdmwFT::Transaction::DS_UPDATE:
        {
            _Data_var_type data =
                Cdmw::OrbSupport::type_traits<DATA>::_default_value();
            m_data_codec.decode(rec.change_data.data_seq(),
                                Cdmw::OrbSupport::type_traits<DATA>::_inout(data));
            update_local(Cdmw::OrbSupport::type_traits<OID>::_in(oid), Cdmw::OrbSupport::type_traits<DATA>::_in(data));
            break;
        }
        case CdmwFT::Transaction::DS_REMOVAL:
        {
            remove_local(Cdmw::OrbSupport::type_traits<OID>::_in(oid));
            break;
        }
    }
}

template <class OID, class DATA, class OIDCOMPARE>
void DataStore<OID, DATA, OIDCOMPARE>::freeze(const std::string& location)
{
    CDMW_MUTEX_GUARD(m_mutex);

    CDMW_DEBUG(FTLogger::GetLogger(),"DataStore["<<this->get_dsid()<<"] freeze invoked for location <"
               << location << ">");
    m_freeze_locations.insert(location);
}

template <class OID, class DATA, class OIDCOMPARE>
void DataStore<OID, DATA, OIDCOMPARE>::unfreeze(const std::string& location)
{
    CDMW_MUTEX_GUARD(m_mutex);

    CDMW_DEBUG(FTLogger::GetLogger(),"DataStore["<<this->get_dsid()<<"] unfreeze invoked for location <"
               << location << ">");
    m_freeze_locations.erase(location);

    if (m_freeze_locations.empty())
    {
        m_condition.broadcast();
    }
}

template <class OID, class DATA, class OIDCOMPARE>
void DataStore<OID, DATA, OIDCOMPARE>::wait_for_no_transactions_in_progress()
{
    if ((m_context.get_locations(this->get_dsid()).size() > 0)
        && m_is_activated)
    {
        m_context.wait_for_no_transactions_in_progress();
    }
}

template <class OID, class DATA, class OIDCOMPARE>
void DataStore<OID, DATA, OIDCOMPARE>::freeze_check()
{
    CDMW_MUTEX_GUARD(m_mutex);

    if (!m_freeze_locations.empty())
    {
        CDMW_DEBUG(FTLogger::GetLogger(),"DataStore["<<this->get_dsid()<<"] About to freeze");
        // Set up a timeout
        TimerId timer_id = m_timer.start_timer(this, 0, m_freeze_timeout);

        do
        {
            m_condition.wait();
        } while (!m_freeze_locations.empty());

        m_timer.cancel_timer(timer_id);
    }
}

template <class OID, class DATA, class OIDCOMPARE>
void DataStore<OID, DATA, OIDCOMPARE>::invoke_timeouts()
{
    CDMW_MUTEX_GUARD(m_mutex);
    typename LocationList::iterator pos;

    CDMW_DEBUG(FTLogger::GetLogger(),"DataStore["<<this->get_dsid()<<"] In invoke_timeouts");
    for (pos = m_freeze_locations.begin(); pos != m_freeze_locations.end();
         ++pos)
    {
        CDMW_WARN(FTLogger::GetLogger(),"DataStore["<<this->get_dsid()<<"] [WARNING] Location <" 
                  << Get_Location_Name() <<"> Calling Timeout for location " << *pos);
        DataStoreBase::Timeout(*pos);
        CDMW_DEBUG(FTLogger::GetLogger(),"DataStore["<<this->get_dsid()<<"] Cleaning up iterator");
        m_iter_map[*pos]->cleanup();
        CDMW_DEBUG(FTLogger::GetLogger(),"DataStore["<<this->get_dsid()<<"] After cleaning up iterator");
        m_iter_map.erase(*pos);
        CDMW_DEBUG(FTLogger::GetLogger(),"DataStore["<<this->get_dsid()<<"] After erase");
    }

    CDMW_DEBUG(FTLogger::GetLogger(),"DataStore["<<this->get_dsid()<<"] Timeouts triggered");
    m_freeze_locations.clear();
    m_condition.broadcast();
}

}  // End namespace FT
}  // End namespace Cdmw

#endif

