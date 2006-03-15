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



#include "SystemMngt/platformlibrary/ProcessKillMngr.hpp"
#include "testplatformlibrary/TestProcessKillMngr.hpp"

#include <string>
#include <exception>
#include <iostream>
#include <sstream>

using namespace Cdmw;
using namespace Cdmw::PlatformMngt;
using namespace Cdmw::OsSupport;

CPPUNIT_TEST_SUITE_REGISTRATION( TestProcessKillMngr );


// ----------------------------------------------------------------------
// TestProcessKillMngr class.
// ----------------------------------------------------------------------
TestProcessKillMngr::TestProcessKillMngr ()
{
}

TestProcessKillMngr::~TestProcessKillMngr()
{
}

void TestProcessKillMngr::do_tests()
{
    // set number of requested successfull tests
//     set_nbOfRequestedTestOK (9);
    
    try
    {
        std::string application1 = "app1";
        std::string process1 = "proc1";
        Cdmw::OsSupport::OS::ProcessId pid1 = 10567;
        
        std::string application2= "app2";
        std::string process2 = "proc2";
        Cdmw::OsSupport::OS::ProcessId pid2= 10568;

        std::string application3= "app3";
        std::string process3 = "proc3";
        Cdmw::OsSupport::OS::ProcessId pid3= 10569;

        // option to set kill process manager to not request the OS kill of the request process
        //std::string arg_option = "--noKillByMonitoring"; 
        
        int argc = 2;
        char* argv[2];
        argv[0] = 0;
        argv[1] = "--noKillByMonitoring";
        
        
        
        //
        // create Process Kill Manager singleton
        //
        ProcessKillMngr::Init (argc, argv);
        
        
        //
        // System
        //
        
        TEST_INFO ("Register the new process");
        
        ProcessKillMngr::Instance().register_process (application1.c_str(),
                                                      process1.c_str(), pid1);
                                                      
        CPPUNIT_ASSERT(true);
        
        TEST_INFO ("Register the new process");
        
        ProcessKillMngr::Instance().register_process (application2.c_str(),
                                                      process2.c_str(), pid2);
                                                      
        CPPUNIT_ASSERT(true);
        
        TEST_INFO ("Register the new process");
        
        ProcessKillMngr::Instance().register_process (application3.c_str(),
                                                      process3.c_str(), pid3);
                                                      
        CPPUNIT_ASSERT(true);

        TEST_INFO ("kill the process");
        ProcessKillMngr::Instance().kill_process (application1.c_str(),
                                                  process1.c_str());
                                                  
        CPPUNIT_ASSERT(true);
        
        TEST_INFO ("remove the process by name");
        ProcessKillMngr::Instance().remove_process (application1.c_str(),
                                                    process1.c_str());
                                                    
        CPPUNIT_ASSERT(true);    
                                                                                           
        TEST_INFO ("remove the process by name");
        ProcessKillMngr::Instance().remove_process (application1.c_str(),
                                                    process1.c_str());
                                                    
        CPPUNIT_ASSERT(true);                                                                                               
                                                    
        TEST_INFO ("remove the process by pid");
        ProcessKillMngr::Instance().remove_process (pid1);
        
        CPPUNIT_ASSERT(true);
        
         TEST_INFO ("remove the process by pid");
        ProcessKillMngr::Instance().remove_process (pid1);
        
        CPPUNIT_ASSERT(true);
        
        TEST_INFO ("remove the process by pid");
        ProcessKillMngr::Instance().remove_process (pid2);
        
        CPPUNIT_ASSERT(true);
        
   }
    catch (const Cdmw::Exception& e)
    {
        std::cout << e.what() << std::endl;
        CPPUNIT_ASSERT(false);
    }

}


