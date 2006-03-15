/* =========================================================================== *
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
 * =========================================================================== */

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <navigation/TestNavigation.hpp>
namespace Cdmw 
{

const std::string OPT_SUPERVISION = "--supervision";



// constructor
TestNavigation::TestNavigation(CORBA::ORB_ptr orb,
                               PortableServer::POA_ptr poa,
                               const std::string& name,
                               const std::string& supervision_corbaloc,
                               const std::string& host1,
                               const std::string& host2)
    : Testable(name),
      m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa))
{    
      m_system = CdmwPlatformMngt::System::_nil();
      m_application1 = CdmwPlatformMngt::Application::_nil();
      m_application2 = CdmwPlatformMngt::Application::_nil();
      m_supervision = supervision_corbaloc;
      m_hostname1 = host1;
      m_hostname2 = host2;
      m_process1 = CdmwPlatformMngt::Process::_nil();
      m_process2 = CdmwPlatformMngt::Process::_nil();
}

// destructor
TestNavigation::~TestNavigation()
{
}

// do_tests
void TestNavigation::do_tests()
{

    using namespace Cdmw;

    set_nbOfRequestedTestOK (36);
    

    // Get the CORBA reference of the system of the CDMW Supervision
    CORBA::Object_var systemObj = m_orb->string_to_object(m_supervision.c_str());
    
    m_system = CdmwPlatformMngt::System::_narrow(systemObj.in());
    if (CORBA::is_nil(m_system.in())) {
        TEST_FAILED();
    }
    
    TEST_INFO("Navigation - System to Host");
    TEST_INFO("Get the number of host in the system");
    TEST_INFO("system->get_number_of_hosts()");
    TEST_CHECK(m_system->get_number_of_hosts() == 2);

    try
    {
        TEST_INFO("Get the host from the system");
        TEST_INFO("system->get_host(hostname1)");
        m_host1 = m_system->get_host(m_hostname1.c_str());
        TEST_SUCCEED();
    }
    catch (const CdmwPlatformMngt::HostNotFound& ex)
    {
        TEST_FAILED();
    }
        
    TEST_CHECK(!strcmp(m_host1->name(), m_hostname1.c_str()));
    
    try
    {
        TEST_INFO("Get a unknown host from the system");
        TEST_INFO("system->get_host(\"UNKNOWN\")");
        m_host2 = m_system->get_host("UNKNOWN");
        TEST_FAILED();
    }
    catch (const CdmwPlatformMngt::HostNotFound& ex)
    {
        TEST_SUCCEED();
    }

    try
    {
        TEST_INFO("Get the host from the system");
        TEST_INFO("system->get_host(hostname2)");
        m_host2 = m_system->get_host(m_hostname2.c_str());
        TEST_SUCCEED();
    }
    catch (const CdmwPlatformMngt::HostNotFound& ex)
    {
        TEST_FAILED();
    }
      
    TEST_CHECK(!strcmp(m_host2->name(), m_hostname2.c_str()));


    

    TEST_INFO("Navigation - System to Application");
    try
    {
        TEST_INFO("Get an unknown application from the system");
        TEST_INFO("system->get_application(\"UNKNOWN\")");
        m_application1 =  m_system->get_application("UNKNOWN");
        TEST_FAILED();
    }
    catch(const CdmwPlatformMngt::ApplicationNotFound& ex)
    {
        TEST_SUCCEED();
    }
    
    try
    {
        TEST_INFO("Get an application from the system");
        TEST_INFO("system->get_application(\"NavigationApplication1\")");
        m_application1 =  m_system->get_application("NavigationApplication1");
        TEST_SUCCEED();
    }
    catch(const CdmwPlatformMngt::ApplicationNotFound& ex)
    {
        TEST_FAILED();
    }

    TEST_CHECK(!strcmp(m_application1->name(), "NavigationApplication1"));
    
    try
    {
        TEST_INFO("Get an application from the system");
        TEST_INFO("system->get_application(\"NavigationApplication2\")");
        m_application2 =  m_system->get_application("NavigationApplication2");
        TEST_SUCCEED();
    }
    catch(const CdmwPlatformMngt::ApplicationNotFound& ex)
    {
        TEST_FAILED();
    }

    TEST_CHECK(!strcmp(m_application2->name(), "NavigationApplication2"));

    
    TEST_INFO("Navigation - Application to process");
    try
    {
        TEST_INFO("Get an unknown process from the system");
        TEST_INFO("application1->get_process(\"UNKNOWN\", \"UNKNOWN\")");
        m_process1 = m_application1->get_process("UNKNOWN", "UNKNOWN");
        TEST_FAILED();
    }
    catch(const CdmwPlatformMngt::ProcessNotFound& ex)
    {
        TEST_SUCCEED();
    }

    try
    {
        TEST_INFO("Get a process from the application");
        TEST_INFO("application1->get_process(\"NavigationProcess1\",hostname1)");
        m_process1 = m_application1->get_process("NavigationProcess1", m_hostname1.c_str());
        TEST_SUCCEED();
    }
    catch(const CdmwPlatformMngt::ProcessNotFound& ex)
    {
        TEST_FAILED();
    }
    TEST_CHECK(!strcmp(m_process1->name(), "NavigationProcess1"));

    try
    {
        TEST_INFO("Get a process from the application");
        TEST_INFO("application2->get_process(\"NavigationProcess2\",hostname2)");
        m_process2 = m_application2->get_process("NavigationProcess2", m_hostname2.c_str());
        TEST_SUCCEED();
    }
    catch(const CdmwPlatformMngt::ProcessNotFound& ex)
    {
        TEST_FAILED();
    }
    TEST_CHECK(!strcmp(m_process2->name(), "NavigationProcess2"));

    TEST_INFO("Navigation - Process to Host");
    TEST_INFO("Get a host from the process");
    TEST_INFO("process1->get_host()");   
    m_host1 = m_process1->get_host();
    TEST_CHECK(!strcmp(m_host1->name(), m_hostname1.c_str()));

    TEST_INFO("Get a host from the process");
    TEST_INFO("process2->get_host()");
    m_host2 = m_process2->get_host();
    TEST_CHECK(!strcmp(m_host2->name(), m_hostname2.c_str()));











    TEST_INFO("Navigation - System to Entity");
    TEST_CHECK(m_system->get_number_of_entities() == 1);

    CdmwPlatformMngt:: Entity_var entity = CdmwPlatformMngt:: Entity::_nil();
    CORBA::String_var entityStatus;
    char * infoStatus;
    CORBA::String_out info_out (infoStatus);

    try
    {
        entity = m_system->get_entity("UNKNOWN"); 
        TEST_FAILED();
    }
    catch(const CdmwPlatformMngt::EntityNotFound& e)
    {
        TEST_SUCCEED();
    }

    try
    {
        std::cout<<__FILE__<<" "<<__LINE__<<std::endl;
        entity = m_system->get_entity("SYSTEM_ENTITY");
        std::cout<<__FILE__<<" "<<__LINE__<<std::endl;
        
        entity->set_status("OK", "the entity is OK");
        std::cout<<__FILE__<<" "<<__LINE__<<std::endl;
        entityStatus = entity->get_status(info_out);
        std::cout<<__FILE__<<" "<<__LINE__<<std::endl;

        std::cout << "entity name   :" << "SYSTEM_ENTITY" << std::endl;
        std::cout << "entity status :" << entityStatus.in() << std::endl;
        std::cout << "entity info   :" << infoStatus << std::endl;  
        TEST_SUCCEED();
    }
    catch(const CdmwPlatformMngt::EntityNotFound& e)
    {
        TEST_FAILED();
    }
   
    TEST_INFO("Navigation - Application to Entity");
    TEST_CHECK(m_application1->get_number_of_entities() == 1);

     try
    {
        entity = m_application1->get_entity("APPLICATION_ENTITY2"); 
        TEST_FAILED();
    }
    catch(const CdmwPlatformMngt::EntityNotFound& e)
    {
        TEST_SUCCEED();
    }

    try
    {
        entity = m_application1->get_entity("APPLICATION_ENTITY1");
        entity->set_status("OK", "the entity is OK");
        entityStatus = entity->get_status(info_out);

        std::cout << "entity name   :" << "APPLICATION_ENTITY1" << std::endl;
        std::cout << "entity status :" << entityStatus.in() << std::endl;
        std::cout << "entity info   :" << infoStatus << std::endl;  
        TEST_SUCCEED();
    }
    catch(const CdmwPlatformMngt::EntityNotFound& e)
    {
        TEST_FAILED();
    }


    TEST_CHECK(m_application2->get_number_of_entities() == 1);

    try
    {
        entity = m_application2->get_entity("APPLICATION_ENTITY1"); 
        TEST_FAILED();
    }
    catch(const CdmwPlatformMngt::EntityNotFound& e)
    {
        TEST_SUCCEED();
    }

    try
    {
        entity = m_application2->get_entity("APPLICATION_ENTITY2");
        entity->set_status("OK", "the entity is OK");
        entityStatus = entity->get_status(info_out);

        std::cout << "entity name   :" << "APPLICATION_ENTITY2" << std::endl;
        std::cout << "entity status :" << entityStatus.in() << std::endl;
        std::cout << "entity info   :" << infoStatus << std::endl;  
        TEST_SUCCEED();
    }
    catch(const CdmwPlatformMngt::EntityNotFound& e)
    {
        TEST_FAILED();
    }

    TEST_INFO("Navigation - Process to Entity");
    TEST_CHECK(m_process1->get_number_of_entities() == 1);

     try
    {
        entity = m_process1->get_entity("PROCESS_ENTITY2"); 
        TEST_FAILED();
    }
    catch(const CdmwPlatformMngt::EntityNotFound& e)
    {
        TEST_SUCCEED();
    }

    try
    {
        entity = m_process1->get_entity("PROCESS_ENTITY1");
        entity->set_status("OK", "the entity is OK");
        entityStatus = entity->get_status(info_out);

        std::cout << "entity name   :" << "PROCESS_ENTITY1" << std::endl;
        std::cout << "entity status :" << entityStatus.in() << std::endl;
        std::cout << "entity info   :" << infoStatus << std::endl;  
        TEST_SUCCEED();
    }
    catch(const CdmwPlatformMngt::EntityNotFound& e)
    {
        TEST_FAILED();
    }

    TEST_CHECK(m_process2->get_number_of_entities() == 1);

    try
    {
        entity = m_process2->get_entity("PROCESS_ENTITY1"); 
        TEST_FAILED();
    }
    catch(const CdmwPlatformMngt::EntityNotFound& e)
    {
        TEST_SUCCEED();
    }

    try
    {
        entity = m_process2->get_entity("PROCESS_ENTITY2");
        entity->set_status("OK", "the entity is OK");
        entityStatus = entity->get_status(info_out);

        std::cout << "entity name   :" << "PROCESS_ENTITY2" << std::endl;
        std::cout << "entity status :" << entityStatus.in() << std::endl;
        std::cout << "entity info   :" << infoStatus << std::endl;  
        TEST_SUCCEED();
    }
    catch(const CdmwPlatformMngt::EntityNotFound& e)
    {
        TEST_FAILED();
    }

    TEST_CHECK(m_host1->get_number_of_entities() == 1);

    try
    {
        entity = m_host1->get_entity("HOST_ENTITYx"); 
        TEST_FAILED();
    }
    catch(const CdmwPlatformMngt::EntityNotFound& e)
    {
        TEST_SUCCEED();
    }

    try
    {
        entity = m_host1->get_entity("HOST_ENTITY");
        entity->set_status("OK", "the entity is OK");
        entityStatus = entity->get_status(info_out);

        std::cout << "entity name   :" << "HOST_ENTITY" << std::endl;
        std::cout << "entity status :" << entityStatus.in() << std::endl;
        std::cout << "entity info   :" << infoStatus << std::endl;  
        TEST_SUCCEED();
    }
    catch(const CdmwPlatformMngt::EntityNotFound& e)
    {
        TEST_FAILED();
    }





}




} // end namespace Cdmw

