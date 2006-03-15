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

#include "SystemMngt/platformelementlibrary/EntityContainer_impl.hpp"
#include "SystemMngt/platformlibrary/EntitiesIterator_impl.hpp"
#include "SystemMngt/platformlibrary/ElementNamesIterator_impl.hpp"



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
EntityContainer_impl::EntityContainer_impl(PortableServer::POA_ptr poa,
                                           PlatformMngt::EntityContainer* pActualContainer,
                                           CdmwPlatformMngt::RWElementRepository_ptr element_repository)
        throw()
        : DeactivableServant_impl (poa),
          m_pActualContainer (pActualContainer)           
{
	m_element_repository = 
           CdmwPlatformMngt::RWElementRepository::_duplicate(element_repository);
}



//
// Destructor
// 
EntityContainer_impl::~EntityContainer_impl()
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
// Get entity selected by its name from container
//
CdmwPlatformMngt::Entity_ptr EntityContainer_impl::get_entity(const char* entity_name)
         throw(CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException)
{
	try
	{
	    Entity_impl* p_entity_impl;
	    
	    bool result = m_pActualContainer->findElement(entity_name, p_entity_impl);
	    
	    if (result == false)
	    {
	    	throw CdmwPlatformMngt::EntityNotFound();
	    }
	    else
	    {
           CdmwPlatformMngt::Entity_var entity = p_entity_impl->_this();
           
	       return entity._retn();
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
CORBA::Boolean EntityContainer_impl::get_entities(const CdmwPlatformMngt::ElementNames& entity_names,
                                                  CdmwPlatformMngt::Entities_out entities)
         throw(CORBA::SystemException)
{
	try
    {
    	  CORBA::Boolean result = true;
    	
        // create the requested sequence of entities
        CdmwPlatformMngt::Entities_var requested_entities = 
                                           new CdmwPlatformMngt::Entities;
                                           
        // set length of requested sequence
        size_t len = entity_names.length();
                     
        if (len > 0)
        {
             requested_entities->length(len);
        }
                                           
    	  for (unsigned int i=0 ; i < len ; i++)
        {
        	   Entity_impl* p_entity_impl;
            CdmwPlatformMngt::Entity_var entity =
               CdmwPlatformMngt::Entity::_nil();

            // get entity name from request list
            #if CDMW_ORB_VDR == orbacus
            std::string name = entity_names[i];
            #else
            std::string name = (entity_names[i]).in();
            #endif
            
            bool findResult = 
               m_pActualContainer->findElement(name, p_entity_impl);
            if (findResult == false)
            {
            	result = false;
            }
    
            else
            {
               // get Entity object reference
               entity = p_entity_impl->_this();
	         }
            
            // stock entity in the sequence           
            requested_entities[i] = entity._retn();
        }
        
        // everything is OK, so return requested entity sequence
        entities = requested_entities._retn();
        
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
                                                CdmwPlatformMngt::ElementNames_out entity_names,
                                                CdmwPlatformMngt::ElementNamesIterator_out rest)
         throw(CORBA::SystemException)
{
	try
    {
    	   // list of request entitie names from container
         PlatformMngt::EntityContainer::EntityNameList entityCtnrReqList;
         
         // list of rest of entitie names from container
         PlatformMngt::EntityContainer::EntityNameList entityCtnrRestList;

         // get list of entitie names from container
         m_pActualContainer->listName(how_many, 
                                      entityCtnrReqList, 
                                      entityCtnrRestList);

         // create the requested sequence of entitie names
         CdmwPlatformMngt::ElementNames_var requested_entities = 
            new CdmwPlatformMngt::ElementNames;
         
         // get length of list                                    
         size_t len = entityCtnrReqList.size();
                
         if (len > 0)
         {
         	 // set length of sequence
             requested_entities->length(len);

             // initialize list iterator
             PlatformMngt::EntityContainer::EntityNameList::iterator it = 
                          entityCtnrReqList.begin();
             
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
         std::list <CdmwPlatformMngt::ElementName_var> rest_of_entities;
          
         // get length of list      
         len = entityCtnrRestList.size();
                
         if (len > 0)
         {
             // initialize list iterator
             PlatformMngt::EntityContainer::EntityNameList::iterator it = 
                          entityCtnrRestList.begin();
             
             for (CORBA::ULong i = 0; i < len; i++)
             {
             	// get entity name from container list using iterator
                CdmwPlatformMngt::ElementName_var entity_name = 
                   CORBA::string_dup((*it).c_str());

                // store entity name in list
                rest_of_entities.push_back(entity_name);

                // increment iterator
                it++;
             }
         }

         // create the ElementNamesIterator with the rest of list
         CdmwPlatformMngt::ElementNamesIterator_var entities_iter = 
            ElementNamesIterator_impl::create (rest_of_entities);

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
void 
EntityContainer_impl::get_all_entities(CORBA::ULong how_many,
                                       CdmwPlatformMngt::Entities_out entities,
                                       CdmwPlatformMngt::EntitiesIterator_out rest)
         throw(CORBA::SystemException)
{   	
    try
    {       
         // list of request entities from container
         PlatformMngt::EntityContainer::EntityList entityCtnrReqList;
         
         // list of rest of entities from container
         PlatformMngt::EntityContainer::EntityList entityCtnrRestList;

         // get list of entities from container
         m_pActualContainer->list(how_many, 
                                  entityCtnrReqList, 
                                  entityCtnrRestList);

         // create the requested sequence of entities
         CdmwPlatformMngt::Entities_var requested_entities = 
            new CdmwPlatformMngt::Entities;
         
         // get length of list                                    
         size_t len = entityCtnrReqList.size();
                
         if (len > 0)
         {
         	 // set length of sequence
             requested_entities->length(len);

             // initialize list iterator
             PlatformMngt::EntityContainer::EntityList::iterator it = 
                          entityCtnrReqList.begin();
             
             for (CORBA::ULong i = 0; i < len; i++)
             {
                // get entity from container list using iterator
                // and convert to Entity object reference
                CdmwPlatformMngt::Entity_var entity = (*it)->_this();

                // store entity in sequence
                requested_entities[i] = entity._retn();

                // increment iterator
                it++;
             }
         }
         
 
         // create the rest of list of entities
         std::list <CdmwPlatformMngt::Entity_var> rest_of_entities;
          
         // get length of list      
         len = entityCtnrRestList.size();
                
         if (len > 0)
         {
             // initialize list iterator
             PlatformMngt::EntityContainer::EntityList::iterator it = 
                          entityCtnrRestList.begin();
             
             for (CORBA::ULong i = 0; i < len; i++)
             {
                // get entity from container list using iterator
                // and convert to Entity object reference
             	 CdmwPlatformMngt::Entity_var entity = (*it)->_this();

                // store entity in list
                rest_of_entities.push_back(entity);

                // increment iterator
                it++;
             }
         }

         // create the EntityIterator with the rest of list
         CdmwPlatformMngt::EntitiesIterator_var entities_iter =
                            EntitiesIterator_impl::create (rest_of_entities);

         // everything is OK, so return requested sequence and iterator
         entities = requested_entities._retn();
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



PlatformMngt::EntityContainer* EntityContainer_impl::getActualContainer () const
{
     return m_pActualContainer;
}
        

} // End namespace PlatformMngt
} // End namespace Cdmw



