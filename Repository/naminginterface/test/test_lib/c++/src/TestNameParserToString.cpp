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


#include "test_lib/TestNameParserToString.hpp"
#include "naminginterface/NameParser.hpp"


namespace Cdmw
{
namespace NamingAndRepository
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/


TestNameParserToString::TestNameParserToString()
    throw()
    :Testable("Cdmw::NamingAndRepository::NameParser::to_string")
{
    // does nothing
}


TestNameParserToString::~TestNameParserToString()
    throw()
{
    // does nothing
}


TestNameParserToString::TestNameParserToString(const TestNameParserToString& rhs)
    throw()
    :Testable("Cdmw::NamingAndRepository::NameParser::to_string")
{
    // does nothing
}
		
TestNameParserToString&
TestNameParserToString::operator=(const TestNameParserToString& rhs)
    throw()
{
    // does nothing
    return *this;
}


void TestNameParserToString::test_to_string()
{
    TEST_INFO("Test name components with both ID and Kind field");
    {
        using namespace CosNaming;
	    Name name(2);
	    name.length(2);
	    name[0].id = CORBA::string_dup("first");
	    name[0].kind = CORBA::string_dup("1st");
	    name[1].id = CORBA::string_dup("second");
	    name[1].kind = CORBA::string_dup("2nd");
	    
        std::string reference("first.1st/second.2nd");
	    try {
	        std::string output = NamingAndRepository::NameParser::to_string(name);
		    TEST_CHECK(reference == output);
	    } catch (const NamingAndRepository::InvalidNameException&) {
	        TEST_FAILED();
	    }
    }
    
    TEST_INFO("Test name components with missing Kind field");
    {
        using namespace CosNaming;
	    Name name(2);
	    name.length(2);
	    name[0].id = CORBA::string_dup("first");
	    name[0].kind = CORBA::string_dup("");
	    name[1].id = CORBA::string_dup("second");
	    name[1].kind = CORBA::string_dup("2nd");
	    
        std::string reference("first/second.2nd");
	    try {
	        std::string output = NamingAndRepository::NameParser::to_string(name);
		    TEST_CHECK(reference == output);
	    } catch (const NamingAndRepository::InvalidNameException&) {
	        TEST_FAILED();
	    }
    }

    TEST_INFO("Test name components with missing ID field");
    {
        using namespace CosNaming;
	    Name name(2);
	    name.length(2);
	    name[0].id = CORBA::string_dup("first");
	    name[0].kind = CORBA::string_dup("1st");
	    name[1].id = CORBA::string_dup("");
	    name[1].kind = CORBA::string_dup("2nd");
	    
        std::string reference("first.1st/.2nd");
	    try {
	        std::string output = NamingAndRepository::NameParser::to_string(name);
		    TEST_CHECK(reference == output);
	    } catch (const NamingAndRepository::InvalidNameException&) {
	        TEST_FAILED();
	    }
    }

    TEST_INFO("Test node with empty ID and Kind field");
    {
        using namespace CosNaming;
	    Name name(3);
	    name.length(3);
	    name[0].id = CORBA::string_dup("first");
	    name[0].kind = CORBA::string_dup("1st");
	    name[1].id = CORBA::string_dup("");
	    name[1].kind = CORBA::string_dup("");
	    name[2].id = CORBA::string_dup("third");
	    name[2].kind = CORBA::string_dup("3rd");
	    
        std::string reference("first.1st/./third.3rd");
	    try {
	        std::string output = NamingAndRepository::NameParser::to_string(name);
		    TEST_CHECK(reference == output);
	    } catch (const NamingAndRepository::InvalidNameException&) {
	        TEST_FAILED();
	    }
    }
    
    TEST_INFO("Test nodes with characters that are escaped");
    {
        using namespace CosNaming;
	    Name name(3);
	    name.length(3);
	    name[0].id = CORBA::string_dup("first");
	    name[0].kind = CORBA::string_dup("1st.dot");
	    name[1].id = CORBA::string_dup("second.dot");
	    name[1].kind = CORBA::string_dup("2nd");
	    name[2].id = CORBA::string_dup("third/slash");
	    name[2].kind = CORBA::string_dup("3rd\n");
	    
        std::string reference("first.1st\\.dot/second\\.dot.2nd/third\\/slash.3rd\n");
	    try {
	        std::string output = NamingAndRepository::NameParser::to_string(name);
		    TEST_CHECK(reference == output);
	    } catch (const NamingAndRepository::InvalidNameException&) {
	        TEST_FAILED();
	    }
    }
    TEST_INFO("Test empty name");
    {
        using namespace CosNaming;
	    Name name(1);
	    name.length(1);
	    name[0].id = CORBA::string_dup("");
	    name[0].kind = CORBA::string_dup("");
	    
        std::string reference(".");
	    try {
	        std::string output = NamingAndRepository::NameParser::to_string(name);
		    TEST_CHECK(reference == output);
	    } catch (const NamingAndRepository::InvalidNameException&) {
	        TEST_FAILED();
	    }
    }
    {
        using namespace CosNaming;
	    Name name(0);
	    name.length(0);
	    try {
	        std::string output = NamingAndRepository::NameParser::to_string(name);
		    TEST_FAILED();
	    } catch (const NamingAndRepository::InvalidNameException&) {
	        TEST_SUCCEED();
	    }
    }

}

void TestNameParserToString::do_tests()		
{   
    // This tests NameParser class
    //-------------------------------------------------
    // Testing name to string conversion
    //-------------------------------------------------
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (7);
    
    TEST_INFO("Testing NameParser::to_string operation");
    test_to_string();
    
}
		
} // End namespace NamingAndRepository
} // End namespace Cdmw

