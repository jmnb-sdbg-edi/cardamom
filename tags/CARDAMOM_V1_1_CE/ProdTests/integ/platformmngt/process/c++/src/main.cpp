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

#include "Foundation/common/Options.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/orbsupport/CORBA.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"

#include "Foundation/testutils/Testable.hpp"

#include "process/MyProcessBehaviour.hpp"

#include <cstdlib>
#include <string>
#include <exception>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace Cdmw;
using namespace CdmwPlatformMngt;

std::string HELP_OPTION = "--help";

static void usage( std::ostream & os, const std::string &program_name )
{
    os << "Usage:"<< std::endl
       << program_name << " options" << std::endl
       << std::endl
       << "Options:" << std::endl
       << HELP_OPTION << "Display this information." << std::endl
       << std:: endl;
}

int main( int argc, char* argv[] )
{
    // the return code
    int ret_code = 0;
    CORBA::ORB_var orb;
    PortableServer::POA_var poa;

    // Check command line arguments
    std::string help_option = OsSupport::OS::get_option_value(
        argc, argv, HELP_OPTION );

    if( help_option == "yes" )
    {
        usage( std::cout, argv[0] );
        return ret_code;
    }

    try
    {
        // Start the ORB
        OrbSupport::StrategyList strategy;
            strategy.add_OrbThreaded();
            strategy.add_PoaThreadPool( 5 );

        orb = OrbSupport::OrbSupport::ORB_init( argc, argv, strategy );

        // Get a reference to the root POA
        CORBA::Object_var obj = orb->resolve_initial_references( "RootPOA" );
        poa = PortableServer::POA::_narrow( obj.in() );

        // Activate the POA manager
        PortableServer::POAManager_var mgr = poa->the_POAManager();
        mgr->activate();

        // Initialise the platform interface
        PlatformInterface::setup( orb.in(), argc, argv );

        // Create the process behaviour
        std::auto_ptr< MyProcessBehaviour > pMyProcess(
            new MyProcessBehaviour() );

        // Acknowledge the creation of the process
        PlatformInterface::acknowledgeCreation( pMyProcess.get() );
        pMyProcess.release();

        // Start orb
        orb->run();
    }
    catch ( const CORBA::Exception& e )
    {  
    	std::cerr << e._name() << std::endl;
        ret_code = 1;
    }
    catch( ... )
    {
        std::cerr << "FAILURE : Unexpected exception" << std::endl;
        ret_code = 1;
    }

    // Stop the POA
    if( !CORBA::is_nil( poa.in() ) )
        poa->destroy( 1, 1 );

    // Cleanup the ORB to exit completely
    if( !CORBA::is_nil( orb.in() ) )
    {
        orb->shutdown( false );
        Cdmw::OrbSupport::OrbSupport::ORB_cleanup( orb.in() );
        orb->destroy();
    }

    return ret_code;
}

