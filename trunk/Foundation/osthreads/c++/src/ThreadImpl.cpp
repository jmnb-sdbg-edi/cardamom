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


#include "osthreads/ThreadImpl.hpp"

#include <cstdio>
#include <cerrno>

// To get PTHREAD_STACK_MIN
#include <limits.h>

#include <sstream>

// If we are compiled with Orbacus, we need to make adopt
//    the thread by Orbacus
#if defined CDMW_ORB_VDR && CDMW_ORB_VDR == orbacus

#include <JTC/JTC.h>

namespace
{
    JTCInitialize toInitializeJtc;
};

#endif

#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/osthreads/Thread.hpp"

namespace Cdmw
{

    namespace OsSupport
    {

        //
        // Ctor
        //
        ThreadImpl::ThreadImpl(Thread* pThread)
        throw (
            OutOfMemoryException,
            InternalErrorException,
            BadParameterException)

                : m_pThread (pThread),
                m_status (THREAD_CREATED),
                m_scopeSet (false),
                m_schedPolicySet (false)

#       ifdef _WIN32
                , m_stackSize(0)
#       endif

        {

#if defined (CDMW_POSIX_THREAD)

            int status_attr;

            m_attr_init = false;

            // We initialise the thread attribute with the default values
            status_attr = ::pthread_attr_init(&m_pthread_attr);

            switch (status_attr)
            {

                    case 0:
                    m_attr_init = true;
                    break;

                    // No more memory to allocate the structure

                    case ENOMEM:
                    CDMW_THROW(OutOfMemoryException);
                    break;

                    // As we have set no parameters ??

                    case EINVAL:
                    CDMW_THROW1(InternalErrorException, "Invalid m_pthread_attr parameters");
                    break;

                    default:
                    std::cout << "Thread pthread_attr_init status error : "
                    << status_attr << std::endl;
                    CDMW_THROW(InternalErrorException);
                    break;
            }

            status_attr = ::pthread_attr_setinheritsched(&m_pthread_attr, PTHREAD_EXPLICIT_SCHED);

            switch (status_attr)
            {

                    case 0:
                    break;

                    case EINVAL:
                    CDMW_THROW1(InternalErrorException, "Invalid m_pthread_attr parameters");
                    break;

                    default:
                    std::cout << "Thread pthread_attr_setinheritsched status error : "
                    << status_attr << std::endl;
                    CDMW_THROW(InternalErrorException);
                    break;
            }


            // inform min and max priority of the thread from system
            int priority = get_posix_max_priority();

            m_priorityMap.setMaxPriority (priority);

            priority = get_posix_min_priority();

            m_priorityMap.setMinPriority (priority);


#elif defined(_WIN32)

            m_win32Handle = NULL;

#else
#   error "Cdmw: Implementation not available"
#endif

#if defined(CDMW_HAVE_THREAD_STACK)

            set_stackSize(Thread::DEFAULT_STACK_SIZE);

#endif

        }



        //
        // Dtor
        //
        ThreadImpl::~ThreadImpl()
        throw ()
        {

#if defined (CDMW_POSIX_THREAD)

            if (m_attr_init)
            {
                // Destroy the thread attribute
                ::pthread_attr_destroy(&m_pthread_attr);
            }

#endif

        }


        //
        // Start the new thread
        //
        Thread::ThreadId
        ThreadImpl::start()
        throw (OutOfMemoryException,
               ThreadSchedulingException,
               InternalErrorException)
        {
	    // THREAD_ENDED case: we can also restart after a stop (a join)
            CDMW_ASSERT((get_status() == THREAD_CREATED)||(get_status() == THREAD_ENDED));

#if defined (CDMW_POSIX_THREAD)

            // Possible status are 0, ENOMEM, EINVAL, EPERM
            int status_thread = ::pthread_create (&m_pthreadId,
                                                  &m_pthread_attr,
                                                  &thread_posix_thread_entry_point,
                                                  this);

            // check os return status

            if (status_thread != 0)
            {
                std::cout << "Thread pthread_create status error : "
                << status_thread << std::endl;
            }

            check_status(status_thread);

            try_set_status(THREAD_RUN_REQUEST, THREAD_CREATED);
	    // THREAD_ENDED case: we can also restart after a stop (a join)
            try_set_status(THREAD_RUN_REQUEST, THREAD_ENDED);


            Thread::ThreadId thread_id = m_thandle.setThreadId (m_pthreadId);

            return thread_id;

#elif defined(_WIN32)

            m_win32Handle = ::CreateThread (NULL,
                                            m_stackSize,
                                            thread_win32_thread_entry_point,
                                            this,
                                            0,
                                            &m_win32Id);

            if (m_win32Handle == NULL)
            {
                DWORD status_thread = ::GetLastError();

                switch (status_thread)
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
                        std::cout << "Thread CreateThread status error : "
                        << status_thread << std::endl;
                        CDMW_THROW(InternalErrorException);
                        break;
                }
            }


