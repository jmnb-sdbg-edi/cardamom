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


#include "testosthreads/TestReaderWriterLock.hpp"

#include <iostream>

#include "Foundation/testutils/Testable.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/osthreads/Thread.hpp"

namespace Cdmw
{

    namespace OsSupport
    {

        TestReaderWriterLock::TestReaderWriterLock(std::ostream& os)
                : m_nbReaders(0),
                m_nbWriters(0),
                m_os(os)
        {
        }


        TestReaderWriterLock::~TestReaderWriterLock()
        throw()
        {
        }


        void
        TestReaderWriterLock::run()
        throw()
        {

            int i(0);
            // 4 is for manipulate seconds,
            //    and 15 seconds is about the live time of each
            //    thread (NB_LOCKS * sleep( 0..3s)

            while ( i < (4*15) )
            {
                {
                    CDMW_MUTEX_GUARD(m_mutex);
                    m_os << ".";
                    m_os.flush();
                }

                int timescale = Cdmw::TestUtils::Testable::get_timescale();
                Thread::sleep(timescale*250);
                i++;
            }

            m_os << std::endl;
        }


        void
        TestReaderWriterLock::increment_reader()
        throw ( InvariantException )
        {
            CDMW_MUTEX_GUARD(m_mutex);
            m_nbReaders++;
        }


        void
        TestReaderWriterLock::decrement_reader()
        throw ( InvariantException )
        {
            CDMW_MUTEX_GUARD(m_mutex);
            m_nbReaders--;

        }


        void
        TestReaderWriterLock::increment_writer()
        throw ( InvariantException )
        {
            CDMW_MUTEX_GUARD(m_mutex);
            m_nbWriters++;
        }


        void
        TestReaderWriterLock::decrement_writer()
        throw ( InvariantException )
        {
            CDMW_MUTEX_GUARD(m_mutex);
            m_nbWriters--;
        }



        int
        TestReaderWriterLock::get_nbReaders()
        throw ( InvariantException )
        {
            CDMW_MUTEX_GUARD(m_mutex);
            return m_nbReaders;
        }


        int
        TestReaderWriterLock::get_nbWriters()
        throw ( InvariantException )
        {

            CDMW_MUTEX_GUARD(m_mutex);

            return m_nbWriters;

        }


        void
        TestReaderWriterLock::check_invariant()
        throw ( InvariantException )
        {
            // We can not have reader if writer are inside

            if ( m_nbWriters != 0 && m_nbReaders != 0 )
            {
                throw InvariantException(" Readers present with writers");
            }


            if ( m_nbReaders < 0 )
            {
                throw InvariantException(" Negative count of readers");
            }

            if ( m_nbWriters < 0 )
            {
                throw InvariantException(" Negative count of writers");
            }


            // We cannot have more than one writer
            if ( m_nbWriters > 1 )
            {
                throw InvariantException("More than one writer");
            }

        }

    } // End namespace OsSupport

} // End namespace Cdmw

