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


#ifndef INCL_OSSUPPORT_WRITERLOCKGUARD_HPP
#define INCL_OSSUPPORT_WRITERLOCKGUARD_HPP

#include "Foundation/common/Exception.hpp"


namespace Cdmw
{

    namespace OsSupport
    {

        class ReaderWriterLock;


        /**
        *Purpose:
        *<p>    This class uses the "Ressource acquisition is
        *        initialisation" to automatically 
        *        writeLock/writeUnlock
        *        the associated ReaderWriterLock.
        *
        *Features:
        *<p> Thread safe, exception safe
        *
        *@see  ReaderWriterLock
        *
        */

        class WriterLockGuard
        {

            public:

                /**
                * Purpose:
                * <p> Construct the object and acquire the lock for writing.
                *
                * @exception OutOfMemoryException
                * @exception InternalErrorException          
                */

                WriterLockGuard(ReaderWriterLock& lock )
                    throw (OutOfMemoryException,
                           InternalErrorException);


                //
                // Dtor
                //
                virtual
                ~WriterLockGuard()
                throw();


            protected:


            private:

                //
                // Copy ctor
                //
                WriterLockGuard(const WriterLockGuard& rhs);

                //
                // Assignement operator
                //
                WriterLockGuard&
                operator=(const WriterLockGuard& rhs);

                /**
                * ReaderWriterLock used by the guard.
                */
                ReaderWriterLock& m_lock;



        }

        ; // End class WriterLockGuard

    } // End namespace osthreads

} // End namespace Cdmw

/**
* Convenient macro to create a temporary variable with
* WriterLockGuard.
*/
#define CDMW_WRITER_LOCK_GUARD(lock) Cdmw::OsSupport::WriterLockGuard lockGuard##__LINE__(lock)


#endif // INCL_OSSUPPORT_WRITERLOCKGUARD_HPP

