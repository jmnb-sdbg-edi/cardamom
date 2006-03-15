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


#include "testorbsupport/ThreadImpl.hpp"

#include <cstdio>
#include <cerrno>

// To get PTHREAD_STACK_MIN
#include <limits.h>

#include <sstream>


#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Exception.hpp"
#include "testorbsupport/Thread.hpp"



namespace Cdmw
{

    namespace TestOrbSupport
    {

        //
        // Ctor
        //
        ThreadImpl::ThreadImpl( Thread* pThread)
        throw (
            OutOfMemoryException,
            InternalErrorException )

                : m_pThread (pThread),
                m_status (CREATED)

#       ifdef _WIN32
                , m_stackSize(0)
#       endif
        {
#if defined (CDMW_POSIX_THREAD)

            // We initialise the thread attribute with the default values
            int status_attr = ::pthread_attr_init(&m_pthread_attr);

            switch ( status_attr)
            {

                    case 0:
                    break;

                    // No more memory to allocate the structure

                    case ENOMEM:
                    CDMW_THROW( OutOfMemoryException );
                    break;

                    // As we have set no parameters ??

                    case EINVAL:

                    default:
                    CDMW_THROW( InternalErrorException );
                    break;
            }

#   elif defined(_WIN32)

            // Nothing to do

#else
#   error "Cdmw: Implementation not available"
#endif

#if defined(CDMW_HAVE_THREAD_STACK)
            set_stackSize( Thread::DEFAULT_STACK_SIZE );

#endif

        }



        //
        // Dtor
        //
        ThreadImpl::~ThreadImpl()
        throw ()
        {
            // Nothing to do
        }


        //
        // Start the new thread
        //
        Thread::ThreadId
        ThreadImpl::start()
        throw ( OutOfMemoryException,
                ThreadSchedulingException,
                InternalErrorException)
        {
#if defined (CDMW_POSIX_THREAD)

            // Possible status are 0, ENOMEM, EINVAL, EPERM
            int status_thread = ::pthread_create ( &m_pthreadId,
                                                   &m_pthread_attr,
                                                   &TestOrbSupport_thread_posix_thread_entry_point,
                                                   this );

            check_status(status_thread);

            return m_pthreadId;

#   elif defined(_WIN32)

            m_threadHandle = ::CreateThread(
                                 NULL,
                                 m_stackSize,
                                 TestOrbSupport_thread_win32_thread_entry_point,
                                 this,
                                 0,
                                 &m_win32Id);

            if (m_threadHandle == NULL)
            {

                switch ( :: GetLastError())
                {

                        case ERROR_MAX_THRDS_REACHED:
                        // No break;

                        case ERROR_TOO_MANY_TCBS:
                        CDMW_THROW(OutOfMemoryException);
                        break;

                        // Null for the thread id

                        case ERROR_BAD_THREADID_ADDR:
                        // No break;

                        default:

                        CDMW_THROW(InternalErrorException);
                        break;
                }

            }

            return m_win32Id;
#else
#   error "Cdmw: Implementation not available"
#endif

        }




        //
        // Return the id of the calling thread
        //
        Thread::ThreadId
        ThreadImpl::self()
        throw()
        {
#if defined (CDMW_POSIX_THREAD)

            // No errors are defined
            return ::pthread_self();

#   elif defined(_WIN32)

            return ::GetCurrentThreadId();

#else
#   error "Cdmw: Implementation not available"
#endif

        }


        //
        // Wait until the completion of the thread
        //
        void
        ThreadImpl::join()
        throw (
            DeadlockException,
            InternalErrorException)
        {
#if defined (CDMW_POSIX_THREAD)

            int status_join = ::pthread_join( m_pthreadId,
                                              NULL); // the thread is not allowed
            //    a value

            switch ( status_join)
            {

                    case 0:
                    // Nothing to do
                    break;

                    // A thread tries on wait on itself.

                    case EDEADLK:
                    CDMW_THROW(DeadlockException);
                    break;

                    // The thread is not joinable. It is not
                    //     possible as all thread are joinable

                    case EINVAL:
                    // No break

                    // The specified thread doesn't exist, it is impossible
                    //    as the the pthread_t structure is not exported
                    //    to the application

                    case ESRCH:
                    // No break

                    default:
                    CDMW_THROW(InternalErrorException);
                    break;
            }

#   elif defined(_WIN32)

            DWORD status_thread_join = ::WaitForSingleObject( m_threadHandle, INFINITE);

            switch (status_thread_join)
            {

                    // No problem

                    case WAIT_OBJECT_0:
                    break;


                    // The thread owning the mutex has dissapeared before releasing
                    //  mutex

                    case WAIT_ABANDONED:
                    CDMW_NEVER_HERE();
                    break;

                    // No timeout specified

                    case WAIT_TIMEOUT:
                    // No break

                    // Unknown problem

                    case WAIT_FAILED:
                    // No break;

                    default:
                    CDMW_THROW ( InternalErrorException );
                    break;


            }

#else
#   error "Cdmw: Implementation not available"
#endif

        }




