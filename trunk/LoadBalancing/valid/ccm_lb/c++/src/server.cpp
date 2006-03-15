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


#include <Foundation/common/System.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <LoadBalancing/lbinit/LBServiceInit.hpp>

#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>

#include <iostream>


namespace {
    const int SUCCESS = 0;
    const int FAILURE = 1;
}; // End anonymous namespace


int main(int argc, char* argv[])
{
    int status = SUCCESS;

    CORBA::ORB_var orb;

    try {
        // Initialize LB service
        std::cout << "----- Initializing LB service -----" << std::endl;
        Cdmw::LB::LBServiceInit::Init(argc, argv);

        // Initialize the ORB
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPerConnection();
	orb_strategies.add_multicast();	

        std::cout << "------------ ORB_init -------------" << std::endl;
        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc,
                                                     argv,
                                                     orb_strategies);

        //
        // Get the root POA
        //
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA =
            PortableServer::POA::_narrow(obj.in());

        //
        // Run implementation (Activate the POA manager)
        //
        PortableServer::POAManager_var poaManager = rootPOA->the_POAManager();

        std::cout << "------ poaManager->activate -------" << std::endl;
        poaManager -> activate();

        std::cout << "----------- CDMW_init -------------" << std::endl;
        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv);

        std::cout << "----------- ORB->run --------------" << std::endl;
        orb->run();
	
    } catch (const Cdmw::Exception & ex) {
        std::cerr << ex.what() << std::endl;
        status = FAILURE;
    } catch(const CORBA::Exception& ex) {
        std::cerr << ex << std::endl;
        status = FAILURE;
    }
    if ( !CORBA::is_nil( orb.in() ) )
    {
        try
	{
	    Cdmw::CdmwInit::CDMW_cleanup(orb.in());  
	    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
	    //  orb->destroy();
        }
        catch ( const CORBA::Exception &e )
        {
            std::cerr << e << std::endl;
        }
    } 
    return status;
}
