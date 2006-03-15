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

#include "ftstatemanager/TransactionEngineCoordinator.hpp"
#include "ftstatemanager/TransactionCoordinator.hpp"
#include "ftstatemanager/DataStoreContext.hpp"
#include "ftstatemanager/MIOPMutex.hpp"

#include <FaultTolerance/ftcommon/FTLogger.hpp>

namespace Cdmw {
namespace FT {

// Helper class to provide exception safe transaction counting
class TransactionCounterGuard
{
public:
    TransactionCounterGuard(Cdmw::OsSupport::Mutex &mutex,
                            Cdmw::OsSupport::Condition& condition,
                            unsigned int& number,
                            unsigned int max)
        : m_transaction_number_mutex(mutex),
          m_condition(condition),
          m_current_transaction_number(number)
    {
        CDMW_MUTEX_GUARD(m_transaction_number_mutex);
        // PCR-0440: Hold caller if m_current_transaction_number exceeded.
        while (m_current_transaction_number >= max)
        {
            CDMW_WARN(FTLogger::GetLogger(),"Maximum number of transactions '" << max <<
                       "' exceeded! Waiting for a transaction to complete.");
            m_condition.wait();
        }
        ++m_current_transaction_number;
    }

    ~TransactionCounterGuard()
    { 
        CDMW_MUTEX_GUARD(m_transaction_number_mutex);
        if (--m_current_transaction_number == 0)
        {
            // Release threads blocked in
            // wait_for_no_transactions_in_progress
            m_condition.broadcast();
        }
    }

private:
    // Mutex for number of transactions in progress
    Cdmw::OsSupport::Mutex& m_transaction_number_mutex;

    // Condition variable for waiting for transactions to complete
    Cdmw::OsSupport::Condition& m_condition;

    // the number of transaction in progress
    unsigned int& m_current_transaction_number;

