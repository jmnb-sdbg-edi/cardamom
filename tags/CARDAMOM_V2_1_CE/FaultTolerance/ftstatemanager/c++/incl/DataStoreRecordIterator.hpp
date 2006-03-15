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

#ifndef INCL_CDMW_FT_FTSTATEMANAGER_DATASTORE_RECORD_ITERATOR_HPP
#define INCL_CDMW_FT_FTSTATEMANAGER_DATASTORE_RECORD_ITERATOR_HPP

#include <FaultTolerance/ftstatemanager/DataStore.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreRecordIteratorBase.hpp>
#include <Foundation/commonsvcs/datastore/DataStoreObserver.hpp>
#include <Foundation/osthreads/Mutex.hpp>
#include <Foundation/osthreads/MutexGuard.hpp>

#include <idllib/CdmwFTStateTransfer.skel.hpp>

#include <list>

namespace Cdmw
{
namespace FT
{

template <class OID, class DATA, class OIDCOMPARE=std::less<OID> >
class DataStoreRecordIterator:
        public DataStoreRecordIteratorBase,
        public Cdmw::CommonSvcs::DataStore::DataStoreObserver<OID, DATA>
{
    typedef DataStore<OID, DATA, OIDCOMPARE> DataStore;
    typedef typename DataStore::_Oid_var_type _Oid_var_type;
    typedef typename DataStore::_Data_var_type _Data_var_type;
    typedef typename DataStore::DataMap DataMap;
    typedef typename DataMap::value_type value_type;
    typedef CdmwFT::Transaction::StateChangeKind StateChangeKind;
    typedef std::map<_Oid_var_type, StateChangeKind, OIDCOMPARE> ChangeMap;
    typedef typename ChangeMap::iterator ChangeMapIterator;
    typedef typename ChangeMap::value_type ChangeMapValueType;
    typedef std::set<_Oid_var_type, OIDCOMPARE> OidSet;
    typedef typename OidSet::iterator OidSetIterator;
    typedef Cdmw::CommonSvcs::DataStore::DataStoreObserver<OID, DATA> DataStoreObserver;
    typedef typename DataStoreObserver::_Oid_in_type _Oid_in_type;
    typedef typename DataStoreObserver::_Data_in_type _Data_in_type;
    typedef typename CdmwFT::Transaction::StateChangeRecord StateChangeRecord;
    typedef typename CdmwFT::Transaction::StateChangeRecords StateChangeRecords;
    typedef Cdmw::OrbSupport::Codec<OID> OidCodec;
    typedef Cdmw::OrbSupport::Codec<DATA> DataCodec;
    typedef Cdmw::OsSupport::MutexGuard MutexGuard;

    struct Populator: public std::unary_function<value_type, void>
    {
        Populator(ChangeMap& map) : m_map(map) {}

        void operator()(const value_type& elt)
        {
            m_map.insert(ChangeMap::value_type(elt.first, CdmwFT::Transaction::DS_INSERTION));
        }

        ChangeMap& m_map;
    };

public:

    /**
     * Constructor for DataStoreRecordIterator. This should be
     * instantiated with a read lock obtained on the DataStore
     * since the DataStore must not change between the population
     * of the internal map and the registration of the observer.
     */
    DataStoreRecordIterator(DataStore& ds,
                            CORBA::ORB_ptr orb,
                            const std::string& location)
       :  m_datastore(ds),
          m_location(location),
          m_is_destroyed(false),
          m_oid_codec(orb),
          m_data_codec(orb)
    {
        // Populate m_map with insertion records for current DataStore state
        ds.for_each(Populator(m_map));
        // register a callback to track changes to the DataStore
        ds.register_backup_observer(this);
    }

    ~DataStoreRecordIterator() throw()
    {
    }

    /**
     * Obtains at most the requested number of StateChangeRecords.
     *
     * @param how_many the maximum number of records to return.
     *
     * @param changes the StateChangeRecords.
     *
     * @return number of records still available
     */
    CORBA::ULong next_n (CORBA::ULong how_many,
                         CdmwFT::Transaction::StateChangeRecords_out changes)
        throw (CORBA::SystemException)
    {
        std::list<ChangeMapValueType> records;
        ChangeMapIterator pos;
        bool empty;

        {
            CDMW_MUTEX_GUARD(m_mutex);

            if (m_is_destroyed)
            {
               throw CORBA::BAD_INV_ORDER(Cdmw::OrbSupport::BAD_INV_ORDERIteratorDestroyed,
                                          CORBA::COMPLETED_NO);
            }

            // Copy and remove at most how_many elements from the
            // iterator's internal map. The copying should be much
            // faster than the encoding.
            if (!m_map.empty())
            {
                pos = m_map.begin();
                std::advance(pos, std::min(m_map.size(), how_many));
                std::copy(m_map.begin(), pos, std::back_inserter(records));
                m_map.erase(m_map.begin(), pos);
            }

            // If the records size is less than the requested number of records
            // then add update Oids from recently changed updates
            if (!m_updates.empty() && (records.size() < how_many))
            {
                OidSetIterator iter = m_updates.begin();

                while ((iter != m_updates.end())
                       && (records.size() < how_many))
                {
                    records.push_back
                        (ChangeMapValueType(*iter,
                                            CdmwFT::Transaction::DS_UPDATE));
                    ++iter;
                }

                m_updates.erase(m_updates.begin(), iter);
            }
        }

        // The lock in not being held while encoding occurs
        changes = new CdmwFT::Transaction::StateChangeRecords;
        changes->length(records.size());

        typename std::list<ChangeMapValueType>::iterator iter;
        CORBA::ULong i = 0;

        for (iter = records.begin(); iter != records.end(); ++iter)
        {
            StateChangeKind kind = iter->second;

            CORBA::OctetSeq_var oid_seq_var =  
                m_oid_codec.encode(Cdmw::OrbSupport::type_traits<OID>::_in(iter->first));
            (*changes)[i].oid_seq = oid_seq_var;

            if (kind != CdmwFT::Transaction::DS_REMOVAL)
            {
                _Data_var_type val =
                    m_datastore.select(Cdmw::OrbSupport::type_traits<OID>::_in(iter->first));
                CORBA::OctetSeq_var data_seq_var =
                    m_data_codec.encode(Cdmw::OrbSupport::type_traits<DATA>::_in(val));
                (*changes)[i].change_data.data_seq(data_seq_var.in());
            }

            (*changes)[i].change_data._d(kind);
            ++i;
        }

        // Is there more state to transfer?
        {
            CDMW_MUTEX_GUARD(m_mutex);
            empty = m_map.empty() && m_updates.empty();
        }

        if (empty)
        {
            // Need to ensure there are no outstanding transactions
            m_datastore.wait_for_no_transactions_in_progress();

            // At this point all transactions in progress have either
            // committed or rolled back. Any commits will have invoked
            // the callbacks.
            {
                CDMW_MUTEX_GUARD(m_mutex);
                empty = m_map.empty() && m_updates.empty();
            }
        }

        return m_map.size() + m_updates.size();
    }

    void destroy ()
        throw (CORBA::SystemException)
    {
        CDMW_MUTEX_GUARD(m_mutex);
        bool already_exists = false;
        bool not_found = false;

        if (m_is_destroyed)
        {
            // already deleted
            return;
        }

        try
        {
            // Allow the location to be added
            m_datastore.iterator_finished(m_location);
        }
        catch (const AlreadyExistException&)
        {
            // delay exception reporting until after clean up
            already_exists = true;
        }
        catch (const NotFoundException&)
        {
            // delay exception reporting until after clean up
            not_found = true;
        }

        cleanup();

        if (already_exists)
        {
            throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLocationAlreadyPresent,
                                  CORBA::COMPLETED_NO);
        }

        if (not_found)
        {
            throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALDataStoreNotFound,
                                  CORBA::COMPLETED_NO);
        }
    }

