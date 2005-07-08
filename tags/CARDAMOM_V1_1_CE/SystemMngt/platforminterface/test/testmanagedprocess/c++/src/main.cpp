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


#include "Foundation/common/Options.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/orbsupport/CORBA.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"

#include "Foundation/testutils/Testable.hpp"

#include "testmanagedprocess/MyProcessBehaviour.hpp"

#include <cstdlib>
#include <string>
#include <exception>
#include <sstream>
#include <iostream>
#include <fstream>


const int POA_THREAD_POOL_SIZE = 5;

namespace
{
    using namespace CdmwPlatformMngt;
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::OrbSupport;
    using namespace Cdmw::PlatformMngt;

    std::string applicationName;    
    std::string processName;

    const std::string PROCESS_NAME_OPTION = "--proc-name";
    const std::string UNMANAGED_OPTION = "--unmanaged";
    const std::string AUTO_END_OPTION = "--auto-end";
    const std::string NB_STEPS_OPTION = "--nb-steps";
    const std::string SERVICE_REQUEST_OPTION = "--service-request";
    const std::string SYS_ENTITY_SET_OPTION = "--sys-entity-set";
    const std::string APP_ENTITY_SET_OPTION = "--app-entity-set";
    const std::string PROC_ENTITY_SET_OPTION = "--proc-entity-set";
    const std::string ENTITY_UPDATING_OPTION = "--entity-update";
    const std::string PROCESS_TIMEOUT_OPTION = "--process-timeout";
    
    
    class UnitTest : virtual public Cdmw::TestUtils::Testable
    {

      public:

        UnitTest(const std::string& name)
             : Testable(name)
        {
        };

        virtual ~UnitTest()
        {
        };
        

      protected:

	    virtual void do_tests()
	    {
	        TEST_INFO("No Test to process");
	    };
    };
}


void exceptionHandler(void)
{
	std::cout << "Unexpected Exception : Test failed" << std::endl;
}


void exitHandler(void)
{
	std::cout << "Process '" << processName << "' exited" << std::endl;
}


static void
usage(std::ostream & os, const std::string &program_name)
{
    os << "Usage:\n"
       << program_name << " options\n"
       << "\n"
       << "Options:\n"
       << "--help                             Display this information.\n"
       << "[--proc-name=<name>]               Name of the process.\n"
       << "[--unmanaged]                      Process is not managed.\n"
       << "[--auto-end=<time-out>]            Process will auto end after time out in msec.\n"
       << "[--nb-steps=<value>]               Number of init stepsName.\n"
       << "[--service-request=<name>]         Service to request.\n"
       << "[--sys-entity-set=<name>]          System entity to set.\n"
       << "[--app-entity-set=<name>]          Application entity to set.\n"
       << "[--proc-entity-set=<name>]         Process entity to set.\n"
       << "[--entity-update=<interval-time]   Entity updating interval in msec. \n"
       << "[--process-timeout=<time-out>]     Process will initialises, run and stop after\n"
       << "                                   a time out in msec.\n"
       << std:: endl;
}
    
    
    
