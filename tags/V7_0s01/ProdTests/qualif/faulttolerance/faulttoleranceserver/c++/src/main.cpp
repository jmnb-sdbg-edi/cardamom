/* =========================================================================== *
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
 * =========================================================================== */

#include <exception>
#include <iostream>
#include <fstream>

#include "Foundation/osthreads/Thread.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/testutils/Testable.hpp"

#include <faulttoleranceserver/ServerORBInitializer_impl.hpp>
#include "faulttoleranceserver/MyProcessBehaviour.hpp"
#include <FaultTolerance/ftinit/FTServiceInit.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>
// internal cdmw init
#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>

#include <faulttoleranceserver/TestHello_impl.hpp>
#include <faulttoleranceserver/TestMembersAdmin_impl.hpp>
#include <faulttoleranceserver/TestPrimaryBackupAdmin_impl.hpp>
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platforminterface/ServiceNames.hpp"
#include <Foundation/orbsupport/PortableInterceptor.hpp>

#if CDMW_ORB_VDR == tao && CDMW_ORB_VER > 13
#include "tao/ORBInitializer_Registry.h"
#endif

#ifdef CDMW_POSIX

#include <sys/types.h>
#include <unistd.h>

#endif

using namespace std;
using namespace Cdmw;
using namespace Cdmw::Common;
using namespace Cdmw::OsSupport;


namespace
{
    const int POA_THREAD_POOL_SIZE = 10;

    const std::string RM_IOR_OPTION  = "--ReplicationMngRef";
    const std::string HOST_NAME_OPTION  = "--HostName";
    const std::string HOST_NAME_FILE_LOC_OPTION  = "--HostNameFileLocation";
    const std::string HELLO_INTERFACE1  = "--HelloInterface1";
    const std::string HELLO_INTERFACE2  = "--HelloInterface2";
    const std::string HELLO_INTERFACE3  = "--HelloInterface3";
    const std::string INTERCEPTOR  = "--Interceptor";

} // end anonymous namespace


CORBA::ORB_var orb = CORBA::ORB::_nil();

void exit_handler(int sig) {
    if (!CORBA::is_nil(orb.in())) {
        try {
            orb->shutdown(false);
        } catch (const CORBA::SystemException& e) {
            std::cerr << "Error while shuting ORB down in exit_handler:\n"
                      << e << " - minor code: " << e.minor() << std::endl;
        }
    }
}

int main(int argc, char* argv[])
{
    // the return code 
    int ret_code = 0;
    
    std::string portNumber = "4999";
   
    // get host name
    std::string host_name = 
       Cdmw::OsSupport::OS::get_option_value (argc, argv, HOST_NAME_OPTION);

    // get host name file location
    std::string host_name_file_location = 
       Cdmw::OsSupport::OS::get_option_value (argc, argv, HOST_NAME_FILE_LOC_OPTION);
    std::cout << "host_name= " << host_name << std::endl;
    std::cout << "host_name_file_location= " << host_name_file_location << std::endl;

    bool interceptor = false;
    if (Cdmw::OsSupport::OS::get_option_value (argc, argv, INTERCEPTOR) == "yes")
        interceptor = true;
    
    int val = 0;
    // get hello interface
    std::string s_hello_interface1 = 
       Cdmw::OsSupport::OS::get_option_value (argc, argv, HELLO_INTERFACE1);
    
    if (s_hello_interface1  == "yes") 
    {
        val = val + 1;
    } 
    std::string s_hello_interface2 = 
       Cdmw::OsSupport::OS::get_option_value (argc, argv, HELLO_INTERFACE2);
    
    if (s_hello_interface2  == "yes") 
    {
        val = val + 2;
    } 
    std::string s_hello_interface3 = 
       Cdmw::OsSupport::OS::get_option_value (argc, argv, HELLO_INTERFACE3);
    
    if (s_hello_interface3  == "yes") 
    {
        val = val + 4;
    }

    
    try
    {        
        // Initialise FT service
        std::cout << "test_server: initialise FT service" << std::endl;
        Cdmw::FT::FTServiceInit::Init(argc, argv);
        
        //Register interceptor
        ServerORBInitializer_impl * temp_initializer = 0;
        PortableInterceptor::ORBInitializer_var initialiser;
        if (interceptor)
        {
            temp_initializer = new ServerORBInitializer_impl;
            initialiser = temp_initializer;
            PortableInterceptor::register_orb_initializer(initialiser.in());
        }
        
        // Initialises the ORB
        OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        std::cout << "test_server: initialise the orb" << std::endl;
        orb =  OrbSupport::OrbSupport::ORB_init(argc,argv, strategyList); 
        
        // And activate the root POA
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
        
        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

        // creates the process behaviour
        std::auto_ptr<MyProcessBehaviour> 
        pProcessBehaviour(new MyProcessBehaviour(orb.in(), host_name, val, host_name_file_location));

        // init Cdmw
        std::cout << host_name << " test_server: init Cdmw"<<std::endl;
        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv,  pProcessBehaviour.get());

        // get application name
        std::string application_name =  Cdmw::PlatformMngt::PlatformInterface::Get_application_name();

        // get process name
        std::string process_name = Cdmw::PlatformMngt::PlatformInterface::Get_process_name();

        ::FT::Location loc;
        loc.length(3);
        loc[0].id = CORBA::string_dup(host_name.c_str());
        loc[0].kind = "hostname";
        loc[1].id = CORBA::string_dup(application_name.c_str());
        loc[1].kind = "applicationname";
        loc[2].id = CORBA::string_dup(process_name.c_str());
        loc[2].kind = "processname";


        if (interceptor)
        {
            ::Cdmw::FT::Location::TestPrimaryBackupAdmin_impl * testPrimaryBackupAdmin_impl
              = new Cdmw::FT::Location::TestPrimaryBackupAdmin_impl(rootPOA.in(), loc);
            
            ::CdmwFT::Location::PrimaryBackupAdmin_var testPrimaryBackupAdmin = testPrimaryBackupAdmin_impl->_this();
            
            // Set the forward references in the server request interceptor.
            PortableInterceptor::ServerRequestInterceptor_var
            server_interceptor = temp_initializer->server_interceptor ();

            Test::ServerRequestInterceptor_var interceptor =
            Test::ServerRequestInterceptor::_narrow (server_interceptor.in ());
            interceptor->forward_references (testPrimaryBackupAdmin.in ());
        }
        
        struct sigaction action;
        action.sa_handler=exit_handler;
        sigemptyset(&action.sa_mask);
        sigaction(SIGTERM, &action, NULL);
        sigaction(SIGINT, &action, NULL);
        sigaction(SIGQUIT, &action, NULL);
        sigaction(SIGABRT, &action, NULL);

        orb->run();
        
        //orb->shutdown() is done by the stop() of the ProcessBehaviour
        Cdmw::CdmwInit::CDMW_cleanup(orb.in());
        Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
        orb->destroy();
        

    }

    catch(const CORBA::Exception &e)
    {
        std::cerr << "FAILURE : Unexpected exception" << std::endl << e._name() << endl;
        ret_code = EXIT_FAILURE;
    }

    return ret_code;

}

