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

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/common/Locations.hpp>
#include "testftrepository/TestFTRepository.hpp"
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
//#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platforminterface/ServiceNames.hpp"
#include <testftrepository/TesterFTRepository_impl.hpp>
#include <testftrepository/TesterFactory_impl.hpp>
#include "testftrepository/NamingDisplayer.hpp"
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>

#include <sstream>

namespace Cdmw 
{

namespace NamingAndRepository 
{

// constructor
TestFTRepository::TestFTRepository(CORBA::ORB_ptr                  orb,
                                   PortableServer::POA_ptr         rootPOA,
                                   const std::string&              name)
    : Testable(name),
      m_orb(CORBA::ORB::_duplicate(orb)),
      m_rootPOA(PortableServer::POA::_duplicate(rootPOA))
{    
    
}

// destructor
TestFTRepository::~TestFTRepository()
{
}

// do_tests
void TestFTRepository::do_tests()
{

    set_nbOfRequestedTestOK (34);

    std::cout << "Init Repository" << std::endl;
    
    CdmwNamingAndRepository::Repository_var repository =
    CdmwNamingAndRepository::Repository::_nil();
    
    
    CORBA::Object_var obj =
    PlatformMngt::PlatformInterface::Get_service(PlatformMngt::ServiceNames::NAMING_AND_REPOSITORY_SERVICE);
    repository = CdmwNamingAndRepository::Repository::_narrow(obj.in());
    
        
    CosNaming::NamingContextExt_var defaultRootCtx;
    CosNaming::NamingContextExt_var adminRootCtx;
    
    // ---------------
    TEST_INFO("Get the default root context");

    try
    {

        CosNaming::NamingContext_var temp_defaultRootCtx =
            repository->resolve_root_context(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);

        defaultRootCtx = CosNaming::NamingContextExt::_narrow(temp_defaultRootCtx.in());
        NamingDisplayer::list_context(defaultRootCtx.in());
        TEST_SUCCEED();
    
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }
    catch(...)
    {
        std::cout << "Cannot get Root Context" << std::endl;
        TEST_FAILED();
        return;
    }

    // ---------------
    TEST_INFO("Get the admin root context");

    try
    {

        CosNaming::NamingContext_var temp_adminRootCtx =
        repository->resolve_root_context(Common::Locations::ADMIN_ROOT_CONTEXT_ID);

        adminRootCtx = CosNaming::NamingContextExt::_narrow(temp_adminRootCtx.in());
        NamingDisplayer::list_context(adminRootCtx.in());    
        TEST_SUCCEED();
    
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }
    catch(...)
    {
        std::cout << "Cannot get Root Context" << std::endl;
        TEST_FAILED();
        return;
    }

    TEST_INFO("");
    TEST_INFO("use naming context");
    TEST_INFO("");
    
    Cdmw::Test::TesterFTRepository_impl* temp1 = new Cdmw::Test::TesterFTRepository_impl("O1");
    Cdmw::Test::TesterFTRepository_var tester1 = temp1->_this();
    TEST_SUCCEED();
    
            // ---------------
    TEST_INFO("Bind the object O1 as \"O1.object\" within the default root context");

    CosNaming::Name_var name = new CosNaming::Name;
    name->length(1);
    CosNaming::NameComponent_var nc = new CosNaming::NameComponent;
    nc->id = CORBA::string_dup("O1");
    nc->kind = CORBA::string_dup("object");
    name->operator[](0) = nc.in();

    try
    {
        defaultRootCtx->bind(name.in(), tester1.in());
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    TEST_INFO("Bind the object O2 as \"O2.object\" within the admin root context");

    Cdmw::Test::TesterFTRepository_impl* temp2 = new Cdmw::Test::TesterFTRepository_impl("O2");
    Cdmw::Test::TesterFTRepository_var tester2 = temp2->_this();

    name->length(1);
    nc->id = CORBA::string_dup("O2");
    nc->kind = CORBA::string_dup("object");
    name->operator[](0) = nc.in();

    try
    {
        adminRootCtx->bind(name.in(), tester2.in());
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }
    // ---------------
    TEST_INFO("Bind a new context \"CTX1.context\" within the default root context");

    name->length(1);
    nc->id = CORBA::string_dup("CTX1");
    nc->kind = CORBA::string_dup("context");
    name->operator[](0) = nc.in();
    
    CosNaming::NamingContext_var ctx1;

    try
    {
        ctx1 = defaultRootCtx->bind_new_context(name.in());
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Bind a new context \"CTX2.context\" within the admin root context");

    name->length(1);
    nc->id = CORBA::string_dup("CTX2");
    nc->kind = CORBA::string_dup("context");
    name->operator[](0) = nc.in();
    
    CosNaming::NamingContext_var ctx2;

    try
    {
        ctx2 = adminRootCtx->bind_new_context(name.in());
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    TEST_INFO("Bind the object O3 as \"O3.object\" within the  CTX1 context of the default root context");

    Cdmw::Test::TesterFTRepository_impl* temp3 = new Cdmw::Test::TesterFTRepository_impl("O3");
    Cdmw::Test::TesterFTRepository_var tester3 = temp3->_this();

    name->length(1);
    nc->id = CORBA::string_dup("O3");
    nc->kind = CORBA::string_dup("object");
    name->operator[](0) = nc.in();

    try
    {
        ctx1->bind(name.in(), tester3.in());
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    TEST_INFO("Bind the object O4 as \"O4.object\" within the  CTX2 context of the admin root context");

    Cdmw::Test::TesterFTRepository_impl* temp4 = new Cdmw::Test::TesterFTRepository_impl("O4");
    Cdmw::Test::TesterFTRepository_var tester4 = temp4->_this();

    name->length(1);
    nc->id = CORBA::string_dup("O4");
    nc->kind = CORBA::string_dup("object");
    name->operator[](0) = nc.in();

    try
    {
        ctx2->bind(name.in(), tester4.in());
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }


    TEST_INFO("");
    TEST_INFO("use name domain");
    TEST_INFO("");
    
    Cdmw::Test::TesterFTRepository_impl* temp5 = new Cdmw::Test::TesterFTRepository_impl("O5");
    Cdmw::Test::TesterFTRepository_var tester5 = temp5->_this();
    TEST_SUCCEED();
    
            // ---------------
    TEST_INFO("register the object O5 as \"\\default_dom1/O5\" within the default root context");
    try
    {
        CdmwNamingAndRepository::NameDomain_var Domain1 =
        repository->resolve_name_domain("default_dom1");        
        CdmwNamingAndRepository::NameDomain::RegistrationId_var regId = 
        Domain1->new_name ("O5");
                                    
        // register the object
        Domain1->register_object (regId.in(), tester5.in());

        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    Cdmw::Test::TesterFactory_impl* tempFac5 = new Cdmw::Test::TesterFactory_impl("F5");
    Cdmw::Test::TesterFactory_var testerFac5 = tempFac5->_this();


    TEST_INFO("Register new factory finder in the \"\\default_dom1/default_dom2/default_dom3\" domain");
    try 
    {
        CdmwNamingAndRepository::NameDomain_var Domain3 =
        repository->resolve_name_domain("default_dom1/default_dom2/default_dom3");  
        Domain3->register_new_factory("F5", testerFac5.in());
        
        
        
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }



    NamingDisplayer::list_context(defaultRootCtx.in());
    NamingDisplayer::list_context(adminRootCtx.in());

    
    // ---------------
    TEST_INFO("Get the factory finder");

    CdmwLifeCycle::FactoryFinder_var facFinder;
    
    try
    {
        CORBA::Object_var temp_facFinder = defaultRootCtx->resolve_str(CdmwNamingAndRepository::FACTORY_FINDER);

        std::cerr << "********** " << m_orb->object_to_string(temp_facFinder.in()) << std::endl;

        facFinder = CdmwLifeCycle::FactoryFinder::_narrow(temp_facFinder.in());
		
        TEST_SUCCEED();
		
	}
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
        return;
    }
	catch(...)
    {
        std::cout << "Cannot get the factory finder" << std::endl;
        TEST_FAILED();
        return;
    }




    TEST_INFO("")
    TEST_INFO("Test failed operation on the object group");
    TEST_INFO("");
    


    TEST_INFO("Bind the object 6 as \"O3.object\" within the  CTX1 context of the default root context");

    Cdmw::Test::TesterFTRepository_impl* temp6 = new Cdmw::Test::TesterFTRepository_impl("O6");
    Cdmw::Test::TesterFTRepository_var tester6 = temp6->_this();

    name->length(1);
    nc->id = CORBA::string_dup("O3");
    nc->kind = CORBA::string_dup("object");
    name->operator[](0) = nc.in();

    try
    {
        ctx1->bind(name.in(), tester6.in());
        TEST_FAILED();
    }
    catch(CosNaming::NamingContext::AlreadyBound &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    TEST_INFO("Bind the object O7 as \"O4.object\" within the  CTX2 context of the admin root context");

    Cdmw::Test::TesterFTRepository_impl* temp7 = new Cdmw::Test::TesterFTRepository_impl("O7");
    Cdmw::Test::TesterFTRepository_var tester7 = temp7->_this();

    name->length(1);
    nc->id = CORBA::string_dup("O4");
    nc->kind = CORBA::string_dup("object");
    name->operator[](0) = nc.in();

    try
    {
        ctx2->bind(name.in(), tester7.in());
        TEST_FAILED();
    }
    catch(CosNaming::NamingContext::AlreadyBound &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }
    
    TEST_INFO("Try to find an unknown root_context");
    try
    {
        repository->resolve_root_context("UNKNOWN_ROOT_CONTEXT");
        TEST_FAILED();
    }
    catch(CdmwNamingAndRepository::Repository::NoRootContext &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Find \"default_dom1/default_dom2/default_dom3/F1\" factory");
    
    try
    {
        CdmwLifeCycle::FactoryBase_var fac1_temp = facFinder->find_factory("default_dom1/default_dom2/default_dom3/F1");
        TEST_FAILED();
	}
    catch(CosLifeCycle::NoFactory &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }



    TEST_INFO("");
    TEST_INFO("Recover information from the primary Repository");
    TEST_INFO("");



    // ---------------
    TEST_INFO("Resolve \"O1.object\" from the root context");

    name->length(1);
    nc->id = CORBA::string_dup("O1");
    nc->kind = CORBA::string_dup("object");
    name->operator[](0) = nc.in();

    try
    {
        obj = defaultRootCtx->resolve(name.in());

        Cdmw::Test::TesterFTRepository_var res_obj1 = Cdmw::Test::TesterFTRepository::_narrow(obj.in());

        std::string res = res_obj1->name();
        if (strcmp(res.c_str(), "O1") == 0)
            TEST_SUCCEED();
        else
            TEST_FAILED();

    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Resolve \"O2.object\" from the root context");

    name->length(1);
    nc->id = CORBA::string_dup("O2");
    nc->kind = CORBA::string_dup("object");
    name->operator[](0) = nc.in();

    try
    {
        obj = adminRootCtx->resolve(name.in());

        Cdmw::Test::TesterFTRepository_var res_obj1 = Cdmw::Test::TesterFTRepository::_narrow(obj.in());

        std::string res = res_obj1->name();
        if (strcmp(res.c_str(), "O2") == 0)
            TEST_SUCCEED();
        else
            TEST_FAILED();

    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Resolve \"O3.object\" from the root context");

    name->length(2);
    nc->id = CORBA::string_dup("CTX1");
    nc->kind = CORBA::string_dup("context");
    name->operator[](0) = nc.in();
    nc->id = CORBA::string_dup("O3");
    nc->kind = CORBA::string_dup("object");
    name->operator[](1) = nc.in();

    try
    {
        obj = defaultRootCtx->resolve(name.in());

        Cdmw::Test::TesterFTRepository_var res_obj1 = Cdmw::Test::TesterFTRepository::_narrow(obj.in());

        std::string res = res_obj1->name();
        if (strcmp(res.c_str(), "O3") == 0)
            TEST_SUCCEED();
        else
            TEST_FAILED();

    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Resolve \"O4.object\" from the root context");

    name->length(2);
    nc->id = CORBA::string_dup("CTX2");
    nc->kind = CORBA::string_dup("context");
    name->operator[](0) = nc.in();
    nc->id = CORBA::string_dup("O4");
    nc->kind = CORBA::string_dup("object");
    name->operator[](1) = nc.in();

    try
    {
        obj = adminRootCtx->resolve(name.in());

        Cdmw::Test::TesterFTRepository_var res_obj1 = Cdmw::Test::TesterFTRepository::_narrow(obj.in());

        std::string res = res_obj1->name();
        if (strcmp(res.c_str(), "O4") == 0)
            TEST_SUCCEED();
        else
            TEST_FAILED();

    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }


    // ---------------
    TEST_INFO("Find  \"default_dom1/O5\" object");
    try
    {
        // ======================================================  
        // create the naming interface for default root context
        // ======================================================                    
        Cdmw::CommonSvcs::Naming::NamingInterface ni_defRoot(defaultRootCtx.in());
	    
        obj = ni_defRoot.resolve("default_dom1/O5");
        Cdmw::Test::TesterFTRepository_var res_obj1 = Cdmw::Test::TesterFTRepository::_narrow(obj.in());
        
        std::string res = res_obj1->name();
        if (strcmp(res.c_str(), "O5") == 0)
            TEST_SUCCEED();
        else
            TEST_FAILED(); 
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }
    


    // ---------------
    TEST_INFO("Find \"default_dom1/default_dom2/default_dom3/F5\" factory");
    
    try
    {
        CdmwLifeCycle::FactoryBase_var fac1_temp = facFinder->find_factory("default_dom1/default_dom2/default_dom3/F5");
        Cdmw::Test::TesterFactory_var fac1 = Cdmw::Test::TesterFactory::_narrow(fac1_temp.in());

        std::string info = fac1->name();

        if (info.compare("F5") == 0)
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
    CosNaming::NamingContext_var new_context1;

    TEST_INFO("Create a new context");
    
    try
    {
        new_context1 = defaultRootCtx->new_context();
        TEST_SUCCEED();
	}
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    TEST_INFO("Bind to new context");
    
    name->length(1);
    nc->id = CORBA::string_dup("O6");
    nc->kind = CORBA::string_dup("object");
    name->operator[](0) = nc.in();

    try
    {
        new_context1->bind(name.in(), tester4.in());
        TEST_SUCCEED();
	}
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    TEST_INFO("resolve the \"\\default_dom1/default_dom2/default_dom3\" domain from the repository");
    try 
    {
        CdmwNamingAndRepository::NameDomain_var Domain3 =
        repository->resolve_name_domain("default_dom1/default_dom2/default_dom3");  
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }


    int timescale = Cdmw::TestUtils::Testable::get_timescale();
    OsSupport::OS::sleep(timescale*1000);

    TEST_INFO("");
    TEST_INFO("Kill the primary Repository");

    

    std::ostringstream proc_run;
    proc_run<< "--proc-stop CdmwServices  RepositoryHost1 "<<OsSupport::OS::get_hostname().c_str();
    OsSupport::OS::create_process( "platform_admin.sh" , proc_run.str());
    
    OsSupport::OS::sleep(timescale*5000);


    TEST_INFO("Recover information from the new primary Repository");
    TEST_INFO("");


    // ---------------
    TEST_INFO("Resolve \"O1.object\" from the root context");

    name->length(1);
    nc->id = CORBA::string_dup("O1");
    nc->kind = CORBA::string_dup("object");
    name->operator[](0) = nc.in();

    try
    {
        obj = defaultRootCtx->resolve(name.in());

        Cdmw::Test::TesterFTRepository_var res_obj1 = Cdmw::Test::TesterFTRepository::_narrow(obj.in());

        std::string res = res_obj1->name();
        if (strcmp(res.c_str(), "O1") == 0)
            TEST_SUCCEED();
        else
            TEST_FAILED();

    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Resolve \"O2.object\" from the root context");

    name->length(1);
    nc->id = CORBA::string_dup("O2");
    nc->kind = CORBA::string_dup("object");
    name->operator[](0) = nc.in();

    try
    {
        obj = adminRootCtx->resolve(name.in());

        Cdmw::Test::TesterFTRepository_var res_obj1 = Cdmw::Test::TesterFTRepository::_narrow(obj.in());

        std::string res = res_obj1->name();
        if (strcmp(res.c_str(), "O2") == 0)
            TEST_SUCCEED();
        else
            TEST_FAILED();

    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Resolve \"O3.object\" from the root context");

    name->length(2);
    nc->id = CORBA::string_dup("CTX1");
    nc->kind = CORBA::string_dup("context");
    name->operator[](0) = nc.in();
    nc->id = CORBA::string_dup("O3");
    nc->kind = CORBA::string_dup("object");
    name->operator[](1) = nc.in();

    try
    {
        obj = defaultRootCtx->resolve(name.in());

        Cdmw::Test::TesterFTRepository_var res_obj1 = Cdmw::Test::TesterFTRepository::_narrow(obj.in());

        std::string res = res_obj1->name();
        if (strcmp(res.c_str(), "O3") == 0)
            TEST_SUCCEED();
        else
            TEST_FAILED();

    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Resolve \"O4.object\" from the root context");

    name->length(2);
    nc->id = CORBA::string_dup("CTX2");
    nc->kind = CORBA::string_dup("context");
    name->operator[](0) = nc.in();
    nc->id = CORBA::string_dup("O4");
    nc->kind = CORBA::string_dup("object");
    name->operator[](1) = nc.in();

    try
    {
        obj = adminRootCtx->resolve(name.in());

        Cdmw::Test::TesterFTRepository_var res_obj1 = Cdmw::Test::TesterFTRepository::_narrow(obj.in());

        std::string res = res_obj1->name();
        if (strcmp(res.c_str(), "O4") == 0)
            TEST_SUCCEED();
        else
            TEST_FAILED();

    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }


    name->length(2);
    nc->id = CORBA::string_dup("default_dom1");
    nc->kind = CORBA::string_dup("context");
    name->operator[](0) = nc.in();
    nc->id = CORBA::string_dup("O5");
    nc->kind = CORBA::string_dup("object");
    name->operator[](1) = nc.in();
    // ---------------
    TEST_INFO("Find  \"default_dom1/O5\" object");
    try
    {
        // ======================================================  
        // create the naming interface for default root context
        // ======================================================                    
        Cdmw::CommonSvcs::Naming::NamingInterface ni_defRoot(defaultRootCtx.in());
	    
        obj = ni_defRoot.resolve("default_dom1/O5");
        Cdmw::Test::TesterFTRepository_var res_obj1 = Cdmw::Test::TesterFTRepository::_narrow(obj.in());
        
        std::string res = res_obj1->name();
        if (strcmp(res.c_str(), "O5") == 0)
            TEST_SUCCEED();
        else
            TEST_FAILED(); 
    }
    catch(CosNaming::NamingContext::CannotProceed &e)
    {
        std::cout << "CannotProceed:" << e.rest_of_name[0].id << std::endl;
        TEST_FAILED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }
    


    // ---------------
    TEST_INFO("Find \"default_dom1/default_dom2/default_dom3/F5\" factory");
    
    try
    {
        CdmwLifeCycle::FactoryBase_var fac1_temp;
try
{
    fac1_temp = facFinder->find_factory("default_dom1/default_dom2/default_dom3/F5");
}
catch(const CORBA::Exception& ex)
    {
        std::cout << ex << std::endl;
        throw;
    }


        Cdmw::Test::TesterFactory_var fac1 = Cdmw::Test::TesterFactory::_narrow(fac1_temp.in());

        std::string info = fac1->name();

        if (info.compare("F5") == 0)
        {
            TEST_SUCCEED();
        }

	}
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    TEST_INFO("resolve the \"\\default_dom1/default_dom2/default_dom3\" domain from the repository");
    try 
    {
        CdmwNamingAndRepository::NameDomain_var Domain3 =
        repository->resolve_name_domain("default_dom1/default_dom2/default_dom3");  
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    TEST_INFO("resolve the \"DefaultRootDomain\" domain from the repository");
    try 
    {
        CosNaming::NamingContext_var temp_defaultRootCtx;
        
        try
        {
            temp_defaultRootCtx = repository->resolve_root_context(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);   
        }
        catch(const CORBA::Exception& ex)
        {
            std::cout << "resolve_root_context threw " << ex << std::endl;
            throw;
        }

        try
        {
            name->length(2);
            nc->id = CORBA::string_dup("default_dom1");
            nc->kind = CORBA::string_dup("");
            name->operator[](0) = nc.in();
            nc->id = CORBA::string_dup("O5");
            nc->kind = CORBA::string_dup("");
            name->operator[](1) = nc.in();

            obj = temp_defaultRootCtx->resolve(name.in());
        }
        catch(const CORBA::Exception& ex)
        {
            std::cout << "resolve threw " << ex << std::endl;
            throw;
        }

        Cdmw::Test::TesterFTRepository_var res_obj1 = Cdmw::Test::TesterFTRepository::_narrow(obj.in());

        std::string res = res_obj1->name();
        if (strcmp(res.c_str(), "O5") == 0)
            TEST_SUCCEED();
        else
            TEST_FAILED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

}


} // end namespace NamingAndRepository

} // end namespace Cdmw

