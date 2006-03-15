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


#ifndef INCL_OSSUPPORT_TESTREADERWRITERLOCKREADERTHREAD_HPP
#define INCL_OSSUPPORT_TESTREADERWRITERLOCKREADERTHREAD_HPP

#include "Foundation/osthreads/Thread.hpp"

namespace Cdmw
{

    namespace OsSupport
    {

        class ReaderWriterLock;

        class TestReaderWriterLock;


        class TestReaderWriterLockReaderThread : public Thread
        {

            public:

                // Ctor
                TestReaderWriterLockReaderThread(

                    ReaderWriterLock& lock ,
                    TestReaderWriterLock& test,
                    int nbLocks)

                    ;

                // Dtor
                virtual
                ~TestReaderWriterLockReaderThread()
                throw();

                //
                // Called from a new thread
                //
                virtual
                void
                run()
                throw();

                unsigned int
                get_sleepTime();

            protected:


            private:

                // Copy ctor
                TestReaderWriterLockReaderThread(const TestReaderWriterLockReaderThread& rhs);

                ReaderWriterLock& m_lock;

                TestReaderWriterLock& m_test;

                int m_nbLocks;

                unsigned int m_seed;


        }

        ; // End class TestReaderWriterLockReaderThread

    } // End namespace OsSupport

} // End namespace Cdmw

#endif // INCL_OSSUPPORT_TESTREADERWRITERLOCKREADERTHREAD_HPP
