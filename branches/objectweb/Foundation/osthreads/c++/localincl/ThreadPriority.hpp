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


#ifndef CDMW_OSSUPPORT_THREAD_PRIORITY_HPP
#define CDMW_OSSUPPORT_THREAD_PRIORITY_HPP

#include "Foundation/common/MacroDefs.h"               // To know the current target
#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Exception.hpp"

#include "Foundation/osthreads/Thread.hpp"

#if defined(CDMW_POSIX_THREAD)

#include <pthread.h>

#elif defined (_WIN32)

#include <windows.h>

#else
#   error CDMW_IMPL_NOT_AVAILABLE
#endif



namespace Cdmw
{

    namespace OsSupport
    {

        /**
        *Purpose:
        *<p>    Implementation of the ThreadPriorityBase class
        *       This class is defined as base class to map OS thread native priority
        *
        *Features:
        *<p> Thread safe, exception safe
        *
        *@see  Thread
        *
        */

        class ThreadPriorityBase
        {

            public:

                /**
                * Purpose:
                * <p> Construct the base class
                * 
                *@param
                *
                *@exception
                */

                ThreadPriorityBase()
                throw ();

                //
                // Dtor
                //
                ~ThreadPriorityBase()
                throw();



                /**
                * Purpose:
                * <p> Transform native priority to Cdmw thread priority
                *
                * @exception BadParameterException
                */
                Thread::Priority fromNativePriority (int nativePriority)
                throw (BadParameterException);


                /**
                * Purpose:
                * <p> Transform Cdmw thread priority to native priority
                *
                * @exception BadParameterException
                */
                int toNativePriority (Thread::Priority priority)
                throw (BadParameterException);


                /**
                * Purpose:
                * <p> Get the Max Cdmw thread priority
                *
                * @exception
                */
                Thread::Priority getMaxPriority ()
                throw ();


                /**
                * Purpose:
                * <p> Get the Min Cdmw thread priority
                *
                * @exception
                */
                Thread::Priority getMinPriority ()
                throw ();



            protected:

                /**
                * Purpose:
                * <p> Set the Max Cdmw thread priority
                *
                * @exception BadParameterException
                */
                void setMaxPriority (Thread::Priority priority)
                throw (BadParameterException);


                /**
                * Purpose:
                * <p> Set the Min Cdmw thread priority
                *
                * @exception BadParameterException
                */
                void setMinPriority (Thread::Priority priority)
                throw (BadParameterException);

            private:

                /**
                * Cdmw Thread Maxi priority
                */
                Thread::Priority m_maxPriority;

                /**
                * Cdmw Thread Mini priority
                */
                Thread::Priority m_minPriority;
        };


#if defined(CDMW_POSIX_THREAD)

        /**
        *Purpose:
        *<p>    Implementation of the ThreadPriority class for Posix threads
        *        to map pthread native priority to Cdmw thread priority
        *
        *Features:
        *<p> Thread safe, exception safe
        *
        *@see  Thread
        *
        */

        class ThreadPriority : public ThreadPriorityBase
        {

            public:

                /**
                * Purpose:
                * <p> Construct the Posix thread priority
                * 
                *@param
                *
                *@exception 
                */
                ThreadPriority ()
                throw ();

                //
                // Dtor
                //
                ~ThreadPriority()
                throw();


                /**
                * Purpose:
                * <p> Transform native Posix priority to Cdmw thread priority
                *
                * @exception BadParameterException
                */
                Thread::Priority fromNativePriority (int nativePriority)
                throw (BadParameterException);


                /**
                * Purpose:
                * <p> Transform Cdmw thread priority to native Posix priority
                *
                * @exception BadParameterException
                */
                int toNativePriority (Thread::Priority priority)
                throw (BadParameterException);


                /**
                * Purpose:
                * <p> Set the Max Cdmw thread priority
                *
                * @exception BadParameterException
                */
                void setMaxPriority (int nativePriority)
                throw (BadParameterException);


                /**
                * Purpose:
                * <p> Set the Min Cdmw thread priority
                *
                * @exception BadParameterException 
                */
                void setMinPriority (int nativePriority)
                throw (BadParameterException);



            private:

        };

#elif defined (_WIN32)


        /**
        *Purpose:
        *<p>    Implementation of the ThreadPriority class for Win32 threads
        *        to map thread native priority to Cdmw thread priority
        *
        *Features:
        *<p> Thread safe, exception safe
        *
        *@see  Thread
        *
        */

        class ThreadPriority : public ThreadPriorityBase
        {

            public:

                enum
                {
                    THR_MIN_PRIORITY = 1,
                    THR_MAX_PRIORITY = 7
            };

                /**
                * Purpose:
                * <p> Construct the Win32 thread priority
                *
                *@param
                *
                *@exception
                */
                ThreadPriority ()
                throw ();

                //
                // Dtor
                //
                ~ThreadPriority()
                throw();

                /**
                * Purpose:
                * <p> Transform native Win32 priority to Cdmw thread priority
                *
                * @exception BadParameterException
                */
                Thread::Priority fromNativePriority (int nativePriority)
                throw (BadParameterException);


                /**
                * Purpose:
                * <p> Transform Cdmw thread priority to native Win32 priority
                *
                * @exception BadParameterException
                */
                int toNativePriority (Thread::Priority priority)
                throw (BadParameterException);

            private:

                int m_mappingTable[MAX_PRIORITY + 1];

        };

#else
# error CDMW_IMPL_NOT_AVAILABLE
#endif

    } // End namespace OsSupport

} // End namespace Cdmw


#endif // CDMW_OSSUPPORT_THREAD_PRIORITY_HPP



