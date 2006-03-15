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


#include "Foundation/orbsupport/CORBA.hpp"
#include "testsupervisionobserver/SupervisionObserverTester2.hpp"

#include "idllib/CdmwPlatformMngtEvent.stub.hpp"
#include "idllib/CdmwPlatformMngtSupervisionObserver.stub.hpp"

#include "SystemMngt/platformvaluetypes/SystemModeChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/SystemStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/HostStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationAgentDeath_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationModeChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ProcessStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/EntityStatusChange_impl.hpp"

#include "platformlibrary/EventHeaderFactory.hpp"

using namespace std;

using namespace CdmwPlatformMngtBase;

using namespace CdmwPlatformMngt;

using namespace Cdmw::PlatformMngt;

CPPUNIT_TEST_SUITE_REGISTRATION( SupervisionObserverTester ) ;

/*
SupervisionObserverTester::SupervisionObserverTester(
    CORBA::ORB_ptr orb,
    const string& name,
    CdmwPlatformMngt::SupervisionObserver_ptr observer )
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_observer = SupervisionObserver::_duplicate( observer );
}
*/

/*
SupervisionObserverTester::~SupervisionObserverTester()
{}
*/

void SupervisionObserverTester::do_tests()
{
    // set number of requested successfull tests
// //     set_nbOfRequestedTestOK (13);


    try
    {
        TEST_INFO( "Notity a system mode change with the INF level" );

#if CDMW_ORB_VDR == tao

        SystemModeChangeFactory systemModeChangeFactory;
#else

        CORBA::ValueFactoryBase_var systemModeChangeFactoryBase =
            new SystemModeChangeFactory();

        SystemModeChangeFactory* systemModeChangeFactory =
            dynamic_cast<SystemModeChangeFactory*>(systemModeChangeFactoryBase.in());
#endif

        EventHeader header =
            Cdmw::PlatformMngt::EventHeaderFactory::createHeader(INF);

#if CDMW_ORB_VDR == tao

        SystemModeChange_var systemModeEvent = systemModeChangeFactory.create(
                                                   header, "System1", NORMAL_MODE, "hello");
#else

        SystemModeChange_var systemModeEvent = systemModeChangeFactory->create(
                                                   header, "System1", NORMAL_MODE, "hello");
#endif

        m_observer->notify(systemModeEvent.in());
        CPPUNIT_ASSERT(true);

        TEST_INFO( "Notity a system status change with the INF level" );

#if CDMW_ORB_VDR == tao

        SystemStatusChangeFactory systemStatusChangeFactory;
#else

        CORBA::ValueFactoryBase_var systemStatusChangeFactoryBase =
            new SystemStatusChangeFactory();

        SystemStatusChangeFactory* systemStatusChangeFactory =
            dynamic_cast<SystemStatusChangeFactory*>(systemStatusChangeFactoryBase.in());
#endif

        header = Cdmw::PlatformMngt::EventHeaderFactory::createHeader(INF);

#if CDMW_ORB_VDR == tao

        SystemStatusChange_var systemEvent = systemStatusChangeFactory.create(
                                                 header, "System1", SYSTEM_INITIAL, "hello");
#else

        SystemStatusChange_var systemEvent = systemStatusChangeFactory->create(
                                                 header, "System1", SYSTEM_INITIAL, "hello");
#endif

        m_observer->notify(systemEvent.in());
        CPPUNIT_ASSERT(true);

        TEST_INFO( "Notity a system status change with level 14" );

        header = Cdmw::PlatformMngt::EventHeaderFactory::createHeader(14);

#if CDMW_ORB_VDR == tao

        systemEvent = systemStatusChangeFactory.create(
                          header, "System1", SYSTEM_STARTING, "user level test");
#else

        systemEvent = systemStatusChangeFactory->create(
                          header, "System1", SYSTEM_STARTING, "user level test");
#endif

        m_observer->notify(systemEvent.in());
        CPPUNIT_ASSERT(true);


        TEST_INFO( "Notity a host status change with the WRN level" );

#if CDMW_ORB_VDR == tao

        HostStatusChangeFactory hostStatusChangeFactory;
#else

        CORBA::ValueFactoryBase_var hostStatusChangeFactoryBase =
            new HostStatusChangeFactory();

        HostStatusChangeFactory* hostStatusChangeFactory =
            dynamic_cast<HostStatusChangeFactory*>(hostStatusChangeFactoryBase.in());
#endif

        header = Cdmw::PlatformMngt::EventHeaderFactory::createHeader(WRN);

#if CDMW_ORB_VDR == tao

        HostStatusChange_var hostEvent = hostStatusChangeFactory.create(
                                             header, "System1", "amidala", HOST_UNREACHABLE);
#else

        HostStatusChange_var hostEvent = hostStatusChangeFactory->create(
                                             header, "System1", "amidala", HOST_UNREACHABLE);
#endif

        m_observer->notify(hostEvent.in());
        CPPUNIT_ASSERT(true);

        // Create an application agent crash supervision event
        TEST_INFO( "Notity an application agent has crashed" );

        CORBA::ValueFactoryBase_var agentDeathFactoryBase
        = new ApplicationAgentDeathFactory();

        ApplicationAgentDeathFactory* agentDeathFactory =
            dynamic_cast<ApplicationAgentDeathFactory*>(
                agentDeathFactoryBase.in() );

        header = Cdmw::PlatformMngt::EventHeaderFactory::createHeader( ERR );

        CdmwPlatformMngt::ApplicationAgentDeath_var agentDeathEvent
        = agentDeathFactory->create(
              header, "System2", "host2", "application2");

        m_observer->notify( agentDeathEvent.in() );


        TEST_INFO( "Notity an application mode change with the ERR level" );

#if CDMW_ORB_VDR == tao

        ApplicationModeChangeFactory applicationModeChangeFactory;
#else

        CORBA::ValueFactoryBase_var applicationModeChangeFactoryBase =
            new ApplicationModeChangeFactory();

        ApplicationModeChangeFactory* applicationModeChangeFactory =
            dynamic_cast<ApplicationModeChangeFactory*>(applicationModeChangeFactoryBase.in());
#endif

        header = Cdmw::PlatformMngt::EventHeaderFactory::createHeader(ERR);

        // the APPLICATION_FAILED is not used as it is detected as a test execution error
#if CDMW_ORB_VDR == tao

        ApplicationModeChange_var applicationModeEvent = applicationModeChangeFactory.create(
                    header, "System1", "App1", DEGRADED_MODE, "Ouch !!!");
#else

        ApplicationModeChange_var applicationModeEvent = applicationModeChangeFactory->create(
                    header, "System1", "App1", DEGRADED_MODE, "Ouch !!!");
#endif

        m_observer->notify(applicationModeEvent.in());
        CPPUNIT_ASSERT(true);


        TEST_INFO( "Notity an application status change with the ERR level" );

#if CDMW_ORB_VDR == tao

        ApplicationStatusChangeFactory applicationStatusChangeFactory;
#else

        CORBA::ValueFactoryBase_var applicationStatusChangeFactoryBase =
            new ApplicationStatusChangeFactory();

        ApplicationStatusChangeFactory* applicationStatusChangeFactory =
            dynamic_cast<ApplicationStatusChangeFactory*>(applicationStatusChangeFactoryBase.in());
#endif

        header = Cdmw::PlatformMngt::EventHeaderFactory::createHeader(ERR);

        // the APPLICATION_FAILED is not used as it is detected as a test execution error
#if CDMW_ORB_VDR == tao

        ApplicationStatusChange_var applicationEvent = applicationStatusChangeFactory.create(
                    header, "System1", "App1", APPLICATION_STOPPED, "Ouch !!!");
#else

        ApplicationStatusChange_var applicationEvent = applicationStatusChangeFactory->create(
                    header, "System1", "App1", APPLICATION_STOPPED, "Ouch !!!");
#endif

        m_observer->notify(applicationEvent.in());
        CPPUNIT_ASSERT(true);


        TEST_INFO( "Notity a process message" );

#if CDMW_ORB_VDR == tao

        ProcessMessageFactory processMessageFactory;
#else

        CORBA::ValueFactoryBase_var processMessageFactoryBase =
            new ProcessMessageFactory();

        ProcessMessageFactory* processMessageFactory =
            dynamic_cast<ProcessMessageFactory*>(processMessageFactoryBase.in());
#endif

        header = Cdmw::PlatformMngt::EventHeaderFactory::createHeader(INF);

        ProcessID pId;
        pId.application_name = CORBA::string_dup("App1");
        pId.process_name = CORBA::string_dup("Process1");
        pId.host_name = CORBA::string_dup("Host1");

        std::string issuer("LIFE");

#if CDMW_ORB_VDR == tao

        ProcessMessage_var messageEvent = processMessageFactory.create(
                                              header, "System1", pId, issuer.c_str(), "message");
#else

        ProcessMessage_var messageEvent = processMessageFactory->create(
                                              header, "System1", pId, issuer.c_str(), "message");
#endif

        m_observer->notify(messageEvent.in());
        CPPUNIT_ASSERT(true);


        TEST_INFO( "Notity a process status change with the FTL level" );

#if CDMW_ORB_VDR == tao

        ProcessStatusChangeFactory processStatusChangeFactory;
#else

        CORBA::ValueFactoryBase_var processStatusChangeFactoryBase =
            new ProcessStatusChangeFactory();

        ProcessStatusChangeFactory* processStatusChangeFactory =
            dynamic_cast<ProcessStatusChangeFactory*>(processStatusChangeFactoryBase.in());
#endif

        header = Cdmw::PlatformMngt::EventHeaderFactory::createHeader(FTL);

        // the PROCESS_FAILED_ERROR is not used as it is detected as a test execution error
#if CDMW_ORB_VDR == tao

        ProcessStatusChange_var processEvent = processStatusChangeFactory.create(
                                                   header, "System1", pId, issuer.c_str(), PROCESS_STOPPED,
                                                   0, "", "configuration file not found");
#else

        ProcessStatusChange_var processEvent = processStatusChangeFactory->create(
                                                   header, "System1", pId, issuer.c_str(), PROCESS_STOPPED,
                                                   0, "", "configuration file not found");
#endif

        m_observer->notify(processEvent.in());
        CPPUNIT_ASSERT(true);

        TEST_INFO( "Notity a process initialisation" );

        header = Cdmw::PlatformMngt::EventHeaderFactory::createHeader(INF);

#if CDMW_ORB_VDR == tao

        processEvent = processStatusChangeFactory.create(
                           header, "System1", pId, issuer.c_str(), PROCESS_INITIALISING,
                           1, "creation step", "");
#else

        processEvent = processStatusChangeFactory->create(
                           header, "System1", pId, issuer.c_str(), PROCESS_INITIALISING,
                           1, "creation step", "");
#endif

        m_observer->notify(processEvent.in());
        CPPUNIT_ASSERT(true);

        header = Cdmw::PlatformMngt::EventHeaderFactory::createHeader(INF);

#if CDMW_ORB_VDR == tao

        processEvent = processStatusChangeFactory.create(
                           header, "System1", pId, issuer.c_str(), PROCESS_STEP_PERFORMED,
                           1, "creation step", "");
#else

        processEvent = processStatusChangeFactory->create(
                           header, "System1", pId, issuer.c_str(), PROCESS_STEP_PERFORMED,
                           1, "creation step", "");
#endif

        m_observer->notify(processEvent.in());
        CPPUNIT_ASSERT(true);


        TEST_INFO( "Notity a system entity status change" );

#if CDMW_ORB_VDR == tao

        SystemEntityStatusChangeFactory systemEntityStatusChangeFactory;
#else

        CORBA::ValueFactoryBase_var systemEntityStatusChangeFactoryBase =
            new SystemEntityStatusChangeFactory();

        SystemEntityStatusChangeFactory* systemEntityStatusChangeFactory =
            dynamic_cast<SystemEntityStatusChangeFactory*>(systemEntityStatusChangeFactoryBase.in());
#endif

        header = Cdmw::PlatformMngt::EventHeaderFactory::createHeader( -9);

#if CDMW_ORB_VDR == tao

        SystemEntityStatusChange_var systemEntityEvent = systemEntityStatusChangeFactory.create(
                    header, "System1", "Entity1", "ENTITY_FUNCTIONING", "negative user level test");
#else

        SystemEntityStatusChange_var systemEntityEvent = systemEntityStatusChangeFactory->create(
                    header, "System1", "Entity1", "ENTITY_FUNCTIONING", "negative user level test");
#endif

        m_observer->notify(systemEntityEvent.in());
        CPPUNIT_ASSERT(true);


        TEST_INFO( "Notity an application entity status change" );

#if CDMW_ORB_VDR == tao

        ApplicationEntityStatusChangeFactory applicationEntityStatusChangeFactory;
#else

        CORBA::ValueFactoryBase_var applicationEntityStatusChangeFactoryBase =
            new ApplicationEntityStatusChangeFactory();

        ApplicationEntityStatusChangeFactory* applicationEntityStatusChangeFactory =
            dynamic_cast<ApplicationEntityStatusChangeFactory*>(applicationEntityStatusChangeFactoryBase.in());
#endif

        header = Cdmw::PlatformMngt::EventHeaderFactory::createHeader( -9999);

#if CDMW_ORB_VDR == tao

        ApplicationEntityStatusChange_var applicationEntityEvent = applicationEntityStatusChangeFactory.create(
                    header, "System1", "App1", "Entity1", "ENTITY_FUNCTIONING", "big negative user level test");
#else

        ApplicationEntityStatusChange_var applicationEntityEvent = applicationEntityStatusChangeFactory->create(
                    header, "System1", "App1", "Entity1", "ENTITY_FUNCTIONING", "big negative user level test");
#endif

        m_observer->notify(applicationEntityEvent.in());
        CPPUNIT_ASSERT(true);


        TEST_INFO( "Notity a process entity status change" );

#if CDMW_ORB_VDR == tao

        ProcessEntityStatusChangeFactory processEntityStatusChangeFactory;
#else

        CORBA::ValueFactoryBase_var processEntityStatusChangeFactoryBase =
            new ProcessEntityStatusChangeFactory();

        ProcessEntityStatusChangeFactory* processEntityStatusChangeFactory =
            dynamic_cast<ProcessEntityStatusChangeFactory*>(processEntityStatusChangeFactoryBase.in());
#endif

        header = Cdmw::PlatformMngt::EventHeaderFactory::createHeader(9999);

#if CDMW_ORB_VDR == tao

        ProcessEntityStatusChange_var processEntityEvent = processEntityStatusChangeFactory.create(
                    header, "System1", "App1", "Process1", "Host1", "Entity1", "ENTITY_FUNCTIONING", "big user level test");
#else

        ProcessEntityStatusChange_var processEntityEvent = processEntityStatusChangeFactory->create(
                    header, "System1", "App1", "Process1", "Host1", "Entity1", "ENTITY_FUNCTIONING", "big user level test");
#endif

        m_observer->notify(processEntityEvent.in());
        CPPUNIT_ASSERT(true);


    }
    catch ( CORBA::Exception& e )
    {
        cerr << e._name();
        CPPUNIT_ASSERT(false);
    }
    catch ( const Cdmw::Exception& e )
    {
        cerr << e.what() << endl;
        CPPUNIT_ASSERT(false);
    }

}

