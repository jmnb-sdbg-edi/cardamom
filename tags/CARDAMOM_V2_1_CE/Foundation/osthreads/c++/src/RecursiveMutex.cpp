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


#include "Foundation/osthreads/RecursiveMutex.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"


namespace Cdmw
{

    namespace OsSupport
    {

        //
        // Ctor
        //
        RecursiveMutex::RecursiveMutex()
        throw (OutOfMemoryException, InternalErrorException)
                : m_available(m_monitor), m_lockCount(0)
        {}



        //
        // Dtor
        //
        RecursiveMutex::~RecursiveMutex() throw()
        {
            // Nothing to do
        }




        //
        // Locks the mutex
        //
        void

        RecursiveMutex::lock ()
            throw (InternalErrorException)
        {

            CDMW_MUTEX_GUARD(m_monitor);

            if (m_lockCount > 0)
            {
                // The mutex is already locked

                if (m_owner != Thread::self())
                {
                    // The thread is not the owner
                    m_available.wait();

                }

            }

            // sets the owner
            m_owner = Thread::self();

            ++m_lockCount;

        }



        //
        // Unlocks the mutex
        //
        void
        RecursiveMutex::unlock()
        throw (PermissionDeniedException, InternalErrorException)
        {
            unsigned long lockCount;

            {
                CDMW_MUTEX_GUARD(m_monitor);

                if (m_lockCount > 0)
                {
                    // The mutex is already locked

                    if (m_owner != Thread::self())
                    {
                        // not owner
                        CDMW_THROW(PermissionDeniedException);
                    }

                    --m_lockCount;
                    lockCount = m_lockCount;
                }
                else
                {
                    // not yet locked
                    CDMW_THROW(InternalErrorException);
                }

            }

            if (lockCount == 0)
                m_available.broadcast();

        }


    } // End namespace OsSupport
} // End namespace Cdmw



