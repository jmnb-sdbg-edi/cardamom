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

#include "ftstatemanager/DataStoreContext.hpp"
#include <FaultTolerance/ftstatemanager/TransactionEngineCohort.hpp>
#include <FaultTolerance/ftstatemanager/TransactionEngineCoordinator.hpp>
#include <sstream>

namespace Cdmw {
namespace FT {

NotFoundException::NotFoundException(const std::string& exceptionInfo)
    : Exception( Exception::SAFE, exceptionInfo )
{}

AlreadyExistException::AlreadyExistException(const std::string& exceptionInfo)
    : Exception( Exception::SAFE, exceptionInfo )
{}

int DataStoreContext::M_context_count = 0;

// TODO Add m_iterator_increment init and usage

DataStoreContext::DataStoreContext
(CORBA::ORB_ptr          orb,
 PortableServer::POA_ptr poa,
 const std::string&      port,
 const std::string&      domain,
 const std::string&      ip_address,
 int                     max_transaction_in_progress,
 int                     max_transaction_done,
 OsSupport::OS::Timeval  cohort_timeout,
 OsSupport::OS::Timeval  coordinator_timeout,
 unsigned short          nbthreads,
 CORBA::ULong            iterator_increment,
 CORBA::ULong            freeze_threshold)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa)),
      m_is_activated(false),
      m_is_miop(true),
      m_iterator_increment(iterator_increment),
      m_freeze_threshold(freeze_threshold)
{
    M_context_count++;

    int cohort_id = M_context_count * 2;
    int coordinator_id = cohort_id + 1;

    // set the port to string
    std::ostringstream buffer1;
    buffer1 << "corbaloc:miop:1.0@1.0-" << domain << "-" << cohort_id << "/"
                << ip_address << ":" << port;

    std::string cohort_corbaloc = buffer1.str();

    std::ostringstream buffer2;
    buffer2 << "corbaloc:miop:1.0@1.0-" << domain << "-" << coordinator_id
            << "/" << ip_address << ":" << port;

    std::string coordinator_corbaloc = buffer2.str();

    m_coordinator_corbaloc = coordinator_corbaloc;
    m_cohort_corbaloc = cohort_corbaloc;

    // create the object
    m_pTransactionCohort =
        new TransactionEngineCohort(*this,
                                    m_orb.in(),
                                    m_poa.in(),
                                    coordinator_corbaloc,
                                    max_transaction_in_progress,
                                    max_transaction_done,
                                    coordinator_timeout);

    m_pTransactionCoordinator = 
        new TransactionEngineCoordinator(*this,
                                         m_orb.in(),
                                         m_poa.in(),
                                         cohort_corbaloc,
                                         max_transaction_in_progress,
                                         max_transaction_done,
                                         cohort_timeout,
                                         nbthreads);
    
    
    // create id for the cohort
    CORBA::Object_var obj =
        m_orb->string_to_object(coordinator_corbaloc.c_str());
    m_coordinator =
        CdmwFT::Transaction::TPCCoordinator::_unchecked_narrow(obj.in());
    m_coordinator_id = m_poa->create_id_for_reference(obj.in());

    obj = m_orb->string_to_object(cohort_corbaloc.c_str());
    m_cohort = CdmwFT::Transaction::TPCCohort::_unchecked_narrow(obj.in());
    m_cohort_id = m_poa->create_id_for_reference(obj.in());

    m_poa->activate_object_with_id (m_coordinator_id.in (),m_pTransactionCoordinator);

    //        CORBA_Object_var obj = m_orb->string_to_object(m_coordinator_corbaloc.c_str());
    // TAO BUG : the disassociate_reference_with_id is not implemented
    //        m_poa->disassociate_reference_with_id(obj.in(), m_coordinator_id.in ());

    // activate the cohort
    m_poa->activate_object_with_id (m_cohort_id.in (),m_pTransactionCohort);
    m_pTransactionEngine = m_pTransactionCohort;
   
    m_pTransactionCoordinator->deactivate();
    m_pTransactionCohort->activate();
}

