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


#include "test_lib/TestNamingInterfaceDestroy.hpp"
#include "test_lib/TestMacroDefs.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "Foundation/commonsvcs/naming/NamingUtil.hpp"

namespace 
{
    typedef Cdmw::CommonSvcs::Naming::NamingUtil<CosNaming::NamingContext> Util;

    bool 
    install_initial_bindings(Cdmw::CommonSvcs::Naming::NamingInterface& n,
                             const std::string &                      base,
                             CORBA::Object_ptr                        obj)
    {    
        bool result = true;
        try {
            Cdmw::CommonSvcs::Naming::NamingInterface ni = n.bind_new_context(base,true);
            
            ni.bind("bin/CVS/Entries",obj,true);
            ni.bind("bin/CVS/Repository",obj,true);
            ni.bind("bin/CVS/Root",obj,true);
            ni.bind("bin/Makefile",obj,true);
            ni.bind("bin/Makefile.mak",obj,true);
            ni.bind("bin/Makefile.tmpl",obj,true);
            ni.bind("bin/mk_idl_include.sh",obj,true);
            ni.bind("bin/CdmwCreateClass",obj,true);
            ni.bind("bin/CdmwCreateCSC",obj,true);
            ni.bind("commonsvcs/naming/c++/CVS/Entries",obj,true);
            ni.bind("commonsvcs/naming/c++/CVS/Entries.Log",obj,true);
            ni.bind("commonsvcs/naming/c++/CVS/Repository",obj,true);
            ni.bind("commonsvcs/naming/c++/CVS/Root",obj,true);
            ni.bind("commonsvcs/naming/c++/incl/CVS/Entries",obj,true);
            ni.bind("commonsvcs/naming/c++/incl/CVS/Repository",obj,true);
            ni.bind("commonsvcs/naming/c++/incl/CVS/Root",obj,true);
            ni.bind("commonsvcs/naming/c++/incl/NameParser.hpp",obj,true);
            ni.bind("commonsvcs/naming/c++/incl/NameParserContext.hpp",obj,true);
            ni.bind("commonsvcs/naming/c++/incl/NameParserFSM.hpp",obj,true);
            ni.bind("commonsvcs/naming/c++/src/CSC.mk",obj,true);
            ni.bind("commonsvcs/naming/c++/src/CVS/Entries",obj,true);
            ni.bind("commonsvcs/naming/c++/src/CVS/Repository",obj,true);
            ni.bind("commonsvcs/naming/c++/src/CVS/Root",obj,true);
            ni.bind("commonsvcs/naming/c++/src/InvalidName.cpp",obj,true);
            ni.bind("commonsvcs/naming/c++/src/main.cpp",obj,true);
            ni.bind("commonsvcs/naming/c++/src/NameParser.cpp",obj,true);
            ni.bind("commonsvcs/naming/c++/src/NameParserContext.cpp",obj,true);
            ni.bind("commonsvcs/naming/c++/src/NameParserFSM.cpp",obj,true);
            ni.bind("commonsvcs/naming/c++/src/NamingInterface.cpp",obj,true);
            Cdmw::CommonSvcs::Naming::NamingInterface n 
                = ni.bind_new_context("commonsvcs/naming/ada.1",true);	
            n   = ni.bind_new_context("commonsvcs/naming/ada.2",true);	
        } catch (...) {
            NI_TEST_CERR("Unexpected error (could'nt install initial bindings)"
                         << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
            result = false;
        }
        return result;
    }

};

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


TestNamingInterfaceDestroy::TestNamingInterfaceDestroy (CosNaming::NamingContext_ptr nc,
						  test::TestObject_ptr               obj)
    throw()
    :Testable("Cdmw::CommonSvcs::Naming::NamingInterface::destroy"),
     m_context(CosNaming::NamingContext::_duplicate(nc)),
     m_test_object(test::TestObject::_duplicate(obj))
{
    // does nothing
}


TestNamingInterfaceDestroy::~TestNamingInterfaceDestroy()
    throw()
{
    // does nothing
}


TestNamingInterfaceDestroy::TestNamingInterfaceDestroy(const TestNamingInterfaceDestroy& rhs)
    throw()
    :Testable("Cdmw::CommonSvcs::Naming::NamingInterface::destroy"),
     m_context(CosNaming::NamingContext::_duplicate(rhs.m_context.in())),
     m_test_object(test::TestObject::_duplicate(rhs.m_test_object.in()))
{
    // does nothing
}
		
TestNamingInterfaceDestroy&
TestNamingInterfaceDestroy::operator=(const TestNamingInterfaceDestroy& rhs)
    throw()
{
    // does nothing
    return *this;
}


void TestNamingInterfaceDestroy::do_tests()		
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (10);
    
    NamingInterface ni0(m_context.in());
    
    // This tests NamingInterface class
    //-------------------------------------------------
    // Testing destroy operation
    //-------------------------------------------------
    TEST_INFO("Testing NamingInterface::destroy operation");
    using namespace CosNaming;    	
	
    const std::string base("test.destroy/runtime");
        
