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


#ifndef CDMW_OSSUPPORT_THREAD_HPP
#define CDMW_OSSUPPORT_THREAD_HPP

#include "Foundation/common/System.hpp"

#include <memory>
#include <list>

#include "Foundation/common/MacroDefs.h"
#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/Mutex.hpp"

namespace Cdmw
{

    namespace OsSupport
    {

        // Forward declaration for use with the pimpl idiom

        class ThreadImpl;

        class ThreadObserver;

        class ThreadHandle;


        /**
        * Define the possible state of a thread
        */
        enum ThreadStatus {

            /**
            * The Thread object has been created, must have never
            * run in its own thread.
            */
            THREAD_CREATED,

            /**
            * The Thread object has been requested to run.
            */
            THREAD_RUN_REQUEST,

            /**
            * The Thread object is running in its own thread.
            */
            THREAD_RUNNING,

            /**
            * The Thread object has run in its own thread, and
            * has terminated its job.
            */
            THREAD_ENDED
        };



        /**
        *Purpose:
        *<p> Exception thrown when the user ask for scheduling parameter
        * not allowed for him.
        *
        */

        class ThreadSchedulingException : public Exception
        {

            public:
                ThreadSchedulingException()
                throw();

            protected:

            private:

        };



        /**
        *Purpose:
        *<p> Exception thrown when the user try to lock twice a mutex
        *    from the same thread
        *
        */

        class DeadlockException : public Exception
        {

            public:
                DeadlockException()
                throw();

            protected:

            private:

        };


        /**
        *Purpose:
        *<p> Exception thrown when the thread does not exist
        *
        */

        class ThreadNotExistException : public Exception
        {

            public:
                ThreadNotExistException()
                throw();

            protected:

            private:

        };

        /**
        *Purpose:
        *<p>    Thrown when a signal has been received and
        *        has interrupted the current fucntion call
        *
        *Features:
        *<p> Thread safe, exception safe
        *
        */

        class InterruptedException : public Exception
        {

            public:

                //
                // Ctor
                //
                InterruptedException();

                //
                // Dtor
                //
                virtual
                ~InterruptedException() throw();

            protected:

            private:

        };


        /**
        *Purpose:
        *<p> This abstract class allow to create a new thread 
        * by derivation. When a thread
        * invokes the start method of the class, a new thread
        * is created and the run() method is called from the
        * new one. The developer must override run() to be
        * called from a new thread.
        */

        class Thread
        {

            public:

                /**
                * Identify a thread in the current process.
                */
                typedef unsigned int ThreadId;


                /**
                * Define values authorized for priority.
                */
                typedef int Priority;

                /**
                * Define values for Contention Scope
                */
                enum ThrScope {
                    THR_PROCESS_SCOPE,
                    THR_SYSTEM_SCOPE
            };

                /**
                * Define values for Scheduling Policy
                */
                enum ThrSchedulPolicy {
                    THR_SCHEDULE_OTHER,
                    THR_SCHEDULE_FIFO,
                    THR_SCHEDULE_RR
            };

#      ifndef _MSC_VER

                /**
                 * Default stack size when the value is not set.
                 */
                static const int DEFAULT_STACK_SIZE = 2048*1024;



                /**
                 * Default priority assigned to the thread when
                 * none is specified.
                 */
                static const int DEFAULT_PRIORITY = 0;

#      else

                // Workaround for Visual C++, it's why the enum is not named,
                //   we just need to introduces in the class scope the names
                //   with their values
                enum {
                    DEFAULT_PRIORITY = 0,
                    DEFAULT_STACK_SIZE = 2048 * 1024
            };

#      endif


                /**
                * Construct the thread object
                *
                * @exception OutOfMemoryException Not enough memory to satisfy the initialisation
                * @exception InternalErrorException one of the m_pthread_attr argument is invalid
                * @exception BadParameterException              
                */
                Thread ()
                throw (
                    OutOfMemoryException,
                    InternalErrorException,
                    BadParameterException);



