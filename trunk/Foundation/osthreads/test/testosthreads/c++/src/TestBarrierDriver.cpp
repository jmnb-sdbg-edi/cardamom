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


#include "testosthreads/TestBarrierDriver.hpp"

#include "Foundation/osthreads/Thread.hpp"
#include "Foundation/osthreads/Barrier.hpp"

#include "testosthreads/TestBarrierThread.hpp"


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


        TestBarrierDriver::TestBarrierDriver()
            : Testable("Cdmw::Barrier"),
              m_valid_only(false)
        {
        }

        TestBarrierDriver::TestBarrierDriver(bool valid_only)
            : Testable("Cdmw::Barrier"),
              m_valid_only(valid_only)
        {
        }


        TestBarrierDriver::~TestBarrierDriver()
        throw()
        {
        }

        void
        TestBarrierDriver::do_timed_out_tests()
        {
            TEST_INFO("Creating a barrier for 3 threads");
            Barrier barrier(3);

            TEST_INFO("Creating 1 thread waiting on the barrier");
            TEST_INFO("   with a value to be changed ");
            int valueToBeChanged_1(0);
            TestBarrierThread testBarrierThread_1( barrier, valueToBeChanged_1);


            TEST_INFO("Starting the thread");
            testBarrierThread_1.start();

            TEST_INFO("Waiting on the Barrier with a timeout");
            int timescale = Cdmw::TestUtils::Testable::get_timescale();
            Barrier::WaitReturn status = barrier.wait(timescale * 500);
            TEST_CHECK( status == Barrier::TIMEDOUT );

            TEST_INFO("Creating a second thread waiting on the barrier");
            TEST_INFO("   with a value to be changed ");
            int valueToBeChanged_2(0);
            TestBarrierThread testBarrierThread_2( barrier, valueToBeChanged_2);
            TEST_INFO("Starting the thread");
            testBarrierThread_2.start();

            TEST_INFO("Sleeping a while to see if all thread have gone across the Barrier");

            Thread::sleep(timescale*2000);
            TEST_CHECK( valueToBeChanged_1 == 999 && valueToBeChanged_2 == 999 );


            testBarrierThread_1.join();
            testBarrierThread_2.join();

        }

        void
        TestBarrierDriver::do_basic_tests()
        {

            TEST_INFO("Creating a barrier for 3 threads");
            Barrier barrier(3);

            TEST_INFO("Creating 2 threads waiting on the barrier");
            TEST_INFO("   with a value to be changed for each one");
            int valueToBeChanged_1(0);
            int valueToBeChanged_2(0);
            TestBarrierThread testBarrierThread_1( barrier, valueToBeChanged_1);
            TestBarrierThread testBarrierThread_2( barrier, valueToBeChanged_2);

            TEST_INFO("Starting all threads");
            testBarrierThread_1.start();
            testBarrierThread_2.start();

            TEST_INFO("Sleeping a while to see if threads are blocked by the Barrier");
            int timescale = Cdmw::TestUtils::Testable::get_timescale();
            Thread::sleep(timescale*2000);
            TEST_CHECK( valueToBeChanged_1 == 0 && valueToBeChanged_2 == 0 );

            TEST_INFO("Waiting on the Barrier and see if we unblocks the Barrier");
            barrier.wait();
            TEST_SUCCEED();

            TEST_INFO("Sleeping a while to see if all thread have gone across the Barrier");
            Thread::sleep(timescale*2000);
            TEST_CHECK( valueToBeChanged_1 == 999 && valueToBeChanged_2 == 999 );

            testBarrierThread_1.join();
            testBarrierThread_2.join();

        }


        void
        TestBarrierDriver::do_tests()
        {
            // set number of requested successfull tests
            if (m_valid_only) set_nbOfRequestedTestOK (3);
            else set_nbOfRequestedTestOK (5);

            do_basic_tests();
            if (!m_valid_only)
            {
              do_timed_out_tests();
            }
        }


    } // End namespace OsSupport
} // End namespace Cdmw

