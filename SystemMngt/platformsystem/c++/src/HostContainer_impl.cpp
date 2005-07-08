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


#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "platformsystem/HostContainer_impl.hpp"
#include "platformsystem/HostProxiesIterator_impl.hpp"
#include "platformsystem/HostNamesIterator_impl.hpp"



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
HostContainer_impl::HostContainer_impl(PlatformMngt::HostContainer* pActualContainer)
        throw()
        : m_actualContainer (pActualContainer)
            
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
// Get number of HostProxies from container
//
CORBA::ULong HostContainer_impl::get_number_of_hosts()
        throw(CORBA::SystemException)
{
	CORBA::ULong number = 
	     m_actualContainer->getNumberOfElements();
	     
	return number;
}




//
// Get HostProxy selected by its name from container
//
CdmwPlatformMngt::HostProxy_ptr HostContainer_impl::get_host(const char* host_name)
         throw(CdmwPlatformMngt::HostNotFound,
               CORBA::SystemException)
{
	try
	{
	    HostProxy_impl* p_hostProxy_impl;
	    
	    bool result = m_actualContainer->findElement(host_name, p_hostProxy_impl);
	    
	    if (result == false)
	    {
	    	throw CdmwPlatformMngt::HostNotFound();
	    }
	    else
	    {
	        // return the HostProxy object reference
	        return p_hostProxy_impl->_this();
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
// Get sequence of HostProxies from container
// 
CORBA::Boolean HostContainer_impl::get_hosts(const CdmwPlatformMngt::HostNames& host_names,
                                             CdmwPlatformMngt::HostProxies_out hosts)
         throw(CORBA::SystemException)
{
	try
    {
    	CORBA::Boolean result = true;
    	
        // create the requested sequence of HostProxies
        CdmwPlatformMngt::HostProxies_var requested_hostProxies = 
                                           new CdmwPlatformMngt::HostProxies;
                                           
        // set length of requested sequence
        size_t len = host_names.length();
                     
        if (len > 0)
        {
             requested_hostProxies->length(len);
        }
                                           
    	for (unsigned int i=0 ; i < len ; i++)
        {
            HostProxy_impl* p_hostProxy_impl;
            CdmwPlatformMngt::HostProxy_var hostProxy = 
                              CdmwPlatformMngt::HostProxy::_nil();

            // get host name from request list
            #if CDMW_ORB_VDR == orbacus
            std::string name = host_names[i];
            #else
            std::string name = (host_names[i]).in();
            #endif
            
            bool findResult = m_actualContainer->findElement(name, p_hostProxy_impl);
	    
	        if (findResult == false)
	        {        	
	            result = false;
	        }
	        else
	        {
	            // get HostProxy object reference
	            hostProxy = p_hostProxy_impl->_this();
	        }
            
            
            // stock object in the sequence           
            requested_hostProxies[i] = hostProxy._retn();
        }
        
        // everything is OK, so return requested HostProxy sequence
        hosts = requested_hostProxies._retn();
        
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
// Get all entities names from container
//
void HostContainer_impl::get_all_host_names (CORBA::ULong how_many,
                                 CdmwPlatformMngt::HostNames_out host_names,
                                 CdmwPlatformMngt::HostNamesIterator_out rest)
         throw(CORBA::SystemException)
{
	try
    {
    	 // list of request host names from container
         HostNameList hostCtnrReqList;
         
         // list of rest of host names from container
         HostNameList hostCtnrRestList;

         // get list of host names from container
         m_actualContainer->listName(how_many, 
                                     hostCtnrReqList, hostCtnrRestList);

         // create the requested sequence of host names
         CdmwPlatformMngt::HostNames_var requested_hosts = 
                                           new CdmwPlatformMngt::HostNames;
         
         // get length of list                                    
         size_t len = hostCtnrReqList.size();
                
         if (len > 0)
         {
         	 // set length of sequence
             requested_hosts->length(len);

             // initialize list iterator
             HostNameList::iterator it = hostCtnrReqList.begin();
             
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
         std::list <CdmwPlatformMngt::HostName_var> rest_of_hosts;
          
         // get length of list      
         len = hostCtnrRestList.size();
                
         if (len > 0)
         {
             // initialize list iterator
             HostNameList::iterator it = hostCtnrRestList.begin();
             
             for (CORBA::ULong i = 0; i < len; i++)
             {
             	// get host name from container list using iterator
                CdmwPlatformMngt::HostName_var host_name = CORBA::string_dup((*it).c_str());

                // store entity name in list
                rest_of_hosts.push_back(host_name);

                // increment iterator
                it++;
             }
         }

         // create the HostNamesIterator with the rest of list
         CdmwPlatformMngt::HostNamesIterator_var hosts_iter =
                          HostNamesIterator_impl::create (rest_of_hosts);

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
                                       CdmwPlatformMngt::HostProxies_out hosts,
                                       CdmwPlatformMngt::HostProxiesIterator_out rest)
         throw(CORBA::SystemException)
{   	
    try
    {       
         // list of request hosts from container
         HostList hostCtnrReqList;
         
         // list of rest of hosts from container
         HostList hostCtnrRestList;

         // get list of hosts from container
         m_actualContainer->list (how_many, 
                                  hostCtnrReqList, hostCtnrRestList);

         // create the requested sequence of hosts
         CdmwPlatformMngt::HostProxies_var requested_hosts = 
                                           new CdmwPlatformMngt::HostProxies;
         
         // get length of list                                    
         size_t len = hostCtnrReqList.size();
                
         if (len > 0)
         {
         	 // set length of sequence
             requested_hosts->length(len);

             // initialize list iterator
             HostList::iterator it = hostCtnrReqList.begin();
             
             for (CORBA::ULong i = 0; i < len; i++)
             {
                // get host from container list using iterator
                // and convert to HostProxy object reference
                CdmwPlatformMngt::HostProxy_var hostProxy = (*it)->_this();
                    
                // store HostProxy in sequence
                requested_hosts[i] = hostProxy._retn();

                // increment iterator
                it++;
             }
         }
         
 
         // create the rest of list of hosts
         std::list <CdmwPlatformMngt::HostProxy_var> rest_of_hosts;
          
         // get length of list      
         len = hostCtnrRestList.size();
                
         if (len > 0)
         {
             // initialize list iterator
             HostList::iterator it = hostCtnrRestList.begin();
             
             for (CORBA::ULong i = 0; i < len; i++)
             {
                // get host from container list using iterator
                // and convert to HostProxy object reference
                CdmwPlatformMngt::HostProxy_var hostProxy = (*it)->_this();
                
                // store hostProxy object in list
                rest_of_hosts.push_back(hostProxy);

                // increment iterator
                it++;
             }
         }

         // create the HostProxiesIterator with the rest of list
         CdmwPlatformMngt::HostProxiesIterator_var hostProxies_iter =
                            HostProxiesIterator_impl::create (rest_of_hosts);

         // everything is OK, so return requested sequence and iterator
         hosts = requested_hosts._retn();
         rest = hostProxies_iter._retn();
        
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



