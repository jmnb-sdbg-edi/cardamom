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


#ifndef INCL_TESTUTILS_TESTABLE_HPP
#define INCL_TESTUTILS_TESTABLE_HPP

#include <iosfwd>
#include <string>
#include <iostream> 

/**
* Root namespace for CDMW runtime.
*/

namespace Cdmw
{
    /**
    * Namespace for TestUtils types used by CDMW runtime.
    */

    namespace TestUtils
    {

        /**
        *Purpose:
        *<p> Base class for Test Drivers. It provides routines to start unit tests and
        * and count the number of tests that have succeeded or failed.
        */

        class Testable
        {

            public:

                Testable(const std::string& name);
                Testable(const std::string& name,
                         std::ostream & os);

                virtual ~Testable();

                /**
                * Start the test
                * return true if no failed test
                */
                bool start();


                /**
                * Do the summary of test results
                * return true if no failed test
                */
                bool do_test_summary();


                /**
                * Set the number of requested Test OK
                */
                void set_nbOfRequestedTestOK (int nbOfRequestedTestOK);

                /**
                * Add the number to requested Test OK
                */
                void add_nbOfRequestedTestOK (int nbOfRequestedTestOK);

                /**
                * Get the number of requested Test OK
                */
                int get_nbOfRequestedTestOK ();

                /**
                * Return the multiplier coefficient of time
                **/
                static int
                get_timescale();

                /**
                * Number for multiplier time
                */
                static int m_timescale;


            protected:

                /**
                * Returns the stream where test result and
                * information must be printed.
                */
                std::ostream& get_resultStream();



                void failed(const std::string& file, int line);

                void succeed();

                const std::string& get_name();


                virtual void do_tests() = 0;

            private:

                /**
                * Name of the test
                */
                std::string m_name;


                /**
                * Number of failed test
                */
                int m_failedTest;

                /**
                * Number of succedeed test
                */
                int m_succeededTest;

                /**
                * Number of requested succedeed test
                */
                int m_nbOfRequestedTestOK;

                /**
                * Resuly Stream (default is std::cout)
                */
                std::ostream & m_resultStream;



        }

        ; // End class Testable

    } // End namespace TestUtils
} // End namespace Cdmw


#define TEST_INFO( str ) get_resultStream() << get_name() << ": " << str << std::endl;

#define TEST_CHECK( condition ) if ( ! ( condition ) ) { \
        failed(__FILE__, __LINE__); \
    } else { \
        succeed();\
    }
#define TEST_SUCCEED() succeed()

#define TEST_FAILED() failed(__FILE__, __LINE__)



#endif // INCL_TESTUTILS_TESTABLE_HPP

