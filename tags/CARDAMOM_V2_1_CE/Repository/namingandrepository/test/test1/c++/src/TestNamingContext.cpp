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


#include "test1/TestNamingContext.hpp"
#include "test1/Tester_impl.hpp"

#include <iostream>

using namespace std;


TestNamingContext::TestNamingContext(const std::string& name,
     CORBA::ORB_ptr orb,
     CdmwNamingAndRepository::Repository_ptr repository)
 : Testable(name)
{

    m_ORB = CORBA::ORB::_duplicate(orb);
    m_repository = CdmwNamingAndRepository::Repository::_duplicate(repository);

    CORBA::Object_var obj = m_ORB->resolve_initial_references("RootPOA");
    m_rootPOA = PortableServer::POA::_narrow(obj.in());

}


TestNamingContext::~TestNamingContext()
{

}



void TestNamingContext::display_binding_list(const CosNaming::BindingList& bl)
{

    cout << "-- binding list --" << endl;

    for (CORBA::ULong i = 0; i < bl.length(); i++)
    {
        display_binding(bl[i]);
    }

    cout << "-- end binding list --" << endl;

}


void TestNamingContext::display_binding(const CosNaming::Binding& b)
{

        cout << b.binding_name[0].id.in() ;
        cout << " . ";
        cout << b.binding_name[0].kind.in() ;

        if (b.binding_type == CosNaming::ncontext)
            cout << " : context" << endl;
        else
            cout << " : object" << endl;

}