        //
        // Return true if the thread is running
        //
        bool
        ThreadImpl::is_running()
        throw()
        {
            // TODO: see lock problem
            return ( m_status == RUNNING );

        }




        bool
        ThreadImpl::is_ended()
        throw()
        {

            // TODO: see lock problem
            return ( m_status == ENDED );

        }



        //
        // Call by the Operating System entry point in a new thread
        //
        void ThreadImpl::run()
        throw()
        {
            // The call is forward to the handle class

            try
            {

                // TODO: lock with mutex, or discuss thread safety
                m_status = RUNNING;

                m_pThread->_run();

                m_status = ENDED;


                // TODO: the exception thrown may be store in an internal
                //  member variable to be thrown later on the father thread request
                //  but to do that we need to be able to copy an exception from the base
                //  class for instance by using a clone() method.
                // We may impose that on Cdmw exception class, but what about CORBA
                //  exception.
                // Here, we manage that case as an assertion: abort in debug, ignore
                //  or transmit to the application.

                // The implemention is not allowed to throw an
                //    exception
            }
            catch ( ...)
            {

                CDMW_NEVER_HERE();
            }

        }


#if defined(CDMW_HAVE_THREAD_STACK) 
        //
        // Set the size of the stack
        //
        void
        ThreadImpl::set_stackSize(size_t stackSize)
        throw (
            BadParameterException,
            AlreadyDoneException,
            InternalErrorException )
        {
            // TODO : add a lock or discuss about thread safety

            if ( m_status == CREATED )
            {

#       if defined (CDMW_POSIX_THREAD)

                // We allocate the specified size.
                // Warning: this function may return EINVAL, if m_pthread_attr or m_stackSize
                //    are invalid. If the error is for m_pthread_attr, we are corrupted,
                //    but if it is for m_stackSize, maybe the size is too big.
                //    We have no way to separate both error, so we throw an internal exception.
                //    A solution may be to check the m_stackSize value before. But what are
                //    the legal values ? it depends on the system configuration, but it must be
                //    possible.
                int status_set_stack = ::pthread_attr_setstacksize ( &m_pthread_attr, stackSize);

                // Possible status are 0, EINVAL
                check_status(status_set_stack);

#   elif defined(_WIN32)

                m_stackSize = stackSize;
#   else
#       error "Cdmw: Implementation not available"
#   endif

            }
            else
            {

                CDMW_THROW(AlreadyDoneException);
            }
        }


        //
        // get the size of the stack
        //
        size_t
        ThreadImpl::get_stackSize() const
        throw ( InternalErrorException)
        {

#   if defined (CDMW_POSIX_THREAD)

            size_t stackSize;

            int status_set_stack = ::pthread_attr_getstacksize ( &m_pthread_attr, &stackSize);

            // Possible status are 0, EINVAL, but the user doesn't specify
            //    any value

            if ( status_set_stack != 0 )
            {
                CDMW_THROW(InternalErrorException);
            }

            return stackSize;

#   elif defined(_WIN32)

            return m_stackSize;

#   else
#       error "Cdmw: Implementation not available"
#   endif // CDMW_POSIX_THREAD
        }

#endif // (CDMW_HAVE_THREAD_STACK)

