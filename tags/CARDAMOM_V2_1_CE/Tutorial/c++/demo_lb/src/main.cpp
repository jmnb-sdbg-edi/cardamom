/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 
// Standard Files 
#include <string>

// Cdmw Files
#include "Foundation/osthreads/Thread.hpp"
#include "LoadBalancing/lbinit/LBServiceInit.hpp"
#include "DemoLB.hpp"

#include "MyProcessBehaviour.hpp"
#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>
#include <iostream>
using namespace std;

using namespace Cdmw;

namespace
{
    const std::string GROUP_MANAGER_PORT = "5040";
}
                               
int main( int argc, char* argv[] )
{

    // get host name
    std::string host_name1 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--host1");
    // get host name
    std::string host_name2 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--host2");
 // get host name
    std::string host_name3 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--host3");
    try
    {
        // Initialise LB service
        Cdmw::LB::LBServiceInit::Init( argc, argv);
        
         
        // Initialises the ORB
        OrbSupport::StrategyList strategyList;
            strategyList.add_OrbThreaded();
            strategyList.add_PoaThreadPerConnection();
            
            
        CORBA::ORB_var orb =  OrbSupport::OrbSupport::ORB_init(argc,argv, strategyList); 

        // Get the root POA
        CORBA::Object_var obj = orb->resolve_initial_references( "RootPOA" );
        PortableServer::POA_var rootPOA
            = PortableServer::POA::_narrow( obj.in() );

        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

        // creates the process behaviour
        std::auto_ptr<MyProcessBehaviour> pProcessBehaviour(
            new MyProcessBehaviour( orb.in(), rootPOA.in(), host_name1, host_name2, host_name3  ) );

        // init Cdmw
        std::cout << " test_server: init Cdmw"<<std::endl;
        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv,  pProcessBehaviour.get());
        // Enter in the main loop
        orb->run();

        //orb->shutdown() is done by the stop() of the ProcessBehaviour
        Cdmw::OrbSupport::OrbSupport::ORB_cleanup( orb.in() );
        orb->destroy();
    }
    catch( const CORBA::Exception &e )
    {
        cerr <<__FILE__<<" "<<__LINE__<<" "<< e._name() << endl;
        return EXIT_FAILURE;
    }
    catch( const Cdmw::Exception &e )
    {
        cerr <<__FILE__<<" "<<__LINE__<<" "<< e.what() << endl;
        return EXIT_FAILURE;
    }
    catch( ... )
    {
        cerr << "Unexpected exception" << endl;
        return EXIT_FAILURE;
    }
    std::cout<<"Press enter to stop the process"<<std::endl;
    std::string dummy;
    std::cin >> dummy;

    return EXIT_SUCCESS;
}

