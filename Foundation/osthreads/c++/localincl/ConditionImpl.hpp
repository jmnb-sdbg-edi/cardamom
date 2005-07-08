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


#ifndef INCL_OSSUPPORT_CONDITIONIMPL_HPP
#define INCL_OSSUPPORT_CONDITIONIMPL_HPP

#ifdef CDMW_POSIX_THREAD

#   include <pthread.h>

#elif defined(_WIN32)

#   include <windows.h>

#endif
#include <memory>

#include "Foundation/osthreads/Condition.hpp"

#include "osthreads/MutexImpl.hpp"

namespace Cdmw
{

    namespace OsSupport
    {

        /**
        *Purpose:
        *<p>    Implementation of the Condition class with
        *        compilation firewall idiom (pimpl)
        *
        *Features:
        *<p> Thread safe, exception safe
        *
        *@see  Condition 
        *@see  Mutex
        *@see  MutexImpl
        *
        */

        class ConditionImpl
        {

            public:

                /**
                * Purpose:
                * <p> Construct the implementation
                * 
                *@param the implementation of the associated Mutex
                *
                *@exception OutOfMemoryException
                *@exception InternalErrorException 
                */
                explicit
                ConditionImpl(MutexImpl& mutexImpl)
                throw( OutOfMemoryException,
                       InternalErrorException );


                /**
                * Purpose:
                * <p> Destructs the condition. See discussion
                *   about the violation of "A destructor must never
                *    throw", in Condition header.
                * 
                *@exception AssertionFailedException
                */
                virtual
                ~ConditionImpl()
                throw( AssertionFailedException );



                /**
                * Purpose:
                * <p> Blocks the calling thread until another
                *  thread call broadcast.
                * 
                *
                *@exception AssertionFailedException
                *@exception InternalErrorException 
                */
                void
                wait()
                throw( AssertionFailedException,
                       InternalErrorException);



                /**
                * Purpose:
                * <p> Blocks the calling thread until another
                *  thread call broadcast, or the timeout expires
                *
                *@param milliseconds timeout before returning although, no
                *   thread have call the broadcast method.

                *@return Condition::TIMEDOUT, if the timeout has expires, 
                *    Condition::SIGNALLED elsewhere.
                *
                *@exception AssertionFailedException
                *@exception InternalErrorException
                */
                Condition::WaitReturn
                wait(size_t milliseconds)
                throw ( AssertionFailedException,
                        InternalErrorException);




                /**
                * Purpose:
                * <p> This function exists but is not accesible from the handle part.
                * In the POSIX specification the signal() function, awake AT LEAST
                * on thread (p. 262-264, 525-528). So the signal may awake more than 
                * one thread. This function is generally provided as an optimisation
                * if not all threads need to be awaken. After test under solaris more
                * than one thread are awaken. I found this behaviour dangerous. If a 
                * program runs right with broadcast(), it will run right with signal()
                * so I decide to not provide signal(), but to let the implementation
                * in place, if possible optimisation are needed in the future.
                * (10/24/2000)
                * 
                *@param
                *@return
                *@exception
                */
                void
                signal()
                throw ( InternalErrorException);


                /**
                * Purpose:
                * <p> Unblocks all threads waiting on wait()s call.
                * 
                *@exception InternalErrorException
                */
                void
                broadcast()
                throw ( InternalErrorException);


            protected:

                /**
                * Purpose:
                * <p> Initialise the underlying operating system dependent 
                * condition attributes
                *
                *@exception OutOfMemoryException
                *@exception InternalErrorException
                */
                void
                init_default_attrCondition()
                throw ( OutOfMemoryException,
                        InternalErrorException);

                /**
                * Purpose:
                * <p> Initialise the underlying operating system dependent condition
                *
                *@exception OutOfMemoryException
                *@exception InternalErrorException
                */
                void
                init_condition()
                throw ( OutOfMemoryException,
                        InternalErrorException);


            private:
                //
                // Copy ctor
                //
                ConditionImpl(const ConditionImpl& rhs)
                throw();


                //
                // Assignement operator
                //
                ConditionImpl&
                operator=(const ConditionImpl& rhs)
                throw();



                /**
                * Mutex implementation associated with the condition
                */
                MutexImpl& m_mutexImpl;

#       ifdef CDMW_POSIX_THREAD


                /**
                * Condition of the underlying operating system
                */
                pthread_cond_t m_pthread_cond;


                /**
                * attributes of the condition
                */
                pthread_condattr_t m_pthread_cond_attr;

#       elif defined(_WIN32)

                /**
                * WIN32 Event used to signal a change on the data
                */
                HANDLE m_event;


#       else
#           error "Cdmw: Implementation not available"
#       endif


                /**
                * Define if the Condition attributes are initialised
                * and so can be used.
                */
                bool m_is_attr_initialised;



                /**
                * Define if the Condition is initialised
                * and so can be used.
                */
                bool m_is_condition_initialised;

        }

        ; // End class ConditionImpl

    } // End namespace osthreads
} // End namespace Cdmw

#endif // INCL_OSSUPPORT_CONDITIONIMPL_HPP