            try_set_status(THREAD_RUN_REQUEST, THREAD_CREATED);
	    // THREAD_ENDED case: we can also restart after a stop (a join)
            try_set_status(THREAD_RUN_REQUEST, THREAD_ENDED);

            Thread::ThreadId thread_id = m_thandle.setThreadId (m_win32Id);

            return thread_id;

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
            pthread_t tid = ::pthread_self();
            ThreadHandle thandle;
            Thread::ThreadId thread_id = thandle.toThreadId (tid);

            return thread_id;


#elif defined(_WIN32)

            DWORD tid = ::GetCurrentThreadId();
            ThreadHandle thandle;
            Thread::ThreadId thread_id = thandle.toThreadId (tid);

            return thread_id;

#else
#   error "Cdmw: Implementation not available"
#endif

        }


        //
        // get the ThreadHandle of calling thread
        //
        ThreadHandle
        ThreadImpl::get_ThreadHandle()
        throw()
        {

#if defined (CDMW_POSIX_THREAD)

            pthread_t tid = ::pthread_self();

#elif defined(_WIN32)

            DWORD tid = ::GetCurrentThreadId();

#else
#   error "Cdmw: Implementation not available"
#endif

            ThreadHandle thandle;
            thandle.setThreadId (tid);

            return thandle;
        }



        //
        // Wait until the completion of the thread
        //
        void
        ThreadImpl::join()
        throw (DeadlockException,
               InternalErrorException)
        {

#if defined (CDMW_POSIX_THREAD)
            int status_thread_join = ::pthread_join (m_pthreadId,
                                     NULL);   // the thread is not allowed
            //    a value

            if (status_thread_join != 0)
            {
                std::cout << "Thread pthread_join status error : "
                << status_thread_join << std::endl;
            }

            switch (status_thread_join)
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

                    switch (get_status())
                    {

                            case THREAD_CREATED:
                            CDMW_THROW1(InternalErrorException, "the thread is not started");
                            break;

                            case THREAD_RUN_REQUEST:
                            CDMW_THROW1(InternalErrorException, "the thread is not started");
                            break;

                            case THREAD_ENDED:
                            CDMW_THROW1(InternalErrorException, "the thread is already stopped");
                            break;

                            default:
                            CDMW_THROW(InternalErrorException);
                    }

                    break;

                    // The specified thread doesn't exist, it is impossible
                    //    as the the pthread_t structure is not exported
                    //    to the application

                    case ESRCH:
                    CDMW_THROW1(InternalErrorException, "the thread doesn't exist");
                    break;

                    default:
                    CDMW_THROW(InternalErrorException);
            }

#elif defined(_WIN32)

            DWORD status_thread_join = ::WaitForSingleObject (m_win32Handle, INFINITE);

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
                    std::cout << "Thread WaitForSingleObject status error : "
                    << status_thread_join << std::endl;
                    CDMW_THROW (InternalErrorException);
                    break;
            }

