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


#include "Foundation/osthreads/Condition.hpp"

#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/Mutex.hpp"

#include "osthreads/ConditionImpl.hpp"



namespace Cdmw
{

    namespace OsSupport
    {

        /**
        *Implementation notes: none 
        *<p>
        *Portability issues: none 
        *<p>
        */


        //
        // Ctor
        //
        Condition::Condition(Mutex& mutex)
        throw ( OutOfMemoryException,
                InternalErrorException )

                : m_spImpl( NULL ),
                m_mutex(mutex)
        {
            try
            {
# ifdef _MSC_VER
                // The auto_ptr implementation shipped with visual is not compliant
                // and doesn't provide reset(), but it broken as it used an assignement
                // operator with const auto_ptr&, we use this deviation
                m_spImpl = std::auto_ptr<ConditionImpl>(
                               new ConditionImpl(mutex.get_implementation()) );
# else

                m_spImpl.reset( new ConditionImpl(mutex.get_implementation()) );
#       endif

            }
            catch ( const std::bad_alloc& )
            {

                CDMW_THROW(OutOfMemoryException);

            }
        }



        //
        // Dtor
        //
        Condition::~Condition()
        throw()
        {
            // Nothing to do
        }




        //
        // Blocks the calling thread until broadcast() call
        //
        void
        Condition::wait()
        throw( InternalErrorException)
        {
            m_spImpl->wait();
        }



        //
        // Wait until a broadcast() call or the timeout expires
        //
        Condition::WaitReturn
        Condition::wait( size_t milliseconds )
        throw( InternalErrorException)
        {
            return m_spImpl->wait( milliseconds );

        }



        //
        // Broadcast a signal to all thread waiting on wait() calls
        //
        void
        Condition::broadcast()
        throw ( InternalErrorException)
        {
            m_spImpl->broadcast();

        }




        //
        // Returns the mutex associated to the condition
        //
        Mutex&
        Condition::get_mutex()
        throw ()
        {
            return m_mutex;
        }



    } // End namespace osthreads
} // End namespace Cdmw

