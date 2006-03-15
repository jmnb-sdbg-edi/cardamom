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


#ifndef INCL_OSSUPPORT_READERWRITERLOCK_HPP
#define INCL_OSSUPPORT_READERWRITERLOCK_HPP

#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Condition.hpp"

namespace Cdmw
{

    namespace OsSupport
    {


        /**
        *Purpose:
        *<p>   This class allow to protect data against concurrent
        *    access. The ReaderWriterLock class allows more than one
        *    thread access the data for reading, but only one thread
        *    to access data for writing
        *
        *Features:
        *<p> Thread safe, exception safe
        *
        *@see  Mutex
        *
        */

        class ReaderWriterLock
        {

            public:

                /**
                * Constructor
                *
                * @exception OutOfMemoryException
                * @exception InternalErrorException     
                */
                ReaderWriterLock()
                throw (OutOfMemoryException,
                       InternalErrorException);



                /**
                * Destructor
                *
                */
                virtual
                ~ReaderWriterLock()
                throw();


                /**
                * Purpose:
                * <p> Acquire the lock for reading, more than one
                * thread can acquire the lock for reading.
                *
                * @exception OutOfMemoryException
                * @exception InternalErrorException     
                *
                */
                void
                readLock()
                throw (OutOfMemoryException,
                       InternalErrorException);



                /**
                * Purpose:
                * <p> Acquire the lock for writing, only one
                * thread can access data, when the lock is acquired
                *  for writing.
                *
                * @exception OutOfMemoryException
                * @exception InternalErrorException     
                */
                void
                writeLock()
                throw (OutOfMemoryException,
                       InternalErrorException);


                /**
                * Purpose:
                * <p> Release the lock previously acquired
                *  for reading.
                *
                * @exception OutOfMemoryException
                * @exception InternalErrorException     
                */
                void
                readUnlock()
                throw (OutOfMemoryException,
                       InternalErrorException);


                /**
                * Purpose:
                * <p> Release the lock previously acquired
                *  for writing.
                *
                * @exception OutOfMemoryException
                * @exception InternalErrorException     
                */
                void
                writeUnlock()
                throw (OutOfMemoryException,
                       InternalErrorException);


            protected:


            private:

                //
                // Copy ctor
                //
                ReaderWriterLock(const ReaderWriterLock& rhs);


                /**
                * Each reader/writer thread must acquire this mutex before 
                * changing data of the ReaderWriterLock.
                * This mutex is change in an atomic way by the 2 internals
                * condition : m_writer_cond, m_writer_cond.
                */
                Mutex m_rwlock_mutex;

                /**
                * This condition allows writers thread to wait until there is no
                * more reader/writer thread in the ReaderWriterLock. It 's 
                * the responsibility of each thread to notify the conditions if
                * threads are waiting on them. This condition unlock/lock the
                * m_rwlock_mutex, in its wait() call.
                */
                Condition m_writer_cond;


                /**
                * This condition allows readers thread to wait until there is no
                * more writer thread in the ReaderWriterLock. It 's 
                * the responsibility of each thread to notify the conditions if
                * threads are waiting on them. This condition unlock/lock the
                * m_rwlock_mutex, in its wait() call.
                */
                Condition m_reader_cond;



                /**
                * This mutex protect the variable containing the
                * number of reader currently locking the ReaderWriterLock.
                */
                Mutex m_nbReaders_mutex;



                /**
                * Contains the number of readers currently locking the
                * ReaderWriterLock. When this variable need to be modify,
                * the corresponding mutex must be acquired.
                */
                size_t m_nbReaders;



                /**
                * This variable must never be over 1. This state is maintain
                * for assertion purpose.
                */
                size_t m_nbWriters;

                int m_nb_waiting_writer;

                int m_nb_waiting_reader;



        }

        ; // End class ReaderWriterLock

    } // End namespace osthreads

} // End namespace Cdmw

#endif // INCL_OSSUPPORT_READERWRITERLOCK_HPP

