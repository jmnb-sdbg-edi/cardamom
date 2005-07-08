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

#include "Foundation/testutils/Testable.hpp"

#include "testmonitoring/PullMonitorable_impl.hpp"

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

    const std::string serviceName = "MonitorableService";
    const std::string defServicePort = "51234";

    std::string SLEEP_OPTION = "--sleep-time";
    const unsigned int defSleepTime = 200;
    
    
    class UnitTest : virtual public Cdmw::TestUtils::Testable
    {

      public:

        UnitTest (const std::string& name)
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


int main(int argc, char* argv[])
{ 
    // if called by unit testing (no argument)
    if (argc == 1)
    {
        UnitTest unit_test ("testmonitoring");
        unit_test.start();
        return 0;
    }
      
    // the return code 
    int ret_code = 0;

    std::set_unexpected(&exceptionHandler);

    std::string servicePort;

    try
    {

        StrategyList strategy;
        strategy.add_OrbThreaded();
        strategy.add_PoaThreadPerRequest();

        CORBA::ORB_var orb;
 
        unsigned int sleepTime;

        servicePort = OS::get_option_value (argc, argv, 
            Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION);

        if (servicePort == "no")
        {
            servicePort = defServicePort;
        }
        else
        {
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
        }
    
        std::string sleepTimeOption = OS::get_option_value( argc, argv, SLEEP_OPTION);

        if (sleepTimeOption == "no")
        {
            sleepTime = defSleepTime;
        }
        else
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

        std::string str_opt(argv[0]);
        str_opt += " ";
        str_opt += Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION;
        str_opt += "=";
        str_opt += servicePort;

        char** argv__ = Cdmw::Common::String::to_char_array (
                              Cdmw::Common::String::to_strings(str_opt));
        int argc__(2);

        orb =  OrbSupport::OrbSupport::ORB_init(argc__, argv__, strategy); 

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
            new PullMonitorable_impl (sleepTime);

        PullMonitorable_impl* p_PullMonitorableServant =
            dynamic_cast<PullMonitorable_impl*>(servant_var.in());

        // create the CORBA reference
        PullMonitorable_var pullMonitorable = p_PullMonitorableServant->_this();
        
        // publishes the monitorable corbaloc
        Cdmw::OrbSupport::OrbSupport::bind_object_to_corbaloc (
                          orb.in(), serviceName, pullMonitorable.in());
    
        std::cout << "Monitorable service starting" << std::endl;
        std::cout << "  listening on port : " << servicePort.c_str() << std::endl;
        std::cout << "  service name : " << serviceName.c_str() << std::endl;
        std::cout << "  sleep time is set to : " << sleepTime << std::endl;

        orb->run();
        orb->shutdown(false);
	OrbSupport::OrbSupport::ORB_cleanup(orb.in());
	orb->destroy();
        
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
    catch(...)
    {
        std::cerr << "FAILURE : Unexpected exception" << std::endl;
        ret_code = EXIT_FAILURE;
    }

    return ret_code;

}


