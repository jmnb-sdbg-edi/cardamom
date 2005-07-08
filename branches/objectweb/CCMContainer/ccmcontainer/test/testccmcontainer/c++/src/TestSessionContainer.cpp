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


#include "testccmcontainer/TestSessionContainer.hpp"
#include "CCMContainer/ccmcontainer/ConfigValue_impl.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "testccmcontainer/hello.stub.hpp"
#include "testccmcontainer/Messages_impl.hpp"
#include "CCMContainer/idllib/CdmwDeployment.stub.hpp"


namespace TestCdmwCCM
{

    TestSessionContainer::TestSessionContainer(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa, std::string process_name)
    throw(CORBA::SystemException)
    : Cdmw::TestUtils::Testable("TestSessionContainer::TestSessionContainer"),
      m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa)),
      m_process_name(process_name)
        
{
}


TestSessionContainer::TestSessionContainer(const TestSessionContainer& rhs)
    throw(CORBA::SystemException)
    : Cdmw::TestUtils::Testable("TestSessionContainer::TestSessionContainer"),
      m_orb(rhs.m_orb),
      m_poa(rhs.m_poa)
{
}

TestSessionContainer::~TestSessionContainer()
    throw()
{
}


TestSessionContainer&
TestSessionContainer::operator=(const TestSessionContainer& rhs)
    throw(CORBA::SystemException)
{
    if (this != & rhs) {
        m_orb = rhs.m_orb;
        m_poa = rhs.m_poa;
    }
    return *this;
}


Components::ConfigValues*
TestSessionContainer::create_ConfigValues(CdmwDeployment::ComponentKindValue   component_kind,
                                          CdmwDeployment::ServantLifetimeValue servant_lifetime,
                                          CdmwDeployment::ThreadingPolicyValue threading_policy,
                                          bool                                  with_TraceMessage) const
    throw ()
{
    const CORBA::ULong nb_values = 6L;
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
    // HOME_REPOSITORY_ID
    {
        const char* repid="IDL:thalesgroup.com/Cdmw_HelloModule/HelloHome:1.0";
        CORBA::Any value;
        value <<= repid;
        cfg[idx++] = new ConfigValue_impl(CdmwDeployment::HOME_REPOSITORY_ID,value);
    }
    // HOME_SERVANT_CLASSNAME
    {
        const char* classname="TestCdmwCCM.CdmwHelloHome_impl";
        CORBA::Any value;
        value <<= classname;
        cfg[idx++] = new ConfigValue_impl(CdmwDeployment::HOME_SERVANT_CLASSNAME,value);
    }
    // SERVANT_LIFETIME
    {
        CORBA::Any value;
        value <<= servant_lifetime;
        cfg[idx++] = new ConfigValue_impl(CdmwDeployment::SERVANT_LIFETIME,value);
    }
    // THREADING_POLICY
    {
        CORBA::Any value;
        value <<= threading_policy;
        cfg[idx++] = new ConfigValue_impl(CdmwDeployment::THREADING_POLICY,value);
    }
    // VALUETYPE_FACTORY_DEPENDENCIES </TD><TD>sequence< ValuetypeFactoryDescription ></TD>
    {
        const char* Message_repid = "IDL:thalesgroup.com/HelloModule/Message:1.0";
        const char* Message_entrypoint = "createMessageFactory";
        const char* TraceMessage_repid = "IDL:thalesgroup.com/HelloModule/TraceMessage:1.0";
        const char* TraceMessage_entrypoint = "createTraceMessageFactory";

        CORBA::ULong lg = 1L;
        if (with_TraceMessage) lg = 2L;
        CdmwDeployment::ValuetypeFactoryDescriptionSeq seq(lg);
        seq.length(lg);
        // Message valuetype
        {
            CdmwDeployment::ValuetypeFactoryDescription desc;
            desc.repid = Message_repid;
            desc.factory_entrypoint = Message_entrypoint;
            seq[0]=desc;
        }
        if (with_TraceMessage) {
            // TraceMessage valuetype
            CdmwDeployment::ValuetypeFactoryDescription desc;
            desc.repid = TraceMessage_repid;
            desc.factory_entrypoint = TraceMessage_entrypoint;
            seq[1]=desc;
        }
        
        CORBA::Any value;
        value <<= seq;
        cfg[idx++] = new ConfigValue_impl(CdmwDeployment::VALUETYPE_FACTORY_DEPENDENCIES,value);
    }
    
    return cfg._retn();
}
// interface Container
// {
//     readonly attribute ConfigValues configuration;
    
