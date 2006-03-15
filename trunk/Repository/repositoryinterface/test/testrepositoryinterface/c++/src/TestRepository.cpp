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


#include "testrepositoryinterface/TestRepository.hpp"
#include "testrepositoryinterface/TestMacroDefs.hpp"
#include <Repository/repositoryinterface/RepositoryInterface.hpp>

namespace
{
    const char* REPOSITORY_BINDING_NAME = "CDMW.I/repository.simulated";
    const char* DEFAULT_DOMAIN_NAME = "MyDefault.system/MyDefault.domain";
};
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


TestRepository::TestRepository(CosNaming::NamingContext_ptr nc)
    throw()
    :Testable("Cdmw::NamingAndRepository::RepositoryInterface"),
     m_context(CosNaming::NamingContext::_duplicate(nc))

{
    // does nothing
}


TestRepository::~TestRepository()
    throw()
{
    // does nothing
}


TestRepository::TestRepository(const TestRepository& rhs)
    throw()
    :Testable("Cdmw::NamingAndRepository::RepositoryInterface"),
    m_context(CosNaming::NamingContext::_duplicate(rhs.m_context.in()))
{
    // does nothing
}
		
TestRepository&
TestRepository::operator=(const TestRepository& rhs)
    throw()
{
    if (this != &rhs) {
        m_context = rhs.m_context;
    }
    return *this;
}

void TestRepository::do_tests()		
{
    // This tests RepositoryInterface class
    //-------------------------------------------------
    
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (8);
    
    
    // Testing string to name conversion
    //-------------------------------------------------
    TEST_INFO("Testing RepositoryInterface");
    
    // Get reference to the repository interface
    CdmwNamingAndRepository::Repository_var rep
        = CdmwNamingAndRepository::Repository::_nil();
    {
        try {
            Cdmw::CommonSvcs::Naming::NamingInterface ni(m_context.in());
            CORBA::Object_var obj = ni.resolve(REPOSITORY_BINDING_NAME);
            rep = CdmwNamingAndRepository::Repository::_narrow(obj.in());
	        if (CORBA::is_nil(rep.in())) {
	            NI_TEST_CERR("Couldn't retrieve reference of the Repository"
                          << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
                TEST_FAILED();
                return;
		    }
	    } catch (...) {
	        NI_TEST_CERR("Couldn't retrieve reference of the Repository"
                          << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")");
            TEST_FAILED();
            return;
	    }
	}
    // Init of the RepositoryInterface
    TEST_INFO("Init of the RepositoryInterface.");
    {
        using namespace Cdmw::NamingAndRepository;
	
	    try {
            RepositoryInterface::init(DEFAULT_DOMAIN_NAME,rep.in());
	        TEST_SUCCEED();
		} catch (const CdmwNamingAndRepository::NoNameDomain &) {
		    TEST_FAILED();
		    return;
		} catch (const CdmwNamingAndRepository::InvalidName & ) {
		    TEST_FAILED();
		    return;
	    } catch (const CORBA::SystemException & e) {
	        NI_TEST_CERR(e._name());
	        TEST_FAILED();
		    return;
	    }
	}

    
    //    MyDefault.system/MyDefault.domain/dom1.1/dom2.2
    //    MyDefault.system/MyDefault.domain/dom2.1
    //    Another.system/Another.domain/domX.Y
    TEST_INFO("Test get_domain_naming_interface()");
    {
        using namespace Cdmw::NamingAndRepository;
	
        try {
            Cdmw::CommonSvcs::Naming::NamingInterface ni
	            = RepositoryInterface::get_domain_naming_interface();
            std::string input("dom1.1/dom2.2");
            CORBA::Object_var obj = ni.resolve(input);
            TEST_CHECK(!CORBA::is_nil(obj.in()));
            input = "MyDefault.system/MyDefault.domain";
            ni = RepositoryInterface::get_domain_naming_interface(input);		    
            input = "dom2.1";
            obj = ni.resolve(input);
            TEST_CHECK(!CORBA::is_nil(obj.in()));
            input = "Another.system/Another.domain";
            ni = RepositoryInterface::get_domain_naming_interface(input);		    
            input = "domX.Y";
            obj = ni.resolve(input);
            TEST_CHECK(!CORBA::is_nil(obj.in()));
        } catch (...) {
            TEST_FAILED();
            return;
        }
    }
    
    TEST_INFO("Test get_domain()");
    {
        using namespace Cdmw::NamingAndRepository;
	
        try {
            CosNaming::NamingContext_var nc
                = RepositoryInterface::get_domain_context();
            Cdmw::CommonSvcs::Naming::NamingInterface ni(nc.in());
            std::string input("dummy");
            CdmwNamingAndRepository::NameDomain_var dom;
            {
                std::string obj_name("dom1.1/dom2.2");
                CORBA::Object_var obj = ni.resolve(obj_name);
                dom = RepositoryInterface::get_domain();
                dom->register_new_object(input.c_str(),obj.in());
            }
            CORBA::Object_var obj = ni.resolve(input);                         
            TEST_CHECK(!CORBA::is_nil(obj.in()));
            dom->release_name(input.c_str());
        } catch (...) {
            TEST_FAILED();
            return;
        }
    }
        
    TEST_INFO("Test get_domain_context()");
    {
        using namespace Cdmw::NamingAndRepository;
	
        try {
            CosNaming::NamingContext_var nc
                = RepositoryInterface::get_domain_context();
            Cdmw::CommonSvcs::Naming::NamingInterface ni(nc.in());
            std::string input("dom1.1/dom2.2");
            CORBA::Object_var obj = ni.resolve(input);
            TEST_CHECK(!CORBA::is_nil(obj.in()));
            input = "MyDefault.system/MyDefault.domain";
            nc = RepositoryInterface::get_domain_context(input);
            ni = Cdmw::CommonSvcs::Naming::NamingInterface(nc.in());
            input = "dom2.1";
            obj = ni.resolve(input);
            TEST_CHECK(!CORBA::is_nil(obj.in()));
            input = "Another.system/Another.domain";		    
            nc = RepositoryInterface::get_domain_context(input);
            ni = Cdmw::CommonSvcs::Naming::NamingInterface(nc.in());
            input = "domX.Y";
            obj = ni.resolve(input);
            TEST_CHECK(!CORBA::is_nil(obj.in()));
        } catch (...) {
            TEST_FAILED();
            return;
        }
    }

    RepositoryInterface::finish();
    
}
		
}; // End namespace NamingAndRepository
}; // End namespace Cdmw

