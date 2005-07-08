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


#include "Foundation/osthreads/Thread.hpp"

#include <cerrno>
#include <cstdio>
#include <new>
#include <algorithm>

#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/osthreads/ThreadObserver.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"

#include "osthreads/ThreadImpl.hpp"

namespace Cdmw
{

    namespace OsSupport
    {



        /**
        * The mem_fun implementation shipped with VC++ is broken as 
        * it doesn't handle void function member. This implementation
        * handle it
        */

        template <typename TY>

        class mem_fun_t
        {

            public:
                mem_fun_t(void (TY::*ptr_memb_func)())
                        : m_ptr_memb_func(ptr_memb_func)
                {
                }

                void operator()(TY* obj_ptr)
                {
                    (obj_ptr->*m_ptr_memb_func)();
                }

            private:

                void (TY::*m_ptr_memb_func)();

        };


        template <typename TY>
        mem_fun_t<TY> mem_fun (void (TY::*ptr_mem_func)())
        {
            return mem_fun_t<TY>(ptr_mem_func) ;
        }




        Mutex Thread::m_list_ThreadObserver_mtx;

        Thread::ThreadObserverCtr Thread::m_threadObserverList;


        //
        // Ctor
        //
        ThreadSchedulingException::ThreadSchedulingException()
        throw()
                : Exception (Exception::SAFE, "Unable to set scheduling parameters")
        {

        }



        //
        // Ctor
        //
        DeadlockException::DeadlockException()
        throw()
                : Exception (Exception::SAFE, "A deadlock between two thread has been detected")
        {

        }



        //
        // Ctor
        //
        ThreadNotExistException::ThreadNotExistException()
        throw()
                : Exception (Exception::SAFE, "Thread does not exist")
        {

        }

        //
        // Ctor
        //
        InterruptedException::InterruptedException()
                : Exception( Exception::SAFE, "Interrupted by signal" )
        {
            // Nothing to do
        }

        //
        // Dtor
        //
        InterruptedException::~InterruptedException()
        throw()
        {
            // Nothing to do
        }

        //
        // Ctor
        //
        Thread::Thread()
        throw (
            OutOfMemoryException,
            InternalErrorException,
            BadParameterException)

                : m_spImpl(NULL),
                m_has_been_joined(false)
        {
            try
            {


                // Pimpl idiom
#   ifdef _MSC_VER
                // The auto_ptr implementation shipped with visual is not compliant
                //  and doesn't provide reset(), but it is broken as it used an assignement
                //  operator with const auto_ptr&, we use this deviation
                m_spImpl = std::auto_ptr<ThreadImpl>(new ThreadImpl (this));
#   else

                m_spImpl.reset (new ThreadImpl (this));
#   endif


            }
            catch (const std::bad_alloc&)
            {

                CDMW_THROW(OutOfMemoryException);
            }
        }


        //
        // Dtor
        //
        Thread::~Thread()
        throw()
        {
            // If the thread is destroyed, it must have ended, its
            //  task before
            CDMW_ASSERT(! m_spImpl->is_running());

            // In case of no other thread has waiting for us
            //  we join to avoid memory leak
            bool threadImpl_is_ended = m_spImpl->is_ended();

            if (threadImpl_is_ended && (!m_has_been_joined))
            {
                this->join();
            }
        }



        void
        Thread::register_observer (ThreadObserver* pThreadObserver)
        {

            CDMW_MUTEX_GUARD(m_list_ThreadObserver_mtx);

            m_threadObserverList.push_back (pThreadObserver);

        }



        void
        Thread::unregister_observer (ThreadObserver* pThreadObserver)
        {

            CDMW_MUTEX_GUARD(m_list_ThreadObserver_mtx);
            m_threadObserverList.remove (pThreadObserver);

        }



        //
        // Start the new thread
        //
        Thread::ThreadId Thread::start()
        throw (
            OutOfMemoryException,
            ThreadSchedulingException,
            InternalErrorException)
        {
            CDMW_ASSERT (m_spImpl.get() != NULL);

            CDMW_MUTEX_GUARD (m_start_mtx);

            return m_spImpl->start();
        }



        //
        // Start the user code from the new thread
        //
        void
        Thread::_run()
        throw ()
        {
            CDMW_MUTEX_GUARD (m_start_mtx);

            // We notify each observer the thread is ready to operate
            {
                CDMW_MUTEX_GUARD(m_list_ThreadObserver_mtx);
                std::for_each (m_threadObserverList.begin(),
                               m_threadObserverList.end(),
                               mem_fun(&ThreadObserver::onEnterRun));
            }



            try
            {
                // We invoke the user code
                this->run();
            }
            catch (...)
            {
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
                CDMW_NEVER_HERE();
            }


            // We notify each observer the thread is exiting
            {
                CDMW_MUTEX_GUARD(m_list_ThreadObserver_mtx);
                std::for_each (m_threadObserverList.begin(),
                               m_threadObserverList.end(),
                               mem_fun(&ThreadObserver::onLeaveRun));

            }


        }