                //
                // Dtor
                //
                virtual
                ~Thread()
                throw();



                /**
                * Purpose:
                * <p> Allows to wait for the completion of the thread.
                * This methods return only when the thread has returned 
                * from the run() method.
                *
                * @exception DeadlockException the calling thread tries to
                *    wait on itself.
                * @exception InternalErrorException the thread is not started 
                *                  (thread status equal to THREAD_CREATED or THREAD_RUN_REQUEST)
                * @exception InternalErrorException the thread is already stopped
                *                  (thread status equal to THREAD_ENDED)
                * @exception InternalErrorException the thread doesn't exist
                *                  (status error equal to ESRCH)
                * @exception InternalErrorException internal error
                *
                *
                */
                void
                join()
                throw (
                    DeadlockException,
                    InternalErrorException);

                /**
                * Purpose:
                * <p> Allows to force the  running  thread  to relinquish
                * the processor until the process again becomes the
                * head of its process list.
                * This methods return only when the thread becomes running
                *
                * @exception InternalErrorException yield is not supported in this implementation (errno = ENOSYS)
                * @exception InternalErrorException internal error
                *
                */
                void
                yield()
                throw (InternalErrorException);


#      if defined(CDMW_HAVE_THREAD_STACK)

                /**
                * Purpose:
                * <p> Change the value of the stack size.
                * 
                * @parameter stackSize : Thread stack size
                *
                * @exception BadParameterException the specified size is not valid
                * @exception AlreadyDoneException thread is already started, its stack can
                *    not be changed.
                * @exception InternalErrorException 
                */
                void
                set_stackSize(size_t stackSize)
                throw (
                    BadParameterException,
                    AlreadyDoneException,
                    InternalErrorException);

                /**
                * Purpose:
                * <p> Returns the stack size of the thread.
                * 
                * @return the stack size of the thread
                *
                * @exception InternalErrorException 
                */
                size_t
                get_stackSize()
                throw (
                    InternalErrorException);


#          endif // defined(CDMW_HAVE_THREAD_STACK)

                /**
                * Purpose:
                * <p> Set the contention scope and scheduling policy of the current thread.
                *
                * @parameter scope  : contention scope
                * @parameter policy : scheduling policy
                *
                * @exception BadParameterException the specified scope is not valid
                * @exception AlreadyDoneException the thread is already started
                *            its scope can no more be changed
                * @exception InternalErrorException     
                *
                */
                void
                set_scopeSchedPolicy(ThrScope scope, ThrSchedulPolicy policy)
                throw (
                    BadParameterException,
                    AlreadyDoneException,
                    InternalErrorException);


                /**
                * Purpose:
                * <p> Get the contention scope and scheduling policy of the current thread.
                *
                * @parameter scope  : returned contention scope
                * @parameter policy : returned scheduling policy
                *
                * @exception InternalErrorException     
                *
                */
                void
                get_scopeSchedPolicy(ThrScope& scope, ThrSchedulPolicy& policy)
                throw (
                    InternalErrorException);


                /**
                * Purpose:
                * <p> Set the priority of the current thread before or after starting.
                *
                * @exception BadParameterException     the specified priority is not valid
                * @exception PermissionDeniedException no permission to change priority
                * @exception ThreadNotExistException   thread does not exist
                * @exception InternalErrorException    internal error
                *
                */
                void
                set_priority(Priority priority)
                throw (
                    BadParameterException,
                    PermissionDeniedException,
                    ThreadNotExistException,
                    InternalErrorException);

                /**
                * Purpose:
                * <p> get the priority of the current thread.
                *
                * @return the priority of the thread.
                *
                * @exception ThreadNotExistException   thread does not exist
                * @exception InternalErrorException    internal error
                *
                */
                Priority
                get_priority()
                throw (
                    ThreadNotExistException,
                    InternalErrorException);


                /**
                * Purpose:
                * <p> get the min priority.
                *
                * @return the min priority.
                *
                * @exception InternalErrorException    sched_get_priority_min function is not supported by the system
                * @exception InternalErrorException    internal error
                *
                */
                Priority
                get_min_priority()
                throw (
                    ThreadNotExistException,
                    InternalErrorException);

