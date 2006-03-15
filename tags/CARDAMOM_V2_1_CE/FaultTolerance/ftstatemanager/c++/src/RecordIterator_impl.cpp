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

#include <FaultTolerance/ftstatemanager/RecordIterator_impl.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreBase.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreContext.hpp>
#include <list>

namespace Cdmw {
namespace FT {

RecordIterator_impl::RecordIterator_impl
(DataStoreContext& context,
 PortableServer::POA_ptr poa,
 CORBA::ORB_ptr orb,
 const std::string location,
 const CdmwFT::StateTransfer::DataStoreIDs& datastore_ids,
 CdmwFT::Transaction::TPCCoordinator_ptr coordinator,
 CdmwFT::Transaction::TPCCohort_ptr cohort,
 CORBA::ULong freeze_threshold)
    : m_context(context),
      m_location(location),
      m_transferred(0),
      m_poa(PortableServer::POA::_duplicate(poa)),
      m_freeze_threshold(freeze_threshold),
      m_frozen(false),
      m_is_destroyed(false),
      m_coordinator(CdmwFT::Transaction::TPCCoordinator::_duplicate(coordinator)),
      m_cohort(CdmwFT::Transaction::TPCCohort::_duplicate(cohort))
{
    // create iterator per datastore
    for (CORBA::ULong i = 0; i < datastore_ids.length(); i++)
    {
        DataStoreID id = datastore_ids[i];
        
        m_datastore_ids.push_back(id);

        m_iter[id] = m_context.get_datastore(id)
            ->get_record_iterator(m_location.c_str());
    } 
}

RecordIterator_impl::~RecordIterator_impl() throw()
{
}

PortableServer::POA_ptr RecordIterator_impl::_default_POA()
{
    return PortableServer::POA::_duplicate(m_poa.in());
}

/**
 * Obtains at most the requested number of StateChangeRecords.
 *
 * @param how_many the maximum number of records to return.
 *
 * @param changes the StateChangeRecords.
 *
 * @return true if more records are available.
 */
CORBA::Boolean RecordIterator_impl::next_n
(CORBA::ULong how_many,
 CdmwFT::Transaction::StateChanges_out changes)
    throw (CORBA::SystemException)
{
    typedef std::list<CdmwFT::Transaction::StateChange*> StateList;

    CORBA::ULong needed = how_many;
    DataStoreID id;
    CORBA::Boolean more = true;
    CdmwFT::Transaction::StateChange* change;
    CdmwFT::Transaction::StateChangeRecords_var records;
    StateList result_temp;
    StateList::iterator iter;
    CORBA::ULong remaining = 0;
    CORBA::ULong pos = 0;


    while ((needed > 0) && more)
    {
        remaining = 0;

        for (IDList::iterator iter = m_datastore_ids.begin();
             iter != m_datastore_ids.end();
             ++iter)
        {
            id = *iter;
            
            remaining += m_iter[id]->next_n(needed, records.out());

            if (records->length() > 0)
            {
                change = new CdmwFT::Transaction::StateChange();
                change->dsid = id;
                change->records = records;
                needed -= records->length();

                result_temp.push_back(change);
            }
        }

        // Freeze the DataStores if there less than the given
        // threshold of records left to transfer.

        if (!m_frozen && (remaining <= m_freeze_threshold))
        {
            m_frozen = true;

            for (IDList::iterator iter = m_datastore_ids.begin();
                 iter != m_datastore_ids.end();
                 ++iter)
            {
                m_context.get_datastore(*iter)->freeze(m_location);
            }
        }

        more = (remaining > 0);
    }

    changes = new CdmwFT::Transaction::StateChanges;
    changes->length(result_temp.size());

    for (iter = result_temp.begin(); iter != result_temp.end(); ++iter)
    {
        (*changes)[pos++] = **iter;
        delete *iter;
    }

    return more;
}

void RecordIterator_impl::destroy ()
    throw (CORBA::SystemException)
{
    if (m_is_destroyed)
    {
        // already deleted
        return;
    }

    m_is_destroyed = true;

    for (IDList::iterator iter = m_datastore_ids.begin();
         iter != m_datastore_ids.end();
         ++iter)
    {
        m_iter[*iter]->destroy();
    }

    m_context.add_ref_for_location(m_location, m_coordinator.in(),
                                   m_cohort.in());

    if (m_frozen)
    {
        for (IDList::iterator iter = m_datastore_ids.begin();
             iter != m_datastore_ids.end();
             ++iter)
        {
            m_iter[*iter]->destroy();

            m_context.get_datastore(*iter)->unfreeze(m_location);
        }
    }

    // deactivating will eventually delete this object
    PortableServer::ObjectId_var oid = m_poa->servant_to_id(this);
    m_poa->deactivate_object(oid.in());
}

}  // End namespace FT
}  // End namespace Cdmw
