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
 
#include "SystemMngt/platformlibrary/ManagedElementKey.hpp"

#include "platformftsystem/ServiceDataStoreObserver.hpp"
#include "platformlibrary/Configuration.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtManagedElement.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtServiceContainer.stub.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


// ----------------------------------------------------------------------
// ServiceDataStoreObserver class.
// ----------------------------------------------------------------------

ServiceDataStoreObserver::ServiceDataStoreObserver (CdmwPlatformMngt::ElementRepository_ptr element_repository)
  throw()
{  
    m_element_repository = 
           CdmwPlatformMngt::ElementRepository::_duplicate(element_repository); 
}


ServiceDataStoreObserver::~ServiceDataStoreObserver()
  throw()
{
}


void ServiceDataStoreObserver::on_insert(_Oid_in_type oid, _Data_in_type data)
  throw ()
{
    if (!Cdmw::PlatformMngt::Configuration::Is_primary())
    {
        try
        {    
            std::cout << "<<ServiceDataStoreObserver::on_insert (" << oid <<")>>" << std::endl;
            
            // get the parent of ServiceDefs           
            std::string element_path;
            std::string parent_path;
            
            // get the object reference
            CdmwPlatformMngt::ManagedElement_var element_obj =
                        m_element_repository->get_element(oid.c_str());
            
            // convert to service container           
            CdmwPlatformMngtService::RWServiceDefContainer_var service_container_obj;
            
            service_container_obj = 
               CdmwPlatformMngtService::RWServiceDefContainer::_narrow (element_obj.in());
            
            // get the number of services in record
            unsigned int serviceCount = data.length();
            
            for (unsigned int serviceIndex=0;
                 serviceIndex < serviceCount;
                 serviceIndex++)
            {
                 // get service definition (ServiceDef is a structure, a reference on the
                 // structure is returned by the [] operator)
                 const CdmwPlatformMngtService::ServiceDef& service_def = data[serviceIndex];
                                          
                 try
                 {              
                     // add the service 
                     service_container_obj->add_service_def (service_def);
                 }
                 catch(const CdmwPlatformMngtService::ServiceAlreadyExists&)
                 {
                     // nothing to do
                 }
            }
       }
       catch (...)
       {
           // nothing to do
       }    
   }
    // else nothing to do because we are the primary and we are inserting the element 
    // into the datastore
}

void ServiceDataStoreObserver::on_update(_Oid_in_type oid, _Data_in_type data)
  throw ()
{
    if (!Cdmw::PlatformMngt::Configuration::Is_primary())
    {
        try
        {
            // get the object reference
            CdmwPlatformMngt::ManagedElement_var element_obj =
                        m_element_repository->get_element(oid.c_str());
             
            // convert to service container           
            CdmwPlatformMngtService::RWServiceDefContainer_var service_container_obj;
            
            service_container_obj = 
               CdmwPlatformMngtService::RWServiceDefContainer::_narrow (element_obj.in());
            
               
            // first remove all services 
            service_container_obj->remove_all_service_defs ();
            
            // then insert the new services
            
            // get the number of services in record
            unsigned int serviceCount = data.length();
            
            for (unsigned int serviceIndex=0;
                 serviceIndex < serviceCount;
                 serviceIndex++)
            {
                // get service definition (ServiceDef is a structure, a reference on the
                // structure is returned by the [] operator)
                const CdmwPlatformMngtService::ServiceDef& service_def = data[serviceIndex];
                                          
                try
                {              
                    // add the service 
                    service_container_obj->add_service_def (service_def);
                }
                catch(const CdmwPlatformMngtService::ServiceAlreadyExists&)
                {
                    // nothing to do
                }
            }
       }
       catch (...)
       {
           // nothing to do
       }    
   }
    // else nothing to do because we are the primary and we are inserting the element 
    // into the datastore
}

void ServiceDataStoreObserver::on_remove(_Oid_in_type oid) 
  throw ()
{
    if (!Cdmw::PlatformMngt::Configuration::Is_primary())
    {
        try
        {
            // get the object reference
            CdmwPlatformMngt::ManagedElement_var element_obj =
                        m_element_repository->get_element(oid.c_str());
             
            // convert to service container           
            CdmwPlatformMngtService::RWServiceDefContainer_var service_container_obj;
            
            service_container_obj = 
               CdmwPlatformMngtService::RWServiceDefContainer::_narrow (element_obj.in());
            
               
            // remove all services 
            service_container_obj->remove_all_service_defs ();
       }
       catch (...)
       {
           // nothing to do
       }    
   }
    // else nothing to do because we are the primary and we are inserting the element 
    // into the datastore
}

     

} // End namespace PlatformMngt
} // End namespace Cdmw

