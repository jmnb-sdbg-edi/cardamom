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
#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"

#include "SystemMngt/platformvaluetypes/SystemStatusChange_impl.hpp"

#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/System.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/testutils/CORBATestManager.hpp"

#include "testplatformlibrary/TestSupervisionEventNotifier.hpp"
#include "testplatformlibrary/TestLogMngr.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <memory>

CPPUNIT_TEST_SUITE_REGISTRATION( TestSupervisionEventNotifier );

/*
TestSupervisionEventNotifier::TestSupervisionEventNotifier(
    const std::string& name, CORBA::ORB_ptr orb,
    const std::string& observerPortNumber)
    :  m_observerPortNumber(observerPortNumber)
{
    m_orb = CORBA::ORB::_duplicate(orb);
}


TestSupervisionEventNotifier::~TestSupervisionEventNotifier()
{
}
*/


void TestSupervisionEventNotifier::do_tests()
{
    // set number of requested successfull tests
// //     set_nbOfRequestedTestOK (6);
    
    m_observerPortNumber = Cdmw::TestUtils::CORBATestManager::instance()->get_aux_info();
    m_orb =  CORBA::ORB::_duplicate( Cdmw::TestUtils::CORBATestManager::instance()->get_ORB());

    unsigned long delay = 5000;

    TEST_INFO("Starts a supervision observer");
    // Starts a supervision observer
    std::string arguments =
        std::string(Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION)
        + "=" + m_observerPortNumber + " --observer-name=observer";

    std::cout << "\"cdmw_platform_supervision_observer " << arguments << "\"" << std::endl;

    Cdmw::OsSupport::OS::ProcessId idObserver =
        Cdmw::OsSupport::OS::create_process("cdmw_platform_supervision_observer", arguments);

	int timescale = Cdmw::TestUtils::get_timescale();
    Cdmw::OsSupport::OS::sleep(timescale*2*delay);

    // Constructs the supervision observer's corbaloc
    std::string observer_corbaloc =
            "corbaloc::localhost:" + m_observerPortNumber + "/observer";

    // Gets the supervision observer's reference
    CORBA::Object_var observer_obj =
        m_orb->string_to_object( observer_corbaloc.c_str() );
    if (CORBA::is_nil(observer_obj.in()))
    {
        std::cout << "Cannot access to observer via corbaloc" << std::endl;
        CPPUNIT_ASSERT(false);
        Cdmw::OsSupport::OS::kill_process(idObserver);
        return;
    }

    CdmwPlatformMngt::SupervisionObserver_var observer =
        CdmwPlatformMngt::SupervisionObserver::_narrow( observer_obj.in() );
    if (CORBA::is_nil(observer.in()))
    {
        std::cout << "Invalid observer reference" << std::endl;
        CPPUNIT_ASSERT(false);
        Cdmw::OsSupport::OS::kill_process(idObserver);
        return;
    }


    Cdmw::PlatformMngt::BoundSyncCallFactory::initialise(m_orb.in());

    TEST_INFO("Initialises the logging manager");
    // deletes the contents of the log file
    std::ofstream temp("eventNotif.log", std::ios::trunc);
    temp.close();

    Cdmw::PlatformMngt::LogMngr::open ("eventNotif.log");

    TEST_INFO("Creates the supervision event notifier and starts it");
    unsigned long timeout = 5000;   // 5 seconds
    Cdmw::PlatformMngt::SupervisionEventNotifier eventNotifier(timeout);
    eventNotifier.start();


    TEST_INFO("Tries to register a NIL observer");
    try
    {
        CdmwPlatformMngt::SupervisionObserver_var prev_observer =
            eventNotifier.registerObserver("theObserver",
            CdmwPlatformMngt::SupervisionObserver::_nil());
        CPPUNIT_ASSERT(false);
    }
    catch(const Cdmw::BadParameterException& e)
    {
        std::cout << e.what() << std::endl;
        CPPUNIT_ASSERT(true);
    }
    catch(const Cdmw::Exception& e)
    {
        std::cout << e.what() << std::endl;
        CPPUNIT_ASSERT(false);
    }


    TEST_INFO("Tries to add a NULL event");
    try
    {
        eventNotifier.addEvent(NULL);
        CPPUNIT_ASSERT(false);
    }
    catch(const Cdmw::BadParameterException& e)
    {
        std::cout << e.what() << std::endl;
        CPPUNIT_ASSERT(true);
    }
    catch(const Cdmw::Exception& e)
    {
        std::cout << e.what() << std::endl;
        CPPUNIT_ASSERT(false);
    }


    TEST_INFO("Registers the observer");
    try
    {
        CdmwPlatformMngt::SupervisionObserver_var prev_observer =
            eventNotifier.registerObserver("theObserver", observer.in());

        if (!CORBA::is_nil(prev_observer.in()))
        {
            std::cout << "NIL was expected for the previous observer" << std::endl;
            CPPUNIT_ASSERT(false);
        }
        else
        {
            CPPUNIT_ASSERT(true); 
        }

    }
    catch( Cdmw::Exception& e )
    {
        std::cout << e.what() << std::endl;
        CPPUNIT_ASSERT(false);
    }


    TEST_INFO("Unregisters the observer and registers it again");
    try
    {
        CdmwPlatformMngt::SupervisionObserver_var prev_observer =
            eventNotifier.unregisterObserver("theObserver");

        // prev_observer corresponds to observer
        // registers it again

        prev_observer =
            eventNotifier.registerObserver("theObserver", prev_observer.in());

        if (!CORBA::is_nil(prev_observer.in()))
        {
            std::cout << "NIL was expected for the observer returned by the "
                "registerObserver method" << std::endl;
            CPPUNIT_ASSERT(false);
        }
        else
        {
            CPPUNIT_ASSERT(true);
        }

    }
    catch( Cdmw::Exception& e )
    {
        std::cout << e.what() << std::endl;
        CPPUNIT_ASSERT(false);
    }


    TEST_INFO( "Adds system status changes as events" );
    try
    {
        #if CDMW_ORB_VDR == tao
        Cdmw::PlatformMngt::SystemStatusChangeFactory systemStatusChangeFactory;
        #else
        CORBA::ValueFactoryBase_var systemStatusChangeFactory =
            new Cdmw::PlatformMngt::SystemStatusChangeFactory();
        #endif

        CdmwPlatformMngtBase::EventHeader header =
            Cdmw::PlatformMngt::EventHeaderFactory::createHeader(CdmwPlatformMngtBase::INF);

        #if CDMW_ORB_VDR == tao
        CdmwPlatformMngt::SystemStatusChange_var systemEvent1
            = systemStatusChangeFactory.create(header, "System1",
                CdmwPlatformMngt::SYSTEM_INITIAL, "");
        #else
        CdmwPlatformMngt::SystemStatusChange_var systemEvent1 =
            dynamic_cast<Cdmw::PlatformMngt::SystemStatusChangeFactory*>
            (systemStatusChangeFactory.in())->create(header, "System1",
            CdmwPlatformMngt::SYSTEM_INITIAL, "");
        #endif

        eventNotifier.addEvent(systemEvent1.in());
        systemEvent1._retn();

        header = Cdmw::PlatformMngt::EventHeaderFactory::createHeader(CdmwPlatformMngtBase::INF);

        #if CDMW_ORB_VDR == tao
        CdmwPlatformMngt::SystemStatusChange_var systemEvent2
            = systemStatusChangeFactory.create(header, "System1",
                CdmwPlatformMngt::SYSTEM_STARTING, "");
        #else
        CdmwPlatformMngt::SystemStatusChange_var systemEvent2 =
            dynamic_cast<Cdmw::PlatformMngt::SystemStatusChangeFactory*>
            (systemStatusChangeFactory.in())->create(header, "System1",
            CdmwPlatformMngt::SYSTEM_STARTING, "");
        #endif

        eventNotifier.addEvent(systemEvent2.in());
        systemEvent2._retn();

        CPPUNIT_ASSERT(true);
    }
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name();
        CPPUNIT_ASSERT(false);
    }


    TEST_INFO( "Waiting a while to let the event notifier do its work" );
    Cdmw::OsSupport::OS::sleep(timescale*2*delay);

    TEST_INFO( "Stops the event notifier" );
    eventNotifier.stop();

    TEST_INFO("Close the logging manager");
    Cdmw::PlatformMngt::LogMngr::close();

    TEST_INFO("Checks the log file");
    std::ifstream logFile("eventNotif.log");

    std::string msg1;
    std::string msg2;
    std::string msg3;

    std::getline(logFile, msg1, '\n');
    /*
    std::getline(logFile, msg2, '\n');
    std::getline(logFile, msg3, '\n');
    */

    std::cout << msg1 << std::endl;
    if (msg1.find("Event notifier is running") != std::string::npos)
        CPPUNIT_ASSERT(true);
    else
        CPPUNIT_ASSERT(false);

    /*
    std::cout << msg2 << std::endl;
    if (msg2.find("Successful notification") != std::string::npos)
        CPPUNIT_ASSERT(true);
    else
        CPPUNIT_ASSERT(false);

    std::cout << msg3 << std::endl;
    if (msg3.find("Successful notification") != std::string::npos)
        CPPUNIT_ASSERT(true);
    else
        CPPUNIT_ASSERT(false);
    */

    Cdmw::PlatformMngt::BoundSyncCallFactory::destroy();

    Cdmw::OsSupport::OS::kill_process(idObserver);

}



