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

#ifndef INCL_CDMW_FT_FTSTATEMANAGER_TRANSACTION_ENGINE_COORDINATOR_HPP
#define INCL_CDMW_FT_FTSTATEMANAGER_TRANSACTION_ENGINE_COORDINATOR_HPP

#include <map>
#include <list>

#include <FaultTolerance/ftstatemanager/TransactionEngine.hpp>
#include <FaultTolerance/ftstatemanager/TransactionEngineRef.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreBase.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreContext.hpp>
#include <FaultTolerance/idllib/CdmwFTTPCCoordinator.skel.hpp>
#include <FaultTolerance/idllib/CdmwFTTPCCohort.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTTPCCommon.stub.hpp>
#include <FaultTolerance/ftstatemanager/TransactionEngineCoordinatorBase.hpp>
#include <FaultTolerance/ftstatemanager/TransactionCoordinator.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/osthreads/ReaderLockGuard.hpp>
#include <Foundation/osthreads/WriterLockGuard.hpp>
#include <FaultTolerance/ftstatemanager/Timer.hpp>
#include <Foundation/osthreads/MutexGuard.hpp>
#include <ace/Future.h>
#include <FaultTolerance/ftcommon/MacroDef.hpp>
#include <FaultTolerance/ftstatemanager/Scheduler.hpp>
#include <Foundation/orbsupport/TypeTraits.hpp>

