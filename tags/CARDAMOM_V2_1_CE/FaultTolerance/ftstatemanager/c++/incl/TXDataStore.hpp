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

#ifndef INCL_CDMW_FT_FTSTATEMANAGER_TXDATASTORE_HPP
#define INCL_CDMW_FT_FTSTATEMANAGER_TXDATASTORE_HPP

#include <FaultTolerance/ftstatemanager/DataStore.hpp>
#include <FaultTolerance/ftstatemanager/TXDataStoreBase.hpp>

namespace Cdmw {
namespace FT {

template <class OID, class DATA, class OIDCOMPARE=std::less<OID> > 
class TXDataStore:
            public Cdmw::CommonSvcs::DataStore::DataStoreInterface<OID, DATA, OIDCOMPARE>,
            public TXDataStoreBase
{
    typedef DataStore<OID, DATA, OIDCOMPARE> DataStore;
    typedef typename DataStore::DataMap DataMap;
    typedef CdmwFT::Transaction::StateChangeKind StateChangeKind;
    typedef typename Cdmw::CommonSvcs::DataStore::DataStoreObserver<OID, DATA> DataStoreObserver;

public:
    typedef typename DataStore::_Oid_var_type  _Oid_var_type;
    typedef typename DataStore::_Oid_in_type   _Oid_in_type;
    typedef typename DataStore::_Oid_retn_type _Oid_retn_type;

    typedef typename DataStore::_Data_var_type  _Data_var_type;
    typedef typename DataStore::_Data_in_type   _Data_in_type;
    typedef typename DataStore::_Data_retn_type _Data_retn_type;

    typedef Cdmw::CommonSvcs::DataStore::Functor<OID, DATA> Functor;

    typedef std::map<_Oid_var_type, StateChangeKind, OIDCOMPARE> ChangeMap;

    typedef Cdmw::OrbSupport::Codec<OID> OidCodec;
    typedef Cdmw::OrbSupport::Codec<DATA> DataCodec;

    TXDataStore(TXManager&              manager,
                DsIdentifier            dsId,
                CORBA::ORB_ptr          orb,
                PortableServer::POA_ptr poa,
                CORBA::ULong            freeze_threshold = DEFAULT_FREEZE_THRESHOLD,
                CORBA::ULong            iterator_increment = DEFAULT_ITERATOR_INCREMENT,
                OsSupport::OS::Timeval  freeze_timeout = DEFAULT_FREEZE_TIMEOUT)
        throw (AlreadyExistException, CORBA::SystemException);

    virtual ~TXDataStore();

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
    select(const _Oid_in_type oid) throw (NotFoundException);

    /**
     * execute function for each element of DataStore
     * NOTE: The implementation does not autorise the given function to modify the data
     *       Only Function with const parameter will compile
     *       (typical example: const std::pair<Oid, Data>& is OK)
     *
     *@param Function f to execute
     */
    void do_for_each(Functor& f);

    /**
     * Remove from the DataStore the entry with the given oid key
     * Do it in the cohorts then localy (if no problem with cohorts)
     *
     * This operation will potentially block until updating replicas in the absence of flat transaction
     *
     *@param oid is the Object Id key
     */
    void remove(const _Oid_in_type oid) throw (NotFoundException,
                                               Cdmw::CommonSvcs::DataStore::StorageErrorException);
    
    /**
     * insert in the DataStore the entry with the given oid key
     * do it in the cohorts then localy (if no problem with cohorts)
     *
     * This operation will potentially block until updating replicas in the absence of flat transaction
     *
     *@param oid is the Object Id key
     *@param data is the data associated with Oid key
     */
    void insert(const _Oid_in_type oid, const _Data_in_type data)
        throw (AlreadyExistException,Cdmw::CommonSvcs::DataStore::StorageErrorException);

    /**
     * update the DataStore entry with the given oid key
     * do it in the cohorts then localy (if no problem with cohorts)
     *
     * This operation will potentially block until updating replicas in the absence of flat transaction
     *
     *@param oid is the Object Id key
     *@param data is the data associated with Oid key
     */
    void update(const _Oid_in_type oid, const _Data_in_type data)
        throw (NotFoundException,Cdmw::CommonSvcs::DataStore::StorageErrorException);

    /**
     * Give the size of the DataStore
     *
     *@return int the size
     */
    int size();

    /**
     * Registers an observer to be notified whenever there is an insertion, update, or removal
     * of a record.
     *
     *@param observer a const pointer to a DataStoreObserver object
     *@return true if the Data Store observer is registered, false if failed to register it
     * (it has already been registered, or exceeded maximum allowed number of observers ...)
     */
     bool register_observer(DataStoreObserver * observer) throw();

    /**
     * Un-registers an already registered observer. The observer will no longer be notified by the datastore.
     *
     *@param observer a const pointer to the DataStoreObserver object
     *@return true if the Data Store observer is unregistered, false if the observer was not previously registered
     */
    bool unregister_observer(DataStoreObserver * observer) throw();

    /**
     * Return the TXManager associated with this TXDataStore.
     *
     * @return the TXManager
     */
     TXManager& get_manager();

    /**
     * Return underlying DataStore.
     *
     * @return DataStore.
     */
    DataStore& get_datastore();

    /**
     * Obtain the set of changes associated with this transaction.
     *
     * @param change the changes
     */
    void get_change(::CdmwFT::Transaction::StateChange& change);

protected:
    /**
     * Clears the cache of uncommited changes.
     */
    virtual void clear_cache();

    /**
     * Applys cached changes locally.
     */
    virtual void flush();

public:
    struct TXDataStoreContext
    {
        ChangeMap m_change_map;
        DataMap m_data_map;
        int m_size_adjustment;

        TXDataStoreContext() : m_size_adjustment(0) {}
    };

private:
    typedef std::map<DsIdentifier, TXDataStoreContext> TXDataStoreContextMap;

    TXManager& m_manager;

    TXDataStoreContext* get_context();

    DataStore m_datastore;

    ACE_TSS<TXDataStoreContextMap> *m_tss_map;

    // Codec for creating octet squence for an oid
    OidCodec m_oid_codec;

    // Codec forxs creating octet squence for data instance
    DataCodec m_data_codec;
};

