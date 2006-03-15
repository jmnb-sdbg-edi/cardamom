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
#include <string>

#include "testmonoccmcif/TestBasicDemo.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "CCMContainer/ccmcontainer/ConfigValue_impl.hpp"
#include "testmonoccmcif/Basic_cif.skel.hpp"
#include "CCMContainer/idllib/CdmwDeployment.skel.hpp"
#include "testmonoccmcif/Event_impl.hpp"

namespace
{
    const char* SERVER_HOME_REP_ID = "IDL:thalesgroup.com/Cdmw_BasicDemo/ServerHome:1.0";
    const char* SERVER_HOME_SERVANT_NAME = "Cdmw.CCM.CIF.CdmwBasicDemo.SessionServerHome_impl";
    const char* SERVER_HOME_UID = "ServerHome_1";
    const char* SERVER_HOME_ENTRY_POINT = "create_CCM_ServerHome";
    const char* SERVER_NAME = "R2D2";

    const char* CLIENT_HOME_REP_ID = "IDL:thalesgroup.com/Cdmw_BasicDemo/ClientHome:1.0";
    const char* CLIENT_HOME_SERVANT_NAME = "Cdmw.CCM.CIF.CdmwBasicDemo.SessionClientHome_impl";
    const char* CLIENT_HOME_UID = "ClientHome_1";
    const char* CLIENT_HOME_ENTRY_POINT = "create_CCM_ClientHome";

    const char* DISPLAY_FACET_NAME = "for_clients";
    const char* INVALID_FACET_NAME = "invalid_facet";

    const char* DISPLAY_RECEP_NAME = "to_server";
    const char* REGISTER_RECEP_NAME = "to_register";
    const char* INVALID_RECEP_NAME = "invalid_recep";

    const char* EVENT_REP_ID = "IDL:acme.com/BasicDemo/Event:1.0";
    const char* EVENT_ENTRYPOINT = "createEventFactory";


}; // End anonymous namespace

namespace Cdmw
{
namespace CCM
{
namespace CIF
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/


TestBasicDemo::TestBasicDemo(CORBA::ORB_ptr orb, const char* containerIOR)
   throw(CORBA::SystemException)
    :Testable("Cdmw::CCM::CIF::BasicDemo"),
     m_orb(CORBA::ORB::_duplicate(orb))
{
    try
    {
        CORBA::Object_var obj = m_orb->string_to_object(containerIOR);

        if (CORBA::is_nil(obj.in()))
        {
            TEST_INFO("TEST FAILED : exception raised during test initialisation!");
        }
        else
        {
            m_container = Components::Deployment::Container::_narrow(obj.in());
        }
    }
    catch (const CORBA::SystemException & e)
    {
        TEST_INFO("TEST FAILED : system exception raised during test initialisation!");
        throw;
    }
    catch (...)
    {
        TEST_INFO("TEST FAILED : unexpected exception raised during test initialisation!");
    }
}


TestBasicDemo::~TestBasicDemo()
    throw()
{
    // does nothing
}


TestBasicDemo::TestBasicDemo(const TestBasicDemo& rhs)
    throw()
    :Testable("Cdmw::CCM::CIF::BasicDemo"),
     m_orb(CORBA::ORB::_duplicate(rhs.m_orb)),
     m_container(Components::Deployment::Container::_duplicate(rhs.m_container))
{
    // does nothing
}
        
TestBasicDemo&
TestBasicDemo::operator=(const TestBasicDemo& rhs)
    throw()
{
    if (this != &rhs) {
        m_orb = CORBA::ORB::_duplicate(rhs.m_orb.in());
        m_container = Components::Deployment::Container::_duplicate(rhs.m_container.in());
    }
    return *this;
}

void TestBasicDemo::do_tests()        
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (52);
    
    bool test_ok=false;
    Components::CCMHome_var server_home;
    BasicDemo::ServerHome_var basic_server_home;
    Components::CCMObject_var ccm_server_comp;
    BasicDemo::Server_var basic_server_comp;
    BasicDemo::Server_var basic_server_comp2;
    BasicDemo::Server_var basic_server_comp3;

    Components::CCMHome_var client_home;
    BasicDemo::ClientHome_var basic_client_home;
    Components::CCMObject_var ccm_client_comp;
    BasicDemo::Client_var basic_client_comp;

    CORBA::Object_var display_facet_obj;


    // This tests CIF classes
    //-------------------------------------------------
    TEST_INFO("Testing CIF classes");
    
