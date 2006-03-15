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


#include "testorbsupport/TestOrbSupportDriver.hpp"


#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

#include "Foundation/common/System.hpp"
#include "Foundation/testutils/Testable.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "testorbsupport/TestOrbSupportServer.stub.hpp"
#include "testorbsupport/TestOrbSupportClientThread.hpp"
#include "testorbsupport/TestOrbSupportServerProcess.hpp"

#if CDMW_ORB_VDR == orbacus
#     include "testorbsupport/TestJTCAdoption.hpp"
#endif


namespace Cdmw
{

    namespace OrbSupport
    {

        /**
        *Implementation notes: [if no pertinent write none ]
        *<p>
        *Portability issues: [if no pertinent write none ]
        *<p>
        */


        TestOrbSupportDriver::TestOrbSupportDriver(const std::string& process_name)
                : Testable("Cdmw::OrbSupport"),
                  m_process_name(process_name),
                  m_valid_only(false)
        {
            // Nothing to do
        }

        TestOrbSupportDriver::TestOrbSupportDriver(const std::string& process_name,
                                                   bool valid_only)
                : Testable("Cdmw::OrbSupport"),
                  m_process_name(process_name),
                  m_valid_only(valid_only)
        {
            // Nothing to do
        }


        TestOrbSupportDriver::~TestOrbSupportDriver()
        throw()
        {
        }


        void
        TestOrbSupportDriver::do_port_number_test(CORBA::ORB_ptr orb)
        {
            add_nbOfRequestedTestOK (7);

            try
            {
                std::stringstream serverMTCorbaloc;
                serverMTCorbaloc << "corbaloc::localhost:"
                << TestOrbSupportServerProcess::SERVER_MT_PORT
                << "/"
                << TestOrbSupportServerProcess::SERVER_MT_CORBALOC_NAME;
                TEST_INFO( "Try to contact multi-threaded server via corbaloc: " )
                TEST_INFO( serverMTCorbaloc.str() );
                CORBA::Object_var objMT = orb->string_to_object( serverMTCorbaloc.str().c_str() );
                CdmwTest::TestOrbSupportServer_var serverMT =
                    CdmwTest::TestOrbSupportServer::_narrow( objMT.in() );
                TEST_CHECK( ! CORBA::is_nil(serverMT.in()) );

                serverMT->compute_square(5);
                TEST_SUCCEED();
            }
            catch (CORBA::Exception& e)
            {
                TEST_FAILED();
                TEST_INFO("Exception: ");
                TEST_INFO(OrbSupport::exception_to_string(e));
            }

            std::stringstream serverSTCorbaloc;
            try
            {
                serverSTCorbaloc << "corbaloc::localhost:"
                << TestOrbSupportServerProcess::SERVER_ST_PORT
                << "/"
                << TestOrbSupportServerProcess::SERVER_ST_CORBALOC_NAME;
                TEST_INFO( "Try to contact single-threaded server via corbaloc: " )
                TEST_INFO( serverSTCorbaloc.str() );
                CORBA::Object_var objST = orb->string_to_object( serverSTCorbaloc.str().c_str() );
                CdmwTest::TestOrbSupportServer_var serverST =
                    CdmwTest::TestOrbSupportServer::_narrow( objST.in() );
                TEST_CHECK( ! CORBA::is_nil(serverST.in()) );

                serverST->compute_square(5);
                TEST_SUCCEED();

                TEST_INFO("Remove the corbaloc binding (on server process)");
                serverST->remove_binding();
                TEST_SUCCEED();
                TEST_INFO("Check contact of single-threaded server via object is still ok");
                serverST->compute_square(5);
                TEST_SUCCEED();

            }
            catch (CORBA::Exception& e)
            {
                TEST_FAILED();
                TEST_INFO("Exception: ");
                TEST_INFO(OrbSupport::exception_to_string(e));
            }

            try
            {
                TEST_INFO("Check that corbaloc can't be used after remove");
                CORBA::Object_var objST = orb->string_to_object( serverSTCorbaloc.str().c_str() );
                CdmwTest::TestOrbSupportServer_var serverST =
                    CdmwTest::TestOrbSupportServer::_narrow( objST.in() );
                TEST_FAILED();
            }
            catch (CORBA::Exception& e)
            {
                TEST_SUCCEED();
            }

        }




