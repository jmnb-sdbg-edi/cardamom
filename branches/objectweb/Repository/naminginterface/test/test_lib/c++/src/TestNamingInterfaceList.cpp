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


#include "test_lib/TestNamingInterfaceList.hpp"
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


TestNamingInterfaceList::TestNamingInterfaceList (CosNaming::NamingContext_ptr nc,
						  TestObject_ptr               obj)
    throw()
    :Testable("Cdmw::NamingAndRepository::NamingInterface::list"),
     m_context(CosNaming::NamingContext::_duplicate(nc)),
     m_test_object(TestObject::_duplicate(obj))
{
    // does nothing
}


TestNamingInterfaceList::~TestNamingInterfaceList()
    throw()
{
    // does nothing
}


TestNamingInterfaceList::TestNamingInterfaceList(const TestNamingInterfaceList& rhs)
    throw()
    :Testable("Cdmw::NamingAndRepository::NamingInterface::list"),
     m_context(CosNaming::NamingContext::_duplicate(rhs.m_context.in())),
     m_test_object(TestObject::_duplicate(rhs.m_test_object.in()))
{
    // does nothing
}
		
TestNamingInterfaceList&
TestNamingInterfaceList::operator=(const TestNamingInterfaceList& rhs)
    throw()
{
    // does nothing
    return *this;
}


void TestNamingInterfaceList::do_tests()		
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (4);
    
    NamingInterface ni0(m_context.in());
    
    // This tests NamingInterface class
    //-------------------------------------------------
    // Testing list operations
    //-------------------------------------------------
    TEST_INFO("Testing NamingInterface::list operations");
    using namespace CosNaming;
    	
	// Install test name tree
	const unsigned long NB_BINDINGS = 10;
    const CORBA::ULong CHUNK_SIZE = 3;
    try {
        // test.list/ctxt.1
	    // test.list/ctxt.2
	    // test.list/ctxt.3
	    // test.list/ctxt.4
	    // test.list/ctxt.5
	    // test.list/obj.1
	    // test.list/obj.2
	    // test.list/obj.3
	    // test.list/obj.4
	    // test.list/obj.5
	    ni0.bind_new_context("test.list/ctxt.1",true);
	    ni0.bind_new_context("test.list/ctxt.2",true);
	    ni0.bind_new_context("test.list/ctxt.3",true);
	    ni0.bind_new_context("test.list/ctxt.4",true);
	    ni0.bind_new_context("test.list/ctxt.5",true);
	    ni0.bind("test.list/obj.1",m_test_object.in(),true);
	    ni0.bind("test.list/obj.2",m_test_object.in(),true);
	    ni0.bind("test.list/obj.3",m_test_object.in(),true);
	    ni0.bind("test.list/obj.4",m_test_object.in(),true);
	    ni0.bind("test.list/obj.5",m_test_object.in(),true);
    } catch (...) {
	    NI_TEST_CERR("Unexpected error (could'nt install initial bindings)"
		          << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
	    TEST_FAILED();
	    return;
	}
	
    std::string input("test.list");
    NamingContext_var nc;
    try {
        CORBA::Object_var obj = ni0.resolve(input);
	    nc = NamingContext::_narrow(obj.in());
	    TEST_CHECK(!CORBA::is_nil(nc.in()));
	} catch(...) {
	    NI_TEST_CERR("Unexpected error (" << input << " not present!)" 
		          << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
	    TEST_FAILED();
	    return;
	}
	
	NamingInterface ni(nc.in());

    //
    // Test list operation
    //
    TEST_INFO("Test list operation");
    try {
		BindingIterator_var it;                  // Iterator reference
        BindingList_var     bl;                  // Binding list
	    
	    unsigned long nb = 0;
	    
	    ni.list(CHUNK_SIZE,bl,it);              // Get first chunk
        nb += bl->length();

        if (!CORBA::is_nil(it.in())) {                       // More bindings?
            CORBA::Boolean more;
            do {
                more = it->next_n(CHUNK_SIZE,bl);       // Get next chunk
                nb += bl->length();
            } while (more);                             // While not done
	        it->destroy();                              // Clean up iterator
		}
		TEST_CHECK(nb == NB_BINDINGS);
    } catch (const CORBA::SystemException &) {
        TEST_FAILED();
    }
    //
    // Test list_context operation
    //
    TEST_INFO("Test list_context operation");
    try {
	    NamingInterface::BindingList bnd_list;	    
	    
	    ni.list_context(bnd_list,CHUNK_SIZE);
	    TEST_CHECK(bnd_list.size() == NB_BINDINGS);
	} catch (const CORBA::SystemException &) {
        TEST_FAILED();
    }
    // check zero value for how_many parameter
    try {
	    NamingInterface::BindingList bnd_list;	    
	    
	    ni.list_context(bnd_list,0L);
	    TEST_CHECK(bnd_list.size() == NB_BINDINGS);
	} catch (const CORBA::SystemException &) {
        TEST_FAILED();
    }
}

		
} // End namespace NamingAndRepository
} // End namespace Cdmw

