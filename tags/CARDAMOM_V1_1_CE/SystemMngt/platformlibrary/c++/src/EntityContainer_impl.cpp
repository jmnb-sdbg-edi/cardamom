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


#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "SystemMngt/platformlibrary/EntityContainer_impl.hpp"
#include "SystemMngt/platformlibrary/EntitiesIterator_impl.hpp"
#include "SystemMngt/platformlibrary/EntityNamesIterator_impl.hpp"



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
// initialization of static member of entity container
//
#ifdef AIX

template <class ELEMENT_DATA> Cdmw::OsSupport::Mutex Container<ELEMENT_DATA>::M_existingContainers_mutex;
template <class ELEMENT_DATA> typename Container<ELEMENT_DATA>::Containers Container<ELEMENT_DATA>::M_existingContainers;

template<> Cdmw::OsSupport::Mutex EntityContainer_impl::EntityContainer::M_existingContainers_mutex;
template<> EntityContainer_impl::EntityContainer::Containers EntityContainer_impl::EntityContainer::M_existingContainers;

#else

Cdmw::OsSupport::Mutex EntityContainer_impl::EntityContainer::M_existingContainers_mutex;
EntityContainer_impl::EntityContainer::Containers EntityContainer_impl::EntityContainer::M_existingContainers;

#endif

//
// Constructor
// 
EntityContainer_impl::EntityContainer_impl(PortableServer::POA_ptr poa,
                                           EntityContainer* pActualContainer)
        throw()
        : DeactivableServant_impl (poa),
          m_pActualContainer (pActualContainer)
           
            
{
}



//
// Destructor
// 
EntityContainer_impl::~EntityContainer_impl()
        throw()
{
}



//
// Get number of entities from container
//
CORBA::ULong EntityContainer_impl::get_number_of_entities()
        throw(CORBA::SystemException)
{
	CORBA::ULong number = m_pActualContainer->getNumberOfElements();
	return number;
}




//
// Get status on entity selected by its name from container
//
CdmwPlatformMngtEntity::EntityStatus EntityContainer_impl::get_entity_status(
                                            const char* entity_name,
                                            CORBA::String_out entity_info)
         throw(CdmwPlatformMngtEntity::EntityNotFound,
               CORBA::SystemException)
{
	try
	{
	    Entity data;
	    
	    bool result = m_pActualContainer->findElement(entity_name, data);
	    
	    if (result == false)
	    {
	    	throw CdmwPlatformMngtEntity::EntityNotFound();
	    }
	    else
	    {
	        entity_info = CORBA::string_dup(data.m_entityInfo.c_str());
	        return data.m_entityState;
	    }
	}
	catch(const std::bad_alloc &)
    {
        throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
    }
    catch(const OutOfResourcesException &)
    {
        throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
    }
    catch(const OutOfMemoryException &)
    {
        throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
    }
    catch(const AssertionFailedException &)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
    catch(const InternalErrorException &)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
	catch(const CORBA::Exception& e)
    {
        throw;
    }
}  
                


