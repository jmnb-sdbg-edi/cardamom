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


#include "test_lib/TestNamingInterfaceBindContext.hpp"
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


TestNamingInterfaceBindContext::TestNamingInterfaceBindContext (CosNaming::NamingContext_ptr nc,
						  TestObject_ptr               obj)
    throw()
    :Testable("Cdmw::NamingAndRepository::NamingInterface::bind_context"),
     m_context(CosNaming::NamingContext::_duplicate(nc)),
     m_test_object(TestObject::_duplicate(obj))
{
    // does nothing
}


TestNamingInterfaceBindContext::~TestNamingInterfaceBindContext()
    throw()
{
    // does nothing
}


TestNamingInterfaceBindContext::TestNamingInterfaceBindContext(const TestNamingInterfaceBindContext& rhs)
    throw()
    :Testable("Cdmw::NamingAndRepository::NamingInterface::bind_context"),
     m_context(CosNaming::NamingContext::_duplicate(rhs.m_context.in())),
     m_test_object(TestObject::_duplicate(rhs.m_test_object.in()))
{
    // does nothing
}
		
TestNamingInterfaceBindContext&
TestNamingInterfaceBindContext::operator=(const TestNamingInterfaceBindContext& rhs)
    throw()
{
    // does nothing
    return *this;
}



