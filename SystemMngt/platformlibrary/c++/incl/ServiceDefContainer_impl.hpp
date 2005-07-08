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


#ifndef INCL_PLATFORMMNGT_SERVICEDEF_CONTAINER_IMPL_HPP 
#define INCL_PLATFORMMNGT_SERVICEDEF_CONTAINER_IMPL_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtServiceContainer.skel.hpp"
#include "SystemMngt/platformlibrary/Container.hpp"
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
*Purpose: Service Def container implementation
*<p> 
*
*Features:
*<p> Thread safe
*
*@see  
*
*/
class ServiceDefContainer_impl : virtual public DeactivableServant_impl,
                                 virtual public POA_CdmwPlatformMngtService::ServiceDefContainer,
                                 virtual public PortableServer::RefCountServantBase
{

    public:

        /**
        * Define the ServiceDefContainer from Container template
        */
        typedef Container<CdmwPlatformMngtService::ServiceProviderID> ServiceDefContainer;
        
        

        /**
        * Purpose:
        * <p> Constructor
        *
        *@param poa              the poa in charge of servants.
        *@param pActualContainer actual container to associate
        *@param parentContainer  reference to parent ServiceDefContainer object
        * 
        */ 
        ServiceDefContainer_impl(PortableServer::POA_ptr poa,
                                 ServiceDefContainer* pActualContainer,
                                 CdmwPlatformMngtService::ServiceDefContainer* pParentContainer =
                                   CdmwPlatformMngtService::ServiceDefContainer::_nil())
                throw();



        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~ServiceDefContainer_impl()
                throw();


        

        /**
		* Purpose:
		* <p> Get processId of service selected by its name from container
		* 
		*@param service_name name of service
		*
		*@return ServiceProviderID of service
		*
		*@exception CdmwPlatformMngtService::ServiceNotFound
		*@exception CORBA::SystemException
		*/ 
        CdmwPlatformMngtService::ServiceProviderID* get_service_def (const char* service_name)                                       
                throw(CdmwPlatformMngtService::ServiceNotFound,
                      CORBA::SystemException);

        


    protected:


        /**
        * Purpose:
        * <p> get the ServiceDef container
        * 
        *@return the ServiceDef container pointer
        *@exception
        */
        ServiceDefContainer* getActualContainer () const;
        

    private:

        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        ServiceDefContainer_impl (const ServiceDefContainer_impl& rhs)
                throw();



        /**
        * Purpose:
        * <p> Assignment operator
        * 
        */ 
        ServiceDefContainer_impl&
        operator=(const ServiceDefContainer_impl& rhs)
                throw();



        /**
        * Purpose:
        * <p> Reference to ServiceDef Container
        * 
        */
        ServiceDefContainer* m_pActualContainer;
        
        
        
        /**
        * Purpose:
        * <p> Reference to parent ServiceDefContainer object
        * 
        */
        CdmwPlatformMngtService::ServiceDefContainer_var m_parentContainer;
        
        

};  // End class ServiceDefContainer_impl 

}  // End namespace PlatformMngt
}  // End namespace Cdmw



#endif // INCL_PLATFORMMNGT_SERVICEDEF_CONTAINER_IMPL_HPP


