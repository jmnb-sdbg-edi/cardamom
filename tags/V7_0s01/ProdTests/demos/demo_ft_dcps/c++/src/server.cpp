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


#include <exception>
#include <iostream>
#include <fstream>

#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <FaultTolerance/ftinit/FTServiceInit.hpp>
#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>

#include "ServerProcessBehaviour.hpp"


#ifdef CDMW_POSIX
    #include <sys/types.h>
    #include <unistd.h>
#endif

using namespace std;
using namespace Cdmw;
using namespace Cdmw::Common;
using namespace Cdmw::OsSupport;


namespace
{
    const int POA_THREAD_POOL_SIZE = 10;

    const std::string HOST_NAME_OPTION = "--HostName";
} // end anonymous namespace


int main( int argc, char* argv[] )
{
    int ret_code = 0;

    // get hello interface
    std::string host_name = Cdmw::OsSupport::OS::get_option_value(
        argc, argv, HOST_NAME_OPTION );

    try
    {
        // Initialise FT service
        std::cout << "test_server: initialise FT service" << std::endl;
        Cdmw::FT::FTServiceInit::Init( argc, argv );

        // Initialise the ORB
        std::cout << "test_server: initialise the orb" << std::endl;
        OrbSupport::StrategyList strategyList;
            strategyList.add_OrbThreaded();
            strategyList.add_PoaThreadPool( POA_THREAD_POOL_SIZE );

        CORBA::ORB_var orb
            = OrbSupport::OrbSupport::ORB_init( argc, argv, strategyList );

        // And activate the root POA            
        CORBA::Object_var obj = orb->resolve_initial_references( "RootPOA" );
        PortableServer::POA_ptr rootPOA
            = PortableServer::POA::_narrow( obj.in() );
            
        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

        // creates the process behaviour
        std::auto_ptr<ServerProcessBehaviour> pProcessBehaviour(
            new ServerProcessBehaviour( orb.in(), argc, argv ) );

        // init Cdmw
        std::cout << host_name << " test_server: init Cdmw" << std::endl;
        Cdmw::CdmwInit::CDMW_init(
            orb.in(), argc, argv, pProcessBehaviour.get() );

        // Enter in the main loop
        orb->run();
    }

    catch(const CORBA::Exception &e)
    {
        std::cerr
            << "FAILURE : Unexpected exception" << std::endl
            << e._name() << std::endl;
        ret_code = EXIT_FAILURE;
    }

    return ret_code;
}


