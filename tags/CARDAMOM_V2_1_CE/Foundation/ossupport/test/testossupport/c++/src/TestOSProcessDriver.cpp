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


#include "testossupport/TestOSProcessDriver.hpp"

#include <cstdio>
#include <fstream>

#include "Foundation/common/MacroDefs.h"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/ossupport/ProcessCallback.hpp"

namespace Cdmw
{

    namespace OsSupport
    {

        /**
        *Implementation notes: [if no pertinent write none ]
        *<p>
        *Portability issues: [if no pertinent write none ]
        *<p>
        */
/*
        extern "C"
        {
            double test(double r)
            {
                return r;
            };
        }
*/

        class TestProcessCallback : public ProcessCallback
        {

            public:

                TestProcessCallback()
                        : m_processId(0)
                {}

                virtual ~TestProcessCallback()
                { }

                OS::ProcessId getProcessId()
                {
                    return m_processId;
                }

                void execute(OsSupport::OS::ProcessId processId) throw()
                {
                    m_processId = processId;
                }

            private:

                OS::ProcessId m_processId;

        };






        TestOSProcessDriver::TestOSProcessDriver( const std::string& processName )
                : Testable("Cdmw::OS processes"),
                  m_processName(processName),
                  m_valid_only(false)
        {
        }

        TestOSProcessDriver::TestOSProcessDriver( const std::string& processName,
                                                  bool valid_only)
                : Testable("Cdmw::OS processes"),
                  m_processName(processName),
                  m_valid_only(valid_only)
        {
        }


        TestOSProcessDriver::~TestOSProcessDriver()
        throw()
        {
        }


