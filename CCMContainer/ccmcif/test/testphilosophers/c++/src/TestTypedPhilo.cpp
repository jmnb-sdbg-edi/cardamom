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

#include "testphilosophers/TestTypedPhilo.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/testutils/Testable.hpp"

#include "CCMContainer/ccmcontainer/ConfigValue_impl.hpp"
#include "testphilosophers/Philo_cif.skel.hpp"
#include "CCMContainer/idllib/CdmwDeployment.skel.hpp"

namespace
{
    const char* PHILOSOPHER_HOME_REP_ID = "IDL:acme.com/Cdmw_Dinner/PhilosopherHome:1.0";
    const char* PHILOSOPHER_HOME_SERVANT_NAME = "Cdmw.CCM.CIF.CdmwDinner.SessionPhilosopherHome_impl";
    const char* PHILOSOPHER_HOME_UID = "PhilosopherHome_1";
    const char* PHILOSOPHER_HOME_ENTRY_POINT = "create_CCM_PhilosopherHome";

    const char* OBSERVER_HOME_REP_ID = "IDL:acme.com/Cdmw_Dinner/ObserverHome:1.0";
    const char* OBSERVER_HOME_SERVANT_NAME = "Cdmw.CCM.CIF.CdmwDinner.SessionObserverHome_impl";
    const char* OBSERVER_HOME_UID = "ObserverHome_1";
    const char* OBSERVER_HOME_ENTRY_POINT = "create_CCM_ObserverHome";

     const char* FORKMANAGER_HOME_REP_ID = "IDL:acme.com/Cdmw_Dinner/ForkHome:1.0";
    const char* FORKMANAGER_HOME_SERVANT_NAME = "Cdmw.CCM.CIF.CdmwDinner.SessionForkHome_impl";
    const char* FORKMANAGER_HOME_UID = "ForkHome_1";
    const char* FORKMANAGER_HOME_ENTRY_POINT = "create_CCM_ForkHome";

     const char* REGISTRATOR_HOME_REP_ID = "IDL:acme.com/Cdmw_Dinner/RegistratorHome:1.0";
    const char* REGISTRATOR_HOME_SERVANT_NAME = "Cdmw.CCM.CIF.CdmwDinner.SessionRegistratorHome_impl";
    const char* REGISTRATOR_HOME_UID = "RegistratorHome_1";
    const char* REGISTRATOR_HOME_ENTRY_POINT = "create_CCM_RegistratorHome";


    const char* FORK_FACET_NAME = "the_fork";

     const char* FORK_RIGHT_RECEP_NAME = "right";
     const char* FORK_LEFT_RECEP_NAME = "left";
    const char* REGISTER_RECEP_NAME = "register";

     const char* INFO_CONSUMER_NAME = "info";
     const char* INFO_PUBLISHER_NAME = "info";
     const char* CONFIG_EMITTER_NAME = "config";
     

