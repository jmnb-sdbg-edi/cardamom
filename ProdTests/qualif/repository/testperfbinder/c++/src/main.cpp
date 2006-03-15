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


#include <string>
#include <Foundation/common/Options.hpp>
#include <Foundation/common/String.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <Foundation/ossupport/OS.hpp>
#include "testperfbinder/MyProcessBehaviour.hpp"
#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>

using namespace Cdmw;
using Cdmw::OsSupport::OS;
using namespace std;



namespace {
    const int POA_THREAD_POOL_SIZE = 3;    
}; // End anonymous namespace

                               
int main( int argc, char* argv[] )
{



    // get the number of bind
    std::string bind_number = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--bind-number");
    

    // No option --process, we are the main test driver 
    try
    {
        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded(); // Mandatory in OrbSupport::ORB_init
        strategyList.add_PoaThreadPool(POA_THREAD_POOL_SIZE);
        // Initialises the ORB
        CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList, "main_orb");
        
        // Get the root POA
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
        
        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        std::cout << " faulttoleranceclient: activate POA"<<std::endl;
        poaMgr->activate();

        // creates the process behaviour
        std::auto_ptr<MyProcessBehaviour> pProcessBehaviour(
            new MyProcessBehaviour( orb.in(), rootPOA.in(),bind_number ) );

        // init Cdmw
        std::cout << " faulttoleraceclient: init Cdmw"<<std::endl;
        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv,  pProcessBehaviour.get());
        // Enter in the main loop
        orb->run();

        //orb->shutdown() is done by the stop() of the ProcessBehaviour
        Cdmw::CdmwInit::CDMW_cleanup(orb.in());
        Cdmw::OrbSupport::OrbSupport::ORB_cleanup( orb.in() );
        orb->destroy();

    }
    catch( const CORBA::Exception &e )
    {
        cerr << e._name() << endl;
    }
    catch( const Cdmw::Exception &e )
    {
        cerr << e.what() << endl;
    }
    catch(...)
    {
        cerr << "Uncaught exception" << endl;
    }
}



