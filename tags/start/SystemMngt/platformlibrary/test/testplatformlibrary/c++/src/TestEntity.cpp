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


#include "testplatformlibrary/TestEntity.hpp"

#include "SystemMngt/platformlibrary/Iterator.hpp"
#include "SystemMngt/platformlibrary/Entity.hpp"
#include "SystemMngt/platformlibrary/EntityContainer_impl.hpp"
#include "SystemMngt/platformlibrary/RWEntityContainer_impl.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/System.hpp"

#include <iostream>
#include <string>
#include <memory>


using namespace CdmwPlatformMngtEntity;
using namespace Cdmw::PlatformMngt;


void displayEntity (const char* s)
{
    std::cout << "entity name :" << s << std::endl << std::endl;
}


void displayEntity (const EntityNames& el)
{
    for (CORBA::ULong i = 0; i < el.length(); ++i)
    {
        std::cout << "entity name :" << el[i] << std::endl;
    }
    
    std::cout << std::endl;
}


void displayEntity (const CdmwPlatformMngtEntity::Entity& e)
{
    std::cout << "entity name   :" << (e.entity_name).in() << std::endl;
    std::cout << "entity status :" << e.entity_status << std::endl;
    std::cout << "entity info   :" << (e.entity_info).in() << std::endl;   
    std::cout << std::endl;
}



TestEntity::TestEntity(const std::string& name, PortableServer::POA_ptr poa)
    : Testable(name)
{
    m_POA = PortableServer::POA::_duplicate(poa);
}


TestEntity::~TestEntity()
{
}


