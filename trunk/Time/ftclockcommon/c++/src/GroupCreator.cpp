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
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>
#include <FaultTolerance/ftinit/FTServiceInit.hpp>

#include "Time/ftclockcommon/GroupCreatorProcessBehaviour.hpp"
#include "Time/clockservice/StateTransferConfig.hpp"

using namespace std;

int main( int argc, char* argv[] )
{

    bool point_to_point = true;
    CORBA::ORB_var orb;

    try
    {
        // Initialise FT service
        Cdmw::FT::FTServiceInit::Init( argc, argv, true );

        // Set ORB strategies
        Cdmw::OrbSupport::StrategyList strategyList;
            strategyList.add_OrbThreaded();
            strategyList.add_PoaThreadPerConnection();

        // Initialises the ORB
        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(
            argc, argv, strategyList );

        // Get the root POA
        CORBA::Object_var obj = orb->resolve_initial_references( "RootPOA" );
        PortableServer::POA_var rootPOA
            = PortableServer::POA::_narrow( obj.in() );

        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();
        
        // get host name
        std::string host_name1 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--host1");
        std::string host_name2 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--host2");
        std::string host_name3 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--host3");

		if( host_name1.empty() or
  		    host_name2.empty() or
            host_name3.empty() )
		{
			std::cerr<<"Error not all host name are filled!!!"<<std::endl;
			std::cerr<<"Please check xml profile!!!"<<std::endl;
            return EXIT_FAILURE;
		}   

        // get process name
        std::string proc_name1 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--proc1");
        std::string proc_name2 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--proc2");
        std::string proc_name3 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--proc3");

		if( proc_name1.empty() or
  		    proc_name2.empty() or
            proc_name3.empty() )
		{
			std::cerr<<"Error not all process name are filled!!!"<<std::endl;
			std::cerr<<"Please check xml profile!!!"<<std::endl;
            return EXIT_FAILURE;
		}   

        // creates the process behaviour
        std::auto_ptr<Cdmw::clock::svcs::GroupCreatorProcessBehaviour>
            pProcessBehaviour(new Cdmw::clock::svcs::GroupCreatorProcessBehaviour(orb.in(),
                                                                                  point_to_point,
                                                                                  host_name1,
                                                                                  host_name2,
                                                                                  host_name3));
		pProcessBehaviour->link( proc_name1,
		                         host_name1 );
		pProcessBehaviour->link( proc_name2,
		                         host_name2 );
		pProcessBehaviour->link( proc_name3,
		                         host_name3 );
		
        // init Cdmw
        Cdmw::CdmwInit::CDMW_init(
            orb.in(), argc, argv, pProcessBehaviour.get() );

        // Enter in the main loop
        orb->run();
    }
    catch( const CORBA::Exception &e )
    {
        cerr <<__FILE__<<" "<<__LINE__<<" "<< e._name() << endl;
        return EXIT_FAILURE;
    }
    catch( const Cdmw::Exception &e )
    {
        cerr <<__FILE__<<" "<<__LINE__<<" "<< e.what() << endl;
        return EXIT_FAILURE;
    }
    catch( ... )
    {
        cerr << "Unexpected exception" << endl;
        return EXIT_FAILURE;
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
           return EXIT_FAILURE;
        }
    }


    return EXIT_SUCCESS;
}

