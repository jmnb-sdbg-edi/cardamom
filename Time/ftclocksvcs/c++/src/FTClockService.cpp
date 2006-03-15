/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <FaultTolerance/ftinit/FTServiceInit.hpp>
#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>

#include "ftclocksvcs/FTClockServiceProcessBehaviour.hpp"
#include "clockservice/StateTransferConfig.hpp"

static const int POA_THREAD_POOL_SIZE = 10;

int main( int argc, char* argv[] )
{
    int ret_code = 0;


    // Check if primary option is specified
    std::string primary_option = Cdmw::OsSupport::OS::get_option_value(
        argc, argv, SET_PRIMARY_OPTION );
    
    bool set_as_primary=false;
    if (primary_option == "yes") {
        set_as_primary=true;
        std::cout << "  This ClockService will be primary." << std::endl;
    }    
    
    
    CORBA::ORB_var orb;
    try
    {
        // Initialise FT service (to be done before ORB_init)
        std::cout << "FTClockSVC: initialise FT service" << std::endl;
        Cdmw::FT::FTServiceInit::Init( argc, argv );

        // Set ORB strategies
        std::cout << "FTClockSVC: initialise the orb" << std::endl;
        Cdmw::OrbSupport::StrategyList strategyList;
            strategyList.add_OrbThreaded();
            strategyList.add_PoaThreadPool( POA_THREAD_POOL_SIZE );

        // Initialise the ORB
        orb
            = Cdmw::OrbSupport::OrbSupport::ORB_init( argc, argv, strategyList );

        // And activate the root POA            
        CORBA::Object_var obj = orb->resolve_initial_references( "RootPOA" );
        PortableServer::POA_ptr rootPOA
            = PortableServer::POA::_narrow( obj.in() );
            
        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

        // creates the process behaviour
        std::auto_ptr<Cdmw::clock::svcs::FTClockServiceProcessBehaviour> pProcessBehaviour(
            new Cdmw::clock::svcs::FTClockServiceProcessBehaviour(orb.in(), set_as_primary ) );

        // init Cdmw
        std::string host_name = Cdmw::OsSupport::OS::get_hostname();
        std::cout << host_name << " Executor: init Cdmw" << std::endl;
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
    if(!CORBA::is_nil(orb.in()))
    {
        try {
            Cdmw::CdmwInit::CDMW_cleanup(orb.in());
            Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
            orb->destroy();
        }
        catch(const CORBA::Exception& e) {
            std::cerr
               << "FAILURE : Unexpected exception" << std::endl
               << e._name() << std::endl;
            ret_code = EXIT_FAILURE;
        }
    }



    return ret_code;
}


