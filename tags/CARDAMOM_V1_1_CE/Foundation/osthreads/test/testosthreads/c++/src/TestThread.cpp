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


#include <iostream>

#include "Foundation/common/MacroDefs.h"
#include "Foundation/testutils/Testable.hpp"
#include "Foundation/osthreads/Thread.hpp"
#include "testosthreads/TestThread.hpp"


namespace Cdmw
{

    namespace OsSupport
    {


        //
        // Ctor
        //
        TestThread::TestThread (const std::string& name,
                                int sleep_value,
                                int& value_to_change,
                                bool loop_set)
                : m_name(name),
                m_sleep_value(sleep_value),
                m_value_to_change(value_to_change)
        {
            if (loop_set)
            {
                // set loop to start
                m_loop_status = 1;
            }
            else
            {
                // set loop inactive
                m_loop_status = 0;
            }
        }


        //
        // Dtor
        //
        TestThread::~TestThread() throw()
        {
            // Nothing to do
        }

        //
        // Stop the started loop
        //
        void TestThread::stop_loop()
        {
            if (m_loop_status != 0)
            {
                // set loop to stop
                m_loop_status = 3;
            }
        }

        //
        // Get thread self identification
        //
        Thread::ThreadId TestThread::get_thr_ident()
        {
            return m_thr_ident;
        }

        //
        // Method called in a new thread
        //
        void TestThread::run() throw ()
        {
            // get thread ident
            m_thr_ident = this->self();
            int timescale = Cdmw::TestUtils::Testable::get_timescale();
            Thread::sleep(timescale*m_sleep_value);
            m_value_to_change = 999;

            // if loop to start

            if (m_loop_status == 1)
            {
                // set loop started
                m_loop_status = 2;

#   if defined (CDMW_POSIX_SOLARIS_THREAD)

                // change the priority
                this->set_priority(10);

#   endif
#   if defined (CDMW_POSIX_AIX_THREAD)

                // change the priority
                this->set_priority(10);

#   endif

                // no loop after a maximum of 1 minute (m_sleep_value in msec)
                int max_loop = 60000 / m_sleep_value;
                int loop_counter = 0;

                // while loop is not requested to stop

                while (m_loop_status != 3 && loop_counter < max_loop)
                {
                    if (loop_counter == 0)
                    {
                        std::cout << "Thread " << self() << " is running" << std::endl;
                    }

                    Thread::sleep(timescale*m_sleep_value);

                    loop_counter++;
                }

                // yield thread before stopping
                this->yield();

                // still loop twice
                for (int i = 0 ; i < 2 ; i++)
                {
                    Thread::sleep(timescale*m_sleep_value);
                }

                std::cout << "Thread " << self() << " is stopping" << std::endl;
            }
        }


    } // End namespace OsSupport
} // End namespace Cdmw


