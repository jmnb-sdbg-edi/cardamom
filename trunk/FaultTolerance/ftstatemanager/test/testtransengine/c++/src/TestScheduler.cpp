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

// Std include files
#include <iostream>
#include <sstream>
#include <string.h>

#include "testtransengine/TestScheduler.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "ftstatemanager/TransactionEngineCoordinator.hpp"
#include "FaultTolerance/ftstatemanager/Scheduler.hpp"
#include "FaultTolerance/idllib/CdmwFTTPCCohort.skel.hpp"

//DEFINE_SIMPLE_DATA_TRAITS(bool);


class CohortTestScheduler 
    : public virtual POA_CdmwFT::Transaction::TPCCohort,
      public virtual PortableServer::RefCountServantBase
{
public:   
    /**
     * Purpose:
     * <p> Constructor with data store
     */
    CohortTestScheduler()
        throw(CORBA::SystemException)
        : m_prepare_is_called(false),
          m_commit_is_called(false),
          m_rollback_is_called(false)
    {};

    /**
     * Purpose:
     * <p> Destructor.
     */
    ~CohortTestScheduler()
    {};



    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/TPCCohort/send_prepare:1.0
     * operation
     */
    void send_prepare(const CdmwFT::Transaction::TransactionIdentification & id,
                      const CdmwFT::Transaction::StateChanges& change)
        throw(CORBA::SystemException){};
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/TPCCohort/commit:1.0
     * operation
     */
    void send_commit (const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException){};
  
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/TPCCohort/rollback:1.0
     * operation
     */
    void send_rollback (const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException){};
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/TPCBase/send_get_transaction_status:1.0
     * operation
     */
    void send_get_transaction_status (const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException){};
  
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
        throw(CORBA::SystemException){};
    
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
        throw(CORBA::SystemException)
    {
        m_prepare_is_called = true;
        return true;
    };
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/TPCCohort/commit:1.0
     * operation
     */
    void commit (CdmwFT::Transaction::TransactionIdentification & id,
                 CORBA::String_out sender)
        throw(CORBA::SystemException)
    {
        m_commit_is_called = true;
    };
  
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/TPCCohort/rollback:1.0
     * operation
     */
    void rollback (CdmwFT::Transaction::TransactionIdentification & id,
                   CORBA::String_out sender)
        throw(CORBA::SystemException)
    {
        m_rollback_is_called = true;
    };
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/TPCBase/get_transaction_status:1.0
     * operation
     */
    CdmwFT::Transaction::TransactionStatus get_transaction_status (CdmwFT::Transaction::TransactionIdentification & id,
                                                      CORBA::String_out sender)
        throw(CORBA::SystemException)
    {
        return CdmwFT::Transaction::UNKNOWN;
    };


    bool prepare_is_called()
    {
        return m_prepare_is_called;
    }
    bool commit_is_called()
    {
        return m_commit_is_called;
    }
    bool rollback_is_called()
    {
        return m_rollback_is_called;
    }
    

private:


    /**
     * Assignement operator is not allowed.
     */
    CohortTestScheduler& operator=(const CohortTestScheduler& rhs);
    bool m_prepare_is_called;
    bool m_commit_is_called;
    bool m_rollback_is_called;



}; // End class CohortTestScheduler



class CoordinatorTestScheduler : public Cdmw::FT::TransactionEngineCoordinatorBase
{
public:
    CoordinatorTestScheduler()
    {
        std::cout<<"new CoordinatorTestScheduler"<<std::endl;
    }

    virtual ~CoordinatorTestScheduler()
    {
    }

    CdmwFT::Transaction::TransactionIdentification get_transaction_with_oid(std::string oid)
    {
        CdmwFT::Transaction::TransactionIdentification id;
        return id;
    }

    bool request(const ::CdmwFT::Transaction::StateChanges& change)
        throw (Cdmw::InternalErrorException)
    {
        return true;
    }

    void send_yes_i(const char * sender,
                    const CdmwFT::Transaction::TransactionIdentification & id)
        throw (Cdmw::InternalErrorException)
    {
         std::cout<<"send_yes_i"<<std::endl;
    };
        
    void send_no_i(const char * sender,
                    const CdmwFT::Transaction::TransactionIdentification & id)
        throw (Cdmw::InternalErrorException)
    {
         std::cout<<"send_no_i"<<std::endl;
    };
        
    void send_end_i(const char * sender,
                    const CdmwFT::Transaction::TransactionIdentification & id)
        throw (Cdmw::InternalErrorException)
    {
         std::cout<<"send_end_i"<<std::endl;
    };
        
