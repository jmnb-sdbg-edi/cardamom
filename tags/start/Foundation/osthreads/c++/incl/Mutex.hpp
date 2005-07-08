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


#ifndef INCL_OSSUPPORT_MUTEX_HPP
#define INCL_OSSUPPORT_MUTEX_HPP

#include <memory>

#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/AbstractMutex.hpp"

namespace Cdmw
{

    namespace OsSupport
    {

        class MutexImpl;


        /**
        *Purpose:
        *<p>    Implementation of the Mutex class with
        *        compilation firewall idiom (pimpl)
        *
        *Features:
        *<p> Thread safe, exception safe
        *
        *@see  MutexGuard 
        *@see  Condition 
        *@see  MutexImpl
        *
        */

        class Mutex : public AbstractMutex
        {

            public:

                /**
                * Purpose:
                * <p> construct the mutex
                *
                *@exception OutOfMemoryException
                *@exception InternalErrorException      
                */
                Mutex()
                throw (
                    OutOfMemoryException,
                    InternalErrorException);



                //
                // Dtor
                //
                virtual
                ~Mutex()
                throw() ;


                /**
                * Purpose:
                * <p> Lock in an atomic way the Mutex, if the mutex
                * is already acquired by another thread, the thread
                * waits until it's getting unlocked. If the mutex is acquired
                * twice by the same thread, an AlreadyDoneException is
                * thrown.
                *
                * @exception AlreadyDoneException if the mutex is already locked
                *            by the calling thread.
                * @exception InternalErrorException 
                */
                virtual

                void lock ()
                    throw (AlreadyDoneException, InternalErrorException);




                /**
                * Purpose:
                * <p> Unlock the Mutex, if other threads are waiting for
                * the mutex, one of them is allowed to acquire the mutex.
                *
                * @exception PermissionDeniedException if the mutex is not locked
                *            or mutex does not belong to calling thread
                * @exception InternalErrorException
                */
                virtual
                void unlock()
                throw (PermissionDeniedException, InternalErrorException);


                /**
                * Purpose:
                * <p> return true if the mutex is locked else false
                *
                */
                virtual
                bool is_locked()
                throw ();


                /**
                * Purpose:
                * <p> Returns the underlying implementation object. This
                * method is intended to be used by the library for internal
                * purpose.
                *
                */
                MutexImpl&
                get_implementation()
                throw();



            protected:


            private:

                //
                // Copy Ctor
                //
                Mutex(const Mutex&);



                //
                // Assignement operator
                //
                Mutex&
                operator=(const Mutex&);


                /**
                * True implementation of the Mutex using the
                * compiler firewall idiom (pimpl)
                */
                std::auto_ptr<MutexImpl> m_spImpl;

        };



    } // End namespace osthreads
} // End namespace Cdmw



#endif //INCL_OSSUPPORT_MUTEX_HPP

