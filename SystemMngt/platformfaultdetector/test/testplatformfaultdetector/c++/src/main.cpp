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


#include "testplatformfaultdetector/PILOT_PlatformFaultDetector.hpp"
#include "testplatformfaultdetector/PullMonitorable_impl.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/testutils/CORBATestManager.hpp"

#include "Foundation/common/Options.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/orbsupport/CORBA.hpp"

#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

#include "SystemMngt/platformlibrary/ProcessKillMngr.hpp"

#include <cstdlib>
#include <string>
#include <exception>
#include <sstream>
#include <iostream>
#include <fstream>


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


int main( int argc, char* argv[])
{
    using namespace Cdmw;
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::OrbSupport;
    using namespace Cdmw::PlatformMngt;


    // the return code 
    int ret_code = 0;
    
    // create the ProcessKillMngr singleton
    ProcessKillMngr::Init (argc, argv);
    
    if ( OS::get_option_value(argc,argv, "--server") == "no" ) 
    {
       std::cout << "Starting FaultDetector Test" << std::endl;

       Cdmw::TestUtils::CORBATestManager::instance()->run_tests();
       Cdmw::TestUtils::CORBATestManager::instance()->summary();
       return Cdmw::TestUtils::CORBATestManager::instance()->was_successful();
    }
    else
    {
        std::cout << "Starting PullMonitorable server" << std::endl;

        const unsigned int defSleepTime = 200;
        std::string serviceName = "PullMonitorable_";
        std::string servicePort;

        try
        {

           StrategyList strategy;
           strategy.add_OrbThreaded();
           strategy.add_PoaThreadPool(5);
           
           CORBA::ORB_var orb;

           servicePort = OS::get_option_value (argc, argv, 
               Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION);

           if (servicePort == "no") return EXIT_FAILURE;

           int portNbr;
           std::istringstream servicePort_strm(servicePort);
           servicePort_strm >> portNbr;

           if (servicePort_strm.fail())
           {
               std::cout << "bad value for "
                         << Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION
                         << "=<value> option\n";
               std::cout << std::endl;
               return EXIT_FAILURE;
           }

          std::string pullName = OS::get_option_value (argc, argv, 
               "--PullMonitorableName");

           if (pullName == "no") return EXIT_FAILURE;
           
           serviceName += pullName;

           unsigned int sleepTime;

           std::string sleepTimeOption = OS::get_option_value( argc, argv, "--Sleep");

           if (sleepTimeOption == "no") sleepTime = defSleepTime;
           else
           {
               std::istringstream sleepTimeOption_strm(sleepTimeOption);
               sleepTimeOption_strm >> sleepTime;

               if (sleepTimeOption_strm.fail())
               {
                 std::cout << "bad value for " << "SLEEP_OPTION" << "=<value> option\n";
                 std::cout << std::endl;
                 return EXIT_FAILURE;
               }
           }

           bool monitoringRequest;

           std::string monitoringRequestOption = OS::get_option_value( argc, argv, "--Request");

           if (monitoringRequestOption == "no") monitoringRequest = true;
           else if (monitoringRequestOption == "true") monitoringRequest = true;
           else if (monitoringRequestOption == "false") monitoringRequest = false;
           else monitoringRequest = true;

           bool monitoringExcept;

           std::string monitoringExceptOption = OS::get_option_value( argc, argv, "--Exception");

           if (monitoringExceptOption == "no") monitoringExcept = false;
           else if (monitoringExceptOption == "true") monitoringExcept = true;
           else if (monitoringExceptOption == "false") monitoringExcept = false;
           else monitoringExcept = false;

           std::string str_opt(argv[0]);
           str_opt += " ";
           str_opt += Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION;
           str_opt += "=";
           str_opt += servicePort;

           /*if (pullName == "pull_4")
           {
               str_opt += " ";
               str_opt += "-ORBDebugLevel 10";
           }*/

           char** argv__ = Cdmw::Common::String::to_char_array (
                Cdmw::Common::String::to_strings(str_opt));
           int argc__ = 2;

           //if (pullName == "pull_4") argc__ = 4;

           orb =  Cdmw::OrbSupport::OrbSupport::ORB_init(argc__, argv__, strategy); 

           CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
           PortableServer::POA_var poa = PortableServer::POA::_narrow(obj.in());

           PortableServer::POAManager_var poaMgr = poa->the_POAManager();
           poaMgr->activate();

           for (int i=0; i < argc__; ++i)
           {
              delete[] argv__[i];
           }
           delete[] argv__;

           // create monitorable servant

           PortableServer::ServantBase_var servant_var =
               new PullMonitorable_impl (pullName, sleepTime, monitoringRequest, monitoringExcept);

          PullMonitorable_impl* p_PullMonitorableServant =
              dynamic_cast<PullMonitorable_impl*>(servant_var.in());

           // create the CORBA reference
           ::FT::PullMonitorable_var pullMonitorable = p_PullMonitorableServant->_this();
        
           // publishes the monitorable corbaloc
           Cdmw::OrbSupport::OrbSupport::bind_object_to_corbaloc (
                       orb.in(), serviceName, pullMonitorable.in());

           std::cout << "Monitorable service starting" << std::endl;
           std::cout << "  listening on port : " << servicePort.c_str() << std::endl;
           std::cout << "  service name : " << serviceName.c_str() << std::endl;
           std::cout << "  sleep time is set to : " << sleepTime << std::endl;

           struct sigaction action;
           action.sa_handler=exit_handler;
           sigemptyset(&action.sa_mask);
           sigaction(SIGTERM, &action, NULL);
           sigaction(SIGINT, &action, NULL);
           sigaction(SIGQUIT, &action, NULL);
           sigaction(SIGABRT, &action, NULL);

           orb->run();
           orb->shutdown(false);
	   Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
	   orb->destroy(); 
	   
	       // cleanup the ProcessKillMngr singleton
           ProcessKillMngr::Cleanup();
        }
        catch (const CORBA::COMM_FAILURE& e)
        {  
    	   std::cerr << "FAILURE : Port " << servicePort << " is already used" << std::endl;
           ret_code = EXIT_FAILURE;
        }
        catch(const std::exception &e)
        {
            std::cerr << "FAILURE : Unexpected exception" << std::endl
                << e.what() << std::endl;
            ret_code = EXIT_FAILURE;
        }
        catch(const CORBA::SystemException& e)
        {
            std::cerr << "FAILURE : " <<e<< std::endl;
            ret_code = EXIT_FAILURE;
        }
        catch(...)
        {
            std::cerr << "FAILURE : Unexpected exception" << std::endl;
            ret_code = EXIT_FAILURE;
        }
        
        if (!CORBA::is_nil(orb.in()))
        {
             try
             {
                 Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
                 orb->destroy();
             }
             catch(const CORBA::Exception& ex)
             {
                 std::cerr << ex << std::endl;
                 std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                           << " IN FILE: " << __FILE__ << std::endl;
                 ret_code = EXIT_FAILURE;
             }
        }        

        return ret_code;
    }
}