void TestEntity::do_tests()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (28);
    
	EntityContainer_impl::EntityContainer *pSystemContainer = NULL;

	
    try
    {
        // create the entity containers
        TEST_INFO("");
        TEST_INFO("Creates a System EntityContainer");
        pSystemContainer = 
                EntityContainer_impl::EntityContainer::createSystemContainer ();
 
        TEST_SUCCEED();  
    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
        return;
    } 
    
    
    PortableServer::ServantBase_var servant_var;
    PortableServer::ServantBase_var rwServant_var;
    EntityContainer_impl* pEntityContainerServant;
    RWEntityContainer_impl* pRWEntityContainerServant;
    EntityContainer_var entityContainer;
    RWEntityContainer_var rwEntityContainer;
    
    try
    {   
    	TEST_INFO("");
        TEST_INFO("Creates an EntityContainer Servant");
        pEntityContainerServant = 
				new EntityContainer_impl (m_POA.in(), pSystemContainer);
 
        // create an object var to take pointer ownership
        // (ref count will be decremented when var will be destroyed at the method end)
	    servant_var = pEntityContainerServant;
	
	    // activate object and get its reference
	    pEntityContainerServant->activate();
        entityContainer = pEntityContainerServant->_this();


        TEST_INFO("Creates an RWEntityContainer Servant");
        pRWEntityContainerServant = 
				new RWEntityContainer_impl (m_POA.in(), pSystemContainer);
 
        // create an object var to take pointer ownership
        // (ref count will be decremented when var will be destroyed at the method end)
	    rwServant_var = pRWEntityContainerServant;
	
	    // activate object and get its reference
	    pRWEntityContainerServant->activate();
        rwEntityContainer = pRWEntityContainerServant->_this();
    
    
        TEST_SUCCEED();
    
    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
        return;
    }
    
    // Check when no entities inserted in container
    try
    {    
    	// Check number of entities : must be 0
    	TEST_INFO("");
    	TEST_INFO("Check EntityContainer is empty");
        CORBA::ULong nbrOfEntities = entityContainer->get_number_of_entities ();
        
        if (nbrOfEntities == 0)
        {
            TEST_SUCCEED();
        }
        else
        {
            TEST_FAILED();
        }
        
        // Get all entity names : must be empty

        EntityNames_var entity_names = new EntityNames;
        EntityNamesIterator_var names_rest;
        TEST_INFO("Check EntityContainer return empty name sequence and empty iterator");
        entityContainer->get_all_entity_names (5, entity_names, names_rest);

        if (entity_names->length() == 0)
        {
            TEST_SUCCEED();
        }
        else
        {
            TEST_FAILED();
        }
        
        // check iterator is empty
        TEST_INFO("Check Entity iterator is empty");
        EntityName_var e_name;
        CORBA::Boolean ret = names_rest->next_one(e_name);

        if (ret)
        {
            TEST_FAILED();
        }
        else
        {
            TEST_SUCCEED();
        }
        
        
        
        // Get all entity : must be empty
        Entities_var entities;
        EntitiesIterator_var entities_rest;
        TEST_INFO("Check EntityContainer return empty entity sequence and empty iterator");
        entityContainer->get_all_entities (5, entities, entities_rest);

        if (entities->length() == 0)
        {
            TEST_SUCCEED();
        }
        else
        {
            TEST_FAILED();
        }
        
        // check iterator is empty
        TEST_INFO("Check Entity iterator is empty");
        Entity_var e_Data;
        ret = entities_rest->next_one(e_Data);

        if (ret)
        {
            TEST_FAILED();
        }
        else
        {
            TEST_SUCCEED();
        }
  
  
  
        // get entity 
        TEST_INFO("Check EntityContainer exception if entity with undefined name is requested");
        try
        {
        	EntityStatus entityStatus;
        	CORBA::String_var entityInfo;
        	
            // Get entity status : exception must be thrown
            entityStatus = entityContainer->get_entity_status ("e1", entityInfo);
            
            TEST_FAILED();
        }
        catch (EntityNotFound& ex)
        {
        	TEST_SUCCEED();
        }
       
  
        // Get entity data from name sequence     
        TEST_INFO("Check EntityContainer return empty entities sequence for requested names");
        entity_names->length(3);
        entity_names->operator[](0) = CORBA::string_dup("e1");
        entity_names->operator[](1) = CORBA::string_dup("e5"); 
	    entity_names->operator[](2) = CORBA::string_dup("e3");
 
        
        ret = entityContainer->get_entities (entity_names.in(), entities);
        
        if (ret)
        {
            TEST_FAILED();
        }
        else
        {
            TEST_SUCCEED();
        }
    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
        return;
    }





    // Check entities insertion in container
    try
    {
    	TEST_INFO("");
        TEST_INFO("Check adding entities in EntityContainer");
        rwEntityContainer->add_entity ("e1");
        rwEntityContainer->add_entity ("e2");
        rwEntityContainer->add_entity ("e3");
        rwEntityContainer->add_entity ("e4");
        rwEntityContainer->add_entity ("e5");
        rwEntityContainer->add_entity ("e11");
        TEST_SUCCEED();
        
        TEST_INFO("Check adding duplicate entity in EntityContainer");
        try
        {
            // Add entity with duplicate name : exception must be thrown
            rwEntityContainer->add_entity ("e11");
            
            TEST_FAILED();
        }
        catch (EntityAlreadyExists& ex)
        {
        	TEST_SUCCEED();
        }
        
        
        TEST_INFO("Check removing undefined entity in EntityContainer");
        try
        {
            // Remove entity with undefined name : exception must be thrown
            rwEntityContainer->remove_entity ("e12");
            
            TEST_FAILED();
        }
        catch (EntityNotFound& ex)
        {
        	TEST_SUCCEED();
        }
        
        TEST_INFO("Check removing entity in EntityContainer");
        rwEntityContainer->remove_entity ("e11");
        TEST_SUCCEED();
    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
        return;
    }
    
 
 
 
 
    
    // Check entities setting in container
    try
    {
    	TEST_INFO("");
        TEST_INFO("Check setting unknown entity in EntityContainer");
        try
        {
            // set entity with bad name : exception must be thrown
            pRWEntityContainerServant->set_entity_status ("e6",ENTITY_FUNCTIONING,"");
            
            TEST_FAILED();
        }
        catch (EntityNotFound& ex)
        {
        	TEST_SUCCEED();
        }
        
        TEST_INFO("Check setting entities in EntityContainer");
        pRWEntityContainerServant->set_entity_status ("e1",ENTITY_FUNCTIONING,
                                          "information 1");
        pRWEntityContainerServant->set_entity_status ("e2",ENTITY_DYSFUNCTION,
                                          "information 2");                                
        pRWEntityContainerServant->set_entity_status ("e3",ENTITY_FAILED_NO_RESPONSE,
                                          "information 3");
        pRWEntityContainerServant->set_entity_status ("e4",ENTITY_FAILED_DEATH,
                                          "information 4");
        pRWEntityContainerServant->set_entity_status ("e5",ENTITY_FUNCTIONING,
                                          "information 5");
        TEST_SUCCEED();       
    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
        return;
    }
  
  
  
  
  
    
    // Get entities from filled container
    try
    { 
        // Check number of entities : must be 5
        TEST_INFO("");
    	TEST_INFO("Check EntityContainer is filled");
        CORBA::ULong nbrOfEntities = entityContainer->get_number_of_entities ();
        
        if (nbrOfEntities == 5)
        {
            TEST_SUCCEED();
        }
        else
        {
            TEST_FAILED();
        }
   
        
        // Get all entity names with iterator
        EntityNames_var entity_names;
        EntityNamesIterator_var names_rest;
        
        TEST_INFO("Check all entity names from EntityContainer with iterator");
        entityContainer->get_all_entity_names (3, entity_names, names_rest);

        if (entity_names->length() == 3)
        {
            TEST_SUCCEED();
        }
        else
        {
            TEST_FAILED();
        }
        
        size_t count;
        
        for (count=0 ; count < entity_names->length() ; count++)
        {
        	displayEntity(entity_names[count].in());
        }
        
        // check iterator
        TEST_INFO("Gets each entity name one by one from the iterator");

        CORBA::Boolean ret;
        EntityName_var e_name;
        count = 0;
        
        do
        {   
            ret = names_rest->next_one(e_name);  

            if (ret)
            {
                count++;
                displayEntity(e_name.in());
            }

        }
        while (ret);

        if (count == 2)
        {
            TEST_SUCCEED();
        }
        else
        {
            TEST_FAILED();
        }


        // Get all entity names
        TEST_INFO("Check all entity names from EntityContainer without iterator");      
        entityContainer->get_all_entity_names (20, entity_names, names_rest);

        if (entity_names->length() == 5)
        {
            TEST_SUCCEED();
        }
        else
        {
            TEST_FAILED();
        }
        
        // check iterator is empty
        TEST_INFO("Check Entity iterator is empty");
        ret = names_rest->next_one(e_name);

        if (ret)
        {
            TEST_FAILED();
        }
        else
        {
            TEST_SUCCEED();
        }
        
        
        // Get all entity with iterator
        Entities_var entities;
        EntitiesIterator_var entities_rest;
        
        TEST_INFO("Check all entities from EntityContainer with iterator");
        entityContainer->get_all_entities (3, entities, entities_rest);

        if (entities->length() == 3)
        {
            TEST_SUCCEED();
        }
        else
        {
            TEST_FAILED();
        }
        
        for (count=0 ; count < entities->length() ; count++)
        {
        	displayEntity(entities[count]);
        }
        
        // check iterator
        TEST_INFO("Gets each entity one by one from the iterator");
        Entity_var e_data;
        count = 0;
        
        do
        {   
            ret = entities_rest->next_one(e_data);  

            if (ret)
            {
                count++;
                displayEntity(e_data.in());
            }

        }
        while (ret);

        if (count == 2)
        {
            TEST_SUCCEED();
        }
        else
        {
            TEST_FAILED();
        }


        // Get all entity
        TEST_INFO("Check all entity from EntityContainer without iterator");      
        entityContainer->get_all_entities (20, entities, entities_rest);

        if (entities->length() == 5)
        {
            TEST_SUCCEED();
        }
        else
        {
            TEST_FAILED();
        }
        
        // check iterator is empty
        TEST_INFO("Check Entity iterator is empty");
        ret = entities_rest->next_one(e_data);

        if (ret)
        {
            TEST_FAILED();
        }
        else
        {
            TEST_SUCCEED();
        }
        
        
        

        // get entity    
        TEST_INFO("Get specific entity from container");
        try
        {
        	EntityStatus entityStatus;
        	CORBA::String_var entityInfo;
        	
            // Get entity status
            EntityName_var e_name = "e1";
            entityStatus = entityContainer->get_entity_status (e_name.in(), entityInfo);                 
                   
            std::cout << "entity name   :" << e_name.in() << std::endl;
            std::cout << "entity status :" << entityStatus << std::endl;
            std::cout << "entity info   :" << entityInfo.in() << std::endl;  
             
            TEST_SUCCEED();
        }
        catch (EntityNotFound& ex)
        {
        	TEST_FAILED();
        }
  
  
        TEST_INFO("Get undefined entity from container");
        try
        {
        	EntityStatus entityStatus;
        	CORBA::String_var entityInfo;
        	
            // Get entity status
            EntityName_var e_name = "e11";
            entityStatus = entityContainer->get_entity_status (e_name.in(), entityInfo);                 
                                
            TEST_FAILED();
        }
        catch (EntityNotFound& ex)
        {
        	TEST_SUCCEED();
        }
       
       
        TEST_INFO("Check EntityContainer return entities for requested names");
        entity_names->length(3);

        entity_names->operator[](0) = CORBA::string_dup("e1");
        entity_names->operator[](1) = CORBA::string_dup("e5"); 
        entity_names->operator[](2) = CORBA::string_dup("e3");
        
        // Get entity data from name sequence
        ret = entityContainer->get_entities (entity_names.in(), entities);
        
        if (ret)
        {
        	for (count=0 ; count < entities->length() ; count++)
            {
        	  displayEntity(entities[count]);
        	}
        	
            TEST_SUCCEED();
        }
        else
        {        
            TEST_FAILED();
        }
        
        
        // destroy the container
        TEST_INFO("Destroy container");
        
        ret = EntityContainer_impl::EntityContainer::destroy (pSystemContainer);
        if (ret)
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
        return;
    }

}