#else
#   error "Cdmw: Implementation not available"
#endif

        }


        //
        // Allows to force the  running  thread  to relinquish
        // the processor until the process again becomes the
        // head of its process list.
        // This methods return only when the thread becomes running
        //
        void
        ThreadImpl::yield()
        throw (InternalErrorException)
        {

#if defined (CDMW_POSIX_THREAD)

            int status_yield = ::sched_yield ();

            if (status_yield != 0)
            {
                std::cout << "Thread sched_yield status error : "
                << status_yield << std::endl;
            }

            switch (status_yield)
            {

                    case 0:
                    // Nothing to do
                    break;

                    case - 1:

                    if (errno == ENOSYS)
                        CDMW_THROW1(InternalErrorException, "yield is not supported in this implementation");
                    else
                        CDMW_THROW(InternalErrorException);

                    break;


                    default:
                    CDMW_THROW(InternalErrorException);

                    break;
            }

#   elif defined(_WIN32)

            ::Sleep (0);

#else
#   error "Cdmw: Implementation not available"
#endif

        }



        /**
        * Returns the status of the thread. 
        */
        ThreadStatus ThreadImpl::get_status()
        throw()
        {
            m_status_mutex.lock();
            ThreadStatus status = m_status;
            m_status_mutex.unlock();

            return status;
        }



        /**
        * Sets the status of the thread. 
        */
        void ThreadImpl::set_status(ThreadStatus status)
        throw()
        {
            m_status_mutex.lock();
            m_status = status;
            m_status_mutex.unlock();
        }



        /**
        * Tries to set the status of the thread
        * if current status equal to expected status
        */
        void ThreadImpl::try_set_status(ThreadStatus new_status, ThreadStatus expected_status)
        throw()
        {
            m_status_mutex.lock();

            if (m_status == expected_status)
            {
                m_status = new_status;
            }

            m_status_mutex.unlock();
        }



        //
        // Return true if the thread is running
        //
        bool
        ThreadImpl::is_running()
        throw()
        {
            ThreadStatus status = get_status();
            return (status == THREAD_RUNNING);
        }




        bool
        ThreadImpl::is_ended()
        throw()
        {
            ThreadStatus status = get_status();
            return (status == THREAD_ENDED);
        }



        //
        // Call by the Operating System entry point in a new thread
        //
        void ThreadImpl::run()
        throw()
        {
            // The call is forward to the handle class
            m_pThread->_run();
        }