DataStoreContext::DataStoreContext
(CORBA::ORB_ptr          orb,
 PortableServer::POA_ptr poa,
 int                     max_transaction_in_progress,
 int                     max_transaction_done,
 OsSupport::OS::Timeval  cohort_timeout,
 OsSupport::OS::Timeval  coordinator_timeout,
 unsigned short          nbthreads,
 CORBA::ULong            iterator_increment,
 CORBA::ULong            freeze_threshold)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa)),
      m_is_activated(false),
      m_is_miop(false),
      m_iterator_increment(iterator_increment),
      m_freeze_threshold(freeze_threshold)
{
    m_coordinator_location = DataStoreBase::Get_Location_Name();
    m_location = DataStoreBase::Get_Location_Name();

    // create the object
    m_pTransactionCohort =
        new TransactionEngineCohort(*this,
                                    m_orb.in(),
                                    m_poa.in(),
                                    "",
                                    max_transaction_in_progress,
                                    max_transaction_done,
                                    coordinator_timeout);
    
    m_pTransactionCoordinator = 
        new TransactionEngineCoordinator(*this,
                                         m_orb.in(),
                                         m_poa.in(),
                                         "",
                                         max_transaction_in_progress,
                                         max_transaction_done,
                                         cohort_timeout,
                                         nbthreads);


    m_coordinator = m_pTransactionCoordinator->_this();
    CdmwFT::Transaction::TPCCohort_var m_cohort_temp =
        m_pTransactionCohort->_this();

    TimeBase::TimeT relative_timeout =
        (cohort_timeout.seconds * 1000000 + cohort_timeout.microseconds) *10 ;
    CORBA::Any relative_timeout_as_any;
    relative_timeout_as_any <<= relative_timeout;
    CORBA::PolicyList policy_list;
    policy_list.length(1);
    policy_list[0] =
        m_orb->create_policy(Messaging::RELATIVE_RT_TIMEOUT_POLICY_TYPE,
                             relative_timeout_as_any);
    CORBA::Object_var object =
        m_cohort_temp->_set_policy_overrides(policy_list, CORBA::SET_OVERRIDE);
    m_cohort = CdmwFT::Transaction::TPCCohort::_narrow(object.in());

    m_pTransactionEngine = m_pTransactionCohort;
   
    m_pTransactionCoordinator->deactivate();
    m_pTransactionCohort->activate();
}

/**
 * Destructor.
 */
DataStoreContext::~DataStoreContext()
{
    m_pTransactionCoordinator->_remove_ref();
    m_pTransactionCoordinator = 0;
    m_pTransactionCohort->_remove_ref();;
    m_pTransactionCohort = 0;
    m_pTransactionEngine = 0;
}

void DataStoreContext::register_datastore(DataStoreBase* ds)
{
    // FIXME: maybe we should add a mutex if this is not called by generated code.
    m_datastore_map[ds->get_dsid()] = ds;
}

DataStoreBase* DataStoreContext::get_datastore(DsIdentifier id)
{
    CDMW_ASSERT(m_datastore_map.find(id) != m_datastore_map.end());
    return m_datastore_map[id];
}

DataStoreContext::DataStoreIdList DataStoreContext::get_datastore_ids()
{
    // FIXME: maybe we should add a mutex if this is not called by generated code.
    DataStoreContext::DataStoreIdList lst;
    for (DataStoreContext::DataStoreMap::iterator it = m_datastore_map.begin(); 
         it != m_datastore_map.end(); 
         ++it) 
    {
        lst.push_back(it->first);
    }
    return lst;
}

/**
 * process failure of a location
 *
 *@param location is the failed location
 */
void DataStoreContext::on_failure(const std::string& location)
{
    TransactionEngineRefMap::iterator iter =
        m_transactionEngineRefMap.find(location);
    if (iter != m_transactionEngineRefMap.end())
        m_transactionEngineRefMap.erase(location);
    
    m_pTransactionEngine->on_failure(location);

    for (LocationListMap::iterator iter = m_cohortsMap.begin();
         iter != m_cohortsMap.end();
         iter ++)
    {
        if (contains((*iter).first, location))
        {
            (*iter).second.erase(location);
        }
    }
}

