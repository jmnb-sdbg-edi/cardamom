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
#include "Foundation/commonsvcs/datastore/DataStoreRegistry.hpp"
#include "ftstatemanager/TransactionCoordinator.hpp"

#include <FaultTolerance/ftcommon/FTLogger.hpp>

#define ECHO_TX(X) "TX[id=" << X.id <<",initiator=" << X.initiator << "] "

namespace Cdmw {
namespace FT {

TransactionCoordinator::TransactionCoordinator
(CdmwFT::Transaction::TransactionIdentification id,
 TransactionEngineCoordinator* coordinator,
 Cdmw::OsSupport::ReaderWriterLock& mutex,
 const CdmwFT::Transaction::StateChanges& change,
 int blocking_count,
 ACE_Future< Scheduler::TransactionResult > trResult,
 int timer_id)
    : Transaction(change, timer_id),
      m_tid(id),
      m_coordinator(coordinator),
      m_status(INITIAL),
      m_transactions_guard(mutex),
      m_timer(coordinator->get_timer()),
      m_trResult(trResult),
      m_timeout(coordinator->get_timeout()),
      m_blocking_count(blocking_count)
{
    if (change.length () > 0)
    {
        // get the list of the cohort location
        // TODO check this
        m_cohorts = m_coordinator->get_context().get_locations(change[0].dsid);

        // clear the cohort map response
        m_cohorts_responses.clear();

        // assign an UNKNOWN status for all the cohort responses
        DataStoreContext::LocationList::iterator it_ll;
        for (it_ll = m_cohorts.begin(); it_ll != m_cohorts.end(); it_ll++)
        {
            m_cohorts_responses[*it_ll] = UNKNOWN;
        }
    }
    else
    {
        // TODO handle failure
    }
}

TransactionCoordinator::TransactionCoordinator
(Transaction* cohortTransation,
 CdmwFT::Transaction::TransactionIdentification id,
 TransactionEngineCoordinator* coordinator,
 Cdmw::OsSupport::ReaderWriterLock& mutex,
 ACE_Future< Scheduler::TransactionResult > trResult)
    : Transaction(*cohortTransation),
      m_tid(id),
      m_coordinator(coordinator),
      m_status(INITIAL),
      m_transactions_guard(mutex),
      m_timer(coordinator->get_timer()),
      m_trResult(trResult),
      m_timeout(coordinator->get_timeout()),
      m_blocking_count(0)
{
    DataStoreContext::DataStoreIdList lst = m_coordinator->get_context().get_datastore_ids();
    DataStoreContext::DataStoreIdList::iterator iter = lst.begin();

    if (iter != lst.end())
    {
        m_cohorts = m_coordinator->get_context().get_locations(*iter);
        m_cohorts_responses.clear();
    
        DataStoreContext::LocationList::iterator it_ll;
        for (it_ll = m_cohorts.begin(); it_ll != m_cohorts.end(); it_ll++)
        {
            m_cohorts_responses[*it_ll] = UNKNOWN;
        }
    
        set_internal_status(RECOVERING);
    }
    else
    {
        // TODO handle failure
    }
}

TransactionCoordinator::~TransactionCoordinator()
{
    // if there is another transaction blocked by a deleted transcation
    // adjust the count of blocking transactions

    TransactionCoordinatorList::iterator iter;
    for (iter = m_blocked_list.begin(); iter != m_blocked_list.end(); ++iter)
    {
        (*iter)->decrement_blocking_count();

        // The affected transaction may now be unblocked
        if (!(*iter)->is_blocked())
        {
            (*iter)->set_internal_status(COLLECTING);
        }
    }
}

bool TransactionCoordinator::set_internal_status(TransactionState status)
{
    return set_internal_status(status, false);
}

bool TransactionCoordinator::set_internal_status(TransactionState status,
                                                 bool timedout)
{ // return true if status really change
    
    switch(status) {
        
    case INITIAL:
        CDMW_WARN(FTLogger::GetLogger(),ECHO_TX(m_tid) << "Warning: INITIAL state set only in constructor");
        break;
            
    case WAITING:
        CDMW_DEBUG(FTLogger::GetLogger(),ECHO_TX(m_tid) << "set_internal_status : WAITING");
        if (m_status == INITIAL) 
            {
                m_status = WAITING;
            }
        break;
        
    case RECOVERING:
        CDMW_DEBUG(FTLogger::GetLogger(),ECHO_TX(m_tid) << "set_internal_status : RECOVERING");
        if ((m_status == INITIAL) || ((m_status == RECOVERING)&&(timedout)))
            {
                clean_responses();
                m_status = RECOVERING;
                // Set Timer used to count "SET_TO_NOT_DONE"
                // About timer: m_timer is the same for all TransactionCoordinator
                // NULL is passed here instead of a TransactionIdentification because
                // we handle only one transaction in this class : m_tid
                m_timer_id = m_timer->start_timer (this, NULL, m_timeout);
                m_coordinator->delegate_get_transaction_status(m_tid, m_cohorts);        
            }
        break;
        
    case COLLECTING:
        CDMW_DEBUG(FTLogger::GetLogger(),ECHO_TX(m_tid) << "set_internal_status : COLLECTING");
        if ((m_status == INITIAL) || (m_status == WAITING) ||
            ((m_status == COLLECTING)&&(timedout)) )
            {
                clean_responses();
                m_status = COLLECTING;
                // Set Timer used to count "YES"
                m_timer_id = m_timer->start_timer (this, NULL, m_timeout);        
                m_coordinator->delegate_request(m_tid, get_change(),
                                                m_cohorts);
            }
        break;
            
    case COMMITTED:
        CDMW_DEBUG(FTLogger::GetLogger(),ECHO_TX(m_tid) << "set_internal_status : COMMITTED");
        if ((m_status == COLLECTING) || (m_status == RECOVERING) ||
            ((m_status == COMMITTED)&&(timedout)) )
            {
                clean_responses();
                m_status = COMMITTED;
                // Cancel timer used to count "YES" or "SET_TO_NOT_DONE" 
                if (!timedout) 
                    m_timer->cancel_timer(m_timer_id);
                // Set Timer used to count "END"
                m_timer_id = m_timer->start_timer (this, NULL, m_timeout);
                m_coordinator->delegate_commit(m_tid, m_cohorts);
            }      
        break;
            
    case ABORTED:
        CDMW_DEBUG(FTLogger::GetLogger(),ECHO_TX(m_tid) << "set_internal_status : ABORTED");
        if ((m_status == COLLECTING) || (m_status == RECOVERING) ||
            ((m_status == ABORTED)&&(timedout)) )
            {
                clean_responses();
                m_status = ABORTED;
                // Cancel timer used to count "YES" or "SET_TO_NOT_DONE" 
                if (!timedout) m_timer->cancel_timer(m_timer_id);
                // Set Timer used to count "END"
                m_timer_id = m_timer->start_timer (this, NULL, m_timeout);        
                m_coordinator->delegate_rollback(m_tid, m_cohorts);
            }      
        break;
            
    case DONE_COMMITTED:
        CDMW_DEBUG(FTLogger::GetLogger(),ECHO_TX(m_tid) << "set_internal_status : DONE_COMMITTED");
        if (m_status == COMMITTED)
            {
                clean_responses();
                m_status = DONE_COMMITTED;
                // Cancel timer used to count "END" 
                m_timer->cancel_timer(m_timer_id);
                // Write future object to unlock caller
                Scheduler::TransactionResult tr;
                tr.status = Scheduler::COMMITTED;
                tr.reason = "TransactionCoordinator set_internal_status";
                m_trResult.set(tr);        
            }      
        break;
            
    case DONE_ABORTED:
        CDMW_DEBUG(FTLogger::GetLogger(),ECHO_TX(m_tid) << "set_internal_status : DONE_ABORTED");
        if (m_status == ABORTED)
            {
                clean_responses();
                m_status = DONE_ABORTED;
                // Cancel timer used to count "END"
                m_timer->cancel_timer(m_timer_id);
                // Write future object to unlock caller
                Scheduler::TransactionResult tr;
                tr.status = Scheduler::ABORTED;
                tr.reason = "TransactionCoordinator set_internal_status";
                m_trResult.set(tr);        
            }      
        break;
            
    }
    
    return (m_status == status);
}

bool TransactionCoordinator::check_status_change()
{
    // result is true if a change occurs
    bool result = false;
    CohortsResponsesMap::iterator it_crm;
    
    switch(m_status) {
        
        case INITIAL:
        case WAITING:
            break;
            
        case RECOVERING:
            CDMW_DEBUG(FTLogger::GetLogger(),ECHO_TX(m_tid) << "check_status_change : RECOVERING");
            result = true;
            for (it_crm = m_cohorts_responses.begin();
                 it_crm != m_cohorts_responses.end();
                 it_crm++)
            {
                if (it_crm->second != SET_TO_NOT_DONE)
                {
                    result = false;
                    break;
                }
            }
            // If all cohorts have answer SET_TO_NOT_DONE we change to ABORTED status
            if (result)
            {
                set_internal_status(ABORTED);
            }
            break;
            
        case COLLECTING:
            CDMW_DEBUG(FTLogger::GetLogger(),ECHO_TX(m_tid) << "check_status_change : COLLECTING");
            result = true;
            for (it_crm = m_cohorts_responses.begin();
                 it_crm != m_cohorts_responses.end();
                 it_crm++)
            {
                if (it_crm->second != YES)
                {
                    result = false;
                    break;
                }
            }
            // If all cohorts have answer YES we change to COMMITED status
            if (result)
            {
                set_internal_status(COMMITTED);
            }
            break;
            
        case COMMITTED:
        case ABORTED:
            CDMW_DEBUG(FTLogger::GetLogger(),ECHO_TX(m_tid) << "check_status_change : COMMITTED ABORTED");
            result = true;
            for (it_crm = m_cohorts_responses.begin();
                 it_crm != m_cohorts_responses.end();
                 it_crm++)
            {
                if (it_crm->second != END)
                {
                    result = false;
                    break;
                }
            }
            // If all cohorts have answer END we change to DONE status
            if (result)
            {
                if (m_status == COMMITTED) set_internal_status(DONE_COMMITTED);
                else set_internal_status(DONE_ABORTED);
            }
            break;

        case DONE_COMMITTED:
        case DONE_ABORTED:
            CDMW_DEBUG(FTLogger::GetLogger(),ECHO_TX(m_tid) << "check_status_change : DONE_COMMITTED DONE_ABORTED");
            break;

    }
    return result;
}

CdmwFT::Transaction::TransactionStatus
TransactionCoordinator::get_transaction_status()
{
    CdmwFT::Transaction::TransactionStatus status = CdmwFT::Transaction::UNKNOWN;
    switch(m_status) {

        case INITIAL:
        case WAITING:
        case RECOVERING:
            // Do nothing : status already set to UNKNOWN
            break;
            
        case COLLECTING:
            status = CdmwFT::Transaction::PREPARED;
            break;

        case COMMITTED:
        case DONE_COMMITTED:
            status = CdmwFT::Transaction::COMMITTED;
            break;

        case ABORTED:
        case DONE_ABORTED:
            status = CdmwFT::Transaction::ABORTED;
    }
    return status;
}

bool TransactionCoordinator::set_transaction_status
(const char* sender,
 CdmwFT::Transaction::TransactionStatus status)
{
    // result is true if status really change
    bool result = false;
    std::string str_sender = sender;
    CohortsResponsesMap::iterator it_crm =
    m_cohorts_responses.find(str_sender);

    if (it_crm != m_cohorts_responses.end())
    {
        if (m_status == RECOVERING)
        {
            result = true;
            if (status == CdmwFT::Transaction::COMMITTED)   
            {
                set_internal_status(COMMITTED);
                // we localy modify datastore
                // we can do this even if transaction not finished
                m_coordinator->commit_local(get_change());
            }
            else if (status == CdmwFT::Transaction::ABORTED)
            {
                set_internal_status(ABORTED);
            }
            else
            {
                // Record the specified sender has answer
                // (something else than COMMITTED or ABORTED)
                it_crm->second = SET_TO_NOT_DONE;
                // If no commited or aborted answer arrive we abort
                // when all cohorts have answer
                result = check_status_change();
            }
        }
        else
        {
            CDMW_WARN(FTLogger::GetLogger(),ECHO_TX(m_tid) << "Warning: set_transaction_status received while not RECOVERING!");
        }
    }
    return result;
}

bool TransactionCoordinator::set_yes(const char* sender)
{
    // result is true if the status change to committed
    bool result = false;
    std::string str_sender = sender;
    CohortsResponsesMap::iterator it_crm =
    m_cohorts_responses.find(str_sender);
    if (it_crm != m_cohorts_responses.end())
    {
        if (m_status == COLLECTING) 
        {
            // Response is set to Yes for the sender
            it_crm->second = YES;
            // Status will change if all cohorts answer yes
            result = check_status_change();
        }
    }
    return result;
}
  
bool TransactionCoordinator::set_no(const char* sender)
{
    // result is true if sender is in the map and we expect a "no"
    bool result = false;
    std::string str_sender = sender;
    CohortsResponsesMap::iterator it_crm =
    m_cohorts_responses.find(str_sender);

    if (it_crm != m_cohorts_responses.end())
    {
        if (m_status == COLLECTING) 
        {
            result = true;
            // Response is set to No for the sender
            it_crm->second = NO;
            // A cohort has answered No so we abort transaction
            set_internal_status(ABORTED);
        }
    }
    return result;
}
  
bool TransactionCoordinator::set_end(const char* sender)
{
    // result is true if status change to Done
    bool result = false;
    std::string str_sender = sender;
    CohortsResponsesMap::iterator it_crm =
    m_cohorts_responses.find(str_sender);

    if (it_crm != m_cohorts_responses.end())
    {
        if ((m_status == COMMITTED) || (m_status == ABORTED))
        {
            // Response is set to Yes for the sender
            it_crm->second = END;
            // Status will change if all cohorts answer end
            result = check_status_change();
        }
    }
    return result;
}

bool TransactionCoordinator::on_failure(const std::string& location)
{
    // result is true if failure leads to a status change
    bool result = false;
    std::string str_sender = location;
    m_cohorts.erase(location);
    CohortsResponsesMap::iterator it_crm =
    m_cohorts_responses.find(str_sender);
    if (it_crm != m_cohorts_responses.end())
    {
        m_cohorts_responses.erase(location);
        result = check_status_change();
    }
    return result;
}

bool TransactionCoordinator::check_status_after_on_failure()
{
    return check_status_change();
}


int TransactionCoordinator::handle_timeout
(const CdmwFT::Transaction::TransactionIdentification* tid)
{
    // Warning: as we have passed NULL to start_timer here, don't use tid but m_tid
    switch(m_status) {

    case INITIAL:
    case WAITING:
        CDMW_WARN(FTLogger::GetLogger(),ECHO_TX(m_tid) << "Warning: No timeout in this state !");
        break;

    case RECOVERING:
        CDMW_DEBUG(FTLogger::GetLogger(),ECHO_TX(m_tid) << "Timeout in RECOVERING state");
        set_internal_status(RECOVERING, true);      
        break;
        
    case COLLECTING:
        CDMW_DEBUG(FTLogger::GetLogger(),ECHO_TX(m_tid) << "Timeout in COLLECTING state");
        set_internal_status(COLLECTING, true);      
        break;
        
    case COMMITTED:
        CDMW_DEBUG(FTLogger::GetLogger(),ECHO_TX(m_tid) << "Timeout in COMMITTED state");
        set_internal_status(COMMITTED, true);
        break;
        
    case ABORTED:
        CDMW_DEBUG(FTLogger::GetLogger(),ECHO_TX(m_tid) << "Timeout in ABORTED state");
        set_internal_status(ABORTED, true);
        break;
        
    case DONE_COMMITTED:
    case DONE_ABORTED:
        CDMW_WARN(FTLogger::GetLogger(),ECHO_TX(m_tid) << "Warning: No timeout in this state !");
        break;
        
    }
    return 0;
}

}  // End namespace FT
}  // End namespace Cdmw
