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


#include "testccmcomponentserver/TestComponentServer.hpp"
#include "CCMContainer/ccmcomponentserver/ComponentServer_impl.hpp"
#include "CCMContainer/ccmcontainer/ConfigValue_impl.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "LifeCycle/idllib/CdmwLifeCycle.stub.hpp"
#include "Event/idllib/CdmwEvent.stub.hpp"


namespace TestCdmwCCM
{

TestComponentServer::TestComponentServer(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa)
    throw(CORBA::SystemException)
    : Cdmw::TestUtils::Testable("TestComponentServer::TestComponentServer"),
      m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa))
{
}


TestComponentServer::TestComponentServer(const TestComponentServer& rhs)
    throw(CORBA::SystemException)
    : Cdmw::TestUtils::Testable("TestComponentServer::TestComponentServer"),
      m_orb(rhs.m_orb),
      m_poa(rhs.m_poa)
{
}

TestComponentServer::~TestComponentServer()
    throw()
{
}


TestComponentServer&
TestComponentServer::operator=(const TestComponentServer& rhs)
    throw(CORBA::SystemException)
{
    if (this != & rhs) {
        m_orb = rhs.m_orb;
        m_poa = rhs.m_poa;
    }
    return *this;
}


Components::ConfigValues*
TestComponentServer::create_ConfigValues(CdmwDeployment::ComponentKindValue   component_kind) const
    throw ()
{
    const CORBA::ULong nb_values = 1L;
    Components::ConfigValues_var cfg = new Components::ConfigValues(nb_values);
    cfg->length(nb_values);
    CORBA::ULong idx = 0L;
    using namespace Cdmw::CCM::Container;
    // COMPONENT_KIND
    {
        CORBA::Any value;
        value <<= component_kind;
        cfg[idx++] = new ConfigValue_impl(CdmwDeployment::COMPONENT_KIND,value);
    }
    
    return cfg._retn();
}

//         interface ComponentServer
//         {
//             readonly attribute ConfigValues configuration;

//             ServerActivator get_server_activator();

//             Container create_container(in ConfigValues config)
//                         raises (CreateFailure, InvalidConfiguration);

//             void remove_container(in Container cref) raises (RemoveFailure);

//             Containers get_containers();

//              void remove() raises (RemoveFailure);
//         };

