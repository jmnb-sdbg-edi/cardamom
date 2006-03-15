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


#ifndef INCL_PLATFORMMNGT_MANAGED_ELEMENT_KEY_HPP 
#define INCL_PLATFORMMNGT_MANAGED_ELEMENT_KEY_HPP 

#include <string>

#include "Foundation/common/Exception.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtCommonBase.stub.hpp"


namespace Cdmw
{

namespace PlatformMngt
{

/**
*Purpose:
*<p>    This class is used to build a unique managed element key
*
*Features:
*<p> Thread safe
*
*
*/
class ManagedElementKey
{
    public:

        /**
        * Purpose:
        * <p> create the system key
        * 
        * @param system_name      the element name of system
        *
        * @return the key of system
        */
        static
        std::string Get_system_path (const std::string & system_name);
        
        /**
        * Purpose:
        * <p> create the host key
        * 
        * @param system_name      the element name of system
        * @param host_name        the element name of host
        *
        * @return the key of host
        */
        static
        std::string Get_host_path (const std::string & system_name,
                                   const std::string & host_name);
        
        /**
        * Purpose:
        * <p> create the application key
        * 
        * @param system_name      the element name of system
        * @param application_name the element name of application
        *
        * @return the key of application
        */
        static
        std::string Get_application_path (
                                      const std::string & system_name,
                                      const std::string & application_name);
                                     
        /**
        * Purpose:
        * <p> create the process key
        * 
        * @param system_name      the element name of system
        * @param application_name the element name of application
        * @param host_name        the element name of host
        * @param process_name     the element name of process
        *
        * @return the key of process
        */
        static
        std::string Get_process_path (const std::string & system_name,
                                      const std::string & application_name,
                                      const std::string & host_name,
                                      const std::string & process_name);
                                      
        /**
        * Purpose:
        * <p> create the process key
        * 
        * @param parent_path      the path of the parent (application)
        * @param host_name        the element name of host
        * @param process_name     the element name of process
        *
        * @return the key of process
        *
        */
        static
        std::string Get_process_path (const std::string & parent_path,
                                      const std::string & host_name,
                                      const std::string & process_name);
                                      
                                      
        /**
        * Purpose:
        * <p> create the process relative key
        * 
        * @param host_name        the element name of host
        * @param process_name     the element name of process
        *
        * @return the relative key of process
        */
        static
        std::string Get_process_rel_key (
                                      const std::string & host_name,
                                      const std::string & process_name);
        
        
        /**
        * Purpose:
        * <p> check element names from relative process key
        * 
        * @param process_rel_key     the relative key of process
        *
        * @param process_name        the element name of process
        *
        * @return false if process key is complete
        *
        * @exception ElementPathInvalid
        */
        static
        bool Check_process_rel_key (const std::string & process_rel_key,
                                    std::string & process_name)
            throw (CdmwPlatformMngt::ElementPathInvalid);
            
                                          
        /**
        * Purpose:
        * <p> get element names from relative process key
        * 
        * @param process_rel_key     the relative key of process
        *
        * @param host_name        the element name of host
        * @param process_name     the element name of process
        *
        *
        * @exception ElementPathInvalid
        */
        static
        void Get_process_element_names (
                                   const std::string & process_rel_key,                                         
                                   std::string & host_name,
                                   std::string & process_name)
            throw (CdmwPlatformMngt::ElementPathInvalid);
                                     
        /**
        * Purpose:
        * <p> create the system entity key
        * 
        * @param system_name the element name of system
        * @param entity_name the element name of entity
        *
        * @return the key of system entity
        */
        static
        std::string Get_system_entity_path (
                                         const std::string & system_name,
                                         const std::string & entity_name);
        
        /**
        * Purpose:
        * <p> create the host entity key
        * 
        * @param system_name      the element name of system
        * @param host_name        the element name of host
        * @param entity_name      the element name of entity
        *
        * @return the key of host entity
        */
        static
        std::string Get_host_entity_path (
                                         const std::string & system_name,
                                         const std::string & host_name,
                                         const std::string & entity_name);
        
        /**
        * Purpose:
        * <p> create the application entity key
        * 
        * @param system_name      the element name of system
        * @param application_name the element name of application
        * @param entity_name      the element name of entity
        *
        * @return the key of application entity
        */
        static
        std::string Get_application_entity_path (
                                         const std::string & system_name,
                                         const std::string & application_name,
                                         const std::string & entity_name);
                                     
