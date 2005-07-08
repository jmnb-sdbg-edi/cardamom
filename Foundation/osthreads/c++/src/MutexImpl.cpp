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


#include "osthreads/MutexImpl.hpp"

#include <cerrno>

#include "Foundation/common/Assert.hpp"

namespace Cdmw
{

    namespace OsSupport
    {


        //
        // Ctor
        //
        MutexImpl::MutexImpl()
        throw (
            OutOfMemoryException,
            InternalErrorException)

                : m_is_attr_initialised(false),
                m_is_mutex_initialised(false),
                m_is_locked(false),
                m_locker_thread(0)

        {
            init_default_attrMutex();
            init_default_mutex();

            CDMW_ASSERT(m_is_attr_initialised == true);
            CDMW_ASSERT(m_is_mutex_initialised == true);

        }



        //
        // Dtor
        //
        MutexImpl::~MutexImpl()
        throw()
        {
#   ifdef CDMW_POSIX_THREAD
            // FIXME: check if the mutex is not locked, if yes we get
            //    undefined behaviour, if we destroy it.
            // There is no check for status, because:
            //   - this function must not failed
            //   - if this function failed, we can do nothing
            //      as we are in a destructor, if we throw,
            //      the terminate() function will be called
            //      and the process may crash, so we ignore
            //      consistency check
            //     One solution will be the user explicitly
            //      calls destroy on the mutex, via a method
            //      and this method could thrown exception

            if (m_is_mutex_initialised)
            {
                ::pthread_mutex_destroy (&m_mutex);
            }

            if (m_is_attr_initialised)
            {
                ::pthread_mutexattr_destroy (&m_attrMutex);
            }

#   elif defined(_WIN32)

            // Same discussion as posix
            ::CloseHandle (m_mutex);

#   else
#       error "CDMW: not supported platform"
#   endif


        }



        //
        //  Do the default attributes initialisation
        //
        void
        MutexImpl::init_default_attrMutex()
        throw (OutOfMemoryException,
               InternalErrorException)
        {

            CDMW_ASSERT(m_is_attr_initialised == false);
            CDMW_ASSERT(m_is_mutex_initialised == false);

#    if defined(CDMW_POSIX_THREAD)

            int status_init_attr = ::pthread_mutexattr_init (&m_attrMutex);

            // Possible status are : 0, ENOMEM

            switch (status_init_attr)
            {

                    case 0:
                    break;

                    // No more memory

                    case ENOMEM:
                    CDMW_THROW (OutOfMemoryException);
                    break;

                    // Panic

                    default:
                    CDMW_THROW (InternalErrorException);
                    break;

            };

            /*--------------------------------------------------------------------
              The following code use the native Solaris approach to detect deadlock
              on non recursive mutex, but the Solaris thread implementation is broken
              There is an (escalated) Solaris 7 bug report about this problem: 

                  Bugid: 4288299
                  Category: library
                  Subcategory: libthread
                  Synopsis: Recursive mutexes are not properly released    
              The patch installed on the host doesn't seems to work. So
              a workaround has been coded.
            --------------------------------------------------------------------
              
            // The mutex is not recursive, we ask to get an error, if the
            //    same thread try to lock twice the mutex.
            int status_attr_settype = ::pthread_mutexattr_settype( 
                                                    &m_attrMutex,
                                                    PTHREAD_MUTEX_ERRORCHECK);
            // Possible status: 0, EINVAL
            //  as the only value set are defined constant, if we get an error,
            //  we panic.                               
            switch (status_attr_settype) {
                
                case 0:
                    break;
                
                case EINVAL:
                    // No break
                  
                default:
                    CDMW_THROW (InternalErrorException);
                    break;

            };
            --------------------------------------------------------------------
            --------------------------------------------------------------------*/

            m_is_attr_initialised = true;

#    elif defined(_WIN32)

            // Nothing to do
            m_is_attr_initialised = true;

#    else
#        error "Cdmw: Implementation not available"
#    endif


            CDMW_ASSERT(m_is_attr_initialised == true);

            CDMW_ASSERT(m_is_mutex_initialised == false);
        }




        //
        //  Do the default mutex os dependant initialisation
        //
        void
        MutexImpl::init_default_mutex()
        throw (OutOfMemoryException,
               InternalErrorException)
        {
            CDMW_ASSERT(m_is_attr_initialised == true);
            CDMW_ASSERT(m_is_mutex_initialised == false);

#   if defined(CDMW_POSIX_THREAD)

            int status_create_mutex = ::pthread_mutex_init (&m_mutex, &m_attrMutex);

            // The only possible error are parameter invalid, as we use
            //    default parameter (NULL), we can't get error

            switch (status_create_mutex)
            {

                    case 0:
                    break;

                    case EINVAL:
                    // No break;

                    default:
                    CDMW_THROW (InternalErrorException);
                    break;

            };

            m_is_mutex_initialised = true;

#   elif defined(_WIN32)

            m_mutex = ::CreateMutex (NULL, false, NULL);

            if (m_mutex == NULL)
            {
                switch (::GetLastError())
                {

                        // We don't name the mutex, so we can't get it

                        case ERROR_INVALID_HANDLE:
                        // No break

                        default:

                        CDMW_THROW (InternalErrorException);
                        break;

                }
            }

            m_is_mutex_initialised = true;

#   else
#        error "Cdmw: Implementation not available"
#   endif

            CDMW_ASSERT(m_is_attr_initialised == true);
            CDMW_ASSERT(m_is_mutex_initialised == true);

        }