        //
        // Return the id of the calling thread
        //
        Thread::ThreadId
        Thread::self()
        throw()
        {

            return ThreadImpl::self();

        }

        //
        // return the ThreadHandle of the calling thread
        //
        ThreadHandle
        Thread::get_ThreadHandle ()
        throw()
        {
            return ThreadImpl::get_ThreadHandle();
        }


        //
        // Wait until the completion if the thread
        //
        void
        Thread::join()
        throw (
            DeadlockException,
            InternalErrorException)
        {
            // The thread has been joined by another one,
            //  it will be useless to "auto-join" in the
            //  destructor to avoid memory leak
            m_has_been_joined = true;

            m_spImpl->join();
        }


        //
        // yield the running thread
        //
        void
        Thread::yield()
        throw (InternalErrorException)
        {
            m_spImpl->yield();
        }



#if defined(CDMW_HAVE_THREAD_STACK) 
        //
        // Set the size of the stack
        //
        void
        Thread::set_stackSize(size_t stackSize)
        throw (
            BadParameterException,
            AlreadyDoneException,
            InternalErrorException)
        {
            m_spImpl->set_stackSize (stackSize);
        }



        //
        // get the size of the stack
        //
        size_t
        Thread::get_stackSize()
        throw (InternalErrorException)
        {
            return m_spImpl->get_stackSize();
        }

#endif // defined(CDMW_HAVE_THREAD_STACK)


        //
        // set the contention scope and scheduling policy
        //
        void
        Thread::set_scopeSchedPolicy(ThrScope scope, ThrSchedulPolicy policy)
        throw (BadParameterException,
               AlreadyDoneException,
               InternalErrorException)
        {
            m_spImpl->set_scopeSchedPolicy (scope, policy);
        }


        //
        // get the contention scope and scheduling policy
        //
        void
        Thread::get_scopeSchedPolicy(ThrScope& scope, ThrSchedulPolicy& policy)
        throw (InternalErrorException)
        {
            m_spImpl->get_scopeSchedPolicy (scope, policy);
        }


        //
        // set the priority of the thread
        //
        void
        Thread::set_priority(Priority priority)
        throw (
            BadParameterException,
            PermissionDeniedException,
            ThreadNotExistException,
            InternalErrorException)
        {
            m_spImpl->set_priority(priority);
        }


        //
        // get the priority of the thread
        //
        Thread::Priority
        Thread::get_priority()
        throw (
            ThreadNotExistException,
            InternalErrorException)
        {
            return m_spImpl->get_priority();
        }

        //
        // get the priority of the thread
        //
        Thread::Priority
        Thread::get_min_priority()
        throw (
            ThreadNotExistException,
            InternalErrorException)
        {
            return m_spImpl->get_min_priority();
        }

        //
        // get the priority of the thread
        //
        Thread::Priority
        Thread::get_max_priority()
        throw (
            ThreadNotExistException,
            InternalErrorException)
        {
            return m_spImpl->get_max_priority();
        }


        /**
        * Returns the status of the thread. 
        */
        ThreadStatus Thread::get_status()
        throw()
        {
            return m_spImpl->get_status();
        }

        //
        // Make sleep the calling thread during X milliseconds
        //
        void
        Thread::sleep(unsigned int milliseconds)
        throw( InterruptedException,
               InternalErrorException)
        {
#if defined (CDMW_POSIX)

            struct timespec tv;
            tv.tv_sec = milliseconds / 1000;
            tv.tv_nsec = (milliseconds % 1000) * 1000000 ;

            int status_nanosleep;

#   if defined(linux)
            // When the system call is interrupted by a signal ( which may
            //  be caused by the LinuxThread implemenation of condition)
            //  The time to sleep not elapsed is returned into tv

            struct timespec tv_not_elapsed;

            while ( (status_nanosleep = nanosleep(&tv, &tv_not_elapsed)) < 0 && (errno == EINTR) )
            {
                tv = tv_not_elapsed;
            }

#   else
            status_nanosleep = ::nanosleep(&tv, NULL);

#   endif

            if ( status_nanosleep < 0 )
            {

                switch ( errno )
                {

                        case EINTR:
                        CDMW_THROW(InterruptedException);
                        break;

                        // The value for nanoseconds is not valid

                        case EINVAL:
                        // No break

                        // The implementation doesn't support nanosleep

                        case ENOSYS:
                        // No break

                        default:
                        CDMW_THROW(InternalErrorException);
                        break;
                }
            }

#elif defined(_WIN32)

            ::Sleep( milliseconds );

#else
#    error "Cdmw: implementation not defined"
#endif


        }

    } // End namespace osthreads

} // End namespace Cdmw

