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

#include "teststorageinterface/TestStateManager.hpp"
#include <Foundation/commonsvcs/datastore/StorageHome.hpp>
#include <Foundation/commonsvcs/datastore/StorageObject.hpp>
#include <FaultTolerance/ftstatemanager/DataStore.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreContext.hpp>


DEFINE_FIXED_LENGTH_DATA_TRAITS(CdmwFT::Test::TimeStamp);

bool std::less<CdmwFT::Test::TimeStamp_var>::operator() 
    (const CdmwFT::Test::TimeStamp_var& x, 
     const CdmwFT::Test::TimeStamp_var& y) const
{
    if (x->seconds < y->seconds)
    {
        return true;
    }
    else if (x->seconds == y->seconds)
    {
        if (x->microseconds < y->microseconds)
        {
            return true;
        }
        else if (x->microseconds == y->microseconds)
        {
            if (x->counter_inx < y->counter_inx)
            {
                return true;
            }
            else 
                return false;
        }
        else 
            return false;
    }
    else 
        return false;
};


TestStateManager::TestStateManager (CORBA::ORB_ptr orb, PortableServer::POA_ptr rootPOA)
    :Cdmw::TestUtils::Testable("Test State manager")
{
    m_orb = CORBA::ORB::_duplicate(orb);
    m_rootPOA = PortableServer::POA::_duplicate(rootPOA);
}

TestStateManager::~TestStateManager ()
{
}
    
void ComplexeDataStoreDumper::operator()(const std::pair<CdmwFT::Test::TimeStamp_var,
                                                         std::string>& elt)
{
    m_os << elt.first->seconds << "s " 
         << elt.first->microseconds << "us:"
         << elt.first->counter_inx  
         << " : " << elt.second << std::endl;
}