        //
        // Lock the mutex
        //
        void

        MutexImpl::lock ()
            throw (AlreadyDoneException,
                   InternalErrorException)
        {
            CDMW_ASSERT(m_is_mutex_initialised == true);

            Thread::ThreadId calling_thread = Thread::self();

            // if the mutex is already locked, we must check
            //    if the same thread is not trying to lock twice

            if (m_is_locked == true)
            {

                // If the thread has already locked the mutex we throw
                //    an exception
                // If another thread has locked, we cannot have a deadlock
                //    and we cannot have concurency with ourself

                if (m_locker_thread == calling_thread)
                {
                    CDMW_THROW(AlreadyDoneException);
                }
            }


#   if defined(CDMW_POSIX_THREAD)

            int status_mutex_lock = ::pthread_mutex_lock (&m_mutex);


            // Possible status are : 0, EDEADLK
            switch (status_mutex_lock)
            {

                    case 0:
                    break;

                    case EDEADLK:

                    // The thread has already lock the mutex
                    // TODO: maybe an AlreadyDoneException
                    CDMW_ASSERT (false == true);
                    break;

                    default:
                    CDMW_THROW (InternalErrorException);
                    break;
            };

            // We update our internal state, we become lock
            set_lock();


#   elif defined(_WIN32)

            DWORD status_mutex_lock = ::WaitForSingleObject (m_mutex, INFINITE);

            switch (status_mutex_lock)
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
                    CDMW_THROW (InternalErrorException);
                    break;


            }

            // We update our internal state, we become lock
            set_lock();

#   else
#        error "Cdmw: Implementation not available"
#   endif

        }



        //
        // Used by the implementation (ConditionImpl), to change the debug
        //    state of the Mutex, without really changing the state
        //
        void
        MutexImpl::set_lock()
        throw()

        {
            CDMW_ASSERT(m_is_mutex_initialised == true);

            // In this part we are protected by the mutex and we are alone
            CDMW_ASSERT(m_is_locked == false);

            // We note the thread which has locked the mutex
            m_locker_thread = Thread::self();
            m_is_locked = true;
        }



        //
        // Unlock the mutex
        //
        void
        MutexImpl::unlock()
        throw (PermissionDeniedException,
               InternalErrorException)

        {
            CDMW_ASSERT(m_is_mutex_initialised == true);

            Thread::ThreadId calling_thread = Thread::self();

            // if the mutex is unlocked, throw an exception
            // if the mutex is locked, we must check if mutex belongs to thread

            if (m_is_locked == false || calling_thread != m_locker_thread)
            {
                CDMW_THROW(PermissionDeniedException);
            }

            // We update our internal state, we become unlock
            set_unlock();


#   if defined(CDMW_POSIX_THREAD)

            int status_mutex_unlock = ::pthread_mutex_unlock(&m_mutex);

            // Possible status are : 0, EPERM
            switch (status_mutex_unlock)
            {

                    case 0:
                    break;

                    // A thread which has not lock the mutex try to unlock it

                    case EPERM:
                    CDMW_ASSERT (false == true);
                    break;

                    default:
                    CDMW_THROW (InternalErrorException);
                    break;
            };

#   elif defined(_WIN32)

            BOOL status_mutex_unlock = ::ReleaseMutex (m_mutex);

            // Possible status are : 0, EPERM
            if ( status_mutex_unlock == 0 )
            {

                switch (::GetLastError())
                {

                        case 0:
                        break;

                        // A thread which has not lock the mutex try to unlock it

                        case ERROR_NOT_OWNER:
                        CDMW_NEVER_HERE();
                        break;

                        default:
                        CDMW_THROW (InternalErrorException);
                        break;
                };
            }


#   else
#        error "Cdmw: Implementation not available"
#   endif

        }

        //
        // is_locked()
        //
        bool
        MutexImpl::is_locked()
        throw ()
        {
            return m_is_locked;
        }



        //
        // Used by the implementation (ConditionImpl), to change the debug
        //    state of the Mutex, without really changing the state
        //
        void
        MutexImpl::set_unlock()
        throw()
        {
            CDMW_ASSERT(m_is_mutex_initialised == true);

            // In this part we are protected by the mutex and we are alone
            CDMW_ASSERT(m_is_locked == true);
            m_is_locked = false;
        }



#if defined(CDMW_POSIX_THREAD)

        //
        // Used by the implementation (ConditionImpl), to retreive the
        //    OS dependent Mutex
        //
        pthread_mutex_t*
        MutexImpl::get_pthread_mutex()
        throw ()
        {
            return &m_mutex;
        }


#endif

#if defined(_WIN32)
        HANDLE
        MutexImpl::get_win32_mutex()
        throw()
        {
            return m_mutex;
        }

#endif
    } // End namespace osthreads

} // End namespace Cdmw


