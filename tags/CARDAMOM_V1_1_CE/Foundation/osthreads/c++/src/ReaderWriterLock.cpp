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


#include "Foundation/osthreads/ReaderWriterLock.hpp"

#include "Foundation/common/Assert.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"



namespace Cdmw
{

    namespace OsSupport
    {

        ReaderWriterLock::ReaderWriterLock()
        throw (OutOfMemoryException,
               InternalErrorException)
                : m_rwlock_mutex(),
                m_writer_cond(m_rwlock_mutex),
                m_reader_cond(m_rwlock_mutex),
                m_nbReaders(0),
                m_nbWriters(0),
                m_nb_waiting_writer(0),
                m_nb_waiting_reader(0)

        {
            // Nothing to do
        }



        //
        // Dtor
        //
        ReaderWriterLock::~ReaderWriterLock()
        throw()
        {
            // Nothing to do
        }



        //
        // Acquire for read the lock
        //
        void
        ReaderWriterLock::readLock()
        throw (OutOfMemoryException,
               InternalErrorException)
        {
            try
            {
                CDMW_MUTEX_GUARD(m_rwlock_mutex);

                CDMW_ASSERT(m_nbReaders >= 0);
                CDMW_ASSERT(m_nbWriters == 0 || m_nbWriters == 1);

                m_nb_waiting_reader++;

                // If there is writer in the condition, we need to wait

                while (m_nbWriters > 0)
                {
                    m_reader_cond.wait();
                }

                // There is one more reader
                m_nb_waiting_reader--;

                m_nbReaders++;

                CDMW_ASSERT(m_nb_waiting_writer >= 0);

                CDMW_ASSERT(m_nb_waiting_reader >= 0);

                CDMW_ASSERT(m_nbReaders >= 1);

                CDMW_ASSERT(m_nbWriters == 0);

                // The Thread must be blocked on wait, so it can not lock
                //    twice the mutex, we have a bug or we are corrupted
            }
            catch (const AlreadyDoneException&)
            {

                CDMW_THROW (InternalErrorException);
            }
        }



        //
        // Acquire for write the lock
        //
        void
        ReaderWriterLock::writeLock()
        throw (OutOfMemoryException,
               InternalErrorException)

        {
            try
            {
                CDMW_MUTEX_GUARD(m_rwlock_mutex);

                CDMW_ASSERT(m_nbReaders >= 0);
                CDMW_ASSERT(m_nbWriters <= 1);

                m_nb_waiting_writer++;

                // To acquire exclusive access no reader or writer must be present

                while (m_nbReaders != 0 || m_nbWriters != 0)
                {
                    m_writer_cond.wait();
                }

                m_nb_waiting_writer--;
                m_nbWriters++;

                CDMW_ASSERT(m_nb_waiting_writer >= 0);
                CDMW_ASSERT(m_nb_waiting_reader >= 0);
                CDMW_ASSERT(m_nbReaders == 0);
                CDMW_ASSERT(m_nbWriters == 1);

                // The Thread must be blocked on wait, so it can not lock
                //    twice the mutex, we have a bug or we are corrupted
            }
            catch (const AlreadyDoneException&)
            {

                CDMW_THROW (InternalErrorException);
            }
        }




        //
        // Unlock the lock previously acquire for reading
        //
        void
        ReaderWriterLock::readUnlock()
        throw (OutOfMemoryException,
               InternalErrorException)
        {
            try
            {
                CDMW_MUTEX_GUARD(m_rwlock_mutex);

                CDMW_ASSERT(m_nbReaders >= 1);
                CDMW_ASSERT(m_nbWriters == 0);

                m_nbReaders--;

                // If there is no more reader, we allow writer to access

                if (m_nbReaders == 0 && m_nb_waiting_writer != 0)
                {

                    m_writer_cond.broadcast();

                }

                CDMW_ASSERT(m_nbWriters == 0);
                CDMW_ASSERT(m_nbReaders >= 0);

                // The Thread can not lock
                //    twice the mutex, we have a bug or we are corrupted
            }
            catch (const AlreadyDoneException&)
            {

                CDMW_THROW (InternalErrorException);
            }
        }



        //
        // Unlock the lock previously acquire for writing
        //
        void
        ReaderWriterLock::writeUnlock()
        throw (OutOfMemoryException,
               InternalErrorException)
        {
            try
            {
                CDMW_MUTEX_GUARD(m_rwlock_mutex);

                CDMW_ASSERT(m_nbWriters == 1);
                CDMW_ASSERT(m_nbReaders == 0);

                m_nbWriters--;

                if (m_nb_waiting_reader != 0)
                {

                    m_reader_cond.broadcast();

                }
                else if (m_nb_waiting_writer != 0)
                {

                    m_writer_cond.broadcast();
                }

                CDMW_ASSERT(m_nbWriters == 0);
                CDMW_ASSERT(m_nbReaders == 0);

                // The Thread can not lock
                //    twice the mutex, we have a bug or we are corrupted
            }
            catch (const AlreadyDoneException&)
            {

                CDMW_THROW (InternalErrorException);
            }
        }


    } // End namespace osthreads

} // End namespace Cdmw

