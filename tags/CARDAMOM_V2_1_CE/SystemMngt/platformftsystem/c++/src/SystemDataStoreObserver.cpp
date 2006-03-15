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
 
#include "platformftsystem/SystemDataStoreObserver.hpp"
#include "platformlibrary/Configuration.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


// ----------------------------------------------------------------------
// SystemDataStoreObserver class.
// ----------------------------------------------------------------------

SystemDataStoreObserver::SystemDataStoreObserver (System_impl* p_system)
  throw()
    : m_system (p_system)
{     
    // create the managed element storage home
    m_storage_home = new ManagedElementStorageHome(Configuration::Get_managed_element_datastore_id());   
}


SystemDataStoreObserver::~SystemDataStoreObserver()
  throw()
{
    delete m_storage_home;
}


void SystemDataStoreObserver::on_insert(_Oid_in_type oid, _Data_in_type data)
  throw ()
{
    if (!Cdmw::PlatformMngt::Configuration::Is_primary())
    {
        try
        {
            std::cout << "<<SystemDataStoreObserver::on_insert (" << oid <<")>>" << std::endl;
            
            // get the associated managed element record
            CdmwPlatformMngt::ManagedElementRecord_var element_data = 
                   m_storage_home->find_data_by_oid (oid);
                   
            // create the system definition
            CdmwPlatformMngt::SystemDef_var system_def =
                       new CdmwPlatformMngt::SystemDef();
        
            system_def->system_name = element_data->name;
            system_def->system_port = data.system_port;
            system_def->entities.length(0);   
            system_def->service_definitions.length(0);
            system_def->hosts.length(0);
        
            m_system->define (system_def);
        }
        catch (...)
        {
            // nothing to do
        }    
    }
    // else nothing to do because we are the primary and we are inserting the element 
    // into the datastore
}

void SystemDataStoreObserver::on_update(_Oid_in_type oid, _Data_in_type data)
  throw ()
{

}

void SystemDataStoreObserver::on_remove(_Oid_in_type oid) 
  throw ()
{

}

     

} // End namespace PlatformMngt
} // End namespace Cdmw