    // No default C-tor, Copy C-tor, and = operator
    TransactionCounterGuard();
    TransactionCounterGuard(const TransactionCounterGuard  &);
    TransactionCounterGuard & operator=(const TransactionCounterGuard &);
};


TransactionEngineCoordinator::TransactionEngineCoordinator
(DataStoreContext&       context,
 CORBA::ORB_ptr          orb,
 PortableServer::POA_ptr poa,
 std::string             cohort_corbaloc,
 int                     max_transaction_in_progress,
 int                     max_transaction_done,
 OsSupport::OS::Timeval  timeout,
 unsigned short          nbthreads)
    throw(CORBA::SystemException)
    : TransactionEngine(orb, poa, max_transaction_in_progress, 
                        max_transaction_done),
      m_context(context),
      m_timeout(timeout),
      m_is_activated(false),
      m_current_transaction_number(0),
      m_condition(m_transaction_number_mutex)
{
    m_is_synch = true;
    if (cohort_corbaloc != "")
    {
        m_is_synch = false;
        CORBA::Object_var group = m_orb->string_to_object(cohort_corbaloc.c_str());
        m_cohortObject = CdmwFT::Transaction::TPCCohort::_unchecked_narrow (group.in());
        
        if (CORBA::is_nil (m_cohortObject.in ()))
        {
            CDMW_WARN(FTLogger::GetLogger(),"m_cohortObject is nil");
            // todo
        }
    }
    
    m_timer = new Timer();
    
    m_reqScheduler = new Scheduler::Scheduler(m_orb.in(), this, nbthreads);
    m_reqScheduler->start();

}

TransactionEngineCoordinator::~TransactionEngineCoordinator()
{
    CDMW_DEBUG(FTLogger::GetLogger(),"~TransactionEngineCoordinator");    
    m_timer->stop();
    m_timer->wait();
    delete m_timer;
    m_timer = 0;
    if (m_reqScheduler!=NULL)
    {
        m_reqScheduler->stop(true);
        delete m_reqScheduler;
        m_reqScheduler = 0;
    }


    CDMW_DEBUG(FTLogger::GetLogger(),"Delete m_transactionMap");    
    TransactionCoordMap::iterator it_tcm;
    for (it_tcm = m_transactionMap.begin(); it_tcm != m_transactionMap.end(); it_tcm++)
    {
        delete it_tcm->second;
        it_tcm->second = 0;
    }
}


TransactionEngineCoordinator::TransactionCoordinatorList
TransactionEngineCoordinator::get_transactions_with_matching_oid
(const CdmwFT::Transaction::StateChanges& change)
{
    TransactionCoordinatorList result;

    // get_transaction_with_oid must have a reader lock for transactionMapMutex
    // but on_failure and end already use writer lock before calling this method
    // so we will read-lock where this method is used in request below

    TransactionCoordMap::iterator it_tcm;
    for (it_tcm = m_transactionMap.begin(); it_tcm != m_transactionMap.end();
         it_tcm++)
    {
        if (m_context.oids_match(change, it_tcm->second->get_change()))
        {
            result.push_back(it_tcm->second);
        }
    }

    return result;
}

bool TransactionEngineCoordinator::request
(const CdmwFT::Transaction::StateChanges& change) 
    throw (InternalErrorException, ValidationFailureException)
{
    CDMW_DEBUG(FTLogger::GetLogger(),"TransactionEngineCoordinator::request");
    Scheduler::TransactionResult tr;
    ACE_Future<Scheduler::TransactionResult> future;

    tr.status = Scheduler::ERROR;
    tr.reason = "remove TransactionResult initialisation";

    TransactionCounterGuard guard(m_transaction_number_mutex, m_condition,
                                  m_current_transaction_number,
                                  m_max_transaction_in_progress);

    {
        CDMW_WRITER_LOCK_GUARD(m_transactionMapMutex);
        TransactionCoordinatorList blocking_transactions;
        {
            // get_transaction_with_oid called with transactionMapMutex
            // read lock. See comment in get_transactions_with_matching_oid
            blocking_transactions =
                get_transactions_with_matching_oid(change);
        }
        
        // validate change - too slow?
        if (blocking_transactions.empty() && !m_context.validate(change))
        {
            throw ValidationFailureException();
        }
        
        do_request_i(change, future, blocking_transactions);
    }
    
    future.get(tr);


    return (tr.status == Scheduler::COMMITTED);

}

void TransactionEngineCoordinator::do_request_i
(const CdmwFT::Transaction::StateChanges& change,
 ACE_Future< Scheduler::TransactionResult >& future,
 TransactionCoordinatorList blocking_transactions) 
    throw (InternalErrorException)
{
    CDMW_DEBUG(FTLogger::GetLogger(),"TransactionEngineCoordinator<Oid, Data>::do_request_i");
    CdmwFT::Transaction::TransactionIdentification id;
    id.id=next_counter();
    id.initiator=m_sender_name.c_str();

    // create the transaction object
    TransactionCoordinator* p_transaction =
        new TransactionCoordinator(id, this, m_context.m_cohorts_mutex, change,
                                   blocking_transactions.size(), future);

    m_transactionMap[id] = p_transaction;

    TransactionCoordinatorList::iterator iter;
    for (iter = blocking_transactions.begin();
         iter != blocking_transactions.end();
         ++iter)
    {
        (*iter)->add_blocked(p_transaction);
    }

    if (p_transaction->is_blocked())
    {
        CDMW_DEBUG(FTLogger::GetLogger(),"do_request_i : WAITING");
        p_transaction->set_internal_status(WAITING);
    }
    else
    {
        CDMW_DEBUG(FTLogger::GetLogger(),"do_request_i : COLLECTING");
        p_transaction->set_internal_status(COLLECTING);
    }
}

void TransactionEngineCoordinator::commit_local
(const CdmwFT::Transaction::StateChanges& change)
{
    for (CORBA::ULong i = 0; i < change.length(); ++i)
    {
        DataStoreBase* ds = m_context.get_datastore(change[i].dsid);

        ds->load_state(change[i].records);
    }
}

void TransactionEngineCoordinator::delegate_request
(const CdmwFT::Transaction::TransactionIdentification & id,
 const CdmwFT::Transaction::StateChanges& change,
 DataStoreContext::LocationList cohorts)
{
    CDMW_DEBUG(FTLogger::GetLogger(),"TransactionEngineCoordinator::delegate_request");

    // if the request is synchronous
    if (m_is_synch)
    {
        // create an iterator on the cohort list
        DataStoreContext::LocationList::iterator iter_location;
        // for each cohort
        for (iter_location = cohorts.begin(); iter_location != cohorts.end();
             ++iter_location)
        {
            TransactionEngineRef engineref;
            try
            {
                //take the cohort reference for this location
                engineref = m_context.get_ref_for_location(*iter_location);

                m_reqScheduler->prepare(engineref.cohort.in(), change, id,
                                        *iter_location);
            } catch (const Cdmw::FT::NotFoundException& e)
            {
                CDMW_DEBUG(FTLogger::GetLogger()," ----> Location  " << *iter_location 
                           << " NotFound !! Cannot initiate transaction "
                           << id.id);            
            }
        }
            
    }
    else
    {
        CDMW_FT_BEGIN_MIOP_MUTEX;
        m_cohortObject->send_prepare(id, change);
        CDMW_FT_END_MIOP_MUTEX;
    }

}

void TransactionEngineCoordinator::on_failure(const std::string& location) 
{
    CDMW_DEBUG(FTLogger::GetLogger(),"[TransactionEngineCoordinator] Failure on the location:"<<location);
    TransactionCoordMap::iterator it_tcm;

    bool status_changed;
    CDMW_WRITER_LOCK_GUARD(m_transactionMapMutex);

    for (it_tcm = m_transactionMap.begin(); it_tcm != m_transactionMap.end(); )
    {
        status_changed = it_tcm->second->on_failure(location);
        if (status_changed)
        {
            // Check if transaction is now done
            TransactionState newState = it_tcm->second->get_state();
            if (newState == DONE_COMMITTED)
            {
                save_committed_trans(it_tcm->first);
                delete it_tcm->second;
                // The it_tcm++ (and not ++it_tcm) must be done inside erase
                // to continue loop with a good iterator
                m_transactionMap.erase(it_tcm++);
            }
            else if (newState == DONE_ABORTED)
            {
                save_aborted_trans(it_tcm->first);
                delete it_tcm->second;
                m_transactionMap.erase(it_tcm++);
            }
            else
            {
                it_tcm++;
            }
        }
        else
        {
            it_tcm++;
        }

    }
}

void TransactionEngineCoordinator::activate
(const TransactionMap & cohortTransactions,
 const DoneTransactionList & cohortTransactionsDone) 
{
    if (!m_is_activated)
    {
        m_is_activated = true;
        m_timer->start();
        // We won't call get in this future object : we are just recovering a transaction
        ACE_Future< Scheduler::TransactionResult > future;
        // Activate called when cohort becomes coordinator : if there is any transactions
        // in m_transactionMap they are obsolete
        CDMW_WRITER_LOCK_GUARD (m_transactionMapMutex);
        m_transactionMap.clear();
        TransactionMap::const_iterator it_tm;
        for (it_tm = cohortTransactions.begin(); it_tm != cohortTransactions.end(); it_tm++)
        {
            // create the transaction object
            TransactionCoordinator* p_transaction =
                new TransactionCoordinator(it_tm->second, it_tm->first, this,
                                           m_context.m_cohorts_mutex, future);
            m_transactionMap[it_tm->first] = p_transaction;
        }
        m_doneTransactionList = cohortTransactionsDone;
    }
}

void TransactionEngineCoordinator::send_yes
(const char * sender,
 const CdmwFT::Transaction::TransactionIdentification & id)
    throw(CORBA::SystemException)
{
    if (!m_is_activated)
        return;

    CDMW_DEBUG(FTLogger::GetLogger(),"[TransactionEngineCoordinator] : send_yes");
    CDMW_WRITER_LOCK_GUARD (m_transactionMapMutex);
    TransactionCoordMap::iterator it_tcm = m_transactionMap.find(id);
    if (it_tcm != m_transactionMap.end())
    {
        it_tcm->second->set_yes(sender);
    }
}    
    
void TransactionEngineCoordinator::send_no
(const char * sender,
 const CdmwFT::Transaction::TransactionIdentification & id)
    throw(CORBA::SystemException)
{
    if (!m_is_activated)
        return;

    CDMW_DEBUG(FTLogger::GetLogger(),"[TransactionEngineCoordinator] : send_no");
    CDMW_WRITER_LOCK_GUARD (m_transactionMapMutex);
    TransactionCoordMap::iterator it_tcm = m_transactionMap.find(id);
    if (it_tcm != m_transactionMap.end())
    {
        it_tcm->second->set_no(sender);
    }
}    
    
void TransactionEngineCoordinator::send_end
(const char * sender,
 const CdmwFT::Transaction::TransactionIdentification & id)
    throw(CORBA::SystemException)
{
    if (!m_is_activated)
        return;

    CDMW_DEBUG(FTLogger::GetLogger(),"[TransactionEngineCoordinator] : send_end "<<id.id<<" "<<sender);
    CDMW_WRITER_LOCK_GUARD (m_transactionMapMutex);
    TransactionCoordMap::iterator it_tcm = m_transactionMap.find(id);
    if (it_tcm != m_transactionMap.end())
    {
        // the transaction set_end return true if transaction is now done after this answer
        if (it_tcm->second->set_end(sender))
        {
            TransactionState newState = it_tcm->second->get_state();
            if ((newState == DONE_COMMITTED) || (newState == DONE_ABORTED))
            {
                if (newState == DONE_COMMITTED) save_committed_trans(it_tcm->first);
                else save_aborted_trans(it_tcm->first);
                
                delete m_transactionMap[id];               
                m_transactionMap.erase(id);
            }
        }
    }
}


void TransactionEngineCoordinator::send_get_transaction_status 
(const CdmwFT::Transaction::TransactionIdentification & id)
    throw(CORBA::SystemException)
{
    if (!m_is_activated)
        return;

    CDMW_DEBUG(FTLogger::GetLogger(),"[TransactionEngineCoordinator] : send_get_transaction_status");
    // if the id is not found, we will send CdmwFT::Transaction::UNKNOWN
    CdmwFT::Transaction::TransactionStatus transactionStatus = CdmwFT::Transaction::UNKNOWN;
    CDMW_READER_LOCK_GUARD (m_transactionMapMutex);
    TransactionCoordMap::iterator it_tcm = m_transactionMap.find(id);
    if (it_tcm != m_transactionMap.end())
    {
        transactionStatus = it_tcm->second->get_transaction_status();
    }
    else
    { // Search in the list of transaction done 
        DoneTransactionList::iterator iter;
        for (iter = m_doneTransactionList.begin(); iter != m_doneTransactionList.end(); iter++)
        {
            if ((*iter).id.id == id.id)
            {
                if (strcmp((*iter).id.initiator, id.initiator) == 0)
                {
                    transactionStatus = (*iter).status;
                    break;
                }
            }
        }
    }

    CDMW_FT_BEGIN_MIOP_MUTEX;
    m_cohortObject->send_set_transaction_status
        ( m_sender_name.c_str(), id, transactionStatus);
    CDMW_FT_END_MIOP_MUTEX;
}

    
void TransactionEngineCoordinator::send_set_transaction_status
(const char * sender,
 const CdmwFT::Transaction::TransactionIdentification & id,
 CdmwFT::Transaction::TransactionStatus status)
    throw(CORBA::SystemException)
{
    if (!m_is_activated)
        return;

    CDMW_DEBUG(FTLogger::GetLogger(),"[TransactionEngineCoordinator] : send_set_transaction_status");
    // When it is a new coordinator it send_get_transaction_status on transactions
    // already transfered (RECOVERING state) from the old cohort state
    // So id must be known
    CDMW_WRITER_LOCK_GUARD (m_transactionMapMutex);
    TransactionCoordMap::iterator it_tcm = m_transactionMap.find(id);
    if (it_tcm != m_transactionMap.end())
    {
        if (it_tcm->second->get_state() == RECOVERING)
        {
            it_tcm->second->set_transaction_status(sender, status);
        }
    }
}

CdmwFT::Transaction::TransactionStatus 
TransactionEngineCoordinator::get_transaction_status
(CdmwFT::Transaction::TransactionIdentification & id,
 CORBA::String_out sender)
    throw(CORBA::SystemException)
{
    if (!m_is_activated){
        CDMW_WARN(FTLogger::GetLogger(),"[TransactionEngineCoordinator] : Coordinator not active. Returning UNKNOWN.");
        return CdmwFT::Transaction::UNKNOWN;
    }
  
    // if the id is not found, we will send CdmwFT::Transaction::UNKNOWN
    CdmwFT::Transaction::TransactionStatus transactionStatus =  CdmwFT::Transaction::UNKNOWN;
    CDMW_READER_LOCK_GUARD (m_transactionMapMutex);
    CDMW_DEBUG(FTLogger::GetLogger(),"[TransactionEngineCoordinator] : get_transaction_status");
    TransactionCoordMap::iterator it_tcm = m_transactionMap.find(id);
    if (it_tcm != m_transactionMap.end())
    {
        transactionStatus = it_tcm->second->get_transaction_status();
    }
    else
    { // Search in the list of transaction done 
        DoneTransactionList::iterator iter;
        for (iter = m_doneTransactionList.begin(); iter != m_doneTransactionList.end(); iter++)
        {
            if ((*iter).id.id == id.id)
            {
                if (strcmp((*iter).id.initiator, id.initiator) == 0)
                {
                    transactionStatus = (*iter).status;
                    break;
                }
            }
        }
    }
    return transactionStatus;
}

/**
 * Purpose:
 * <p> delegate send_no
 *
 *@param sender
 *@param id
 */
void TransactionEngineCoordinator::send_no_i
(const char * sender,
 const CdmwFT::Transaction::TransactionIdentification & id)
    throw (InternalErrorException)
{
    this->send_no(sender, id);
}


/**
 * Purpose:
 * <p> delegate send_end
 *
 *@param sender
 *@param id
 */
void TransactionEngineCoordinator::send_end_i
(const char * sender,
 const CdmwFT::Transaction::TransactionIdentification & id) 
    throw (InternalErrorException)
{
    this->send_end(sender, id);
}


/**
 * Purpose:
 * <p> delegate send_end
 *
 *@param sender
 *@param id
 */
void TransactionEngineCoordinator::send_set_transaction_status_i
(const char * sender,
 const CdmwFT::Transaction::TransactionIdentification & id,
 CdmwFT::Transaction::TransactionStatus status)
    throw (InternalErrorException)
{
    this->send_set_transaction_status(sender, id, status);
}


/**
 * Purpose: Ask the cohorts to commit for the transaction
 * <p> 
 */
void TransactionEngineCoordinator::delegate_commit
(const CdmwFT::Transaction::TransactionIdentification & id,
 DataStoreContext::LocationList cohorts)
{
    // if the request is synchronous
    if (m_is_synch)
    {
        // create an iterator on the cohort list
        DataStoreContext::LocationList::iterator iter_location;
        // for each cohort
        for (iter_location = cohorts.begin(); iter_location != cohorts.end(); iter_location ++ )
        {
            //take the cohort reference for this location
            TransactionEngineRef engineref =
                get_ref_for_location(id, *iter_location);

            // send commit
            m_reqScheduler->commit(engineref.cohort.in(), id,*iter_location);
        }
    }
    else
    {
        CDMW_FT_BEGIN_MIOP_MUTEX;
        m_cohortObject->send_commit(id);
        CDMW_FT_END_MIOP_MUTEX;
    }
}

/**
 * Purpose: Ask the cohorts to commit for the transaction
 * <p> 
 */
void TransactionEngineCoordinator::delegate_get_transaction_status
(const CdmwFT::Transaction::TransactionIdentification & id,
 DataStoreContext::LocationList cohorts)
{
    CDMW_DEBUG(FTLogger::GetLogger(),"Coordinator delegate_get_transaction_status for transaction " << id.id);
    // if the request is synchronous
    if (m_is_synch)
    {
        // create an iterator on the cohort list
        DataStoreContext::LocationList::iterator iter_location;
        // for each cohort
        for (iter_location = cohorts.begin(); iter_location != cohorts.end(); iter_location ++ )
        {
            //take the cohort reference for this location
            TransactionEngineRef engineref =
                get_ref_for_location(id, *iter_location);

            // send get_transaction_status
            m_reqScheduler->get_transaction_status(engineref.cohort.in(), id, *iter_location);
        }
    }
    else
    {
        CDMW_FT_BEGIN_MIOP_MUTEX;
        m_cohortObject->send_get_transaction_status(id);
        CDMW_FT_END_MIOP_MUTEX;
    }
}

/**
 * Purpose: Ask the cohorts to commit for the transaction
 * <p> 
 */
void TransactionEngineCoordinator::delegate_rollback
(const CdmwFT::Transaction::TransactionIdentification & id,
 DataStoreContext::LocationList cohorts)
{
    // if the request is synchronous
    if (m_is_synch)
    {
        // create an iterator on the cohort list
        DataStoreContext::LocationList::iterator iter_location;
        
        // for each cohort
        for (iter_location = cohorts.begin(); iter_location != cohorts.end(); iter_location ++ )
        {
            //take the cohort reference for this location
            TransactionEngineRef engineref =
                get_ref_for_location(id, *iter_location);
            // send rollback
            m_reqScheduler->rollback(engineref.cohort.in(), id, *iter_location);
        }
    }
    else
    {
        CDMW_FT_BEGIN_MIOP_MUTEX;
        m_cohortObject->send_rollback(id);
        CDMW_FT_END_MIOP_MUTEX;
    }
}

TransactionEngineRef TransactionEngineCoordinator::get_ref_for_location
(const CdmwFT::Transaction::TransactionIdentification & id,
 const std::string& location)
{
    return m_context.get_ref_for_location(location);
}

void TransactionEngineCoordinator::deactivate()
{
    if (m_is_activated)
    {
        m_is_activated = false;
        m_timer->stop();
    }
}

// return true if transaction is synch else false
bool TransactionEngineCoordinator::transaction_is_synch()
{
    return m_is_synch;
}

/**
 * Wait for transactions in progress to complete.
 */
void TransactionEngineCoordinator::wait_for_no_transactions_in_progress()
{
    CDMW_MUTEX_GUARD(m_transaction_number_mutex);

    while (m_current_transaction_number > 0)
    {
        m_condition.wait();
    }
}

unsigned long TransactionEngineCoordinator::next_counter()
{
    const unsigned long MAX_COUNTER = 0xFFFFFFFF;
    static Cdmw::OsSupport::Mutex count_mutex;
    static unsigned long counter=0L;
    CDMW_MUTEX_GUARD(count_mutex);

    if (counter == MAX_COUNTER)
        counter = 0L;
    else
        counter++;
    return counter;
}

}  // End namespace FT
}  // End namespace Cdmw
