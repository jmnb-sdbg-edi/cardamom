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

#include "Repository/repositoryinterface/RepositoryInterface.hpp"

//#include "ClientThread.hpp"
#include "ConfAndPlug/cdmwinit/CdmwInterface.hpp"
#include "ConfAndPlug/cdmwinit/ProcessControl.hpp"
#include "MyProcessControl.hpp"
#include <string>
#include <sstream>
#include <iostream>

namespace {
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5;    

}; // End anonymous namespace



int main(int argc, char* argv[])
{
    using namespace Cdmw::OsSupport;

    std::string allTests = OS::get_option_value(argc, argv, "--interactive") ;
 
    // ---- Does it use interactive mode? ----
    bool interactive_mode = false;
    if ( allTests != "no" ) {
        interactive_mode = true;
    } 
    
    int status = SUCCESS;
    
    // Command for all the tests
    std::string commands("verbose \n"
                         "sleep 6000\n"
                         "entity\n"
                         "unmanaged\n"
                         "session\n"
                         "service\n"
                         "event\n"
                         "stop Resource.Factories/Name Process Agent.factory\n"
                         "stop Resource.Factories/Anonymous Process Agent.factory\n"
                         "exit \n");
    std::istringstream commands_is(commands);

    CORBA::ORB_var orb;

    
    try {
        
        // Initialize the ORB
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);      

	std::istream * is = 0;

	if (interactive_mode) {
	    // Enter interactive mode
	    is = &std::cin;
	} else {
            is = &commands_is;
	}
        
	Cdmw::CdmwInit::ProcessControl_var ctrl
            = new Cdmw::LifeCycle::MyProcessControl(orb.in(),
                                                     *is,
                                                     std::cout);




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

        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv, ctrl.in());

        orb->run();
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

