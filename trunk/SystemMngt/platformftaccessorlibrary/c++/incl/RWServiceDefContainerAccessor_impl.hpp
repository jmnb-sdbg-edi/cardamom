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


#ifndef INCL_FTSYSTMNGT_RWSERVICEDEF_CONTAINER_ACCESSOR_IMPL_HPP 
#define INCL_FTSYSTMNGT_RWSERVICEDEF_CONTAINER_ACCESSOR_IMPL_HPP 

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

#include "SystemMngt/idllib/CdmwFTSystMngtServiceContainer.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtServiceContainer.stub.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtManagedElement.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp"

#include "SystemMngt/platformftaccessorlibrary/ServiceDefContainerAccessor_impl.hpp"


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
*Purpose: RW ServiceDef container accessor
*<p> 
*
*/
class RWServiceDefContainerAccessor_impl : 
                 virtual public POA_CdmwFTSystMngt::RWServiceDefContainerAccessor,
                 public ServiceDefContainerAccessor_impl
{

    public:

        /**
         *Purpose:
         *<p> Constructor.
         *
         */
        RWServiceDefContainerAccessor_impl(
            CdmwPlatformMngt::ElementRepository_ptr element_repository)
                throw();


        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~RWServiceDefContainerAccessor_impl();

        /**
         *Purpose:
          *<p>
          * Implements the
          * IDL:thalesgroup.com/CdmwFTSystMngt/ServiceDefContainerAccessor/add_service_def:1.0
          * operation
          */ 
		void add_service_def (
		     const char* target_element_path,
		     const CdmwPlatformMngtService::ServiceDef& service_def)
		        throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CdmwPlatformMngtService::ServiceAlreadyExists,
		              CORBA::SystemException);


        /**
         *Purpose:
          *<p>
          * Implements the
          * IDL:thalesgroup.com/CdmwFTSystMngt/ServiceDefContainerAccessor/remove_service_def:1.0
          * operation
          */
		void remove_service_def (
		     const char* target_element_path,
		     const char* service_name)
                throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CdmwPlatformMngtService::ServiceNotFound,
                      CORBA::SystemException);
                      
                      
        /**
         *Purpose:
          *<p>
          * Implements the
          * IDL:thalesgroup.com/CdmwFTSystMngt/ServiceDefContainerAccessor/remove_all_service_defs:1.0
          * operation
          */
		void remove_all_service_defs (
		     const char* target_element_path)
                throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CORBA::SystemException);
                      
                      
                      
    private:

        /**
        *Purpose:
        *<p> Get the RWServiceDefContainer object.
        */
        CdmwPlatformMngtService::RWServiceDefContainer_ptr get_rw_servicedef_container (const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound);

        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        RWServiceDefContainerAccessor_impl (const RWServiceDefContainerAccessor_impl& rhs)
                throw();



        /**
        * Purpose:
        * <p> Assignment operator
        * 
        */ 
        RWServiceDefContainerAccessor_impl&
        operator=(const RWServiceDefContainerAccessor_impl& rhs)
                throw();



};  // End class RWServiceDefContainerAccessor_impl 

}  // End namespace PlatformMngt
}  // End namespace Cdmw



#endif // INCL_FTSYSTMNGT_RWSERVICEDEF_CONTAINER_ACCESSOR_IMPL_HPP


