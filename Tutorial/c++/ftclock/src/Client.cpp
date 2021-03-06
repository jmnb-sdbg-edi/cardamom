/* =========================================================================== *
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
 * =========================================================================== */


// Standard Files 
#include <string>
#include <iostream>

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <FaultTolerance/ftinit/FTServiceInit.hpp>

#include "ClientProcessBehaviour.hpp"
#include "ClientClockProcessBehaviour.hpp"


// using namespace Cdmw;
using namespace std;

int main( int argc, char* argv[] )
{

    try
    {
        // Initialise FT service (to be done before ORB_init)
        Cdmw::FT::FTServiceInit::Init( argc, argv, true );

        // Set ORB strategies
        Cdmw::OrbSupport::StrategyList strategyList;
            strategyList.add_OrbThreaded();
            strategyList.add_PoaThreadPerConnection();

        // Initialises the ORB
        CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(
            argc, argv, strategyList );

        // Get the root POA
        CORBA::Object_var obj = orb->resolve_initial_references( "RootPOA" );
        PortableServer::POA_var rootPOA
            = PortableServer::POA::_narrow( obj.in() );

        // activate the POA Manager
        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

        // creates the process behaviour
        std::auto_ptr< ::Cdmw::clock::valid::ClientProcessBehaviour > 
            pProcessBehaviour(
                new  ::Cdmw::clock::valid::ClientProcessBehaviour(
                orb.in(), rootPOA.in()) );
/*
        std::auto_ptr< ::Cdmw::clock::valid::ClientClockProcessBehaviour > 
            pClockProcessBehaviour(
                new  ::Cdmw::clock::valid::ClientClockProcessBehaviour(
                orb.in(), rootPOA.in()) );
*/
        // init Cdmw
        Cdmw::CdmwInit::CDMW_init(
            orb.in(), argc, argv, pProcessBehaviour.get() );

        // Enter in the main loop
        orb->run();
    }
    catch( const CORBA::Exception &e )
    {
        std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e._name() << std::endl;
        return EXIT_FAILURE;
    }
    catch( const Cdmw::Exception &e )
    {
        std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch( ... )
    {
        std::cerr << "Unexpected exception" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

