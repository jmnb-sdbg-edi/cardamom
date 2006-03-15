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

#include "testexceptions/MyProcessBehaviour.hpp"

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
}


void exceptionHandler(void)
{
	std::cout << "Unexpected Exception : Test failed" << std::endl;
}


void exitHandler(void)
{
	std::cout << "Process '" << processName << "' exited" << std::endl;
}



    
    
int main(int argc, char* argv[])
{  
    int ret_code = 0;

    try
    {
        std::cout<<"Start managed process"<<std::endl;

        std::set_unexpected(&exceptionHandler);

        ::atexit(&exitHandler);

        StrategyList strategy;
        strategy.add_OrbThreaded();
        strategy.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        CORBA::ORB_var orb =  Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategy); 
    
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
    
        PortableServer::POA_var poa = PortableServer::POA::_narrow(obj.in());

        PortableServer::POAManager_var poaMgr = poa->the_POAManager();
    
        poaMgr->activate();

        try
        {
            bool platformManaged =
            PlatformInterface::Is_launched_by_PlatformManagement(argc, argv);
            
            if (platformManaged)
            {
                // initialise the platform interface
                PlatformInterface::Setup(orb.in(), argc, argv);
            }
            
            // creates the process behaviour
            std::auto_ptr<MyProcessBehaviour> pMyProcess(new MyProcessBehaviour(orb.in(), argc, argv));
            sleep(1);
            // acknowledge the creation of the process
            PlatformInterface::Acknowledge_creation(pMyProcess.get());
            pMyProcess.release();


            // start orb
            orb->run();
            
            if (platformManaged)
            {
                // cleanup the platform interface
                PlatformInterface::Cleanup();
            }

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


