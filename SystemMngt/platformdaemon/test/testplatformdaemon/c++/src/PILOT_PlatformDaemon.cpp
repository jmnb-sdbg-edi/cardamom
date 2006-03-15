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
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <fstream>

#include "testplatformdaemon/PILOT_PlatformDaemon.hpp"

#include <list>

#include "Foundation/common/Options.hpp"
#include "Foundation/testutils/CORBATestManager.hpp"

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"

#include "testplatformdaemon/SIM_OrbThread.hpp"

#include "testplatformdaemon/SIM_ProcessObserver.hpp"
#include "testplatformdaemon/SIM_ServiceBroker.hpp"


#include "idllib/CdmwPlatformMngtDaemon.stub.hpp"
#include "idllib/CdmwPlatformMngtApplicationAgent.stub.hpp"
#include "idllib/CdmwPlatformMngtProcessDelegateWrapper.stub.hpp"
#include "idllib/CdmwPlatformMngtMonitoringObserver.stub.hpp"

#include "SystemMngt/platforminterface/test/testmanagedprocess/TestManagedProcess.skel.hpp"

#ifdef CDMW_USE_FAULTTOLERANCE
#include "testplatformdaemon/SIM_FaultManager.hpp"
#include "testplatformdaemon/TypeRepositoryStub.hpp"
#include "FaultTolerance/ftreplicationmanager/InitUtils.hpp"
#include <FaultTolerance/ftcommon/FactoryInfos.hpp>
#include <FaultTolerance/idllib/CdmwFTManager.stub.hpp>
#endif

// namespace {

// #ifdef CDMW_USE_FAULTTOLERANCE

//    class GroupUtils {
//       public:
//          GroupUtils(CdmwFT::ReplicationManager_ptr replication_manager)
//             : m_replication_manager(CdmwFT::ReplicationManager::_duplicate(replication_manager))
//             {}

//          ::FT::ObjectGroup* create_group(const char* group_name,
//                                         const char* type_id)
//             throw (CORBA::SystemException)
//          {
//              unsigned int min_num_replicas = 2;

//              ::FT::Criteria crit;        
//              crit.length(2);

//              ::FT::Name OGDName;
//              OGDName.length(1);
//              OGDName[0].id=CORBA::string_dup(group_name);

//              crit[0].nam.length(1);
//              crit[0].nam[0].id="com.thalesgroup.cdmw.ft.Name";
//              crit[0].val <<= OGDName;

//              ::FT::Properties prop;

//              prop.length(2);
//              prop[0].nam.length(1);
//              prop[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
//              prop[0].val <<= (CORBA::UShort)min_num_replicas;

//              std::string location("HOST1.hostname/SMG_SUPERVISION_PROCESS.processname");

//              ::FT::FactoryInfos factoryInfos;
//              factoryInfos.length(1);
//              ::FT::Location_var loc = 
//                Cdmw::CommonSvcs::Naming::NamingInterface::to_name(location);
//              factoryInfos[0].the_location = loc.in();
//              ::FT::Criteria factoryCrit;        
//              factoryCrit.length(0);
//              factoryInfos[0].the_criteria = factoryCrit;

//              prop[1].nam.length(1);
//              prop[1].nam[0].id="org.omg.ft.Factories";
//              prop[1].val <<= factoryInfos;

//              crit[1].nam.length(1);
//              crit[1].nam[0].id="org.omg.ft.FTProperties";
//              crit[1].val <<= prop;

//              ::FT::GenericFactory::FactoryCreationId_var factory_creation_id;
//              ::FT::ObjectGroup_var obj_group;

//              // create the object group 
//              try
//              {
//                  obj_group =  
//                     m_replication_manager->create_object(type_id, 
//                                                          crit, 
//                                                          factory_creation_id.out() );
//              }
//              catch( CORBA::SystemException& e )
//              {
//                  std::cerr << e._name() << std::endl;
//                  throw;
//              }
//              catch( CORBA::Exception& e )
//              {
//                  std::cerr << e._name() << std::endl;
//                  throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
//                                               CORBA::COMPLETED_NO);
//              }
//              return obj_group._retn();
//          }

//       private:
//          GroupUtils();
//          GroupUtils(const GroupUtils&);

//          CdmwFT::ReplicationManager_var m_replication_manager;
//    };
// #endif
// } // end anonymous namspace

CPPUNIT_TEST_SUITE_REGISTRATION (Cdmw::PlatformMngt::PILOT_PlatformDaemon);

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

        const int CHKMONITORINGLOCPORTAP1 = 40001;
        const int CHKMONITORINGLOCPORTAP2 = 40002;
        const int CHKMONITORINGLOCPORTAP3 = 40003;
        const int CHKMONITORINGLOCPORTAP4 = 40004;
        const int CHKMONITORINGLOCPORTAP5 = 40005;
#ifdef TEST_JAVA_PROCESS
        const int CHKMONITORINGLOCPORTAP6 = 40006;