    template <class OID, class DATA, class OIDCOMPARE=std::less<OID> > 
    class AdjustFunc: public Cdmw::CommonSvcs::DataStore::Functor<OID,DATA>
    {
        public:
            typedef typename TXDataStore<OID, DATA, OIDCOMPARE>::TXDataStoreContext TXDataStoreContext;
            typedef typename TXDataStore<OID, DATA, OIDCOMPARE>::ChangeMap ChangeMap;

            AdjustFunc(TXDataStoreContext* ctx, Cdmw::CommonSvcs::DataStore::Functor<OID,DATA>& f)
                : m_ctx(ctx),
                  m_func(f)
            {
            }

            void operator()(const std::pair<typename Cdmw::CommonSvcs::DataStore::Functor<OID,DATA>::_Oid_var_type , typename Cdmw::CommonSvcs::DataStore::Functor<OID,DATA>::_Data_var_type > & elt)
            {
                typename ChangeMap::iterator iter =
                    m_ctx->m_change_map.find(elt.first);

                if (iter != m_ctx->m_change_map.end())
                {
                    // Apply updates, ignore removes
                    if (iter->second == CdmwFT::Transaction::DS_UPDATE)
                    {
                        std::pair<typename Cdmw::CommonSvcs::DataStore::Functor<OID,DATA>::_Oid_var_type , typename Cdmw::CommonSvcs::DataStore::Functor<OID,DATA>::_Data_var_type >
                            val(iter->first, m_ctx->m_data_map[iter->first]);

                        m_func(val);
                    }
                }
                else
                {
                    m_func(elt);
                }
            }

