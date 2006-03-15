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

#include "platformftaccessorlibrary/HostContainerAccessor_impl.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtHost.stub.hpp"

#include "SystemMngt/platformlibrary/ManagedElementKey.hpp"

#include "SystemMngt/platformlibrary/ElementPathsIterator_impl.hpp"



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
HostContainerAccessor_impl::HostContainerAccessor_impl(
    CdmwPlatformMngt::ElementRepository_ptr element_repository)
throw()
{        
    m_host_element_repository = 
           CdmwPlatformMngt::ElementRepository::_duplicate(element_repository);
}

//
// Destructor
// 
HostContainerAccessor_impl::~HostContainerAccessor_impl()
{
}


CdmwPlatformMngt::HostContainer_ptr HostContainerAccessor_impl::get_host_container (
          const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound)
{
    CdmwPlatformMngt::HostContainer_var host_container_obj;
    
    try
    {
        CORBA::Object_var element_obj =
              m_host_element_repository->get_element(target_element_path);
             
        host_container_obj = CdmwPlatformMngt::HostContainer::_narrow (element_obj.in());

        
    }
    catch (const CdmwPlatformMngt::ElementPathInvalid&)
    {
        throw CdmwPlatformMngt::ManagedElementNotFound(target_element_path);
    }
    catch(const CORBA::SystemException&)
    {
        throw CdmwPlatformMngt::ManagedElementNotFound(target_element_path);
    }
    
    return host_container_obj._retn();                   
} 

CORBA::ULong HostContainerAccessor_impl::get_number_of_hosts(
        const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{	
    CdmwPlatformMngt::HostContainer_var host_container_obj = 
                        get_host_container (target_element_path);
                        
    return host_container_obj->get_number_of_hosts();
}

 	

void HostContainerAccessor_impl::get_all_host_names (
         const char* target_element_path,
         CORBA::ULong how_many,
         CdmwPlatformMngt::ElementNames_out host_names,
         CdmwPlatformMngt::ElementNamesIterator_out rest)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::HostContainer_var host_container_obj = 
                        get_host_container (target_element_path);
    
    host_container_obj->get_all_host_names (how_many, host_names, rest);
      
}
    	
char* HostContainerAccessor_impl::get_host(
         const char* target_element_path,
		 const char* host_name)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CdmwPlatformMngt::HostNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::HostContainer_var host_container_obj = 
                        get_host_container (target_element_path);
    
    CdmwPlatformMngt::Host_var host =
             host_container_obj->get_host (host_name);
                              
    return host->element_path();
}


CORBA::Boolean HostContainerAccessor_impl::get_hosts (
		   const char* target_element_path,
           const CdmwPlatformMngt::ElementNames& host_names,
           CdmwPlatformMngt::ElementPaths_out hosts)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
    CdmwPlatformMngt::HostContainer_var host_container_obj = 
                        get_host_container (target_element_path);
            
    // get the list of host references
    CdmwPlatformMngt::Hosts_var hosts_seq;
    
    bool result = host_container_obj->get_hosts (host_names, hosts_seq.out());
	            
    // convert each host reference in element path
    
    // create sequence of element path
    CdmwPlatformMngt::ElementPaths_var elements =
                          new CdmwPlatformMngt::ElementPaths();
    
    // set element sequence length 
    size_t seq_len = hosts_seq->length();
    elements->length(seq_len);
    
    // fill element sequence
    unsigned int count;
    for (count=0 ; count < seq_len ; count++)
    {
        // Store process path in sequence
        // path is allocated by element_path()
        if (!CORBA::is_nil(hosts_seq[count]))
            elements[count] = (hosts_seq[count])->element_path();                                           
    }

    hosts = elements._retn();
    
    return result;
}



void HostContainerAccessor_impl::get_all_hosts(
          const char* target_element_path,
          CORBA::ULong how_many,
          CdmwPlatformMngt::ElementPaths_out hosts,
          CdmwPlatformMngt::ElementPathsIterator_out rest)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{   	
    CdmwPlatformMngt::HostContainer_var host_container_obj = 
                        get_host_container (target_element_path);
            
    // get the list of host references
    CdmwPlatformMngt::Hosts_var hosts_seq;
    CdmwPlatformMngt::HostsIterator_var hostsRemainingIt;
    
    host_container_obj->get_all_hosts (
	                   how_many, hosts_seq.out(), hostsRemainingIt.out());
	            
    // convert each host reference in element path
    
    // create sequence of element path
    CdmwPlatformMngt::ElementPaths_var elements =
                          new CdmwPlatformMngt::ElementPaths();
    
    // set element sequence length 
    size_t seq_len = hosts_seq->length();
    elements->length(seq_len);
    
    // fill element sequence
    unsigned int count;
    for (count=0 ; count < seq_len ; count++)
    {
        // Store process path in sequence
        // path is allocated by element_path()
        elements[count] = (hosts_seq[count])->element_path();                                           
    }
        
    // Get an iterator on the rest of the elements
    std::list<CdmwPlatformMngt::ElementPath_var> elementsRemaining;
    CdmwPlatformMngt::Host_var host;
    
    // fore each element contained in reference iterator
    while (hostsRemainingIt->next_one (host.out()))
    { 
        elementsRemaining.push_back (host->element_path());
    }
    
    // create the path iterator
    CdmwPlatformMngt::ElementPathsIterator_var elementsRemainingIt =
              ElementPathsIterator_impl::create (elementsRemaining);

    hosts = elements._retn();
    rest = elementsRemainingIt._retn();
}
      

} // End namespace PlatformMngt
} // End namespace Cdmw