/**
 * Add the references to the Cohort and the Coordinator objects of another new location.
 * This operation is used in case of point-to-point protocol.
 *
 *@param location is the new location
 *@param coordinator is the reference to the Coordinator object
 *@param cohort is the reference to the Cohort object
 */
void DataStoreContext::add_ref_for_location
(const std::string& location,
 CdmwFT::Transaction::TPCCoordinator_ptr coordinator,
 CdmwFT::Transaction::TPCCohort_ptr cohort)
    throw (AlreadyExistException)
{
    // We can change locations only when no transactions in all datastores
    CDMW_WRITER_LOCK_GUARD(m_cohorts_mutex);

    if (!m_is_miop && (location != DataStoreBase::Get_Location_Name()))
    {
        TransactionEngineRef EngineStruct;
        EngineStruct.coordinator =
            CdmwFT::Transaction::TPCCoordinator::_duplicate(coordinator);
        EngineStruct.cohort =
            CdmwFT::Transaction::TPCCohort::_duplicate(cohort);
        m_transactionEngineRefMap[location] = EngineStruct;
    }

    m_iter_map.erase(location);
}

/**
 * Return the reference to the local Coordinator object
 */
CdmwFT::Transaction::TPCCoordinator_ptr DataStoreContext::get_tpc_coordinator()
    const
{
    return CdmwFT::Transaction::TPCCoordinator::_duplicate(m_coordinator.in());
}

/**
 * Return the reference to the local Cohort object
 */
CdmwFT::Transaction::TPCCohort_ptr DataStoreContext::get_tpc_cohort() const
{
    return CdmwFT::Transaction::TPCCohort::_duplicate(m_cohort.in());
}

/**
 * Return the references to the Coordinator and the Cohort object of a location
 *
 *@param location is the location
 *@result a structure with the references to the Coordinator and the Cohort
 */
TransactionEngineRef
DataStoreContext::get_ref_for_location(const std::string& location) const
    throw (NotFoundException)
{
    if(location == DataStoreBase::Get_Location_Name())
    {
        TransactionEngineRef engine;

        engine.coordinator =
            CdmwFT::Transaction::TPCCoordinator::_duplicate(m_coordinator.in());
        engine.cohort =
            CdmwFT::Transaction::TPCCohort::_duplicate(m_cohort.in ());

        return engine;
    } 
    else
    {
        TransactionEngineRefMap::const_iterator iter =
            m_transactionEngineRefMap.find(location);
        if (iter == m_transactionEngineRefMap.end())
            throw NotFoundException();
        else
        {
            return iter->second;
        }
    
    }
}

/**
* set coordinator location
*
*@param location 
*/
void DataStoreContext::set_coordinator_location(const std::string& location)
    throw ()
{
    m_coordinator_location = location;
}


/**
* get coordinator location
*
*@param location 
*/
std::string DataStoreContext::get_coordinator_location() const
    throw ()
{
    return m_coordinator_location;
}


CdmwFT::Transaction::TPCCoordinator_ptr DataStoreContext::get_coordinator_ref()
    const
{
    return CdmwFT::Transaction::TPCCoordinator::_duplicate(m_coordinator.in());
}

CdmwFT::Transaction::TPCCohort_ptr DataStoreContext::get_cohort_ref() const
{
    return CdmwFT::Transaction::TPCCohort::_duplicate(m_cohort.in());
}


/**
 * The data store becomes the one of the master
 */
void DataStoreContext::activate()
{
    if (m_is_activated)
    {
        return;
    }

    m_is_activated = true;

    // TAO BUG : the disassociate_reference_with_id is not implemented
    //    m_poa->disassociate_reference_with_id(m_cohort.in(), m_cohort_id.in ());
    
    //    m_poa->associate_reference_with_id(m_coordinator.in(), m_coordinator_id.in ());
    m_pTransactionCohort->deactivate();

    m_pTransactionEngine = m_pTransactionCoordinator;
    m_pTransactionCoordinator->activate
        (m_pTransactionCohort->get_cohort_transactions(),
         m_pTransactionCohort->get_cohort_transactions_done());
}

/**
 * The data store becomes the one of the slave
 */
