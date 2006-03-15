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

#include "testtransengine/TestTransEngineCohort.hpp"
#include "Foundation/commonsvcs/datastore/StorageHome.hpp"
#include "Foundation/commonsvcs/datastore/StorageObject.hpp"
#include <FaultTolerance/ftstatemanager/DataStoreContext.hpp>
#include <FaultTolerance/ftstatemanager/TXDataStore.hpp>
#include <FaultTolerance/ftstatemanager/DataStore.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <ftstatemanager/MIOPMutex.hpp>
// namespace Cdmw {
// namespace FT {


class CoordinatorTest : public virtual POA_CdmwFT::Transaction::TPCCoordinator,
                        public virtual PortableServer::RefCountServantBase
{
public:
    CoordinatorTest()
        : m_status(CdmwFT::Transaction::UNKNOWN)
    {
    }

    virtual ~CoordinatorTest()
    {
    }

    void send_yes (const char * sender,
                   const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException)
    {
        //std::cout<<"send_yes : "<<sender<<std::endl;
		m_answer = "send_yes";
    }
      
    void send_no (const char * sender,
                  const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException)
    {
        //std::cout<<"send_no : "<<sender<<std::endl;
		m_answer = "send_no";
    }

    void send_end (const char * sender,
                   const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException)
    {
        //std::cout<<"send_end : "<<sender<<std::endl;
		m_answer = "send_end";
    }

    void send_get_transaction_status (const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException)
    {
        //std::cout<<"get_transaction_status : "<<std::endl;
    }

    void send_set_transaction_status (const char * sender,
                                      const CdmwFT::Transaction::TransactionIdentification & id,
                                      CdmwFT::Transaction::TransactionStatus status)
        throw(CORBA::SystemException)
    {
        m_status = status;
        std::cout << "send_set_transaction_status status =  " << m_status << std::endl;
    }

    CdmwFT::Transaction::TransactionStatus get_transaction_status (CdmwFT::Transaction::TransactionIdentification & id,
                                                      CORBA::String_out sender)
        throw(CORBA::SystemException){/*std::cout<<__LINE__<<std::endl;*/; return CdmwFT::Transaction::ABORTED;};
    
    
	std::string get_answer()
	{
		std::string temp = m_answer;
		m_answer = "";
		return temp;
	}

	CdmwFT::Transaction::TransactionStatus get_status()
	{
		return m_status;	
	}
private:
	CdmwFT::Transaction::TransactionStatus m_status;
	std::string m_answer;

};

TestTransEngineCohort::TestTransEngineCohort (CORBA::ORB_ptr orb, PortableServer::POA_ptr rootPOA)
	:Cdmw::TestUtils::Testable("Test Trans Engine Cohort")
{
    m_orb = CORBA::ORB::_duplicate(orb);
    m_rootPOA = PortableServer::POA::_duplicate(rootPOA);
}

TestTransEngineCohort::~TestTransEngineCohort ()
{
}


