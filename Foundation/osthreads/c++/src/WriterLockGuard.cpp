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


#include "Foundation/osthreads/WriterLockGuard.hpp"

#include "Foundation/osthreads/ReaderWriterLock.hpp"


namespace Cdmw
{

    namespace OsSupport
    {


        //
        // Construct and acquire for writing the lock
        //

        WriterLockGuard::WriterLockGuard(ReaderWriterLock& lock )
            throw (OutOfMemoryException,
                   InternalErrorException)

                    : m_lock(lock )
        {
            m_lock.writeLock();
        }


        //
        // Destroy and release the writing lock
        //
        WriterLockGuard::~WriterLockGuard()
        throw()
        {
            // FIXME: We ignore the error !!!

            try
            {
                m_lock.writeUnlock();

            }
            catch ( ...)
            {
            }


        }

    } // End namespace osthreads

} // End namespace Cdmw