void DataStoreContext::deactivate()
{
    if (!m_is_activated)
    {
        return;
    }

    m_is_activated = false;

    // TAO BUG : the disassociate_reference_with_id is not implemented
    //    m_poa->disassociate_reference_with_id(m_coordinator.in(), m_coordinator_id.in ());
    
    //    m_poa->associate_reference_with_id(m_cohort.in(), m_cohort_id.in ());

    m_pTransactionCoordinator->deactivate();
    m_pTransactionCohort->activate();

    m_pTransactionEngine = m_pTransactionCohort;
}

bool DataStoreContext::request(const CdmwFT::Transaction::StateChanges& change)
    throw (InternalErrorException, ValidationFailureException)
{
    return m_pTransactionCoordinator->request(change);
}

void DataStoreContext::wait_for_no_transactions_in_progress()
{
    m_pTransactionCoordinator->wait_for_no_transactions_in_progress();
}

::CdmwFT::StateTransfer::RecordIterator_ptr
DataStoreContext::get_record_iterator
(const char * from_location,
 const CdmwFT::StateTransfer::DataStoreIDs& datastore_ids,
 CdmwFT::Transaction::TPCCoordinator_ptr coordinator,
 CdmwFT::Transaction::TPCCohort_ptr cohort)
   throw (CORBA::SystemException,
          CdmwFT::StateTransfer::InvalidDataStoreState)
{
    RecordIteratorMap::iterator pos;
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

//TODO
#if 0
    locations = DataStoreBase::Get_Locations(get_dsid());

    if (locations.find(from_location) != locations.end())
    {
        // DataStore already exists for this location
        throw CORBA::BAD_INV_ORDER(Cdmw::OrbSupport::BAD_INV_ORDERLocationAlreadyPresent,
                                   CORBA::COMPLETED_NO);
    }
#endif

    RecordIterator_impl* iter_impl =
       new RecordIterator_impl(*this, m_poa.in(), m_orb.in(), from_location,
                               datastore_ids, coordinator, cohort,
                               m_freeze_threshold);
    
    CdmwFT::StateTransfer::RecordIterator_var iter = iter_impl->_this();
    m_iter_map[from_location] = iter_impl;

    return iter._retn();
}

void DataStoreContext::update_with_coordinator(
    CdmwFT::StateTransfer::DataStoreGroup_ptr primary_datastore_group,
    const std::string & location,
    const CdmwFT::StateTransfer::DataStoreIDs& datastore_ids)
    throw (CORBA::SystemException,
           CdmwFT::StateTransfer::InvalidDataStoreState)
{
    CdmwFT::StateTransfer::RecordIterator_var rec_iter =
        primary_datastore_group->get_record_iterator(location.c_str(),
                                                     datastore_ids,
                                                     m_coordinator.in(),
                                                     m_cohort.in());
    
    bool more;

    try
    {
        do
        {
            CdmwFT::Transaction::StateChanges_var changes;

            more = rec_iter->next_n(m_iterator_increment, changes.out());

            for (CORBA::ULong i = 0; i < changes->length(); i++)
            {
                DataStoreBase* ds = m_datastore_map[changes[i].dsid];
                ds->load_state(changes[i].records);
            }

        } while (more);

        rec_iter->destroy();
    }
    catch (const CORBA::Exception & ex)
    {
        // TODO recover fron transient network failure
        // TODO conditional, configurable exit
	     CDMW_FT_ERROR << "[ERROR] Location <" << m_location
                      << " updating with coordinator "
                      << m_coordinator_location
                      << "> Aborting location! \n"
		                << "     Reason: received exception "
                      << Cdmw::OrbSupport::OrbSupport::exception_to_string(ex)
		                << std::endl;
        // TODO define configurable error code value
        ::exit(-1);
    }
    catch (const NotFoundException& e)
    {
        // TODO conditional, configurable exit
        CDMW_FT_ERROR << "[ERROR] Location <" << m_location
                      << " updating with coordinator "
                      << m_coordinator_location
                      << "> Aborting location! \n"
		                << "     Reason: received NotFoundException "
                      << e.what()
		                << std::endl;
        // TODO define configurable error code value
        ::exit(-1);
    }
    catch (const AlreadyExistException& e)
    {
        // TODO conditional, configurable exit
        CDMW_FT_ERROR << "[ERROR] Location <" << m_location
                      << " updating with coordinator "
                      << m_coordinator_location
                      << "> Aborting location! \n"
		                << "     Reason: received AlreadyExistException "
                      << e.what()
		                << std::endl;
        // TODO define configurable error code value
        ::exit(-1);
    }
}

