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


#include "Foundation/osthreads/ThreadHandle.hpp"


namespace Cdmw
{

    namespace OsSupport
    {

        // Thread Handle base class to be derived according to OS

        //
        // Ctor
        //
        ThreadHandleBase::ThreadHandleBase()
        throw ()
                : m_threadId (0)
        {}

        //
        // Dtor
        //
        ThreadHandleBase::~ThreadHandleBase()
        throw ()
        {}

        //
        // Get the thread Cdmw id
        //
        Thread::ThreadId ThreadHandleBase::getThreadId()
        throw ()
        {
            return m_threadId;
        }

        //
        // Set the thread Cdmw id
        //
        void ThreadHandleBase::setThreadId(Thread::ThreadId value)
        throw ()
        {
            m_threadId = value;
        }







#if defined(CDMW_POSIX_THREAD)

        // Thread Handle class for POSIX thread

        //
        // Ctor
        //
        ThreadHandle::ThreadHandle ()
        throw ()
        {}

        //
        // Ctor with pthread posix identification
        //
        ThreadHandle::ThreadHandle (pthread_t &tid)
        throw ()
        {
            setThreadId(tid);
        }

        //
        // Dtor
        //
        ThreadHandle::~ThreadHandle ()
        throw ()
        {}

        //
        // Convert Posix thread identification
        // and store Cdmw thread id
        //
        Thread::ThreadId ThreadHandle::setThreadId (pthread_t &tid)
        throw ()
        {
            m_pthreadId = tid;

            Thread::ThreadId id = toThreadId(tid);
            ThreadHandleBase::setThreadId (id);

            return id;
        }

        //
        // Convert from Posix thread identification to Cdmw thread id
        //
        Thread::ThreadId ThreadHandle::toThreadId (pthread_t &tid)
        throw ()
        {
            Thread::ThreadId id;

            id = tid;
            return id;
        }

        // Check if Posix thread are equivalent
        // return 0 if different

        int
        ThreadHandle::operator==(const ThreadHandle& rhs)
        throw()
        {
            int result = 0;

            result = pthread_equal(m_pthreadId, rhs.m_pthreadId);

            return result;
        }

#elif defined (_WIN32)


        // Thread Handle class for Win32 thread

        //
        // Ctor
        //
        ThreadHandle::ThreadHandle ()
        throw ()
        {}

        //
        // Ctor with Win32 thread id
        //
        ThreadHandle::ThreadHandle (DWORD winTid)
        throw ()
        {
            setThreadId(winTid);
        }

        //
        // Dtor
        //
        ThreadHandle::~ThreadHandle ()
        throw ()
        {}

        //
        // Convert Win32 thread identification
        // and store Cdmw thread id
        //
        Thread::ThreadId ThreadHandle::setThreadId (DWORD winTid)
        throw ()
        {
            m_win32Id = winTid;

            Thread::ThreadId id = toThreadId(winTid);

            ThreadHandleBase::setThreadId (id);

            return id;
        }

        //
        // Convert from Win32 thread identification to Cdmw thread id
        //
        Thread::ThreadId ThreadHandle::toThreadId (DWORD winTid)
        throw ()
        {
            Thread::ThreadId id;

            id = winTid;
            return id;
        }


        // Check if win32 thread are equivalent
        // return 0 if different

        int
        ThreadHandle::operator==(const ThreadHandle& rhs)
        throw()
        {
            int result = 0;

            if (m_win32Id == rhs.m_win32Id)
            {
                result = 1;
            }

            return result;
        }

#else
#   error "Cdmw: Implementation not available"
#endif

    } // End namespace osthreads

}  // End namespace Cdmw