void TestNamingInterfaceBindContext::test_bind_new_context()		
{
    NamingInterface ni0(m_context.in());
    // This tests NamingInterface class
    //-------------------------------------------------
    // Testing bind_new_context operation
    //-------------------------------------------------
    TEST_INFO("Testing NamingInterface::bind_new_context operation");
    
    // This test creates the following bindings in the naming service:
    //     cdmw.test/runtime/config.mk
    //     cdmw.test/runtime/naminginterface/c++/test/idl
    using namespace CosNaming;
    std::string input("cdmw.test");
    TEST_INFO("Creating cdmw.test binding");
    {
	    try {
	        NamingInterface ni = ni0.bind_new_context(input,false);
		    CORBA::Object_var obj = ni0.resolve(input);
		    TEST_CHECK(!CORBA::is_nil(obj.in()));
	    } catch (const NamingContext::AlreadyBound&) {
	        NI_TEST_CERR("cdmw.test context should be cleared before testing."
		            << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
	        TEST_FAILED();
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
    // expecting an AlreadyBound exception
    {
	    try {
	        NamingInterface ni = ni0.bind_new_context(input,false);
	        TEST_FAILED();
	    } catch (const NamingContext::AlreadyBound&) {
	        TEST_SUCCEED();
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
    // This should not be OK (AlreadyBound exception expected)
    {
        try {
	        NamingInterface ni = ni0.bind_new_context(input,true);
	        TEST_FAILED();
	    } catch (const NamingContext::AlreadyBound&) {
	        TEST_SUCCEED();
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
    // cdmw.test/runtime/naminginterface/c++/test/idl
    input += "/runtime/naminginterface/c++/test/idl";
    TEST_INFO("Creating cdmw.test/runtime/naminginterface/c++/test/idl binding");
    {
	    try {
	        NamingInterface ni = ni0.bind_new_context(input,false);
	        TEST_FAILED();
	    } catch (const NamingContext::NotFound&) {
	        TEST_SUCCEED();
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
    //
    // Try to bind a context when an intermediate context
    // doesn't exist (Forcing creation)
    // 
    {
	    try {
	        NamingInterface ni = ni0.bind_new_context(input,true);
		    CORBA::Object_var obj = ni0.resolve(input);
		    TEST_CHECK(!CORBA::is_nil(obj.in()));
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
    //
    // Try to bind a context when an object is in the path
    //
    //    cdmw.test/runtime/config.mk
    TEST_INFO("Binding a context when an object is in the path");
	try {
	    Name_var name // This has already been tested
            = NamingInterface::to_name("cdmw.test/runtime/config.mk"); 
	    m_context->bind(name.in(),m_test_object.in());
	} catch (const CORBA::Exception & ex) {
	    NI_TEST_CERR("Unexpected error " << ex._name() 
		          << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");  
	    return;
	} catch (const InvalidNameException & ex) {
	    NI_TEST_CERR("Unexpected error " << ex.what() 
	              << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
	    return;
	}
	input = "cdmw.test/runtime/config.mk";
    try {
        NamingInterface ni = ni0.bind_new_context(input,true);
	    TEST_FAILED();
	} catch (const NamingContext::AlreadyBound&) {
	    TEST_SUCCEED();
	} catch (...) {
	    TEST_FAILED();
    }
	input = "cdmw.test/runtime/config.mk/bla.bla";
    try {
        NamingInterface ni = ni0.bind_new_context(input,true);
	    TEST_FAILED();
	} catch (const NamingContext::NotFound&) {
	    TEST_SUCCEED();
	} catch (...) {
	    TEST_FAILED();
    }
}

void TestNamingInterfaceBindContext::test_bind_context()
{	
    NamingInterface ni0(m_context.in());
    //-------------------------------------------------
    // Testing bind_context operation
    //-------------------------------------------------
    TEST_INFO("Testing NamingInterface::bind_context operation");
    
    // This test creates the following bindings in the naming service:
    //     cdmw.test/archive/orbacus.mk
    //     cdmw.test/archive/naminginterface/c++ 
    //        (link to cdmw.test/runtime/naminginterface/c++)
    using namespace CosNaming;
    
    // Get a reference to the naming context bound to 
    // cdmw.test/runtime/naminginterface/c++
    NamingContext_var nc;    
	try {
	    // resolve has already been tested
	    CORBA::Object_var obj = ni0.resolve("cdmw.test/runtime/naminginterface/c++");
	    nc = NamingContext::_narrow(obj.in());
	    if (CORBA::is_nil(nc.in())) {
	        NI_TEST_CERR("Unexpected error (test_bind_new_context failed before)" 
		            << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
		    return;
	    }
	} catch (const CORBA::Exception & ex) {
	    NI_TEST_CERR("Unexpected error " << ex._name() 
		          << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
	    return;
	} catch (const InvalidNameException & ex) {
	    NI_TEST_CERR("Unexpected error " << ex.what() 
		          << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
	    return;
	}
    //
    // Try to bind a context when an intermediate context doesn't exist
    //
    // Expecting NamingContext::NotFound exception 
    //     ex.why == NamingContext::not_context
    std::string input("cdmw.test/archive/runtime/naminginterface/c++");
    TEST_INFO("Attempt to bind cdmw.test/archive/runtime/naminginterface/c++");
    {
	    try {
	        ni0.bind_context(input,nc.in(),false);
	        TEST_FAILED();
	    } catch (const NamingContext::NotFound& ex) {
	        TEST_CHECK(ex.why == NamingContext::missing_node);
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
    // Forcing creation
    TEST_INFO("Forcing bind of cdmw.test/archive/runtime/naminginterface/c++");
    {
	    try {
	        ni0.bind_context(input,nc.in(),true);
		    CORBA::Object_var obj = ni0.resolve(input);
		    TEST_CHECK(!CORBA::is_nil(obj.in()));
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
	  
	//
    // Try to bind a context when an object is in the path
    //
    //    cdmw.test/archive/orbacus.mk
    TEST_INFO("Binding a context when an object is in the path");
	try {
	    Name_var name // This has already been tested
            = NamingInterface::to_name("cdmw.test/archive/orbacus.mk"); 
	    m_context->bind(name.in(),m_test_object.in());
	} catch (const CORBA::Exception & ex) {
	    NI_TEST_CERR("Unexpected error " << ex._name() 
		          << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
	    return;
	} catch (const InvalidNameException & ex) {
	    NI_TEST_CERR("Unexpected error " << ex.what() 
		          << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
	    return;
	}
    // Expecting NamingContext::NotFound exception 
    //     ex.why == NamingContext::not_context
    input = "cdmw.test/archive/orbacus.mk/runtime/naminginterface/c++";
    TEST_INFO("Attempt to bind cdmw.test/archive/orbacus.mk/runtime/naminginterface/c++");
    {
	    try {
	        NamingInterface n(nc.in()); // to test bind_context witn NamingInterface
	        ni0.bind_context(input,nc.in(),false);
	        TEST_FAILED();
	    } catch (const NamingContext::NotFound& ex) {
	        TEST_CHECK(ex.why == NamingContext::not_context);
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
    // Forcing creation (exception NamingContext::NotFound expected)
    TEST_INFO("Forcing bind of cdmw.test/archive/orbacus.mk/runtime/naminginterface/c++");
    {
	    try {
	        ni0.bind_context(input,nc.in(),true);
	        TEST_FAILED();
	    } catch (const NamingContext::NotFound& ex) {
	        TEST_CHECK(ex.why == NamingContext::not_context);
	    } catch (...) {
	        TEST_FAILED();
	    }
    }    
        
    //
    // Try to bind a context using the name of an existing object
    //    
    input = "cdmw.test/archive/orbacus.mk";
    TEST_INFO("Test of binding a context using the name of an existing object");
    {
	    try {
	        ni0.bind_context(input,nc.in(),false);
	        TEST_FAILED();
	    } catch (const NamingContext::AlreadyBound& ) {
	        TEST_SUCCEED();
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
    // Forcing bind (exception NamingContext::NotFound expected)
    TEST_INFO("Test of forcing bind of a context using the name of an existing object");
    {
	    try {
	        ni0.bind_context(input,nc.in(),true);
	        TEST_FAILED();
	    } catch (const NamingContext::NotFound& ex) {
	        TEST_CHECK(ex.why == NamingContext::not_context);
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
	    
    //
    // Try to bind a context using the name of an existing context
    //    
    input = "cdmw.test/archive/runtime/naminginterface/c++";
    TEST_INFO("Test of binding a context using the name of an existing context");
    {
	    try {
	        ni0.bind_context(input,nc.in(),false);
	        TEST_FAILED();
	    } catch (const NamingContext::AlreadyBound& ) {
	        TEST_SUCCEED();
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
    // Forcing bind 
    TEST_INFO("Test of forcing bind of a context using the name of an existing context");
    {
	    try {
	        ni0.bind_context(input,nc.in(),true);
	        TEST_SUCCEED();
	    } catch (...) {
	        TEST_FAILED();
	    }
    }										        	    
}

void TestNamingInterfaceBindContext::test_rebind_context()
{   
    NamingInterface ni0(m_context.in());
    //-------------------------------------------------
    // Testing rebind_context operation
    //-------------------------------------------------
    TEST_INFO("Testing NamingInterface::rebind_context operation");
    
    // This test creates the following bindings in the naming service:
    //     cdmw.test/archive.2/orbacus.mk
    //     cdmw.test/archive.2/naminginterface/c++ 
    //        (link to cdmw.test/runtime/naminginterface/c++)
    using namespace CosNaming;
    
    // Get a reference to the naming context bound to 
    // cdmw.test/runtime/naminginterface/c++
    NamingContext_var nc;    
	try {
	    Name_var name // This has already been tested
            = NamingInterface::to_name("cdmw.test/runtime/naminginterface/c++"); 
	    CORBA::Object_var obj = m_context->resolve(name.in());
	    nc = NamingContext::_narrow(obj.in());
	    if (CORBA::is_nil(nc.in())) {
	        NI_TEST_CERR("Unexpected error (test_bind_new_context failed before)" 
		            << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
		    return;
	    }
	} catch (const CORBA::Exception & ex) {
	    NI_TEST_CERR("Unexpected error " << ex._name() 
		            << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
	    return;
	} catch (const InvalidNameException & ex) {
	    NI_TEST_CERR("Unexpected error " << ex.what() 
		            << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
	    return;
	}
    //
    // Try to rebind a context when an intermediate context doesn't exist
    //
    // Expecting NamingContext::NotFound exception 
    //     ex.why == NamingContext::not_context
    std::string input("cdmw.test/archive.2/runtime/naminginterface/c++");
    TEST_INFO("Attempt to rebind cdmw.test/archive.2/runtime/naminginterface/c++");
    {
	    try {
	        ni0.rebind_context(input,nc.in(),false);
	        TEST_FAILED();
	    } catch (const NamingContext::NotFound& ex) {
	        TEST_CHECK(ex.why == NamingContext::missing_node);
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
    // Forcing creation
    TEST_INFO("Forcing rebind of cdmw.test/archive.2/runtime/naminginterface/c++");
    {
	    try {
	        NamingInterface n(nc.in()); // to test rebind_context with NamingInterface
	        ni0.rebind_context(input,nc.in(),true);
		    CORBA::Object_var obj = ni0.resolve(input);
		    TEST_CHECK(!CORBA::is_nil(obj.in()));
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
	  
	//
    // Try to rebind a context when an object is in the path
    //
    //    cdmw.test/archive.2/orbacus.mk
    TEST_INFO("Rebinding a context when an object is in the path");
	try {
	    Name_var name // This has already been tested
            = NamingInterface::to_name("cdmw.test/archive.2/orbacus.mk"); 
	    m_context->bind(name.in(),m_test_object.in());
	} catch (const CORBA::Exception & ex) {
	    NI_TEST_CERR("Unexpected error " << ex._name() 
		          << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
	    return;
	} catch (const InvalidNameException & ex) {
	    NI_TEST_CERR("Unexpected error " << ex.what() 
		          << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
	    return;
	}
    // Expecting NamingContext::NotFound exception 
    //     ex.why == NamingContext::not_context
    input = "cdmw.test/archive.2/orbacus.mk/runtime/naminginterface/c++";
    TEST_INFO("Attempt to rebind cdmw.test/archive.2/orbacus.mk/runtime/naminginterface/c++");
    {
	    try {
	        ni0.rebind_context(input,nc.in(),false);
	        TEST_FAILED();
	    } catch (const NamingContext::NotFound& ex) {
	        TEST_CHECK(ex.why == NamingContext::not_context);
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
    // Forcing creation (exception NamingContext::NotFound expected)
    TEST_INFO("Forcing rebind of cdmw.test/archive/orbacus.mk/runtime/naminginterface/c++");
    {
	    try {
	        ni0.rebind_context(input,nc.in(),true);
	        TEST_FAILED();
	    } catch (const NamingContext::NotFound& ex) {
	        TEST_CHECK(ex.why == NamingContext::not_context);
	    } catch (...) {
	        TEST_FAILED();
	    }
    }    
        
    //
    // Try to rebind a context using the name of an existing object
    //    
    input = "cdmw.test/archive.2/orbacus.mk";
    TEST_INFO("Test of rebinding a context using the name of an existing object");
    {
	    try {
	        ni0.rebind_context(input,nc.in(),false);
	        TEST_FAILED();
	    } catch (const NamingContext::NotFound& ex) {
	        TEST_CHECK(ex.why == NamingContext::not_context);
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
    // Forcing rebind (exception NamingContext::NotFound expected)
    TEST_INFO("Test of forcing rebind of a context using the name of an existing object");
    {
	    try {
	        ni0.rebind_context(input,nc.in(),true);
	        TEST_FAILED();
	    } catch (const NamingContext::NotFound& ex) {
	        TEST_CHECK(ex.why == NamingContext::not_context);
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
	    
    //
    // Try to rebind a context using the name of an existing context
    //    
    input = "cdmw.test/archive.2/runtime/naminginterface/c++";
    TEST_INFO("Test of rebinding a context using the name of an existing context");
    {
	    try {
	        ni0.rebind_context(input,nc.in(),false);
	        TEST_SUCCEED();
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
    // Forcing bind 
    TEST_INFO("Test of forcing rebind of a context using the name of an existing context");
    {
	    try {
	        ni0.rebind_context(input,nc.in(),true);
	        TEST_SUCCEED();
	    } catch (...) {
	        TEST_FAILED();
	    }
    }
}

void TestNamingInterfaceBindContext::test_new_context()
{  
    NamingInterface ni0(m_context.in());
    //-------------------------------------------------
    // Testing new_context operation
    //-------------------------------------------------
    TEST_INFO("Testing NamingInterface::new_context operation");
    
    // This test creates the following bindings in the naming service:
    //     cdmw.test/runtime/naminginterface/c++/test/incl
    using namespace CosNaming;
    
    std::string input("cdmw.test/runtime/naminginterface/c++/test");
    
    try {
        CORBA::Object_var obj = ni0.resolve(input);
	    NamingContext_var nc = NamingContext::_narrow(obj.in());
	    
	    if (CORBA::is_nil(nc.in())) {
	        NI_TEST_CERR("Unexpected error (" << input << " not present!)" 
		            << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
		    return;
	    }
        NamingInterface ni(nc.in());
	    
	    ni.bind_context("incl",ni.new_context(),false);
	    
	    input += "/incl";
	    obj = ni0.resolve(input);
	    TEST_CHECK(!CORBA::is_nil(obj.in()));	    
	} catch (...) {
	    TEST_FAILED();
    }
}

void TestNamingInterfaceBindContext::do_tests()		
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (24);
    
    test_bind_new_context();
    test_bind_context();
	test_rebind_context();
	test_new_context();
}


		
} // End namespace NamingAndRepository
} // End namespace Cdmw