#endif

        PILOT_PlatformDaemon::PILOT_PlatformDaemon ()
        {

        }


        PILOT_PlatformDaemon::~PILOT_PlatformDaemon ()
        {

        }




        void PILOT_PlatformDaemon::do_tests ()
        {
            // set number of requested successfull tests
#ifdef CDMW_USE_FAULTTOLERANCE
            // // //     set_nbOfRequestedTestOK (37);
#else
            // // //     set_nbOfRequestedTestOK (36);
#endif

#ifdef TEST_JAVA_PROCESS
            // // //     add_nbOfRequestedTestOK(2);
#endif


            TEST_INFO ("Creating the ORB");
            OrbThread orbThread;

            try
            {
                orbThread.start ();
            }
            catch (...)
            {
                return;
            }


            int timescale = Cdmw::TestUtils::get_timescale ();
            OsSupport::OS::sleep (timescale * 4000);
            CORBA::ORB_var orb = orbThread.orb ();
            PortableServer::POA_var poa = orbThread.poa ();

            // uses a specific port to avoid conflicts
            // with a running daemon that used the default port
            std::
            string daemonArgs ("--CdmwXMLFile=CdmwPlatformMngtDaemon_conf.xml");

            std::string testDaemonPortStr ("21879");


            try
            {

                OsSupport::OS::sleep (timescale * 2000);

                TEST_INFO ("Starting the PlatformDaemon");
                TEST_INFO (daemonArgs);
                OsSupport::OS::create_process ("cdmw_platform_daemon.sh", daemonArgs);

                OsSupport::OS::sleep (timescale * 10000);

                TEST_INFO ("Resolving the corbaloc reference");
                std::ostringstream corbaloc_ref;
                corbaloc_ref << "corbaloc::localhost:" << testDaemonPortStr;
                corbaloc_ref << "/" << CdmwPlatformMngt::DAEMON_NAME;

                std::string corbaloc = corbaloc_ref.str ();

                CORBA::Object_ptr obj = orb->string_to_object (corbaloc.c_str ());
                CPPUNIT_ASSERT (!CORBA::is_nil (obj));

                TEST_INFO ("Tries to narrowing the reference");
                CdmwPlatformMngt::Daemon_var daemon =
                CdmwPlatformMngt::Daemon::_narrow (obj);
                CPPUNIT_ASSERT (!CORBA::is_nil (daemon.in ()));


                TEST_INFO
                ("Asking to the daemon to resolve its own reference using corbaloc")
                CORBA::Object_var corbalocObj =
                daemon->resolve_corbaloc (corbaloc.c_str ());

#if CDMW_ORB_VDR == orbacus
                CPPUNIT_ASSERT (daemon->_is_equivalent (corbalocObj));
#else
                CPPUNIT_ASSERT (daemon->_servant () == corbalocObj->_servant ());
#endif

#ifdef CDMW_USE_FAULTTOLERANCE
                using namespace Cdmw::OsSupport;
                OS::ProcessId replMngt_id
                =
                OS::create_process ("cdmw_ft_manager",
                                    " --CdmwXMLFile=CdmwFaultToleranceManager_conf.xml  --groupConf=CdmwFTSystemMngtGroupCreator_conf.xml");
                //= OS::create_process ("replMngt_process"," ");

                int timescale = Cdmw::TestUtils::get_timescale ();
                OsSupport::OS::sleep (timescale * 20000);


                //         // Get the fault Manager IOR
                //         std::cout << "Obtain the fault_manager " << std::endl;
                //         CORBA::Object_var _obj = orb->string_to_object("corbaloc::localhost:4555/fault_manager");

                //         CdmwFT::FTManager_var ftManager = CdmwFT::FTManager::_narrow(_obj.in());
                //         _obj = ftManager->get_replication_manager();

                obj =
                orb->
                string_to_object ("corbaname::localhost:21879#ReplicationManager");

                CdmwFT::ReplicationManager_var replication_manager =
                CdmwFT::ReplicationManager::_narrow (obj);
                std::cout << orb->object_to_string (replication_manager.in ()) << std::endl;
                if (CORBA::is_nil (replication_manager.in ()))
                    CPPUNIT_ASSERT(false);
                else
                    CPPUNIT_ASSERT(true);

                obj = orb->string_to_object ("corbaname::localhost:21879#XXXX/YYYY");

                if (CORBA::is_nil (obj))
                    CPPUNIT_ASSERT(true);
                else
                    CPPUNIT_ASSERT(false);

                obj = orb->string_to_object ("corbaname::localhost:21879#");

                if (CORBA::is_nil (obj))
                    CPPUNIT_ASSERT(true);
                else
                    CPPUNIT_ASSERT(false);



#endif


                TEST_INFO ("Creates a simulated ProcessObserver");
                PortableServer::ServantBase_var processObserver_servantBase =
                new SIM_ProcessObserver ("process_observer.log");
                SIM_ProcessObserver *processObserver_servant =
                dynamic_cast <
                SIM_ProcessObserver * >(processObserver_servantBase.in ());
                CdmwPlatformMngt::ProcessObserver_var processObserver =
                processObserver_servant->_this ();
                CPPUNIT_ASSERT (!CORBA::is_nil (processObserver.in ()));

                TEST_INFO ("Creates a simulated ServiceBroker");
                PortableServer::ServantBase_var serviceBroker_servantBase =
                new SIM_ServiceBroker ();
                SIM_ServiceBroker *serviceBroker_servant =
                dynamic_cast <
                SIM_ServiceBroker * >(serviceBroker_servantBase.in ());
                CdmwPlatformMngtService::ServiceBroker_var serviceBroker =
                serviceBroker_servant->_this ();
                CPPUNIT_ASSERT (!CORBA::is_nil (serviceBroker.in ()));



                CdmwPlatformMngt::ProcessInfo processInfo;
#ifdef TEST_JAVA_PROCESS
                CdmwPlatformMngt::ProcessInfo processInfoJ;
#endif
                // application agent creation tests

                OsSupport::OS::sleep (timescale * 1000);

                const std::string APPLICATION_NAME = "Foo";
                const std::string APPLICATION_NAME2 = "Foo2";

                CdmwPlatformMngt::ApplicationAgentInfo appAgentInfo;
                appAgentInfo.application_name =
                CORBA::string_dup (APPLICATION_NAME.c_str ());
                appAgentInfo.process_observer = processObserver;
                appAgentInfo.service_broker = serviceBroker;

                CdmwPlatformMngt::ApplicationAgentInfo appAgentInfo2;
                appAgentInfo2.application_name =
                CORBA::string_dup (APPLICATION_NAME2.c_str ());
                appAgentInfo2.process_observer = processObserver;
                appAgentInfo2.service_broker = serviceBroker;

#ifdef CDMW_USE_FAULTTOLERANCE
                std::string replication_manager_ior =
                orb->object_to_string (replication_manager.in ());
                appAgentInfo.replication_manager_iogr =
                CORBA::string_dup (replication_manager_ior.c_str ());
#endif

                CdmwPlatformMngt::ApplicationAgent_var agent;
                CdmwPlatformMngt::ApplicationAgent_var agent2;

                try
                {
                    TEST_INFO ("Creates the agent for the application 'Foo'");
                    agent = daemon->create_application_agent (appAgentInfo);

                    TEST_INFO ("Creates the agent2 for the application 'Foo2'");
                    agent2 = daemon->create_application_agent (appAgentInfo2);
                    CPPUNIT_ASSERT (true);
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "Unexpected CORBA exception : " << e << std::endl;
                    CPPUNIT_ASSERT (false);
                }
                catch (...)
                {
                    CPPUNIT_ASSERT (false);
                }

                // APPLICATION AGENT TEST

                CORBA::String_var result_info;

                OsSupport::OS::sleep (timescale * 1000);

                TEST_INFO
                ("Tries to create an unmanaged process with an unknown executable");
                processInfo.process_exe = CORBA::string_dup ("unknown_executable");
                processInfo.process_port = -1;	// unspecified port
                processInfo.working_directory = CORBA::string_dup ("");
                processInfo.process_args = CORBA::string_dup ("");

                try
                {
                    CdmwPlatformMngt::ProcessCommandResult result =
                    agent->create_process ("unknown_executable", processInfo,
                                           result_info.out ());

                    if (result == CdmwPlatformMngt::PROCESS_INVALID_INFO)
                    {
                        std::cout << "Process invalid info" << std::endl;
                        CPPUNIT_ASSERT (true);
                    }
                    else
                    {
                        CPPUNIT_ASSERT (false);
                    }
                }
                catch (...)
                {
                    CPPUNIT_ASSERT (false);
                }

                OsSupport::OS::sleep (timescale * 1000);

                TEST_INFO ("Creates the unmanaged process 'AU'");
                processInfo.process_exe = CORBA::string_dup ("cdmw_test_monitoring");
                //      processInfo.process_port = -1;
                processInfo.process_port = 51239;
                processInfo.working_directory = CORBA::string_dup ("");
                processInfo.process_args = CORBA::string_dup ("--no-unit-test");

                //std::cout << "!!!!! Waiting Operator to attach daemon !!!!!!!" << std::endl;

                //int nb;
                //scanf ("%d", &nb);

                //TEST_INFO("Sleeping a while to wait for the daemon creation");

                try
                {
                    CdmwPlatformMngt::ProcessCommandResult result =
                    agent->create_process ("AU", processInfo, result_info.out ());

                    if (result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED)
                    {
                        CPPUNIT_ASSERT (true);
                    }
                    else
                    {
                        std::cout << "Process creation unexpected error" << std::endl;
                        CPPUNIT_ASSERT (false);
                    }
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "Unexpected CORBA exception : " << e << std::endl;
                    CPPUNIT_ASSERT (false);
                }

                OsSupport::OS::sleep (timescale * 1000);

                processInfo.process_exe =
                CORBA::string_dup ("cdmw_test_managed_process");
                processInfo.process_port = -1;	// unspecified port
                processInfo.working_directory = CORBA::string_dup ("");
                processInfo.process_args = CORBA::string_dup ("--no-unit-test");
#ifdef TEST_JAVA_PROCESS
                processInfoJ.process_exe =
                CORBA::string_dup ("cdmw_testmanagedprocess.sh");
                processInfoJ.process_port = -1;	// unspecified port
                processInfoJ.working_directory = CORBA::string_dup ("");
                processInfoJ.process_args = CORBA::string_dup ("--no-unit-test");
#endif

                CdmwPlatformMngt::ProcessDelegateWrapper_var processP1;
                CdmwPlatformMngt::ProcessDelegateWrapper_var processP2;
                CdmwPlatformMngt::ProcessDelegateWrapper_var processP3;
                CdmwPlatformMngt::ProcessDelegateWrapper_var processP4;
                CdmwPlatformMngt::ProcessDelegateWrapper_var processP5;
#ifdef TEST_JAVA_PROCESS
                CdmwPlatformMngt::ProcessDelegateWrapper_var processP6;
#endif

                TEST_INFO ("Creates managed processes 'AP1','AP2','AP3','AP4','AP5'");
                CORBA::String_var error_info;
                CORBA::String_var error_issuer;
                try
                {
                    std::ostringstream pullArgs, pullArgs1, pullArgs2,
                    pullArgs3, pullArgs4, pullArgs5;
#ifdef TEST_JAVA_PROCESS
                    std::ostringstream pullArgs6;
#endif

                    //pullArgs << "-ORBDebugLevel 10" << " ";
                    pullArgs << "--Sleep=0 --Request=true --Exception=false";

                    pullArgs1 << pullArgs.
                    str () << " --CdmwLocalisationService=" <<
                    CHKMONITORINGLOCPORTAP1;
                    pullArgs2 << pullArgs.
                    str () << " --CdmwLocalisationService=" <<
                    CHKMONITORINGLOCPORTAP2;
                    pullArgs3 << pullArgs.
                    str () << " --CdmwLocalisationService=" <<
                    CHKMONITORINGLOCPORTAP3;
#ifdef TEST_JAVA_PROCESS
                    pullArgs6 << pullArgs.
                    str () << " --CdmwLocalisationService=" <<
                    CHKMONITORINGLOCPORTAP6;
#endif

                    pullArgs4 << "--Sleep=0 --Request=false --Exception=false";
                    pullArgs4 << " --CdmwLocalisationService=" <<
                    CHKMONITORINGLOCPORTAP4;

                    pullArgs5 << "--Sleep=0 --Request=true --Exception=true";
                    pullArgs5 << " --CdmwLocalisationService=" <<
                    CHKMONITORINGLOCPORTAP5;

                    processInfo.process_args =
                    CORBA::string_dup (pullArgs1.str ().c_str ());
                    CdmwPlatformMngt::ProcessCommandResult result1 =
                    agent->create_managed_process ("AP1", processInfo, 10000,
                                                   processP1.out (),
                                                   error_info.out (),
                                                   error_issuer.out ());

                    processInfo.process_args =
                    CORBA::string_dup (pullArgs2.str ().c_str ());
                    CdmwPlatformMngt::ProcessCommandResult result2 =
                    agent->create_managed_process ("AP2", processInfo, 10000,
                                                   processP2.out (),
                                                   error_info.out (),
                                                   error_issuer.out ());

#ifdef TEST_JAVA_PROCESS
                    // For Java test : for the moment the monitoring test need a visual check to be checked completly
                    //  this is not a problem for now because this java part is not launched in unit test (the -D is needed)
                    //  But if needed implement in testmanagedprocess/java the CheckMonitoring and create the servant
                    processInfoJ.process_args =
                    CORBA::string_dup (pullArgs6.str ().c_str ());
                    CdmwPlatformMngt::ProcessCommandResult result6 =
                    agent->create_managed_process ("AP6", processInfoJ, 10000,
                                                   processP6.out (),
                                                   error_info.out (),
                                                   error_issuer.out ());
#endif

                    processInfo.process_args =
                    CORBA::string_dup (pullArgs3.str ().c_str ());
                    CdmwPlatformMngt::ProcessCommandResult result3 =
                    agent2->create_managed_process ("AP3", processInfo, 10000,
                                                    processP3.out (),
                                                    error_info.out (),
                                                    error_issuer.out ());

                    processInfo.process_args =
                    CORBA::string_dup (pullArgs4.str ().c_str ());
                    CdmwPlatformMngt::ProcessCommandResult result4 =
                    agent->create_managed_process ("AP4", processInfo, 10000,
                                                   processP4.out (),
                                                   error_info.out (),
                                                   error_issuer.out ());

                    processInfo.process_args =
                    CORBA::string_dup (pullArgs5.str ().c_str ());
                    CdmwPlatformMngt::ProcessCommandResult result5 =
                    agent->create_managed_process ("AP5", processInfo, 10000,
                                                   processP5.out (),
                                                   error_info.out (),
                                                   error_issuer.out ());

                    if (result1 == CdmwPlatformMngt::PROCESS_CMD_SUCCEED &&
                        result2 == CdmwPlatformMngt::PROCESS_CMD_SUCCEED &&
#ifdef TEST_JAVA_PROCESS
                        result6 == CdmwPlatformMngt::PROCESS_CMD_SUCCEED &&
#endif
                        result3 == CdmwPlatformMngt::PROCESS_CMD_SUCCEED &&
                        result4 == CdmwPlatformMngt::PROCESS_CMD_SUCCEED &&
                        result5 == CdmwPlatformMngt::PROCESS_CMD_SUCCEED)
                    {
                        CPPUNIT_ASSERT (true);
                    }
                    else
                    {
                        if (result1 == CdmwPlatformMngt::PROCESS_CMD_TIMEOUT ||
                            result2 == CdmwPlatformMngt::PROCESS_CMD_TIMEOUT ||
                            result3 == CdmwPlatformMngt::PROCESS_CMD_TIMEOUT ||
                            result4 == CdmwPlatformMngt::PROCESS_CMD_TIMEOUT ||
                            result5 == CdmwPlatformMngt::PROCESS_CMD_TIMEOUT)
                        {
                            std::
                            cerr <<
                            "TIMEOUT problem : Reflects the host overload and not an actual problem."
                                 << std::endl;
                        }

                        CPPUNIT_ASSERT (false);
                    }

                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (false);
                }
                catch (...)
                {
                    CPPUNIT_ASSERT (false);
                }

                OsSupport::OS::sleep (timescale * 5000);

                try
                {
                    TEST_INFO
                    ("Call again create_managed_process on an already created process");
                    CdmwPlatformMngt::ProcessDelegateWrapper_var processP1bis;
                    CdmwPlatformMngt::ProcessCommandResult result =
                    agent->create_managed_process ("AP1", processInfo, 10000,
                                                   processP1bis.out (),
                                                   error_info.out (),
                                                   error_issuer.out ());
                    if (result == CdmwPlatformMngt::PROCESS_ALREADY_EXIST)
                    {
                        if (processP1->_is_equivalent (processP1bis.in ()))
                        {
                            CPPUNIT_ASSERT (true);
                        }
                        else
                        {
                            TEST_INFO ("The _is_equivalent test did not success:");
                            TEST_INFO
                            (" As this function is sure only when true is answered");
                            TEST_INFO
                            (" we should simply suppress this DelegateWrapper id test");
                            CPPUNIT_ASSERT (false);
                        }
                    }
                    else
                    {
                        CPPUNIT_ASSERT (false);
                    }
                    TEST_INFO
                    ("Call again create_process on an already created process");
                    result =
                    agent->create_process ("AU", processInfo, result_info.out ());
                    if (result == CdmwPlatformMngt::PROCESS_ALREADY_EXIST)
                    {
                        CPPUNIT_ASSERT (true);
                    }
                    else
                    {
                        CPPUNIT_ASSERT (false);
                    }
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (false);
                }
                catch (...)
                {
                    CPPUNIT_ASSERT (false);
                }

                TEST_INFO ("Try to get the process Id of an unexisting process");
                try
                {
                    agent->get_pid ("unexisting");
                    CPPUNIT_ASSERT (false);
                }
                catch (const CdmwPlatformMngt::ProcessNotFound &)
                {
                    CPPUNIT_ASSERT (true);
                }
                TEST_INFO ("Get the process Id of an existing process");
                CdmwPlatformMngt::ProcessPID pid = 0;
                pid = agent->get_pid ("AP1");
                if (pid == 0)
                {
                    CPPUNIT_ASSERT (false);
                }
                else
                {
                    CPPUNIT_ASSERT (true);
                }

                CdmwPlatformMngt::ProcessCommandResult result =
                CdmwPlatformMngt::PROCESS_CMD_SUCCEED;

                TEST_INFO
                ("Requests the processes 'AP1','AP2','AP3','AP4','AP5' to initialise, run and stop");
                try
                {
                    CdmwPlatformMngtBase::StartupKind startupKind;
                    startupKind.state_retrieval = CdmwPlatformMngtBase::LAST_STATE;
                    startupKind.startup_mode = 0;
                    startupKind.specified_state_id = CORBA::string_dup ("");


                    TEST_INFO ("Requests the processes 'AP1' to initialise");
                    result = processP1->initialise_process (startupKind, 10000);

                    if (result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED)
                    {
                        TEST_INFO ("Requests the processes 'AP2' to initialise");
                        result = processP2->initialise_process (startupKind, 10000);
                    }
#ifdef TEST_JAVA_PROCESS
                    if (result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED)
                    {
                        TEST_INFO ("Requests the processes 'AP6' to initialise");
                        result = processP6->initialise_process (startupKind, 10000);
                    }
#endif
                    if (result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED)
                    {
                        TEST_INFO ("Requests the processes 'AP3' to initialise");
                        result = processP3->initialise_process (startupKind, 10000);
                    }

                    if (result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED)
                    {
                        TEST_INFO ("Requests the processes 'AP4' to initialise");
                        result = processP4->initialise_process (startupKind, 10000);
                    }

                    if (result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED)
                    {
                        TEST_INFO ("Requests the processes 'AP5' to initialise");
                        result = processP5->initialise_process (startupKind, 10000);
                    }

                    if (result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED)
                    {
                        TEST_INFO ("Requests the processes 'AP1' to run");
                        result = processP1->run_process (10000);
                    }

                    if (result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED)
                    {
                        TEST_INFO ("Requests the processes 'AP2' to run");
                        result = processP2->run_process (10000);
                    }
#ifdef TEST_JAVA_PROCESS
                    if (result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED)
                    {
                        TEST_INFO ("Requests the processes 'AP6' to run");
                        result = processP6->run_process (10000);
                    }
#endif
                    if (result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED)
                    {
                        TEST_INFO ("Requests the processes 'AP3' to run");
                        result = processP3->run_process (10000);
                    }

                    if (result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED)
                    {
                        TEST_INFO ("Requests the processes 'AP4' to run");
                        result = processP4->run_process (10000);
                    }

                    if (result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED)
                    {
                        TEST_INFO ("Requests the processes 'AP5' to run");
                        result = processP5->run_process (10000);
                    }

                    if (result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED)
                    {
                        CPPUNIT_ASSERT (true);
                    }
                    else
                    {
                        CPPUNIT_ASSERT (false);
                    }
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (false);
                }

                OsSupport::OS::sleep (timescale * 5000);

                // Part concerning monitoring


                CdmwPlatformMngt::CompleteMonitoringInfo process1MonitoringInfo;
                CdmwPlatformMngt::CompleteMonitoringInfo process2MonitoringInfo;
                CdmwPlatformMngt::CompleteMonitoringInfo process3MonitoringInfo;
                CdmwPlatformMngt::CompleteMonitoringInfo process4MonitoringInfo;
                CdmwPlatformMngt::CompleteMonitoringInfo process5MonitoringInfo;

                process1MonitoringInfo.monitoring_model =
                CdmwPlatformMngt::NO_MONITORING;
                process1MonitoringInfo.pull_monitoring_interval = 2000;
                process1MonitoringInfo.pull_monitoring_timeout = 6000;
                process1MonitoringInfo.push_monitoring_interval = 0;
                process1MonitoringInfo.push_monitoring_timeout = 0;

                process2MonitoringInfo.monitoring_model =
                CdmwPlatformMngt::PULL_MONITORING_MODEL;
                process2MonitoringInfo.pull_monitoring_interval = 4000;
                process2MonitoringInfo.pull_monitoring_timeout = 1000;
                process2MonitoringInfo.push_monitoring_interval = 0;
                process2MonitoringInfo.push_monitoring_timeout = 0;

                process3MonitoringInfo.monitoring_model =
                CdmwPlatformMngt::PULL_MONITORING_MODEL;
                process3MonitoringInfo.pull_monitoring_interval = 6000;
                process3MonitoringInfo.pull_monitoring_timeout = 1000;
                process3MonitoringInfo.push_monitoring_interval = 0;
                process3MonitoringInfo.push_monitoring_timeout = 0;

                process4MonitoringInfo.monitoring_model =
                CdmwPlatformMngt::PULL_MONITORING_MODEL;
                process4MonitoringInfo.pull_monitoring_interval = 2000;
                process4MonitoringInfo.pull_monitoring_timeout = 1000;
                process4MonitoringInfo.push_monitoring_interval = 0;
                process4MonitoringInfo.push_monitoring_timeout = 0;

                process5MonitoringInfo.monitoring_model =
                CdmwPlatformMngt::PULL_MONITORING_MODEL;
                process5MonitoringInfo.pull_monitoring_interval = 2000;
                process5MonitoringInfo.pull_monitoring_timeout = 1000;
                process5MonitoringInfo.push_monitoring_interval = 0;
                process5MonitoringInfo.push_monitoring_timeout = 0;

                TEST_INFO
                ("Requests the monitoring of a process with a bad monitoring model");
                try
                {
                    agent->start_process_monitoring ("AP1", process1MonitoringInfo);
                    CPPUNIT_ASSERT (false);
                }
                catch (const CdmwPlatformMngt::UnsupportedMonitoringModel &)
                {
                    CPPUNIT_ASSERT (true);
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (false);
                }

                OsSupport::OS::sleep (timescale * 1000);

                process1MonitoringInfo.monitoring_model =
                CdmwPlatformMngt::PULL_MONITORING_MODEL;

                TEST_INFO ("Requests the monitoring of an unmanaged process");
                try
                {
                    agent->start_process_monitoring ("AU", process1MonitoringInfo);
                    CPPUNIT_ASSERT (false);
                }
                catch (const CdmwPlatformMngt::IncompatibleType &)
                {
                    CPPUNIT_ASSERT (true);
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (false);
                }

                OsSupport::OS::sleep (timescale * 1000);

                TEST_INFO
                ("Requests the monitoring of a process that does not exist");
                try
                {
                    agent->start_process_monitoring ("WW", process1MonitoringInfo);
                    CPPUNIT_ASSERT (false);
                }
                catch (const CdmwPlatformMngt::ProcessNotFound &)
                {
                    CPPUNIT_ASSERT (true);
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (false);
                }

                OsSupport::OS::sleep (timescale * 1000);

                TEST_INFO
                ("Requests the termination of a monitoring that is not started");
                try
                {
                    agent->stop_process_monitoring ("AP1");
                    CPPUNIT_ASSERT (false);
                }
                catch (const CdmwPlatformMngt::IncompatibleMonitoringCommand &)
                {
                    CPPUNIT_ASSERT (true);
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (false);
                }

                OsSupport::OS::sleep (timescale * 1000);

                TEST_INFO
                ("Requests the monitoring of a process with bad monitoring values");
                try
                {
                    agent->start_process_monitoring ("AP1", process1MonitoringInfo);
                    CPPUNIT_ASSERT (false);
                }
                catch (const CdmwPlatformMngt::IncompatibleMonitoringValue &)
                {
                    CPPUNIT_ASSERT (true);
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (false);
                }

                OsSupport::OS::sleep (timescale * 1000);

                TEST_INFO ("Retrieve CheckMonitoring object of AP1");
                int oldCounter = 0;
                int newCounter = 0;
                Cdmw::TestManagedProcess::CheckMonitoring_var checkMonitoringAP1;
                try
                {
                    std::stringstream checkMonitoringCorbaloc;
                    checkMonitoringCorbaloc << "corbaloc::localhost:"
                                            << CHKMONITORINGLOCPORTAP1 << "/CheckMonitoring";
                    std::cout << "corbaloc is " << checkMonitoringCorbaloc.str () << std::endl;
                    CORBA::Object_var objref =
                    orb->string_to_object (checkMonitoringCorbaloc.str ().c_str ());
                    // get the CheckMonitoring proxy
                    checkMonitoringAP1 =
                    Cdmw::TestManagedProcess::CheckMonitoring::_narrow (objref.in ());

                    oldCounter = checkMonitoringAP1->get_counter ();
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (false);
                }

                process1MonitoringInfo.pull_monitoring_timeout = 1000;
                TEST_INFO ("Requests the monitoring of the process AP1");
                try
                {
                    agent->start_process_monitoring ("AP1", process1MonitoringInfo);
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (false);
                }
                catch (...)
                {
                    CPPUNIT_ASSERT (false);
                }

                OsSupport::OS::sleep (process1MonitoringInfo.
                                      pull_monitoring_interval * 2);
                newCounter = checkMonitoringAP1->get_counter ();
                TEST_INFO ("Check that is_alive counter has changed on AP1");
                // Assume counter decrease when is_alive called
                if (newCounter < oldCounter)
                    CPPUNIT_ASSERT (true);
                else
                    CPPUNIT_ASSERT (false);


                TEST_INFO
                ("Requests the monitoring of a process that is already monitored");
                try
                {
                    agent->start_process_monitoring ("AP1", process1MonitoringInfo);
                    CPPUNIT_ASSERT (true);
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (false);
                }

                OsSupport::OS::sleep (timescale * 5000);

                TEST_INFO ("Retrieve CheckMonitoring object of AP2");
                Cdmw::TestManagedProcess::CheckMonitoring_var checkMonitoringAP2;
                try
                {
                    std::stringstream checkMonitoringCorbaloc;
                    checkMonitoringCorbaloc << "corbaloc::localhost:"
                                            << CHKMONITORINGLOCPORTAP2 << "/CheckMonitoring";
                    std::cout << "corbaloc is " << checkMonitoringCorbaloc.str () << std::endl;
                    CORBA::Object_var objref =
                    orb->string_to_object (checkMonitoringCorbaloc.str ().c_str ());
                    // get the CheckMonitoring proxy
                    checkMonitoringAP2 =
                    Cdmw::TestManagedProcess::CheckMonitoring::_narrow (objref.in ());

                    oldCounter = checkMonitoringAP2->get_counter ();
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (false);
                }

                TEST_INFO ("Requests the monitoring of the process AP2");
                try
                {
                    agent->start_process_monitoring ("AP2", process2MonitoringInfo);
                }
                catch (...)
                {
                    CPPUNIT_ASSERT (false);
                }

                OsSupport::OS::sleep (process2MonitoringInfo.
                                      pull_monitoring_interval * 2);

                newCounter = checkMonitoringAP2->get_counter ();
                TEST_INFO ("Check that is_alive counter has changed on AP2");
                // Assume counter decrease when is_alive called
                if (newCounter < oldCounter)
                    CPPUNIT_ASSERT (true);
                else
                    CPPUNIT_ASSERT (false);

#ifdef TEST_JAVA_PROCESS
                TEST_INFO ("Requests the monitoring of the process AP6");
                try
                {
                    agent->start_process_monitoring ("AP6", process2MonitoringInfo);
                    CPPUNIT_ASSERT (true);
                }
                catch (...)
                {
                    CPPUNIT_ASSERT (false);
                }
                OsSupport::OS::sleep (process2MonitoringInfo.
                                      pull_monitoring_interval * 2);
#endif

                TEST_INFO ("Retrieve CheckMonitoring object of AP3");
                Cdmw::TestManagedProcess::CheckMonitoring_var checkMonitoringAP3;
                try
                {
                    std::stringstream checkMonitoringCorbaloc;
                    checkMonitoringCorbaloc << "corbaloc::localhost:"
                                            << CHKMONITORINGLOCPORTAP3 << "/CheckMonitoring";
                    std::cout << "corbaloc is " << checkMonitoringCorbaloc.str () << std::endl;
                    CORBA::Object_var objref =
                    orb->string_to_object (checkMonitoringCorbaloc.str ().c_str ());
                    // get the CheckMonitoring proxy
                    checkMonitoringAP3 =
                    Cdmw::TestManagedProcess::CheckMonitoring::_narrow (objref.in ());

                    oldCounter = checkMonitoringAP3->get_counter ();
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (false);
                }

                TEST_INFO ("Requests the monitoring of the process AP3");
                try
                {
                    agent2->start_process_monitoring ("AP3", process3MonitoringInfo);
                }
                catch (...)
                {
                    CPPUNIT_ASSERT (false);
                }

                OsSupport::OS::sleep (process3MonitoringInfo.
                                      pull_monitoring_interval * 2);
                newCounter = checkMonitoringAP3->get_counter ();
                TEST_INFO ("Check that is_alive counter has changed on AP3");
                // Assume counter decrease when is_alive called
                if (newCounter < oldCounter)
                    CPPUNIT_ASSERT (true);
                else
                    CPPUNIT_ASSERT (false);

                TEST_INFO ("Stops the monitoring of the process AP1");
                try
                {
                    agent->stop_process_monitoring ("AP1");
                }
                catch (...)
                {
                    CPPUNIT_ASSERT (false);
                }

                oldCounter = checkMonitoringAP1->get_counter ();
                OsSupport::OS::sleep (process1MonitoringInfo.
                                      pull_monitoring_interval * 2);
                TEST_INFO ("Check that is_alive counter has not changed on AP1");
                newCounter = checkMonitoringAP1->get_counter ();
                if (newCounter == oldCounter)
                    CPPUNIT_ASSERT (true);
                else
                    CPPUNIT_ASSERT (false);

                TEST_INFO
                ("Requests the termination of a monitoring already stopped");
                try
                {
                    agent->stop_process_monitoring ("AP1");
                    CPPUNIT_ASSERT (false);
                }
                catch (const CdmwPlatformMngt::IncompatibleMonitoringCommand &)
                {
                    CPPUNIT_ASSERT (true);
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (false);
                }

                // sleep to see AP1 is stopped and the other still running
                OsSupport::OS::sleep (timescale * 6000);
                oldCounter = checkMonitoringAP1->get_counter ();
                TEST_INFO ("Requests again the monitoring of the process AP1");
                try
                {
                    agent->start_process_monitoring ("AP1", process1MonitoringInfo);
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (false);
                }
                catch (...)
                {
                    CPPUNIT_ASSERT (false);
                }
                OsSupport::OS::sleep (process1MonitoringInfo.
                                      pull_monitoring_interval * 2);
                newCounter = checkMonitoringAP1->get_counter ();
                TEST_INFO ("Check that is_alive counter has changed on AP1");
                // Assume counter decrease when is_alive called
                if (newCounter < oldCounter)
                    CPPUNIT_ASSERT (true);
                else
                    CPPUNIT_ASSERT (false);

                TEST_INFO ("Stops again the monitoring of the process AP1");
                try
                {
                    agent->stop_process_monitoring ("AP1");
                }
                catch (...)
                {
                    CPPUNIT_ASSERT (false);
                }
                oldCounter = checkMonitoringAP1->get_counter ();
                OsSupport::OS::sleep (process1MonitoringInfo.
                                      pull_monitoring_interval * 2);
                TEST_INFO ("Check that is_alive counter has not changed on AP1");
                newCounter = checkMonitoringAP1->get_counter ();
                if (newCounter == oldCounter)
                    CPPUNIT_ASSERT (true);
                else
                    CPPUNIT_ASSERT (false);

                TEST_INFO ("Stops the monitoring of the process AP2");
                try
                {
                    agent->stop_process_monitoring ("AP2");
                }
                catch (...)
                {
                    CPPUNIT_ASSERT (false);
                }

                oldCounter = checkMonitoringAP2->get_counter ();
                OsSupport::OS::sleep (process2MonitoringInfo.
                                      pull_monitoring_interval * 2);
                TEST_INFO ("Check that is_alive counter has not changed on AP2");
                newCounter = checkMonitoringAP2->get_counter ();
                if (newCounter == oldCounter)
                    CPPUNIT_ASSERT (true);
                else
                    CPPUNIT_ASSERT (false);

#ifdef TEST_JAVA_PROCESS
                TEST_INFO ("Stops the monitoring of the process AP6");
                try
                {
                    agent->stop_process_monitoring ("AP6");
                    CPPUNIT_ASSERT (true);
                }
                catch (...)
                {
                    CPPUNIT_ASSERT (false);
                }
#endif

                TEST_INFO ("Stops the monitoring of the process AP3");
                try
                {
                    agent2->stop_process_monitoring ("AP3");
                }
                catch (...)
                {
                    CPPUNIT_ASSERT (false);
                }
                oldCounter = checkMonitoringAP3->get_counter ();
                OsSupport::OS::sleep (process3MonitoringInfo.
                                      pull_monitoring_interval * 2);
                TEST_INFO ("Check that is_alive counter has not changed on AP3");
                newCounter = checkMonitoringAP3->get_counter ();
                if (newCounter == oldCounter)
                    CPPUNIT_ASSERT (true);
                else
                    CPPUNIT_ASSERT (false);

                TEST_INFO ("Retrieve CheckMonitoring object of AP4");
                Cdmw::TestManagedProcess::CheckMonitoring_var checkMonitoringAP4;
                try
                {
                    std::stringstream checkMonitoringCorbaloc;
                    checkMonitoringCorbaloc << "corbaloc::localhost:"
                                            << CHKMONITORINGLOCPORTAP4 << "/CheckMonitoring";
                    std::cout << "corbaloc is " << checkMonitoringCorbaloc.str () << std::endl;
                    CORBA::Object_var objref =
                    orb->string_to_object (checkMonitoringCorbaloc.str ().c_str ());
                    // get the CheckMonitoring proxy
                    checkMonitoringAP4 =
                    Cdmw::TestManagedProcess::CheckMonitoring::_narrow (objref.in ());

                    oldCounter = checkMonitoringAP4->get_counter ();
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (false);
                }

                TEST_INFO ("Receives a pullMonitorable bad processing");
                try
                {
                    agent->start_process_monitoring ("AP4", process4MonitoringInfo);
                }
                catch (...)
                {
                    CPPUNIT_ASSERT (false);
                }

                OsSupport::OS::sleep (process4MonitoringInfo.
                                      pull_monitoring_interval * 3);
                TEST_INFO ("Try to call again CheckMonitoring object of AP4");
                try
                {
                    newCounter = checkMonitoringAP4->get_counter ();
                    CPPUNIT_ASSERT(false);
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT(true);
                }

                TEST_INFO ("Retrieve CheckMonitoring object of AP5");
                Cdmw::TestManagedProcess::CheckMonitoring_var checkMonitoringAP5;
                try
                {
                    std::stringstream checkMonitoringCorbaloc;
                    checkMonitoringCorbaloc << "corbaloc::localhost:"
                                            << CHKMONITORINGLOCPORTAP5 << "/CheckMonitoring";
                    std::cout << "corbaloc is " << checkMonitoringCorbaloc.str () << std::endl;
                    CORBA::Object_var objref =
                    orb->string_to_object (checkMonitoringCorbaloc.str ().c_str ());
                    // get the CheckMonitoring proxy
                    checkMonitoringAP5 =
                    Cdmw::TestManagedProcess::CheckMonitoring::_narrow (objref.in ());

                    oldCounter = checkMonitoringAP5->get_counter ();
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (false);
                }
                TEST_INFO ("Detects a pullMonitorable failure");
                try
                {
                    agent->start_process_monitoring ("AP5", process5MonitoringInfo);
                }
                catch (...)
                {
                    CPPUNIT_ASSERT (false);
                }
                // The process AP5 will raise exception at the 2nd is_alive
                OsSupport::OS::sleep (process5MonitoringInfo.
                                      pull_monitoring_interval * 3);
                TEST_INFO ("Try to call again CheckMonitoring object of AP5");
                try
                {
                    newCounter = checkMonitoringAP5->get_counter ();
                    CPPUNIT_ASSERT (false);
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (true);
                }

                // End of part done concerning monitoring

                try
                {
                    if (result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED)
                    {
                        result = processP1->stop_process (10000);
                    }

                    if (result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED)
                    {
                        result = processP2->stop_process (10000);
                    }

#ifdef TEST_JAVA_PROCESS
                    if (result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED)
                    {
                        result = processP6->stop_process (10000);
                    }
#endif

                    if (result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED)
                    {
                        result = processP3->stop_process (10000);
                    }

                    if (result == CdmwPlatformMngt::PROCESS_CMD_SUCCEED)
                    {
                        CPPUNIT_ASSERT (true);
                    }
                    else
                    {
                        CPPUNIT_ASSERT (false);
                    }
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (false);
                }


                TEST_INFO ("Kills the unmanaged process 'AU'");
                try
                {
                    agent->kill_process ("AU");
                    CPPUNIT_ASSERT (true);
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (false);
                }


                OsSupport::OS::sleep (timescale * 2000);

                TEST_INFO ("Tries to kill the unmanaged process 'AU' once more");
                try
                {
                    agent->kill_process ("AU");
                    CPPUNIT_ASSERT (false);
                }
                catch (const CdmwPlatformMngt::ProcessNotFound & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (true);
                }


                TEST_INFO ("Wait a while");
                OsSupport::OS::sleep (timescale * 5000);

                TEST_INFO ("Kills the ApplicationAgent");
                try
                {
                    daemon->kill_application_agent (APPLICATION_NAME.c_str ());
                    CPPUNIT_ASSERT (true);
                }
                catch (const CORBA::Exception & e)
                {
                    std::cerr << "CORBA exception raised : " << e._name () << std::endl;
                    CPPUNIT_ASSERT (false);
                }


#ifdef CDMW_USE_FAULTTOLERANCE
                OS::kill_process (replMngt_id);

                TEST_INFO ("Sleeping a while to wait for the ft_manager to stop");
                OsSupport::OS::sleep (timescale * 15000);
#endif
            }
            catch (...)
            {
                std::cerr << "unexpected exception raised" << std::endl;
                CPPUNIT_ASSERT (false);
            }

            TEST_INFO ("Stopping the daemon");
            daemonArgs += " stop";
            OsSupport::OS::create_process ("cdmw_platform_daemon.sh", daemonArgs);

            TEST_INFO ("Sleeping a while to wait for the daemon to stop");
            OsSupport::OS::sleep (timescale * 15000);


            // 
            // restart daemon with unauthorised port to check ORB CORBA::Exception
            //
            TEST_INFO ("Restart the PlatformDaemon with arguments : ");
            std::string daemon_log ("daemon_err.log");
            std::string daemonPortArgs (" --CdmwLocalisationService=50 ");
            std::string daemonPortArgs2 = daemonPortArgs + daemon_log;

            TEST_INFO (daemonPortArgs2);
            OsSupport::OS::create_process ("start_daemon_with_bad_param.sh", "");

            OsSupport::OS::sleep (timescale * 10000);

            // check the log file : exception is logged
            const int MAX_LINE = 512;
            char line[MAX_LINE + 1];

            std::ifstream result (daemon_log.c_str (), std::ios::in);

            if (!result)
            {
                CPPUNIT_ASSERT(false);
                // // 	  get_resultStream () << "cannot open file : " << daemon_log << std:: endl;
            }
            else
            {
                // file is opened
                CPPUNIT_ASSERT(true);

                int found = 0;

                std::string test_line = "!!!! CORBA::Exception: ";

                while (result.getline (line, MAX_LINE))
                {
                    if (!strncmp (line, test_line.c_str (), test_line.size ()))
                    {
                        found = 1;
                    }
                }

                if (!found)
                {
                    CPPUNIT_ASSERT(false);
                    // // 	      get_resultStream () << "error in daemon log message" << std:: endl << "read     : " << line << std::endl;
                }
                else
                {
                    CPPUNIT_ASSERT(true);
                }
            }

            TEST_INFO ("Stopping the orb");
            orbThread.shutdown ();


        }



    }				// End namespace PlatformMngt
}				// End namespace Cdmw
