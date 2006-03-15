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


#ifndef INCL_PLATFORMMNGT_ENTITY_CONTAINER_HPP 
#define INCL_PLATFORMMNGT_ENTITY_CONTAINER_HPP 


#include "Foundation/orbsupport/CORBA.hpp"
#include "platformlibrary/SimpleContainer.hpp"
#include "platformelementlibrary/Entity_impl.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp"

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

/**
*Purpose: Entity container
*<p> 
*
*Features:
*<p> Thread safe
*
*@see  
*
*/
class EntityContainer : public SimpleContainer<Entity_impl*>
{

    public:
        
        /**
        * Define the EntityNameList
        */
        typedef SimpleContainer<Entity_impl*>::ElementNameList EntityNameList;
    
        /**
        * Define the EntityList
        */
        typedef SimpleContainer<Entity_impl*>::ElementList EntityList;


        /**
        * Purpose:
        * <p> Constructor
        *
        *@param poa                the poa in charge of servants
        *@param element_repository the element repository object reference
        *@param entity_parent_path the path of the parent of the entities        
        *
        *@exception BadParameterException if the parameter is invalid.
        *
        */ 
        EntityContainer (PortableServer::POA_ptr poa,
                         CdmwPlatformMngt::RWElementRepository_ptr element_repository,
                         const char* entity_parent_path)
                throw(BadParameterException);


        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~EntityContainer()
                throw();
                
                
        /**
        * Purpose:
        * <p> Set the parent path for all entities of container
        *     (to be done before addEntity() if parent path not set in constructor)
        * 
        */        
        void setParentPath (const char* entity_parent_path)
                 throw(BadParameterException);



        /**
         *Purpose:
         *<p> Actally adds a entity to the container.
         *
         *@param entity_def the definition of the entity to add.
         *
         *@return the reference of the added entity
         *
         *@exception EntityAlreadyExists if the entity name has already been 
         *              added into the container.
         */
        CdmwPlatformMngt::Entity_ptr 
              addEntity(const CdmwPlatformMngt::EntityDef& entity_def)
                throw (CdmwPlatformMngt::EntityAlreadyExists,CosPropertyService::MultipleExceptions,
                       CORBA::SystemException);

        /**
         *Purpose:
         *<p> Removes a entity from the container.
         *
         *@param entity_name the name identifying the entity to remove.
         *
         *@exception EntityNotFound if entity_name doesn't denote
         *           an existing entity.
         */
        void removeEntity(const char* entity_name)
                throw (CdmwPlatformMngt::EntityNotFound,
                       CORBA::SystemException);
                  


        /**
         *Purpose:
         *<p> Get the snapshot of entities.
         *
         *@return  sequence of EntitySnapshots.
         *
         *@exception Cdmw::OutOfResourcesException
         *
         */
        CdmwPlatformMngt::EntitiesSnapshot* getSnapshot()
                 throw (Cdmw::OutOfResourcesException);
        
        
        
    protected:


    private:

        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        EntityContainer (const EntityContainer& rhs)
                throw();



        /**
        * Purpose:
        * <p> Assignment operator
        * 
        */ 
        EntityContainer&
        operator=(const EntityContainer& rhs)
                throw();
 
        /**
        * The specific POA in charge of servants.
        */
        PortableServer::POA_var m_poa;
        
        /**
        * The full name of the parent.
        */
        CdmwPlatformMngt::ElementPath_var m_parent;
        
        /**
        * The repository of the managed elements.
        */
        CdmwPlatformMngt::RWElementRepository_var m_element_repository;

};  // End class EntityContainer 

}  // End namespace PlatformMngt
}  // End namespace Cdmw



#endif // INCL_PLATFORMMNGT_ENTITY_CONTAINER_HPP