/**
*Purpose:
* TransactionEngineCoordinator template.
*<p>
*
*Features:
*<p> Thread safe. Exception safe.
*/
namespace Cdmw {
namespace FT {

const unsigned int INVALID_TRANSACTION_ID = 0;
const char* const INVALID_INITIATOR = "invalid";

class TransactionCoordinator;

class TransactionEngineCoordinator
    : public TransactionEngine,
      public virtual POA_CdmwFT::Transaction::TPCCoordinator,
      public virtual PortableServer::RefCountServantBase,
      public TransactionEngineCoordinatorBase
{
    typedef TransactionEngine::TransactionMap TransactionMap;
    typedef Timer<CdmwFT::Transaction::TransactionIdentification> Timer;
    typedef std::map<CdmwFT::Transaction::TransactionIdentification, 
                     TransactionCoordinator*, 
                     std::less< ::CdmwFT::Transaction::TransactionIdentification> > TransactionCoordMap;

public:
    typedef std::list<TransactionCoordinator*> TransactionCoordinatorList;

    /**
     * Purpose:
     * <p> Constructor with data store
     */
    TransactionEngineCoordinator
    (DataStoreContext&       context,
     CORBA::ORB_ptr          orb,
     PortableServer::POA_ptr poa,
     std::string             cohort_corbaloc,
     int                     max_transaction_in_progress,
     int                     max_transaction_done,
     OsSupport::OS::Timeval  timeout,
     unsigned short          nbthreads)
        throw(CORBA::SystemException);
    
    /**
     * Purpose:
     * <p> Destructor.
     */
    virtual ~TransactionEngineCoordinator();

    DataStoreContext& get_context()
    {
        return m_context;
    }

    /**
     * Purpose:
     * <p> Get the timer created in the coordinator
     *     
     */
    Timer* get_timer() { 
        return m_timer; 
    }
    
    /**
     * Purpose:
     * <p> Get the timer created in the coordinator
     *     
     */
    OsSupport::OS::Timeval get_timeout() {
        return m_timeout; 
    }

    /**
     * Purpose:
     * <p> Obtain a list of transactions with matching oids
     *
     *@param the StateChanges containing the oids to match
     */
    TransactionCoordinatorList get_transactions_with_matching_oid
    (const CdmwFT::Transaction::StateChanges& change);

    /**
     * Purpose:
     * <p> Ask the cohorts to make a change (inserts, updates and removes).
     *
     *@param oid is the Object Id key
     */
    bool request(const CdmwFT::Transaction::StateChanges& change)
        throw (InternalErrorException, ValidationFailureException);

    /**
     * Purpose:
     * <p> delegate send_yes
     *
     *@param sender
     *@param id
     */
    virtual void send_yes_i(const char * sender,
                            const CdmwFT::Transaction::TransactionIdentification & id)
        throw (InternalErrorException)
    {
        this->send_yes(sender, id);
    }
    
    /**
     * Purpose:
     * <p> delegate send_no
     *
     *@param sender
     *@param id
     */
    virtual void send_no_i(const char * sender,
                           const CdmwFT::Transaction::TransactionIdentification & id)
        throw (InternalErrorException);

    /**
     * Purpose:
     * <p> delegate send_end
     *
     *@param sender
     *@param id
     */
    virtual void send_end_i(const char * sender,
                            const CdmwFT::Transaction::TransactionIdentification & id) 
        throw (InternalErrorException);

    /**
     * Purpose:
     * <p> delegate send_end
     *
     *@param sender
     *@param id
     */
    virtual void send_set_transaction_status_i(const char * sender,
                                               const CdmwFT::Transaction::TransactionIdentification & id,
                                               CdmwFT::Transaction::TransactionStatus status)
        throw (InternalErrorException);

    /**
     * Purpose: Ask the cohorts to do the given request with the given entry
     * <p> 
     *
     *@param oid is the Object Id key
     */
    void delegate_request(const CdmwFT::Transaction::TransactionIdentification & id,
                          const CdmwFT::Transaction::StateChanges& change,
                          DataStoreContext::LocationList cohorts);

    /**
     * Purpose: Ask the cohorts to commit for the transaction
     * <p> 
     */
    void delegate_commit(const CdmwFT::Transaction::TransactionIdentification & id,
                         DataStoreContext::LocationList cohorts);
    
    /**
     * Purpose: Ask the cohorts to commit for the transaction
     * <p> 
     */
    void delegate_get_transaction_status(const CdmwFT::Transaction::TransactionIdentification & id,
                                         DataStoreContext::LocationList cohorts);

    /**
     * Purpose: Ask the cohorts to commit for the transaction
     * <p> 
     */
    void delegate_rollback(const CdmwFT::Transaction::TransactionIdentification & id,
                           DataStoreContext::LocationList cohorts);

    /**
     * Purpose: Used to update datastore when retrieving from old cohort mode
     *          transactions that are commited in other cohorts
     * <p> 
     */
    void commit_local(const CdmwFT::Transaction::StateChanges& change);

    /**
     * Purpose:
     * <p> process warning of a failure
     * (the coordinator won't wait for answers of the failed location)
     *
     *@param location is the failed location
     */
    void on_failure(const std::string& location);
    
    /**
     * Purpose:
     * <p> The process was cohort and becomes coordinator
     *     We must save the cohort transactions in RECOVERING state
     *
     */
    void activate(const TransactionMap & cohortTransactions,
                  const DoneTransactionList & cohortTransactionsDone);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/TPCCoordinator/send_yes:1.0
     * operation
     */
    void send_yes (const char * sender,
                   const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException);
    
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/TPCCoordinator/send_no:1.0
     * operation
     */       
    void send_no (const char * sender,
                  const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException);
    
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/TPCCoordinator/send_end:1.0
     * operation
     */
    void send_end (const char * sender,
                   const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException);

        
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/TPCBase/send_get_transaction_status:1.0
     * operation
     */
    void send_get_transaction_status (const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/TPCBase/send_set_transaction_status:1.0
     * operation
     */
    void send_set_transaction_status (const char * sender,
                                      const CdmwFT::Transaction::TransactionIdentification & id,
                                      CdmwFT::Transaction::TransactionStatus status)
        throw(CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/TPCBase/get_transaction_status:1.0
     * operation
     */
    CdmwFT::Transaction::TransactionStatus get_transaction_status
    (CdmwFT::Transaction::TransactionIdentification & id,
     CORBA::String_out sender)
        throw(CORBA::SystemException);
    
  /* Not in IIOP : the only moment where cohort set the transaction status of
     coordinator is when coordinator get the transaction status (recovering).
    void set_transaction_status (CdmwFT::Transaction::TransactionStatus status,
                                 char *& sender,
                                 CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException);
  */
    

    void deactivate();

    // return true if transaction is synch else false
    bool transaction_is_synch();

    /**
     * Wait for transactions in progress to complete.
     */
    void wait_for_no_transactions_in_progress();
    
protected:
    
    /**
     * Purpose:
     * <p> Default constructor.
     */
    //TransactionEngineCoordinator();

private:
    
    /**
     * Assignement operator is not allowed.
     */
    TransactionEngineCoordinator& operator=(const TransactionEngineCoordinator& rhs);
    
    void do_request_i(const CdmwFT::Transaction::StateChanges& change,
                      ACE_Future< Scheduler::TransactionResult >& future, 
                      TransactionCoordinatorList blocking_transactions) 
        throw (InternalErrorException);
    
    TransactionEngineRef get_ref_for_location
    (const ::CdmwFT::Transaction::TransactionIdentification & id,
     const std::string& location);

    unsigned long next_counter();
    
    DataStoreContext& m_context;

    TransactionCoordMap m_transactionMap;
    
    /**
     * the cohort object multicast
     */
    CdmwFT::Transaction::TPCCohort_var m_cohortObject;

    // Ace Scheduler for (update, remove, insert) requests 
    Scheduler::Scheduler* m_reqScheduler;
    
    // timer
    Timer* m_timer;
    // timeout
    OsSupport::OS::Timeval m_timeout;

    // is_activated
    bool m_is_activated;

    // is_synch : true if request to the cohort are synchrone
    bool m_is_synch;

    /**
     * The mutex used to protect the TransactionMap
     */
    OsSupport::ReaderWriterLock m_transactionMapMutex;

    // the number of transaction in progress
    unsigned int m_current_transaction_number;

    // Mutex for number of transactions in progress
    Cdmw::OsSupport::Mutex m_transaction_number_mutex;
    
    // Condition variable for waiting for transactions to complete
    Cdmw::OsSupport::Condition m_condition;

}; // End class TransactionEngineCoordinator


}  // End namespace FT
}  // End namespace Cdmw

#endif