        void
        TestOrbSupportDriver::do_poa_creation_tests(
            CdmwTest::TestOrbSupportServer_var server)
        {
            // add number of requested successfull tests
            add_nbOfRequestedTestOK (2);

            int timescale = Cdmw::TestUtils::Testable::get_timescale();

            TEST_INFO( "Starting a thread with requesting a slow service" );
            TestOrbSupportClientThread thread_1(server, "slow");
            thread_1.start();

            // wait thread 1 starting
            OsSupport::OS::sleep(timescale*2000);

            TEST_INFO( "Starting a thread with requesting a fast service" );
            TestOrbSupportClientThread thread_2(server, "");
            thread_2.start();

            TEST_INFO( "Sleeping a while to wait the second thread" );
            OsSupport::OS::sleep(timescale*2000);

            TEST_INFO("Checking if the second thread is blocked");
            TEST_INFO("  because the single thread server processes");
            TEST_INFO("  the slow request of the first one");
            TEST_CHECK( thread_2.get_result() == 0 );

            TEST_INFO("Waiting for the completion of the first thread");
            thread_1.join();

            TEST_INFO( "Sleeping a while to wait the second thread" );
            OsSupport::OS::sleep(timescale*1000);
            TEST_INFO( "Checking the second thread can have its request processed" );
            TEST_CHECK( thread_2.get_result() == 25 );
            thread_2.join();


        }

        void
        TestOrbSupportDriver::do_orb_thread_pool_limit_tests(
            CdmwTest::TestOrbSupportServer_var server)
        {
            // add number of requested successfull tests
            add_nbOfRequestedTestOK (2);

            const int NB_THREADS = 5;

            TEST_INFO( "Creating five threads with a reference to the server" );
            std::vector< TestOrbSupportClientThread* > vecThreads;

            for (int iThread = 0; iThread < NB_THREADS; ++iThread )
            {
                vecThreads.push_back( new TestOrbSupportClientThread(server, "slow") );
            }

            TEST_INFO( "Starting the threads" );

            for (int iThread = 0; iThread < NB_THREADS; ++iThread )
            {
                vecThreads[iThread]->start();
            }


            TEST_INFO( "Sleeping a while to wait the threads" );
            int timescale = Cdmw::TestUtils::Testable::get_timescale();
            OsSupport::OS::sleep(timescale*2000);

            TEST_INFO( "Starting a new thread" );
            TestOrbSupportClientThread testOrbSupportClientThread( server, "" );
            testOrbSupportClientThread.start();

            TEST_INFO( "Sleeping a while to wait the last thread" );
            OsSupport::OS::sleep(timescale*1000);
            TEST_INFO( "Checking the last thread is blocked by the" );
            TEST_INFO( "  limit of the server thread pool" );
            TEST_CHECK( testOrbSupportClientThread.get_result() == 0 );

            TEST_INFO( "Waiting the threads completion" );

            for (int iThread = 0; iThread < NB_THREADS; ++iThread )
            {
                vecThreads[iThread]->join();
                delete vecThreads[iThread];
            }

            TEST_INFO( "Sleeping a while to wait the last thread" );
            OsSupport::OS::sleep(timescale*1000);

            TEST_INFO( "Checking the last thread is unblocked" );
            TEST_CHECK( testOrbSupportClientThread.get_result() == 25 );
            testOrbSupportClientThread.join();


        }

        void
        TestOrbSupportDriver::do_orb_multithread_tests(
            CdmwTest::TestOrbSupportServer_var server)
        {
            // add number of requested successfull tests
            add_nbOfRequestedTestOK (1);

            const int NB_THREADS = 4;

            TEST_INFO( "Creating four threads with a reference to the server" );
            std::vector< TestOrbSupportClientThread* > vecThreads;

            for (int iThread = 0; iThread < NB_THREADS; ++iThread )
            {
                vecThreads.push_back( new TestOrbSupportClientThread(server, "slow") );
            }

            TEST_INFO( "Starting the threads" );

            for (int iThread = 0; iThread < NB_THREADS; ++iThread )
            {
                vecThreads[iThread]->start();
            }


            TEST_INFO( "Sleeping a while to wait the threads" );
            int timescale = Cdmw::TestUtils::Testable::get_timescale();
            OsSupport::OS::sleep(timescale*3000);

            TEST_INFO( "Querying the server how many server are in service ?" );
            CORBA::Long nbThreads = server->get_number_of_threads();

            TEST_CHECK( nbThreads == NB_THREADS );

            TEST_INFO( "Waiting the threads completion" );

            for (int iThread = 0; iThread < NB_THREADS; ++iThread )
            {
                vecThreads[iThread]->join();
                delete vecThreads[iThread];
            }

        }

