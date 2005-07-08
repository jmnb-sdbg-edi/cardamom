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


#include "testosthreads/TestMutexThreadDriver.hpp"
#include "testosthreads/TestMutexThread.hpp"
#include "testosthreads/TestMutexGuardThread.hpp" 
//#include <unistd.h>

#include "Foundation/osthreads/Thread.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/NullMutex.hpp"

namespace Cdmw
{

    namespace OsSupport
    {

        TestMutexThreadDriver::TestMutexThreadDriver()
                : Testable("Cdmw::Mutex")
        {
        }


        TestMutexThreadDriver::~TestMutexThreadDriver()
        {
        }


        void
        TestMutexThreadDriver::do_test_with_NullMutex()
        {

            TEST_INFO("Creating a NullMutex");
            NullMutex mutex;

            TEST_INFO("Locking the mutex");
            mutex.lock();

            TEST_INFO("Creating a thread with the mutex as reference and a variable to be changed");
            int valueToBeChanged(0);
            TestMutexThread testMutexThread(mutex, valueToBeChanged);

            TEST_INFO("Starting the thread");
            testMutexThread.start();

            TEST_INFO("Sleeping a while to see if the thread changes the variable");
            TEST_INFO("   the thread must not be blocked by the NullMutex");
            int timescale = Cdmw::TestUtils::Testable::get_timescale();
            Thread::sleep(timescale*2000);

            TEST_CHECK( valueToBeChanged != 0);

            TEST_INFO("Unlocking the mutex");
            mutex.unlock();

        }



        void
        TestMutexThreadDriver::do_test_with_guard()
        {

            TEST_INFO("Creating a Mutex");
            Mutex mutex;

            TEST_INFO("check if the mutex is unlocked");
            TEST_CHECK(!mutex.is_locked());

            TEST_INFO("Locking the mutex");
            mutex.lock();

            TEST_INFO("check if the mutex is locked");
            TEST_CHECK(mutex.is_locked());

            TEST_INFO("Creating a thread using automatic lock/unlock of a thread");
            int valueToBeChanged(0);
            TestMutexGuardThread testMutexGuardThread(mutex, valueToBeChanged);

            TEST_INFO("Starting the thread");
            testMutexGuardThread.start();

            TEST_INFO("Sleeping a while to see if the thread changes the variable");
            int timescale = Cdmw::TestUtils::Testable::get_timescale();
            Thread::sleep(timescale*2000);

            TEST_CHECK( valueToBeChanged == 0);

            TEST_INFO("Unlocking the mutex");
            mutex.unlock();

            TEST_INFO("check if the mutex is unlocked");
            TEST_CHECK(!mutex.is_locked());

            TEST_INFO("Sleeping a while to see if the thread changes the variable");
            Thread::sleep(timescale*2000);

            TEST_CHECK( valueToBeChanged != 0);

            TEST_INFO("Waiting for the completion of the thread");
            testMutexGuardThread.join();


        }



        void
        TestMutexThreadDriver::do_test_without_guard()
        {

            TEST_INFO("Creating a Mutex");
            Mutex mutex;

            TEST_INFO("check if the mutex is unlocked");
            TEST_CHECK(!mutex.is_locked());

            TEST_INFO("Locking the mutex");
            mutex.lock();

            TEST_INFO("check if the mutex is locked");
            TEST_CHECK(mutex.is_locked());

            TEST_INFO("Creating a thread with the mutex as reference and a variable to be changed");
            int valueToBeChanged(0);
            TestMutexThread testMutexThread(mutex, valueToBeChanged);

            TEST_CHECK(testMutexThread.get_status() == THREAD_CREATED);

            TEST_INFO("Starting the thread");
            testMutexThread.start();

            TEST_INFO("Sleeping a while to wait for the thread startup");
            int timescale = Cdmw::TestUtils::Testable::get_timescale();
            Thread::sleep(timescale*2000);
            TEST_CHECK( valueToBeChanged == 0);

            TEST_INFO("Unlocking the mutex");
            mutex.unlock();

            TEST_INFO("check if the mutex is unlocked");
            TEST_CHECK(!mutex.is_locked());

            TEST_INFO("Sleeping a while to see if the thread changes the variable");

            Thread::sleep(timescale*2000);
            TEST_CHECK(testMutexThread.get_status() == THREAD_ENDED);
            TEST_CHECK( valueToBeChanged != 0);

            TEST_INFO("Waiting for the completion of the thread");
            testMutexThread.join();
        }


        void
        TestMutexThreadDriver::do_test_recursive_mutex()
        {
            TEST_INFO("Creating a non recursive Mutex");
            Mutex mutex;

            TEST_INFO("check if the mutex is unlocked");
            TEST_CHECK(!mutex.is_locked());

            TEST_INFO("Locking the mutex");
            mutex.lock();

            TEST_INFO("check if the mutex is locked");
            TEST_CHECK(mutex.is_locked());

            TEST_INFO("Trying to lock twice the same mutex");

            try
            {

                mutex.lock();

            }
            catch (Cdmw::AlreadyDoneException& )
            {

                TEST_SUCCEED();

            }
            catch (...)
            {
                TEST_FAILED();
            }


            TEST_INFO("Unlocking the mutex");
            mutex.unlock();

            TEST_INFO("check if the mutex is unlocked");
            TEST_CHECK(!mutex.is_locked());

        }

        void
        TestMutexThreadDriver::do_tests()
        {
            // set number of requested successfull tests
            set_nbOfRequestedTestOK (17);

            do_test_without_guard();
            do_test_with_guard();
            do_test_with_NullMutex();
            do_test_recursive_mutex();

        }

    } // End namespace OsSupport

} // End namespace Cdmw