        /**
        * Purpose:
        * <p> create the process entity key
        * 
        * @param system_name      the element name of system
        * @param application_name the element name of application
        * @param host_name        the element name of host
        * @param process_name     the element name of process
        * @param entity_name      the element name of entity
        *
        * @return the key of process entity
        */
        static
        std::string Get_process_entity_path (
                                         const std::string & system_name,
                                         const std::string & application_name,
                                         const std::string & host_name,
                                         const std::string & process_name,
                                         const std::string & entity_name);
                                         
        /**
        * Purpose:
        * <p> create the entity key (system, host, application, process)
        * 
        * @param parent_path      the path of the parent (system, application, host, process)
        * @param entity_name      the element name of entity
        *
        * @return the key of entity
        *
        * @exception ElementPathInvalid
        */
        static
        std::string Get_entity_path (const std::string & parent_path,
                                     const std::string & entity_name)
            throw (CdmwPlatformMngt::ElementPathInvalid);
                                     
                                     
        /**
        * Purpose:
        * <p> get element names from element path
        * 
        * @param element_path     the element path of the supervised object
        *
        * Note : 
        * System element             : application name, host name, process_name, entity_name are null
        * Host element               : application name, process_name, entity_name are null
        * Application element        : host name, process_name, entity_name are null
        * Process element            : entity_name is null
        *
        * System entity element      : application name, host name, process_name are null
        * Host entity element        : application name, process_name are null
        * Application entity element : host name, process_name are null
        * Process entity element     : nothing is null
        *
        * @param system_name      the element name of system
        * @param application_name the element name of application
        * @param host_name        the element name of host
        * @param process_name     the element name of process
        * @param entity_name      the element name of entity
        *
        * @return the element type
        *
        * @exception ElementPathInvalid
        *
        */
        static
        CdmwPlatformMngt::ElementType Get_element_names (
                                   const std::string & element_path,                                         
                                   std::string & system_name,
                                   std::string & application_name,
                                   std::string & host_name,
                                   std::string & process_name,
                                   std::string & entity_name)
            throw (CdmwPlatformMngt::ElementPathInvalid);
            
        /**
        * Purpose:
        * <p> get element type from element path
        * 
        * @param element_path     the element path of the supervised object
        *
        * @return the element type
        *
        * @exception ElementPathInvalid
        *
        */
        static
        CdmwPlatformMngt::ElementType Get_element_type (
                                   const std::string & element_path)
            throw (CdmwPlatformMngt::ElementPathInvalid);
            
            
        /**
        * Purpose:
        * <p> get parent path from element path
        * 
        * @param element_path     the element path of the supervised object
        *
        * @param element_name     the name of the element
        * @param parent_path      the path of the parent
        *
        * @return the element type of parent
        *
        * @exception ElementPathInvalid
        *
        */
        static
        CdmwPlatformMngt::ElementType Get_parent_path (
                                   const std::string & element_path,
                                   std::string & element_name,                                         
                                   std::string & parent_path)
            throw (CdmwPlatformMngt::ElementPathInvalid);
            
            
        /**
        * Purpose:
        * <p> get parent path, process name and host name from process element path
        * 
        * @param element_path     the element path of the process object
        *
        * @param process_name     the name of the process
        * @param host_name        the name of the host
        * @param parent_path      the path of the parent
        *
        * @return the element type of parent
        *
        * @exception ElementPathInvalid
        *
        */
        static
        void Get_process_parent_path (
                                   const std::string & element_path,
                                   std::string & process_name, 
                                   std::string & host_name,                                        
                                   std::string & parent_path)
            throw (CdmwPlatformMngt::ElementPathInvalid);


    protected:
    

    private:
        
        
        /**
        * Purpose:
        * <p> Constructor
        *   
        */ 
        ManagedElementKey ()
            throw ();
            
        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~ManagedElementKey()
            throw();
            
                
        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        ManagedElementKey (const ManagedElementKey& rhs)
                throw();


        /**
        * Purpose:
        * <p> Assignement operator
        * 
        */ 
        ManagedElementKey& operator=(const ManagedElementKey& rhs)
                throw();

		
}; // End class ManagedElementKey 

} // End namespace PlatformMngt
} // End namespace Cdmw
#endif // INCL_PLATFORMMNGT_MANAGED_ELEMENT_KEY_HPP

