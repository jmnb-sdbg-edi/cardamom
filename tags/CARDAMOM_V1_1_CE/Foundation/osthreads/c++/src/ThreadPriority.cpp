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


#include "osthreads/ThreadPriority.hpp"

#include <sstream>


namespace Cdmw
{

    namespace OsSupport
    {

        // Thread Priority base class to be derived according to OS

        //
        // Ctor
        //
        ThreadPriorityBase::ThreadPriorityBase()
        throw ()
                : m_maxPriority (0),
                m_minPriority (0)
        {}

        //
        // Dtor
        //
        ThreadPriorityBase::~ThreadPriorityBase()
        throw ()
        {}

        //
        // Transform native priority to Cdmw thread priority
        //
        Thread::Priority ThreadPriorityBase::fromNativePriority (int nativePriority)
        throw (BadParameterException)
        {
            return nativePriority;
        }

        //
        // Transform Cdmw thread priority to native priority
        //
        int ThreadPriorityBase::toNativePriority (Thread::Priority priority)
        throw (BadParameterException)
        {
            return priority;
        }

        //
        // Get the Max Cdmw thread priority
        //
        Thread::Priority ThreadPriorityBase::getMaxPriority ()
        throw ()
        {
            return m_maxPriority;
        }


        //
        // Get the Min Cdmw thread priority
        //
        Thread::Priority ThreadPriorityBase::getMinPriority ()
        throw ()
        {
            return m_minPriority;
        }


        //
        // Set the Max Cdmw thread priority
        //
        void ThreadPriorityBase::setMaxPriority (Thread::Priority priority)
        throw (BadParameterException)
        {
            m_maxPriority = priority;
        }


        //
        // Set the Min Cdmw thread priority
        //
        void ThreadPriorityBase::setMinPriority (Thread::Priority priority)
        throw (BadParameterException)
        {
            m_minPriority = priority;
        }



#if defined(CDMW_POSIX_THREAD)

        // Thread Priority class for POSIX thread

        //
        // Ctor
        //
        ThreadPriority::ThreadPriority ()
        throw ()
        {}

        //
        // Dtor
        //
        ThreadPriority::~ThreadPriority ()
        throw ()
        {}

        //
        // Transform native Posix priority to Cdmw thread priority
        //
        Thread::Priority ThreadPriority::fromNativePriority (int nativePriority)
        throw (BadParameterException)
        {
            return nativePriority;
        }

        //
        // Transform Cdmw thread priority to native Posix priority
        //
        int ThreadPriority::toNativePriority (Thread::Priority priority)
        throw (BadParameterException)
        {
            return priority;
        }

        //
        // Set the Max Cdmw thread priority
        //
        void ThreadPriority::setMaxPriority (int nativePriority)
        throw (BadParameterException)
        {
            Thread::Priority priority = fromNativePriority (nativePriority);
            ThreadPriorityBase::setMaxPriority (priority);
        }


        //
        // Set the Min Cdmw thread priority
        //
        void ThreadPriority::setMinPriority (int nativePriority)
        throw (BadParameterException)
        {
            Thread::Priority priority = fromNativePriority (nativePriority);
            ThreadPriorityBase::setMinPriority (priority);
        }

#elif defined (_WIN32)


        // Thread Priority class for Win32 thread

        //
        // Ctor
        //
        ThreadPriority::ThreadPriority ()
        throw ()
        {
            // initialize the priority mapping table
            m_mappingTable[0] = 0;            // unused
            m_mappingTable[1] = THREAD_PRIORITY_ABOVE_NORMAL;
            m_mappingTable[2] = THREAD_PRIORITY_BELOW_NORMAL;
            m_mappingTable[3] = THREAD_PRIORITY_HIGHEST;
            m_mappingTable[4] = THREAD_PRIORITY_IDLE;
            m_mappingTable[5] = THREAD_PRIORITY_LOWEST;
            m_mappingTable[6] = THREAD_PRIORITY_NORMAL;
            m_mappingTable[7] = THREAD_PRIORITY_TIME_CRITICAL;

            // set thread max priority
            ThreadPriorityBase::setMaxPriority(THR_MAX_PRIORITY);

            // set thread min priority
            ThreadPriorityBase::setMinPriority(THR_MIN_PRIORITY);
        }

        //
        // Dtor
        //
        ThreadPriority::~ThreadPriority ()
        throw ()
        {}


        //
        // Transform native Win32 priority to Cdmw thread priority
        //
        Thread::Priority ThreadPriority::fromNativePriority (int nativePriority)
        throw (BadParameterException)
        {
            Thread::Priority priority;

            bool found = false;

            for (int i = THR_MIN_PRIORITY ; i <= THR_MAX_PRIORITY && found == false ; i++)
            {
                if (m_mappingTable[i] == nativePriority)
                {
                    priority = i;
                    found = true;
                }
            }

            if (found == false)
            {
                std::stringstream buffer;
                buffer << nativePriority << std::ends;
                CDMW_THROW2(BadParameterException, "Native Priority : ", buffer.str());
            }

            return priority;
        }

        //
        // Transform Cdmw thread priority to native Win32 priority
        //
        int ThreadPriority::toNativePriority (Thread::Priority priority)
        throw (BadParameterException)
        {
            int native_priority;

            if (priority >= THR_MIN_PRIORITY && priority <= THR_MAX_PRIORITY)
            {
                native_priority = m_mappingTable[priority];
            }
            else
            {
                std::stringstream buffer;
                buffer << priority << std::ends;
                CDMW_THROW2(BadParameterException, "Thread::Priority : ", buffer.str());
            }

            return native_priority;
        }


#else
#   error "Cdmw: Implementation not available"
#endif

    } // End namespace osthreads

}  // End namespace Cdmw



