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


#ifndef INCL_PLATFORMMNGT_RWSERVICEDEF_CONTAINER_IMPL_HPP 
#define INCL_PLATFORMMNGT_RWSERVICEDEF_CONTAINER_IMPL_HPP 


#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/platformlibrary/ServiceDefContainer_impl.hpp"


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
*Purpose: RW ServiceDef container implementation
*<p> 
*
*Features:
*<p> Thread safe
*
*@see  
*
*/
class RWServiceDefContainer_impl : virtual public ServiceDefContainer_impl,
                                   virtual public POA_CdmwPlatformMngtService::RWServiceDefContainer
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        *
        *@param poa              the poa in charge of servants.
        *@param pActualContainer actual container to associate
        *@param parentContainer  reference to parent ServiceDefContainer object
        * 
        */ 
        RWServiceDefContainer_impl(
                     PortableServer::POA_ptr poa,
                     ServiceDefContainer_impl::ServiceDefContainer* pActualContainer,
                     CdmwPlatformMngtService::ServiceDefContainer* pParentContainer =
                                      CdmwPlatformMngtService::ServiceDefContainer::_nil())
                throw();



        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~RWServiceDefContainer_impl()
                throw();


        /**
		* Purpose:
		* <p> Add service in container
		* 
		*@param service_def definition of service to add
		*
		*@exception CdmwPlatformMngtService::ServiceAlreadyExists
		*@exception CORBA::SystemException
		*
		*/ 
		void add_service_def (const CdmwPlatformMngtService::ServiceDef& service_def)
		        throw(CdmwPlatformMngtService::ServiceAlreadyExists,
		              CORBA::SystemException);


        
        /**
		* Purpose:
		* <p> Remove service from container
		* 
		*@param service_name name of service to remove
		*
		*@exception CdmwPlatformMngtService::ServiceNotFound
		*@exception CORBA::SystemException
		*
		*/
		void remove_service_def (const char* service_name)
                throw(CdmwPlatformMngtService::ServiceNotFound,
                      CORBA::SystemException);
                      
                      
                      
                      


    protected:


    private:

        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        RWServiceDefContainer_impl (const RWServiceDefContainer_impl& rhs)
                throw();



        /**
        * Purpose:
        * <p> Assignment operator
        * 
        */ 
        RWServiceDefContainer_impl&
        operator=(const RWServiceDefContainer_impl& rhs)
                throw();

   

};  // End class RWServiceDefContainer_impl 

}  // End namespace PlatformMngt
}  // End namespace Cdmw



#endif // INCL_PLATFORMMNGT_RWSERVICEDEF_CONTAINER_IMPL_HPP


