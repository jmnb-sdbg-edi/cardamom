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


#include "Foundation/osthreads/Barrier.hpp"

#include "Foundation/osthreads/MutexGuard.hpp"


namespace Cdmw
{

    namespace OsSupport
    {

        /**
        *Implementation notes:  none 
        *<p>
        *Portability issues: none
        *<p>
        */


        //
        // Ctor
        //
        Barrier::Barrier (int count)
        throw (OutOfMemoryException,
               InternalErrorException)

                : m_count_mutex(),
                m_condition(m_count_mutex),
                m_count(count)
        {
        }



        //
        // Dtor
        //
        Barrier::~Barrier()
        throw()
        {
            // Nothing to do
        }




        //
        // Block the thread until all thread reach the Barrier
        //
        void
        Barrier::wait()
        throw (OutOfMemoryException,
               InternalErrorException)
        {
            try
            {
                CDMW_MUTEX_GUARD(m_count_mutex);

                // There is one less thread to be waited
                m_count--;

                // If there is no more thread we unlock all others thread

                if ( m_count == 0 )
                {
                    m_condition.broadcast();

                    // Else we wait all others threads
                }
                else
                {
                    m_condition.wait();
                }

                // The Thread must be blocked on wait, so it can not lock
                //    twice the mutex, we have a bug or we are corrupted
            }
            catch (const AlreadyDoneException&)
            {

                CDMW_THROW (InternalErrorException);

            }
        }


        Barrier::WaitReturn
        Barrier::wait (size_t milliseconds)
        throw (OutOfMemoryException,
               InternalErrorException)
        {
            try
            {
                Barrier::WaitReturn returnedStatus = Barrier::REACHED;

                CDMW_MUTEX_GUARD(m_count_mutex);

                // There is one less thread to be waited
                m_count--;

                // If there is no more thread we unlock all others thread

                if (m_count == 0)
                {
                    m_condition.broadcast();
                }

                // Else we wait all others threads
                else
                {
                    Condition::WaitReturn status = m_condition.wait(milliseconds);

                    if (status == Condition::TIMEDOUT)
                    {
                        returnedStatus = Barrier::TIMEDOUT;
                    }
                }

                return returnedStatus;

                // The Thread must be blocked on wait, so it can not lock
                //    twice the mutex, we have a bug or we are corrupted
            }
            catch (const AlreadyDoneException&)
            {

                CDMW_THROW (InternalErrorException);

            }
        }



    } // End namespace OsSupport
} // End namespace Cdmw