    void send_set_transaction_status_i(const char * sender,
                                       const CdmwFT::Transaction::TransactionIdentification & id,
                                       CdmwFT::Transaction::TransactionStatus status)
        throw (Cdmw::InternalErrorException)
    {
         std::cout<<"send_set_transaction_status_i"<<std::endl;
    };

    void on_failure(std::string location)
    {};

    void on_timeout(const CdmwFT::Transaction::TransactionIdentification & id)
    {};

    void send_yes (const char * sender,
                   const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException)
    {};
    
    void send_no (const char * sender,
                  const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException)
    {};
    
    void send_end (const char * sender,
                   const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException)
    {};

    void send_get_transaction_status (const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException)
    {};
    
    void send_set_transaction_status (const char * sender,
                                      const CdmwFT::Transaction::TransactionIdentification & id,
                                      CdmwFT::Transaction::TransactionStatus status)
        throw(CORBA::SystemException)
    {};

    CdmwFT::Transaction::TransactionStatus 
    get_transaction_status(CdmwFT::Transaction::TransactionIdentification & id,
                           CORBA::String_out sender)
        throw(CORBA::SystemException)
    {
        return CdmwFT::Transaction::ABORTED;
    };
    
    void set_transaction_status(CdmwFT::Transaction::TransactionStatus status,
                                char *& sender,
                                CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException)
    {};

    void set_state(int i)
    {
        //case1 : set COMMITTED
        //case2 : set ABORTED
        //case3 : set ERROR
        switch(i){
            case 1:
                {
                    Cdmw::FT::Scheduler::TransactionResult tr;
                    tr.status = Cdmw::FT::Scheduler::COMMITTED;
                    tr.reason = "Commited";
                    future.set(tr);
                    break;
                }
            case 2:
                {
                    Cdmw::FT::Scheduler::TransactionResult tr;
                    tr.status = Cdmw::FT::Scheduler::ABORTED;
                    tr.reason = "Aborted";
                    future.set(tr);
                    break;
                }
            case 3:
                {
                    Cdmw::FT::Scheduler::TransactionResult tr;
                    tr.status = Cdmw::FT::Scheduler::ERROR;
                    tr.reason = "Error";
                    future.set(tr);
                    break;
                }
        }
    };
    
    
private:
    ACE_Future< Cdmw::FT::Scheduler::TransactionResult> future;
    
};


TestScheduler::TestScheduler (CORBA::ORB_ptr orb, PortableServer::POA_ptr rootPOA)
	:Cdmw::TestUtils::Testable("Test Scheduler")
{
    m_orb = CORBA::ORB::_duplicate(orb);
    m_rootPOA = PortableServer::POA::_duplicate(rootPOA);
}

TestScheduler::~TestScheduler ()
{
}


void 
TestScheduler::do_tests()
{
    set_nbOfRequestedTestOK(3);
    
    try 
    {         
        TEST_INFO("Create the coordinator");
        //typedef CoordinatorTestScheduler<std::string , bool> MyCoordinator;
        //MyCoordinator* coord = new MyCoordinator();
        CoordinatorTestScheduler* coord = new CoordinatorTestScheduler();

        CohortTestScheduler* cohort_obj = new CohortTestScheduler();
        CdmwFT::Transaction::TPCCohort_var cohort = cohort_obj->_this();
        
        TEST_INFO("Create the scheduler");
        Cdmw::FT::Scheduler::Scheduler scheduler(m_orb.in(), coord, 1);
        
        TEST_INFO("Start the scheduler");
        scheduler.start();

        CdmwFT::Transaction::StateChanges change;

        TEST_INFO("request 1");
        CdmwFT::Transaction::TransactionIdentification id;
        id.id=1;
        id.initiator="initiator";

        scheduler.prepare(cohort.in(), change, id, "here");


// void Scheduler::prepare(CdmwFT::Transaction::TPCCohort_ptr cohort, 
//                         const CdmwFT::Transaction::StateChanges& change, 
//                         CdmwFT::Transaction::TransactionIdentification id,
//                         const std::string & location)


        TEST_INFO("request 2");
        scheduler.commit(cohort.in(), id, "here");

        TEST_INFO("request 3");
        scheduler.rollback(cohort.in(), id, "here");
        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        Cdmw::OsSupport::OS::sleep( timescale * 2000 );


        TEST_CHECK(cohort_obj->prepare_is_called());
        
        TEST_CHECK(cohort_obj->commit_is_called());
        
        TEST_CHECK(cohort_obj->rollback_is_called());


        TEST_INFO("Stop the scheduler");
        scheduler.stop(true);

        //        TEST_INFO("Join the scheduler");
        //        scheduler.join();
        
    } 
    catch (...)
    {
        std::ostringstream info_stream1;
        info_stream1 << "Test State Manager Exception caught : " << std::endl;
        TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }
    
}
