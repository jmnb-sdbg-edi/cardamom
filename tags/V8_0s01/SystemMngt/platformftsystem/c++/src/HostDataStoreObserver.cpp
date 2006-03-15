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

#include "platformftsystem/HostDataStoreObserver.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


// ----------------------------------------------------------------------
// HostDataStoreObserver class.
// ----------------------------------------------------------------------

HostDataStoreObserver::HostDataStoreObserver (System_impl* p_system)
  throw()
    : m_system (p_system)
{     
    // create the managed element storage home
    m_storage_home = new ManagedElementStorageHome(
          Configuration::Get_managed_element_datastore_id());   
}


HostDataStoreObserver::~HostDataStoreObserver()
  throw()
{
    delete m_storage_home;
}


void HostDataStoreObserver::on_insert(_Oid_in_type oid, _Data_in_type data)
  throw ()
{
    if (!Cdmw::PlatformMngt::Configuration::Is_primary())
    {
        try
        {
            std::cout << "<<HostDataStoreObserver::on_insert (" << oid <<")>>" << std::endl;
            
            // get the associated managed element record
            CdmwPlatformMngt::ManagedElementRecord_var element_data = 
                   m_storage_home->find_data_by_oid (oid);
                   
            // create the host definition
            CdmwPlatformMngt::HostDef_var host_def =
                       new CdmwPlatformMngt::HostDef();
    
            // Get the host's name
            host_def->host_name = element_data->name;
    
            // Set the application process's info
            host_def->platform_application_exe = data.platform_application_exe;
            
            // Set the monitoring's info
            host_def->daemon_monitoring_info = data.daemon_monitoring_info;
            
            // set host entities
            host_def->entities.length(0);
    
            // add host in system
            m_system->add_host (host_def.in()); 
        }
        catch (...)
        {
            // nothing to do
        }         
    }
    // else nothing to do because we are the primary and we are inserting the element 
    // into the datastore
}

void HostDataStoreObserver::on_update(_Oid_in_type oid, _Data_in_type data)
  throw ()
{

}

void HostDataStoreObserver::on_remove(_Oid_in_type oid) 
  throw ()
{
    if (!Cdmw::PlatformMngt::Configuration::Is_primary())
    {
        try
        {
            std::string host_name;
            std::string parent_path;
            
            // get the element name and parent path
            ManagedElementKey::Get_parent_path (oid, host_name,parent_path);
                   
            m_system->remove_host (host_name.c_str());
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

