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

#include "SystemMngt/platformlibrary/ManagedElementKey.hpp"

#include "platformsystemlibrary/HostContainer.hpp"
#include "platformsystemlibrary/System_impl.hpp"


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
    HostContainer::HostContainer(System_impl* system,
                                 PortableServer::POA_ptr poa,
                                 CdmwPlatformMngt::RWElementRepository_ptr element_repository)
            throw(BadParameterException, CosPropertyService::MultipleExceptions )
    {
        if (system == NULL)
        {
            CDMW_THROW2 (BadParameterException, "system", "NULL");
        }
     
        if (CORBA::is_nil(poa))
        {
            CDMW_THROW2 (BadParameterException, "poa", "nil");
        }

        
        // Set the internal data
        m_system = system;
        m_poa = PortableServer::POA::_duplicate(poa);
          
        // Set the element repository
        m_element_repository = 
           CdmwPlatformMngt::RWElementRepository::_duplicate(element_repository);
    } 



    //
    // Destructor
    // 
    HostContainer::~HostContainer()
            throw()
    {
    } 

#ifdef CDMW_USE_FAULTTOLERANCE

    //
    // Set the fault detector
    //
    void HostContainer::setFaultDetector (CdmwFT::FaultManagement::FaultDetector_ptr faultDetector)
    {
        m_faultDetector = CdmwFT::FaultManagement::FaultDetector::_duplicate(faultDetector);
    }

