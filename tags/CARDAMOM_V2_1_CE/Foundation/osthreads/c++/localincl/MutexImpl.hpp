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


#ifndef INCL_OSSUPPORT_MUTEXIMPL_HPP
#define INCL_OSSUPPORT_MUTEXIMPL_HPP


#include "Foundation/common/MacroDefs.h"


#if defined(CDMW_POSIX_THREAD)

#    include <pthread.h>

#elif defined(_WIN32)

#    include <windows.h>

#else
#    error "Cdmw: Implementation not available"
#endif

#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/Thread.hpp"


namespace Cdmw
{

    namespace OsSupport
    {


        /**
        *Purpose:
        *<p>    Implementation of the Mutex class with
        *        compilation firewall idiom (pimpl)
        *
        *Features:
        *<p> Thread safe, exception safe
        *
        *@see  Condition_impl 
        *@see  Mutex
        *
        */

        class MutexImpl
        {

            public:

                //
                // Ctor
                //
                MutexImpl()
                throw (
                    OutOfMemoryException,
                    InternalErrorException);



                //
                // Dtor
                //
                virtual ~MutexImpl()
                throw();




                /**
                * Purpose:
                * <p> Lock the Mutex, 
                *
                * @see Mutex::lock
                *
                * @exception AlreadyDoneException the mutex is already locked
                *            by the calling thread.
                * @exception InternalErrorException 
                */
                void

                lock ()
                    throw (AlreadyDoneException,
                           InternalErrorException);



                /**
                * Purpose:
                * <p> unlock the Mutex, 
                *
                * @see Mutex::unlock 
                *
                * @exception PermissionDeniedException 
                * @exception InternalErrorException 
                */
                void
                unlock()
                throw (PermissionDeniedException,
                       InternalErrorException);


                /**
                * Purpose:
                * <p> is_locked
                *
                * @see Mutex::is_locked
                *
                */
                bool
                is_locked()
                throw ();


            protected:


                /**
                * Purpose:
                * <p> Change the internal state to unlocked, without
                *  really changing its state. Also used by ConditionImpl.
                *
                */
                void set_unlock()
                throw() ;



                /**
                * Purpose:
                * <p> Change the internal state to locked, without
                *  really changing its state. Also used by ConditionImpl.
                *
                */
                void set_lock()
                throw();



            private:

                /**
                * Condition is allowed to update internal information
                * about the lock state, because OS API, wait and lock/unlock
                * the system mutex in an atomic way. So before doing this
                * operation the Condition must update the MutexImpl internal
                * lock/unlock state.
                */

                friend class ConditionImpl;


                //
                // No Copy Ctor
                //
                MutexImpl( const MutexImpl& );



                /**
                * Purpose:
                * <p> Initialises with default value the attribute 
                *  of the mutex.
                *
                * @exception OutOfMemoryException
                * @exception InternalErrorException     
                */
                void init_default_attrMutex()
                throw (
                    OutOfMemoryException,
                    InternalErrorException);



                /**
                * Purpose:
                * <p> Initialises with default value the mutex
                * @exception OutOfMemoryException
                * @exception InternalErrorException     
                */
                void init_default_mutex()
                throw (
                    OutOfMemoryException,
                    InternalErrorException);


#       if defined(CDMW_POSIX_THREAD)

                /**
                * Purpose:
                * <p> Returns the Os dependent mutex, use by others
                * part of the implementation to use OS APIs.
                *
                */
                pthread_mutex_t*
                get_pthread_mutex()
                throw() ;



                /**
                * True POSIX mutex used to lock/unlock
                */
                pthread_mutex_t m_mutex;



                /**
                * Attribute used to initialize the mutex
                */
                pthread_mutexattr_t m_attrMutex;

#       endif // defined(CDMW_POSIX_THREAD)


#       if defined(_WIN32)

                /**
                * Purpose:
                * <p> Returns the Os dependent mutex, use by others
                * part of the implementation to use OS APIs.
                *
                */
                HANDLE
                get_win32_mutex()
                throw() ;



                /**
                * True WIN32 mutex used to lock/unlock
                */
                HANDLE m_mutex;



                /**
                * Not used on win32
                * Attribute used to initialize the mutex
                */ 
                //LPSECURITY_ATTRIBUTES  m_attrMutex;

#       endif // defined(CDMW_POSIX_THREAD)



                /**
                * Define if the Mutex attributes are initialised
                * and so can be used.
                */
                bool m_is_attr_initialised;



                /**
                * Define if the Mutex is initialised
                * and so can be used.
                */
                bool m_is_mutex_initialised;



                /**
                * True if the mutex is already locked.
                * this value is only used to detect recursive
                * lock from the same thread.
                */
                bool m_is_locked;



                /**
                * Id of the thread which has locked the mutex
                */
                Thread::ThreadId m_locker_thread;

        };


    } // End namespace osthreads

} // End namespace Cdmw


#endif // INCL_OSSUPPORT_MUTEXIMPL_HPP

