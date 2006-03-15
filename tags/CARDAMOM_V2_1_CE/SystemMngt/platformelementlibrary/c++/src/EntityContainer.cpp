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


#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "SystemMngt/platformelementlibrary/EntityContainer.hpp"
#include "SystemMngt/platformlibrary/ManagedElementKey.hpp"


/**
* Root namespace for CDMW runtime.
*/
namespace Cdmw
{
/**
* Namespace for CDMW PlatformMngt
*/
namespace PlatformMngt
{

    //
    // Constructor
    //
    EntityContainer::EntityContainer(PortableServer::POA_ptr poa,
                                     CdmwPlatformMngt::RWElementRepository_ptr element_repository,
                                     const char* entity_parent_path)
            throw(BadParameterException)
    {
        if (CORBA::is_nil(poa))
        {
            CDMW_THROW2 (BadParameterException, "poa", "nil");
        }
        
        // Set the internal data
        m_poa = PortableServer::POA::_duplicate(poa);
        
        // Set the element repository
        m_element_repository = 
           CdmwPlatformMngt::RWElementRepository::_duplicate(element_repository);
        
        // Set the entity parent path
        if (entity_parent_path == 0)
        {
            CDMW_THROW2 (BadParameterException, "entity_parent_path", "null");
        }
        
        m_parent = CORBA::string_dup (entity_parent_path);
    } 


    //
    // Destructor
    // 
    EntityContainer::~EntityContainer()
            throw()
    {
    } 


    //
    // set entity parent path
    //
    void EntityContainer::setParentPath (const char* entity_parent_path)
            throw(BadParameterException)
    {
        if (strcmp(entity_parent_path, "") == 0)
        {
            CDMW_THROW2 (BadParameterException, "entity_parent_path", "empty");
        }

        m_parent = CORBA::string_dup (entity_parent_path);
    } 


    //
    // Add entity in container
    //
    CdmwPlatformMngt::Entity_ptr 
    EntityContainer::addEntity(const CdmwPlatformMngt::EntityDef& entity_def)
        throw (CdmwPlatformMngt::EntityAlreadyExists,CosPropertyService::MultipleExceptions,
               CORBA::SystemException)
    {
       // get entity name
       std::string entity_name = entity_def.entity_name.in();
        
       try
       {
       	   bool result;
       	   
       	   // build the entity path
           std::string entity_path = 
                  ManagedElementKey::Get_entity_path (
                                         m_parent.in(), entity_name);
                                         
           // check if element already exists
           result = existsElement(entity_name);
           if (result == true)
           {  
               throw CdmwPlatformMngt::EntityAlreadyExists();
           }
                                         
           // Create the Entity servant
           Entity_impl* p_entity = new Entity_impl (m_poa.in(), entity_def,
                                                    entity_path.c_str(),
                                                    m_element_repository.in());
                   
           result = addElement(entity_name, p_entity);
        
           if (result == false)
           {  
               throw CdmwPlatformMngt::EntityAlreadyExists();
           }
                                  
           // create an object var to take pointer ownership
           PortableServer::ServantBase_var entityServant = p_entity;        
           // activate object
           p_entity->activate();
            
           // return Entity object reference
           CdmwPlatformMngt::Entity_var entityObject = p_entity->_this();
           
           // add in repository the entity object
           m_element_repository->add_element (entity_path.c_str(),
                                              CdmwPlatformMngt::ENTITY_TYPE,
                                              entityObject.in());
           
           return entityObject._retn();
       }
       catch(const CdmwPlatformMngt::ElementPathInvalid)
       {
       	   throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
       }
       catch(const CdmwPlatformMngt::ElementAlreadyExists)
       {
       	   throw CdmwPlatformMngt::EntityAlreadyExists();
       }
       catch(const std::bad_alloc &)
       {
           throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
       }
       catch(const OutOfResourcesException &)
       {
           throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
       }
       catch(const InternalErrorException &)
       {
           throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
       }
       catch (const AlreadyDoneException &)
       {
           throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
       }
       catch(const BadParameterException &)
       {
           throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
       }
       catch(const CORBA::Exception& e)
       {
           throw;
       }
    } 


        
    //
    // Remove entity from container
    //
    void EntityContainer::removeEntity(const char* entity_name)
                throw (CdmwPlatformMngt::EntityNotFound,
                       CORBA::SystemException)
    {
       // check if entity name is valid
       if (entity_name == NULL)
       {
           throw CdmwPlatformMngt::EntityNotFound();
       }

       if (*entity_name == '\0')
       { 
           throw CdmwPlatformMngt::EntityNotFound();
       }
    
       try
       {
           bool result;
            
           // first get the entity_impl object
           Entity_impl* p_entity_impl;
       
           result = findElement(entity_name, p_entity_impl);
        
           if (result == false)
           {
               throw CdmwPlatformMngt::EntityNotFound();
           }

           // suppress Entity_impl object from the container map
           result = removeElement(entity_name);
        
           if (result == false)
           {  
               throw CdmwPlatformMngt::EntityNotFound();
           }

           // remove from repository the entity object
           m_element_repository->remove_element (p_entity_impl->get_element_path());
           
           // deactivate the Corba object (object will be deleted by orb)
           p_entity_impl->deactivate();
            
       }
       catch(const CdmwPlatformMngt::ElementPathInvalid)
       {
       	   throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
       }
       catch(const CdmwPlatformMngt::ManagedElementNotFound)
       {
       	   throw CdmwPlatformMngt::EntityNotFound();
       }      
       catch(const std::bad_alloc &)
       {
           throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
       }
       catch(const OutOfResourcesException &)
       {
           throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
       }
       catch(const InternalErrorException &)
       {
           throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
       }
       catch(const BadOrderException &)
       {
           throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
       }
       catch(const CORBA::Exception& e)
       {
           throw;
       }
    } 
                      
    
    //
    // return entities snapshot
    //
    CdmwPlatformMngt::EntitiesSnapshot* EntityContainer::getSnapshot()
        throw (Cdmw::OutOfResourcesException)
    {
        try
        {
            CdmwPlatformMngt::EntitiesSnapshot_var snapshot =
                   new CdmwPlatformMngt::EntitiesSnapshot;
                                     
            // Get the snapshot of all entities
        
            // set sequence length
            snapshot->length (getNumberOfElements());
        
            // get list of entity 
            EntityList entityList;
            list (entityList);
        
            // fill snapshot sequence
            EntityList::iterator entityIt;
            unsigned int i;
        
            for (entityIt=entityList.begin(), i=0;
                 entityIt != entityList.end();
                 entityIt++, i++)
            {
                snapshot[i].entity_name = (*entityIt)->name();
                snapshot[i].entity_status = 
                       (*entityIt)->get_status ((snapshot[i].entity_info).out());
            }
            
            // return snapshot sequence
            return snapshot._retn();
        }
        catch(const std::bad_alloc &)
        {
            CDMW_THROW (Cdmw::OutOfResourcesException);
        }
    }
                     

} // End namespace PlatformMngt
} // End namespace Cdmw