        //
        // Set the priority of the thread
        //
        void
        ThreadImpl::set_priority(Thread::Priority priority)
        throw (
            BadParameterException,
            AlreadyDoneException,
            InternalErrorException )
        {

            // TODO : add a lock or discuss about thread safety

            if ( m_status == CREATED )
            {
#       if defined (CDMW_POSIX_THREAD)

                // We get the default scheduling parameters of the thread

                struct sched_param schedParam;
                int status_get_sched = ::pthread_attr_getschedparam ( &m_pthread_attr, &schedParam);

                // Possible status are 0, EINVAL
                // but the structure must be correct, we have change nothing

                if ( status_get_sched != 0)
                {
                    CDMW_THROW (InternalErrorException);
                };

                // TODO how unify the priority values between OS ?
                schedParam.sched_priority = priority;

                int status_set_sched = ::pthread_attr_setschedparam ( &m_pthread_attr, &schedParam);

                // Possible status are 0, EINVAL
                // But if EINVAL is returned maybe
                //  ==> m_pthread_attr is invalid, so we are corrupted
                //  ==> schedParam is set with a bad value
                // We have no way to know the error, so we hope we are
                //    not corrupted and return a BadParameter
                switch ( status_set_sched )
                {

                        case 0:
                        break;

                        case EINVAL:
                        {
                            try
                            {
                                std::stringstream buffer;
                                buffer << priority << std::ends;
                                CDMW_THROW2( BadParameterException, "Thread::Priority", buffer.str() );

                                // No more memory ? Ok, backup mode
                            }
                            catch ( const std::bad_alloc& )
                            {
                                const size_t sizeBuffer = 20;
                                char buffer[sizeBuffer];
                                ::sprintf(buffer, "%ud", priority);

                                CDMW_THROW2( BadParameterException, "Thread::Priority", buffer);
                            }
                        }

                        break;

                        default:
                        CDMW_THROW(InternalErrorException);
                        break;
                }

#   elif defined(_WIN32)

                // Under windows we need a valid handle to set the priority,
                //  so we need to wait for the thread creation
                m_win32_priority = priority;

#   else
#   error "Cdmw: Implementation not available"
#       endif
                // The thread has already be started, now we cannot change its size
            }
            else
            {

                CDMW_THROW(AlreadyDoneException);

            }
        }


        //
        // get the priority of the thread
        //
        Thread::Priority
        ThreadImpl::get_priority() const
        throw ( InternalErrorException )
        {

#   if defined (CDMW_POSIX_THREAD)

            // We get the scheduling parameters of the thread

            struct sched_param schedParam;
            int status_get_sched = ::pthread_attr_getschedparam ( &m_pthread_attr, &schedParam);

            // Possible status are 0, EINVAL
            //  as the user has not specified any new parameter,
            //  if we get an error, it means we are corrupted

            if ( status_get_sched != 0 )
            {
                CDMW_THROW(InternalErrorException);
            }

            // TODO how unify the priority values between OS ?
            return schedParam.sched_priority;


#   elif defined(_WIN32)

            // Under windows we need a valid handle to set the priority,
            //  so we need to wait for the thread creation
            return m_win32_priority;

#   else
#   error "Cdmw: Implementation not available"
#   endif // CDMW_POSIX_THREAD

        }


#if defined (CDMW_POSIX_THREAD)

        //
        // Check the return statis of POSIX function and throw an exception
        //    in case of error
        //
        void
        ThreadImpl::check_status(int status)
        throw ( OutOfMemoryException,
                ThreadSchedulingException,
                InternalErrorException)
        {

            switch (status)
            {

                    // No problem the initialisation is done

                    case 0:
                    break;

                    // Not enough memory to satisfy the initialisation

                    case ENOMEM:
                    CDMW_THROW(OutOfMemoryException);
                    break;

                    // The user is not allowed to use the specified thread
                    // policy

                    case EPERM:
                    CDMW_THROW(ThreadSchedulingException);
                    break;

                    // one of the m_pthread_attr argument is invalid,

                    case EINVAL:
                    // No break

                    // Unspecified error code returned

                    default:
                    CDMW_THROW(InternalErrorException);
                    break;
            };
        }

#endif



#if defined (CDMW_POSIX_THREAD) 
        //
        // Static functions called by the POSIX function, in a new thread
        //
        extern "C"
            void*
            TestOrbSupport_thread_posix_thread_entry_point ( void* pThreadData)
            throw()

#elif defined (_WIN32)

        DWORD WINAPI
        TestOrbSupport_thread_win32_thread_entry_point ( LPVOID pThreadData)
        throw()

#endif
        {
            CDMW_ASSERT( pThreadData != NULL);

            // The received pointer designates an instance of the
            // ThreadImpl class
            ThreadImpl* pThreadImpl = static_cast<ThreadImpl*>(pThreadData);

            // The thread must not be already started
            CDMW_ASSERT( pThreadImpl->m_status == ThreadImpl::CREATED );

            pThreadImpl->run();

            // At this time we return nothing,
            // TODO: check if run may return a value
            return NULL;
        }

    } // End namespace TestOrbSupport

}  // End namespace Cdmw



