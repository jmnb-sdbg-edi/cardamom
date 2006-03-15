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
#include "SystemMngt/platformvaluetypes/ProcessDef_impl.hpp"

#include "platformftsystem/ProcessDataStoreObserver.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtApplication.stub.hpp"

namespace Cdmw
{
namespace PlatformMngt
{


// ----------------------------------------------------------------------
// ProcessDataStoreObserver class.
// ----------------------------------------------------------------------

ProcessDataStoreObserver::ProcessDataStoreObserver (CdmwPlatformMngt::ElementRepository_ptr element_repository)
  throw()
{  
    m_element_repository = 
           CdmwPlatformMngt::ElementRepository::_duplicate(element_repository);
              
    // create the managed element storage home
    m_storage_home = new ManagedElementStorageHome(
          Configuration::Get_managed_element_datastore_id());   
}


ProcessDataStoreObserver::~ProcessDataStoreObserver()
  throw()
{
    delete m_storage_home;
}


void ProcessDataStoreObserver::on_insert(_Oid_in_type oid, _Data_in_type data)
  throw ()
{
    if (!Cdmw::PlatformMngt::Configuration::Is_primary())
    {
         try
         {
            std::cout << "<<ProcessDataStoreObserver::on_insert (" << oid <<")>>" << std::endl;
            
             // get the application parent of process           
             std::string process_name;
             std::string host_name;
             std::string parent_path;
             
             // get the element name and parent path
             ManagedElementKey::Get_process_parent_path (oid, process_name, 
                                                         host_name, parent_path);
         
             // get the application object reference
             CdmwPlatformMngt::ManagedElement_var element_obj =
                         m_element_repository->get_element(parent_path.c_str());
                         
             CdmwPlatformMngt::Application_var application_obj =
                         CdmwPlatformMngt::Application::_narrow (element_obj.in());
                    
                    
             // Create the process definition
             CdmwPlatformMngt::ProcessDef_var process_def;
     
             if (data.type == CdmwPlatformMngt::UNMANAGED_PROCESS)
             {
                 #if CDMW_ORB_VDR == tao
                 
                 PlatformMngt::ProcessDefFactory process_def_factory;
     
                 process_def = process_def_factory.create (process_name.c_str());
                 
                 #else
                 
                 CORBA::ValueFactoryBase_var process_def_factory =
                         new PlatformMngt::ProcessDefFactory();
     
                 process_def = 
                   dynamic_cast <PlatformMngt::ProcessDef*> 
                             (process_def_factory.in())->create (process_name.c_str());
                         
                 #endif
             }
             else
             {
                 #if CDMW_ORB_VDR == tao
                 
                 PlatformMngt::ManagedProcessDefFactory managed_process_def_factory;
     
                 process_def =
                       managed_process_def_factory.create(process_name.c_str(),
                                                          data.type);
                         
                 #else
                 
                 CORBA::ValueFactoryBase_var managed_process_def_factory =
                         new PlatformMngt::ManagedProcessDefFactory();
     
                 process_def =
                   dynamic_cast <PlatformMngt::ManagedProcessDefFactory*>
                             (managed_process_def_factory.in())->create (process_name.c_str(),
                                                                         data.type);
     
                 #endif
             }
     
             // Set the process definition
             process_def->auto_ending (data.auto_ending);
             process_def->host_name (data.host_name);
             process_def->process_info(data.process_info);
     
             // Set the managed process definition
             if (process_def->type() != CdmwPlatformMngt::UNMANAGED_PROCESS)
             {
                 CdmwPlatformMngt::ManagedProcessDef* managed_process_def =
                       CdmwPlatformMngt::ManagedProcessDef::_downcast(process_def.in());
     
                 managed_process_def->entities().length(0);
                 managed_process_def->service_definitions().length(0);
                 managed_process_def->life_cycle(data.life_cycle);
                 managed_process_def->monitoring_info(data.monitoring_info);
             }
     
             application_obj->add_process (process_def.in()); 
        }
        catch (...)
        {
            // nothing to do
        }    
    }
    // else nothing to do because we are the primary and we are inserting the element 
    // into the datastore
}

void ProcessDataStoreObserver::on_update(_Oid_in_type oid, _Data_in_type data)
  throw ()
{

}

void ProcessDataStoreObserver::on_remove(_Oid_in_type oid) 
  throw ()
{
    if (!Cdmw::PlatformMngt::Configuration::Is_primary())
    {
        try
        {
            // get the application parent of process           
            std::string process_name;
            std::string host_name;
            std::string parent_path;
            
            // get the element name and parent path
            ManagedElementKey::Get_process_parent_path (oid, process_name, 
                                                        host_name, parent_path);
        
            // get the application object reference
            CdmwPlatformMngt::ManagedElement_var element_obj =
                        m_element_repository->get_element(parent_path.c_str());
                        
            CdmwPlatformMngt::Application_var application_obj =
                        CdmwPlatformMngt::Application::_narrow (element_obj.in());
            
    
            application_obj->remove_process (process_name.c_str(), host_name.c_str());
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

