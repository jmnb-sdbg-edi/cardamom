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

// Standard Files 
#include <string>

// Cardamom Files
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>
#include <ConfAndPlug/cdmwinit/InitUtils.hpp>

#include "LBGroupCreatorProcessBehaviour.hpp"




namespace 
{
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5;    
}; // End anonymous namespace

using namespace Cdmw;

int main( int argc, char* argv[] )
{
    int status = SUCCESS;

    CORBA::ORB_var orb;
    try
    {
        // Initialises the ORB
        OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init
			(argc, argv, strategyList );

        // Get the root POA
        CORBA::Object_var obj = orb->resolve_initial_references( "RootPOA" );
        PortableServer::POA_var rootPOA
            = PortableServer::POA::_narrow( obj.in() );

        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

        // creates the process behaviour
        std::auto_ptr<LBGroupCreatorProcessBehaviour> pProcessBehaviour(
            new LBGroupCreatorProcessBehaviour(orb.in()));

        // init Cardamom
        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv, pProcessBehaviour.get() );

        // ===================================================
        // Get the process name and application name
        // ===================================================
        std::string processName = 
            Cdmw::CdmwInit::InitUtils::get_cdmw_process_name();
        
        std::string applicationName =
            Cdmw::CdmwInit::InitUtils::get_cdmw_application_name();

        std::cout << "===> main LB Group Creator " 
                  << applicationName.c_str() << "/" << processName.c_str() 
                  << std::endl;

        // Enter in the main loop
        orb->run();
    }
    catch( const CORBA::Exception &e )
    {
        std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
        status = FAILURE;
    }
    catch( const Cdmw::Exception &e )
    {
        std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
        status = FAILURE;
    }
    catch( ... )
    {
        std::cerr << "Unexpected exception" << std::endl;
        status = FAILURE;
    }



    // ========================================================
    // program stopping
    // ========================================================
    
    // ===================================================
    // Call generated Cardamom cleanup
    // ===================================================
    Cdmw::CdmwInit::CDMW_cleanup(orb.in());
            
    // ===================================================
    // Call ORB cleanup
    // ===================================================
    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
       
    // =====================================================
    // Destroy orb
    // =====================================================
    if(!CORBA::is_nil(orb.in()))
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
    return EXIT_SUCCESS;
}
