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


#include "test2/NamingContextBinder.hpp"
#include "test2/Tester_impl.hpp"


#include <iostream>
#include <fstream>
#include <sstream>


using namespace std;


NamingContextBinder::NamingContextBinder(const std::string& name,
     PortableServer::POA_ptr rootPOA, CORBA::ORB_ptr orb)
 : Testable(name), m_rootPOA(rootPOA), m_ORB(orb)
{

}


NamingContextBinder::~NamingContextBinder()
{

}



void NamingContextBinder::display_binding_list(const CosNaming::BindingList& bl)
{

    cout << "-- binding list --" << endl;

    for (CORBA::ULong i = 0; i < bl.length(); i++)
    {
        display_binding(bl[i]);
    }

    cout << "-- end binding list --" << endl;

}


void NamingContextBinder::display_binding(const CosNaming::Binding& b)
{

    cout << b.binding_name[0].id.in() ;
    cout << " . ";
    cout << b.binding_name[0].kind.in() ;

    if (b.binding_type == CosNaming::ncontext)
        cout << " : context" << endl;
    else
        cout << " : object" << endl;

}



void NamingContextBinder::do_tests()
{

    CosNaming::NamingContextExt_var rootCtx;

    // ---------------
    TEST_INFO("Get the context");

    try
    {
        string rootCtxIOR;
        ifstream iorFile("/tmp/ThreadingTestNamingContext.IOR");
        iorFile >> rootCtxIOR;
        iorFile.close();

        CORBA::Object_var rootCtx_temp = m_ORB->string_to_object(rootCtxIOR.c_str());
        rootCtx = CosNaming::NamingContextExt::_narrow(rootCtx_temp.in());
    }
    catch(...)
    {
        cout << "Cannot get context" << endl;
        TEST_FAILED();
        return;
    }


    // ---------------
    TEST_INFO("Create the object O1");

    string obj1_name = "O1";
    Tester_impl *obj1_i = new Tester_impl(obj1_name);
    PortableServer::ObjectId_var obj1_oid = m_rootPOA->activate_object(obj1_i);
    Tester_var obj1 = obj1_i->_this();


    // --------------- LIST
    TEST_INFO("List from the root context");

    unsigned long id = 0;
    unsigned long size;
    
    const unsigned long NB_DISPLAY = 20;


    for (;;)
    {

        try
        {
            CosNaming::BindingList_var bl;
            CosNaming::BindingIterator_var bi;

            rootCtx->list(NB_DISPLAY, bl, bi);

            size = bl->length();

            display_binding_list(bl.in());

            if (size < NB_DISPLAY)
            {

                std::string gen_name;
                id++;

                std::ostringstream temp;
                temp << "name" << id; 

                gen_name = temp.str();

                CosNaming::Name_var name = new CosNaming::Name(1);
                CosNaming::NameComponent_var nc = new CosNaming::NameComponent;
                name->length(1);
                nc->id = CORBA::string_dup(gen_name.c_str());
                nc->kind = CORBA::string_dup("");
                name->operator[](0) = nc;

                try
                {

                    rootCtx->bind(name.in(), obj1.in());	    

                }
                catch(CosNaming::NamingContext::AlreadyBound &e)
                {
                    // the name is already bound
                    // caused by a relaunch of this test
                    std::cout << e._name();
                }
                catch(CORBA::Exception &e)
                {
                    // abnormal
                    std::cout << e._name() << std::endl;
                    TEST_FAILED();
                    break;
                }

            }

            if (!CORBA::is_nil(bi.in()))
            {
                bi->destroy();
            }

        }
        catch(CORBA::Exception &e)
        {
            std::cout << e._name() << std::endl;
            TEST_FAILED();
            break;
        }

    }
    
}