//
// Get sequence of entities from container
// 
CORBA::Boolean EntityContainer_impl::get_entities(const CdmwPlatformMngtEntity::EntityNames& entity_names,
                                                  CdmwPlatformMngtEntity::Entities_out nentities)
         throw(CORBA::SystemException)
{
	try
    {
    	CORBA::Boolean result = true;
    	
        // create the requested sequence of entities
        CdmwPlatformMngtEntity::Entities_var requested_entities = 
                                           new CdmwPlatformMngtEntity::Entities;
                                           
        // set length of requested sequence
        size_t len = entity_names.length();
                     
        if (len > 0)
        {
             requested_entities->length(len);
        }
                                           
    	for (unsigned int i=0 ; i < len ; i++)
        {
        	Entity data;
            CdmwPlatformMngtEntity::Entity entity;

            // get entity name from request list
            #if CDMW_ORB_VDR == orbacus
            std::string name = entity_names[i];
            #else
            std::string name = (entity_names[i]).in();
            #endif
            
            entity.entity_name = CORBA::string_dup(name.c_str());
            
            entity.entity_info = CORBA::string_dup("");
            
            // check if entity name is valid
            if (name.empty())
            {
            	result = false;
            }
    
            else
            {
                bool findResult = m_pActualContainer->findElement(name, data);
	    
	            if (findResult == false)
	            {        	
	        	    result = false;
	            }
	            else
	            {
	                entity.entity_info = CORBA::string_dup(data.m_entityInfo.c_str());
	                entity.entity_status = data.m_entityState;
	            }
	        }
            
            
            // stock entity in the sequence           
            requested_entities[i] = entity;
        }
        
        // everything is OK, so return requested entity sequence
        nentities = requested_entities._retn();
        
        // return method result
        return result;

    }
    catch(const std::bad_alloc &)
    {
        throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
    }
    catch(const OutOfResourcesException &)
    {
        throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
    }
    catch(const OutOfMemoryException &)
    {
        throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
    }
    catch(const AssertionFailedException &)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
    catch(const InternalErrorException &)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
	catch(const CORBA::Exception& e)
    {
        throw;
    }
}
    	
    	
    	
    	
//
// Get all entities names from container
//
void EntityContainer_impl::get_all_entity_names(CORBA::ULong how_many,
                                                CdmwPlatformMngtEntity::EntityNames_out entity_names,
                                                CdmwPlatformMngtEntity::EntityNamesIterator_out rest)
         throw(CORBA::SystemException)
{
	try
    {
    	 // list of request entitie names from container
         EntityNameList entityCtnrReqList;
         
         // list of rest of entitie names from container
         EntityNameList entityCtnrRestList;

         // get list of entitie names from container
         m_pActualContainer->listName(how_many, 
                                      entityCtnrReqList, entityCtnrRestList);

         // create the requested sequence of entitie names
         CdmwPlatformMngtEntity::EntityNames_var requested_entities = 
                                           new CdmwPlatformMngtEntity::EntityNames;
         
         // get length of list                                    
         size_t len = entityCtnrReqList.size();
                
         if (len > 0)
         {
         	 // set length of sequence
             requested_entities->length(len);

             // initialize list iterator
             EntityNameList::iterator it = entityCtnrReqList.begin();
             
             for (CORBA::ULong i = 0; i < len; i++)
             {
             	// get entity name from container list using iterator
                char *p_entity_name = CORBA::string_dup((*it).c_str());
                    
                // store entity name in sequence
                requested_entities[i] = p_entity_name;

                // increment iterator
                it++;
             }
         }
         
 
         // create the rest of list of entitie names
         std::list <CdmwPlatformMngtEntity::EntityName_var> rest_of_entities;
          
         // get length of list      
         len = entityCtnrRestList.size();
                
         if (len > 0)
         {
             // initialize list iterator
             EntityNameList::iterator it = entityCtnrRestList.begin();
             
             for (CORBA::ULong i = 0; i < len; i++)
             {
             	// get entity name from container list using iterator
                CdmwPlatformMngtEntity::EntityName_var entity_name = CORBA::string_dup((*it).c_str());

                // store entity name in list
                rest_of_entities.push_back(entity_name);

                // increment iterator
                it++;
             }
         }

         // create the EntityNamesIterator with the rest of list
         CdmwPlatformMngtEntity::EntityNamesIterator_var entities_iter =
                          EntityNamesIterator_impl::create (rest_of_entities);

         // everything is OK, so return requested list and iterator
         entity_names = requested_entities._retn();
         rest = entities_iter._retn();
        
     }
     catch(const std::bad_alloc &)
     {
         throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
     }
     catch(const OutOfResourcesException &)
     {
         throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
     }
     catch(const OutOfMemoryException &)
     {
         throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
     }
     catch(const AssertionFailedException &)
     {
         throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
     }
     catch(const InternalErrorException &)
     {
         throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
     }
     catch(const CORBA::Exception& e)
     {
         throw;
     }
   
}
    	
    	
//
// Get all entities from container
//
void EntityContainer_impl::get_all_entities (CORBA::ULong how_many,
                                             CdmwPlatformMngtEntity::Entities_out nentities,
                                             CdmwPlatformMngtEntity::EntitiesIterator_out rest)
         throw(CORBA::SystemException)
{   	
    try
    {       
         // list of request entities from container
         EntityList entityCtnrReqList;
         
         // list of rest of entities from container
         EntityList entityCtnrRestList;

         // get list of entities from container
         m_pActualContainer->list(how_many, 
                                  entityCtnrReqList, entityCtnrRestList);

         // create the requested sequence of entities
         CdmwPlatformMngtEntity::Entities_var requested_entities = 
                                           new CdmwPlatformMngtEntity::Entities;
         
         // get length of list                                    
         size_t len = entityCtnrReqList.size();
                
         if (len > 0)
         {
         	 // set length of sequence
             requested_entities->length(len);

             // initialize list iterator
             EntityList::iterator it = entityCtnrReqList.begin();
             
             for (CORBA::ULong i = 0; i < len; i++)
             {
                // get entity from container list using iterator
                EntityElement entityElement = *it;
                    
                CdmwPlatformMngtEntity::Entity entity;

                entity.entity_name = CORBA::string_dup(entityElement.m_name.c_str());
                entity.entity_status = entityElement.m_data.m_entityState;
                entity.entity_info = 
                            CORBA::string_dup(entityElement.m_data.m_entityInfo.c_str());


                // store entity in sequence
                requested_entities[i] = entity;

                // increment iterator
                it++;
             }
         }
         
 
         // create the rest of list of entities
         std::list <CdmwPlatformMngtEntity::Entity_var> rest_of_entities;
          
         // get length of list      
         len = entityCtnrRestList.size();
                
         if (len > 0)
         {
             // initialize list iterator
             EntityList::iterator it = entityCtnrRestList.begin();
             
             for (CORBA::ULong i = 0; i < len; i++)
             {
                // get entity from container list using iterator
                EntityElement entityElement = *it;
                 
                // create entity var
             	CdmwPlatformMngtEntity::Entity_var entity = new CdmwPlatformMngtEntity::Entity;   

                entity->entity_name = CORBA::string_dup(entityElement.m_name.c_str());
                entity->entity_status = entityElement.m_data.m_entityState;
                entity->entity_info = 
                            CORBA::string_dup(entityElement.m_data.m_entityInfo.c_str());


                // store entity in list
                rest_of_entities.push_back(entity);

                // increment iterator
                it++;
             }
         }

         // create the EntityIterator with the rest of list
         CdmwPlatformMngtEntity::EntitiesIterator_var entities_iter =
                            EntitiesIterator_impl::create (rest_of_entities);

         // everything is OK, so return requested sequence and iterator
         nentities = requested_entities._retn();
         rest = entities_iter._retn();
        
     }
     catch(const std::bad_alloc &)
     {
         throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
     }
     catch(const OutOfResourcesException &)
     {
         throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
     }
     catch(const OutOfMemoryException &)
     {
         throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
     }
     catch(const AssertionFailedException &)
     {
         throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
     }
     catch(const InternalErrorException &)
     {
         throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
     }
     catch(const CORBA::Exception& e)
     {
         throw;
     }
   
}



EntityContainer_impl::EntityContainer* EntityContainer_impl::getActualContainer () const
{
     return m_pActualContainer;
}
        

} // End namespace PlatformMngt
} // End namespace Cdmw



