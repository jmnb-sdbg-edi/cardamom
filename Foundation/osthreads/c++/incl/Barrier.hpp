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


#ifndef INCL_OSSUPPORT_BARRIER_HPP
#define INCL_OSSUPPORT_BARRIER_HPP

#include <cstddef>

#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Condition.hpp"

namespace Cdmw
{

    namespace OsSupport
    {

        /**
        *Purpose:
        *<p>    Allows several thread to wait each thread
        *       reach a point. The Barrier is initialised with
        *       the number of thread to be waited, each thread
        *       is blocked in the wait() call until the number
        *       thread have call wait(). When all thread
        *       are waiting on the wait(), all thread are unblocked
        *
        *Features:
        *<p> 
        *
        *@see  Condition 
        *
        */

        class Barrier
        {

            public:

                /**
                * Purpose:
                * <p> Initialise the barrier with the total
                *      number of thread to be waited.
                * 
                *@param count number of thread to be waited
                *
                *@exception OutOfMemoryException
                *@exception InternalErrorException
                */
                explicit
                Barrier (int count)
                throw (OutOfMemoryException,
                       InternalErrorException);


                //
                // Dtor
                //
                virtual
                ~Barrier()
                throw();


                /**
                * Purpose:
                * <p> Blocks the calling thread until the number
                *     of thread specified in the constructor
                *     have called a wait().
                * 
                *@exception OutOfMemoryException
                *@exception InternalErrorException 
                */
                void
                wait()
                throw (OutOfMemoryException,
                       InternalErrorException);


                /**
                * When a timeout is specified in a wait call,
                * the wait may return: TIMEDOUT when the specified
                * timeout has elapsed but all thread have not reached
                * the Barrier, REACHED the thread is unblocked because
                * all the thread have reached the Barrier.
                */
                enum WaitReturn {
                    TIMEDOUT,
                    REACHED
            };


                /**
                * Purpose:
                * <p> Blocks the calling thread until the number
                *     of thread specified in the constructor
                *     have called a wait() function. If all the thread
                *     have not reached the Barrier before milliseconds,
                *     the calling thread is unblock, and wait() returns
                *     TIMEDOUT. Although the calling thread is unblock
                *     the barrier counter keeps the same.
                * 
                *@param milliseconds time expressed in milli-seconds before
                *       returning although all threads are not on the Barrier
                *
                *@return REACHED if all the thread have reached the Barrier
                *        TIMEDOUT if the time has expired.
                *        
                *@exception OutOfMemoryException
                *@exception InternalErrorException 
                */
                Barrier::WaitReturn
                wait (size_t milliseconds)
                throw (OutOfMemoryException,
                       InternalErrorException);

            protected:


            private:

                //
                // Copy Ctor
                //
                Barrier(const Barrier& rhs)
                throw();



                //
                // Assignement operator
                //
                Barrier&
                operator=(const Barrier& rhs)
                throw();


                /**
                * Protect the variable containing the number of thread not
                * already on the Barrier.
                */
                Mutex m_count_mutex;


                /**
                * Condition on which threads are waiting
                */
                Condition m_condition;


                /**
                * Number of thread to be wait before releasing all threads.
                */
                int m_count;



        }

        ; // End class Barrier

    } // End namespace OsSupport
} // End namespace Cdmw

#endif // INCL_OSSUPPORT_BARRIER_HPP