void 
TestTransEngineCohort::do_tests()
{
    set_nbOfRequestedTestOK(89);
    
    const int MAX_TRANSACTION_IN_PROGRESS = 100;
    const int MAX_TRANSACTION_DONE = 100;

    try 
    {         
        {
            TEST_INFO("Create a DataStore");
            typedef Cdmw::FT::DataStore<int, std::string> TestDataStore;
            Cdmw::OsSupport::OS::Timeval cohort_timeout;
            cohort_timeout.seconds=10;
            cohort_timeout.microseconds=0;
            Cdmw::OsSupport::OS::Timeval coordinator_timeout;
            coordinator_timeout.seconds=10;
            coordinator_timeout.microseconds=0;
            std::ostringstream multicast_domain;
            multicast_domain << Cdmw::OsSupport::OS::get_logname(); 
            multicast_domain << "ftdomain" << std::ends;

           Cdmw::FT::DataStoreContext * context =
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
            
            TestDataStore* myDataStore = new TestDataStore(2,
                                                           m_orb.in(),
                                                           m_rootPOA.in(),
                                                           *context);
            
            TEST_SUCCEED();

			// essayer de changer l'adresse et le groupe 
            TEST_INFO("Create a cohort");
             // WARNING: a context should already have been created by previous test (TestTransEngine) 
             //          and expected counter for ftdomain-i should now be 4 (cohort) and 5 (coordinator) ! 
             std::ostringstream multicast_addr;
             multicast_addr << "corbaloc:miop:1.0@1.0-" << Cdmw::OsSupport::OS::get_logname() << "ftdomain-4/225.1.1.8:5555" << std::ends;
//             CORBA::Object_var group = m_orb->string_to_object(multicast_addr.str().c_str());

//            CdmwFT::Transaction::TPCCohort_var  cohort = CdmwFT::Transaction::TPCCohort::_unchecked_narrow (group.in());


            CdmwFT::Transaction::TPCCohort_var cohort = CdmwFT::Transaction::TPCCohort::_duplicate(context->get_cohort_ref());

            TEST_SUCCEED();

            TEST_INFO("Create the coordinator");
            CoordinatorTest * coordinator_obj = new CoordinatorTest();
            TEST_SUCCEED();

            myDataStore->activate();
            TEST_SUCCEED();

            Cdmw::OsSupport::OS::sleep( 1000 );

            myDataStore->deactivate();
            TEST_SUCCEED();

            Cdmw::OsSupport::OS::sleep( 1000 );

            myDataStore->activate();
            TEST_SUCCEED();

            Cdmw::OsSupport::OS::sleep( 1000 );

            myDataStore->deactivate();
            TEST_SUCCEED();

            Cdmw::OsSupport::OS::sleep( 1000 );
            

            std::ostringstream multicast_addr2;
            multicast_addr2 << "corbaloc:miop:1.0@1.0-" << Cdmw::OsSupport::OS::get_logname() << "ftdomain-5/225.1.1.8:5555" << std::ends;
            CORBA::Object_var group2 = m_orb->string_to_object(multicast_addr2.str().c_str());
            PortableServer::ObjectId_var coordinator_id = m_rootPOA->create_id_for_reference(group2.in());
            TEST_SUCCEED();


            TEST_INFO("activate the coordinator")
            try
            {
                m_rootPOA->activate_object_with_id (coordinator_id.in (),coordinator_obj);
            }
            catch(...)
            {
                TEST_FAILED();
            }

            TEST_INFO("Create a Codec");
            typedef Cdmw::OrbSupport::Codec<int> TestCodecOid;
            typedef Cdmw::OrbSupport::Codec<std::string> TestCodecData;

            TestCodecData* datacodec = new TestCodecData(m_orb.in());
            TestCodecOid* oidcodec = new TestCodecOid(m_orb.in());


            std::string text("hello1");
            CORBA::OctetSeq_var seq_oid = oidcodec->encode(1);
            CORBA::OctetSeq_var seq_data = datacodec->encode(text);
            TEST_SUCCEED();



            CdmwFT::Transaction::TransactionIdentification id;
            id.id=1;
            id.initiator="initiator"; 

			TEST_INFO("realise a prepare_insert on the cohort");

            CdmwFT::Transaction::StateChanges change;
            change.length(1);
            change[0].dsid = myDataStore->get_dsid();
            CdmwFT::Transaction::StateChangeRecords records;
            records.length(1);
            records[0].oid_seq = seq_oid;
            records[0].change_data.data_seq(seq_data);
            records[0].change_data._d(CdmwFT::Transaction::DS_INSERTION);
            change[0].records = records;


            CDMW_FT_BEGIN_MIOP_MUTEX;
            cohort->send_prepare(id, change);
            CDMW_FT_END_MIOP_MUTEX;
            int timescale = Cdmw::TestUtils::Testable::get_timescale();

            Cdmw::OsSupport::OS::sleep( timescale * 500 );
            TEST_SUCCEED();

            TEST_INFO("Try to check if the OID is not present on the data store");
            try
            {
                myDataStore->select(1);
                TEST_FAILED();
            }
            catch(Cdmw::FT::NotFoundException)
            {
                TEST_SUCCEED();
            }

            TEST_INFO("Realise a commit on the previous transaction");
            CDMW_FT_BEGIN_MIOP_MUTEX;
            cohort->send_commit(id);  
            CDMW_FT_END_MIOP_MUTEX;
            Cdmw::OsSupport::OS::sleep( timescale * 500 );
            TEST_SUCCEED();

            TEST_INFO("Try to check if the OID is present on the data store");
            try
            {
                std::cout<<myDataStore->size()<<std::endl;
                myDataStore->select(1);
                TEST_SUCCEED();
            }
            catch(Cdmw::FT::NotFoundException)
            {
                TEST_FAILED();
            }


            TEST_INFO("prepared insert oid=2");
            id.id=2;
            id.initiator="initiator"; 
            text = "hello2";
            seq_oid = oidcodec->encode(2);
            seq_data = datacodec->encode(text);
            TEST_SUCCEED();

            records[0].oid_seq = seq_oid;
            records[0].change_data.data_seq(seq_data);
            records[0].change_data._d(CdmwFT::Transaction::DS_INSERTION);
            change[0].records = records;


            TEST_INFO("Realise a prepared_insert");
            CDMW_FT_BEGIN_MIOP_MUTEX;
            cohort->send_prepare(id, change);  
            CDMW_FT_END_MIOP_MUTEX;
            Cdmw::OsSupport::OS::sleep( timescale * 500 );
            TEST_SUCCEED();

            TEST_INFO("prepared insert oid=3");
            id.id=3;
            id.initiator="initiator"; 
            text = "hello3";
            seq_oid = oidcodec->encode(3);
            seq_data = datacodec->encode(text);
            TEST_SUCCEED();
            records[0].oid_seq = seq_oid;
            records[0].change_data.data_seq(seq_data);
            records[0].change_data._d(CdmwFT::Transaction::DS_INSERTION);
            change[0].records = records;

			TEST_INFO("Realise a prepared_insert");
            CDMW_FT_BEGIN_MIOP_MUTEX;
            cohort->send_prepare(id, change);  
            CDMW_FT_END_MIOP_MUTEX;
            Cdmw::OsSupport::OS::sleep( timescale * 500 );
            TEST_SUCCEED();

            TEST_INFO("prepared insert oid=4");
            id.id=4;
            id.initiator="initiator"; 
            text = "hello4";
            seq_oid = oidcodec->encode(4);
            seq_data = datacodec->encode(text);
            TEST_SUCCEED();
            records[0].oid_seq = seq_oid;
            records[0].change_data.data_seq(seq_data);
            records[0].change_data._d(CdmwFT::Transaction::DS_INSERTION);
            change[0].records = records;

			TEST_INFO("Realise a prepared_insert");
            CDMW_FT_BEGIN_MIOP_MUTEX;
            cohort->send_prepare(id, change);  
            CDMW_FT_END_MIOP_MUTEX;
            Cdmw::OsSupport::OS::sleep( timescale * 500 );
            TEST_SUCCEED();

            TEST_INFO("prepared insert oid=5");
            id.id=5;
            id.initiator="initiator"; 
            text = "hello5";
            seq_oid = oidcodec->encode(5);
            seq_data = datacodec->encode(text);
            TEST_SUCCEED();
            records[0].oid_seq = seq_oid;
            records[0].change_data.data_seq(seq_data);
            records[0].change_data._d(CdmwFT::Transaction::DS_INSERTION);
            change[0].records = records;

			TEST_INFO("Realise a prepared_insert");
            CDMW_FT_BEGIN_MIOP_MUTEX;
            cohort->send_prepare(id, change);  
            CDMW_FT_END_MIOP_MUTEX;
            Cdmw::OsSupport::OS::sleep( timescale * 500 );
            TEST_SUCCEED();

            TEST_INFO("Try to check if the OID is not present on the data store");
            try
            {
                myDataStore->select(2);
                TEST_FAILED();
            }
            catch(Cdmw::FT::NotFoundException)
            {
                TEST_SUCCEED();
            }

            TEST_INFO("realise a commit on oid=3");
            id.id=3;
            CDMW_FT_BEGIN_MIOP_MUTEX;
            cohort->send_commit(id);  
            CDMW_FT_END_MIOP_MUTEX;
            Cdmw::OsSupport::OS::sleep( timescale * 500 );
            TEST_SUCCEED();

            TEST_INFO("Try to check if the OID is not present on the data store");
            try
            {
                myDataStore->select(3);
                TEST_SUCCEED();
            }
            catch(Cdmw::FT::NotFoundException)
            {
                TEST_FAILED();
            }

            TEST_INFO("realise a commit on oid=4");
            id.id=4;
            CDMW_FT_BEGIN_MIOP_MUTEX;
            cohort->send_commit(id);  
            CDMW_FT_END_MIOP_MUTEX;
            Cdmw::OsSupport::OS::sleep( timescale * 500 );
            TEST_SUCCEED();

            TEST_INFO("Try to check if the OID is not present on the data store");
            try
            {
                myDataStore->select(4);
                TEST_SUCCEED();
            }
            catch(Cdmw::FT::NotFoundException)
            {
                TEST_FAILED();
            }

			TEST_INFO("realise an aborted on oid=5");
            id.id=5;
            CDMW_FT_BEGIN_MIOP_MUTEX;
            cohort->send_rollback(id);  
            CDMW_FT_END_MIOP_MUTEX;
            Cdmw::OsSupport::OS::sleep( timescale * 500 );
            TEST_SUCCEED();

            TEST_INFO("Try to check if the OID is not present on the data store");
            try
            {
                myDataStore->select(5);
				TEST_FAILED();
            }
            catch(Cdmw::FT::NotFoundException)
            {
                TEST_SUCCEED();
            }

            CdmwFT::Transaction::TransactionStatus status;

			// transaction status
			// oid 1 => committed
			// oid 2 => prepared
			// oid 3 => committed
			// oid 4 => committed
			// oid 5 => aborted
			id.id = 1;
            CDMW_FT_BEGIN_MIOP_MUTEX;
			cohort->send_get_transaction_status(id);
            CDMW_FT_END_MIOP_MUTEX;
			Cdmw::OsSupport::OS::sleep( timescale * 2000 );
                        status = coordinator_obj->get_status();
                        std::cout << "status = " << status << std::endl;
			TEST_CHECK(status == CdmwFT::Transaction::COMMITTED);

			id.id = 2;
            CDMW_FT_BEGIN_MIOP_MUTEX;
			cohort->send_get_transaction_status(id);
            CDMW_FT_END_MIOP_MUTEX;
			Cdmw::OsSupport::OS::sleep( timescale * 2000 );
                        status = coordinator_obj->get_status();
                        std::cout << "status = " << status << std::endl;
                        
			TEST_CHECK(status == CdmwFT::Transaction::PREPARED);

			id.id = 3;
            CDMW_FT_BEGIN_MIOP_MUTEX;
			cohort->send_get_transaction_status(id);
            CDMW_FT_END_MIOP_MUTEX;
			Cdmw::OsSupport::OS::sleep( timescale * 2000 );
			TEST_CHECK(coordinator_obj->get_status() == CdmwFT::Transaction::COMMITTED);

			id.id = 4;
            CDMW_FT_BEGIN_MIOP_MUTEX;
			cohort->send_get_transaction_status(id);
            CDMW_FT_END_MIOP_MUTEX;
			Cdmw::OsSupport::OS::sleep( timescale * 2000 );
			TEST_CHECK(coordinator_obj->get_status() == CdmwFT::Transaction::COMMITTED);

			id.id = 5;
            CDMW_FT_BEGIN_MIOP_MUTEX;
			cohort->send_get_transaction_status(id);
            CDMW_FT_END_MIOP_MUTEX;
			Cdmw::OsSupport::OS::sleep( timescale * 2000 );
			TEST_CHECK(coordinator_obj->get_status() == CdmwFT::Transaction::ABORTED);

			id.id = 6;
            CDMW_FT_BEGIN_MIOP_MUTEX;
			cohort->send_get_transaction_status(id);
            CDMW_FT_END_MIOP_MUTEX;
			Cdmw::OsSupport::OS::sleep( timescale * 2000 );
			TEST_CHECK(coordinator_obj->get_status() == CdmwFT::Transaction::UNKNOWN);

			id.id = 2;
            CDMW_FT_BEGIN_MIOP_MUTEX;
			cohort->send_set_transaction_status("moi", id, CdmwFT::Transaction::COMMITTED);
            CDMW_FT_END_MIOP_MUTEX;
			Cdmw::OsSupport::OS::sleep( timescale * 2000 );

            CDMW_FT_BEGIN_MIOP_MUTEX;
			cohort->send_get_transaction_status(id);
            CDMW_FT_END_MIOP_MUTEX;
			Cdmw::OsSupport::OS::sleep( timescale * 2000 );
			TEST_CHECK(coordinator_obj->get_status() == CdmwFT::Transaction::COMMITTED);

			TEST_INFO("update a data");
			id.id = 7;
			text = "hello4";
            seq_data = datacodec->encode(text);
            seq_oid = oidcodec->encode(4);
            records[0].oid_seq = seq_oid;
            records[0].change_data.data_seq(seq_data);
            records[0].change_data._d(CdmwFT::Transaction::DS_UPDATE);
            change[0].records = records;

            CDMW_FT_BEGIN_MIOP_MUTEX;
			cohort->send_prepare(id, change);
            CDMW_FT_END_MIOP_MUTEX;
			Cdmw::OsSupport::OS::sleep( timescale * 2000 );
			TEST_CHECK(coordinator_obj->get_answer() == "send_yes");

			id.id = 8;
            CDMW_FT_BEGIN_MIOP_MUTEX;
			cohort->send_prepare(id, change);
            CDMW_FT_END_MIOP_MUTEX;
			Cdmw::OsSupport::OS::sleep( timescale * 500 );
			TEST_CHECK(coordinator_obj->get_answer() == "send_no");

 			id.id = 9;
            records[0].change_data._d(CdmwFT::Transaction::DS_INSERTION);
            change[0].records = records;
            CDMW_FT_BEGIN_MIOP_MUTEX;
			cohort->send_prepare(id, change);
            CDMW_FT_END_MIOP_MUTEX;
			Cdmw::OsSupport::OS::sleep( timescale * 500 );
			TEST_CHECK(coordinator_obj->get_answer() == "send_no");

			id.id = 10;
            records[0].oid_seq = seq_oid;
            records[0].change_data;
            records[0].change_data._d(CdmwFT::Transaction::DS_REMOVAL);
            change[0].records = records;
            CDMW_FT_BEGIN_MIOP_MUTEX;
			cohort->send_prepare(id, change);
            CDMW_FT_END_MIOP_MUTEX;
			Cdmw::OsSupport::OS::sleep( timescale * 500 );
			TEST_CHECK(coordinator_obj->get_answer() == "send_no");

			id.id = 11;
            CDMW_FT_BEGIN_MIOP_MUTEX;
			cohort->send_prepare(id, change);
            CDMW_FT_END_MIOP_MUTEX;
			Cdmw::OsSupport::OS::sleep( timescale * 500 );
			TEST_CHECK(coordinator_obj->get_answer() == "send_no");

			id.id = 12;
            seq_oid = oidcodec->encode(1);
            records[0].oid_seq = seq_oid;
            records[0].change_data;
            records[0].change_data._d(CdmwFT::Transaction::DS_REMOVAL);
            change[0].records = records;
            CDMW_FT_BEGIN_MIOP_MUTEX;
			cohort->send_prepare(id, change);
            CDMW_FT_END_MIOP_MUTEX;
			Cdmw::OsSupport::OS::sleep( timescale * 500 );
			TEST_CHECK(coordinator_obj->get_answer() == "send_yes");

            CDMW_FT_BEGIN_MIOP_MUTEX;
			cohort->send_commit(id);
            CDMW_FT_END_MIOP_MUTEX;
			Cdmw::OsSupport::OS::sleep( timescale * 500 );
			TEST_CHECK(coordinator_obj->get_answer() == "send_end");

			id.id = 7;
            CDMW_FT_BEGIN_MIOP_MUTEX;
			cohort->send_commit(id);
            CDMW_FT_END_MIOP_MUTEX;
			Cdmw::OsSupport::OS::sleep( timescale * 500 );
			TEST_CHECK(coordinator_obj->get_answer() == "send_end");

			TEST_INFO("Try to check if the OID is not present on the data store");
            try
            {
                myDataStore->select(1);
				TEST_FAILED();
            }
            catch(Cdmw::FT::NotFoundException)
            {
                 TEST_SUCCEED();
            }

			TEST_INFO("realise " << MAX_TRANSACTION_IN_PROGRESS*1.1 << " prepared insert");
			text = "hello";
            seq_data = datacodec->encode(text);


			for (int i = 20; i<20+MAX_TRANSACTION_IN_PROGRESS*1.1; i++)
			{
				id.id = i;
				seq_oid = oidcodec->encode(i);
                records[0].oid_seq = seq_oid;
                records[0].change_data.data_seq(seq_data);
                records[0].change_data._d(CdmwFT::Transaction::DS_INSERTION);
                change[0].records = records;

                CDMW_FT_BEGIN_MIOP_MUTEX;
				cohort->send_prepare(id, change);
                CDMW_FT_END_MIOP_MUTEX;
			}
			Cdmw::OsSupport::OS::sleep( timescale * 3000 );
			TEST_INFO("check if the cohort supports " << MAX_TRANSACTION_IN_PROGRESS*1.1 << " transactions while the max is " << MAX_TRANSACTION_IN_PROGRESS);
			id.id = 221;
			seq_oid = oidcodec->encode(221);
            records[0].oid_seq = seq_oid;
            records[0].change_data.data_seq(seq_data);
            records[0].change_data._d(CdmwFT::Transaction::DS_INSERTION);
            change[0].records = records;
            CDMW_FT_BEGIN_MIOP_MUTEX;
			cohort->send_prepare(id, change);
            CDMW_FT_END_MIOP_MUTEX;
			Cdmw::OsSupport::OS::sleep( timescale * 500 );
			TEST_CHECK(coordinator_obj->get_answer() == "send_no");

			for (int i = 20; i<20+MAX_TRANSACTION_IN_PROGRESS*1.1; i++)
			{
				id.id = i;
                CDMW_FT_BEGIN_MIOP_MUTEX;
				cohort->send_commit(id);
                CDMW_FT_END_MIOP_MUTEX;
			}
			Cdmw::OsSupport::OS::sleep( timescale * 3000 );

			TEST_INFO("check if the status for the first request is now unknown by the cohort");
			id.id = 1;
            CDMW_FT_BEGIN_MIOP_MUTEX;
			cohort->send_get_transaction_status(id);
            CDMW_FT_END_MIOP_MUTEX;
			Cdmw::OsSupport::OS::sleep( timescale * 500 );
			TEST_CHECK(coordinator_obj->get_status() == CdmwFT::Transaction::UNKNOWN);

       }

    } 
    catch (...)
    {
        std::ostringstream info_stream1;
        info_stream1 << "Test State Manager Exception caught : " << std::endl;
        TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }
 

     try 
     {         
        {
            TEST_INFO("Create a DataStore");
            typedef Cdmw::FT::DataStore<int, std::string> TestDataStore;
            Cdmw::OsSupport::OS::Timeval cohort_timeout;
            cohort_timeout.seconds=10;
            cohort_timeout.microseconds=0;
            Cdmw::OsSupport::OS::Timeval coordinator_timeout;
            coordinator_timeout.seconds=10;
            coordinator_timeout.microseconds=0;
            TEST_SUCCEED();


           Cdmw::FT::DataStoreContext * context =
               new Cdmw::FT::DataStoreContext
               (m_orb.in(),
                m_rootPOA.in(),
                MAX_TRANSACTION_IN_PROGRESS,
                MAX_TRANSACTION_DONE,
                cohort_timeout,
                coordinator_timeout,
                1);

            TestDataStore* myDataStore = new TestDataStore(3,
                                                           m_orb.in(),
                                                           m_rootPOA.in(),
                                                           *context);

            CdmwFT::Transaction::TPCCohort_var cohort = CdmwFT::Transaction::TPCCohort::_duplicate(context->get_cohort_ref());


            myDataStore->activate();
            TEST_SUCCEED();

            Cdmw::OsSupport::OS::sleep( 1000 );

            myDataStore->deactivate();
            TEST_SUCCEED();

            Cdmw::OsSupport::OS::sleep( 1000 );

            myDataStore->activate();
            TEST_SUCCEED();

            Cdmw::OsSupport::OS::sleep( 1000 );

            myDataStore->deactivate();
            TEST_SUCCEED();

            Cdmw::OsSupport::OS::sleep( 1000 );




            TEST_INFO("Create a Codec");
            typedef Cdmw::OrbSupport::Codec<int> TestCodecOid;
            typedef Cdmw::OrbSupport::Codec<std::string> TestCodecData;

            TestCodecData* datacodec = new TestCodecData(m_orb.in());
            TestCodecOid* oidcodec = new TestCodecOid(m_orb.in());


            std::string text("hello1");
            CORBA::OctetSeq_var seq_oid = oidcodec->encode(1);
            CORBA::OctetSeq_var seq_data = datacodec->encode(text);
            TEST_SUCCEED();

            CdmwFT::Transaction::StateChanges change;
            change.length(1);
            change[0].dsid = 3;
            CdmwFT::Transaction::StateChangeRecords records;
            records.length(1);
            records[0].oid_seq = seq_oid;
            records[0].change_data.data_seq(seq_data);
            records[0].change_data._d(CdmwFT::Transaction::DS_INSERTION);
            change[0].records = records;


            CdmwFT::Transaction::TransactionIdentification id;
            id.id=1;
            id.initiator="initiator"; 

			TEST_INFO("realise a prepare_insert on the cohort");
            CORBA::Boolean result = false;
            CORBA::String_var sender;
            result = cohort->prepare(change ,  id, sender.out());
            TEST_CHECK(result);
            int timescale = Cdmw::TestUtils::Testable::get_timescale();

            TEST_INFO("Try to check if the OID is not present on the data store");
            try
            {
                myDataStore->select(1);
                TEST_FAILED();
            }
            catch(Cdmw::FT::NotFoundException)
            {
                TEST_SUCCEED();
            }

			TEST_INFO("Realise a commit on the previous transaction");
            cohort->commit(id, sender.out());  
            Cdmw::OsSupport::OS::sleep( timescale * 500 );
            TEST_SUCCEED();

            TEST_INFO("Try to check if the OID is present on the data store");
            try
            {
                myDataStore->select(1);
                TEST_SUCCEED();
            }
            catch(Cdmw::FT::NotFoundException)
            {
                TEST_FAILED();
            }


            TEST_INFO("prepared insert oid=2");
            id.id=2;
            id.initiator="initiator"; 
            text = "hello2";
            seq_oid = oidcodec->encode(2);
            seq_data = datacodec->encode(text);
            TEST_SUCCEED();
            records[0].oid_seq = seq_oid;
            records[0].change_data.data_seq(seq_data);
            records[0].change_data._d(CdmwFT::Transaction::DS_INSERTION);
            change[0].records = records;

			TEST_INFO("Realise a prepared_insert");
            result = cohort->prepare(change, id, sender.out());  
            TEST_CHECK(result);

            TEST_INFO("prepared insert oid=3");
            id.id=3;
            id.initiator="initiator"; 
            text = "hello3";
            seq_oid = oidcodec->encode(3);
            seq_data = datacodec->encode(text);
            TEST_SUCCEED();
            records[0].oid_seq = seq_oid;
            records[0].change_data.data_seq(seq_data);
            records[0].change_data._d(CdmwFT::Transaction::DS_INSERTION);
            change[0].records = records;

            TEST_INFO("Realise a prepared_insert");
            result = cohort->prepare(change, id, sender.out());  
            TEST_CHECK(result);

            TEST_INFO("prepared insert oid=4");
            id.id=4;
            id.initiator="initiator"; 
            text = "hello4";
            seq_oid = oidcodec->encode(4);
            seq_data = datacodec->encode(text);
            TEST_SUCCEED();
            records[0].oid_seq = seq_oid;
            records[0].change_data.data_seq(seq_data);
            records[0].change_data._d(CdmwFT::Transaction::DS_INSERTION);
            change[0].records = records;

			TEST_INFO("Realise a prepared_insert");
            result = cohort->prepare(change, id, sender.out());  
            TEST_CHECK(result);

            TEST_INFO("prepared insert oid=5");
            id.id=5;
            id.initiator="initiator"; 
            text = "hello5";
            seq_oid = oidcodec->encode(5);
            seq_data = datacodec->encode(text);
            TEST_SUCCEED();
            records[0].oid_seq = seq_oid;
            records[0].change_data.data_seq(seq_data);
            records[0].change_data._d(CdmwFT::Transaction::DS_INSERTION);
            change[0].records = records;

			TEST_INFO("Realise a prepared_insert");
            result = cohort->prepare(change, id, sender.out());  
            TEST_CHECK(result);

            TEST_INFO("Try to check if the OID is not present on the data store");
            try
            {
                myDataStore->select(2);
                TEST_FAILED();
            }
            catch(Cdmw::FT::NotFoundException)
            {
                TEST_SUCCEED();
            }

            TEST_INFO("realise a commit on oid=3");
            id.id=3;
            cohort->commit(id, sender.out());  
            TEST_SUCCEED();

            TEST_INFO("Try to check if the OID is not present on the data store");
            try
            {
                myDataStore->select(3);
                TEST_SUCCEED();
            }
            catch(Cdmw::FT::NotFoundException)
            {
                TEST_FAILED();
            }

            TEST_INFO("realise a commit on oid=4");
            id.id=4;
            cohort->commit(id, sender.out());  
            TEST_SUCCEED();

            TEST_INFO("Try to check if the OID is not present on the data store");
            try
            {
                myDataStore->select(4);
                TEST_SUCCEED();
            }
            catch(Cdmw::FT::NotFoundException)
            {
                TEST_FAILED();
            }

			TEST_INFO("realise an aborted on oid=5");
            id.id=5;
            TEST_INFO("Try to check if the OID is not present on the data store");
            try
            {
                myDataStore->select(5);
				TEST_FAILED();
            }
            catch(Cdmw::FT::NotFoundException)
            {
                TEST_SUCCEED();
            }

            cohort->rollback(id, sender.out());  
            TEST_SUCCEED();

            TEST_INFO("Try to check if the OID is not present on the data store");
            try
            {
                myDataStore->select(5);
				TEST_FAILED();
            }
            catch(Cdmw::FT::NotFoundException)
            {
                TEST_SUCCEED();
            }



			// transaction status
			// oid 1 => committed
			// oid 2 => prepared
			// oid 3 => committed
			// oid 4 => committed
			// oid 5 => aborted
			id.id = 1;
            CdmwFT::Transaction::TransactionStatus status;
			status = cohort->get_transaction_status(id, sender.out());
			TEST_CHECK(status == CdmwFT::Transaction::COMMITTED);

			id.id = 2;
			status = cohort->get_transaction_status(id, sender.out());
			TEST_CHECK(status == CdmwFT::Transaction::PREPARED);

			id.id = 3;
			status = cohort->get_transaction_status(id, sender.out());
			TEST_CHECK(status == CdmwFT::Transaction::COMMITTED);

			id.id = 4;
			status = cohort->get_transaction_status(id, sender.out());
			TEST_CHECK(status == CdmwFT::Transaction::COMMITTED);

			id.id = 5;
			status = cohort->get_transaction_status(id, sender.out());
			TEST_CHECK(status == CdmwFT::Transaction::ABORTED);

			id.id = 6;
			status = cohort->get_transaction_status(id, sender.out());
			TEST_CHECK(status == CdmwFT::Transaction::UNKNOWN);

/* Not in IIOP : in the state manager the only moment where coordinator
   set the transaction status of cohort is when cohort get the transaction status.
   In IIOP this is done via the return value of get_transaction_status.
			id.id = 2;
            CORBA::String_var name = CORBA::string_dup ("sender");
			cohort->set_transaction_status(CdmwFT::Transaction::COMMITTED, name.inout(), id);
			Cdmw::OsSupport::OS::sleep( timescale * 1000 );

			status = cohort->get_transaction_status(id, sender.out());
			TEST_CHECK(status == CdmwFT::Transaction::COMMITTED);
*/

			TEST_INFO("update a data");
			id.id = 7;
			text = "hello4";
            seq_data = datacodec->encode(text);
            seq_oid = oidcodec->encode(4);
            records[0].oid_seq = seq_oid;
            records[0].change_data.data_seq(seq_data);
            records[0].change_data._d(CdmwFT::Transaction::DS_UPDATE);
            change[0].records = records;
            // update
			result = cohort->prepare(change, id, sender.out());
			TEST_CHECK(result);

            // update
			id.id = 8;
			result = cohort->prepare(change, id, sender.out());
			TEST_CHECK(!result);

            // insert
 			id.id = 9;
            records[0].change_data._d(CdmwFT::Transaction::DS_INSERTION);
            change[0].records = records;
			result = cohort->prepare(change, id, sender.out());
			TEST_CHECK(!result);

            // delete
            TEST_INFO("DELETE");
			id.id = 10;
            records[0].oid_seq = seq_oid;
            records[0].change_data;
            records[0].change_data._d(CdmwFT::Transaction::DS_REMOVAL);
            change[0].records = records;
			result = cohort->prepare(change, id, sender.out());
			TEST_CHECK(!result);

            // delete
			id.id = 11;
			result = cohort->prepare(change, id, sender.out());
			TEST_CHECK(!result);

            // delete
			id.id = 12;
            seq_oid = oidcodec->encode(1);
            records[0].oid_seq = seq_oid;
            records[0].change_data;
            records[0].change_data._d(CdmwFT::Transaction::DS_REMOVAL);
            change[0].records = records;
			result = cohort->prepare(change, id, sender.out());
			TEST_CHECK(result);

			cohort->commit(id, sender.out());
			TEST_SUCCEED();

			id.id = 7;
			cohort->commit(id, sender.out());
			TEST_SUCCEED();

			TEST_INFO("Try to check if the OID is not present on the data store");
            try
            {
                myDataStore->select(1);
				TEST_FAILED();
            }
            catch(Cdmw::FT::NotFoundException)
            {
                 TEST_SUCCEED();
            }

			TEST_INFO("realise " << MAX_TRANSACTION_IN_PROGRESS*1.1 << " prepared insert");
			text = "hello";
            seq_data = datacodec->encode(text);


			for (int i = 20; i<20+MAX_TRANSACTION_IN_PROGRESS*1.1; i++)
			{
				id.id = i;
				seq_oid = oidcodec->encode(i);
                // insert
                records[0].oid_seq = seq_oid;
                records[0].change_data.data_seq(seq_data);
                records[0].change_data._d(CdmwFT::Transaction::DS_INSERTION);
                change[0].records = records;
				cohort->prepare(change, id, sender.out());
			}
			Cdmw::OsSupport::OS::sleep( timescale * 3000 );
			TEST_INFO("check if the cohort support " << MAX_TRANSACTION_IN_PROGRESS*1.1 << " transactions while the max is " << MAX_TRANSACTION_IN_PROGRESS);
			id.id = 221;
			seq_oid = oidcodec->encode(221);
            // insert 
            records[0].oid_seq = seq_oid;
            records[0].change_data.data_seq(seq_data);
            records[0].change_data._d(CdmwFT::Transaction::DS_INSERTION);
            change[0].records = records;
			result = cohort->prepare(change, id, sender.out());
			Cdmw::OsSupport::OS::sleep( timescale * 500 );
			TEST_CHECK(!result);

			for (int i = 20; i<20+MAX_TRANSACTION_IN_PROGRESS*1.1; i++)
			{
				id.id = i;
				cohort->commit(id, sender.out());
			}
			Cdmw::OsSupport::OS::sleep( timescale * 3000 );

			TEST_INFO("check if the status for the first request is now unknown by the cohort");
			id.id = 1;
			status = cohort->get_transaction_status(id, sender.out());
			TEST_CHECK(status == CdmwFT::Transaction::UNKNOWN);

         }

    } 
    catch (CORBA::SystemException & ex)
    {
        std::cout<<"exception :"<<ex<<std::endl;
    }
    
    catch (...)
    {
        std::ostringstream info_stream1;
        info_stream1 << "Test State Manager Exception caught : " << std::endl;
        TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }
   
}

