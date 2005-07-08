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



// Cdmw Files
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>
#include <FaultTolerance/ftinit/FTServiceInit.hpp>

// Demo Files
#include <GroupCreatorProcessBehaviour.hpp>
#include <StateTransferConfiguration.hpp>


int main( int argc, char* argv[] )
{

    // Check if point-to-point option is specified
    std::string point_to_point_option = Cdmw::OsSupport::OS::get_option_value(
        argc, argv, POINT_TO_POINT_OPTION );

    bool point_to_point = false;
    if (point_to_point_option == "yes") {
        point_to_point=true;
        std::cout << "  Point to point State Transfer will be used." << std::endl;
    } else {
        std::cout << "  Multicast State Transfer will be used." << std::endl;
    }

    try
    {
        // Initialise FT service
        Cdmw::FT::FTServiceInit::init( argc, argv, true );

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

        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

        // creates the process behaviour
        std::auto_ptr<GroupCreatorProcessBehaviour> pProcessBehaviour(
            new GroupCreatorProcessBehaviour(orb.in(), point_to_point));

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

