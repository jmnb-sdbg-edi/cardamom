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


#include "osthreads/ConditionImpl.hpp"

#ifdef CDMW_POSIX_THREAD 
// FIXME: The gettimeofday method is not available if compiled
//    _POSIX_C_SOURCE. Why ?
#undef _POSIX_C_SOURCE

#include <sys/time.h>
#endif

#include <cerrno>

#include "Foundation/common/Assert.hpp"
#include "Foundation/common/MacroDefs.h"
#include "osthreads/MutexImpl.hpp"


namespace Cdmw
{

    namespace OsSupport
    {

        /**
        *Implementation notes:  none 
        *<p>
        *Portability issues:  none 
        *<p>
        */


        //
        // Ctor
        //
        ConditionImpl::ConditionImpl(MutexImpl& mutexImpl)
        throw (OutOfMemoryException,
               InternalErrorException)

                : m_mutexImpl(mutexImpl),
                m_is_attr_initialised(false),
                m_is_condition_initialised(false)
        {

#   ifdef CDMW_POSIX_THREAD

            init_default_attrCondition();

#   endif

#   if defined(CDMW_POSIX_THREAD) || defined(_WIN32)

            init_condition();

#   else
#       error "Cdmw: implementation not available"
#   endif

        }



        //
        // Dtor, see discussion in header
        //
        ConditionImpl::~ConditionImpl()
        throw (AssertionFailedException)
        {

#   ifdef CDMW_POSIX_THREAD


            // FIXME: a destructor should never throw, this class is an exception,
            //    I found, the EBUSY error, too important to be ignored. Destroying
            //    a condition in use, may introduce serious bugs. Doing an
            //    assertion may be a great help for the developer in its debugging
            //    so I decide to add an assertion.
            //    This point must be discussed in the global discussion about safety.

            // Possible error are: EINVAL, EBUSY

            if (m_is_condition_initialised)
            {
                int status_cond_destroy = ::pthread_cond_destroy( &m_pthread_cond );

                switch ( status_cond_destroy)
                {

                        case 0:
                        break;

                        case EBUSY:
                        CDMW_NEVER_HERE();
                        break;

                        case EINVAL:
                        break;

                        default:
                        break;
                }
            }



            if (m_is_attr_initialised)
            {
                // Possible error are: EINVAL
                // We decide to ignore this error, as we are
                //    in a destructor we don't propagate an exception
                // A solution will be to explictly require a destruction
                //    in a specific method.
                ::pthread_condattr_destroy (&m_pthread_cond_attr);
            }

#   elif defined(_WIN32)

            ::CloseHandle (m_event);

#   endif

        }



        //
        // Block the calling thread until a broadcast() call
        //
        void
        ConditionImpl::wait()
        throw (AssertionFailedException,
               InternalErrorException)
        {
#   ifdef CDMW_POSIX_THREAD

            // After the pthread_cond_wait call the OS mutex will be unlocked
            //    we must maintain consistent the information in the
            //    mutex implementation, so we unlock its state.
            //    No race condition is possible since we are protected by
            //    the mutex
            m_mutexImpl.set_unlock();

            int status_cond_wait = ::pthread_cond_wait (&m_pthread_cond,
                                   m_mutexImpl.get_pthread_mutex());


            switch (status_cond_wait)
            {

                    case 0:
                    break;

                    // With this code we may have 2 kinds of errors:
                    //    - different mutex have been used with the same condition
                    //    - the condition attribute is not valid
                    // Both errors couldn't be caused by the user, so we get corrupted
                    // But also,
                    //    - the mutex is not locked before waiting
                    // the call to set_unlock() checks in an assertion if the mutex is locked
                    //    before "unlocking" its state, so here we are sure, the mutex is locked
                    //    If it is not the case, we get corrupted

                    case EINVAL:
                    // No break

                    default:
                    CDMW_THROW(InternalErrorException);
                    break;
            }

            // If we are here, we have reacquired the OS mutex, we must maintain
            //    the consistent the information in the
            //    mutex implementation, so we lock its state
            m_mutexImpl.set_lock();


#   elif defined(_WIN32)

            // After the SignalObjectAndWait call the OS mutex will be unlocked
            //    we must maintain consistent the information in the
            //    mutex implementation, so we unlock its state.
            //    No race condition is possible since we are protected by
            //    the mutex
            m_mutexImpl.set_unlock();

            DWORD status_signal = ::SignalObjectAndWait (m_mutexImpl.get_win32_mutex(),
                                  m_event,
                                  INFINITE,
                                  false);

            if (status_signal != WAIT_OBJECT_0)
            {
                CDMW_THROW(InternalErrorException);
            }

            DWORD status_wait = ::WaitForSingleObject (m_mutexImpl.get_win32_mutex(),
                                INFINITE);

            switch (status_wait)
            {

                    case WAIT_OBJECT_0:
                    break;

                    // The thread owning the mutex has died before releasing the mutes

                    case WAIT_ABANDONED:
                    CDMW_NEVER_HERE();
                    break;

                    default:
                    CDMW_THROW(InternalErrorException);
                    break;
            }

            // If we are here, we have reacquired the OS mutex, we must maintain
            //    the consistent the information in the
            //    mutex implementation, so we lock its state
            m_mutexImpl.set_lock();

#   else
#       error "Cdmw: implementation not available"
#   endif
        }




