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

#include "testdatastore/TestDataStore.hpp"
#include <Foundation/commonsvcs/datastore/StorageHome.hpp>
#include <Foundation/commonsvcs/datastore/StorageObject.hpp>
#include <Foundation/commonsvcs/datastore/StdMapDataStoreFactory.hpp>


TestDataStore::TestDataStore ()
	:Cdmw::TestUtils::Testable("Test StdMapDataStore")
{
}

TestDataStore::~TestDataStore ()
{
}
	
void DataStoreDumper::operator()(const std::pair<int, std::string>&elt)
{
    m_os << elt.first << ":" << elt.second << std::endl;
}

bool key_and_stringSize_odd(const std::pair<int, std::string>&elt)
{
  bool result = false;
  if ( ((elt.first % 2)==1) && ((elt.second.size() % 2)==1) ) result = true;
  return result;
}

void 
TestDataStore::do_tests()
{
    set_nbOfRequestedTestOK(22);

    try {
      typedef Cdmw::CommonSvcs::DataStore::StorageHome<int, std::string> TestStorageHome;
      typedef Cdmw::CommonSvcs::DataStore::StorageObject<int, std::string> TestStorageObject;
      //typedef Cdmw::CommonSvcs::DataStore::DataStoreInterface<int, std::string> TestDataStore;
      typedef Cdmw::CommonSvcs::DataStore::StdMapDataStoreFactory<int, std::string> TestDataStoreFactory;

      int INT_STRING_HOME = 1;

      TEST_INFO("Create a DataStore");

      TestDataStoreFactory::Create_data_store(INT_STRING_HOME); 
      TEST_SUCCEED();

      TEST_INFO("Instanciate StorageHome and get/link DataStore with given Id");
      TestStorageHome myStorageHome(INT_STRING_HOME);
      TEST_SUCCEED();

      TEST_INFO("Test size of Data Store");
      TEST_CHECK(myStorageHome.size()==0);

      TEST_INFO("Create a Storage Object");
      TestStorageObject myStorageObject1 = myStorageHome.create(1, "un");
      TEST_SUCCEED();
      
      TEST_INFO("Try to create Storage Object with same Oid");
      try
      {
        TestStorageObject myStorageObject2 = myStorageHome.create(1, "deux");
        TEST_FAILED();
      }
      catch(Cdmw::CommonSvcs::DataStore::AlreadyExistException)
      {
        TEST_SUCCEED();
      }

      TEST_INFO("Create a second Storage Object");
      TestStorageObject myStorageObject2 = myStorageHome.create(2, "deux");
      TEST_SUCCEED();
      
      TEST_INFO("Verify that Storage Object are stored");
      TEST_INFO(" -via StorageHome Data find_data_by_oid(1)");
      TEST_INFO(myStorageHome.find_data_by_oid(1));
      TEST_INFO(" -via StorageHome StorageObject find_by_oid(2)");
      TestStorageObject myStorageObject2Cpy = myStorageHome.find_by_oid(2);
      TEST_INFO(myStorageObject2Cpy.get());
      TEST_SUCCEED();
      
      TEST_INFO(" -via StorageHome size");
      TEST_CHECK(myStorageHome.size()==2);
      
      TEST_INFO(" -via StorageObject 1 get_oid&get");
      TEST_INFO(myStorageObject1.get_oid());
      TEST_INFO(myStorageObject1.get());
      TEST_SUCCEED();
      
      TEST_INFO(" -via StorageObject 2 get_oid&Data()");
      TEST_INFO(myStorageObject2.get_oid());
      TEST_INFO(std::string(myStorageObject2));
      TEST_SUCCEED();
      
      TEST_INFO("Create a third Storage Object");
      TestStorageObject myStorageObject3 = myStorageHome.create(3, "TROIS");
      TEST_SUCCEED();

      TEST_INFO("Set new data for the third Storage Object");
      myStorageObject3.set("TrOiS");
      TEST_SUCCEED();
      
      TEST_INFO("Assign new data for the third Storage Object");
      myStorageObject3="trois";
      TEST_SUCCEED();
      
      TEST_INFO("Test size of Data Store");
      TEST_CHECK(myStorageHome.size()==3);

      TEST_INFO("Test for_each : dump data store");
      myStorageHome.for_each(DataStoreDumper(std::cout));
      TEST_SUCCEED();

      TEST_INFO("Test find_if :");
      TEST_INFO(" get elements with odd key and odd string size of the value");
      std::list<TestStorageObject> resultStObjects;
      myStorageHome.find_if(key_and_stringSize_odd,
                            std::back_inserter(resultStObjects));
      for (std::list<TestStorageObject>::iterator itResult = resultStObjects.begin();
           itResult != resultStObjects.end();
	   itResult++)
      {
        std::cout << (*itResult).get_oid() << ":" << (*itResult).get() << std::endl;
      }
      TEST_CHECK(resultStObjects.size()==1);

      TEST_INFO("Try to remove inexistant entry");
      try
      {
        myStorageHome.remove(4);
        TEST_FAILED();
      }
      catch(Cdmw::CommonSvcs::DataStore::NotFoundException)
      {
        TEST_SUCCEED();
      }

      TEST_INFO("Remove a entry via StorageHome");
      myStorageHome.remove(3);
      TEST_CHECK(myStorageHome.size()==2);

      TEST_INFO("Remove a entry via StorageObject");
      myStorageObject2.remove();
      TEST_CHECK(myStorageHome.size()==1);

      TEST_INFO("Try to access previous removed data");
      try
      {
	TEST_INFO(myStorageObject2.get());
        TEST_FAILED();
      }
      catch(Cdmw::CommonSvcs::DataStore::NotFoundException)
      {
        TEST_SUCCEED();
      }

      TEST_INFO("Try to find_data_by_oid(inexistant Oid)");
      try
      {
        std::string unknown = myStorageHome.find_data_by_oid(2);
        TEST_FAILED();
      }
      catch(Cdmw::CommonSvcs::DataStore::NotFoundException)
      {
        TEST_SUCCEED();
      }

      TEST_INFO("Try same via find_by_oid(inexistant Oid)");
      try
      {
        myStorageObject2 = myStorageHome.find_by_oid(2);
        TEST_FAILED();
      }
      catch(Cdmw::CommonSvcs::DataStore::NotFoundException)
      {
        TEST_SUCCEED();
      }

    } catch (...) {
	std::ostringstream info_stream1;
	info_stream1 << "Test State Manager Exception caught : " << std::endl;
	TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }
  
}

