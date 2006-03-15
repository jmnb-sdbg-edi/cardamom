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


#include "test1/TestNameDomain.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"

#include "test1/UserFactoryFinder_impl.hpp"
#include "test1/UserNameDomain_impl.hpp"

#include "test1/NamingDisplayer.hpp"

#include <iostream>


using namespace std;


TestNameDomain::TestNameDomain(const std::string& name,
     CORBA::ORB_ptr orb,
     CdmwNamingAndRepository::Repository_ptr repository)
 : Testable(name)
{
    m_ORB = CORBA::ORB::_duplicate(orb);
    m_repository = CdmwNamingAndRepository::Repository::_duplicate(repository);

    CORBA::Object_var obj = m_ORB->resolve_initial_references("RootPOA");
    m_rootPOA = PortableServer::POA::_narrow(obj.in());

    CORBA::Object_var tempOrbRootCtx = m_ORB->resolve_initial_references("NameService");
    m_orbRootCtx = CosNaming::NamingContext::_narrow(tempOrbRootCtx.in());
}


TestNameDomain::~TestNameDomain()
{

}


Tester_ptr TestNameDomain::create_Tester(const std::string& name)
{

    Tester_impl *servant = new Tester_impl(name);
    PortableServer::ObjectId_var oid = m_rootPOA->activate_object(servant);
    return servant->_this();

}


TesterFactory_ptr TestNameDomain::create_TesterFactory(const std::string& name)
{

    TesterFactory_impl *servant = new TesterFactory_impl(name);
    PortableServer::ObjectId_var oid = m_rootPOA->activate_object(servant);
    return servant->_this();

}


ObjectFactory_ptr TestNameDomain::create_ObjectFactory(const std::string& name)
{

    ObjectFactory_impl *servant = new ObjectFactory_impl(name);
    PortableServer::ObjectId_var oid = m_rootPOA->activate_object(servant);
    return servant->_this();

}


void TestNameDomain::display_factories(const CosLifeCycle::Factories& factories)
{

    for (CORBA::ULong i = 0; i < factories.length(); ++i)
    {
        ObjectFactory_var objFac = ObjectFactory::_narrow(factories[i]);
        if (!CORBA::is_nil(objFac.in()))
        {
            std::cout << "Factory[" << i << "] : " << objFac->info() << endl;
        }
        else
        {
            TesterFactory_var testerFac = TesterFactory::_narrow(factories[i]);
            std::cout << "Factory[" << i << "] : " << testerFac->info() << endl;
        }

    }

}

