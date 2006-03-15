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

// Cdmw Files
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "ConfAndPlug/cdmwinit/CdmwInterface.hpp"
#include "FaultTolerance/ftinit/FTServiceInit.hpp"

#include "ccm_ft/GroupCreatorProcessBehaviour.hpp"



const int SUCCESS = 0;
const int FAILURE = 1;  



void
usage(std::ostream & os, const std::string &program_name)
{
    os << "Usage:\n"
       << program_name << " options\n"
       << "\n"
       << "Options:\n"
       << "--help                        Display this information.\n"
       << "--host1=<hostname>  hostname for server1.\n"
       << "--host2=<hostname>  hostname for server2.\n"
       << "--host3=<hostname>  hostname for server3.\n"
       << std:: endl;
}

    
int main( int argc, char* argv[] )
{

    try
    {
        //
        // help argument
        //
        if(strcmp(argv[1], "--help") == 0) {
            usage(std::cout, argv[0]);
            return SUCCESS;
        }
        
        std::cout << "  ---> GrouCreator args:" << std::endl;
        for (int i=0; i<argc; ++i) {
            std::cout << "    -> " << argv[i] << std::endl;
        }
        
        //
        // get arguments --host1 --host2 --host3
        //
        std::string host1 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "host1");
        if ((host1 == "no") || (host1=="yes")) {
            std::cerr << "host1=<hostname> option must be defined"
	                  << std::endl;
            usage(std::cerr, argv[0]);
            return FAILURE;
        }
        std::string host2 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "host2");
        if ((host2 == "no") || (host2=="yes")) {
            std::cerr << "host2=<hostname> option must be defined"
	                  << std::endl;
            usage(std::cerr, argv[0]);
            return FAILURE;
        }
        std::string host3 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "host3");
        if ((host3 == "no") || (host3=="yes")) {
            std::cerr << "host3=<hostname> option must be defined"
	                  << std::endl;
            usage(std::cerr, argv[0]);
            return FAILURE;
        }
        
        
        // Initialise FT service
        Cdmw::FT::FTServiceInit::Init( argc, argv, true );

        // Initialises the ORB
        Cdmw::OrbSupport::StrategyList strategyList;
		strategyList.add_OrbThreaded();
		strategyList.add_PoaThreadPerConnection();

        CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init
			(argc, argv, strategyList );

        // Get the root POA
        CORBA::Object_var obj = orb->resolve_initial_references( "RootPOA" );
        PortableServer::POA_var rootPOA
            = PortableServer::POA::_narrow( obj.in() );

        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

        // creates the process behaviour
        std::auto_ptr<GroupCreatorProcessBehaviour> pProcessBehaviour(
            new GroupCreatorProcessBehaviour(orb.in(), host1, host2, host3));

        // init Cdmw
        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv, pProcessBehaviour.get() );

        // Enter in the main loop
        orb->run();
        
        Cdmw::CdmwInit::CDMW_cleanup(orb.in());
        Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
        orb->destroy();
    }
    catch( const CORBA::Exception &e )
    {
        std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
        return EXIT_FAILURE;
    }
    catch( const Cdmw::Exception &e )
    {
        std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
        return EXIT_FAILURE;
    }
    catch( ... )
    {
        std::cerr << "Unexpected exception" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