        //
        // Blocks the calling thread until broadcast call or the timeout expires
        //
        Condition::WaitReturn
        ConditionImpl::wait(size_t milliseconds)
        throw (AssertionFailedException,
               InternalErrorException)
        {

            Condition::WaitReturn waitReturn;

#   ifdef CDMW_POSIX_THREAD

            struct timeval now;
            ::gettimeofday (&now, NULL);

            // We compute the date we wish to be awaken

            struct timespec timeout;
            timeout.tv_sec = now.tv_sec + milliseconds / 1000;
            timeout.tv_nsec = (now.tv_usec * 1000) + (milliseconds % 1000) * 1000000 ;

            if (timeout.tv_nsec >= 1000000000 )
            {
                timeout.tv_sec++;
                timeout.tv_nsec = timeout.tv_nsec - 1000000000;
            }

            // After the pthread_cond_wait call the OS mutex will be unlocked
            //    we must maintain consistent the information in the
            //    mutex implementation, so we unlock its state.
            //    No race condition is possible since we are protected by
            //    the mutex

            m_mutexImpl.set_unlock();

            int status_cond_timedwait = ::pthread_cond_timedwait (&m_pthread_cond,
                                        m_mutexImpl.get_pthread_mutex(),
                                        &timeout);

            switch ( status_cond_timedwait )
            {

                    case 0:
                    waitReturn = Condition::SIGNALLED;
                    break;

                    case ETIMEDOUT:
                    waitReturn = Condition::TIMEDOUT;
                    break;

                    // See discussion in wait()

                    case EINVAL:
                    // No break;

                    // We have been interrupted by a signal

                    case EINTR:
                    // No break;

                    default:
                    CDMW_THROW(InternalErrorException);
                    break;
            }


            // If we are here, we have reacquired the OS mutex, we must maintain
            //    the consistent the information in the
            //    mutex implementation, so we lock its state
            m_mutexImpl.set_lock();


#   elif defined(_WIN32)

            // After the SignalObjectAndWait call the OS mutex will be unlocked
            //    we must maintain consistent the information in the
            //    mutex implementation, so we unlock its state.
            //    No race condition is possible since we are protected by
            //    the mutex
            m_mutexImpl.set_unlock();

            DWORD status_signal = SignalObjectAndWait (m_mutexImpl.get_win32_mutex(),
                                  m_event,
                                  milliseconds,
                                  false);


            switch (status_signal)
            {

                    // The timeout has expired, sorry !

                    case WAIT_TIMEOUT:
                    waitReturn = Condition::TIMEDOUT;
                    break;

                    case WAIT_OBJECT_0:
                    waitReturn = Condition::SIGNALLED;
                    break;

                    case WAIT_ABANDONED:
                    CDMW_NEVER_HERE();
                    break;

                    default:
                    CDMW_THROW(InternalErrorException);
                    break;

            }


            DWORD status_wait = ::WaitForSingleObject (m_mutexImpl.get_win32_mutex(),
                                INFINITE);

            // If we are here, we have reacquired the OS mutex, we must maintain
            //    the consistent the information in the
            //    mutex implementation, so we lock its state
            m_mutexImpl.set_lock();

            switch (status_wait)
            {

                    case WAIT_OBJECT_0:
                    // Nothing to do
                    break;

                    // The thread owning the mutex has died before releasing the mutes

                    case WAIT_ABANDONED:
                    CDMW_NEVER_HERE();
                    break;

                    default:
                    CDMW_THROW(InternalErrorException);
                    break;
            }


#   else
#       error "Cdmw: implementation not available"
#   endif


            return waitReturn;
        }




