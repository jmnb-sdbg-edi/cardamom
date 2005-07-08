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


#include "test_lib/TestNameParserToName.hpp"
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


TestNameParserToName::TestNameParserToName()
    throw()
    :Testable("Cdmw::NamingAndRepository::NameParser::to_name")
{
    // does nothing
}


TestNameParserToName::~TestNameParserToName()
    throw()
{
    // does nothing
}


TestNameParserToName::TestNameParserToName(const TestNameParserToName& rhs)
    throw()
    :Testable("Cdmw::NamingAndRepository::NameParser::to_name")
{
    // does nothing
}
		
TestNameParserToName&
TestNameParserToName::operator=(const TestNameParserToName& rhs)
    throw()
{
    // does nothing
    return *this;
}


void TestNameParserToName::test_to_name()
{
    TEST_INFO("Test name components with both ID and Kind field");
    {
        using namespace CosNaming;
        std::string input("first.1st/second.2nd");
	    try {
	        Name_var name = NamingAndRepository::NameParser::to_name(input);
		    TEST_CHECK(name->length()==2);
		    TEST_CHECK(strcmp((name.in())[0].id.in(),"first")==0);
		    TEST_CHECK(strcmp((name.in())[0].kind.in(),"1st")==0);
		    TEST_CHECK(strcmp((name.in())[1].id.in(),"second")==0);
		    TEST_CHECK(strcmp((name.in())[1].kind.in(),"2nd")==0);
	    } catch (const NamingAndRepository::InvalidNameException&) {
	        TEST_FAILED();
	    }
    }
    
    TEST_INFO("Test name components with missing Kind field");
    {
        using namespace CosNaming;
        std::string input("first.1st/second");
	    try {
	        Name_var name = NamingAndRepository::NameParser::to_name(input);
		    TEST_CHECK(name->length()==2);
		    TEST_CHECK(strcmp((name.in())[0].id.in(),"first")==0);
		    TEST_CHECK(strcmp((name.in())[0].kind.in(),"1st")==0);
		    TEST_CHECK(strcmp((name.in())[1].id.in(),"second")==0);
		    TEST_CHECK(strlen((name.in())[1].kind.in())==0);
	    } catch (const NamingAndRepository::InvalidNameException&) {
	        TEST_FAILED();
	    }
    }

    TEST_INFO("Test name components with missing ID field");
    {
        using namespace CosNaming;
        std::string input("first.1st/.2nd");
	    try {
	        Name_var name = NamingAndRepository::NameParser::to_name(input);
		    TEST_CHECK(name->length()==2);
		    TEST_CHECK(strcmp((name.in())[0].id.in(),"first")==0);
		    TEST_CHECK(strcmp((name.in())[0].kind.in(),"1st")==0);
		    TEST_CHECK(strlen((name.in())[1].id.in())==0);
		    TEST_CHECK(strcmp((name.in())[1].kind.in(),"2nd")==0);
	    } catch (const NamingAndRepository::InvalidNameException&) {
	        TEST_FAILED();
	    }
    }

    TEST_INFO("Test node with empty ID and Kind field");
    {
        using namespace CosNaming;
        std::string input("first.1st/./third.3rd");
	    try {
	        Name_var name = NamingAndRepository::NameParser::to_name(input);
		    TEST_CHECK(name->length()==3);
		    TEST_CHECK(strcmp((name.in())[0].id.in(),"first")==0);
		    TEST_CHECK(strcmp((name.in())[0].kind.in(),"1st")==0);
		    TEST_CHECK(strlen((name.in())[1].id.in())==0);
		    TEST_CHECK(strlen((name.in())[1].kind.in())==0);
		    TEST_CHECK(strcmp((name.in())[2].id.in(),"third")==0);
		    TEST_CHECK(strcmp((name.in())[2].kind.in(),"3rd")==0);
	    } catch (const NamingAndRepository::InvalidNameException&) {
	        TEST_FAILED();
	    }
    }
    TEST_INFO("Test nodes with characters that are escaped");
    {
        using namespace CosNaming;
        std::string input("first.1st\\.dot/second\\.dot.2nd/third\\/slash.3rd\n");
	    try {
	        Name_var name = NamingAndRepository::NameParser::to_name(input);
		    TEST_CHECK(name->length()==3);
		    TEST_CHECK(strcmp((name.in())[0].id.in(),"first")==0);
		    TEST_CHECK(strcmp((name.in())[0].kind.in(),"1st.dot")==0);
		    TEST_CHECK(strcmp((name.in())[1].id.in(),"second.dot")==0);
		    TEST_CHECK(strcmp((name.in())[1].kind.in(),"2nd")==0);
		    TEST_CHECK(strcmp((name.in())[2].id.in(),"third/slash")==0);
		    TEST_CHECK(strcmp((name.in())[2].kind.in(),"3rd\n")==0);
	    } catch (const NamingAndRepository::InvalidNameException&) {
	        TEST_FAILED();
	    }
    }
    TEST_INFO("Test empty name");
    {
        using namespace CosNaming;
        std::string input(".");
	    try {
	        Name_var name = NamingAndRepository::NameParser::to_name(input);
		    TEST_CHECK(name->length()==1);
		    TEST_CHECK(strlen((name.in())[0].id.in())==0);
		    TEST_CHECK(strlen((name.in())[0].kind.in())==0);
	    } catch (const NamingAndRepository::InvalidNameException&) {
	        TEST_FAILED();
	    }
    }
    {
        using namespace CosNaming;
        std::string input("");
	    try {
	        Name_var name = NamingAndRepository::NameParser::to_name(input);
    	    // The empty sequence is not a legal name
		    TEST_FAILED();
	    } catch (const NamingAndRepository::InvalidNameException&) {
	        TEST_SUCCEED();
	    }
    }
    TEST_INFO("Test ending '.'");
    {
        using namespace CosNaming;
        std::string input("first.1st/second.");
	    try {
	        Name_var name = NamingAndRepository::NameParser::to_name(input);
    		// If a name component has a non-empty id field and an empty kind field, 
            // the stringified representation consists only of the id field. A trailing 
            // '.' character is not permitted.
	        TEST_FAILED();
	    } catch (const NamingAndRepository::InvalidNameException&) {
	        TEST_SUCCEED();
	    }
    }
    TEST_INFO("Test starting '/'");
    {
        using namespace CosNaming;
        std::string input("/first.1st/second.2nd");
	    try {
	        Name_var name = NamingAndRepository::NameParser::to_name(input);
    		// The single '.' character is the only representation of a name 
            // component with empty id and kind fields.
	        TEST_FAILED();
	    } catch (const NamingAndRepository::InvalidNameException&) {
	        TEST_SUCCEED();
	    }
    }

}

void TestNameParserToName::do_tests()		
{    
    // This tests NameParser class
    //-------------------------------------------------
    // Testing string to name conversion
    //-------------------------------------------------
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (35);
    
    TEST_INFO("Testing NameParser::to_name operation");
    test_to_name();
}
		
} // End namespace NamingAndRepository
} // End namespace Cdmw

