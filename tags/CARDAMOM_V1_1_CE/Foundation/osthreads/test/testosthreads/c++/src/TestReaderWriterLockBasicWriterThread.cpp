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


#include "testosthreads/TestReaderWriterLockBasicWriterThread.hpp"


namespace Cdmw
{

    namespace OsSupport
    {

        TestReaderWriterLockBasicWriterThread::TestReaderWriterLockBasicWriterThread(

            ReaderWriterLock& lock ,
            int& value)
                    : m_lock(lock )
                    ,
                    m_value(value)
        {
        }


        TestReaderWriterLockBasicWriterThread::~TestReaderWriterLockBasicWriterThread() throw()

        {
        }

        void
        TestReaderWriterLockBasicWriterThread::run()
        throw()
        {

            m_lock.writeLock();
            m_value = 999;
            m_lock.writeUnlock();

        }

    } // End namespace OsSupport

} // End namespace Cdmw

