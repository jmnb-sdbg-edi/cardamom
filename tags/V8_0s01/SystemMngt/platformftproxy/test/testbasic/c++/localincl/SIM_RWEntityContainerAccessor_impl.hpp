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


#ifndef INCL_PLATFORMMNGT_PROXY_SIM_RWENTITY_CONTAINER_IMPL_HPP 
#define INCL_PLATFORMMNGT_PROXY_SIM_RWENTITY_CONTAINER_IMPL_HPP 

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

#include "SystemMngt/idllib/CdmwFTSystMngtEntityContainer.skel.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp"

#include "testbasic/SIM_EntityContainerAccessor_impl.hpp"


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

namespace Proxy
{

/**
*Purpose: RW Entity container accessor
*<p> 
*
*/
class SIM_RWEntityContainerAccessor_impl : 
                 virtual public POA_CdmwFTSystMngt::RWEntityContainerAccessor,
                 public SIM_EntityContainerAccessor_impl
                 
{

    public:

        /**
         *Purpose:
         *<p> Constructor.
         *
         */
        SIM_RWEntityContainerAccessor_impl()
                throw();

        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~SIM_RWEntityContainerAccessor_impl();


        /**
         *Purpose:
          *<p>
          * Implements the
          * IDL:thalesgroup.com/CdmwFTSystMngt/EntityContainerAccessor/add_entity:1.0
          * operation
          */  
        char* add_entity (
                const char* target_element_path,
                const CdmwPlatformMngt::EntityDef & entity_def)
		     throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::EntityAlreadyExists,
		           CORBA::SystemException);

   

        /**
         *Purpose:
          *<p>
          * Implements the
          * IDL:thalesgroup.com/CdmwFTSystMngt/EntityContainerAccessor/remove_entity:1.0
          * operation
          */
		void remove_entity(
		        const char* target_element_path,
		        const char* entity_name)
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::EntityNotFound,
                   CORBA::SystemException);
                      

    protected:

        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        SIM_RWEntityContainerAccessor_impl (const SIM_RWEntityContainerAccessor_impl& rhs)
                throw();



        /**
        * Purpose:
        * <p> Assignment operator
        * 
        */ 
        SIM_RWEntityContainerAccessor_impl&
        operator=(const SIM_RWEntityContainerAccessor_impl& rhs)
                throw();
                
    private:

   

}; // End class SIM_RWEntityContainerAccessor_impl 

}  // End namespace Proxy
}  // End namespace PlatformMngt
}  // End namespace Cdmw



#endif // INCL_PLATFORMMNGT_PROXY_SIM_RWENTITY_CONTAINER_IMPL_HPP


