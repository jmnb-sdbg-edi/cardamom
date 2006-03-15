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
#include <set>

#include "teststorageinterface/TestStateTransfer.hpp"
#include "ftstatemanager/DataStoreGroup_impl.hpp"

using namespace std;

namespace Cdmw {
namespace FT {

typedef std::set<int> OidSet;

struct Dump: public std::unary_function<int, void>
{
      void operator()(const TestDataStore::DataMap::value_type& elt)
      {
          std::cout << "key: " << elt.first << " value: " << elt.second << std::endl;
      }
};

TestStateTransfer::TestStateTransfer (CORBA::ORB_ptr orb, PortableServer::POA_ptr rootPOA)
	:Cdmw::TestUtils::Testable("Test State manager")
{
    m_orb = CORBA::ORB::_duplicate(orb);
    m_rootPOA = PortableServer::POA::_duplicate(rootPOA);
}

TestStateTransfer::~TestStateTransfer ()
{
}
	
void 
TestStateTransfer::do_tests()
{
    set_nbOfRequestedTestOK(17);

    try
    {
        int INT_STRING_HOME1 = 1;
//         int INT_STRING_HOME2 = 2;
//         int INT_STRING_HOME3 = 3;
//         int INT_STRING_HOME4 = 4;
//         int INT_STRING_HOME5 = 5;
        const int MAX_TRANSACTION_DONE = 100;
        const int MAX_TRANSACTION_IN_PROGRESS = 100;

        TEST_INFO("Create empty DataStores");
        Cdmw::OsSupport::OS::Timeval cohort_timeout;
        cohort_timeout.seconds=2;
        cohort_timeout.microseconds=0;
        Cdmw::OsSupport::OS::Timeval coordinator_timeout;
        coordinator_timeout.seconds=2;
        coordinator_timeout.microseconds=0;

        std::cerr << __FILE__ << ": " << __LINE__ << std::endl;
        Cdmw::FT::DataStoreContext context1(m_orb.in(), m_rootPOA.in(), 
                                            MAX_TRANSACTION_IN_PROGRESS,
                                            MAX_TRANSACTION_DONE, 
                                            cohort_timeout,
                                            coordinator_timeout,
                                            1);
        std::cerr << __FILE__ << ": " << __LINE__ << std::endl;

        Cdmw::FT::DataStoreContext context2(m_orb.in(), m_rootPOA.in(), 
                                            MAX_TRANSACTION_IN_PROGRESS,
                                            MAX_TRANSACTION_DONE, 
                                            cohort_timeout,
                                            coordinator_timeout,
                                            1);
        std::cerr << __FILE__ << ": " << __LINE__ << std::endl;

        Cdmw::FT::DataStoreContext context3(m_orb.in(), m_rootPOA.in(), 
                                            MAX_TRANSACTION_IN_PROGRESS,
                                            MAX_TRANSACTION_DONE, 
                                            cohort_timeout,
                                            coordinator_timeout,
                                            1);
        std::cerr << __FILE__ << ": " << __LINE__ << std::endl;

        Cdmw::FT::DataStoreContext context4(m_orb.in(), m_rootPOA.in(), 
                                            MAX_TRANSACTION_IN_PROGRESS,
                                            MAX_TRANSACTION_DONE, 
                                            cohort_timeout,
                                            coordinator_timeout,
                                            1);
        std::cerr << __FILE__ << ": " << __LINE__ << std::endl;

        Cdmw::FT::DataStoreContext context5(m_orb.in(), m_rootPOA.in(), 
                                            MAX_TRANSACTION_IN_PROGRESS,
                                            MAX_TRANSACTION_DONE, 
                                            cohort_timeout,
                                            coordinator_timeout,
                                            1);
        std::cerr << __FILE__ << ": " << __LINE__ << std::endl;

        TestDataStore myDataStore1(INT_STRING_HOME1, m_orb.in(),
                                   m_rootPOA.in(), context1);
        // Hack! prevent exception due to reuse of DataStore id
        Cdmw::CommonSvcs::DataStore::DataStoreRegistry::
            Unregister_data_store(INT_STRING_HOME1);
        std::cerr << __FILE__ << ": " << __LINE__ << std::endl;
        TestDataStore myDataStore2(INT_STRING_HOME1, m_orb.in(),
                                   m_rootPOA.in(), context2);
        Cdmw::CommonSvcs::DataStore::DataStoreRegistry::
            Unregister_data_store(INT_STRING_HOME1);
        std::cerr << __FILE__ << ": " << __LINE__ << std::endl;
        TestDataStore myDataStore3(INT_STRING_HOME1, m_orb.in(),
                                   m_rootPOA.in(), context3);
        Cdmw::CommonSvcs::DataStore::DataStoreRegistry::
            Unregister_data_store(INT_STRING_HOME1);
        std::cerr << __FILE__ << ": " << __LINE__ << std::endl;
        TestDataStore myDataStore4(INT_STRING_HOME1, m_orb.in(),
                                   m_rootPOA.in(), context4);
        Cdmw::CommonSvcs::DataStore::DataStoreRegistry::
            Unregister_data_store(INT_STRING_HOME1);
        std::cerr << __FILE__ << ": " << __LINE__ << std::endl;
        TestDataStore myDataStore5(INT_STRING_HOME1, m_orb.in(),
                                   m_rootPOA.in(), context5);
        Cdmw::CommonSvcs::DataStore::DataStoreRegistry::
            Unregister_data_store(INT_STRING_HOME1);
        TEST_SUCCEED();

        TEST_INFO("Create CORBA DataStore interface");
        CdmwFT::StateTransfer::DataStoreGroup_var dsgroup
            = (new Cdmw::FT::DataStoreGroup_impl(m_rootPOA.in(), context1))->_this();
        TEST_SUCCEED();

        CdmwFT::StateTransfer::DataStoreIDs ids;
        ids.length(1);
        ids[0] = INT_STRING_HOME1;

        TEST_INFO("Try to copy an inactive DataStore");
        try
        {
            context2.update_with_coordinator(dsgroup.in(), "here1", ids);
            TEST_FAILED();
        }
        catch(CdmwFT::StateTransfer::InvalidDataStoreState&)
        {
            TEST_SUCCEED();
        }

        TEST_INFO("Activate a DataStore");
        myDataStore1.activate();
        TEST_SUCCEED();

        TEST_INFO("Copy an empty DataStore");
        context2.update_with_coordinator(dsgroup.in(), "here1", ids);
        TEST_SUCCEED();

        TEST_INFO("Test size of destination DataStore");
        TEST_CHECK(myDataStore2.size() == 0);

        TEST_INFO("Add data to source DataStore");
        myDataStore1.insert(1, "one");
        myDataStore1.insert(2, "two");
        myDataStore1.insert(3, "three");
        myDataStore1.insert(4, "four");
        myDataStore1.insert(5, "five");
        TEST_SUCCEED();

        TEST_INFO("Copy from one DataStore to another");
        context3.update_with_coordinator(dsgroup.in(), "here2", ids);
        TEST_SUCCEED();

        TEST_INFO("Test size of destination DataStore");
        TEST_CHECK(myDataStore3.size() == 5);
        TEST_INFO("Test content of destination DataStore");
        TEST_CHECK(equivalent(myDataStore1, myDataStore3));
      
        TEST_INFO("Test transfer of DataStore during inserts");
        // Increase initial DataStore size
        for (int i = 10; i < 5000; i++)
        {
            if ((i % 1000) == 0)
            {
                cout << "adding data " << i << endl;
            }

            myDataStore1.insert(i, "something");
        }

        Inserter inserter(myDataStore1);
        inserter.start();
        Cdmw::OsSupport::OS::sleep(100);

        context4.update_with_coordinator(dsgroup.in(), "here3", ids);
        cout << "update_with_coordinator done" << endl << flush;
        inserter.join();
        TEST_SUCCEED();

        TEST_INFO("Test size of destination DataStore");
        cout << "myDataStore1 size is " << myDataStore1.size() << endl;
        cout << "myDataStore4 size is " << myDataStore4.size() << endl;
        TEST_CHECK(myDataStore4.size() == 5495);

        //myDataStore4.for_each(Dump());

        TEST_INFO("Test content of destination DataStore");
        TEST_CHECK(equivalent(myDataStore1, myDataStore4));

        TEST_INFO("Test transfer of DataStore during updates");

        // Add observer for ordering info
        Observer observer;
        myDataStore5.register_observer(&observer);

        Modifier modifier(myDataStore1);
        modifier.start();
        Cdmw::OsSupport::OS::sleep(100);

        context5.update_with_coordinator(dsgroup.in(), "here4", ids);
        cout << "update_with_coordinator done" << endl << flush;
        modifier.join();
        TEST_SUCCEED();

        TEST_INFO("Test size of destination DataStore");
        cout << "size is " << myDataStore5.size() << endl;
        TEST_CHECK(myDataStore5.size() == 5495);

        TEST_INFO("Test content of destination DataStore");
        TEST_CHECK(equivalent(myDataStore1, myDataStore5));

        TEST_INFO("Test order of transfer of DataStore during updates");
        TEST_CHECK(observer.validate());
    }
    catch (const CORBA::UserException& ex)
    {
	std::ostringstream info_stream1;
	info_stream1 << "Test State Manager Exception caught : " << ex
                     << std::endl;
	TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& ex)
    {
	std::ostringstream info_stream1;
	info_stream1 << "Test State Manager Exception caught : " << ex
                     << std::endl;
	TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }
    catch (...)
    {
	std::ostringstream info_stream1;
	info_stream1 << "Test State Manager Exception caught : " << std::endl;
	TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }
}

struct CollectOids: public std::unary_function<int, void>
{
      CollectOids(OidSet& oid_set) : m_set(oid_set) {}