    const char* EVENT_STATUSINFO_REP_ID = "IDL:acme.com/Dinner/StatusInfo:1.0";
    const char* EVENT_STATUSINFO_ENTRYPOINT = "createStatusInfoFactory";


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


TestTypedPhilo::TestTypedPhilo(CORBA::ORB_ptr orb, const char* containerIOR)
   throw(CORBA::SystemException)
    :Testable("Cdmw::CCM::CIF::TypedPhilo"),
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


TestTypedPhilo::~TestTypedPhilo()
    throw()
{
    // does nothing
}


TestTypedPhilo::TestTypedPhilo(const TestTypedPhilo& rhs)
    throw()
    :Testable("Cdmw::CCM::CIF::Philo"),
     m_orb(CORBA::ORB::_duplicate(rhs.m_orb)),
     m_container(Components::Deployment::Container::_duplicate(rhs.m_container))
{
    // does nothing
}
        
TestTypedPhilo&
TestTypedPhilo::operator=(const TestTypedPhilo& rhs)
    throw()
{
    if (this != &rhs) {
        m_orb = CORBA::ORB::_duplicate(rhs.m_orb.in());
        m_container = Components::Deployment::Container::_duplicate(rhs.m_container.in());
    }
    return *this;
}

void TestTypedPhilo::do_tests()        
{
     // set number of requested successfull tests
    set_nbOfRequestedTestOK (45);
    
    bool test_ok=false;
    Components::CCMHome_var philosopher_home;
    Dinner::PhilosopherHome_var dinner_philosopher_home;
    Components::CCMObject_var ccm_philosopher_comp;
    Dinner::Philosopher_var dinner_philosopher_comp1;
    Dinner::Philosopher_var dinner_philosopher_comp2;
    Dinner::Philosopher_var dinner_philosopher_comp3;
    Dinner::Philosopher_var dinner_philosopher_comp4;

    Components::CCMHome_var observer_home;
    Dinner::ObserverHome_var dinner_observer_home;
    Components::CCMObject_var ccm_observer_comp;
    Dinner::Observer_var dinner_observer_comp;
    Dinner::StatusInfoConsumer_var info_consumer;

    Components::CCMHome_var fork_home;
    Dinner::ForkHome_var dinner_fork_home;
    Components::CCMObject_var ccm_fork_comp;
    Dinner::ForkManager_var dinner_fork_comp1;
    Dinner::ForkManager_var dinner_fork_comp2;
    Dinner::ForkManager_var dinner_fork_comp3;
    Dinner::ForkManager_var dinner_fork_comp4;
     Dinner::Fork_var fork_facet_obj1;
     Dinner::Fork_var fork_facet_obj2;
     Dinner::Fork_var fork_facet_obj3;
     Dinner::Fork_var fork_facet_obj4;

    Components::CCMHome_var registrator_home;
    Dinner::RegistratorHome_var dinner_registrator_home;
    Dinner::Registrator_var dinner_registrator_comp;

    Components::Cookie_var ck1;
    Components::Cookie_var ck2;
    Components::Cookie_var ck2_bis;
    Components::Cookie_var ck3;
    Components::Cookie_var ck4;

    int timescale = Cdmw::TestUtils::Testable::get_timescale();

    // This tests CIF classes
    //-------------------------------------------------
    TEST_INFO("Testing CIF classes");
    
    try 
    {    
        TEST_INFO("Install a PhilosopherHome");
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

            value <<= PHILOSOPHER_HOME_REP_ID;
            config_values[3]
                = new Container::ConfigValue_impl(CdmwDeployment::HOME_REPOSITORY_ID, 
                                                   value);

            value <<= PHILOSOPHER_HOME_SERVANT_NAME;
            config_values[4]
                = new Container::ConfigValue_impl(CdmwDeployment::HOME_SERVANT_CLASSNAME, 
                                                   value);
            philosopher_home 
               = m_container->install_home(PHILOSOPHER_HOME_UID,
                                           PHILOSOPHER_HOME_ENTRY_POINT,
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
        
        TEST_INFO("Install a ObserverHome");
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

            value <<= OBSERVER_HOME_REP_ID;
            config_values[3] = new Container::ConfigValue_impl(CdmwDeployment::HOME_REPOSITORY_ID, 
                                                   value);

            value <<= OBSERVER_HOME_SERVANT_NAME;
            config_values[4] = new Container::ConfigValue_impl(CdmwDeployment::HOME_SERVANT_CLASSNAME, 
                                                   value);

            CORBA::ULong lg = 1L;
            CdmwDeployment::ValuetypeFactoryDescriptionSeq seq(lg);
            seq.length(lg);
            // Event StatusInfo valuetype
            {
                CdmwDeployment::ValuetypeFactoryDescription desc;
                desc.repid = EVENT_STATUSINFO_REP_ID;
                desc.factory_entrypoint = EVENT_STATUSINFO_ENTRYPOINT;
                seq[0]=desc;
            }            
            value <<= seq;
            config_values[5] = new Container::ConfigValue_impl(CdmwDeployment::VALUETYPE_FACTORY_DEPENDENCIES,
                                                   value);

            observer_home 
               = m_container->install_home(OBSERVER_HOME_UID,
                                           OBSERVER_HOME_ENTRY_POINT,
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
        TEST_INFO("Install a ForkHome");
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

            value <<= FORKMANAGER_HOME_REP_ID;
            config_values[3]
                = new Container::ConfigValue_impl(CdmwDeployment::HOME_REPOSITORY_ID, 
                                                   value);

            value <<= FORKMANAGER_HOME_SERVANT_NAME;
            config_values[4]
                = new Container::ConfigValue_impl(CdmwDeployment::HOME_SERVANT_CLASSNAME, 
                                                   value);
            fork_home 
               = m_container->install_home(FORKMANAGER_HOME_UID,
                                           FORKMANAGER_HOME_ENTRY_POINT,
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

                  TEST_INFO("Install a RegistratorHome");
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

            value <<= REGISTRATOR_HOME_REP_ID;
            config_values[3]
                = new Container::ConfigValue_impl(CdmwDeployment::HOME_REPOSITORY_ID, 
                                                   value);

            value <<= REGISTRATOR_HOME_SERVANT_NAME;
            config_values[4]
                = new Container::ConfigValue_impl(CdmwDeployment::HOME_SERVANT_CLASSNAME, 
                                                   value);
            registrator_home 
               = m_container->install_home(REGISTRATOR_HOME_UID,
                                           REGISTRATOR_HOME_ENTRY_POINT,
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

          
        TEST_INFO("Create a Philosopher component using create_component() operation");
        test_ok=false;
        try
        {
            Components::KeylessCCMHome_var keyless_philosopher_home 
                = Components::KeylessCCMHome::_narrow(philosopher_home.in());

            if (CORBA::is_nil(keyless_philosopher_home.in()))
            {
                TEST_INFO("ERROR: Created home is not a keyless home!");
            }
            else
            {
                ccm_philosopher_comp
                   = keyless_philosopher_home->create_component();

                dinner_philosopher_comp1
                    = Dinner::Philosopher::_narrow(ccm_philosopher_comp.in());

                if (CORBA::is_nil(dinner_philosopher_comp1.in()))
                {
                    TEST_INFO("ERROR: Created component is not a Dinner::Philosopher!")
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
          
           TEST_INFO("Create other Philosopher component using create() operation");
        test_ok=false;
        try
        {
            dinner_philosopher_home 
                = Dinner::PhilosopherHome::_narrow(philosopher_home.in());

            if (CORBA::is_nil(dinner_philosopher_home.in()))
            {
                TEST_INFO("Home is not a Dinner::PhilosopherHome!");
            }
            else
            {
                dinner_philosopher_comp2 = dinner_philosopher_home->create();
                dinner_philosopher_comp3 = dinner_philosopher_home->create();
                dinner_philosopher_comp4 = dinner_philosopher_home->create();

                if (CORBA::is_nil(dinner_philosopher_comp2.in()))
                {
                    TEST_INFO("ERROR: dinner_philosopher_comp2 is nil!");
                }
                else
                {
                    if (CORBA::is_nil(dinner_philosopher_comp3.in()))
                    {
                        TEST_INFO("ERROR: dinner_philosopher_comp3 is nil!");
                    }
                    else
                    {
                               if (CORBA::is_nil(dinner_philosopher_comp4.in()))
                        {
                            TEST_INFO("ERROR: dinner_philosopher_comp4 is nil!");
                        }
                        else
                        {
                            test_ok=true;
                                }
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
       
        TEST_INFO("Create a Observer component using create_component() operation");
        test_ok=false;
        try
        {
            Components::KeylessCCMHome_var keyless_observer_home 
                = Components::KeylessCCMHome::_narrow(observer_home.in());

            if (CORBA::is_nil(keyless_observer_home.in()))
            {
                TEST_INFO("ERROR: Created home is not a keyless home!");
            }
            else
            {
                ccm_observer_comp
                   = keyless_observer_home->create_component();

                dinner_observer_comp
                    = Dinner::Observer::_narrow(ccm_observer_comp.in());

                if (CORBA::is_nil(dinner_observer_comp.in()))
                {
                    TEST_INFO("ERROR: Created component is not a Dinner::Observer!")
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

          TEST_INFO("Create a ForkManager component using create_component() operation");
        test_ok=false;
        try
        {
            Components::KeylessCCMHome_var keyless_fork_home 
                = Components::KeylessCCMHome::_narrow(fork_home.in());

            if (CORBA::is_nil(keyless_fork_home.in()))
            {
                TEST_INFO("ERROR: Created home is not a keyless home!");
            }
            else
            {
                ccm_fork_comp
                   = keyless_fork_home->create_component();

                dinner_fork_comp1
                    = Dinner::ForkManager::_narrow(ccm_fork_comp.in());

                if (CORBA::is_nil(dinner_fork_comp1.in()))
                {
                    TEST_INFO("ERROR: Created component is not a Dinner::ForkManager!")
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


          TEST_INFO("Create other ForkManager component using create() operation");
        test_ok=false;
        try
        {
            dinner_fork_home 
                = Dinner::ForkHome::_narrow(fork_home.in());

            if (CORBA::is_nil(dinner_fork_home.in()))
            {
                TEST_INFO("Home is not a Dinner::ForkHome!");
            }
            else
            {
                dinner_fork_comp2 = dinner_fork_home->create();
                dinner_fork_comp3 = dinner_fork_home->create();
                dinner_fork_comp4 = dinner_fork_home->create();

                if (CORBA::is_nil(dinner_fork_comp2.in()))
                {
                    TEST_INFO("ERROR: dinner_fork_comp2 is nil!");
                }
                else
                {
                    if (CORBA::is_nil(dinner_fork_comp3.in()))
                    {
                        TEST_INFO("ERROR: dinner_fork_comp3 is nil!");
                    }
                    else
                    {
                               if (CORBA::is_nil(dinner_fork_comp4.in()))
                        {
                            TEST_INFO("ERROR: dinner_fork_comp4 is nil!");
                        }
                        else
                        {
                            test_ok=true;
                                }
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


        TEST_INFO("Create Registrator component using create() operation");
        test_ok=false;
        try
        {
            dinner_registrator_home 
                = Dinner::RegistratorHome::_narrow(registrator_home.in());

            if (CORBA::is_nil(dinner_registrator_home.in()))
            {
                TEST_INFO("Home is not a Dinner::RegistratorHome!");
            }
            else
            {
                dinner_registrator_comp = dinner_registrator_home->create();

                if (CORBA::is_nil(dinner_registrator_comp.in()))
                {
                    TEST_INFO("ERROR: dinner_registrator_comp is nil!");
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

        TEST_INFO("Testing generated operation on ForkManager component...");
        TEST_INFO("provide_the_fork operation on ForkManager component...");
        test_ok=false;
        try
        {
            fork_facet_obj1 = dinner_fork_comp1->provide_the_fork();
            fork_facet_obj2 = dinner_fork_comp2->provide_the_fork();
            fork_facet_obj3 = dinner_fork_comp3->provide_the_fork();
            fork_facet_obj4 = dinner_fork_comp4->provide_the_fork();

            if (CORBA::is_nil(fork_facet_obj1.in()))
            {
                TEST_INFO("ERROR: no facet found for ForkManager1 component");
            }
                else if (CORBA::is_nil(fork_facet_obj2.in()))
            {
                TEST_INFO("ERROR: No facet found for ForkManager2 component");
            }
                else if (CORBA::is_nil(fork_facet_obj3.in()))
            {
                TEST_INFO("ERROR: No facet found for ForkManager3 component");
            }
            else if (CORBA::is_nil(fork_facet_obj4.in()))
            {
                TEST_INFO("ERROR: No facet found for ForkManager4 component");
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
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Connect operation on Philosopher components to ForkManager...");
        test_ok=false;
        try
        {
            dinner_philosopher_comp1->connect_right(fork_facet_obj1.in());
            dinner_philosopher_comp1->connect_left(fork_facet_obj2.in());
            dinner_philosopher_comp2->connect_right(fork_facet_obj2.in());
            dinner_philosopher_comp2->connect_left(fork_facet_obj3.in());
            dinner_philosopher_comp3->connect_right(fork_facet_obj3.in());
            dinner_philosopher_comp3->connect_left(fork_facet_obj4.in());
            dinner_philosopher_comp4->connect_right(fork_facet_obj4.in());
            dinner_philosopher_comp4->connect_left(fork_facet_obj1.in());
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

        TEST_INFO("Connect operation again on Philosopher components to ForkManager...");
        test_ok=false;
        try
        {
            dinner_philosopher_comp1->connect_right(fork_facet_obj1.in());
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

          TEST_INFO("Connect operation on Philosopher components to Registrator...");
        test_ok=false;
        try
        {
            dinner_philosopher_comp1->connect_register(dinner_registrator_comp.in());
            dinner_philosopher_comp2->connect_register(dinner_registrator_comp.in());
            dinner_philosopher_comp3->connect_register(dinner_registrator_comp.in());
            dinner_philosopher_comp4->connect_register(dinner_registrator_comp.in());
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

        TEST_INFO("Get_connections operation on Philosopher component...");
        test_ok=false;
        try
        {
            Components::ConnectionDescriptions_var conx_desc
                = dinner_philosopher_comp1->get_connections(REGISTER_RECEP_NAME);

            if (conx_desc->length() != 1)
            {
                TEST_INFO("ERROR: Unexpected number of connection found for Philosopher component => " << conx_desc->length());
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

        TEST_INFO("Get_all_publishers operation on Philosopher component...");
        test_ok=false;
        try
        {
            Components::PublisherDescriptions_var publisher_desc
                = dinner_philosopher_comp1->get_all_publishers();

            if (publisher_desc->length() != 1)
            {
                TEST_INFO("ERROR: Unexpected number of publisher found for Philosopher component");
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

        TEST_INFO("Get_all_emitters operation on Philosopher component...");
        test_ok=false;
        try
        {
            Components::EmitterDescriptions_var emitter_desc
                = dinner_philosopher_comp1->get_all_emitters();

            if (emitter_desc->length() != 1)
            {
                TEST_INFO("ERROR: Unexpected number of emitter found for Philosopher component");
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

        TEST_INFO("Get_all_consumers operation on Observer component...");
        test_ok=false;
        try
        {
            Components::ConsumerDescriptions_var consumer_desc
                = dinner_observer_comp->get_all_consumers();

            if (consumer_desc->length() != 1)
            {
                TEST_INFO("ERROR: Unexpected number of consumers found for Observer component");
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

          TEST_INFO("Get_named_consumers operation on Observer component...");
        test_ok=false;
        try
        {                
            Components::NameList consumer_name_list(2);
            consumer_name_list.length(1);
            consumer_name_list[0] = INFO_CONSUMER_NAME;
            Components::ConsumerDescriptions_var consumer_desc
                = dinner_observer_comp->get_named_consumers(consumer_name_list);

            if (consumer_desc->length() != 1)
            {
                TEST_INFO("ERROR: Unexpected number of consumers found for Observer component");
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

          
          TEST_INFO("Get_consumer_info operation on Observer component...");
        test_ok=false;
        try
        {                
            info_consumer
                = dinner_observer_comp->get_consumer_info();

            if (CORBA::is_nil(info_consumer.in()))
            {
                TEST_INFO("ERROR: no consumer found for Observer component");
            }
            else
            {
                test_ok=true;
            }
        }
          catch (const Components::InvalidName& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO("ERROR: InvalidName exception raised!");
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

  		  TEST_INFO("Call _get_component operation on a consumer");
        test_ok=false;
        try
        {
			   CORBA::Object_var obj = info_consumer->_get_component();

				if (CORBA::is_nil(obj.in()))
            {
					 test_ok = true;
            }
				else
				{
				    if (obj->_is_equivalent(dinner_observer_comp.in()))
				    {
                   TEST_INFO("ERROR: _get_component doesn't return nil but Observer component!");
				    }
				    else
				    {
						 if (obj->_is_equivalent(info_consumer.in()))
						 {
                       TEST_INFO("ERROR: _get_component doesn't return nil but consumer ref!");
						 }
				    }
		      }
		  }
		  catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
        }
        TEST_CHECK(test_ok);

        TEST_INFO("Subscribe operation on Philosopher components from Observer...");
        test_ok=false;
        try
        {
            ck1 = dinner_philosopher_comp1->subscribe_info(info_consumer.in());
            ck2 = dinner_philosopher_comp2->subscribe_info(info_consumer.in());
            ck3 = dinner_philosopher_comp3->subscribe_info(info_consumer.in());
            ck4 = dinner_philosopher_comp4->subscribe_info(info_consumer.in());
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

           TEST_INFO("Try to unsubscribe operation on Philosopher1 components with invalid cookie...");
        test_ok=false;
        try
        {
            dinner_philosopher_comp1->unsubscribe_info(ck2);
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

           TEST_INFO("Unsubscribe operation on Philosopher1 components from Observer...");
        test_ok=false;
        try
        {
            dinner_philosopher_comp1->unsubscribe_info(ck1);
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

          TEST_INFO("Subscribe operation on Philosopher1 components from Observer...");
        test_ok=false;
        try
        {
            ck1 = dinner_philosopher_comp1->subscribe_info(info_consumer.in());
            ck2_bis = dinner_philosopher_comp2->subscribe_info(info_consumer.in());
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

          TEST_INFO("Connect_config operation on Philosopher1 components from Observer...");
        test_ok=false;
        try
        {
            dinner_philosopher_comp1->connect_config(info_consumer.in());
            test_ok=true;
        }
        catch (const Components::InvalidName& )
        {
            TEST_INFO("Unexpected exception raised: InvalidName");
        }
        catch (const Components::AlreadyConnected& )
        {
            TEST_INFO("Unexpected exception raised: AlreadyConnected");
        }
        catch (const Components::InvalidConnection& )
        {
            TEST_INFO("Unexpected exception raised: InvalidConnection");
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
          
          TEST_INFO("Try to connect_config again operation on Philosopher1 components from Observer...");
        test_ok=false;
        try
        {
            dinner_philosopher_comp1->connect_config(info_consumer.in());
        }
        catch (const Components::InvalidName& )
        {
            TEST_INFO("Unexpected exception raised: InvalidName");
        }
        catch (const Components::AlreadyConnected& )
        {
            TEST_INFO("Expected exception raised: AlreadyConnected");
            test_ok=true;
        }
        catch (const Components::InvalidConnection& )
        {
            TEST_INFO("Unexpected exception raised: InvalidConnection");
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

          TEST_INFO("Disconnect_config operation on Philosopher1 components from Observer...");
        test_ok=false;
        try
        {
               Components::EventConsumerBase_var consumer
               = dinner_philosopher_comp1->disconnect_config();
            test_ok=true;
        }
        catch (const Components::InvalidName& )
        {
            TEST_INFO("Unexpected exception raised: InvalidName");
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
          
          TEST_INFO("Disconnect_config operation again on Philosopher1 components from Observer...");
        test_ok=false;
        try
        {
               Components::EventConsumerBase_var consumer
               = dinner_philosopher_comp1->disconnect_config();
        }
        catch (const Components::InvalidName& )
        {
            TEST_INFO("Unexpected exception raised: InvalidName");
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

           TEST_INFO("Disconnect_config operation on Philosopher1 components from Observer...");
        test_ok=false;
        try
        {
              Components::EventConsumerBase_var consumer
                  = dinner_philosopher_comp2->disconnect_config();
        }
        catch (const Components::InvalidName& )
        {
            TEST_INFO("Unexpected exception raised: InvalidName");
        }
        catch (const Components::NoConnection& )
        {
            TEST_INFO("Unexpected exception raised: NoConnection");
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

          TEST_INFO("Try to connect_config again operation on Philosopher1 components from Observer...");
        test_ok=false;
        try
        {
            dinner_philosopher_comp1->connect_config(info_consumer.in());

            test_ok=true;
        }
        catch (const Components::InvalidName& )
        {
            TEST_INFO("Unexpected exception raised: InvalidName");
        }
        catch (const Components::AlreadyConnected& )
        {
            TEST_INFO("Unexpected exception raised: AlreadyConnected");
        }
        catch (const Components::InvalidConnection& )
        {
            TEST_INFO("Unexpected exception raised: InvalidConnection");
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

        TEST_INFO("Get_ccm_home operation on Observer component...");
        test_ok=false;
        try
        {
            Components::CCMHome_var c_home
                = dinner_observer_comp->get_ccm_home();

            if (!c_home->_is_equivalent(observer_home.in()))
            {
                TEST_INFO("ERROR: Unexpected home for Observer component");
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

        TEST_INFO("Get_primary_key operation on Observer component...");
        test_ok=false;
        try
        {
            Components::PrimaryKeyBase_var key
                = dinner_observer_comp->get_primary_key();
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

        TEST_INFO("Get_all_ports operation on Philosopher component...");
        test_ok=false;
        try
        {
            Components::ComponentPortDescription_var port_desc
                = dinner_philosopher_comp2->get_all_ports();

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
                TEST_INFO("ERROR: unexpected number of facet for Philosopher component");
            }
            else if (recep_desc.length() != 3)
            {
                TEST_INFO("ERROR: unexpected number of receptacle for Philosopher component");
            }
            else if (consumer_desc.length() != 0)
            {
                TEST_INFO("ERROR: unexpected number of consumer for Philosopher component");
            }
            else if (publisher_desc.length() != 1)
            {
                TEST_INFO("ERROR: unexpected number of publisher for Philosopher component");
            }
            else if (emitter_desc.length() != 1)
            {
                TEST_INFO("ERROR: unexpected number of emitter for Philosopher component");
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


        TEST_INFO("Configuration_complete operation on Philosopher components...");
        test_ok=false;
        try
        {
               TEST_INFO("Configuration_complete operation on Philosopher1 components...");
            dinner_philosopher_comp1->configuration_complete();

               OsSupport::OS::sleep(timescale*1000);

               TEST_INFO("Configuration_complete operation on Philosopher2 components...");
            dinner_philosopher_comp2->configuration_complete();
               OsSupport::OS::sleep(timescale*1000);

               TEST_INFO("Configuration_complete operation on Philosopher3 components...");
            dinner_philosopher_comp3->configuration_complete();
               OsSupport::OS::sleep(timescale*1000);
                
               TEST_INFO("Configuration_complete operation on Philosopher4 components...");
            dinner_philosopher_comp4->configuration_complete();
               OsSupport::OS::sleep(timescale*1000);

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

        TEST_INFO("Configuration_complete operation on Observer component...");
        test_ok=false;
        try
        {
            dinner_observer_comp->configuration_complete();
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

        TEST_INFO("Get name of Philosopher component1...");
        test_ok=false;
        try
        {
            CORBA::String_var philosopher_name = dinner_philosopher_comp1->name();
            std::ostringstream os1;
            os1 << "Philosopher1 name is " << philosopher_name.in() << std::ends;
            TEST_INFO(os1.str());

            philosopher_name = dinner_philosopher_comp2->name();
            std::ostringstream os2;
            os2 << "Philosopher2 name is " << philosopher_name.in() << std::ends;
            TEST_INFO(os2.str());
                
            philosopher_name = dinner_philosopher_comp3->name();
            std::ostringstream os3;
            os3 << "Philosopher3 name is " << philosopher_name.in() << std::ends;
            TEST_INFO(os3.str());
            
                philosopher_name = dinner_philosopher_comp4->name();
            std::ostringstream os4;
            os4 << "Philosopher4 name is " << philosopher_name.in() << std::ends;
            TEST_INFO(os4.str());
            
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

          OsSupport::OS::sleep(timescale*30000);

        TEST_INFO("Disconnect_left and right operation on Philosopher component...");
        test_ok=false;
        try
        {
            dinner_philosopher_comp1->disconnect_left();
            dinner_philosopher_comp2->disconnect_left();
            dinner_philosopher_comp3->disconnect_left();
            dinner_philosopher_comp4->disconnect_left();
            dinner_philosopher_comp1->disconnect_right();
            dinner_philosopher_comp2->disconnect_right();
            dinner_philosopher_comp3->disconnect_right();
            dinner_philosopher_comp4->disconnect_right();
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

        TEST_INFO("Disconnect_register operation on Philosopher components...");
        test_ok=false;
        try
        {
            dinner_philosopher_comp1->disconnect_register();
            dinner_philosopher_comp2->disconnect_register();
            dinner_philosopher_comp3->disconnect_register();
            dinner_philosopher_comp4->disconnect_register();
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

          TEST_INFO("Unsubscribe_info operation on Philosopher components...");
        test_ok=false;
        try
        {
            dinner_philosopher_comp1->unsubscribe_info(ck1);
            dinner_philosopher_comp2->unsubscribe_info(ck2);
            dinner_philosopher_comp3->unsubscribe_info(ck3);
            dinner_philosopher_comp4->unsubscribe_info(ck4);
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
        catch (const CORBA::SystemException& e)
        {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str());
            test_ok=false;
        }
        TEST_CHECK(test_ok);

          TEST_INFO("Disconnect_config operation on Philosopher components...");
        test_ok=false;
        try
        {
               Components::EventConsumerBase_var consumer 
                    = dinner_philosopher_comp1->disconnect_config();
            test_ok=true;
        }
        catch (const Components::InvalidName& )
        {
            TEST_INFO("Unexpected exception raised: InvalidName");
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


        TEST_INFO("Remove operation on the Philosopher home...");
        test_ok=false;
        try
        {
            m_container->remove_home(dinner_philosopher_home.in());
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
        
        TEST_INFO("Remove operation on the Observer home...");
        test_ok=false;
        try
        {
            m_container->remove_home(observer_home.in());
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

          TEST_INFO("Remove operation on the Fork home...");
        test_ok=false;
        try
        {
            m_container->remove_home(fork_home.in());
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

          TEST_INFO("Remove operation on the Registrator home...");
        test_ok=false;
        try
        {
            m_container->remove_home(registrator_home.in());
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


        // remove container
          m_container->remove();

    } 
    catch (const CORBA::Exception & e) 
    {
            std::ostringstream os;
            os << "Unexpected exception raised: " << e << std::ends;
            TEST_INFO(os.str()); 
            TEST_CHECK(false);
    }

    TEST_INFO("Set to nil philosopher_home"); 
    philosopher_home = Components::CCMHome::_nil();
    TEST_INFO("Set to nil dinner_philosopher_home"); 
    dinner_philosopher_home = Dinner::PhilosopherHome::_nil();
    TEST_INFO("Set to nil ccm_philosopher_comp"); 
    ccm_philosopher_comp = Components::CCMObject::_nil();
    TEST_INFO("Set to nil dinner_philosopher_comp2"); 
    dinner_philosopher_comp1 = Dinner::Philosopher::_nil();
    dinner_philosopher_comp2 = Dinner::Philosopher::_nil();
    TEST_INFO("Set to nil dinner_philosopher_comp3"); 
    dinner_philosopher_comp3 = Dinner::Philosopher::_nil();
    dinner_philosopher_comp4 = Dinner::Philosopher::_nil();

    TEST_INFO("Set to nil observer_home"); 
    observer_home = Components::CCMHome::_nil();
    TEST_INFO("Set to nil dinner_observer_home"); 
    dinner_observer_home = Dinner::ObserverHome::_nil();
    TEST_INFO("Set to nil ccm_observer_comp"); 
    ccm_observer_comp = Components::CCMObject::_nil();
    TEST_INFO("Set to nil dinner_observer_comp"); 
    dinner_observer_comp = Dinner::Observer::_nil();

   
    TEST_INFO("Set to nil fork_facet_obj's"); 
    fork_facet_obj1 = Dinner::Fork::_nil();
    fork_facet_obj2 = Dinner::Fork::_nil();
    fork_facet_obj3 = Dinner::Fork::_nil();
    fork_facet_obj4 = Dinner::Fork::_nil();
   

}
        
}; // End namespace CIF
}; // End namespace CCM
}; // End namespace Cdmw

