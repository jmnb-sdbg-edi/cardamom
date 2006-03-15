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
#include <limits>

#include "testnavigability/TestNavigability.hpp"

#include "Foundation/common/Options.hpp"
#include "Foundation/testutils/CORBATestManager.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "Foundation/common/Exception.hpp"
#include "Foundation/common/System.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

#include "idllib/CdmwPlatformMngtSupervisionObserver.skel.hpp"
#include "idllib/CdmwPlatformMngtSystem.stub.hpp"
#include "idllib/CdmwPlatformMngtApplication.stub.hpp"

using namespace std;

using namespace Cdmw;

using namespace Common;

using namespace PlatformMngt;

using namespace CdmwPlatformMngt;

CPPUNIT_TEST_SUITE_REGISTRATION (TestNavigability);

namespace Cdmw
{
    namespace PlatformMngt
    {



        TestNavigability::TestNavigability () throw ()
        {

        }


        TestNavigability::~TestNavigability () throw ()
        {

        }

        /* @brief   do_tests method for new navigability methods of IBB2
           @brief   developments  .
        */

        void TestNavigability::do_tests ()
        {
            // //  set_nbOfRequestedTestOK (11);

            //code for the test

            //CORBA::ORB_var orb;                        // orb reference

            try
            {
                m_orb =
                CORBA::ORB::_duplicate (Cdmw::TestUtils::CORBATestManager::
                                        instance ()->get_ORB ());

                /* 
                   ===================================================
                   Get the HelloInterface object
                   ===================================================
                */

                /* import the object reference from the ior file */
                CORBA::Object_var objref;

                /*define the corbaloc */
                const char *corbaloc_str =
                "corbaloc::localhost:21871/CdmwPlatformMngtSupervision";

                objref = m_orb->string_to_object (corbaloc_str);

                /*resolve the System */
                CdmwPlatformMngt::System_var mysystem =
                CdmwPlatformMngt::System::_narrow (objref.in ());

                if (CORBA::is_nil (mysystem.in ()))
                {
                    TEST_INFO ("Error in narrowing CdmwPlatformMngt::System");
                    exit (-1);
                }

                /*
                  use the get_host function of System to know
                  an host

                  the signature is
                  CdmwPlatformMngt::Host_ptr get_host(const char* host_name)
                */

                // get the name of the localhost from environment
                char *hostname = getenv ("HOSTNAME");

                CdmwPlatformMngt::Host_var myhost = mysystem->get_host (hostname);

                if (CORBA::is_nil (myhost.in ()))
                {
                    TEST_INFO
                    ("Error in return of get_host function from System to Host");
                    exit (-1);
                }

                /*
                  from myhost get the system
                */

                CdmwPlatformMngt::System_var mysystem_new =
                CdmwPlatformMngt::System::_narrow (myhost->get_parent ());

                if (CORBA::is_nil (mysystem_new.in ()))
                {
                    TEST_INFO
                    ("Error in return of get_parent function from Host to System");
                    exit (-1);
                }

                TEST_INFO
                ("Test using get_number_of_applications() from System to Applications:");
                CORBA::ULong numapps;
                numapps = mysystem->get_number_of_applications ();
                if (numapps == 1)
                {
                    CPPUNIT_ASSERT (1);
                }
                else
                {
                    CPPUNIT_ASSERT (0);
                }

                /*
                  compare the 2 _ptr variables: if they are equal, then the test
                  is a success
                */

                TEST_INFO ("Test using get_parent() from Host to System");

                if (mysystem->_is_equivalent (mysystem_new.in ()))
                {
                    CPPUNIT_ASSERT (1);
                }
                else
                {
                    CPPUNIT_ASSERT (0);
                }



                /*
                  Other test: now we test the function get_processes from Host to Process

                  use the new method get_processes of Host
                  ad store it to a new sequence
                */

                CdmwPlatformMngt::Processes_var my_processes_from_host;

                CdmwPlatformMngt::ProcessesIterator_var proc_iter;	
                my_processes_from_host = myhost->get_processes (1000, proc_iter);

                CdmwPlatformMngt::Applications_var my_apps;

                CdmwPlatformMngt::ApplicationsIterator_var app_iter;

                numeric_limits < CORBA::ULong > big_number;
                CORBA::ULong max_ulong;
                max_ulong = big_number.max ();

                /*retrieve the applications of the system */
                my_apps = (mysystem->get_applications (max_ulong, app_iter.out ()));

                CORBA::ULong how_many;	/*number of applications
                                          in this system */

                /*other needed declarations */
                how_many = my_apps->length ();

                CdmwPlatformMngt::Processes_var my_processes_from_app;
                CdmwPlatformMngt::Processes_var my_host_processes_from_app =
                new CdmwPlatformMngt::Processes ();

                CORBA::ULong size_seq;

                CdmwPlatformMngt::Host_var actual_host;
                CdmwPlatformMngt::Application_ptr app;
                CdmwPlatformMngt::ProcessesIterator_var process_iter;
                CdmwPlatformMngt::Process_ptr tmp_process;


                CORBA::ULong i, j, k;

                /* use a cycle with the single get_application */
                k = 0;
                for (i = 0; i < how_many; i++)
                {
                    app = my_apps[i];

                    my_processes_from_app =
                    app->get_processes (max_ulong, process_iter.out ());

                    size_seq = my_processes_from_app->length ();


                    for (j = 0; j < size_seq; j++)
                    {
                        actual_host = my_processes_from_app[j]->get_host ();

                        if (actual_host->_is_equivalent (myhost.in ()))
                        {
                            tmp_process = my_processes_from_app[j];

                            if (CORBA::is_nil (tmp_process))
                            {
                                std::cout << "Error tmp_process is nil" << std::endl;
                                exit (-1);
                            }

                            // store the process corresponding to the local host
                            // var is duplicated
                            my_host_processes_from_app->length (k + 1);
                            my_host_processes_from_app[k] =
                            CdmwPlatformMngt::Process::_duplicate (tmp_process);

                            k++;
                        }
                    }
                }

                /* compare the 2 sequences to see if the test is ok */
                CORBA::ULong size1, size2;

                size1 = my_host_processes_from_app->length ();
                size2 = my_processes_from_host->length ();

                TEST_INFO
                ("Test using getprocesses() from Host to Process (test size of returned sequence)");

                if ((size1 == 3) && (size1 == size2))
                {
                    CPPUNIT_ASSERT (1);
                }
                else
                {
                    CPPUNIT_ASSERT (0);
                }

                char flag_test = 0;

                /*check if the 2 sequences are equal */
                for (j = 0; j < size1; j++)
                {
                    if (my_processes_from_host[j]->
                        _is_equivalent ((my_host_processes_from_app[j]).in ()))
                    {
                        ;
                    }
                    else
                    {
                        flag_test = 1;	/*error found in a element sequence */
                    }
                }

                TEST_INFO
                ("Test using getprocesses() from Host to Process (check elements of returned sequence)");

                if (flag_test == 0)
                {
                    CPPUNIT_ASSERT (1);	/*ok */
                }
                else
                {
                    CPPUNIT_ASSERT (0);	/*test error */
                }



                CdmwPlatformMngt::Application_var application =
                mysystem->get_application ("HELLO_WORLD");

                TEST_INFO
                ("Test using get_number_of_processes() from Application to Process: ");
                CORBA::ULong numprocs;
                numprocs = application->get_number_of_processes ();
                if (numprocs == 3)
                {
                    CPPUNIT_ASSERT (1);
                }
                else
                {
                    CPPUNIT_ASSERT (0);
                }


                TEST_INFO ("Test using get_parent() from Application to System: ");


                if (CORBA::is_nil (application->get_parent ()))
                {
                    std::cout << " returned form get_parent is nil!" << std::endl;


                }


                if (mysystem->
                    _is_equivalent (CdmwPlatformMngt::System::
                                    _narrow (application->get_parent ())))
                {
                    CPPUNIT_ASSERT (1);
                }
                else
                {
                    CPPUNIT_ASSERT (0);
                }

                TEST_INFO
                ("Test using get_processes(... , ...) from Host to Process (also check elements of returned iterator):");

                CdmwPlatformMngt::Processes_var my_part_processes;
//                 CdmwPlatformMngt::Processes_var my_processes_from_host =
//                 new CdmwPlatformMngt::Processes ();

                CdmwPlatformMngt::ProcessesIterator_var proc_part_iter;
                my_part_processes = myhost->get_processes (1, proc_part_iter);

                CdmwPlatformMngt::Processes_var rest_processes;
                CORBA::ULong part1, part2;

                part1 = my_part_processes->length ();
                char part1_flag = 0;
                char part2_flag = 0;


                if (part1 == 1)
                {
                    /*first check on first part */
                    if (my_part_processes[(unsigned int)0]->
                        _is_equivalent ((my_processes_from_host[(unsigned int)0]).in()))
                    {
                        part1_flag = 1;
                    }

                }

                proc_part_iter->next_n (1000, rest_processes.out());

                part2 = rest_processes->length ();

                if (part2 == 2)
                {
                    if ((rest_processes[(unsigned int)0]->_is_equivalent (( my_processes_from_host[(unsigned int)1]).in()))
                        && (rest_processes[(unsigned int)1]->_is_equivalent (( my_processes_from_host[(unsigned int)2]).in())))
                    {
                        part2_flag = 1;
                    }
                }

                if ((part1_flag == 1) && (part2_flag == 1))
                {
                    CPPUNIT_ASSERT (1);	/*test ok */
                }
                else
                {
                    CPPUNIT_ASSERT (0);	/*test not ok */
                }


                TEST_INFO
                ("Test using get_parent() from Process to Application: HelloProcess1");
                CdmwPlatformMngt::Process_var process =
                application->get_process ("HelloProcess1", hostname);

                if (application->
                    _is_equivalent (CdmwPlatformMngt::Application::
                                    _narrow (process->get_parent ())))
                {
                    CPPUNIT_ASSERT (1);
                }
                else
                {
                    CPPUNIT_ASSERT (0);
                }

                TEST_INFO
                ("Test using get_parent() from Process to Application: HelloProcess2");
                process = application->get_process ("HelloProcess2", hostname);

                if (application->
                    _is_equivalent (CdmwPlatformMngt::Application::
                                    _narrow (process->get_parent ())))
                {
                    CPPUNIT_ASSERT (1);
                }
                else
                {
                    CPPUNIT_ASSERT (0);
                }

                TEST_INFO
                ("Test using get_parent() from Process to Application: HelloProcess3");
                process = application->get_process ("HelloProcess3", hostname);

                if (application->
                    _is_equivalent (CdmwPlatformMngt::Application::
                                    _narrow (process->get_parent ())))
                {
                    CPPUNIT_ASSERT (1);
                }
                else
                {
                    CPPUNIT_ASSERT (0);
                }



                /* test on function get_process(const char *) */
                CdmwPlatformMngt::Process_var my_process;

                // get a specific process (specified by host name, application name, process name)
                my_process = myhost->get_process ("HelloProcess1", "HELLO_WORLD");

                /*use a cycle with the single get_application */
                CdmwPlatformMngt::Process_var my_process_new;

                char flag_found = 0;
                k = 0;

                for (i = 0; i < how_many; i++)
                {
                    // get process for each application
                    app = my_apps[i];

                    my_process_new = app->get_process ("HelloProcess1", hostname);

                    // get the host of the found process
                    actual_host = my_process_new->get_host ();

                    // if host corresponds and process corresponds : process is found
                    if ((actual_host->_is_equivalent (myhost.in ())) &&
                        (my_process->_is_equivalent (my_process_new.in ())))
                    {
                        flag_found = 1;
                        break;
                    }
                }

                TEST_INFO
                ("Test using get_process(const char *) from Host to Process");
                if (flag_found == 1)
                {
                    CPPUNIT_ASSERT (1);
                }
                else
                {
                    CPPUNIT_ASSERT (0);
                }

            }
            catch (const Cdmw::Exception & ex)
            {
                std::cerr << ex.what () << std::endl;
                exit (-1);
            }
            catch (const CORBA::Exception & ex)
            {
                std::cerr << ex << std::endl;
                exit (-1);
            }
            catch (...)
            {
                std::cerr << "unexpected exception" << std::endl;
                exit (-1);
            }

            /*
              ========================================================
              program stopping
              ========================================================
            */

            // Call ORB cleanup
            Cdmw::OrbSupport::OrbSupport::ORB_cleanup (m_orb.in ());

            // Destroy orb
            if (!CORBA::is_nil (m_orb.in ()))
            {
                try
                {
                    m_orb->destroy ();
                }
                catch (const CORBA::Exception & ex)
                {
                    std::cerr << ex << std::endl;
                    exit (-1);
                }
            }

        }



    }				/* End namespace PlatformMngt */
}				/* End namespace Cdmw */