#if defined(CDMW_HAVE_THREAD_STACK) 
        //
        // Set the size of the stack
        //
        void
        ThreadImpl::set_stackSize(size_t stackSize)
        throw (BadParameterException,
               AlreadyDoneException,
               InternalErrorException)
        {
            if (get_status() == THREAD_CREATED)
            {

#   if defined (CDMW_POSIX_THREAD)

                // We allocate the specified size.
                // Warning: this function may return EINVAL, if m_pthread_attr or m_stackSize
                //    are invalid. If the error is for m_pthread_attr, we are corrupted,
                //    but if it is for m_stackSize, maybe the size is too big.
                //    We have no way to separate both error, so we throw an internal exception.
                //    A solution may be to check the m_stackSize value before. But what are
                //    the legal values ? it depends on the system configuration, but it must be
                //    possible.
                int status_set_stack = ::pthread_attr_setstacksize (&m_pthread_attr, stackSize);

                // Possible status are 0, EINVAL
                check_status_set_stackSize(status_set_stack);

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
        ThreadImpl::get_stackSize()
        throw (InternalErrorException)
        {

#   if defined (CDMW_POSIX_THREAD)

            size_t stackSize;

            int status_get_stack = ::pthread_attr_getstacksize (&m_pthread_attr, &stackSize);

            // Possible status are 0, EINVAL, but the user doesn't specify
            //    any value

            switch (status_get_stack)
            {

                    case 0:
                    break;

                    case EINVAL:

                    default:
                    std::cout << "Thread pthread_attr_getstacksize status error : "
                    << status_get_stack << std::endl;
                    CDMW_THROW(InternalErrorException);
                    break;
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
        // set the contention scope and scheduling policy
        //
        void
        ThreadImpl::set_scopeSchedPolicy (Thread::ThrScope scope, Thread::ThrSchedulPolicy policy)
        throw (BadParameterException,
               AlreadyDoneException,
               InternalErrorException)
        {

            if (get_status() == THREAD_CREATED)
            {

#   if defined (CDMW_POSIX_THREAD)

                // lock mutex to block concurent threads
                // (mutex is unlocked at end of block)
                CDMW_MUTEX_GUARD (m_mutex);

                int pthread_scope = 0;
                bool scope_ok = true;

                switch (scope)
                {

                        case Thread::THR_PROCESS_SCOPE:
                        // Linux implementation does not allow process-scope threads
                        // ENOTSUP error is returned
                        pthread_scope = PTHREAD_SCOPE_PROCESS;
                        break;

                        case Thread::THR_SYSTEM_SCOPE:
                        pthread_scope = PTHREAD_SCOPE_SYSTEM;
                        break;

                        default:
                        scope_ok = false;
                        break;
                }

                // error has been detected
                if (scope_ok == false)
                {
                    std::stringstream buffer;
                    buffer << scope << std::ends;
                    CDMW_THROW2(BadParameterException, "Thread::ThrScope", buffer.str());
                    return ;
                }

                // no error
                int status_set_scope =
                    ::pthread_attr_setscope (&m_pthread_attr, pthread_scope);

                // Possible status are 0, EINVAL, ENOTSUP
                // But if EINVAL is returned maybe
                //  ==> m_pthread_attr is invalid, so we are corrupted
                //  ==> pthread_scope is set with a bad value
                // We have no way to know the error, so we hope we are
                //    not corrupted and return a BadParameter
                switch (status_set_scope)
                {

                        case 0:
                        break;

                        case EINVAL:

                        case ENOTSUP:
                        {
                            scope_ok = false;
                            std::stringstream buffer;
                            buffer << pthread_scope << std::ends;
                            CDMW_THROW2(BadParameterException, "pthread scope ", buffer.str());
                        }

                        break;

                        default:
                        scope_ok = false;
                        std::cout << "Thread pthread_attr_setscope status error : "
                        << status_set_scope << std::endl;
                        CDMW_THROW(InternalErrorException);
                        break;
                }

                // if no error store the requested scope else return
                if (scope_ok)
                {
                    m_scopeSet = true;
                    m_scope = scope;
                }
                else
                {
                    return ;
                }

                int pthread_schedPolicy = 0;
                bool policy_ok = true;

                switch (policy)
                {

                        case Thread::THR_SCHEDULE_OTHER:
                        pthread_schedPolicy = SCHED_OTHER;
                        break;

                        case Thread::THR_SCHEDULE_FIFO:
                        // Linux implementation does not allow SCHED_FIFO policy if not superuser
                        // ENOTSUP error is returned
                        // Posix Solaris implementation does not allow SCHED_FIFO in process-scope threads

                        pthread_schedPolicy = SCHED_FIFO;
                        break;

                        case Thread::THR_SCHEDULE_RR:
                        // Linux implementation does not allow SCHED_RR policy if not superuser
                        // ENOTSUP error is returned
                        // Posix Solaris implementation does not allow SCHED_RR in process-scope threads

                        pthread_schedPolicy = SCHED_RR;
                        break;

                        default:
                        policy_ok = false;
                        break;
                }

                // error has been detected
                if (policy_ok == false)
                {
                    std::stringstream buffer;
                    buffer << policy << std::ends;
                    CDMW_THROW2(BadParameterException, "Thread::ThrSchedulPolicy", buffer.str());
                    return ;
                }

                // no error
                int status_set_policy =
                    ::pthread_attr_setschedpolicy (&m_pthread_attr, pthread_schedPolicy);

                // Possible status are 0, EINVAL, ENOTSUP
                // But if EINVAL is returned maybe
                //  ==> m_pthread_attr is invalid, so we are corrupted
                //  ==> pthread_schedPolicy is set with a bad value
                // We have no way to know the error, so we hope we are
                //    not corrupted and return a BadParameter
                switch (status_set_policy)
                {

                        case 0:
                        break;

                        case EINVAL:

                        case ENOTSUP:
                        {
                            policy_ok = false;
                            std::stringstream buffer;
                            buffer << pthread_schedPolicy << std::ends;
                            CDMW_THROW2(BadParameterException, "pthread scheduling policy ", buffer.str());
                        }

                        break;

                        default:
                        policy_ok = false;
                        std::cout << "Thread pthread_attr_setschedpolicy status error : "
                        << status_set_policy << std::endl;
                        CDMW_THROW(InternalErrorException);
                        break;
                }

                // if no error store the requested scheduling policy
                // and inform the new min/max priority of thread
                if (policy_ok)
                {
                    m_schedPolicySet = true;
                    m_schedPolicy = policy;

                    int priority = get_posix_max_priority();
                    m_priorityMap.setMaxPriority (priority);

                    priority = get_posix_min_priority();
                    m_priorityMap.setMinPriority (priority);
                }

#   elif defined(_WIN32)

                // Nothing to do under windows

#   else
#   error "Cdmw: Implementation not available"
#   endif

                // The thread has already be started, now we cannot change its scope and policy
            }
            else
            {

                CDMW_THROW(AlreadyDoneException);
            }

        }


        //
        // get the contention scope and scheduling policy
        //
        void
        ThreadImpl::get_scopeSchedPolicy (Thread::ThrScope& scope, Thread::ThrSchedulPolicy& policy)
        throw (InternalErrorException)
        {

#   if defined (CDMW_POSIX_THREAD)

            int pthread_scope = 0;

            int status_get_scope =
                ::pthread_attr_getscope (&m_pthread_attr, &pthread_scope);

            // Possible status are 0, EINVAL

            switch (status_get_scope)
            {

                    case 0:
                    break;

                    case EINVAL:

                    default:
                    std::cout << "Thread pthread_attr_getscope status error : "
                    << status_get_scope << std::endl;
                    CDMW_THROW(InternalErrorException);
                    break;
            }

            switch (pthread_scope)
            {

                    case PTHREAD_SCOPE_PROCESS:
                    scope = Thread::THR_PROCESS_SCOPE;
                    break;

                    case PTHREAD_SCOPE_SYSTEM:
                    scope = Thread::THR_SYSTEM_SCOPE;
                    break;

                    default:
                    CDMW_THROW(InternalErrorException);
                    break;
            }

            int pthread_schedPolicy;

            int status_get_policy =
                ::pthread_attr_getschedpolicy (&m_pthread_attr, &pthread_schedPolicy);

            // Possible status are 0, EINVAL

            switch (status_get_policy)
            {

                    case 0:
                    break;

                    case EINVAL:

                    default:
                    std::cout << "Thread pthread_attr_getschedpolicy status error : "
                    << status_get_policy << std::endl;
                    CDMW_THROW(InternalErrorException);
                    break;
            }

            switch (pthread_schedPolicy)
            {

                    case SCHED_OTHER:
                    policy = Thread::THR_SCHEDULE_OTHER;
                    break;

                    case SCHED_FIFO:
                    policy = Thread::THR_SCHEDULE_FIFO;
                    break;

                    case SCHED_RR:
                    policy = Thread::THR_SCHEDULE_RR;
                    break;

                    default:
                    CDMW_THROW(InternalErrorException);
                    break;
            }

#   elif defined(_WIN32)

            // Nothing to do under windows

#   else
#   error "Cdmw: Implementation not available"
#   endif

        }


        //
        // Set the priority of the thread
        //
        void
        ThreadImpl::set_priority(Thread::Priority priority)
        throw (BadParameterException,
               PermissionDeniedException,
               ThreadNotExistException,
               InternalErrorException)
        {
            // lock mutex to block concurent threads
            // (mutex is unlocked at end of block)
            CDMW_MUTEX_GUARD (m_mutex);


#   if defined (CDMW_POSIX_THREAD)

            // PCR-0323: deactivate set_priority implementation as it is not  
            // available with Linux kernel 2.4.x.
            CDMW_THROW1(InternalErrorException,
                        "ThreadImpl::set_priority not implemented with Linux kernel 2.4.x."); 
            
/****** PCR-0323: deactivate set_priority implementation *******

            struct sched_param schedParam;

            // if thread is not started

            if (get_status() == THREAD_CREATED)
            {
                // We get the scheduling parameters of the thread
                int status_get_sched = ::pthread_attr_getschedparam (&m_pthread_attr, &schedParam);

                // Possible status are 0, EINVAL
                // but the structure must be correct, we have change nothing

                switch (status_get_sched)
                {

                        case 0:
                        break;

                        case EINVAL:

                        default:
                        std::cout << "Thread pthread_attr_getschedparam status error : "
                        << status_get_sched << std::endl;
                        CDMW_THROW(InternalErrorException);
                        break;
                }

                try
                {
                    // Set priority
                    schedParam.sched_priority = m_priorityMap.toNativePriority (priority);
                }
                catch (const BadParameterException&)
                {
                    // error has been detected
                    std::stringstream buffer;
                    buffer << priority << std::ends;
                    CDMW_THROW2(BadParameterException, "Thread::Priority", buffer.str());
                }

                // then set the scheduling parameters of the thread
                int status_set_sched = ::pthread_attr_setschedparam (&m_pthread_attr, &schedParam);

                // Possible status are 0, EINVAL
                // But if EINVAL is returned maybe
                //  ==> m_pthread_attr is invalid, so we are corrupted
                //  ==> schedParam is set with a bad value
                // We have no way to know the error, so we hope we are
                //    not corrupted and return a BadParameter
                switch (status_set_sched)
                {

                        case 0:
                        break;

                        case EINVAL:
                        {
                            std::stringstream buffer;
                            buffer << priority << std::ends;
                            CDMW_THROW2(BadParameterException, "Thread::Priority", buffer.str());
                        }

                        break;

                        default:
                        std::cout << "Thread pthread_attr_setschedparam status error : "
                        << status_set_sched << std::endl;
                        CDMW_THROW(InternalErrorException);
                        break;
                }
            }

            // else if thread is running
            else if (get_status() == THREAD_RUNNING)
            {
                int pthread_schedPolicy = 0;

                // We get the scheduling parameters of the thread
                int status_get_sched =
                    ::pthread_getschedparam (m_pthreadId, &pthread_schedPolicy, &schedParam);


                // Possible status are 0, ESRCH

                switch (status_get_sched)
                {

                        case 0:
                        break;

                        case ESRCH:
                        {
                            CDMW_THROW(ThreadNotExistException);
                        }

                        break;

                        default:
                        std::cout << "Thread pthread_attr_getschedparam status error : "
                        << status_get_sched << std::endl;
                        CDMW_THROW(InternalErrorException);
                        break;
                }

                try
                {
                    // Change priority
                    schedParam.sched_priority = m_priorityMap.toNativePriority (priority);
                }
                catch (const BadParameterException&)
                {
                    // error has been detected
                    std::stringstream buffer;
                    buffer << priority << std::ends;
                    CDMW_THROW2(BadParameterException, "Thread::Priority", buffer.str());
                }

                // then set the scheduling parameters of the thread
                int status_set_sched =
                    ::pthread_setschedparam (m_pthreadId, pthread_schedPolicy, &schedParam);

                // Possible status are 0, EINVAL, EPERM, ESRCH
                switch (status_set_sched)
                {

                        case 0:
                        break;

                        case EINVAL:
                        {
                            std::stringstream buffer;
                            buffer << priority << std::ends;
                            CDMW_THROW2(BadParameterException, "Thread::Priority", buffer.str());
                        }

                        break;

                        case EPERM:
                        {
                            CDMW_THROW(PermissionDeniedException);
                        }

                        break;

                        case ESRCH:
                        {
                            CDMW_THROW(ThreadNotExistException);
                        }

                        break;

                        default:
                        std::cout << "Thread pthread_attr_setschedparam status error : "
                        << status_set_sched << std::endl;
                        CDMW_THROW(InternalErrorException);
                        break;
                }
            }

            // else thread is terminated
            else
            {
                CDMW_THROW(ThreadNotExistException);
            }

****** PCR-0323 ***********************************************/
            
#   elif defined(_WIN32)

            // Under windows we need a valid handle to set the priority,
            //  so we need to wait for the thread creation

            if (m_win32Handle != NULL)
            {
                DWORD thread_priority;

                try
                {
                    thread_priority = m_priorityMap.toNativePriority (priority);
                }
                catch (const BadParameterException&)
                {
                    // error has been detected
                    std::stringstream buffer;
                    buffer << priority << std::ends;
                    CDMW_THROW2(BadParameterException, "Thread::Priority", buffer.str());
                }

                int status_set_priority =
                    ::SetThreadPriority (m_win32Handle, thread_priority);

                if (status_set_priority == 0)
                {
                    DWORD err_code = ::GetLastError();

                    switch (err_code)
                    {

                            case ERROR_INVALID_DATA:

                            case ERROR_INVALID_PRIORITY:
                            {
                                std::stringstream buffer;
                                buffer << priority << std::ends;
                                CDMW_THROW2(BadParameterException, "Thread::ThrPriority", buffer.str());
                            }

                            break;

                            case ERROR_ACCESS_DENIED:

                            case ERROR_PRIVILEGE_NOT_HELD:
                            CDMW_THROW(PermissionDeniedException);
                            break;

                            default:
                            std::cout << "Thread SetThreadPriority status error : "
                            << status_set_priority << std::endl;
                            CDMW_THROW(InternalErrorException);
                            break;
                    }
                }
            }

            // The thread does not still exist, now we cannot change its priority
            else
            {
                CDMW_THROW(ThreadNotExistException);
            }

#   else
#   error "Cdmw: Implementation not available"
#   endif

        }


        //
        // get the max priority
        //
        Thread::Priority
        ThreadImpl::get_max_priority()
        throw (InternalErrorException)
        {
            // lock mutex to block concurent threads
            // (mutex is unlocked at end of block)
            CDMW_MUTEX_GUARD (m_mutex);

            return (m_priorityMap.getMaxPriority());
        };


        //
        // get the min priority
        //
        Thread::Priority
        ThreadImpl::get_min_priority()
        throw (InternalErrorException)
        {

            // lock mutex to block concurent threads
            // (mutex is unlocked at end of block)
            CDMW_MUTEX_GUARD (m_mutex);

            return (m_priorityMap.getMinPriority());
        };

        //
        // get the posix max priority
        //
        int ThreadImpl::get_posix_max_priority()
        throw (InternalErrorException)
        {

#if defined (CDMW_POSIX_AIX_THREAD)

            return (PTHREAD_PRIO_MAX);

#else

            int priority;
            int pthread_schedPolicy;

            int status_get_policy =
                ::pthread_attr_getschedpolicy (&m_pthread_attr, &pthread_schedPolicy);

            // Possible status are 0, EINVAL

            switch (status_get_policy)
            {

                    case 0:
                    break;

                    case EINVAL:

                    default:
                    std::cout << "Thread pthread_attr_getschedpolicy status error : "
                    << status_get_policy << std::endl;

                    CDMW_THROW(InternalErrorException);
                    break;
            }


            priority = ::sched_get_priority_max(pthread_schedPolicy);


            if (priority == -1)
            {
                switch (errno == EINVAL)
                {

                        case EINVAL :
                        CDMW_THROW1(InternalErrorException, "policy error");
                        break;

                        case ENOSYS :
                        CDMW_THROW1(InternalErrorException, "sched_get_priority_min function is not supported by the system");
                        break;

                        default :
                        std::cout << "Thread sched_get_priority_max status error : "
                        << errno << std::endl;
                        CDMW_THROW(InternalErrorException);
                }
            }

            return priority;

#endif

        };

        //
        // get the posix min priority
        //
        int ThreadImpl::get_posix_min_priority()
        throw (InternalErrorException)
        {

#if defined (CDMW_POSIX_AIX_THREAD)

            return (PTHREAD_PRIO_MIN);

#else

            int priority;
            int pthread_schedPolicy;

            int status_get_policy =
                ::pthread_attr_getschedpolicy (&m_pthread_attr, &pthread_schedPolicy);

            // Possible status are 0, EINVAL

            switch (status_get_policy)
            {

                    case 0:
                    break;

                    case EINVAL:

                    default:
                    std::cout << "Thread pthread_attr_getschedpolicy status error : "
                    << status_get_policy << std::endl;

                    CDMW_THROW(InternalErrorException);
                    break;
            }


            priority = ::sched_get_priority_min(pthread_schedPolicy);

            if (priority == -1)
            {
                switch (errno == EINVAL)
                {

                        case EINVAL :
                        CDMW_THROW1(InternalErrorException, "internal policy error");
                        break;

                        case ENOSYS :
                        CDMW_THROW1(InternalErrorException, "sched_get_priority_min function is not supported by the system");
                        break;

                        default :
                        std::cout << "Thread sched_get_priority_max status error : "
                        << errno << std::endl;
                        CDMW_THROW(InternalErrorException);
                }
            }

            return priority;

#endif

        };

        //
        // get the priority of the thread
        //
        Thread::Priority
        ThreadImpl::get_priority()
        throw (
            ThreadNotExistException,
            InternalErrorException)
        {

            // lock mutex to block concurent threads
            // (mutex is unlocked at end of block)
            CDMW_MUTEX_GUARD (m_mutex);


#   if defined (CDMW_POSIX_THREAD)

            struct sched_param schedParam;

            // if thread is not started

            if (get_status() == THREAD_CREATED)
            {
                // We get the scheduling parameters of the thread
                int status_get_sched = ::pthread_attr_getschedparam (&m_pthread_attr, &schedParam);

                // Possible status are 0, EINVAL
                //  as the user has not specified any new parameter,
                //  if we get an error, it means we are corrupted

                switch (status_get_sched)
                {

                        case 0:
                        break;

                        case EINVAL:

                        default:
                        std::cout << "Thread pthread_attr_getschedparam status error : "
                        << status_get_sched << std::endl;
                        CDMW_THROW(InternalErrorException);
                        break;
                }
            }


            // else if thread is running
            else if (get_status() == THREAD_RUNNING)
            {
                int pthread_schedPolicy = 0;

                // We get the scheduling parameters of the thread
                int status_get_sched =
                    ::pthread_getschedparam (m_pthreadId, &pthread_schedPolicy, &schedParam);

                // Possible status are 0, ESRCH

                switch (status_get_sched)
                {

                        case 0:
                        break;

                        case ESRCH:
                        {
                            CDMW_THROW(ThreadNotExistException);
                        }

                        break;

                        default:
                        std::cout << "Thread pthread_attr_getschedparam status error : "
                        << status_get_sched << std::endl;
                        CDMW_THROW(InternalErrorException);
                        break;
                }
            }

            // else thread is terminated
            else
            {
                CDMW_THROW(ThreadNotExistException);
            }

            // Get priority and return

            Thread::Priority priority;

            try
            {
                priority = m_priorityMap.fromNativePriority (schedParam.sched_priority);
            }
            catch (const BadParameterException& ex)
            {
                std::cerr << "CDMW Exception : \n" << ex.what() << std::endl;

                CDMW_THROW (InternalErrorException);
            }

            return priority;


#   elif defined(_WIN32)

            int native_priority;

            if (m_win32Handle != NULL)
            {
                native_priority = ::GetThreadPriority (m_win32Handle);

                if (native_priority == THREAD_PRIORITY_ERROR_RETURN)
                {
                    DWORD err_code = ::GetLastError();
                    std::cout << "Thread GetThreadPriority status error : "
                    << err_code << std::endl;
                    CDMW_THROW(InternalErrorException);
                }
            }

            // The thread does not still exist, now we cannot get its priority class
            else
            {
                CDMW_THROW(ThreadNotExistException);
            }

            // Get priority and return

            Thread::Priority priority;

            try
            {
                priority = m_priorityMap.fromNativePriority (native_priority);
            }
            catch (const BadParameterException& ex)
            {
                std::cerr << "CDMW Exception : \n" << ex.what() << std::endl;

                CDMW_THROW (InternalErrorException);
            }

            return priority;

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
        throw (OutOfMemoryException,
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
                    CDMW_THROW1(InternalErrorException, " one of the m_pthread_attr argument is invalid");

                    break;


                    // Unspecified error code returned

                    default:
                    CDMW_THROW(InternalErrorException);
                    break;
            };
        }


        //
        // Check the return statis of POSIX function and throw an exception
        //    in case of error
        //
        void
        ThreadImpl::check_status_set_stackSize(int status)
        throw (OutOfMemoryException,
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
                    CDMW_THROW1(InternalErrorException, " one of the m_pthread_attr argument is invalid");

                    break;


                    // Unspecified error code returned

                    default:
                    std::cout << "Thread pthread_attr_setstacksize status error : "
                    << status << std::endl;

                    CDMW_THROW(InternalErrorException);
                    break;
            };
        }


#   endif


#if defined (CDMW_POSIX_THREAD) 
        //
        // Static functions called by the POSIX function, in a new thread
        //
        extern "C"
            void*
            thread_posix_thread_entry_point (void* pThreadData)
            throw()

#elif defined (_WIN32)

        DWORD WINAPI
        thread_win32_thread_entry_point (LPVOID pThreadData)
        throw()

#endif
        {
            CDMW_ASSERT(pThreadData != NULL);

            // The received pointer designates an instance of the
            // ThreadImpl class
            ThreadImpl* pThreadImpl = static_cast<ThreadImpl*>(pThreadData);




            // If the ORB is orbacus, we need to inform the JTC layer
            //    of the birth of the new thread
#   if defined CDMW_ORB_VDR && CDMW_ORB_VDR == orbacus

            JTCAdoptCurrentThread toInformJTC;

            //
            // Using JTCAdoptCurrentThread leads to have a small increase
            // in memory consumption. It is not a leak memory but an allocation
            // of Thread Group data structure in table not deleted when thread
            // is terminated, only deleted when process terminates
            // Set thread Group as daemon force to clean the Thread Group table
            // when thread terminates.
            //
#     if (CDMW_ORB_VER > 410)
#       error Check if in JTC : set group daemon to true has been set!
#     else

            ((JTCThread::currentThread())->getThreadGroup())->setDaemon(true);
#     endif
#   endif



            // Sets the status of the thread
            pThreadImpl->set_status(THREAD_RUNNING);

            pThreadImpl->run();

            pThreadImpl->set_status(THREAD_ENDED);

            // At this time we return nothing,
            // TODO: check if run may return a value

#if defined (CDMW_POSIX_THREAD)

            return NULL;

#elif defined (_WIN32)

            return -1;

#endif

        }

    }  // End namespace osthreads

}  // End namespace Cdmw