                /**
                * Purpose:
                * <p> get the max priority.
                *
                * @return the max priority.
                *
                * @exception InternalErrorException    sched_get_priority_min function is not supported by the system
                * @exception InternalErrorException    internal error
                *
                */
                Priority
                get_max_priority()
                throw (
                    ThreadNotExistException,
                    InternalErrorException);



                /**
                * Purpose:
                * <p> This method create a new thread and invoke
                * the run() method in this new thread.
                *
                * @return the id of the newly created thread.
                *
                * @exception OutOfMemoryException Not enough memory to satisfy the initialisation
                * @exception ThreadSchedulingException the caller is not allowed
                *            to start a thread from this priority
                * @exception InternalErrorException one of the m_pthread_attr argument is invalid
                * @exception InternalErrorException internal error
                */
                ThreadId
                start()
                throw (
                    OutOfMemoryException,
                    ThreadSchedulingException,
                    InternalErrorException);


                /**
                * Purpose:
                * <p> Returns the identifier of the calling thread.
                */
                static
                ThreadId
                self()
                throw();



                /**
                * Purpose:
                * <p> Returns the status of the thread. 
                */
                ThreadStatus
                get_status()
                throw();



                /**
                * Purpose:
                * <p> Return the Thread Handle of the calling thread
                * 
                */
                static
                ThreadHandle
                get_ThreadHandle()
                throw();

                /**
                * Purpose:
                * <p> Register a ThreadObserver. The registered ThreadObserver
                * will be notified at each step of the life of thread.
                * 
                * @param pThreadObserver pointer on the object to be notified
                */
                static
                void
                register_observer(ThreadObserver* pThreadObserver);



                /**
                * Purpose:
                * <p> Unregister a ThreadObserver.
                * 
                * @param pThreadObserver pointer on the object to be unregistered
                */
                static
                void
                unregister_observer(ThreadObserver* pThreadObserver);

                /**
                * Purpose:
                * <p> The calling thread is blocked during the specified
                * time expressed in milliseconds.
                *
                * @param milliseconds time to sleep expressed in
                *        milliseconds
                *
                * @exception InterruptedException
                * @exception InternalErrorException
                */
                static
                void
                sleep( unsigned int milliseconds)
                throw( InterruptedException,
                       InternalErrorException);


            protected:

                /**
                 * The implementation shall be able to invoke
                 * run() from a new thread.
                 */

                friend class ThreadImpl;

                /**
                 * Purpose:
                 * <p>This methods must be overriden by the
                 * derived class to implement a thread object.
                 * This method is always called in a new thread
                 * after a start() call.
                 */
                virtual void run() throw() = 0;


                /**
                 * Purpose:
                 * <p>This methods is called by the OS dependent
                 * implementation in a new thread.
                 */
                void
                _run()
                throw();



            private:

                /**
                * Body 
                */
                std::auto_ptr<ThreadImpl> m_spImpl;


                /**
                * Set if the thread has been join, to avoid
                * memory leak, the thread is automatically join,
                * if it has ended, but not joined, in the destruction.
                *
                */
                bool m_has_been_joined;


                /**
                * This mutex protects the start method
                *      to wait its execution end before thread running
                */
                Mutex m_start_mtx;


                /**
                * Define list of Thread Observer
                */
                typedef std::list<ThreadObserver*> ThreadObserverCtr;


                /**
                * This mutex protects the list of ThreadObserver
                */
                static Mutex m_list_ThreadObserver_mtx;


                /**
                * Contains the list of all object interrested in receiving
                * about the live of thread. This list must be manipulated
                * under the control of m_list_ThreadObserver_mtx.
                */
                static ThreadObserverCtr m_threadObserverList;


        };


    } // End namespace osthreads

} // End namespace Cdmw

#endif //CDMW_OSSUPPORT_THREAD_HPP


