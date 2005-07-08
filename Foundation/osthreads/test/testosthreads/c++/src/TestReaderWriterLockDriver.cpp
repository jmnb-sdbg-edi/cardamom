/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your 
 * option) any later version. 
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public 
 * License for more details. 
 * 
 * You should have received a copy of the GNU Library General 
 * Public License along with CARDAMOM; see the file COPYING. If not, write to 
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


#include "testosthreads/TestReaderWriterLockDriver.hpp"

#include <vector>

#include "Foundation/common/System.hpp"
#include "Foundation/osthreads/ReaderWriterLock.hpp"
#include "Foundation/osthreads/Thread.hpp"


#include "testosthreads/TestReaderWriterLockBasicReaderThread.hpp"
#include "testosthreads/TestReaderWriterLockBasicWriterThread.hpp"

#include "testosthreads/TestReaderWriterLock.hpp"
#include "testosthreads/TestReaderWriterLockReaderThread.hpp"
#include "testosthreads/TestReaderWriterLockWriterThread.hpp"


namespace Cdmw
{

    namespace OsSupport
    {

        TestReaderWriterLockDriver::TestReaderWriterLockDriver()
                : Testable("Cdmw::ReaderWriterLock")
        {
        }


        TestReaderWriterLockDriver::~TestReaderWriterLockDriver()
        {
        }

        void
        TestReaderWriterLockDriver::do_basic_tests()
        {
            TEST_INFO("Creating the ReaderWriterLock");

            ReaderWriterLock lock ;

        TEST_SUCCEED()

            ;

            TEST_INFO("Creating the value to be protected");

            int valueToBeChanged(0);

            TestReaderWriterLockBasicReaderThread testReaderWriterLockBasicReaderThread (
                lock ,
                valueToBeChanged)

                ;

            TestReaderWriterLockBasicWriterThread testReaderWriterLockBasicWriterThread (
                lock ,
                valueToBeChanged)

                ;

            TEST_INFO("Locking as a reader");

            lock.readLock();

            TEST_SUCCEED();

            TEST_INFO("Starting the Writer thread");

            testReaderWriterLockBasicWriterThread.start();

            TEST_INFO("Sleeping a while to see if the writer change the variable");

            int timescale = Cdmw::TestUtils::Testable::get_timescale();

            Thread::sleep(timescale*2000);

            TEST_CHECK(valueToBeChanged == 0);


            TEST_INFO("Starting the Reader thread");

            testReaderWriterLockBasicReaderThread.start();

            TEST_INFO("Checking if the Reader thread has not been blocked by the lock");

            testReaderWriterLockBasicReaderThread.join();

            TEST_SUCCEED();

            TEST_INFO("Unlocking the lock");

            lock.readUnlock();

            TEST_INFO("Sleeping a while to see if the writer change the variable");

            Thread::sleep(timescale*2000);

            TEST_CHECK( valueToBeChanged != 0 );

            testReaderWriterLockBasicWriterThread.join();



        }


        void
        TestReaderWriterLockDriver::do_concurrent_tests()
        {
            // The time max in each thread is 3s * NB_TEST_LOCKS
            //    as each thread sleep between 0 and 3s
            const int NB_TEST_LOCKS = 5;

            TEST_INFO("Creating the ReaderWriterLock");

            ReaderWriterLock lock ;

        TEST_SUCCEED()

            ;

            TEST_INFO("Creating the object protected by the lock");

            TestReaderWriterLock testReaderWriter(get_resultStream());

            testReaderWriter.start();

            TEST_INFO("Creating the Two writer threads");

            std::vector<Thread*> threads;

            for ( int iWriterThread = 0; iWriterThread < 50; ++iWriterThread)
            {
                threads.push_back(new TestReaderWriterLockWriterThread(

                                      lock ,
                                      testReaderWriter,
                                      NB_TEST_LOCKS) )

                    ;

            }


            for ( int iReaderThread = 0; iReaderThread < 8; ++iReaderThread)
            {
                threads.push_back(new TestReaderWriterLockReaderThread(

                                      lock ,
                                      testReaderWriter,
                                      NB_TEST_LOCKS) )

                    ;

            }



            try
            {
                TEST_INFO("Starting all threads");

                for ( size_t iThread = 0; iThread < threads.size(); ++iThread)
                {
                    threads[iThread]->start();
                }


                TEST_INFO("Waiting for the completion of all threads");

                for ( size_t iThread = 0; iThread < threads.size(); ++iThread)
                {
                    threads[iThread]->join();
                }


                testReaderWriter.join();

                for ( size_t iThread = 0; iThread < threads.size(); ++iThread)
                {
                    delete threads[iThread];
                }

            }
            catch ( const InvariantException& ex)
            {

                TEST_FAILED();
                TEST_INFO(ex.c_str());

            }
            catch (...)
            {

                TEST_FAILED();
                TEST_INFO("Unknow exception received");

            }

            TEST_SUCCEED();

        }

        void
        TestReaderWriterLockDriver::do_tests()
        {
            // set number of requested successfull tests
            set_nbOfRequestedTestOK (7);

            do_basic_tests();
            do_concurrent_tests();
        }


    } // End namespace OsSupport

} // End namespace Cdmw