        void
        TestOSProcessDriver::do_tests()
        {
            // set number of requested successfull tests
            if (!m_valid_only) set_nbOfRequestedTestOK(17);
            else set_nbOfRequestedTestOK(3); 

	        int timescale = Cdmw::TestUtils::Testable::get_timescale();

            // This part is only for unit test
            if (!m_valid_only)
            {
                TEST_INFO("Remove a temporary file name");
                std::string tmp_file_str = "Cdmw";
                ::unlink(tmp_file_str.c_str());

                TEST_INFO("Checking this file not exists");
                TEST_CHECK( ! OS::file_exists(tmp_file_str) );

                // Process Creation
            
                TEST_INFO("Creating a new process");

                TestProcessCallback creationCallback;

                // warning : the ending callback is created on the stack in this basic
                // test. A normal use requires that the ending callback survives
                // until the ending is notified asynchronously.
                TestProcessCallback endingCallback;

                OS::ProcessId pid_created = OS::create_process(m_processName, "--create-file=" + tmp_file_str,
                                                               "", &creationCallback, &endingCallback);

                // We sleep a while to wait the process and file creation
                OS::sleep(timescale*4000);

                TEST_INFO("Checking if we have been notified of the process creation");
                TEST_CHECK( pid_created == creationCallback.getProcessId());

                TEST_INFO("Checking if we have been notified of the process termination");
                TEST_CHECK( pid_created == endingCallback.getProcessId());

                TEST_INFO("Checking the file has been created by process");
                TEST_CHECK( OS::file_exists(tmp_file_str) );

                {
                    TEST_INFO("Checking if a process can retreive its own process id");
                    std::ifstream pidfile(tmp_file_str.c_str());
                    OS::ProcessId pid_retreived;
                    pidfile >> pid_retreived;
                    pidfile.close();
                    TEST_CHECK( pid_created == pid_retreived);
                }

                TEST_INFO("Trying to execute a non executable file");

                try
                {
                    OS::create_process(tmp_file_str, "");

                }
                catch ( const BadParameterException& )
                {

                    TEST_SUCCEED();

                }
                catch (...)
                {

                    TEST_FAILED();
                }

                TEST_INFO("Trying to delete an existing file");

                try
                {
                    OS::unlink(tmp_file_str);
                    TEST_CHECK( ! OS::file_exists(tmp_file_str) );

                }
                catch ( ... )
                {

                    TEST_FAILED();

                }

                TEST_INFO("Trying to delete an unexisting file");

                try
                {
                    OS::unlink(tmp_file_str);
                    TEST_FAILED();

                }
                catch ( const BadParameterException& )
                {

                    TEST_SUCCEED();

                }


                // Process Creation with working directory and set with PATH environment

                TEST_INFO("Creating a new process defined by PATH and with working directory");
                TEST_INFO(m_processName.c_str());

                TestProcessCallback creationCallback2;

                // warning : the ending callback is created on the stack in this basic
                // test. A normal use requires that the ending callback survives
                // until the ending is notified asynchronously.
                TestProcessCallback endingCallback2;

                OS::ProcessId pid_created2 = OS::create_process("test_process", "--create-file=" + tmp_file_str,
                                                                ".", &creationCallback2, &endingCallback2);

                // We sleep a while to wait the process and file creation
                OS::sleep(timescale*4000);

                TEST_INFO("Checking if we have been notified of the process creation");
                TEST_CHECK( pid_created2 == creationCallback2.getProcessId());

                TEST_INFO("Checking if we have been notified of the process termination");
                TEST_CHECK( pid_created2 == endingCallback2.getProcessId());

                TEST_INFO("Checking the file has been created by process");
                TEST_CHECK( OS::file_exists(tmp_file_str) );
            
            
                // delete the created file
                try
                {
                    OS::unlink(tmp_file_str);
                }
                catch ( ... )
                {
                }
        
            
                // Process Creation with not existing working directory

                TEST_INFO("Creating a new process with not existing working directory");

                TestProcessCallback creationCallback3;

                // warning : the ending callback is created on the stack in this basic
                // test. A normal use requires that the ending callback survives
                // until the ending is notified asynchronously.
                TestProcessCallback endingCallback3;

                OS::ProcessId pid_created3 = OS::create_process(m_processName, "--create-file=" + tmp_file_str,
                                                                "/notexistdir", &creationCallback3, &endingCallback3);

                // We sleep a while to wait the process and file creation
                OS::sleep(timescale*4000);

                TEST_INFO("Checking if we have been notified of the process creation");
                TEST_CHECK( pid_created3 == creationCallback3.getProcessId());

                TEST_INFO("Checking if we have been notified of the process termination");
                TEST_CHECK( pid_created3 == endingCallback3.getProcessId());

                // the process has not started effectiveley in fact (exec has not been called)
                TEST_INFO("Checking the file is not created by process");
                TEST_CHECK( !OS::file_exists(tmp_file_str) );            
            

                try
                {
                    OS::unlink(tmp_file_str);
                }
                catch ( ... )
                {
                }
            }
            
            // Test of kill process

            TestProcessCallback creationCallback4;
            // warning : the ending callback is created on the stack in this basic
            // test. A normal use requires that the ending callback survives
            // until the ending is notified asynchronously.
            TestProcessCallback endingCallback4;

            OS::ProcessId pid_created4 = OS::create_process("test_process", "--sleep=9",
                                        ".", &creationCallback4, &endingCallback4);

            // We sleep a while to wait the process
            // (for this test it must be significatly < --sleep * 1000 *timescale )
            OS::sleep(timescale*4000);

            TEST_INFO("Checking if we have been notified of the process creation");
            TEST_CHECK( pid_created4 == creationCallback4.getProcessId());
            TEST_INFO("Checking that we have not been notified of the process termination");
            TEST_CHECK( pid_created4 != endingCallback4.getProcessId());
            TEST_INFO("Perform a kill on the created process");
            OS::kill_process( pid_created4 );
            OS::sleep(timescale*500);
            TEST_INFO("Checking if we have been notified of the process termination");
            TEST_CHECK( pid_created4 == endingCallback4.getProcessId());
        }


    } // End namespace OsSupport
} // End namespace Cdmw