        void
        TestOrbSupportDriver::do_orb_tests()
        throw()
        {
            // add number of requested successfull tests
            if (m_valid_only) add_nbOfRequestedTestOK (3);
            else add_nbOfRequestedTestOK (6);


            const std::string IOR_MT_FILE = "SupportServerMT.ior";
            const std::string IOR_ST_FILE = "SupportServerST.ior";

            //    TEST_INFO( "Creating an OrbSupport object" );
            //    OrbSupport orbSupport;
            //    TEST_SUCCEED();

            StrategyList strategies;
            strategies.add_OrbThreaded();
            strategies.add_PoaThreadPerRequest();

            int argc_1 = 0;
            CORBA::ORB_var orb = OrbSupport::ORB_init(argc_1, NULL, strategies);
            TEST_SUCCEED();

            //     TEST_INFO( "Trrying to retrieve the ORB_var from the OrbSupport object" );
            //     CORBA::ORB_var  orb_2 = orbSupport.get_ORB();
            //     TEST_CHECK( orb_2->_is_equivalent( orb ) );


            TEST_INFO( "Starting a CORBA server in another process" );

            // Insure on Win32 platform required program to be started with a
            //  parasoft tool
#   if defined( __INSURE__ ) && defined( _WIN32 )

            OsSupport::OS::ProcessId pid =
                OsSupport::OS::create_process( "inject" , m_process_name + " --server");

#   else

            OsSupport::OS::ProcessId pid =
                OsSupport::OS::create_process( m_process_name, "--server");
#   endif


            TEST_INFO( "Sleeping a while to wait the process creation" );

            // Starting a process when instrumented with insure++, is quite
            //    slow, so we allow more time
            int timescale = Cdmw::TestUtils::Testable::get_timescale();
#   ifdef __INSURE__

            OsSupport::OS::sleep(timescale*15000);
#   else

            OsSupport::OS::sleep(timescale*3000);
#   endif

            TEST_INFO( "Getting back the IOR from the file for the multi-thread POA" );
            std::ifstream iorFile( IOR_MT_FILE.c_str() );

            std::string iorStrMT;
            iorFile >> iorStrMT;
            iorFile.close();

            TEST_INFO( "Getting back the IOR from the file for the single thread POA" );
            iorFile.open( IOR_ST_FILE.c_str() );
            std::string iorStrST;
            iorFile >> iorStrST;
            iorFile.close();


            TEST_INFO( "Creating the server stub multi-thread POA" );
            CORBA::Object_var objMT = orb->string_to_object( iorStrMT.c_str() );
            CdmwTest::TestOrbSupportServer_var serverMT =
                CdmwTest::TestOrbSupportServer::_narrow( objMT.in() );
            TEST_CHECK( ! CORBA::is_nil(serverMT.in()) );


            TEST_INFO( "Creating the server stub single-thread POA" );
            CORBA::Object_var objST = orb->string_to_object( iorStrST.c_str() );
            CdmwTest::TestOrbSupportServer_var serverST =
                CdmwTest::TestOrbSupportServer::_narrow( objST.in() );
            TEST_CHECK( ! CORBA::is_nil(serverST.in()) );

            if (!m_valid_only)
            {
                TEST_INFO("Extract the repository_id from the multi-thread server ref");
                std::string repository_id = OrbSupport::get_repository_id_from_ior(orb.in(), serverMT.in());
                TEST_INFO("repository_id "<<repository_id);
                TEST_CHECK(repository_id == "IDL:CdmwTest/TestOrbSupportServer:1.0");

                TEST_INFO( "Invoking a method on the server Multi-thread" );
                CORBA::Long resultMT = serverMT->compute_square(5);
                TEST_CHECK( resultMT == 25 );

                TEST_INFO( "Invoking a method on the server Single-thread" );
                CORBA::Long resultST = serverST->compute_square(5);
                TEST_CHECK( resultST == 25 );
            }

            // Check if the server accept concurrent requests
            do_orb_multithread_tests(serverMT);

            // Check if the server use a thread pool limiting
            //    the number of request to be handled
            // No need to do this test to validate policies management :
            // the test multi/single thread prove that policies are managed
            if (!m_valid_only) 
            {
              do_orb_thread_pool_limit_tests(serverMT);
            }

            // Check if the server is truely single-threaded
            do_poa_creation_tests(serverST);

            // Check if servers are listening to the appropriate ports
            do_port_number_test(orb.in());

            TEST_INFO( "Killing the server" );
            OsSupport::OS::kill_process( pid );

            TEST_INFO( "Removing IOR files" );
            OsSupport::OS::unlink(IOR_ST_FILE);
            OsSupport::OS::unlink(IOR_MT_FILE);

            orb->shutdown(false);
            OrbSupport::OrbSupport::ORB_cleanup(orb.in());
            TEST_INFO( "Destroy ORB" );
            orb->destroy();

        }

