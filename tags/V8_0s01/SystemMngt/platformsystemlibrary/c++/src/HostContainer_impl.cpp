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

#include "platformsystemlibrary/HostContainer_impl.hpp"
#include "platformsystemlibrary/HostsIterator_impl.hpp"

#include "SystemMngt/platformlibrary/ElementNamesIterator_impl.hpp"



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
HostContainer_impl::HostContainer_impl(
      PortableServer::POA_ptr poa,
      PlatformMngt::HostContainer* pActualContainer,
      CdmwPlatformMngt::RWElementRepository_ptr element_repository) 
   throw() 
   : DeactivableServant_impl(poa),
     m_element_repository(CdmwPlatformMngt::RWElementRepository::_duplicate(element_repository)),
     m_actualContainer(pActualContainer)
            
{
}



//
// Destructor
// 
HostContainer_impl::~HostContainer_impl()
        throw()
{
}



//
// Get number of Hosts from container
//
CORBA::ULong HostContainer_impl::get_number_of_hosts()
        throw(CORBA::SystemException)
{
	CORBA::ULong number = 
	     m_actualContainer->getNumberOfElements();
	     
	return number;
}




//
// Get Host selected by its name from container
//
CdmwPlatformMngt::Host_ptr HostContainer_impl::get_host(const char* host_name)
         throw(CdmwPlatformMngt::HostNotFound,
               CORBA::SystemException)
{
	try
	{
	    Host_impl* p_host_impl;
	    
	    bool result = m_actualContainer->findElement(host_name, p_host_impl);
	    
	    if (result == false)
	    {
	    	throw CdmwPlatformMngt::HostNotFound();
	    }
	    else
	    {
	        // return the Host object reference
	        return p_host_impl->_this();
	    }
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
// Get sequence of Hosts from container
// 
CORBA::Boolean HostContainer_impl::get_hosts(const CdmwPlatformMngt::ElementNames& host_names,
                                             CdmwPlatformMngt::Hosts_out hosts)
         throw(CORBA::SystemException)
{
	try
    {
    	CORBA::Boolean result = true;
    	
        // create the requested sequence of Hosts
        CdmwPlatformMngt::Hosts_var requested_hosts = 
                                           new CdmwPlatformMngt::Hosts;
                                           
        // set length of requested sequence
        size_t len = host_names.length();
                     
        if (len > 0)
        {
             requested_hosts->length(len);
        }
                                           
    	for (unsigned int i=0 ; i < len ; i++)
        {
            Host_impl* p_host_impl;
            CdmwPlatformMngt::Host_var host = 
                              CdmwPlatformMngt::Host::_nil();

            // get host name from request list
            #if CDMW_ORB_VDR == orbacus
            std::string name = host_names[i];
            #else
            std::string name = (host_names[i]).in();
            #endif
            
            bool findResult = m_actualContainer->findElement(name, p_host_impl);
	    
	        if (findResult == false)
	        {        	
	            result = false;
	        }
	        else
	        {
	            // get Host object reference
	            host = p_host_impl->_this();
	        }
            
            
            // stock object in the sequence           
            requested_hosts[i] = host._retn();
        }
        
        // everything is OK, so return requested Host sequence
        hosts = requested_hosts._retn();
        
        // return method result
        return result;

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
// Get all host names from container
//
void HostContainer_impl::get_all_host_names (CORBA::ULong how_many,
                                 CdmwPlatformMngt::ElementNames_out host_names,
                                 CdmwPlatformMngt::ElementNamesIterator_out rest)
         throw(CORBA::SystemException)
{
	try
    {
    	 // list of request host names from container
         PlatformMngt::HostContainer::HostNameList hostCtnrReqList;
         
         // list of rest of host names from container
         PlatformMngt::HostContainer::HostNameList hostCtnrRestList;

         // get list of host names from container
         m_actualContainer->listName(how_many, 
                                     hostCtnrReqList, 
                                     hostCtnrRestList);

         // create the requested sequence of host names
         CdmwPlatformMngt::ElementNames_var requested_hosts = 
                                           new CdmwPlatformMngt::ElementNames;
         
         // get length of list                                    
         size_t len = hostCtnrReqList.size();
                
         if (len > 0)
         {
         	 // set length of sequence
             requested_hosts->length(len);

             // initialize list iterator
             PlatformMngt::HostContainer::HostNameList::iterator it = hostCtnrReqList.begin();
             
             for (CORBA::ULong i = 0; i < len; i++)
             {
             	// get host name from container list using iterator
                char *p_host_name = CORBA::string_dup((*it).c_str());
                    
                // store host name in sequence
                requested_hosts[i] = p_host_name;

                // increment iterator
                it++;
             }
         }
         
 
         // create the rest of list of host names
         std::list <CdmwPlatformMngt::ElementName_var> rest_of_hosts;
          
         // get length of list      
         len = hostCtnrRestList.size();
                
         if (len > 0)
         {
             // initialize list iterator
             PlatformMngt::HostContainer::HostNameList::iterator it = hostCtnrRestList.begin();
             
             for (CORBA::ULong i = 0; i < len; i++)
             {
             	// get host name from container list using iterator
                CdmwPlatformMngt::ElementName_var host_name = 
                   CORBA::string_dup((*it).c_str());

                // store entity name in list
                rest_of_hosts.push_back(host_name);

                // increment iterator
                it++;
             }
         }

         // create the HostNamesIterator with the rest of list
         CdmwPlatformMngt::ElementNamesIterator_var hosts_iter = 
            ElementNamesIterator_impl::create (rest_of_hosts);

         // everything is OK, so return requested list and iterator
         host_names = requested_hosts._retn();
         rest = hosts_iter._retn();
        
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
// Get all entities from container
//
void HostContainer_impl::get_all_hosts(CORBA::ULong how_many,
                                       CdmwPlatformMngt::Hosts_out hosts,
                                       CdmwPlatformMngt::HostsIterator_out rest)
         throw(CORBA::SystemException)
{   	
    try
    {       
         // list of request hosts from container
         PlatformMngt::HostContainer::HostList hostCtnrReqList;
         
         // list of rest of hosts from container
         PlatformMngt::HostContainer::HostList hostCtnrRestList;

         // get list of hosts from container
         m_actualContainer->list (how_many, 
                                  hostCtnrReqList, 
                                  hostCtnrRestList);

         // create the requested sequence of hosts
         CdmwPlatformMngt::Hosts_var requested_hosts = 
            new CdmwPlatformMngt::Hosts;
         
         // get length of list                                    
         size_t len = hostCtnrReqList.size();
                
         if (len > 0)
         {
         	 // set length of sequence
             requested_hosts->length(len);

             // initialize list iterator
             PlatformMngt::HostContainer::HostList::iterator it = hostCtnrReqList.begin();
             
             for (CORBA::ULong i = 0; i < len; i++)
             {
                // get host from container list using iterator
                // and convert to Host object reference
                CdmwPlatformMngt::Host_var host = (*it)->_this();
                    
                // store Host in sequence
                requested_hosts[i] = host._retn();

                // increment iterator
                it++;
             }
         }
         
 
         // create the rest of list of hosts
         std::list <CdmwPlatformMngt::Host_var> rest_of_hosts;
          
         // get length of list      
         len = hostCtnrRestList.size();
                
         if (len > 0)
         {
             // initialize list iterator
             PlatformMngt::HostContainer::HostList::iterator it = hostCtnrRestList.begin();
             
             for (CORBA::ULong i = 0; i < len; i++)
             {
                // get host from container list using iterator
                // and convert to Host object reference
                CdmwPlatformMngt::Host_var host = (*it)->_this();
                
                // store host object in list
                rest_of_hosts.push_back(host);

                // increment iterator
                it++;
             }
         }

         // create the HostsIterator with the rest of list
         CdmwPlatformMngt::HostsIterator_var hosts_iter =
                            HostsIterator_impl::create (rest_of_hosts);

         // everything is OK, so return requested sequence and iterator
         hosts = requested_hosts._retn();
         rest = hosts_iter._retn();
        
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



PlatformMngt::HostContainer* HostContainer_impl::getActualContainer () const
{
     return m_actualContainer;
}
        

} // End namespace PlatformMngt
} // End namespace Cdmw



