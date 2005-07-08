/* ========================================================================== *
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
 * ========================================================================= */
 
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>

#include "Foundation/common/System.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

#include "ConfAndPlug/cdmwinit/CdmwInterface.hpp"
#include "ConfAndPlug/cdmwinit/InitUtils.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"

#include "ClientProcessControl.hpp"

// -- Predictability Addon --
#include <sys/mman.h>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
//--

namespace 
{
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 2;
 
}; // End anonymous namespace

int main(int argc, char* argv[])
{
    // -- Predictability Addon --
//      mlockall(MCL_CURRENT | MCL_FUTURE);
      struct sched_param sp;
      sp.sched_priority = 99;
      int retVal =
          sched_setscheduler(getpid(), SCHED_FIFO, &sp);

      if (retVal != 0)
          perror(">>>>> sched_setscheduler: ");
    // --
      
    int status = SUCCESS;
    
    std::cout << "Start ClockService Client" << std::endl;
    
    //number of test
    int niter = 1000;

    //delay between calls
    unsigned long period = 0;

    std::cout << "Reading Configuration File..." << std::endl;
    //read configuration file
    
    FILE * fd = fopen("client.cfg", "r");
    fscanf(fd,"%d",&niter);                     // number of waking ups requested to the Timer
    fscanf(fd,"%lu",&period);                   // period (msec) of the waking ups
    
    fclose(fd);

    std::cout << "Sample invocation= " << niter << std::endl;            
    std::cout << "Delay (ms)= " << period << std::endl;

    // transform millisec in TimeBase::TimeT formats
    // Note: 1s == 10000000 TimeBase::TimeT units.
    period = period * 10000;
    std::cout << "Delay (usec/10)= " << period << std::endl;


    CORBA::ORB_var orb;                        // orb reference
   
    try 
    {
        // ===================================================
        // Initialize the ORB
        // ===================================================
        printf("*** client: Initialise The ORB\n");
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);
        printf("*** client: Get Root POA\n");
        
        // ===================================================
        // Get the root POA 
        // ===================================================
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

        // ===================================================
        // Activate the root POA manager
        // ===================================================
        PortableServer::POAManager_var poaManager = rootPOA->the_POAManager();
        poaManager->activate();
        printf("*** client: Activate the ORB\n");

        // ===================================================
        // create the process control for platform management
        // ===================================================
            Cdmw::CdmwInit::ProcessControl_var process_ctrl =
              new TimerInvocation::ClientProcessControl(orb.in(),
                                                 niter,
                                                 period);
        // ===================================================
        // Call generated CdmwInit
        // ===================================================
        printf("*** client: Call generated CDMW_Init\n");
        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv, process_ctrl.in());
        
        
        // ===================================================
        // wait for CORBA message
        // ===================================================
        printf("*** client: ORB run\n");
        orb->run();

    } 
    catch (const Cdmw::OrbSupport::CORBASystemExceptionWrapper & ex)
    {
        std::cerr << ex.what() << std::endl;
        status = FAILURE;
    }
    catch (const Cdmw::Exception & ex)
    {
        std::cerr << ex.what() << std::endl;
        status = FAILURE;
    } 
    catch(const CORBA::Exception& ex)  
    {        
        std::cerr << ex << std::endl;
        status = FAILURE;
    }
    catch(...)  
    {        
        std::cerr << "unexpected exception" << std::endl;
        status = FAILURE;
    }


    // ========================================================
    // program stopping
    // ========================================================
    
    // ===================================================
    // Call generated Cdmw cleanup
    // ===================================================
    Cdmw::CdmwInit::CDMW_cleanup(orb.in());
    
    // ===================================================
    // Call ORB cleanup
    // ===================================================
    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
            
    // ===================================================
    // destroy orb
    // ===================================================
    
    if (!CORBA::is_nil(orb.in()))
    {
        try 
        {
            orb -> destroy();
        }
        catch(const CORBA::Exception& ex)
        {           
            std::cerr << ex << std::endl;
            status = FAILURE;
        }
    }        

    return status;    
}