        //
        // Not provided see discussion in header
        //
        void
        ConditionImpl::signal()
        throw (InternalErrorException)
        {
#   ifdef CDMW_POSIX_THREAD

            int status_cond_signal = ::pthread_cond_signal ( &m_pthread_cond );

            // Possible status are: 0, EINVAL,

            switch ( status_cond_signal )
            {

                    case 0:
                    break;

                    // The condition is not a condition !!

                    case EINVAL:
                    // No break;

                    default:
                    CDMW_THROW(InternalErrorException);
                    break;
            }

#   elif defined(_WIN32)

            // Not provided the Event creation must be changed to do it

#   else
#       error "Cdmw: implementation not available"
#   endif

        }



        //
        // Broadcast a signal on the condition for all threads
        //
        void
        ConditionImpl::broadcast()
        throw (InternalErrorException)
        {
#   ifdef CDMW_POSIX_THREAD

            int status_cond_broadcast = ::pthread_cond_broadcast ( &m_pthread_cond );


            // Possible status are: 0, EINVAL,

            switch ( status_cond_broadcast )
            {

                    case 0:
                    break;

                    // The condition is not a condition !!

                    case EINVAL:
                    // No break;

                    default:
                    CDMW_THROW(InternalErrorException);
                    break;
            }

#   elif defined(_WIN32)

            // Workaround for a bug in the PulseEvent, in debug environment
            //  see ID: Q173260
            ::Sleep(0) ;

            if ( ::PulseEvent( m_event ) == false )
            {

                CDMW_THROW(InternalErrorException);

            }


#   else
#       error "Cdmw: implementation not available"
#   endif

        }




#ifdef CDMW_POSIX_THREAD

        //
        // Initialise the with default value the posix condition attribute
        //
        void
        ConditionImpl::init_default_attrCondition()
        throw (OutOfMemoryException,
               InternalErrorException)
        {

            int status_condattr_init = ::pthread_condattr_init(&m_pthread_cond_attr);

            // Possible status are: 0, ENOMEM

            switch (status_condattr_init)
            {

                    case 0:
                    break;

                    case ENOMEM:
                    CDMW_THROW(OutOfMemoryException);
                    break;

                    default:
                    CDMW_THROW(InternalErrorException);
                    break;
            }

            m_is_attr_initialised = true;
        }

#endif



        //
        // Default initialisation of the posix condition
        //
        void
        ConditionImpl::init_condition()
        throw (OutOfMemoryException,
               InternalErrorException)
        {

#   ifdef CDMW_POSIX_THREAD

            int status_cond_init = ::pthread_cond_init (&m_pthread_cond,
                                   &m_pthread_cond_attr);

            // Possible status are: 0, EAGAIN, ENOMEM, EBUSY

            switch ( status_cond_init )
            {

                    case 0:
                    break;

                    // The system has not enough system ressource
                    //    other than memory, we propagate an
                    //    OutOfMemory
                    // FIXME: propagate a ressouce exhausted exception

                    case EAGAIN:
                    // No break

                    case ENOMEM:
                    CDMW_THROW(OutOfMemoryException);
                    break;

                    // The condition has been already initialised,
                    // As init, and destruction are wrapped in
                    //   ctor, dtor this must not happened.
                    // The condition object is not valid

                    case EBUSY:
                    // No break

                    default:
                    CDMW_THROW(InternalErrorException);
                    break;
            }

            m_is_condition_initialised = true;

#   elif defined(_WIN32)

            m_event = ::CreateEvent (NULL,   // No security descriptor
                                     TRUE,   // Manual-reset, so each thread will be notified (broadcast)
                                     FALSE,  // Creation as non-signalled
                                     NULL   // Event without name
                                    );


            if (m_event == NULL)
            {

                switch (::GetLastError())
                {
                        // We don't specify a valid name ??

                        case ERROR_INVALID_EVENTNAME:
                        // no break

                        default:
                        CDMW_THROW(InternalErrorException);
                        break;

                }
            }

            m_is_condition_initialised = true;

#   else
#       error "Cdmw: implementation not available"
#   endif

        }



    } // End namespace osthreads
} // End namespace Cdmw

