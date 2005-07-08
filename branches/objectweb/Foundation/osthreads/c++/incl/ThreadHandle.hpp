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


#ifndef CDMW_OSSUPPORT_THREAD_HANDLE_HPP
#define CDMW_OSSUPPORT_THREAD_HANDLE_HPP

#include "Foundation/common/MacroDefs.h"               // To know the current target
#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Exception.hpp"

#include "Foundation/osthreads/Thread.hpp"

#if defined(CDMW_POSIX_THREAD)

#include <pthread.h>

#elif defined (_WIN32)

#include <windows.h>

#else
#   error CDMW_IMPL_NOT_AVAILABLE
#endif



namespace Cdmw
{

    namespace OsSupport
    {

        /**
        *Purpose:
        *<p>    Implementation of the ThreadHandleBase class
        *       This class is defined as base class to map OS thread handle identification
        *       to Cdmw thread identification
        *
        *Features:
        *<p> Thread safe, exception safe
        *
        *@see  Thread
        *
        */

        class ThreadHandleBase
        {

            public:

                /**
                * Purpose:
                * <p> Construct the base class
                * 
                *@param
                *
                *@exception
                */

                ThreadHandleBase()
                throw ();

                //
                // Dtor
                //
                ~ThreadHandleBase()
                throw();



                /**
                * Purpose:
                * <p> Get the Cdmw id of thread
                *
                * @exception
                */
                Thread::ThreadId getThreadId()
                throw ();


            protected:

                /**
                * Purpose:
                * <p> Set the Cdmw id of thread (processed by derived class)
                *
                * @param thread id to store
                *
                * @exception
                */
                void setThreadId(Thread::ThreadId value)
                throw ();

            private:

                /**
                * Cdmw Thread ID
                */
                Thread::ThreadId m_threadId;
        };


#if defined(CDMW_POSIX_THREAD)

        /**
        *Purpose:
        *<p>    Implementation of the ThreadHandle class for Posix threads
        *        to map pthread handle identification to Cdmw thread identification
        *
        *Features:
        *<p> Thread safe, exception safe
        *
        *@see  Thread
        *
        */

        class ThreadHandle : public ThreadHandleBase
        {

            public:

                /**
                * Purpose:
                * <p> Construct the Posix thread handle
                * 
                *@param
                *
                *@exception 
                */
                ThreadHandle ()
                throw ();

                /**
                * Purpose:
                * <p> Construct the Posix thread handle
                * 
                *@param Posix thread identification
                *
                *@exception 
                */

                ThreadHandle (pthread_t &tid)
                throw ();

                //
                // Dtor
                //
                ~ThreadHandle()
                throw();

                /**
                * Purpose:
                * <p> Convert Posix thread identification and store Cdmw thread id
                *
                *@param Posix thread identification
                *
                *@return Cdmw Thread Id
                *
                *@exception
                */
                Thread::ThreadId setThreadId(pthread_t &tid)
                throw ();

                /**
                * Purpose:
                * <p> Convert Posix thread identification and return Cdmw thread id
                *
                *@param Posix thread identification
                *
                *@return Cdmw Thread Id
                *
                *@exception
                */
                Thread::ThreadId toThreadId(pthread_t &tid)
                throw ();

                /**
                * Purpose:
                * <p> Check if thread are equivalent
                *     return 0 if different
                *
                *@exception
                */
                int
                operator==(const ThreadHandle& rhs)
                throw();

            private:

                /**
                * Posix id of the thread
                */
                pthread_t m_pthreadId;
        };

#elif defined (_WIN32)


        /**
        *Purpose:
        *<p>    Implementation of the ThreadHandle class for Win32 threads
        *        to map thread handle identification to Cdmw thread identification
        *
        *Features:
        *<p> Thread safe, exception safe
        *
        *@see  Thread
        *
        */

        class ThreadHandle : public ThreadHandleBase
        {

            public:

                /**
                * Purpose:
                * <p> Construct the Win32 thread handle
                *
                *@param
                *
                *@exception
                */
                ThreadHandle ()
                throw ();

                /**
                * Purpose:
                * <p> Construct the Win32 thread handle
                *
                *@param Win32 thread id
                *
                *@exception
                */

                ThreadHandle (DWORD winTid)
                throw ();

                //
                // Dtor
                //
                ~ThreadHandle()
                throw();

                /**
                * Purpose:
                * <p> Convert Win32 thread id and store Cdmw thread id
                *
                *@param Win32 thread id
                *
                *@return Cdmw Thread Id
                *
                *@exception
                */
                Thread::ThreadId setThreadId(DWORD winTid)
                throw ();

                /**
                * Purpose:
                * <p> Convert Win32 thread id and return Cdmw thread id
                *
                *@param Win32 thread id
                *
                *@return Cdmw Thread Id
                *
                *@exception
                */
                Thread::ThreadId toThreadId(DWORD winTid)
                throw ();

                /**
                * Purpose:
                * <p> Check if thread are equivalent
                *     return 0 if different
                *
                *@exception
                */
                int
                operator==(const ThreadHandle& rhs)
                throw();

            private:

                /**
                * Win32 id of the thread
                */
                DWORD m_win32Id;
        };

#else
# error CDMW_IMPL_NOT_AVAILABLE
#endif

    } // End namespace OsSupport

} // End namespace Cdmw


#endif // CDMW_OSSUPPORT_THREAD_HANDLE_HPP



