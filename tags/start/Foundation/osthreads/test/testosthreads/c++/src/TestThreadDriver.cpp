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


#include "Foundation/common/MacroDefs.h"

#include "testosthreads/TestThreadDriver.hpp"

#include "Foundation/osthreads/Thread.hpp"

#include "testosthreads/TestThread.hpp"
#include "Foundation/osthreads/ThreadObserver.hpp"

#include <sstream>

#if defined (CDMW_POSIX_THREAD)
#include <unistd.h>
#include <sched.h>
#endif

namespace Cdmw
{

    namespace OsSupport
    {



        class TestThreadObserver : public ThreadObserver
        {


            public:


                TestThreadObserver()
                        : m_enter(0),
                        m_leave(0)
                {
                }


                void
                onEnterRun()
                {
                    m_enter++;

                }


                void
                onLeaveRun()
                {
                    m_leave++;
                }


                int
                get_enter()
                {
                    return m_enter;
                }



                int
                get_leave()
                {
                    return m_leave;
                }

            private:

                int m_enter;
                int m_leave;


        };


        TestThreadDriver::TestThreadDriver()
                : Testable("Cdmw::Thread")
        {
        }

        TestThreadDriver::~TestThreadDriver()
        {
        }


        void
        TestThreadDriver::do_tests()
        {
            int nbOfRequestedTestOK = 1;
            int nbOfRequestedTestOKMemo;

            TestThreadObserver testThreadObserver;
            TEST_INFO("Registering an Thread Observer");
            Thread::register_observer (&testThreadObserver);
            TEST_SUCCEED();


            // --------------------------------------------------------------------
            // Thread creation
            // --------------------------------------------------------------------
            nbOfRequestedTestOK += 2;

            TEST_INFO(" ");
            TEST_INFO("Creating a variable with value 0");
            int valueToBeChanged(0);

            TEST_INFO("Starting a new thread with a reference at this");
            TestThread testThread_1("Thread_1", 1, valueToBeChanged, false);
            Thread::ThreadId threadId1 = testThread_1.start();

            {
                std::stringstream buffer;
                buffer << "  Thread 1 Id : " << threadId1 << std::endl;
                TEST_INFO(buffer.str());
            }

            TEST_INFO("Sleeping a while to wait the thread changes the variable");
            int timescale = Cdmw::TestUtils::Testable::get_timescale();
            Thread::sleep(timescale*2000);

            TEST_CHECK (testThread_1.get_status() == THREAD_ENDED);
            TEST_CHECK (valueToBeChanged != 0);


            // --------------------------------------------------------------------
            // Thread join
            // --------------------------------------------------------------------
            nbOfRequestedTestOK += 1;

            TEST_INFO(" ");
            TEST_INFO("Trying to wait for the thread completion");
            testThread_1.join();
            TEST_SUCCEED();


            // --------------------------------------------------------------------
            // Thread Observer
            // --------------------------------------------------------------------
            nbOfRequestedTestOK += 3;

            TEST_INFO(" ");
            TEST_INFO("Checking if the observer has been notified");
            TEST_CHECK(testThreadObserver.get_enter() == 1);
            TEST_CHECK(testThreadObserver.get_leave() == 1);

            TEST_INFO("Unregistering the Thread Observer");
            Thread::unregister_observer(&testThreadObserver);
            TEST_SUCCEED();





            // --------------------------------------------------------------------
            // Thread stack change
            // --------------------------------------------------------------------
#   if defined (CDMW_HAVE_THREAD_STACK)

            nbOfRequestedTestOK += 4;

            const size_t STACK_SIZE = 8193;

            TEST_INFO(" ");
            TEST_INFO("Trying to change stack of an already started thread");

            try
            {
                testThread_1.set_stackSize(STACK_SIZE);
                TEST_FAILED();

            }
            catch (const Cdmw::AlreadyDoneException&)
            {
                TEST_SUCCEED();

            }
            catch ( ... )
            {
                TEST_FAILED();
            }


            TEST_INFO("Trying to change stack of a new thread");
            TestThread testThread_2 ("Thread_2", 2, valueToBeChanged, false);

            try
            {
                testThread_2.set_stackSize(STACK_SIZE);
                TEST_SUCCEED();

            }
            catch ( const Cdmw::AlreadyDoneException& )
            {
                TEST_FAILED();

            }
            catch ( ... )
            {
                TEST_FAILED();
            }

            TEST_INFO("Trying to start the stack specified thread");

            try
            {
                Thread::ThreadId threadId2 = testThread_2.start();

                std::stringstream buffer;
                buffer << "  Thread 2 Id : " << threadId2 << std::endl;
                TEST_INFO(buffer.str());

                TEST_SUCCEED();

            }
            catch (...)
            {
                TEST_FAILED();
            }

            TEST_INFO("Trying to retreive the stack of a new thread");

            try
            {
                TEST_CHECK (STACK_SIZE == testThread_2.get_stackSize());
            }
            catch (...)
            {
                TEST_FAILED();
            }


            TEST_INFO("Waiting for the completion of the thread");
            testThread_2.join();
#   endif // defined( CDMW_HAVE_THREAD_STACK )


            // --------------------------------------------------------------------
            // Thread priority change
            // --------------------------------------------------------------------

            Thread::Priority thread_init_priority = 0;
            Thread::Priority thread_priority = 0;
            Thread::Priority priority = 0;

            TEST_INFO(" ");
            TEST_INFO("Trying to set the priority of a new thread");

            TestThread testThread_3 ("Thread_3", 2, valueToBeChanged, true);

#   if defined (CDMW_POSIX_THREAD)

            nbOfRequestedTestOK += 2;

            try
            {
                thread_init_priority = testThread_3.get_priority();
                priority = thread_init_priority + 1;
#       if defined (CDMW_POSIX_SOLARIS_THREAD)

                priority = (testThread_3.get_max_priority() + testThread_3.get_min_priority() ) / 2;
#       elif defined (CDMW_POSIX_AIX_THREAD)

                priority = (testThread_3.get_max_priority() + testThread_3.get_min_priority() ) / 2;
#       elif defined (CDMW_POSIX_LINUX_THREAD)

                priority = 0;
#       endif

                testThread_3.set_priority(priority);
                TEST_SUCCEED();

            }
            catch (const Cdmw::AlreadyDoneException&)
            {
                TEST_FAILED();

            }
            catch (const Cdmw::InternalErrorException & ex)
            {
                TEST_FAILED();

            }
            catch (const Cdmw::Exception& ex)
            {
                TEST_FAILED();
                TEST_INFO("Exception " << ex.what() );


            }
            catch ( ... )
            {
                TEST_FAILED();
            }

            TEST_INFO("Trying to retreive the priority of the thread");

            thread_priority = testThread_3.get_priority();

            {
                std::stringstream buffer;
                buffer << " Thread  Init Priority : " << thread_init_priority
                << " Thread  Priority      : " << thread_priority
                << " Request Priority      : " << priority << std::endl;
                TEST_INFO(buffer.str());
            }

            TEST_CHECK(thread_priority == priority);

#   endif

            // start thread

            TEST_INFO(" ");

            nbOfRequestedTestOK += 3;

            Thread::ThreadId threadId3 = testThread_3.start();

            {
                std::stringstream buffer;
                buffer << "  Thread 3 Id : " << threadId3 << std::endl;
                TEST_INFO(buffer.str());
            }

            Thread::sleep(timescale*2000);

            TEST_INFO("Trying to change priority of started thread");

            try
            {
                thread_init_priority = testThread_3.get_priority();
                priority = thread_init_priority + 1;

#       if defined (CDMW_POSIX_SOLARIS_THREAD)

                priority = (testThread_3.get_max_priority() + testThread_3.get_min_priority() ) / 2 + 2;
#       elif defined (CDMW_POSIX_AIX_THREAD)

                priority = (testThread_3.get_max_priority() + testThread_3.get_min_priority() ) / 2 + 2;
#       elif defined (CDMW_POSIX_LINUX_THREAD)

                priority = 0;
#       elif defined(_WIN32)

                priority = 1;
#       endif

                testThread_3.set_priority(priority);
                TEST_SUCCEED();

            }
            catch (const ThreadNotExistException&)
            {
                TEST_FAILED();

            }
            catch (const Cdmw::InternalErrorException & ex)
            {
                TEST_FAILED();

            }
            catch (const Cdmw::Exception& ex)
            {
                TEST_FAILED();
                TEST_INFO("Exception " << ex.what() );

            }
            catch ( ... )
            {
                TEST_FAILED();
            }

            TEST_INFO("Trying to retreive the priority of the thread");

            thread_priority = testThread_3.get_priority();

            {
                std::stringstream buffer;
                buffer << " Thread  Init Priority : " << thread_init_priority
                << " Thread  Priority      : " << thread_priority
                << " Request Priority      : " << priority << std::endl;
                TEST_INFO(buffer.str());
            }

            TEST_CHECK(thread_priority == priority);

            // stop thread
            testThread_3.stop_loop();

            TEST_INFO("Waiting for the completion of the thread");
            testThread_3.join();

            TEST_INFO("Check Thread ident get by self()");
            Thread::ThreadId threadId3_self = testThread_3.get_thr_ident();
            std::stringstream buffer;
            buffer << "  Thread 3 Id Self : " << threadId3_self << std::endl;
            TEST_INFO(buffer.str());

            TEST_CHECK(threadId3_self == threadId3);

#   if defined (CDMW_POSIX_THREAD)


            // --------------------------------------------------------------------
            // set the contention scope and scheduling policy
            // --------------------------------------------------------------------

            nbOfRequestedTestOK += 2;

            // priority is set by the thread itself
            priority = 10;

            TEST_INFO(" ");
            TEST_INFO("Trying to change scope and policy of an already started thread");

            try
            {
                testThread_1.set_scopeSchedPolicy (Thread::THR_PROCESS_SCOPE, Thread::THR_SCHEDULE_FIFO);
                TEST_FAILED();

            }
            catch (const Cdmw::AlreadyDoneException&)
            {
                TEST_SUCCEED();

            }
            catch (const Cdmw::InternalErrorException & ex)
            {
                TEST_FAILED();

            }
            catch ( ... )
            {
                TEST_FAILED();
            }


            TEST_INFO("Trying to set scope and scheduling policy of a new thread");

            TEST_INFO("PROCESS SCOPE and SCHED OTHER");
            TestThread testThread_4 ("Thread_4", 5, valueToBeChanged, true);

            try
            {
                testThread_4.set_scopeSchedPolicy (Thread::THR_PROCESS_SCOPE, Thread::THR_SCHEDULE_OTHER);
                TEST_SUCCEED();

            }
            catch (const Cdmw::InternalErrorException & ex)
            {
                TEST_FAILED();

            }
            catch (const Cdmw::BadParameterException&)
            {
#       if defined (CDMW_POSIX_LINUX_THREAD)
                // PROCESS SCOPE is not implemented on Linux
                TEST_INFO("Policy not implemented on Linux");
                TEST_SUCCEED();
#       else

                TEST_FAILED();
#       endif

            }
            catch (const Cdmw::Exception& ex)
            {
                TEST_FAILED();
                TEST_INFO("Exception " << ex.what());

            }
            catch ( ... )
            {
                TEST_FAILED();
            }

#   if !defined (CDMW_POSIX_LINUX_THREAD)



            nbOfRequestedTestOKMemo = nbOfRequestedTestOK;

            nbOfRequestedTestOK += 4;

            TEST_INFO(" ");

            try
            {
                // start thread
                TEST_INFO("Trying to start the thread");
                testThread_4.start();

                Thread::sleep(timescale*2000);

                TEST_INFO("Trying to retreive the priority of the thread");

                thread_priority = testThread_4.get_priority();

                TEST_CHECK(thread_priority == priority);

                TEST_INFO("Trying to retreive the scope and policy of the thread");
                Thread::ThrScope scope;
                Thread::ThrSchedulPolicy policy;

                testThread_4.get_scopeSchedPolicy (scope, policy);
                TEST_CHECK(scope == Thread::THR_PROCESS_SCOPE);
                TEST_CHECK(policy == Thread::THR_SCHEDULE_OTHER);
                // stop thread
                testThread_4.stop_loop();

                testThread_4.join();
                TEST_SUCCEED();

            }
            catch (const Cdmw::InternalErrorException & ex)
            {
                TEST_FAILED();

            }
            catch (const Cdmw::Exception& ex)
            {
                TEST_FAILED();
                TEST_INFO("Exception " << ex.what());

            }
            catch ( ... )
            {
                TEST_FAILED();
            }




#   endif

            nbOfRequestedTestOK += 1;

            TEST_INFO(" ");

            TEST_INFO("PROCESS SCOPE and SCHED FIFO");

            TestThread testThread_5 ("Thread_5", 5, valueToBeChanged, true);

            try
            {
                testThread_5.set_scopeSchedPolicy (Thread::THR_PROCESS_SCOPE, Thread::THR_SCHEDULE_FIFO);

                TEST_SUCCEED();

            }
            catch (const Cdmw::InternalErrorException & ex)
            {
                TEST_FAILED();

            }
            catch (const Cdmw::BadParameterException&)
            {
#       if defined (CDMW_POSIX_LINUX_THREAD)
                // PROCESS SCOPE is not implemented on Linux
                TEST_INFO("Policy not implemented on Linux");
                TEST_SUCCEED();
#       else

                TEST_FAILED();
#       endif

            }
            catch (const Cdmw::Exception& ex)
            {
                TEST_FAILED();
                TEST_INFO("Exception " << ex.what());

            }
            catch ( ... )
            {
                TEST_FAILED();
            }

#   if !defined (CDMW_POSIX_LINUX_THREAD)

            nbOfRequestedTestOK += 2;

            TEST_INFO(" ");

            try
            {
                // start thread
                TEST_INFO("Trying to start the thread");
                testThread_5.start();

                Thread::sleep(timescale*2000);

                TEST_INFO("Trying to retreive the priority of the thread");

                thread_priority = testThread_5.get_priority();

                TEST_CHECK(thread_priority == priority);

                // stop thread
                testThread_5.stop_loop();

                testThread_5.join();
                TEST_SUCCEED();


            }
            catch (const Cdmw::InternalErrorException & ex)
            {
                TEST_FAILED();

            }
            catch (const Cdmw::Exception& ex)
            {
                TEST_FAILED();
                TEST_INFO("Exception " << ex.what());

            }
            catch ( ... )
            {
                TEST_FAILED();
            }

#   endif

            nbOfRequestedTestOK += 1;

            TEST_INFO(" ");

            TEST_INFO("PROCESS SCOPE and SCHED RR");

            TestThread testThread_6 ("Thread_6", 5, valueToBeChanged, true);

            try
            {
                testThread_6.set_scopeSchedPolicy (Thread::THR_PROCESS_SCOPE, Thread::THR_SCHEDULE_RR);

                TEST_SUCCEED();

            }
            catch (const Cdmw::InternalErrorException & ex)
            {
                TEST_FAILED();

            }
            catch (const Cdmw::BadParameterException&)
            {
#       if defined (CDMW_POSIX_LINUX_THREAD)
                // PROCESS SCOPE is not implemented on Linux
                TEST_INFO("Policy not implemented on Linux");
                TEST_SUCCEED();
#       else

                TEST_FAILED();
#       endif

            }
            catch (const Cdmw::Exception& ex)
            {
                TEST_FAILED();
                TEST_INFO("Exception " << ex.what());

            }
            catch ( ... )
            {
                TEST_FAILED();
            }

#   if !defined (CDMW_POSIX_LINUX_THREAD)

            nbOfRequestedTestOK += 2;

            TEST_INFO(" ");

            try
            {
                // start thread
                TEST_INFO("Trying to start the thread");
                testThread_6.start();

                Thread::sleep(timescale*2000);

                TEST_INFO("Trying to retreive the priority of the thread");

                thread_priority = testThread_6.get_priority();

                TEST_CHECK(thread_priority == priority);

                // stop thread
                testThread_6.stop_loop();

                testThread_6.join();
                TEST_SUCCEED();

            }
            catch (const Cdmw::InternalErrorException & ex)
            {
                TEST_FAILED();

            }
            catch (const Cdmw::Exception& ex)
            {
                TEST_FAILED();
                TEST_INFO("Exception " << ex.what());

            }
            catch ( ... )
            {
                TEST_FAILED();
            }

#   endif


            nbOfRequestedTestOK += 1;

            TEST_INFO(" ");

            TEST_INFO("SYSTEM SCOPE and SCHED OTHER");

            TestThread testThread_7 ("Thread_7", 5, valueToBeChanged, true);

            try
            {
                testThread_7.set_scopeSchedPolicy (Thread::THR_SYSTEM_SCOPE, Thread::THR_SCHEDULE_OTHER);

                int max_priority = ::sched_get_priority_max(SCHED_OTHER);
                int min_priority = ::sched_get_priority_min(SCHED_OTHER);
                std::stringstream buffer;
                buffer << "euid : " << ::geteuid()
                << " Max Priority : " << max_priority
                << " Min Priority : " << min_priority
                << " Max Cdmw Priority : " << testThread_7.get_max_priority()
                << " Min Cdmw Priority : " << testThread_7.get_min_priority()
                << std::endl;
                TEST_INFO(buffer.str());

                TEST_SUCCEED();

            }
            catch (const Cdmw::InternalErrorException & ex)
            {
                TEST_FAILED();

            }
            catch (const Cdmw::Exception& ex)
            {
                TEST_FAILED();
                TEST_INFO("Exception " << ex.what());

            }
            catch ( ... )
            {
                TEST_FAILED();
            }


            nbOfRequestedTestOKMemo = nbOfRequestedTestOK;
            nbOfRequestedTestOK += 4;

            try
            {
                // start thread
                TEST_INFO("Trying to start the thread");
                testThread_7.start();

                Thread::sleep(timescale*2000);

                TEST_INFO("Trying to retreive the priority of the thread");

#       if defined (CDMW_POSIX_SOLARIS_THREAD)

                priority = 10;
#       elif defined (CDMW_POSIX_AIX_THREAD)

                priority = 10;
#       elif defined (CDMW_POSIX_LINUX_THREAD)

                priority = 0;
#       endif

                thread_priority = testThread_7.get_priority();

                TEST_CHECK(thread_priority == priority);

                TEST_INFO("Trying to retreive the scope and policy of the thread");
                Thread::ThrScope scope;
                Thread::ThrSchedulPolicy policy;

                testThread_7.get_scopeSchedPolicy (scope, policy);
                TEST_CHECK(scope == Thread::THR_SYSTEM_SCOPE);
                TEST_CHECK(policy == Thread::THR_SCHEDULE_OTHER);

                // stop thread
                testThread_7.stop_loop();

                testThread_7.join();
                TEST_SUCCEED();

            }
            catch (const Cdmw::InternalErrorException & ex)
            {
                TEST_FAILED();

            }
            catch (const Cdmw::Exception& ex)
            {
                TEST_FAILED();
                TEST_INFO("Exception " << ex.what());

            }
            catch ( ... )
            {
                TEST_FAILED();
            }


            nbOfRequestedTestOK += 1;

            TEST_INFO(" ");
            TEST_INFO("SYSTEM SCOPE and SCHED FIFO");
            TestThread testThread_8 ("Thread_8", 10, valueToBeChanged, true);

            try
            {
                testThread_8.set_scopeSchedPolicy (Thread::THR_SYSTEM_SCOPE, Thread::THR_SCHEDULE_FIFO);

                int max_priority = ::sched_get_priority_max(SCHED_FIFO);
                int min_priority = ::sched_get_priority_min(SCHED_FIFO);
                std::stringstream buffer;
                buffer << "euid : " << ::geteuid()
                << " Max Priority : " << max_priority
                << " Min Priority : " << min_priority
                << " Max Cdmw Priority : " << testThread_8.get_max_priority()
                << " Min Cdmw Priority : " << testThread_8.get_min_priority()
                << std::endl;
                TEST_INFO(buffer.str());

#       if defined (CDMW_POSIX_LINUX_THREAD)

                TEST_INFO("Trying to change priority of created thread");
                priority = 1;
                testThread_8.set_priority(priority);

#       endif

                TEST_SUCCEED();

            }
            catch (const Cdmw::InternalErrorException & ex)
            {
                TEST_FAILED();

            }
            catch (const Cdmw::Exception& ex)
            {
                TEST_FAILED();
                TEST_INFO("Exception " << ex.what());

            }
            catch ( ... )
            {
                TEST_FAILED();
            }

            nbOfRequestedTestOKMemo = nbOfRequestedTestOK;
            nbOfRequestedTestOK += 2;

            try
            {
                // start thread
                TEST_INFO("Trying to start the thread");
                testThread_8.start();

                Thread::sleep(timescale*2000);

                TEST_INFO("Trying to change priority of started thread");
                priority = testThread_8.get_priority();
                priority ++;
                testThread_8.set_priority(priority);

                TEST_INFO("Trying to retreive the priority of the thread");
                TEST_CHECK(testThread_8.get_priority() == priority);

                // stop thread
                testThread_8.stop_loop();

                testThread_8.join();
                TEST_SUCCEED();

            }
            catch (const ThreadSchedulingException& ex)
            {

#   if defined (CDMW_POSIX_SOLARIS_THREAD)

                nbOfRequestedTestOK = nbOfRequestedTestOKMemo;
                nbOfRequestedTestOK += 1;

                // SYSTEM SCOPE and SCHED FIFO is not permitted on Posix Solaris (must be superuser)
                TEST_INFO("Policy not permitted on Posix Solaris as user");
                TEST_INFO("Exception " << ex.what());
                TEST_SUCCEED();
#   elif defined (CDMW_POSIX_LINUX_THREAD)

                nbOfRequestedTestOK = nbOfRequestedTestOKMemo;
                nbOfRequestedTestOK += 1;

                // SYSTEM SCOPE and SCHED FIFO is not permitted on Linux (must be superuser)
                TEST_INFO("Policy not permitted on Linux as user");
                TEST_INFO("Exception " << ex.what());
                TEST_SUCCEED();
#   elif defined (CDMW_POSIX_AIX_THREAD)

                nbOfRequestedTestOK = nbOfRequestedTestOKMemo;
                nbOfRequestedTestOK += 1;

                // SYSTEM SCOPE and SCHED FIFO is not permitted on AIX (must be superuser)
                TEST_INFO("Policy not permitted on AIX as user");
                TEST_INFO("Exception " << ex.what());
                TEST_SUCCEED();
#   else

                TEST_FAILED();
#   endif

            }
            catch (const Cdmw::InternalErrorException & ex)
            {
                TEST_FAILED();

            }
            catch (const Cdmw::Exception& ex)
            {
                TEST_FAILED();
                TEST_INFO("Exception " << ex.what());

            }
            catch ( ... )
            {
                TEST_FAILED();
            }


            nbOfRequestedTestOK += 1;

            TEST_INFO(" ");
            TEST_INFO("SYSTEM SCOPE and SCHED RR");
            TestThread testThread_9 ("Thread_9", 10, valueToBeChanged, true);

            try
            {
                testThread_9.set_scopeSchedPolicy (Thread::THR_SYSTEM_SCOPE, Thread::THR_SCHEDULE_RR);

                int max_priority = ::sched_get_priority_max(SCHED_RR);
                int min_priority = ::sched_get_priority_min(SCHED_RR);
                std::stringstream buffer;
                buffer << "euid : " << ::geteuid()
                << " Max Priority : " << max_priority
                << " Min Priority : " << min_priority
                << " Max Cdmw Priority : " << testThread_9.get_max_priority()
                << " Min Cdmw Priority : " << testThread_9.get_min_priority()
                << std::endl;

                TEST_INFO(buffer.str());

#       if defined (CDMW_POSIX_LINUX_THREAD)

                TEST_INFO("Trying to change priority of created thread");
                priority = 1;
                testThread_9.set_priority(priority);

#       endif

                TEST_SUCCEED();

            }
            catch (const Cdmw::InternalErrorException & ex)
            {
                TEST_FAILED();

            }
            catch (const Cdmw::Exception& ex)
            {
                TEST_FAILED();
                TEST_INFO("Exception " << ex.what());

            }
            catch ( ... )
            {
                TEST_FAILED();
            }


            nbOfRequestedTestOKMemo = nbOfRequestedTestOK;
            nbOfRequestedTestOK += 2;

            try
            {
                // start thread
                TEST_INFO("Trying to start the thread");
                testThread_9.start();

                Thread::sleep(timescale*2000);

                TEST_INFO("Trying to change priority of started thread");
                priority = testThread_9.get_priority();
                priority ++;
                testThread_9.set_priority(priority);

                TEST_INFO("Trying to retreive the priority of the thread");
                TEST_CHECK(testThread_9.get_priority() == priority);

                // stop thread
                testThread_9.stop_loop();

                testThread_9.join();
                TEST_SUCCEED();

            }
            catch (const Cdmw::InternalErrorException & ex)
            {
                TEST_FAILED();

            }
            catch (const ThreadSchedulingException& ex)
            {

#   if defined (CDMW_POSIX_SOLARIS_THREAD)

                nbOfRequestedTestOK = nbOfRequestedTestOKMemo;
                nbOfRequestedTestOK += 1;

                // SYSTEM SCOPE and SCHED RR is not permitted on Posix Solaris (must be superuser)
                TEST_INFO("Policy not permitted on Posix Solaris as user");
                TEST_INFO("Exception " << ex.what());
                TEST_SUCCEED();
#   elif defined (CDMW_POSIX_LINUX_THREAD)

                nbOfRequestedTestOK = nbOfRequestedTestOKMemo;
                nbOfRequestedTestOK += 1;

                // SYSTEM SCOPE and SCHED RR is not permitted on Linux (must be superuser)
                TEST_INFO("Policy not permitted on Linux as user");
                TEST_INFO("Exception " << ex.what());
                TEST_SUCCEED();
#   elif defined (CDMW_POSIX_AIX_THREAD)

                nbOfRequestedTestOK = nbOfRequestedTestOKMemo;
                nbOfRequestedTestOK += 1;

                // SYSTEM SCOPE and SCHED RR is not permitted on AIX (must be superuser)
                TEST_INFO("Policy not permitted on AIX as user");
                TEST_INFO("Exception " << ex.what());
                TEST_SUCCEED();
#   else

                TEST_FAILED();
#   endif

            }
            catch (const Cdmw::Exception& ex)
            {
                TEST_FAILED();
                TEST_INFO("Exception " << ex.what());

            }
            catch ( ... )
            {
                TEST_FAILED();
            }

#   endif

            // set number of requested successfull tests
            set_nbOfRequestedTestOK (nbOfRequestedTestOK);

        }

    } // End namespace OsSupport
} // End namespace Cdmw



