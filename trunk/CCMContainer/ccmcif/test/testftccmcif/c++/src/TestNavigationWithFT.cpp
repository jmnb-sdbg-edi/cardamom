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


#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

#include "testftccmcif/TestNavigationWithFT.hpp"
#include "testftccmcif/ftccmcif_cif.skel.hpp"

#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <CCMContainer/ccmcontainer/ConfigValue_impl.hpp>
#include <CCMContainer/idllib/Components.stub.hpp>
#include <CCMContainer/idllib/CdmwDeployment.stub.hpp>
#include <CCMContainer/idllib/CdmwCcmCif.stub.hpp>



namespace
{
    const char* COMP_WITH_FACET_HOME_REP_ID = "IDL:thalesgroup.com/TestFtCcmCif/ComponentWithFacetHome:1.0";
    const char* COMP_WITH_FACET_HOME_SERVANT_NAME = "Cdmw.CCM.CIF.CdmwTestFtCcmCif.SessionComponentWithFacetHome_impl";
    const char* COMP_WITH_FACET_HOME_UID = "ComponentWithFacetHome_1";
    const char* COMP_WITH_FACET_HOME_ENTRY_POINT = "create_CCM_ComponentWithFacetHome";

    const char* COMP_WITH_CONSUMER_HOME_REP_ID = "IDL:thalesgroup.com/TestFtCcmCif/ComponentWithConsumerHome:1.0";
    const char* COMP_WITH_CONSUMER_HOME_SERVANT_NAME = "Cdmw.CCM.CIF.CdmwTestFtCcmCif.SessionComponentWithConsumerHome_impl";
    const char* COMP_WITH_CONSUMER_HOME_UID = "ComponentWithConsumerHome_1";
    const char* COMP_WITH_CONSUMER_HOME_ENTRY_POINT = "create_CCM_ComponentWithConsumerHome";

    const char* TEST_FACET_NAME = "the_facet";
    const char* TEST_CONSUMER_NAME = "the_consumer";

    const char* TEST_EVENT_REP_ID = "IDL:thalesgroup.com/TestFtCcmCif/TestEvent:1.0";
    const char* TEST_EVENT_ENTRYPOINT = "createTestEventFactory";

}; // End anonymous namespace



