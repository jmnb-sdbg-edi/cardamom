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


#include "Foundation/common/System.hpp"
#include <Foundation/orbsupport/Codec.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include <Foundation/ossupport/OS.hpp>
#include "FaultTolerance/ftinit/FTServiceInit.hpp"
#include <Foundation/orbsupport/PortableInterceptor.hpp>
#include "ccm_ft_request_logging/TestORBInitializer.hpp"

#include "ConfAndPlug/cdmwinit/CdmwInterface.hpp"

#include <iostream>
#include <string>


#if CDMW_ORB_VDR == tao && CDMW_ORB_VER > 13
#include <tao/ORBInitializer_Registry.h>
#endif


namespace {
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5;    
}; // End anonymous namespace



int main(int argc, char* argv[])
{    
    int status = SUCCESS;

    CORBA::ORB_var orb;    
   
    try {
        // get host name
        std::string host_name1 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--host1");
        // get host name
        std::string host_name2 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--host2");
        // get host name
        std::string host_name3 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--host3");

        std::cout << "--------------- Initialise FT service ---------------" << std::endl;
        // first add test interceptor.
        PortableInterceptor::ORBInitializer_var test_initializer 
               = new Cdmw::TestORBInitializer(host_name1, host_name2, host_name3);
        
        ::PortableInterceptor::register_orb_initializer(test_initializer.in());

        // Initialise FT service
        Cdmw::FT::FTServiceInit::Init( argc, argv, false );

        // Initialize the ORB
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        std::cout << "---------------------- ORB_init ----------------------" << std::endl;
        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);

        // PCR-0049
        Cdmw::OrbSupport::CodecBase::init(orb.in());

        //
        // Get the root POA 
        //
        CORBA::Object_var obj = orb -> resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

        std::cout << "---------------- poaManager->activate ----------------" << std::endl;
        //
        // Run implementation (Activate the POA manager)
        //
        PortableServer::POAManager_var poaManager = rootPOA -> the_POAManager();
        poaManager -> activate();

        std::cout << "--------------------- CDMW_init ----------------------" << std::endl;
        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv);

        std::cout << "---------------------- ORB->run ----------------------" << std::endl;
        orb->run();
    } catch (const Cdmw::Exception & ex) {
        std::cerr << ex.what() << std::endl;
        status = FAILURE;
    } catch(const CORBA::Exception& ex)  {        
        std::cerr << ex << std::endl;
        status = FAILURE;
    }

    if(!CORBA::is_nil(orb.in()))
    {
        try {
            Cdmw::CdmwInit::CDMW_cleanup(orb.in());
	    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
	    orb->destroy();
        }
        catch(const CORBA::Exception& ex)
        {           
            std::cerr << ex << std::endl;
            status = FAILURE;
        }
    }        

    return status;    
}


