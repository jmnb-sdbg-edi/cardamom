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


#ifndef CDMW_OSSUPPORT_THREAD_IMPL_HPP
#define CDMW_OSSUPPORT_THREAD_IMPL_HPP

#include "Foundation/common/MacroDefs.h"               // To know the current target

#if defined(CDMW_POSIX_THREAD)

#   include <pthread.h>

#elif defined (_WIN32)

#   include <windows.h>

#else
#   error CDMW_IMPL_NOT_AVAILABLE
#endif

#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Thread.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"

#include "osthreads/ThreadPriority.hpp"

namespace Cdmw
{

    namespace OsSupport
    {

        /**
        * Functions called by the operating system in a new thread,
        * this methods calls the run() method of the implementation
        * in a new thread.
        */

#if defined(CDMW_POSIX_THREAD)

        extern "C"
            void*
            thread_posix_thread_entry_point (void* pThreadData)
            throw();

#elif defined (_WIN32)

        DWORD WINAPI
        thread_win32_thread_entry_point (LPVOID pThreadData)
        throw();

#endif



        /**
        *Purpose:
        *<p>    Implementation of the Thread class with
        *        compilation firewall idiom (pimpl)
        *
        *Features:
        *<p> Thread safe, exception safe
        *
        *@see  Thread
        *@see  Mutex
        *
        */

        class ThreadImpl
        {

            public:

                /**
                * Purpose:
                * <p> Construct the implementation link to the handle
                * 
                *@param handle of thread
                *
                *@exception OutOfMemoryException  Not enough memory to satisfy the initialisation
                *@exception InternalErrorException  one of the m_pthread_attr argument is invalid
                *@exception BadParameterException  
                */
                explicit
                ThreadImpl(Thread* pThread)
                throw (
                    OutOfMemoryException,
                    InternalErrorException,
                    BadParameterException);

                //
                // Dtor
                //
                ~ThreadImpl()
                throw();



                /**
                * Purpose:
                * <p> Start the new thread and returns its id.
                *
                * @exception OutOfMemoryException Not enough memory to satisfy the initialisation
                * @exception ThreadSchedulingException The caller is not
                *            allowed to start a thread from this priority
                * @exception InternalErrorException one of the m_pthread_attr argument is invalid       
                */
                Thread::ThreadId
                start()
                throw (
                    OutOfMemoryException,
                    ThreadSchedulingException,
                    InternalErrorException);


                /**
                * @see Thread::join()
                */
                void
                join()
                throw (
                    DeadlockException,
                    InternalErrorException);

                /**
                * @see Thread::yield()
                */
                void
                yield()
                throw (InternalErrorException);


                /**
                * @see Thread::self()
                */
                static
                Thread::ThreadId
                self()
                throw();


                /**
                * @see Thread::get_status
                */
                ThreadStatus
                get_status()
                throw();


                /**
                * @see Thread::get_ThreadHandle
                */
                static
                ThreadHandle
                get_ThreadHandle()
                throw();


                /**
                * Indicate if the thread is currently running or has terminated 
                * its job.
                *
                * @return true if the thread is running, false else
                *
                */
                bool
                is_running()
                throw();


                /**
                * Indicate if the thread has already run and terminate 
                * its job.
                *
                * @return true if the thread is ended
                *
                */
                bool
                is_ended()
                throw();


                /**
                * @see Thread::set_stackSize()
                */
                void
                set_stackSize(size_t stackSize)
                throw (
                    BadParameterException,
                    AlreadyDoneException,
                    InternalErrorException);



                /**
                * @see Thread::get_stackSize()
                */
                size_t
                get_stackSize()
                throw (
                    InternalErrorException);



                /**
                * @see Thread::set_scopeSchedPolicy()
                */
                void
                set_scopeSchedPolicy (Thread::ThrScope scope, Thread::ThrSchedulPolicy policy)
                throw (
                    BadParameterException,
                    AlreadyDoneException,
                    InternalErrorException);


                /**
                * @see Thread::get_scopeSchedPolicy()
                */
                void
                get_scopeSchedPolicy (Thread::ThrScope& scope, Thread::ThrSchedulPolicy& policy)
                throw (
                    InternalErrorException);


                /**
                * @see Thread::set_priority()
                */
                void
                set_priority(Thread::Priority priority)
                throw (
                    BadParameterException,
                    PermissionDeniedException,
                    ThreadNotExistException,
                    InternalErrorException);


                /**
                * @see Thread::get_min_priority()
                */
                Thread::Priority
                get_min_priority()
                throw (InternalErrorException);