      void operator()(const TestDataStore::DataMap::value_type& elt)
      {
         m_set.insert(elt.first);
      }

      OidSet& m_set;
};


bool TestStateTransfer::equivalent(TestDataStore& ds1, TestDataStore& ds2)
{
   bool result;

   result = ds1.size() == ds2.size();

   if (result)
   {
      OidSet set1, set2;

      ds1.for_each(CollectOids(set1));
      ds2.for_each(CollectOids(set2));

      result = set1 == set2;

      if (result)
      {
         int count = 0;

         for (OidSet::iterator iter = set1.begin(); iter != set1.end(); ++iter)
         {
            if ((count++ % 1000) == 0)
            {
               cout << "equivalent count is " << count << endl;
            }

            result = ds1.select(*iter) == ds2.select(*iter);

            if (!result)
            {
               break;
            }
         }

         cout << "equivalent final count is " << count << endl;
      }
   }

   return result;
}

Inserter::Inserter(TestDataStore& data_store)
    throw (OutOfMemoryException,
           InternalErrorException,
           BadParameterException)
   : m_data_store(data_store)
{
}

void Inserter::run() throw()
{
    for (int i = 0; i < 500; i++)
    {
        m_data_store.insert(i + 20000, "hello world");
        this->sleep(50);
    }
}


Modifier::Modifier(TestDataStore& data_store)
    throw (OutOfMemoryException,
           InternalErrorException,
           BadParameterException)
   : m_data_store(data_store)
{
}

void Modifier::run() throw()
{
    for (int i = 0; i < 500; i++)
    {
        m_data_store.update((i % 5) + 1, "updated");
        //this->sleep(50);
    }
}

Observer::Observer()
   : m_inserts(0),
     m_updates(0),
     m_removes(0),
     m_failure(false)
{
}

void Observer::on_insert(_Oid_in_type oid, _Data_in_type data) throw ()
{
   m_inserts++;
   m_failure = m_updates > 0;

   if (m_failure)
   {
      cout << "Error: insert came after updates" << endl;
   }

   Cdmw::OsSupport::OS::sleep(1);
}

void Observer::on_update(_Oid_in_type oid, _Data_in_type data) throw ()
{
   m_updates++;
   m_update_oids.push_front(oid);
   Cdmw::OsSupport::OS::sleep(1);
}

void Observer::on_remove(_Oid_in_type oid) throw ()
{
   m_removes++;
}

bool Observer::validate()
{
   bool result;

   result = !m_failure &&
      (m_inserts == 5495) && (m_removes == 0) && (m_updates == 5);

   if (!result)
   {
      cout << "counts are inserts: " << m_inserts << " removes: " << m_removes
           << " updates: " << m_updates << endl;
   }
   else
   {
      cout << "counts are inserts: " << m_inserts << " removes: " << m_removes
           << " updates: " << m_updates << endl;
   }

   return result;
}

}; // namespace FT
}; // namespace Cdmw
