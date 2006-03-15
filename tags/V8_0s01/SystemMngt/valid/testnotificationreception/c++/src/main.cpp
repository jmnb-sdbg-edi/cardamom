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


#include "Foundation/common/Options.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/orbsupport/CORBA.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"

#include "Foundation/testutils/TestManager.hpp"

#include "testnotificationreception/MyProcessBehaviour.hpp"
#include "SystemMngt/platformvaluetypes/ProcessStatusChange_impl.hpp"

#include <cstdlib>
#include <string>
#include <exception>
#include <sstream>
#include <iostream>
#include <fstream>

namespace
{
    using namespace CdmwPlatformMngt;
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::OrbSupport;
    using namespace Cdmw::PlatformMngt;

    const std::string FUNCTION_OPTION = "--function";
    
static void
usage(std::ostream & os, const std::string &program_name)
{
    os << "Usage:\n"
       << program_name << " options\n"
       << "\n"
       << "Options:\n"
       << "--help                                Display this information.\n"
       << "--function=<receptor|--notificator>   receptor    : Process receive message to the supervision.\n"
       << "                                      notificator : Process send message to the supervision.\n"
       << std:: endl;
}
}

    
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

    StrategyList strategy;
    strategy.add_OrbThreaded();
    //strategy.add_PoaThreadPerRequest();
    strategy.add_PoaThreadPerConnection();

    //
    // help argument
    //
    if (argc > 1)
    {
        if(strcmp(argv[1], "--help") == 0)
	    {
            usage(std::cout, argv[0]);
            return ret_code;
        }
    }
    
    
    try
    {
        std::string specifiedFunction = OS::get_option_value (argc, argv, FUNCTION_OPTION);
        if ( specifiedFunction == "yes")
        {
            std::cout << "bad value for " << FUNCTION_OPTION.c_str() << "=<receptor|--notificator> option\n";
            std::cout << std::endl;
            return EXIT_FAILURE;
        }
        else if (specifiedFunction == "no")
        {
            std::cout << "no value for " << FUNCTION_OPTION.c_str() << "=<receptor|notificator> option\n";
            std::cout << std::endl;
            return EXIT_FAILURE;
        }
  

            
        orb =  Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategy); 

        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");

        PortableServer::POA_var poa = PortableServer::POA::_narrow(obj.in());

        PortableServer::POAManager_var poaMgr = poa->the_POAManager();
        
        poaMgr->activate();
        
        // Register event factories
        CORBA::ValueFactoryBase_var factory, old_factory;
        factory = new ProcessMessageFactory;
        old_factory = orb->register_value_factory("IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessage:1.0",
                                                  factory.in() );



        try
        {
            // initialise the platform interface
            Cdmw::PlatformMngt::PlatformInterface::Setup(orb.in(), argc, argv);
            
            // creates the process behaviour
            std::auto_ptr<TestSupervision::MyProcessBehaviour> pMyProcess(new TestSupervision::MyProcessBehaviour(orb.in(), "TestNotificationReception", specifiedFunction.c_str()));
            
            
            // acknowledge the creation of the process
            Cdmw::PlatformMngt::PlatformInterface::Acknowledge_creation(pMyProcess.get());
            pMyProcess.release();
            
            
            struct sigaction action;
            action.sa_handler=exit_handler;
            sigemptyset(&action.sa_mask);
            sigaction(SIGTERM, &action, NULL);
            sigaction(SIGINT, &action, NULL);
            sigaction(SIGQUIT, &action, NULL);
            sigaction(SIGABRT, &action, NULL);

            // start orb
            orb->run();

            // cleanup the platform interface
            PlatformInterface::Cleanup();

        }
        
        catch(...)
        {
            // cleanup the ORB to exit completely
            orb->shutdown(false);
            Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
            orb->destroy();
            throw;
        }
        
        
        //orb->shutdown(false) is done by the ProcessBehaviour
        Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
        orb->destroy();
            
    }
    catch (const CORBA::Exception& e)
    {  
    	std::cerr << "FAILURE : CORBA exception raised :" << std::endl
                  << e._name() << std::endl;
        ret_code = EXIT_FAILURE;
    }
    catch(const std::exception &e)
    {
        std::cerr << "FAILURE : Exception raised : " << std::endl
                  << e.what() << std::endl;
        ret_code = EXIT_FAILURE;
    }
    catch(...)
    {
        std::cerr << "FAILURE : Unexpected exception" << std::endl;
        ret_code = EXIT_FAILURE;
    }
    
    return ret_code;
    
}


