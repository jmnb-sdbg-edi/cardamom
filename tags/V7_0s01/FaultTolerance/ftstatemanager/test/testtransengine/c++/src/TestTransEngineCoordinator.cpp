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

#include "testtransengine/TestTransEngineCoordinator.hpp"
#include <Foundation/ossupport/OS.hpp>
#include <ftstatemanager/MIOPMutex.hpp>



// namespace Cdmw {
// namespace FT {

typedef Cdmw::FT::DataStore<CORBA::Long, std::string> TestDataStore;
CdmwFT::Transaction::TPCCoordinator_var coordinator;


CohortTest::CohortTest()
    : m_answer("")
    , m_prev_answer("")
    , m_do_nothing(false)
{    
};

CohortTest::~CohortTest()
{
    std::cerr << "In ~CohortTest" << std::endl;
}

void CohortTest::do_nothing()
{
    m_do_nothing = true;
}


void CohortTest::send_prepare (const CdmwFT::Transaction::TransactionIdentification & id,
                   const CdmwFT::Transaction::StateChanges& change)
    throw(CORBA::SystemException)
{
    std::cout<< "send_prepare of CohortTest answers Yes for transaction " << id.id <<std::endl;
    CDMW_FT_BEGIN_MIOP_MUTEX;
    coordinator->send_yes("CohortTest", id);
    CDMW_FT_END_MIOP_MUTEX;
    
    switch(change[0].records[0].change_data._d())
    {
        case CdmwFT::Transaction::DS_INSERTION:
            {
                set_answer("send_prepare_insert/yes");
                break;
            }
        case CdmwFT::Transaction::DS_UPDATE:
            {
                set_answer("send_prepare_update/yes");
                break;
            }
        case CdmwFT::Transaction::DS_REMOVAL:
            {
                set_answer("send_prepare_delete/yes");
            }
    }
};

void CohortTest::send_commit (const CdmwFT::Transaction::TransactionIdentification & id)
    throw(CORBA::SystemException)
{
      std::cout<< "send_commit of CohortTest answers End for transaction " << id.id <<std::endl;
      CDMW_FT_BEGIN_MIOP_MUTEX;
      coordinator->send_end("CohortTest", id);
      CDMW_FT_END_MIOP_MUTEX;
      set_answer("send_commit/end");
};

void CohortTest::send_rollback (const CdmwFT::Transaction::TransactionIdentification & id)
    throw(CORBA::SystemException)
{
    std::cout<< "send_rollback of CohortTest answers End for transaction " << id.id <<std::endl;
    CDMW_FT_BEGIN_MIOP_MUTEX;
    coordinator->send_end("CohortTest", id);
    CDMW_FT_END_MIOP_MUTEX;
    set_answer("send_rollback/end");
};

void CohortTest::send_get_transaction_status (const CdmwFT::Transaction::TransactionIdentification & id)
    throw(CORBA::SystemException)
{
    std::cout<< "send_get_transaction_status of CohortTest answers COMMITTED for transaction " << id.id <<std::endl;
    // This get_transaction_status will be called for recovering test
    CDMW_FT_BEGIN_MIOP_MUTEX;
    coordinator->send_set_transaction_status("CohortTest", id, CdmwFT::Transaction::COMMITTED);
    CDMW_FT_END_MIOP_MUTEX;
    set_answer("send_get_transaction_status/COMMITTED");
};

void CohortTest::send_set_transaction_status (const char * sender,
                                  const CdmwFT::Transaction::TransactionIdentification & id,
                                  CdmwFT::Transaction::TransactionStatus status)
    throw(CORBA::SystemException)
{
    std::cout<< "send_set_transaction_status of CohortTest does nothing for transaction " << id.id <<std::endl;
    set_answer("send_set_transaction_status");
};


CORBA::Boolean CohortTest::prepare (const CdmwFT::Transaction::StateChanges& change,
                        CdmwFT::Transaction::TransactionIdentification & id,
                        CORBA::String_out sender)
    throw(CORBA::SystemException)
{
    if (!m_do_nothing)
    {
        sender = CORBA::string_dup("CohortTest"); 
        } else {
        Cdmw::OsSupport::OS::sleep( 3000 );
        sender = CORBA::string_dup("UnexistingCohortTest");
    }
    switch(change[0].records[0].change_data._d())
    {
        case CdmwFT::Transaction::DS_INSERTION:
            {
                set_answer("prepare_insert/yes");
                break;
            }
        case CdmwFT::Transaction::DS_UPDATE:
            {
                set_answer("prepare_update/yes");
                break;
            }
        case CdmwFT::Transaction::DS_REMOVAL:
            {
                set_answer("prepare_delete/yes");
                }
    }
    
    return true;
};

void CohortTest::commit (CdmwFT::Transaction::TransactionIdentification & id,
             CORBA::String_out sender)
    throw(CORBA::SystemException)
{
    if (!m_do_nothing)
    {
        sender = CORBA::string_dup("CohortTest");
    } else {
            Cdmw::OsSupport::OS::sleep( 3000 );
            sender = CORBA::string_dup("UnexistingCohortTest");
    }
    set_answer("commit/end");
};


void CohortTest::rollback (CdmwFT::Transaction::TransactionIdentification & id,
               CORBA::String_out sender)
    throw(CORBA::SystemException)
{
    if (!m_do_nothing)
    {
        sender = CORBA::string_dup("CohortTest");
    } else {
        Cdmw::OsSupport::OS::sleep( 3000 );
        sender = CORBA::string_dup("UnexistingCohortTest");
    }
    set_answer("rollback/end");
};


CdmwFT::Transaction::TransactionStatus CohortTest::get_transaction_status (CdmwFT::Transaction::TransactionIdentification & id,
                                                               CORBA::String_out sender)
    throw(CORBA::SystemException)
{
    sender = CORBA::string_dup("CohortTest");
    set_answer("get_transaction_status/COMMITTED");
    return CdmwFT::Transaction::COMMITTED;
};


