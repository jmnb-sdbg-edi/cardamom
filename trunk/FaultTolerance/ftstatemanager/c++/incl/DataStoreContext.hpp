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

#ifndef INCL_CDMW_FT_FTSTATEMANAGER_DATASTORECONTEXT_HPP
#define INCL_CDMW_FT_FTSTATEMANAGER_DATASTORECONTEXT_HPP

#include <FaultTolerance/idllib/CdmwFTTPCCohort.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTTPCCoordinator.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTStateTransfer.stub.hpp>
#include <FaultTolerance/ftstatemanager/TransactionEngineRef.hpp>
#include <FaultTolerance/ftstatemanager/ValidationFailureException.hpp>
#include <FaultTolerance/ftstatemanager/RecordIterator_impl.hpp>
#include <Foundation/common/AlreadyExistException.hpp>
#include <Foundation/common/NotFoundException.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <list>

namespace Cdmw {
namespace FT {

const unsigned short DEFAULT_NB_WORKER_TREADS = 4;
const CORBA::ULong DEFAULT_CONTEXT_ITERATOR_INCREMENT = 100;
const CORBA::ULong DEFAULT_CONTEXT_FREEZE_THRESHOLD = 100;
    
class TransactionEngineCoordinator;
class TransactionEngineCohort;
class TransactionEngine;

class DataStoreContext
{
    typedef Cdmw::Common::AlreadyExistException AlreadyExistException;
    typedef Cdmw::Common::NotFoundException NotFoundException;

    typedef std::map<std::string, TransactionEngineRef>
        TransactionEngineRefMap;

    typedef std::map<std::string, RecordIterator_impl*, std::less<std::string> >
        RecordIteratorMap;

    typedef std::map<DsIdentifier, DataStoreBase*, std::less<DsIdentifier> >
        DataStoreMap;
        
public:
    typedef std::list<DsIdentifier> DataStoreIdList;
    typedef std::set<std::string> LocationList;
    typedef std::map<DsIdentifier, LocationList> LocationListMap;

    DataStoreContext(CORBA::ORB_ptr          orb,
                     PortableServer::POA_ptr poa,
                     const std::string&      port,
                     const std::string&      domain,
                     const std::string&      ip_address,
                     int                     max_transaction_in_progress,
                     int                     max_transaction_done,
                     OsSupport::OS::Timeval  cohort_timeout,
                     OsSupport::OS::Timeval  coordinator_timeout,
                     unsigned short          nbthreads = DEFAULT_NB_WORKER_TREADS,
                     CORBA::ULong            iterator_increment = DEFAULT_CONTEXT_ITERATOR_INCREMENT,
                     CORBA::ULong            freeze_threshold = DEFAULT_CONTEXT_FREEZE_THRESHOLD)
        throw(CORBA::SystemException);

    DataStoreContext(CORBA::ORB_ptr          orb,
                     PortableServer::POA_ptr poa,
                     int                     max_transaction_in_progress,
                     int                     max_transaction_done,
                     OsSupport::OS::Timeval  cohort_timeout,
                     OsSupport::OS::Timeval  coordinator_timeout,
                     unsigned short          nbthreads = DEFAULT_NB_WORKER_TREADS,
                     CORBA::ULong            iterator_increment = DEFAULT_CONTEXT_ITERATOR_INCREMENT,
                     CORBA::ULong            freeze_threshold = DEFAULT_CONTEXT_FREEZE_THRESHOLD)
        throw(CORBA::SystemException);

    /**
     * Destructor.
     */
    virtual ~DataStoreContext();

    void register_datastore(DataStoreBase* ds);

    DataStoreBase* get_datastore(DsIdentifier id);

    DataStoreIdList get_datastore_ids();
    
    /**
     * The data store becomes the one of the master
     */
    void activate();

    /**
     * The data store becomes the one of the slave
     */
    void deactivate();

    /**
     * Purpose:
     * <p> Ask the cohorts to apply a change
     *
     *@param oid is the Object Id key
     */
    bool request(const CdmwFT::Transaction::StateChanges& change)
        throw (InternalErrorException, ValidationFailureException);

    /**
     * Wait for transactions in progress to complete.
     * Deprecated. Only for use by RecordIterator.
     */
    void wait_for_no_transactions_in_progress();

    /**
     * process failure of a location
     *
     *@param location is the failed location
     */
    void on_failure(const std::string& location);

    /**
     * Add the references to the Cohort and the Coordinator objects of another new location.
     * This operation is used in case of point-to-point protocol.
     *
     *@param location is the new location
     *@param coordinator is the reference to the Coordinator object
     *@param cohort is the reference to the Cohort object
     */
    void add_ref_for_location(const std::string& location,
                      CdmwFT::Transaction::TPCCoordinator_ptr coordinator,
                      CdmwFT::Transaction::TPCCohort_ptr cohort)
        throw (AlreadyExistException);

    /**
     * Return the reference to the local Coordinator object
     */
    CdmwFT::Transaction::TPCCoordinator_ptr get_tpc_coordinator() const;
    
