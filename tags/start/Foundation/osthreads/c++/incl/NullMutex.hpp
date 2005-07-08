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


#ifndef INCL_OSSUPPORT_NULLMUTEX_HPP
#define INCL_OSSUPPORT_NULLMUTEX_HPP

#include "Foundation/osthreads/Mutex.hpp"

namespace Cdmw
{

    namespace OsSupport
    {



        /**
        *Purpose:
        *<p> This class can be used instead of the Mutex 
        *    to suppress the cost of a lock. The lock/unlock
        *    provided method do nothing, and so doesn't provide
        *    any synchronisation between thread, but allow to 
        *    dynamically suppress lock cost when it isn't required    
        *
        *Features:
        *<p> Thread safe, exception safe
        *
        *@see  Mutex
        *
        */

        class NullMutex : public Mutex
        {

            public:

                //
                // Ctor
                //
                NullMutex()
                throw ();



                //
                // Dtor
                //
                virtual
                ~NullMutex()
                throw ();


                /**
                * Purpose:
                * <p> This method does nothing
                * 
                *@exception OutOfMemoryException
                *@exception InternalErrorException     
                */
                virtual
                void

                lock ()
                    throw (AlreadyDoneException,
                           InternalErrorException);



                /**
                * Purpose:
                * <p> This method does nothing
                * 
                *@exception OutOfMemoryException
                *@exception InternalErrorException     
                */
                virtual
                void
                unlock()
                throw (InternalErrorException);

            protected:


            private:

                //
                // Copy ctor
                //
                NullMutex(const NullMutex& rhs);



                //
                // Assignement operator
                //
                NullMutex&
                operator=(const NullMutex& rhs);




        }

        ; // End class NullMutex

    } // End namespace osthreads
} // End namespace Cdmw

#endif // INCL_OSSUPPORT_NULLMUTEX_HPP

