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


#include "testosthreads/TestMutexGuardThread.hpp"

#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"

namespace Cdmw
{

    namespace OsSupport
    {

        //
        // Ctor
        //
        TestMutexGuardThread::TestMutexGuardThread(Mutex& mutex, int& valueToBeChanged)
                : m_mutex(mutex),
                m_valueToBeChanged(valueToBeChanged)

        {
        }



        //
        // Dtor
        //
        TestMutexGuardThread::~TestMutexGuardThread() throw()
        {}


        //
        // When invoked in a new threa we try to automatically
        //    acquire/release the mutex
        //
        void
        TestMutexGuardThread::run() throw()
        {
            // We try to acquire the mutex for changing the variable
            MutexGuard guard(m_mutex);

            m_valueToBeChanged = 10;

        }

    } // End namespace OsSupport

} // End namespace Cdmw

