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


#include <Foundation/common/System.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/common/Locations.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/testutils/Testable.hpp>

#include "Menu.hpp"

const int SUCCESS = 0;
const int FAILURE = 1;
const int POA_THREAD_POOL_SIZE = 5;

int main(int argc, char* argv[])
{
    int status = SUCCESS;
    CORBA::ORB_var orb;
    PortableServer::POA_var poa;

    try 
	{
        // Initialize the ORB
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool( POA_THREAD_POOL_SIZE );

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(
            argc, argv, orb_strategies );

        // Enter interactive mode
        Menu menu( orb.in(), std::cin, std::cout );

		// start client and run orb
        menu.start();
        orb->run();

	    // wait end of client thread after corba loop stopping
        menu.join();
    }
	catch( const Cdmw::Exception & ex )
	{
        std::cerr << ex.what() << std::endl;
        status = FAILURE;
    }
   	catch( const CORBA::Exception& ex )
	{
        std::cerr << ex << std::endl;
        status = FAILURE;
    }
	catch( ... )
	{
        std::cerr << "unknown exception in main" << std::endl;
        status = FAILURE;
    }

	// end of corba processing
    if( !CORBA::is_nil( orb.in() ) )
    {
        try 
        {
            Cdmw::OrbSupport::OrbSupport::ORB_cleanup( orb.in() );
            orb->destroy();
        }
        catch( const CORBA::Exception& ex )
        {
            std::cerr << ex << std::endl;
            status = FAILURE;
        }
    }

    return status;
}