void CohortTest::set_transaction_status (CdmwFT::Transaction::TransactionStatus status,
                             char *& sender,
                             CdmwFT::Transaction::TransactionIdentification & id)
    throw(CORBA::SystemException)
{
    sender = CORBA::string_dup("CohortTest"); 
    set_answer("set_transaction_status");
};

std::string CohortTest::get_answer()
{
    std::string temp;
    if (m_prev_answer == "")
    {
        temp = m_answer;
    }
    else
    {
        //std::cout<<__LINE__<<" "<<m_prev_answer<<std::endl;
        temp = m_prev_answer;
        m_prev_answer = m_answer;
    }
    m_answer = "";
    std::cout<<"get_answer "<<temp<<std::endl;
    return temp;
}

void CohortTest::set_answer(std::string answer)
{
    if (m_answer != "") m_prev_answer = m_answer;
    m_answer = answer;
    std::cerr<<"set_answer "<<m_answer<<std::endl;
}



class BadCohortTest : public virtual POA_CdmwFT::Transaction::TPCCohort,
                   public virtual PortableServer::RefCountServantBase
{
public:   

    /**
     * Purpose:
     * <p> Constructor
     */
    BadCohortTest()
    {};

    virtual ~BadCohortTest()
    {
    }

    void send_prepare (const CdmwFT::Transaction::TransactionIdentification & id,
                       const CdmwFT::Transaction::StateChanges& change)
        throw(CORBA::SystemException)
    {
      std::cout<< "send_prepare_insert of BadCohortTest answers No for transaction " << id.id <<std::endl;
      CDMW_FT_BEGIN_MIOP_MUTEX;
      coordinator->send_no("BadCohortTest", id);
      CDMW_FT_END_MIOP_MUTEX;
    };

    void send_commit (const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException)
    {
      std::cout<< "send_commit of BadCohortTest doesn't answers for transaction " << id.id <<std::endl;
    };
    
    void send_rollback (const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException)
    {
      std::cout<< "send_rollback of BadCohortTest doesn't answers for transaction " << id.id <<std::endl;
    };

    void send_get_transaction_status (const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException)
    {
      std::cout<< "send_get_transaction_status of BadCohortTest answers ABORTED for transaction " << id.id <<std::endl;
      CDMW_FT_BEGIN_MIOP_MUTEX;
      coordinator->send_set_transaction_status("BadCohortTest", id, CdmwFT::Transaction::ABORTED);
      CDMW_FT_END_MIOP_MUTEX;
    };

    void send_set_transaction_status (const char * sender,
                                 const CdmwFT::Transaction::TransactionIdentification & id,
                                 CdmwFT::Transaction::TransactionStatus status)
      throw(CORBA::SystemException)
    {
      std::cout<< "send_set_transaction_status of BadCohortTest does nothing for transaction " << id.id <<std::endl;
    };

    CORBA::Boolean prepare (const CdmwFT::Transaction::StateChanges& change,
                           CdmwFT::Transaction::TransactionIdentification & id,
                           CORBA::String_out sender)
        throw(CORBA::SystemException)
    {
        sender = CORBA::string_dup("BadCohortTest");
        return false;
    };

    void commit (CdmwFT::Transaction::TransactionIdentification & id,
                 CORBA::String_out sender)
        throw(CORBA::SystemException)
    {
        sender = CORBA::string_dup("");
    };
    

    void rollback (CdmwFT::Transaction::TransactionIdentification & id,
                   CORBA::String_out sender)
        throw(CORBA::SystemException)
    {
        sender = CORBA::string_dup("");
    };
    

    CdmwFT::Transaction::TransactionStatus get_transaction_status (CdmwFT::Transaction::TransactionIdentification & id,
                                                      CORBA::String_out sender)
        throw(CORBA::SystemException)
    {
        sender = CORBA::string_dup("BadCohortTest");
        return CdmwFT::Transaction::ABORTED;
    };
    
    
    void set_transaction_status (CdmwFT::Transaction::TransactionStatus status,
                                 char *& sender,
                                 CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException)
    {
        sender = CORBA::string_dup("BadCohortTest");
    };


};

static int inc = 0;
class RIUOperationThread : public Cdmw::OsSupport::Thread
{
public:
    RIUOperationThread(TestDataStore* dataStore, 
                       int oid, std::string data, ::CdmwFT::Transaction::StateChangeKind opType)
        : m_dataStore(dataStore), 
          m_oid(oid), m_data(data), m_done(false), m_transaction(opType), m_result(true)
    {
        
    }
    
    ~RIUOperationThread() throw()
    {
    }
    
    bool get_done()
    {
        return m_done; 
    }
    bool get_result()
    {
        return m_result;
    }
protected:
    void run() throw()
    {
        try
        {
        inc ++;
        m_inc = inc;
        switch(m_transaction) {
        case CdmwFT::Transaction::DS_INSERTION:
            m_dataStore->insert(m_oid,m_data);
            m_result = true;
            std::cout << "TestThread: insert done in datastore" <<m_inc<< std::endl;
            break;
            
        case CdmwFT::Transaction::DS_UPDATE:
            std::cout << "TestThread: before update in datastore " <<m_inc<< " " << m_dataStore->contains(m_oid) << std::endl << std::flush;
            m_dataStore->update(m_oid,m_data);  
            m_result = true;  
            std::cout << "TestThread: update done in datastore " <<m_inc<< std::endl << std::flush;
            break;
            
        case CdmwFT::Transaction::DS_REMOVAL:
            m_dataStore->remove(m_oid);  
            m_result = true;      
            std::cout << "TestThread: remove done in datastore" <<m_inc<< std::endl;
        }
        }
        catch (const CORBA::Exception& ex)
        {
            std::cout << "caught " << ex << std::endl;
        }
        catch (const Cdmw::FT::NotFoundException& ex)
        {
            std::cout << "caught NotFoundException" << std::endl;
        }
        catch (...)
        {
            std::cout << "caught something" << std::endl;
        }

        m_done = true;
    }
private:
    TestDataStore* m_dataStore;
    int m_oid;
    std::string m_data;
    bool m_done;
    ::CdmwFT::Transaction::StateChangeKind m_transaction;
    bool m_result;
    int m_inc;
};