	// Install test name tree
    if (!install_initial_bindings(ni0, base, m_test_object.in())) {
	    TEST_FAILED();
	    return;
    }
    // Install test name tree
//     if (!install_initial_bindings(ni0, "toto/test.destroy/runtime", m_test_object.in())) {
// 	    TEST_FAILED();
// 	    return;
//     }
	// 
	// Additional test to test NamingUtil<>::resolve_name() with a 'NotFound' context
	//
	TEST_INFO("Additional tests to NamingUtil<>::resolve_name() function");
	{
	    std::string input(base);
        // Testing NotFound Exception
	    input += "/commonsvcs/naming/dummy/ada.1";
	    try {
            NamingContext_var nc = Util::resolve_name(ni0,input);
		    TEST_FAILED();
        } catch (const CosNaming::NamingContext::NotFound&) {
		    TEST_SUCCEED();
        } catch (...) {
	        TEST_FAILED();
	    }
        // Testing TypeMismatch Exception
	    input = base + "/bin/CVS/Root";
	    try {
            NamingContext_var nc = Util::resolve_name(ni0,input);
		    TEST_FAILED();
        } catch (const Cdmw::Common::TypeMismatchException &) {
		    TEST_SUCCEED();
	    } catch (...) {
	        TEST_FAILED();
	    }
	}
	// 
	// Destroy commonsvcs/naming/ada.1 naming context
	//
	TEST_INFO("Destroy test.destroy/runtime/commonsvcs/naming/ada.1 (empty) naming context");
	{
	    std::string input(base);
	    input += "/commonsvcs/naming/ada.1";
	    try {
            NamingContext_var nc = Util::resolve_name(ni0,input);
		    NamingInterface n(nc.in());
		    n.destroy();
		    ni0.unbind(input);
		    TEST_SUCCEED();
	    } catch (...) {
	        TEST_FAILED();
	    }
	    // Forcing destroy
	    input = base + "/commonsvcs/naming/ada.2";
	    try {
            NamingContext_var nc = Util::resolve_name(ni0,input);
		    NamingInterface n(nc.in());
		    bool destroyed = n.destroy_all();
		    TEST_CHECK(destroyed);
		    if (destroyed) ni0.unbind(input);
		    TEST_SUCCEED();
	    } catch (...) {
	        TEST_FAILED();
	    }
	}

    //
    // Try to destroy a context that's not empty
    //    
	TEST_INFO("Test of destroying a context that's not empty");
	{
	    std::string input(base);
	    NamingContext_var nc;
	    try {
            nc = Util::resolve_name(ni0,input);
		    NamingInterface n(nc.in());
		    n.destroy();
		    TEST_FAILED();
		} catch (const NamingContext::NotEmpty&) {
		    TEST_SUCCEED();
	    } catch (...) {
	        TEST_FAILED();
	    }
	    // Forcing destroy
	    try {
		    NamingInterface n(nc.in());
		    TEST_CHECK(n.destroy_all());
	    } catch (...) {
	        TEST_FAILED();
	    }
	    // Is it destroyed ?
	    try {
	        Name_var name = NamingInterface::to_name("toto");
    // The following test (replaced by the bind_new_context) produce
    // a naming&repository AssertionFailedException, Cf PCR-0062
    //		ni0.bind(base + "/toto", m_test_object.in(), false);
	        NamingContext_var nc__ = nc->bind_new_context(name.in());
		    TEST_FAILED();
	    } catch (const CORBA::OBJECT_NOT_EXIST&) {
	        TEST_SUCCEED();
		    // remove binding
		    try {
		        ni0.unbind(input);
		    } catch (...) {
		        // Unexpected error
			    TEST_FAILED();
		    }
		} catch (...) {
		    TEST_FAILED();
	    }
	    
	}
    //
    // Try to destroy a context that's not empty with an undestroyable context
    //       
    // Install test name tree
    if (!install_initial_bindings(ni0, base, m_test_object.in())) {
	    TEST_FAILED();
	    return;
    }
	TEST_INFO("Test of destroying a context that's not empty with an undestroyable context");
	{
	    // To simulate an undestroyable context, we destroy a context without
	    // removing the binding. This will generate a system exception when its
	    // destruction is requested.
	    std::string input(base);
	    input += "/commonsvcs/naming/ada.1";
	    try {
            NamingContext_var nc = Util::resolve_name(ni0,input);
		    NamingInterface n(nc.in());
		    n.destroy();
	    } catch (...) {
	        TEST_FAILED();
		    return;
	    }
	
	    // Forcing destroy	    
        NamingContext_var nc;
	    try {
            nc = Util::resolve_name(ni0,base);
	    } catch (...) {
	        TEST_FAILED();
		    return;
	    }	    
	    try {
		    NamingInterface n(nc.in());
		    TEST_CHECK(!n.destroy_all());
	    } catch (...) {
	        TEST_FAILED();
	    }	    
	    // Is it destroyed
	    try {
		    NamingInterface n(nc.in());
		    NamingInterface::BindingList bnd_list;	    
		    n.list_context(bnd_list);
		    TEST_CHECK(bnd_list.size() == 1);
	    } catch (...) {
	        TEST_FAILED();
	    }	    
	}	
}

		
} // End namespace Naming
} // End namespace CommonSvcs
} // End namespace Cdmw

