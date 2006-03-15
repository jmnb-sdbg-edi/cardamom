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


#include "testosthreads/TestReaderWriterLockReaderThread.hpp"

#include <stdlib.h>

#include "Foundation/testutils/Testable.hpp"
#include "Foundation/osthreads/Thread.hpp"
#include "Foundation/osthreads/ReaderWriterLock.hpp"
#include "Foundation/osthreads/WriterLockGuard.hpp"

#include "testosthreads/TestReaderWriterLock.hpp"

namespace Cdmw
{

    namespace OsSupport
    {

        TestReaderWriterLockReaderThread::TestReaderWriterLockReaderThread(

            ReaderWriterLock& lock ,
            TestReaderWriterLock& test,
            int nbLocks)
                    : m_lock(lock )
                    ,
                    m_test(test),
                    m_nbLocks(nbLocks),
                    m_seed(reinterpret_cast<unsigned int>(this))
        {
        }


        TestReaderWriterLockReaderThread::~TestReaderWriterLockReaderThread()

        throw()
        {
        }

        void
        TestReaderWriterLockReaderThread::run()
        throw()
        {

            for ( int iCount = 0; iCount < m_nbLocks; ++iCount )
            {
                m_lock.readLock();
                int timescale = Cdmw::TestUtils::Testable::get_timescale();
                m_test.increment_reader();
                Thread::sleep(timescale*get_sleepTime());
                m_test.decrement_reader();

                m_lock.readUnlock();

                Thread::sleep(timescale*get_sleepTime());

            }


        }


        unsigned int
        TestReaderWriterLockReaderThread::get_sleepTime()
        {
            int rand_value;

#   ifdef CDMW_POSIX_THREAD

            // We return a value between 0 and 3 seconds
            rand_value = ::rand_r(&m_seed);

            // Under Linux, the rand_r seems returns beetwen -RAND_MAX, and +RAND_MAX
#   ifdef linux

            if ( rand_value < 0 )
            {
                rand_value = -rand_value;
            }

#   endif
#   elif defined(_WIN32)
            rand_value = rand();

#   endif


            unsigned int sleep_value( static_cast<unsigned int> (
                                          double(3) * rand_value / double(RAND_MAX) ));

            return sleep_value;

        }

    } // End namespace OsSupport

} // End namespace Cdmw

