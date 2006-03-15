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

#include "ftstatemanager/TransactionEngineCohort.hpp"
#include "ftstatemanager/DataStoreContext.hpp"
#include "ftstatemanager/MIOPMutex.hpp"

#include <FaultTolerance/ftcommon/FTLogger.hpp>

namespace Cdmw {
namespace FT {

TransactionEngineCohort::TransactionEngineCohort
(DataStoreContext& context,
 CORBA::ORB_ptr orb,
 PortableServer::POA_ptr poa,
 std::string coordinator_corbaloc,
 int max_transaction_in_progress,
 int max_transaction_done,
 OsSupport::OS::Timeval timeout)
    throw(CORBA::SystemException)
    : TransactionEngine(orb, poa, max_transaction_in_progress,
                        max_transaction_done),
      m_context(context)
{
    m_timeout = timeout;
    m_is_activated = false;

    m_is_synch = true;
    if (coordinator_corbaloc != "")
    {
        m_is_synch = false;
        CORBA::Object_var group = m_orb->string_to_object(coordinator_corbaloc.c_str());
        m_coordinatorObject = CdmwFT::Transaction::TPCCoordinator::_unchecked_narrow (group.in());
        
        if (CORBA::is_nil (m_coordinatorObject.in ()))
        {
            CDMW_WARN(FTLogger::GetLogger(),"m_coordinatorObject is nil");
            // todo
        }
    }

    // create the timer for the timeout on the transaction
    m_timer_handler = new Time_Handler(this, m_is_synch);
    m_timer = new Timer();
}


/**
 * Purpose:
 * <p> Destructor.
 */
TransactionEngineCohort::~TransactionEngineCohort()
{
    CDMW_DEBUG(FTLogger::GetLogger(),"Stop timer");

    m_timer->stop();
    delete m_timer;
    m_timer = NULL;
    if (m_timer_handler!=NULL)
    {
        delete m_timer_handler;
        m_timer_handler = NULL;
    } 
    CDMW_DEBUG(FTLogger::GetLogger(),"End ~TransactionEngineCohort()");
}

/**
 * Purpose:
 * <p> get the transaction map (to be used when cohort becomes coordinator)
 *
 */
TransactionEngineCohort::TransactionMap
TransactionEngineCohort::get_cohort_transactions() 
{
    return m_transactionMap; 
}

int Time_Handler::handle_timeout
(const CdmwFT::Transaction::TransactionIdentification* tid)
{
    CDMW_DEBUG(FTLogger::GetLogger(),"------>  handle_timeout for transaction: " << tid->id
               << " from " << tid->initiator);
    CdmwFT::Transaction::TransactionIdentification id;
    id.id = tid->id;
    id.initiator = tid->initiator;
    // this pointer is no more used after timeout : new one created in on_timeout
    delete tid;
    m_cohort->on_timeout(id, m_is_synch);
    return 0; 
}

void TransactionEngineCohort::on_failure(const std::string& location) 
{
    CDMW_INFO(FTLogger::GetLogger(),"[TransactionEngineCohort] Failure on the location:"<<location);
}

void TransactionEngineCohort::on_timeout
(const CdmwFT::Transaction::TransactionIdentification & id, bool is_synch) 
{
    CDMW_INFO(FTLogger::GetLogger(),"[TransactionEngineCohort] Timeout on the request: " << id.id
              << " initiated by " << id.initiator << "." );
    CDMW_MUTEX_GUARD (m_mutex);
    
/******** correction for PCR-0368 & PCR-0369 ********/

    TransactionMap::iterator iter = m_transactionMap.find(id);
    if (iter !=  m_transactionMap.end())
    {
        if (!is_synch)
        {
            // re-start timer
            CdmwFT::Transaction::TransactionIdentification* temp_id =
                new CdmwFT::Transaction::TransactionIdentification(id);

            int timer_id = m_timer->start_timer (m_timer_handler, temp_id,
                                                 m_timeout);
            //std::cout << "## on_timeout: start_timer->timer_id: " << timer_id << std::endl;
            iter->second->set_timer_id(timer_id);

            CDMW_FT_BEGIN_MIOP_MUTEX;
            m_coordinatorObject->send_get_transaction_status(id);
            CDMW_FT_END_MIOP_MUTEX;
        }
        else
        {
            std::string sender_s (id.initiator);
            std::string coordinator = m_context.get_coordinator_location();

            CDMW_INFO(FTLogger::GetLogger(),
                      "[TransactionEngineCohort] Timeout on the request: " 
                      << id.id
                      << ", asking the status to coordinator:"
                      << coordinator);
            try
            {
                TransactionEngineRef engineref =
                    m_context.get_ref_for_location(coordinator.c_str());
                CORBA::String_var sender;
                CdmwFT::Transaction::TransactionIdentification id_temp = id;
                CdmwFT::Transaction::TransactionStatus status =
                    engineref.coordinator->get_transaction_status
                    (id_temp, sender.out());
                set_transaction_status_i(id.initiator, id, status, !is_synch);
            }
            catch (const NotFoundException&)
            {
                CDMW_WARN(FTLogger::GetLogger(),"[TransactionEngineCohort] Timeout on the request: "
                          << id.id << " no ref for coordinator " << coordinator );
            }
        }
    }
    else
    {
        // This timer has already been canceled.
        // The transaction has already been moved in transactionDone map.
        CDMW_DEBUG(FTLogger::GetLogger(),"[TransactionEngineCohort] Timeout on the request: "
                   << id.id << " not in map" );
 
    }
/****************************************************/    
}

CORBA::Boolean 
TransactionEngineCohort::prepare_i
(const CdmwFT::Transaction::TransactionIdentification & id,
 const CdmwFT::Transaction::StateChanges& change,
 bool is_asynchronous)
    throw(CORBA::SystemException)
{ 
    CDMW_DEBUG(FTLogger::GetLogger(),"  ----> prepare_i for Transaction: " << id.id
               << " from " << id.initiator);
    
    try
    {
        if (!m_context.validate(change))
        {
            // if the data exist, send no
            if (is_asynchronous) {
                CDMW_FT_BEGIN_MIOP_MUTEX;
                m_coordinatorObject->send_no(m_sender_name.c_str(), id);
                CDMW_FT_END_MIOP_MUTEX;
            }
            CDMW_WARN(FTLogger::GetLogger(), "   --->"<<m_sender_name.c_str()<<" send no : object already in the map");

            // insert the transaction into the DoneTransaction list with the ABORTED status
            save_aborted_trans(id);
            return false;
        }
        else {
            // the oid is not found => send_prepared is OK
            // => continue
        }
    }
    catch(...)
    {
        // the oid is not found => send_prepared is no OK
        if (is_asynchronous) {
            CDMW_FT_BEGIN_MIOP_MUTEX;
            m_coordinatorObject->send_no(m_sender_name.c_str(), id);
            CDMW_FT_END_MIOP_MUTEX;
        }
        // inset the transaction into the DoneTransaction list with the ABORTED status
        save_aborted_trans(id);
        return false;
    }

    // check if a transaction is already in progress
    {
        TransactionMap::iterator iter = m_transactionMap.find(id);
        if (iter !=  m_transactionMap.end())
        {
            if (is_asynchronous) {
                CDMW_FT_BEGIN_MIOP_MUTEX;
                m_coordinatorObject->send_yes(m_sender_name.c_str(), id);
                CDMW_FT_END_MIOP_MUTEX;
            }
            CDMW_DEBUG(FTLogger::GetLogger(),"   --->"<<m_sender_name.c_str()<<" send_yes ");

            // Restart the timer
            // no need to create again the transaction identifier for the request
            // because we will have it via the &p_tid of cancel_timer
            const void* p_tid;
            int cancel_succeess = m_timer->cancel_timer((*iter).second->get_timer_id(), &p_tid);
            if (!cancel_succeess) {
                // Timer already expired, p_tid was deleted. Recreate it.
                p_tid = new CdmwFT::Transaction::TransactionIdentification(id);
            }
            
            int timer_id = m_timer->start_timer (m_timer_handler, (CdmwFT::Transaction::TransactionIdentification*)p_tid, m_timeout);

            // update the transaction object
            (*iter).second->set_timer_id(timer_id);
            return true;
        }

        for (iter = m_transactionMap.begin(); iter != m_transactionMap.end();
             ++iter)
        {
            if (m_context.oids_match(change, iter->second->get_change()))
            {
                break;
            }
        }

        if (iter !=  m_transactionMap.end())
        {
            if (is_asynchronous) {
                CDMW_FT_BEGIN_MIOP_MUTEX;
                m_coordinatorObject->send_no(m_sender_name.c_str(), id);
                CDMW_FT_END_MIOP_MUTEX;
            }
            CDMW_WARN(FTLogger::GetLogger(),"   --->"<<m_sender_name.c_str()<<" send no : oid already in a transaction in progress");
            // inset the transaction into the DoneTransaction list with the ABORTED status
            save_aborted_trans(id);
            return false;
        }
    }

    // check if the number of transaction in progress is under the m_max_transaction_in_progress
    CDMW_DEBUG(FTLogger::GetLogger(),"transaction_in_progress :" << m_transactionMap.size());
    if (m_max_transaction_in_progress < m_transactionMap.size())
    {
        if (is_asynchronous) {
            CDMW_FT_BEGIN_MIOP_MUTEX;
            m_coordinatorObject->send_no(m_sender_name.c_str(), id);
            CDMW_FT_END_MIOP_MUTEX;
        }
        CDMW_WARN(FTLogger::GetLogger(),"   --->"<<m_sender_name.c_str()<<" sends no : number of transaction in progress is over the max '"
                  << m_max_transaction_in_progress << "'");
        // inset the transaction into the DoneTransaction list with the ABORTED status
        save_aborted_trans(id);
        return false;  
    }
    CDMW_DEBUG(FTLogger::GetLogger(),"insert into the map");
    // create the transaction identifier for the request
    CdmwFT::Transaction::TransactionIdentification* temp_id = new CdmwFT::Transaction::TransactionIdentification(id);

    //start the timer
    int timer_id = m_timer->start_timer (m_timer_handler, temp_id, m_timeout);

    // create the transaction object
    Transaction* current = new Transaction(change, timer_id);
    m_transactionMap.insert(std::make_pair(id, current));
    if (is_asynchronous) {
        CDMW_FT_BEGIN_MIOP_MUTEX;
        m_coordinatorObject->send_yes(m_sender_name.c_str(), id);
        CDMW_FT_END_MIOP_MUTEX;
    }
    CDMW_DEBUG(FTLogger::GetLogger(),"   --->"<<m_sender_name.c_str()<<" send_yes "<<__LINE__);
    return true;
}


void TransactionEngineCohort::commit_i
(const CdmwFT::Transaction::TransactionIdentification & id,
 bool is_asynchronous)
    throw(CORBA::SystemException)
{
    CDMW_DEBUG(FTLogger::GetLogger(),"  ----> commit_i for Transaction: " << id.id << " from "
               << id.initiator);
    
    // realise the commit for the transaction 
    // find the transaction
        
    TransactionMap::iterator iter = m_transactionMap.find(id);
    if (iter !=  m_transactionMap.end())
    {
        // realise the transaction
        try
        {
            const void* p_tidToDelete;
            int cancel_success = m_timer->cancel_timer((*iter).second->get_timer_id(), &p_tidToDelete);
            if (cancel_success && p_tidToDelete != NULL)
            {
                delete (CdmwFT::Transaction::TransactionIdentification*)p_tidToDelete;
            }

            const CdmwFT::Transaction::StateChanges& changes =
                iter->second->get_change();

            for (CORBA::ULong i = 0; i < changes.length(); ++i)
            {
                DsIdentifier dsid = changes[i].dsid;
                DataStoreBase *dataStore = m_context.get_datastore(dsid);
                dataStore->load_state(changes[i].records);
            }

            if (is_asynchronous) {
                CDMW_FT_BEGIN_MIOP_MUTEX;
                m_coordinatorObject->send_end(m_sender_name.c_str(), id);
                CDMW_FT_END_MIOP_MUTEX;
            }
                
            CDMW_DEBUG(FTLogger::GetLogger(),"send_end good");
            // inset the transaction into the DoneTransaction list with the COMMITED status
            save_committed_trans(id);
            // erase the transaction from the TransactionMap
            m_transactionMap.erase(iter);
            return;
        }
        catch(...)
        {
            if (is_asynchronous) {
                CDMW_FT_BEGIN_MIOP_MUTEX;
                m_coordinatorObject->send_end(m_sender_name.c_str(), id); 
                CDMW_FT_END_MIOP_MUTEX;
            }
            CDMW_DEBUG(FTLogger::GetLogger(),"send_end bad");
            // inset the transaction into the DoneTransaction list with the ABORTED status
            save_aborted_trans(id);
            // erase the transaction from the TransactionMap
            m_transactionMap.erase(iter);
            return;
        }

    } else {
            
        DoneTransactionList::iterator iter;
        for (iter = m_doneTransactionList.begin(); iter != m_doneTransactionList.end(); iter++)
        {
            if ((*iter).id.id == id.id)
            {
                if (strcmp((*iter).id.initiator, id.initiator) == 0)
                {
                    if (is_asynchronous) {
                        CDMW_FT_BEGIN_MIOP_MUTEX;
                        m_coordinatorObject->send_end(m_sender_name.c_str(), id); 
                        CDMW_FT_END_MIOP_MUTEX;
                    }
                    CDMW_DEBUG(FTLogger::GetLogger(),m_sender_name.c_str()<<" send_end");
                    return;
                }
                    
            }            
        }
    }

    // if no operation was realise, send end to the coordinator
    if (is_asynchronous) {
        CDMW_FT_BEGIN_MIOP_MUTEX;
        m_coordinatorObject->send_end(m_sender_name.c_str(), id);
        CDMW_FT_END_MIOP_MUTEX;
    }
}

    
void TransactionEngineCohort::rollback_i
(const CdmwFT::Transaction::TransactionIdentification & id,
 bool is_asynchronous)
    throw(CORBA::SystemException)
{
    CDMW_DEBUG(FTLogger::GetLogger(),"  ----> rollback_i for Transaction: " << id.id << " from "
               << id.initiator);
    
    TransactionMap::iterator iter = m_transactionMap.find(id);
    if (iter !=  m_transactionMap.end())
    {
        const void* p_tidToDelete;
        int cancel_success = m_timer->cancel_timer((*iter).second->get_timer_id(), &p_tidToDelete);
        if (cancel_success && p_tidToDelete != NULL) {
            delete (CdmwFT::Transaction::TransactionIdentification*)p_tidToDelete;
        }

        if (is_asynchronous) {
            CDMW_FT_BEGIN_MIOP_MUTEX;
            m_coordinatorObject->send_end(m_sender_name.c_str(), id);
            CDMW_FT_END_MIOP_MUTEX;
        }
        // inset the transaction into the DoneTransaction list with the ABORTED status
        save_aborted_trans(id);
        // erase the transaction from the TransactionMap
        m_transactionMap.erase(iter);
        return;
    } else {
            
        DoneTransactionList::iterator iter;
        for (iter = m_doneTransactionList.begin(); iter != m_doneTransactionList.end(); iter++)
        {
            if ((*iter).id.id == id.id)
            {
                if (strcmp((*iter).id.initiator, id.initiator) == 0)
                {
                    if (is_asynchronous) {
                        CDMW_FT_BEGIN_MIOP_MUTEX;
                        m_coordinatorObject->send_end(m_sender_name.c_str(), id); 
                        CDMW_FT_END_MIOP_MUTEX;
                    }
                    CDMW_DEBUG(FTLogger::GetLogger(),m_sender_name.c_str()<<" send_end");
                    return;
                }
                    
            }
                
        }
    }
    
    // if no operation was realise, send end to the coordinator
    if (is_asynchronous) {
        CDMW_FT_BEGIN_MIOP_MUTEX;
        m_coordinatorObject->send_end(m_sender_name.c_str(), id); 
        CDMW_FT_END_MIOP_MUTEX;
    }
}


CdmwFT::Transaction::TransactionStatus 
TransactionEngineCohort::get_transaction_status_i
(const CdmwFT::Transaction::TransactionIdentification & id,
 bool is_asynchronous)
    throw(CORBA::SystemException)
{
    CDMW_DEBUG(FTLogger::GetLogger(),"  ----> get_transaction_status_i for Transaction: " << id.id
               << " from " << id.initiator);
    
    // if the status is find in the TransactionMap the status is CdmwFT::Transaction::PREPARED,
    // else if the status is in the DoneTransactionMap, it is the second element of the map
    // if the id is not found, send CdmwFT::Transaction::UNKNOWN

    // search in the TransactionMap 
    {
        TransactionMap::iterator iter = m_transactionMap.find(id);
        if (iter !=  m_transactionMap.end())
        {
           if (is_asynchronous) {
                CDMW_FT_BEGIN_MIOP_MUTEX;
                m_coordinatorObject->send_set_transaction_status(m_sender_name.c_str(), id, CdmwFT::Transaction::PREPARED);
                CDMW_FT_END_MIOP_MUTEX;
           }
            return CdmwFT::Transaction::PREPARED;
        }
    }

    // search in the DoneTransactionMap
    {
        DoneTransactionList::iterator iter;
        for (iter = m_doneTransactionList.begin(); iter != m_doneTransactionList.end(); iter++)
        {
            if ((*iter).id.id == id.id)
            {
               if (strcmp((*iter).id.initiator, id.initiator) == 0)
                {
                   if (is_asynchronous) {
                        CDMW_FT_BEGIN_MIOP_MUTEX;
                       m_coordinatorObject->send_set_transaction_status(m_sender_name.c_str(), id, (*iter).status);
                        CDMW_FT_END_MIOP_MUTEX;
                    }
                    return (*iter).status;
                }
                
            }
            
        }
    }
    
    // if the id was not found in the previous map , send UNKNOWN
    if (is_asynchronous) {
        CDMW_FT_BEGIN_MIOP_MUTEX;
       m_coordinatorObject->send_set_transaction_status(m_sender_name.c_str(), id,CdmwFT::Transaction::UNKNOWN);
       CDMW_FT_END_MIOP_MUTEX;
    }
    return CdmwFT::Transaction::UNKNOWN;
}


    
void
TransactionEngineCohort::set_transaction_status_i
(const char * sender,
 const CdmwFT::Transaction::TransactionIdentification & id,
 CdmwFT::Transaction::TransactionStatus status,
 bool is_asynchronous)
    throw(CORBA::SystemException)
{
    CDMW_DEBUG(FTLogger::GetLogger(),"  ----> set_transaction_status_i for Transaction: " << id.id
               << " from " << id.initiator);
    
    switch (status){
        case CdmwFT::Transaction::UNKNOWN:
            // do nothing 
            break;

        case CdmwFT::Transaction::PREPARED:
            // ????
            break;
            
        case CdmwFT::Transaction::COMMITTED:
            {
                // check if the transaction is in progress
                // search in the TransactionMap 
                {
                    TransactionMap::iterator iter = m_transactionMap.find(id);
                    if (iter !=  m_transactionMap.end())
                    {
                        commit_i(id, is_asynchronous);
                        return;
                    }
                }
            }
        break;
        
        case CdmwFT::Transaction::ABORTED:
            {
                
                // check if the transaction is in progress
                // search in the TransactionMap 
                {
                    TransactionMap::iterator iter = m_transactionMap.find(id);
                    if (iter !=  m_transactionMap.end())
                    {
                        rollback_i(id, is_asynchronous);
                        return;
                    }
                }
            }
        break;
    }
    
}


CORBA::Boolean TransactionEngineCohort::prepare
(const CdmwFT::Transaction::StateChanges& change,
 CdmwFT::Transaction::TransactionIdentification & id,
 CORBA::String_out sender)
    throw(CORBA::SystemException)
{
    CDMW_DEBUG(FTLogger::GetLogger(),"------> prepare_insert for Transaction: " << id.id << " from "
               << id.initiator);
    
    if (!m_is_activated)
        return false;
    
    CDMW_MUTEX_GUARD (m_mutex);

    sender = CORBA::string_dup (m_sender_name.c_str());
    CORBA::Boolean  ret = prepare_i(id, change, false);
    CDMW_DEBUG(FTLogger::GetLogger(),"<---END prepare_insert for Transaction: " << id.id << " from "
               << id.initiator);

    return ret;
}


void TransactionEngineCohort::commit (CdmwFT::Transaction::TransactionIdentification & id,
                                                             CORBA::String_out sender)
    throw(CORBA::SystemException)
{
    CDMW_DEBUG(FTLogger::GetLogger(),"------> commit for Transaction: " << id.id << " from "
               << id.initiator);
    
    if (!m_is_activated)
        return ;
   
    CDMW_MUTEX_GUARD (m_mutex);

    sender = CORBA::string_dup (m_sender_name.c_str());
    commit_i(id, false);
    CDMW_DEBUG(FTLogger::GetLogger(),"<---END commit for Transaction: " << id.id << " from "
               << id.initiator); 
}

    
void TransactionEngineCohort::rollback
(CdmwFT::Transaction::TransactionIdentification & id,
 CORBA::String_out sender)
    throw(CORBA::SystemException)
{
    CDMW_DEBUG(FTLogger::GetLogger(),"------> rollback for Transaction: " << id.id << " from "
               << id.initiator);
    
    if (!m_is_activated)
        return ;
  
    CDMW_MUTEX_GUARD (m_mutex);

    sender = CORBA::string_dup (m_sender_name.c_str());
    rollback_i(id, false);
    CDMW_DEBUG(FTLogger::GetLogger(),"<---END rollback for Transaction: " << id.id << " from "
               << id.initiator);
}


CdmwFT::Transaction::TransactionStatus
TransactionEngineCohort::get_transaction_status
(CdmwFT::Transaction::TransactionIdentification & id,
 CORBA::String_out sender)
    throw(CORBA::SystemException)
{
    CDMW_DEBUG(FTLogger::GetLogger(),"------> get_transaction_status for Transaction: " << id.id
               << " from " << id.initiator);
    
   if (!m_is_activated)
        return CdmwFT::Transaction::UNKNOWN;
  
    CDMW_MUTEX_GUARD (m_mutex);

    sender = CORBA::string_dup (m_sender_name.c_str());
    CdmwFT::Transaction::TransactionStatus ret = get_transaction_status_i(id, false);
    CDMW_DEBUG(FTLogger::GetLogger(),"<---END get_transaction_status for Transaction: " << id.id
               << " from " << id.initiator);

    return ret;
}

void TransactionEngineCohort::send_prepare
(const CdmwFT::Transaction::TransactionIdentification & id,
 const CdmwFT::Transaction::StateChanges& change)
    throw(CORBA::SystemException)
{
    CDMW_DEBUG(FTLogger::GetLogger(),"------> send_prepare for Transaction: " << id.id
               << " from " << id.initiator);
    
    if (!m_is_activated)
        return;
    
    CDMW_MUTEX_GUARD (m_mutex);

    prepare_i(id, change, true);
    CDMW_DEBUG(FTLogger::GetLogger(),"<---END send_prepare for Transaction: " << id.id
               << " from " << id.initiator);
}


void TransactionEngineCohort::send_commit
(const CdmwFT::Transaction::TransactionIdentification & id)
    throw(CORBA::SystemException)
{
    CDMW_DEBUG(FTLogger::GetLogger(),"------> send_commit for Transaction: " << id.id << " from "
               << id.initiator);
    
    if (!m_is_activated)
        return;
   
    CDMW_MUTEX_GUARD (m_mutex);

    commit_i(id, true);
    CDMW_DEBUG(FTLogger::GetLogger(),"<---END send_commit for Transaction: " << id.id << " from "
               << id.initiator);
}

    
void TransactionEngineCohort::send_rollback
(const CdmwFT::Transaction::TransactionIdentification & id)
    throw(CORBA::SystemException)
{
    CDMW_DEBUG(FTLogger::GetLogger(),"------> send_rollback for Transaction: " << id.id << " from "
               << id.initiator);
    
    if (!m_is_activated)
        return;
  
    CDMW_MUTEX_GUARD (m_mutex);

    rollback_i(id, true);
    CDMW_DEBUG(FTLogger::GetLogger(),"<---END send_rollback for Transaction: " << id.id << " from "
               << id.initiator);
}


void TransactionEngineCohort::send_get_transaction_status
(const CdmwFT::Transaction::TransactionIdentification & id)
    throw(CORBA::SystemException)
{
    CDMW_DEBUG(FTLogger::GetLogger(),"------> send_get_transaction_status for Transaction: "
               << id.id << " from " << id.initiator);
    
    if (!m_is_activated)
        return;
  
    CDMW_MUTEX_GUARD (m_mutex);

    get_transaction_status_i(id, true);
    CDMW_DEBUG(FTLogger::GetLogger(),"<---END send_get_transaction_status for Transaction: "
               << id.id << " from " << id.initiator);
 }


void TransactionEngineCohort::send_set_transaction_status
(const char * sender,
 const CdmwFT::Transaction::TransactionIdentification & id,
 CdmwFT::Transaction::TransactionStatus status)
    throw(CORBA::SystemException)
{
    CDMW_DEBUG(FTLogger::GetLogger(),"------> send_set_transaction_status for Transaction: "
               << id.id << " from " << id.initiator);
    
    if (!m_is_activated)
        return;

    CDMW_MUTEX_GUARD (m_mutex);

    set_transaction_status_i(sender, id, status, true);
    CDMW_DEBUG(FTLogger::GetLogger(),"<---END send_set_transaction_status for Transaction: "
               << id.id << " from " << id.initiator);
}

}  // End namespace FT
}  // End namespace Cdmw