//     ComponentServer get_component_server();
    
//     CCMHome install_home(in UUID id,
//                          in string entrypt,
//                          in ConfigValues config)
//         raises (UnknownImplId,
//                 ImplEntryPointNotFound,
//                 InstallationFailure,
//                 InvalidConfiguration);
    
//     void remove_home(in CCMHome href) raises (RemoveFailure);
    
//     CCMHomes get_homes();
    
//     void remove() raises (RemoveFailure);
// };
void TestSessionContainer::do_tests()
{
     // set number of requested successfull tests
    set_nbOfRequestedTestOK (14);
    

    std::string componentServer = "componentServer.ior";
    try {
        Cdmw::OsSupport::OS::unlink (componentServer);	
    }
    catch (...) {
    }



    TEST_INFO("Testing Session Container");
    using namespace Components;
//     Deployment::ComponentServer_var component_server = Deployment::ComponentServer::_nil();
//     {
//         // Creating the Component Installtion
//         MyComponentInstallation* cmp_inst=new MyComponentInstallation(m_poa.in());
//         PortableServer::ServantBase_var cmp_inst_servant = cmp_inst;
//         CdmwDeployment::ComponentInstallation_var component_installation = cmp_inst->_this();
//         // Creating the Component Server
//         MyComponentServer* cmp_svr = new MyComponentServer(m_orb.in(),
//                                                            m_poa.in(),
//                                                            component_installation.in());
//         PortableServer::ServantBase_var cmp_svr_servant = cmp_svr;
        
//         component_server = cmp_svr->_this();
//     }
    
    Cdmw::OsSupport::OS::ProcessId pid  = 
    Cdmw::OsSupport::OS::create_process( m_process_name,  " --server");
    
    int timescale = Cdmw::TestUtils::Testable::get_timescale();
    Cdmw::OsSupport::OS::sleep(timescale*2000);

    TEST_INFO("get the reference of the component_server");
    std::string file_url = "file://componentServer.ior";
    
    
    CORBA::Object_var object = m_orb->string_to_object(file_url.c_str());
    Deployment::ComponentServer_var component_server = Deployment::ComponentServer::_narrow(object.in());    

    // Create a container instance
    // Create a set of ConfigValues.
    ConfigValues_var cfg_compserver = new Components::ConfigValues(0);
    cfg_compserver->length(0L);
    
    Deployment::Container_var the_container =
        component_server->create_container(cfg_compserver.in());

    // Testing the_container->configuration() ...

    // Testing the_container->get_component_server() ...

    // Testing the_container->install_home() ...
    TEST_INFO("Testing SESSION Component with COMPONENT_LIFETIME policy");
    const char * ENTRY_PT   = "createCCM_HelloHome_impl";
    const char * UUID_home1 = "SessionHome1";
    
    HelloModule::HelloHome_var hellohome1 = HelloModule::HelloHome::_nil();
    try {
        // Create a set of ConfigValues.
        ConfigValues_var cfg = create_ConfigValues(CdmwDeployment::SESSION,
                                                   CdmwDeployment::COMPONENT_LIFETIME,
                                                   CdmwDeployment::MULTITHREAD,
                                                   false);
        TEST_INFO("Creating " << UUID_home1 << " home...");
        CCMHome_var session_home = the_container->install_home(UUID_home1,
                                                               ENTRY_PT,
                                                               cfg.in());
        hellohome1 = HelloModule::HelloHome::_narrow(session_home.in());
        TEST_CHECK(!CORBA::is_nil(hellohome1.in()));
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_FAILED();
    }

    // Testing the_container->get_homes()
    TEST_INFO("Getting all container's homes...");
    CCMHomes_var homes =  the_container->get_homes();
    TEST_CHECK(homes->length() == 1L);

    // Testing component creation ...

    // Testing the_container->remove_home() ..
    try {
        TEST_INFO("Removing " << UUID_home1 << " home...");
        the_container->remove_home(hellohome1.in());
        homes =  the_container->get_homes();
        TEST_CHECK(homes->length() == 0L);
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_FAILED();
    }
    // Testing  component creation
    try {
        // Create a set of ConfigValues.
        ConfigValues_var cfg = create_ConfigValues(CdmwDeployment::SESSION,
                                                   CdmwDeployment::COMPONENT_LIFETIME,
                                                   CdmwDeployment::MULTITHREAD,
                                                   false);
        TEST_INFO("Creating " << UUID_home1 << " home again ...");
        CCMHome_var session_home = the_container->install_home(UUID_home1,
                                                               ENTRY_PT,
                                                               cfg.in());
        hellohome1 = HelloModule::HelloHome::_narrow(session_home.in());
        TEST_CHECK(!CORBA::is_nil(hellohome1.in()));
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_FAILED();
    }
    // Creating a component
    HelloModule::HelloWorld_var comp1 = HelloModule::HelloWorld::_nil();
    try {
        TEST_INFO("Creating 1st component using create()...");
        
        CCMObject_var cmp = hellohome1->create();
        comp1 = HelloModule::HelloWorld::_narrow(cmp.in());
        TEST_CHECK(!CORBA::is_nil(comp1.in()));
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_FAILED();
    }
    HelloModule::HelloWorld_var comp2 = HelloModule::HelloWorld::_nil();
    try {
        TEST_INFO("Creating 2nd component using create_component()...");
        
        CCMObject_var cmp = hellohome1->create_component();
        comp2 = HelloModule::HelloWorld::_narrow(cmp.in());
        TEST_CHECK(!CORBA::is_nil(comp2.in()));
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_FAILED();
    }
    // Calling say_hello on the 1st and 2nd components
    try {
        TEST_INFO("Calling say_hello() on the 1st component...");
        HelloModule::Message_var msg = new Message_impl("Hello!");
        comp1->say_hello(msg.in());
        TEST_SUCCEED();
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_FAILED();
    }
    try {
        TEST_INFO("Calling say_hello() on the 2nd component...");
        HelloModule::Message_var msg = new Message_impl("Hello again!");
        comp2->say_hello(msg.in());
        TEST_SUCCEED();
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_FAILED();
    }
    // Testing valuetype factories
    try {
        TEST_INFO("Calling say_hello() on the 2nd component with no available valuetype factory...");
        HelloModule::TraceMessage_var msg = new TraceMessage_impl("Hello ...",5L);

        comp2->log_message(msg.in());
        TEST_FAILED();
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_SUCCEED();
    }
    
    const char * UUID_home2 = "SessionHome2";
    try {
        // Create a set of ConfigValues.
        ConfigValues_var cfg = create_ConfigValues(CdmwDeployment::SESSION,
                                                   CdmwDeployment::COMPONENT_LIFETIME,
                                                   CdmwDeployment::MULTITHREAD,
                                                   true);
        TEST_INFO("Creating " << UUID_home2 << " home to install a valuetype factory ...");
        CCMHome_var session_home = the_container->install_home(UUID_home2,
                                                               ENTRY_PT,
                                                               cfg.in());
        HelloModule::HelloHome_var hellohome = HelloModule::HelloHome::_narrow(session_home.in());
        TEST_CHECK(!CORBA::is_nil(hellohome.in()));
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_FAILED();
    }
    
    try {
        TEST_INFO("Calling say_hello() on the 2nd component WITH a valuetype factory installed...");
        HelloModule::TraceMessage_var msg = new TraceMessage_impl("Hello ...",5L);
        comp2->log_message(msg.in());
        TEST_SUCCEED();
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_FAILED();
    }
    
    // Deleting 1st component
    try {
        TEST_INFO("Asking 1st component to remove itself...");
        comp1->remove();
        TEST_SUCCEED();
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_FAILED();
    }
    // Deleting 2nd component
    try {
        TEST_INFO("Asking home to remove 2nd component...");
        hellohome1->remove_component(comp2.in());
        TEST_SUCCEED();
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_FAILED();
    }
    // Tesing valuetype support


    
    // Testing the_container->remove() ...
    try {
        TEST_INFO("Removing the container...");
        the_container->remove();
        TEST_SUCCEED();
    } catch (const CORBA::Exception & ex) {
        std::cerr << "ex = " << ex << std::endl;
        TEST_FAILED();
    }
    
    TEST_INFO( "Killing process component_server" );
    Cdmw::OsSupport::OS::kill_process( pid);
    Cdmw::OsSupport::OS::sleep(timescale*2000);
}


}; // End namespace TestCdmwCCM