void TestNamingContext::do_tests()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (36);


    CORBA::Object_ptr obj;

    CosNaming::Name_var name = new CosNaming::Name;
    CosNaming::NameComponent_var nc = new CosNaming::NameComponent;

    CosNaming::NamingContextExt_var rootCtx;

    // ---------------
    TEST_INFO("Get the root context");

    try
    {
        CosNaming::NamingContext_var temp_rootCtx =
	        m_repository->resolve_root_context(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);

        rootCtx = CosNaming::NamingContextExt::_narrow(temp_rootCtx.in());
	
    }
    catch(...)
    {
        cout << "Cannot get Root Context" << endl;
        TEST_FAILED();
        return;
    }


    // ---------------
    TEST_INFO("Create the object O1");

    string obj1_name = "O1";
    Tester_impl *obj1_i = new Tester_impl(obj1_name);
    PortableServer::ObjectId_var obj1_oid = m_rootPOA->activate_object(obj1_i);
    Tester_var obj1 = obj1_i->_this();


    // --------- BIND AND BIND CONTEXT ---------


    

    
    // ---------------
    TEST_INFO("Bind the object O1 as \"O1.object\" within the root context");

    name->length(1);
    nc->id = CORBA::string_dup("O1");
    nc->kind = CORBA::string_dup("object");
    name->operator[](0) = nc.in();

    try
    {
        rootCtx->bind(name.in(), obj1.in());
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Bind a new context \"CTX1.context\" within the root context");

    name->length(1);
    nc->id = CORBA::string_dup("CTX1");
    nc->kind = CORBA::string_dup("context");
    name->operator[](0) = nc.in();
    
    CosNaming::NamingContext_var ctx1;

    try
    {
        ctx1 = rootCtx->bind_new_context(name.in());
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Create a context");
  
    CosNaming::NamingContext_var ctx2;

    try
    {
        ctx2 = rootCtx->new_context();
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

   
    // ---------------
    TEST_INFO("Bind the previous context as the object \"CTX2.object\" within the root context");
  
    name->length(1);
    nc->id = CORBA::string_dup("CTX2");
    nc->kind = CORBA::string_dup("object");
    name->operator[](0) = nc.in();

    try
    {
        rootCtx->bind(name.in(), ctx2.in());
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Bind the previous context as the context \"CTX2.context\" within the root context");
  
    name->length(1);
    nc->id = CORBA::string_dup("CTX2");
    nc->kind = CORBA::string_dup("context");
    name->operator[](0) = nc.in();

    try
    {
        rootCtx->bind_context(name.in(), ctx2.in());
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }


    // ---------------
    TEST_INFO("Try to bind a NIL context within the root context");
  
    name->length(1);
    nc->id = CORBA::string_dup("NIL");
    nc->kind = CORBA::string_dup("context");
    name->operator[](0) = nc.in();

    try
    {
        CosNaming::NamingContext_ptr nil = CosNaming::NamingContext::_nil();
        
        rootCtx->bind_context(name.in(), nil);
    }
    catch(CORBA::BAD_PARAM &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }
    

    // ---------------
    TEST_INFO("Try to bind a new context \"CTX1.context\" within the root context");

    name->length(1);
    nc->id = CORBA::string_dup("CTX1");
    nc->kind = CORBA::string_dup("context");
    name->operator[](0) = nc.in();
    
    try
    {
        CosNaming::NamingContext_var ctx = rootCtx->bind_new_context(name.in());
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }


    // ---------------
    TEST_INFO("Create the object O2");

    string obj2_name = "O2";
    Tester_impl *obj2_i = new Tester_impl(obj2_name);
    PortableServer::ObjectId_var obj2_oid = m_rootPOA->activate_object(obj2_i);
    Tester_var obj2 = obj2_i->_this();

    // ---------------
    TEST_INFO("Try to bind the object O2 as \"O1.object\" within the root context");

    name->length(1);
    nc->id = CORBA::string_dup("O1");
    nc->kind = CORBA::string_dup("object");
    name->operator[](0) = nc.in();

    try
    {
        rootCtx->bind(name.in(), obj2.in());
        TEST_FAILED();
    }
    catch(CosNaming::NamingContext::AlreadyBound &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }
    
    // ---------------
    TEST_INFO("Bind the object O2 as \"O2.object\" within \"CTX1.context\"");

    name->length(1);
    nc->id = CORBA::string_dup("O2");
    nc->kind = CORBA::string_dup("object");
    name->operator[](0) = nc.in();

    try
    {
        ctx1->bind(name.in(), obj2.in());
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Create the object O3");

    string obj3_name = "O3";
    Tester_impl *obj3_i = new Tester_impl(obj3_name);
    PortableServer::ObjectId_var obj3_oid = m_rootPOA->activate_object(obj3_i);
    Tester_var obj3 = obj3_i->_this();

    // ---------------
    TEST_INFO("Try to bind the object O3 as \"CTX1.context/O2.object\" within the root context");

    name->length(2);
    nc->id = CORBA::string_dup("CTX1");
    nc->kind = CORBA::string_dup("context");
    name->operator[](0) = nc.in();
    nc->id = CORBA::string_dup("O2");
    nc->kind = CORBA::string_dup("object");
    name->operator[](1) = nc.in();

    try
    {
        rootCtx->bind(name.in(), obj2.in());
        TEST_FAILED();
    }
    catch(CosNaming::NamingContext::AlreadyBound &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }
   
    
    // ---------------
    TEST_INFO("Bind the object O3 as \"CTX1.context/O3.object\" within the root context");

    name->length(2);
    nc->id = CORBA::string_dup("CTX1");
    nc->kind = CORBA::string_dup("context");
    name->operator[](0) = nc.in();
    nc->id = CORBA::string_dup("O3");
    nc->kind = CORBA::string_dup("object");
    name->operator[](1) = nc.in();

    try
    {
        rootCtx->bind(name.in(), obj3.in());
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Create the object O4");

    string obj4_name = "O4";
    Tester_impl *obj4_i = new Tester_impl(obj4_name);
    PortableServer::ObjectId_var obj4_oid = m_rootPOA->activate_object(obj4_i);
    Tester_var obj4 = obj4_i->_this();
    
    // ---------------
    TEST_INFO("Try to bind the object O4 as \"CTX2.object/O4.object\" within the root context");

    name->length(2);
    nc->id = CORBA::string_dup("CTX2");
    nc->kind = CORBA::string_dup("object");
    name->operator[](0) = nc.in();
    nc->id = CORBA::string_dup("O4");
    nc->kind = CORBA::string_dup("object");
    name->operator[](1) = nc.in();

    try
    {
        rootCtx->bind(name.in(), obj4.in());
    }
    catch(CosNaming::NamingContext::NotFound &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }
    
    // ---------------
    TEST_INFO("Bind the object O4 as \"CTX2.context/O4.object\" within the root context");

    
    name->length(2);
    nc->id = CORBA::string_dup("CTX2");
    nc->kind = CORBA::string_dup("context");
    name->operator[](0) = nc.in();
    nc->id = CORBA::string_dup("O4");
    nc->kind = CORBA::string_dup("object");
    name->operator[](1) = nc.in();

    try
    {
        rootCtx->bind(name.in(), obj4.in());
        TEST_SUCCEED();
    }
    catch(CosNaming::NamingContext::NotFound &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // --------- RESOLVE ---------
    

    // ---------------
    TEST_INFO("Try to resolve \"OX\" from the root context");

    name->length(1);
    nc->id = CORBA::string_dup("OX");
    nc->kind = CORBA::string_dup("");
    name->operator[](0) = nc.in();

    try
    {
        obj = rootCtx->resolve(name.in());
    }
    catch(CosNaming::NamingContext::NotFound &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }

    // ---------------
    TEST_INFO("Resolve \"O1.object\" from the root context");

    name->length(1);
    nc->id = CORBA::string_dup("O1");
    nc->kind = CORBA::string_dup("object");
    name->operator[](0) = nc.in();

    try
    {
        obj = rootCtx->resolve(name.in());

        Tester_var res_obj1 = Tester::_narrow(obj);

        CORBA::release(obj);

        string info1 = res_obj1->info();

        if (!info1.compare("O1"))
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
    TEST_INFO("Try to resolve \"OX/O2.object\" from the root context");

    name->length(2);
    nc->id = CORBA::string_dup("OX");
    nc->kind = CORBA::string_dup("");
    name->operator[](0) = nc.in();
    nc->id = CORBA::string_dup("O2");
    nc->kind = CORBA::string_dup("object");
    name->operator[](1) = nc.in();

    try
    {
        obj = rootCtx->resolve(name.in());
    }
    catch(CosNaming::NamingContext::NotFound &e)
    {
        std::cout << e._name();

        TEST_SUCCEED();
    }

    // ---------------
    TEST_INFO("Try to resolve \"O1.object/O2.object\" from the root context");

    name->length(2);
    nc->id = CORBA::string_dup("O1");
    nc->kind = CORBA::string_dup("object");
    name->operator[](0) = nc.in();
    nc->id = CORBA::string_dup("O2");
    nc->kind = CORBA::string_dup("object");
    name->operator[](1) = nc.in();

    try
    {
        obj = rootCtx->resolve(name.in());
    }
    catch(CosNaming::NamingContext::NotFound &e)
    {
        std::cout << e._name();

        TEST_SUCCEED();
    }

    // ---------------
    TEST_INFO("Resolve \"CTX1.context/O2.object\" from the root context");

    name->length(2);
    nc->id = CORBA::string_dup("CTX1");
    nc->kind = CORBA::string_dup("context");
    name->operator[](0) = nc.in();
    nc->id = CORBA::string_dup("O2");
    nc->kind = CORBA::string_dup("object");
    name->operator[](1) = nc.in();


    try
    {
        obj = rootCtx->resolve(name.in());

        Tester_var res_obj2 = Tester::_narrow(obj);

        CORBA::release(obj);

        string info2 = res_obj2->info();

        if (!info2.compare("O2"))
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
    TEST_INFO("Try to resolve \"CTX1.context/OX\" from the root context");

    name->length(2);
    nc->id = CORBA::string_dup("CTX1");
    nc->kind = CORBA::string_dup("context");
    name->operator[](0) = nc.in();
    nc->id = CORBA::string_dup("OX");
    nc->kind = CORBA::string_dup("");
    name->operator[](1) = nc.in();

    try
    {
        obj = rootCtx->resolve(name.in());
    }
    catch(CosNaming::NamingContext::NotFound &e)
    {
        std::cout << e._name();

        TEST_SUCCEED();
    }



    // --------------- RESOLVE_STR
    TEST_INFO("Resolve \"O1.object\" string from the root context");

    try
    {
        obj = rootCtx->resolve_str("O1.object");

        Tester_var res_obj1 = Tester::_narrow(obj);

        CORBA::release(obj);

        string info1 = res_obj1->info();

        if (!info1.compare("O1"))
        {
            TEST_SUCCEED();
        }

    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }


    // --------------- RESOLVE_STR
    TEST_INFO("Resolve \"CTX1.context/O2.object\" string from the root context");


    try
    {
        obj = rootCtx->resolve_str("CTX1.context/O2.object");

        Tester_var res_obj2 = Tester::_narrow(obj);

        CORBA::release(obj);

        string info2 = res_obj2->info();

        if (!info2.compare("O2"))
        {
            TEST_SUCCEED();
        }
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }


    // --------------- LIST
    TEST_INFO("List from the root context");


    try
    {
        CosNaming::BindingList_var bl;
        CosNaming::BindingIterator_var bi;

        rootCtx->list(2, bl, bi);

        display_binding_list(bl.in());

        if (!CORBA::is_nil(bi.in()))
        {
            CosNaming::BindingList_var bl;

            while (bi->next_n(2, bl))
            {
                display_binding_list(bl.in());
            }

            bi->destroy();

        }

    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }


    // --------------- LIST
    TEST_INFO("List from the root context - Using Binding Iterator");


    try
    {
        CosNaming::BindingList_var bl;
        CosNaming::BindingIterator_var bi;

        rootCtx->list(0, bl, bi);

        if (!CORBA::is_nil(bi.in()))
        {
            CosNaming::Binding_var binding;

            while (bi->next_one(binding))
            {
                display_binding(*binding);
            }

            bi->destroy();

            try
            {
                bi->next_one(binding);
            }
            catch(CORBA::OBJECT_NOT_EXIST &e)
            {
                std::cout << e._name();
                TEST_SUCCEED();
            }

        }


    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }



    // --------- REBIND AND REBIND CONTEXT ---------


    // ---------------
    TEST_INFO("Rebind the object O1 as \"O1.rebind\" within the root context");

    name->length(1);
    nc->id = CORBA::string_dup("O1");
    nc->kind = CORBA::string_dup("rebind");
    name->operator[](0) = nc.in();

    try
    {
        rootCtx->rebind(name.in(), obj1.in());
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Rebind the context CTX1 as \"CTX1.rebind\" within the root context");

    name->length(1);
    nc->id = CORBA::string_dup("CTX1");
    nc->kind = CORBA::string_dup("rebind");
    name->operator[](0) = nc.in();

    try
    {
        rootCtx->rebind_context(name.in(), ctx1.in());
        TEST_SUCCEED();
    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // ---------------
    TEST_INFO("Try to rebind the object O1 as \"CTX1.context\" within the root context");

    name->length(1);
    nc->id = CORBA::string_dup("CTX1");
    nc->kind = CORBA::string_dup("context");
    name->operator[](0) = nc.in();
    
    try
    {
        rootCtx->rebind(name.in(), obj1.in());
	TEST_FAILED();
    }
    catch(CosNaming::NamingContext::NotFound &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }

    // ---------------
    TEST_INFO("Try to rebind the context CTX1 as \"O1.object\" within the root context");

    name->length(1);
    nc->id = CORBA::string_dup("O1");
    nc->kind = CORBA::string_dup("object");
    name->operator[](0) = nc.in();

    try
    {
        rootCtx->rebind_context(name.in(), ctx1.in());
         TEST_FAILED();
    }
    catch(CosNaming::NamingContext::NotFound &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }

    // ---------------
    TEST_INFO("Rebind the object O2 as \"O1.object\" within the root context");

    name->length(1);
    nc->id = CORBA::string_dup("O1");
    nc->kind = CORBA::string_dup("object");
    name->operator[](0) = nc.in();

    try
    {
        rootCtx->rebind(name.in(), obj2.in());

        obj = rootCtx->resolve(name.in());

        Tester_var res_obj = Tester::_narrow(obj);

        CORBA::release(obj);

        string info = res_obj->info();

        if (!info.compare("O2"))
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
    TEST_INFO("Rebind the CTX2 context as \"CTX1.context\" within the root context");

    name->length(1);
    nc->id = CORBA::string_dup("CTX1");
    nc->kind = CORBA::string_dup("context");
    name->operator[](0) = nc.in();

    try
    {
        try
        {
            rootCtx->rebind_context(name.in(), ctx2.in());
        }
        catch(CORBA::Exception &e)
        {
            std::cout << "rebind_context threw " << e << std::endl;
            throw;
        }

        name->length(2);
        nc->id = CORBA::string_dup("CTX1");
        nc->kind = CORBA::string_dup("context");
        name->operator[](0) = nc.in();
        nc->id = CORBA::string_dup("O4");
        nc->kind = CORBA::string_dup("object");
        name->operator[](1) = nc.in();

        obj = rootCtx->resolve(name.in());

        Tester_var res_obj = Tester::_narrow(obj);

        CORBA::release(obj);

        string info = res_obj->info();

        if (!info.compare("O4"))
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
    TEST_INFO("Unbind \"CTX2.object\" within the root context");


    try
    {
    
        CosNaming::Name_var name = rootCtx->to_name("CTX2.object");
        rootCtx->unbind(name.in());

        TEST_SUCCEED();

    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }



    // --------------- LIST
    TEST_INFO("List from the root context");


    try
    {
        CosNaming::BindingList_var bl;
        CosNaming::BindingIterator_var bi;

        rootCtx->list(100, bl, bi);

        display_binding_list(bl.in());

        if (!CORBA::is_nil(bi.in()))
        {
            bi->destroy();
        }

    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }


    // ---------------
    TEST_INFO("Try to destroy \"CTX2.context\"");

    CosNaming::NamingContextExt_var ctx_to_destroy;

    try
    {
    
        CORBA::Object_var ctx_obj = rootCtx->resolve_str("CTX2.context");
        ctx_to_destroy = CosNaming::NamingContextExt::_narrow(ctx_obj.in());

        ctx_to_destroy->destroy();
    }
    catch(CosNaming::NamingContext::NotEmpty &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();

        // --------------- LIST
        TEST_INFO("List \"CTX2.context\"");

        CosNaming::BindingList_var bl;
        CosNaming::BindingIterator_var bi;

        ctx_to_destroy->list(100, bl, bi);

        display_binding_list(bl.in());

        if (!CORBA::is_nil(bi.in()))
        {
            bi->destroy();
        }

    }


        // ---------------
    TEST_INFO("Unbind \"O4.object\" from \"CTX2.context\" and destroy the latter");


    try
    {
    
        CosNaming::Name_var name = ctx_to_destroy->to_name("O4.object");
        ctx_to_destroy->unbind(name.in());

        ctx_to_destroy->destroy();

        TEST_SUCCEED();

    }
    catch(CORBA::Exception &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }


    // ---------------
    TEST_INFO("Try to invoke \"CTX2.context\"");

    try
    {
        CosNaming::Name_var name = ctx_to_destroy->to_name("O4.object");
    }
    catch(CORBA::OBJECT_NOT_EXIST  &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }


    // --------------- 
    TEST_INFO("Try to invoke get the URLString Using a empty Address and Empty StringName")
    try
    {
      CosNaming::NamingContextExt::URLString_var url_string = rootCtx->to_url("","");
    }
    catch(CosNaming::NamingContextExt::InvalidAddress &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }

  
    // --------------- 
    TEST_INFO("Try to invoke get the URLString Using a valid Address and Empty StringName")
    try
    {
      CosNaming::NamingContextExt::URLString_var url_string = rootCtx->to_url("thalesgroup.com","");
    }
    catch(CosNaming::NamingContextExt::InvalidAddress &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }
    catch(CosNaming::NamingContext::InvalidName &e)
    {
        std::cout << e._name();
        TEST_SUCCEED();
    }

    // --------------- 
    TEST_INFO("Try to invoke get the URLString Using a valid Address and a valid StringName")
    try
    {
      CosNaming::NamingContextExt::URLString_var url_string = rootCtx->to_url("555xyz.com/dev/NContext1","a/b/c");
      std::cout << "The URLString is : "<< url_string << " " << std::endl;
      TEST_SUCCEED();
    }
    catch(CosNaming::NamingContextExt::InvalidAddress &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }
    catch(CosNaming::NamingContext::InvalidName &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

    // --------------- 
    TEST_INFO("Try to invoke get the URLString Using a valid Address and a valid StringName that contains Escape characters")
    try
    {
      CosNaming::NamingContextExt::URLString_var url_string = rootCtx->to_url("555xyz.com/dev/NContext1","a\\/b/c/<a>.b/c.d");
      std::cout << "The URLString is : "<< url_string << " " << std::endl;
      TEST_SUCCEED();
    }
    catch(CosNaming::NamingContextExt::InvalidAddress &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }
    catch(CosNaming::NamingContext::InvalidName &e)
    {
        std::cout << e._name();
        TEST_FAILED();
    }

}



