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


#ifndef INCL_OSSUPPORT_TESTREADERWRITERLOCK_HPP
#define INCL_OSSUPPORT_TESTREADERWRITERLOCK_HPP

#include <iosfwd>

#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Thread.hpp"


namespace Cdmw
{

    namespace OsSupport
    {



        class InvariantException
        {

            public:
                InvariantException(const std::string str)
                        : m_str(str)
                {}


                const char*
                c_str() const
                {
                    return m_str.c_str();
                }

            private:
                std::string m_str;
        };

        class TestReaderWriterLock : public Thread
        {

            public:

                // Ctor
                TestReaderWriterLock(std::ostream& os);


                // Dtor
                virtual
                ~TestReaderWriterLock()
                throw();

                void run()
                throw();

                void
                increment_reader()
                throw ( InvariantException );

                void
                decrement_reader()
                throw ( InvariantException );

                void
                increment_writer()
                throw ( InvariantException );

                void
                decrement_writer()
                throw ( InvariantException );


                int
                get_nbReaders()
                throw ( InvariantException );

                int
                get_nbWriters()
                throw ( InvariantException );


            protected:

                void check_invariant()
                throw ( InvariantException );


            private:

                // Copy ctor
                TestReaderWriterLock(const TestReaderWriterLock& rhs);

                Mutex m_mutex;

                int m_nbReaders;
                int m_nbWriters;

                std::ostream& m_os;

        }

        ; // End class TestReaderWriterLock

    } // End namespace OsSupport

} // End namespace Cdmw

#endif // INCL_OSSUPPORT_TESTREADERWRITERLOCK_HPP

