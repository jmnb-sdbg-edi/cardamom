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


#ifndef CDMW_TESTORBSUPPORT_THREAD_IMPL_HPP
#define CDMW_TESTORBSUPPORT_THREAD_IMPL_HPP

#include "Foundation/common/MacroDefs.h"               // To know the current target

#if defined(CDMW_POSIX_THREAD)

#   include <pthread.h>

#elif defined (_WIN32)

#   include <windows.h>

#else
#   error CDMW_IMPL_NOT_AVAILABLE
#endif

#include "Foundation/common/Assert.hpp"
#include "testorbsupport/Thread.hpp"


namespace Cdmw
{

    namespace TestOrbSupport
    {

        /**
        * Functions called by the operating system in a new thread,
        * this methods calls the run() method of the implementation
        * in a new thread.
        */
#if defined(CDMW_POSIX_THREAD)

        extern "C"
            void*
            TestOrbSupport_thread_posix_thread_entry_point ( void* pThreadData)
            throw();

#elif defined (_WIN32)

        DWORD WINAPI
        TestOrbSupport_thread_win32_thread_entry_point ( LPVOID pThreadData)
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
                *@exception OutOfMemoryException
                *@exception InternalErrorException  
                */
                explicit
                ThreadImpl( Thread* pThread)
                throw (
                    OutOfMemoryException,
                    InternalErrorException);

                //
                // Dtor
                //
                ~ThreadImpl()
                throw();



                /**
                * Purpose:
                * <p> Start the new thread and returns its id.
                *
                * @exception OutOfMemoryException
                * @exception ThreadSchedulingException The caller is not
                *            allowed to start a thread from this priority
                * @exception InternalErrorException              
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
                * @see Thread::self()
                */
                static
                Thread::ThreadId
                self()
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
                    InternalErrorException );



                /**
                * @see Thread::get_stackSize()
                */
                size_t
                get_stackSize() const
                throw (
                    InternalErrorException);



                /**
                * @see Thread::set_priority()
                */
                void
                set_priority( Thread::Priority priority)
                throw (
                    BadParameterException,
                    AlreadyDoneException,
                    InternalErrorException );



                /**
                * @see Thread::get_priority()
                */
                Thread::Priority
                get_priority() const
                throw (
                    InternalErrorException ) ;

                // Friend to allow the call of the run() method
                friend
#       if defined(CDMW_POSIX_THREAD)

                void*
                TestOrbSupport_thread_posix_thread_entry_point ( void* pThreadData)
                throw();

#       elif defined (_WIN32)

                DWORD WINAPI
                TestOrbSupport_thread_win32_thread_entry_point ( LPVOID pThreadData)
                throw();

#       endif

            private:

                //
                // Copy constructor
                //
                ThreadImpl( const ThreadImpl&);


                //
                // Assignment operator
                //
                ThreadImpl&
                operator=( const ThreadImpl& );

                /**
                * Handle of the pimpl idiom
                */
                Thread* m_pThread;


                /**
                * Define the possible state of a thread
                */
                enum Status {

                    /**
                    * The Thread object has been created, must have never
                    * run in its own thread.
                    */
                    CREATED,

                    /**
                    * The Thread object is running in its own thread.
                    */
                    RUNNING,

                    /**
                    * The Thread object has run in its own thread, and
                    * has terminated its job.
                    */
                    ENDED
            };

                /**
                * Contains the current Thread state.
                */
                Status m_status;


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
                 * Purpose:
                 * <p> Check the return code of Operating System,
                 * and throw the appropriate exception in case
                 * of errors.
                 *
                 * @param status value whose error number must be checked
                 *
                 * @exception OutOfMemoryException
                 * @exception ThreadSchedulingException
                 * @exception InternalErrorException                 
                 */
                void check_status( int status)
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
                * Use to store the user priority until the thread start.
                */
                Thread::Priority m_win32_priority;



                /**
                * Win32 id of the thread
                */
                DWORD m_win32Id;


                /**
                * Win32 handle of the thread
                */
                HANDLE m_threadHandle;


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

        };

    } // End namespace TestOrbSupport

} // End namespace Cdmw


#endif // CDMW_TESTORBSUPPORT_THREAD_IMPL_HPP



