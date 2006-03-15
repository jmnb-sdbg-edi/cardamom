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

#include "testtxdatastore/TestTXDataStore.hpp"
#include "ftstatemanager/TXManager.hpp"
#include "ftstatemanager/TXDataStore.hpp"

namespace Cdmw {
namespace FT {

TestTXDataStore::TestTXDataStore (
    CORBA::ORB_ptr orb,
    PortableServer::POA_ptr rootPOA,
    const std::string& name)
    : Testable( name )
{
    m_orb = CORBA::ORB::_duplicate(orb);
    m_rootPOA = PortableServer::POA::_duplicate(rootPOA);
}

TestTXDataStore::~TestTXDataStore ()
{
}
	
void 
TestTXDataStore::do_tests()
{
    set_nbOfRequestedTestOK(25);

    try
    {
      const int INT_STRING_HOME1 = 1;
      const int MAX_TRANSACTION_DONE = 100;
      const int MAX_TRANSACTION_IN_PROGRESS = 100;

      TEST_INFO("Create DataStoreContext");
      Cdmw::OsSupport::OS::Timeval cohort_timeout;
      cohort_timeout.seconds=2;
      cohort_timeout.microseconds=0;
      Cdmw::OsSupport::OS::Timeval coordinator_timeout;
      coordinator_timeout.seconds=2;
      coordinator_timeout.microseconds=0;

      Cdmw::FT::DataStoreContext context(m_orb.in(), m_rootPOA.in(), 
                                         MAX_TRANSACTION_IN_PROGRESS,
                                         MAX_TRANSACTION_DONE, 
                                         cohort_timeout, coordinator_timeout,
                                         1);
      context.activate();
      TEST_SUCCEED();

      typedef Cdmw::FT::TXDataStore<int, std::string> TestDataStore;

      TEST_INFO("Create a TXManager");
      TXManager::TXManager manager(context);
      TEST_SUCCEED();

      TEST_INFO("Create empty TXDataStore");
      TestDataStore myDataStore1(manager, INT_STRING_HOME1, m_orb.in(),
                                 m_rootPOA.in());
      context.activate();
      TEST_SUCCEED();

      TEST_INFO("size of TX DataStore no transaction");
      TEST_CHECK(myDataStore1.size() == 0);

      TEST_INFO("begin transaction");
      manager.begin();
      TEST_SUCCEED();

      TEST_INFO("commit empty transaction");
      manager.commit();
      TEST_SUCCEED();

      TEST_INFO("Test size of TX DataStore");
      TEST_CHECK(myDataStore1.size() == 0);

      TEST_INFO("begin transaction");
      manager.begin();
      TEST_SUCCEED();

      TEST_INFO("rollback empty transaction");
      manager.rollback();
      TEST_SUCCEED();

      TEST_INFO("Test size of TX DataStore");
      TEST_CHECK(myDataStore1.size() == 0);

      TEST_INFO("begin transaction");
      manager.begin();
      TEST_SUCCEED();

      TEST_INFO("Add data to TX DataStore");
      myDataStore1.insert(1, "one");
      myDataStore1.insert(2, "two");
      myDataStore1.insert(3, "three");
      myDataStore1.insert(4, "four");
      myDataStore1.insert(5, "five");
      TEST_SUCCEED();

      TEST_INFO("Test size of TX DataStore");
      TEST_CHECK(myDataStore1.size() == 5);

      TEST_INFO("Test size of underlying DataStore");
      TEST_CHECK(myDataStore1.get_datastore().size() == 0);

      TEST_INFO("commit transaction");
      manager.commit();
      TEST_SUCCEED();

      TEST_INFO("Test size of underlying DataStore");
      TEST_CHECK(myDataStore1.get_datastore().size() == 5);

      TEST_INFO("begin transaction");
      manager.begin();
      TEST_SUCCEED();

      TEST_INFO("Add data to TX DataStore");
      myDataStore1.insert(11, "one");
      myDataStore1.insert(22, "two");
      myDataStore1.insert(33, "three");
      myDataStore1.insert(44, "four");
      myDataStore1.insert(55, "five");
      TEST_SUCCEED();

      TEST_INFO("Test size of TX DataStore");
      TEST_CHECK(myDataStore1.size() == 10);

      TEST_INFO("Test size of underlying DataStore");
      TEST_CHECK(myDataStore1.get_datastore().size() == 5);

      TEST_INFO("rollback transaction");
      manager.rollback();
      TEST_SUCCEED();

      TEST_INFO("Test size of underlying DataStore");
      TEST_CHECK(myDataStore1.get_datastore().size() == 5);

      TEST_INFO("begin transaction");
      manager.begin();
      TEST_SUCCEED();

      try
      {
          manager.begin();
          TEST_FAILED();
      }
      catch (const InTransactionException&)
      {
          TEST_SUCCEED();
      }

      TEST_INFO("rollback transaction");
      manager.rollback();
      TEST_SUCCEED();
    }
    catch (const InTransactionException& ex)
    {
	std::ostringstream info_stream1;
	info_stream1 << "Test TXDataStore Exception caught InTransactionException: " << ex
                     << std::endl;
	TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }
    catch (const NoTransactionException& ex)
    {
	std::ostringstream info_stream1;
	info_stream1 << "Test TXDataStore Exception caught NoTransactionException: " << ex
                     << std::endl;
	TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }
    catch (const CORBA::UserException& ex)
    {
	std::ostringstream info_stream1;
	info_stream1 << "Test TXDataStore Exception caught : " << ex
                     << std::endl;
	TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& ex)
    {
	std::ostringstream info_stream1;
	info_stream1 << "Test TXDataStore Exception caught : " << ex
                     << std::endl;
	TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }
    catch (...)
    {
	std::ostringstream info_stream1;
	info_stream1 << "Test TXDataStore Exception caught : " << std::endl;
	TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }
}

}; // namespace FT
}; // namespace Cdmw