    /**
     * Return the reference to the local Cohort object
     */
    CdmwFT::Transaction::TPCCohort_ptr get_tpc_cohort() const;

    /**
     * Return the references to the Coordinator and the Cohort object of a location
     *
     *@param location is the location
     *@result a structure with the references to the Coordinator and the Cohort
     */
    TransactionEngineRef
    get_ref_for_location(const std::string& location) const
        throw (NotFoundException);
    
    /**
    * set coordinator location
    *
    *@param location 
    */
    void set_coordinator_location(const std::string& location)
        throw ();

    /**
    * get coordinator location
    *
    *@param location 
    */
    std::string get_coordinator_location() const
        throw ();

    CdmwFT::Transaction::TPCCoordinator_ptr get_coordinator_ref() const;

    CdmwFT::Transaction::TPCCohort_ptr get_cohort_ref() const;
    
    inline bool is_activated() const
    {
        return m_is_activated;
    }
    

    /**
     * Returns an iterator that provides state information necessary to copy
     * a DataStore.
     *
     * @param from_location is the name of the location asking the records.
     * @param datastore_ids Ids of DataStores to be transferred.
     *
     * @exception InvalidDataStoreState is raised if the data store is not the 
     * coordinator
     */
    virtual ::CdmwFT::StateTransfer::RecordIterator_ptr get_record_iterator
    (const char * from_location, 
     const CdmwFT::StateTransfer::DataStoreIDs& datastore_ids,
     CdmwFT::Transaction::TPCCoordinator_ptr coordinator,
     CdmwFT::Transaction::TPCCohort_ptr cohort)
        throw (CORBA::SystemException,
               CdmwFT::StateTransfer::InvalidDataStoreState);

    /*
     * Transfer state from primary.
     *
     * @param primary_datastore_group source DataStoreGroup.
     * @param from_location is the name of the location asking the records.
     * @param datastore_ids Ids of DataStores to be transferred.
     */
    void update_with_coordinator
    (CdmwFT::StateTransfer::DataStoreGroup_ptr primary_datastore_group,
     const std::string & location,
     const CdmwFT::StateTransfer::DataStoreIDs& datastore_ids)
        throw (CORBA::SystemException,
               CdmwFT::StateTransfer::InvalidDataStoreState);

    /**
     * Validate a set of changes against DataStores
     *
     * @result true if changes valid, false if any change is invalid
     */
    bool validate(const CdmwFT::Transaction::StateChanges& changes);

    /**
     * Compare the oids for a set of changes.
     *
     * @result true if any oids for the same DataStore match, false otherwise
     */
    bool oids_match(const CdmwFT::Transaction::StateChanges& changes1,
                    const CdmwFT::Transaction::StateChanges& changes2);

    static void On_Failure(const std::string& location)
        throw (NotFoundException);

    /**
    * add one location (to the ack check) to all DataStores
    *
    *@param location the new location
    */
    void add_location(const DsIdentifier& dsid, const std::string& location) 
        throw (AlreadyExistException, NotFoundException);

    /**
    * set locations (to the ack check)
    *
    *@param new locations
    */
    void set_locations(const DsIdentifier& dsid,
                       const LocationList& locations);

    /**
    * Get locations
    */
    LocationList get_locations(const DsIdentifier& dsid)
        throw (NotFoundException);

    // Check if element is contained 
    bool contains(const DsIdentifier& dsid, const std::string& location)
        throw (NotFoundException);

    // The mutex used to maintain cohorts list coherency
    Cdmw::OsSupport::ReaderWriterLock m_cohorts_mutex;
    
private:
    /**
     * Assignement operator is not allowed.
     */
    DataStoreContext& operator=(const DataStoreContext& rhs);

    CORBA::ORB_var m_orb;
    PortableServer::POA_var m_poa;

    DataStoreMap m_datastore_map;
    
    TransactionEngineCoordinator * m_pTransactionCoordinator;
    TransactionEngineCohort * m_pTransactionCohort;
    TransactionEngine * m_pTransactionEngine;

    CdmwFT::Transaction::TPCCoordinator_var m_coordinator;
    CdmwFT::Transaction::TPCCohort_var m_cohort;

    PortableServer::ObjectId_var m_coordinator_id;
    PortableServer::ObjectId_var m_cohort_id;

    // corbaloc
    std::string m_coordinator_corbaloc;
    std::string m_cohort_corbaloc;

    TransactionEngineRefMap m_transactionEngineRefMap;

    bool m_is_activated;

    /**
     * coordinator location
     */
    std::string m_coordinator_location;

    /**
     * local location
     */
    std::string m_location;

    /**
     *
     */
    bool m_is_miop;

    LocationListMap m_cohortsMap;

    // Iterator map
    RecordIteratorMap m_iter_map;

    // Size of sequence used by update_with_coordinator to transfer state
    CORBA::ULong m_iterator_increment;

    // Size of change set for freezing
    CORBA::ULong m_freeze_threshold;

    // Count of multicast DataStoreContexts for miop group id
    static int M_context_count;
};

}  // End namespace FT
}  // End namespace Cdmw

#endif
