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


#include "testosthreads/TestConditionDriver.hpp"

#include "Foundation/osthreads/Thread.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Condition.hpp"

#include "testosthreads/TestConditionThread.hpp"
#include "testosthreads/TestConditionBroadcastThread.hpp"



namespace Cdmw
{

    namespace OsSupport
    {

        /**
        *Implementation notes: [if no pertinent write none ]
        *<p>
        *Portability issues: [if no pertinent write none ]
        *<p>
        */


        TestConditionDriver::TestConditionDriver()
                : Testable("Cdmw::Condition")
        {
        }


        TestConditionDriver::~TestConditionDriver()
        throw()
        {
        }

        // The signal() function is not provided, see discussion
        // in ConditionImpl.hpp
        //
#if 0
        void
        TestConditionDriver::do_tests_signal()
        {
            TEST_INFO("Creating a Mutex");
            Mutex mutex;

            TEST_INFO("Creating a Condition associated with the previous Mutex");
            Condition condition(mutex);
            TEST_SUCCEED();

            TEST_INFO("Creating 2 values protected by the condition to be changed by the threads");
            int valueToBeChanged_1(0);
            int valueToBeChanged_2(0);

            TEST_INFO("Creating the threads with value and condition as parameters");
            TestConditionThread testConditionThread_1(condition, valueToBeChanged_1);
            TestConditionThread testConditionThread_2(condition, valueToBeChanged_2);

            TEST_INFO("Starting the threads");
            testConditionThread_1.start();
            testConditionThread_2.start();

            TEST_INFO("Locking the mutex and changing both values");
            mutex.lock();
            valueToBeChanged_1 = 1;
            valueToBeChanged_2 = 2;
            mutex.unlock();

            TEST_INFO("Signaling the condition to notify the value change");
            condition.signal();


            TEST_INFO("Sleeping a while to see if only one thread changes its value");
            int timescale = Cdmw::TestUtils::Testable::get_timescale();
            Thread::sleep(timescale*2000);
            TEST_CHECK( ! (valueToBeChanged_1 == 999 && valueToBeChanged_2 == 999 ) );


            TEST_INFO("Signaling again the condition for the second thread");
            condition.signal();

            TEST_INFO("Sleeping a while to see if the second thread changes its value");
            Thread::sleep(timescale*2000);
            TEST_CHECK( ( valueToBeChanged_1 == 999 && valueToBeChanged_2 == 999 ) );

            testConditionThread_1.join();
            testConditionThread_2.join();

        }

#endif // 0


        void
        TestConditionDriver::do_tests_broadcast()
        {
            TEST_INFO("Creating a Mutex");
            Mutex mutex;

            TEST_INFO("Creating a Condition associated with the previous Mutex");
            Condition condition(mutex);
            TEST_SUCCEED();

            TEST_INFO("Creating 2 values protected by the condition to be changed by the threads");
            int valueToBeChanged_1(0);
            int valueToBeChanged_2(0);

            TEST_INFO("Creating the threads with value and condition as parameters");
            TestConditionThread testConditionThread_1(condition, valueToBeChanged_1);
            TestConditionThread testConditionThread_2(condition, valueToBeChanged_2);

            TEST_INFO("Starting the threads");
            testConditionThread_1.start();
            testConditionThread_2.start();

            TEST_INFO("Locking the mutex and changing both values");
            mutex.lock();
            valueToBeChanged_1 = 1;
            valueToBeChanged_2 = 2;
            mutex.unlock();

            TEST_INFO("Signaling the condition to notify the value change");
            condition.broadcast();


            TEST_INFO("Sleeping a while to see if all threads have changed their value");
            int timescale = Cdmw::TestUtils::Testable::get_timescale();
            Thread::sleep(timescale*2000);
            TEST_CHECK( valueToBeChanged_1 == 999 && valueToBeChanged_2 == 999 );

            testConditionThread_1.join();
            testConditionThread_2.join();
        }



        void
        TestConditionDriver::do_tests_timed_wait()
        {
            int timescale = Cdmw::TestUtils::Testable::get_timescale();
            TEST_INFO("Creating a Mutex");
            Mutex mutex;

            TEST_INFO("Creating a Condition associated with the previous Mutex");
            Condition condition(mutex);
            TEST_SUCCEED();

            TEST_INFO("Creating the thread with Condition as parameter");
            TestConditionBroadcastThread testConditionBroadcastThread( condition );

            TEST_INFO("Starting the thread");
            testConditionBroadcastThread.start();

            TEST_INFO("Locking the associated mutex");
            mutex.lock();

            TEST_INFO("Waiting to be signaled on a condition with timeout");
            Condition::WaitReturn status = condition.wait(timescale * 5000);

            TEST_CHECK( status == Condition::SIGNALLED );
            TEST_INFO("Unlocking the associated mutex");
            mutex.unlock();
            testConditionBroadcastThread.join();


            TEST_INFO("Locking the associated mutex");
            mutex.lock();

            TEST_INFO("Creating a new thread with Condition as parameter");
            TestConditionBroadcastThread testConditionBroadcastThread_2( condition );
            testConditionBroadcastThread_2.start();

            TEST_INFO("Waiting to be signaled on a condition whose timeout");
            TEST_INFO("   is less than the delay to be signalled");
            status = condition.wait(timescale * 500);

            TEST_CHECK( status == Condition::TIMEDOUT );
            TEST_INFO("Unlocking the associated mutex");
            mutex.unlock();

            testConditionBroadcastThread_2.join();

        }




        void
        TestConditionDriver::do_basic_tests()
        {
            TEST_INFO("Creating a Mutex");
            Mutex mutex;

            TEST_INFO("Creating a Condition associated with the previous Mutex");
            Condition condition(mutex);
            TEST_SUCCEED();

            TEST_INFO("Creating a value protected by the condition to be changed by a thread");
            int valueToBeChanged(0);

            TEST_INFO("Creating the thread with value and condition as parameters");
            TestConditionThread testConditionThread(condition, valueToBeChanged);

            TEST_INFO("Starting the thread");
            testConditionThread.start();

            TEST_INFO("Sleeping a while to see if the thread changes the value");
            int timescale = Cdmw::TestUtils::Testable::get_timescale();
            Thread::sleep(timescale*2000);
            TEST_CHECK( valueToBeChanged == 0 );

            TEST_INFO("Signalling the condition without changing the value");
            TEST_INFO("    the thread must check and wait again.");
            condition.broadcast();

            TEST_INFO("Sleeping a while to see if the thread changes the value");
            Thread::sleep(timescale*2000);

            TEST_INFO("Checking if the value has not changed");
            mutex.lock();
            TEST_CHECK( valueToBeChanged == 0 );
            mutex.unlock();


            TEST_INFO("Locking the mutex and changing the value");
            mutex.lock();
            valueToBeChanged = 1;
            mutex.unlock();

            TEST_INFO("Signalling the condition to notify the value change");
            condition.broadcast();

            TEST_INFO("Sleeping a while to see if the thread changes the value");
            Thread::sleep(timescale*2000);
            TEST_CHECK( valueToBeChanged == 999 );


            testConditionThread.join();

        }

        void
        TestConditionDriver::do_tests()
        {
            // set number of requested successfull tests
            set_nbOfRequestedTestOK (9);

            do_basic_tests();
            do_tests_broadcast();
            do_tests_timed_wait();
        }

    } // End namespace osthreads
} // End namespace Cdmw

