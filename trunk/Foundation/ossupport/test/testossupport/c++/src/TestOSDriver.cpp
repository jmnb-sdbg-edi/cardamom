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


#include "testossupport/TestOSDriver.hpp"

#include <cstdio>
#include <fstream>

#include "Foundation/common/MacroDefs.h"
#include "Foundation/ossupport/OS.hpp"
//#include "Foundation/ossupport/ProcessCallback.hpp"

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

        TestOSDriver::TestOSDriver( )
                : Testable("Cdmw::OS")
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
            set_nbOfRequestedTestOK (24);
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

