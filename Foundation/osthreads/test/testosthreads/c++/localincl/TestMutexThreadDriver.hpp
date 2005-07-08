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


#ifndef INCL_OSSUPORT_TESTMUTEXTHREADDRIVER_HPP
#define INCL_OSSUPORT_TESTMUTEXTHREADDRIVER_HPP

#include "Foundation/testutils/Testable.hpp"


namespace Cdmw
{

    namespace OsSupport
    {

        class TestMutexThreadDriver : public TestUtils::Testable
        {

            public:

                // Ctor
                TestMutexThreadDriver();

                // Dtor
                virtual
                ~TestMutexThreadDriver();




            protected:

                virtual
                void
                do_tests();

                /**
                * Test the MutexGuard object
                */
                void
                do_test_without_guard();

                /**
                * Thread the Mutex object
                */
                void
                do_test_with_guard();

                /**
                * Test for the NullMutex object
                */
                void
                do_test_with_NullMutex();

                /**
                * test to check if an exception is thrown, when
                * a thread try to lock the same mutex twice.
                */
                void
                do_test_recursive_mutex();


            private:

                // Copy ctor
                TestMutexThreadDriver(const TestMutexThreadDriver& rhs);



        }

        ; // End class TestMutexThreadDriver

    } // End namespace OsSupport

} // End namespace Cdmw

#endif // INCL_OSSUPORT_TESTMUTEXTHREADDRIVER_HPP

