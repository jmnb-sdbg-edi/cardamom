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

#include <cstdlib>
#include "Foundation/common/System.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

//#include "Repository/repositoryinterface/RepositoryInterface.hpp"

//#include "ClientThread.hpp"
#include "ConfAndPlug/cdmwinit/CdmwInterface.hpp"
#include "ConfAndPlug/cdmwinit/ProcessControl.hpp"
#include "testcdmwprocess/MyProcessControl.hpp"
#include <string>
#include <sstream>
#include <iostream>

/* This test has been built by taking the Cdmw process model of initialisation (Cdmw_Init..)
   and by extraction of a part of the UT test managed process */

namespace {
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5;  
  
    const std::string UNMANAGED_OPTION = "--unmanaged";
    const std::string SLEEP_OPTION = "--Sleep";
    const std::string MONITORING_REQUEST_OPTION = "--Request";
    const std::string MONITORING_EXCEPT_OPTION = "--Exception";

}; // End anonymous namespace



static void
usage(std::ostream & os, const std::string &program_name)
{
    os << "Usage:\n"
       << program_name << " options\n"
       << "\n"
       << "Options:\n"
       << "--help                             Display this information.\n"
       << "[--unmanaged]                      Process is not managed.\n"
       << "[--Sleep=<time>]                   Time before is_alive return.\n"
       << "[--Request=<bool>]                 bool returned by is_alive\n"
       << "[--Exception=<bool>]               exception raised by is_alive (after a certain number of times)\n"
       << "                                   a time out in msec.\n"
       << std:: endl;
}

int main(int argc, char* argv[])
{
    using namespace Cdmw::OsSupport;

    int status = SUCCESS;
    

    CORBA::ORB_var orb;
    
    //
    // help argument
    //
    if (argc > 1)
    {
        if(strcmp(argv[1], "--help") == 0)
	    {
            usage(std::cout, argv[0]);
            return status;
        }
    }

    try {
        std::string unmanagedOption = OS::get_option_value (argc, argv, UNMANAGED_OPTION);
        bool managedProcess = true;
        if (unmanagedOption == "yes")
        {
            managedProcess = false;
        }
        
        unsigned int sleepTime;

        std::string sleepTimeOption = OS::get_option_value( argc, argv, SLEEP_OPTION);

        if (sleepTimeOption == "no") sleepTime = 100;
        else if (sleepTimeOption == "yes")
        {
            std::cout << "bad value for " << SLEEP_OPTION.c_str() 
                      << "=<value> option\n";
            std::cout << std::endl;
            return EXIT_FAILURE;
        }
        else if (sleepTimeOption != "no")
        {
            std::istringstream sleepTimeOption_strm(sleepTimeOption);
            sleepTimeOption_strm >> sleepTime;

            if (sleepTimeOption_strm.fail())
            {
              std::cout << "bad value for " << SLEEP_OPTION.c_str() << "=<value> option\n";
              std::cout << std::endl;
              return EXIT_FAILURE;
            }
        }

        bool monitoringRequest;

        std::string monitoringRequestOption = OS::get_option_value( argc, argv, MONITORING_REQUEST_OPTION);

        if (monitoringRequestOption == "no") monitoringRequest = true;
        else if (monitoringRequestOption == "true") monitoringRequest = true;
        else if (monitoringRequestOption == "false") monitoringRequest = false;
        else monitoringRequest = true;

        bool monitoringExcept;

        std::string monitoringExceptOption = OS::get_option_value( argc, argv, MONITORING_EXCEPT_OPTION);

        if (monitoringExceptOption == "no") monitoringExcept = false;
        else if (monitoringExceptOption == "true") monitoringExcept = true;
        else if (monitoringExceptOption == "false") monitoringExcept = false;
        else monitoringExcept = true;
                
        std::cout << "test cdmw process, unmanagedOption:" << unmanagedOption << " sleepTimeOption:" << sleepTimeOption << " monitoringRequestOption:" << monitoringRequestOption << " monitoringExceptOption:" <<  monitoringExceptOption << std::endl;

       // Initialize the ORB
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);      
        
        //
        // Get the root POA 
        //
        CORBA::Object_var obj = orb -> resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

        //
        // Run implementation (Activate the POA manager)
        //
        PortableServer::POAManager_var poaManager = rootPOA -> the_POAManager();
        poaManager -> activate();

        if (managedProcess)
        {
            try
            {
                Cdmw::CdmwInit::ProcessControl_var ctrl
                    = new Cdmw::PlatformMngt::MyProcessControl
                                                (orb.in(), std::cout,
                                                 sleepTime, monitoringRequest, monitoringExcept);

                Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv, ctrl.in());

                orb->run();
            }
            catch(...)
            {
                // cleanup the ORB to exit completely
                orb->shutdown(false);
		        Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
		        orb->destroy();
                throw;
            }
        }
        else
        {
            std::cout << "Start Unmanaged Process : " << argv[0] << std::endl;
            
            // start orb
            orb->run();
        }

    } catch (const Cdmw::Exception & ex) {
        std::cerr << ex.what() << std::endl;
        status = FAILURE;
    } catch(const CORBA::Exception& ex)  {     
        std::cerr << ex << std::endl;   
        status = FAILURE;
    }

    if(!CORBA::is_nil(orb.in()))
    {
        try {
            Cdmw::CdmwInit::CDMW_cleanup(orb.in());
	    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
	    orb->destroy();
        }
        catch(const CORBA::Exception& ex) {     
            std::cerr << ex << std::endl;
            status = FAILURE;
        }
    }        

    return status;    
}

