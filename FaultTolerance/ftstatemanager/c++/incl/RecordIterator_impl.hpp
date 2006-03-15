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

#ifndef INCL_CDMW_FT_FTSTATEMANAGER_RECORD_ITERATOR_IMPL_HPP
#define INCL_CDMW_FT_FTSTATEMANAGER_RECORD_ITERATOR_IMPL_HPP

#include <FaultTolerance/ftstatemanager/DataStoreBase.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreRecordIteratorBase.hpp>
#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include <Foundation/osthreads/Mutex.hpp>
#include <Foundation/osthreads/MutexGuard.hpp>

#include <idllib/CdmwFTStateTransfer.skel.hpp>

#include <list>
#include <map>

namespace Cdmw
{
namespace FT
{

class DataStoreContext;

class RecordIterator_impl:
         virtual public POA_CdmwFT::StateTransfer::RecordIterator,
         virtual public Cdmw::OrbSupport::RefCountServantBase
{

    typedef CdmwFT::Transaction::DataStoreID DataStoreID;
    typedef std::list<DataStoreID> IDList;
    typedef std::map<DataStoreID, DataStoreRecordIteratorBase*> IterMap;

public:

    RecordIterator_impl
    (DataStoreContext& context,
     PortableServer::POA_ptr poa,
     CORBA::ORB_ptr orb,
     const std::string location,
     const CdmwFT::StateTransfer::DataStoreIDs& datastore_ids,
     CdmwFT::Transaction::TPCCoordinator_ptr coordinator,
     CdmwFT::Transaction::TPCCohort_ptr cohort,
     CORBA::ULong freeze_threshold);

    ~RecordIterator_impl() throw();

    virtual PortableServer::POA_ptr _default_POA();

    /**
     * Obtains at most the requested number of StateChangeRecords.
     *
     * @param how_many the maximum number of records to return.
     *
     * @param changes the StateChangeRecords.
     *
     * @return true if more records are available.
     */
    CORBA::Boolean next_n (CORBA::ULong how_many,
                           CdmwFT::Transaction::StateChanges_out changes)
        throw (CORBA::SystemException);

    void destroy ()
        throw (CORBA::SystemException);

private:
    // DataStoreContext
    DataStoreContext& m_context;

    IDList m_datastore_ids;

    // Location being added
    std::string m_location;

    // Count of transferred DataStores
    CORBA::ULong m_transferred;

    // Iterators over DataStores
    IterMap m_iter;

    // Mutex for concurrency guard
    Cdmw::OsSupport::Mutex m_mutex;

    // POA for this object
    PortableServer::POA_var m_poa;

    // Size of transfer data at which to freeze the DataStore
    CORBA::ULong m_freeze_threshold;

    // Flag for freeze of underlying DataStore
    bool m_frozen;

    // Flag set if destroy has been called
    bool m_is_destroyed;

    CdmwFT::Transaction::TPCCoordinator_var m_coordinator;
    CdmwFT::Transaction::TPCCohort_var m_cohort;
};

}  // End namespace FT
}  // End namespace Cdmw

#endif // INCL_CDMW_FT_FTSTATEMANAGER_RECORD_ITERATOR_IMPL_HPP