int main(int argc, char* argv[])
{  
    // if called by unit testing (no argument)
    if (argc == 1)
    {
        UnitTest unit_test ("testmanagedprocess");
        unit_test.start();
        return 0;
    }
    
    
    processName = argv[0];
   
    // the return code 
    int ret_code = 0;

    std::set_unexpected(&exceptionHandler);

    ::atexit(&exitHandler);

    StrategyList strategy;
    strategy.add_OrbThreaded();
    //strategy.add_PoaThreadPerRequest();
    strategy.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

    CORBA::ORB_var orb;


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
        std::string specifiedProcessName = OS::get_option_value (argc, argv, PROCESS_NAME_OPTION);
        if (specifiedProcessName == "yes")
        {
            std::cout << "bad value for " << PROCESS_NAME_OPTION.c_str() << "=<name> option\n";
            std::cout << std::endl;
            return EXIT_FAILURE;
        }
        else if (processName != "no")
        {
	        processName = specifiedProcessName;
        }
  
        std::string unmanagedOption = OS::get_option_value (argc, argv, UNMANAGED_OPTION);
        bool managedProcess = true;
        if (unmanagedOption == "yes")
        {
            managedProcess = false;
        }
        
        std::string autoEndOption = OS::get_option_value (argc, argv, AUTO_END_OPTION);
	    int autoEndTimeOut;
     
        if (autoEndOption == "no")
        {
            autoEndTimeOut = -1;
        }
        else if (autoEndOption == "yes")
        {
            std::cout << "bad value for " << AUTO_END_OPTION.c_str() 
                      << "=<time-out> option\n";
            std::cout << std::endl;
            return EXIT_FAILURE;
        }
	    else if (autoEndOption != "no")
        {
	        std::istringstream autoEndOption_strm(autoEndOption);
	        autoEndOption_strm >> autoEndTimeOut;
	        if (autoEndOption_strm.fail())
            {
	            std::cout << "bad value for " << AUTO_END_OPTION.c_str() 
	                      << "=<time-out> option\n";
                std::cout << std::endl;
                return EXIT_FAILURE;
            }
	    }
        
        std::string nbStepsOption = OS::get_option_value (argc, argv, NB_STEPS_OPTION);
	    int nbStepsValue;
        if (nbStepsOption == "no")
        {
            nbStepsValue = 1;
        }
        else if (nbStepsOption == "yes")
        {
            std::cout << "bad value for " << NB_STEPS_OPTION.c_str() << "=<value> option\n";
            std::cout << std::endl;
            return EXIT_FAILURE;
        }
	    else
        {
	        std::istringstream nbStepsOption_strm(nbStepsOption);
	        nbStepsOption_strm >> nbStepsValue;
	        if (nbStepsOption_strm.fail())
            {
	            std::cout << "bad value for " << NB_STEPS_OPTION.c_str() << "=<value> option\n";
                std::cout << std::endl;
                return EXIT_FAILURE;
            }
	    }

        std::string serviceRequestName = OS::get_option_value (argc, argv, SERVICE_REQUEST_OPTION);
        if (serviceRequestName == "yes")
        {
            std::cout << "bad value for " << SERVICE_REQUEST_OPTION.c_str() << "=<name> option\n";
            std::cout << std::endl;
            return EXIT_FAILURE;
        }        
        else if (serviceRequestName == "no")
        { 
            serviceRequestName = "";  
        }
        
        std::string sysEntityName = OS::get_option_value (argc, argv, SYS_ENTITY_SET_OPTION);
        if (sysEntityName == "yes")
        {
            std::cout << "bad value for " << SYS_ENTITY_SET_OPTION.c_str() << "=<name> option\n";
            std::cout << std::endl;
            return EXIT_FAILURE;
        }        
        else if (sysEntityName == "no")
        { 
            sysEntityName = "";  
        }
        
        std::string appEntityName = OS::get_option_value (argc, argv, APP_ENTITY_SET_OPTION);
        if (appEntityName == "yes")
        {
            std::cout << "bad value for " << APP_ENTITY_SET_OPTION.c_str() << "=<name> option\n";
            std::cout << std::endl;
            return EXIT_FAILURE;
        }        
        else if (appEntityName == "no")
        { 
            appEntityName = "";  
        }
        
        std::string procEntityName = OS::get_option_value (argc, argv, PROC_ENTITY_SET_OPTION);
        if (procEntityName == "yes")
        {
            std::cout << "bad value for " << PROC_ENTITY_SET_OPTION.c_str() << "=<name> option\n";
            std::cout << std::endl;
            return EXIT_FAILURE;
        }        
        else if (procEntityName == "no")
        { 
            procEntityName = "";  
        }
           
        
        std::string entityUpdtOption = OS::get_option_value (argc, argv, ENTITY_UPDATING_OPTION);
	    int entityUpdating;
     
        if (entityUpdtOption == "no")
        {
            entityUpdating = 5000;
        }
        else if (entityUpdtOption == "yes")
        {
            std::cout << "bad value for " << ENTITY_UPDATING_OPTION.c_str() 
                      << "=<interval-time> option\n";
            std::cout << std::endl;
            return EXIT_FAILURE;
        }
	    else if (entityUpdtOption != "no")
        {
	        std::istringstream entityUpdtOption_strm(entityUpdtOption);
	        entityUpdtOption_strm >> entityUpdating;
	        if (entityUpdtOption_strm.fail())
            {
	            std::cout << "bad value for " << ENTITY_UPDATING_OPTION.c_str() 
	                      << "=<interval-time> option\n";
                std::cout << std::endl;
                return EXIT_FAILURE;
            }
	    }
	    
	    
	    
	    std::string processTimeOutOption = OS::get_option_value (argc, argv, PROCESS_TIMEOUT_OPTION);
	    int processTimeOut;
     
        if (processTimeOutOption == "no")
        {
            processTimeOut = 0;
        }
        else if (processTimeOutOption == "yes")
        {
            std::cout << "bad value for " << PROCESS_TIMEOUT_OPTION.c_str() 
                      << "=<time-out> option\n";
            std::cout << std::endl;
            return EXIT_FAILURE;
        }
	    else if (processTimeOutOption != "no")
        {
	        std::istringstream processTimeOutOption_strm(processTimeOutOption);
	        processTimeOutOption_strm >> processTimeOut;
	        if (processTimeOutOption_strm.fail())
            {
	            std::cout << "bad value for " << PROCESS_TIMEOUT_OPTION.c_str() 
	                      << "=<time-out> option\n";
                std::cout << std::endl;
                return EXIT_FAILURE;
            }
	    }
	    
            
        orb =  Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategy); 

        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");

        PortableServer::POA_var poa = PortableServer::POA::_narrow(obj.in());

        PortableServer::POAManager_var poaMgr = poa->the_POAManager();

        poaMgr->activate();

        if (managedProcess)
        {
            try
            {
                bool platformManaged =
                     PlatformInterface::isLaunchedByPlatformManagement(argc, argv);

                if (platformManaged)
                {
                    // initialise the platform interface
                    PlatformInterface::setup(orb.in(), argc, argv);
                }

                // creates the autoEnding class
		        std::auto_ptr<AutoEnding> pAutoEnding (
                                new AutoEnding (autoEndTimeOut));
                                
                // creates the process behaviour
                std::auto_ptr<MyProcessBehaviour> pMyProcess(
                                new MyProcessBehaviour(orb.in(), platformManaged, argv[0],
                                                       pAutoEnding.get(), autoEndTimeOut, entityUpdating,
                                                       nbStepsValue));
             
      
                if (!serviceRequestName.empty())
                {  
                    // set service name to request                   
                    pMyProcess->setServiceToRequest (serviceRequestName.c_str());
                }
                
                if (!sysEntityName.empty())
                { 
                    // set system entity name to set                   
                    pMyProcess->setSysEntityToSet (sysEntityName.c_str());
                }

                if (!appEntityName.empty())
                { 
                    // set application entity name to set                   
                    pMyProcess->setAppEntityToSet (appEntityName.c_str());
                }
            
                if (!procEntityName.empty())
                { 
                    // set process entity name to set                   
                    pMyProcess->setProcEntityToSet (procEntityName.c_str());
                }


                // get processId
                OS::ProcessId processId = OS::get_processId();

                if (platformManaged)
                {
                    // acknowledge the creation of the process
                    PlatformInterface::acknowledgeCreation(pMyProcess.get());
                    pMyProcess.release();
                    
                    // sample usage of the platform interface                    
                    applicationName = PlatformInterface::getApplicationName();

                    processName = PlatformInterface::getProcessName();


                    std::cout << "Application name : "
                              << applicationName.c_str() << std::endl;

                    std::cout << "Process name : " 
                              << processName.c_str() 
                              << " Process id : "
                              << processId << std::endl;
                }
                else
                {
                    CdmwPlatformMngtBase::StartupKind startupKind;
                    startupKind.state_retrieval = CdmwPlatformMngtBase::LAST_STATE;
                    startupKind.startup_mode = processTimeOut;
                    startupKind.specified_state_id = CORBA::string_dup("");

                    (pMyProcess.get())->initialise(startupKind);

                    (pMyProcess.get())->run();                    
                }
                
                // start orb
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


