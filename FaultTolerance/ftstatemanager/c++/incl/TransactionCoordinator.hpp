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

#ifndef INCL_CDMW_FT_FTSTATEMANAGER_TRANSACTION_COORDINATOR_HPP
#define INCL_CDMW_FT_FTSTATEMANAGER_TRANSACTION_COORDINATOR_HPP

#include <map>
#include <list>

#include <Foundation/osthreads/ReaderLockGuard.hpp>
#include <FaultTolerance/ftstatemanager/Timer.hpp>
#include <FaultTolerance/ftstatemanager/Transaction.hpp>
#include <FaultTolerance/ftstatemanager/TransactionEngineCoordinator.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreBase.hpp>
#include <FaultTolerance/idllib/CdmwFTTPCCommon.stub.hpp>
#include <FaultTolerance/ftstatemanager/Transaction_Result.hpp>
#include <FaultTolerance/ftcommon/MacroDef.hpp>
#include <ace/Future.h>


/**
*Purpose:
* TransactionCoordinator .
*<p>
*
*Features:
*<p> Thread safe. Exception safe.
*/
namespace Cdmw {
namespace FT {

  // Cohorts Responses
  enum ResponseType {
    UNKNOWN,
    SET_TO_NOT_DONE,
    YES,
    NO,
    END
  };

  enum TransactionState {
    // Initial state = CdmwFT::Transaction::UNKNOWN
    INITIAL,

    // If another transaction with same oid is in progress = CdmwFT::Transaction::UNKNOWN
    WAITING,         

    // (new) Coordinator must recover transaction status from Cohorts
    RECOVERING,     // = CdmwFT::Transaction::UNKNOWN

    // Coordinator has call prepare_X = CdmwFT::Transaction::PREPARED
    COLLECTING,

    // All cohorts agreed = CdmwFT::Transaction::COMMITTED
    COMMITTED,

    // At least one cohort replied "no" = CdmwFT::Transaction::ABORTED
    ABORTED,

    // All cohorts send "end" message after commit = CdmwFT::Transaction::COMMITTED
    DONE_COMMITTED,