bool DataStoreContext::validate
(const CdmwFT::Transaction::StateChanges& changes)
{
    for (CORBA::ULong i = 0; i < changes.length(); ++i)
    {
        DsIdentifier dsid = changes[i].dsid;
        DataStoreBase *dataStore = m_datastore_map[dsid];

        if (!dataStore || !dataStore->validate(changes[i].records))
        {
            return false;
        }
    }

    return true;
}

bool DataStoreContext::oids_match
(const CdmwFT::Transaction::StateChanges& changes1,
 const CdmwFT::Transaction::StateChanges& changes2)
{
    for (CORBA::ULong i = 0; i < changes1.length(); ++i)
    {
        DsIdentifier dsid1 = changes1[i].dsid;
        DataStoreBase *dataStore = m_datastore_map[dsid1];

        for (CORBA::ULong j = 0; j < changes2.length(); ++j)
        {
            DsIdentifier dsid2 = changes2[j].dsid;

            if (dsid1 == dsid2)
            {
                if (dataStore->oids_match(changes1[i].records,
                                          changes2[j].records))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

/**
 * Purpose:
 * <p> add one location (to the yes/no/end check)
 *
 *@param new location
 */
void DataStoreContext::add_location(const DsIdentifier& dsid,
                                    const std::string& location)
    throw (AlreadyExistException, NotFoundException)
{
    DataStoreBase *datastore = m_datastore_map[dsid];

    // if the datastore is not found and the location is not the local location
    if ((datastore == 0) && (location != m_location))
    {
        throw NotFoundException("dsid is not found");
    }

    // We can change locations only when no transactions in all datastores
    CDMW_WRITER_LOCK_GUARD(m_cohorts_mutex);
    if (contains(dsid, location))
    {
        throw AlreadyExistException("The location is already registered");
    }
    else if (location == m_location)
    {
        throw AlreadyExistException("Should be a coordinator");
    }
    else
    {
        // if the dsid is not found, it is the first insertion
        LocationListMap::iterator iter = m_cohortsMap.find(dsid);
        if (iter == m_cohortsMap.end())
        {
            LocationList list;
            list.insert(location);
            m_cohortsMap[dsid] = list;
        } else {
            (*iter).second.insert(location);
        }
    }
}

/**
 * Purpose:
 * <p> set locations (to the ack check)
 *
 *@param new locations
 */
void DataStoreContext::set_locations(const DsIdentifier& dsid,
                                     const LocationList& locations)
{
    // We can change locations only when no transactions in all datastores
    CDMW_WRITER_LOCK_GUARD(m_cohorts_mutex);
    m_cohortsMap[dsid] = locations;
    LocationListMap::iterator iter = m_cohortsMap.find(dsid);
    (*iter).second.erase(m_location);
}


DataStoreContext::LocationList
DataStoreContext::get_locations(const DsIdentifier& dsid)
    throw (NotFoundException)
{
    
    LocationListMap::iterator iter = m_cohortsMap.find(dsid);
    if (iter == m_cohortsMap.end())
    {
        LocationList list;
        return list;
    }
    
    LocationList locations = (*iter).second;
    return locations;
}

bool DataStoreContext::contains(const DsIdentifier& dsid,
                                const std::string& location)
    throw (NotFoundException)
{
    bool result = false;

    DataStoreBase *datastore = m_datastore_map[dsid];

    // if the datastore is not found and the location is not the local location
    if (datastore == 0)
        throw NotFoundException("dsid is not found");
  

    LocationListMap::iterator iter = m_cohortsMap.find(dsid);
    if (iter != m_cohortsMap.end())
    {
        LocationList list = (*iter).second;
        
        LocationList::iterator it =
        find(list.begin(), list.end(), location);
        
        if (it != list.end())
        {
            result = true;
        }
    }
    return result;
}

}  // End namespace FT
}  // End namespace Cdmw