    // The callback processing needs to work out the minimum needed to
    // be sent as simply sending each state change in sequence could
    // be inefficient. The following table can be used to understand
    // the logic. A map holds the kind of change that needs to be
    // transferred. As each change is transferred it is removed from
    // the map. The table entry is the new state that applies when a
    // callback occurs given the existing state represented by the
    // columns.
    //
    //                  Existing map entry
    //           None     Insert  Update  Remove
    // C
    // a Insert  Insert   Illegal Illegal Update
    // l
    // l
    // b Update  Update   Insert  Update  Illegal
    // a
    // c
    // k Remove  Remove   delete  Remove  Illegal
    //
    // where delete means delete map entry. Illegal actions are assumed
    // not to happen.

    void on_insert(_Oid_in_type oid, _Data_in_type data)
        throw ()
    {
        CDMW_MUTEX_GUARD(m_mutex);
 
        ChangeMapIterator pos = m_map.find(oid);

        if (pos != m_map.end())
        {
            m_map.erase(pos);
            m_updates.insert(oid);
        }
        else
        {
            m_map[oid] = CdmwFT::Transaction::DS_INSERTION;
        }
    }

    void on_update(_Oid_in_type oid, _Data_in_type data)
        throw ()
    {
        CDMW_MUTEX_GUARD(m_mutex);

        ChangeMapIterator pos = m_map.find(oid);

        if (pos == m_map.end())
        {
            m_updates.insert(oid);
        }
        // else, nothing to do.
        // (oid was not sent, keep it as an insert in m_map)
    }

    void on_remove(_Oid_in_type oid)
        throw ()
    {
        CDMW_MUTEX_GUARD(m_mutex);

        ChangeMapIterator pos = m_map.find(oid);

        if ((pos != m_map.end())
            && (pos->second == CdmwFT::Transaction::DS_INSERTION))
        {
            m_map.erase(pos);
        }
        else
        {
            // might be a recent update so try and erase it
            m_updates.erase(oid);
            m_map[oid] = CdmwFT::Transaction::DS_REMOVAL;
        }
    }

    void cleanup()
    {
        m_datastore.unregister_backup_observer(this);
        m_is_destroyed = true;
    }

private:
    // Underlyng DataStore
    DataStore& m_datastore;

    // Map of oid to update type (insert, update or remove)
    ChangeMap m_map;

    // Set of oids that have multiple updates
    OidSet m_updates;

    // Location being added
    std::string m_location;

    // Flag set if destroy has been called
    bool m_is_destroyed;

    // Mutex for concurrency guard
    Cdmw::OsSupport::Mutex m_mutex;

    // POA for this object
    PortableServer::POA_var m_poa;

    // Codec for creating octet squence for an oid
    OidCodec m_oid_codec;

    // Codec forxs creating octet squence for data instance
    DataCodec m_data_codec;
};

}  // End namespace FT
}  // End namespace Cdmw

#endif // INCL_CDMW_FT_FTSTATEMANAGER_DATASTORE_RECORD_ITERATOR_HPP