                /**
                * @see Thread::get_max_priority()
                */
                Thread::Priority
                get_max_priority()
                throw (InternalErrorException);

                /**
                * @see Thread::get_priority()
                */
                Thread::Priority
                get_priority()
                throw (
                    ThreadNotExistException,
                    InternalErrorException);


                // Friend to allow the call of the run() method
                friend
#       if defined(CDMW_POSIX_THREAD)

                void*
                thread_posix_thread_entry_point (void* pThreadData)
                throw();

#       elif defined (_WIN32)

                DWORD WINAPI
                thread_win32_thread_entry_point (LPVOID pThreadData)
                throw();

#       endif

            private:

                //
                // Copy constructor
                //
                ThreadImpl(const ThreadImpl&);


                //
                // Assignment operator
                //
                ThreadImpl&
                operator=(const ThreadImpl& );


                /**
                * get posix min priority
                */
                int get_posix_min_priority()
                throw (InternalErrorException);

                /**
                * get posix min priority
                */
                int get_posix_max_priority()
                throw (InternalErrorException);


                /**
                * Sets the status of the thread.
                *
                * @param status the new status of the thread.
                */
                void set_status(ThreadStatus status)
                throw();

                /**
                * Sets the status of the thread if the current status
                * matches the expected status. 
                *
                * @param new_status the new status of the thread.
                * @param expected_status the expected current status.
                */
                void try_set_status(ThreadStatus new_status,
                                    ThreadStatus expected_status)
                throw();




                /**
                * Handle of the thread
                */
                ThreadHandle m_thandle;


                /**
                * Handle of the pimpl idiom
                */
                Thread* m_pThread;


                /**
                * Contains the current Thread state.
                */
                ThreadStatus m_status;


                /**
                * The mutex protecting concurrent access to m_status.
                */
                Mutex m_status_mutex;




#       if defined(CDMW_POSIX_THREAD)


                /**
                * Posix id of the thread
                */
                pthread_t m_pthreadId;


                /**
                * Posix attribute of the thread
                */
                pthread_attr_t m_pthread_attr;


                /**
                * Status of thread attribute initialisation
                * =true if initialised
                */
                bool m_attr_init;



                /**
                 * Purpose:
                 * <p> Check the return code of Operating System,
                 * and throw the appropriate exception in case
                 * of errors.
                 *
                 * @param status value whose error number must be checked
                 *
                 * @exception OutOfMemoryException  Not enough memory to satisfy the initialisation
                 * @exception ThreadSchedulingException The caller is not
                 *            allowed to start a thread from this priority
                 * @exception InternalErrorException one of the m_pthread_attr argument is invalid                  
                 */
                void check_status(int status)
                throw (
                    OutOfMemoryException,
                    ThreadSchedulingException,
                    InternalErrorException);


                /**
                 * Purpose:
                 * <p> Check the return code of Operating System,
                 * and throw the appropriate exception in case
                 * of errors for set_stackSize function.
                 *
                 * @param status value whose error number must be checked
                 *
                 * @exception OutOfMemoryException Not enough memory to satisfy the initialisation
                 * @exception ThreadSchedulingException The caller is not
                 *            allowed to start a thread from this priority
                 * @exception InternalErrorException                 
                 */
                void check_status_set_stackSize(int status)
                throw (
                    OutOfMemoryException,
                    ThreadSchedulingException,
                    InternalErrorException);

#       elif defined (_WIN32)

                /**
                * Stack size required by the user
                */
                DWORD m_stackSize;


                /**
                * Win32 id of the thread
                */
                DWORD m_win32Id;


                /**
                * Win32 handle of the thread
                */
                HANDLE m_win32Handle;


#       else
#          error CDMW_IMPL_NOT_AVAILABLE
#       endif


                /**
                 * Purpose:
                 * <p> Method call in a new thread from the
                 * Operating System dependent static function
                 */
                void run()
                throw() ;

                /**
                * Thread scheduling contention scope set
                */
                bool m_scopeSet;

                /**
                * Thread scheduling contention scope
                */
                Thread::ThrScope m_scope;

                /**
                * Thread scheduling policy set
                */
                bool m_schedPolicySet;

                /**
                * Thread scheduling policy
                */
                Thread::ThrSchedulPolicy m_schedPolicy;

                /**
                * Thread Priority mapping
                */
                ThreadPriority m_priorityMap;

                /**
                * mutex for priority change
                */
                OsSupport::Mutex m_mutex;


        };

    } // End namespace OsSupport

} // End namespace Cdmw


#endif // CDMW_OSSUPPORT_THREAD_IMPL_HPP