    // All cohorts send "end" message after abort = CdmwFT::Transaction::ABORTED
    DONE_ABORTED,
          
  };

class TransactionEngineCoordinator;

class TransactionCoordinator
  : public Transaction,
    public Cdmw::FT::Timer_Event_Handler<CdmwFT::Transaction::TransactionIdentification>
{
    typedef Timer<CdmwFT::Transaction::TransactionIdentification> Timer;

public:

    typedef std::list<TransactionCoordinator*> TransactionCoordinatorList;

    /**
    * Purpose:
    * <p> Constructor called when a transaction begins
    *     This constructor will initiate transaction in INITIAL state
    *
    */
    TransactionCoordinator(CdmwFT::Transaction::TransactionIdentification id,
                           TransactionEngineCoordinator* coordinator,
                           OsSupport::ReaderWriterLock& mutex,
                           const CdmwFT::Transaction::StateChanges& change,
                           int blocking_count,
                           ACE_Future< Scheduler::TransactionResult > trResult,
                           int timer_id = -1);
    /**
    * Purpose:
    * <p> Constructor
    *     called when the (new) coordinator recovers cohort transactions
    *     This constructor will initiate transaction in RECOVERING state
    */
    TransactionCoordinator(Transaction* cohortTransation,
                           CdmwFT::Transaction::TransactionIdentification id,
                           TransactionEngineCoordinator* coordinator,
                           OsSupport::ReaderWriterLock& mutex,
                           ACE_Future< Scheduler::TransactionResult > trResult);

    /**
     * Destructor. Updates TransactionCoordinators blocked by
     * this TransactionCoordinator
     */
    virtual ~TransactionCoordinator();

    /**
    * Purpose:
    * <p> record the yes answer of the cohort if he was in collecting state
    */
    bool set_yes(const char* sender);

    /**
    * Purpose:
    * <p> record the yes answer of the cohort if he was in collecting state
    */
    bool set_no(const char* sender);

    /**
    * Purpose:
    * <p> record the end answer of the cohort if he was commited or aborted
    */
    bool set_end(const char* sender);

    TransactionState get_state()
    {
      return m_status;
    }

    /**
    * Purpose:
    * <p> Map the internal Transaction State to the Transaction status used in Idl
    */
    CdmwFT::Transaction::TransactionStatus get_transaction_status();

    /**
    * Purpose:
    * <p> Cohorts set the status from recovering state
    */
    bool set_transaction_status(const char* sender,
                                CdmwFT::Transaction::TransactionStatus status);

    /**
    * Purpose:
    * <p> set the status and eventually cancel_timer, start a new one, clean..
    */
    bool set_internal_status(TransactionState status);

    /**
    * Purpose:
    * <p> Remove the corresponding cohort entry from the Response Map
    */
    bool on_failure(const std::string& location);

    /**
    * Purpose:
    * <p> Remove the corresponding cohort entry from the Response Map
    */
    bool check_status_after_on_failure();

    /**
    * Purpose:
    * <p> Decrement count of transactions blocking this transaction
    */
    void decrement_blocking_count()
    {
        --m_blocking_count;
    }

    /**
    * Purpose:
    * <p> Return true if transaction is blocked by another
    */
    bool is_blocked()
    {
        return m_blocking_count > 0;
    }

    /**
    * Purpose:
    * <p> Add to list of transactions blocked by this transaction
    */
    void add_blocked(TransactionCoordinator* blocked)
    {
        m_blocked_list.push_back(blocked);
    }

  private:

    /**
    * Purpose:
    * <p> Default constructor.
    */
    TransactionCoordinator(OsSupport::ReaderWriterLock& mutex)
      : m_transactions_guard(mutex)
    {
    }

    /**
    * Purpose:
    * <p> set all responses to unknown
    */
    void clean_responses()
    {
        for (CohortsResponsesMap::iterator it_crm = m_cohorts_responses.begin();
             it_crm != m_cohorts_responses.end();
             it_crm++) {
            it_crm->second = UNKNOWN;
        }
    }

    /**
    * Purpose:
    * <p> Check if status can change because of tests concerning
    *     the whole set of cohorts
    */
    bool check_status_change();

    /**
    * Purpose:
    * <p> set the status and eventually cancel_timer, start a new one, clean..
    */
    bool set_internal_status(TransactionState status, bool timedout);

    /**
    * Purpose:
    * <p> virtual method of the inherited Cdmw::FT::Timer_Event_Handler class
    *     which is called back by the Reactor when timeout occurs.
    *     
    */
    int handle_timeout(const CdmwFT::Transaction::TransactionIdentification* tid);

    
    /**
    * Assignement operator is not allowed.
    */
    TransactionCoordinator& operator=(const TransactionCoordinator& rhs);

    CdmwFT::Transaction::TransactionIdentification m_tid;
    TransactionEngineCoordinator* m_coordinator;

    typedef std::map<std::string, ResponseType> CohortsResponsesMap;

    DataStoreContext::LocationList m_cohorts;
    // Cohorts responses map
    CohortsResponsesMap m_cohorts_responses;
    // Transaction status
    TransactionState m_status;

    // This guard will protect against location changes for
    // ALL transactions of ALL datastores (by using DataStoreBase::M_cohorts_mutex)
    OsSupport::ReaderLockGuard m_transactions_guard;

    Timer* m_timer;
    ACE_Future< Scheduler::TransactionResult > m_trResult;
    OsSupport::OS::Timeval m_timeout;

    int m_blocking_count;

    TransactionCoordinatorList m_blocked_list;
 
}; // End class TransactionCoordinator

}  // End namespace FT
}  // End namespace Cdmw

#endif

