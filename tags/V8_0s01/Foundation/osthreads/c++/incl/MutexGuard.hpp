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


#ifndef INCL_OSSUPPORT_MUTEXGUARD_HPP
#define INCL_OSSUPPORT_MUTEXGUARD_HPP

#include "Foundation/common/Exception.hpp"


namespace Cdmw
{

    namespace OsSupport
    {

        class AbstractMutex;


        /**
        *Purpose:
        *<p>    This class uses the "Ressource acquisition is
        *        initialisation" to automatically lock/unlock
        *        the associated Mutex.
        *
        *Features:
        *<p> Thread safe, exception safe
        *
        *@see  Mutex
        *
        */

        class MutexGuard
        {

            public:

                /**
                * Construct and lock the specified Mutex
                *
                *@exception OutOfMemoryException
                *@exception AlreadyDoneException the mutex is already locked
                *           by the calling thread.
                *@exception InternalErrorException          
                */
                explicit
                MutexGuard (AbstractMutex& mutex)
                throw (OutOfMemoryException,
                       AlreadyDoneException,
                       InternalErrorException);


                /**
                * Destroy the guard and release the associted 
                * Mutex.
                */
                ~MutexGuard()
                throw();


            protected:



            private:

                //
                // Copy ctor
                //
                MutexGuard(const MutexGuard& rhs);



                //
                // Assignement operator
                //
                MutexGuard&
                operator= (const MutexGuard& rhs);



                /**
                * Mutex automatically lock/unlock by the guard.
                */
                AbstractMutex& m_mutex;


        }

        ; // End class MutexGuard

    } // End namespace osthreads
} // End namespace Cdmw

/**
* Convenient macro to create a temporary variable with
* MutexGuard.
*/
#define CDMW_MUTEX_GUARD(mutex) Cdmw::OsSupport::MutexGuard cdmw_mutexGuard##__LINE__(mutex)


#endif // INCL_OSSUPPORT_MUTEXGUARD_HPP

