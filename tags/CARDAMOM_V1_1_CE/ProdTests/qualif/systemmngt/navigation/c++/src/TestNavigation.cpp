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
}

// destructor
TestNavigation::~TestNavigation()
{
}

// do_tests
void TestNavigation::do_tests()
{

    using namespace Cdmw;

    set_nbOfRequestedTestOK (33);
    

    // Get the CORBA reference of the system of the CDMW Supervision
    CORBA::Object_var systemObj = m_orb->string_to_object(m_supervision.c_str());
    
    m_system = CdmwPlatformMngt::System::_narrow(systemObj.in());
    if (CORBA::is_nil(m_system.in())) {
        TEST_FAILED();
    }
    
    TEST_INFO("Navigation - System to Host");
    TEST_CHECK(m_system->get_number_of_hosts() == 2);

    try
    {
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
        m_host2 = m_system->get_host("UNKNOWN");
        TEST_FAILED();
    }
    catch (const CdmwPlatformMngt::HostNotFound& ex)
    {
        TEST_SUCCEED();
    }

    try
    {
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
        m_application1 =  m_system->get_application("UNKNOWN");
        TEST_FAILED();
    }
    catch(const CdmwPlatformMngt::ApplicationNotFound& ex)
    {
        TEST_SUCCEED();
    }
    
    try
    {
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
        m_process1 = m_application1->get_process("UNKNOWN");
        TEST_FAILED();
    }
    catch(const CdmwPlatformMngt::ProcessNotFound& ex)
    {
        TEST_SUCCEED();
    }

    try
    {
        m_process1 = m_application1->get_process("NavigationProcess1");
        TEST_SUCCEED();
    }
    catch(const CdmwPlatformMngt::ProcessNotFound& ex)
    {
        TEST_FAILED();
    }
    TEST_CHECK(!strcmp(m_process1->name(), "NavigationProcess1"));

    try
    {
        m_process2 = m_application2->get_process("NavigationProcess2");
        TEST_SUCCEED();
    }
    catch(const CdmwPlatformMngt::ProcessNotFound& ex)
    {
        TEST_FAILED();
    }
    TEST_CHECK(!strcmp(m_process2->name(), "NavigationProcess2"));

    TEST_INFO("Navigation - Process to Host");
    m_host1 = m_process1->get_host();;
    TEST_CHECK(!strcmp(m_host1->name(), m_hostname1.c_str()));

    m_host2 = m_process2->get_host();;
    TEST_CHECK(!strcmp(m_host2->name(), m_hostname2.c_str()));
    


    TEST_INFO("Navigation - System to Entity");
    TEST_CHECK(m_system->get_number_of_entities() == 1);
    CdmwPlatformMngtEntity::EntityStatus entityStatus;
    CORBA::String_var entityInfo;

    try
    {
        entityStatus = m_system->get_entity_status("UNKNOWN", entityInfo); 
        TEST_FAILED();
    }
    catch(const CdmwPlatformMngtEntity::EntityNotFound& e)
    {
        TEST_SUCCEED();
    }

    try
    {
        entityStatus = m_system->get_entity_status("SYSTEM_ENTITY", entityInfo);
        std::cout << "entity name   :" << "SYSTEM_ENTITY" << std::endl;
        std::cout << "entity status :" << entityStatus << std::endl;
        std::cout << "entity info   :" << entityInfo.in() << std::endl;  
        TEST_SUCCEED();
    }
    catch(const CdmwPlatformMngtEntity::EntityNotFound& e)
    {
        TEST_FAILED();
    }
   
    TEST_INFO("Navigation - Application to Entity");
    TEST_CHECK(m_application1->get_number_of_entities() == 1);

     try
    {
        entityStatus = m_application1->get_entity_status("APPLICATION_ENTITY2", entityInfo); 
        TEST_FAILED();
    }
    catch(const CdmwPlatformMngtEntity::EntityNotFound& e)
    {
        TEST_SUCCEED();
    }

    try
    {
        entityStatus = m_application1->get_entity_status("APPLICATION_ENTITY1", entityInfo);
        std::cout << "entity name   :" << "APPLICATION_ENTITY1" << std::endl;
        std::cout << "entity status :" << entityStatus << std::endl;
        std::cout << "entity info   :" << entityInfo.in() << std::endl;  
        TEST_SUCCEED();
    }
    catch(const CdmwPlatformMngtEntity::EntityNotFound& e)
    {
        TEST_FAILED();
    }


    TEST_CHECK(m_application2->get_number_of_entities() == 1);

    try
    {
        entityStatus = m_application2->get_entity_status("APPLICATION_ENTITY1", entityInfo); 
        TEST_FAILED();
    }
    catch(const CdmwPlatformMngtEntity::EntityNotFound& e)
    {
        TEST_SUCCEED();
    }

    try
    {
        entityStatus = m_application2->get_entity_status("APPLICATION_ENTITY2", entityInfo);
        std::cout << "entity name   :" << "APPLICATION_ENTITY2" << std::endl;
        std::cout << "entity status :" << entityStatus << std::endl;
        std::cout << "entity info   :" << entityInfo.in() << std::endl;  
        TEST_SUCCEED();
    }
    catch(const CdmwPlatformMngtEntity::EntityNotFound& e)
    {
        TEST_FAILED();
    }

    TEST_INFO("Navigation - Process to Entity");
    TEST_CHECK(m_process1->get_number_of_entities() == 1);

     try
    {
        entityStatus = m_process1->get_entity_status("PROCESS_ENTITY2", entityInfo); 
        TEST_FAILED();
    }
    catch(const CdmwPlatformMngtEntity::EntityNotFound& e)
    {
        TEST_SUCCEED();
    }

    try
    {
        entityStatus = m_process1->get_entity_status("PROCESS_ENTITY1", entityInfo);
        std::cout << "entity name   :" << "PROCESS_ENTITY1" << std::endl;
        std::cout << "entity status :" << entityStatus << std::endl;
        std::cout << "entity info   :" << entityInfo.in() << std::endl;  
        TEST_SUCCEED();
    }
    catch(const CdmwPlatformMngtEntity::EntityNotFound& e)
    {
        TEST_FAILED();
    }

    TEST_CHECK(m_process2->get_number_of_entities() == 1);

     try
    {
        entityStatus = m_process2->get_entity_status("PROCESS_ENTITY1", entityInfo); 
        TEST_FAILED();
    }
    catch(const CdmwPlatformMngtEntity::EntityNotFound& e)
    {
        TEST_SUCCEED();
    }

    try
    {
        entityStatus = m_process2->get_entity_status("PROCESS_ENTITY2", entityInfo);
        std::cout << "entity name   :" << "PROCESS_ENTITY2" << std::endl;
        std::cout << "entity status :" << entityStatus << std::endl;
        std::cout << "entity info   :" << entityInfo.in() << std::endl;  
        TEST_SUCCEED();
    }
    catch(const CdmwPlatformMngtEntity::EntityNotFound& e)
    {
        TEST_FAILED();
    }


}




} // end namespace Cdmw

