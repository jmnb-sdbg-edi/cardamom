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


#ifndef INCL_PLATFORMMNGT_ENTITY_CONTAINER_IMPL_HPP 
#define INCL_PLATFORMMNGT_ENTITY_CONTAINER_IMPL_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntityContainer.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp"
#include "SystemMngt/platformelementlibrary/EntityContainer.hpp"
#include "SystemMngt/platformlibrary/DeactivableServant_impl.hpp"



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
*Purpose: Entity container implementation
*<p> 
*
*Features:
*<p> Thread safe
*
*@see  
*
*/
class EntityContainer_impl : virtual public POA_CdmwPlatformMngt::EntityContainer,
                             virtual public PortableServer::RefCountServantBase,
                             public DeactivableServant_impl
                             
{

    public:

     /**
      * Purpose:
      * <p> Constructor
      *
      *@param poa                the poa in charge of servants.
      *@param pActualContainer   actual container to associate.
      *@param element_repository the element repository object reference
      * 
      */ 
      EntityContainer_impl(PortableServer::POA_ptr poa,
                           PlatformMngt::EntityContainer* pActualContainer,
                           CdmwPlatformMngt::RWElementRepository_ptr element_repository)
                throw();



     /**
      * Purpose:
      * <p> Destructor
      * 
      */ 
      virtual 
      ~EntityContainer_impl();


     /**
	  * Purpose:
	  * <p> Get number of entities from container
	  * 
	  *@return the number of entities
	  *
	  *@exception CORBA::SystemException
	  *
	  */ 
       CORBA::ULong get_number_of_entities()
                throw(CORBA::SystemException);

     /**
	  * Purpose:
	  * <p> Get all entities names from container
      * 
	  *@param how_many     nbr of entities to get
	  *@param entity_names sequence of entity names returned
	  *@param rest         iterator on entity names container returned
	  *
	  *@exception CORBA::SystemException
	  *
	  */
      void get_all_entity_names(CORBA::ULong how_many,
                                CdmwPlatformMngt::ElementNames_out entity_names,
                                CdmwPlatformMngt::ElementNamesIterator_out rest)
                throw(CORBA::SystemException);

     /**
	  * Purpose:
	  * <p> Get entity selected by its name from container
	  * 
	  *@param entity_name name of entity
	  *
	  *@return entity object
	  *
	  *@exception CdmwPlatformMngtEntity::EntityNotFound
	  *@exception CORBA::SystemException
	  */ 
      CdmwPlatformMngt::Entity_ptr get_entity(const char* entity_name)
                throw(CdmwPlatformMngt::EntityNotFound,
                      CORBA::SystemException);

     /**
      * Purpose:
	  * <p> Get sequence of entities from container
	  * 
	  *@param entity_names sequence of entities name
	  *@param nentities    sequence of entities info and status
	  *
	  *@return true if all returned entities are valid
	  *        false if at least one entitie is not valid
	  *
	  *@exception CORBA::SystemException
	  */ 
	  CORBA::Boolean get_entities(const CdmwPlatformMngt::ElementNames& entity_names,
                                  CdmwPlatformMngt::Entities_out nentities)
                 throw(CORBA::SystemException);
        


     /**
	  * Purpose:
	  * <p> Get all entities from container
	  * 
	  *@param how_many     nbr of entities to get
	  *@param nentities    sequence of entities returned
	  *@param rest         iterator on entities container returned
	  *
	  *@exception CORBA::SystemException
	  *
	  */
      void get_all_entities (CORBA::ULong how_many,
                             CdmwPlatformMngt::Entities_out nentities,
                             CdmwPlatformMngt::EntitiesIterator_out rest)
                throw(CORBA::SystemException);

     /**
      * Purpose:
      * <p> get the Entity container
      * 
      *@return the Entity container pointer
      *@exception
      */
      PlatformMngt::EntityContainer* getActualContainer () const;

   protected:

    /**
     * The repository of the managed elements.
     */
    CdmwPlatformMngt::RWElementRepository_var m_element_repository;
    
    
   private:
   
      /**
      * Purpose:
      * <p>  Copy constructor
      * 
      */ 
      EntityContainer_impl (const EntityContainer_impl& rhs)
                throw();



      /**
      * Purpose:
      * <p> Assignment operator
      * 
      */ 
      EntityContainer_impl&
      operator=(const EntityContainer_impl& rhs)
                throw();



      /**
      * Purpose:
      * <p> Reference to Entity Container
      * 
      */
      PlatformMngt::EntityContainer* m_pActualContainer;
        

}; // End class EntityContainer_impl 

}  // End namespace PlatformMngt
}  // End namespace Cdmw



#endif // INCL_PLATFORMMNGT_ENTITY_CONTAINER_IMPL_HPP


