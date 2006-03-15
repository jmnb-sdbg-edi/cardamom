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

#ifndef INCL_CDMW_FT_FTSTATEMANAGER_TRANSACTION_ENGINE_COHORT_HPP
#define INCL_CDMW_FT_FTSTATEMANAGER_TRANSACTION_ENGINE_COHORT_HPP

#include <FaultTolerance/ftstatemanager/TransactionEngine.hpp>
#include <FaultTolerance/idllib/CdmwFTTPCCohort.skel.hpp>
#include <FaultTolerance/idllib/CdmwFTTPCCoordinator.stub.hpp>
#include <FaultTolerance/ftstatemanager/Timer.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreBase.hpp>
#include <Foundation/orbsupport/TypeTraits.hpp>
#include <map>
#include <list>
#include <FaultTolerance/ftcommon/MacroDef.hpp>
#include <Foundation/osthreads/Mutex.hpp>
#include <Foundation/osthreads/MutexGuard.hpp>


/**
 *Purpose:
 * TransactionEngineCohort template.
 *<p>
 *
 *Features:
 *<p> Thread safe. Exception safe.
 */
namespace Cdmw {
namespace FT {

class TransactionEngineCohort;

class Time_Handler :
         public  Cdmw::FT::Timer_Event_Handler<CdmwFT::Transaction::TransactionIdentification>
{ 
public:
    Time_Handler(TransactionEngineCohort* cohort, bool is_synch)
        : m_cohort(cohort),
          m_is_synch(is_synch)
    {};
    
    ~Time_Handler()
    {};
    
    //Method which is called back by the Reactor when timeout occurs.
    int handle_timeout
    (const CdmwFT::Transaction::TransactionIdentification* tid);

private:
    TransactionEngineCohort* m_cohort;
    bool m_is_synch;
};


class TransactionEngineCohort 
    : public TransactionEngine,
      public virtual POA_CdmwFT::Transaction::TPCCohort,
      public virtual PortableServer::RefCountServantBase
{
    typedef TransactionEngine::TransactionMap TransactionMap;
    typedef Timer<CdmwFT::Transaction::TransactionIdentification> Timer;

public:   
    /**
     * Purpose:
     * <p> Constructor with data store
     */
    TransactionEngineCohort(DataStoreContext& context,
                            CORBA::ORB_ptr orb,
                            PortableServer::POA_ptr poa,
                            std::string coordinator_corbaloc,
                            int max_transaction_in_progress,
                            int max_transaction_done,
                            OsSupport::OS::Timeval timeout)
        throw(CORBA::SystemException);

    /**
     * Purpose:
     * <p> Destructor.
     */
    virtual ~TransactionEngineCohort();

    /**
     * Purpose:
     * <p> get the transaction map (to be used when cohort becomes coordinator)
     *
     */
    TransactionMap get_cohort_transactions();

    /**
     * Purpose:
     * <p> process warning of a failure
     * (if location is coordinator the cohort should abort)
     *
     *@param location is the failed location
     */
    void on_failure(const std::string& location);

    /**
    * Purpose:
    * <p> process warning of a failure
    *
    *@param location is the failed location
    *@param is_synch request between coordinator and cohort is synchronous
    */
    void on_timeout(const CdmwFT::Transaction::TransactionIdentification & id, bool is_synch);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/TPCCohort/send_prepare:1.0
     * operation
     */
    void send_prepare(const CdmwFT::Transaction::TransactionIdentification & id,
                      const CdmwFT::Transaction::StateChanges& change)
        throw(CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/TPCCohort/commit:1.0
     * operation
     */
    void send_commit (const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException);
  
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/TPCCohort/rollback:1.0
     * operation
     */
    void send_rollback (const CdmwFT::Transaction::TransactionIdentification & id)
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
     * IDL:thalesgroup.com/CdmwFT/TPCCohort/prepare_insert:1.0
     * operation
     */
    CORBA::Boolean prepare(const CdmwFT::Transaction::StateChanges& change,
                           CdmwFT::Transaction::TransactionIdentification & id,
                           CORBA::String_out sender)
        throw(CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/TPCCohort/commit:1.0
     * operation
     */
    void commit (CdmwFT::Transaction::TransactionIdentification & id,
                 CORBA::String_out sender)
        throw(CORBA::SystemException);
  
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/TPCCohort/rollback:1.0
     * operation
     */
    void rollback (CdmwFT::Transaction::TransactionIdentification & id,
                   CORBA::String_out sender)
        throw(CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/TPCBase/get_transaction_status:1.0
     * operation
     */
    CdmwFT::Transaction::TransactionStatus get_transaction_status (CdmwFT::Transaction::TransactionIdentification & id,
                                                      CORBA::String_out sender)
        throw(CORBA::SystemException);

  /* Not in IIOP : the only moment where coordinator set the transaction status
     of cohort is when cohort get the transaction status.
     In IIOP this is done via the return value of get_transaction_status.
    void set_transaction_status (CdmwFT::Transaction::TransactionStatus status,
                                 char *& sender,
                                 CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException);
  */

    void activate()
    {
        m_is_activated = true;
        m_timer->start();
    }
    
    void deactivate()
    {
        m_is_activated = false;
        m_timer->stop();
    }
protected:

    /**
     * Purpose:
     * <p> Default constructor.
     */
//     TransactionEngineCohort()
//     {
//     }

private:

    CORBA::Boolean prepare_i
    (const CdmwFT::Transaction::TransactionIdentification & id,
     const CdmwFT::Transaction::StateChanges& change,
     bool is_asynchronous)
        throw(CORBA::SystemException);
    
    void commit_i (const CdmwFT::Transaction::TransactionIdentification & id,
                   bool is_asynchronous)
        throw(CORBA::SystemException);
    
    void rollback_i (const CdmwFT::Transaction::TransactionIdentification & id,
                     bool is_asynchronous)
        throw(CORBA::SystemException);    
    
    CdmwFT::Transaction::TransactionStatus get_transaction_status_i (const CdmwFT::Transaction::TransactionIdentification & id,
                                                        bool is_asynchronous)
        throw(CORBA::SystemException);
    
    
    void set_transaction_status_i (const char * sender,
                                   const CdmwFT::Transaction::TransactionIdentification & id,
                                   CdmwFT::Transaction::TransactionStatus status,
                                   bool is_asynchronous)
        throw(CORBA::SystemException);


    /**
     * Assignement operator is not allowed.
     */
    TransactionEngineCohort& operator=(const TransactionEngineCohort& rhs);

    DataStoreContext& m_context;

    /**
     * The current transaction
     */
  
    TransactionMap m_transactionMap;

    CdmwFT::Transaction::TPCCoordinator_var m_coordinatorObject;
    
    // timer
    Time_Handler* m_timer_handler;
    Timer* m_timer;
    OsSupport::OS::Timeval m_timeout;

    // is_activated
    bool m_is_activated;

    // is_synch : true if request to the cohort are synchrone
    bool m_is_synch;

    /**
     * The mutex used to protect the TransactionMap
     */
    OsSupport::Mutex m_mutex;

}; // End class TransactionEngineCohort

}  // End namespace FT
}  // End namespace Cdmw

#endif

