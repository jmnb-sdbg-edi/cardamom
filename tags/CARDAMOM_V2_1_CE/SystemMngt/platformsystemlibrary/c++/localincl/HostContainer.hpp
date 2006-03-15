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


#ifndef INCL_PLATFORMMNGT_HOST_CONTAINER_HPP 
#define INCL_PLATFORMMNGT_HOST_CONTAINER_HPP 


#include "Foundation/orbsupport/CORBA.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp"

#include "SystemMngt/platformlibrary/SimpleContainer.hpp"

#include "platformsystemlibrary/Host_impl.hpp"


#ifdef CDMW_USE_FAULTTOLERANCE
#include "FaultTolerance/idllib/CdmwFTFaultDetector.stub.hpp"
#endif

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
	
class System_impl;

/**
*Purpose: Host container
*<p> 
*
*Features:
*<p> Thread safe
*
*@see  
*
*/
class HostContainer : public SimpleContainer<Host_impl*>
{

    public:
        
        /**
        * Define the HostNameList
        */
        typedef SimpleContainer<Host_impl*>::ElementNameList HostNameList;
    
        /**
        * Define the HostList
        */
        typedef SimpleContainer<Host_impl*>::ElementList HostList;


        /**
        * Purpose:
        * <p> Constructor
        *
        *@param system the system responsible for the host
        *@param poa    the poa in charge of servants
        *@param element_repository the element repository object reference
        *
        *@exception BadParameterException if the parameter is invalid.
        *
        */ 
        HostContainer (System_impl* system, 
                       PortableServer::POA_ptr poa,
                       CdmwPlatformMngt::RWElementRepository_ptr element_repository)
                throw(BadParameterException, CosPropertyService::MultipleExceptions);


        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~HostContainer()
                throw();

#ifdef CDMW_USE_FAULTTOLERANCE

        /**
        *Purpose:
        *<p> Set the fault detector reference.
        *
        */
        void setFaultDetector (CdmwFT::FaultManagement::FaultDetector_ptr faultDetector);
        
#endif


        /**
         *Purpose:
         *<p> Actally adds a host to the system.
         *
         *@param host_def the definition of the host to add.
         *
         *@return the reference of the added host
         *
         *@exception HostAlreadyExists if the host name is already used
         *           within the system.
         *@exception DuplicateEntity if entity is duplicated
         *@exception CosPropertyService::MultipleExceptions if multiple exception in property
         */
        CdmwPlatformMngt::Host_ptr 
              addHost(const CdmwPlatformMngt::HostDef& host_def)
                throw (CdmwPlatformMngt::HostAlreadyExists,
                       CdmwPlatformMngt::DuplicateEntity,
                       CosPropertyService::MultipleExceptions,
                       CORBA::SystemException);

        /**
         *Purpose:
         *<p> Removes a host from the system.
         *
         *@param host_name the name identifying the host to remove.
         *
         *@exception HostNotFound if host_name doesn't denote
         *           an existing host.
         */
        void removeHost(const char* host_name)
                throw (CdmwPlatformMngt::HostNotFound,
                       CORBA::SystemException);
                  
        /**
         *Purpose:
         *<p> Get a host implementation from the system.
         *
         *@param host_name the name identifying the host.
         *
         *@exception HostNotFound if host_name doesn't denote
         *           an existing host.
         */
        Host_impl* getHost(const char* host_name)
                throw (CdmwPlatformMngt::HostNotFound,
                       CORBA::SystemException);

        /**
         *Purpose:
         *<p> Get the snapshot of hosts.
         *
         *@return  sequence of HostSnapshots.
         *
         *@exception Cdmw::OutOfResourcesException
         *
         */
        CdmwPlatformMngt::HostsSnapshot* getSnapshot()
                 throw (Cdmw::OutOfResourcesException);
        
        
        
    protected:


    private:

        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        HostContainer (const HostContainer& rhs)
                throw();



        /**
        * Purpose:
        * <p> Assignment operator
        * 
        */ 
        HostContainer&
        operator=(const HostContainer& rhs)
                throw();
 
        /**
        * The system responsible for the entity.
        */
        System_impl* m_system;
    
        /**
        * The specific POA in charge of servants.
        */
        PortableServer::POA_var m_poa;  

        /**
        * The repository of the managed elements.
        */
        CdmwPlatformMngt::RWElementRepository_var m_element_repository;

#ifdef CDMW_USE_FAULTTOLERANCE

        /**
         * The fault detector
         **/
        CdmwFT::FaultManagement::FaultDetector_var m_faultDetector;  

#endif
        

};  // End class HostContainer 

}  // End namespace PlatformMngt
}  // End namespace Cdmw



#endif // INCL_PLATFORMMNGT_HOST_CONTAINER_HPP


