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


#include "test_lib/TestNamingInterfaceResolve.hpp"
#include "test_lib/TestMacroDefs.hpp"
#include "Repository/naminginterface/NamingInterface.hpp"


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


TestNamingInterfaceResolve::TestNamingInterfaceResolve (CosNaming::NamingContext_ptr nc,
						  TestObject_ptr               obj)
    throw()
    :Testable("Cdmw::NamingAndRepository::NamingInterface::resolve"),
     m_context(CosNaming::NamingContext::_duplicate(nc)),
     m_test_object(TestObject::_duplicate(obj))
{
    // does nothing
}


TestNamingInterfaceResolve::~TestNamingInterfaceResolve()
    throw()
{
    // does nothing
}


TestNamingInterfaceResolve::TestNamingInterfaceResolve(const TestNamingInterfaceResolve& rhs)
    throw()
    :Testable("Cdmw::NamingAndRepository::NamingInterface::resolve"),
     m_context(CosNaming::NamingContext::_duplicate(rhs.m_context.in())),
     m_test_object(TestObject::_duplicate(rhs.m_test_object.in()))
{
    // does nothing
}
		
TestNamingInterfaceResolve&
TestNamingInterfaceResolve::operator=(const TestNamingInterfaceResolve& rhs)
    throw()
{
    // does nothing
    return *this;
}


void TestNamingInterfaceResolve::do_tests()		
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (4);
    
    NamingInterface ni0(m_context.in());
    using namespace CosNaming;
    
    // This tests NamingInterface class
    //-------------------------------------------------
    // Testing resolve operation
    //-------------------------------------------------
    
    TEST_INFO("Testing NamingInterface::resolve operation");
    std::string input("test.resolve");
    //
    // Try to resolve a binding that doesn't exist
    //
    TEST_INFO("Test resolve a binding that doesn't exist");
    {
        try {
	        CORBA::Object_var obj = ni0.resolve(input);
		    TEST_FAILED();
		} catch (const NamingContext::NotFound & ex) {
		    TEST_CHECK(ex.why == NamingContext::missing_node);
		} catch (...) {
		    TEST_FAILED();
		}		
    }
       
    //
    // Try to resolve a binding when an intermediate context doesn't exist
    //
    TEST_INFO("Test resolve a binding when an intermediate context doesn't exist");
    input = "test.resolve/no.node/name";
    {
        try {
	        CORBA::Object_var obj = ni0.resolve(input);
		    TEST_FAILED();
		} catch (const NamingContext::NotFound & ex) {
		    TEST_CHECK(ex.why == NamingContext::missing_node);
		} catch (...) {
		    TEST_FAILED();
		}		
    }
    
    // Creating correct binding 
    //     test.resolve/name
    try {
        Name_var name = NamingInterface::to_name("test.resolve");
		NamingContext_var nc = m_context->bind_new_context(name.in());
		name = NamingInterface::to_name("test.resolve/name");
		m_context->bind(name.in(),m_test_object.in());
	} catch (...) {
	    NI_TEST_CERR("Failed to create initial binding test.resolve/name."
		          << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
	    TEST_FAILED();
	}
	    
    //
    // Test resolve
    //
    TEST_INFO("Test resolve");
    input = "test.resolve/name";
    {
        try {
	        CORBA::Object_var obj = ni0.resolve(input);
		    TEST_CHECK(!CORBA::is_nil(obj.in()));
		} catch (...) {
		    TEST_FAILED();
		}		
    }
        
    //
    // Try to resolve a binding when an object is in the path
    //
    TEST_INFO("Test resolve a binding when an object is in the path");
    input = "test.resolve/name/no.name";
    {
        try {
	        CORBA::Object_var obj = ni0.resolve(input);
		    TEST_FAILED();
		} catch (const NamingContext::NotFound & ex) {
		    TEST_CHECK(ex.why == NamingContext::not_context);
		} catch (...) {
		    TEST_FAILED();
		}		
    }
}

		
} // End namespace NamingAndRepository
} // End namespace Cdmw