void TestNameDomain::do_tests()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (33);

    //CORBA::Object_ptr obj;

    CosNaming::Name_var name = new CosNaming::Name;
    CosNaming::NameComponent_var nc = new CosNaming::NameComponent;
    
    CosNaming::NamingContextExt_var rootCtx;

    // ---------------
    TEST_INFO("Display the default root context");

    try
    {

        CosNaming::NamingContext_var temp_rootCtx =
            m_repository->resolve_root_context(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);

        rootCtx = CosNaming::NamingContextExt::_narrow(temp_rootCtx.in());
    
        NamingDisplayer::list_context(rootCtx.in());
        
        TEST_SUCCEED();
    
    }
    catch(CdmwNamingAndRepository::Repository::NoRootContext & ex) {
        cout << "Cannot get Root Context" << ex._name() << endl;
        TEST_FAILED();
        return;	
    } 
    catch(CORBA::SystemException & ex) {
        cout << "Failed Narrowing " << ex._name() << endl;
        TEST_FAILED();
        return;	
    } 
    catch (...) {
        cout << "Unknown exception - Cannot get Root Context" << endl;
        TEST_FAILED();
        return;
    }


    // ---------------
    TEST_INFO("Get the \"dom1\" name domain");
    CdmwNamingAndRepository::NameDomain_var dom1;
    
    try
    {
        dom1 = m_repository->resolve_name_domain("dom1");
		
        TEST_SUCCEED();
		
	}
	catch(...)
    {
        cout << "Cannot get the \"dom1\" name domain" << endl;
        TEST_FAILED();
        return;
    }


    // ---------------
    TEST_INFO("Create the object O1");
    Tester_var obj1 = create_Tester("O1");

    // ---------------
    TEST_INFO("Try to reserve the name \"X/Y\" within \"dom1\"");
    
    try
    {
        CdmwNamingAndRepository::NameDomain::RegistrationId_var regId = dom1->new_name("X/Y");
        TEST_FAILED();
	}
    catch(const CdmwNamingAndRepository::InvalidName &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }

    // ---------------
    TEST_INFO("Reserve the name \"X\" within \"dom1\"");
    
    try
    {
        CdmwNamingAndRepository::NameDomain::RegistrationId_var regId = dom1->new_name("X");
        TEST_SUCCEED();
	}
    catch(const CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }
    
    
    // ---------------
    TEST_INFO("Try to register the object O1 as \"X\" directly within \"dom1\"");
    
    try
    {
        dom1->register_new_object("X", obj1.in());
        TEST_FAILED();
	}
    catch(const CdmwNamingAndRepository::NameDomain::AlreadyExists &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }


    // ---------------
    TEST_INFO("Register the object O1 as \"dom1_O1\" directly within \"dom1\"");
    
    try
    {
        dom1->register_new_object("dom1_O1", obj1.in());
	
	    NamingDisplayer::list_context(rootCtx.in());
		
        TEST_SUCCEED();
		
	}
    catch(const CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }
    
    // ---------------
    TEST_INFO("Try to reserve the name \"dom1_O1\" within \"dom1\"");
    
    try
    {
        CdmwNamingAndRepository::NameDomain::RegistrationId_var regId = dom1->new_name("dom1_O1");
        TEST_FAILED();
	}
    catch(const CdmwNamingAndRepository::NameDomain::AlreadyExists &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }

	    
    // ---------------
    TEST_INFO("Reserve the name \"dom1_O2\" directly within \"dom1\" and register the object O1");
    
    try
    {
        CdmwNamingAndRepository::NameDomain::RegistrationId_var regId = dom1->new_name("dom1_O2");
        dom1->register_object(regId.in(), obj1.in());
	
        NamingDisplayer::list_context(rootCtx.in());
	
        TEST_SUCCEED();
	}
    catch(const CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Create the factory F1");

    TesterFactory_var fac1 = create_TesterFactory("F1");

    // ---------------
    TEST_INFO("Register the factory F1 as \"dom1_F1\" directly within \"dom1\"");
    
    try
    {
        dom1->register_new_factory("dom1_F1", fac1.in());
	
	    NamingDisplayer::list_context(rootCtx.in());
		
        TEST_SUCCEED();
		
	}
    catch(const CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }
    
    // ---------------
    TEST_INFO("Create the factory F2");

    TesterFactory_var fac2 = create_TesterFactory("F2");
    
    // ---------------
    TEST_INFO("Reserve the name \"dom1_F2\" directly within \"dom1\" and register the factory F2");
    
    try
    {
        CdmwNamingAndRepository::NameDomain::RegistrationId_var regId = dom1->new_name("dom1_F2");
        dom1->register_factory(regId.in(), fac2.in());
	
        NamingDisplayer::list_context(rootCtx.in());
	
        TEST_SUCCEED();
	}
    catch(const CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }
    
    // ---------------
    TEST_INFO("Create the factory F3");

    TesterFactory_var fac3 = create_TesterFactory("F3");
    
    // ---------------
    TEST_INFO("Register the factory F3 as \"dom2_F3\" within \"dom1/dom2\"");
    
    try
    {
        CdmwNamingAndRepository::NameDomain_var dom2 =
            m_repository->resolve_name_domain("dom1/dom2");

        dom2->register_new_factory("dom2_F3", fac3.in());
	
        NamingDisplayer::list_context(rootCtx.in());
	
        TEST_SUCCEED();
	}
    catch(const CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }
    
    // ---------------
    TEST_INFO("Create the factory F4_1");

    ObjectFactory_var fac4_1 = create_ObjectFactory("F4_1");

    // ---------------
    TEST_INFO("Register the factory F4_1 as \"F4\" directly within \"dom1\"");
    
    try
    {
        dom1->register_new_factory("F4", fac4_1.in());
	
	    NamingDisplayer::list_context(rootCtx.in());
		
        TEST_SUCCEED();
		
	}
    catch(const CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Create the factory F4_2");

    ObjectFactory_var fac4_2 = create_ObjectFactory("F4_2");

    // ---------------
    TEST_INFO("Register the factory F4_2 as \"F4\" within \"dom1/dom2\"");
    
    try
    {
        CdmwNamingAndRepository::NameDomain_var dom2 =
            m_repository->resolve_name_domain("dom1/dom2");

        dom2->register_new_factory("F4", fac4_2.in());
	
        NamingDisplayer::list_context(rootCtx.in());
	
        TEST_SUCCEED();
	}
    catch(const CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }


    // ---------------
    TEST_INFO("Create the factory F4_3");

    ObjectFactory_var fac4_3 = create_ObjectFactory("F4_3");

    // ---------------
    TEST_INFO("Register the factory F4_3 as \"F4\" within \"dom1/dom2/dom3\"");
    
    try
    {
        CdmwNamingAndRepository::NameDomain_var dom3 =
            m_repository->resolve_name_domain("dom1/dom2/dom3");

        dom3->register_new_factory("F4", fac4_3.in());
	
        NamingDisplayer::list_context(rootCtx.in());
	
        TEST_SUCCEED();
	}
    catch(const CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // NAME DOMAIN SECURITY

    // ---------------
    TEST_INFO("Try to unbind the \"dom1\" domain from the root context");

    name->length(1);
    nc->id = CORBA::string_dup("dom1");
    nc->kind = CORBA::string_dup("");
    name->operator[](0) = nc.in();

    try
    {
        rootCtx->unbind(name.in());
        TEST_FAILED();
    }
    catch(const CORBA::NO_PERMISSION &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }

    // ---------------
    TEST_INFO("Try to rebind the \"dom1\" domain from the root context");

    name->length(1);
    nc->id = CORBA::string_dup("dom1");
    nc->kind = CORBA::string_dup("");
    name->operator[](0) = nc.in();

    try
    {
        rootCtx->rebind_context(name.in(), rootCtx.in());
        TEST_FAILED();
    }
    catch(const CORBA::NO_PERMISSION &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }


    // ---------------
    TEST_INFO("Try to bind the object O1 as \"dom1/X\" within the root context");

    name->length(2);
    nc->id = CORBA::string_dup("dom1");
    nc->kind = CORBA::string_dup("");
    name->operator[](0) = nc.in();
    nc->id = CORBA::string_dup("X");
    nc->kind = CORBA::string_dup("");
    name->operator[](1) = nc.in();

    try
    {
        rootCtx->bind(name.in(), obj1.in());
        TEST_FAILED();
    }
    catch(const CORBA::NO_PERMISSION &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }


    // USER NAME DOMAIN
    // ---------------

    CosNaming::NamingContextExt_var orbRootCtx = CosNaming::NamingContextExt::_narrow(m_orbRootCtx.in());

    UserNameDomain_impl *userND_i = new UserNameDomain_impl(orbRootCtx.in());
    PortableServer::ObjectId_var userND_oid = m_rootPOA->activate_object(userND_i);
    CdmwNamingAndRepository::NameDomain_var userND = userND_i->_this();

    UserFactoryFinder_impl *userFF_i = new UserFactoryFinder_impl(orbRootCtx.in());
    PortableServer::ObjectId_var userFF_oid = m_rootPOA->activate_object(userFF_i);
    CdmwLifeCycle::FactoryFinder_var userFF = userFF_i->_this();

    CdmwNamingAndRepository::NameDomain::NameDomainDescription ndDesc;
    ndDesc.the_name_domain = userND;
    ndDesc.the_factory_finder = userFF;
    ndDesc.the_context = m_orbRootCtx;

    // ---------------
    TEST_INFO("Register the user name domain \"user_dom\" within \"dom1/dom2\"");
    
    try
    {
        CdmwNamingAndRepository::NameDomain_var dom2 =
            m_repository->resolve_name_domain("dom1/dom2");

        dom2->register_new_name_domain("user_dom", ndDesc);
	
        NamingDisplayer::list_context(rootCtx.in());
	
        TEST_SUCCEED();
	}
    catch(const CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Create the object user_dom_O1");

    Tester_var user_dom_O1 = create_Tester("user_dom_O1");

    // ---------------
    TEST_INFO("Register the object O1 as \"user_dom_O1\" directly within \"dom1/dom2/user_dom\"");
    
    try
    {
        CdmwNamingAndRepository::NameDomain_var userDom =
            m_repository->resolve_name_domain("dom1/dom2/user_dom");

        userDom->register_new_object("user_dom_O1", user_dom_O1.in());
	
	    NamingDisplayer::list_context(rootCtx.in());
		
        TEST_SUCCEED();
		
	}
    catch(const CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Create the factory user_dom_F1");

    TesterFactory_var user_dom_F1 = create_TesterFactory("user_dom_F1");

    // ---------------
    TEST_INFO("Register the factory F1 as \"user_dom_F1\" directly within \"dom1/dom2/user_dom\"");
    
    try
    {
        CdmwNamingAndRepository::NameDomain_var userDom =
            m_repository->resolve_name_domain("dom1/dom2/user_dom");

        userDom->register_new_factory("user_dom_F1", user_dom_F1.in());
	
	    NamingDisplayer::list_context(rootCtx.in());
		
        TEST_SUCCEED();
		
	}
    catch(const CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // FACTORY FINDER

    // ---------------
    TEST_INFO("Get the factory finder");

    CdmwLifeCycle::FactoryFinder_var facFinder;
    
    try
    {
        CORBA::Object_var temp_facFinder = rootCtx->resolve_str(CdmwNamingAndRepository::FACTORY_FINDER);
        facFinder = CdmwLifeCycle::FactoryFinder::_narrow(temp_facFinder.in());
		
        TEST_SUCCEED();
		
	}
	catch(...)
    {
        cout << "Cannot get the \"dom1\" name domain" << endl;
        TEST_FAILED();
        return;
    }

    // ---------------
    TEST_INFO("Find \"dom1/dom1_F1\" factory");

    try
    {
        CdmwLifeCycle::FactoryBase_var fac1_temp = facFinder->find_factory("dom1/dom1_F1");
        TesterFactory_var fac1 = TesterFactory::_narrow(fac1_temp.in());

        string info = fac1->info();

        if (info.compare("F1") == 0)
        {
            TEST_SUCCEED();
        }

	}
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Find factories supporting Tester within \"dom1\"");

    name->length(2);
    nc->id = CORBA::string_dup("dom1");
    nc->kind = CORBA::string_dup("");
    name->operator[](0) = nc.in();
    nc->id = CORBA::string_dup("IDL:Tester:1.0");
    nc->kind = CORBA::string_dup("object interface");
    name->operator[](1) = nc.in();

    try
    {
        CosLifeCycle::Factories_var factories = facFinder->find_factories(name.in());

        display_factories(factories.in());

        TEST_SUCCEED();

	}
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }


    // ---------------
    TEST_INFO("Find factories supporting Object within \"dom1\"");

    name->length(2);
    nc->id = CORBA::string_dup("dom1");
    nc->kind = CORBA::string_dup("");
    name->operator[](0) = nc.in();
    nc->id = CORBA::string_dup("IDL:Object:1.0");
    nc->kind = CORBA::string_dup("object interface");
    name->operator[](1) = nc.in();

    try
    {
        CosLifeCycle::Factories_var factories = facFinder->find_factories(name.in());

        display_factories(factories.in());

        TEST_SUCCEED();

	}
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Find factories having the name \"dom1_F1\" within \"dom1\"");

    name->length(2);
    nc->id = CORBA::string_dup("dom1");
    nc->kind = CORBA::string_dup("");
    name->operator[](0) = nc.in();
    nc->id = CORBA::string_dup("dom1_F1");
    nc->kind = CORBA::string_dup("factory name");
    name->operator[](1) = nc.in();

    try
    {
        CosLifeCycle::Factories_var factories = facFinder->find_factories(name.in());

        display_factories(factories.in());

        TEST_SUCCEED();

	}
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }


    // ---------------
    TEST_INFO("Find factories having the name \"F4\" within \"dom1\"");

    name->length(2);
    nc->id = CORBA::string_dup("dom1");
    nc->kind = CORBA::string_dup("");
    name->operator[](0) = nc.in();
    nc->id = CORBA::string_dup("F4");
    nc->kind = CORBA::string_dup("factory name");
    name->operator[](1) = nc.in();

    try
    {
        CosLifeCycle::Factories_var factories = facFinder->find_factories(name.in());

        display_factories(factories.in());

        TEST_SUCCEED();

	}
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Find all factories within \"dom1\"");

    name->length(1);
    nc->id = CORBA::string_dup("dom1");
    nc->kind = CORBA::string_dup("");
    name->operator[](0) = nc.in();

    try
    {
        CosLifeCycle::Factories_var factories = facFinder->find_factories(name.in());

        display_factories(factories.in());

        TEST_SUCCEED();

	}
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Find all factories within \"dom1/dom2\"");

    name->length(2);
    nc->id = CORBA::string_dup("dom1");
    nc->kind = CORBA::string_dup("");
    name->operator[](0) = nc.in();
    nc->id = CORBA::string_dup("dom2");
    nc->kind = CORBA::string_dup("");
    name->operator[](1) = nc.in();

    try
    {
        CosLifeCycle::Factories_var factories = facFinder->find_factories(name.in());

        display_factories(factories.in());

        TEST_SUCCEED();

	}
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Try to pass invalid parameters to the factory finder");

    name->length(3);
    nc->id = CORBA::string_dup("dom1");
    nc->kind = CORBA::string_dup("");
    name->operator[](0) = nc.in();
    nc->id = CORBA::string_dup("XX");
    nc->kind = CORBA::string_dup("factory name");
    name->operator[](1) = nc.in();
    nc->id = CORBA::string_dup("dom2");
    nc->kind = CORBA::string_dup("");
    name->operator[](2) = nc.in();

    try
    {
        CosLifeCycle::Factories_var factories = facFinder->find_factories(name.in());

        TEST_FAILED();
    }
    catch(const CORBA::BAD_PARAM &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }


    // UNREGISTRATION

    // ---------------
    TEST_INFO("Unregister the \"dom1_O2\" object from \"dom1\"");

    try
    {
        dom1->release_name("dom1_O2");
        TEST_SUCCEED();

	}
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Unregister the \"dom1_F2\" factory from \"dom1\"");

    try
    {
        dom1->release_name("dom1_F2");
        TEST_SUCCEED();

	}
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    NamingDisplayer::list_context(rootCtx.in());

    // ---------------
    TEST_INFO("Unregister the \"user_dom\" name domain from \"dom1/dom2\"");

    try
    {
        CdmwNamingAndRepository::NameDomain_var dom2 =
            m_repository->resolve_name_domain("dom1/dom2");

        dom2->release_name("user_dom");
        TEST_SUCCEED();
    }
    catch(const CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    NamingDisplayer::list_context(rootCtx.in());

    // ---------------
    TEST_INFO("Try to unregister the \"dom2\" name domain from \"dom1\"");

    try
    {
        dom1->release_name("dom2");
        TEST_FAILED();
    }
    catch(const CORBA::NO_PERMISSION &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }


}



