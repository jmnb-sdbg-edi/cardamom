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

#include "datastoretest/TestDataStore.hpp"

namespace Cdmw
{
namespace FT 
{

bool                       TestDataStore::M_pred12WaitOk = false;
Cdmw::OsSupport::Mutex     TestDataStore::M_synchro12Mutex;
Cdmw::OsSupport::Condition TestDataStore::M_synchro12Condition(M_synchro12Mutex);
bool                       TestDataStore::M_pred21WaitOk = false;
Cdmw::OsSupport::Mutex     TestDataStore::M_synchro21Mutex;
Cdmw::OsSupport::Condition TestDataStore::M_synchro21Condition(M_synchro21Mutex);

bool                       TestDataStore::M_continueLoop = true;

const int TestDataStore::MAPSIZE;
Cdmw::OsSupport::OS::Timeval TestDataStore::M_beforeOp[MAPSIZE];
Cdmw::OsSupport::OS::Timeval TestDataStore::M_afterOp[MAPSIZE];

// constructor
TestDataStore::TestDataStore(DataPart1StorageHome* dataPart1StorageHome,
                             DataPart2StorageHome* dataPart2StorageHome,
                             short scenarioIndex,
                             bool realOverlap)
    throw()
    : Cdmw::TestUtils::Testable( "Datastore validation test" ),
      m_dataPart1StorageHome(dataPart1StorageHome),
      m_dataPart2StorageHome(dataPart2StorageHome),
      m_scenarioIndex(scenarioIndex),
      m_realOverlap(realOverlap)
{
}

// destructor
TestDataStore::~TestDataStore()
   throw()
{
}

TestDataStore::CheckResult TestDataStore::checkTimestamps(int inter1[], int inter2[])
{
    CheckResult res;
    res.firstInterval = 0;
    res.overlapIndice = -1; res.realOverlapIndice1 = -1; res.realOverlapIndice2 = -1;
    int firstInterval[2], secondInterval[2];
    
    // determine which interval has the oldest timestamp
    if ( M_beforeOp[inter1[0]] > M_beforeOp[inter2[0]] )
    {
        firstInterval[0] = inter2[0]; firstInterval[1] = inter2[1];
        secondInterval[0] = inter1[0]; secondInterval[1] = inter1[1];
        res.firstInterval = 2;
    }
    else
    {
        firstInterval[0] = inter1[0]; firstInterval[1] = inter1[1];
        secondInterval[0] = inter2[0]; secondInterval[1] = inter2[1];
        res.firstInterval = 1;
    }

    // determine if there is an overlap
    if ( M_afterOp[firstInterval[1]] > M_beforeOp[secondInterval[0]] )
    {
        // determine the overlap indice in the firstInterval
        for ( int i=firstInterval[0]; i<=firstInterval[1]; i++ )
        {
            if (M_beforeOp[i] > M_beforeOp[secondInterval[0]])
            {
                res.overlapIndice = i - 1;
                TEST_INFO("Overlap detected");
                std::cout << "Op done for oid:" << res.overlapIndice << " has time:" <<
                    M_beforeOp[res.overlapIndice].seconds << " " <<
                    M_beforeOp[res.overlapIndice].microseconds << std::endl;
                std::cout << "Op done for oid:" << secondInterval[0] << " has time:" <<
                    M_beforeOp[secondInterval[0]].seconds << " " <<
                    M_beforeOp[secondInterval[0]].microseconds << std::endl;
                std::cout << "Op done for oid:" << res.overlapIndice+1 << " has time:" <<
                    M_beforeOp[res.overlapIndice+1].seconds << " " <<
                    M_beforeOp[res.overlapIndice+1].microseconds << std::endl;
                break;
            }
        }

        if (res.overlapIndice!=-1)
        {
            // determine if an operation has not finished
            // while a second operation has began (more real parallelism)
            for ( int i=res.overlapIndice+1 ; i<=firstInterval[1]; i++ )
            {
                // search the first timestamp in the second interval > to the current
                int firstSup = 0;
                for ( int j=secondInterval[0]+1 ; j<=secondInterval[1] ; j++ )
                {
                    if ( M_beforeOp[j] > M_beforeOp[i] )
                    {
                        firstSup = j;
                        break;
                    }
                }
                // check if this time is < timestamp of the end of op in first interval
                if ((firstSup!=0)&&( M_beforeOp[firstSup] < M_afterOp[i] ))
                {
                    res.realOverlapIndice1 = i;
                    res.realOverlapIndice2 = firstSup;
                    std::cout << "real parrallelism of op has occured" << std::endl;
                    std::cout << "Op done for oid:" << res.realOverlapIndice1 << " has time_before:" <<
                        M_beforeOp[res.realOverlapIndice1].seconds << " " <<
                        M_beforeOp[res.realOverlapIndice1].microseconds << " and time_after:" <<
                        M_afterOp[res.realOverlapIndice1].seconds << " " <<
                        M_afterOp[res.realOverlapIndice1].microseconds << std::endl;
                    std::cout << "Op done for oid:" << res.realOverlapIndice2 << " has time:" <<
                        M_beforeOp[res.realOverlapIndice2].seconds << " " <<
                        M_beforeOp[res.realOverlapIndice2].microseconds << std::endl;                
                    break;
                }
            }
            if (res.realOverlapIndice1 == -1)
            {
              std::cout << "Overlap of the series of op but no real parrallelism" << std::endl;
            }
        }
    }

    // I choose to clear all the timestamps after each time i launch check on it
    for (int i=1; i<MAPSIZE; i++)
    {
        M_beforeOp[i].seconds = 0; M_beforeOp[i].microseconds = 0;
        M_afterOp[i].seconds = 0; M_afterOp[i].microseconds = 0;
    }

    //std::cout << res.firstInterval << " " << res.overlapIndice << " " << res.realOverlapIndice1 << " " << res.realOverlapIndice2 << std::endl;
    return res;
}

// do_tests
void TestDataStore::do_tests()
{
    // set number of requested successfull tests

	const int timescale = Cdmw::TestUtils::Testable::get_timescale();
    const int synchroWait = 1000+SIZESCALE*50;
    //const long nbIter = 500000;

    switch (m_scenarioIndex)
    {
       case 1:
          {
             set_nbOfRequestedTestOK(10);

             TEST_INFO("Check of the timestamp check");
             int inter1[2]={0,2};
             int inter2[2]={3,5};
             // Test no overlap at all
             for (int i=0; i<6; i++) 
             {
                 M_beforeOp[i]=Cdmw::OsSupport::OS::get_time();
                 Cdmw::OsSupport::OS::sleep(1);
             }
             CheckResult res = checkTimestamps(inter1, inter2);
             TEST_CHECK( ((res.firstInterval==1)&&(res.overlapIndice==-1)&&
                          (res.realOverlapIndice1==-1)&&(res.realOverlapIndice2==-1)) );

             // Test overlap
             for (int i=0; i<3; i++)
             {
               M_beforeOp[i]=Cdmw::OsSupport::OS::get_time();
               Cdmw::OsSupport::OS::sleep(1);
               M_beforeOp[i+3]=Cdmw::OsSupport::OS::get_time();
               Cdmw::OsSupport::OS::sleep(1);
             }            
             M_afterOp[2].seconds=M_beforeOp[2].seconds;
             M_afterOp[2].microseconds=M_beforeOp[2].microseconds+1;
             res = checkTimestamps(inter1, inter2);
             TEST_CHECK( ((res.firstInterval==1)&&(res.overlapIndice==0)&&
                          (res.realOverlapIndice1==-1)&&(res.realOverlapIndice2==-1)) );
             // Test of Op in parallel
             for (int i=0; i<3; i++)
             {
               M_beforeOp[i]=Cdmw::OsSupport::OS::get_time();
               Cdmw::OsSupport::OS::sleep(1);
               M_beforeOp[i+3]=Cdmw::OsSupport::OS::get_time();
               Cdmw::OsSupport::OS::sleep(1);
             }            
             M_afterOp[2].seconds=M_beforeOp[2].seconds;
             M_afterOp[2].microseconds=M_beforeOp[2].microseconds+1;
             M_afterOp[1].seconds=M_beforeOp[4].seconds;
             M_afterOp[1].microseconds=M_beforeOp[4].microseconds+1;
             res = checkTimestamps(inter1, inter2);
             TEST_CHECK( ((res.firstInterval==1)&&(res.overlapIndice==0)&&
                          (res.realOverlapIndice1==1)&&(res.realOverlapIndice2==4)) );
             // Clean
             for (int i=0; i<6; i++)
             {
                 M_beforeOp[i].seconds=0; M_beforeOp[i].microseconds=0; 
                 M_afterOp[i].seconds=0; M_beforeOp[i].microseconds=0; 
             }
/*
             TEST_INFO("THREAD-1 ---> Test Start: Waiting to synchronise with Thread 2");
             Cdmw::OsSupport::OS::sleep(timescale*5000);
             {
                {
                  CDMW_MUTEX_GUARD(M_synchro12Mutex);
                  M_pred12WaitOk = true;
                }
                M_synchro12Condition.broadcast();
             }

             TEST_INFO("THREAD-1: 0a) Test concurrent Read/Write of one data instance of the same datastore instance");
             try 
             {
                // First part: the Thread 1 create 1 entry in the datastore
                // No concurrency here: this part is done only by thread 1 to prepare
                // the test of concurent updates: Thread 2 is waiting on condition
                TEST_INFO("THREAD-1 ---> Creating data in datastore 1");
                Test::Ident id=0;
                Test::DataPart1 data;
                data.thread_name = CORBA::string_dup("Thread nb 1");
                data.index=0;
                DataPart1StorageObject obj=m_dataPart1StorageHome->create(id, data);

                TEST_INFO("THREAD-1 ---> Preliminary test: perform updates WITHOUT Thread 2");
                Test::DataPart1_var dataCheck;
                for (long i=0; i<nbIter; i++)
                {
                    dataCheck = obj.get();
                    data.index = dataCheck.in().index + 1;
                    obj.set(data);
                }
                std::cout << std::endl;
                dataCheck = obj.get();
                std::cout << "Thread 1 final value:" << dataCheck.in().index << std::endl;

                TEST_CHECK(dataCheck.in().index == nbIter);
                
                TEST_INFO("THREAD-1 ---> Now give hand to Thread 2 for preliminary test");
                {
                    {
                        CDMW_MUTEX_GUARD(M_synchro12Mutex);
                        M_pred12WaitOk = true;
                    }
                    M_synchro12Condition.broadcast();
                }
                Cdmw::OsSupport::OS::sleep(timescale*100);
                TEST_INFO("THREAD-1: wait for Thread 2 synchronisation signal");
                {
                    CDMW_MUTEX_GUARD(M_synchro21Mutex);
                    while (!M_pred21WaitOk) M_synchro21Condition.wait();
                    M_pred21WaitOk = false;
                }
   
                TEST_INFO("THREAD-1 ---> Test: perform updates WITH Thread 2");
                TEST_INFO("THREAD-1 ---> Updating datastore 1");
                // Begin of the concurrent update part on same datastore
                dataCheck = obj.get();
                long initialVal = dataCheck.in().index;
                for (long i=0; i<nbIter; i++)
                {
                    dataCheck = obj.get();
                    data.index = dataCheck.in().index + 1;
                    obj.set(data);
                }
                std::cout << std::endl;
                std::cout << "Thread 1 initial value:" << initialVal << std::endl;
                dataCheck = obj.get();
                std::cout << "Thread 1 final value:" << dataCheck.in().index << std::endl;

                TEST_CHECK(dataCheck.in().index != (initialVal+nbIter) );
                //Clean after this test
                obj.remove();
                Cdmw::OsSupport::OS::sleep(timescale*1000);
             }
             catch (const AlreadyExistException& )
             {
                std::cout << "AlreadyExistException raised!" << std::endl;
                TEST_FAILED();
             }
             catch (const CORBA::SystemException& e)
             {
                std::cerr << "CORBA::SystemException raised: " << e << std::endl;
                TEST_FAILED();
             }

             Cdmw::OsSupport::OS::sleep(timescale*5000);
             {
                {
                  CDMW_MUTEX_GUARD(M_synchro12Mutex);
                  M_pred12WaitOk = true;
                }
                M_synchro12Condition.broadcast();
             }
             Cdmw::OsSupport::OS::sleep(timescale*SIZESCALE);

             TEST_INFO("THREAD-1: 0b) Test concurrent Write of data instances of the same datastore instance");
             try 
             {
                // First part: the Thread 1 create entries in the datastore
                // No concurrency here: this part is done only by thread 1 to prepare
                // the test of concurent updates: Thread 2 is waiting on condition
                TEST_INFO("THREAD-1 ---> Creating data in datastore 1");
                Test::Ident id=0;
                Test::DataPart1 data;
                data.thread_name = CORBA::string_dup("Thread nb 1");
                data.index=0;
                DataPart1StorageObject obj=m_dataPart1StorageHome->create(id, data);
   
                for (int i=1; i<100*SIZESCALE; i++)
                {
                   id = i;
                   data.index = i;
      
                   obj = m_dataPart1StorageHome->create(id, data);
                   //std::cout << "c1";
                }
                bool testOk = false;
                for (int j=1; j<11; j++)
                {
                    std::cout << std::endl << "THREAD-1 try number:" << j << std::endl;
                    TEST_INFO("THREAD-1 ---> Waiting before allowing Thread-2 to update");
                    Cdmw::OsSupport::OS::sleep(timescale*synchroWait);
                    {
                        {
                            CDMW_MUTEX_GUARD(M_synchro12Mutex);
                            M_pred12WaitOk = true;
                        }
                        M_synchro12Condition.broadcast();
                    }
                    TEST_INFO("THREAD-1 ---> Updating datastore 1");
                    Cdmw::OsSupport::OS::sleep(timescale*SIZESCALE);
                    // Begin of the concurrent update part on same datastore
                    for (int i=0; i<100*SIZESCALE; i++)
                    {
                        id = i;
                        data.index = i + j;
                        obj = m_dataPart1StorageHome->find_by_oid(id);
                        obj.set(data);
                        //std::cout << "s1";
                    }
                    std::cout << std::endl;
                    TEST_INFO("THREAD-1: wait for Thread 2 synchronisation signal");
                    {
                        CDMW_MUTEX_GUARD(M_synchro21Mutex);
                        while (!M_pred21WaitOk) M_synchro21Condition.wait();
                        M_pred21WaitOk = false;
                    }
                    TEST_INFO("THREAD-1: checking concurent updates");
                    // third part: the Thread 1 verifies the datastore entries
                    // No concurrency here: this check part is done only by thread 1
                    // Thread 2 is waiting on condition for next time
                    Test::DataPart1_var dataCheck = m_dataPart1StorageHome->find_data_by_oid(0);
                    int val=0, lastVal=dataCheck.in().index; 
                    for (int i=1; i<100*SIZESCALE; i++)
                    {
                        id = i;
                        dataCheck = m_dataPart1StorageHome->find_data_by_oid(id);
                        val = dataCheck.in().index;
                        //std::cout << val << " ";
                        if (( val == i + j ) || ( val == i + j + 10))
                        {
                            if  ( (val == (lastVal + 11)) || (val == (lastVal - 9)) )
                            {
                                testOk=true;
                                cout << "Found lastVal:" << lastVal << " val:" << val << std::endl;
                            }
                            lastVal = val;
                        }
                        else
                        {
                            TEST_INFO("Data are not coherent: maybe access not protected or update pb!");
                            TEST_FAILED();
                        }
                    }                    
                    if (testOk)
                    {
                        TEST_INFO("Test ok");
                        // release wait on Thread 2 loop and End of loops in both Threads
                        Cdmw::OsSupport::OS::sleep(timescale*synchroWait);
                        {
                            {
                                CDMW_MUTEX_GUARD(M_synchro12Mutex);
                                M_pred12WaitOk = true;
                            }
                            M_continueLoop = false;
                            M_synchro12Condition.broadcast();
                        }
                        Cdmw::OsSupport::OS::sleep(timescale*SIZESCALE);
                        break;
                    }
                    else
                    {
                        TEST_INFO("Try another time..");
                    }                    
                }

                if (testOk)
                {
                    TEST_SUCCEED();
                }
                else
                {
                    std::cout << std::endl;
                    TEST_INFO("No trace of concurency");
                    TEST_FAILED();
                }
                //Clean after this test
                for (int i=0; i<100*SIZESCALE; i++)
                {
                  m_dataPart1StorageHome->remove(i);
                }
             }
             catch (const AlreadyExistException& )
             {
                std::cout << "AlreadyExistException raised!" << std::endl;
                TEST_FAILED();
             }
             catch (const CORBA::SystemException& e)
             {
                std::cerr << "CORBA::SystemException raised: " << e << std::endl;
                TEST_FAILED();
             }
*/
             TEST_INFO("THREAD-1: 1) Test concurrent Write of different data instances of the same datastore instance");
             try 
             {
                // First part: the Thread 1 create entries in the datastore
                // No concurrency here: this part is done only by thread 1 to prepare
                // the test of concurent updates: Thread 2 is waiting on condition
                Test::Ident id=0;
                Test::DataPart1 data;
                data.thread_name = CORBA::string_dup("Thread nb 1");
                data.index=0;
                DataPart1StorageObject obj=m_dataPart1StorageHome->create(id, data);
                TEST_INFO("THREAD-1 ---> Test Start: Waiting to synchronise with Thread 2");
                Cdmw::OsSupport::OS::sleep(timescale*5000);
                {
                    {
                        CDMW_MUTEX_GUARD(M_synchro12Mutex);
                        M_pred12WaitOk = true;
                    }
                    M_synchro12Condition.broadcast();
                }

   
                bool testOk = true;
                for (int j=1; j<11; j++)
                {
                    std::cout << std::endl << "THREAD-1 try number:" << j << std::endl;
                    TEST_INFO("THREAD-1 ---> Waiting before allowing Thread-2 to update");
                    Cdmw::OsSupport::OS::sleep(timescale*synchroWait);
                    {
                        {
                            CDMW_MUTEX_GUARD(M_synchro12Mutex);
                            M_pred12WaitOk = true;
                        }
                        M_synchro12Condition.broadcast();
                    }
                     TEST_INFO("THREAD-1 ---> Creating data in datastore 1 section 1");
                    //Cdmw::OsSupport::OS::sleep(timescale*SIZESCALE);
                    // Begin of the concurrent update part on same datastore
                    for (int i=1; i<100*SIZESCALE; i++)
                    {
                        id = i;
                        data.index = i + j;
                        M_beforeOp[i] = Cdmw::OsSupport::OS::get_time();
                        obj = m_dataPart1StorageHome->create(id, data);
                        M_afterOp[i] = Cdmw::OsSupport::OS::get_time();
                        //std::cout << "c1";
                    }
                    std::cout << std::endl;
                    TEST_INFO("THREAD-1: wait for Thread 2 synchronisation signal");
                    {
                        CDMW_MUTEX_GUARD(M_synchro21Mutex);
                        while (!M_pred21WaitOk) M_synchro21Condition.wait();
                        M_pred21WaitOk = false;
                    }
                    TEST_INFO("THREAD-1: checking concurent updates");
                    // third part: the Thread 1 verifies the datastore entries
                    // No concurrency here: this check part is done only by thread 1
                    // Thread 2 is waiting on condition for next time
                    Test::DataPart1_var dataCheck = m_dataPart1StorageHome->find_data_by_oid(0);
                    testOk=true;
                    for (int i=1; ((i<100*SIZESCALE)&&(testOk==true)); i++)
                    {
                        id = i;
                        dataCheck = m_dataPart1StorageHome->find_data_by_oid(id);
                        if (dataCheck.in().index != (i + j)) testOk=false;
                    }                    
                    for (int i=100*SIZESCALE; ((i<200*SIZESCALE)&&(testOk==true)); i++)
                    {
                        id = i;
                        dataCheck = m_dataPart1StorageHome->find_data_by_oid(id);
                        if (dataCheck.in().index != (i + j + 10)) testOk=false;
                    }                    
                    if (testOk)
                    {
                        int inter1[2]={1,(100*SIZESCALE)-1};
                        int inter2[2]={100*SIZESCALE,(200*SIZESCALE)-1}; 
                        CheckResult res = checkTimestamps(inter1, inter2);   
                        if ((res.firstInterval == 0)||(res.overlapIndice == -1))
                        {
                            testOk=false;
                        }
                        else if ((m_realOverlap)&&(res.realOverlapIndice1 == -1))
                        {
                            testOk=false;
                        }
                    }
                    if (testOk)
                    {
                        TEST_INFO("Test ok");
                        // release wait on Thread 2 loop and End of loops in both Threads
                        Cdmw::OsSupport::OS::sleep(timescale*synchroWait);
                        {
                            {
                                CDMW_MUTEX_GUARD(M_synchro12Mutex);
                                M_pred12WaitOk = true;
                            }
                            M_continueLoop = false;
                            M_synchro12Condition.broadcast();
                        }
                        break;
                    }
                    else
                    {
                        TEST_INFO("Try another time..");
                    }                    
                    std::cout << "Clean data store before new creation" << std::endl;
                    for (int i=1; i<200*SIZESCALE; i++)
                    {
                        id = i;
                        m_dataPart1StorageHome->remove(id);
                    }
                }
                if (testOk)
                {
                    TEST_INFO("THREAD-1: 1) OK");
                    TEST_SUCCEED();
                }
                else
                {
                    std::cout << std::endl;
                    TEST_INFO("No trace of concurency");
                    TEST_FAILED();
                }
             }
             catch (const AlreadyExistException& )
             {
                std::cout << "AlreadyExistException raised!" << std::endl;
                TEST_FAILED();
             }
             catch (const CORBA::SystemException& e)
             {
                std::cerr << "CORBA::SystemException raised: " << e << std::endl;
                TEST_FAILED();
             }

             TEST_INFO("THREAD-1: 2) Test concurrent Write and Read of different data instances of the same datastore instance");
             try 
             {
                // No concurrency here: this part is done only by thread 1 to prepare
                // the test of concurent updates: Thread 2 is waiting on condition
                Test::Ident id=0;
   
                bool testOk = true;
                for (int j=1; j<11; j++)
                {
                    std::cout << std::endl << "THREAD-1 try number:" << j << std::endl;
                    TEST_INFO("THREAD-1 ---> Waiting before allowing Thread-2 to update");
                    Cdmw::OsSupport::OS::sleep(timescale*synchroWait);
                    {
                        {
                            CDMW_MUTEX_GUARD(M_synchro12Mutex);
                            M_pred12WaitOk = true;
                        }
                        M_synchro12Condition.broadcast();
                    }
                    TEST_INFO("THREAD-1 ---> Reading data in datastore 1 section 1");
                    //Cdmw::OsSupport::OS::sleep(timescale*SIZESCALE);
                    // Begin of the concurrent update part on same datastore
                    for (int i=1; i<100*SIZESCALE; i++)
                    {
                        id = i;
                        M_beforeOp[i] = Cdmw::OsSupport::OS::get_time();
                        Test::DataPart1_var data = m_dataPart1StorageHome->find_data_by_oid(id);
                        M_afterOp[i] = Cdmw::OsSupport::OS::get_time();
                        //std::cout << "r1";
                    }
                    std::cout << std::endl;
                    TEST_INFO("THREAD-1: wait for Thread 2 synchronisation signal");
                    {
                        CDMW_MUTEX_GUARD(M_synchro21Mutex);
                        while (!M_pred21WaitOk) M_synchro21Condition.wait();
                        M_pred21WaitOk = false;
                    }
                    TEST_INFO("THREAD-1: checking concurent updates");
                    // third part: the Thread 1 verifies the datastore entries
                    // No concurrency here: this check part is done only by thread 1
                    // Thread 2 is waiting on condition for next time
                    Test::DataPart1_var dataCheck = m_dataPart1StorageHome->find_data_by_oid(0);
                    testOk=true;
                    for (int i=100*SIZESCALE; ((i<200*SIZESCALE)&&(testOk==true)); i++)
                    {
                        id = i;
                        dataCheck = m_dataPart1StorageHome->find_data_by_oid(id);
                        if (dataCheck.in().index != (i + j + 10)) testOk=false;
                    }                    
                    if (testOk)
                    {
                        int inter1[2]={1,(100*SIZESCALE)-1};
                        int inter2[2]={100*SIZESCALE,(200*SIZESCALE)-1};                        
                        CheckResult res = checkTimestamps(inter1, inter2);   
                        if ((res.firstInterval == 0)||(res.overlapIndice == -1))
                        {
                            testOk=false;
                        }
                        else if ((m_realOverlap)&&(res.realOverlapIndice1 == -1))
                        {
                            testOk=false;
                        }
                    }
                    if (testOk)
                    {
                        TEST_INFO("Test ok");
                        // release wait on Thread 2 loop and End of loops in both Threads
                        Cdmw::OsSupport::OS::sleep(timescale*synchroWait);
                        {
                            {
                                CDMW_MUTEX_GUARD(M_synchro12Mutex);
                                M_pred12WaitOk = true;
                            }
                            M_continueLoop = false;
                            M_synchro12Condition.broadcast();
                        }
                        break;
                    }
                    else
                    {
                        TEST_INFO("Try another time..");
                    }                    
                }
                if (testOk)
                {
                    TEST_INFO("THREAD-1: 2) OK");
                    TEST_SUCCEED();
                }
                else
                {
                    std::cout << std::endl;
                    TEST_INFO("No trace of concurency");
                    TEST_FAILED();
                }
             }
             catch (const AlreadyExistException& )
             {
                std::cout << "AlreadyExistException raised!" << std::endl;
                TEST_FAILED();
             }
             catch (const CORBA::SystemException& e)
             {
                std::cerr << "CORBA::SystemException raised: " << e << std::endl;
                TEST_FAILED();
             }

             //Cdmw::OsSupport::OS::sleep(timescale*SIZESCALE);
             TEST_INFO("THREAD-1: 3) Test concurrent Reads of different data instances of the same datastore instance");
             try 
             {
                // No concurrency here: this part is done only by thread 1 to prepare
                // the test of concurent updates: Thread 2 is waiting on condition
                Test::Ident id=0;
   
                bool testOk = true;
                for (int j=1; j<11; j++)
                {
                    std::cout << std::endl << "THREAD-1 try number:" << j << std::endl;
                    TEST_INFO("THREAD-1 ---> Waiting before allowing Thread-2 to update");
                    Cdmw::OsSupport::OS::sleep(timescale*synchroWait);
                    {
                        {
                            CDMW_MUTEX_GUARD(M_synchro12Mutex);
                            M_pred12WaitOk = true;
                        }
                        M_synchro12Condition.broadcast();
                    }
                     TEST_INFO("THREAD-1 ---> Reading data in datastore 1 section 1");
                    //Cdmw::OsSupport::OS::sleep(timescale*SIZESCALE);
                    // Begin of the concurrent update part on same datastore
                    for (int i=1; i<100*SIZESCALE; i++)
                    {
                        id = i;
                        M_beforeOp[i] = Cdmw::OsSupport::OS::get_time();
                        Test::DataPart1_var data = m_dataPart1StorageHome->find_data_by_oid(id);
                        M_afterOp[i] = Cdmw::OsSupport::OS::get_time();
                        //std::cout << "r1";
                    }
                    std::cout << std::endl;
                    TEST_INFO("THREAD-1: wait for Thread 2 synchronisation signal");
                    {
                        CDMW_MUTEX_GUARD(M_synchro21Mutex);
                        while (!M_pred21WaitOk) M_synchro21Condition.wait();
                        M_pred21WaitOk = false;
                    }
                    TEST_INFO("THREAD-1: checking concurent updates");
                    // third part: the Thread 1 verifies the datastore entries
                    // No concurrency here: this check part is done only by thread 1
                    // Thread 2 is waiting on condition for next time
                    
                    int inter1[2]={1,(100*SIZESCALE)-1};
                    int inter2[2]={100*SIZESCALE,(200*SIZESCALE)-1};          
                    testOk=true;
                    CheckResult res = checkTimestamps(inter1, inter2);        
                    if ((res.firstInterval == 0)||(res.overlapIndice == -1))
                    {
                        testOk=false;
                    }
                    else if ((m_realOverlap)&&(res.realOverlapIndice1 == -1))
                    {
                        testOk=false;
                    }

                    if (testOk)
                    {
                        TEST_INFO("Test ok");
                        // release wait on Thread 2 loop and End of loops in both Threads
                        Cdmw::OsSupport::OS::sleep(timescale*synchroWait);
                        {
                            {
                                CDMW_MUTEX_GUARD(M_synchro12Mutex);
                                M_pred12WaitOk = true;
                            }
                            M_continueLoop = false;
                            M_synchro12Condition.broadcast();
                        }
                        break;
                    }
                    else
                    {
                        TEST_INFO("Try another time..");
                    }                    
                }
                if (testOk)
                {
                    TEST_INFO("THREAD-1: 3) OK");
                    TEST_SUCCEED();
                }
                else
                {
                    std::cout << std::endl;
                    TEST_INFO("No trace of concurency");
                    TEST_FAILED();
                }
             }
             catch (const AlreadyExistException& )
             {
                std::cout << "AlreadyExistException raised!" << std::endl;
                TEST_FAILED();
             }
             catch (const CORBA::SystemException& e)
             {
                std::cerr << "CORBA::SystemException raised: " << e << std::endl;
                TEST_FAILED();
             }

             TEST_INFO("THREAD-1: 4) Test concurrent Write on different datastore instance within the same process");
             try 
             {
                // First part: the Thread 1 create entries in the datastore
                // No concurrency here: this part is done only by thread 1 to prepare
                // the test of concurent updates: Thread 2 is waiting on condition
                Test::Ident id=0;
                Test::DataPart1 data;
                data.thread_name = CORBA::string_dup("Thread nb 1");
                data.index=0;
                DataPart1StorageObject obj=m_dataPart1StorageHome->find_by_oid(0);
   
                bool testOk = true;
                for (int j=1; j<11; j++)
                {
                    std::cout << std::endl << "THREAD-1 try number:" << j << std::endl;
                    TEST_INFO("THREAD-1 ---> Waiting before allowing Thread-2 to update");
                    Cdmw::OsSupport::OS::sleep(timescale*synchroWait);
                    {
                        {
                            CDMW_MUTEX_GUARD(M_synchro12Mutex);
                            M_pred12WaitOk = true;
                        }
                        M_synchro12Condition.broadcast();
                    }
                     TEST_INFO("THREAD-1 ---> Creating data in datastore 1 section 3");
                    //Cdmw::OsSupport::OS::sleep(timescale*SIZESCALE);
                    // Begin of the concurrent update part on same datastore
                    for (int i=200*SIZESCALE; i<300*SIZESCALE; i++)
                    {
                        id = i;
                        data.index = i + j;
                        M_beforeOp[i] = Cdmw::OsSupport::OS::get_time();
                        obj = m_dataPart1StorageHome->create(id, data);
                        M_afterOp[i] = Cdmw::OsSupport::OS::get_time();
                        //std::cout << "c1";
                    }
                    std::cout << std::endl;
                    TEST_INFO("THREAD-1: wait for Thread 2 synchronisation signal");
                    {
                        CDMW_MUTEX_GUARD(M_synchro21Mutex);
                        while (!M_pred21WaitOk) M_synchro21Condition.wait();
                        M_pred21WaitOk = false;
                    }
                    TEST_INFO("THREAD-1: checking concurent updates");
                    // third part: the Thread 1 verifies the datastore entries
                    // No concurrency here: this check part is done only by thread 1
                    // Thread 2 is waiting on condition for next time
                    Test::DataPart1_var dataCheck1 = m_dataPart1StorageHome->find_data_by_oid(200);
                    Test::DataPart2_var dataCheck2 = m_dataPart2StorageHome->find_data_by_oid(0);
                    testOk=true;
                    for (int i=200*SIZESCALE; ((i<300*SIZESCALE)&&(testOk==true)); i++)
                    {
                        id = i;
                        dataCheck1 = m_dataPart1StorageHome->find_data_by_oid(id);
                        if (dataCheck1.in().index != (i + j)) testOk=false;
                    }                    
                    for (int i=1; ((i<100*SIZESCALE)&&(testOk==true)); i++)
                    {
                        id = i;
                        dataCheck2 = m_dataPart2StorageHome->find_data_by_oid(id);
                        if ((dataCheck2.in().index != (i + j + 10))||(dataCheck2.in().is_alive == false)) testOk=false;
                    }                    
                    if (testOk)
                    {
                        int inter1[2]={1,(100*SIZESCALE)-1};
                        int inter2[2]={200*SIZESCALE,(300*SIZESCALE)-1};                        
                        CheckResult res = checkTimestamps(inter1, inter2);        
                        if ((res.firstInterval == 0)||(res.overlapIndice == -1))
                        {
                            testOk=false;
                        }
                        else if ((m_realOverlap)&&(res.realOverlapIndice1 == -1))
                        {
                            testOk=false;
                        }

                    }
                    if (testOk)
                    {
                        TEST_INFO("Test ok");
                        // release wait on Thread 2 loop and End of loops in both Threads
                        Cdmw::OsSupport::OS::sleep(timescale*synchroWait);
                        {
                            {
                                CDMW_MUTEX_GUARD(M_synchro12Mutex);
                                M_pred12WaitOk = true;
                            }
                            M_continueLoop = false;
                            M_synchro12Condition.broadcast();
                        }
                        break;
                    }
                    else
                    {
                        TEST_INFO("Try another time..");
                    }                    
                    std::cout << "Clean data store before new creation" << std::endl;
                    for (int i=200*SIZESCALE; i<300*SIZESCALE; i++)
                    {
                        id = i;
                        m_dataPart1StorageHome->remove(id);
                    }
                    for (int i=1; i<100*SIZESCALE; i++)
                    {
                        id = i;
                        m_dataPart2StorageHome->remove(id);
                    }
                }
                if (testOk)
                {
                    TEST_INFO("THREAD-1: 4) OK");
                    TEST_SUCCEED();
                }
                else
                {
                    std::cout << std::endl;
                    TEST_INFO("No trace of concurency");
                    TEST_FAILED();
                }
             }
             catch (const AlreadyExistException& )
             {
                std::cout << "AlreadyExistException raised!" << std::endl;
                TEST_FAILED();
             }
             catch (const CORBA::SystemException& e)
             {
                std::cerr << "CORBA::SystemException raised: " << e << std::endl;
                TEST_FAILED();
             }

             TEST_INFO("THREAD-1: 5) Test concurrent Write and Read on different datastore instance within the same process");
             try 
             {
                // No concurrency here: this part is done only by thread 1 to prepare
                // the test of concurent updates: Thread 2 is waiting on condition
                Test::Ident id=0;
   
                bool testOk = true;
                for (int j=1; j<11; j++)
                {
                    std::cout << std::endl << "THREAD-1 try number:" << j << std::endl;
                    TEST_INFO("THREAD-1 ---> Waiting before allowing Thread-2 to update");
                    Cdmw::OsSupport::OS::sleep(timescale*synchroWait);
                    {
                        {
                            CDMW_MUTEX_GUARD(M_synchro12Mutex);
                            M_pred12WaitOk = true;
                        }
                        M_synchro12Condition.broadcast();
                    }
                    TEST_INFO("THREAD-1 ---> Reading data in datastore 1 section 3");
                    //Cdmw::OsSupport::OS::sleep(timescale*SIZESCALE);
                    // Begin of the concurrent update part on same datastore
                    for (int i=200*SIZESCALE; i<300*SIZESCALE; i++)
                    {
                        id = i;
                        M_beforeOp[i] = Cdmw::OsSupport::OS::get_time();
                        Test::DataPart1_var data = m_dataPart1StorageHome->find_data_by_oid(id);
                        M_afterOp[i] = Cdmw::OsSupport::OS::get_time();
                        //std::cout << "r1";
                    }
                    std::cout << std::endl;
                    TEST_INFO("THREAD-1: wait for Thread 2 synchronisation signal");
                    {
                        CDMW_MUTEX_GUARD(M_synchro21Mutex);
                        while (!M_pred21WaitOk) M_synchro21Condition.wait();
                        M_pred21WaitOk = false;
                    }
                    TEST_INFO("THREAD-1: checking concurent updates");
                    // third part: the Thread 1 verifies the datastore entries
                    // No concurrency here: this check part is done only by thread 1
                    // Thread 2 is waiting on condition for next time
                    Test::DataPart1_var dataCheck1 = m_dataPart1StorageHome->find_data_by_oid(0);
                    Test::DataPart2_var dataCheck2 = m_dataPart2StorageHome->find_data_by_oid(0);
                    testOk=true;
                    for (int i=1; ((i<100*SIZESCALE)&&(testOk==true)); i++)
                    {
                        id = i;
                        dataCheck2 = m_dataPart2StorageHome->find_data_by_oid(id);
                        if ((dataCheck2.in().index != (i + j + 10))||(dataCheck2.in().is_alive == false)) testOk=false;
                    }                    
                    if (testOk)
                    {
                        int inter1[2]={1,(100*SIZESCALE)-1};
                        int inter2[2]={200*SIZESCALE,(300*SIZESCALE)-1};                        
                        CheckResult res = checkTimestamps(inter1, inter2);        
                        if ((res.firstInterval == 0)||(res.overlapIndice == -1))
                        {
                            testOk=false;
                        }
                        else if ((m_realOverlap)&&(res.realOverlapIndice1 == -1))
                        {
                            testOk=false;
                        }
                    }
                    if (testOk)
                    {
                        TEST_INFO("Test ok");
                        // release wait on Thread 2 loop and End of loops in both Threads
                        Cdmw::OsSupport::OS::sleep(timescale*synchroWait);
                        {
                            {
                                CDMW_MUTEX_GUARD(M_synchro12Mutex);
                                M_pred12WaitOk = true;
                            }
                            M_continueLoop = false;
                            M_synchro12Condition.broadcast();
                        }
                        break;
                    }
                    else
                    {
                        TEST_INFO("Try another time..");
                    }                    
                }
                if (testOk)
                {
                    TEST_INFO("THREAD-1: 5) OK");
                    TEST_SUCCEED();
                }
                else
                {
                    std::cout << std::endl;
                    TEST_INFO("No trace of concurency");
                    TEST_FAILED();
                }
             }
             catch (const AlreadyExistException& )
             {
                std::cout << "AlreadyExistException raised!" << std::endl;
                TEST_FAILED();
             }
             catch (const CORBA::SystemException& e)
             {
                std::cerr << "CORBA::SystemException raised: " << e << std::endl;
                TEST_FAILED();
             }

             TEST_INFO("THREAD-1: 6) Test concurrent Reads on different datastore instance within the same process");
             try 
             {
                // No concurrency here: this part is done only by thread 1 to prepare
                // the test of concurent updates: Thread 2 is waiting on condition
                Test::Ident id=0;
   
                bool testOk = true;
                for (int j=1; j<11; j++)
                {
                    std::cout << std::endl << "THREAD-1 try number:" << j << std::endl;
                    TEST_INFO("THREAD-1 ---> Waiting before allowing Thread-2 to update");
                    Cdmw::OsSupport::OS::sleep(timescale*synchroWait);
                    {
                        {
                            CDMW_MUTEX_GUARD(M_synchro12Mutex);
                            M_pred12WaitOk = true;
                        }
                        M_synchro12Condition.broadcast();
                    }
                     TEST_INFO("THREAD-1 ---> Reading data in datastore 1 section 3");
                    //Cdmw::OsSupport::OS::sleep(timescale*SIZESCALE);
                    // Begin of the concurrent update part on same datastore
                    for (int i=200*SIZESCALE; i<300*SIZESCALE; i++)
                    {
                        id = i;
                        M_beforeOp[i] = Cdmw::OsSupport::OS::get_time();
                        Test::DataPart1_var data = m_dataPart1StorageHome->find_data_by_oid(id);
                        M_afterOp[i] = Cdmw::OsSupport::OS::get_time();
                        //std::cout << "r1";
                    }
                    std::cout << std::endl;
                    TEST_INFO("THREAD-1: wait for Thread 2 synchronisation signal");
                    {
                        CDMW_MUTEX_GUARD(M_synchro21Mutex);
                        while (!M_pred21WaitOk) M_synchro21Condition.wait();
                        M_pred21WaitOk = false;
                    }
                    TEST_INFO("THREAD-1: checking concurent updates");
                    // third part: the Thread 1 verifies the datastore entries
                    // No concurrency here: this check part is done only by thread 1
                    // Thread 2 is waiting on condition for next time                    
                    testOk=true;
                    int inter1[2]={1,(100*SIZESCALE)-1};
                    int inter2[2]={200*SIZESCALE,(300*SIZESCALE)-1};                        
                    CheckResult res = checkTimestamps(inter1, inter2);        
                    if ((res.firstInterval == 0)||(res.overlapIndice == -1))
                    {
                        testOk=false;
                    }
                    else if ((m_realOverlap)&&(res.realOverlapIndice1 == -1))
                    {
                        testOk=false;
                    }

                    if (testOk)
                    {
                        TEST_INFO("Test ok");
                        // release wait on Thread 2 loop and End of loops in both Threads
                        Cdmw::OsSupport::OS::sleep(timescale*synchroWait);
                        {
                            {
                                CDMW_MUTEX_GUARD(M_synchro12Mutex);
                                M_pred12WaitOk = true;
                            }
                            M_continueLoop = false;
                            M_synchro12Condition.broadcast();
                        }
                        break;
                    }
                    else
                    {
                        TEST_INFO("Try another time..");
                    }                    
                }
                if (testOk)
                {
                    TEST_INFO("THREAD-1: 6) OK");
                    TEST_SUCCEED();
                }
                else
                {
                    std::cout << std::endl;
                    TEST_INFO("No trace of concurency");
                    TEST_FAILED();
                }
             }
             catch (const AlreadyExistException& )
             {
                std::cout << "AlreadyExistException raised!" << std::endl;
                TEST_FAILED();
             }
             catch (const CORBA::SystemException& e)
             {
                std::cerr << "CORBA::SystemException raised: " << e << std::endl;
                TEST_FAILED();
             }

             TEST_INFO("THREAD-1: Final clean of datastores ");
             try 
             {
                 Test::Ident id=0;
                 for (int i=0; i<300*SIZESCALE; i++)
                 {
                     id = i;
                     m_dataPart1StorageHome->remove(id);
                 }
                 for (int i=0; i<100*SIZESCALE; i++)
                 {
                     id = i;
                     m_dataPart2StorageHome->remove(id);
                 }
                 TEST_SUCCEED();
             }
             catch (const AlreadyExistException& )
             {
                std::cout << "AlreadyExistException raised!" << std::endl;
                TEST_FAILED();
             }
             catch (const CORBA::SystemException& e)
             {
                std::cerr << "CORBA::SystemException raised: " << e << std::endl;
                TEST_FAILED();
             }

/*
             Test::DataPart1 data;
             data.thread_name = CORBA::string_dup("Thread nb 1");
             data.index = 900*SIZESCALE;
             DataPart1StorageObject obj = m_dataPart1StorageHome->create(900*SIZESCALE, data);
*/
             TEST_INFO("THREAD-1: end of test: last synchro with THREAD-2");
             Cdmw::OsSupport::OS::sleep(timescale*5000);
             {
                {
                  CDMW_MUTEX_GUARD(M_synchro12Mutex);
                  M_pred12WaitOk = true;
                }
                M_synchro12Condition.broadcast();
             }
             TEST_INFO("THREAD-1: end of test: synchro with THREAD-2 done");
          }
          break;

       case 2:
          {
             set_nbOfRequestedTestOK(6);

             TEST_INFO("THREAD-2: Test Start: wait for Thread 1 synchronisation signal");
             {
               CDMW_MUTEX_GUARD(M_synchro12Mutex);
               while (!M_pred12WaitOk) M_synchro12Condition.wait();
               M_pred12WaitOk = false;
             }
/*
             TEST_INFO("THREAD-2: 0a) Test concurrent Read/Write of one data instance of the same datastore instance");
             try 
             {
                Test::Ident id = 0;
                Test::DataPart1 data;
                data.thread_name = CORBA::string_dup("Thread nb 2");
                DataPart1StorageObject obj = m_dataPart1StorageHome->find_by_oid(id);

                TEST_INFO("THREAD-2: wait for Thread 1 synchronisation signal");
                {
                    CDMW_MUTEX_GUARD(M_synchro12Mutex);
                    while (!M_pred12WaitOk) M_synchro12Condition.wait();
                    M_pred12WaitOk = false;
                }

                TEST_INFO("THREAD-1 ---> Preliminary test: perform updates WITHOUT Thread 1");
                Test::DataPart1_var dataCheck;
                for (long i=0; i<nbIter; i++)
                {
                    dataCheck = obj.get();
                    data.index = dataCheck.in().index - 1;
                    obj.set(data);
                }
                std::cout << std::endl;
                dataCheck = obj.get();
                std::cout << "Thread 2 final value:" << dataCheck.in().index << std::endl;

                TEST_CHECK(dataCheck.in().index == 0);

                TEST_INFO("THREAD-2 ---> Waiting before allowing Thread-1 to update");
                Cdmw::OsSupport::OS::sleep(timescale*5000);
                {
                    {
                        CDMW_MUTEX_GUARD(M_synchro21Mutex);
                        M_pred21WaitOk = true;
                    }
                    M_synchro21Condition.broadcast();
                }

                TEST_INFO("THREAD-2 ---> Test: perform updates WITH Thread 1");
                TEST_INFO("THREAD-2 ---> Updating datastore 1");
                // Begin of the concurrent update part on same datastore
                dataCheck = obj.get();
                long initialVal = dataCheck.in().index;
                for (long i=0; i<nbIter; i++)
                {
                    dataCheck = obj.get();
                    data.index = dataCheck.in().index - 1;
                    obj.set(data);
                }
                std::cout << std::endl;
                std::cout << "Thread 2 initial value:" << initialVal << std::endl;
                dataCheck = obj.get();
                std::cout << "Thread 2 final value:" << dataCheck.in().index << std::endl;

                TEST_CHECK(dataCheck.in().index != (initialVal-nbIter));
             }
             catch (const AlreadyExistException& )
             {
                std::cout << "AlreadyExistException raised!" << std::endl;
                TEST_FAILED();
             }
             catch (const CORBA::SystemException& e)
             {
                std::cerr << "CORBA::SystemException raised: " << e << std::endl;
                TEST_FAILED();
             }

             TEST_INFO("THREAD-2: wait for Thread 1 synchronisation signal");
             {
               CDMW_MUTEX_GUARD(M_synchro12Mutex);
               while (!M_pred12WaitOk) M_synchro12Condition.wait();
               M_pred12WaitOk = false;
             }

             TEST_INFO("THREAD-2: 0b) Test concurrent Write of data instances of the same datastore instance");
             try 
             {
                Test::Ident id;
                Test::DataPart1 data;
                data.thread_name = CORBA::string_dup("Thread nb 2");

                for (int j=11; ((j<21)&&M_continueLoop); j++)
                {
                    TEST_INFO("THREAD-2: wait for Thread 1 synchronisation signal");
                    {
                        CDMW_MUTEX_GUARD(M_synchro12Mutex);
                        while (!M_pred12WaitOk) M_synchro12Condition.wait();
                        M_pred12WaitOk = false;
                    }
                    if (M_continueLoop)
                    {
                        // Begin of the concurrent update part on same datastore
                        DataPart1StorageObject obj = m_dataPart1StorageHome->find_by_oid(0);
                        TEST_INFO("THREAD-2 ---> Updating datastore 1");
                        for (int i=0; i<100*SIZESCALE; i++)
                        {
                            id = i;
                            data.index = i + j;
                            obj = m_dataPart1StorageHome->find_by_oid(id);
                            obj.set(data);
                            //std::cout << "s2";
                        }
                        std::cout << std::endl;
                        TEST_INFO("THREAD-2 ---> Waiting before allowing Thread-1 to verify");
                        Cdmw::OsSupport::OS::sleep(timescale*synchroWait);
                        {
                            {
                                CDMW_MUTEX_GUARD(M_synchro21Mutex);
                                M_pred21WaitOk = true;
                            }
                            M_synchro21Condition.broadcast();
                        }
                        Cdmw::OsSupport::OS::sleep(timescale*SIZESCALE);
                    }
                }
                TEST_SUCCEED();
             }
             catch (const AlreadyExistException& )
             {
                std::cout << "AlreadyExistException raised!" << std::endl;
                TEST_FAILED();
             }
             catch (const CORBA::SystemException& e)
             {
                std::cerr << "CORBA::SystemException raised: " << e << std::endl;
                TEST_FAILED();
             }
*/
             TEST_INFO("THREAD-2: 1) Test concurrent Write of different data instances of the same datastore instance");
             try 
             {
                Test::Ident id;
                Test::DataPart1 data;
                data.thread_name = CORBA::string_dup("Thread nb 2");
                data.index=0;
                DataPart1StorageObject obj = m_dataPart1StorageHome->find_by_oid(0);

                for (int j=11; ((j<21)&&M_continueLoop); j++)
                {
                    TEST_INFO("THREAD-2: wait for Thread 1 synchronisation signal");
                    {
                        CDMW_MUTEX_GUARD(M_synchro12Mutex);
                        while (!M_pred12WaitOk) M_synchro12Condition.wait();
                        M_pred12WaitOk = false;
                    }
                    if (M_continueLoop)
                    {
                        // Begin of the concurrent update part on same datastore
                        TEST_INFO("THREAD-2 ---> Creating data in datastore 1 section 2");
                        for (int i=100*SIZESCALE; i<200*SIZESCALE; i++)
                        {
                            id = i;
                            data.index = i + j;
                            M_beforeOp[i] = Cdmw::OsSupport::OS::get_time();
                            obj = m_dataPart1StorageHome->create(id, data);
                            M_afterOp[i] = Cdmw::OsSupport::OS::get_time();
                            //std::cout << "c2";
                        }
                        std::cout << std::endl;
                        TEST_INFO("THREAD-2 ---> Waiting before allowing Thread-1 to verify");
                        Cdmw::OsSupport::OS::sleep(timescale*synchroWait);
                        {
                            {
                                CDMW_MUTEX_GUARD(M_synchro21Mutex);
                                M_pred21WaitOk = true;
                            }
                            M_synchro21Condition.broadcast();
                        }
                        Cdmw::OsSupport::OS::sleep(timescale*SIZESCALE);
                    }
                }
                TEST_INFO("THREAD-2: 1) OK");
                TEST_SUCCEED();
             }
             catch (const AlreadyExistException& )
             {
                std::cout << "AlreadyExistException raised!" << std::endl;
                TEST_FAILED();
             }
             catch (const CORBA::SystemException& e)
             {
                std::cerr << "CORBA::SystemException raised: " << e << std::endl;
                TEST_FAILED();
             }

             TEST_INFO("THREAD-2: 2) Test concurrent Write and Read of different data instances of the same datastore instance");
             try 
             {
                Test::Ident id;
                Test::DataPart1 data;
                data.thread_name = CORBA::string_dup("Thread nb 2");
                data.index=0;
                DataPart1StorageObject obj = m_dataPart1StorageHome->find_by_oid(0);
                M_continueLoop = true;

                for (int j=11; ((j<21)&&M_continueLoop); j++)
                {
                    TEST_INFO("THREAD-2: wait for Thread 1 synchronisation signal");
                    {
                        CDMW_MUTEX_GUARD(M_synchro12Mutex);
                        while (!M_pred12WaitOk) M_synchro12Condition.wait();
                        M_pred12WaitOk = false;
                    }
                    if (M_continueLoop)
                    {
                        // Begin of the concurrent update part on same datastore
                        TEST_INFO("THREAD-2 ---> Updating data in datastore 1 section 2");
                        for (int i=100*SIZESCALE; i<200*SIZESCALE; i++)
                        {
                            id = i;
                            data.index = i + j;
                            obj = m_dataPart1StorageHome->find_by_oid(id);
                            M_beforeOp[i] = Cdmw::OsSupport::OS::get_time();
                            obj.set(data);
                            M_afterOp[i] = Cdmw::OsSupport::OS::get_time();
                            //std::cout << "s2";
                        }
                        std::cout << std::endl;
                        TEST_INFO("THREAD-2 ---> Waiting before allowing Thread-1 to verify");
                        Cdmw::OsSupport::OS::sleep(timescale*synchroWait);
                        {
                            {
                                CDMW_MUTEX_GUARD(M_synchro21Mutex);
                                M_pred21WaitOk = true;
                            }
                            M_synchro21Condition.broadcast();
                        }
                        Cdmw::OsSupport::OS::sleep(timescale*SIZESCALE);
                    }
                }
                TEST_INFO("THREAD-2: 2) OK");
                TEST_SUCCEED();
             }
             catch (const AlreadyExistException& )
             {
                std::cout << "AlreadyExistException raised!" << std::endl;
                TEST_FAILED();
             }
             catch (const CORBA::SystemException& e)
             {
                std::cerr << "CORBA::SystemException raised: " << e << std::endl;
                TEST_FAILED();
             }

             TEST_INFO("THREAD-2: 3) Test concurrent Reads of different data instances of the same datastore instance");
             try 
             {
                Test::Ident id;
                M_continueLoop = true;

                for (int j=11; ((j<21)&&M_continueLoop); j++)
                {
                    TEST_INFO("THREAD-2: wait for Thread 1 synchronisation signal");
                    {
                        CDMW_MUTEX_GUARD(M_synchro12Mutex);
                        while (!M_pred12WaitOk) M_synchro12Condition.wait();
                        M_pred12WaitOk = false;
                    }
                    if (M_continueLoop)
                    {
                        // Begin of the concurrent update part on same datastore
                        TEST_INFO("THREAD-2 ---> Reading data in datastore 1 section 2");
                        for (int i=100*SIZESCALE; i<200*SIZESCALE; i++)
                        {
                            id = i;
                            M_beforeOp[i] = Cdmw::OsSupport::OS::get_time();
                            Test::DataPart1_var data = m_dataPart1StorageHome->find_data_by_oid(id);
                            M_afterOp[i] = Cdmw::OsSupport::OS::get_time();
                            //std::cout << "r2";
                        }
                        std::cout << std::endl;
                        TEST_INFO("THREAD-2 ---> Waiting before allowing Thread-1 to verify");
                        Cdmw::OsSupport::OS::sleep(timescale*synchroWait);
                        {
                            {
                                CDMW_MUTEX_GUARD(M_synchro21Mutex);
                                M_pred21WaitOk = true;
                            }
                            M_synchro21Condition.broadcast();
                        }
                        Cdmw::OsSupport::OS::sleep(timescale*SIZESCALE);
                    }
                }
                TEST_INFO("THREAD-2: 3) OK");
                TEST_SUCCEED();
             }
             catch (const AlreadyExistException& )
             {
                std::cout << "AlreadyExistException raised!" << std::endl;
                TEST_FAILED();
             }
             catch (const CORBA::SystemException& e)
             {
                std::cerr << "CORBA::SystemException raised: " << e << std::endl;
                TEST_FAILED();
             }

             TEST_INFO("THREAD-2: 4) Test concurrent Write on different datastore instances within the same process");
             try 
             {
                Test::Ident id=0;
                Test::DataPart2 data;
                data.thread_name = CORBA::string_dup("Thread nb 2");
                data.index=0;
                DataPart2StorageObject obj = m_dataPart2StorageHome->create(id,data);
                M_continueLoop = true;


                for (int j=11; ((j<21)&&M_continueLoop); j++)
                {
                    TEST_INFO("THREAD-2: wait for Thread 1 synchronisation signal");
                    {
                        CDMW_MUTEX_GUARD(M_synchro12Mutex);
                        while (!M_pred12WaitOk) M_synchro12Condition.wait();
                        M_pred12WaitOk = false;
                    }
                    if (M_continueLoop)
                    {
                        // Begin of the concurrent update part on same datastore
                        TEST_INFO("THREAD-2 ---> Creating data in datastore 2 section 1");
                        for (int i=1; i<100*SIZESCALE; i++)
                        {
                            id = i;
                            data.index = i + j;
                            data.is_alive = true;
                            M_beforeOp[i] = Cdmw::OsSupport::OS::get_time();
                            obj = m_dataPart2StorageHome->create(id, data);
                            M_afterOp[i] = Cdmw::OsSupport::OS::get_time();
                            //std::cout << "c2";
                        }
                        std::cout << std::endl;
                        TEST_INFO("THREAD-2 ---> Waiting before allowing Thread-1 to verify");
                        Cdmw::OsSupport::OS::sleep(timescale*synchroWait);
                        {
                            {
                                CDMW_MUTEX_GUARD(M_synchro21Mutex);
                                M_pred21WaitOk = true;
                            }
                            M_synchro21Condition.broadcast();
                        }
                        Cdmw::OsSupport::OS::sleep(timescale*SIZESCALE);
                    }
                }
                TEST_INFO("THREAD-2: 4) OK");
                TEST_SUCCEED();
             }
             catch (const AlreadyExistException& )
             {
                std::cout << "AlreadyExistException raised!" << std::endl;
                TEST_FAILED();
             }
             catch (const CORBA::SystemException& e)
             {
                std::cerr << "CORBA::SystemException raised: " << e << std::endl;
                TEST_FAILED();
             }

             TEST_INFO("THREAD-2: 5) Test concurrent Write and Read on different datastore instances within the same process");
             try 
             {
                Test::Ident id=0;
                Test::DataPart2 data;
                data.thread_name = CORBA::string_dup("Thread nb 2");
                data.index=0;
                DataPart2StorageObject obj = m_dataPart2StorageHome->find_by_oid(0);
                M_continueLoop = true;

                for (int j=11; ((j<21)&&M_continueLoop); j++)
                {
                    TEST_INFO("THREAD-2: wait for Thread 1 synchronisation signal");
                    {
                        CDMW_MUTEX_GUARD(M_synchro12Mutex);
                        while (!M_pred12WaitOk) M_synchro12Condition.wait();
                        M_pred12WaitOk = false;
                    }
                    if (M_continueLoop)
                    {
                        // Begin of the concurrent update part on same datastore
                        TEST_INFO("THREAD-2 ---> Updating data in datastore 2 section 1");
                        for (int i=1; i<100*SIZESCALE; i++)
                        {
                            id = i;
                            data.index = i + j;
                            data.is_alive = true;
                            obj = m_dataPart2StorageHome->find_by_oid(id);
                            M_beforeOp[i] = Cdmw::OsSupport::OS::get_time();
                            obj.set(data);
                            M_afterOp[i] = Cdmw::OsSupport::OS::get_time();
                            //std::cout << "s2";
                        }
                        std::cout << std::endl;
                        TEST_INFO("THREAD-2 ---> Waiting before allowing Thread-1 to verify");
                        Cdmw::OsSupport::OS::sleep(timescale*synchroWait);
                        {
                            {
                                CDMW_MUTEX_GUARD(M_synchro21Mutex);
                                M_pred21WaitOk = true;
                            }
                            M_synchro21Condition.broadcast();
                        }
                        Cdmw::OsSupport::OS::sleep(timescale*SIZESCALE);
                    }
                }
                TEST_INFO("THREAD-2: 5) OK");
                TEST_SUCCEED();
             }
             catch (const AlreadyExistException& )
             {
                std::cout << "AlreadyExistException raised!" << std::endl;
                TEST_FAILED();
             }
             catch (const CORBA::SystemException& e)
             {
                std::cerr << "CORBA::SystemException raised: " << e << std::endl;
                TEST_FAILED();
             }

             TEST_INFO("THREAD-2: 6) Test concurrent Reads on different datastore instances within the same process");
             try 
             {
                Test::Ident id=0;
                M_continueLoop = true;

                for (int j=11; ((j<21)&&M_continueLoop); j++)
                {
                    TEST_INFO("THREAD-2: wait for Thread 1 synchronisation signal");
                    {
                        CDMW_MUTEX_GUARD(M_synchro12Mutex);
                        while (!M_pred12WaitOk) M_synchro12Condition.wait();
                        M_pred12WaitOk = false;
                    }
                    if (M_continueLoop)
                    {
                        // Begin of the concurrent update part on same datastore
                        TEST_INFO("THREAD-2 ---> Reading data in datastore 2 section 1");
                        for (int i=1; i<100*SIZESCALE; i++)
                        {
                            id = i;
                            M_beforeOp[i] = Cdmw::OsSupport::OS::get_time();
                            Test::DataPart2_var data = m_dataPart2StorageHome->find_data_by_oid(id);
                            M_afterOp[i] = Cdmw::OsSupport::OS::get_time();
                            //std::cout << "r2";
                        }
                        std::cout << std::endl;
                        TEST_INFO("THREAD-2 ---> Waiting before allowing Thread-1 to verify");
                        Cdmw::OsSupport::OS::sleep(timescale*synchroWait);
                        {
                            {
                                CDMW_MUTEX_GUARD(M_synchro21Mutex);
                                M_pred21WaitOk = true;
                            }
                            M_synchro21Condition.broadcast();
                        }
                        Cdmw::OsSupport::OS::sleep(timescale*SIZESCALE);
                    }
                }
                TEST_INFO("THREAD-2: 6) OK");
                TEST_SUCCEED();
             }
             catch (const AlreadyExistException& )
             {
                std::cout << "AlreadyExistException raised!" << std::endl;
                TEST_FAILED();
             }
             catch (const CORBA::SystemException& e)
             {
                std::cerr << "CORBA::SystemException raised: " << e << std::endl;
                TEST_FAILED();
             }

/*
             Test::DataPart2 data;
             data.thread_name = CORBA::string_dup("Thread nb 2");
             data.index = 900*SIZESCALE;
             data.is_alive = true;
             DataPart2StorageObject obj = m_dataPart2StorageHome->create(900*SIZESCALE, data);
*/
             TEST_INFO("THREAD-2: end of test: last synchro with THREAD-1");
             {
               CDMW_MUTEX_GUARD(M_synchro12Mutex);
               while (!M_pred12WaitOk) M_synchro12Condition.wait();
               M_pred12WaitOk = false;
             }
             TEST_INFO("THREAD-2: end of test: synchro with THREAD-1 done");
          }
          break;

       default:
          {
             std::cerr << "ERROR: Invalid scenario selected" << std::endl;
          }
          break;
    } // end switch


    TEST_INFO("End of the test THREAD-" << m_scenarioIndex);
}

} // End namespace FT 

} // End namespace Cdmw