#endif

    //
    // Add host in container
    //
    CdmwPlatformMngt::Host_ptr 
              HostContainer::addHost(const CdmwPlatformMngt::HostDef& host_def)
                throw (CdmwPlatformMngt::HostAlreadyExists,
                       CdmwPlatformMngt::DuplicateEntity,
                       CosPropertyService::MultipleExceptions,
                       CORBA::SystemException)
    {
        // get host name
        std::string host_name = host_def.host_name.in();
        
        // get system name
        CORBA::String_var system_name = m_system->name();
        
	    try
	    {
	       bool result;
       	   
       	   // build the host path
           std::string host_path = 
                  ManagedElementKey::Get_host_path (
                                         system_name.in(), host_name);
                                         
           // check if element already exists
           result = existsElement(host_name);
           if (result == true)
           {  
               throw CdmwPlatformMngt::HostAlreadyExists();
           }
           
           
	       // Create the Host servant
	       
#ifdef CDMW_USE_FAULTTOLERANCE
           
           // check fault detector has been initialised
		   if (CORBA::is_nil(m_faultDetector.in()))
           {
               throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
           }
        
		   Host_impl* p_host =	  
				   new Host_impl (m_poa.in(), 
                                  host_def,
                                  host_path.c_str(), 
                                  m_system, 
                                  m_element_repository.in(),
                                  m_faultDetector.in());

#else

           Host_impl* p_host = 
                   new Host_impl (m_poa.in(), 
                                  host_def, 
                                  host_path.c_str(),
                                  m_system, 
                                  m_element_repository.in());
#endif				                                 
                   
	        result = addElement(host_name, p_host);
	    
	        if (result == false)
	        {  
	    	    throw CdmwPlatformMngt::HostAlreadyExists();
	        }
	        
	        // call end creation of host
	        p_host->end_creation (host_def);
	        
	        // create an object var to take pointer ownership
            PortableServer::ServantBase_var hostServant = p_host;        
            // activate object
            p_host->activate();
            
            // start monitoring
            p_host->start();
            
            // return Host object reference
            CdmwPlatformMngt::Host_var hostObject = p_host->_this();
            
            // add in repository the host object
            m_element_repository->add_element (host_path.c_str(),
                                               CdmwPlatformMngt::HOST_TYPE,
                                               hostObject.in());
            
            return hostObject._retn();
	    }
	    catch(const CdmwPlatformMngt::ElementPathInvalid)
        {
       	    throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
        catch(const CdmwPlatformMngt::ElementAlreadyExists)
        {
       	    throw CdmwPlatformMngt::HostAlreadyExists();
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
        catch (const AlreadyDoneException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
        catch (const OsSupport::ThreadSchedulingException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
        catch(const BadParameterException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
	    catch(const CORBA::Exception& e)
        {
            throw;
        }
    } 


        
    //
    // Remove host from container
    //
    void HostContainer::removeHost(const char* host_name)
                throw (CdmwPlatformMngt::HostNotFound,
                       CORBA::SystemException)
    {
    	// check if host name is valid
	    if (host_name == NULL)
	    {
	        throw CdmwPlatformMngt::HostNotFound();
        }

        if (*host_name == '\0')
        { 
    	    throw CdmwPlatformMngt::HostNotFound();
        }
    
	    try
	    {
	        bool result;
	        
	        // first get the host_impl object
	        Host_impl* p_host_impl;
	    
	        result = findElement(host_name, p_host_impl);
	    
	        if (result == false)
	        {
	    	    throw CdmwPlatformMngt::HostNotFound();
	        }

            // suppress Host_impl object from the container map
	        result = removeElement(host_name);
	    
	        if (result == false)
	        {  
	    	    throw CdmwPlatformMngt::HostNotFound();
	        }
	        
	        // stop the monitoring
	        p_host_impl->stop();
	        
	        // remove from repository the host object
            m_element_repository->remove_element (p_host_impl->get_element_path());
 
	        // deactivate the Corba object (object will be deleted by orb)
	        p_host_impl->deactivate();
	        
	    }
	    catch(const CdmwPlatformMngt::ElementPathInvalid)
        {
       	   throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
        catch(const CdmwPlatformMngt::ManagedElementNotFound)
        {
       	   throw CdmwPlatformMngt::HostNotFound();
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
    // Get a host implementation from the system.
    //
    Host_impl* HostContainer::getHost(const char* host_name)
                throw (CdmwPlatformMngt::HostNotFound,
                       CORBA::SystemException)
    {
        // check if host name is valid
	    if (host_name == NULL)
	    {
	        throw CdmwPlatformMngt::HostNotFound();
        }

        if (*host_name == '\0')
        { 
    	    throw CdmwPlatformMngt::HostNotFound();
        }
    
	    try
	    {
	        bool result;
	        
	        // first get the host_impl object
	        Host_impl* p_host_impl;
	    
	        result = findElement(host_name, p_host_impl);
	    
	        if (result == false)
	        {
	    	    throw CdmwPlatformMngt::HostNotFound();
	        }	  
	        
	        return p_host_impl;   
	    }
	    catch(const OutOfResourcesException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
    }
    
                      
    
    //
    // return hosts snapshot
    //
    CdmwPlatformMngt::HostsSnapshot* HostContainer::getSnapshot()
        throw (Cdmw::OutOfResourcesException)
    {
        try
        {
            CdmwPlatformMngt::HostsSnapshot_var snapshot =
                   new CdmwPlatformMngt::HostsSnapshot;
                                     
            // Get the snapshot of all hosts
        
            // set sequence length
            snapshot->length (getNumberOfElements());
        
            // get list of host 
            HostList hostList;
            list (hostList);
        
            // fill snapshot sequence
            HostList::iterator hostIt;
            unsigned int i;
        
            for (hostIt=hostList.begin(), i=0;
                 hostIt != hostList.end();
                 hostIt++, i++)
            {
                snapshot[i].host_name = (*hostIt)->name();
                snapshot[i].host_status = (*hostIt)->status();
                
                // Get the snapshot of all entities of the host
                CdmwPlatformMngt::EntitiesSnapshot_var entitiesSnapshot
		  = new CdmwPlatformMngt::EntitiesSnapshot();
		
                // Gets all the entities of the host
                CdmwPlatformMngt::Entities_var entities;
                CdmwPlatformMngt::EntitiesIterator_var entitiesIt;
                CORBA::ULong nbr_of_entities;
		
                nbr_of_entities = (*hostIt)->get_number_of_entities();
		
                (*hostIt)->get_all_entities(nbr_of_entities,
                                            entities.out(), entitiesIt.out());
		
                entitiesSnapshot->length(nbr_of_entities);
                for (unsigned int j = 0; j < nbr_of_entities; j++)
		  {
                    entitiesSnapshot[j] = *(entities[j]->get_snapshot());
		  }
		//snapshot[i].host_entities = entitiesSnapshot.in();
		
	        CosPropertyService::PropertiesIterator_var propertiesIt;
       		CORBA::ULong nbr_of_properties;
       		CdmwPlatformMngt::PropertiesSnapshot_var propertiesSnapshot = new CdmwPlatformMngt::PropertiesSnapshot;
        	nbr_of_properties = (*hostIt)->get_number_of_properties();
	        CosPropertyService::PropertyNames_var prop_names;
       	 	CosPropertyService::PropertyNamesIterator_var prop_namesIt;
		(*hostIt)->get_all_property_names(nbr_of_properties, prop_names.out(), prop_namesIt.out());
		propertiesSnapshot->length(nbr_of_properties);
        	for (unsigned int j = 0; j < nbr_of_properties ; j++)
		{
		    CdmwPlatformMngt::PropertySnapshot* propertySnapshot = new CdmwPlatformMngt::PropertySnapshot();
		    propertySnapshot -> property_name = CORBA::string_dup(prop_names[j]);
		    CORBA::Any prop_value;
		    prop_value = *((*hostIt)->get_property_value(prop_names[j]));
		    CORBA::TypeCode_var tc;
		    tc = prop_value.type();
		    switch (tc->kind())	
		    {
		    default:
			break;
		    case CORBA::tk_string:
			propertySnapshot -> property_type = CORBA::string_dup("String");
			break;
		    case CORBA::tk_short:
			propertySnapshot -> property_type = CORBA::string_dup("Short");
			break;
		    case CORBA::tk_float:
			propertySnapshot -> property_type = CORBA::string_dup("Float");
			break;
		    };
		    
		    CosPropertyService::PropertyModeType mode= (*hostIt)->get_property_mode(prop_names[j]);
		    propertySnapshot->property_value = prop_value;
		    if (mode==0)  propertySnapshot->property_mode = CORBA::string_dup("Read/Write");
		    else  propertySnapshot->property_mode = CORBA::string_dup("Read/Only");
		    propertySnapshot->property_value = prop_value;
		    propertiesSnapshot[j] = *(propertySnapshot);
		}
		snapshot[i].host_entities = entitiesSnapshot.in();
		snapshot[i].host_properties = propertiesSnapshot.in();
	    }
            // return snapshot sequence
	    
	    return snapshot._retn();
	}
	    
	catch(const std::bad_alloc &)
	      {
		  CDMW_THROW (Cdmw::OutOfResourcesException);
	      }
	
    }
    
    
} // End namespace PlatformMngt
} // End namespace Cdmw

  

