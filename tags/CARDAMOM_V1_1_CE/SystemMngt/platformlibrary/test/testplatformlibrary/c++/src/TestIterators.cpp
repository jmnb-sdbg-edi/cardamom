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


#include "testplatformlibrary/TestIterators.hpp"
#include "SystemMngt/platformlibrary/EntityNamesIterator_impl.hpp"
#include "SystemMngt/platformlibrary/EntitiesIterator_impl.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/System.hpp"

#include <iostream>
#include <string>
#include <memory>


using namespace CdmwPlatformMngtEntity;


void display(const char* s)
{
    std::cout << "entity name :" << s << std::endl;
}


void display(const EntityNames& el)
{
    for (CORBA::ULong i = 0; i < el.length(); ++i)
    {
        std::cout << "entity name :" << el[i] << std::endl;
    }

}


void display(const Entity& e)
{
    std::cout << "entity name :" << (e.entity_name).in() << std::endl;
}



TestIterators::TestIterators(const std::string& name, PortableServer::POA_ptr poa)
    : Testable(name)
{
    m_POA = PortableServer::POA::_duplicate(poa);
}


TestIterators::~TestIterators()
{
}


void TestIterators::do_tests()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (7);
    
    TEST_INFO("Creates an EntityNamesIterator");
    EntityName_var e1 = CORBA::string_dup("e1");
    EntityName_var e2 = CORBA::string_dup("e2");
    EntityName_var e3 = CORBA::string_dup("e3");
    EntityName_var e4 = CORBA::string_dup("e4");
    EntityName_var e5 = CORBA::string_dup("e5");

    std::list<EntityName_var> entityNames;
    entityNames.push_back(e1);
    entityNames.push_back(e2);
    entityNames.push_back(e3);
    entityNames.push_back(e4);
    entityNames.push_back(e5);

    EntityNamesIterator_var entityNamesIt =
        Cdmw::PlatformMngt::EntityNamesIterator_impl::create(entityNames);

    TEST_INFO("Gets each entity name one by one from the iterator");

    try
    {
        EntityName_var e;

        CORBA::Boolean ret;
        size_t count = 0;

        do
        {
        
            ret = entityNamesIt->next_one(e);

            if (ret)
            {
                ++count;
                display(e.in());
            }

        }
        while (ret);

        if (count == 5)
        {
            TEST_SUCCEED();
        }
        else
        {
            TEST_FAILED();
        }

    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
    }


    TEST_INFO("Resets the iterator");

    entityNamesIt->reset();


    TEST_INFO("Gets each entity name two by two from the iterator");

    try
    {
        EntityNames_var el;

        CORBA::Boolean ret;
        size_t count = 0;

        do
        {
        
            ret = entityNamesIt->next_n(2, el);

            if (ret)
            {
                count += el->length();
                display(*el);
            }

        }
        while (ret);

        if (count == 5)
        {
            TEST_SUCCEED();
        }
        else
        {
            TEST_FAILED();
        }

    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
    }


    TEST_INFO("Resets the iterator");

    entityNamesIt->reset();


    TEST_INFO("Try to get 0 entity name");

    try
    {
        EntityNames_var el;
        entityNamesIt->next_n(0, el);

        TEST_FAILED();

    }
    catch(const CORBA::BAD_PARAM &e)
    {
        std::cout << e._name() << std::endl;
        TEST_SUCCEED();
    }


    TEST_INFO("Gets directly all entity names from the iterator");

    try
    {
        EntityNames_var el;

        CORBA::Boolean ret = entityNamesIt->next_n(10, el);

        if (ret)
        {
            display(*el);
        }

        if (el->length() == 5)
        {
            TEST_SUCCEED();
        }
        else
        {
            TEST_FAILED();
        }

    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
    }


    TEST_INFO("Resets the iterator");

    entityNamesIt->reset();

    TEST_INFO("Gets one entity name then all other entity names from the iterator");

    try
    {
        EntityName_var e;
        EntityNames_var el;
        CORBA::Boolean ret;

        size_t count = 0;

        ret = entityNamesIt->next_one(e);

        if (ret)
        {
            ++count;
            display(e.in());
        }

        ret = entityNamesIt->next_n(10, el);

        if (ret)
        {
            count += el->length();
            display(*el);
        }

        if (count == 5)
        {
            TEST_SUCCEED();
        }
        else
        {
            TEST_FAILED();
        }

    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
    }


    TEST_INFO("Destroys the iterator");

    entityNamesIt->destroy();


    TEST_INFO("Try to access the iterator");

    try
    {
        entityNamesIt->reset();
        TEST_FAILED();

    }
    catch(const CORBA::OBJECT_NOT_EXIST &e)
    {
        std::cout << e._name() << std::endl;
        TEST_SUCCEED();
    }


    // test subset for a CORBA struct iterator 

    TEST_INFO("Creates an EntitiesIterator");
    Entity_var E1 = new Entity;
    E1->entity_name = CORBA::string_dup("E1");
    Entity_var E2 = new Entity;
    E2->entity_name = CORBA::string_dup("E2");
    Entity_var E3 = new Entity;
    E3->entity_name = CORBA::string_dup("E3");
    Entity_var E4 = new Entity;
    E4->entity_name = CORBA::string_dup("E4");
    Entity_var E5 = new Entity;
    E5->entity_name = CORBA::string_dup("E5");

    std::list<Entity_var> entities;
    entities.push_back(E1);
    entities.push_back(E2);
    entities.push_back(E3);
    entities.push_back(E4);
    entities.push_back(E5);

    EntitiesIterator_var entitiesIt =
        Cdmw::PlatformMngt::EntitiesIterator_impl::create(entities);

    TEST_INFO("Gets each entity one by one from the iterator");

    try
    {
        Entity_var E;

        CORBA::Boolean ret;
        size_t count = 0;

        do
        {
        
            ret = entitiesIt->next_one(E);

            if (ret)
            {
                ++count;
                display(*E);
            }

        }
        while (ret);

        if (count == 5)
        {
            TEST_SUCCEED();
        }
        else
        {
            TEST_FAILED();
        }

    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
    }

}


