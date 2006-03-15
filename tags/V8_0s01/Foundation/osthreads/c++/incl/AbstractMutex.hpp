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


#ifndef INCL_OSSUPPORT_ABSTRACTMUTEX_HPP
#define INCL_OSSUPPORT_ABSTRACTMUTEX_HPP

#include "Foundation/common/Exception.hpp"

/**
* Root namespace for CDMW runtime.
*/

namespace Cdmw
{
    /**
    * Namespace for CDMW OsSupport types and data.
    */

    namespace OsSupport
    {

        /**
        *Purpose:
        *<p>    This class defines function to be provided
        *        by a mutex implementation.
        *
        *Features:
        *<p>    none
        *
        *
        *@see  Mutex 
        *@see  NullMutex
        *
        */

        class AbstractMutex
        {

            public:

                //
                // Ctor
                //
                AbstractMutex();

                //
                // Dtor
                //
                virtual ~AbstractMutex();

                /**
                * Gain exclusive access. Only one thread can return
                * from the lock function at a time.
                *
                *@exception AlreadyDoneException
                *@exception InternalErrorException
                */

                virtual void lock ()
                    throw (AlreadyDoneException, InternalErrorException) = 0;

                /**
                * Release exclusive access.
                *
                *@exception PermissionDeniedException
                *@exception InternalErrorException
                */
                virtual void unlock()
                throw (PermissionDeniedException, InternalErrorException) = 0;


                /**
                 * return true is the mutex is locked else false
                 */
                virtual bool is_locked()
                throw () = 0;

            protected:


            private:

                //
                // Copy ctor
                //
                AbstractMutex(const AbstractMutex& rhs);


                //
                // Assign operator
                //
                AbstractMutex&
                operator=(const AbstractMutex& rhs);




        }

        ; // End class AbstractMutex

    } // End namespace osthreads

} // End namespace Cdmw

#endif // INCL_OSSUPPORT_ABSTRACTMUTEX_HPP

