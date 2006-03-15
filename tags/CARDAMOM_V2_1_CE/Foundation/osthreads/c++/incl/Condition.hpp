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


#ifndef INCL_OSSUPPORT_CONDITION_HPP
#define INCL_OSSUPPORT_CONDITION_HPP

#include <cstddef>

#include "Foundation/osthreads/Mutex.hpp"

namespace Cdmw
{

    namespace OsSupport
    {

        class ConditionImpl;

        /**
        *Purpose:
        *<p>    The Condition is used by one or more thread to wait
        * for a change signalled by another thread. The Condition is 
        * associated to a Mutex. This Mutex is used to protect against 
        * concurrent access, the data whose value must be monitored.
        * When a thread want to wait for a change, it must acquire the
        * Mutex, checking if the data satisfies or not its condition.
        * If the thread wants to wait for a change, it must use a wait()
        * call. The associated mutex is released and the thread waits
        * in an atomic operation. When the thread leaves the wait() call,
        * the Mutex has been re-acquired, so the Thread can safely
        * consult and the modify associated data.
        *
        *Features:
        *<p> Thread safe, exception safe
        *
        *@see  Thread 
        *@see  Mutex
        *
        */

        class Condition
        {

            public:

                /**
                * Purpose:
                * <p> Construct a condition associated with the
                *  mutex protecting data, whose change are
                *  signalling using this Condition object.
                * 
                *@param mutex protecting associated data
                *
                *@exception OutOfMemoryException
                *@exception InternalErrorException  
                */
                explicit
                Condition( Mutex& mutex )
                throw( OutOfMemoryException,
                       InternalErrorException);


                /**
                * Purpose:
                * <p> Destructor 
                */
                virtual
                ~Condition()
                throw();






                /**
                * Purpose:
                * <p> When a thread calls this method, it is blocked
                * until another thread call broadcast(), to signal that
                * data protected by this condition have changed.
                * 
                *@exception InternalErrorException
                */
                void
                wait()
                throw( InternalErrorException);


                /**
                * Values returned by the timed wait.
                * SIGNALLED: the condition has been signalled by 
                * another thread.
                * TIMEDOUT: the specified time has elapsed before 
                * the condition goes signalled.
                */
                enum WaitReturn {
                    SIGNALLED,
                    TIMEDOUT
            };



                /**
                * Purpose:
                * <p> When a thread calls this method, it is blocked
                * until another thread calls broadcast(), to signal that
                * data protected by this condition has changed or that
                * the timeout specified has expired.
                * 
                *@param milliseconds tiemout expressed in milliseconds
                *  if no thread has called broadcast(), during this delay
                *  the method returns TIMEDOUT. 
                * 
                *@return TIMEDOUT if the timeout has expired, SIGNALLED if
                *  the method return because a thread has called the broadcast
                *  method.
                *
                *@exception InternalErrorException
                */
                WaitReturn
                wait( size_t milliseconds )
                throw ( InternalErrorException);



                /**
                * Purpose:
                * <p> When a thread has changed the data protected by this
                *  condition, it notifies all waiting threads by calling
                *  broadcast().
                * 
                *@exception InternalErrorException
                */
                void
                broadcast()
                throw ( InternalErrorException);

                /**
                * Purpose:
                * <p> Allow to retreive the mutex associated with the
                *  condition.
                * 
                *@return the Mutex associated with the Condition
                */
                Mutex&
                get_mutex()
                throw ();



            protected:


            private:

                //
                // Copy ctor
                //
                Condition(const Condition& rhs)
                throw();



                //
                // Assignement operator
                //
                Condition&
                operator=(const Condition& rhs)
                throw();


                /**
                * Implementation of the Condition using 
                * the compiler firewall idiom (pimpl) 
                * 
                */
                std::auto_ptr<ConditionImpl> m_spImpl;

                /**
                * Mutex associated with the condition.
                */
                Mutex& m_mutex;

        }

        ; // End class Condition

    } // End namespace osthreads
} // End namespace Cdmw

#endif // INCL_OSSUPPORT_CONDITION_HPP

