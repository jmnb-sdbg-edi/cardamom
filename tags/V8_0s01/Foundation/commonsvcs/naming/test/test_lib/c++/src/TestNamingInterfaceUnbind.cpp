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


#include "test_lib/TestNamingInterfaceUnbind.hpp"
#include "test_lib/TestMacroDefs.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"


namespace Cdmw
{
namespace CommonSvcs
{
namespace Naming
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/


TestNamingInterfaceUnbind::TestNamingInterfaceUnbind (CosNaming::NamingContext_ptr nc,
						  test::TestObject_ptr               obj)
    throw()
    :Testable("Cdmw::CommonSvcs::Naming::NamingInterface::unbind"),
     m_context(CosNaming::NamingContext::_duplicate(nc)),
     m_test_object(test::TestObject::_duplicate(obj))
{
    // does nothing
}


TestNamingInterfaceUnbind::~TestNamingInterfaceUnbind()
    throw()
{
    // does nothing
}


TestNamingInterfaceUnbind::TestNamingInterfaceUnbind(const TestNamingInterfaceUnbind& rhs)
    throw()
    :Testable("Cdmw::CommonSvcs::Naming::NamingInterface::unbind"),
     m_context(CosNaming::NamingContext::_duplicate(rhs.m_context.in())),
     m_test_object(test::TestObject::_duplicate(rhs.m_test_object.in()))
{
    // does nothing
}
		
TestNamingInterfaceUnbind&
TestNamingInterfaceUnbind::operator=(const TestNamingInterfaceUnbind& rhs)
    throw()
{
    // does nothing
    return *this;
}


void TestNamingInterfaceUnbind::do_tests()		
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (5);
    
    NamingInterface ni0(m_context.in());
    
    // This tests NamingInterface class
    //-------------------------------------------------
    // Testing unbind operation
    //-------------------------------------------------
    TEST_INFO("Testing NamingInterface::unbind operation");
    std::string input("test.unbind");
    using namespace CosNaming;
    
    //
    // Try to unbind a binding that doesn't exist
    //
    TEST_INFO("Test unbind a binding that doesn't exist");
    {
        try {
	        ni0.unbind(input);
		    TEST_FAILED();
		} catch (const NamingContext::NotFound & ex) {
		    TEST_CHECK(ex.why == NamingContext::missing_node);
		} catch (...) {
		    TEST_FAILED();
		}		
    } 
	
	//
    // Try to unbind a binding when an intermediate context doesn't exist
    //    
    TEST_INFO("Test unbind a binding when an intermediate context doesn't exist");
    input = "test.unbind/no.node/name";
    {
        try {
	        ni0.unbind(input);
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
        ni0.bind("test.unbind/name",m_test_object.in(),true);
	} catch (...) {
	    NI_TEST_CERR("Failed to create initial binding test.unbind/name."
		          << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
	    TEST_FAILED();
	    return;
	}   
	
	//
    // Try to unbind a binding when an object is in the path
    //    
    TEST_INFO("Test unbind a binding when an object is in the path");
    input = "test.unbind/name/no.name";
    {
        try {
	        ni0.unbind(input);
		    TEST_FAILED();
		} catch (const NamingContext::NotFound & ex) {
		    TEST_CHECK(ex.why == NamingContext::not_context);
		} catch (...) {
		    TEST_FAILED();
		}		
    }
    
    //
    // Test unbind
    //    
    TEST_INFO("Test unbind");
    input = "test.unbind/name";
    {
        try {
	        ni0.unbind(input);
		    TEST_SUCCEED();
		} catch (...) {
		    TEST_FAILED();
		}		
    }     
	
	//
    // Try to unbind the same binding again
    //    
    TEST_INFO("Test unbind the same binding again");
    {
        try {
	        ni0.unbind(input);
		    TEST_FAILED();
		} catch (const NamingContext::NotFound & ex) {
		    TEST_CHECK(ex.why == NamingContext::missing_node);
		} catch (...) {
		    TEST_FAILED();
		}		
    }    
}

		
} // End namespace Naming
} // End namespace CommonSvcs
} // End namespace Cdmw

