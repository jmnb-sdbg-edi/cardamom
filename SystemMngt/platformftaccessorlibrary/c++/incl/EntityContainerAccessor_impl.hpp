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


#ifndef INCL_FTSYSTMNGT_ENTITY_CONTAINER_IMPL_HPP 
#define INCL_FTSYSTMNGT_ENTITY_CONTAINER_IMPL_HPP 

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

#include "SystemMngt/idllib/CdmwFTSystMngtEntityContainer.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntityContainer.stub.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtManagedElement.stub.hpp"
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
*Purpose: Entity container accessor
*<p> 
*
*/
class EntityContainerAccessor_impl : 
                 virtual public POA_CdmwFTSystMngt::EntityContainerAccessor,
                 virtual public PortableServer::RefCountServantBase
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
        */ 
        EntityContainerAccessor_impl(
            CdmwPlatformMngt::ElementRepository_ptr element_repository)
                throw();



        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~EntityContainerAccessor_impl();

      
        /**
         *Purpose:
          *<p>
          * Implements the
          * IDL:thalesgroup.com/CdmwFTSystMngt/EntityContainerAccessor/get_number_of_entities:1.0
          * operation
          */ 
		CORBA::ULong get_number_of_entities(const char* target_element_path)
                throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CORBA::SystemException);


        /**
         *Purpose:
          *<p>
          * Implements the
          * IDL:thalesgroup.com/CdmwFTSystMngt/EntityContainerAccessor/get_all_entity_names:1.0
          * operation
          */
		void get_all_entity_names (const char* target_element_path,
		                           CORBA::ULong how_many,
                                   CdmwPlatformMngt::ElementNames_out entity_names,
                                   CdmwPlatformMngt::ElementNamesIterator_out rest)
                throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CORBA::SystemException);


        /**
         *Purpose:
          *<p>
          * Implements the
          * IDL:thalesgroup.com/CdmwFTSystMngt/EntityContainerAccessor/get_entity:1.0
          * operation
          */
		char* get_entity(const char* target_element_path,
		                 const char* entity_name)
                throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CdmwPlatformMngt::EntityNotFound,
                      CORBA::SystemException);
                      
        /**
         *Purpose:
          *<p>
          * Implements the
          * IDL:thalesgroup.com/CdmwFTSystMngt/EntityContainerAccessor/get_entities:1.0
          * operation
          */
		CORBA::Boolean get_entities (
		                const char* target_element_path,
                        const CdmwPlatformMngt::ElementNames& entity_names,
                        CdmwPlatformMngt::ElementPaths_out entities)
                throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CORBA::SystemException);

        /**
         *Purpose:
          *<p>
          * Implements the
          * IDL:thalesgroup.com/CdmwFTSystMngt/EntityContainerAccessor/get_all_entities:1.0
          * operation
          */
		void get_all_entities(const char* target_element_path,
		                      CORBA::ULong how_many,
                              CdmwPlatformMngt::ElementPaths_out entities,
                              CdmwPlatformMngt::ElementPathsIterator_out rest)
                throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CORBA::SystemException);

      
    protected:
    
        /**
         * The repository of the managed elements.
         */
        CdmwPlatformMngt::ElementRepository_var m_entity_element_repository;
    
        
    private:
    
        /**
         *Purpose:
         *<p> Get the EntityContainer object.
         */
        CdmwPlatformMngt::EntityContainer_ptr get_entity_container (const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound);
              
              
        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        EntityContainerAccessor_impl (const EntityContainerAccessor_impl& rhs)
                throw();



        /**
        * Purpose:
        * <p> Assignment operator
        * 
        */ 
        EntityContainerAccessor_impl&
        operator=(const EntityContainerAccessor_impl& rhs)
                throw();

    

}; // End class EntityContainerAccessor_impl 

}  // End namespace PlatformMngt
}  // End namespace Cdmw



#endif // INCL_FTSYSTMNGT_ENTITY_CONTAINER_IMPL_HPP


