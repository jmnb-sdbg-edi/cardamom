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


#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "SystemMngt/platformlibrary/RWServiceDefContainer_impl.hpp"
#include "SystemMngt/platformlibrary/Configuration.hpp"


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

    //
    // Constructor
    //
    RWServiceDefContainer_impl::RWServiceDefContainer_impl(
                 PortableServer::POA_ptr poa,
                 ServiceDefContainer_impl::ServiceDefContainer* pActualContainer,
                 CdmwPlatformMngtService::ServiceDefContainer* pParentContainer)
            throw()
            : DeactivableServant_impl (poa),
              ServiceDefContainer_impl(poa, pActualContainer, pParentContainer)
    {
        try
        {
            Cdmw::CommonSvcs::DataStore::DataStoreBase* ds =
                Cdmw::CommonSvcs::DataStore::DataStoreRegistry::Get_data_store(
                   Configuration::Get_service_defs_datastore_id());
            m_service_defs_ds = 
                 dynamic_cast<Cdmw::PlatformMngt::ServiceDefsDataStore*>(ds);
        }
        catch (const NotExistException& e)
        {
            CDMW_ASSERT(false);
        }
    } 



    //
    // Destructor
    // 
    RWServiceDefContainer_impl::~RWServiceDefContainer_impl()
            throw()
    {
    } 

    //
    // Set container path
    //
    void RWServiceDefContainer_impl::set_container_path (const char* container_path)
    {
        m_container_path = CORBA::string_dup (container_path);  
    }

    //
    // Add service in container
    //
    void RWServiceDefContainer_impl::add_service_def (
                                  const CdmwPlatformMngtService::ServiceDef& service_def)
		    throw(CdmwPlatformMngtService::ServiceAlreadyExists,
		          CORBA::SystemException)
    
    {
        std::string name = service_def.service_name.in();
        
	    try
	    {
	        bool result = getActualContainer()->addElement (name,
	                                                        service_def.service_provider);
	    
	        if (result == false)
	        {  
	    	    throw CdmwPlatformMngtService::ServiceAlreadyExists();
	        }
	        
	        // update the service defs record in datastore
	        set_record_in_datastore();
	    }
	    catch(const std::bad_alloc &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const OutOfResourcesException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const OutOfMemoryException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const AssertionFailedException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
        catch(const InternalErrorException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
	    catch(const CORBA::Exception& e)
        {
            throw;
        }
    } 


        
    //
    // Remove service from container
    //
    void RWServiceDefContainer_impl::remove_service_def (const char* service_name)
             throw(CdmwPlatformMngtService::ServiceNotFound,
                   CORBA::SystemException)
    {
	    try
	    {
	        bool result = getActualContainer()->removeElement(service_name);
	    
	        if (result == false)
	        {  
	    	    throw CdmwPlatformMngtService::ServiceNotFound();
	        }
	        
	        // update the service defs record in datastore
	        set_record_in_datastore();
	    }
	    catch(const std::bad_alloc &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const OutOfResourcesException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const OutOfMemoryException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const AssertionFailedException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
        catch(const InternalErrorException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
	    catch(const CORBA::Exception& e)
        {
            throw;
        }
    } 
    
    
    
    //
    // Remove all services from container
    //
    void RWServiceDefContainer_impl::remove_all_service_defs ()
             throw(CORBA::SystemException)
    {
	    getActualContainer()->removeAllElements();
	    
	    // remove all record from datastore
	    remove_record_in_datastore();
    }
                      
                      
                      
    // 
    // Set the servicedef record in data store
    //
    void RWServiceDefContainer_impl::set_record_in_datastore()
    {
        if (Configuration::Is_primary())
        {      
            // get the list of services from container
            ServiceProviderIDList service_list;
            

            getActualContainer()->list(service_list);
            
            // get the number of services
            unsigned int service_count = getActualContainer()->getNumberOfElements();
            
            // transfert the list of services in sequence
            CdmwPlatformMngtService::ServiceDefs service_definitions;
            service_definitions.length (service_count);
            
            ServiceProviderIDList::iterator it = service_list.begin(); 
            
            for (unsigned int service_index=0;
                 service_index<service_count;
                 service_index++, it++)
            {
                // Get the service definition from the list
                
                CdmwPlatformMngtService::ServiceDef service_def;
                service_def.service_name = CORBA::string_dup((it->m_name).c_str());
                service_def.service_provider = it->m_data;
                
                // store in sequence
                service_definitions[service_index] = service_def;
            }

            // update the datastore  
            if (m_service_defs_ds->contains (m_container_path.in()))
            {
                m_service_defs_ds->update(m_container_path.in(), service_definitions);
            }
            else
            {
                m_service_defs_ds->insert(m_container_path.in(), service_definitions);
            }
        }
    }

    //
    // Remove the servicedef record in datastore
    //
    void RWServiceDefContainer_impl::remove_record_in_datastore()
    {
       // inhibition if backup
       if (Configuration::Is_primary())
       {
           try
           {
               m_service_defs_ds->remove(m_container_path.in());
           }
           catch (Common::NotFoundException&)
           {
           }
       }
    }                  
                      
    


} // End namespace PlatformMngt
} // End namespace Cdmw