class AddLocationThread : public Cdmw::OsSupport::Thread
{
public:
  AddLocationThread(std::string location, bool witoutMIOP = false)
      : m_location(location), m_done(false), m_witoutMIOP(witoutMIOP)
  {
  }

  ~AddLocationThread() throw()
  {
  }

  bool get_done()
  {
    return m_done;
  }
protected:
  void run() throw()
  {
      if (m_witoutMIOP)
      {
          try {
              cohort_obj = new CohortTest();
              cohort_test = cohort_obj->_this();
              Cdmw::FT::DataStoreBase::Get_dataStore(4)->get_context().
                  add_ref_for_location(m_location,
                                       CdmwFT::Transaction::TPCCoordinator::_nil(),
                                       cohort_test.in());
          

          }
          catch (const Cdmw::FT::AlreadyExistException&)
          {
              std::cerr << "AlreadyExistException" << std::endl;
          }
          catch (const Cdmw::FT::NotFoundException&)
          {
              std::cerr << "NotFoundException" << std::endl;
          }
          catch (...)
          {
              std::cerr << "something else" << std::endl;
          }

      } else {
          try
          {
              Cdmw::FT::DataStoreBase::Get_dataStore(1)->get_context().add_location(1, m_location);
          }
          catch (const Cdmw::FT::AlreadyExistException&)
          {
              std::cerr << "AlreadyExistException" << std::endl;
          }
          catch (const Cdmw::FT::NotFoundException&)
          {
              std::cerr << "NotFoundException" << std::endl;
          }
          catch (...)
          {
              std::cerr << "something else" << std::endl;
          }
      }
      
      m_done = true;
  }
private:
    std::string m_location;
    bool m_done;
    CohortTest * cohort_obj ;
    CdmwFT::Transaction::TPCCohort_var cohort_test;
    bool m_witoutMIOP;
    
};

TestTransEngineCoordinator::TestTransEngineCoordinator (CORBA::ORB_ptr orb, PortableServer::POA_ptr rootPOA)
	:Cdmw::TestUtils::Testable("Test Trans Engine Coordinator")
{
    m_orb = CORBA::ORB::_duplicate(orb);
    m_rootPOA = PortableServer::POA::_duplicate(rootPOA);
}

TestTransEngineCoordinator::~TestTransEngineCoordinator ()
{
}

