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


#ifndef INCL_PLATFORMMNGT_RWENTITY_CONTAINER_IMPL_HPP 
#define INCL_PLATFORMMNGT_RWENTITY_CONTAINER_IMPL_HPP 


#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntityContainer.skel.hpp"
#include "SystemMngt/platformelementlibrary/EntityContainer_impl.hpp"


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
*Purpose: RW Entity container implementation
*<p> 
*
*Features:
*<p> Thread safe
*
*@see  
*
*/
class RWEntityContainer_impl : virtual public POA_CdmwPlatformMngt::RWEntityContainer,
                               public EntityContainer_impl
                               
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        *
        *@param poa                the poa in charge of servants.
        *@param element_repository the element repository object reference
        *@param pActualContainer   actual container to associate
        * 
        */ 
        RWEntityContainer_impl(PortableServer::POA_ptr poa,
                               PlatformMngt::EntityContainer* pActualContainer,
                               CdmwPlatformMngt::RWElementRepository_ptr element_repository)
                throw();



        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~RWEntityContainer_impl();


        /**
		* Purpose:
		* <p> Add entity in container
		* 
		*@param entity_def  entity definition to add
		*
		*@exception CdmwPlatformMngt::EntityAlreadyExists
		*@exception CORBA::SystemException
		*
		*/ 
		CdmwPlatformMngt::Entity_ptr add_entity (
                const CdmwPlatformMngt::EntityDef & entity_def)
                throw(CdmwPlatformMngt::EntityAlreadyExists,CosPropertyService::MultipleExceptions,
		              CORBA::SystemException);


        
        /**
		* Purpose:
		* <p> Remove entity from container
		* 
		*@param entity_name name of entity to remove
		*
		*@exception CdmwPlatformMngt::EntityNotFound
		*@exception CORBA::SystemException
		*
		*/
		void remove_entity(const char* entity_name)
                throw(CdmwPlatformMngt::EntityNotFound,
                      CORBA::SystemException);
                      

    protected:


    private:

        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        RWEntityContainer_impl (const RWEntityContainer_impl& rhs)
                throw();



        /**
        * Purpose:
        * <p> Assignment operator
        * 
        */ 
        RWEntityContainer_impl&
        operator=(const RWEntityContainer_impl& rhs)
                throw();

   

}; // End class RWEntityContainer_impl 

}  // End namespace PlatformMngt
}  // End namespace Cdmw



#endif // INCL_PLATFORMMNGT_RWENTITY_CONTAINER_IMPL_HPP


