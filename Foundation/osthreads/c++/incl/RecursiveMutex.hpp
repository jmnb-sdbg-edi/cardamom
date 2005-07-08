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


#ifndef INCL_OSSUPPORT_RECURSIVE_MUTEX_HPP
#define INCL_OSSUPPORT_RECURSIVE_MUTEX_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/Thread.hpp"
#include "Foundation/osthreads/AbstractMutex.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Condition.hpp"

namespace Cdmw
{

    namespace OsSupport
    {

        class MutexImpl;


        /**
        *Purpose:
        *<p> Implementation of a recursive mutex.
        *
        * A thread that has already acquired a recursive mutex will not be blocked if it
        * locks again this recursive mutex. This thread is responsible for unlocking the
        * recursive mutex as many times as it has locked the mutex. Otherwise, the mutex
        * will remain locked definitively.
        *
        *
        *Features:
        *<p> Thread safe, exception safe
        *
        *@see  Mutex
        *@see  Condition 
        *
        */

        class RecursiveMutex : public AbstractMutex
        {

            public:

                /**
                * Purpose:
                * <p> construct the mutex
                *
                *@exception OutOfMemoryException
                *@exception InternalErrorException      
                */
                RecursiveMutex()
                throw (OutOfMemoryException, InternalErrorException);



                //
                // Dtor
                //
                virtual
                ~RecursiveMutex()
                throw() ;


                /**
                * Purpose:
                * <p> Locks in an atomic way the Mutex.
                * If the mutex is already acquired by another thread, the thread
                * waits until it is get unlocked.
                * If the mutex is already acquired by the same thread, the lock
                * count is simply incremented.
                *
                * @exception InternalErrorException 
                */

                virtual void lock ()
                    throw (InternalErrorException);


                /**
                * Purpose:
                * <p> Decrements the lock count and unlocks the mutex when
                * the lock count equals 0.
                *
                * @exception PermissionDeniedException if the calling thread 
                * is not the owner of the mutex.
                * @exception InternalErrorException
                */
                virtual void unlock()
                throw (PermissionDeniedException, InternalErrorException);



            protected:


            private:

                //
                // Copy Ctor
                //
                RecursiveMutex( const RecursiveMutex&);



                //
                // Assignment operator
                //
                RecursiveMutex&
                operator=( const RecursiveMutex&);

                /**
                * The monitor for recursive lock.
                */
                Mutex m_monitor;

                /**
                * Condition indicating whether the recursive mutex is available
                * for locking.
                */
                Condition m_available;

                /**
                * The count of locks acquired by the owner thread.
                */
                unsigned long m_lockCount;

                /**
                * The owner of this recursive mutex if any.
                */
                Thread::ThreadId m_owner;

        };


    } // End namespace osthreads
} // End namespace Cdmw



#endif //INCL_OSSUPPORT_RECURSIVE_MUTEX_HPP