    try 
    {    
        TEST_INFO("Install a ServerHome");
        test_ok=false;

        try
        {
            Components::ConfigValues config_values(8);
            config_values.length(5);
            CORBA::Any value;
            value <<= CdmwDeployment::SESSION;
            config_values[0] 
                = new Container::ConfigValue_impl(CdmwDeployment::COMPONENT_KIND, 
                                                   value);

            value <<= CdmwDeployment::CONTAINER_LIFETIME;
            config_values[1]
                = new Container::ConfigValue_impl(CdmwDeployment::SERVANT_LIFETIME, 
                                                   value);

            value <<= CdmwDeployment::MULTITHREAD;
            config_values[2]
                = new Container::ConfigValue_impl(CdmwDeployment::THREADING_POLICY, 
                                                   value);

            value <<= SERVER_HOME_REP_ID;
            config_values[3]
                = new Container::ConfigValue_impl(CdmwDeployment::HOME_REPOSITORY_ID, 
                                                   value);

            value <<= SERVER_HOME_SERVANT_NAME;
            config_values[4]
                = new Container::ConfigValue_impl(CdmwDeployment::HOME_SERVANT_CLASSNAME, 
                                                   value);
            server_home 
               = m_container->install_home(SERVER_HOME_UID,
                                           SERVER_HOME_ENTRY_POINT,
                                           config_values);
            test_ok=true;
        }
        catch (const CORBA::BAD_PARAM& e)
        {
            TEST_INFO(e);
        }
        catch (const Components::Deployment::InstallationFailure& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: Components::Deployment::InstallationFailure " << e.reason << std::ends;
            TEST_INFO(os.str());
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected system exception raised: " << e << std::ends;
            TEST_INFO(os.str());
        }
        catch (const CORBA::UserException& e)
        {
            std::ostringstream os;
            os << "Unexpected User exception raised: " << e << std::ends;
            TEST_INFO(os.str());
        }
        TEST_CHECK(test_ok);
        
        TEST_INFO("Install a ClientHome");
        test_ok=false;

        try
        {
            Components::ConfigValues config_values(8);
            config_values.length(6);
            CORBA::Any value;
            value <<= CdmwDeployment::SESSION;
            config_values[0] = new Container::ConfigValue_impl(CdmwDeployment::COMPONENT_KIND, 
                                                   value);

            value <<= CdmwDeployment::CONTAINER_LIFETIME;
            config_values[1] = new Container::ConfigValue_impl(CdmwDeployment::SERVANT_LIFETIME, 
                                                   value);

            value <<= CdmwDeployment::MULTITHREAD;
            config_values[2] = new Container::ConfigValue_impl(CdmwDeployment::THREADING_POLICY, 
                                                   value);

            value <<= CLIENT_HOME_REP_ID;
            config_values[3] = new Container::ConfigValue_impl(CdmwDeployment::HOME_REPOSITORY_ID, 
                                                   value);

            value <<= CLIENT_HOME_SERVANT_NAME;
            config_values[4] = new Container::ConfigValue_impl(CdmwDeployment::HOME_SERVANT_CLASSNAME, 
                                                   value);

            CORBA::ULong lg = 1L;
            CdmwDeployment::ValuetypeFactoryDescriptionSeq seq(lg);
            seq.length(lg);
            // Event valuetype
            {
                CdmwDeployment::ValuetypeFactoryDescription desc;
                desc.repid = EVENT_REP_ID;
                desc.factory_entrypoint = EVENT_ENTRYPOINT;
                seq[0]=desc;
            }            
            value <<= seq;
            config_values[5] = new Container::ConfigValue_impl(CdmwDeployment::VALUETYPE_FACTORY_DEPENDENCIES,
                                                   value);

            client_home 
               = m_container->install_home(CLIENT_HOME_UID,
                                           CLIENT_HOME_ENTRY_POINT,
                                           config_values);
            test_ok=true;
        }
        catch (const CORBA::BAD_PARAM& e)
        {
            TEST_INFO(e);
        }
        catch (const Components::Deployment::InstallationFailure& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: Components::Deployment::InstallationFailure " << e.reason << std::ends;
            TEST_INFO(os.str());
        }
        TEST_CHECK(test_ok);


        TEST_INFO("Create a Server component using create_component() operation");
        test_ok=false;
        try
        {
            Components::KeylessCCMHome_var keyless_server_home 
                = Components::KeylessCCMHome::_narrow(server_home.in());

            if (CORBA::is_nil(keyless_server_home.in()))
            {
                TEST_INFO("ERROR: Created home is not a keyless home!");
            }
            else
            {
                ccm_server_comp
                   = keyless_server_home->create_component();

                basic_server_comp
                    = BasicDemo::Server::_narrow(ccm_server_comp.in());

                if (CORBA::is_nil(basic_server_comp.in()))
                {
                    TEST_INFO("ERROR: Created component is not a BasicDemo::Server!")
                }
                else
                {
                    test_ok=true;
                }
            }
        }
        catch (const Components::CreateFailure& e)
        {
            TEST_INFO("Unexpected exception raised: CreateFailure");
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
        }
        TEST_CHECK(test_ok);
        
        TEST_INFO("Create a Client component using create_component() operation");
        test_ok=false;
        try
        {
            Components::KeylessCCMHome_var keyless_client_home 
                = Components::KeylessCCMHome::_narrow(client_home.in());

            if (CORBA::is_nil(keyless_client_home.in()))
            {
                TEST_INFO("ERROR: Created home is not a keyless home!");
            }
            else
            {
                ccm_client_comp
                   = keyless_client_home->create_component();

                basic_client_comp
                    = BasicDemo::Client::_narrow(ccm_client_comp.in());

                if (CORBA::is_nil(basic_client_comp.in()))
                {
                    TEST_INFO("ERROR: Created component is not a BasicDemo::Client!")
                }
                else
                {
                    test_ok=true;
                }
            }
        }
        catch (const Components::CreateFailure& e)
        {
            TEST_INFO("Unexpected exception raised: CreateFailure");
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
        }
        TEST_CHECK(test_ok);
        
        TEST_INFO("Create other Server component using create() operation");
        test_ok=false;
        try
        {
            basic_server_home 
                = BasicDemo::ServerHome::_narrow(server_home.in());

            if (CORBA::is_nil(basic_server_home.in()))
            {
                TEST_INFO("Home is not a BasicDemo::ServerHome!");
            }
            else
            {
                basic_server_comp2 = basic_server_home->create();
                basic_server_comp3 = basic_server_home->create();

                if (CORBA::is_nil(basic_server_comp2.in()))
                {
                    TEST_INFO("ERROR: basic_server_comp2 is nil!");
                }
                else
                {
                    if (CORBA::is_nil(basic_server_comp3.in()))
                    {
                        TEST_INFO("ERROR: basic_server_comp3 is nil!");
                    }
                    else
                    {
                        test_ok=true;
                    }
                }
            }
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Check Server component is same using same_component() operation");
        test_ok=false;
        try
        {
            CORBA::Boolean result1
                = basic_server_comp2->same_component(basic_server_comp3.in());
            CORBA::Boolean result2
                = basic_server_comp3->same_component(basic_server_comp3.in());
            
            if (result1 == true)
            {
                TEST_INFO("ERROR: basic_server_comp2 is not the same component!");
            }
            else
            {
                if (result2 == false)
                {
                    TEST_INFO("ERROR: basic_server_comp3 is the same component!");
                }
                else
                {
                    test_ok=true;
                }
            }
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Remove a Server component using remove_component() operation");
        test_ok=false;
        try
        {
            server_home->remove_component(basic_server_comp2.in());
            test_ok=true;
        }
        catch (const Components::RemoveFailure& e)
        {
            TEST_INFO("Unexpected exception raised: RemoveFailure");
            test_ok=false;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);


        TEST_INFO("Remove again a same Server component using remove_component() operation");
        test_ok=false;
        try
        {
            server_home->remove_component(basic_server_comp2.in());
        }
        catch (const Components::RemoveFailure& e)
        {
            std::ostringstream os;
            os << "Expected exception raised: RemoveFailure" << std::ends;
            TEST_INFO(os.str());
            test_ok=true;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Testing Navigation operation on Server component...");
        TEST_INFO("Get_all_facets operation on Server component...");
        test_ok=false;
        try
        {
            Components::FacetDescriptions_var facet_desc
                = basic_server_comp3->get_all_facets();

            if (facet_desc->length() != 1)
            {
                TEST_INFO("ERROR: Unexpected number of facet found for Server component");
            }
            else
            {
                test_ok=true;
            }
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Get_all_facets operation on Client component...");
        test_ok=false;
        try
        {
            Components::FacetDescriptions_var facet_desc
                = basic_client_comp->get_all_facets();

            if (facet_desc->length() != 0)
            {
                TEST_INFO("ERROR: Unexpected facet found for Client component");
            }
            else
            {
                test_ok=true;
            }
        }
        catch (const Components::RemoveFailure& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: RemoveFailure" << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Get_named_facets operation on Server component...");
        test_ok=false;
        try
        {
            Components::NameList facet_name_list(2);
            facet_name_list.length(1);
            facet_name_list[0] = DISPLAY_FACET_NAME;
            Components::FacetDescriptions_var facet_desc
                = basic_server_comp->get_named_facets(facet_name_list);

            if (facet_desc->length() != 1)
            {
                TEST_INFO("ERROR: Unexpected number of facet found for Server component");
            }
            else
            {
                test_ok=true;
            }
        }
        catch (const Components::InvalidName& e)
        {
            TEST_INFO("Unexpected exception raised: InvalidName");
            test_ok=false;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Get_named_facets operation on Server component with invalid name...");
        test_ok=false;
        try
        {
            Components::NameList facet_name_list(3);
            facet_name_list.length(2);
            facet_name_list[0] = DISPLAY_FACET_NAME;
            facet_name_list[1] = INVALID_FACET_NAME;

            Components::FacetDescriptions_var facet_desc
                = basic_server_comp->get_named_facets(facet_name_list);
        }
        catch (const Components::InvalidName& e)
        {
            TEST_INFO("Expected exception raised: InvalidName");
            test_ok=true;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("provide_facet operation on Server component...");
        test_ok=false;
        try
        {
            display_facet_obj
                = basic_server_comp->provide_facet(DISPLAY_FACET_NAME);

            if (CORBA::is_nil(display_facet_obj.in()))
            {
                TEST_INFO("ERROR: No facet found for Server component");
            }
            else
            {
                test_ok=true;
            }
        }
        catch (const Components::InvalidName& e)
        {
            TEST_INFO("Unexpected exception raised: InvalidName");
            test_ok=false;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("provide_facet operation on Server component with invalid name...");
        test_ok=false;
        try
        {
            CORBA::Object_var facet_obj
                = basic_server_comp->provide_facet(INVALID_FACET_NAME);
        }
        catch (const Components::InvalidName& e)
        {
            TEST_INFO("Expected exception raised: InvalidName");
            test_ok=true;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);        

        TEST_INFO("Testing Receptacle operation on Server component...");
        TEST_INFO("Get_all_receptacles operation on Server component...");
        test_ok=false;
        try
        {
            Components::ReceptacleDescriptions_var recep_desc
                = basic_server_comp3->get_all_receptacles();

            if (recep_desc->length() != 0)
            {
                TEST_INFO("ERROR: Unexpected receptacle found for Server component");
            }
            else
            {
                test_ok=true;
            }
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Get_all_receptacles operation on Client component...");
        test_ok=false;
        try
        {
            Components::ReceptacleDescriptions_var recep_desc
                = basic_client_comp->get_all_receptacles();

            if (recep_desc->length() != 2)
            {
                TEST_INFO("ERROR: Unexpected number of receptacle found for Client component");
            }
            else
            {
                test_ok=true;
            }
        }
        catch (const Components::RemoveFailure& e)
        {
            TEST_INFO("Unexpected exception raised: RemoveFailure");
            test_ok=false;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Get_named_receptacles operation on Client component...");
        test_ok=false;
        try
        {
            Components::NameList recep_name_list(3);
            recep_name_list.length(2);
            recep_name_list[0] = DISPLAY_RECEP_NAME;
            recep_name_list[1] = REGISTER_RECEP_NAME;
            Components::ReceptacleDescriptions_var recep_desc
                = basic_client_comp->get_named_receptacles(recep_name_list);

            if (recep_desc->length() != 2)
            {
                TEST_INFO("ERROR: Unexpected number of receptacle found for Client component");
            }
            else
            {
                test_ok=true;
            }
        }
        catch (const Components::InvalidName& e)
        {
            TEST_INFO("Unexpected exception raised: InvalidName");
            test_ok=false;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Get_named_receptacles operation on Client component with invalid name...");
        test_ok=false;
        try
        {
            Components::NameList recep_name_list(3);
            recep_name_list.length(2);
            recep_name_list[0] = DISPLAY_RECEP_NAME;
            recep_name_list[1] = INVALID_RECEP_NAME;

            Components::ReceptacleDescriptions_var recep_desc
                = basic_client_comp->get_named_receptacles(recep_name_list);
        }
        catch (const Components::InvalidName& e)
        {
            TEST_INFO("Expected exception raised: InvalidName");
            test_ok=true;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Get_connections operation on Client component...");
        test_ok=false;
        try
        {
            Components::ConnectionDescriptions_var conx_desc
                = basic_client_comp->get_connections(DISPLAY_RECEP_NAME);

            if (conx_desc->length() != 0)
            {
                TEST_INFO("ERROR: Unexpected connection found for Client component");
            }
            else
            {
                test_ok=true;
            }
        }
        catch (const Components::InvalidName& e)
        {
            TEST_INFO("Unexpected exception raised: InvalidName");
            test_ok=false;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Get_connections operation on Client component with invalid name...");
        test_ok=false;
        try
        {
            Components::ConnectionDescriptions_var conx_desc
                = basic_client_comp->get_connections(INVALID_RECEP_NAME);
        }
        catch (const Components::InvalidName& e)
        {
            TEST_INFO("Expected exception raised: InvalidName");
            test_ok=true;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Connect operation on Client component with invalid name...");
        test_ok=false;
        try
        {
            Components::Cookie_var ck
                = basic_client_comp->connect(INVALID_RECEP_NAME, display_facet_obj.in());
        }
        catch (const Components::InvalidName& )
        {
            TEST_INFO("Expected exception raised: InvalidName");
            test_ok=true;
        }
        catch (const Components::InvalidConnection& )
        {
            TEST_INFO("Unexpected exception raised: InvalidConnection");
        }
        catch (const Components::AlreadyConnected& )
        {
            TEST_INFO("Unexpected exception raised: AlreadyConnected");
        }
        catch (const Components::ExceededConnectionLimit& )
        {
            TEST_INFO("Unexpected exception raised: ExceededConnectionLimit");
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Connect operation on Client component with invalid connection...");
        test_ok=false;
        try
        {
            Components::Cookie_var ck
                = basic_client_comp->connect(REGISTER_RECEP_NAME, ccm_client_comp.in());
        }
        catch (const Components::InvalidName& )
        {
            TEST_INFO("Unexpected exception raised: InvalidName");
        }
        catch (const Components::InvalidConnection& )
        {
            TEST_INFO("Expected exception raised: InvalidConnection");
            test_ok=true;
        }
        catch (const Components::AlreadyConnected& )
        {
            TEST_INFO("Unexpected exception raised: AlreadyConnected");
        }
        catch (const Components::ExceededConnectionLimit& )
        {
            TEST_INFO("Unexpected exception raised: ExceededConnectionLimit");
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Connect operation on Client component...");
        test_ok=false;
        try
        {
            Components::Cookie_var ck
                = basic_client_comp->connect(DISPLAY_RECEP_NAME,
                                             display_facet_obj.in());
            test_ok=true;
        }
        catch (const Components::InvalidName& )
        {
            TEST_INFO("Unexpected exception raised: InvalidName");
        }
        catch (const Components::InvalidConnection& )
        {
            TEST_INFO("Expected exception raised: InvalidConnection");
        }
        catch (const Components::AlreadyConnected& )
        {
            TEST_INFO("Unexpected exception raised: AlreadyConnected");
        }
        catch (const Components::ExceededConnectionLimit& )
        {
            TEST_INFO("Unexpected exception raised: ExceededConnectionLimit");
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Connect operation again on Client component...");
        test_ok=false;
        try
        {
            Components::Cookie_var ck
                = basic_client_comp->connect(DISPLAY_RECEP_NAME,
                                             display_facet_obj.in());
        }
        catch (const Components::InvalidName& )
        {
            TEST_INFO("Unexpected exception raised: InvalidName");
        }
        catch (const Components::InvalidConnection& )
        {
            TEST_INFO("Unexpected exception raised: InvalidConnection");
        }
        catch (const Components::AlreadyConnected& )
        {
            TEST_INFO("Expected exception raised: AlreadyConnected");
            test_ok=true;
        }
        catch (const Components::ExceededConnectionLimit& )
        {
            TEST_INFO("Unexpected exception raised: ExceededConnectionLimit");
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Disconnect operation on Client component...");
        test_ok=false;
        try
        {
            basic_client_comp->disconnect(DISPLAY_RECEP_NAME, 0);
            test_ok=true;
        }
        catch (const Components::InvalidName& )
        {
            TEST_INFO("Unexpected exception raised: InvalidName");
        }
        catch (const Components::InvalidConnection& )
        {
            TEST_INFO("Unexpected exception raised: InvalidConnection");
        }
        catch (const Components::CookieRequired& )
        {
            TEST_INFO("Unexpected exception raised: CookieRequired");
        }
        catch (const Components::NoConnection& )
        {
            TEST_INFO("Unexpected exception raised: NoConnection");
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Disconnect operation on Client component with an invalid name...");
        test_ok=false;
        try
        {
            basic_client_comp->disconnect(INVALID_RECEP_NAME, 0);
        }
        catch (const Components::InvalidName& )
        {
            TEST_INFO("Expected exception raised: InvalidName");
            test_ok=true;
        }
        catch (const Components::InvalidConnection& )
        {
            TEST_INFO("Unexpected exception raised: InvalidConnection");
        }
        catch (const Components::CookieRequired& )
        {
            TEST_INFO("Unexpected exception raised: CookieRequired");
        }
        catch (const Components::NoConnection& )
        {
            TEST_INFO("Unexpected exception raised: NoConnection");
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Disconnect operation again on Client component...");
        test_ok=false;
        try
        {
             basic_client_comp->disconnect(DISPLAY_RECEP_NAME, 0);
        }
        catch (const Components::InvalidName& )
        {
            TEST_INFO("Unexpected exception raised: InvalidName");
        }
        catch (const Components::InvalidConnection& )
        {
            TEST_INFO("Unexpected exception raised: InvalidConnection");
        }
        catch (const Components::CookieRequired& )
        {
            TEST_INFO("Unexpected exception raised: CookieRequired");
        }
        catch (const Components::NoConnection& )
        {
            TEST_INFO("Expected exception raised: NoConnection");
            test_ok=true;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Connect operation again on Client component...");
        test_ok=false;
        try
        {
            Components::Cookie_var ck
                = basic_client_comp->connect(DISPLAY_RECEP_NAME,
                                             display_facet_obj.in());
            test_ok=true;
        }
        catch (const Components::InvalidName& )
        {
            TEST_INFO("Unexpected exception raised: InvalidName");
        }
        catch (const Components::InvalidConnection& )
        {
            TEST_INFO("Unexpected exception raised: InvalidConnection");
        }
        catch (const Components::AlreadyConnected& )
        {
            TEST_INFO("Unexpected exception raised: AlreadyConnected");
        }
        catch (const Components::ExceededConnectionLimit& )
        {
            TEST_INFO("Unexpected exception raised: ExceededConnectionLimit");
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Configuration_complete operation on Client component...");
        test_ok=false;
        try
        {
            basic_client_comp->configuration_complete();
        }
        catch (const Components::InvalidConfiguration &)
        {
            TEST_INFO("Expected exception raised: InvalidConfiguration");
            test_ok=true;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Connect operation on Client component...");
        test_ok=false;
        try
        {
            Components::Cookie_var ck
                = basic_client_comp->connect(REGISTER_RECEP_NAME,
                                             basic_server_comp.in());
            test_ok=true;
        }
        catch (const Components::InvalidName& )
        {
            TEST_INFO("Unexpected exception raised: InvalidName");
        }
        catch (const Components::InvalidConnection& )
        {
            TEST_INFO("Unexpected exception raised: InvalidConnection");
        }
        catch (const Components::AlreadyConnected& )
        {
            TEST_INFO("Unexpected exception raised: AlreadyConnected");
        }
        catch (const Components::ExceededConnectionLimit& )
        {
            TEST_INFO("Unexpected exception raised: ExceededConnectionLimit");
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Get_connections operation on Client component...");
        test_ok=false;
        try
        {
            Components::ConnectionDescriptions_var conx_desc
                = basic_client_comp->get_connections(REGISTER_RECEP_NAME);

            if (conx_desc->length() != 1)
            {
                TEST_INFO("ERROR: Unexpected number of connection found for Client component => " << conx_desc->length());
            }
            else
            {
                test_ok=true;
            }
        }
        catch (const Components::InvalidName& e)
        {
            TEST_INFO("Unexpected exception raised: InvalidName");
            test_ok=false;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Get_all_publishers operation on Client component...");
        test_ok=false;
        try
        {
            Components::PublisherDescriptions_var publisher_desc
                = basic_client_comp->get_all_publishers();

            if (publisher_desc->length() != 0)
            {
                TEST_INFO("ERROR: Unexpected number of publisher found for Client component");
            }
            else
            {
                test_ok=true;
            }
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Get_all_emitters operation on Client component...");
        test_ok=false;
        try
        {
            Components::EmitterDescriptions_var emitter_desc
                = basic_client_comp->get_all_emitters();

            if (emitter_desc->length() != 0)
            {
                TEST_INFO("ERROR: Unexpected number of emitter found for Client component");
            }
            else
            {
                test_ok=true;
            }
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Get_all_consumers operation on Client component...");
        test_ok=false;
        try
        {
            Components::ConsumerDescriptions_var consumer_desc
                = basic_client_comp->get_all_consumers();

            if (consumer_desc->length() != 0)
            {
                TEST_INFO("ERROR: Unexpected number of consumers found for Client component");
            }
            else
            {
                test_ok=true;
            }
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Get_ccm_home operation on Client component...");
        test_ok=false;
        try
        {
            Components::CCMHome_var c_home
                = basic_client_comp->get_ccm_home();

            if (!c_home->_is_equivalent(client_home.in()))
            {
                TEST_INFO("ERROR: Unexpected home for Client component");
            }
            else
            {
                test_ok=true;
            }
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Get_primary_key operation on Client component...");
        test_ok=false;
        try
        {
            Components::PrimaryKeyBase_var key
                = basic_client_comp->get_primary_key();
        }
        catch (const Components::NoKeyAvailable &)
        {
            TEST_INFO("Expected exception raised: NoKeyAvailable");
            test_ok=true;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Get_all_ports operation on Client component...");
        test_ok=false;
        try
        {
            Components::ComponentPortDescription_var port_desc
                = basic_client_comp->get_all_ports();

            Components::FacetDescriptions facet_desc
                = port_desc->facets();
            Components::ReceptacleDescriptions recep_desc
                = port_desc->receptacles();
            Components::ConsumerDescriptions consumer_desc
                = port_desc->consumers();
            Components::EmitterDescriptions emitter_desc
                = port_desc->emitters();
            Components::PublisherDescriptions publisher_desc
                = port_desc->publishers();

            if (facet_desc.length() != 0)
            {
                TEST_INFO("ERROR: unexpected number of facet for Client component");
            }
            else if (recep_desc.length() != 2)
            {
                TEST_INFO("ERROR: unexpected number of receptacle for Client component");
            }
            else if (consumer_desc.length() != 0)
            {
                TEST_INFO("ERROR: unexpected number of consumer for Client component");
            }
            else if (publisher_desc.length() != 0)
            {
                TEST_INFO("ERROR: unexpected number of publisher for Client component");
            }
            else if (emitter_desc.length() != 0)
            {
                TEST_INFO("ERROR: unexpected number of emitter for Client component");
            }
            else
            {
                test_ok = true;
            }
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Remove operation on a Server component...");
        test_ok=false;
        try
        {
            basic_server_comp3->remove();
            test_ok=true;
        }
        catch (const Components::RemoveFailure &)
        {
            TEST_INFO("Unexpected exception raised: NoKeyAvailable");
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Remove operation again on the same Server component...");
        test_ok=false;
        try
        {
            basic_server_comp3->remove();
        }
        catch (const Components::RemoveFailure &)
        {
            TEST_INFO("Unexpected exception raised: RemoveFailure");
            test_ok=false;
        }
        catch (const CORBA::OBJECT_NOT_EXIST& )
        {
            TEST_INFO("Expected exception raised: OBJECT_NOT_EXIST");
            test_ok=true;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Configuration_complete operation on a removed Server component...");
        test_ok=false;
        try
        {
            basic_server_comp3->configuration_complete();
        }
        catch (const Components::InvalidConfiguration &)
        {
            TEST_INFO("Unexpected exception raised: InvalidConfiguration");
        }
        catch (const CORBA::OBJECT_NOT_EXIST& )
        {
            TEST_INFO("Expected exception raised: OBJECT_NOT_EXIST");
            test_ok=true;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Configuration_complete operation on an other removed Server component...");
        test_ok=false;
        try
        {
            basic_server_comp2->configuration_complete();
        }
        catch (const Components::InvalidConfiguration &)
        {
            TEST_INFO("Unexpected exception raised: InvalidConfiguration");
        }
        catch (const CORBA::OBJECT_NOT_EXIST& )
        {
            TEST_INFO("Expected exception raised: OBJECT_NOT_EXIST");
            test_ok=true;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Configuration_complete operation on Server component...");
        test_ok=false;
        try
        {
            basic_server_comp->configuration_complete();
        }
        catch (const Components::InvalidConfiguration &)
        {
            TEST_INFO("Expected exception raised: InvalidConfiguration");
            test_ok=true;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Init attribute operation on Server component...");
        test_ok=false;
        try
        {
            basic_server_comp->name(SERVER_NAME);
            test_ok=true;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Configuration_complete operation on Server component...");
        test_ok=false;
        try
        {
            basic_server_comp->configuration_complete();
            test_ok=true;
        }
        catch (const Components::InvalidConfiguration &)
        {
            TEST_INFO("Unexpected exception raised: InvalidConfiguration");
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Init attribute operation on Client component...");
        test_ok=false;
        try
        {
            BasicDemo::Event_var event = new BasicDemo::Event_impl();
            event->text("Hello World!");
            basic_client_comp->event(event.in());
            test_ok=true;
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Configuration_complete operation on Client component...");
        test_ok=false;
        try
        {
            basic_client_comp->configuration_complete();
            test_ok=true;
        }
        catch (const Components::InvalidConfiguration &)
        {
            TEST_INFO("Unexpected exception raised: InvalidConfiguration");
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Get name of Server component...");
        test_ok=false;
        try
        {
            CORBA::String_var server_name = basic_server_comp->name();
            std::ostringstream os;
            os << "Server name is " << server_name.in() << std::ends;
            TEST_INFO(os.str());
            test_ok=true;
        }
        catch (const Components::InvalidConfiguration &)
        {
            TEST_INFO("Unexpected exception raised: InvalidConfiguration");
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Get name of Client component...");
        test_ok=false;
        try
        {
            CORBA::String_var client_name = basic_client_comp->name();
            std::ostringstream os;
            os << "Client name is " << client_name.in() << std::ends;
            TEST_INFO(os.str());
            test_ok=true;
        }
        catch (const Components::InvalidConfiguration &)
        {
            TEST_INFO("Unexpected exception raised: InvalidConfiguration");
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Disconnect operation on Client component...");
        test_ok=false;
        try
        {
            basic_client_comp->disconnect(DISPLAY_RECEP_NAME, 0);
            test_ok=true;
        }
        catch (const Components::InvalidName& )
        {
            TEST_INFO("Unexpected exception raised: InvalidName");
        }
        catch (const Components::InvalidConnection& )
        {
            TEST_INFO("Unexpected exception raised: InvalidConnection");
        }
        catch (const Components::CookieRequired& )
        {
            TEST_INFO("Unexpected exception raised: CookieRequired");
        }
        catch (const Components::NoConnection& )
        {
            TEST_INFO("Unexpected exception raised: NoConnection");
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Disconnect operation on Client component...");
        test_ok=false;
        try
        {
            basic_client_comp->disconnect(REGISTER_RECEP_NAME, 0);
            test_ok=true;
        }
        catch (const Components::InvalidName& )
        {
            TEST_INFO("Unexpected exception raised: InvalidName");
        }
        catch (const Components::InvalidConnection& )
        {
            TEST_INFO("Unexpected exception raised: InvalidConnection");
        }
        catch (const Components::CookieRequired& )
        {
            TEST_INFO("Unexpected exception raised: CookieRequired");
        }
        catch (const Components::NoConnection& )
        {
            TEST_INFO("Unexpected exception raised: NoConnection");
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Remove operation on the Server home...");
        test_ok=false;
        try
        {
            m_container->remove_home(basic_server_home.in());
            test_ok=true;
        }
        catch (const Components::RemoveFailure &)
        {
            TEST_INFO("Unexpected exception raised: NoKeyAvailable");
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);
        
        TEST_INFO("Remove operation on the Client home...");
        test_ok=false;
        try
        {
            m_container->remove_home(client_home.in());
            test_ok=true;
        }
        catch (const Components::RemoveFailure &)
        {
            TEST_INFO("Unexpected exception raised: NoKeyAvailable");
        }
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);
    } 
    catch (const CORBA::Exception & e) 
    {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str()); 
            TEST_CHECK(false);
    }

    TEST_INFO("Set to nil server_home"); 
    server_home = Components::CCMHome::_nil();
    TEST_INFO("Set to nil basic_server_home"); 
    basic_server_home = BasicDemo::ServerHome::_nil();
    TEST_INFO("Set to nil ccm_server_comp"); 
    ccm_server_comp = Components::CCMObject::_nil();
    TEST_INFO("Set to nil basic_server_comp2"); 
    basic_server_comp2 = BasicDemo::Server::_nil();
    TEST_INFO("Set to nil basic_server_comp3"); 
    basic_server_comp3 = BasicDemo::Server::_nil();

    TEST_INFO("Set to nil client_home"); 
    client_home = Components::CCMHome::_nil();
    TEST_INFO("Set to nil basic_client_home"); 
    basic_client_home = BasicDemo::ClientHome::_nil();
    TEST_INFO("Set to nil ccm_client_comp"); 
    ccm_client_comp = Components::CCMObject::_nil();
    TEST_INFO("Set to nil basic_client_comp"); 
    basic_client_comp = BasicDemo::Client::_nil();

    TEST_INFO("Set to nil basic_server_comp"); 
    basic_server_comp = BasicDemo::Server::_nil();
   
    TEST_INFO("Set to nil display_facet_obj"); 
    display_facet_obj = CORBA::Object::_nil();
   

}
        
}; // End namespace CIF
}; // End namespace CCM
}; // End namespace Cdmw

