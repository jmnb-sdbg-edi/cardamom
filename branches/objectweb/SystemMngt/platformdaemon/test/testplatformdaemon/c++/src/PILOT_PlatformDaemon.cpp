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

#include "testplatformdaemon/PILOT_PlatformDaemon.hpp"
#include <sstream>

#include <list>

#include "Foundation/common/Options.hpp"
#include "Foundation/testutils/Testable.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "testplatformdaemon/SIM_OrbThread.hpp"

#include "testplatformdaemon/SIM_PlatformObserver.hpp"
#include "testplatformdaemon/SIM_ProcessObserver.hpp"
#include "testplatformdaemon/SIM_EntityObserver.hpp"
#include "testplatformdaemon/SIM_ServiceBroker.hpp"


#include "idllib/CdmwPlatformMngtDaemon.stub.hpp"
#include "idllib/CdmwPlatformMngtApplicationAgent.stub.hpp"
#include "idllib/CdmwPlatformMngtProcess.stub.hpp"
#include "idllib/CdmwPlatformMngtMonitoringObserver.stub.hpp"

#ifdef CDMW_USE_FAULTTOLERANCE
#include "testplatformdaemon/SIM_FaultManager.hpp"
#include "testplatformdaemon/TypeRepositoryStub.hpp"
#include "FaultTolerance/ftreplicationmanager/InitUtils.hpp"
#endif
namespace Cdmw
{
namespace PlatformMngt
{

/**
*Implementation notes: none 
*<p>
*Portability issues: none 
*<p>
*/

PILOT_PlatformDaemon::PILOT_PlatformDaemon()
    throw()
    : Testable("Cdmw::PlatformDaemon")
{

}


PILOT_PlatformDaemon::~PILOT_PlatformDaemon()
    throw()
{

}


bool checkPlatformObserverLogFile(std::list<std::string>& logMessages)
{

    std::list<std::string>::iterator it;
    
    bool found = false;
    
    // checks the process startup
    for (it = logMessages.begin() ;
         it != logMessages.end() ;
         it++)
    {
        if (*it == "Process \"DP1\" started")
        {
            found = true;
            break;
        }
    }

    if (!found)
        return false;

    // checks the process ending
    for (it = logMessages.begin() ;
         it != logMessages.end() ;
         it++)
    {
        if (*it == "Process \"DP1\" ended")
        {
            found = true;
            break;
        }
    }

    if (!found)
        return false;
    
    
    
    return true;

}


bool checkProcessObserverLogFile(std::list<std::string>& logMessages)
{
    
    bool found = false;

    std::list<std::string>::iterator it;

    std::string processMessage("Process \"AP1\" reported the following message : ");
    
    
    for (it = logMessages.begin() ;
         it != logMessages.end() ;
         it++)
    {
        if (*it == processMessage + "Initialisation requested")
        {
            found = true;
            break;
        }
    }

    if (!found)
        return false;


    for (it = logMessages.begin() ;
         it != logMessages.end() ;
         it++)
    {
        if (*it == processMessage + "Run requested")
        {
            found = true;
            break;
        }
    }

    if (!found)
        return false;
    
   

    for (it = logMessages.begin() ;
         it != logMessages.end() ;
         it++)
    {
        if (*it == processMessage + "Stop requested")
        {
            found = true;
            break;
        }
    }

    if (!found)
        return false;




    for (it = logMessages.begin() ;
         it != logMessages.end() ;
         it++)
    {
        if (*it == "Process \"AP1\" has ended")
        {
            found = true;
            break;
        }
    }

    if (!found)
        return false;


    for (it = logMessages.begin() ;
         it != logMessages.end() ;
         it++)
    {
        if (*it == "Process \"AU\" has ended")
        {
            found = true;
            break;
        }
    }

    if (!found)
        return false;





    return true;

}


void
PILOT_PlatformDaemon::do_tests()                
    throw()
{
    // set number of requested successfull tests
#ifdef CDMW_USE_FAULTTOLERANCE
    set_nbOfRequestedTestOK (34);
#else
    set_nbOfRequestedTestOK (33);
#endif
    
    
    
    TEST_INFO("Creating the ORB");
    OrbThread orbThread;
    
    try
    {
        orbThread.start();
    }
    catch (...)
    {   
        return;
    }

    
	int timescale = Cdmw::TestUtils::Testable::get_timescale();
    OsSupport::OS::sleep(timescale*4000);
    CORBA::ORB_var  orb = orbThread.orb();
    PortableServer::POA_var poa = orbThread.poa();    

    // uses a specific port to avoid conflicts
    // with a running daemon that used the default port
    std::string daemonArgs("--CdmwXMLFile=CdmwPlatformMngtDaemon_conf.xml"); 

    std::string testDaemonPortStr("21879");


    try
    {
        TEST_INFO("Starting the PlatformDaemon");
        OsSupport::OS::create_process("cdmw_platform_daemon.sh", daemonArgs);

        TEST_INFO("Sleeping a while to wait for the daemon creation");
        OsSupport::OS::sleep(timescale*10000);

        TEST_INFO("Resolving the corbaloc reference");
        std::ostringstream corbaloc_ref;
        corbaloc_ref << "corbaloc::localhost:" << testDaemonPortStr;
        corbaloc_ref << "/" << CdmwPlatformMngt::DAEMON_NAME;

        std::string corbaloc = corbaloc_ref.str();
    
        CORBA::Object_ptr obj = orb->string_to_object(corbaloc.c_str());
        TEST_CHECK( ! CORBA::is_nil(obj) );

        TEST_INFO("Tries to narrowing the reference");
        CdmwPlatformMngt::Daemon_var daemon = CdmwPlatformMngt::Daemon::_narrow( obj );
        TEST_CHECK( ! CORBA::is_nil(daemon.in()) )


        TEST_INFO("Asking to the daemon to resolve its own reference using corbaloc")
        CORBA::Object_var corbalocObj = 
                    daemon->resolve_corbaloc(corbaloc.c_str());
#if CDMW_ORB_VDR == orbacus
        TEST_CHECK(daemon->_is_equivalent(corbalocObj));
#else
        TEST_CHECK(daemon->_servant() == corbalocObj->_servant());
#endif


        TEST_INFO("Tries to retrieve the PlatformNotifier interface");
        CdmwPlatformMngt::PlatformNotifier_var platformNotifier =
                        daemon->platform_notifier();
        TEST_CHECK(!CORBA::is_nil(platformNotifier.in()) );


        TEST_INFO("Creates a simulated PlatformObserver");
        PortableServer::ServantBase_var platformObserver_servantBase =
            new SIM_PlatformObserver("platform_observer.log");
        SIM_PlatformObserver *platformObserver_servant =
            dynamic_cast<SIM_PlatformObserver*>(platformObserver_servantBase.in());
        CdmwPlatformMngt::PlatformObserver_var platformObserver = platformObserver_servant->_this();
        TEST_CHECK(!CORBA::is_nil(platformObserver.in()) );

        TEST_INFO("Creates a simulated ProcessObserver");
        PortableServer::ServantBase_var processObserver_servantBase =
            new SIM_ProcessObserver("process_observer.log");
        SIM_ProcessObserver *processObserver_servant =
            dynamic_cast<SIM_ProcessObserver*>(processObserver_servantBase.in());
        CdmwPlatformMngt::ProcessObserver_var processObserver = processObserver_servant->_this();
        TEST_CHECK(!CORBA::is_nil(processObserver.in()) );

        TEST_INFO("Creates a simulated ServiceBroker");
        PortableServer::ServantBase_var serviceBroker_servantBase = new SIM_ServiceBroker();
        SIM_ServiceBroker *serviceBroker_servant =
            dynamic_cast<SIM_ServiceBroker*>(serviceBroker_servantBase.in());
        CdmwPlatformMngtService::ServiceBroker_var serviceBroker = serviceBroker_servant->_this();
        TEST_CHECK(!CORBA::is_nil(serviceBroker.in()) );

        TEST_INFO("Creates a simulated EntityObserver");
        PortableServer::ServantBase_var entityObserver_servantBase =
            new SIM_EntityObserver();
        SIM_EntityObserver *entityObserver_servant =
            dynamic_cast<SIM_EntityObserver*>(entityObserver_servantBase.in());
        CdmwPlatformMngtEntity::EntityObserver_var entityObserver = entityObserver_servant->_this();
        TEST_CHECK(!CORBA::is_nil(entityObserver.in()) );


#ifdef CDMW_USE_FAULTTOLERANCE
        TEST_INFO("Creates a simulated Fault Manager");
        PortableServer::ServantBase_var faultManager_servantBase =
            new Cdmw::FT::SIM_FaultManager();
        Cdmw::FT::SIM_FaultManager *faultManager_servant =
            dynamic_cast<Cdmw::FT::SIM_FaultManager*>(faultManager_servantBase.in());
        CdmwFT::FTManager_var faultManager = faultManager_servant->_this();
        TEST_CHECK(!CORBA::is_nil(faultManager.in()) );

        OrbSupport::OrbSupport::bind_object_to_corbaloc(orb.in(),"FaultManager" , faultManager.in() );

        // Create the TypeRepositoryStub object and register it
        CdmwFT::TypeRepository_var typeRepository =
            new Cdmw::FT::TypeRepositoryStub();
        orb->register_initial_reference("FTTypeRepository", typeRepository.in());
        
        // Create a replication manager, bind to corbaloc
        // then register its initial referance
        std::string ft_domain("ft_domain_test");
        char** argv_tmp = NULL;
        int argc_tmp = 0;

        Cdmw::OsSupport::OS::Timeval cohort_timeout;
        cohort_timeout.seconds=2;
        cohort_timeout.microseconds=0;
        Cdmw::OsSupport::OS::Timeval coordinator_timeout;
        coordinator_timeout.seconds=2;
        coordinator_timeout.microseconds=0;

        TEST_INFO("Creates a ReplicationManager");
        CdmwFT::ReplicationManager_ptr obj1 =
            Cdmw::FT::ReplicationManager::InitUtils::create_replication_manager (
                                                     argc_tmp,
                                                     argv_tmp,
                                                     orb.in(),
                                                     poa.in(),
                                                     ft_domain.c_str(),
                                                     "5555",
                                                     "ftdomain",
                                                     "225.1.1.8",
                                                     100,100,
                                                     cohort_timeout,
                                                     coordinator_timeout,
                                                     "SAFE_SECURITY_LEVEL",
                                                     "STRICT",
                                                     std::cout );
                                                     
        CdmwFT::ReplicationManager_var replication_manager = 
            CdmwFT::ReplicationManager::_duplicate(obj1);
        ::FT::FaultNotifier_var fault_notifier = 
            ::FT::FaultNotifier::_duplicate(faultManager->get_fault_notifier());
        replication_manager->register_fault_notifier(fault_notifier.in());
    
#endif

        const char* PLATFORM_OBSERVER_NAME = "simulated observer";

        TEST_INFO("Registering a PlatformObserver");
        CdmwPlatformMngt::PlatformObserver_var prevPlatformObserver;
        CdmwPlatformMngt::PlatformSnapshot_var platformSnapshot;
        
        try
        {
            prevPlatformObserver = 
                      platformNotifier->_cxx_register(PLATFORM_OBSERVER_NAME,
                                                      platformObserver.in(), platformSnapshot);
            TEST_CHECK(CORBA::is_nil(prevPlatformObserver.in()));
        }
        catch(const CORBA::Exception &e)
        {
            std::cerr << "CORBA exception raised : " << e._name() << std::endl;
            TEST_FAILED();
        }


        CdmwPlatformMngt::ProcessInfo processInfo;


        TEST_INFO("Tries to create a process with an unknown executable");
        processInfo.process_exe = CORBA::string_dup("unknown_executable");
        processInfo.process_port = -1; // unspecified port
        processInfo.working_directory = CORBA::string_dup("");
        processInfo.process_args = CORBA::string_dup("");

        try
        {
            daemon->create_process("unknown_executable", processInfo);
            TEST_FAILED();
        }
        catch (const CdmwPlatformMngt::InvalidInfo &e)
        {
            std::cout << e._name() << " : " << e.reason.in() << std::endl;
            TEST_SUCCEED();
        }
        catch (...)
        {
            TEST_FAILED();
        }


        OsSupport::OS::sleep(timescale*1000);
        
        TEST_INFO("Creates the process 'DP1'");


        const std::string PROCESS_NAME = "DP1";

        processInfo.process_exe = CORBA::string_dup("cdmw_test_monitoring");
        processInfo.process_port = -1;
//      processInfo.process_port = 51235;
        processInfo.working_directory = CORBA::string_dup("");
        processInfo.process_args = CORBA::string_dup("--no-unit-test");

        try
        {
            daemon->create_process(PROCESS_NAME.c_str(), processInfo);
            TEST_SUCCEED();
        }
        catch (const CdmwPlatformMngt::CreationTimeout &)
        {    
            std::cerr << "TIMEOUT problem : Reflects the host overload and not an actual problem." << std::endl;
            TEST_FAILED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
        }
        catch (...)
        {
            TEST_FAILED();
        }

        // application agent creation tests
        
        OsSupport::OS::sleep(timescale*1000);

        const std::string APPLICATION_NAME="Foo";

        CdmwPlatformMngt::ApplicationAgentAck appAgentAck;
        appAgentAck.application_name = CORBA::string_dup(APPLICATION_NAME.c_str());
        appAgentAck.process_observer = processObserver;
        
#ifdef CDMW_USE_FAULTTOLERANCE
        std::string replication_manager_ior = orb->object_to_string(replication_manager.in());    
        appAgentAck.replication_manager_iogr = CORBA::string_dup(replication_manager_ior.c_str());
#endif
    
        CdmwPlatformMngt::ApplicationAgentInfo appAgentInfo;

        appAgentInfo.application_agent_ack = appAgentAck;
        appAgentInfo.application_agent_exe = CORBA::string_dup("unknown_cdmw_platform_agent");
//      appAgentInfo.application_agent_port = -1; // unspecified port
        appAgentInfo.application_agent_port = 33247;
        appAgentInfo.application_agent_working_directory = CORBA::string_dup("");
//      appAgentInfo.application_agent_args = CORBA::string_dup("--CdmwLogFile=agent.out");
        appAgentInfo.application_agent_args = CORBA::string_dup("");
        appAgentInfo.application_agent_observer = ::CdmwPlatformMngt::MonitorableProcessObserver::_nil ();

        CdmwPlatformMngt::ApplicationAgent_var agent;
        
        TEST_INFO("Tries to create an application agent with an invalid executable");
        try
        {
            agent = daemon->create_application_agent(appAgentInfo, 10000);
            TEST_FAILED();
        }
        catch (const CdmwPlatformMngt::InvalidInfo &e)
        {
            std::cout << e._name() << " : " << e.reason.in() << std::endl;
            TEST_SUCCEED();
        }
        catch (...)
        {
            TEST_FAILED();
        }


        OsSupport::OS::sleep(timescale*1000);
        
        TEST_INFO("Creates the agent for the application 'Foo'");
        appAgentInfo.application_agent_exe = CORBA::string_dup("cdmw_platform_agent");
        appAgentInfo.application_agent_args = CORBA::string_dup("");
    
        try
        {        
            agent = daemon->create_application_agent(appAgentInfo, 10000);
            TEST_SUCCEED();
        }
        catch (const CdmwPlatformMngt::CreationTimeout &)
        {    
            std::cerr << "TIMEOUT problem : Reflects the host overload and not an actual problem." << std::endl;
            TEST_FAILED();
        } 
        catch (...) 
        {
            TEST_FAILED();
        }


        OsSupport::OS::sleep(timescale*1000);
        
        TEST_INFO("Tries to creates another agent with the same port");
        appAgentAck.application_name = CORBA::string_dup("agent_with_unavailable_port");
        appAgentInfo.application_agent_ack = appAgentAck;
    
        try
        {
            CdmwPlatformMngt::ApplicationAgent_var agent
                = daemon->create_application_agent(appAgentInfo, 10000);
            TEST_FAILED();
        }
        catch (const CdmwPlatformMngt::CreationError &e)
        {
            TEST_SUCCEED();
        }
        catch (const CdmwPlatformMngt::CreationTimeout &)
        {
            std::cerr << "TIMEOUT problem : Reflects the host overload and not an actual problem." << std::endl;
            TEST_FAILED();
        }
        catch (const CORBA::Exception &e)
        {
            std::cerr << "Unexpected CORBA exception : " << e << std::endl;
            TEST_FAILED();
        }
        catch (...)
        {
            TEST_FAILED();
        }


        // APPLICATION AGENT TEST

        OsSupport::OS::sleep(timescale*1000);
        
        TEST_INFO("Tries to create an unmanaged process with an unknown executable");
        processInfo.process_exe = CORBA::string_dup("unknown_executable");
        processInfo.process_port = -1; // unspecified port
        processInfo.working_directory = CORBA::string_dup("");
        processInfo.process_args = CORBA::string_dup("");

        try
        {
            agent->create_process("unknown_executable", processInfo);
            TEST_FAILED();
        }
        catch (const CdmwPlatformMngt::InvalidInfo &e)
        {
            std::cout << e._name() << " : " << e.reason.in() << std::endl;
            TEST_SUCCEED();
        }
        catch (...)
        {
            TEST_FAILED();
        }


        OsSupport::OS::sleep(timescale*1000);
        
        TEST_INFO("Creates the unmanaged process 'AU'");
        processInfo.process_exe = CORBA::string_dup("cdmw_test_monitoring");
//      processInfo.process_port = -1;
        processInfo.process_port = 51239;
        processInfo.working_directory = CORBA::string_dup("");
        processInfo.process_args = CORBA::string_dup("--no-unit-test");

        try
        {
            agent->create_process("AU", processInfo);
            TEST_SUCCEED();
        }
        catch (const CORBA::Exception &e)
        {
            std::cerr << "Unexpected CORBA exception : " << e << std::endl;
            TEST_FAILED();
        }


        OsSupport::OS::sleep(timescale*1000);
        
        TEST_INFO("Tries to create a managed process 'AP1' with a NIL entity observer");

        processInfo.process_exe = CORBA::string_dup("cdmw_test_managed_process");
        processInfo.process_port = -1; // unspecified port
        processInfo.working_directory = CORBA::string_dup("");
        processInfo.process_args = CORBA::string_dup("--no-unit-test");

        CdmwPlatformMngt::Process_var processP1;
        try 
        {
            processP1 = agent->create_managed_process(
                                 "AP1", processInfo, 10000,
                                 CdmwPlatformMngtEntity::EntityObserver::_nil(),
                                 CdmwPlatformMngtService::ServiceBroker::_nil());

            TEST_FAILED();
        }
        catch (const CORBA::BAD_PARAM &)
        {
            TEST_SUCCEED();
        }

        OsSupport::OS::sleep(timescale*1000); 
           
        TEST_INFO("Creates a managed process 'AP1'");
        try 
        {
            processP1 = agent->create_managed_process(
                                  "AP1", processInfo, 10000,
                                  entityObserver.in(), serviceBroker.in());

            TEST_SUCCEED();
        }
        catch (const CdmwPlatformMngt::CreationTimeout &)
        {   
            std::cerr << "TIMEOUT problem : Reflects the host overload and not an actual problem."
                      << std::endl;
            TEST_FAILED();

        } catch (...) {

            TEST_FAILED();

        }

        OsSupport::OS::sleep(timescale*1000);
    
        TEST_INFO("Tries to create a duplicated unmanaged process 'AP1'");

        try
        {
            agent->create_process("AP1", processInfo);
            TEST_FAILED();
        }
        catch (const CdmwPlatformMngt::ProcessAlreadyExists &)
        {
            TEST_SUCCEED();
        }

        OsSupport::OS::sleep(timescale*1000);
        
        TEST_INFO("Requests the process 'AP1' to initialise, run and stop");
        try 
        {
            CdmwPlatformMngtBase::StartupKind startupKind;
            startupKind.state_retrieval = CdmwPlatformMngtBase::LAST_STATE;
            startupKind.startup_mode = 0;
            startupKind.specified_state_id = CORBA::string_dup("");

            processP1->initialise(startupKind);
            processP1->run();

            processP1->stop();
        }
        catch (const CORBA::Exception &e)
        {
            std::cerr << "CORBA exception raised : " << e._name() << std::endl;
            TEST_FAILED();
        }


        OsSupport::OS::sleep(timescale*1000);
        
        TEST_INFO("Kills the unmanaged process 'AU'");
        try
        {
            agent->kill_process("AU");
            TEST_SUCCEED();
        }
        catch (const CORBA::Exception &e)
        {
            std::cerr << "CORBA exception raised : " << e._name() << std::endl;
            TEST_FAILED();
        }


        OsSupport::OS::sleep(timescale*1000);
        
        TEST_INFO("Tries to kill the unmanaged process 'AU' once more");
        try
        {
            agent->kill_process("AU");
            TEST_FAILED();
        }
        catch (const CdmwPlatformMngt::ProcessNotFound &e)
        {
            std::cerr << "CORBA exception raised : " << e._name() << std::endl;
            TEST_SUCCEED();
        }


        OsSupport::OS::sleep(timescale*1000);
    
        TEST_INFO("Registering again the PlatformObserver in order to get the snapshot");
        try
        {
            prevPlatformObserver = platformNotifier->_cxx_register(PLATFORM_OBSERVER_NAME,
                                      platformObserver.in(), platformSnapshot);

            TEST_INFO("Checks the snapshot");

            TEST_CHECK(platformSnapshot->length() == 1);

            TEST_CHECK(!PROCESS_NAME.compare(platformSnapshot->operator[](0).process_name.in()));
            TEST_CHECK(platformSnapshot->operator[](0).process_status
                                                         == CdmwPlatformMngt::STARTED);

        }
        catch(const CORBA::Exception &e)
        {
            std::cerr << "CORBA exception raised : " << e._name() << std::endl;
            TEST_FAILED();
        }


        TEST_INFO("Unregistering the PlatformObserver");
        try
        {
            prevPlatformObserver = platformNotifier->unregister(PLATFORM_OBSERVER_NAME);
            TEST_CHECK(platformObserver->_is_equivalent(prevPlatformObserver.in()));
        }
        catch(const CORBA::Exception &e)
        {
            std::cerr << "CORBA exception raised : " << e._name() << std::endl;

            TEST_FAILED();
        }

        TEST_INFO("Tries to unregister the PlatformObserver once more");
        try
        {
            prevPlatformObserver = platformNotifier->unregister(PLATFORM_OBSERVER_NAME);
            TEST_FAILED();
        }
        catch(const CdmwPlatformMngt::ObserverNotFound &e)
        {
            std::cerr << "CORBA exception raised : " << e._name() << std::endl;
            TEST_SUCCEED();
        }


        TEST_INFO("Registering again the PlatformObserver in order to get the snapshot");
        try
        {
            prevPlatformObserver = platformNotifier->_cxx_register(PLATFORM_OBSERVER_NAME,
                                                        platformObserver.in(), platformSnapshot);

            TEST_INFO("Checks the snapshot");

            TEST_CHECK(platformSnapshot->length() == 1);

            TEST_CHECK(!PROCESS_NAME.compare(platformSnapshot->operator[](0).process_name.in()));
            TEST_CHECK(platformSnapshot->operator[](0).process_status
                                            == CdmwPlatformMngt::STARTED);
        }
        catch (const CORBA::Exception &e)
        {
            std::cerr << "CORBA exception raised : " << e._name() << std::endl;
            TEST_FAILED();
        }


        TEST_INFO("Wait a while");
        OsSupport::OS::sleep(timescale*5000);

        TEST_INFO("Kills the ApplicationAgent");
        try
        {
            daemon->kill_application_agent(APPLICATION_NAME.c_str());
            TEST_SUCCEED();
        }
        catch(const CORBA::Exception &e)
        {
            std::cerr << "CORBA exception raised : " << e._name() << std::endl;
            TEST_FAILED();
        }


        TEST_INFO("Kills the process");
        try
        {
            daemon->kill_process(PROCESS_NAME.c_str());
            TEST_SUCCEED();
        }
        catch(const CORBA::Exception &e)
        {
            std::cerr << "CORBA exception raised : " << e._name() << std::endl;
            TEST_FAILED();
        }


        TEST_INFO("Sleeping a while to wait for the platform notification");
        OsSupport::OS::sleep(timescale*10000);

        platformObserver_servant->stop();
        processObserver_servant->stop();

    
        TEST_INFO("Checks the log file of the PlatformObserver");
        std::ifstream platformObserverlogFile("platform_observer.log");

        std::string msg;
        std::list<std::string> logMessages;

        for (int i=0; i<4; i++)
        {
            std::getline(platformObserverlogFile, msg, '\n');
            logMessages.push_back(msg);
        }

        if (checkPlatformObserverLogFile(logMessages))
            TEST_SUCCEED();
        else
            TEST_FAILED();

        TEST_INFO("Checks the log file of the ProcessObserver");
        std::ifstream processObserverlogFile("process_observer.log");

        logMessages.clear();

        for (int i=0; i<5; i++)
        {
            std::getline(processObserverlogFile, msg, '\n');
            logMessages.push_back(msg);
        }

        if (checkProcessObserverLogFile(logMessages))
            TEST_SUCCEED();
        else
            TEST_FAILED();

    } 
    catch (...) 
    {
        std::cerr << "unexpected exception raised" << std::endl;
        TEST_FAILED();
    }
    
    TEST_INFO("Stopping the daemon");
    daemonArgs += " stop";
    OsSupport::OS::create_process("cdmw_platform_daemon.sh", daemonArgs);

    TEST_INFO("Sleeping a while to wait for the daemon to stop");
    OsSupport::OS::sleep(timescale*15000);

    TEST_INFO("Stopping the orb");
    orbThread.shutdown();
}    



} // End namespace PlatformMngt
} // End namespace Cdmw


