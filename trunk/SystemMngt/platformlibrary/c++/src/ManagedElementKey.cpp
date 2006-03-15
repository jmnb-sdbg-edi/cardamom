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


#include <iostream>
#include <sstream>

#include "SystemMngt/platformlibrary/ManagedElementKey.hpp"

#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"

// definition of local constants in an unnammed namespace
namespace 
{   
	const std::string key_sep = "/";
    const std::string system_suffix = ".sys";
    const std::string host_suffix = ".host";
    const std::string application_suffix = ".appl";
    const std::string process_suffix = ".proc";
    const std::string entity_suffix = ".ent";
}

namespace Cdmw
{

namespace PlatformMngt
{


std::string ManagedElementKey::Get_system_path (
                                    const std::string & system_name)
{
	// do not use system_name but a reduced name
	std::string element_path = "s";
	element_path += system_suffix;
	
	return element_path;
}
        

std::string ManagedElementKey::Get_host_path (
                                     const std::string & system_name,
                                     const std::string & host_name)
{
	std::string element_path = Get_system_path (system_name);
	element_path += key_sep;
	element_path += host_name;
	element_path += host_suffix;
	
	return element_path;
}                                     
        

std::string ManagedElementKey::Get_application_path (
                                     const std::string & system_name,
                                     const std::string & application_name)
{
	std::string element_path = Get_system_path (system_name);
	element_path += key_sep;
	element_path += application_name;
	element_path += application_suffix;
	
	return element_path;
}
                                     

std::string ManagedElementKey::Get_process_path (
                                     const std::string & system_name,
                                     const std::string & application_name,
                                     const std::string & host_name,
                                     const std::string & process_name)
{
	std::string element_path = Get_application_path (system_name, application_name);
	element_path += key_sep;
	element_path += host_name;
	element_path += host_suffix;
	
	element_path += key_sep;
	element_path += process_name;
	element_path += process_suffix;
	
	return element_path;
}

std::string ManagedElementKey::Get_process_path (
                                      const std::string & parent_path,
                                      const std::string & host_name,
                                      const std::string & process_name)
{
	std::string element_path = parent_path;
	element_path += key_sep;
	element_path += host_name;
	element_path += host_suffix;
	
	element_path += key_sep;
	element_path += process_name;
	element_path += process_suffix;
	
	return element_path;
}

                                      
                                      
std::string ManagedElementKey::Get_process_rel_key (
                                      const std::string & host_name,
                                      const std::string & process_name)
{
	std::string element_key = host_name;
	element_key += key_sep;
	element_key += process_name;
	
	return element_key;
}

bool ManagedElementKey::Check_process_rel_key (const std::string & process_rel_key,
                                               std::string & process_name)
            throw (CdmwPlatformMngt::ElementPathInvalid)
{
	bool result = false;    // false if no error
	process_name = "";
	
    std::string::size_type pos_sep1;
    std::string::size_type key_size = process_rel_key.size();
    
    pos_sep1 = process_rel_key.find(key_sep);
    
    // check if host name and process name are found
    if (pos_sep1 != std::string::npos)
    {
    	// set error if host is undefined
    	if (pos_sep1 < 2)
    	{
    		result = true;
    	}
    	
    	pos_sep1 += key_sep.size();
    }
    else
    {
        // no separator : host name is undefined, only process is defined
        result = true;
        pos_sep1 = 0;
    }
    
    // check if process name found
    if (pos_sep1 < key_size)
    { 
        process_name = process_rel_key.substr(pos_sep1);
    }
    else
    {
    	// relative key is not valid
    	throw CdmwPlatformMngt::ElementPathInvalid();
    }
    
    return result;
}
                                      
void ManagedElementKey::Get_process_element_names (
                                   const std::string & process_rel_key,                                         
                                   std::string & host_name,
                                   std::string & process_name)
    throw (CdmwPlatformMngt::ElementPathInvalid)
{
    host_name = "";
    process_name = "";
    
    std::string::size_type pos_sep1;
    std::string::size_type key_size = process_rel_key.size();
    
    pos_sep1 = process_rel_key.find(key_sep);
    
    // check if host name found
    if (pos_sep1 != std::string::npos)
    {
    	if (pos_sep1 > 1)
    	{
    	    host_name = process_rel_key.substr(0, pos_sep1);
    	}
    	
    	pos_sep1 += key_sep.size();
    }
    else
    {
    	// relative key is not valid
    	throw CdmwPlatformMngt::ElementPathInvalid();
    }
    
    // check if process name found
    if (pos_sep1 < key_size)
    { 
        process_name = process_rel_key.substr(pos_sep1);
    }
    else
    {
    	// relative key is not valid
    	throw CdmwPlatformMngt::ElementPathInvalid();
    }
}

                                     

std::string ManagedElementKey::Get_system_entity_path (
                                     const std::string & system_name,
                                     const std::string & entity_name)
{
	std::string element_path = Get_system_path (system_name);
	element_path += key_sep;
	element_path += entity_name;
	element_path += entity_suffix;
	
	return element_path;
}        


std::string ManagedElementKey::Get_host_entity_path (
                                     const std::string & system_name,
                                     const std::string & host_name,
                                     const std::string & entity_name)
{
	std::string element_path = Get_host_path (system_name, host_name);
	element_path += key_sep;
	element_path += entity_name;
	element_path += entity_suffix;
	
	return element_path;
}
        

std::string ManagedElementKey::Get_application_entity_path (
                                     const std::string & system_name,
                                     const std::string & application_name,
                                     const std::string & entity_name)
{
	std::string element_path = Get_application_path (system_name, application_name);
	element_path += key_sep;
	element_path += entity_name;
	element_path += entity_suffix;
	
	return element_path;
}
                                     
        
std::string ManagedElementKey::Get_process_entity_path (
                                     const std::string & system_name,
                                     const std::string & application_name,
                                     const std::string & host_name,
                                     const std::string & process_name,
                                     const std::string & entity_name)
{
	std::string element_path = Get_process_path (system_name, application_name,
	                                             host_name, process_name);
	element_path += key_sep;
	element_path += entity_name;
	element_path += entity_suffix;
	
	return element_path;
}

std::string ManagedElementKey::Get_entity_path (
                                     const std::string & parent_path,
                                     const std::string & entity_name)
    throw (CdmwPlatformMngt::ElementPathInvalid)
{
	std::string system_name = "";
    std::string application_name = "";
    std::string host_name = "";
    std::string process_name = "";
    std::string buffer = "";
    
    std::string entity_path = "";
        
    // get element path components from parent
    CdmwPlatformMngt::ElementType parent_type = 
    	Get_element_names (parent_path,                                         
                           system_name,
                           application_name,
                           host_name,
                           process_name,
                           buffer);
	
	switch (parent_type)
    {
         case (CdmwPlatformMngt::SYSTEM_TYPE) :
            entity_path = Get_system_entity_path (system_name, entity_name);
            break;
         case (CdmwPlatformMngt::HOST_TYPE) :
            entity_path = Get_host_entity_path (system_name,
                                                host_name,
                                                entity_name);
            break;
         case (CdmwPlatformMngt::APPLICATION_TYPE) :
            entity_path = Get_application_entity_path (system_name, 
                                                       application_name,
                                                       entity_name);
            break;
         case (CdmwPlatformMngt::PROCESS_TYPE) :
            entity_path = Get_process_entity_path (system_name, 
                                                   application_name,
                                                   host_name,
                                                   process_name,
                                                   entity_name);
            break;
         default:
            // element path is not valid
    	    throw CdmwPlatformMngt::ElementPathInvalid();
    }
    
    return entity_path;
}


CdmwPlatformMngt::ElementType ManagedElementKey::Get_element_names (
                                       const std::string & element_path,                                         
                                       std::string & system_name,
                                       std::string & application_name,
                                       std::string & host_name,
                                       std::string & process_name,
                                       std::string & entity_name)
    throw (CdmwPlatformMngt::ElementPathInvalid)
{
	CdmwPlatformMngt::ElementType element_type;
	
	system_name = "";
    application_name = "";
    host_name = "";
    process_name = "";
    entity_name = "";
    
    std::string::size_type pos_suffix1;
    std::string::size_type pos_suffix2;
    std::string::size_type path_size = element_path.size();
    
    pos_suffix1 = element_path.find(system_suffix);
    
    // check if system name found
    if (pos_suffix1 != std::string::npos)
    {
    	system_name = element_path.substr(0, pos_suffix1);
    	pos_suffix1 += (system_suffix.size()+ 1);
    	element_type = CdmwPlatformMngt::SYSTEM_TYPE;
    }
    else
    {
    	// element path is not valid
    	throw CdmwPlatformMngt::ElementPathInvalid();
    }
    
    // check if application name found
    if (pos_suffix1 < path_size)
    { 
        pos_suffix2 = element_path.find(application_suffix, pos_suffix1);
    
        // if application name found
        if (pos_suffix2 != std::string::npos)
        {
    	    application_name = element_path.substr(pos_suffix1, pos_suffix2-pos_suffix1);
    	    pos_suffix1 = pos_suffix2 + (application_suffix.size()+ 1);
    	    element_type = CdmwPlatformMngt::APPLICATION_TYPE;
        }
    }
    
    // check if host name found
    if (element_type == CdmwPlatformMngt::SYSTEM_TYPE && pos_suffix1 < path_size)
    { 
        pos_suffix2 = element_path.find(host_suffix, pos_suffix1);
    
        // if host name found
        if (pos_suffix2 != std::string::npos)
        {
    	    host_name = element_path.substr(pos_suffix1, pos_suffix2-pos_suffix1);
    	    pos_suffix1 = pos_suffix2 + (host_suffix.size()+ 1);
    	    element_type = CdmwPlatformMngt::HOST_TYPE;
        }
    }
    
    
    // check if process name found
    if (element_type == CdmwPlatformMngt::APPLICATION_TYPE && pos_suffix1 < path_size)
    { 
    	// first search host name
        pos_suffix2 = element_path.find(host_suffix, pos_suffix1);
    
        // if host name found
        if (pos_suffix2 != std::string::npos)
        {
    	    host_name = element_path.substr(pos_suffix1, pos_suffix2-pos_suffix1);
    	    pos_suffix1 = pos_suffix2 + (host_suffix.size()+ 1);
    	    
    	    // then search process name
    	    if (pos_suffix1 < path_size)
            { 
                pos_suffix2 = element_path.find(process_suffix, pos_suffix1);
                
                // if process name found
                if (pos_suffix2 != std::string::npos)
                {
    	            process_name = element_path.substr(pos_suffix1, pos_suffix2-pos_suffix1);
    	            pos_suffix1 = pos_suffix2 + (process_suffix.size()+ 1);
    	            element_type = CdmwPlatformMngt::PROCESS_TYPE;
    	        }
    	        else
                {
    	            // element path is not valid
    	            throw CdmwPlatformMngt::ElementPathInvalid();
                } 
    	    }
    	    else
            {
    	        // element path is not valid
    	        throw CdmwPlatformMngt::ElementPathInvalid();
            }    	    
        }
    }
    
    // check if entity name found
    if (pos_suffix1 < path_size)
    { 
        pos_suffix2 = element_path.find(entity_suffix, pos_suffix1);
    
        // if entity name found
        if (pos_suffix2 != std::string::npos)
        {
    	    entity_name = element_path.substr(pos_suffix1, pos_suffix2-pos_suffix1);
    	    element_type = CdmwPlatformMngt::ENTITY_TYPE;
        }
    }
    
    return element_type;
}

CdmwPlatformMngt::ElementType ManagedElementKey::Get_element_type (
                                       const std::string & element_path)
    throw (CdmwPlatformMngt::ElementPathInvalid)
{
	CdmwPlatformMngt::ElementType element_type;
	
    std::string::size_type pos_suffix1;
    std::string::size_type path_size = element_path.size();
    
    pos_suffix1 = element_path.find(system_suffix);
    
    // check if system name found
    if (pos_suffix1 != std::string::npos)
    {
    	pos_suffix1 += (system_suffix.size()+ 1);
    	element_type = CdmwPlatformMngt::SYSTEM_TYPE;
    }
    else
    {
    	// element path is not valid
    	throw CdmwPlatformMngt::ElementPathInvalid();
    }
    
    // check if application name found
    if (pos_suffix1 < path_size)
    { 
        pos_suffix1 = element_path.find(application_suffix, pos_suffix1);
    
        // if application name found
        if (pos_suffix1 != std::string::npos)
        {
    	    pos_suffix1 += (application_suffix.size()+ 1);
    	    element_type = CdmwPlatformMngt::APPLICATION_TYPE;
        }
    }
    
    // check if host name found
    if (element_type == CdmwPlatformMngt::SYSTEM_TYPE && pos_suffix1 < path_size)
    { 
        pos_suffix1 = element_path.find(host_suffix, pos_suffix1);
    
        // if host name found
        if (pos_suffix1 != std::string::npos)
        {
    	    pos_suffix1 += (host_suffix.size()+ 1);
    	    element_type = CdmwPlatformMngt::HOST_TYPE;
        }
    }
    
    
    // check if process name found
    if (element_type == CdmwPlatformMngt::APPLICATION_TYPE && pos_suffix1 < path_size)
    { 
    	// first search host name
        pos_suffix1 = element_path.find(host_suffix, pos_suffix1);
    
        // if host name found
        if (pos_suffix1 != std::string::npos)
        {
    	    pos_suffix1 += (host_suffix.size()+ 1);
    	    
    	    // then search process name
    	    if (pos_suffix1 < path_size)
            { 
                pos_suffix1 = element_path.find(process_suffix, pos_suffix1);
                
                // if process name found
                if (pos_suffix1 != std::string::npos)
                {
    	            pos_suffix1 += (process_suffix.size()+ 1);
    	            element_type = CdmwPlatformMngt::PROCESS_TYPE;
    	        }
    	        else
                {
    	            // element path is not valid
    	            throw CdmwPlatformMngt::ElementPathInvalid();
                } 
    	    }
    	    else
            {
    	        // element path is not valid
    	        throw CdmwPlatformMngt::ElementPathInvalid();
            }    	    
        }
    }
    
    // check if entity name found
    if (pos_suffix1 < path_size)
    { 
        pos_suffix1 = element_path.find(entity_suffix, pos_suffix1);
    
        // if entity name found
        if (pos_suffix1 != std::string::npos)
        {
    	    element_type = CdmwPlatformMngt::ENTITY_TYPE;
        }
    }
    
    return element_type;
}


CdmwPlatformMngt::ElementType ManagedElementKey::Get_parent_path (
                                   const std::string & element_path,
                                   std::string & element_name,                                         
                                   std::string & parent_path)
    throw (CdmwPlatformMngt::ElementPathInvalid)
{
	CdmwPlatformMngt::ElementType element_type;
	CdmwPlatformMngt::ElementType parent_type;
	
    element_name = "";
    parent_path = "";
    
    std::string::size_type pos_suffix1;
    std::string::size_type pos_suffix2;
    
    std::string::size_type pos_elem1;
    std::string::size_type pos_elem2;
    std::string::size_type pos_path;
    
    std::string::size_type path_size = element_path.size();
    
    pos_elem1 = 0;
    pos_elem2 = 0;
    pos_path = 0;
    
    // search system suffix
    pos_suffix1 = element_path.find(system_suffix);
    
    
    // check if system name found
    if (pos_suffix1 != std::string::npos)
    {
    	pos_elem1 = 0;
        pos_elem2 = pos_suffix1;
        pos_path = 0;
    
    	pos_suffix1 += (system_suffix.size()+ 1);
    	element_type = CdmwPlatformMngt::SYSTEM_TYPE;
    	parent_type = CdmwPlatformMngt::SYSTEM_TYPE;
    }
    else
    {
    	// element path is not valid
    	throw CdmwPlatformMngt::ElementPathInvalid();
    }
    
   
    // check if application name found
    if (pos_suffix1 < path_size)
    { 
        pos_suffix2 = element_path.find(application_suffix, pos_suffix1);
    
        // if application name found
        if (pos_suffix2 != std::string::npos)
        {
        	pos_elem1 = pos_suffix1;
            pos_elem2 = pos_suffix2;
            pos_path = pos_suffix1-1;
            
    	    pos_suffix1 = pos_suffix2 + (application_suffix.size()+ 1);
    	    
    	    parent_type = CdmwPlatformMngt::SYSTEM_TYPE;
    	    element_type = CdmwPlatformMngt::APPLICATION_TYPE;    	    
        }
    }
    
    // check if host name found
    if (element_type == CdmwPlatformMngt::SYSTEM_TYPE && pos_suffix1 < path_size)
    { 
        pos_suffix2 = element_path.find(host_suffix, pos_suffix1);
    
        // if host name found
        if (pos_suffix2 != std::string::npos)
        {
        	pos_elem1 = pos_suffix1;
            pos_elem2 = pos_suffix2;
            pos_path = pos_suffix1-1;
        
    	    pos_suffix1 = pos_suffix2 + (host_suffix.size()+ 1);
    	    
    	    parent_type = CdmwPlatformMngt::SYSTEM_TYPE;
    	    element_type = CdmwPlatformMngt::HOST_TYPE;    	    
        }
    }
    
    // check if process name found
    if (element_type == CdmwPlatformMngt::APPLICATION_TYPE && pos_suffix1 < path_size)
    { 
    	// first search host name
        pos_suffix2 = element_path.find(host_suffix, pos_suffix1);
    
        // if host name found
        if (pos_suffix2 != std::string::npos)
        {
        	pos_path = pos_suffix1-1;
        	
    	    pos_suffix1 = pos_suffix2 + (host_suffix.size()+ 1);
    	    
    	    // then search process name
    	    if (pos_suffix1 < path_size)
            { 
                pos_suffix2 = element_path.find(process_suffix, pos_suffix1);
                
                // if process name found
                if (pos_suffix2 != std::string::npos)
                {
            	    pos_elem1 = pos_suffix1;
                    pos_elem2 = pos_suffix2;
                           
    	            pos_suffix1 = pos_suffix2 + (process_suffix.size()+ 1);
    	            
    	            parent_type = CdmwPlatformMngt::APPLICATION_TYPE;
    	            element_type = CdmwPlatformMngt::PROCESS_TYPE;    	            
    	        }
    	        else
                {
    	            // element path is not valid
    	            throw CdmwPlatformMngt::ElementPathInvalid();
                }
            }
    	    else
            {
    	        // element path is not valid
    	        throw CdmwPlatformMngt::ElementPathInvalid();
            }    	    
        }
    }
    
    // check if entity name found
    if (pos_suffix1 < path_size)
    { 
        pos_suffix2 = element_path.find(entity_suffix, pos_suffix1);
    
        // if entity name found
        if (pos_suffix2 != std::string::npos)
        {
        	pos_elem1 = pos_suffix1;
            pos_elem2 = pos_suffix2;
            pos_path = pos_suffix1-1;
            
    	    // set parent type with the elemnt type found previously
    	    parent_type = element_type;
    	    element_type = CdmwPlatformMngt::ENTITY_TYPE;
        }
    }
    
    if (element_type == CdmwPlatformMngt::SYSTEM_TYPE)
    {
    	// element path is not valid
        throw CdmwPlatformMngt::ElementPathInvalid();        
    }
    
    parent_path = element_path.substr(0, pos_path);
    element_name = element_path.substr(pos_elem1, pos_elem2-pos_elem1);
    
    return parent_type;
}

void ManagedElementKey::Get_process_parent_path (
                                   const std::string & element_path,
                                   std::string & process_name, 
                                   std::string & host_name,                                        
                                   std::string & parent_path)
    throw (CdmwPlatformMngt::ElementPathInvalid)
{
    process_name = "";
    host_name = "";
    parent_path = "";
    
    std::string::size_type pos_suffix1;
    std::string::size_type pos_suffix2;
    
    std::string::size_type pos_elem1;
    std::string::size_type pos_elem2;
    std::string::size_type pos_elem3;
    std::string::size_type pos_elem4;
    std::string::size_type pos_path;
    
    std::string::size_type path_size = element_path.size();
    
    pos_elem1 = 0;
    pos_elem2 = 0;
    pos_elem3 = 0;
    pos_elem4 = 0;
    pos_path = 0;
    
    // search system suffix
    pos_suffix1 = element_path.find(system_suffix);
    
    
    // check if system name found
    if (pos_suffix1 != std::string::npos)
    {
    	pos_elem1 = 0;
        pos_elem2 = pos_suffix1;
        pos_path = 0;
    
    	pos_suffix1 += (system_suffix.size()+ 1);
    }
    else
    {
    	// element path is not valid
    	throw CdmwPlatformMngt::ElementPathInvalid();
    }
    

    // check if application name found
    if (pos_suffix1 < path_size)
    { 
        pos_suffix2 = element_path.find(application_suffix, pos_suffix1);
    
        // if application name found
        if (pos_suffix2 != std::string::npos)
        {
        	pos_elem1 = pos_suffix1;
            pos_elem2 = pos_suffix2;
            pos_path = pos_suffix1-1;
            
    	    pos_suffix1 = pos_suffix2 + (application_suffix.size()+ 1);    	    
        }
        else
        {
    	    // element path is not valid
    	    throw CdmwPlatformMngt::ElementPathInvalid();
        }
    }
    
    // check if process name found
    
    bool path_valid = false;
    
    if (pos_suffix1 < path_size)
    { 
    	// first search host name
        pos_suffix2 = element_path.find(host_suffix, pos_suffix1);
    
        // if host name found
        if (pos_suffix2 != std::string::npos)
        {
        	pos_path = pos_suffix1-1;
        	
        	pos_elem1 = pos_suffix1;
            pos_elem2 = pos_suffix2;
        	
    	    pos_suffix1 = pos_suffix2 + (host_suffix.size()+ 1);
    	    
    	    // then search process name
    	    if (pos_suffix1 < path_size)
            { 
                pos_suffix2 = element_path.find(process_suffix, pos_suffix1);
                
                // if process name found
                if (pos_suffix2 != std::string::npos)
                {
            	    pos_elem3 = pos_suffix1;
                    pos_elem4 = pos_suffix2;
                           
    	            path_valid = true;  	            
    	        }
            }
        }   	     
    }
    
    if (path_valid == false)
    {
        // element path is not valid
    	throw CdmwPlatformMngt::ElementPathInvalid();
    }
    
    
    parent_path = element_path.substr(0, pos_path);
    host_name = element_path.substr(pos_elem1, pos_elem2-pos_elem1);
    process_name = element_path.substr(pos_elem3, pos_elem4-pos_elem3);
}

}; // End namespace PlatformMngt
}; // End namespace Cdmw