            TXDataStoreContext* m_ctx;
            Cdmw::CommonSvcs::DataStore::Functor<OID,DATA>& m_func;
    };

template <class OID, class DATA, class OIDCOMPARE>
TXDataStore<OID, DATA, OIDCOMPARE>::TXDataStore
(TXManager&              manager,
 DsIdentifier            dsId,
 CORBA::ORB_ptr          orb,
 PortableServer::POA_ptr poa,
 CORBA::ULong            freeze_threshold,
 CORBA::ULong            iterator_increment,
 OsSupport::OS::Timeval  freeze_timeout)
    throw (AlreadyExistException, CORBA::SystemException)
    : TXDataStoreBase(dsId),
      m_manager(manager),
      m_datastore(dsId, orb, poa, manager.get_context(), freeze_threshold,
                  iterator_increment, freeze_timeout, false),
      m_oid_codec(orb),
      m_data_codec(orb)
{
    m_tss_map = new ACE_TSS<TXDataStoreContextMap>;
    TXDataStoreBase::Register_datastore(this);
    Cdmw::CommonSvcs::DataStore::DataStoreRegistry::Register_data_store(this);
}

template <class OID, class DATA, class OIDCOMPARE>
TXDataStore<OID, DATA, OIDCOMPARE>::~TXDataStore()
{
    TXDataStoreBase::Unregister_datastore(this);

    try
    {
        Cdmw::CommonSvcs::DataStore::DataStoreRegistry::
            Unregister_data_store(this->get_dsid());
    }
    catch (const NotExistException&)
    {
        // Ignore
    }

    delete m_tss_map;
}

template <class OID, class DATA, class OIDCOMPARE>
bool TXDataStore<OID, DATA, OIDCOMPARE>::contains(_Oid_in_type oid) throw()
{
    TXDataStoreContext* ctx = get_context();
    bool result = false;

    if (ctx)
    {
        typename ChangeMap::iterator iter = ctx->m_change_map.find(oid);

        if (iter != ctx->m_change_map.end ())
        {
            result = (iter->second != CdmwFT::Transaction::DS_REMOVAL);
        }
        else
        {
            result = m_datastore.contains(oid);
        }
    }
    else
    {
        result = m_datastore.contains(oid);
    }

    return result;
}

template <class OID, class DATA, class OIDCOMPARE>
typename DataStore<OID, DATA, OIDCOMPARE>::_Data_retn_type 
TXDataStore<OID, DATA, OIDCOMPARE>::select(const _Oid_in_type oid) 
    throw (NotFoundException)
{
    TXDataStoreContext* ctx = get_context();

    if (ctx)
    {
        typename ChangeMap::iterator iter = ctx->m_change_map.find(oid);

        if (iter != ctx->m_change_map.end ())
        {
            if (iter->second != CdmwFT::Transaction::DS_REMOVAL)
            {
                return Cdmw::OrbSupport::type_traits<DATA>::_retn(ctx->m_data_map[oid]);
            }
            else
            {
                throw NotFoundException("The Oid does not exist");
            }
        }
        else
        {
            return m_datastore.select(oid);
        }
    }
    else
    {
        return m_datastore.select(oid);
    }
}

template <class OID, class DATA, class OIDCOMPARE>
void TXDataStore<OID, DATA, OIDCOMPARE>::insert(const _Oid_in_type oid,
                                                const _Data_in_type data) 
    throw (AlreadyExistException,Cdmw::CommonSvcs::DataStore::StorageErrorException)
{
    TXDataStoreContext* ctx = get_context();

    if (ctx)
    {
        typename ChangeMap::iterator iter = ctx->m_change_map.find(oid);

        if (iter != ctx->m_change_map.end ())
        {
            switch (iter->second)
            {
                case CdmwFT::Transaction::DS_INSERTION:
                case CdmwFT::Transaction::DS_UPDATE:
                {
                    throw AlreadyExistException
                        ("DataStore: The oid already exists");
                    break;
                }
                case CdmwFT::Transaction::DS_REMOVAL:
                {
                    ctx->m_change_map[oid] = CdmwFT::Transaction::DS_UPDATE;
                    ctx->m_data_map[oid] = Cdmw::OrbSupport::type_traits<DATA>::_copy(data);
                    break;
                }
            }
        }
        else if (!m_datastore.contains(oid))
        {
            ctx->m_change_map[oid] = CdmwFT::Transaction::DS_INSERTION;
            ctx->m_data_map[oid] = Cdmw::OrbSupport::type_traits<DATA>::_copy(data);
        }
        else
        {
            throw AlreadyExistException("DataStore: The oid already exists");
        }

        ctx->m_size_adjustment++;
    }
    else
    {
        m_datastore.insert(oid, data);
    }
}

template <class OID, class DATA, class OIDCOMPARE>
void TXDataStore<OID, DATA, OIDCOMPARE>::update(const _Oid_in_type oid,
                                                const _Data_in_type data) 
    throw (NotFoundException,Cdmw::CommonSvcs::DataStore::StorageErrorException)
{
    TXDataStoreContext* ctx = get_context();

    if (ctx)
    {
        typename ChangeMap::iterator iter = ctx->m_change_map.find(oid);

        if (iter != ctx->m_change_map.end ())
        {
            switch (iter->second)
            {
                case CdmwFT::Transaction::DS_INSERTION:
                case CdmwFT::Transaction::DS_UPDATE:
                {
                    ctx->m_data_map[oid] = Cdmw::OrbSupport::type_traits<DATA>::_copy(data);
                    break;
                }
                case CdmwFT::Transaction::DS_REMOVAL:
                {
                    ctx->m_change_map[oid] = CdmwFT::Transaction::DS_UPDATE;
                    ctx->m_data_map[oid] = Cdmw::OrbSupport::type_traits<DATA>::_copy(data);
                    break;
                }
            }
        }
        else if (m_datastore.contains(oid))
        {
            ctx->m_change_map[oid] = CdmwFT::Transaction::DS_UPDATE;
            ctx->m_data_map[oid] = Cdmw::OrbSupport::type_traits<DATA>::_copy(data);
        }
        else
        {
            throw NotFoundException("DataStore: The Oid does not exist");
        }
    }
    else
    {
        m_datastore.update(oid, data);
    }
}

template <class OID, class DATA, class OIDCOMPARE>
void TXDataStore<OID, DATA, OIDCOMPARE>::remove(const _Oid_in_type oid) 
    throw (NotFoundException,Cdmw::CommonSvcs::DataStore::StorageErrorException)
{
    TXDataStoreContext* ctx = get_context();

    if (ctx)
    {
        typename ChangeMap::iterator iter = ctx->m_change_map.find(oid);

        if (iter != ctx->m_change_map.end ())
        {
            switch (iter->second)
            {
                case CdmwFT::Transaction::DS_INSERTION:
                {
                    ctx->m_change_map.erase(iter);
                    break;
                }
                case CdmwFT::Transaction::DS_UPDATE:
                {
                    ctx->m_change_map[oid] = CdmwFT::Transaction::DS_REMOVAL;
                    break;
                }
                case CdmwFT::Transaction::DS_REMOVAL:
                {
                    throw NotFoundException("The Oid does not exist");
                    break;
                }
            }

        }
        else if (m_datastore.contains(oid))
        {
            ctx->m_change_map[oid] = CdmwFT::Transaction::DS_REMOVAL;
        }
        else
        {
            throw NotFoundException("The Oid does not exist");
        }

        ctx->m_size_adjustment--;
    }
    else
    {
        m_datastore.remove(oid);
    }
}

template <class OID, class DATA, class OIDCOMPARE>
void TXDataStore<OID, DATA, OIDCOMPARE>::do_for_each(Functor& f)
{
    TXDataStoreContext* ctx = get_context();

    if (ctx)
    {
        AdjustFunc<OID, DATA, OIDCOMPARE> adj(ctx, f);
        m_datastore.for_each(adj);

        // Apply predicate to inserted elements
        for (typename ChangeMap::iterator iter = ctx->m_change_map.begin();
             iter != ctx->m_change_map.end();
             ++iter)
        {
            if (iter->second == CdmwFT::Transaction::DS_INSERTION)
            {
                typename DataMap::value_type
                    val(iter->first, ctx->m_data_map[iter->first]);

                f(val);
            }
        }
    }
    else
    {
        m_datastore.for_each(Cdmw::CommonSvcs::DataStore::FunctorWrapper<OID, DATA>(f));
    }
}

template <class OID, class DATA, class OIDCOMPARE>
int TXDataStore<OID, DATA, OIDCOMPARE>::size()
{
    TXDataStoreContext* ctx = get_context();

    if (ctx)
    {
        return m_datastore.size() + ctx->m_size_adjustment;
    }
    else
    {
        return m_datastore.size();
    }
}

template <class OID, class DATA, class OIDCOMPARE>
bool
TXDataStore<OID, DATA, OIDCOMPARE>::register_observer(DataStoreObserver * observer) throw()
{
    return m_datastore.register_observer(observer);
}

template <class OID, class DATA, class OIDCOMPARE>
bool
TXDataStore<OID, DATA, OIDCOMPARE>::unregister_observer(DataStoreObserver * observer) throw()
{
    return m_datastore.unregister_observer(observer);
}

template <class OID, class DATA, class OIDCOMPARE>
TXManager& TXDataStore<OID, DATA, OIDCOMPARE>::get_manager()
{
    return m_manager;
}

template <class OID, class DATA, class OIDCOMPARE>
typename TXDataStore<OID, DATA, OIDCOMPARE>::DataStore&
TXDataStore<OID, DATA, OIDCOMPARE>::get_datastore()
{
    return m_datastore;
}

template <class OID, class DATA, class OIDCOMPARE>
void
TXDataStore<OID, DATA, OIDCOMPARE>::get_change
(::CdmwFT::Transaction::StateChange& change)
{
    TXDataStoreContext* ctx = get_context();

    change.dsid = m_datastore.get_dsid();

    if (ctx)
    {
        typename ChangeMap::iterator iter;
        CORBA::ULong i = 0;

        change.records.length(ctx->m_change_map.size());

        for (iter = ctx->m_change_map.begin(); iter != ctx->m_change_map.end();
             ++iter)
        {
            StateChangeKind kind = iter->second;

            CORBA::OctetSeq_var oid_seq_var =  
                m_oid_codec.encode
                (Cdmw::OrbSupport::type_traits<OID>::_in(iter->first));
            change.records[i].oid_seq = oid_seq_var;

            if (kind != CdmwFT::Transaction::DS_REMOVAL)
            {
                CORBA::OctetSeq_var data_seq_var =
                    m_data_codec.encode
                    (Cdmw::OrbSupport::type_traits<DATA>::_in
                     (ctx->m_data_map[iter->first]));
                change.records[i].change_data.data_seq(data_seq_var.in());
            }

            change.records[i].change_data._d(kind);
            ++i;
        }
    }
}

template <class OID, class DATA, class OIDCOMPARE>
void TXDataStore<OID, DATA, OIDCOMPARE>::clear_cache()
{
    TXDataStoreContext* ctx = get_context();

    if (ctx)
    {
        ctx->m_change_map.clear();
        ctx->m_data_map.clear();
        ctx->m_size_adjustment = 0;
    }
}

template <class OID, class DATA, class OIDCOMPARE>
void TXDataStore<OID, DATA, OIDCOMPARE>::flush()
{
    TXDataStoreContext* ctx = get_context();

    if (ctx)
    {
        typename ChangeMap::iterator iter;

        for (iter = ctx->m_change_map.begin(); iter != ctx->m_change_map.end();
             ++iter)
        {
            switch (iter->second)
            {
                case CdmwFT::Transaction::DS_INSERTION:
                {
                    m_datastore.insert_local(Cdmw::OrbSupport::type_traits<OID>::_in(iter->first),
                                             Cdmw::OrbSupport::type_traits<DATA>::_in(ctx->m_data_map[iter->first]));
                    break;
                }
                case CdmwFT::Transaction::DS_UPDATE:
                {
                    m_datastore.update_local(Cdmw::OrbSupport::type_traits<OID>::_in(iter->first),
                                             Cdmw::OrbSupport::type_traits<DATA>::_in(ctx->m_data_map[iter->first]));
                    break;
                }
                case CdmwFT::Transaction::DS_REMOVAL:
                {
                    m_datastore.remove_local(Cdmw::OrbSupport::type_traits<OID>::_in(iter->first));
                    break;
                }
            }
        }

        clear_cache();
    }
}

template <class OID, class DATA, class OIDCOMPARE>
typename TXDataStore<OID, DATA, OIDCOMPARE>::TXDataStoreContext*
TXDataStore<OID, DATA, OIDCOMPARE>::get_context()
{
    TXDataStoreContext* result = 0;
    if (m_manager.in_transaction())
    {
        m_manager.register_datastore(this);
        result = &(**m_tss_map)[m_datastore.get_dsid()];
    }

    return result;
}

}  // End namespace FT
}  // End namespace Cdmw


#endif /* INCL_CDMW_FT_FTSTATEMANAGER_TXDATASTORE_HPP */