bool complexe_key_and_stringSize_odd(const std::pair<CdmwFT::Test::TimeStamp_var, 
                                                     std::string>& elt)
{
  bool result = false;
  if ( ((elt.first->counter_inx % 2)==1) && ((elt.second.size() % 2)==1) ) result = true;
  return result;
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
TestStateManager::do_tests()
{
    set_nbOfRequestedTestOK(39);

    try {
      typedef Cdmw::CommonSvcs::DataStore::StorageHome<int, std::string> 
              TestStorageHome;
      typedef Cdmw::CommonSvcs::DataStore::StorageObject<int, std::string> 
              TestStorageObject;
      typedef Cdmw::FT::DataStore<int, std::string> TestDataStore;


      typedef Cdmw::CommonSvcs::DataStore::StorageHome<CdmwFT::Test::TimeStamp, 
                                                       std::string> 
              TestComplexeStorageHome;
      typedef Cdmw::CommonSvcs::DataStore::StorageObject<CdmwFT::Test::TimeStamp, 
                                                         std::string> 
              TestComplexeStorageObject;
      typedef Cdmw::FT::DataStore<CdmwFT::Test::TimeStamp, std::string> 
              TestComplexeDataStore;


      int INT_STRING_HOME = 1;
      int TIMESTAMP_STRING_HOME = 2;
      const int MAX_TRANSACTION_DONE = 100;
      const int MAX_TRANSACTION_IN_PROGRESS = 100;

      TEST_INFO("Create a DataStore");
      Cdmw::OsSupport::OS::Timeval cohort_timeout;
      cohort_timeout.seconds=2;
      cohort_timeout.microseconds=0;
      Cdmw::OsSupport::OS::Timeval coordinator_timeout;
      coordinator_timeout.seconds=2;
      coordinator_timeout.microseconds=0;

      Cdmw::FT::DataStoreContext context(m_orb.in(), m_rootPOA.in(), 
                                         "5555", "ft_domain", "225.1.1.8", 
                                         MAX_TRANSACTION_IN_PROGRESS,
                                         MAX_TRANSACTION_DONE, 
                                         cohort_timeout, coordinator_timeout,
                                         1);

      TestDataStore myDataStore(INT_STRING_HOME, m_orb.in(), m_rootPOA.in(),
                                context);

      TEST_SUCCEED();


      TEST_INFO("Instanciate StorageHome and get/link DataStore with given Id");
      TestStorageHome myStorageHome(INT_STRING_HOME);
      TEST_SUCCEED();

      TEST_INFO("Create a DataStore with a complexe type oid");
      TestComplexeDataStore myComplexeDataStore(TIMESTAMP_STRING_HOME, 
                                                m_orb.in(), 
                                                m_rootPOA.in(), 
                                                context);
      TEST_SUCCEED();

      TEST_INFO("Instanciate StorageHome and get/link DataStore with given Id");
      TestComplexeStorageHome myComplexeStorageHome(TIMESTAMP_STRING_HOME);
      TEST_SUCCEED();

      TEST_INFO("Test size of Data Store");
      TEST_CHECK(myStorageHome.size()==0);

      TEST_INFO("Test size of Complexe Data Store");
      TEST_CHECK(myComplexeStorageHome.size()==0);

      TEST_INFO("Create a Storage Object");
      TestStorageObject myStorageObject1 = myStorageHome.create(1, "un");
      TEST_SUCCEED();

      TEST_INFO("Create a Storage Object with a complexe oid");
      CdmwFT::Test::TimeStamp ts_oid;
      ts_oid.seconds = 1000;
      ts_oid.microseconds = 1000;
      ts_oid.counter_inx = 1;

      TestComplexeStorageObject myComplexeStorageObject1 = 
         myComplexeStorageHome.create(ts_oid, "un");
      TEST_SUCCEED();
      
      TEST_INFO("Try to create Storage Object with same Oid");
      try
      {
        myStorageHome.create(1, "deux");
        TEST_FAILED();
      }
      catch(Cdmw::FT::AlreadyExistException)
      {
        TEST_SUCCEED();
      }

      TEST_INFO("Try to create Storage Object with same complexe Oid");
      try
      {
        TestComplexeStorageObject myComplexeStorageObject2 = 
           myComplexeStorageHome.create(ts_oid, "deux");
        TEST_FAILED();
      }
      catch(Cdmw::FT::AlreadyExistException)
      {
        TEST_SUCCEED();
      }


      TEST_INFO("Create a second Storage Object");
      TestStorageObject myStorageObject2 = myStorageHome.create(2, "deux");
      TEST_SUCCEED();

      TEST_INFO("Create a second Storage Object with a complexe oid");
      ts_oid.counter_inx = 2;
      TestComplexeStorageObject myComplexeStorageObject2 = 
         myComplexeStorageHome.create(ts_oid, "deux");
      TEST_SUCCEED();
      
      TEST_INFO("Verify that Storage Object are stored");
      TEST_INFO(" -via StorageHome Data find_data_by_oid(1)");
      TEST_INFO(myStorageHome.find_data_by_oid(1));
      TEST_INFO(" -via StorageHome StorageObject find_by_oid(2)");
      TestStorageObject myStorageObject2Cpy = myStorageHome.find_by_oid(2);
      TEST_INFO(myStorageObject2Cpy.get());
      TEST_SUCCEED();

      TEST_INFO("Verify that Storage Object are stored with complexe oid");
      TEST_INFO(" -via ComplexeStorageStorageHome Data find_data_by_oid(&ts_oid) ts_oid.counter_inx = 1");
      ts_oid.counter_inx = 1;
      TEST_INFO(myComplexeStorageHome.find_data_by_oid(ts_oid));
      TEST_INFO(" -via ComplexeStorageStorageHome StorageObject find_by_oid(&ts_oid) ts_oid.counter_inx = 2");
      ts_oid.counter_inx = 2;
      TestComplexeStorageObject myComplexeStorageObject2Cpy = 
         myComplexeStorageHome.find_by_oid(ts_oid);
      TEST_INFO(myComplexeStorageObject2Cpy.get());
      TEST_SUCCEED();
      
      TEST_INFO(" -via StorageHome size");
      TEST_CHECK(myStorageHome.size()==2);

      TEST_INFO(" -via ComplexeStorageHome size");
      TEST_CHECK(myComplexeStorageHome.size()==2);
      
      TEST_INFO(" -via StorageObject 1 get_oid&get");
      TEST_INFO(myStorageObject1.get_oid());
      TEST_INFO(myStorageObject1.get());
      TEST_SUCCEED();

      TEST_INFO(" -via StorageObject 2 get_oid&Data()");
      TEST_INFO(myStorageObject2.get_oid());
      TEST_INFO(std::string(myStorageObject2));
      TEST_SUCCEED();

      
      TEST_INFO(" -via ComplexeStorageObject 2 get_oid&Data()");
      CdmwFT::Test::TimeStamp_var r_oid = myComplexeStorageObject2.get_oid();
      TEST_INFO(r_oid->counter_inx);
      TEST_INFO(std::string(myComplexeStorageObject2));
      TEST_SUCCEED();

      TEST_INFO(" -via ComplexeStorageObject 1 get_oid&get");
      r_oid = myComplexeStorageObject1.get_oid();
      TEST_INFO(r_oid->counter_inx);
      TEST_INFO(myComplexeStorageObject1.get());
      TEST_SUCCEED();
      
      TEST_INFO("Create a third Storage Object with a complexe oid");
      ts_oid.counter_inx = 3;
      TestComplexeStorageObject myComplexeStorageObject3 = 
         myComplexeStorageHome.create(ts_oid, "TROIS");
      TEST_SUCCEED();

      TEST_INFO("Set new data for the third Storage Object with a complexe oid");
      myComplexeStorageObject3.set("TrOiS");
      TEST_SUCCEED();
      
      TEST_INFO("Assign new data for the third Storage Object with a complexe oid");
      myComplexeStorageObject3="trois";
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

      TEST_INFO("Test for_each : dump complexe data store");
      myComplexeStorageHome.for_each(ComplexeDataStoreDumper(std::cout));
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
          std::cout << (*itResult).get_oid() << ":" << (*itResult).get() 
                    << std::endl;
      }
      TEST_CHECK(resultStObjects.size()==1);

      TEST_INFO("Test find_if :");
      TEST_INFO(" get elements with complexe odd key and odd string size of the value");
      std::list<TestComplexeStorageObject> resultComplexeStObjects;
      myComplexeStorageHome.find_if(complexe_key_and_stringSize_odd,
                            std::back_inserter(resultComplexeStObjects));
      for (std::list<TestComplexeStorageObject>::iterator itResult = 
               resultComplexeStObjects.begin();
           itResult != resultComplexeStObjects.end();
           itResult++)
      {
         r_oid = (*itResult).get_oid();
         std::cout << r_oid->seconds << "s " 
                   << r_oid->microseconds << "us:"
                   << r_oid->counter_inx << ":" << (*itResult).get() << std::endl;
      }
      TEST_CHECK(resultComplexeStObjects.size()==1);


      TEST_INFO("Try to remove inexistant entry");
      try
      {
        myStorageHome.remove(4);
        TEST_FAILED();
      }
      catch(Cdmw::FT::NotFoundException)
      {
        TEST_SUCCEED();
      }

      TEST_INFO("Remove a entry via StorageHome");
      myStorageHome.remove(3);
      TEST_CHECK(myStorageHome.size()==2);

      TEST_INFO("Remove a entry via ComplexeStorageHome");
      ts_oid.counter_inx = 1;
      myComplexeStorageHome.remove(ts_oid);
      TEST_CHECK(myComplexeStorageHome.size()==2);

      TEST_INFO("Remove a entry via StorageObject");
      myStorageObject2.remove();
      TEST_CHECK(myStorageHome.size()==1);

      TEST_INFO("Remove a entry via ComplexeStorageObject");
      myComplexeStorageObject2.remove();
      TEST_CHECK(myComplexeStorageHome.size()==1);

      TEST_INFO("Try to access previous removed data");
      try
      {
         TEST_INFO(myStorageObject2.get());
        TEST_FAILED();
      }
      catch(Cdmw::FT::NotFoundException)
      {
        TEST_SUCCEED();
      }

      TEST_INFO("Try to find_data_by_oid(inexistant Oid)");
      try
      {
        myStorageHome.find_data_by_oid(2);
        TEST_FAILED();
      }
      catch(Cdmw::FT::NotFoundException)
      {
        TEST_SUCCEED();
      }

      TEST_INFO("Try same via find_by_oid(inexistant Oid)");
      try
      {
        myStorageObject2 = myStorageHome.find_by_oid(2);
        TEST_FAILED();
      }
      catch(Cdmw::FT::NotFoundException)
      {
        TEST_SUCCEED();
      }

    } catch (const CORBA::Exception& ex) {
        std::ostringstream info_stream1;
        info_stream1 << "Test State Manager Exception caught : " << ex << std::endl;
        TEST_INFO(info_stream1.str());    
        TEST_FAILED();
    } catch (const std::exception& ex) {
        std::ostringstream info_stream1;
        info_stream1 << "Test State Manager Exception caught : " << ex.what() << std::endl;
        TEST_INFO(info_stream1.str());    
        TEST_FAILED();
    } catch (...) {
        std::ostringstream info_stream1;
        info_stream1 << "Test State Manager Exception caught : " << std::endl;
        TEST_INFO(info_stream1.str());    
        TEST_FAILED();
    }
  
}

