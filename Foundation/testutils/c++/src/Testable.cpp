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


#include "Foundation/common/System.hpp"

#include "Foundation/testutils/Testable.hpp"

#include <iostream>
#include <sstream>

using std::endl;

namespace Cdmw
{

    namespace TestUtils
    {

        Testable::Testable(const std::string& name)
                : m_name(name),
                m_failedTest(0),
                m_succeededTest(0),
                m_nbOfRequestedTestOK(0),
                m_resultStream(std::cout)
        {

            // Nothing to do

        }

        Testable::Testable(const std::string& name,
                           std::ostream & os)
                : m_name(name),
                m_failedTest(0),
                m_succeededTest(0),
                m_nbOfRequestedTestOK(0),
                m_resultStream(os)
        {

            // Nothing to do

        }


        Testable::~Testable()
        {
        }


        std::ostream&
        Testable::get_resultStream()
        {
            return m_resultStream;
        }


        bool
        Testable::start()
        {
            // reset nbr of failed and succeeded tests
            m_failedTest = 0;
            m_succeededTest = 0;

            get_resultStream() << "===================================================" << endl;
            get_resultStream() << "Starting test " << m_name << endl;
            get_resultStream() << "===================================================" << endl;

            do_tests();

            // process the test summary
            return do_test_summary();

        }


        bool
        Testable::do_test_summary ()
        {
            get_resultStream() << "===================================================" << endl;
            get_resultStream() << "Ending test " << m_name << endl;
            get_resultStream() << "===================================================" << endl;
            get_resultStream() << endl;
            get_resultStream() << "Summary:" << endl;
            get_resultStream() << "   Failed    : " << m_failedTest << endl;
            get_resultStream() << "   Succeeded : " << m_succeededTest << endl;
            get_resultStream() << endl;

            if (m_nbOfRequestedTestOK != 0)
            {
                get_resultStream() << "   Expected successfull tests (CPP) : " << m_nbOfRequestedTestOK << endl;
                get_resultStream() << endl;
            }

            if (m_succeededTest == m_nbOfRequestedTestOK && m_failedTest == 0)
            {
                get_resultStream() << "   >>>> Test result is set as TESTABLE_OK <<<<" << endl;
            }
            else if (m_nbOfRequestedTestOK != 0 || m_failedTest != 0)
            {
                get_resultStream() << "   >>>> Test result is NOT OK <<<<" << endl;
            }

            get_resultStream() << endl;

            if (m_failedTest != 0)
            {
                return false;
            }
            else
            {
                return true;
            }
        }


        void
        Testable::failed(const std::string& file, int line)
        {

            get_resultStream() << " **** TEST FAILED AT LINE " << line << " IN FILE " << file << endl;
            m_failedTest++;

        }


        void
        Testable::succeed()
        {
            get_resultStream() << m_name << ": Test succeed" << endl;
            m_succeededTest++;
        }

        void
        Testable::set_nbOfRequestedTestOK (int nbOfRequestedTestOK)
        {
            m_nbOfRequestedTestOK = nbOfRequestedTestOK;
        }

        void
        Testable::add_nbOfRequestedTestOK (int nbOfRequestedTestOK)
        {
            m_nbOfRequestedTestOK += nbOfRequestedTestOK;
        }


        int
        Testable::get_nbOfRequestedTestOK ()
        {
            return m_nbOfRequestedTestOK;
        }


        const std::string&
        Testable::get_name()
        {
            return m_name;

        }



        // FIXME: find a way to discover the targer
#undef sparc
#undef sun

        int Testable::m_timescale( -1);


        //
        // Return the multiplier coefficient of time
        //
        int
        Testable::get_timescale()
        {
            if (m_timescale == -1)
            {
                char* p_env = ::getenv("CDMW_TIMESCALE");

                if (p_env == NULL)
                {
                    m_timescale = 1;
                }
                else
                {
                    m_timescale = atoi(p_env);
                }
            }

            return (m_timescale);

        }



    } // End namespace TestUtils
} // End namespace Cdmw


