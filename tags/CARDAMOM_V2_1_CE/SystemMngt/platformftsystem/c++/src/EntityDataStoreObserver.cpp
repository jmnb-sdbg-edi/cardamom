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
#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntityContainer.stub.hpp"

#include "platformftsystem/EntityDataStoreObserver.hpp"



namespace Cdmw
{
namespace PlatformMngt
{


// ----------------------------------------------------------------------
// EntityDataStoreObserver class.
// ----------------------------------------------------------------------

EntityDataStoreObserver::EntityDataStoreObserver (
      CdmwPlatformMngt::ElementRepository_ptr element_repository)
  throw()
{  
    m_element_repository = 
           CdmwPlatformMngt::ElementRepository::_duplicate(element_repository);
              
    // create the managed element storage home
    m_storage_home = new ManagedElementStorageHome(
          Configuration::Get_managed_element_datastore_id());   
}


EntityDataStoreObserver::~EntityDataStoreObserver()
  throw()
{
    delete m_storage_home;
}


void EntityDataStoreObserver::on_insert(_Oid_in_type oid, _Data_in_type data)
  throw ()
{
    if (!Cdmw::PlatformMngt::Configuration::Is_primary())
    {
        try
        {
            std::cout << "<<EntityDataStoreObserver::on_insert (" << oid <<")>>" << std::endl;
            
            // get the associated managed element record
            CdmwPlatformMngt::ManagedElementRecord_var element_data = 
                   m_storage_home->find_data_by_oid (oid);
        
            // get the parent of entity           
            std::string entity_name;
            std::string parent_path;
            
            // get the element name and parent path
            ManagedElementKey::Get_parent_path (oid, entity_name, parent_path);
                  
            // get the parent object reference
            CdmwPlatformMngt::ManagedElement_var element_obj =
                        m_element_repository->get_element(parent_path.c_str());
           
            // convert to entity container      
            CdmwPlatformMngt::RWEntityContainer_var entity_container_obj;
            
            entity_container_obj = 
               CdmwPlatformMngt::RWEntityContainer::_narrow (element_obj.in());
           
            // Create the entity definition      
            CdmwPlatformMngt::EntityDef_var entity_def =
                    new CdmwPlatformMngt::EntityDef();
    
            // Get the entity's name
            entity_def->entity_name = element_data->name;
            entity_def->status_values = data.status_values;
            entity_def->properties.length(0);
            entity_def->commands.length(0);
        
            // add the entity
            entity_container_obj->add_entity (entity_def);             
        }
        catch (...)
        {
            // nothing to do
        }    
    }
    // else nothing to do because we are the primary and we are inserting the element 
    // into the datastore
}

void EntityDataStoreObserver::on_update(_Oid_in_type oid, _Data_in_type data)
  throw ()
{

}

void EntityDataStoreObserver::on_remove(_Oid_in_type oid) 
  throw ()
{
    if (!Cdmw::PlatformMngt::Configuration::Is_primary())
    {
        try
        {
            // get the parent of entity           
            std::string entity_name;
            std::string parent_path;
            
            // get the element name and parent path
            ManagedElementKey::Get_parent_path (oid, entity_name, parent_path);
    
            // get the parent object reference
            CdmwPlatformMngt::ManagedElement_var element_obj =
                        m_element_repository->get_element(parent_path.c_str());
                        
             // convert to entity container      
            CdmwPlatformMngt::RWEntityContainer_var entity_container_obj;
            
            entity_container_obj = 
               CdmwPlatformMngt::RWEntityContainer::_narrow (element_obj.in());
    
            // remove entity
            entity_container_obj->remove_entity (entity_name.c_str());
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