        void
        TestOrbSupportDriver::do_tests()
        throw()
        {
            int nbOfRequestedTestOK = 0;

#if CDMW_ORB_VDR == orbacus

            {
                nbOfRequestedTestOK += 2;

                TEST_INFO("Testing JTC Thread Adoption");

                TestJTCAdoption thread;

                thread.start();
                thread.join();
                TEST_INFO("Testing Thread Registration");
                TEST_CHECK(thread.registration_ok());
                TEST_INFO("Testing Thread Unregistration");
                TEST_CHECK(thread.unregistration_ok());

            }
#endif

            //nbOfRequestedTestOK += 22;
            nbOfRequestedTestOK += 10;

            TEST_INFO("Creating a strategy list");
            StrategyList strategyList;
            StrategyList strategyList_2;
            StrategyList strategyList_3;
            TEST_SUCCEED();

            size_t nbThread(0);
            //    size_t poaPort(0);
            TEST_INFO( "Checking the strategy list is empty" );
            TEST_CHECK( strategyList.is_OrbThreaded() == false );
            TEST_CHECK( strategyList.is_PoaThreadPool( nbThread ) == false );
            TEST_CHECK( strategyList.is_PoaThreadPerRequest() == false );
            TEST_CHECK( strategyList.is_PoaThreadPerConnection() == false );
            //    TEST_CHECK( strategyList.is_PoaPortAssigned(poaPort)   == false );

            TEST_INFO( "Setting the Orb ThreadPerConnection strategy in the list" );
            strategyList.add_OrbThreaded();
            TEST_INFO( "Checking the Orb ThreadPerConnection strategy in the list" );
            TEST_CHECK( strategyList.is_OrbThreaded() == true );

            TEST_INFO( "Setting the Orb ThreadPerRequest strategy in the list" );
            strategyList.add_OrbThreaded();
            TEST_INFO( "Checking the Orb ThreadPerRequest strategy in the list" );
            TEST_CHECK( strategyList.is_OrbThreaded() == true );


            TEST_INFO( "Setting the Poa Thread pool strategy in the list" );
            strategyList.add_PoaThreadPool( 3 );
            TEST_INFO( "Checking the Poa Thread pool strategy in the list" );
            TEST_CHECK( strategyList.is_PoaThreadPool( nbThread ) == true );
            TEST_CHECK( nbThread == 3 );

            //     TEST_INFO( "Setting the Poa Port assignment strategy in the list" );
            //     strategyList.add_PoaPortAssigned(5577);
            //     TEST_INFO( "Checking the Poa Port assignment strategy in the list" );
            //     TEST_CHECK( strategyList.is_PoaPortAssigned( poaPort ) == true );
            //     TEST_CHECK( poaPort == 5577 );



            TEST_INFO( "Setting the POA ThreadPerRequest strategy incompatible with the thread pool" );

            try
            {
                strategyList.add_PoaThreadPerRequest();
                TEST_FAILED();
            }
            catch ( ... )
            {
                TEST_SUCCEED();
            }

            // No need in validation context to do other test on strategies (for policies)
            // and to perform ORB id test : an ORB_init is done in do_orb_tests() 
            if (!m_valid_only)
            {
                nbOfRequestedTestOK += 12;

                TEST_INFO( "Setting the POA Threaded strategy in the list (Thread_per_request)" );
                strategyList_2.add_PoaThreadPerRequest();
                TEST_INFO( "Checking the POA Threaded strategy in the list" );
                TEST_CHECK( strategyList_2.is_PoaThreadPerRequest() == true );

                TEST_INFO( "Setting the POA Thread Pool strategy incompatible with the threaded" );

                try
                {
                    strategyList_2.add_PoaThreadPerRequest();
                    TEST_FAILED();
                }
                catch ( ... )
                {
                    TEST_SUCCEED();
                }

                TEST_INFO( "Setting the POA Threaded strategy in the list (Thread_per_connection)" );
                strategyList_3.add_PoaThreadPerConnection();
                TEST_INFO( "Checking the POA Threaded strategy in the list" );
                TEST_CHECK( strategyList_3.is_PoaThreadPerConnection() == true );

                TEST_INFO( "Setting the POA Thread Pool strategy incompatible with the threaded" );

                try
                {
                    strategyList_3.add_PoaThreadPerConnection();
                    TEST_FAILED();
                }
                catch ( ... )
                {
                    TEST_SUCCEED();
                }

                TEST_INFO( "Trying to construct a new strategy from this one" );
                StrategyList strategyListCopied(strategyList);
                StrategyList strategyListCopied_2(strategyList_2);
                TEST_INFO( "Checking the value" );
                TEST_CHECK( strategyListCopied.is_OrbThreaded() == true );
                TEST_CHECK( strategyListCopied.is_PoaThreadPool( nbThread ) == true );
                TEST_CHECK( nbThread == 3 );
                //    TEST_CHECK( strategyListCopied.is_PoaPortAssigned( poaPort ) == true );
                //    TEST_CHECK( poaPort == 5577 );

                TEST_CHECK( strategyListCopied_2.is_PoaThreadPerRequest() == true );



                TEST_INFO( "Trying to assign a new strategy from this one" );
                StrategyList strategyListAssigned;
                StrategyList strategyListAssigned_2;
                strategyListAssigned = strategyList;
                strategyListAssigned_2 = strategyList_2;

                TEST_INFO( "Checking the value" );
                TEST_CHECK( strategyListAssigned.is_OrbThreaded() == true );
                TEST_CHECK( strategyListAssigned.is_PoaThreadPool( nbThread ) == true );
                TEST_CHECK( nbThread == 3 );
                //    TEST_CHECK( strategyListAssigned.is_PoaPortAssigned( poaPort ) == true );
                //    TEST_CHECK( poaPort == 5577 );

                TEST_CHECK( strategyListAssigned_2.is_PoaThreadPerRequest() == true );
            

                nbOfRequestedTestOK += 9;

                StrategyList strategies;
                strategies.add_OrbThreaded();
                strategies.add_PoaThreadPool(5);
                int timescale = Cdmw::TestUtils::Testable::get_timescale();

                int argc_1 = 0;
                TEST_INFO( "Checking the used of the ORBid" );
                TEST_INFO( "Checking the used of the NULL ORBid" );
                CORBA::ORB_var orb = OrbSupport::ORB_init(argc_1, NULL, strategies);
                TEST_SUCCEED();
                CORBA::ORB_var orb2 = OrbSupport::ORB_init(argc_1, NULL, strategies);
                TEST_SUCCEED();
                OsSupport::OS::sleep(timescale*1000);
                TEST_CHECK(orb.in() == orb2.in());

                OrbSupport::OrbSupport::ORB_cleanup(orb.in());
                orb->destroy();

                TEST_INFO( "Checking the used of the different ORBid" );
                CORBA::ORB_var orb3 = OrbSupport::ORB_init(argc_1, NULL, strategies, "test1");
                TEST_SUCCEED();
                CORBA::ORB_var orb4 = OrbSupport::ORB_init(argc_1, NULL, strategies, "test2");
                TEST_SUCCEED();
                TEST_CHECK(orb3.in() != orb4.in());
                OsSupport::OS::sleep(timescale*1000);
                OrbSupport::OrbSupport::ORB_cleanup(orb3.in());
                orb3->destroy();
                OrbSupport::OrbSupport::ORB_cleanup(orb4.in());
                orb4->destroy();

                TEST_INFO( "Checking the used of the same  ORBid" );
                CORBA::ORB_var orb5 = OrbSupport::ORB_init(argc_1, NULL, strategies, "test");
                TEST_SUCCEED();
                CORBA::ORB_var orb6 = OrbSupport::ORB_init(argc_1, NULL, strategies, "test");
                TEST_SUCCEED();
                TEST_CHECK(orb5.in() == orb6.in());
                OsSupport::OS::sleep(timescale*1000);
                OrbSupport::OrbSupport::ORB_cleanup(orb5.in());
                orb5->destroy();
            }

            // set number of requested successfull tests
            set_nbOfRequestedTestOK (nbOfRequestedTestOK);

            do_orb_tests();

        }

    } // End namespace OrbSupport
} // End namespace Cdmw