void 
TestTransEngineCoordinator::do_tests()
{
    set_nbOfRequestedTestOK(85);

    TestDataStore* myDataStore1 = NULL;
    TestDataStore* myDataStore2 = NULL;

    try 
    {         
        TEST_INFO("--MIOP Section--");
        const int MAX_TRANSACTION_DONE = 10;
        const int MAX_TRANSACTION_IN_PROGRESS = 10;
        TEST_INFO("Create a DataStore");
        Cdmw::OsSupport::OS::Timeval cohort_timeout;
        cohort_timeout.seconds=2;
        cohort_timeout.microseconds=0;
        Cdmw::OsSupport::OS::Timeval coordinator_timeout;
        coordinator_timeout.seconds=20;
        coordinator_timeout.microseconds=0;
      
        std::ostringstream multicast_domain;
        multicast_domain << "cdmw";
        multicast_domain << Cdmw::OsSupport::OS::get_logname(); 
        multicast_domain << "ftdomain" << std::ends;


        Cdmw::FT::DataStoreContext* context1 =
            new Cdmw::FT::DataStoreContext(m_orb.in(),
                                           m_rootPOA.in(),
                                           "5555",
                                           multicast_domain.str().c_str(),
                                           "225.1.1.8",
                                           MAX_TRANSACTION_IN_PROGRESS,
                                           MAX_TRANSACTION_DONE,
                                           cohort_timeout,
                                           coordinator_timeout,
                                           1);
        
        myDataStore1 = new TestDataStore(1,
                                         m_orb.in(),
                                         m_rootPOA.in(),
                                         *context1);

        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        Cdmw::OsSupport::OS::sleep( timescale * 500 );
      
        TEST_INFO("Activate Coordinator");
        myDataStore1->activate();
        TEST_SUCCEED();
      
        // Test local insertion
        TEST_INFO("Test of Insert when no cohorts are declared");
        myDataStore1->insert(0,"zero");
        TEST_CHECK(myDataStore1->select(0)=="zero");

        TEST_INFO("Get coordinator");
        // WARNING: a context should already have been created by previous tests (TestTransEngine & TestTransEngineCohort)
        //          and expected counter for ftdomain-i should now be 6 (cohort) and 7 (coordinator) ! 
        std::ostringstream multicast_addr;
        multicast_addr << "corbaloc:miop:1.0@1.0-cdmw" << Cdmw::OsSupport::OS::get_logname() << "ftdomain-7/225.1.1.8:5555" << std::ends;
        CORBA::Object_var group = m_orb->string_to_object(multicast_addr.str().c_str());
        // coordinator = CdmwFT::Transaction::TPCCoordinator::_unchecked_narrow (group.in());
        coordinator = context1->get_tpc_coordinator();
        TEST_SUCCEED();
      
        TEST_INFO("Instanciate CohortTest impl");
        CohortTest * cohort_obj = new CohortTest();
        TEST_SUCCEED();
 
        TEST_INFO("Get cohort");
        // Group Id 2 is 1 (first param of TestDataStore) * 2
        std::ostringstream multicast_addr2;
        multicast_addr2 << "corbaloc:miop:1.0@1.0-cdmw" << Cdmw::OsSupport::OS::get_logname() << "ftdomain-6/225.1.1.8:5555" << std::ends;
        CORBA::Object_var group2 = m_orb->string_to_object(multicast_addr2.str().c_str());
        PortableServer::ObjectId_var cohort_id = m_rootPOA->create_id_for_reference(group2.in());
        TEST_SUCCEED();

        std::cout<<"coordinator --- :"<<multicast_addr.str().c_str()<<std::endl;
        std::cout<<"cohort --- :"<<multicast_addr2.str().c_str()<<std::endl;


        for (CORBA::ULong i = 0; i < cohort_id->length(); i++)
        {
            std::cerr << "id[" << i << "] = " << cohort_id[i] + 0 << std::endl;
        }

        TEST_INFO("Activate also the cohort")
        try
        {
            m_rootPOA->activate_object_with_id (cohort_id.in (),cohort_obj);
            TEST_SUCCEED();
        }
        catch(const CORBA::Exception& ex)
        {
            std::cerr << "caught " << ex << std::endl;
            TEST_FAILED();
        }
        catch(...)
        {
            TEST_FAILED();
        }
        /*
        // Note 1: The following is the only solution when we want to associate
        // another object to the same reference (see BadCohortTest below)
        // because another activate_object_with_id with same id (create_id_for_reference
        // returns a unique id) and a different object leads to exception 
        // Note 2: Instead of using the previous create_id_for_reference and
        // activate_object_with_id we could do the following :

        // Activate_object need System_id and Retain policy : this is what has rootPOA
        PortableServer::ObjectId_var cohort_id = m_rootPOA->activate_object(cohort_obj);
        m_rootPOA->associate_reference_with_id(group2.in(), cohort_id.in());
        */
        TEST_INFO("Set cohorts locations")
        context1->add_location(1, "CohortTest");

        TEST_INFO("Insert a <key, value>")
        myDataStore1->insert(1,"un");
        Cdmw::OsSupport::OS::sleep( timescale * 1000 );
        TEST_SUCCEED();

        TEST_INFO("Verify the CohortTest received send_prepare_insert")
        TEST_CHECK(cohort_obj->get_answer()=="send_prepare_insert/yes");
        TEST_INFO("then received commit")
        TEST_CHECK(cohort_obj->get_answer()=="send_commit/end");
        TEST_INFO("And that insertion done localy");
        TEST_CHECK(myDataStore1->select(1)=="un");

        // This kind of test that catch  AlreadyExistException is already done
        // in local (see teststorageinterface) but here we test the exception
        // raised in Transaction Engine Coordinator
        TEST_INFO("Try to insert already existing key");
        try
        {
            myDataStore1->insert(1,"un");
            TEST_FAILED();
        }
        catch(const Cdmw::FT::AlreadyExistException& e)
        {
            TEST_INFO(e.what());
            TEST_SUCCEED();
        }
     
        Cdmw::OsSupport::OS::sleep( timescale * 2000 );
        // In this section we test behaviour when a cohort answer no (and one yes)

        {
            // The transaction above have finished (cause we are in the same tread)
            // and there is no other transaction so we can add a location
            TEST_INFO("Instanciate BadCohortTest impl");
            BadCohortTest * bad_cohort_obj = new BadCohortTest();
            TEST_SUCCEED();
            TEST_INFO("Activate also the bad cohort")
            // Note: there is now two ids associated with group2 reference.
            // Activate_object need System_id and Retain policy : this is what has rootPOA
            PortableServer::ObjectId_var bad_cohort_id = m_rootPOA->activate_object(bad_cohort_obj);
            m_rootPOA->associate_reference_with_id(group2.in(), bad_cohort_id.in());
            TEST_SUCCEED();
	
            TEST_INFO("Add a cohort location")
            context1->add_location(1, "BadCohortTest");
            TEST_SUCCEED();
            TEST_INFO("Thread instanciated for Update a <key, value>")
            RIUOperationThread it1(myDataStore1, 1, "one", CdmwFT::Transaction::DS_UPDATE);
            it1.start();
            Cdmw::OsSupport::OS::sleep( timescale * 1000 );
            TEST_SUCCEED();
            TEST_INFO("Verify the CohortTest received send_prepare_update..")
            TEST_CHECK(cohort_obj->get_answer()=="send_prepare_update/yes");
            TEST_INFO("..then received send_rollback/end")
            TEST_CHECK(cohort_obj->get_answer()=="send_rollback/end");
            // BadCohortTest don't answer end to rollback
            TEST_INFO("Verify the transaction is not finished")
            TEST_CHECK(!(it1.get_done()));	
            TEST_INFO("Declare the BadCohortTest on_failure")
            Cdmw::FT::DataStoreBase::On_Failure("BadCohortTest");
            Cdmw::OsSupport::OS::sleep( timescale * 1000 );
            TEST_SUCCEED();
            TEST_INFO("Verify the transaction is finished")
            TEST_CHECK(it1.get_done());
            TEST_INFO("Verify update not done localy")
            TEST_CHECK(myDataStore1->select(1)=="un");
            it1.join();
	
            // Deactivation necessary before delete (or orb shutdown segmentation fault)
            // IMPORTANT NOTE ABOUT TAO AND ITS HANDLING OF GROUP REFERENCE :
            // Normaly we have to disassociate_reference_with_id but this method is
            // not implemented in TAO 1.3.1..
            // If we do only a deactivate_object the (good) cohort_id seems to be
            // mysteriously desactivated also !
            // We have to do an associate_reference_with_id for the (good) cohort_id as
            // if this method was overloading. Which is not the officialy expected
            // behaviour because this method should associate several id : the miop doc
            // specify a reference_to_ids giving all the ids associated to the group ref.
            // But this method also is not implemented  in TAO 1.3.1..
            //m_rootPOA->disassociate_reference_with_id(group2.in(), bad_cohort_id.in());
            m_rootPOA->deactivate_object (bad_cohort_id.in ());      
            delete bad_cohort_obj;
            m_rootPOA->associate_reference_with_id(group2.in(), cohort_id.in());
        }
      
      
        // In this section we test the MAX_TRANSACTION_IN_PROGRESS
        // and the location change forbidden during transactions in progress
        {
            // The transaction above have finished (cause we are in the same tread)
            // and there is no other transaction so we can add a location
            TEST_INFO("Set cohorts locations")
	  
            Cdmw::FT::DataStoreContext::LocationList locations;
            locations.insert("CohortTest");
            locations.insert("UnexistingCohortTest");
            context1->set_locations(1, locations);
            // The next transactions are done in a separate thread and will not end
            // immediatly because the location "NotExisting" will not answer
            TEST_INFO("Thread instanciated for Insert a <key, value>")
            RIUOperationThread it1(myDataStore1, 2, "deux", CdmwFT::Transaction::DS_INSERTION);
            it1.start();
            Cdmw::OsSupport::OS::sleep( timescale * 2000 );
            TEST_INFO("Verify transaction not finished..")
            TEST_CHECK(it1.get_done()==false);
            TEST_INFO("..but CohortTest receives send_prepare_insert & answers yes")
            TEST_CHECK(cohort_obj->get_answer()=="send_prepare_insert/yes");

            TEST_INFO("Create MAX_TRANSACTION_IN_PROGRESS-1 update transactions");
            // which is MAX_TRANSACTION_IN_PROGRESS transactions with previous Insert
            RIUOperationThread* upd_tbl[MAX_TRANSACTION_IN_PROGRESS];
            for (int i =0; i<(MAX_TRANSACTION_IN_PROGRESS-1); i++)
            {
                upd_tbl[i] = new RIUOperationThread(myDataStore1, 2, "two", CdmwFT::Transaction::DS_UPDATE);
                upd_tbl[i]->start();
            }
            Cdmw::OsSupport::OS::sleep( timescale * MAX_TRANSACTION_IN_PROGRESS * 150 );
            TEST_SUCCEED();
            TEST_INFO("Verify transactions not finished.")
            bool test_ok = true;
            for (int i =0; i<(MAX_TRANSACTION_IN_PROGRESS-1); i++)
            {
                if (upd_tbl[i]->get_done())
                { 
                    test_ok = false;
                    break;
                }
            }
            TEST_CHECK(test_ok);
            TEST_INFO("and that CohortTest doesn't receive send_prepare_update & doesn't answer yes")
            TEST_CHECK(cohort_obj->get_answer()!="send_prepare_update/yes");

            TEST_INFO("Try to create one more update transaction..")
            upd_tbl[MAX_TRANSACTION_IN_PROGRESS-1] =
                new RIUOperationThread(myDataStore1, 2, "two", CdmwFT::Transaction::DS_UPDATE);
            upd_tbl[MAX_TRANSACTION_IN_PROGRESS-1]->start();
            TEST_SUCCEED();
            Cdmw::OsSupport::OS::sleep( timescale * 500 );
            // MAX_TRANSACTION_IN_PROGRESS reached : transaction not created
            TEST_INFO("Verify this operation is not finished.")
            TEST_CHECK( !upd_tbl[MAX_TRANSACTION_IN_PROGRESS-1]->get_done() );
            
            TEST_INFO("..but CohortTest doesn't receive send_prepare_update & doesn't answer yes")
            TEST_CHECK(cohort_obj->get_answer()!="prepare_insert/yes");
            
/*                
            
            TEST_INFO("Verify this operation is finished and result false")
            TEST_CHECK( (upd_tbl[MAX_TRANSACTION_IN_PROGRESS-1]->get_done()) &&
                        !(upd_tbl[MAX_TRANSACTION_IN_PROGRESS-1]->get_result()) )
*/
            // The Add_Location is done in a separate thread and will not end
            // while a transaction is not finished
            TEST_INFO("Try to add a cohort location")
            AddLocationThread adt1("AnotherLocation");
            adt1.start();
            Cdmw::OsSupport::OS::sleep( timescale * 500 );
            TEST_CHECK(adt1.get_done()==false);

            // The declaration of the unexisting location on_failure allow the
            // transactions to be finished
            TEST_INFO("Declare the unexisting location on_failure")
            Cdmw::FT::DataStoreBase::On_Failure("UnexistingCohortTest");
            Cdmw::OsSupport::OS::sleep( timescale * MAX_TRANSACTION_IN_PROGRESS * 150 );
            TEST_SUCCEED();

            TEST_INFO("Verify operation threads are now done with good result")
            TEST_CHECK(it1.get_done());
            test_ok = true;
            for (int i =0; i<MAX_TRANSACTION_IN_PROGRESS; i++)
            {
                if ( !(upd_tbl[i]->get_done()) || !(upd_tbl[i]->get_result()) )
                { 
                    test_ok = false;
                    TEST_INFO("Operation " << i << " has unexpected result");
                    break;
                }
            }
            TEST_CHECK(test_ok);
            TEST_INFO("and that CohortTest received send_prepare_update..")
	        Cdmw::OsSupport::OS::sleep( timescale * 1000 );
            std::string answer = cohort_obj->get_answer();
            std::cout<<"answer "<<answer<<std::endl;
            TEST_CHECK(answer=="send_prepare_update/yes")
            TEST_INFO("..then received commit")
	        Cdmw::OsSupport::OS::sleep( timescale * 1000 );
            answer = cohort_obj->get_answer();
            std::cout<<"answer "<<answer<<std::endl;
            TEST_CHECK(answer=="send_commit/end");
            TEST_INFO("check update done localy")
            TEST_CHECK(myDataStore1->select(2)=="two");

            TEST_INFO("Verify add_location thread is now done")
	        Cdmw::OsSupport::OS::sleep( timescale * 1000 );
            TEST_CHECK(adt1.get_done());
            adt1.join();
            Cdmw::FT::DataStoreBase::On_Failure("AnotherLocation");
            it1.join();
            for (int i =0; i<MAX_TRANSACTION_IN_PROGRESS; i++)
            {
                upd_tbl[i]->join();
                delete upd_tbl[i];
            }
        }
     
      
        // In this section we test Coordinator resumption when Cohort -> Coordinator
        // We first go to cohort mode and begin a transaction for cohort
        // then we activate again the coordinator and check its resumption
        Cdmw::OsSupport::OS::sleep( timescale * 1000 );
        TEST_INFO("Deactivate Coordinator (activate cohort)")
        myDataStore1->deactivate();
        TEST_SUCCEED();

        TEST_INFO("Get cohort");
        // Group Id 2 is 1 (first param of TestDataStore) * 2
        CORBA::Object_var group_cohort = m_orb->string_to_object(multicast_addr2.str().c_str());
        CdmwFT::Transaction::TPCCohort_var cohort = CdmwFT::Transaction::TPCCohort::_unchecked_narrow (group_cohort.in());
        TEST_SUCCEED();
        TEST_INFO("Create a Codec");
        typedef Cdmw::OrbSupport::Codec<int> TestCodecOid;
        TestCodecOid* oidcodec = new TestCodecOid(m_orb.in());
        CORBA::OctetSeq_var seq_oid = oidcodec->encode(1);
        TEST_SUCCEED();

        TEST_INFO("realise a send_prepare_delete on the cohort")
        CdmwFT::Transaction::TransactionIdentification id;
        id.id=1;
        id.initiator=Cdmw::FT::DataStoreBase::Get_Location_Name(); 


        CdmwFT::Transaction::StateChanges change;
        change.length(1);
        change[0].dsid = myDataStore1->get_dsid();
        CdmwFT::Transaction::StateChangeRecords records;
        records.length(1);
        records[0].oid_seq = seq_oid;
        records[0].change_data;
        records[0].change_data._d(CdmwFT::Transaction::DS_REMOVAL);
        change[0].records = records;


        cohort->send_prepare(id, change);
        TEST_SUCCEED();
        Cdmw::OsSupport::OS::sleep( timescale * 500 );

        TEST_INFO("Activate again the coordinator")
        myDataStore1->activate();
        TEST_SUCCEED();
        Cdmw::OsSupport::OS::sleep( timescale * 500 );
        TEST_INFO("Verify CohortTest received send_get_transaction_status")
        TEST_CHECK(cohort_obj->get_answer()=="send_get_transaction_status/COMMITTED");
        TEST_INFO("then received commit")
        TEST_CHECK(cohort_obj->get_answer()=="send_commit/end");
           

        // Deactivation necessary before delete (or orb shutdown segmentation fault)
        Cdmw::FT::DataStoreBase::On_Failure("CohortTest");
        m_rootPOA->deactivate_object (cohort_id.in ());      
        delete cohort_obj;

    } 
    catch (...)
    {
        std::ostringstream info_stream1;
        info_stream1 << "Test State Manager Exception caught : " << std::endl;
        TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }
  

    TEST_INFO("");
    TEST_INFO("--IIOP Section--");
    TEST_INFO("");
    TEST_INFO("");



    try 
    {         
        const int MAX_TRANSACTION_DONE = 10;
        const int MAX_TRANSACTION_IN_PROGRESS = 10;
        TEST_INFO("Create a DataStore");
        Cdmw::OsSupport::OS::Timeval cohort_timeout;
        cohort_timeout.seconds=20;
        cohort_timeout.microseconds=0;
        Cdmw::OsSupport::OS::Timeval coordinator_timeout;
        coordinator_timeout.seconds=20;
        coordinator_timeout.microseconds=0;
      
        Cdmw::FT::DataStoreContext context2(m_orb.in(),
                                            m_rootPOA.in(),
                                            MAX_TRANSACTION_IN_PROGRESS, MAX_TRANSACTION_DONE,
                                            cohort_timeout,
                                            coordinator_timeout,
                                            1);
        
        myDataStore2 = new TestDataStore(4,
                                         m_orb.in(),
                                         m_rootPOA.in(),
                                         context2);
        
        TEST_SUCCEED();
        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        Cdmw::OsSupport::OS::sleep( timescale * 500 );
      
        TEST_INFO("Activate Coordinator");
        myDataStore2->activate();
        TEST_SUCCEED();
      
        // Test local insertion
        TEST_INFO("Test of Insert when no cohorts are declared");
        myDataStore2->insert(0,"zero");
        TEST_CHECK(myDataStore2->select(0)=="zero");

        TEST_INFO("Get coordinator");
        Cdmw::FT::TransactionEngineRef ref = context2.get_ref_for_location(Cdmw::OsSupport::OS::get_hostname());
        coordinator = ref.coordinator;
        TEST_SUCCEED();


        TEST_INFO("Instanciate CohortTest impl");
        CohortTest * cohort_obj = new CohortTest();
        TEST_SUCCEED();
 
        TEST_INFO("Set cohorts locations")
        CdmwFT::Transaction::TPCCohort_var cohort_test = cohort_obj->_this();
        
        context2.add_location(4, "CohortTest");
        context2.add_ref_for_location("CohortTest",
                                      CdmwFT::Transaction::TPCCoordinator::_nil(),
                                      cohort_test.in());
        

        TEST_INFO("Insert a <key, value>")
        myDataStore2->insert(1,"un");
        Cdmw::OsSupport::OS::sleep( timescale * 1000 );
        TEST_SUCCEED();

        TEST_INFO("Verify the CohortTest received send_prepare_insert")
        TEST_CHECK(cohort_obj->get_answer()=="prepare_insert/yes");
        TEST_INFO("then received commit")
        TEST_CHECK(cohort_obj->get_answer()=="commit/end");
        TEST_INFO("And that insertion done localy");
        TEST_CHECK(myDataStore2->select(1)=="un");

        // This kind of test that catch  AlreadyExistException is already done
        // in local (see teststorageinterface) but here we test the exception
        // raised in Transaction Engine Coordinator
        TEST_INFO("Try to insert already existing key");
        try
        {
            myDataStore2->insert(1,"un");
            TEST_FAILED();
        }
        catch(const Cdmw::FT::AlreadyExistException& e)
        {
            TEST_INFO(e.what());
            TEST_SUCCEED();
        }
     

        // In this section we test behaviour when a cohort answer no (and one yes)
        {
            // The transaction above have finished (cause we are in the same tread)
            // and there is no other transaction so we can add a location
            TEST_INFO("Instanciate BadCohortTest impl");
            BadCohortTest * bad_cohort_obj = new BadCohortTest();
            TEST_SUCCEED();
            
            CdmwFT::Transaction::TPCCohort_var bad_cohort = bad_cohort_obj->_this();


            TEST_INFO("Add a cohort location")
            context2.add_location(4, "BadCohortTest");
            context2.add_ref_for_location("BadCohortTest",
                                          CdmwFT::Transaction::TPCCoordinator::_nil(),
                                          bad_cohort.in());
            
            TEST_SUCCEED();
	
    
            TEST_INFO("Thread instanciated for Update a <key, value>")
            RIUOperationThread it1(myDataStore2, 1, "one", CdmwFT::Transaction::DS_UPDATE);
            it1.start();
            Cdmw::OsSupport::OS::sleep( timescale * 1000 );
            TEST_SUCCEED();
            TEST_INFO("Verify the CohortTest received prepare_update..")
            TEST_CHECK(cohort_obj->get_answer()=="prepare_update/yes");
            TEST_INFO("..then received rollback/end")
            TEST_CHECK(cohort_obj->get_answer()=="rollback/end");
            TEST_INFO("Declare the BadCohortTest on_failure")	
            Cdmw::OsSupport::OS::sleep( timescale * 1000 );
            Cdmw::FT::DataStoreBase::On_Failure("BadCohortTest");
            Cdmw::OsSupport::OS::sleep( timescale * 1000 );
            TEST_SUCCEED();
            TEST_INFO("Verify the transaction is finished")
            TEST_CHECK(it1.get_done());
            TEST_INFO("Verify update not done localy")
            TEST_CHECK(myDataStore2->select(1)=="un");
            it1.join();
        }
      
      
        // In this section we test the MAX_TRANSACTION_IN_PROGRESS
        // and the location change forbidden during transactions in progress
        {
            // The transaction above have finished (cause we are in the same tread)
            // and there is no other transaction so we can add a location
            TEST_INFO("Instanciate CohortTest impl");
            CohortTest * nothing_cohort_obj = new CohortTest();
            nothing_cohort_obj->do_nothing();
            TEST_SUCCEED();

            CdmwFT::Transaction::TPCCohort_var nothing_cohort_test = nothing_cohort_obj->_this();
            context2.add_location(4, "UnexistingCohortTest");
            context2.add_ref_for_location("BadCohortTest",
                                          CdmwFT::Transaction::TPCCoordinator::_nil(),
                                          nothing_cohort_test.in());
            

            // The next transactions are done in a separate thread and will not end
            // immediatly because the location "NotExisting" will not answer
            TEST_INFO("Thread instanciated for Insert a <key, value>")
            RIUOperationThread it1(myDataStore2, 2, "deux", CdmwFT::Transaction::DS_INSERTION);
            it1.start();
            Cdmw::OsSupport::OS::sleep( timescale * 1000 );
            TEST_INFO("Verify transaction not finished..")
            TEST_CHECK(it1.get_done()==false);

            TEST_INFO("..but CohortTest receives prepare_insert & answers yes")
            TEST_CHECK(cohort_obj->get_answer()=="prepare_insert/yes");

            TEST_INFO("Create MAX_TRANSACTION_IN_PROGRESS-1 update transactions");
            // which is MAX_TRANSACTION_IN_PROGRESS transactions with previous Insert
            RIUOperationThread* upd_tbl[MAX_TRANSACTION_IN_PROGRESS];
            for (int i =0; i<(MAX_TRANSACTION_IN_PROGRESS-1); i++)
            {
                upd_tbl[i] = new RIUOperationThread(myDataStore2, 2, "two", CdmwFT::Transaction::DS_UPDATE);
                upd_tbl[i]->start();
            }
            Cdmw::OsSupport::OS::sleep( timescale * MAX_TRANSACTION_IN_PROGRESS * 150 );
            TEST_SUCCEED();
            TEST_INFO("Verify transactions not finished.")
            bool test_ok = true;
            for (int i =0; i<(MAX_TRANSACTION_IN_PROGRESS-1); i++)
            {
                if (upd_tbl[i]->get_done())
                { 
                    test_ok = false;
                    break;
                }
            }
            TEST_CHECK(test_ok);
            TEST_INFO("and that CohortTest doesn't receive prepare_update & doesn't answer yes")
            TEST_CHECK(cohort_obj->get_answer()!="prepare_update/yes");

            TEST_INFO("Try to create one more update transaction..")
            upd_tbl[MAX_TRANSACTION_IN_PROGRESS-1] =
                new RIUOperationThread(myDataStore2, 2, "two", CdmwFT::Transaction::DS_UPDATE);
            upd_tbl[MAX_TRANSACTION_IN_PROGRESS-1]->start();
            TEST_SUCCEED();
            Cdmw::OsSupport::OS::sleep( timescale * 500 );
            // MAX_TRANSACTION_IN_PROGRESS reached : transaction not created
            TEST_INFO("Verify this operation is not finished.");
            TEST_CHECK( !upd_tbl[MAX_TRANSACTION_IN_PROGRESS-1]->get_done() );
            
            TEST_INFO("and that CohortTest doesn't receive prepare_update & doesn't answer yes")
            TEST_CHECK(cohort_obj->get_answer()!="prepare_update/yes");
            
//             std::cerr << "get_done: " << (upd_tbl[MAX_TRANSACTION_IN_PROGRESS-1]->get_done() ? "true" : "false") << std::endl;
//             std::cerr << "get_result: " << (upd_tbl[MAX_TRANSACTION_IN_PROGRESS-1]->get_result()  ? "true" : "false") << std::endl;

//             TEST_CHECK( (upd_tbl[MAX_TRANSACTION_IN_PROGRESS-1]->get_done()) &&
//                         !(upd_tbl[MAX_TRANSACTION_IN_PROGRESS-1]->get_result()) )

            // The Add_Location is done in a separate thread and will not end
            // while a transaction is not finished
            TEST_INFO("Try to add a cohort location")
            AddLocationThread adt1("AnotherLocation", true);
            adt1.start();
            Cdmw::OsSupport::OS::sleep( timescale * 500 );
            TEST_CHECK(adt1.get_done()==false);

            // The declaration of the unexisting location on_failure allow the
            // transactions to be finished
            TEST_INFO("Declare the unexisting location on_failure")
            Cdmw::FT::DataStoreBase::On_Failure("UnexistingCohortTest");
            Cdmw::OsSupport::OS::sleep( timescale * MAX_TRANSACTION_IN_PROGRESS * 150 );
            TEST_SUCCEED();
            Cdmw::OsSupport::OS::sleep( timescale * 3000 );
            
            TEST_INFO("Verify operation threads are now done with good result")
            TEST_CHECK(it1.get_done());
            test_ok = true;
            for (int i =0; i<MAX_TRANSACTION_IN_PROGRESS; i++)
            {
                if ( !(upd_tbl[i]->get_done()) || !(upd_tbl[i]->get_result()) )
                { 
                    test_ok = false;
                    break;
                }
            }
            TEST_CHECK(test_ok);

            TEST_INFO("and that CohortTest received prepare_update..")
            std::string answer = cohort_obj->get_answer();
            std::cout<<"answer "<<answer<<std::endl;
            TEST_CHECK(answer=="prepare_update/yes")
            TEST_INFO("..then received commit")
            answer = cohort_obj->get_answer();
            std::cout<<"answer "<<answer<<std::endl;
            TEST_CHECK(answer=="commit/end");
            TEST_INFO("check update done localy")
            TEST_CHECK(myDataStore2->select(2)=="two");

            TEST_INFO("Verify add_location thread is now done")
            TEST_CHECK(adt1.get_done());
            adt1.join();
            Cdmw::FT::DataStoreBase::On_Failure("AnotherLocation");
            it1.join();
            for (int i =0; i<MAX_TRANSACTION_IN_PROGRESS; i++)
            {
                upd_tbl[i]->join();
                delete upd_tbl[i];
            }
        }

      
        // In this section we test Coordinator resumption when Cohort -> Coordinator
        // We first go to cohort mode and begin a transaction for cohort
        // then we activate again the coordinator and check its resumption
        Cdmw::OsSupport::OS::sleep( timescale * 1000 );
        TEST_INFO("Deactivate Coordinator (activate cohort)")
        myDataStore2->deactivate();
        TEST_SUCCEED();

        TEST_INFO("Create a Codec");
        typedef Cdmw::OrbSupport::Codec<int> TestCodecOid;
        TestCodecOid* oidcodec = new TestCodecOid(m_orb.in());
        CORBA::OctetSeq_var seq_oid = oidcodec->encode(1);
        TEST_SUCCEED();

        TEST_INFO("realise a prepare_delete on the cohort")
        CdmwFT::Transaction::TransactionIdentification id;
        id.id=1;
        id.initiator=Cdmw::FT::DataStoreBase::Get_Location_Name();
        char * sender;

        CdmwFT::Transaction::StateChanges change;
        change.length(1);
        change[0].dsid = myDataStore2->get_dsid();
        CdmwFT::Transaction::StateChangeRecords records;
        records.length(1);
        records[0].oid_seq = seq_oid;
        records[0].change_data;
        records[0].change_data._d(CdmwFT::Transaction::DS_REMOVAL);
        change[0].records = records;
        

        ref.cohort->prepare( change, id, sender);
        std::cout<<"initiator "<<id.initiator<< " sender "<<sender<<std::endl;
        TEST_SUCCEED();
        Cdmw::OsSupport::OS::sleep( timescale * 500 );

        TEST_INFO("Activate again the coordinator")
        myDataStore2->activate();
        TEST_SUCCEED();
        Cdmw::OsSupport::OS::sleep( timescale * 2000 );
        TEST_INFO("Verify CohortTest received get_transaction_status")
        TEST_CHECK(cohort_obj->get_answer()=="get_transaction_status/COMMITTED");
        TEST_INFO("then received commit")
        TEST_CHECK(cohort_obj->get_answer()=="commit/end");   

        cohort_obj->_remove_ref();

    } 
    catch (CORBA::SystemException& ex)
    {
        std::ostringstream info_stream1;
        info_stream1 << "Test State Manager Exception caught : " <<ex<< std::endl;
        TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }  
    catch(const std::bad_alloc & )
    {
        std::cout<<"bad_alloc"<<std::endl;
    }
    catch (...)
    {
        std::ostringstream info_stream1;
        info_stream1 << "Test State Manager Exception caught : " << std::endl;
        TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }  
    delete myDataStore1;
    delete myDataStore2;
}

