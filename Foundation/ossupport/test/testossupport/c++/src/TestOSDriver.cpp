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


#include "testossupport/TestOSDriver.hpp"

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

        extern "C"
        {
            double test(double r)
            {
                return r;
            };
        }


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






        TestOSDriver::TestOSDriver( const std::string& processName )
                : Testable("Cdmw::OS"),
                m_processName(processName)
        {
        }


        TestOSDriver::~TestOSDriver()
        throw()
        {
        }


        void
        TestOSDriver::do_tests()
        {
            // set number of requested successfull tests
            set_nbOfRequestedTestOK (38);
	    int timescale = Cdmw::TestUtils::Testable::get_timescale();


            // TODO: finding how to better test the returned date.
            TEST_INFO("Trying to get the time");

            try
            {
                OS::Timeval timeval = OS::get_time();
                TEST_INFO("Seconds are     : " << timeval.seconds);
                TEST_INFO("Microseconds are: " << timeval.microseconds);
                TEST_CHECK( timeval.seconds > 1000000000 );
            }
            catch ( ...)
            {
                TEST_FAILED();
            }

            TEST_INFO("Try to compare two times");
            try
            {
                OS::Timeval timeval1 = OS::get_time();
		OS::sleep(timescale*1000);
                OS::Timeval timeval2 = OS::get_time();
		TEST_CHECK( timeval2 > timeval1 );
                TEST_CHECK( timeval1 < timeval2 );
                TEST_CHECK( !(timeval2 == timeval1) )
            }
            catch ( ...)
            {
                TEST_FAILED();
            }

            TEST_INFO("Trying to get the hostname");
            try
            {
                std::string hostname = OS::get_hostname();
                TEST_INFO( hostname );
                TEST_SUCCEED();
            }
            catch (...)
            {
                TEST_FAILED();
            }

            TEST_INFO("Trying to get the user name");
            try
            {
                std::string logname = OS::get_logname();
                TEST_INFO( logname );
                TEST_SUCCEED();
            }
            catch (...)
            {
                TEST_FAILED();
            }

            TEST_INFO("Creating a temporary file name");
            char* tmp_file = ::tempnam(NULL, "Prco");
            std::string tmp_file_str = tmp_file;
            ::free(tmp_file);

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
            

            std::auto_ptr<OS::SharedObjectHandle> handle;
            std::auto_ptr<OS::SharedObjectHandle> handle2;
            std::auto_ptr<OS::SharedObjectHandle> handle3;
#ifdef AIX

            std::string libraryName = "libc.a(shr.o)";
            std::string FullPathLibraryName = "/usr/lib/libc.a(shr.o)";
            std::string functionName = "atoi";
            int dlopenFlags = RTLD_MEMBER | RTLD_LAZY;
#else

            std::string libraryName = "libm.so";
            std::string FullPathLibraryName = "/usr/lib/libm.so";
            std::string functionName = "cos";
            int dlopenFlags = RTLD_LAZY;
#endif



            TEST_INFO("Trying to load a non-existent dynamic library");

            try
            {
                std::auto_ptr<OS::SharedObjectHandle> handle_tmp(OS::dlopen("libmazeazerazreza.so"));
                handle = handle_tmp;
                TEST_CHECK (handle->is_null());
            }
            catch ( ...)
            {
                TEST_FAILED();
            }


            TEST_INFO("Trying to load a dynamic library");

            try
            {
                std::auto_ptr<OS::SharedObjectHandle> handle_tmp(OS::dlopen(libraryName.c_str(), dlopenFlags));
                handle = handle_tmp;
                TEST_CHECK (!handle->is_null());
            }
            catch ( ...)
            {
                TEST_FAILED();
            }


            TEST_INFO("Trying to load a dynamic library (second time in an other handle)");

            try
            {
                std::auto_ptr<OS::SharedObjectHandle> handle_tmp(OS::dlopen(libraryName.c_str(), dlopenFlags));
                handle2 = handle_tmp;
                TEST_CHECK (!handle2->is_null());
            }
            catch ( ...)
            {
                TEST_FAILED();
            }

            TEST_INFO("Trying to execute a function via a dynamic library");

            try
            {
                typedef double (*fun_t)(double);
                fun_t fun = NULL;
                fun = (fun_t)OS::dlsym(handle.get(), functionName.c_str());
                TEST_CHECK (fun != NULL);
                std::cout << "cos result :" << (*fun)(3.1415927 / 4) << std::endl;
            }
            catch ( ...)
            {
                TEST_FAILED();
            }


            TEST_INFO("Trying to execute a non-existent function via a dynamic library");

            try
            {
                typedef double (*fun_t)(double);
                fun_t fun2 = NULL;
                fun2 = (fun_t) OS::dlsym(handle.get(), "cosjhjkhjk");

                if (fun2 == NULL)
                {
                    TEST_CHECK (OS::dlerror() != "");
                }
            }
            catch ( ...)
            {
                TEST_FAILED();
            }

            TEST_INFO("Trying to unload a dynamic library");

            try
            {
                TEST_CHECK (OS::dlclose(handle.get()) == 0);
            }
            catch ( ...)
            {
                TEST_FAILED();
            }

            TEST_INFO("Trying to execute a function via a dynamic library already closed");

            try
            {
                typedef double (*fun_t)(double);
                fun_t fun = NULL;
                fun = (fun_t)OS::dlsym(handle.get(), functionName.c_str());
                TEST_CHECK (fun != NULL);
                std::cout << "cos result :" << (*fun)(3.1415927 / 2) << std::endl;
            }
            catch ( ...)
            {
                TEST_FAILED();
            }

            TEST_INFO("Trying to unload a dynamic library before unloaded (but open 2 times)");

            try
            {
                TEST_CHECK (OS::dlclose(handle.get()) == 0 );
            }
            catch ( ...)
            {
                TEST_FAILED();
            }



            TEST_INFO("Trying to unload a dynamic library before unloaded (reference counter = 0)");

            try
            {

                TEST_CHECK (OS::dlclose(handle.get()) != 0 );
            }
            catch ( ...)
            {
                TEST_FAILED();
            }

            TEST_INFO("Trying to unload a dynamic library before unloaded (second handle and reference counter = 0)");

            try
            {
                TEST_CHECK (OS::dlclose(handle2.get()) != 0 );
            }
            catch ( ...)
            {
                TEST_FAILED();
            }


            TEST_INFO("Trying to load all functions by a dlopen(NULL)");

            try
            {
                std::auto_ptr<OS::SharedObjectHandle> handle_tmp(OS::dlopen(NULL, RTLD_LAZY | RTLD_GLOBAL));
                handle = handle_tmp;
                TEST_CHECK (!handle->is_null());
            }
            catch ( ...)
            {
                TEST_FAILED();
            }

            TEST_INFO("Trying to load all functions by a dlopen(NULL) (second time)");

            try
            {
                std::auto_ptr<OS::SharedObjectHandle> handle_tmp(OS::dlopen(NULL, RTLD_LAZY | RTLD_GLOBAL));
                handle = handle_tmp;
                TEST_CHECK (!handle->is_null());
            }
            catch ( ...)
            {
                TEST_FAILED();
            }

            TEST_INFO("Trying to execute a function via the previous");

            try
            {

                typedef double (*fun_t)(double);
                fun_t fun = NULL;
                fun = (fun_t)OS::dlsym(handle.get(), "test");
                TEST_CHECK (fun(3) == 3);
            }
            catch ( ...)
            {
                TEST_FAILED();
            }

            TEST_INFO("Trying to unload a dynamic library loaded with  dlopen(NULL)");

            try
            {
                TEST_CHECK (OS::dlclose(handle.get()) == 0 );
            }
            catch ( ...)
            {
                TEST_FAILED();
            }


            TEST_INFO("Trying to execute a function via the previous");

            try
            {

                typedef double (*fun_t)(double);
                fun_t fun = NULL;
                fun = (fun_t)OS::dlsym(handle.get(), "test");
                TEST_CHECK (fun(3) == 3);
            }
            catch ( ...)
            {
                TEST_FAILED();
            }

            TEST_INFO("Trying to unload a dynamic library loaded with  dlopen(NULL)(ref counter = 1)");

            try
            {
                TEST_CHECK (OS::dlclose(handle.get()) == 0 );
            }
            catch ( ...)
            {
                TEST_FAILED();
            }

            TEST_INFO("Trying to load a dynamic library with a full path");

            try
            {
                std::auto_ptr<OS::SharedObjectHandle> handle_tmp(OS::dlopen(FullPathLibraryName.c_str(), dlopenFlags));
                handle3 = handle_tmp;
                TEST_CHECK (!handle3->is_null());
            }
            catch ( ...)
            {
                TEST_FAILED();
            }


            TEST_INFO("Trying to unload a dynamic library (fullpath)");

            try
            {

                TEST_CHECK (OS::dlclose(handle3.get()) == 0 );
            }
            catch ( ...)
            {
                TEST_FAILED();
            }

        }



    } // End namespace OsSupport
} // End namespace Cdmw

