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


#include <string>

#include "teststring/TestString.hpp"
#include "Foundation/common/String.hpp"



namespace Cdmw
{

    namespace Common
    {

        /**
        *Implementation notes: [if no pertinent write none ]
        *<p>
        *Portability issues: [if no pertinent write none ]
        *<p>
        */


        TestString::TestString()
                : Testable("Cdmw::String")
        {
        }


        TestString::~TestString()
        throw()
        {
        }

        void
        TestString::do_tests()
        {
            // set number of requested successfull tests
            set_nbOfRequestedTestOK (8);

            TEST_INFO("Creating a string composed of 3 string separated by a space");
            std::string test = "foo bar foobar=25";

            TEST_INFO("Transforming the string into substring");
            std::vector< std::string > strings = String::to_strings( test );

            TEST_INFO("Checking results");
            TEST_CHECK( strings.size() == 3 );
            TEST_CHECK( strings[0] == "foo" );
            TEST_CHECK( strings[1] == "bar" );
            TEST_CHECK( strings[2] == "foobar=25" );

            TEST_INFO("Transforming the string vector into char**");
            char** ppstrings = String::to_char_array (strings);
            TEST_INFO("Checking results");
            TEST_CHECK( strcmp(ppstrings[0], "foo") == 0 );
            TEST_CHECK( strcmp(ppstrings[1], "bar") == 0 );
            TEST_CHECK( strcmp(ppstrings[2], "foobar=25") == 0 );

            TEST_INFO("Freeing allocated memory");
            delete [] ppstrings[0];
            delete [] ppstrings[1];
            delete [] ppstrings[2];
            delete [] ppstrings;
            TEST_SUCCEED();

        }


    } // End namespace Common
} // End namespace Cdmw