namespace TestFtCcmCif
{


TestNavigationWithFT::TestNavigationWithFT(
        CORBA::ORB_ptr orb,
        FT::Location primaryLocation,
        FT::Location backupLocation,
        Components::Deployment::Container_ptr containerPrimary,
        Components::Deployment::Container_ptr containerBackup)
    throw(CORBA::Exception)
    : Testable("Test CCM Navigation with FT"),
      m_orb(CORBA::ORB::_duplicate(orb)),
      m_primaryLocation(primaryLocation),
      m_backupLocation(backupLocation),
      m_containerPrimary(Components::Deployment::Container::_duplicate(containerPrimary)),
      m_containerBackup(Components::Deployment::Container::_duplicate(containerBackup))
{
    //
    // Get Replication Manager
    //
    CORBA::Object_var obj;
    try {
        obj = m_orb -> resolve_initial_references("ReplicationManager");
    } catch(const CORBA::ORB::InvalidName&) {
        std::cerr<< "Can't resolve `ReplicationManager'" << std::endl;
        throw;
    }
    if(CORBA::is_nil(obj.in())) {
        std::cerr << "`ReplicationManager' is a nil object reference" << std::endl;
        throw CORBA::INTERNAL();
    }
    try {
        m_replicationManager = CdmwFT::ReplicationManager::_narrow(obj.in());
    } catch (const CORBA::Exception&) {
        std::cerr << "Obj is not a ReplicationManager" << std::endl;
        throw;
    }
}




ComponentWithFacet_ptr
TestNavigationWithFT::create_ComponentWithFacet(Components::Deployment::Container_ptr container, 
                                                ComponentWithFacet_ptr componentWithFacetGroup)
    throw (CORBA::Exception)
{
    add_nbOfRequestedTestOK(2);
    
    Components::CCMHome_var componentWithFacetHome;
    ComponentWithFacet_var  componentWithFacet = ComponentWithFacet::_nil();

    TEST_INFO("  Install a FT componentWithFacetHome");
    try
    {
        Components::ConfigValues config_values(8);
        config_values.length(6);
        CORBA::Any value;
        value <<= CdmwDeployment::SESSION;
        config_values[0] =
            new Cdmw::CCM::Container::ConfigValue_impl(
                CdmwDeployment::COMPONENT_KIND, 
                value);

        value <<= CdmwDeployment::CONTAINER_LIFETIME;
        config_values[1] =
            new Cdmw::CCM::Container::ConfigValue_impl(
                CdmwDeployment::SERVANT_LIFETIME, 
                value);

        value <<= CdmwDeployment::MULTITHREAD;
        config_values[2] =
            new Cdmw::CCM::Container::ConfigValue_impl(
                CdmwDeployment::THREADING_POLICY, 
                value);

        value <<= COMP_WITH_FACET_HOME_REP_ID;
        config_values[3] =
            new Cdmw::CCM::Container::ConfigValue_impl(
                CdmwDeployment::HOME_REPOSITORY_ID, 
                value);

        value <<= COMP_WITH_FACET_HOME_SERVANT_NAME;
        config_values[4] =
            new Cdmw::CCM::Container::ConfigValue_impl(
                CdmwDeployment::HOME_SERVANT_CLASSNAME, 
                value);

        value <<= CdmwDeployment::WARM_PASSIVE;
        config_values[5] =
            new Cdmw::CCM::Container::ConfigValue_impl(
                CdmwDeployment::FAULT_TOLERANCE_REPLICATION_STYLE,
                value);
                
        componentWithFacetHome =
           container->install_home(COMP_WITH_FACET_HOME_UID,
                                   COMP_WITH_FACET_HOME_ENTRY_POINT,
                                   config_values);
        TEST_SUCCEED();
    } catch (const CORBA::BAD_PARAM& e) {
        std::ostringstream os;
        os << "Unexpected BAD_PARAM exception: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_FAILED();
    } catch (const Components::Deployment::InstallationFailure& e) {
        std::ostringstream os;
        os << "Unexpected InstallationFailure exception: " << e.reason << std::ends;
        TEST_INFO(os.str());
        TEST_FAILED();
    } catch (const CORBA::SystemException& e) {
        std::ostringstream os;
        os << "Unexpected system exception raised: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_FAILED();
    } catch (const CORBA::UserException& e) {
        std::ostringstream os;
        os << "Unexpected User exception raised: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_FAILED();
    }

    TEST_INFO("  Create a ComponentWithFacet with config values");
    try
    {
        CdmwCcmCif::KeylessCCMHome_var keyless_componentWithFacetHome 
            = CdmwCcmCif::KeylessCCMHome::_narrow(componentWithFacetHome.in());

        if (CORBA::is_nil(keyless_componentWithFacetHome.in())) {
            TEST_INFO("ERROR: Created home is not a keyless home!");
            TEST_FAILED();
        } else {
            Components::ConfigValues config_values(3);
            config_values.length(1);
            CORBA::Any value;
            value <<= componentWithFacetGroup;
            config_values[0] = 
                new Cdmw::CCM::Container::ConfigValue_impl(
                                            CdmwDeployment::FAULT_TOLERANCE_GROUP_REF, 
                                            value);

            Components::CCMObject_var obj = 
                keyless_componentWithFacetHome->create_component_with_config_values(config_values);

            componentWithFacet = 
                ComponentWithFacet::_narrow(obj.in());

            if (CORBA::is_nil(componentWithFacet.in())) {
                TEST_INFO("ERROR: Created component is not a TestFtCcmCif::ComponentWithFacet!");
                TEST_FAILED();
            } else {
                TEST_SUCCEED();
                return componentWithFacet._retn();
            }
        }
    } catch (const Components::CreateFailure& e) {
        std::ostringstream os;
        os << "Unexpected CreateFailure exception: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_FAILED();
    } catch (const CORBA::SystemException& e) {
        std::ostringstream os;
        os << "Unexpected SystemException exception: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_FAILED();
    }

    // an error occured
    throw CORBA::INTERNAL();
}




ComponentWithConsumer_ptr
TestNavigationWithFT::create_ComponentWithConsumer(Components::Deployment::Container_ptr container, 
                                                   ComponentWithConsumer_ptr componentWithConsumerGroup)
    throw (CORBA::Exception)
{
    add_nbOfRequestedTestOK(7);
    
    Components::CCMHome_var componentWithConsumerHome;
    Components::CCMHome_var FTComponentWithConsumerHome;
    ComponentWithConsumer_var componentWithConsumer = ComponentWithConsumer::_nil();

    try
    {
        Components::ConfigValues config_values(8);
        config_values.length(6);
        CORBA::Any value;
        value <<= CdmwDeployment::SESSION;
        config_values[0] = 
            new Cdmw::CCM::Container::ConfigValue_impl(
                CdmwDeployment::COMPONENT_KIND, 
                                            value);

        value <<= CdmwDeployment::CONTAINER_LIFETIME;
        config_values[1] = 
            new Cdmw::CCM::Container::ConfigValue_impl(
                CdmwDeployment::SERVANT_LIFETIME, 
                value);

        value <<= CdmwDeployment::MULTITHREAD;
        config_values[2] = 
            new Cdmw::CCM::Container::ConfigValue_impl(
                CdmwDeployment::THREADING_POLICY, 
                value);

        value <<= COMP_WITH_CONSUMER_HOME_REP_ID;
        config_values[3] = 
            new Cdmw::CCM::Container::ConfigValue_impl(
                CdmwDeployment::HOME_REPOSITORY_ID, 
                value);

        value <<= COMP_WITH_CONSUMER_HOME_SERVANT_NAME;
        config_values[4] = 
            new Cdmw::CCM::Container::ConfigValue_impl(
                CdmwDeployment::HOME_SERVANT_CLASSNAME, 
                value);

        CORBA::ULong lg = 1L;
        CdmwDeployment::ValuetypeFactoryDescriptionSeq seq(lg);
        seq.length(lg);
        // Event TestEvent valuetype
        {
            CdmwDeployment::ValuetypeFactoryDescription desc;
            desc.repid = TEST_EVENT_REP_ID;
            desc.factory_entrypoint = TEST_EVENT_ENTRYPOINT;
            seq[0]=desc;
        }            
        value <<= seq;
        config_values[5] = 
            new Cdmw::CCM::Container::ConfigValue_impl(
                CdmwDeployment::VALUETYPE_FACTORY_DEPENDENCIES,
                value);

        TEST_INFO("  Install a non FT componentWithConsumerHome");
        componentWithConsumerHome = 
            container->install_home(COMP_WITH_CONSUMER_HOME_UID,
                                    COMP_WITH_CONSUMER_HOME_ENTRY_POINT,
                                    config_values);

        config_values.length(7);
        value <<= CdmwDeployment::WARM_PASSIVE;
        config_values[6] =
            new Cdmw::CCM::Container::ConfigValue_impl(
                CdmwDeployment::FAULT_TOLERANCE_REPLICATION_STYLE,
                value);

        TEST_INFO("  Install a FT componentWithConsumerHome");
        FTComponentWithConsumerHome = 
            container->install_home(COMP_WITH_CONSUMER_HOME_UID,
                                    COMP_WITH_CONSUMER_HOME_ENTRY_POINT,
                                    config_values);

        TEST_SUCCEED();
    
    } catch (const CORBA::BAD_PARAM& e) {
        std::ostringstream os;
        os << "Unexpected BAD_PARAM exception: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_FAILED();
    } catch (const Components::Deployment::InstallationFailure& e) {
        std::ostringstream os;
        os << "Unexpected InstallationFailure exception: " << e.reason << std::ends;
        TEST_INFO(os.str());
        TEST_FAILED();
    } catch (const CORBA::SystemException& e) {
        std::ostringstream os;
        os << "Unexpected system exception raised: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_FAILED();
    } catch (const CORBA::UserException& e) {
        std::ostringstream os;
        os << "Unexpected User exception raised: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_FAILED();
    }

    TEST_INFO("  Create a ComponentWithConsumer without config values on a FT Home");
    try
    {
        CdmwCcmCif::KeylessCCMHome_var keyless_componentWithConsumerHome 
            = CdmwCcmCif::KeylessCCMHome::_narrow(FTComponentWithConsumerHome.in());

        if (CORBA::is_nil(keyless_componentWithConsumerHome.in())) {
            TEST_INFO("ERROR: Created home is not a keyless home!");
            TEST_FAILED();
        } else {
            Components::CCMObject_var obj = 
                keyless_componentWithConsumerHome->create_component();

            TEST_FAILED();
        }
    } catch (const Components::CreateFailure& e) {
        std::ostringstream os;
        os << "Expected CreateFailure exception: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_SUCCEED();
    } catch (const CORBA::SystemException& e) {
        std::ostringstream os;
        os << "Unexpected SystemException exception: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_FAILED();
    }


    TEST_INFO("  Create a ComponentWithConsumer with config values on a non FT Home");
    try
    {
        CdmwCcmCif::KeylessCCMHome_var keyless_componentWithConsumerHome 
            = CdmwCcmCif::KeylessCCMHome::_narrow(componentWithConsumerHome.in());

        if (CORBA::is_nil(keyless_componentWithConsumerHome.in())) {
            TEST_INFO("ERROR: Created home is not a keyless home!");
            TEST_FAILED();
        } else {
            Components::ConfigValues config_values(3);
            config_values.length(1);
            CORBA::Any value;
            value <<= componentWithConsumerGroup;
            config_values[0] = 
                new Cdmw::CCM::Container::ConfigValue_impl(
                                            CdmwDeployment::FAULT_TOLERANCE_GROUP_REF, 
                                            value);

            Components::CCMObject_var obj = 
                keyless_componentWithConsumerHome->create_component_with_config_values(config_values);

            TEST_FAILED();
        }
    } catch (const Components::CreateFailure& e) {
        std::ostringstream os;
        os << "Expected CreateFailure exception: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_SUCCEED();
    } catch (const CORBA::SystemException& e) {
        std::ostringstream os;
        os << "Unexpected SystemException exception: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_FAILED();
    }


    TEST_INFO("  Create a ComponentWithConsumer with invalid config values on a FT Home");
    try
    {
        CdmwCcmCif::KeylessCCMHome_var keyless_componentWithConsumerHome 
            = CdmwCcmCif::KeylessCCMHome::_narrow(FTComponentWithConsumerHome.in());

        if (CORBA::is_nil(keyless_componentWithConsumerHome.in())) {
            TEST_INFO("ERROR: Created home is not a keyless home!");
            TEST_FAILED();
        } else {
            Components::ConfigValues config_values(3);
            config_values.length(1);
            CORBA::Any value;
            value <<= componentWithConsumerGroup;
            config_values[0] = 
                new Cdmw::CCM::Container::ConfigValue_impl(
                                            CdmwDeployment::FAULT_TOLERANCE_REPLICATION_STYLE, 
                                            value);

            Components::CCMObject_var obj = 
                keyless_componentWithConsumerHome->create_component_with_config_values(config_values);

            TEST_FAILED();
        }
    } catch (const Components::CreateFailure& e) {
        std::ostringstream os;
        os << "Expected CreateFailure exception: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_SUCCEED();
    } catch (const CORBA::SystemException& e) {
        std::ostringstream os;
        os << "Unexpected SystemException exception: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_FAILED();
    }

    TEST_INFO("  Create a ComponentWithConsumer with empty config values on a FT Home");
    try
    {
        CdmwCcmCif::KeylessCCMHome_var keyless_componentWithConsumerHome 
            = CdmwCcmCif::KeylessCCMHome::_narrow(FTComponentWithConsumerHome.in());

        if (CORBA::is_nil(keyless_componentWithConsumerHome.in())) {
            TEST_INFO("ERROR: Created home is not a keyless home!");
            TEST_FAILED();
        } else {
            Components::ConfigValues config_values(3);
            config_values.length(0);

            Components::CCMObject_var obj = 
                keyless_componentWithConsumerHome->create_component_with_config_values(config_values);

            TEST_FAILED();
        }
    } catch (const Components::CreateFailure& e) {
        std::ostringstream os;
        os << "Expected CreateFailure exception: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_SUCCEED();
    } catch (const CORBA::SystemException& e) {
        std::ostringstream os;
        os << "Unexpected SystemException exception: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_FAILED();
    }

    TEST_INFO("  Create a ComponentWithConsumer with nil group_ref in config values on a FT Home");
    try
    {
        CdmwCcmCif::KeylessCCMHome_var keyless_componentWithConsumerHome 
            = CdmwCcmCif::KeylessCCMHome::_narrow(FTComponentWithConsumerHome.in());

        if (CORBA::is_nil(keyless_componentWithConsumerHome.in())) {
            TEST_INFO("ERROR: Created home is not a keyless home!");
            TEST_FAILED();
        } else {
            Components::ConfigValues config_values(3);
            CORBA::Any value;
            value <<= componentWithConsumer.in();
            config_values[0] = 
                new Cdmw::CCM::Container::ConfigValue_impl(
                                            CdmwDeployment::FAULT_TOLERANCE_GROUP_REF, 
                                            value);

            config_values.length(0);

            Components::CCMObject_var obj = 
                keyless_componentWithConsumerHome->create_component_with_config_values(config_values);

            TEST_FAILED();
        }
    } catch (const Components::CreateFailure& e) {
        std::ostringstream os;
        os << "Expected CreateFailure exception: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_SUCCEED();
    } catch (const CORBA::SystemException& e) {
        std::ostringstream os;
        os << "Unexpected SystemException exception: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_FAILED();
    }


    TEST_INFO("  Create a ComponentWithConsumer with config values on a FT Home");
    try
    {
        CdmwCcmCif::KeylessCCMHome_var keyless_componentWithConsumerHome 
            = CdmwCcmCif::KeylessCCMHome::_narrow(FTComponentWithConsumerHome.in());

        if (CORBA::is_nil(keyless_componentWithConsumerHome.in())) {
            TEST_INFO("ERROR: Created home is not a keyless home!");
            TEST_FAILED();
        } else {
            Components::ConfigValues config_values(3);
            config_values.length(1);
            CORBA::Any value;
            value <<= componentWithConsumerGroup;
            config_values[0] = 
                new Cdmw::CCM::Container::ConfigValue_impl(
                                            CdmwDeployment::FAULT_TOLERANCE_GROUP_REF, 
                                            value);

            Components::CCMObject_var obj = 
                keyless_componentWithConsumerHome->create_component_with_config_values(config_values);

            componentWithConsumer = 
                ComponentWithConsumer::_narrow(obj.in());

            if (CORBA::is_nil(componentWithConsumer.in())) {
                TEST_INFO("ERROR: Created component is not a TestFtCcmCif::ComponentWithConsumer!");
                TEST_FAILED();
            } else {
                TEST_SUCCEED();
                return componentWithConsumer._retn();
            }
        }
    } catch (const Components::CreateFailure& e) {
        std::ostringstream os;
        os << "Unexpected CreateFailure exception: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_FAILED();
    } catch (const CORBA::SystemException& e) {
        std::ostringstream os;
        os << "Unexpected SystemException exception: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_FAILED();
    }

    // an error occured
    throw CORBA::INTERNAL();
}

void
TestNavigationWithFT::call_ConfigurationComplete(Components::CCMObject_ptr component)
   throw (CORBA::Exception)
{
    add_nbOfRequestedTestOK(2);

    try {
       component->configuration_complete();
       TEST_SUCCEED();
    } catch (const Components::InvalidConfiguration& e) {
        std::ostringstream os;
        os << "Unexpected InvalidConfiguration exception: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_FAILED();
    } catch (const CORBA::SystemException& e) {
        std::ostringstream os;
        os << "Unexpected SystemException exception: " << e << std::ends;
        TEST_INFO(os.str());
        TEST_FAILED();
    }

    ComponentWithFacet_var facetComponent; 
    try {
        facetComponent = ComponentWithFacet::_narrow(component);

        if (CORBA::is_nil(facetComponent.in())) 
        {
            // component is a Consumer component
            // Check if its state file has been created:
            std::ifstream is("CCM_ComponentWithConsumer.stt");
            if (is.good())
            {
                TEST_INFO(" File CCM_ComponentWithConsumer.stt shall not exist now");
                TEST_FAILED();
            }
            else {
                TEST_SUCCEED();
            }
        }
        else
        {
            // component is a Facet component
            // Check if its state file has been created:
            std::ifstream is("CCM_ComponentWithFacet.stt");
            if (is.good())
            {
                TEST_INFO(" File CCM_ComponentWithFacet.stt shall not exist now");
                TEST_FAILED();
            }
            else {
                TEST_SUCCEED();
            }
        }
    } catch (const CORBA::Exception& e) {
        std::cerr << "Error while narrowing group to ComponentWithFacet: " 
                  << e << std::endl;
        TEST_FAILED();
        throw;
    }
}

ComponentWithFacet_ptr
TestNavigationWithFT::create_ComponentWithFacetGroup()
    throw (CORBA::Exception)
{
    add_nbOfRequestedTestOK(1);
    
    //
    // create FT::Criteria
    //
    ::FT::Properties prop;
    
    prop.length(4);
    prop[0].nam.length(1);
    prop[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
    prop[0].val <<= (CORBA::UShort)2;
    
    prop[1].nam.length(1);
    prop[1].nam[0].id="org.omg.ft.ReplicationStyle";
    prop[1].val <<= (CORBA::UShort)2;
    
    prop[2].nam.length(1);
    prop[2].nam[0].id="org.omg.ft.MembershipStyle";
    prop[2].val <<= (CORBA::UShort)0;
    
    prop[3].nam.length(1);
    prop[3].nam[0].id="org.omg.ft.Factories";
    
    ::FT::FactoryInfos factoryInfos;
    factoryInfos.length(2);
    factoryInfos[0].the_factory = ::FT::GenericFactory::_nil();
    factoryInfos[0].the_location = m_primaryLocation;
    ::FT::Criteria factoryCrit;        
    factoryCrit.length(0);
    factoryInfos[0].the_criteria = factoryCrit;
    
    factoryInfos[1].the_factory = ::FT::GenericFactory::_nil();
    factoryInfos[1].the_location = m_backupLocation;
    factoryInfos[1].the_criteria = factoryCrit;
    
    prop[3].val <<= factoryInfos;
    
    ::FT::Criteria ft_criteria;       
    ft_criteria.length(1);
    ft_criteria[0].nam.length(1);
    ft_criteria[0].nam[0].id="org.omg.ft.FTProperties";
    ft_criteria[0].val <<= prop;

    const char * repId = "IDL:thalesgroup.com/Cdmw_TestFtCcmCif/ComponentWithFacet:1.0";
    CORBA::Object_var group;
    ::FT::GenericFactory::FactoryCreationId_var creationId;
    
    //
    // Create group
    //
    try {
        group = m_replicationManager->create_object(repId,
                                                    ft_criteria,
                                                    creationId.out());

    } catch (const CORBA::Exception& e ) {
        std::cerr << "Error during group creation: " << e << std::endl;
        TEST_FAILED();
        throw;
    }

    ComponentWithFacet_var componentGroup; 
    try {
        componentGroup = ComponentWithFacet::_narrow(group.in());
    } catch (const CORBA::Exception& e) {
        std::cerr << "Error while narrowing group to ComponentWithFacet: " 
                  << e << std::endl;
        TEST_FAILED();
        throw;
    }
            
    if (CORBA::is_nil(componentGroup.in())) {
        std::cerr << "Error: componentGroup is null." << std::endl;
        TEST_FAILED();
        throw CORBA::INTERNAL();
    }

    TEST_SUCCEED(); 
    return componentGroup._retn();

}
    
ComponentWithFacet_ptr
TestNavigationWithFT::addMembers_ToComponentWithFacetGroup(
        ComponentWithFacet_ptr componentWithFacetGroup,
        ComponentWithFacet_ptr componentWithFacetOnPrimary,
        ComponentWithFacet_ptr componentWithFacetOnBackup)
    throw (CORBA::Exception)
{
    add_nbOfRequestedTestOK(1);

    CORBA::Object_var group;

    //
    // add members
    //
    try {
        group = m_replicationManager->add_member(componentWithFacetGroup,
                                                 m_primaryLocation,
                                                 componentWithFacetOnPrimary);
    } catch (const CORBA::Exception& e) {
        std::cerr << "Error while adding primary ComponentWithFacet: " 
                  << e << std::endl;
        TEST_FAILED();
        throw;
    }
    try {
        group = m_replicationManager->add_member(group.in(),
                                                 m_backupLocation,
                                                 componentWithFacetOnBackup);
    } catch (const CORBA::Exception& e) {
        std::cerr << "Error while adding backup ComponentWithFacet: " 
                  << e << std::endl;
        TEST_FAILED();
        throw;
    }
    
    ComponentWithFacet_var componentGroup; 
    try {
        componentGroup = ComponentWithFacet::_narrow(group.in());
    } catch (const CORBA::Exception& e) {
        std::cerr << "Error while narrowing group to ComponentWithFacet: " 
                  << e << std::endl;
        TEST_FAILED();
        throw;
    }
            
    if (CORBA::is_nil(componentGroup.in())) {
        std::cerr << "Error: componentGroup is null." << std::endl;
        TEST_FAILED();
        throw CORBA::INTERNAL();
    }

    TEST_SUCCEED(); 
    return componentGroup._retn();
}



ComponentWithConsumer_ptr
TestNavigationWithFT::create_ComponentWithConsumerGroup()
    throw (CORBA::Exception)
{
    add_nbOfRequestedTestOK(1);
    
    //
    // create FT::Criteria
    //
    ::FT::Properties prop;
    
    prop.length(4);
    prop[0].nam.length(1);
    prop[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
    prop[0].val <<= (CORBA::UShort)2;
    
    prop[1].nam.length(1);
    prop[1].nam[0].id="org.omg.ft.ReplicationStyle";
    prop[1].val <<= (CORBA::UShort)2;
    
    prop[2].nam.length(1);
    prop[2].nam[0].id="org.omg.ft.MembershipStyle";
    prop[2].val <<= (CORBA::UShort)0;
    
    prop[3].nam.length(1);
    prop[3].nam[0].id="org.omg.ft.Factories";
    
    ::FT::FactoryInfos factoryInfos;
    factoryInfos.length(2);
    factoryInfos[0].the_factory = ::FT::GenericFactory::_nil();
    factoryInfos[0].the_location = m_primaryLocation;
    ::FT::Criteria factoryCrit;        
    factoryCrit.length(0);
    factoryInfos[0].the_criteria = factoryCrit;
    
    factoryInfos[1].the_factory = ::FT::GenericFactory::_nil();
    factoryInfos[1].the_location = m_backupLocation;
    factoryInfos[1].the_criteria = factoryCrit;
    
    prop[3].val <<= factoryInfos;
    
    ::FT::Criteria ft_criteria;       
    ft_criteria.length(1);
    ft_criteria[0].nam.length(1);
    ft_criteria[0].nam[0].id="org.omg.ft.FTProperties";
    ft_criteria[0].val <<= prop;

    const char * repId = "IDL:thalesgroup.com/Cdmw_TestFtCcmCif/ComponentWithConsumer:1.0";
    CORBA::Object_var group;
    ::FT::GenericFactory::FactoryCreationId_var creationId;

    //
    // Create group
    //
    try {
        group = m_replicationManager->create_object(repId,
                                                    ft_criteria,
                                                    creationId.out());

    } catch (const CORBA::Exception& e ) {
        std::cerr << "Error during group creation: " << e << std::endl;
        TEST_FAILED();
        throw;
    }

    ComponentWithConsumer_var componentGroup; 
    try {
        componentGroup = ComponentWithConsumer::_narrow(group.in());
    } catch (const CORBA::Exception& e) {
        std::cerr << "Error while narrowing group to ComponentWithConsumer: " 
                  << e << std::endl;
        TEST_FAILED();
        throw;
    }
            
    if (CORBA::is_nil(componentGroup.in())) {
        std::cerr << "Error: componentGroup is null." << std::endl;
        TEST_FAILED();
        throw CORBA::INTERNAL();
    }

    TEST_SUCCEED(); 
    return componentGroup._retn();
}


ComponentWithConsumer_ptr
TestNavigationWithFT::addMembers_ToComponentWithConsumerGroup(
        ComponentWithConsumer_ptr componentWithConsumerGroup,
        ComponentWithConsumer_ptr componentWithConsumerOnPrimary,
        ComponentWithConsumer_ptr componentWithConsumerOnBackup)
    throw (CORBA::Exception)
{
    add_nbOfRequestedTestOK(1);

    CORBA::Object_var group;

    //
    // add members
    //
    try {
        group = m_replicationManager->add_member(componentWithConsumerGroup,
                                                 m_primaryLocation,
                                                 componentWithConsumerOnPrimary);
    } catch (const CORBA::Exception& e) {
        std::cerr << "Error while adding primary ComponentWithFacet: " 
                  << e << std::endl;
        TEST_FAILED();
        throw;
    }
    try {
        group = m_replicationManager->add_member(group.in(),
                                                 m_backupLocation,
                                                 componentWithConsumerOnBackup);
    } catch (const CORBA::Exception& e) {
        std::cerr << "Error while adding backup ComponentWithFacet: " 
                  << e << std::endl;
        TEST_FAILED();
        throw;
    }
    
    ComponentWithConsumer_var componentGroup; 
    try {
        componentGroup = ComponentWithConsumer::_narrow(group.in());
    } catch (const CORBA::Exception& e) {
        std::cerr << "Error while narrowing group to ComponentWithConsumer: " 
                  << e << std::endl;
        TEST_FAILED();
        throw;
    }
            
    if (CORBA::is_nil(componentGroup.in())) {
        std::cerr << "Error: componentGroup is null." << std::endl;
        TEST_FAILED();
        throw CORBA::INTERNAL();
    }

    TEST_SUCCEED(); 
    return componentGroup._retn();
}



bool 
TestNavigationWithFT::isGroupReference(CORBA::Object_ptr obj)
    throw (CORBA::Exception)
{
    try {
        std::cout << " --------- isGroupReference ----------" << std::endl;
        CORBA::String_var str = m_orb->object_to_string(obj);
        std::cout << str << std::endl;
        std::cout << " -------------------------------------" << std::endl;
        
        m_replicationManager->get_object_group_id(obj);
        return true;
            
    } catch (const FT::ObjectGroupNotFound& e) {
        return false;
    } catch (const CORBA::Exception& e) {
        std::cerr << "Exception in ReplicationManager->get_object_group_id(): "
                  << e << std::endl;
        throw;
    }
}




void 
TestNavigationWithFT::testNavigationInterface(ComponentWithFacet_ptr group,
                                              ComponentWithFacet_ptr primary,
                                              ComponentWithFacet_ptr backup)
    throw (CORBA::Exception)
{
    TEST_INFO("  test provide_facet()");
    try {
        add_nbOfRequestedTestOK(3);

        CORBA::Object_var facetGroup = 
            group->provide_facet(TEST_FACET_NAME);
        if (isGroupReference(facetGroup.in()))
            TEST_SUCCEED();
        else
            TEST_FAILED();

        CORBA::Object_var facetPrimary = 
            primary->provide_facet(TEST_FACET_NAME);
        if (isGroupReference(facetPrimary.in()))
            TEST_FAILED();
        else
            TEST_SUCCEED();

        CORBA::Object_var facetBackup = 
            backup->provide_facet(TEST_FACET_NAME);
        if (isGroupReference(facetBackup.in()))
            TEST_FAILED();
        else
            TEST_SUCCEED();
    } catch (const Components::InvalidName& e) {
        std::cerr << "group->provide_facet() threw InvalidName" << std::endl;
        TEST_FAILED();
    } catch (const CORBA::Exception& e) {
        std::cerr << "Unexpected exception while testing provide_facet() " 
                  << e << std::endl;
        TEST_FAILED();
    }
    
    TEST_INFO("  test get_all_facets()");
    try {
        Components::FacetDescriptions_var facetsGroup = 
            group->get_all_facets();
        for (CORBA::ULong i=0; i<facetsGroup->length(); ++i) {
            add_nbOfRequestedTestOK(1);
                if (isGroupReference(facetsGroup[i]->facet_ref()))
                    TEST_SUCCEED();
                else
                    TEST_FAILED();
        }
        
        Components::FacetDescriptions_var facetsPrimary = 
            primary->get_all_facets();
        for (CORBA::ULong i=0; i<facetsPrimary->length(); ++i) {
            add_nbOfRequestedTestOK(1);
                if (isGroupReference(facetsPrimary[i]->facet_ref()))
                    TEST_FAILED();
                else
                    TEST_SUCCEED();
        }
        
        Components::FacetDescriptions_var facetsBackup = 
            backup->get_all_facets();
        for (CORBA::ULong i=0; i<facetsBackup->length(); ++i) {
            add_nbOfRequestedTestOK(1);
                if (isGroupReference(facetsBackup[i]->facet_ref()))
                    TEST_FAILED();
                else
                    TEST_SUCCEED();
        }
    } catch (const CORBA::Exception& e) {
        std::cerr << "Unexpected exception while testing get_all_facets() " 
                  << e << std::endl;
        TEST_FAILED();
    }

    TEST_INFO("  test get_named_facets()");
    try {
        add_nbOfRequestedTestOK(3);
        
        Components::NameList nameList;
        nameList.length(1);
        nameList[0] = TEST_FACET_NAME;
        
        Components::FacetDescriptions_var facetsGroup = 
            group->get_named_facets(nameList);
        if (isGroupReference(facetsGroup[(CORBA::ULong) 0]->facet_ref()))
            TEST_SUCCEED();
        else
            TEST_FAILED();

        Components::FacetDescriptions_var facetsPrimary = 
            primary->get_named_facets(nameList);
        if (isGroupReference(facetsPrimary[(CORBA::ULong) 0]->facet_ref()))
            TEST_FAILED();
        else
            TEST_SUCCEED();

        Components::FacetDescriptions_var facetsBackup = 
            backup->get_named_facets(nameList);
        if (isGroupReference(facetsBackup[(CORBA::ULong) 0]->facet_ref()))
            TEST_FAILED();
        else
            TEST_SUCCEED();
        
    } catch (const Components::InvalidName& e) {
        std::cerr << "group->get_named_facets() threw InvalidName" << std::endl;
        TEST_FAILED();
    } catch (const CORBA::Exception& e) {
        std::cerr << "Unexpected exception while testing get_named_facets() " 
                  << e << std::endl;
        TEST_FAILED();
    }
}



void 
TestNavigationWithFT::testTypedNavigationInterface(ComponentWithFacet_ptr group,
                                                   ComponentWithFacet_ptr primary,
                                                   ComponentWithFacet_ptr backup)
    throw (CORBA::Exception)
{
    TEST_INFO("  test provide_the_facet()");
    try {
        add_nbOfRequestedTestOK(3);

        TestFtCcmCif::TestInterface_var facetGroup = 
            group->provide_the_facet();
        if (isGroupReference(facetGroup.in()))
            TEST_SUCCEED();
        else
            TEST_FAILED();

        TestFtCcmCif::TestInterface_var facetPrimary = 
            primary->provide_the_facet();
        if (isGroupReference(facetPrimary.in()))
            TEST_FAILED();
        else
            TEST_SUCCEED();

        TestFtCcmCif::TestInterface_var facetBackup = 
            backup->provide_the_facet();
        if (isGroupReference(facetBackup.in()))
            TEST_FAILED();
        else
            TEST_SUCCEED();
    } catch (const Components::InvalidName& e) {
        std::cerr << "group->provide_the_facet() threw InvalidName" << std::endl;
        TEST_FAILED();
    } catch (const CORBA::Exception& e) {
        std::cerr << "Unexpected exception while testing provide_the_facet() " 
                  << e << std::endl;
        TEST_FAILED();
    }
}


void 
TestNavigationWithFT::testEventsInterface(ComponentWithConsumer_ptr group,
                                          ComponentWithConsumer_ptr primary,
                                          ComponentWithConsumer_ptr backup)
    throw (CORBA::Exception)
{
    TEST_INFO("  test get_consumer()");
    try {
        add_nbOfRequestedTestOK(3);

        Components::EventConsumerBase_var consumerGroup = 
            group->get_consumer(TEST_CONSUMER_NAME);
        if (isGroupReference(consumerGroup.in()))
            TEST_SUCCEED();
        else
            TEST_FAILED();

        Components::EventConsumerBase_var consumerPrimary = 
            primary->get_consumer(TEST_CONSUMER_NAME);
        if (isGroupReference(consumerPrimary.in()))
            TEST_FAILED();
        else
            TEST_SUCCEED();

        Components::EventConsumerBase_var consumerBackup = 
            backup->get_consumer(TEST_CONSUMER_NAME);
        if (isGroupReference(consumerBackup.in()))
            TEST_FAILED();
        else
            TEST_SUCCEED();
    } catch (const Components::InvalidName& e) {
        std::cerr << "group->get_consumer() threw InvalidName" << std::endl;
        TEST_FAILED();
    } catch (const CORBA::Exception& e) {
        std::cerr << "Unexpected exception while testing get_consumer() " 
                  << e << std::endl;
        TEST_FAILED();
    }
    
    TEST_INFO("  test get_all_consumers()");
    try {
        Components::ConsumerDescriptions_var consumersGroup = 
            group->get_all_consumers();
        for (CORBA::ULong i=0; i<consumersGroup->length(); ++i) {
            add_nbOfRequestedTestOK(1);
                if (isGroupReference(consumersGroup[i]->consumer()))
                    TEST_SUCCEED();
                else
                    TEST_FAILED();
        }
        
        Components::ConsumerDescriptions_var consumersPrimary = 
            primary->get_all_consumers();
        for (CORBA::ULong i=0; i<consumersPrimary->length(); ++i) {
            add_nbOfRequestedTestOK(1);
                if (isGroupReference(consumersPrimary[i]->consumer()))
                    TEST_FAILED();
                else
                    TEST_SUCCEED();
        }
        
        Components::ConsumerDescriptions_var consumersBackup = 
            backup->get_all_consumers();
        for (CORBA::ULong i=0; i<consumersBackup->length(); ++i) {
            add_nbOfRequestedTestOK(1);
                if (isGroupReference(consumersBackup[i]->consumer()))
                    TEST_FAILED();
                else
                    TEST_SUCCEED();
        }
    } catch (const CORBA::Exception& e) {
        std::cerr << "Unexpected exception while testing get_all_consumers() " 
                  << e << std::endl;
        TEST_FAILED();
    }

    TEST_INFO("  test get_named_consumers()");
    try {
        add_nbOfRequestedTestOK(3);
        
        Components::NameList nameList;
        nameList.length(1);
        nameList[0] = TEST_CONSUMER_NAME;
        
        Components::ConsumerDescriptions_var consumersGroup = 
            group->get_named_consumers(nameList);
        if (isGroupReference(consumersGroup[(CORBA::ULong) 0]->consumer()))
            TEST_SUCCEED();
        else
            TEST_FAILED();

        Components::ConsumerDescriptions_var consumersPrimary = 
            primary->get_named_consumers(nameList);
        if (isGroupReference(consumersPrimary[(CORBA::ULong) 0]->consumer()))
            TEST_FAILED();
        else
            TEST_SUCCEED();

        Components::ConsumerDescriptions_var consumersBackup = 
            backup->get_named_consumers(nameList);
        if (isGroupReference(consumersBackup[(CORBA::ULong) 0]->consumer()))
            TEST_FAILED();
        else
            TEST_SUCCEED();
        
    } catch (const Components::InvalidName& e) {
        std::cerr << "group->get_named_consumers() threw InvalidName" << std::endl;
        TEST_FAILED();
    } catch (const CORBA::Exception& e) {
        std::cerr << "Unexpected exception while testing get_named_consumers() " 
                  << e << std::endl;
        TEST_FAILED();
    }
}


void 
TestNavigationWithFT::testTypedEventsInterface(ComponentWithConsumer_ptr group,
                                               ComponentWithConsumer_ptr primary,
                                               ComponentWithConsumer_ptr backup)
    throw (CORBA::Exception)
{
    TEST_INFO("  test get_consumer_the_consumer()()");
    try {
        add_nbOfRequestedTestOK(3);

        TestFtCcmCif::TestEventConsumer_var consumerGroup = 
            group->get_consumer_the_consumer();
        if (isGroupReference(consumerGroup.in()))
            TEST_SUCCEED();
        else
            TEST_FAILED();

        TestFtCcmCif::TestEventConsumer_var consumerPrimary = 
            primary->get_consumer_the_consumer();
        if (isGroupReference(consumerPrimary.in()))
            TEST_FAILED();
        else
            TEST_SUCCEED();

        TestFtCcmCif::TestEventConsumer_var consumerBackup = 
            backup->get_consumer_the_consumer();
        if (isGroupReference(consumerBackup.in()))
            TEST_FAILED();
        else
            TEST_SUCCEED();
    } catch (const Components::InvalidName& e) {
        std::cerr << "group->get_consumer_the_consumer()() threw InvalidName" << std::endl;
        TEST_FAILED();
    } catch (const CORBA::Exception& e) {
        std::cerr << "Unexpected exception while testing get_consumer_the_consumer()() " 
                  << e << std::endl;
        TEST_FAILED();
    }
}


void TestNavigationWithFT::do_tests()        
{
    // int timescale = Cdmw::TestUtils::Testable::get_timescale();

    // set number of requested successfull tests
    set_nbOfRequestedTestOK (0);

    try 
    {
        // delete state file
        Cdmw::OsSupport::OS::unlink("CCM_ComponentWithFacet.stt");
        Cdmw::OsSupport::OS::unlink("CCM_ComponentWithConsumer.stt");
    }
    catch(...)
    {
    }
    
    ComponentWithFacet_var  componentWithFacetOnPrimary;
    ComponentWithConsumer_var componentWithConsumerOnPrimary;
    ComponentWithFacet_var  componentWithFacetOnBackup;
    ComponentWithConsumer_var componentWithConsumerOnBackup;
    ComponentWithFacet_var  componentWithFacetGroup;
    ComponentWithConsumer_var componentWithConsumerGroup;

    try {
        TEST_INFO("Create ComponentWithFacet group");
        componentWithFacetGroup = create_ComponentWithFacetGroup();
        
        TEST_INFO("Create ComponentWithConsumer group");
        componentWithConsumerGroup = create_ComponentWithConsumerGroup();

        TEST_INFO("Create primary ComponentWithFacet");
        componentWithFacetOnPrimary = 
            create_ComponentWithFacet(m_containerPrimary.in(),
                                      componentWithFacetGroup.in());
    
        TEST_INFO("Create primary ComponentWithConsumer"); 
        componentWithConsumerOnPrimary = 
            create_ComponentWithConsumer(m_containerPrimary.in(),
                                         componentWithConsumerGroup.in());
    
        TEST_INFO("Create backup ComponentWithFacet");
        componentWithFacetOnBackup = 
            create_ComponentWithFacet(m_containerBackup.in(),
                                      componentWithFacetGroup.in());
    
        TEST_INFO("Create backup ComponentWithConsumer"); 
        componentWithConsumerOnBackup = 
            create_ComponentWithConsumer(m_containerBackup.in(),
                                         componentWithConsumerGroup.in());

        TEST_INFO("Call configuration_complete on all created components");
        call_ConfigurationComplete(componentWithFacetOnPrimary.in());
        call_ConfigurationComplete(componentWithConsumerOnPrimary.in());
        call_ConfigurationComplete(componentWithFacetOnBackup.in());
        call_ConfigurationComplete(componentWithConsumerOnBackup.in());

        TEST_INFO("Add members to ComponentWithFacet group");
        componentWithFacetGroup = 
            addMembers_ToComponentWithFacetGroup(componentWithFacetGroup.in(),
                                                 componentWithFacetOnPrimary.in(),
                                                 componentWithFacetOnBackup.in());
        
        TEST_INFO("Add members ComponentWithConsumer group");
        componentWithConsumerGroup = 
            addMembers_ToComponentWithConsumerGroup(componentWithConsumerGroup.in(),
                                                    componentWithConsumerOnPrimary.in(),
                                                    componentWithConsumerOnBackup.in());

        TEST_INFO("Has primary ComponentWithFacet been activated ?");
        add_nbOfRequestedTestOK(1);
        std::ifstream is1("CCM_ComponentWithFacet.stt");
        if (is1.good()) {
            TEST_SUCCEED();
        }
        else {
            TEST_INFO(" File CCM_ComponentWithFacet.stt doesn't exist");
            TEST_FAILED();
        }
                                                    
        TEST_INFO("Has primary ComponentWithConsumer been activated ?");
        add_nbOfRequestedTestOK(1);
        std::ifstream is2("CCM_ComponentWithConsumer.stt");
        if (is2.good()) {
            TEST_SUCCEED();
        }
        else {
            TEST_INFO(" File CCM_ComponentWithConsumer.stt doesn't exist");
            TEST_FAILED();
        }
                                                    
        TEST_INFO("Test Navigation interface");
        testNavigationInterface(componentWithFacetGroup.in(),
                                componentWithFacetOnPrimary.in(),
                                componentWithFacetOnBackup.in());
            
        TEST_INFO("Test Typed Navigation interface");
        testTypedNavigationInterface(componentWithFacetGroup.in(),
                                     componentWithFacetOnPrimary.in(),
                                     componentWithFacetOnBackup.in());
            
        TEST_INFO("Test Events interface");
        testEventsInterface(componentWithConsumerGroup.in(),
                            componentWithConsumerOnPrimary.in(),
                            componentWithConsumerOnBackup.in());
            
        TEST_INFO("Test Typed Events interface");
        testTypedEventsInterface(componentWithConsumerGroup.in(),
                                 componentWithConsumerOnPrimary.in(),
                                 componentWithConsumerOnBackup.in());
            
        // TODO: add a test when the primary crash
        // to do so add the location name in stt file : to get location
        // do a resolve_initial_references("FTGroupRepository") on the context (deprecated op)
        // kill primary process and advise the Fault Notifier of the death of this process.
            
    } 
    catch (const CORBA::Exception & e) 
    {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str()); 
            TEST_CHECK(false);
    }

    
    try 
    {
        // delete state file
        Cdmw::OsSupport::OS::unlink("CCM_ComponentWithFacet.stt");
        Cdmw::OsSupport::OS::unlink("CCM_ComponentWithConsumer.stt");
    }
    catch(...)
    {
    }
}
        
}; // End namespace 

