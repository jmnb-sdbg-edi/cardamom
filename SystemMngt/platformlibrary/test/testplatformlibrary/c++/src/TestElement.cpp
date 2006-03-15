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


#include "testplatformlibrary/TestElement.hpp"

#include "SystemMngt/platformlibrary/ManagedElementKey.hpp"
#include "SystemMngt/platformlibrary/RWElementRepository_impl.hpp"

#include <string>
#include <exception>
#include <iostream>
#include <sstream>

using namespace CdmwPlatformMngt;
using namespace Cdmw::PlatformMngt;
using namespace Cdmw::OsSupport;

CPPUNIT_TEST_SUITE_REGISTRATION( TestElement ) ;


// ----------------------------------------------------------------------
// TestElement class.
// ----------------------------------------------------------------------
TestElement::TestElement ()
{
}

TestElement::~TestElement()
{
}

void TestElement::do_tests()
{
    // set number of requested successfull tests
// //     set_nbOfRequestedTestOK (43);
    
    try
    {
        std::string system = "system";
        std::string application = "app1";
        std::string process = "proc1";
        std::string host = "host1";
        std::string entity = "ent1";
        
        std::string system_buf;
        std::string application_buf;
        std::string process_buf;
        std::string host_buf;
        std::string entity_buf;
        
        std::string element_buf;
        std::string parent_buf;
        
        CdmwPlatformMngt::ElementType elem_type;
        
        //
        // create element repository
        //
        RWElementRepository_impl* p_elementRepository =
                                        new RWElementRepository_impl();
        
        
        //
        // System
        //
        
        TEST_INFO ("Check System Key");
        
        std::string sys_key = ManagedElementKey::Get_system_path (system);
 
        elem_type =
                ManagedElementKey::Get_element_names (
                        sys_key, system_buf, application_buf,
                        host_buf, process_buf, entity_buf);
                        
        std::cout << "system path : " << sys_key.c_str() << std::endl;
        
        if (elem_type == CdmwPlatformMngt::SYSTEM_TYPE)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        try
        {
            elem_type =
                ManagedElementKey::Get_parent_path (
                        sys_key, element_buf, parent_buf);
                        
            CPPUNIT_ASSERT(false);
        }
        catch (CdmwPlatformMngt::ElementPathInvalid &)
        {
            std::cout << "path : " << sys_key.c_str() 
                      << " has no parent" << std::endl;
        
            CPPUNIT_ASSERT(true);
        }
        
        //
        // Application
        //
        
        TEST_INFO ("Check Application Key");
        
        std::string app_key = 
          ManagedElementKey::Get_application_path (system, application);
 
        elem_type =
                ManagedElementKey::Get_element_names (
                        app_key, system_buf, application_buf,
                        host_buf, process_buf, entity_buf);
                        
        std::cout << "application path : " << app_key.c_str() << std::endl;
        
        if (elem_type == CdmwPlatformMngt::APPLICATION_TYPE &&
            application_buf == application)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        elem_type =
                ManagedElementKey::Get_parent_path (
                        app_key, element_buf, parent_buf);

        std::cout << "system path : " << parent_buf.c_str() << std::endl;
        
        if (elem_type == CdmwPlatformMngt::SYSTEM_TYPE &&
            parent_buf == sys_key &&
            element_buf == application)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        
        //
        // Process
        //
        
        TEST_INFO ("Check Process Key");
        
        std::string proc_key = 
          ManagedElementKey::Get_process_path (system, application, host, process);
 
        elem_type =
                ManagedElementKey::Get_element_names (
                        proc_key, system_buf, application_buf,
                        host_buf, process_buf, entity_buf);
                        
        std::cout << "process path : " << proc_key.c_str() << std::endl;
        
        if (elem_type == CdmwPlatformMngt::PROCESS_TYPE &&
            application_buf == application &&
            host_buf == host &&
            process_buf == process)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }

        elem_type =
                ManagedElementKey::Get_parent_path (
                        proc_key, element_buf, parent_buf);

        std::cout << "application path : " << parent_buf.c_str() << std::endl;
        
        if (elem_type == CdmwPlatformMngt::APPLICATION_TYPE &&
            parent_buf == app_key &&
            element_buf == process)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        ManagedElementKey::Get_process_parent_path (
                        proc_key, process_buf, host_buf, parent_buf);

        std::cout << "application path : " << parent_buf.c_str() << std::endl;
        
        if (parent_buf == app_key &&
            process_buf == process &&
            host_buf == host)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        //
        // 
        
        std::string proc_key2 =    
            ManagedElementKey::Get_process_path (app_key, host, process);    
               
        std::cout << "process path : " << proc_key2.c_str() << std::endl;
        
        if (proc_key2 == proc_key)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        //
        //
        
        std::string proc_rel_key =
            ManagedElementKey::Get_process_rel_key (host, process);
            
        std::cout << "process rel path : " << proc_rel_key.c_str() << std::endl;
        
        ManagedElementKey::Get_process_element_names (
                        proc_rel_key, host_buf, process_buf);
                        
        if (host_buf == host &&
            process_buf == process)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        //
        //
        bool result;
        std::string key = "";
        
        result = ManagedElementKey::Check_process_rel_key (
                        proc_rel_key, process_buf);
        
        if (!result && process_buf == process)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        key = "proc2";   // key is incomplete, host undefined
        result = ManagedElementKey::Check_process_rel_key (
                        key, process_buf);
        
        if (result && process_buf == key)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        std::string key_p = "proc2";
        key = "/" + key_p;   // key is incomplete, host undefined
        result = ManagedElementKey::Check_process_rel_key (
                        key, process_buf);
        
        if (result && process_buf == key_p)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        try
        {
            // error key is not a complete relative process path
            std::string key = "host2/";
            result = ManagedElementKey::Check_process_rel_key (
                        key, process_buf);
                                   
            CPPUNIT_ASSERT(false);
        }
        catch (CdmwPlatformMngt::ElementPathInvalid &)
        {
            std::cout << "process rel path : " << key.c_str() 
                      << " is not a relative process path" << std::endl;
        
            CPPUNIT_ASSERT(true);
        }
        
        try
        {
            // error key is not a complete relative process path
            std::string key = "/";
            result = ManagedElementKey::Check_process_rel_key (
                        key, process_buf);
                        
            CPPUNIT_ASSERT(false);
        }
        catch (CdmwPlatformMngt::ElementPathInvalid &)
        {
            std::cout << "process rel path : " << key.c_str() 
                      << " is not a relative process path" << std::endl;
        
            CPPUNIT_ASSERT(true);
        }
        
        try
        {
            // error key is not a complete relative process path
            std::string key = "";
            result = ManagedElementKey::Check_process_rel_key (
                        key, process_buf);
                        
            CPPUNIT_ASSERT(false);
        }
        catch (CdmwPlatformMngt::ElementPathInvalid &)
        {
            std::cout << "process rel path : " << key.c_str() 
                      << " is not a relative process path" << std::endl;
        
            CPPUNIT_ASSERT(true);
        }
        
        //
        // Host
        //
        
        TEST_INFO ("Check Host Key");
        
        std::string host_key = 
          ManagedElementKey::Get_host_path (system, host);
 
        elem_type =
                ManagedElementKey::Get_element_names (
                        host_key, system_buf, application_buf,
                        host_buf, process_buf, entity_buf);
                        
        std::cout << "host path : " << host_key.c_str() << std::endl;
        
        if (elem_type == CdmwPlatformMngt::HOST_TYPE &&
            host_buf == host)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        elem_type =
                ManagedElementKey::Get_parent_path (
                        host_key, element_buf, parent_buf);

        std::cout << "system path : " << parent_buf.c_str() << std::endl;
        
        if (elem_type == CdmwPlatformMngt::SYSTEM_TYPE &&
            parent_buf == sys_key &&
            element_buf == host)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        try
        {
            ManagedElementKey::Get_process_parent_path (
                        host_key, process_buf, host_buf, parent_buf);
                        
            CPPUNIT_ASSERT(false);
        }
        catch (CdmwPlatformMngt::ElementPathInvalid &)
        {
            std::cout << "path : " << host_key.c_str() 
                      << " is not a process path" << std::endl;
        
            CPPUNIT_ASSERT(true);
        }


        //
        // System entity
        // 
        
        TEST_INFO ("Check System Entity Key");
        
        std::string sys_ent_key = 
          ManagedElementKey::Get_system_entity_path (system, entity);
 
        elem_type =
                ManagedElementKey::Get_element_names (
                        sys_ent_key, system_buf, application_buf,
                        host_buf, process_buf, entity_buf);
                        
        std::cout << "system entity path : " << sys_ent_key.c_str() << std::endl;
        
        if (elem_type == CdmwPlatformMngt::ENTITY_TYPE &&
            entity_buf == entity)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        elem_type =
                ManagedElementKey::Get_parent_path (
                        sys_ent_key, element_buf, parent_buf);

        if (elem_type == CdmwPlatformMngt::SYSTEM_TYPE &&
            parent_buf == sys_key &&
            element_buf == entity)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        std::string sys_ent_key2 = 
          ManagedElementKey::Get_entity_path (sys_key, entity);
 
        std::cout << "sys entity path : " << sys_ent_key2.c_str() << std::endl;
        
        if (sys_ent_key2 == sys_ent_key)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        //
        // Application entity
        // 
        
        TEST_INFO ("Check Application Entity Key");
        
        std::string app_ent_key = 
          ManagedElementKey::Get_application_entity_path (system, application, entity);
 
        elem_type =
                ManagedElementKey::Get_element_names (
                        app_ent_key, system_buf, application_buf,
                        host_buf, process_buf, entity_buf);
                        
        std::cout << "application entity path : " << app_ent_key.c_str() << std::endl;
        
        if (elem_type == CdmwPlatformMngt::ENTITY_TYPE &&
            application_buf == application &&
            entity_buf == entity)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        elem_type =
                ManagedElementKey::Get_parent_path (
                        app_ent_key, element_buf, parent_buf);

        if (elem_type == CdmwPlatformMngt::APPLICATION_TYPE &&
            parent_buf == app_key &&
            element_buf == entity)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        std::string app_ent_key2 = 
          ManagedElementKey::Get_entity_path (app_key, entity);
 
        std::cout << "app entity path : " << app_ent_key2.c_str() << std::endl;
        
        if (app_ent_key2 == app_ent_key)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        //
        // Process entity
        // 
        
        TEST_INFO ("Check Process Entity Key");
        
        std::string proc_ent_key = 
          ManagedElementKey::Get_process_entity_path (system, application,
                                                      host, process, entity);
 
        elem_type =
                ManagedElementKey::Get_element_names (
                        proc_ent_key, system_buf, application_buf,
                        host_buf, process_buf, entity_buf);
                        
        std::cout << "process entity path : " << proc_ent_key.c_str() << std::endl;
        
        if (elem_type == CdmwPlatformMngt::ENTITY_TYPE &&
            application_buf == application &&
            host_buf == host &&
            process_buf == process &&
            entity_buf == entity)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        elem_type =
                ManagedElementKey::Get_parent_path (
                        proc_ent_key, element_buf, parent_buf);

        if (elem_type == CdmwPlatformMngt::PROCESS_TYPE &&
            parent_buf == proc_key &&
            element_buf == entity)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        std::string proc_ent_key2 = 
          ManagedElementKey::Get_entity_path (proc_key, entity);

        std::cout << "proc entity path : " << proc_ent_key2.c_str() << std::endl;
        
        if (proc_ent_key2 == proc_ent_key)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        //
        // Host entity
        // 
        
        TEST_INFO ("Check Host Entity Key");
        
        std::string host_ent_key = 
          ManagedElementKey::Get_host_entity_path (system, host, entity);
 
        elem_type =
                ManagedElementKey::Get_element_names (
                        host_ent_key, system_buf, application_buf,
                        host_buf, process_buf, entity_buf);
                        
        std::cout << "host entity path : " << host_ent_key.c_str() << std::endl;
        
        if (elem_type == CdmwPlatformMngt::ENTITY_TYPE &&
            host_buf == host &&
            entity_buf == entity)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        elem_type =
                ManagedElementKey::Get_parent_path (
                        host_ent_key, element_buf, parent_buf);

        if (elem_type == CdmwPlatformMngt::HOST_TYPE &&
            parent_buf == host_key &&
            element_buf == entity)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        std::string host_ent_key2 = 
          ManagedElementKey::Get_entity_path (host_key, entity);

        std::cout << "host entity path : " << host_ent_key2.c_str() << std::endl;
        
        if (host_ent_key2 == host_ent_key)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        //
        // unvalid path
        //
        
        std::string path = "xxxx";
        
        try
        { 
            elem_type =
                ManagedElementKey::Get_element_names (
                        path, system_buf, application_buf,
                        host_buf, process_buf, entity_buf);
                        
                        
            CPPUNIT_ASSERT(false);
        }
        catch (CdmwPlatformMngt::ElementPathInvalid &)
        {
            std::cout << "path : " << path.c_str() 
                      << " is not a valid element path" << std::endl;
        
            CPPUNIT_ASSERT(true);
        }
        
        
        //
        // store elements in repository
        //
        
        // set a null managed element object
        CdmwPlatformMngt::ManagedElement_var element_obj = 
                       CdmwPlatformMngt::ManagedElement::_nil();
        
        TEST_INFO ("Check Element repository");
        
        p_elementRepository->add_element(sys_key.c_str(), 
                                         CdmwPlatformMngt::SYSTEM_TYPE,
                                         element_obj.in());
        CPPUNIT_ASSERT(true);
                                         
        p_elementRepository->add_element(app_key.c_str(), 
                                         CdmwPlatformMngt::APPLICATION_TYPE,
                                         element_obj.in());
        CPPUNIT_ASSERT(true);
                                         
        p_elementRepository->add_element(proc_key.c_str(), 
                                         CdmwPlatformMngt::PROCESS_TYPE,
                                         element_obj.in());
        CPPUNIT_ASSERT(true);
                                         
        p_elementRepository->add_element(host_key.c_str(), 
                                         CdmwPlatformMngt::HOST_TYPE,
                                         element_obj.in());
        CPPUNIT_ASSERT(true);
        
        try
        {
            p_elementRepository->add_element(host_key.c_str(), 
                                         CdmwPlatformMngt::HOST_TYPE,
                                         element_obj.in());
                    
            CPPUNIT_ASSERT(false);
        }
        catch (CdmwPlatformMngt::ElementAlreadyExists &)
        {
            CPPUNIT_ASSERT(true);
        }
        
        p_elementRepository->remove_element(host_key.c_str());
        CPPUNIT_ASSERT(true);
        
        try
        {
            p_elementRepository->remove_element(host_key.c_str());
                    
            CPPUNIT_ASSERT(false);
        }
        catch (CdmwPlatformMngt::ManagedElementNotFound &)
        {
            CPPUNIT_ASSERT(true);
        }
        
        CdmwPlatformMngt::ManagedElement_var element_obj2 =
                 p_elementRepository->get_element(proc_key.c_str());
        CPPUNIT_ASSERT(true);   
        
        try
        {
            element_obj2 =
                 p_elementRepository->get_element(host_key.c_str());
                    
            CPPUNIT_ASSERT(false);
        }
        catch (CdmwPlatformMngt::ManagedElementNotFound &)
        {
            CPPUNIT_ASSERT(true);
        }  
        
        
        CdmwPlatformMngt::ElementRepositoryData_var element_data =
                 p_elementRepository->get_element_data(app_key.c_str());
                 
        if (element_data->element_type == CdmwPlatformMngt::APPLICATION_TYPE)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        
        element_data =
                 p_elementRepository->get_element_data(proc_key.c_str());
                 
        if (element_data->element_type == CdmwPlatformMngt::PROCESS_TYPE)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }        
        
        try
        {
            element_data =
                 p_elementRepository->get_element_data(host_key.c_str());
                    
            CPPUNIT_ASSERT(false);
        }
        catch (CdmwPlatformMngt::ManagedElementNotFound &)
        {
            CPPUNIT_ASSERT(true);
        }     

    }
    catch (const Cdmw::Exception& e)
    {
        std::cout << e.what() << std::endl;
        CPPUNIT_ASSERT(false);
    }

}


