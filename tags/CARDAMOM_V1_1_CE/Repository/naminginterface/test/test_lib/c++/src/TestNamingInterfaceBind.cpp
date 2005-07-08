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


#include "test_lib/TestNamingInterfaceBind.hpp"
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


TestNamingInterfaceBind::TestNamingInterfaceBind (CosNaming::NamingContext_ptr nc,
						  TestObject_ptr               obj)
    throw()
    :Testable("Cdmw::NamingAndRepository::NamingInterface::bind"),
     m_context(CosNaming::NamingContext::_duplicate(nc)),
     m_test_object(TestObject::_duplicate(obj))
{
    // does nothing
}


TestNamingInterfaceBind::~TestNamingInterfaceBind()
    throw()
{
    // does nothing
}


TestNamingInterfaceBind::TestNamingInterfaceBind(const TestNamingInterfaceBind& rhs)
    throw()
    :Testable("Cdmw::NamingAndRepository::NamingInterface::bind")
{
    // does nothing
}
		
TestNamingInterfaceBind&
TestNamingInterfaceBind::operator=(const TestNamingInterfaceBind& rhs)
    throw()
{
    // does nothing
    return *this;
}


	    
void TestNamingInterfaceBind::test_bind()
{
    NamingInterface ni0(m_context.in());
    //-------------------------------------------------
    // Testing bind operation
    //-------------------------------------------------
    TEST_INFO("Testing NamingInterface::bind operation");
    
    // This test creates the following bindings in the naming service:
    //     cdmw.test/archive.3/orbacus.mk
    using namespace CosNaming;

    //
    // Try to bind an object when an intermediate context doesn't exist
    //    
    std::string input("cdmw.test/archive.3/orbacus.mk");
    TEST_INFO("Attempt to bind cdmw.test/archive.3/orbacus.mk");
    {
	    try {
	        ni0.bind(input,m_test_object.in(),false);
	        TEST_FAILED();
	    } catch (const NamingContext::NotFound& ex) {
	        TEST_CHECK(ex.why == NamingContext::missing_node);
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
    // Forcing binding
    TEST_INFO("Forcing bind of cdmw.test/archive.3/orbacus.mk");
    {
	    try {
	        ni0.bind(input,m_test_object.in(),true);
		    CORBA::Object_var obj = ni0.resolve(input);
		    TEST_CHECK(!CORBA::is_nil(obj.in()));
	    } catch (...) {
	        TEST_FAILED();
	    }
    }    
    //
    // Try to bind an object when an object is in the path
    //	 
	input = "cdmw.test/archive.3/orbacus.mk/bla.bla";
	TEST_INFO("Attempt to bind cdmw.test/archive.3/orbacus.mk/bla.bla");
    try {
        ni0.bind(input,m_test_object.in(),false);
	    TEST_FAILED();
	} catch (const NamingContext::NotFound&) {
	    TEST_SUCCEED();
	} catch (...) {
	    TEST_FAILED();
    }
    // Forcing Binding
    try {
        ni0.bind(input,m_test_object.in(),true);
	    TEST_FAILED();
	} catch (const NamingContext::NotFound&) {
	    TEST_SUCCEED();
	} catch (...) {
	    TEST_FAILED();
    }   
    //
    // Try to bind an object using the name of an existing context
    //    
    input = "cdmw.test/archive.3";
    TEST_INFO("Test of binding an object using the name of an existing context");
    {
	    try {
	        ni0.bind(input,m_test_object.in(),false);
	        TEST_FAILED();
	    } catch (const NamingContext::AlreadyBound& ) {
    	    TEST_SUCCEED();
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
    // Forcing bind 
    TEST_INFO("Test of forcing binding an object using the name of an existing context");
    {
	    try {
	        ni0.bind(input,m_test_object.in(),true);
	        TEST_FAILED();
	    } catch (const NamingContext::NotFound& ex) {
	        TEST_CHECK(ex.why == NamingContext::not_object);
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
       
    //
    // Try to bind an object using the name of an existing object
    //    
    input = "cdmw.test/archive.3/orbacus.mk";
    TEST_INFO("Test of binding an object using the name of an existing object");
    {
	    try {
	        ni0.bind(input,m_test_object.in(),false);
	        TEST_FAILED();
	    } catch (const NamingContext::AlreadyBound& ) {
    	    TEST_SUCCEED();
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
    // Forcing bind 
    TEST_INFO("Test of forcing binding an object using the name of an existing object");
    {
	    try {
	        ni0.bind(input,m_test_object.in(),true);
	        TEST_SUCCEED();
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
}

void TestNamingInterfaceBind::test_rebind()
{
    NamingInterface ni0(m_context.in());
    //-------------------------------------------------
    // Testing rebind operation
    //-------------------------------------------------
    TEST_INFO("Testing NamingInterface::rebind operation");
    
    // This test creates the following bindings in the naming service:
    //     cdmw.test/archive.4/orbacus.mk
    using namespace CosNaming;

    //
    // Try to rebind an object when an intermediate context doesn't exist
    //    
    std::string input("cdmw.test/archive.4/orbacus.mk");
    TEST_INFO("Attempt to rebind cdmw.test/archive.4/orbacus.mk");
    {
	    try {
	        ni0.rebind(input,m_test_object.in(),false);
	        TEST_FAILED();
	    } catch (const NamingContext::NotFound& ex) {
	        TEST_CHECK(ex.why == NamingContext::missing_node);
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
    // Forcing binding
    TEST_INFO("Forcing rebind of cdmw.test/archive.4/orbacus.mk");
    {
	    try {
	        ni0.rebind(input,m_test_object.in(),true);
		    CORBA::Object_var obj = ni0.resolve(input);
		    TEST_CHECK(!CORBA::is_nil(obj.in()));
	    } catch (...) {
	        TEST_FAILED();
	    }
    }    
    //
    // Try to rebind an object when an object is in the path
    //	 
	input = "cdmw.test/archive.4/orbacus.mk/bla.bla";
	TEST_INFO("Attempt to rebind cdmw.test/archive.4/orbacus.mk/bla.bla");
    try {
        ni0.rebind(input,m_test_object.in(),false);
	    TEST_FAILED();
	} catch (const NamingContext::NotFound& ex) {
	    TEST_CHECK(ex.why == NamingContext::not_context);
	} catch (...) {
	    TEST_FAILED();
    }
    // Forcing Rebind
    try {
        ni0.rebind(input,m_test_object.in(),true);
	    TEST_FAILED();
	} catch (const NamingContext::NotFound& ex) {
	    TEST_CHECK(ex.why == NamingContext::not_context);
	} catch (...) {
	    TEST_FAILED();
    }
    //
    // Try to rebind an object using the name of an existing context
    //    
    input = "cdmw.test/archive.4";
    TEST_INFO("Test of rebinding an object using the name of an existing context");
    {
	    try {
	        ni0.rebind(input,m_test_object.in(),false);
	        TEST_FAILED();
	    } catch (const NamingContext::NotFound& ex) {
	        TEST_CHECK(ex.why == NamingContext::not_object);
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
    // Forcing rebind 
    TEST_INFO("Test of forcing rebinding an object using the name of an existing context");
    {
	    try {
	        ni0.rebind(input,m_test_object.in(),true);
	        TEST_FAILED();
	    } catch (const NamingContext::NotFound& ex) {
	        TEST_CHECK(ex.why == NamingContext::not_object);
	    } catch (...) {
	        TEST_FAILED();
	    }
    }       
    //
    // Try to rebind an object using the name of an existing object
    //    
    input = "cdmw.test/archive.4/orbacus.mk";
    TEST_INFO("Test of rebinding an object using the name of an existing object");
    {
	    try {
	        ni0.rebind(input,m_test_object.in(),false);
	        TEST_SUCCEED();
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
    // Forcing rebind 
    TEST_INFO("Test of forcing rebinding an object using the name of an existing object");
    {
	    try {
	        ni0.rebind(input,m_test_object.in(),true);
	        TEST_SUCCEED();
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
}

void TestNamingInterfaceBind::do_tests()		
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (16);
    
    test_bind();
    test_rebind();
}

		
} // End namespace NamingAndRepository
} // End namespace Cdmw