void TestComponentServer::do_tests()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (21);
    
    
    TEST_INFO("Testing Component Server");
    using namespace Components;

    Deployment::ComponentServer_var the_component_server = Deployment::ComponentServer::_nil();
    
    // 1) Create a ComponentServer
    {
        // Create a set of ConfigValues.
        ConfigValues cfg_compserver(0);
        cfg_compserver.length(0L);
        using namespace Cdmw::CCM::ComponentServer;
        CdmwEvent::EventChannelFactory_var no_event_channel_factory
            = CdmwEvent::EventChannelFactory::_nil();
        std::string process_name("DummyProcessName");
        std::string application_name("DummyApplicationName");
        Components::HomeRegistration_var home_registration = Components::HomeRegistration::_nil();
        ComponentServer_impl * cs = new ComponentServer_impl(m_orb.in(),
                                                             m_poa.in(),
                                                             home_registration.in(),
                                                             no_event_channel_factory.in(),
                                                             process_name,
                                                             application_name,
                                                             cfg_compserver);
        PortableServer::ServantBase_var serv  = cs;
        
        the_component_server = cs->_this();
    }
    // Testing the_component_server->get_containers()
    {
        TEST_INFO("Getting all containers...");
        Deployment::Containers_var containers =  the_component_server->get_containers();
        TEST_CHECK(containers->length() == 0L);
    }
    
    // 2) Creating a first container
    Deployment::Container_var container1 = Deployment::Container::_nil();
    try {
        TEST_INFO("Creating a first session container ...");
        ConfigValues_var cfg0 = create_ConfigValues(CdmwDeployment::SESSION);        
        container1 = the_component_server->create_container(cfg0.in());
        TEST_CHECK(!CORBA::is_nil(container1.in()));
        TEST_INFO("Checking used config values...");
        TEST_INFO("Checking number of config values...");
        ConfigValues_var cfg1 = container1->configuration();
        TEST_CHECK(cfg1->length() == 1L);
        TEST_INFO("Checking content of config values...");
        FeatureName_var name = cfg1->operator[](0)->name();
        TEST_CHECK(strcmp(name.in(),CdmwDeployment::COMPONENT_KIND) == 0);
        CORBA::Any & value = cfg1->operator[](0)->value();
        CdmwDeployment::ComponentKindValue   component_kind;
        value >>= component_kind;
        TEST_CHECK(component_kind == CdmwDeployment::SESSION);
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_FAILED();
    }
    Deployment::Container_var container2 = Deployment::Container::_nil(); 
    try {
        TEST_INFO("Creating a second session container ...");
        ConfigValues_var cfg0 = create_ConfigValues(CdmwDeployment::SERVICE);
        container2 = the_component_server->create_container(cfg0.in());
        TEST_CHECK(!CORBA::is_nil(container2.in()));
        TEST_INFO("Checking used config values...");
        TEST_INFO("Checking number of config values...");
        ConfigValues_var cfg1 = container2->configuration();
        TEST_CHECK(cfg1->length() == 1L);
        TEST_INFO("Checking content of config values...");
        FeatureName_var name = cfg1->operator[](0)->name();
        TEST_CHECK(strcmp(name.in(),CdmwDeployment::COMPONENT_KIND) == 0);
        CORBA::Any & value = cfg1->operator[](0)->value();
        CdmwDeployment::ComponentKindValue   component_kind;
        value >>= component_kind;
        TEST_CHECK(component_kind == CdmwDeployment::SERVICE);
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_FAILED();
    }
    // Creating an Entity container (NOT supported yet).
    try {
        TEST_INFO("Creating an Entity container (NOT supported yet) ...");
        ConfigValues_var cfg0 = create_ConfigValues(CdmwDeployment::PROCESS);
        Deployment::Container_var container = the_component_server->create_container(cfg0.in());
        TEST_FAILED();
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_SUCCEED();
    }
    // Testing the_component_server->get_containers()
    {
        TEST_INFO("Getting all containers...");
        Deployment::Containers_var containers =  the_component_server->get_containers();
        std::cout << "containers->length() = " << containers->length() << std::endl;
        TEST_CHECK(containers->length() == 2L);
    }
    // 3) Removing first container
    TEST_INFO("Removing first container...");
    try {
        the_component_server->remove_container(container1.in());
        TEST_SUCCEED();
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_FAILED();
    }
    // Testing the_component_server->get_containers()
    {
        TEST_INFO("Getting all containers...");
        Deployment::Containers_var containers =  the_component_server->get_containers();
        std::cout << "containers->length() = " << containers->length() << std::endl;
        TEST_CHECK(containers->length() == 1L);
    }
    TEST_INFO("Removing first container again...");
    try {
        the_component_server->remove_container(container1.in());
        TEST_FAILED();
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_SUCCEED();
    }
    // 4) Testing supported interfaces...
    TEST_INFO("Narrowing to FactoryBase object...");
    CdmwLifeCycle::FactoryBase_var fact
        = CdmwLifeCycle::FactoryBase::_narrow(the_component_server.in());
    TEST_CHECK(!CORBA::is_nil(fact.in()));
    TEST_INFO("Testing supported interfaces...");
    try {
        CdmwLifeCycle::RepositoryIdSeq_var seq = fact->get_supported_objects();
        TEST_CHECK(seq->length() == 3L);
        std::cout << "Supported objects:" << std::endl;
        for (CORBA::ULong i=0L; i < seq->length();++i)
            std::cout << "\t" << seq[i].in() << std::endl;
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_FAILED();
    }
    TEST_INFO("Testing support of IDL:thalesgroup.com/CdmwCcmContainer/Container:1.0 ...");
    try {
        const char* rep_id="IDL:thalesgroup.com/CdmwCcmContainer/Container:1.0";
        CORBA::Boolean res = fact->supports(rep_id);
        TEST_CHECK(res == true);
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_FAILED();
    }
    TEST_INFO("Testing support of IDL:omg.org/Components/Deployment/Container:1.0 ...");
    try {
        const char* rep_id="IDL:omg.org/Components/Deployment/Container:1.0";
        CORBA::Boolean res = fact->supports(rep_id);
        TEST_CHECK(res == true);
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_FAILED();
    }
    TEST_INFO("Testing support of IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase:1.0 ...");
    try {
        const char* rep_id="IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase:1.0";
        CORBA::Boolean res = fact->supports(rep_id);
        TEST_CHECK(res == true);
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_FAILED();
    }
    TEST_INFO("Testing support of IDL:dummy:1.0 ...");
    try {
        const char* rep_id="IDL:dummy:1.0";
        CORBA::Boolean res = fact->supports(rep_id);
        TEST_CHECK(res == false);
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_FAILED();
    }
    // 5) Removing the component server
    TEST_INFO("Removing the component server...");
    try {
        the_component_server->remove();
        TEST_SUCCEED();
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_FAILED();
    }
    
}


}; // End namespace TestCdmwCCM

